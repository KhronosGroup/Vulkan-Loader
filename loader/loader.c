/*
 *
 * Copyright (c) 2014-2023 The Khronos Group Inc.
 * Copyright (c) 2014-2023 Valve Corporation
 * Copyright (c) 2014-2023 LunarG, Inc.
 * Copyright (C) 2015 Google Inc.
 * Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * Copyright (c) 2023-2023 RasterGrid Kft.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.

 *
 * Author: Jon Ashburn <jon@lunarg.com>
 * Author: Courtney Goeltzenleuchter <courtney@LunarG.com>
 * Author: Mark Young <marky@lunarg.com>
 * Author: Lenny Komow <lenny@lunarg.com>
 * Author: Charles Giessen <charles@lunarg.com>
 *
 */

#include "loader.h"

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

#if defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#include <sys/param.h>
#endif

#include <sys/types.h>
#if defined(_WIN32)
#include "dirent_on_windows.h"
#elif COMMON_UNIX_PLATFORMS
#include <dirent.h>
#else
#warning dirent.h not available on this platform
#endif  // _WIN32

#include "allocation.h"
#include "cJSON.h"
#include "debug_utils.h"
#include "loader_environment.h"
#include "loader_json.h"
#include "log.h"
#include "unknown_function_handling.h"
#include "vk_loader_platform.h"
#include "wsi.h"

#if defined(WIN32)
#include "loader_windows.h"
#endif
#if defined(LOADER_ENABLE_LINUX_SORT)
// This header is currently only used when sorting Linux devices, so don't include it otherwise.
#include "loader_linux.h"
#endif  // LOADER_ENABLE_LINUX_SORT

// Generated file containing all the extension data
// NOLINTNEXTLINE(bugprone-suspicious-include) - intentionally including a .c file to inline generated tables
#include "vk_loader_extensions.c"

struct loader_struct loader = {0};

struct activated_layer_info {
    char* name;
    char* manifest;
    char* library;
    bool is_implicit;
    enum loader_layer_enabled_by_what enabled_by_what;
    char* disable_env;
    char* enable_name_env;
    char* enable_value_env;
};

// thread safety lock for accessing global data structures such as "loader"
// all entrypoints on the instance chain need to be locked except GPA
// additionally CreateDevice and DestroyDevice needs to be locked
loader_platform_thread_mutex loader_lock;
loader_platform_thread_mutex loader_preload_icd_lock;

// A list of ICDs that gets initialized when the loader does its global initialization. This list should never be used by anything
// other than EnumerateInstanceExtensionProperties(), vkDestroyInstance, and loader_release(). This list does not change
// functionality, but the fact that the libraries already been loaded causes any call that needs to load ICD libraries to speed up
// significantly. This can have a huge impact when making repeated calls to vkEnumerateInstanceExtensionProperties and
// vkCreateInstance.
struct loader_icd_tramp_list preloaded_icds;

LOADER_PLATFORM_THREAD_ONCE_DECLARATION(once_init);

// Creates loader_api_version struct that contains the major and minor fields, setting patch to 0
loader_api_version loader_make_version(uint32_t version) {
    loader_api_version out_version;
    out_version.major = VK_API_VERSION_MAJOR(version);
    out_version.minor = VK_API_VERSION_MINOR(version);
    out_version.patch = 0;
    return out_version;
}

// Creates loader_api_version struct containing the major, minor, and patch fields
loader_api_version loader_make_full_version(uint32_t version) {
    loader_api_version out_version;
    out_version.major = VK_API_VERSION_MAJOR(version);
    out_version.minor = VK_API_VERSION_MINOR(version);
    out_version.patch = VK_API_VERSION_PATCH(version);
    return out_version;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters) - no safer ordering for these version component fields
loader_api_version loader_combine_version(uint32_t major, uint32_t minor, uint32_t patch) {
    loader_api_version out_version;
    out_version.major = (uint16_t)major;
    out_version.minor = (uint16_t)minor;
    out_version.patch = (uint16_t)patch;
    return out_version;
}

// Helper macros for determining if a version is valid or not
bool loader_check_version_meets_required(loader_api_version required, loader_api_version version) {
    // major version is satisfied
    return (version.major > required.major) ||
           // major version is equal, minor version is patch version is greater to minimum minor
           (version.major == required.major && version.minor > required.minor) ||
           // major and minor version are equal, patch version is greater or equal to minimum patch
           (version.major == required.major && version.minor == required.minor && version.patch >= required.patch);
}

const char* get_enabled_by_what_str(enum loader_layer_enabled_by_what enabled_by_what) {
    switch (enabled_by_what) {
        case ENABLED_BY_WHAT_UNSET:
        default:
            assert(true && "Shouldn't reach this");
            return "Unknown";
        case ENABLED_BY_WHAT_LOADER_SETTINGS_FILE:
            return "Loader Settings File (Vulkan Configurator)";
        case ENABLED_BY_WHAT_IMPLICIT_LAYER:
            return "Implicit Layer";
        case ENABLED_BY_WHAT_VK_INSTANCE_LAYERS:
            return "Environment Variable VK_INSTANCE_LAYERS";
        case ENABLED_BY_WHAT_VK_LOADER_LAYERS_ENABLE:
            return "Environment Variable VK_LOADER_LAYERS_ENABLE";
        case ENABLED_BY_WHAT_IN_APPLICATION_API:
            return "By the Application";
    }
}

// Wrapper around opendir so that the dirent_on_windows gets the instance it needs
// while linux opendir & readdir does not
DIR* loader_opendir(const struct loader_instance* instance, const char* name) {
#if defined(_WIN32)
    return opendir(instance, name);
#elif COMMON_UNIX_PLATFORMS
    (void)instance;
    return opendir(name);
#else
#warning dirent.h - opendir not available on this platform
#endif  // _WIN32
}
int loader_closedir(const struct loader_instance* instance, DIR* dir) {
#if defined(_WIN32)
    return closedir(instance, dir);
#elif COMMON_UNIX_PLATFORMS
    (void)instance;
    return closedir(dir);
#else
#warning dirent.h - closedir not available on this platform
#endif  // _WIN32
}
struct dirent* loader_readdir(const struct loader_instance* instance, DIR* dir) {
#if defined(_WIN32)
    return readdir(instance, dir);
#elif COMMON_UNIX_PLATFORMS
    (void)instance;
    // NOLINTNEXTLINE(concurrency-mt-unsafe) - readdir is safe here since each thread uses its own dir_stream
    return readdir(dir);
#else
#warning dirent.h - readdir not available on this platform
#endif  // _WIN32
}
bool is_json(const char* path, size_t len) {
    if (len < 5) {
        return false;
    }
    return !strncmp(path + len - 5, ".json", 5);
}

// Handle error from to library loading
void loader_handle_load_library_error(const struct loader_instance* inst, const char* filename,
                                      enum loader_layer_library_status* lib_status) {
    const char* error_message = loader_platform_open_library_error(filename);
    // If the error is due to incompatible architecture (eg 32 bit vs 64 bit), report it with INFO level
    // Discussed in Github issue 262 & 644
    // "wrong ELF class" is a linux error, " with error 193" is a windows error
    VkFlags err_flag = VULKAN_LOADER_ERROR_BIT;
    if (strstr(error_message, "wrong ELF class:") != NULL || strstr(error_message, " with error 193") != NULL) {
        err_flag = VULKAN_LOADER_INFO_BIT;
        if (NULL != lib_status) {
            *lib_status = LOADER_LAYER_LIB_ERROR_WRONG_BIT_TYPE;
        }
    }
    // Check if the error is due to lack of memory
    // "with error 8" is the windows error code for OOM cases, aka ERROR_NOT_ENOUGH_MEMORY
    // Linux doesn't have such a nice error message - only if there are reported issues should this be called
    else if (strstr(error_message, " with error 8") != NULL) {
        if (NULL != lib_status) {
            *lib_status = LOADER_LAYER_LIB_ERROR_OUT_OF_MEMORY;
        }
    } else if (NULL != lib_status) {
        *lib_status = LOADER_LAYER_LIB_ERROR_FAILED_TO_LOAD;
    }
    loader_log(inst, err_flag, 0, "%s", error_message);
}

VKAPI_ATTR VkResult VKAPI_CALL vkSetInstanceDispatch(VkInstance instance, void* object) {
    struct loader_instance* inst = loader_get_instance(instance);
    if (!inst) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "vkSetInstanceDispatch: Can not retrieve Instance dispatch table.");
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    loader_set_dispatch(object, inst->disp);
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkSetDeviceDispatch(VkDevice device, void* object) {
    struct loader_device* dev;
    struct loader_icd_term* icd_term = loader_get_icd_and_device(device, &dev);

    if (NULL == icd_term || NULL == dev) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    loader_set_dispatch(object, &dev->loader_dispatch);
    return VK_SUCCESS;
}

void loader_free_layer_properties(const struct loader_instance* inst, struct loader_layer_properties* layer_properties) {
    loader_instance_heap_free(inst, layer_properties->manifest_file_name);
    loader_instance_heap_free(inst, layer_properties->lib_name);
    loader_instance_heap_free(inst, layer_properties->functions.str_gipa);
    loader_instance_heap_free(inst, layer_properties->functions.str_gdpa);
    loader_instance_heap_free(inst, layer_properties->functions.str_negotiate_interface);
    loader_destroy_generic_list(inst, (struct loader_generic_list*)&layer_properties->instance_extension_list);
    if (layer_properties->device_extension_list.capacity > 0 && NULL != layer_properties->device_extension_list.list) {
        for (uint32_t i = 0; i < layer_properties->device_extension_list.count; i++) {
            free_string_list(inst, &layer_properties->device_extension_list.list[i].entrypoints);
        }
    }
    loader_destroy_generic_list(inst, (struct loader_generic_list*)&layer_properties->device_extension_list);
    loader_instance_heap_free(inst, layer_properties->disable_env_var.name);
    loader_instance_heap_free(inst, layer_properties->disable_env_var.value);
    loader_instance_heap_free(inst, layer_properties->enable_env_var.name);
    loader_instance_heap_free(inst, layer_properties->enable_env_var.value);
    loader_instance_heap_free(inst, layer_properties->pre_instance_functions.enumerate_instance_extension_properties);
    loader_instance_heap_free(inst, layer_properties->pre_instance_functions.enumerate_instance_layer_properties);
    loader_instance_heap_free(inst, layer_properties->pre_instance_functions.enumerate_instance_version);

    // Make sure to clear out the removed layer, in case new layers are added in the previous location
    memset(layer_properties, 0, sizeof(struct loader_layer_properties));
}

VkResult loader_init_library_list(struct loader_layer_list* instance_layers, loader_platform_dl_handle** libs) {
    if (instance_layers->count > 0) {
        *libs = (loader_platform_dl_handle*)loader_calloc(NULL, sizeof(loader_platform_dl_handle) * instance_layers->count,
                                                          VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (*libs == NULL) {
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }
    }
    return VK_SUCCESS;
}

bool is_string_in_string_list(struct loader_string_list* string_list, const char* str) {
    // Remove duplicate paths, or it would result in duplicate extensions, duplicate devices, etc.
    for (size_t i = 0; i < string_list->count; i++) {
        if (strcmp(string_list->list[i], str) == 0) {
            return true;
        }
    }
    return false;
}

VkResult loader_copy_to_new_str(const struct loader_instance* inst, const char* source_str, char** dest_str) {
    assert(source_str && dest_str);
    size_t str_len = strlen(source_str) + 1;
    *dest_str = loader_instance_heap_calloc(inst, str_len, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    if (NULL == *dest_str) return VK_ERROR_OUT_OF_HOST_MEMORY;
    loader_strncpy(*dest_str, str_len, source_str, str_len);
    (*dest_str)[str_len - 1] = 0;
    return VK_SUCCESS;
}

VkResult create_string_list(const struct loader_instance* inst, uint32_t allocated_count, struct loader_string_list* string_list) {
    assert(string_list);
    string_list->list =
        (char**)loader_instance_heap_calloc(inst, sizeof(char*) * allocated_count, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    if (NULL == string_list->list) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    string_list->allocated_count = allocated_count;
    string_list->count = 0;
    return VK_SUCCESS;
}

VkResult increase_str_capacity_by_at_least_one(const struct loader_instance* inst, struct loader_string_list* string_list) {
    assert(string_list);
    if (string_list->allocated_count == 0) {
        string_list->allocated_count = 32;
        string_list->list = (char**)loader_instance_heap_calloc(inst, sizeof(char*) * string_list->allocated_count,
                                                                VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (NULL == string_list->list) {
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }
    } else if (string_list->count + 1 > string_list->allocated_count) {
        uint32_t new_allocated_count = string_list->allocated_count * 2;
        void* new_ptr = loader_instance_heap_realloc(inst, (void*)string_list->list, sizeof(char*) * string_list->allocated_count,
                                                     sizeof(char*) * new_allocated_count, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (NULL == new_ptr) {
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }
        string_list->list = (char**)new_ptr;
        string_list->allocated_count *= 2;
    }
    return VK_SUCCESS;
}

VkResult append_str_to_string_list(const struct loader_instance* inst, struct loader_string_list* string_list, char* str) {
    assert(string_list && str);
    VkResult res = increase_str_capacity_by_at_least_one(inst, string_list);
    if (res == VK_ERROR_OUT_OF_HOST_MEMORY) {
        loader_instance_heap_free(inst, str);  // Must clean up in case of failure
        return res;
    }
    string_list->list[string_list->count++] = str;
    return VK_SUCCESS;
}

VkResult append_str_to_string_list_if_unique(const struct loader_instance* inst, struct loader_string_list* string_list,
                                             char* str) {
    if (is_string_in_string_list(string_list, str)) {
        // Since this string is a duplicate and this function takes ownership, we need to free it.
        loader_instance_heap_free(inst, str);
        return VK_SUCCESS;
    }
    return append_str_to_string_list(inst, string_list, str);
}

VkResult prepend_str_to_string_list(const struct loader_instance* inst, struct loader_string_list* string_list, char* str) {
    assert(string_list && str);
    VkResult res = increase_str_capacity_by_at_least_one(inst, string_list);
    if (res == VK_ERROR_OUT_OF_HOST_MEMORY) {
        loader_instance_heap_free(inst, str);  // Must clean up in case of failure
        return res;
    }
    // Shift everything down one
    void* ptr_to_list = memmove((void*)(string_list->list + 1), (const void*)string_list->list, sizeof(char*) * string_list->count);
    if (ptr_to_list) string_list->list[0] = str;  // Write new string to start of list
    string_list->count++;
    return VK_SUCCESS;
}

VkResult copy_str_to_string_list(const struct loader_instance* inst, struct loader_string_list* string_list, const char* str,
                                 size_t str_len) {
    assert(string_list && str);
    char* new_str = loader_instance_heap_calloc(inst, str_len + 1, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    if (NULL == new_str) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    loader_strncpy(new_str, str_len + 1, str, str_len);
    new_str[str_len] = '\0';
    return append_str_to_string_list(inst, string_list, new_str);
}

VkResult copy_str_to_string_list_if_unique(const struct loader_instance* inst, struct loader_string_list* string_list,
                                           const char* str, size_t str_len) {
    if (is_string_in_string_list(string_list, str)) {
        return VK_SUCCESS;
    }
    return copy_str_to_string_list(inst, string_list, str, str_len);
}

VkResult copy_str_to_start_of_string_list(const struct loader_instance* inst, struct loader_string_list* string_list,
                                          const char* str, size_t str_len) {
    assert(string_list && str);
    char* new_str = loader_instance_heap_calloc(inst, str_len + 1, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    if (NULL == new_str) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    loader_strncpy(new_str, str_len + 1, str, str_len);
    new_str[str_len] = '\0';
    return prepend_str_to_string_list(inst, string_list, new_str);
}

void remove_str_from_string_list(const struct loader_instance* inst, struct loader_string_list* string_list, const char* str) {
    assert(string_list && str);
    for (uint32_t i = 0; i < string_list->count; i++) {
        if (strcmp(string_list->list[i], str) == 0) {
            loader_instance_heap_free(inst, string_list->list[i]);
            memmove((void*)(string_list->list + i), (const void*)(string_list->list + i + 1),
                    sizeof(char*) * (string_list->count - i - 1));
            string_list->count--;
            return;
        }
    }
}

void free_string_list(const struct loader_instance* inst, struct loader_string_list* string_list) {
    assert(string_list);
    if (string_list->list) {
        for (uint32_t i = 0; i < string_list->count; i++) {
            loader_instance_heap_free(inst, string_list->list[i]);
            string_list->list[i] = NULL;
        }
        loader_instance_heap_free(inst, (void*)string_list->list);
    }
    memset(string_list, 0, sizeof(struct loader_string_list));
}

// In place modify the passed in path to do the following:
// If HAVE_REALPATH is defined, then this simply calls realpath() so its behavior is defined by realpath()
// Else:
// * Windows-only: Replace forward slashes with backwards slashes (platform correct directory separator)
// * Replace contiguous directory separators with a single directory separator
// * Replace "/./" separator with "/" (where / is the platform correct directory separator)
// * Replace "/<directory_name>/../" with just "/" (where / is the platform correct directory separator)
VkResult normalize_path(const struct loader_instance* inst, char** passed_in_path) {
    // passed_in_path doesn't point to anything, can't modify inplace so just return
    if (passed_in_path == NULL) {
        return VK_SUCCESS;
    }

// POSIX systems has the realpath() function to do this for us, fallback to basic normalization on other platforms
#if defined(HAVE_REALPATH)
    char* path = loader_instance_heap_calloc(inst, PATH_MAX, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    if (NULL == path) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    char* ret = realpath(*passed_in_path, path);
    if (NULL == ret) {
        // error path
        int error_code = errno;
        loader_log(inst, VULKAN_LOADER_DEBUG_BIT, 0,
                   "normalize_path: Call to realpath() failed with error code %d when given the path %s", error_code,
                   *passed_in_path);
        loader_instance_heap_free(inst, path);
    } else {
        // Replace string pointed to by passed_in_path with the one given to us by realpath()
        loader_instance_heap_free(inst, *passed_in_path);
        *passed_in_path = path;
    }
    return VK_SUCCESS;

#elif defined(WIN32)
    return windows_normalize_path(inst, passed_in_path);

#else
    VkResult res = VK_SUCCESS;
    char* path = *passed_in_path;
    size_t path_len = strlen(path) + 1;

    size_t output_index = 0;
    // Iterate through the string up to the last character, excluding the null terminator
    for (size_t i = 0; i < path_len - 1; i++) {
        if (i + 1 < path_len && path[i] == DIRECTORY_SYMBOL && path[i + 1] == DIRECTORY_SYMBOL) {
            continue;
        } else if (i + 2 < path_len && path[i] == DIRECTORY_SYMBOL && path[i + 1] == '.' && path[i + 2] == DIRECTORY_SYMBOL) {
            i += 1;
        } else {
            path[output_index++] = path[i];
        }
    }
    // Add null terminator and set the new length
    path[output_index++] = '\0';
    path_len = output_index;

    // Loop while there are still ..'s in the path. Easiest implementation resolves them one by one, which requires quadratic
    // iteration through the string
    char* directory_stack = loader_instance_heap_calloc(inst, path_len, VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
    if (directory_stack == NULL) {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    size_t top_of_stack = 0;

    // Iterate through the path, push characters as we see them, if we find a "..", pop off the top of the directory stack until the
    // current directory is gone.
    for (size_t i = 0; i < path_len - 1; i++) {
        // if the next part of path is "/../" we need to pop from the directory stack until we hit the previous directory symbol.
        if (i + 3 < path_len && path[i] == DIRECTORY_SYMBOL && path[i + 1] == '.' && path[i + 2] == '.' && path_len &&
            path[i + 3] == DIRECTORY_SYMBOL) {
            // Pop until we hit the next directory symbol in the stack
            while (top_of_stack > 0 && directory_stack[top_of_stack - 1] != DIRECTORY_SYMBOL) {
                top_of_stack--;
                directory_stack[top_of_stack] = '\0';
            }
            // Amend the directory stack so that the top isn't a directory separator
            if (top_of_stack > 0 && directory_stack[top_of_stack - 1] == DIRECTORY_SYMBOL) {
                top_of_stack--;
                directory_stack[top_of_stack] = '\0';
            }
            i += 2;  // need to skip the second dot & directory separator
        } else {
            // push characters as we come across them
            directory_stack[top_of_stack++] = path[i];
        }
    }

    // Can't forget the null terminator
    directory_stack[top_of_stack] = '\0';

    // We now have the path without any ..'s, so just copy it out
    loader_strncpy(path, path_len, directory_stack, path_len);
    path[top_of_stack] = '\0';
    path_len = top_of_stack + 1;

out:
    loader_instance_heap_free(inst, directory_stack);

    return res;
#endif
}

// Queries the path to the library that lib_handle & gipa are associated with, allocating a string to hold it and returning it in
// out_path
VkResult get_library_path_of_dl_handle(const struct loader_instance* inst, loader_platform_dl_handle lib_handle,
                                       PFN_vkGetInstanceProcAddr gipa, char** out_path) {
#if COMMON_UNIX_PLATFORMS
    (void)lib_handle;
    Dl_info dl_info = {0};
    if (dladdr(gipa, &dl_info) != 0 && NULL != dl_info.dli_fname) {
        return loader_copy_to_new_str(inst, dl_info.dli_fname, out_path);
    }
    return VK_SUCCESS;

#elif defined(WIN32)
    return windows_get_library_path_of_dl_handle(inst, lib_handle, gipa, out_path);
#else
    // Do nothing, platform doesn't handle getting the path to a library
#endif
}

// Find and replace the path that was loaded using the lib_name path with the real path of the library. This is done to provide
// accurate logging info for users.
// This function prints a warning if there is a mismatch between the lib_name path and the real path.
VkResult fixup_library_binary_path(const struct loader_instance* inst, char** lib_name, loader_platform_dl_handle lib_handle,
                                   PFN_vkGetInstanceProcAddr gipa) {
    if (lib_name == NULL) {
        // do nothing as we got an invalid lib_path pointer
        return VK_SUCCESS;
    }

    bool system_path = true;
    size_t lib_name_len = strlen(*lib_name) + 1;
    for (size_t i = 0; i < lib_name_len; i++) {
        if ((*lib_name)[i] == DIRECTORY_SYMBOL) {
            system_path = false;
            break;
        }
    }

    if (!system_path) {
        // The OS path we get for a binary is normalized, so we need to normalize the path passed into LoadLibrary/dlopen so that
        // mismatches are minimized. EG, do not warn when we give dlopen/LoadLibrary "/foo/./bar" but get "/foo/bar" as the loaded
        // binary path from the OS.
        VkResult res = normalize_path(inst, lib_name);
        if (res == VK_ERROR_OUT_OF_HOST_MEMORY) {
            return res;
        }
    }
    char* os_determined_lib_name = NULL;
    VkResult res = get_library_path_of_dl_handle(inst, lib_handle, gipa, &os_determined_lib_name);
    if (res == VK_ERROR_OUT_OF_HOST_MEMORY) {
        return res;
    }

    if (NULL != os_determined_lib_name) {
        // Normalize the path so that the comparison doesn't yield false positives
        res = normalize_path(inst, &os_determined_lib_name);
        if (res == VK_ERROR_OUT_OF_HOST_MEMORY) {
            loader_instance_heap_free(inst, os_determined_lib_name);
            return res;
        }

        if (0 != strcmp(os_determined_lib_name, *lib_name)) {
            // Paths do not match, so we need to replace lib_name with the real path
            if (!system_path) {
                // Only warn when the library_path is relative or absolute, not system. EG lib_name had no directory separators
                loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                           "Path to given binary %s was found to differ from OS loaded path %s", *lib_name, os_determined_lib_name);
            }
            loader_instance_heap_free(inst, *lib_name);
            *lib_name = os_determined_lib_name;
        } else {
            // Paths match, so just need to free temporary allocation
            loader_instance_heap_free(inst, os_determined_lib_name);
        }
    }

    return res;
}

// Given string of three part form "maj.min.pat" convert to a vulkan version number.
// Also can understand four part form "variant.major.minor.patch" if provided.
uint32_t loader_parse_version_string(char* vers_str) {
    uint32_t variant = 0;
    uint32_t major = 0;
    uint32_t minor = 0;
    uint32_t patch = 0;
    char* vers_tok;
    char* context = NULL;
    if (!vers_str) {
        return 0;
    }

    vers_tok = thread_safe_strtok(vers_str, ".\"\n\r", &context);
    if (NULL != vers_tok) {
        major = (uint16_t)strtoul(vers_tok, NULL, 10);
        vers_tok = thread_safe_strtok(NULL, ".\"\n\r", &context);
        if (NULL != vers_tok) {
            minor = (uint16_t)strtoul(vers_tok, NULL, 10);
            vers_tok = thread_safe_strtok(NULL, ".\"\n\r", &context);
            if (NULL != vers_tok) {
                patch = (uint16_t)strtoul(vers_tok, NULL, 10);
                vers_tok = thread_safe_strtok(NULL, ".\"\n\r", &context);
                // check that we are using a 4 part version string
                if (NULL != vers_tok) {
                    // if we are, move the values over into the correct place
                    variant = major;
                    major = minor;
                    minor = patch;
                    patch = (uint16_t)strtoul(vers_tok, NULL, 10);
                }
            }
        }
    }

    return VK_MAKE_API_VERSION(variant, major, minor, patch);
}

TEST_FUNCTION_EXPORT bool compare_vk_extension_properties(const VkExtensionProperties* op1, const VkExtensionProperties* op2) {
    return strncmp(op1->extensionName, op2->extensionName, VK_MAX_EXTENSION_NAME_SIZE) == 0 ? true : false;
}

// Search the given ext_array for an extension matching the given vk_ext_prop
bool has_vk_extension_property_array(const VkExtensionProperties* vk_ext_prop, const uint32_t count,
                                     const VkExtensionProperties* ext_array) {
    for (uint32_t i = 0; i < count; i++) {
        if (compare_vk_extension_properties(vk_ext_prop, &ext_array[i])) return true;
    }
    return false;
}

// Search the given ext_list for an extension matching the given vk_ext_prop
bool has_vk_extension_property(const VkExtensionProperties* vk_ext_prop, const struct loader_extension_list* ext_list) {
    for (uint32_t i = 0; i < ext_list->count; i++) {
        if (compare_vk_extension_properties(&ext_list->list[i], vk_ext_prop)) return true;
    }
    return false;
}

// Search the given ext_list for a device extension matching the given ext_prop
bool has_vk_dev_ext_property(const VkExtensionProperties* ext_prop, const struct loader_device_extension_list* ext_list) {
    for (uint32_t i = 0; i < ext_list->count; i++) {
        if (compare_vk_extension_properties(&ext_list->list[i].props, ext_prop)) return true;
    }
    return false;
}

VkResult loader_append_layer_property(const struct loader_instance* inst, struct loader_layer_list* layer_list,
                                      struct loader_layer_properties* layer_property) {
    VkResult res = VK_SUCCESS;
    if (layer_list->capacity == 0) {
        res = loader_init_generic_list(inst, (struct loader_generic_list*)layer_list, sizeof(struct loader_layer_properties));
        if (VK_SUCCESS != res) {
            goto out;
        }
    }

    // Ensure enough room to add an entry
    if ((layer_list->count + 1) * sizeof(struct loader_layer_properties) > layer_list->capacity) {
        void* new_ptr = loader_instance_heap_realloc(inst, layer_list->list, layer_list->capacity, layer_list->capacity * 2,
                                                     VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (NULL == new_ptr) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_append_layer_property: realloc failed for layer list");
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }
        layer_list->list = new_ptr;
        layer_list->capacity *= 2;
    }
    memcpy(&layer_list->list[layer_list->count], layer_property, sizeof(struct loader_layer_properties));
    layer_list->count++;
    memset(layer_property, 0, sizeof(struct loader_layer_properties));
out:
    if (res != VK_SUCCESS) {
        loader_free_layer_properties(inst, layer_property);
    }
    return res;
}

// Search the given layer list for a layer property matching the given layer name
struct loader_layer_properties* loader_find_layer_property(const char* name, const struct loader_layer_list* layer_list) {
    for (uint32_t i = 0; i < layer_list->count; i++) {
        const VkLayerProperties* item = &layer_list->list[i].info;
        if (strcmp(name, item->layerName) == 0) return &layer_list->list[i];
    }
    return NULL;
}

struct loader_layer_properties* loader_find_pointer_layer_property(const char* name,
                                                                   const struct loader_pointer_layer_list* layer_list) {
    for (uint32_t i = 0; i < layer_list->count; i++) {
        const VkLayerProperties* item = &layer_list->list[i]->info;
        if (strcmp(name, item->layerName) == 0) return layer_list->list[i];
    }
    return NULL;
}

// Search the given layer list for a layer matching the given layer name
bool loader_find_layer_name_in_list(const char* name, const struct loader_pointer_layer_list* layer_list) {
    if (NULL == layer_list) {
        return false;
    }
    if (NULL != loader_find_pointer_layer_property(name, layer_list)) {
        return true;
    }
    return false;
}

// Remove all layer properties entries from the list
TEST_FUNCTION_EXPORT void loader_delete_layer_list_and_properties(const struct loader_instance* inst,
                                                                  struct loader_layer_list* layer_list) {
    uint32_t i;
    if (!layer_list) return;

    for (i = 0; i < layer_list->count; i++) {
        if (layer_list->list[i].lib_handle) {
            loader_platform_close_library(layer_list->list[i].lib_handle);
            loader_log(inst, VULKAN_LOADER_DEBUG_BIT | VULKAN_LOADER_LAYER_BIT, 0, "Unloading layer library %s",
                       layer_list->list[i].lib_name);
            layer_list->list[i].lib_handle = NULL;
        }
        loader_free_layer_properties(inst, &(layer_list->list[i]));
    }
    layer_list->count = 0;

    if (layer_list->capacity > 0) {
        layer_list->capacity = 0;
        loader_instance_heap_free(inst, layer_list->list);
    }
    memset(layer_list, 0, sizeof(struct loader_layer_list));
}

void loader_remove_layer_in_list(const struct loader_instance* inst, struct loader_layer_list* layer_list,
                                 uint32_t layer_to_remove) {
    if (layer_list == NULL || layer_to_remove >= layer_list->count) {
        return;
    }
    loader_free_layer_properties(inst, &(layer_list->list[layer_to_remove]));

    // Remove the layer from the layer list. Use memmove since we are overlapping the source and destination addresses.
    if (layer_to_remove + 1 <= layer_list->count) {
        memmove(&layer_list->list[layer_to_remove], &layer_list->list[layer_to_remove + 1],
                sizeof(struct loader_layer_properties) * (layer_list->count - 1 - layer_to_remove));
    }
    // Decrement the count (because we now have one less) and decrement the loop index since we need to
    // re-check this index.
    layer_list->count--;
}

VkResult loader_add_instance_extensions(const struct loader_instance* inst,
                                        const PFN_vkEnumerateInstanceExtensionProperties fp_get_props, const char* lib_name,
                                        struct loader_extension_list* ext_list) {
    uint32_t i;
    uint32_t count = 0;
    VkExtensionProperties* ext_props = NULL;
    VkResult res = VK_SUCCESS;

    if (!fp_get_props) {
        // No EnumerateInstanceExtensionProperties defined
        goto out;
    }

    // Make sure we never call ourself by accident, this should never happen outside of error paths
    if (fp_get_props == vkEnumerateInstanceExtensionProperties) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "loader_add_instance_extensions: %s's vkEnumerateInstanceExtensionProperties points to the loader, this would "
                   "lead to infinite recursion.",
                   lib_name);
        goto out;
    }

    res = fp_get_props(NULL, &count, NULL);
    if (res != VK_SUCCESS) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "loader_add_instance_extensions: Error getting Instance extension count from %s", lib_name);
        goto out;
    }

    if (count == 0) {
        // No ExtensionProperties to report
        goto out;
    }

    ext_props = loader_instance_heap_calloc(inst, count * sizeof(VkExtensionProperties), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
    if (NULL == ext_props) {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }
    memset(ext_props, 0, count * sizeof(VkExtensionProperties));

    uint32_t allocated_count = count;
    res = fp_get_props(NULL, &count, ext_props);
    if (res != VK_SUCCESS) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_add_instance_extensions: Error getting Instance extensions from %s",
                   lib_name);
        goto out;
    }

    // The count returned by the second call sizes the array, but never read past what we actually allocated.
    for (i = 0; i < count && i < allocated_count; i++) {
        bool ext_unsupported = wsi_unsupported_instance_extension(&ext_props[i]);
        if (!ext_unsupported) {
            res = loader_add_to_ext_list(inst, ext_list, 1, &ext_props[i]);
            if (res != VK_SUCCESS) {
                goto out;
            }
        }
    }

out:
    loader_instance_heap_free(inst, ext_props);
    return res;
}

VkResult loader_add_device_extensions(const struct loader_instance* inst,
                                      PFN_vkEnumerateDeviceExtensionProperties fpEnumerateDeviceExtensionProperties,
                                      VkPhysicalDevice physical_device, const char* lib_name,
                                      struct loader_extension_list* ext_list) {
    uint32_t i = 0;
    uint32_t count = 0;
    VkResult res = VK_SUCCESS;
    VkExtensionProperties* ext_props = NULL;

    res = fpEnumerateDeviceExtensionProperties(physical_device, NULL, &count, NULL);
    if (res != VK_SUCCESS) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "loader_add_device_extensions: Error getting physical device extension info count from library %s", lib_name);
        goto out;
    }
    if (count > 0) {
        ext_props = loader_instance_heap_calloc(inst, count * sizeof(VkExtensionProperties), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (NULL == ext_props) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_add_device_extensions: Failed to allocate space for device extension properties from library %s.",
                       lib_name);
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }
        uint32_t allocated_count = count;
        res = fpEnumerateDeviceExtensionProperties(physical_device, NULL, &count, ext_props);
        if (res != VK_SUCCESS) {
            goto out;
        }
        // The count returned by the second call sizes the array, but never read past what we actually allocated.
        for (i = 0; i < count && i < allocated_count; i++) {
            res = loader_add_to_ext_list(inst, ext_list, 1, &ext_props[i]);
            if (res != VK_SUCCESS) {
                goto out;
            }
        }
    }
out:
    loader_instance_heap_free(inst, ext_props);
    return res;
}

VkResult loader_init_generic_list(const struct loader_instance* inst, struct loader_generic_list* list_info, size_t element_size) {
    size_t capacity = 32 * element_size;
    list_info->count = 0;
    list_info->capacity = 0;
    list_info->list = loader_instance_heap_calloc(inst, capacity, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    if (list_info->list == NULL) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_init_generic_list: Failed to allocate space for generic list");
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    list_info->capacity = capacity;
    return VK_SUCCESS;
}

VkResult loader_resize_generic_list(const struct loader_instance* inst, struct loader_generic_list* list_info) {
    size_t new_capacity = list_info->capacity * 2;
    void* new_ptr =
        loader_instance_heap_realloc(inst, list_info->list, list_info->capacity, new_capacity, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    if (new_ptr == NULL) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_resize_generic_list: Failed to allocate space for generic list");
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    // Consumers of the grown region expect unused entries to read as zero (eg a VK_NULL_HANDLE surface). An app supplied
    // pfnReallocation is not required to zero the newly grown bytes, so do it here rather than relying on the allocator.
    memset((uint8_t*)new_ptr + list_info->capacity, 0, new_capacity - list_info->capacity);
    list_info->list = new_ptr;
    list_info->capacity = new_capacity;
    return VK_SUCCESS;
}

void loader_destroy_generic_list(const struct loader_instance* inst, struct loader_generic_list* list) {
    loader_instance_heap_free(inst, list->list);
    memset(list, 0, sizeof(struct loader_generic_list));
}

VkResult loader_get_next_available_entry(const struct loader_instance* inst, struct loader_used_object_list* list_info,
                                         uint32_t* free_index, const VkAllocationCallbacks* pAllocator) {
    if (NULL == list_info->list) {
        VkResult res =
            loader_init_generic_list(inst, (struct loader_generic_list*)list_info, sizeof(struct loader_used_object_status));
        if (VK_SUCCESS != res) {
            return res;
        }
    }
    for (uint32_t i = 0; i < list_info->capacity / sizeof(struct loader_used_object_status); i++) {
        if (list_info->list[i].status == VK_FALSE) {
            list_info->list[i].status = VK_TRUE;
            if (pAllocator) {
                list_info->list[i].allocation_callbacks = *pAllocator;
            } else {
                memset(&list_info->list[i].allocation_callbacks, 0, sizeof(VkAllocationCallbacks));
            }
            *free_index = i;
            return VK_SUCCESS;
        }
    }
    // No free space, must resize

    size_t old_capacity = list_info->capacity;
    VkResult res = loader_resize_generic_list(inst, (struct loader_generic_list*)list_info);
    if (VK_SUCCESS != res) {
        return res;
    }
    uint32_t new_index = (uint32_t)(old_capacity / sizeof(struct loader_used_object_status));
    // Zero out the newly allocated back half of list.
    memset(&list_info->list[new_index], 0, old_capacity);
    list_info->list[new_index].status = VK_TRUE;
    if (pAllocator) {
        list_info->list[new_index].allocation_callbacks = *pAllocator;
    } else {
        memset(&list_info->list[new_index].allocation_callbacks, 0, sizeof(VkAllocationCallbacks));
    }
    *free_index = new_index;
    return VK_SUCCESS;
}

void loader_release_object_from_list(struct loader_used_object_list* list_info, uint32_t index_to_free) {
    if (list_info->list && list_info->capacity > index_to_free * sizeof(struct loader_used_object_status)) {
        list_info->list[index_to_free].status = VK_FALSE;
        memset(&list_info->list[index_to_free].allocation_callbacks, 0, sizeof(VkAllocationCallbacks));
    }
}

// Append non-duplicate extension properties defined in props to the given ext_list.
// Return - Vk_SUCCESS on success
VkResult loader_add_to_ext_list(const struct loader_instance* inst, struct loader_extension_list* ext_list,
                                uint32_t prop_list_count, const VkExtensionProperties* props) {
    if (ext_list->list == NULL || ext_list->capacity == 0) {
        VkResult res = loader_init_generic_list(inst, (struct loader_generic_list*)ext_list, sizeof(VkExtensionProperties));
        if (VK_SUCCESS != res) {
            return res;
        }
    }

    for (uint32_t i = 0; i < prop_list_count; i++) {
        const VkExtensionProperties* cur_ext = &props[i];

        // look for duplicates
        if (has_vk_extension_property(cur_ext, ext_list)) {
            continue;
        }

        // add to list at end
        // check for enough capacity
        if (ext_list->count * sizeof(VkExtensionProperties) >= ext_list->capacity) {
            void* new_ptr = loader_instance_heap_realloc(inst, ext_list->list, ext_list->capacity, ext_list->capacity * 2,
                                                         VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
            if (new_ptr == NULL) {
                loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                           "loader_add_to_ext_list: Failed to reallocate space for extension list");
                return VK_ERROR_OUT_OF_HOST_MEMORY;
            }
            ext_list->list = new_ptr;

            // double capacity
            ext_list->capacity *= 2;
        }

        // every caller passes the address of an array element or on-stack struct, never NULL
        // NOLINTNEXTLINE(clang-analyzer-core.NonNullParamChecker)
        memcpy(&ext_list->list[ext_list->count], cur_ext, sizeof(VkExtensionProperties));
        ext_list->count++;
    }
    return VK_SUCCESS;
}

// Append one extension property defined in props with entrypoints defined in entries to the given
// ext_list. Do not append if a duplicate.
// If this is a duplicate, this function free's the passed in entries - as in it takes ownership over that list (if it is not
// NULL) Return - Vk_SUCCESS on success
VkResult loader_add_to_dev_ext_list(const struct loader_instance* inst, struct loader_device_extension_list* ext_list,
                                    const VkExtensionProperties* props, struct loader_string_list* entrys) {
    VkResult res = VK_SUCCESS;
    bool should_free_entrys = true;
    if (ext_list->list == NULL || ext_list->capacity == 0) {
        res = loader_init_generic_list(inst, (struct loader_generic_list*)ext_list, sizeof(struct loader_dev_ext_props));
        if (VK_SUCCESS != res) {
            goto out;
        }
    }

    // look for duplicates
    if (has_vk_dev_ext_property(props, ext_list)) {
        goto out;
    }

    uint32_t idx = ext_list->count;
    // add to list at end
    // check for enough capacity
    if (idx * sizeof(struct loader_dev_ext_props) >= ext_list->capacity) {
        void* new_ptr = loader_instance_heap_realloc(inst, ext_list->list, ext_list->capacity, ext_list->capacity * 2,
                                                     VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);

        if (NULL == new_ptr) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_add_to_dev_ext_list: Failed to reallocate space for device extension list");
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }
        ext_list->list = new_ptr;

        // double capacity
        ext_list->capacity *= 2;
    }

    // every caller passes the address of an array element or on-stack struct, never NULL
    // NOLINTNEXTLINE(clang-analyzer-core.NonNullParamChecker)
    memcpy(&ext_list->list[idx].props, props, sizeof(*props));
    if (entrys) {
        ext_list->list[idx].entrypoints = *entrys;
        should_free_entrys = false;
    } else {
        memset(&ext_list->list[idx].entrypoints, 0, sizeof(ext_list->list[idx].entrypoints));
    }
    ext_list->count++;
out:
    if (NULL != entrys && should_free_entrys) {
        free_string_list(inst, entrys);
    }
    return res;
}

// Create storage for pointers to loader_layer_properties
bool loader_init_pointer_layer_list(struct loader_instance* inst) {
    struct loader_pointer_layer_list* list = &inst->expanded_activated_layer_list;

    list->capacity = 32 * sizeof(void*);
    list->list =
        (struct loader_layer_properties**)loader_instance_heap_calloc(inst, list->capacity, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    if (list->list == NULL) {
        return false;
    }
    list->count = 0;
    return true;
}

// Search the given array of layer names for an entry matching the given VkLayerProperties
bool loader_names_array_has_layer_property(const VkLayerProperties* vk_layer_prop, uint32_t layer_info_count,
                                           struct activated_layer_info* layer_info) {
    for (uint32_t i = 0; i < layer_info_count; i++) {
        if (strcmp(vk_layer_prop->layerName, layer_info[i].name) == 0) {
            return true;
        }
    }
    return false;
}

void loader_destroy_pointer_layer_list(const struct loader_instance* inst, struct loader_pointer_layer_list* layer_list) {
    loader_instance_heap_free(inst, (void*)layer_list->list);
    memset(layer_list, 0, sizeof(struct loader_pointer_layer_list));
}

// Append layer properties defined in prop_list to the given layer_info list
VkResult loader_add_layer_properties_to_list(struct loader_instance* inst, struct loader_layer_properties* props) {
    struct loader_pointer_layer_list* list = &inst->expanded_activated_layer_list;
    if (list->list == NULL || list->capacity == 0) {
        if (!loader_init_pointer_layer_list(inst)) {
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }
    }

    // Check for enough capacity
    if (((list->count + 1) * sizeof(struct loader_layer_properties*)) >= list->capacity) {
        size_t new_capacity = list->capacity * 2;
        void* new_ptr = loader_instance_heap_realloc(inst, (void*)list->list, list->capacity, new_capacity,
                                                     VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (NULL == new_ptr) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_add_layer_properties_to_list: Realloc failed for when attempting to add new layer");
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }
        list->list = (struct loader_layer_properties**)new_ptr;
        list->capacity = new_capacity;
    }
    list->list[list->count++] = props;

    return VK_SUCCESS;
}

// Determine if the provided explicit layer should be available by querying the appropriate environmental variables.
bool loader_layer_is_available(const struct loader_instance* inst, const struct loader_envvar_all_filters* filters,
                               const struct loader_layer_properties* prop) {
    bool available = true;
    bool is_implicit = (0 == (prop->type_flags & VK_LAYER_TYPE_FLAG_EXPLICIT_LAYER));
    bool disabled_by_type =
        is_implicit ? filters->disable_filter.disable_all_implicit : filters->disable_filter.disable_all_explicit;
    if ((filters->disable_filter.disable_all || disabled_by_type ||
         check_name_matches_filter_environment_var(prop->info.layerName, &filters->disable_filter.additional_filters)) &&
        !check_name_matches_filter_environment_var(prop->info.layerName, &filters->allow_filter)) {
        available = false;
    }
    if (check_name_matches_filter_environment_var(prop->info.layerName, &filters->enable_filter)) {
        available = true;
    } else if (!available) {
        loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                   "Layer \"%s\" forced disabled because name matches filter of env var \'%s\'.", prop->info.layerName,
                   VK_LAYERS_DISABLE_ENV_VAR);
    }

    return available;
}

// Search the given search_list for any layers in the props list.  Add these to the
// output layer_list.
VkResult loader_add_layer_names_to_list(struct loader_instance* inst, const struct loader_envvar_all_filters* filters,
                                        uint32_t name_count, const char* const* names) {
    VkResult err = VK_SUCCESS;

    for (uint32_t i = 0; i < name_count; i++) {
        const char* source_name = names[i];

        struct loader_layer_properties* layer_prop = loader_find_layer_property(source_name, &inst->instance_layer_list);
        if (NULL == layer_prop) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                       "loader_add_layer_names_to_list: Unable to find layer \"%s\"", source_name);
            err = VK_ERROR_LAYER_NOT_PRESENT;
            continue;
        }

        // Make sure the layer isn't already in the output_list, skip adding it if it is.
        if (loader_find_layer_name_in_list(source_name, &inst->expanded_activated_layer_list)) {
            continue;
        }

        if (!loader_layer_is_available(inst, filters, layer_prop)) {
            continue;
        }

        layer_prop->enabled_by_what = ENABLED_BY_WHAT_IN_APPLICATION_API;
        err = loader_add_layer_properties_to_list(inst, layer_prop);
        if (err == VK_ERROR_OUT_OF_HOST_MEMORY) return err;
    }

    return err;
}

// Determine if the provided implicit layer should be enabled by querying the appropriate environmental variables.
// For an implicit layer, at least a disable environment variable is required.
bool loader_implicit_layer_is_enabled(const struct loader_instance* inst, const struct loader_envvar_all_filters* filters,
                                      const struct loader_layer_properties* prop) {
    bool enable = false;
    bool forced_disabled = false;
    bool forced_enabled = false;

    if ((filters->disable_filter.disable_all || filters->disable_filter.disable_all_implicit ||
         check_name_matches_filter_environment_var(prop->info.layerName, &filters->disable_filter.additional_filters)) &&
        !check_name_matches_filter_environment_var(prop->info.layerName, &filters->allow_filter)) {
        forced_disabled = true;
    }
    if (check_name_matches_filter_environment_var(prop->info.layerName, &filters->enable_filter)) {
        forced_enabled = true;
    }

    // If no enable_environment variable is specified, this implicit layer is always be enabled by default.
    if (NULL == prop->enable_env_var.name) {
        enable = true;
    } else {
        char* env_value = loader_getenv(prop->enable_env_var.name, inst);
        if (env_value && !strcmp(prop->enable_env_var.value, env_value)) {
            enable = true;
        }

        // Otherwise, only enable this layer if the enable environment variable is defined
        loader_free_getenv(env_value, inst);
    }

    if (forced_enabled) {
        // Only report a message that we've forced on a layer if it wouldn't have been enabled
        // normally.
        if (!enable) {
            enable = true;
            loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                       "Implicit layer \"%s\" forced enabled due to env var \'%s\'.", prop->info.layerName,
                       VK_LAYERS_ENABLE_ENV_VAR);
        }
    } else if (enable && forced_disabled) {
        enable = false;
        // Report a message that we've forced off a layer if it would have been enabled normally.
        loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                   "Implicit layer \"%s\" forced disabled because name matches filter of env var \'%s\'.", prop->info.layerName,
                   VK_LAYERS_DISABLE_ENV_VAR);
        return enable;
    }

    // The disable_environment has priority over everything else.  If it is defined, the layer is always
    // disabled.
    if (NULL != prop->disable_env_var.name) {
        char* env_value = loader_getenv(prop->disable_env_var.name, inst);
        if (NULL != env_value) {
            enable = false;
        }
        loader_free_getenv(env_value, inst);
    } else if ((prop->type_flags & VK_LAYER_TYPE_FLAG_EXPLICIT_LAYER) == 0) {
        loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                   "Implicit layer \"%s\" missing disabled environment variable!", prop->info.layerName);
    }

    return enable;
}

// Check the individual implicit layer for the enable/disable environment variable settings.  Only add it after
// every check has passed indicating it should be used, including making sure a layer of the same name hasn't already been
// added.
VkResult loader_add_implicit_layer(struct loader_instance* inst, struct loader_layer_properties* prop,
                                   const struct loader_envvar_all_filters* filters) {
    VkResult result = VK_SUCCESS;
    if (loader_implicit_layer_is_enabled(inst, filters, prop)) {
        // Make sure the layer isn't already in the output_list, skip adding it if it is.
        if (loader_find_layer_name_in_list(&prop->info.layerName[0], &inst->expanded_activated_layer_list)) {
            return result;
        }
        prop->enabled_by_what = ENABLED_BY_WHAT_IMPLICIT_LAYER;
        result = loader_add_layer_properties_to_list(inst, prop);
        if (result == VK_ERROR_OUT_OF_HOST_MEMORY) return result;
    }
    return result;
}

VkExtensionProperties* get_extension_property(const char* name, const struct loader_extension_list* list) {
    for (uint32_t i = 0; i < list->count; i++) {
        if (strcmp(name, list->list[i].extensionName) == 0) return &list->list[i];
    }
    return NULL;
}

VkExtensionProperties* get_dev_extension_property(const char* name, const struct loader_device_extension_list* list) {
    for (uint32_t i = 0; i < list->count; i++) {
        if (strcmp(name, list->list[i].props.extensionName) == 0) return &list->list[i].props;
    }
    return NULL;
}

// For Instance extensions implemented within the loader (i.e. DEBUG_REPORT
// the extension must provide two entry points for the loader to use:
// - "trampoline" entry point - this is the address returned by GetProcAddr
//                              and will always do what's necessary to support a
//                              global call.
// - "terminator" function    - this function will be put at the end of the
//                              instance chain and will contain the necessary logic
//                              to call / process the extension for the appropriate
//                              ICDs that are available.
// There is no generic mechanism for including these functions, the references
// must be placed into the appropriate loader entry points.
// GetInstanceProcAddr: call extension GetInstanceProcAddr to check for GetProcAddr
// requests
// loader_coalesce_extensions(void) - add extension records to the list of global
//                                    extension available to the app.
// instance_disp                    - add function pointer for terminator function
//                                    to this array.
// The extension itself should be in a separate file that will be linked directly
// with the loader.
VkResult loader_get_icd_loader_instance_extensions(const struct loader_instance* inst, struct loader_icd_tramp_list* icd_tramp_list,
                                                   struct loader_extension_list* inst_exts) {
    struct loader_extension_list icd_exts;
    VkResult res = VK_SUCCESS;
    char* env_value;
    bool filter_extensions = true;

    // Check if a user wants to disable the instance extension filtering behavior
    env_value = loader_getenv("VK_LOADER_DISABLE_INST_EXT_FILTER", inst);
    if (NULL != env_value && strtol(env_value, NULL, 10) != 0) {
        filter_extensions = false;
    }
    loader_free_getenv(env_value, inst);

    // traverse scanned icd list adding non-duplicate extensions to the list
    for (uint32_t i = 0; i < icd_tramp_list->count; i++) {
        res = loader_init_generic_list(inst, (struct loader_generic_list*)&icd_exts, sizeof(VkExtensionProperties));
        if (VK_SUCCESS != res) {
            goto out;
        }
        res = loader_add_instance_extensions(inst, icd_tramp_list->scanned_list[i].EnumerateInstanceExtensionProperties,
                                             icd_tramp_list->scanned_list[i].lib_name, &icd_exts);
        if (VK_SUCCESS == res) {
            if (filter_extensions) {
                // Remove any extensions not recognized by the loader
                for (int32_t j = 0; j < (int32_t)icd_exts.count; j++) {
                    // See if the extension is in the list of supported extensions
                    bool found = false;
                    for (uint32_t k = 0; LOADER_INSTANCE_EXTENSIONS[k] != NULL; k++) {
                        if (strcmp(icd_exts.list[j].extensionName, LOADER_INSTANCE_EXTENSIONS[k]) == 0) {
                            found = true;
                            break;
                        }
                    }

                    // If it isn't in the list, remove it
                    if (!found) {
                        for (uint32_t k = j + 1; k < icd_exts.count; k++) {
                            icd_exts.list[k - 1] = icd_exts.list[k];
                        }
                        --icd_exts.count;
                        --j;
                    }
                }
            }

            res = loader_add_to_ext_list(inst, inst_exts, icd_exts.count, icd_exts.list);
        }
        loader_destroy_generic_list(inst, (struct loader_generic_list*)&icd_exts);
        if (VK_SUCCESS != res) {
            goto out;
        }
    };

    // Traverse loader's extensions, adding non-duplicate extensions to the list
    res = add_debug_extensions_to_ext_list(inst, inst_exts);
    if (res == VK_ERROR_OUT_OF_HOST_MEMORY) {
        goto out;
    }
    const VkExtensionProperties portability_enumeration_extension_info[] = {
        {VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME, VK_KHR_PORTABILITY_ENUMERATION_SPEC_VERSION}};

    // Add VK_KHR_portability_subset
    res = loader_add_to_ext_list(inst, inst_exts, sizeof(portability_enumeration_extension_info) / sizeof(VkExtensionProperties),
                                 portability_enumeration_extension_info);
    if (res == VK_ERROR_OUT_OF_HOST_MEMORY) {
        goto out;
    }

    const VkExtensionProperties direct_driver_loading_extension_info[] = {
        {VK_LUNARG_DIRECT_DRIVER_LOADING_EXTENSION_NAME, VK_LUNARG_DIRECT_DRIVER_LOADING_SPEC_VERSION}};

    // Add VK_LUNARG_direct_driver_loading
    res = loader_add_to_ext_list(inst, inst_exts, sizeof(direct_driver_loading_extension_info) / sizeof(VkExtensionProperties),
                                 direct_driver_loading_extension_info);
    if (res == VK_ERROR_OUT_OF_HOST_MEMORY) {
        goto out;
    }

out:
    return res;
}

struct loader_icd_term* loader_get_icd_and_device(const void* device, struct loader_device** found_dev) {
    VkLayerDispatchTable* dispatch_table_device = loader_get_dispatch(device);
    if (NULL == dispatch_table_device) {
        *found_dev = NULL;
        return NULL;
    }
    loader_platform_thread_lock_mutex(&loader_lock);
    *found_dev = NULL;

    for (struct loader_instance* inst = loader.instances; inst; inst = inst->next) {
        for (struct loader_icd_term* icd_term = inst->icd_terms; icd_term; icd_term = icd_term->next) {
            for (struct loader_device* dev = icd_term->logical_device_list; dev; dev = dev->next) {
                // Value comparison of device prevents object wrapping by layers
                if (loader_get_dispatch(dev->icd_device) == dispatch_table_device ||
                    (dev->chain_device != VK_NULL_HANDLE && loader_get_dispatch(dev->chain_device) == dispatch_table_device)) {
                    *found_dev = dev;
                    loader_platform_thread_unlock_mutex(&loader_lock);
                    return icd_term;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&loader_lock);
    return NULL;
}

void loader_destroy_logical_device(struct loader_device* dev, const VkAllocationCallbacks* pAllocator) {
    if (pAllocator) {
        dev->alloc_callbacks = *pAllocator;
    }
    loader_device_heap_free(dev, dev);
}

struct loader_device* loader_create_logical_device(const struct loader_instance* inst, const VkAllocationCallbacks* pAllocator) {
    struct loader_device* new_dev;
    new_dev = loader_calloc(pAllocator, sizeof(struct loader_device), VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);

    if (!new_dev) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_create_logical_device: Failed to alloc struct loader_device");
        return NULL;
    }

    new_dev->loader_dispatch.core_dispatch.magic = DEVICE_DISP_TABLE_MAGIC_NUMBER;

    if (pAllocator) {
        new_dev->alloc_callbacks = *pAllocator;
    }

    return new_dev;
}

void loader_add_logical_device(struct loader_icd_term* icd_term, struct loader_device* dev) {
    dev->next = icd_term->logical_device_list;
    icd_term->logical_device_list = dev;
}

void loader_remove_logical_device(struct loader_icd_term* icd_term, struct loader_device* found_dev,
                                  const VkAllocationCallbacks* pAllocator) {
    struct loader_device* dev;
    struct loader_device* prev_dev;

    if (!icd_term || !found_dev) return;

    prev_dev = NULL;
    dev = icd_term->logical_device_list;
    while (dev && dev != found_dev) {
        prev_dev = dev;
        dev = dev->next;
    }

    if (prev_dev)
        prev_dev->next = found_dev->next;
    else
        icd_term->logical_device_list = found_dev->next;
    loader_destroy_logical_device(found_dev, pAllocator);
}

const VkAllocationCallbacks* ignore_null_callback(const VkAllocationCallbacks* callbacks) {
    return NULL != callbacks->pfnAllocation && NULL != callbacks->pfnFree && NULL != callbacks->pfnReallocation &&
                   NULL != callbacks->pfnInternalAllocation && NULL != callbacks->pfnInternalFree
               ? callbacks
               : NULL;
}

// Try to close any open objects on the loader_icd_term - this must be done before destroying the instance
void loader_icd_close_objects(struct loader_instance* ptr_inst, struct loader_icd_term* icd_term) {
    for (uint32_t i = 0; i < icd_term->surface_list.capacity / sizeof(VkSurfaceKHR); i++) {
        if (ptr_inst->surfaces_list.capacity > i * sizeof(struct loader_used_object_status) &&
            ptr_inst->surfaces_list.list[i].status == VK_TRUE && NULL != icd_term->surface_list.list &&
            icd_term->surface_list.list[i] && NULL != icd_term->dispatch.DestroySurfaceKHR) {
            icd_term->dispatch.DestroySurfaceKHR(icd_term->instance, icd_term->surface_list.list[i],
                                                 ignore_null_callback(&(ptr_inst->surfaces_list.list[i].allocation_callbacks)));
            icd_term->surface_list.list[i] = VK_NULL_HANDLE;
        }
    }
    for (uint32_t i = 0; i < icd_term->debug_utils_messenger_list.capacity / sizeof(VkDebugUtilsMessengerEXT); i++) {
        if (ptr_inst->debug_utils_messengers_list.capacity > i * sizeof(struct loader_used_object_status) &&
            ptr_inst->debug_utils_messengers_list.list[i].status == VK_TRUE && NULL != icd_term->debug_utils_messenger_list.list &&
            icd_term->debug_utils_messenger_list.list[i] && NULL != icd_term->dispatch.DestroyDebugUtilsMessengerEXT) {
            icd_term->dispatch.DestroyDebugUtilsMessengerEXT(
                icd_term->instance, icd_term->debug_utils_messenger_list.list[i],
                ignore_null_callback(&(ptr_inst->debug_utils_messengers_list.list[i].allocation_callbacks)));
            icd_term->debug_utils_messenger_list.list[i] = VK_NULL_HANDLE;
        }
    }
    for (uint32_t i = 0; i < icd_term->debug_report_callback_list.capacity / sizeof(VkDebugReportCallbackEXT); i++) {
        if (ptr_inst->debug_report_callbacks_list.capacity > i * sizeof(struct loader_used_object_status) &&
            ptr_inst->debug_report_callbacks_list.list[i].status == VK_TRUE && NULL != icd_term->debug_report_callback_list.list &&
            icd_term->debug_report_callback_list.list[i] && NULL != icd_term->dispatch.DestroyDebugReportCallbackEXT) {
            icd_term->dispatch.DestroyDebugReportCallbackEXT(
                icd_term->instance, icd_term->debug_report_callback_list.list[i],
                ignore_null_callback(&(ptr_inst->debug_report_callbacks_list.list[i].allocation_callbacks)));
            icd_term->debug_report_callback_list.list[i] = VK_NULL_HANDLE;
        }
    }
}
// Free resources allocated inside the loader_icd_term
void loader_icd_destroy(struct loader_instance* ptr_inst, struct loader_icd_term* icd_term,
                        const VkAllocationCallbacks* pAllocator) {
    ptr_inst->icd_terms_count--;
    for (struct loader_device* dev = icd_term->logical_device_list; dev;) {
        struct loader_device* next_dev = dev->next;
        loader_destroy_logical_device(dev, pAllocator);
        dev = next_dev;
    }

    loader_destroy_generic_list(ptr_inst, (struct loader_generic_list*)&icd_term->surface_list);
    loader_destroy_generic_list(ptr_inst, (struct loader_generic_list*)&icd_term->debug_utils_messenger_list);
    loader_destroy_generic_list(ptr_inst, (struct loader_generic_list*)&icd_term->debug_report_callback_list);

    loader_instance_heap_free(ptr_inst, icd_term);
}

struct loader_icd_term* loader_icd_add(struct loader_instance* ptr_inst, const struct loader_scanned_icd* scanned_icd) {
    struct loader_icd_term* icd_term;

    icd_term = loader_instance_heap_calloc(ptr_inst, sizeof(struct loader_icd_term), VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    if (!icd_term) {
        return NULL;
    }

    icd_term->scanned_icd = scanned_icd;
    icd_term->this_instance = ptr_inst;

    // Prepend to the list
    icd_term->next = ptr_inst->icd_terms;
    ptr_inst->icd_terms = icd_term;
    ptr_inst->icd_terms_count++;

    return icd_term;
}

// Determine the ICD interface version to use.
//     @param icd
//     @param pVersion Output parameter indicating which version to use or 0 if
//            the negotiation API is not supported by the ICD
//     @return  bool indicating true if the selected interface version is supported
//            by the loader, false indicates the version is not supported
bool loader_get_icd_interface_version(PFN_vkNegotiateLoaderICDInterfaceVersion fp_negotiate_icd_version, uint32_t* pVersion) {
    if (fp_negotiate_icd_version == NULL) {
        // ICD does not support the negotiation API, it supports version 0 or 1
        // calling code must determine if it is version 0 or 1
        *pVersion = 0;
    } else {
        // ICD supports the negotiation API, so call it with the loader's
        // latest version supported
        *pVersion = CURRENT_LOADER_ICD_INTERFACE_VERSION;
        VkResult result = fp_negotiate_icd_version(pVersion);

        if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
            // ICD no longer supports the loader's latest interface version so
            // fail loading the ICD
            return false;
        }
    }

#if MIN_SUPPORTED_LOADER_ICD_INTERFACE_VERSION > 0
    if (*pVersion < MIN_SUPPORTED_LOADER_ICD_INTERFACE_VERSION) {
        // Loader no longer supports the ICD's latest interface version so fail
        // loading the ICD
        return false;
    }
#endif
    return true;
}

void loader_clear_scanned_icd_list(const struct loader_instance* inst, struct loader_icd_tramp_list* icd_tramp_list) {
    if (0 != icd_tramp_list->capacity && icd_tramp_list->scanned_list) {
        for (uint32_t i = 0; i < icd_tramp_list->count; i++) {
            if (icd_tramp_list->scanned_list[i].handle) {
                loader_platform_close_library(icd_tramp_list->scanned_list[i].handle);
                icd_tramp_list->scanned_list[i].handle = NULL;
            }
            loader_instance_heap_free(inst, icd_tramp_list->scanned_list[i].lib_name);
        }
        loader_instance_heap_free(inst, icd_tramp_list->scanned_list);
    }
    memset(icd_tramp_list, 0, sizeof(struct loader_icd_tramp_list));
}

VkResult loader_init_scanned_icd_list(const struct loader_instance* inst, struct loader_icd_tramp_list* icd_tramp_list) {
    VkResult res = VK_SUCCESS;
    loader_clear_scanned_icd_list(inst, icd_tramp_list);
    icd_tramp_list->capacity = 8 * sizeof(struct loader_scanned_icd);
    icd_tramp_list->scanned_list = loader_instance_heap_calloc(inst, icd_tramp_list->capacity, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    if (NULL == icd_tramp_list->scanned_list) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "loader_init_scanned_icd_list: Realloc failed for layer list when attempting to add new layer");
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    return res;
}

VkResult loader_add_direct_driver(const struct loader_instance* inst, uint32_t index,
                                  const VkDirectDriverLoadingInfoLUNARG* pDriver, struct loader_icd_tramp_list* icd_tramp_list) {
    // Assume pDriver is valid, since there is no real way to check it. Calling code should make sure the pointer to the array
    // of VkDirectDriverLoadingInfoLUNARG structures is non-null.
    if (NULL == pDriver->pfnGetInstanceProcAddr) {
        loader_log(
            inst, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
            "loader_add_direct_driver: VkDirectDriverLoadingInfoLUNARG structure at index %d contains a NULL pointer for the "
            "pfnGetInstanceProcAddr member, skipping.",
            index);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    PFN_vkGetInstanceProcAddr fp_get_proc_addr = pDriver->pfnGetInstanceProcAddr;
    PFN_vkCreateInstance fp_create_inst = NULL;
    PFN_vkEnumerateInstanceExtensionProperties fp_get_inst_ext_props = NULL;
    PFN_GetPhysicalDeviceProcAddr fp_get_phys_dev_proc_addr = NULL;
    PFN_vkNegotiateLoaderICDInterfaceVersion fp_negotiate_icd_version = NULL;
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    PFN_vk_icdEnumerateAdapterPhysicalDevices fp_enum_dxgi_adapter_phys_devs = NULL;
#endif
    struct loader_scanned_icd* new_scanned_icd;
    uint32_t interface_version = 0;

    // Try to get the negotiate ICD interface version function
    fp_negotiate_icd_version = (PFN_vk_icdNegotiateLoaderICDInterfaceVersion)pDriver->pfnGetInstanceProcAddr(
        NULL, "vk_icdNegotiateLoaderICDInterfaceVersion");

    if (NULL == fp_negotiate_icd_version) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "loader_add_direct_driver: Could not get 'vk_icdNegotiateLoaderICDInterfaceVersion' from "
                   "VkDirectDriverLoadingInfoLUNARG structure at "
                   "index %d, skipping.",
                   index);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (!loader_get_icd_interface_version(fp_negotiate_icd_version, &interface_version)) {
        loader_log(
            inst, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
            "loader_add_direct_driver: VkDirectDriverLoadingInfoLUNARG structure at index %d supports interface version %d, "
            "which is incompatible with the Loader Driver Interface version that supports the VK_LUNARG_direct_driver_loading "
            "extension, skipping.",
            index, interface_version);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (interface_version < 7) {
        loader_log(
            inst, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
            "loader_add_direct_driver: VkDirectDriverLoadingInfoLUNARG structure at index %d supports interface version %d, "
            "which is incompatible with the Loader Driver Interface version that supports the VK_LUNARG_direct_driver_loading "
            "extension, skipping.",
            index, interface_version);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    fp_create_inst = (PFN_vkCreateInstance)pDriver->pfnGetInstanceProcAddr(NULL, "vkCreateInstance");
    if (NULL == fp_create_inst) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "loader_add_direct_driver: Could not get 'vkCreateInstance' from VkDirectDriverLoadingInfoLUNARG structure at "
                   "index %d, skipping.",
                   index);
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    fp_get_inst_ext_props =
        (PFN_vkEnumerateInstanceExtensionProperties)pDriver->pfnGetInstanceProcAddr(NULL, "vkEnumerateInstanceExtensionProperties");
    if (NULL == fp_get_inst_ext_props) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "loader_add_direct_driver: Could not get 'vkEnumerateInstanceExtensionProperties' from "
                   "VkDirectDriverLoadingInfoLUNARG structure at index %d, skipping.",
                   index);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    fp_get_phys_dev_proc_addr =
        (PFN_vk_icdGetPhysicalDeviceProcAddr)pDriver->pfnGetInstanceProcAddr(NULL, "vk_icdGetPhysicalDeviceProcAddr");
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    // Query "vk_icdEnumerateAdapterPhysicalDevices" with vk_icdGetInstanceProcAddr if the library reports interface version
    // 7 or greater, otherwise fallback to loading it from the platform dynamic linker
    fp_enum_dxgi_adapter_phys_devs =
        (PFN_vk_icdEnumerateAdapterPhysicalDevices)pDriver->pfnGetInstanceProcAddr(NULL, "vk_icdEnumerateAdapterPhysicalDevices");
#endif

    // check for enough capacity
    if ((icd_tramp_list->count * sizeof(struct loader_scanned_icd)) >= icd_tramp_list->capacity) {
        void* new_ptr = loader_instance_heap_realloc(inst, icd_tramp_list->scanned_list, icd_tramp_list->capacity,
                                                     icd_tramp_list->capacity * 2, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (NULL == new_ptr) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_add_direct_driver: Realloc failed on icd library list for ICD index %u", index);
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }
        icd_tramp_list->scanned_list = new_ptr;

        // double capacity
        icd_tramp_list->capacity *= 2;
    }

    // Driver must be 1.1 to support version 7
    uint32_t api_version = VK_API_VERSION_1_1;
    PFN_vkEnumerateInstanceVersion icd_enumerate_instance_version =
        (PFN_vkEnumerateInstanceVersion)pDriver->pfnGetInstanceProcAddr(NULL, "vkEnumerateInstanceVersion");

    if (icd_enumerate_instance_version) {
        VkResult res = icd_enumerate_instance_version(&api_version);
        if (res != VK_SUCCESS) {
            return res;
        }
    }

    new_scanned_icd = &(icd_tramp_list->scanned_list[icd_tramp_list->count]);
    new_scanned_icd->handle = NULL;
    new_scanned_icd->api_version = api_version;
    new_scanned_icd->GetInstanceProcAddr = fp_get_proc_addr;
    new_scanned_icd->GetPhysicalDeviceProcAddr = fp_get_phys_dev_proc_addr;
    new_scanned_icd->EnumerateInstanceExtensionProperties = fp_get_inst_ext_props;
    new_scanned_icd->CreateInstance = fp_create_inst;
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    new_scanned_icd->EnumerateAdapterPhysicalDevices = fp_enum_dxgi_adapter_phys_devs;
#endif
    new_scanned_icd->interface_version = interface_version;

    new_scanned_icd->lib_name = NULL;
    icd_tramp_list->count++;

    loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
               "loader_add_direct_driver: Adding driver found in index %d of "
               "VkDirectDriverLoadingListLUNARG::pDrivers structure. pfnGetInstanceProcAddr was set to %p",
               index, pDriver->pfnGetInstanceProcAddr);

    return VK_SUCCESS;
}

// Search through VkInstanceCreateInfo's pNext chain for any drivers from the direct driver loading extension and load them.
VkResult loader_scan_for_direct_drivers(const struct loader_instance* inst, const VkInstanceCreateInfo* pCreateInfo,
                                        struct loader_icd_tramp_list* icd_tramp_list, bool* direct_driver_loading_exclusive_mode) {
    if (NULL == pCreateInfo) {
        // Don't do this logic unless we are being called from vkCreateInstance, when pCreateInfo will be non-null
        return VK_SUCCESS;
    }
    bool direct_driver_loading_enabled = false;
    // Try to if VK_LUNARG_direct_driver_loading is enabled and if we are using it exclusively
    // Skip this step if inst is NULL, aka when this function is being called before instance creation
    if (inst != NULL && pCreateInfo->ppEnabledExtensionNames && pCreateInfo->enabledExtensionCount > 0) {
        // Look through the enabled extension list, make sure VK_LUNARG_direct_driver_loading is present
        for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; i++) {
            if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_LUNARG_DIRECT_DRIVER_LOADING_EXTENSION_NAME) == 0) {
                direct_driver_loading_enabled = true;
                break;
            }
        }
    }
    const VkDirectDriverLoadingListLUNARG* ddl_list = NULL;
    // Find the VkDirectDriverLoadingListLUNARG struct in the pNext chain of vkInstanceCreateInfo
    const void* pNext = pCreateInfo->pNext;
    while (pNext) {
        VkBaseInStructure out_structure = {0};
        memcpy(&out_structure, pNext, sizeof(VkBaseInStructure));
        if (out_structure.sType == VK_STRUCTURE_TYPE_DIRECT_DRIVER_LOADING_LIST_LUNARG) {
            ddl_list = (VkDirectDriverLoadingListLUNARG*)pNext;
            break;
        }
        pNext = out_structure.pNext;
    }
    if (NULL == ddl_list) {
        if (direct_driver_loading_enabled) {
            loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                       "loader_scan_for_direct_drivers: The VK_LUNARG_direct_driver_loading extension was enabled but the "
                       "pNext chain of "
                       "VkInstanceCreateInfo did not contain the "
                       "VkDirectDriverLoadingListLUNARG structure.");
        }
        // Always want to exit early if there was no VkDirectDriverLoadingListLUNARG in the pNext chain
        return VK_SUCCESS;
    }

    if (!direct_driver_loading_enabled) {
        loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "loader_scan_for_direct_drivers: The pNext chain of VkInstanceCreateInfo contained the "
                   "VkDirectDriverLoadingListLUNARG structure, but the VK_LUNARG_direct_driver_loading extension was "
                   "not enabled.");
        return VK_SUCCESS;
    }
    // If we are using exclusive mode, skip looking for any more drivers from system or environment variables
    if (ddl_list->mode == VK_DIRECT_DRIVER_LOADING_MODE_EXCLUSIVE_LUNARG) {
        *direct_driver_loading_exclusive_mode = true;
        loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "loader_scan_for_direct_drivers: The VK_LUNARG_direct_driver_loading extension is active and specified "
                   "VK_DIRECT_DRIVER_LOADING_MODE_EXCLUSIVE_LUNARG, skipping system and environment "
                   "variable driver search mechanisms.");
    }
    if (NULL == ddl_list->pDrivers) {
        loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "loader_scan_for_direct_drivers: The VkDirectDriverLoadingListLUNARG structure in the pNext chain of "
                   "VkInstanceCreateInfo has a NULL pDrivers member.");
        return VK_SUCCESS;
    }
    if (ddl_list->driverCount == 0) {
        loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "loader_scan_for_direct_drivers: The VkDirectDriverLoadingListLUNARG structure in the pNext chain of "
                   "VkInstanceCreateInfo has a non-null pDrivers member but a driverCount member with a value "
                   "of zero.");
        return VK_SUCCESS;
    }
    // Go through all VkDirectDriverLoadingInfoLUNARG entries and add each driver
    // Because icd_tramp's are prepended, this will result in the drivers appearing at the end
    for (uint32_t i = 0; i < ddl_list->driverCount; i++) {
        VkResult res = loader_add_direct_driver(inst, i, &ddl_list->pDrivers[i], icd_tramp_list);
        if (res == VK_ERROR_OUT_OF_HOST_MEMORY) {
            return res;
        }
    }

    return VK_SUCCESS;
}

VkResult loader_scanned_icd_add(const struct loader_instance* inst, struct loader_icd_tramp_list* icd_tramp_list,
                                const char* filename, uint32_t api_version, enum loader_layer_library_status* lib_status) {
    loader_platform_dl_handle handle = NULL;
    PFN_vkCreateInstance fp_create_inst = NULL;
    PFN_vkEnumerateInstanceExtensionProperties fp_get_inst_ext_props = NULL;
    PFN_vkGetInstanceProcAddr fp_get_proc_addr = NULL;
    PFN_GetPhysicalDeviceProcAddr fp_get_phys_dev_proc_addr = NULL;
    PFN_vkNegotiateLoaderICDInterfaceVersion fp_negotiate_icd_version = NULL;
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    PFN_vk_icdEnumerateAdapterPhysicalDevices fp_enum_dxgi_adapter_phys_devs = NULL;
#endif
    struct loader_scanned_icd* new_scanned_icd = NULL;
    uint32_t interface_vers;
    VkResult res = VK_SUCCESS;

    // loader_icd_scan reads *lib_status whenever this returns VK_ERROR_INCOMPATIBLE_DRIVER, but several of the failure
    // paths below (NULL filename, missing vkCreateInstance / vkEnumerateInstanceExtensionProperties) don't set it. Seed a
    // default so the caller never switches on an indeterminate value; the more specific paths still override it.
    if (NULL != lib_status) {
        *lib_status = LOADER_LAYER_LIB_ERROR_FAILED_TO_LOAD;
    }

    // This shouldn't happen, but the check is necessary because dlopen returns a handle to the main program when
    // filename is NULL
    if (filename == NULL) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_scanned_icd_add: A NULL filename was used, skipping this ICD");
        res = VK_ERROR_INCOMPATIBLE_DRIVER;
        goto out;
    }

    bool oom = false;
// TODO implement smarter opening/closing of libraries. For now this
// function leaves libraries open and the scanned_icd_clear closes them
#if defined(__Fuchsia__)
    handle = loader_platform_open_driver(filename);
#else
    handle = loader_platform_open_library(inst, filename, &oom);
#endif
    if (oom) {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }
    if (NULL == handle) {
        loader_handle_load_library_error(inst, filename, lib_status);
        if (lib_status && *lib_status == LOADER_LAYER_LIB_ERROR_OUT_OF_MEMORY) {
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
        } else {
            res = VK_ERROR_INCOMPATIBLE_DRIVER;
        }
        goto out;
    }

    // Try to load the driver's exported vk_icdNegotiateLoaderICDInterfaceVersion
    fp_negotiate_icd_version = loader_platform_get_proc_address(handle, "vk_icdNegotiateLoaderICDInterfaceVersion");

    // If it isn't exported, we are dealing with either a v0, v1, or a v7 and up driver
    if (NULL == fp_negotiate_icd_version) {
        // Try to load the driver's exported vk_icdGetInstanceProcAddr - if this is a v7 or up driver, we can use it to get
        // the driver's vk_icdNegotiateLoaderICDInterfaceVersion function
        fp_get_proc_addr = loader_platform_get_proc_address(handle, "vk_icdGetInstanceProcAddr");

        // If we successfully loaded vk_icdGetInstanceProcAddr, try to get vk_icdNegotiateLoaderICDInterfaceVersion
        if (fp_get_proc_addr) {
            fp_negotiate_icd_version =
                (PFN_vk_icdNegotiateLoaderICDInterfaceVersion)fp_get_proc_addr(NULL, "vk_icdNegotiateLoaderICDInterfaceVersion");
        }
    }

    // Try to negotiate the Loader and Driver Interface Versions
    // loader_get_icd_interface_version will check if fp_negotiate_icd_version is NULL, so we don't have to.
    // If it *is* NULL, that means this driver uses interface version 0 or 1
    if (!loader_get_icd_interface_version(fp_negotiate_icd_version, &interface_vers)) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "loader_scanned_icd_add: ICD %s doesn't support interface version compatible with loader, skip this ICD.",
                   filename);
        res = VK_ERROR_INCOMPATIBLE_DRIVER;
        *lib_status = LOADER_LAYER_LIB_ERROR_NEGOTIATE_INTERFACE_VERSION_FAILED;
        goto out;
    }

    // If we didn't already query vk_icdGetInstanceProcAddr, try now
    if (NULL == fp_get_proc_addr) {
        fp_get_proc_addr = loader_platform_get_proc_address(handle, "vk_icdGetInstanceProcAddr");
    }

    // If vk_icdGetInstanceProcAddr is NULL, this ICD is using version 0 and so we should respond accordingly.
    if (NULL == fp_get_proc_addr) {
        // Exporting vk_icdNegotiateLoaderICDInterfaceVersion but not vk_icdGetInstanceProcAddr violates Version 2's
        // requirements, as for Version 2 to be supported Version 1 must also be supported
        if (interface_vers != 0) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_scanned_icd_add: ICD %s reports an interface version of %d but doesn't export "
                       "vk_icdGetInstanceProcAddr, skip this ICD.",
                       filename, interface_vers);
            res = VK_ERROR_INCOMPATIBLE_DRIVER;
            *lib_status = LOADER_LAYER_LIB_ERROR_UNABLE_TO_FIND_VK_GET_INSTANCE_PROC_ADDR;
            goto out;
        }
        // Use deprecated interface from version 0
        fp_get_proc_addr = loader_platform_get_proc_address(handle, "vkGetInstanceProcAddr");
        if (NULL == fp_get_proc_addr) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_scanned_icd_add: Attempt to retrieve either \'vkGetInstanceProcAddr\' or "
                       "\'vk_icdGetInstanceProcAddr\' from ICD %s failed.",
                       filename);
            res = VK_ERROR_INCOMPATIBLE_DRIVER;
            *lib_status = LOADER_LAYER_LIB_ERROR_UNABLE_TO_FIND_VK_GET_INSTANCE_PROC_ADDR;
            goto out;
        } else {
            loader_log(inst, VULKAN_LOADER_WARN_BIT, 0,
                       "loader_scanned_icd_add: Using deprecated ICD interface of \'vkGetInstanceProcAddr\' instead of "
                       "\'vk_icdGetInstanceProcAddr\' for ICD %s",
                       filename);
        }
        fp_create_inst = loader_platform_get_proc_address(handle, "vkCreateInstance");
        if (NULL == fp_create_inst) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_scanned_icd_add:  Failed querying \'vkCreateInstance\' via dlsym/LoadLibrary for ICD %s", filename);
            res = VK_ERROR_INCOMPATIBLE_DRIVER;
            goto out;
        }
        fp_get_inst_ext_props = loader_platform_get_proc_address(handle, "vkEnumerateInstanceExtensionProperties");
        if (NULL == fp_get_inst_ext_props) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_scanned_icd_add: Could not get \'vkEnumerateInstanceExtensionProperties\' via dlsym/LoadLibrary "
                       "for ICD %s",
                       filename);
            res = VK_ERROR_INCOMPATIBLE_DRIVER;
            goto out;
        }
    } else {
        // vk_icdGetInstanceProcAddr was successfully found, we can assume the version is at least one
        // If vk_icdNegotiateLoaderICDInterfaceVersion was also found, interface_vers must be 2 or greater, so this check is
        // fine
        if (interface_vers == 0) {
            interface_vers = 1;
        }

        fp_create_inst = (PFN_vkCreateInstance)fp_get_proc_addr(NULL, "vkCreateInstance");
        if (NULL == fp_create_inst) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_scanned_icd_add: Could not get \'vkCreateInstance\' via \'vk_icdGetInstanceProcAddr\' for ICD %s",
                       filename);
            res = VK_ERROR_INCOMPATIBLE_DRIVER;
            *lib_status = LOADER_LAYER_LIB_ERROR_UNABLE_TO_FIND_VK_GET_INSTANCE_PROC_ADDR;
            goto out;
        }
        fp_get_inst_ext_props =
            (PFN_vkEnumerateInstanceExtensionProperties)fp_get_proc_addr(NULL, "vkEnumerateInstanceExtensionProperties");
        if (NULL == fp_get_inst_ext_props) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_scanned_icd_add: Could not get \'vkEnumerateInstanceExtensionProperties\' via "
                       "\'vk_icdGetInstanceProcAddr\' for ICD %s",
                       filename);
            res = VK_ERROR_INCOMPATIBLE_DRIVER;
            goto out;
        }
        // Query "vk_icdGetPhysicalDeviceProcAddr" with vk_icdGetInstanceProcAddr if the library reports interface version 7 or
        // greater, otherwise fallback to loading it from the platform dynamic linker
        if (interface_vers >= 7) {
            fp_get_phys_dev_proc_addr =
                (PFN_vk_icdGetPhysicalDeviceProcAddr)fp_get_proc_addr(NULL, "vk_icdGetPhysicalDeviceProcAddr");
        }
        if (NULL == fp_get_phys_dev_proc_addr && interface_vers >= 3) {
            fp_get_phys_dev_proc_addr = loader_platform_get_proc_address(handle, "vk_icdGetPhysicalDeviceProcAddr");
        }
#if defined(VK_USE_PLATFORM_WIN32_KHR)
        // Query "vk_icdEnumerateAdapterPhysicalDevices" with vk_icdGetInstanceProcAddr if the library reports interface version
        // 7 or greater, otherwise fallback to loading it from the platform dynamic linker
        if (interface_vers >= 7) {
            fp_enum_dxgi_adapter_phys_devs =
                (PFN_vk_icdEnumerateAdapterPhysicalDevices)fp_get_proc_addr(NULL, "vk_icdEnumerateAdapterPhysicalDevices");
        }
        if (NULL == fp_enum_dxgi_adapter_phys_devs && interface_vers >= 6) {
            fp_enum_dxgi_adapter_phys_devs = loader_platform_get_proc_address(handle, "vk_icdEnumerateAdapterPhysicalDevices");
        }
#endif
    }

    // check for enough capacity
    if ((icd_tramp_list->count * sizeof(struct loader_scanned_icd)) >= icd_tramp_list->capacity) {
        void* new_ptr = loader_instance_heap_realloc(inst, icd_tramp_list->scanned_list, icd_tramp_list->capacity,
                                                     icd_tramp_list->capacity * 2, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (NULL == new_ptr) {
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_scanned_icd_add: Realloc failed on icd library list for ICD %s",
                       filename);
            goto out;
        }
        icd_tramp_list->scanned_list = new_ptr;

        // double capacity
        icd_tramp_list->capacity *= 2;
    }

    loader_api_version api_version_struct = loader_make_version(api_version);
    if (interface_vers <= 4 && loader_check_version_meets_required(LOADER_VERSION_1_1_0, api_version_struct)) {
        loader_log(inst, VULKAN_LOADER_WARN_BIT, 0,
                   "loader_scanned_icd_add: Driver %s supports Vulkan %u.%u, but only supports loader interface version %u."
                   " Interface version 5 or newer required to support this version of Vulkan (Policy #LDP_DRIVER_7)",
                   filename, api_version_struct.major, api_version_struct.minor, interface_vers);
    }

    new_scanned_icd = &(icd_tramp_list->scanned_list[icd_tramp_list->count]);
    new_scanned_icd->handle = handle;
    new_scanned_icd->api_version = api_version;
    new_scanned_icd->GetInstanceProcAddr = fp_get_proc_addr;
    new_scanned_icd->GetPhysicalDeviceProcAddr = fp_get_phys_dev_proc_addr;
    new_scanned_icd->EnumerateInstanceExtensionProperties = fp_get_inst_ext_props;
    new_scanned_icd->CreateInstance = fp_create_inst;
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    new_scanned_icd->EnumerateAdapterPhysicalDevices = fp_enum_dxgi_adapter_phys_devs;
#endif
    new_scanned_icd->interface_version = interface_vers;

    res = loader_copy_to_new_str(inst, filename, &new_scanned_icd->lib_name);
    if (VK_ERROR_OUT_OF_HOST_MEMORY == res) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_scanned_icd_add: Out of memory can't add ICD %s", filename);
        goto out;
    }

    // Uses OS calls to find the 'true' path to the binary, for more accurate logging later on.
    res = fixup_library_binary_path(inst, &(new_scanned_icd->lib_name), new_scanned_icd->handle, fp_get_proc_addr);
    if (res == VK_ERROR_OUT_OF_HOST_MEMORY) {
        loader_instance_heap_free(inst, new_scanned_icd->lib_name);
        goto out;
    }

    icd_tramp_list->count++;

out:
    if (res != VK_SUCCESS) {
        if (NULL != handle) {
            loader_platform_close_library(handle);
        }
    }

    return res;
}

#if defined(_WIN32)
BOOL __stdcall loader_initialize(PINIT_ONCE InitOnce, PVOID Parameter, PVOID* Context) {
    (void)InitOnce;
    (void)Parameter;
    (void)Context;
#else
void loader_initialize(void) {
    loader_platform_thread_create_mutex(&loader_lock);
    loader_platform_thread_create_mutex(&loader_preload_icd_lock);
    init_global_loader_settings();
#endif

    // initialize logging
    loader_init_global_debug_level();
#if defined(_WIN32)
    windows_initialization();
#endif

    loader_api_version version = loader_make_full_version(VK_HEADER_VERSION_COMPLETE);
    loader_log(NULL, VULKAN_LOADER_INFO_BIT, 0, "Vulkan Loader Version %d.%d.%d", version.major, version.minor, version.patch);

#if defined(GIT_BRANCH_NAME) && defined(GIT_TAG_INFO)
    loader_log(NULL, VULKAN_LOADER_INFO_BIT, 0, "[Vulkan Loader Git - Tag: " GIT_BRANCH_NAME ", Branch/Commit: " GIT_TAG_INFO "]");
#endif

    char* loader_disable_dynamic_library_unloading_env_var = loader_getenv("VK_LOADER_DISABLE_DYNAMIC_LIBRARY_UNLOADING", NULL);
    if (loader_disable_dynamic_library_unloading_env_var &&
        // NOLINTNEXTLINE(bugprone-not-null-terminated-result) - n=2 intentionally excludes "1x" values like "10"
        0 == strncmp(loader_disable_dynamic_library_unloading_env_var, "1", 2)) {
        loader_disable_dynamic_library_unloading = true;
        loader_log(NULL, VULKAN_LOADER_WARN_BIT, 0, "Vulkan Loader: library unloading is disabled");
    } else {
        loader_disable_dynamic_library_unloading = false;
    }
    loader_free_getenv(loader_disable_dynamic_library_unloading_env_var, NULL);
#if defined(LOADER_USE_UNSAFE_FILE_SEARCH)
    loader_log(NULL, VULKAN_LOADER_WARN_BIT, 0, "Vulkan Loader: unsafe searching is enabled");
#endif
#if defined(_WIN32)
    return TRUE;
#endif
}

void loader_release(void) {
    // Guarantee release of the preloaded ICD libraries. This may have already been called in vkDestroyInstance.
    loader_unload_preloaded_icds();

    // release mutexes
    teardown_global_loader_settings();
    loader_platform_thread_delete_mutex(&loader_lock);
    loader_platform_thread_delete_mutex(&loader_preload_icd_lock);
}

// Preload the ICD libraries that are likely to be needed so we don't repeatedly load/unload them later
void loader_preload_icds(void) {
    loader_platform_thread_lock_mutex(&loader_preload_icd_lock);

    // Already preloaded, skip loading again.
    if (preloaded_icds.scanned_list != NULL) {
        loader_platform_thread_unlock_mutex(&loader_preload_icd_lock);
        return;
    }

    VkResult result = loader_icd_scan(NULL, &preloaded_icds, NULL, NULL);
    if (result != VK_SUCCESS) {
        loader_clear_scanned_icd_list(NULL, &preloaded_icds);
    }
    loader_platform_thread_unlock_mutex(&loader_preload_icd_lock);
}

// Release the ICD libraries that were preloaded
void loader_unload_preloaded_icds(void) {
    loader_platform_thread_lock_mutex(&loader_preload_icd_lock);
    loader_clear_scanned_icd_list(NULL, &preloaded_icds);
    loader_platform_thread_unlock_mutex(&loader_preload_icd_lock);
}

#if !defined(_WIN32)
__attribute__((constructor)) void loader_init_library(void) { loader_initialize(); }

__attribute__((destructor)) void loader_free_library(void) { loader_release(); }
#endif

// Get next file or dirname given a string list or registry key path
//
// \returns
// A pointer to first char in the next path.
// The next path (or NULL) in the list is returned in next_path.
// Note: input string is modified in some cases. PASS IN A COPY!
char* loader_get_next_path(char* path) {
    uint32_t len;
    char* next;

    if (path == NULL) return NULL;
    next = strchr(path, PATH_SEPARATOR);
    if (next == NULL) {
        len = (uint32_t)strlen(path);
        next = path + len;
    } else {
        *next = '\0';
        next++;
    }

    return next;
}

/* Processes a json manifest's library_path and the location of the json manifest to create the path of the library
 * The output is stored in out_fullpath by allocating a string - so its the caller's responsibility to free it
 * The output is the combination of the base path of manifest_file_path concatenated with library path
 * If library_path is an absolute path, we do not prepend the base path of manifest_file_path
 *
 * This function takes ownership of library_path - caller does not need to worry about freeing it.
 */
VkResult combine_manifest_directory_and_library_path(const struct loader_instance* inst, char* library_path,
                                                     const char* manifest_file_path, char** out_fullpath) {
    assert(library_path && manifest_file_path && out_fullpath);
    if (loader_platform_is_path_absolute(library_path)) {
        *out_fullpath = library_path;
        return VK_SUCCESS;
    }
    VkResult res = VK_SUCCESS;

    size_t library_path_len = strlen(library_path);
    size_t manifest_file_path_str_len = strlen(manifest_file_path);
    bool library_path_contains_directory_symbol = false;
    for (size_t i = 0; i < library_path_len; i++) {
        if (library_path[i] == DIRECTORY_SYMBOL) {
            library_path_contains_directory_symbol = true;
            break;
        }
    }
    // Means that the library_path is neither absolute nor relative - thus we should not modify it at all
    if (!library_path_contains_directory_symbol) {
        *out_fullpath = library_path;
        return VK_SUCCESS;
    }
    // must include both a directory symbol and the null terminator
    size_t new_str_len = library_path_len + manifest_file_path_str_len + 1 + 1;

    *out_fullpath = loader_instance_heap_calloc(inst, new_str_len, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    if (NULL == *out_fullpath) {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }
    size_t cur_loc_in_out_fullpath = 0;
    // look for the last occurrence of DIRECTORY_SYMBOL in manifest_file_path
    size_t last_directory_symbol = 0;
    bool found_directory_symbol = false;
    for (size_t i = 0; i < manifest_file_path_str_len; i++) {
        if (manifest_file_path[i] == DIRECTORY_SYMBOL) {
            last_directory_symbol = i + 1;  // we want to include the symbol
            found_directory_symbol = true;
            // dont break because we want to find the last occurrence
        }
    }
    // Add manifest_file_path up to the last directory symbol
    if (found_directory_symbol) {
        loader_strncpy(*out_fullpath, new_str_len, manifest_file_path, last_directory_symbol);
        cur_loc_in_out_fullpath += last_directory_symbol;
    }
    loader_strncpy(&(*out_fullpath)[cur_loc_in_out_fullpath], new_str_len - cur_loc_in_out_fullpath, library_path,
                   library_path_len);
    cur_loc_in_out_fullpath += library_path_len + 1;
    (*out_fullpath)[cur_loc_in_out_fullpath] = '\0';

out:
    loader_instance_heap_free(inst, library_path);

    return res;
}

// Given a filename (file)  and a list of paths (in_dirs), try to find an existing
// file in the paths.  If filename already is a path then no searching in the given paths.
//
// @return - A string in out_fullpath of either the full path or file.
VkResult loader_get_fullpath(const struct loader_instance* inst, const char* file, const char* in_dirs, size_t out_size,
                             char* out_fullpath) {
    char* dirs_copy = NULL;
    VkResult res = VK_SUCCESS;
    if (!loader_platform_is_path(file) && *in_dirs) {
        size_t dirs_copy_len = strlen(in_dirs) + 1;
        dirs_copy = loader_instance_heap_calloc(inst, dirs_copy_len, VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (NULL == dirs_copy) {
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }
        loader_strncpy(dirs_copy, dirs_copy_len, in_dirs, dirs_copy_len);

        // find if file exists after prepending paths in given list
        // for (dir = dirs_copy; *dir && (next_dir = loader_get_next_path(dir)); dir = next_dir) {
        char* dir = dirs_copy;
        char* next_dir = loader_get_next_path(dir);
        while (*dir && next_dir && out_fullpath) {
            int path_concat_ret = snprintf(out_fullpath, out_size, "%s%c%s", dir, DIRECTORY_SYMBOL, file);
            if (path_concat_ret > 0) {
                bool oom = false;
                bool file_exists = loader_platform_file_exists(inst, out_fullpath, &oom);
                if (oom) {
                    res = VK_ERROR_OUT_OF_HOST_MEMORY;
                    goto out;
                }
                if (file_exists) {
                    goto out;
                }
            }
            dir = next_dir;
            next_dir = loader_get_next_path(dir);
        }
    }

    (void)snprintf(out_fullpath, out_size, "%s", file);

out:
    loader_instance_heap_free(inst, dirs_copy);
    return res;
}

/* The following are required in the "layer" object:
 * "name"
 * "type"
 * "library_path"
 * "api_version"
 * "implementation_version"
 * "description"
 * (for implicit layers) "disable_environment"
 */

VkResult loader_read_layer_json(const struct loader_instance* inst, struct loader_layer_list* layer_instance_list,
                                cJSON* layer_node, loader_api_version version, bool is_implicit, char* filename) {
    assert(layer_instance_list);
    char* library_path = NULL;
    VkResult result = VK_SUCCESS;
    struct loader_layer_properties props = {0};

    result = loader_copy_to_new_str(inst, filename, &props.manifest_file_name);
    if (result == VK_ERROR_OUT_OF_HOST_MEMORY) {
        goto out;
    }

    // Parse name

    result = loader_parse_json_string_to_existing_str(layer_node, "name", VK_MAX_EXTENSION_NAME_SIZE, props.info.layerName);
    if (VK_ERROR_INITIALIZATION_FAILED == result) {
        loader_log(inst, VULKAN_LOADER_WARN_BIT, 0,
                   "Layer located at %s didn't find required layer value \"name\" in manifest JSON file, skipping this layer",
                   filename);
        goto out;
    }

    if (0 != strncmp(props.info.layerName, "VK_LAYER_", 9)) {
        loader_log(inst, VULKAN_LOADER_WARN_BIT, 0, "Layer name %s does not conform to naming standard (Policy #LLP_LAYER_3)",
                   props.info.layerName);
    }

    // Parse type
    char* type = loader_cJSON_GetStringValue(loader_cJSON_GetObjectItem(layer_node, "type"));
    if (NULL == type) {
        loader_log(inst, VULKAN_LOADER_WARN_BIT, 0,
                   "Layer located at %s didn't find required layer value \"type\" in manifest JSON file, skipping this layer",
                   filename);
        result = VK_ERROR_INITIALIZATION_FAILED;
        goto out;
    }

    // Add list entry
    if (!strcmp(type, "DEVICE")) {
        loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_LAYER_BIT, 0, "Device layers are deprecated. Skipping layer %s",
                   props.info.layerName);
        result = VK_ERROR_INITIALIZATION_FAILED;
        goto out;
    }

    // Allow either GLOBAL or INSTANCE type interchangeably to handle layers that must work with older loaders
    if (!strcmp(type, "INSTANCE") || !strcmp(type, "GLOBAL")) {
        props.type_flags = VK_LAYER_TYPE_FLAG_INSTANCE_LAYER;
        if (!is_implicit) {
            props.type_flags |= VK_LAYER_TYPE_FLAG_EXPLICIT_LAYER;
        }
    } else {
        result = VK_ERROR_INITIALIZATION_FAILED;
        goto out;
    }

    // Parse api_version
    char* api_version = loader_cJSON_GetStringValue(loader_cJSON_GetObjectItem(layer_node, "api_version"));
    if (NULL == api_version) {
        loader_log(
            inst, VULKAN_LOADER_WARN_BIT, 0,
            "Layer located at %s didn't find required layer value \"api_version\" in manifest JSON file, skipping this layer",
            filename);
        result = VK_ERROR_INITIALIZATION_FAILED;
        goto out;
    }

    props.info.specVersion = loader_parse_version_string(api_version);

    // Make sure the layer's manifest doesn't contain a non zero variant value
    if (VK_API_VERSION_VARIANT(props.info.specVersion) != 0) {
        loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                   "Layer \"%s\" has an \'api_version\' field which contains a non-zero variant value of %d. "
                   " Skipping Layer.",
                   props.info.layerName, VK_API_VERSION_VARIANT(props.info.specVersion));
        result = VK_ERROR_INITIALIZATION_FAILED;
        goto out;
    }

    // Parse implementation_version
    char* implementation_version = loader_cJSON_GetStringValue(loader_cJSON_GetObjectItem(layer_node, "implementation_version"));
    if (NULL == implementation_version) {
        loader_log(inst, VULKAN_LOADER_WARN_BIT, 0,
                   "Layer located at %s didn't find required layer value \"implementation_version\" in manifest JSON file, "
                   "skipping this layer",
                   filename);
        result = VK_ERROR_INITIALIZATION_FAILED;
        goto out;
    }
    props.info.implementationVersion = (uint32_t)strtoul(implementation_version, NULL, 10);

    // Parse description

    result = loader_parse_json_string_to_existing_str(layer_node, "description", VK_MAX_DESCRIPTION_SIZE, props.info.description);
    if (VK_ERROR_INITIALIZATION_FAILED == result) {
        loader_log(
            inst, VULKAN_LOADER_WARN_BIT, 0,
            "Layer located at %s didn't find required layer value \"description\" in manifest JSON file, skipping this layer",
            filename);
        goto out;
    }

    // Parse library_path

    // Library path no longer required unless component_layers is also not defined
    result = loader_parse_json_string(layer_node, "library_path", &library_path);
    if (result == VK_ERROR_OUT_OF_HOST_MEMORY) {
        loader_log(inst, VULKAN_LOADER_WARN_BIT, 0,
                   "Skipping layer \"%s\" due to problem accessing the library_path value in the manifest JSON file",
                   props.info.layerName);
        result = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }
    if (NULL == library_path) {
        if (NULL != loader_cJSON_GetObjectItem(layer_node, "component_layers")) {
            loader_log(inst, VULKAN_LOADER_WARN_BIT, 0,
                       "Layer \"%s\" contains meta-layer-specific component_layers. Meta-layer support has been removed from the "
                       "loader, so skipping this layer",
                       props.info.layerName);
        } else {
            loader_log(inst, VULKAN_LOADER_WARN_BIT, 0,
                       "Layer \"%s\" is missing the library_path field. It MUST be defined. Skipping this layer",
                       props.info.layerName);
        }
        result = VK_ERROR_INITIALIZATION_FAILED;
        goto out;
    }

    // This function takes ownership of library_path_str - so we don't need to clean it up
    result = combine_manifest_directory_and_library_path(inst, library_path, filename, &props.lib_name);
    if (result == VK_ERROR_OUT_OF_HOST_MEMORY) goto out;

    // Parse disable_environment

    if (is_implicit) {
        cJSON* disable_environment = loader_cJSON_GetObjectItem(layer_node, "disable_environment");
        if (disable_environment == NULL) {
            loader_log(inst, VULKAN_LOADER_WARN_BIT, 0,
                       "Layer \"%s\" doesn't contain required layer object disable_environment in the manifest JSON file, skipping "
                       "this layer",
                       props.info.layerName);
            result = VK_ERROR_INITIALIZATION_FAILED;
            goto out;
        }

        if (!disable_environment->child || disable_environment->child->type != cJSON_String ||
            !disable_environment->child->string || !disable_environment->child->valuestring) {
            loader_log(inst, VULKAN_LOADER_WARN_BIT, 0,
                       "Layer \"%s\" doesn't contain required child value in object disable_environment in the manifest JSON file, "
                       "skipping this layer (Policy #LLP_LAYER_9)",
                       props.info.layerName);
            result = VK_ERROR_INITIALIZATION_FAILED;
            goto out;
        }
        result = loader_copy_to_new_str(inst, disable_environment->child->string, &(props.disable_env_var.name));
        if (VK_SUCCESS != result) goto out;
        result = loader_copy_to_new_str(inst, disable_environment->child->valuestring, &(props.disable_env_var.value));
        if (VK_SUCCESS != result) goto out;
    }

    // Now get all optional items and objects and put in list:
    // functions
    // instance_extensions
    // device_extensions
    // enable_environment (implicit layers only)
    // library_arch

    // Layer interface functions
    //    vkGetInstanceProcAddr
    //    vkGetDeviceProcAddr
    //    vkNegotiateLoaderLayerInterfaceVersion (starting with JSON file 1.1.0)
    cJSON* functions = loader_cJSON_GetObjectItem(layer_node, "functions");
    if (functions != NULL) {
        if (loader_check_version_meets_required(loader_combine_version(1, 1, 0), version)) {
            result = loader_parse_json_string(functions, "vkNegotiateLoaderLayerInterfaceVersion",
                                              &props.functions.str_negotiate_interface);
            if (result == VK_ERROR_OUT_OF_HOST_MEMORY) goto out;
        }
        result = loader_parse_json_string(functions, "vkGetInstanceProcAddr", &props.functions.str_gipa);
        if (result == VK_ERROR_OUT_OF_HOST_MEMORY) goto out;

        if (NULL == props.functions.str_negotiate_interface && props.functions.str_gipa &&
            loader_check_version_meets_required(loader_combine_version(1, 1, 0), version)) {
            loader_log(inst, VULKAN_LOADER_INFO_BIT, 0,
                       "Layer \"%s\" using deprecated \'vkGetInstanceProcAddr\' tag which was deprecated starting with JSON "
                       "file version 1.1.0. The new vkNegotiateLoaderLayerInterfaceVersion function is preferred, though for "
                       "compatibility reasons it may be desirable to continue using the deprecated tag.",
                       props.info.layerName);
        }

        result = loader_parse_json_string(functions, "vkGetDeviceProcAddr", &props.functions.str_gdpa);
        if (result == VK_ERROR_OUT_OF_HOST_MEMORY) goto out;

        if (NULL == props.functions.str_negotiate_interface && props.functions.str_gdpa &&
            loader_check_version_meets_required(loader_combine_version(1, 1, 0), version)) {
            loader_log(inst, VULKAN_LOADER_INFO_BIT, 0,
                       "Layer \"%s\" using deprecated \'vkGetDeviceProcAddr\' tag which was deprecated starting with JSON "
                       "file version 1.1.0. The new vkNegotiateLoaderLayerInterfaceVersion function is preferred, though for "
                       "compatibility reasons it may be desirable to continue using the deprecated tag.",
                       props.info.layerName);
        }
    }

    // instance_extensions
    //   array of {
    //     name
    //     spec_version
    //   }

    cJSON* instance_extensions = loader_cJSON_GetObjectItem(layer_node, "instance_extensions");
    if (instance_extensions != NULL && instance_extensions->type == cJSON_Array) {
        cJSON* ext_item = NULL;
        cJSON_ArrayForEach(ext_item, instance_extensions) {
            if (ext_item->type != cJSON_Object) {
                continue;
            }

            VkExtensionProperties ext_prop = {0};
            result = loader_parse_json_string_to_existing_str(ext_item, "name", VK_MAX_EXTENSION_NAME_SIZE, ext_prop.extensionName);
            if (result == VK_ERROR_INITIALIZATION_FAILED) {
                continue;
            }
            char* spec_version = NULL;
            result = loader_parse_json_string(ext_item, "spec_version", &spec_version);
            if (result == VK_ERROR_OUT_OF_HOST_MEMORY) goto out;
            if (NULL != spec_version) {
                ext_prop.specVersion = (uint32_t)strtoul(spec_version, NULL, 10);
            }
            loader_instance_heap_free(inst, spec_version);
            bool ext_unsupported = wsi_unsupported_instance_extension(&ext_prop);
            if (!ext_unsupported) {
                result = loader_add_to_ext_list(inst, &props.instance_extension_list, 1, &ext_prop);
                if (result == VK_ERROR_OUT_OF_HOST_MEMORY) goto out;
            }
        }
    }

    // device_extensions
    //   array of {
    //     name
    //     spec_version
    //     entrypoints
    //   }
    cJSON* device_extensions = loader_cJSON_GetObjectItem(layer_node, "device_extensions");
    if (device_extensions != NULL && device_extensions->type == cJSON_Array) {
        cJSON* ext_item = NULL;
        cJSON_ArrayForEach(ext_item, device_extensions) {
            if (ext_item->type != cJSON_Object) {
                continue;
            }

            VkExtensionProperties ext_prop = {0};
            result = loader_parse_json_string_to_existing_str(ext_item, "name", VK_MAX_EXTENSION_NAME_SIZE, ext_prop.extensionName);
            if (result == VK_ERROR_INITIALIZATION_FAILED) {
                continue;
            }

            char* spec_version = NULL;
            result = loader_parse_json_string(ext_item, "spec_version", &spec_version);
            if (result == VK_ERROR_OUT_OF_HOST_MEMORY) goto out;
            if (NULL != spec_version) {
                ext_prop.specVersion = (uint32_t)strtoul(spec_version, NULL, 10);
            }
            loader_instance_heap_free(inst, spec_version);

            cJSON* entrypoints = loader_cJSON_GetObjectItem(ext_item, "entrypoints");
            if (entrypoints == NULL) {
                result = loader_add_to_dev_ext_list(inst, &props.device_extension_list, &ext_prop, NULL);
                if (result == VK_ERROR_OUT_OF_HOST_MEMORY) goto out;
                continue;
            }

            struct loader_string_list entrys = {0};
            result = loader_parse_json_array_of_strings(inst, ext_item, "entrypoints", &entrys);
            if (result == VK_ERROR_OUT_OF_HOST_MEMORY) goto out;
            result = loader_add_to_dev_ext_list(inst, &props.device_extension_list, &ext_prop, &entrys);
            if (result == VK_ERROR_OUT_OF_HOST_MEMORY) goto out;
        }
    }
    if (is_implicit) {
        cJSON* enable_environment = loader_cJSON_GetObjectItem(layer_node, "enable_environment");

        // enable_environment is optional
        if (enable_environment && enable_environment->child && enable_environment->child->type == cJSON_String &&
            enable_environment->child->string && enable_environment->child->valuestring) {
            result = loader_copy_to_new_str(inst, enable_environment->child->string, &(props.enable_env_var.name));
            if (VK_SUCCESS != result) goto out;
            result = loader_copy_to_new_str(inst, enable_environment->child->valuestring, &(props.enable_env_var.value));
            if (VK_SUCCESS != result) goto out;
        }
    }

    // Read in the pre-instance stuff
    cJSON* pre_instance = loader_cJSON_GetObjectItem(layer_node, "pre_instance_functions");
    if (NULL != pre_instance) {
        // Supported versions started in 1.1.2, so anything newer
        if (!loader_check_version_meets_required(loader_combine_version(1, 1, 2), version)) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "Found pre_instance_functions section in layer from \"%s\". This section is only valid in manifest version "
                       "1.1.2 or later. The section will be ignored",
                       filename);
        } else if (!is_implicit) {
            loader_log(inst, VULKAN_LOADER_WARN_BIT, 0,
                       "Found pre_instance_functions section in explicit layer from \"%s\". This section is only valid in implicit "
                       "layers. The section will be ignored",
                       filename);
        } else {
            result = loader_parse_json_string(pre_instance, "vkEnumerateInstanceExtensionProperties",
                                              &props.pre_instance_functions.enumerate_instance_extension_properties);
            if (result == VK_ERROR_OUT_OF_HOST_MEMORY) goto out;

            result = loader_parse_json_string(pre_instance, "vkEnumerateInstanceLayerProperties",
                                              &props.pre_instance_functions.enumerate_instance_layer_properties);
            if (result == VK_ERROR_OUT_OF_HOST_MEMORY) goto out;

            result = loader_parse_json_string(pre_instance, "vkEnumerateInstanceVersion",
                                              &props.pre_instance_functions.enumerate_instance_version);
            if (result == VK_ERROR_OUT_OF_HOST_MEMORY) goto out;
        }
    }

    char* library_arch = loader_cJSON_GetStringValue(loader_cJSON_GetObjectItem(layer_node, "library_arch"));
    if (NULL != library_arch) {
        if ((strncmp(library_arch, "32", 2) == 0 && sizeof(void*) != 4) ||
            (strncmp(library_arch, "64", 2) == 0 && sizeof(void*) != 8)) {
            loader_log(inst, VULKAN_LOADER_INFO_BIT, 0,
                       "The library architecture in layer %s doesn't match the current running architecture, skipping this layer",
                       filename);
            result = VK_ERROR_INITIALIZATION_FAILED;
            goto out;
        }
    }

    result = VK_SUCCESS;

out:
    // Try to append the layer property
    if (VK_SUCCESS == result) {
        result = loader_append_layer_property(inst, layer_instance_list, &props);
    }
    // If appending fails - free all the memory allocated in it
    if (VK_SUCCESS != result) {
        loader_free_layer_properties(inst, &props);
    }
    return result;
}

bool is_valid_layer_json_version(const loader_api_version* layer_json) {
    // Supported versions are: 1.0.0, 1.0.1, 1.1.0 - 1.1.2, and 1.2.0 - 1.2.1.
    if ((layer_json->major == 1 && layer_json->minor == 2 && layer_json->patch < 2) ||
        (layer_json->major == 1 && layer_json->minor == 1 && layer_json->patch < 3) ||
        (layer_json->major == 1 && layer_json->minor == 0 && layer_json->patch < 2)) {
        return true;
    }
    return false;
}

// Given a cJSON struct (json) of the top level JSON object from layer manifest
// file, add entry to the layer_list. Fill out the layer_properties in this list
// entry from the input cJSON object.
//
// \returns
// void
// layer_list has a new entry and initialized accordingly.
// If the json input object does not have all the required fields no entry
// is added to the list.
VkResult loader_add_layer_properties(const struct loader_instance* inst, struct loader_layer_list* layer_instance_list, cJSON* json,
                                     bool is_implicit, char* filename) {
    // The following Fields in layer manifest file that are required:
    //   - "file_format_version"
    //   - If more than one "layer" object are used, then the "layers" array is
    //     required
    VkResult result = VK_ERROR_INITIALIZATION_FAILED;
    // Make sure sure the top level json value is an object
    if (!json || json->type != cJSON_Object) {
        goto out;
    }
    char* file_vers = loader_cJSON_GetStringValue(loader_cJSON_GetObjectItem(json, "file_format_version"));
    if (NULL == file_vers) {
        loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                   "loader_add_layer_properties: Manifest %s missing required field file_format_version", filename);
        goto out;
    }

    loader_log(inst, VULKAN_LOADER_INFO_BIT, 0, "Found manifest file %s (file version %s)", filename, file_vers);
    // Get the major/minor/and patch as integers for easier comparison
    loader_api_version json_version = loader_make_full_version(loader_parse_version_string(file_vers));

    if (!is_valid_layer_json_version(&json_version)) {
        loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                   "loader_add_layer_properties: %s has unknown layer manifest file version %d.%d.%d.  May cause errors.", filename,
                   json_version.major, json_version.minor, json_version.patch);
    }

    // If "layers" is present, read in the array of layer objects
    cJSON* layers_node = loader_cJSON_GetObjectItem(json, "layers");
    if (layers_node != NULL) {
        // Supported versions started in 1.0.1, so anything newer
        if (!loader_check_version_meets_required(loader_combine_version(1, 0, 1), json_version)) {
            loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                       "loader_add_layer_properties: \'layers\' tag not supported until file version 1.0.1, but %s is reporting "
                       "version %s",
                       filename, file_vers);
        }
        cJSON* layer_node = NULL;
        cJSON_ArrayForEach(layer_node, layers_node) {
            if (layer_node->type != cJSON_Object) {
                loader_log(
                    inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                    "loader_add_layer_properties: Array element in \"layers\" field in manifest JSON file %s is not an object.  "
                    "Skipping this file",
                    filename);
                goto out;
            }
            result = loader_read_layer_json(inst, layer_instance_list, layer_node, json_version, is_implicit, filename);
        }
    } else {
        // Otherwise, try to read in individual layers
        cJSON* layer_node = loader_cJSON_GetObjectItem(json, "layer");
        if (layer_node == NULL) {
            // Don't warn if this happens to be an ICD manifest
            if (loader_cJSON_GetObjectItem(json, "ICD") == NULL) {
                loader_log(
                    inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                    "loader_add_layer_properties: Can not find 'layer' object in manifest JSON file %s.  Skipping this file.",
                    filename);
            }
            goto out;
        }
        // Loop through all "layer" objects in the file to get a count of them
        // first.
        uint16_t layer_count = 0;
        cJSON* tempNode = layer_node;
        do {
            tempNode = tempNode->next;
            layer_count++;
        } while (tempNode != NULL);

        // Throw a warning if we encounter multiple "layer" objects in file
        // versions newer than 1.0.0.  Having multiple objects with the same
        // name at the same level is actually a JSON standard violation.
        if (layer_count > 1 && loader_check_version_meets_required(loader_combine_version(1, 0, 1), json_version)) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                       "loader_add_layer_properties: Multiple 'layer' nodes are deprecated starting in file version \"1.0.1\".  "
                       "Please use 'layers' : [] array instead in %s.",
                       filename);
        } else {
            do {
                result = loader_read_layer_json(inst, layer_instance_list, layer_node, json_version, is_implicit, filename);
                if (result == VK_ERROR_OUT_OF_HOST_MEMORY) {
                    goto out;
                }
                layer_node = layer_node->next;
            } while (layer_node != NULL);
        }
    }

out:

    return result;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters) - no safer ordering for these two path string parameters
VkResult copy_data_file_info(const struct loader_instance* inst, const char* cur_path, const char* relative_path,
                             size_t relative_path_size, struct loader_string_list* search_paths) {
    if (NULL != cur_path) {
        uint32_t start = 0;
        uint32_t stop = 0;

        while (cur_path[start] != '\0') {
            while (cur_path[start] == PATH_SEPARATOR && cur_path[start] != '\0') {
                start++;
            }
            stop = start;
            while (cur_path[stop] != PATH_SEPARATOR && cur_path[stop] != '\0') {
                stop++;
            }
            const size_t s = stop - start;
            if (s) {
                // space enough for base path, relative path, path separator, and null terminator
                size_t new_str_len = s + relative_path_size + 2;

                char* new_str = loader_instance_heap_calloc(inst, new_str_len, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
                if (NULL == new_str) {
                    return VK_ERROR_OUT_OF_HOST_MEMORY;
                }
                memcpy(new_str, &cur_path[start], s);
                new_str[s] = '\0';

                // If this is a specific JSON file, just add it and don't add any
                // relative path or directory symbol to it.
                if (!is_json(new_str, s)) {
                    // Add the relative directory if present.
                    if (relative_path_size > 0) {
                        char* rel_start = new_str + s;

                        // If last symbol written was not a directory symbol, add it.
                        if (*(rel_start - 1) != DIRECTORY_SYMBOL) {
                            *rel_start++ = DIRECTORY_SYMBOL;
                        }
                        memcpy(rel_start, relative_path, relative_path_size);
                        rel_start[relative_path_size] = '\0';
                    }
                }

                VkResult res = append_str_to_string_list_if_unique(inst, search_paths, new_str);
                if (VK_SUCCESS != res) {
                    return res;
                }
                start = stop;
            }
        }
    }
    return VK_SUCCESS;
}

// If the file found is a manifest file name, add it to the end of out_files manifest list.
VkResult add_if_manifest_file(const struct loader_instance* inst, const char* file_name, struct loader_string_list* out_files) {
    assert(NULL != file_name && "add_if_manifest_file: Received NULL pointer for file_name");
    assert(NULL != out_files && "add_if_manifest_file: Received NULL pointer for out_files");

    // Look for files ending with ".json" suffix
    size_t name_len = strlen(file_name);
    if (!is_json(file_name, name_len)) {
        // Use incomplete to indicate invalid name, but to keep going.
        return VK_INCOMPLETE;
    }

    // A directory search path and a file inside it can reach the same manifest.
    return copy_str_to_string_list_if_unique(inst, out_files, file_name, name_len);
}

// If the file found is a manifest file name, add it to the start of the out_files manifest list.
VkResult prepend_if_manifest_file(const struct loader_instance* inst, const char* file_name, struct loader_string_list* out_files) {
    assert(NULL != file_name && "prepend_if_manifest_file: Received NULL pointer for file_name");
    assert(NULL != out_files && "prepend_if_manifest_file: Received NULL pointer for out_files");

    // Look for files ending with ".json" suffix
    size_t name_len = strlen(file_name);
    if (!is_json(file_name, name_len)) {
        // Use incomplete to indicate invalid name, but to keep going.
        return VK_INCOMPLETE;
    }

    // Move an existing entry to the front instead of loading it twice.
    remove_str_from_string_list(inst, out_files, file_name);
    return copy_str_to_start_of_string_list(inst, out_files, file_name, name_len);
}

// Iterate all strings in search_paths and add any files found.  If any path in the search path points to a specific JSON, attempt
// to only open that one JSON.  Otherwise, if the path is a folder, search the folder for JSON files.
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters) - no safer ordering for these two string-list parameters
VkResult add_data_files(const struct loader_instance* inst, struct loader_string_list* search_paths,
                        struct loader_string_list* out_files) {
    VkResult vk_result = VK_SUCCESS;
    char full_path[2048];
#if !defined(_WIN32)
    char temp_path[2048];
#endif

    for (size_t i = 0; i < search_paths->count; i++) {
        // Now, parse the paths
        char* next_file = search_paths->list[i];
        while (NULL != next_file && *next_file != '\0') {
            char* name = NULL;
            char* cur_file = next_file;
            next_file = loader_get_next_path(cur_file);

            // Is this a JSON file, then try to open it.
            size_t len = strlen(cur_file);
            if (is_json(cur_file, len)) {
#if defined(_WIN32)
                name = cur_file;
#elif COMMON_UNIX_PLATFORMS
                // Only Linux has relative paths, make a copy of location so it isn't modified
                size_t str_len;
                if (NULL != next_file) {
                    str_len = next_file - cur_file + 1;
                } else {
                    str_len = strlen(cur_file) + 1;
                }
                if (str_len > sizeof(temp_path)) {
                    loader_log(inst, VULKAN_LOADER_DEBUG_BIT, 0, "add_data_files: Path to %s too long", cur_file);
                    continue;
                }
                strncpy(temp_path, cur_file, str_len);
                name = temp_path;
#else
#warning add_data_files must define relative path copy for this platform
#endif
                vk_result = loader_get_fullpath(inst, cur_file, name, sizeof(full_path), full_path);
                if (VK_ERROR_OUT_OF_HOST_MEMORY == vk_result) {
                    break;
                }
                name = full_path;

                VkResult local_res;
                local_res = add_if_manifest_file(inst, name, out_files);

                // Incomplete means this was not a valid data file.
                if (local_res == VK_INCOMPLETE) {
                    continue;
                }
                if (local_res != VK_SUCCESS) {
                    vk_result = local_res;
                    break;
                }
            } else {  // Otherwise, treat it as a directory
                DIR* dir_stream = loader_opendir(inst, cur_file);
                if (NULL == dir_stream) {
                    continue;
                }
                while (1) {
                    errno = 0;
                    struct dirent* dir_entry = loader_readdir(inst, dir_stream);
#if !defined(WIN32)  // Windows doesn't use readdir, don't check errors on functions which aren't called
                    if (errno != 0) {
                        // NOLINTNEXTLINE(concurrency-mt-unsafe) - strerror's static buffer is used immediately and not retained
                        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "readdir failed with %d: %s", errno, strerror(errno));
                        break;
                    }
#endif
                    if (NULL == dir_entry) {
                        break;
                    }

                    name = &(dir_entry->d_name[0]);
                    // 'name' is the bare filename, 'cur_file' is the directory being scanned; the substring "file" in
                    // cur_file just confuses the naming heuristic below.
                    // NOLINTNEXTLINE(readability-suspicious-call-argument) - arguments are correctly ordered, not swapped
                    vk_result = loader_get_fullpath(inst, name, cur_file, sizeof(full_path), full_path);
                    if (VK_ERROR_OUT_OF_HOST_MEMORY == vk_result) {
                        break;
                    }
                    // loader_get_fullpath falls back to the bare filename when the entry can't be found in cur_file (eg a dangling
                    // symlink), which would then be opened relative to the current working directory. Skip the entry instead.
                    if (!loader_platform_is_path(full_path)) {
                        loader_log(inst, VULKAN_LOADER_DEBUG_BIT, 0, "add_data_files: Unable to find %s in %s, skipping it", name,
                                   cur_file);
                        continue;
                    }
                    name = full_path;

                    VkResult local_res;
                    local_res = add_if_manifest_file(inst, name, out_files);

                    // Incomplete means this was not a valid data file.
                    if (local_res == VK_INCOMPLETE) {
                        continue;
                    }
                    if (local_res != VK_SUCCESS) {
                        vk_result = local_res;
                        break;
                    }
                }
                loader_closedir(inst, dir_stream);
                if (vk_result != VK_SUCCESS) {
                    goto out;
                }
            }
        }
        // Double nested loop, so need to propagate break from inner loop to outer loop
        if (VK_ERROR_OUT_OF_HOST_MEMORY == vk_result) {
            break;
        }
    }

out:

    return vk_result;
}

// Look for data files in the provided paths, but first check the environment override to determine if we should use that
// instead.
VkResult read_data_files_in_search_paths(const struct loader_instance* inst, enum loader_data_files_type manifest_type,
                                         const struct loader_string_list* path_overrides, bool* override_active,
                                         struct loader_string_list* out_files) {
    VkResult vk_result = VK_SUCCESS;
    char* override_env = NULL;
    char* additional_env = NULL;
    struct loader_string_list search_paths = {0};

#if COMMON_UNIX_PLATFORMS
    const char* relative_location = NULL;  // Only used on unix platforms
    size_t rel_size = 0;                   // unused in windows, dont declare so no compiler warnings are generated
#endif

#if defined(__APPLE__)
    char* bundle_path = NULL;
    char* search_only_in_bundle_env_var = loader_secure_getenv(VK_LOADER_SEARCH_ONLY_IN_BUNDLE_ENV_VAR, inst);
#endif

#if defined(_WIN32)
    char* package_path = NULL;
#elif COMMON_UNIX_PLATFORMS
    // Determine how much space is needed to generate the full search path
    // for the current manifest files.
    char* xdg_config_home = loader_secure_getenv("XDG_CONFIG_HOME", inst);
    char* xdg_config_dirs = loader_secure_getenv("XDG_CONFIG_DIRS", inst);

#if !defined(__Fuchsia__) && !defined(__QNX__)
    if (NULL == xdg_config_dirs || '\0' == xdg_config_dirs[0]) {
        xdg_config_dirs = FALLBACK_CONFIG_DIRS;
    }
#endif

    char* xdg_data_home = loader_secure_getenv("XDG_DATA_HOME", inst);
    char* xdg_data_dirs = loader_secure_getenv("XDG_DATA_DIRS", inst);

#if !defined(__Fuchsia__) && !defined(__QNX__)
    if (NULL == xdg_data_dirs || '\0' == xdg_data_dirs[0]) {
        xdg_data_dirs = FALLBACK_DATA_DIRS;
    }
#endif

    char* home = NULL;
    char* default_data_home = NULL;
    char* default_config_home = NULL;
    char* home_data_dir = NULL;
    char* home_config_dir = NULL;

    // Only use HOME if XDG_DATA_HOME is not present on the system
    home = loader_secure_getenv("HOME", inst);
    if (home != NULL) {
        if (NULL == xdg_config_home || '\0' == xdg_config_home[0]) {
            const char config_suffix[] = "/.config";
            size_t default_config_home_len = strlen(home) + sizeof(config_suffix) + 1;
            default_config_home = loader_instance_heap_calloc(inst, default_config_home_len, VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
            if (default_config_home == NULL) {
                vk_result = VK_ERROR_OUT_OF_HOST_MEMORY;
                goto out;
            }
            strncpy(default_config_home, home, default_config_home_len);
            strncat(default_config_home, config_suffix, default_config_home_len);
        }
        if (NULL == xdg_data_home || '\0' == xdg_data_home[0]) {
            const char data_suffix[] = "/.local/share";
            size_t default_data_home_len = strlen(home) + sizeof(data_suffix) + 1;
            default_data_home = loader_instance_heap_calloc(inst, default_data_home_len, VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
            if (default_data_home == NULL) {
                vk_result = VK_ERROR_OUT_OF_HOST_MEMORY;
                goto out;
            }
            strncpy(default_data_home, home, default_data_home_len);
            strncat(default_data_home, data_suffix, default_data_home_len);
        }
    }

    if (NULL != default_config_home) {
        home_config_dir = default_config_home;
    } else {
        home_config_dir = xdg_config_home;
    }
    if (NULL != default_data_home) {
        home_data_dir = default_data_home;
    } else {
        home_data_dir = xdg_data_home;
    }
#else
#warning read_data_files_in_search_paths unsupported platform
#endif

    switch (manifest_type) {
        case LOADER_DATA_FILE_MANIFEST_DRIVER:
            if (loader_settings_should_use_driver_environment_variables(inst)) {
                override_env = loader_secure_getenv(VK_DRIVER_FILES_ENV_VAR, inst);
                if (NULL == override_env) {
                    // Not there, so fall back to the old name
                    override_env = loader_secure_getenv(VK_ICD_FILENAMES_ENV_VAR, inst);
                }
                additional_env = loader_secure_getenv(VK_ADDITIONAL_DRIVER_FILES_ENV_VAR, inst);
            }
#if COMMON_UNIX_PLATFORMS
            relative_location = VK_DRIVERS_INFO_RELATIVE_DIR;
#endif
#if defined(_WIN32)
            {
                bool oom = false;
                package_path = windows_get_app_package_manifest_path(inst, &oom);
                if (oom) {
                    vk_result = VK_ERROR_OUT_OF_HOST_MEMORY;
                    goto out;
                }
            }
#endif
            break;
        case LOADER_DATA_FILE_MANIFEST_IMPLICIT_LAYER:
            override_env = loader_secure_getenv(VK_IMPLICIT_LAYER_PATH_ENV_VAR, inst);
            additional_env = loader_secure_getenv(VK_ADDITIONAL_IMPLICIT_LAYER_PATH_ENV_VAR, inst);
#if COMMON_UNIX_PLATFORMS
            relative_location = VK_ILAYERS_INFO_RELATIVE_DIR;
#endif
#if defined(_WIN32)
            {
                bool oom = false;
                package_path = windows_get_app_package_manifest_path(inst, &oom);
                if (oom) {
                    vk_result = VK_ERROR_OUT_OF_HOST_MEMORY;
                    goto out;
                }
            }
#endif
            break;
        case LOADER_DATA_FILE_MANIFEST_EXPLICIT_LAYER:
            override_env = loader_secure_getenv(VK_EXPLICIT_LAYER_PATH_ENV_VAR, inst);
            additional_env = loader_secure_getenv(VK_ADDITIONAL_EXPLICIT_LAYER_PATH_ENV_VAR, inst);
#if COMMON_UNIX_PLATFORMS
            relative_location = VK_ELAYERS_INFO_RELATIVE_DIR;
#endif
            break;
        default:
            assert(false && "Shouldn't get here!");
            break;
    }

    // Log a message when VK_LAYER_PATH is set but the override layer paths take priority
    if (manifest_type == LOADER_DATA_FILE_MANIFEST_EXPLICIT_LAYER &&
        (NULL != override_env && NULL != path_overrides && path_overrides->count > 0)) {
        loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                   "Ignoring VK_LAYER_PATH. The Override layer is active and has override paths set, which takes priority. "
                   "VK_LAYER_PATH is set to %s",
                   override_env);
    }

    // Add the remaining paths to the list
    if (NULL != path_overrides && path_overrides->count > 0) {
        for (size_t i = 0; i < path_overrides->count; i++) {
            if (NULL != path_overrides->list[i]) {
                if (manifest_type == LOADER_DATA_FILE_MANIFEST_EXPLICIT_LAYER) {
                    loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_LAYER_BIT, 0, "%s", path_overrides->list[i]);
                }
                vk_result = copy_data_file_info(inst, path_overrides->list[i], NULL, 0, &search_paths);
                if (vk_result == VK_ERROR_OUT_OF_HOST_MEMORY) {
                    goto out;
                }
            }
        }
    } else if (override_env != NULL) {
        vk_result = copy_data_file_info(inst, override_env, NULL, 0, &search_paths);
        if (vk_result == VK_ERROR_OUT_OF_HOST_MEMORY) {
            goto out;
        }
    } else {
        // Add any additional search paths defined in the additive environment variable
        if (NULL != additional_env) {
            vk_result = copy_data_file_info(inst, additional_env, NULL, 0, &search_paths);
            if (vk_result == VK_ERROR_OUT_OF_HOST_MEMORY) {
                goto out;
            }
        }

#if defined(_WIN32)
        if (NULL != package_path) {
            vk_result = copy_data_file_info(inst, package_path, NULL, 0, &search_paths);
            if (vk_result == VK_ERROR_OUT_OF_HOST_MEMORY) {
                goto out;
            }
        }
#elif COMMON_UNIX_PLATFORMS
        bool search_outside_of_bundle = true;

        rel_size = strlen(relative_location);
        if (rel_size > 0) {
#if defined(__APPLE__)
            // Add the bundle's Resources dir to the beginning of the search path.
            // Looks for manifests in the bundle first, before any system directories.
            // This also appears to work unmodified for iOS, it finds the app bundle on the devices
            // file system. (RSW)
            CFBundleRef main_bundle = CFBundleGetMainBundle();
            if (NULL != main_bundle) {
                CFURLRef ref = CFBundleCopyResourcesDirectoryURL(main_bundle);
                if (NULL != ref) {
                    // Allocate large scratch buffer to hold the bundles path, the relative location inside the bundle, plus a path
                    // separator and null terminator. Since we are using a while loop, initialize length with half the intended size
                    size_t bundle_path_len = (MAXPATHLEN + rel_size + 2) / 2;
                    bool bundle_query_success = false;
                    uint32_t retry_count = 0;
                    while (!bundle_query_success && retry_count < 4) {
                        void* new_bundle_path = loader_instance_heap_realloc(
                            inst, bundle_path, bundle_path_len, bundle_path_len * 2, VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
                        if (NULL == new_bundle_path) {
                            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                                       "read_data_files_in_search_paths: Failed to allocate space for bundle path of length %d",
                                       (uint32_t)bundle_path_len * 2);
                            vk_result = VK_ERROR_OUT_OF_HOST_MEMORY;
                            goto out;
                        }
                        bundle_path = new_bundle_path;
                        bundle_path_len = bundle_path_len * 2;
                        memset(bundle_path, 0, bundle_path_len);
                        bundle_query_success = CFURLGetFileSystemRepresentation(ref, TRUE, (UInt8*)bundle_path, bundle_path_len);
                        retry_count++;
                    }

                    if (bundle_query_success) {
                        size_t cur_bundle_len = strlen(bundle_path);

                        if (cur_bundle_len + rel_size + 2 > bundle_path_len) {
                            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                                       "read_data_files_in_search_paths: bundle_path %s of size %d does not have enough space to "
                                       "append relative_path",
                                       bundle_path, (uint32_t)bundle_path_len);
                            vk_result = VK_ERROR_INITIALIZATION_FAILED;
                            goto out;
                        }
                        bundle_path[cur_bundle_len] = DIRECTORY_SYMBOL;
                        cur_bundle_len++;
                        memcpy(bundle_path + cur_bundle_len, relative_location, rel_size);
                        cur_bundle_len += rel_size;
                        bundle_path[cur_bundle_len] = '\0';

                        vk_result = copy_data_file_info(inst, bundle_path, NULL, 0, &search_paths);
                        if (vk_result == VK_ERROR_OUT_OF_HOST_MEMORY) {
                            goto out;
                        }
                    }
                    CFRelease(ref);
                }
            }

            if (NULL != search_only_in_bundle_env_var) {
                search_outside_of_bundle = false;
            }
#endif  // __APPLE__
            if (search_outside_of_bundle) {
                // Only add the home folders if not NULL
                if (NULL != home_config_dir) {
                    vk_result = copy_data_file_info(inst, home_config_dir, relative_location, rel_size, &search_paths);
                    if (VK_SUCCESS != vk_result) {
                        goto out;
                    }
                }
                vk_result = copy_data_file_info(inst, xdg_config_dirs, relative_location, rel_size, &search_paths);
                if (VK_SUCCESS != vk_result) {
                    goto out;
                }
                vk_result = copy_data_file_info(inst, SYSCONFDIR, relative_location, rel_size, &search_paths);
                if (VK_SUCCESS != vk_result) {
                    goto out;
                }
#if defined(EXTRASYSCONFDIR)
                vk_result = copy_data_file_info(inst, EXTRASYSCONFDIR, relative_location, rel_size, &search_paths);
                if (VK_SUCCESS != vk_result) {
                    goto out;
                }
#endif

                // Only add the home folders if not NULL
                if (NULL != home_data_dir) {
                    vk_result = copy_data_file_info(inst, home_data_dir, relative_location, rel_size, &search_paths);
                    if (VK_SUCCESS != vk_result) {
                        goto out;
                    }
                }
                vk_result = copy_data_file_info(inst, xdg_data_dirs, relative_location, rel_size, &search_paths);
                if (VK_SUCCESS != vk_result) {
                    goto out;
                }
            }
        }
#else
#warning read_data_files_in_search_paths unsupported platform
#endif
    }

    // Print out the paths being searched if debugging is enabled
    uint32_t log_flags = 0;

    if (manifest_type == LOADER_DATA_FILE_MANIFEST_DRIVER) {
        log_flags = VULKAN_LOADER_DRIVER_BIT;
        loader_log(inst, VULKAN_LOADER_DRIVER_BIT, 0, "Searching for driver manifest files");
    } else {
        log_flags = VULKAN_LOADER_LAYER_BIT;
        loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "Searching for %s layer manifest files",
                   manifest_type == LOADER_DATA_FILE_MANIFEST_EXPLICIT_LAYER ? "explicit" : "implicit");
    }
    loader_log(inst, log_flags, 0, "   In following locations:");

    for (size_t i = 0; i < search_paths.count; i++) {
        loader_log(inst, log_flags, 0, "      %s", search_paths.list[i]);
    }

    // Now, parse the paths and add any manifest files found in them.
    vk_result = add_data_files(inst, &search_paths, out_files);

    if (log_flags != 0 && out_files->count > 0) {
        loader_log(inst, log_flags, 0, "   Found the following files:");
        for (uint32_t cur_file = 0; cur_file < out_files->count; ++cur_file) {
            loader_log(inst, log_flags, 0, "      %s", out_files->list[cur_file]);
        }
    } else {
        loader_log(inst, log_flags, 0, "   Found no files");
    }

    if ((NULL != path_overrides && path_overrides->count > 0) || NULL != override_env) {
        *override_active = true;
    } else {
        *override_active = false;
    }

out:

    loader_free_getenv(additional_env, inst);
    loader_free_getenv(override_env, inst);
#if defined(_WIN32)
    loader_instance_heap_free(inst, package_path);
#elif COMMON_UNIX_PLATFORMS
#if defined(__APPLE__)
    loader_instance_heap_free(inst, bundle_path);
    loader_free_getenv(search_only_in_bundle_env_var, inst);
#endif
    loader_free_getenv(xdg_config_home, inst);
    loader_free_getenv(xdg_config_dirs, inst);
    loader_free_getenv(xdg_data_home, inst);
    loader_free_getenv(xdg_data_dirs, inst);
    loader_free_getenv(xdg_data_home, inst);
    loader_free_getenv(home, inst);
    loader_instance_heap_free(inst, default_data_home);
    loader_instance_heap_free(inst, default_config_home);
#else
#warning read_data_files_in_search_paths unsupported platform
#endif

    free_string_list(inst, &search_paths);

    return vk_result;
}

// Find the Vulkan library manifest files.
//
// This function scans the appropriate locations for a list of JSON manifest files based on the
// "manifest_type".  The location is interpreted as Registry path on Windows and a directory path(s)
// on Linux.
// "home_location" is an additional directory in the users home directory to look at. It is
// expanded into the dir path $XDG_DATA_HOME/home_location or $HOME/.local/share/home_location
// depending on environment variables. This "home_location" is only used on Linux.
//
// \returns
// VKResult
// A string list of manifest files to be opened in out_files param.
// List has a pointer to string for each manifest filename.
// When done using the list in out_files, pointers should be freed.
// Location or override  string lists can be either files or directories as
// follows:
//            | location | override
// --------------------------------
// Win ICD    | files    | files
// Win Layer  | files    | dirs
// Linux ICD  | dirs     | files
// Linux Layer| dirs     | dirs

VkResult loader_get_data_files(const struct loader_instance* inst, enum loader_data_files_type manifest_type,
                               const struct loader_string_list* path_overrides, struct loader_string_list* out_files) {
    VkResult res = VK_SUCCESS;
    bool override_active = false;

    // Free and init the out_files information so there's no false data left from uninitialized variables.
    free_string_list(inst, out_files);

    res = read_data_files_in_search_paths(inst, manifest_type, path_overrides, &override_active, out_files);
    if (VK_SUCCESS != res) {
        goto out;
    }

#if defined(_WIN32)
    // Read the registry if the override wasn't active.
    if (!override_active) {
        bool warn_if_not_present = false;
        wchar_t* registry_location = NULL;

        switch (manifest_type) {
            default:
                goto out;
            case LOADER_DATA_FILE_MANIFEST_DRIVER:
                warn_if_not_present = true;
                registry_location = VK_DRIVERS_INFO_REGISTRY_LOC;
                break;
            case LOADER_DATA_FILE_MANIFEST_IMPLICIT_LAYER:
                registry_location = VK_ILAYERS_INFO_REGISTRY_LOC;
                break;
            case LOADER_DATA_FILE_MANIFEST_EXPLICIT_LAYER:
                warn_if_not_present = true;
                registry_location = VK_ELAYERS_INFO_REGISTRY_LOC;
                break;
        }
        VkResult tmp_res =
            windows_read_data_files_in_registry(inst, manifest_type, warn_if_not_present, registry_location, out_files);
        // Only return an error if there was an error this time, and no manifest files from before.
        if (VK_SUCCESS != tmp_res && out_files->count == 0) {
            res = tmp_res;
            goto out;
        }
    }
#endif

out:

    if (VK_SUCCESS != res) {
        free_string_list(inst, out_files);
    }

    return res;
}

struct ICDManifestInfo {
    char* full_library_path;
    uint32_t version;
};

// Takes a json file, opens, reads, and parses an ICD Manifest out of it.
// Should only return VK_SUCCESS, VK_ERROR_INCOMPATIBLE_DRIVER, or VK_ERROR_OUT_OF_HOST_MEMORY
VkResult loader_parse_icd_manifest(const struct loader_instance* inst, char* file_str, struct ICDManifestInfo* icd,
                                   bool* skipped_portability_drivers) {
    VkResult res = VK_SUCCESS;
    cJSON* icd_manifest_json = NULL;

    if (file_str == NULL) {
        goto out;
    }

    res = loader_get_json(inst, file_str, &icd_manifest_json);
    if (res == VK_ERROR_OUT_OF_HOST_MEMORY) {
        goto out;
    }
    if (res != VK_SUCCESS || NULL == icd_manifest_json) {
        res = VK_ERROR_INCOMPATIBLE_DRIVER;
        goto out;
    }

    cJSON* file_format_version_json = loader_cJSON_GetObjectItem(icd_manifest_json, "file_format_version");
    if (file_format_version_json == NULL) {
        loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "loader_parse_icd_manifest: ICD JSON %s does not have a \'file_format_version\' field. Skipping ICD JSON.",
                   file_str);
        res = VK_ERROR_INCOMPATIBLE_DRIVER;
        goto out;
    }

    char* file_vers_str = loader_cJSON_GetStringValue(file_format_version_json);
    if (NULL == file_vers_str) {
        // The field is present but isn't a string
        loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "loader_parse_icd_manifest: Failed retrieving ICD JSON %s \'file_format_version\' field. Skipping ICD JSON",
                   file_str);
        res = VK_ERROR_INCOMPATIBLE_DRIVER;
        goto out;
    }
    loader_log(inst, VULKAN_LOADER_DRIVER_BIT, 0, "Found ICD manifest file %s, version %s", file_str, file_vers_str);

    // Get the version of the driver manifest
    loader_api_version json_file_version = loader_make_full_version(loader_parse_version_string(file_vers_str));

    // Loader only knows versions 1.0.0 and 1.0.1, anything above it is unknown
    if (loader_check_version_meets_required(loader_combine_version(1, 0, 2), json_file_version)) {
        loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "loader_parse_icd_manifest: %s has unknown icd manifest file version %d.%d.%d. May cause errors.", file_str,
                   json_file_version.major, json_file_version.minor, json_file_version.patch);
    }

    cJSON* itemICD = loader_cJSON_GetObjectItem(icd_manifest_json, "ICD");
    if (itemICD == NULL) {
        // Don't warn if this happens to be a layer manifest file
        if (loader_cJSON_GetObjectItem(icd_manifest_json, "layer") == NULL &&
            loader_cJSON_GetObjectItem(icd_manifest_json, "layers") == NULL) {
            loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                       "loader_parse_icd_manifest: Can not find \'ICD\' object in ICD JSON file %s. Skipping ICD JSON", file_str);
        }
        res = VK_ERROR_INCOMPATIBLE_DRIVER;
        goto out;
    }

    cJSON* library_path_json = loader_cJSON_GetObjectItem(itemICD, "library_path");
    if (library_path_json == NULL) {
        loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "loader_parse_icd_manifest: Failed to find \'library_path\' object in ICD JSON file %s. Skipping ICD JSON.",
                   file_str);
        res = VK_ERROR_INCOMPATIBLE_DRIVER;
        goto out;
    }
    bool out_of_memory = false;
    char* library_path = loader_cJSON_Print(library_path_json, &out_of_memory);
    if (out_of_memory) {
        loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "loader_parse_icd_manifest: Failed retrieving ICD JSON %s \'library_path\' field. Skipping ICD JSON.", file_str);
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    } else if (!library_path || strlen(library_path) == 0) {
        loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "loader_parse_icd_manifest: ICD JSON %s \'library_path\' field is empty. Skipping ICD JSON.", file_str);
        res = VK_ERROR_INCOMPATIBLE_DRIVER;
        loader_instance_heap_free(inst, library_path);
        goto out;
    }

    // Print out the paths being searched if debugging is enabled
    loader_log(inst, VULKAN_LOADER_DEBUG_BIT | VULKAN_LOADER_DRIVER_BIT, 0, "Searching for ICD drivers named %s", library_path);
    // This function takes ownership of library_path - so we don't need to clean it up
    res = combine_manifest_directory_and_library_path(inst, library_path, file_str, &icd->full_library_path);
    if (VK_SUCCESS != res) {
        goto out;
    }

    cJSON* api_version_json = loader_cJSON_GetObjectItem(itemICD, "api_version");
    if (api_version_json == NULL) {
        loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "loader_parse_icd_manifest: ICD JSON %s does not have an \'api_version\' field. Skipping ICD JSON.", file_str);
        res = VK_ERROR_INCOMPATIBLE_DRIVER;
        goto out;
    }
    char* version_str = loader_cJSON_GetStringValue(api_version_json);
    if (NULL == version_str) {
        // The field is present but isn't a string
        loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "loader_parse_icd_manifest: Failed retrieving ICD JSON %s \'api_version\' field. Skipping ICD JSON.", file_str);
        res = VK_ERROR_INCOMPATIBLE_DRIVER;
        goto out;
    }
    icd->version = loader_parse_version_string(version_str);

    if (VK_API_VERSION_VARIANT(icd->version) != 0) {
        loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "loader_parse_icd_manifest: Driver's ICD JSON %s \'api_version\' field contains a non-zero variant value of %d. "
                   " Skipping ICD JSON.",
                   file_str, VK_API_VERSION_VARIANT(icd->version));
        res = VK_ERROR_INCOMPATIBLE_DRIVER;
        goto out;
    }

    // Skip over ICD's which contain a true "is_portability_driver" value whenever the application doesn't enable
    // portability enumeration.
    cJSON* is_portability_driver_json = loader_cJSON_GetObjectItem(itemICD, "is_portability_driver");
    if (loader_cJSON_IsTrue(is_portability_driver_json) && inst && !inst->portability_enumeration_enabled) {
        if (skipped_portability_drivers) {
            *skipped_portability_drivers = true;
        }
        res = VK_ERROR_INCOMPATIBLE_DRIVER;
        goto out;
    }

    char* library_arch_str = loader_cJSON_GetStringValue(loader_cJSON_GetObjectItem(itemICD, "library_arch"));
    if (library_arch_str != NULL) {
        if ((strncmp(library_arch_str, "32", 2) == 0 && sizeof(void*) != 4) ||
            (strncmp(library_arch_str, "64", 2) == 0 && sizeof(void*) != 8)) {
            loader_log(inst, VULKAN_LOADER_INFO_BIT, 0,
                       "loader_parse_icd_manifest: Driver library architecture doesn't match the current running "
                       "architecture, skipping this driver");
            res = VK_ERROR_INCOMPATIBLE_DRIVER;
            goto out;
        }
    }
out:
    loader_cJSON_Delete(icd_manifest_json);
    return res;
}

// Try to find the Vulkan ICD driver(s).
//
// This function scans the default system loader path(s) or path specified by either the
// VK_DRIVER_FILES or VK_ICD_FILENAMES environment variable in order to find loadable
// VK ICDs manifest files.
// From these manifest files it finds the ICD libraries.
//
// skipped_portability_drivers is used to report whether the loader found drivers which report
// portability but the application didn't enable the bit to enumerate them
// Can be NULL
//
// \returns
// Vulkan result
// (on result == VK_SUCCESS) a list of icds that were discovered
VkResult loader_icd_scan(const struct loader_instance* inst, struct loader_icd_tramp_list* icd_tramp_list,
                         const VkInstanceCreateInfo* pCreateInfo, bool* skipped_portability_drivers) {
    VkResult res = VK_SUCCESS;
    struct loader_string_list manifest_files = {0};
    struct loader_envvar_filter select_filter = {0};
    struct loader_envvar_filter disable_filter = {0};
    struct ICDManifestInfo* icd_details = NULL;

    // Set up the ICD Trampoline list so elements can be written into it.
    res = loader_init_scanned_icd_list(inst, icd_tramp_list);
    if (res == VK_ERROR_OUT_OF_HOST_MEMORY) {
        return res;
    }

    bool direct_driver_loading_exclusive_mode = false;
    res = loader_scan_for_direct_drivers(inst, pCreateInfo, icd_tramp_list, &direct_driver_loading_exclusive_mode);
    if (res == VK_ERROR_OUT_OF_HOST_MEMORY) {
        goto out;
    }
    if (direct_driver_loading_exclusive_mode) {
        // Make sure to jump over the system & env-var driver discovery mechanisms if exclusive mode is set, even if no drivers
        // were successfully found through the direct driver loading mechanism
        goto out;
    }

    if (loader_settings_should_use_driver_environment_variables(inst)) {
        // Parse the filter environment variables to determine if we have any special behavior
        res = parse_generic_filter_environment_var(inst, VK_DRIVERS_SELECT_ENV_VAR, &select_filter);
        if (VK_SUCCESS != res) {
            goto out;
        }
        res = parse_generic_filter_environment_var(inst, VK_DRIVERS_DISABLE_ENV_VAR, &disable_filter);
        if (VK_SUCCESS != res) {
            goto out;
        }
    }

    // Get a list of manifest files for ICDs
    res = loader_get_data_files(inst, LOADER_DATA_FILE_MANIFEST_DRIVER, NULL, &manifest_files);
    if (VK_SUCCESS != res) {
        goto out;
    }

    // Add any drivers provided by the loader settings file
    res = loader_settings_get_additional_driver_files(inst, &manifest_files);
    if (VK_SUCCESS != res) {
        goto out;
    }

    if (manifest_files.count > 0) {
        icd_details = loader_instance_heap_calloc(inst, sizeof(struct ICDManifestInfo) * manifest_files.count,
                                                  VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (NULL == icd_details) {
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }
    }

    for (uint32_t i = 0; i < manifest_files.count; i++) {
        VkResult icd_res = VK_SUCCESS;

        icd_res = loader_parse_icd_manifest(inst, manifest_files.list[i], &icd_details[i], skipped_portability_drivers);
        if (VK_ERROR_OUT_OF_HOST_MEMORY == icd_res) {
            res = icd_res;
            goto out;
        } else if (VK_ERROR_INCOMPATIBLE_DRIVER == icd_res) {
            continue;
        }

        if (select_filter.count > 0 || disable_filter.count > 0) {
            // Get only the filename for comparing to the filters
            char* just_filename_str = strrchr(manifest_files.list[i], DIRECTORY_SYMBOL);

            // No directory symbol, just the filename
            if (NULL == just_filename_str) {
                just_filename_str = manifest_files.list[i];
            } else {
                just_filename_str++;
            }

            bool name_matches_select =
                (select_filter.count > 0 && check_name_matches_filter_environment_var(just_filename_str, &select_filter));
            bool name_matches_disable =
                (disable_filter.count > 0 && check_name_matches_filter_environment_var(just_filename_str, &disable_filter));

            if (name_matches_disable && !name_matches_select) {
                loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                           "Driver \"%s\" ignored because it was disabled by env var \'%s\'", just_filename_str,
                           VK_DRIVERS_DISABLE_ENV_VAR);
                continue;
            }
            if (select_filter.count != 0 && !name_matches_select) {
                loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                           "Driver \"%s\" ignored because not selected by env var \'%s\'", just_filename_str,
                           VK_DRIVERS_SELECT_ENV_VAR);
                continue;
            }
        }

        enum loader_layer_library_status lib_status;
        icd_res =
            loader_scanned_icd_add(inst, icd_tramp_list, icd_details[i].full_library_path, icd_details[i].version, &lib_status);
        if (VK_ERROR_OUT_OF_HOST_MEMORY == icd_res) {
            res = icd_res;
            goto out;
        } else if (VK_ERROR_INCOMPATIBLE_DRIVER == icd_res) {
            switch (lib_status) {
                case LOADER_LAYER_LIB_NOT_LOADED:
                case LOADER_LAYER_LIB_ERROR_FAILED_TO_LOAD:
                case LOADER_LAYER_LIB_ERROR_NEGOTIATE_INTERFACE_VERSION_FAILED:
                case LOADER_LAYER_LIB_ERROR_UNABLE_TO_FIND_VK_GET_INSTANCE_PROC_ADDR:
                    loader_log(inst, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                               "loader_icd_scan: Failed loading library associated with ICD JSON %s. Ignoring this JSON",
                               icd_details[i].full_library_path);
                    break;
                case LOADER_LAYER_LIB_ERROR_WRONG_BIT_TYPE:
                    loader_log(inst, VULKAN_LOADER_DRIVER_BIT, 0, "Requested ICD %s was wrong bit-type. Ignoring this JSON",
                               icd_details[i].full_library_path);
                    break;
                case LOADER_LAYER_LIB_SUCCESS_LOADED:
                case LOADER_LAYER_LIB_ERROR_OUT_OF_MEMORY:
                    // Shouldn't be able to reach this but if it is, best to report a debug
                    loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                               "Shouldn't reach this. A valid version of requested ICD %s was loaded but something bad "
                               "happened afterwards.",
                               icd_details[i].full_library_path);
                    break;
            }
        }
    }

out:
    if (NULL != icd_details) {
        // Successfully got the icd_details structure, which means we need to free the paths contained within
        for (uint32_t i = 0; i < manifest_files.count; i++) {
            loader_instance_heap_free(inst, icd_details[i].full_library_path);
        }
        loader_instance_heap_free(inst, icd_details);
    }
    free_string_list(inst, &manifest_files);
    return res;
}

// Gets the layer data files corresponding to manifest_type & path_override, then parses the resulting json objects
// into instance_layers
// Manifest type must be either implicit or explicit
VkResult loader_parse_instance_layers(struct loader_instance* inst, enum loader_data_files_type manifest_type,
                                      const struct loader_string_list* path_overrides, struct loader_layer_list* instance_layers) {
    assert(manifest_type == LOADER_DATA_FILE_MANIFEST_IMPLICIT_LAYER || manifest_type == LOADER_DATA_FILE_MANIFEST_EXPLICIT_LAYER);
    VkResult res = VK_SUCCESS;
    struct loader_string_list manifest_files = {0};

    res = loader_get_data_files(inst, manifest_type, path_overrides, &manifest_files);
    if (VK_SUCCESS != res) {
        goto out;
    }

    for (uint32_t i = 0; i < manifest_files.count; i++) {
        char* file_str = manifest_files.list[i];
        if (file_str == NULL) {
            continue;
        }

        // Parse file into JSON struct
        cJSON* json = NULL;
        VkResult local_res = loader_get_json(inst, file_str, &json);
        if (VK_ERROR_OUT_OF_HOST_MEMORY == local_res) {
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        } else if (VK_SUCCESS != local_res || NULL == json) {
            continue;
        }

        local_res = loader_add_layer_properties(inst, instance_layers, json,
                                                manifest_type == LOADER_DATA_FILE_MANIFEST_IMPLICIT_LAYER, file_str);
        loader_cJSON_Delete(json);

        // If the error is anything other than out of memory we still want to try to load the other layers
        if (VK_ERROR_OUT_OF_HOST_MEMORY == local_res) {
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }
    }
out:
    free_string_list(inst, &manifest_files);

    return res;
}

void loader_remove_duplicate_layers(struct loader_instance* inst, struct loader_layer_list* layers) {
    // Remove duplicate layers (that share the same layerName)
    for (uint32_t i = 0; i < layers->count; ++i) {
        bool has_duplicate = false;
        uint32_t duplicate_index = 0;
        for (uint32_t j = i + 1; j < layers->count; ++j) {
            if (strcmp(layers->list[i].info.layerName, layers->list[j].info.layerName) == 0) {
                has_duplicate = true;
                duplicate_index = j;
                break;
            }
        }
        if (has_duplicate) {
            loader_log(inst, VULKAN_LOADER_WARN_BIT, 0, "Removing layer %s (%s) because it is a duplicate of %s (%s)",
                       layers->list[duplicate_index].info.layerName, layers->list[duplicate_index].manifest_file_name,
                       layers->list[i].info.layerName, layers->list[i].manifest_file_name);
            loader_remove_layer_in_list(inst, layers, duplicate_index);
            // There may be more copies of this layer further down the list, so check it again
            i--;
        }
    }
}

VkResult loader_scan_for_layers(struct loader_instance* inst, struct loader_layer_list* instance_layers,
                                const struct loader_envvar_all_filters* filters) {
    VkResult res = VK_SUCCESS;
    struct loader_layer_list settings_layers = {0};
    struct loader_layer_list regular_instance_layers = {0};
    struct loader_string_list override_paths = {0};

    bool should_search_for_other_layers = true;
    res = get_settings_layers(inst, &settings_layers, &should_search_for_other_layers);
    if (VK_SUCCESS != res) {
        goto out;
    }

    // If we should not look for layers using other mechanisms, assign settings_layers to instance_layers and jump to the
    // output
    if (!should_search_for_other_layers) {
        *instance_layers = settings_layers;
        memset(&settings_layers, 0, sizeof(struct loader_layer_list));
        goto out;
    }

    res = loader_parse_instance_layers(inst, LOADER_DATA_FILE_MANIFEST_IMPLICIT_LAYER, NULL, &regular_instance_layers);
    if (VK_SUCCESS != res) {
        goto out;
    }

    // Get a list of manifest files for explicit layers
    res = loader_parse_instance_layers(inst, LOADER_DATA_FILE_MANIFEST_EXPLICIT_LAYER, &override_paths, &regular_instance_layers);
    if (VK_SUCCESS != res) {
        goto out;
    }

    // Remove disabled layers
    for (uint32_t i = 0; i < regular_instance_layers.count; ++i) {
        if (!loader_layer_is_available(inst, filters, &regular_instance_layers.list[i])) {
            loader_remove_layer_in_list(inst, &regular_instance_layers, i);
            i--;
        }
    }

    // Make sure no layers have the same layerName
    loader_remove_duplicate_layers(inst, &regular_instance_layers);

    res = combine_settings_layers_with_regular_layers(inst, &settings_layers, &regular_instance_layers, instance_layers);

out:
    loader_delete_layer_list_and_properties(inst, &settings_layers);
    loader_delete_layer_list_and_properties(inst, &regular_instance_layers);

    free_string_list(inst, &override_paths);
    return res;
}

VkResult loader_scan_for_implicit_layers(struct loader_instance* inst, struct loader_layer_list* instance_layers,
                                         const struct loader_envvar_all_filters* layer_filters) {
    VkResult res = VK_SUCCESS;
    struct loader_layer_list settings_layers = {0};
    struct loader_layer_list regular_instance_layers = {0};
    struct loader_string_list override_paths = {0};

    bool should_search_for_other_layers = true;
    res = get_settings_layers(inst, &settings_layers, &should_search_for_other_layers);
    if (VK_SUCCESS != res) {
        goto out;
    }

    // Remove layers from settings file that are off, are explicit, or are implicit layers that aren't active
    for (uint32_t i = 0; i < settings_layers.count; ++i) {
        // Keep the unordered layer location, combine_settings_layers_with_regular_layers needs it to place the regular layers
        if (settings_layers.list[i].settings_control_value == LOADER_SETTINGS_LAYER_UNORDERED_LAYER_LOCATION) {
            continue;
        }
        if (settings_layers.list[i].settings_control_value == LOADER_SETTINGS_LAYER_CONTROL_OFF ||
            (settings_layers.list[i].type_flags & VK_LAYER_TYPE_FLAG_EXPLICIT_LAYER) == VK_LAYER_TYPE_FLAG_EXPLICIT_LAYER ||
            !loader_implicit_layer_is_enabled(inst, layer_filters, &settings_layers.list[i])) {
            loader_remove_layer_in_list(inst, &settings_layers, i);
            i--;
        }
    }

    // If we should not look for layers using other mechanisms, assign settings_layers to instance_layers and jump to the
    // output
    if (!should_search_for_other_layers) {
        *instance_layers = settings_layers;
        memset(&settings_layers, 0, sizeof(struct loader_layer_list));
        goto out;
    }

    res = loader_parse_instance_layers(inst, LOADER_DATA_FILE_MANIFEST_IMPLICIT_LAYER, NULL, &regular_instance_layers);
    if (VK_SUCCESS != res) {
        goto out;
    }

    // Remove disabled layers
    for (uint32_t i = 0; i < regular_instance_layers.count; ++i) {
        if (!loader_implicit_layer_is_enabled(inst, layer_filters, &regular_instance_layers.list[i])) {
            loader_remove_layer_in_list(inst, &regular_instance_layers, i);
            i--;
        }
    }

    // Make sure no layers have the same layerName
    loader_remove_duplicate_layers(inst, &regular_instance_layers);

    res = combine_settings_layers_with_regular_layers(inst, &settings_layers, &regular_instance_layers, instance_layers);

out:
    loader_delete_layer_list_and_properties(inst, &settings_layers);
    loader_delete_layer_list_and_properties(inst, &regular_instance_layers);

    free_string_list(inst, &override_paths);
    return res;
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL loader_gpdpa_instance_terminator(VkInstance inst, const char* pName) {
    // inst is not wrapped
    if (inst == VK_NULL_HANDLE) {
        return NULL;
    }

    VkLayerInstanceDispatchTable* disp_table = *(VkLayerInstanceDispatchTable**)inst;

    if (disp_table == NULL) return NULL;

    struct loader_instance* loader_inst = loader_get_instance(inst);

    if (loader_inst->instance_finished_creation) {
        disp_table = &loader_inst->terminator_dispatch;
    }

    bool found_name;
    void* addr = loader_lookup_instance_dispatch_table(disp_table, pName, &found_name);
    if (found_name) {
        return addr;
    }

    // Check if any drivers support the function, and if so, add it to the unknown function list
    addr = loader_phys_dev_ext_gpa_term(loader_get_instance(inst), pName);
    if (NULL != addr) return addr;

    // Don't call down the chain, this would be an infinite loop
    loader_log(NULL, VULKAN_LOADER_DEBUG_BIT, 0, "loader_gpdpa_instance_terminator() unrecognized name %s", pName);
    return NULL;
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL loader_gpa_instance_terminator(VkInstance inst, const char* pName) {
    // Global functions - Do not need a valid instance handle to query
    if (!strcmp(pName, "vkGetInstanceProcAddr")) {
        return (PFN_vkVoidFunction)loader_gpa_instance_terminator;
    }
    if (!strcmp(pName, "vk_layerGetPhysicalDeviceProcAddr")) {
        return (PFN_vkVoidFunction)loader_gpdpa_instance_terminator;
    }
    if (!strcmp(pName, "vkCreateInstance")) {
        return (PFN_vkVoidFunction)terminator_CreateInstance;
    }
    // If a layer is querying pre-instance functions using vkGetInstanceProcAddr, we need to return function pointers that match the
    // Vulkan API
    if (!strcmp(pName, "vkEnumerateInstanceLayerProperties")) {
        return (PFN_vkVoidFunction)terminator_EnumerateInstanceLayerProperties;
    }
    if (!strcmp(pName, "vkEnumerateInstanceExtensionProperties")) {
        return (PFN_vkVoidFunction)terminator_EnumerateInstanceExtensionProperties;
    }
    if (!strcmp(pName, "vkEnumerateInstanceVersion")) {
        return (PFN_vkVoidFunction)terminator_EnumerateInstanceVersion;
    }

    // While the spec is very clear that querying vkCreateDevice requires a valid VkInstance, because the loader allowed querying
    // with a NULL VkInstance handle for a long enough time, it is impractical to fix this bug in the loader

    // As such, this is a bug to maintain compatibility for the RTSS layer (Riva Tuner Statistics Server) but may
    // be depended upon by other layers out in the wild.
    if (!strcmp(pName, "vkCreateDevice")) {
        return (PFN_vkVoidFunction)terminator_CreateDevice;
    }

    // inst is not wrapped
    if (inst == VK_NULL_HANDLE) {
        return NULL;
    }
    VkLayerInstanceDispatchTable* disp_table = *(VkLayerInstanceDispatchTable**)inst;

    if (disp_table == NULL) return NULL;

    struct loader_instance* loader_inst = loader_get_instance(inst);

    // The VK_EXT_debug_utils functions need a special case here so the terminators can still be found from
    // vkGetInstanceProcAddr This is because VK_EXT_debug_utils is an instance level extension with device level functions, and
    // is 'supported' by the loader.
    // These functions need a terminator to handle the case of a driver not supporting VK_EXT_debug_utils when there are layers
    // present which not check for NULL before calling the function.
    if (!strcmp(pName, "vkSetDebugUtilsObjectNameEXT")) {
        return loader_inst->enabled_extensions.ext_debug_utils ? (PFN_vkVoidFunction)terminator_SetDebugUtilsObjectNameEXT : NULL;
    }
    if (!strcmp(pName, "vkSetDebugUtilsObjectTagEXT")) {
        return loader_inst->enabled_extensions.ext_debug_utils ? (PFN_vkVoidFunction)terminator_SetDebugUtilsObjectTagEXT : NULL;
    }
    if (!strcmp(pName, "vkQueueBeginDebugUtilsLabelEXT")) {
        return loader_inst->enabled_extensions.ext_debug_utils ? (PFN_vkVoidFunction)terminator_QueueBeginDebugUtilsLabelEXT : NULL;
    }
    if (!strcmp(pName, "vkQueueEndDebugUtilsLabelEXT")) {
        return loader_inst->enabled_extensions.ext_debug_utils ? (PFN_vkVoidFunction)terminator_QueueEndDebugUtilsLabelEXT : NULL;
    }
    if (!strcmp(pName, "vkQueueInsertDebugUtilsLabelEXT")) {
        return loader_inst->enabled_extensions.ext_debug_utils ? (PFN_vkVoidFunction)terminator_QueueInsertDebugUtilsLabelEXT
                                                               : NULL;
    }
    if (!strcmp(pName, "vkCmdBeginDebugUtilsLabelEXT")) {
        return loader_inst->enabled_extensions.ext_debug_utils ? (PFN_vkVoidFunction)terminator_CmdBeginDebugUtilsLabelEXT : NULL;
    }
    if (!strcmp(pName, "vkCmdEndDebugUtilsLabelEXT")) {
        return loader_inst->enabled_extensions.ext_debug_utils ? (PFN_vkVoidFunction)terminator_CmdEndDebugUtilsLabelEXT : NULL;
    }
    if (!strcmp(pName, "vkCmdInsertDebugUtilsLabelEXT")) {
        return loader_inst->enabled_extensions.ext_debug_utils ? (PFN_vkVoidFunction)terminator_CmdInsertDebugUtilsLabelEXT : NULL;
    }

    if (loader_inst->instance_finished_creation) {
        disp_table = &loader_inst->terminator_dispatch;
    }

    bool found_name;
    void* addr = loader_lookup_instance_dispatch_table(disp_table, pName, &found_name);
    if (found_name) {
        return addr;
    }

    // Check if it is an unknown physical device function, to see if any drivers support it.
    addr = loader_phys_dev_ext_gpa_term(loader_get_instance(inst), pName);
    if (addr) {
        return addr;
    }

    // Assume it is an unknown device function, check to see if any drivers support it.
    addr = loader_dev_ext_gpa_term(loader_get_instance(inst), pName);
    if (addr) {
        return addr;
    }

    // Don't call down the chain, this would be an infinite loop
    loader_log(NULL, VULKAN_LOADER_DEBUG_BIT, 0, "loader_gpa_instance_terminator() unrecognized name %s", pName);
    return NULL;
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL loader_gpa_device_terminator(VkDevice device, const char* pName) {
    struct loader_device* dev;
    struct loader_icd_term* icd_term = loader_get_icd_and_device(device, &dev);

    // Return this function if a layer above here is asking for the vkGetDeviceProcAddr.
    // This is so we can properly intercept any device commands needing a terminator.
    if (!strcmp(pName, "vkGetDeviceProcAddr")) {
        return (PFN_vkVoidFunction)loader_gpa_device_terminator;
    }

    // NOTE: Device Funcs needing Trampoline/Terminator.
    // Overrides for device functions needing a trampoline and
    // a terminator because certain device entry-points still need to go
    // through a terminator before hitting the ICD.  This could be for
    // several reasons, but the main one is currently unwrapping an
    // object before passing the appropriate info along to the ICD.
    // This is why we also have to override the direct ICD call to
    // vkGetDeviceProcAddr to intercept those calls.
    // If the pName is for a 'known' function but isn't available, due to
    // the corresponding extension/feature not being enabled, we need to
    // return NULL and not call down to the driver's GetDeviceProcAddr.
    if (NULL != dev) {
        bool found_name = false;
        PFN_vkVoidFunction addr = get_extension_device_proc_terminator(dev, pName, &found_name);
        if (found_name) {
            return addr;
        }
    }

    if (icd_term == NULL) {
        return NULL;
    }

    return icd_term->dispatch.GetDeviceProcAddr(device, pName);
}

struct loader_instance* loader_get_instance(VkInstance instance) {
    // look up the loader_instance in our list by comparing dispatch tables, as
    // there is no guarantee the instance is still a loader_instance* after any
    // layers which wrap the instance object.
    const VkLayerInstanceDispatchTable* disp;
    struct loader_instance* ptr_instance = (struct loader_instance*)instance;
    if (VK_NULL_HANDLE == instance || LOADER_MAGIC_NUMBER != ptr_instance->magic) {
        return NULL;
    }
    disp = loader_get_instance_layer_dispatch(instance);
    loader_platform_thread_lock_mutex(&loader_lock);
    for (struct loader_instance* inst = loader.instances; inst; inst = inst->next) {
        if (&inst->disp->layer_inst_disp == disp) {
            ptr_instance = inst;
            break;
        }
    }
    loader_platform_thread_unlock_mutex(&loader_lock);
    return ptr_instance;
}

loader_platform_dl_handle loader_open_layer_file(const struct loader_instance* inst, struct loader_layer_properties* prop,
                                                 bool* oom) {
    loader_platform_dl_handle lib_handle = loader_platform_open_library(inst, prop->lib_name, oom);
    prop->lib_handle = lib_handle;
    if (lib_handle == NULL) {
        loader_handle_load_library_error(inst, prop->lib_name, &prop->lib_status);
    } else {
        prop->lib_status = LOADER_LAYER_LIB_SUCCESS_LOADED;
        loader_log(inst, VULKAN_LOADER_DEBUG_BIT | VULKAN_LOADER_LAYER_BIT, 0, "Loading layer library %s", prop->lib_name);
    }

    return prop->lib_handle;
}

// Returns true if name is one of the PATH_SEPARATOR separated entries of layers_env. Matches whole entries only, the same way
// loader_add_environment_layers splits and compares them.
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters) - distinct roles (entry list vs. single name), no safer ordering
static bool layers_env_contains_name(const char* layers_env, const char* name) {
    size_t name_len = strlen(name);
    const char* entry = layers_env;
    while (*entry != '\0') {
        const char* separator = strchr(entry, PATH_SEPARATOR);
        size_t entry_len = NULL != separator ? (size_t)(separator - entry) : strlen(entry);
        if (entry_len == name_len && 0 == strncmp(entry, name, name_len)) {
            return true;
        }
        if (NULL == separator) {
            break;
        }
        entry = separator + 1;
    }
    return false;
}

// Go through the search_list and find any layers which match type. If layer
// type match is found in then add it to ext_list.
// If the layer name is in enabled_layers_env, do not add it to the list, that way it can be ordered alongside the other env-var
// enabled layers
VkResult loader_add_implicit_layers(struct loader_instance* inst, const char* enabled_layers_env,
                                    const struct loader_envvar_all_filters* filters) {
    for (uint32_t src_layer = 0; src_layer < inst->instance_layer_list.count; src_layer++) {
        struct loader_layer_properties* prop = &inst->instance_layer_list.list[src_layer];
        if (0 == (prop->type_flags & VK_LAYER_TYPE_FLAG_EXPLICIT_LAYER)) {
            // If this layer appears in the enabled_layers_env, don't add it. We will let loader_add_environment_layers handle it
            if (NULL == enabled_layers_env || !layers_env_contains_name(enabled_layers_env, prop->info.layerName)) {
                VkResult result = loader_add_implicit_layer(inst, prop, filters);
                if (result == VK_ERROR_OUT_OF_HOST_MEMORY) return result;
            }
        }
    }
    return VK_SUCCESS;
}

void warn_if_layers_are_older_than_application(struct loader_instance* inst) {
    for (uint32_t i = 0; i < inst->expanded_activated_layer_list.count; i++) {
        // Verify that the layer api version is at least that of the application's request, if not, throw a warning since
        // undefined behavior could occur.
        struct loader_layer_properties* prop = inst->expanded_activated_layer_list.list[i];
        loader_api_version prop_spec_version = loader_make_version(prop->info.specVersion);
        if (!loader_check_version_meets_required(inst->app_api_version, prop_spec_version)) {
            loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                       "Layer %s uses API version %u.%u which is older than the application specified "
                       "API version of %u.%u. May cause issues.",
                       prop->info.layerName, prop_spec_version.major, prop_spec_version.minor, inst->app_api_version.major,
                       inst->app_api_version.minor);
        }
    }
}

VkResult loader_enable_instance_layers(struct loader_instance* inst, const VkInstanceCreateInfo* pCreateInfo,
                                       const struct loader_envvar_all_filters* layer_filters) {
    VkResult res = VK_SUCCESS;
    char* enabled_layers_env = NULL;

    assert(inst && "Cannot have null instance");

    if (!loader_init_pointer_layer_list(inst)) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "loader_enable_instance_layers: Failed to initialize expanded version of the layer list");
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    if (inst->settings.settings_active && inst->settings.layer_configurations_active) {
        res = enable_correct_layers_from_settings(inst, layer_filters, pCreateInfo->enabledLayerCount,
                                                  pCreateInfo->ppEnabledLayerNames);
        warn_if_layers_are_older_than_application(inst);

        goto out;
    }

    enabled_layers_env = loader_getenv(ENABLED_LAYERS_ENV, inst);

    // Add any implicit layers first
    res = loader_add_implicit_layers(inst, enabled_layers_env, layer_filters);
    if (res != VK_SUCCESS) {
        goto out;
    }

    // Add any layers specified via environment variable next
    res = loader_add_environment_layers(inst, enabled_layers_env, layer_filters);
    if (res != VK_SUCCESS) {
        goto out;
    }

    // Add layers specified by the application
    res = loader_add_layer_names_to_list(inst, layer_filters, pCreateInfo->enabledLayerCount, pCreateInfo->ppEnabledLayerNames);

    warn_if_layers_are_older_than_application(inst);
out:
    if (enabled_layers_env != NULL) {
        loader_free_getenv(enabled_layers_env, inst);
    }

    return res;
}

// Determine the layer interface version to use.
bool loader_get_layer_interface_version(PFN_vkNegotiateLoaderLayerInterfaceVersion fp_negotiate_layer_version,
                                        VkNegotiateLayerInterface* interface_struct) {
    memset(interface_struct, 0, sizeof(VkNegotiateLayerInterface));
    interface_struct->sType = LAYER_NEGOTIATE_INTERFACE_STRUCT;
    interface_struct->loaderLayerInterfaceVersion = 1;
    interface_struct->pNext = NULL;

    // If the layer doesn't support the negotiation API, then negotiation failed.
    if (fp_negotiate_layer_version == NULL) {
        return false;
    }

    // Layer supports the negotiation API, so call it with the loader's
    // latest version supported
    interface_struct->loaderLayerInterfaceVersion = CURRENT_LOADER_LAYER_INTERFACE_VERSION;
    VkResult result = fp_negotiate_layer_version(interface_struct);

    if (result != VK_SUCCESS) {
        // Layer no longer supports the loader's latest interface version so fail loading the Layer.
        return false;
    }

    if (interface_struct->loaderLayerInterfaceVersion < MIN_SUPPORTED_LOADER_LAYER_INTERFACE_VERSION) {
        // Loader no longer supports the layer's latest interface version so fail loading the layer.
        return false;
    }
    return true;
}

// Every extension that has a loader-defined trampoline needs to be marked as enabled or disabled so that we know whether or
// not to return that trampoline when vkGetDeviceProcAddr is called
void setup_logical_device_enabled_layer_extensions(const struct loader_instance* inst, struct loader_device* dev,
                                                   const struct loader_extension_list* icd_exts,
                                                   const VkDeviceCreateInfo* pCreateInfo) {
    // no enabled extensions, early exit
    if (pCreateInfo->ppEnabledExtensionNames == NULL) {
        return;
    }
    // Can only setup debug marker as debug utils is an instance extensions.
    for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; ++i) {
        if (pCreateInfo->ppEnabledExtensionNames[i] &&
            !strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_EXT_DEBUG_MARKER_EXTENSION_NAME)) {
            // Check if its supported by the driver
            for (uint32_t j = 0; j < icd_exts->count; ++j) {
                if (!strcmp(icd_exts->list[j].extensionName, VK_EXT_DEBUG_MARKER_EXTENSION_NAME)) {
                    dev->layer_extensions.ext_debug_marker_enabled = true;
                }
            }
            // also check if any layers support it.
            for (uint32_t j = 0; j < inst->expanded_activated_layer_list.count; j++) {
                struct loader_layer_properties* layer = inst->expanded_activated_layer_list.list[j];
                if (layer->lib_status == LOADER_LAYER_LIB_SUCCESS_LOADED) {
                    for (uint32_t k = 0; k < layer->device_extension_list.count; k++) {
                        if (!strcmp(layer->device_extension_list.list[k].props.extensionName, VK_EXT_DEBUG_MARKER_EXTENSION_NAME)) {
                            dev->layer_extensions.ext_debug_marker_enabled = true;
                        }
                    }
                }
            }
        }
    }
}

VKAPI_ATTR VkResult VKAPI_CALL loader_layer_create_device(VkInstance instance, VkPhysicalDevice physicalDevice,
                                                          const VkDeviceCreateInfo* pCreateInfo,
                                                          const VkAllocationCallbacks* pAllocator, VkDevice* pDevice,
                                                          PFN_vkGetInstanceProcAddr layerGIPA, PFN_vkGetDeviceProcAddr* nextGDPA) {
    VkResult res = VK_SUCCESS;
    VkPhysicalDevice internal_device = VK_NULL_HANDLE;
    struct loader_device* dev = NULL;
    struct loader_instance* inst = NULL;

    loader_platform_thread_lock_mutex(&loader_lock);

    // A layer creating a device through pfnLayerCreateDevice passes an instance handle along with a physical device from
    // its own point in the call chain, so only the application's physical devices are loader trampoline objects.
    const bool app_provided_phys_devs = (VK_NULL_HANDLE == instance);

    if (!app_provided_phys_devs) {
        inst = loader_get_instance(instance);
        internal_device = physicalDevice;
    } else {
        struct loader_physical_device_tramp* phys_dev = (struct loader_physical_device_tramp*)physicalDevice;
        internal_device = phys_dev->phys_dev;
        inst = phys_dev->this_instance;
    }

    // Get the physical device (ICD) extensions
    struct loader_extension_list icd_exts = {0};
    icd_exts.list = NULL;
    res = loader_init_generic_list(inst, (struct loader_generic_list*)&icd_exts, sizeof(VkExtensionProperties));
    if (VK_SUCCESS != res) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "vkCreateDevice: Failed to create ICD extension list");
        goto out;
    }

    PFN_vkEnumerateDeviceExtensionProperties enumDeviceExtensionProperties = NULL;
    if (layerGIPA != NULL) {
        enumDeviceExtensionProperties =
            (PFN_vkEnumerateDeviceExtensionProperties)layerGIPA(instance, "vkEnumerateDeviceExtensionProperties");
    } else {
        enumDeviceExtensionProperties = inst->disp->layer_inst_disp.EnumerateDeviceExtensionProperties;
    }
    res = loader_add_device_extensions(inst, enumDeviceExtensionProperties, internal_device, "Unknown", &icd_exts);
    if (res != VK_SUCCESS) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "vkCreateDevice: Failed to add extensions to list");
        goto out;
    }

    // Make sure requested extensions to be enabled are supported
    res = loader_validate_device_extensions(inst, &inst->expanded_activated_layer_list, &icd_exts, pCreateInfo);
    if (res != VK_SUCCESS) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "vkCreateDevice: Failed to validate extensions in list");
        goto out;
    }

    dev = loader_create_logical_device(inst, pAllocator);
    if (dev == NULL) {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    setup_logical_device_enabled_layer_extensions(inst, dev, &icd_exts, pCreateInfo);

    res = loader_create_device_chain(internal_device, pCreateInfo, pAllocator, inst, dev, layerGIPA, nextGDPA,
                                     app_provided_phys_devs);
    if (res != VK_SUCCESS) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "vkCreateDevice:  Failed to create device chain.");
        goto out;
    }

    *pDevice = dev->chain_device;

    // Initialize any device extension dispatch entry's from the instance list
    loader_init_dispatch_dev_ext(inst, dev);

    // Initialize WSI device extensions as part of core dispatch since loader
    // has dedicated trampoline code for these
    loader_init_device_extension_dispatch_table(&dev->loader_dispatch, inst->disp->layer_inst_disp.GetInstanceProcAddr,
                                                dev->loader_dispatch.core_dispatch.GetDeviceProcAddr, inst->instance, *pDevice);

out:

    // Failure cleanup
    if (VK_SUCCESS != res) {
        if (NULL != dev) {
            // Find the icd_term this device belongs to then remove it from that icd_term.
            // Need to iterate the linked lists and remove the device from it. Don't delete
            // the device here since it may not have been added to the icd_term and there
            // are other allocations attached to it.
            struct loader_icd_term* icd_term = inst->icd_terms;
            bool found = false;
            while (!found && NULL != icd_term) {
                struct loader_device* cur_dev = icd_term->logical_device_list;
                struct loader_device* prev_dev = NULL;
                while (NULL != cur_dev) {
                    if (cur_dev == dev) {
                        if (cur_dev == icd_term->logical_device_list) {
                            icd_term->logical_device_list = cur_dev->next;
                        } else if (prev_dev) {
                            prev_dev->next = cur_dev->next;
                        }

                        found = true;
                        break;
                    }
                    prev_dev = cur_dev;
                    cur_dev = cur_dev->next;
                }
                icd_term = icd_term->next;
            }
            // Now destroy the device and the allocations associated with it.
            loader_destroy_logical_device(dev, pAllocator);
        }
    }

    if (NULL != icd_exts.list) {
        loader_destroy_generic_list(inst, (struct loader_generic_list*)&icd_exts);
    }
    loader_platform_thread_unlock_mutex(&loader_lock);

    return res;
}

VKAPI_ATTR void VKAPI_CALL loader_layer_destroy_device(VkDevice device, const VkAllocationCallbacks* pAllocator,
                                                       PFN_vkDestroyDevice destroyFunction) {
    struct loader_device* dev;

    if (device == VK_NULL_HANDLE) {
        return;
    }

    loader_platform_thread_lock_mutex(&loader_lock);

    struct loader_icd_term* icd_term = loader_get_icd_and_device(device, &dev);

    destroyFunction(device, pAllocator);
    if (NULL != dev) {
        dev->chain_device = NULL;
        dev->icd_device = NULL;
        loader_remove_logical_device(icd_term, dev, pAllocator);
    }
    loader_platform_thread_unlock_mutex(&loader_lock);
}

// Given the list of layers to activate in the loader_instance
// structure. This function will add a VkLayerInstanceCreateInfo
// structure to the VkInstanceCreateInfo.pNext pointer.
// Each activated layer will have it's own VkLayerInstanceLink
// structure that tells the layer what Get*ProcAddr to call to
// get function pointers to the next layer down.
// Once the chain info has been created this function will
// execute the CreateInstance call chain. Each layer will
// then have an opportunity in it's CreateInstance function
// to setup it's dispatch table when the lower layer returns
// successfully.
// Each layer can wrap or not-wrap the returned VkInstance object
// as it sees fit.
// The instance chain is terminated by a loader function
// that will call CreateInstance on all available ICD's and
// cache those VkInstance objects for future use.
VkResult loader_create_instance_chain(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator,
                                      struct loader_instance* inst, VkInstance* created_instance) {
    uint32_t num_activated_layers = 0;
    struct activated_layer_info* activated_layers = NULL;
    VkLayerInstanceCreateInfo chain_info = {0};
    VkLayerInstanceLink* layer_instance_link_info = NULL;
    VkInstanceCreateInfo loader_create_info = {0};
    VkResult res = VK_SUCCESS;

    PFN_vkGetInstanceProcAddr next_gipa = loader_gpa_instance_terminator;
    PFN_vkGetInstanceProcAddr cur_gipa = loader_gpa_instance_terminator;
    PFN_vkGetDeviceProcAddr cur_gdpa = loader_gpa_device_terminator;
    PFN_GetPhysicalDeviceProcAddr next_gpdpa = loader_gpdpa_instance_terminator;
    PFN_GetPhysicalDeviceProcAddr cur_gpdpa = loader_gpdpa_instance_terminator;

    memcpy(&loader_create_info, pCreateInfo, sizeof(VkInstanceCreateInfo));

    if (inst->expanded_activated_layer_list.count > 0) {
        chain_info.u.pLayerInfo = NULL;
        chain_info.pNext = pCreateInfo->pNext;
        chain_info.sType = VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO;
        chain_info.function = VK_LAYER_LINK_INFO;
        loader_create_info.pNext = &chain_info;

        layer_instance_link_info = loader_instance_heap_calloc(
            inst, sizeof(VkLayerInstanceLink) * inst->expanded_activated_layer_list.count, VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (!layer_instance_link_info) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_create_instance_chain: Failed to alloc Instance objects for layer");
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }

        activated_layers =
            loader_instance_heap_calloc(inst, sizeof(struct activated_layer_info) * inst->expanded_activated_layer_list.count,
                                        VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (!activated_layers) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_create_instance_chain: Failed to alloc activated layer storage array");
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }

        // Create instance chain of enabled layers
        for (int32_t i = (int32_t)inst->expanded_activated_layer_list.count - 1; i >= 0; i--) {
            struct loader_layer_properties* layer_prop = inst->expanded_activated_layer_list.list[i];
            loader_platform_dl_handle lib_handle;

            // Skip it if a Layer with the same name has been already successfully activated
            // Ignore this if the settings file is active
            if ((!inst->settings.settings_active || !inst->settings.layer_configurations_active) &&
                loader_names_array_has_layer_property(&layer_prop->info, num_activated_layers, activated_layers)) {
                continue;
            }

            bool oom = false;
            lib_handle = loader_open_layer_file(inst, layer_prop, &oom);
            if (oom || layer_prop->lib_status == LOADER_LAYER_LIB_ERROR_OUT_OF_MEMORY) {
                res = VK_ERROR_OUT_OF_HOST_MEMORY;
                goto out;
            }
            if (!lib_handle) {
                continue;
            }

            if (NULL == layer_prop->functions.negotiate_layer_interface) {
                PFN_vkNegotiateLoaderLayerInterfaceVersion negotiate_interface = NULL;
                bool functions_in_interface = false;
                if (!layer_prop->functions.str_negotiate_interface || strlen(layer_prop->functions.str_negotiate_interface) == 0) {
                    negotiate_interface = (PFN_vkNegotiateLoaderLayerInterfaceVersion)loader_platform_get_proc_address(
                        lib_handle, "vkNegotiateLoaderLayerInterfaceVersion");
                } else {
                    negotiate_interface = (PFN_vkNegotiateLoaderLayerInterfaceVersion)loader_platform_get_proc_address(
                        lib_handle, layer_prop->functions.str_negotiate_interface);
                }

                // If we can negotiate an interface version, then we can get everything we need from the one function call.
                if (NULL != negotiate_interface) {
                    layer_prop->functions.negotiate_layer_interface = negotiate_interface;

                    VkNegotiateLayerInterface interface_struct = {0};
                    bool compatible = loader_get_layer_interface_version(negotiate_interface, &interface_struct);
                    // When a layer supports vkNegotiateLoaderLayerInterfaceVersion but fails when the loader calls it, skip the
                    // layer as it isn't compatible.
                    if (!compatible) {
                        loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                                   "loader_create_instance_chain: Failed to negotiate a compatible interface version with layer "
                                   "\"%s\", skipping",
                                   layer_prop->lib_name);
                        layer_prop->lib_status = LOADER_LAYER_LIB_ERROR_NEGOTIATE_INTERFACE_VERSION_FAILED;
                        continue;
                    }

                    // Go ahead and set the properties version to the correct value.
                    layer_prop->interface_version = interface_struct.loaderLayerInterfaceVersion;

                    // If the interface is 2 or newer, we have access to the
                    // new GetPhysicalDeviceProcAddr function, so grab it,
                    // and the other necessary functions, from the structure.
                    if (interface_struct.loaderLayerInterfaceVersion > 1) {
                        cur_gipa = interface_struct.pfnGetInstanceProcAddr;
                        cur_gdpa = interface_struct.pfnGetDeviceProcAddr;
                        cur_gpdpa = interface_struct.pfnGetPhysicalDeviceProcAddr;
                        if (cur_gipa != NULL) {
                            // We've set the functions, so make sure we
                            // don't do the unnecessary calls later.
                            functions_in_interface = true;
                        }
                    }
                }

                if (!functions_in_interface) {
                    cur_gipa = layer_prop->functions.get_instance_proc_addr;
                    if (cur_gipa == NULL) {
                        if (layer_prop->functions.str_gipa == NULL || strlen(layer_prop->functions.str_gipa) == 0) {
                            cur_gipa =
                                (PFN_vkGetInstanceProcAddr)loader_platform_get_proc_address(lib_handle, "vkGetInstanceProcAddr");
                            layer_prop->functions.get_instance_proc_addr = cur_gipa;

                            if (NULL == cur_gipa) {
                                loader_log(inst, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                                           "loader_create_instance_chain: Failed to find \'vkGetInstanceProcAddr\' in layer \"%s\"",
                                           layer_prop->lib_name);
                                layer_prop->lib_status = LOADER_LAYER_LIB_ERROR_UNABLE_TO_FIND_VK_GET_INSTANCE_PROC_ADDR;
                                continue;
                            }
                        } else {
                            cur_gipa = (PFN_vkGetInstanceProcAddr)loader_platform_get_proc_address(lib_handle,
                                                                                                   layer_prop->functions.str_gipa);

                            if (NULL == cur_gipa) {
                                loader_log(inst, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                                           "loader_create_instance_chain: Failed to find \'%s\' in layer \"%s\"",
                                           layer_prop->functions.str_gipa, layer_prop->lib_name);
                                layer_prop->lib_status = LOADER_LAYER_LIB_ERROR_UNABLE_TO_FIND_VK_GET_INSTANCE_PROC_ADDR;
                                continue;
                            }
                        }
                    }
                }
            }

            layer_instance_link_info[num_activated_layers].pNext = chain_info.u.pLayerInfo;
            layer_instance_link_info[num_activated_layers].pfnNextGetInstanceProcAddr = next_gipa;
            layer_instance_link_info[num_activated_layers].pfnNextGetPhysicalDeviceProcAddr = next_gpdpa;
            next_gipa = cur_gipa;
            if (layer_prop->interface_version > 1 && cur_gpdpa != NULL) {
                layer_prop->functions.get_physical_device_proc_addr = cur_gpdpa;
                next_gpdpa = cur_gpdpa;
            }
            if (layer_prop->interface_version > 1 && cur_gipa != NULL) {
                layer_prop->functions.get_instance_proc_addr = cur_gipa;
            }
            if (layer_prop->interface_version > 1 && cur_gdpa != NULL) {
                layer_prop->functions.get_device_proc_addr = cur_gdpa;
            }

            chain_info.u.pLayerInfo = &layer_instance_link_info[num_activated_layers];

            res = fixup_library_binary_path(inst, &(layer_prop->lib_name), layer_prop->lib_handle, cur_gipa);
            if (res == VK_ERROR_OUT_OF_HOST_MEMORY) {
                goto out;
            }

            activated_layers[num_activated_layers].name = layer_prop->info.layerName;
            activated_layers[num_activated_layers].manifest = layer_prop->manifest_file_name;
            activated_layers[num_activated_layers].library = layer_prop->lib_name;
            activated_layers[num_activated_layers].is_implicit = !(layer_prop->type_flags & VK_LAYER_TYPE_FLAG_EXPLICIT_LAYER);
            activated_layers[num_activated_layers].enabled_by_what = layer_prop->enabled_by_what;
            if (activated_layers[num_activated_layers].is_implicit) {
                activated_layers[num_activated_layers].disable_env = layer_prop->disable_env_var.name;
                activated_layers[num_activated_layers].enable_name_env = layer_prop->enable_env_var.name;
                activated_layers[num_activated_layers].enable_value_env = layer_prop->enable_env_var.value;
            }

            loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_LAYER_BIT, 0, "Insert instance layer \"%s\" (%s)",
                       layer_prop->info.layerName, layer_prop->lib_name);

            num_activated_layers++;
        }
    }

    // Make sure each layer requested by the application was actually loaded
    for (uint32_t exp = 0; exp < inst->expanded_activated_layer_list.count; ++exp) {
        struct loader_layer_properties* exp_layer_prop = inst->expanded_activated_layer_list.list[exp];
        bool found = false;
        for (uint32_t act = 0; act < num_activated_layers; ++act) {
            if (!strcmp(activated_layers[act].name, exp_layer_prop->info.layerName)) {
                found = true;
                break;
            }
        }
        // If it wasn't found we want to at least log an error.  However if it was enabled by the application directly, we want to
        // return a bad layer error.
        if (!found) {
            bool app_requested = false;
            for (uint32_t act = 0; act < pCreateInfo->enabledLayerCount; ++act) {
                if (!strcmp(pCreateInfo->ppEnabledLayerNames[act], exp_layer_prop->info.layerName)) {
                    app_requested = true;
                    break;
                }
            }
            VkFlags log_flag = VULKAN_LOADER_LAYER_BIT;
            char ending = '.';
            if (app_requested) {
                log_flag |= VULKAN_LOADER_ERROR_BIT;
                ending = '!';
            } else {
                log_flag |= VULKAN_LOADER_INFO_BIT;
            }
            switch (exp_layer_prop->lib_status) {
                case LOADER_LAYER_LIB_NOT_LOADED:
                    loader_log(inst, log_flag, 0, "Requested layer \"%s\" was not loaded%c", exp_layer_prop->info.layerName,
                               ending);
                    break;
                case LOADER_LAYER_LIB_ERROR_WRONG_BIT_TYPE: {
                    loader_log(inst, log_flag, 0, "Requested layer \"%s\" was wrong bit-type%c", exp_layer_prop->info.layerName,
                               ending);
                    break;
                }
                case LOADER_LAYER_LIB_ERROR_FAILED_TO_LOAD:
                    loader_log(inst, log_flag, 0, "Requested layer \"%s\" failed to load%c", exp_layer_prop->info.layerName,
                               ending);
                    break;
                case LOADER_LAYER_LIB_ERROR_NEGOTIATE_INTERFACE_VERSION_FAILED:
                    loader_log(inst, log_flag, 0,
                               "Requested layer \"%s\" failed to negotiate a compatible interface version with layer%c",
                               exp_layer_prop->info.layerName, ending);
                    break;
                case LOADER_LAYER_LIB_ERROR_UNABLE_TO_FIND_VK_GET_INSTANCE_PROC_ADDR:
                    loader_log(inst, log_flag, 0, "Requested layer \"%s\" is missing vkGetInstanceProcAddr %c",
                               exp_layer_prop->info.layerName, ending);
                    break;
                case LOADER_LAYER_LIB_SUCCESS_LOADED:
                case LOADER_LAYER_LIB_ERROR_OUT_OF_MEMORY:
                    // Shouldn't be able to reach this but if it is, best to report a debug
                    loader_log(inst, log_flag, 0,
                               "Shouldn't reach this. A valid version of requested layer %s was loaded but was not found in the "
                               "list of activated layers%c",
                               exp_layer_prop->info.layerName, ending);
                    break;
            }
            if (app_requested) {
                res = VK_ERROR_LAYER_NOT_PRESENT;
                goto out;
            }
        }
    }

    VkLoaderFeatureFlags feature_flags = 0;
#if defined(_WIN32)
    feature_flags = windows_initialize_dxgi();
#endif

    // The following line of code is actually invalid at least according to the Vulkan spec with header update 1.2.193 and onwards.
    // The update required calls to vkGetInstanceProcAddr querying "global" functions (which includes vkCreateInstance) to pass NULL
    // for the instance parameter. Because it wasn't required to be NULL before, there may be layers which expect the loader's
    // behavior of passing a non-NULL value into vkGetInstanceProcAddr.
    // In an abundance of caution, the incorrect code remains as is, with a big comment to indicate that its wrong
    PFN_vkCreateInstance fpCreateInstance = (PFN_vkCreateInstance)next_gipa(*created_instance, "vkCreateInstance");
    if (fpCreateInstance) {
        VkLayerInstanceCreateInfo instance_dispatch;
        instance_dispatch.sType = VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO;
        instance_dispatch.pNext = loader_create_info.pNext;
        instance_dispatch.function = VK_LOADER_DATA_CALLBACK;
        instance_dispatch.u.pfnSetInstanceLoaderData = vkSetInstanceDispatch;

        VkLayerInstanceCreateInfo device_callback;
        device_callback.sType = VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO;
        device_callback.pNext = &instance_dispatch;
        device_callback.function = VK_LOADER_LAYER_CREATE_DEVICE_CALLBACK;
        device_callback.u.layerDevice.pfnLayerCreateDevice = loader_layer_create_device;
        device_callback.u.layerDevice.pfnLayerDestroyDevice = loader_layer_destroy_device;

        VkLayerInstanceCreateInfo loader_features;
        loader_features.sType = VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO;
        loader_features.pNext = &device_callback;
        loader_features.function = VK_LOADER_FEATURES;
        loader_features.u.loaderFeatures = feature_flags;

        loader_create_info.pNext = &loader_features;

        // If layer debugging is enabled, let's print out the full callstack with layers in their
        // defined order.
        loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "vkCreateInstance layer callstack setup to:");
        loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "   <Application>");
        loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "     ||");
        loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "   <Loader>");
        loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "     ||");
        for (uint32_t cur_layer = 0; cur_layer < num_activated_layers; ++cur_layer) {
            uint32_t index = num_activated_layers - cur_layer - 1;
            loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "   %s", activated_layers[index].name);
            loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "           Type: %s",
                       activated_layers[index].is_implicit ? "Implicit" : "Explicit");
            loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "           Enabled By: %s",
                       get_enabled_by_what_str(activated_layers[index].enabled_by_what));
            if (activated_layers[index].is_implicit) {
                loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "               Disable Env Var:  %s",
                           activated_layers[index].disable_env);
                if (activated_layers[index].enable_name_env) {
                    loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0,
                               "               This layer was enabled because Env Var %s was set to Value %s",
                               activated_layers[index].enable_name_env, activated_layers[index].enable_value_env);
                }
            }
            loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "           Manifest: %s", activated_layers[index].manifest);
            loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "           Library:  %s", activated_layers[index].library);
            loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "     ||");
        }
        loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "   <Drivers>");

        res = fpCreateInstance(&loader_create_info, pAllocator, created_instance);
    } else {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_create_instance_chain: Failed to find \'vkCreateInstance\'");
        // Couldn't find CreateInstance function!
        res = VK_ERROR_INITIALIZATION_FAILED;
    }

    if (res == VK_SUCCESS) {
        // Copy the current disp table into the terminator_dispatch table so we can use it in loader_gpa_instance_terminator()
        memcpy(&inst->terminator_dispatch, &inst->disp->layer_inst_disp, sizeof(VkLayerInstanceDispatchTable));

        loader_init_instance_core_dispatch_table(&inst->disp->layer_inst_disp, next_gipa, *created_instance);
        inst->instance = *created_instance;

        if (pCreateInfo->enabledLayerCount > 0 && pCreateInfo->ppEnabledLayerNames != NULL) {
            res = create_string_list(inst, pCreateInfo->enabledLayerCount, &inst->enabled_layer_names);
            if (res != VK_SUCCESS) {
                goto out;
            }

            for (uint32_t i = 0; i < pCreateInfo->enabledLayerCount; ++i) {
                res = copy_str_to_string_list(inst, &inst->enabled_layer_names, pCreateInfo->ppEnabledLayerNames[i],
                                              strlen(pCreateInfo->ppEnabledLayerNames[i]));
                if (res != VK_SUCCESS) {
                    goto out;
                }
            }
        }
    }
out:
    loader_instance_heap_free(inst, layer_instance_link_info);
    loader_instance_heap_free(inst, activated_layers);

    return res;
}

void loader_activate_instance_layer_extensions(struct loader_instance* inst, VkInstance created_inst) {
    loader_init_instance_extension_dispatch_table(&inst->disp->layer_inst_disp, inst->disp->layer_inst_disp.GetInstanceProcAddr,
                                                  created_inst);
}

#if defined(__APPLE__)
VkResult loader_create_device_chain(VkPhysicalDevice pd, const VkDeviceCreateInfo* pCreateInfo,
                                    const VkAllocationCallbacks* pAllocator, const struct loader_instance* inst,
                                    struct loader_device* dev, PFN_vkGetInstanceProcAddr callingLayer,
                                    PFN_vkGetDeviceProcAddr* layerNextGDPA, bool app_provided_phys_devs) __attribute__((optnone)) {
#else
VkResult loader_create_device_chain(VkPhysicalDevice pd, const VkDeviceCreateInfo* pCreateInfo,
                                    const VkAllocationCallbacks* pAllocator, const struct loader_instance* inst,
                                    struct loader_device* dev, PFN_vkGetInstanceProcAddr callingLayer,
                                    PFN_vkGetDeviceProcAddr* layerNextGDPA, bool app_provided_phys_devs) {
#endif
    uint32_t num_activated_layers = 0;
    struct activated_layer_info* activated_layers = NULL;
    VkLayerDeviceLink* layer_device_link_info = NULL;
    VkLayerDeviceCreateInfo chain_info = {0};
    VkDeviceCreateInfo loader_create_info = {0};
    VkDeviceGroupDeviceCreateInfo* original_device_group_create_info_struct = NULL;
    VkDeviceGroupDeviceCreateInfo* temp_struct = NULL;
    VkPhysicalDevice* phys_dev_array = NULL;
    bool modified_passed_in_pNext_chain = false;
    VkResult res = VK_SUCCESS;

    PFN_vkGetDeviceProcAddr fpGDPA = NULL;
    PFN_vkGetDeviceProcAddr nextGDPA = loader_gpa_device_terminator;
    PFN_vkGetInstanceProcAddr fpGIPA = NULL;
    PFN_vkGetInstanceProcAddr nextGIPA = loader_gpa_instance_terminator;

    memcpy(&loader_create_info, pCreateInfo, sizeof(VkDeviceCreateInfo));

    if (loader_create_info.enabledLayerCount > 0 && loader_create_info.ppEnabledLayerNames != NULL) {
        bool invalid_device_layer_usage = false;

        if ((loader_create_info.enabledLayerCount != inst->enabled_layer_names.count && loader_create_info.enabledLayerCount > 0) ||
            (loader_create_info.enabledLayerCount > 0 && loader_create_info.ppEnabledLayerNames == NULL) ||
            (loader_create_info.enabledLayerCount == 0 && loader_create_info.ppEnabledLayerNames != NULL)) {
            invalid_device_layer_usage = true;
        } else if (inst->enabled_layer_names.list != NULL) {
            for (uint32_t i = 0; i < loader_create_info.enabledLayerCount; i++) {
                const char* device_layer_names = loader_create_info.ppEnabledLayerNames[i];

                if (strcmp(device_layer_names, inst->enabled_layer_names.list[i]) != 0) {
                    invalid_device_layer_usage = true;
                    break;
                }
            }
        }

        if (invalid_device_layer_usage) {
            loader_log(
                inst, VULKAN_LOADER_WARN_BIT, 0,
                "loader_create_device_chain: Using deprecated and ignored 'ppEnabledLayerNames' member of 'VkDeviceCreateInfo' "
                "when creating a Vulkan device.");
        }
    }

    // Before we continue, we need to find out if the KHR_device_group extension is in the enabled list.  If it is, we then
    // need to look for the corresponding VkDeviceGroupDeviceCreateInfo struct in the device list.  This is because we
    // need to replace all the incoming physical device values (which are really loader trampoline physical device values)
    // with the layer/ICD version.
    // Only the handles the application passed in are trampoline objects. A layer creating a device through
    // pfnLayerCreateDevice hands back the create info it was given, whose group already holds the handles for its own
    // point in the chain, so they must be left alone - the same way its physicalDevice argument is.
    if (app_provided_phys_devs) {
        VkBaseOutStructure* pNext = (VkBaseOutStructure*)loader_create_info.pNext;
        VkBaseOutStructure* pPrev = (VkBaseOutStructure*)&loader_create_info;
        while (NULL != pNext) {
            if (VK_STRUCTURE_TYPE_DEVICE_GROUP_DEVICE_CREATE_INFO == pNext->sType) {
                VkDeviceGroupDeviceCreateInfo* cur_struct = (VkDeviceGroupDeviceCreateInfo*)pNext;
                if (0 < cur_struct->physicalDeviceCount && NULL != cur_struct->pPhysicalDevices) {
                    temp_struct = loader_calloc_with_instance_fallback(pAllocator, inst, sizeof(VkDeviceGroupDeviceCreateInfo),
                                                                       VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
                    if (NULL == temp_struct) {
                        res = VK_ERROR_OUT_OF_HOST_MEMORY;
                        goto out;
                    }
                    memcpy(temp_struct, cur_struct, sizeof(VkDeviceGroupDeviceCreateInfo));
                    phys_dev_array = (VkPhysicalDevice*)loader_calloc_with_instance_fallback(
                        pAllocator, inst, sizeof(VkPhysicalDevice) * cur_struct->physicalDeviceCount,
                        VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
                    if (NULL == phys_dev_array) {
                        res = VK_ERROR_OUT_OF_HOST_MEMORY;
                        goto out;
                    }

                    // Before calling down, replace the incoming physical device values (which are really loader trampoline
                    // physical devices) with the next layer (or possibly even the terminator) physical device values.
                    struct loader_physical_device_tramp* cur_tramp;
                    for (uint32_t phys_dev = 0; phys_dev < cur_struct->physicalDeviceCount; phys_dev++) {
                        cur_tramp = (struct loader_physical_device_tramp*)cur_struct->pPhysicalDevices[phys_dev];
                        phys_dev_array[phys_dev] = cur_tramp->phys_dev;
                    }
                    temp_struct->pPhysicalDevices = phys_dev_array;

                    original_device_group_create_info_struct = (VkDeviceGroupDeviceCreateInfo*)pPrev->pNext;

                    // Replace the old struct in the pNext chain with this one.
                    pPrev->pNext = (VkBaseOutStructure*)temp_struct;
                    modified_passed_in_pNext_chain = true;
                }
                break;
            }

            pPrev = pNext;
            pNext = pNext->pNext;
        }
    }
    if (inst->expanded_activated_layer_list.count > 0) {
        layer_device_link_info = loader_calloc_with_instance_fallback(
            pAllocator, inst, sizeof(VkLayerDeviceLink) * inst->expanded_activated_layer_list.count,
            VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (!layer_device_link_info) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_create_device_chain: Failed to alloc Device objects for layer. Skipping Layer.");
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }

        activated_layers = loader_calloc_with_instance_fallback(
            pAllocator, inst, sizeof(struct activated_layer_info) * inst->expanded_activated_layer_list.count,
            VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (!activated_layers) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_create_device_chain: Failed to alloc activated layer storage array");
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }

        chain_info.sType = VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO;
        chain_info.function = VK_LAYER_LINK_INFO;
        chain_info.u.pLayerInfo = NULL;
        chain_info.pNext = loader_create_info.pNext;
        loader_create_info.pNext = &chain_info;

        // Create instance chain of enabled layers
        for (int32_t i = (int32_t)inst->expanded_activated_layer_list.count - 1; i >= 0; i--) {
            struct loader_layer_properties* layer_prop = inst->expanded_activated_layer_list.list[i];
            loader_platform_dl_handle lib_handle = layer_prop->lib_handle;

            // Skip it if a Layer with the same name has been already successfully activated
            if (loader_names_array_has_layer_property(&layer_prop->info, num_activated_layers, activated_layers)) {
                continue;
            }

            // Skip the layer if the handle is NULL or if the lib_status isn't set to success - this is likely because the library
            // failed to load but wasn't removed from the list.
            if (!lib_handle || layer_prop->lib_status != LOADER_LAYER_LIB_SUCCESS_LOADED) {
                continue;
            }

            // The Get*ProcAddr pointers will already be filled in if they were received from either the json file or the
            // version negotiation
            fpGIPA = layer_prop->functions.get_instance_proc_addr;
            if (fpGIPA == NULL) {
                if (layer_prop->functions.str_gipa == NULL || strlen(layer_prop->functions.str_gipa) == 0) {
                    fpGIPA = (PFN_vkGetInstanceProcAddr)loader_platform_get_proc_address(lib_handle, "vkGetInstanceProcAddr");
                    layer_prop->functions.get_instance_proc_addr = fpGIPA;
                } else
                    fpGIPA =
                        (PFN_vkGetInstanceProcAddr)loader_platform_get_proc_address(lib_handle, layer_prop->functions.str_gipa);
                if (!fpGIPA) {
                    loader_log(inst, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                               "loader_create_device_chain: Failed to find \'vkGetInstanceProcAddr\' in layer \"%s\".  "
                               "Skipping layer.",
                               layer_prop->lib_name);
                    continue;
                }
            }

            if (fpGIPA == callingLayer) {
                if (layerNextGDPA != NULL) {
                    *layerNextGDPA = nextGDPA;
                }
                // Break here because if fpGIPA is the same as callingLayer, that means a layer is trying to create a device,
                // and once we don't want to continue any further as the next layer will be the calling layer
                break;
            }

            fpGDPA = layer_prop->functions.get_device_proc_addr;
            if (fpGDPA == NULL) {
                if (layer_prop->functions.str_gdpa == NULL || strlen(layer_prop->functions.str_gdpa) == 0) {
                    fpGDPA = (PFN_vkGetDeviceProcAddr)loader_platform_get_proc_address(lib_handle, "vkGetDeviceProcAddr");
                    layer_prop->functions.get_device_proc_addr = fpGDPA;
                } else
                    fpGDPA = (PFN_vkGetDeviceProcAddr)loader_platform_get_proc_address(lib_handle, layer_prop->functions.str_gdpa);
                if (!fpGDPA) {
                    loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                               "Failed to find vkGetDeviceProcAddr in layer \"%s\"", layer_prop->lib_name);
                    continue;
                }
            }

            layer_device_link_info[num_activated_layers].pNext = chain_info.u.pLayerInfo;
            layer_device_link_info[num_activated_layers].pfnNextGetInstanceProcAddr = nextGIPA;
            layer_device_link_info[num_activated_layers].pfnNextGetDeviceProcAddr = nextGDPA;
            chain_info.u.pLayerInfo = &layer_device_link_info[num_activated_layers];
            nextGIPA = fpGIPA;
            nextGDPA = fpGDPA;

            activated_layers[num_activated_layers].name = layer_prop->info.layerName;
            activated_layers[num_activated_layers].manifest = layer_prop->manifest_file_name;
            activated_layers[num_activated_layers].library = layer_prop->lib_name;
            activated_layers[num_activated_layers].is_implicit = !(layer_prop->type_flags & VK_LAYER_TYPE_FLAG_EXPLICIT_LAYER);
            activated_layers[num_activated_layers].enabled_by_what = layer_prop->enabled_by_what;
            if (activated_layers[num_activated_layers].is_implicit) {
                activated_layers[num_activated_layers].disable_env = layer_prop->disable_env_var.name;
            }

            loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_LAYER_BIT, 0, "Inserted device layer \"%s\" (%s)",
                       layer_prop->info.layerName, layer_prop->lib_name);

            num_activated_layers++;
        }
    }

    // This struct needs to live in the top level scope of the function because the cleanup routine to un-link the modified pNext
    // chain needs to access it.
    VkLayerDeviceCreateInfo create_info_disp = {0};
    create_info_disp.sType = VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO;
    create_info_disp.function = VK_LOADER_DATA_CALLBACK;
    create_info_disp.u.pfnSetDeviceLoaderData = vkSetDeviceDispatch;

    VkDevice created_device = (VkDevice)dev;
    PFN_vkCreateDevice fpCreateDevice = (PFN_vkCreateDevice)nextGIPA(inst->instance, "vkCreateDevice");

    if (fpCreateDevice) {
        // If layer debugging is enabled, let's print out the full callstack with layers in their
        // defined order.
        uint32_t layer_driver_bits = VULKAN_LOADER_LAYER_BIT | VULKAN_LOADER_DRIVER_BIT;
        loader_log(inst, layer_driver_bits, 0, "vkCreateDevice layer callstack setup to:");
        loader_log(inst, layer_driver_bits, 0, "   <Application>");
        loader_log(inst, layer_driver_bits, 0, "     ||");
        loader_log(inst, layer_driver_bits, 0, "   <Loader>");
        loader_log(inst, layer_driver_bits, 0, "     ||");
        for (uint32_t cur_layer = 0; cur_layer < num_activated_layers; ++cur_layer) {
            uint32_t index = num_activated_layers - cur_layer - 1;
            loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "   %s", activated_layers[index].name);
            loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "           Type: %s",
                       activated_layers[index].is_implicit ? "Implicit" : "Explicit");
            loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "           Enabled By: %s",
                       get_enabled_by_what_str(activated_layers[index].enabled_by_what));
            if (activated_layers[index].is_implicit) {
                loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "               Disable Env Var:  %s",
                           activated_layers[index].disable_env);
            }
            loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "           Manifest: %s", activated_layers[index].manifest);
            loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "           Library:  %s", activated_layers[index].library);
            loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "     ||");
        }
        loader_log(inst, layer_driver_bits, 0, "   <Device>");
        create_info_disp.pNext = loader_create_info.pNext;
        loader_create_info.pNext = &create_info_disp;
        res = fpCreateDevice(pd, &loader_create_info, pAllocator, &created_device);

        if (res != VK_SUCCESS) {
            goto out;
        }
        dev->chain_device = created_device;
    } else {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "loader_create_device_chain: Failed to find \'vkCreateDevice\' in layers or ICD");
        // Couldn't find CreateDevice function!
        res = VK_ERROR_INITIALIZATION_FAILED;
        goto out;
    }

    // Initialize device dispatch table
    loader_init_device_dispatch_table(&dev->loader_dispatch, nextGDPA, dev->chain_device);
    // Initialize the dispatch table to functions which need terminators
    // These functions point directly to the driver, not the terminator functions
    init_extension_device_proc_terminator_dispatch(dev);
out:
    // Because we changed the pNext chain to use our own VkDeviceGroupDeviceCreateInfo, we need to fixup the chain to
    // point back at the original VkDeviceGroupDeviceCreateInfo. This must happen even if device creation failed, otherwise
    // the caller's chain is left pointing at our copy.
    if (modified_passed_in_pNext_chain) {
        VkBaseOutStructure* pNext = (VkBaseOutStructure*)loader_create_info.pNext;
        VkBaseOutStructure* pPrev = (VkBaseOutStructure*)&loader_create_info;
        while (NULL != pNext) {
            if (VK_STRUCTURE_TYPE_DEVICE_GROUP_DEVICE_CREATE_INFO == pNext->sType) {
                VkDeviceGroupDeviceCreateInfo* cur_struct = (VkDeviceGroupDeviceCreateInfo*)pNext;
                if (0 < cur_struct->physicalDeviceCount && NULL != cur_struct->pPhysicalDevices) {
                    pPrev->pNext = (VkBaseOutStructure*)original_device_group_create_info_struct;
                }
                break;
            }

            pPrev = pNext;
            pNext = pNext->pNext;
        }
    }

    loader_free_with_instance_fallback(pAllocator, inst, temp_struct);
    loader_free_with_instance_fallback(pAllocator, inst, (void*)phys_dev_array);
    loader_free_with_instance_fallback(pAllocator, inst, layer_device_link_info);
    loader_free_with_instance_fallback(pAllocator, inst, activated_layers);

    return res;
}

VkResult loader_validate_layers(const struct loader_instance* inst, const uint32_t layer_count,
                                const char* const* ppEnabledLayerNames, const struct loader_layer_list* list) {
    struct loader_layer_properties* prop;

    if (layer_count > 0 && ppEnabledLayerNames == NULL) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "loader_validate_layers: ppEnabledLayerNames is NULL but enabledLayerCount is greater than zero");
        return VK_ERROR_LAYER_NOT_PRESENT;
    }

    for (uint32_t i = 0; i < layer_count; i++) {
        VkStringErrorFlags result = vk_string_validate(MaxLoaderStringLength, ppEnabledLayerNames[i]);
        if (result != VK_STRING_ERROR_NONE) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_validate_layers: ppEnabledLayerNames contains string that is too long or is badly formed");
            return VK_ERROR_LAYER_NOT_PRESENT;
        }

        prop = loader_find_layer_property(ppEnabledLayerNames[i], list);
        if (NULL == prop) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_validate_layers: Layer %d does not exist in the list of available layers", i);
            return VK_ERROR_LAYER_NOT_PRESENT;
        }
        if (inst->settings.settings_active && inst->settings.layer_configurations_active &&
            prop->settings_control_value != LOADER_SETTINGS_LAYER_CONTROL_ON &&
            prop->settings_control_value != LOADER_SETTINGS_LAYER_CONTROL_DEFAULT) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_validate_layers: Layer %d was explicitly prevented from being enabled by the loader settings file",
                       i);
            return VK_ERROR_LAYER_NOT_PRESENT;
        }
    }
    return VK_SUCCESS;
}

VkResult loader_validate_instance_extensions(struct loader_instance* inst, const struct loader_extension_list* icd_exts,
                                             const struct loader_layer_list* instance_layers,
                                             const struct loader_pointer_layer_list* expanded_layers,
                                             const VkInstanceCreateInfo* pCreateInfo) {
    char* env_value = loader_getenv("VK_LOADER_DISABLE_INST_EXT_FILTER", inst);
    bool check_if_known = true;
    VkResult res = VK_SUCCESS;

    if (pCreateInfo->enabledExtensionCount > 0 && pCreateInfo->ppEnabledExtensionNames == NULL) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "loader_validate_instance_extensions: Instance ppEnabledExtensionNames is NULL but enabledExtensionCount is "
                   "greater than zero");
        res = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; i++) {
        VkStringErrorFlags result = vk_string_validate(MaxLoaderStringLength, pCreateInfo->ppEnabledExtensionNames[i]);
        if (result != VK_STRING_ERROR_NONE) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_validate_instance_extensions: Instance ppEnabledExtensionNames contains "
                       "string that is too long or is badly formed");
            res = VK_ERROR_EXTENSION_NOT_PRESENT;
            goto out;
        }

        // Check if a user wants to disable the instance extension filtering behavior
        if (NULL != env_value && strtol(env_value, NULL, 10) != 0) {
            check_if_known = false;
        }

        if (check_if_known) {
            // See if the extension is in the list of supported extensions
            bool found = false;
            for (uint32_t j = 0; LOADER_INSTANCE_EXTENSIONS[j] != NULL; j++) {
                if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], LOADER_INSTANCE_EXTENSIONS[j]) == 0) {
                    found = true;
                    break;
                }
            }

            // If it isn't in the list, return an error
            if (!found) {
                loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                           "loader_validate_instance_extensions: Extension %s not found in list of known instance extensions.",
                           pCreateInfo->ppEnabledExtensionNames[i]);
                res = VK_ERROR_EXTENSION_NOT_PRESENT;
                goto out;
            }
        }

        VkExtensionProperties* extension_prop = get_extension_property(pCreateInfo->ppEnabledExtensionNames[i], icd_exts);

        if (extension_prop) {
            continue;
        }

        // Not in global list, search layer extension lists
        for (uint32_t j = 0; NULL == extension_prop && j < expanded_layers->count; ++j) {
            extension_prop =
                get_extension_property(pCreateInfo->ppEnabledExtensionNames[i], &expanded_layers->list[j]->instance_extension_list);
            if (extension_prop) {
                // Found the extension in one of the layers enabled by the app.
                break;
            }

            struct loader_layer_properties* layer_prop =
                loader_find_layer_property(expanded_layers->list[j]->info.layerName, instance_layers);
            if (NULL == layer_prop) {
                // Should NOT get here, loader_validate_layers should have already filtered this case out.
                continue;
            }
        }

        if (!extension_prop) {
            // Didn't find extension name in any of the global layers, error out
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_validate_instance_extensions: Instance extension %s not supported by available ICDs or enabled "
                       "layers.",
                       pCreateInfo->ppEnabledExtensionNames[i]);
            res = VK_ERROR_EXTENSION_NOT_PRESENT;
            goto out;
        }
    }

out:
    loader_free_getenv(env_value, inst);
    return res;
}

VkResult loader_validate_device_extensions(struct loader_instance* this_instance,
                                           const struct loader_pointer_layer_list* activated_device_layers,
                                           const struct loader_extension_list* icd_exts, const VkDeviceCreateInfo* pCreateInfo) {
    // Early out to prevent nullptr dereference
    if (pCreateInfo->enabledExtensionCount == 0 || pCreateInfo->ppEnabledExtensionNames == NULL) {
        return VK_SUCCESS;
    }
    for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; i++) {
        if (pCreateInfo->ppEnabledExtensionNames[i] == NULL) {
            continue;
        }
        VkStringErrorFlags result = vk_string_validate(MaxLoaderStringLength, pCreateInfo->ppEnabledExtensionNames[i]);
        if (result != VK_STRING_ERROR_NONE) {
            loader_log(this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_validate_device_extensions: Device ppEnabledExtensionNames contains "
                       "string that is too long or is badly formed");
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        const char* extension_name = pCreateInfo->ppEnabledExtensionNames[i];
        VkExtensionProperties* extension_prop = get_extension_property(extension_name, icd_exts);

        if (extension_prop) {
            continue;
        }

        // Not in global list, search activated layer extension lists
        for (uint32_t j = 0; j < activated_device_layers->count; j++) {
            struct loader_layer_properties* layer_prop = activated_device_layers->list[j];

            extension_prop = get_dev_extension_property(extension_name, &layer_prop->device_extension_list);
            if (extension_prop) {
                // Found the extension in one of the layers enabled by the app.
                break;
            }
        }

        if (!extension_prop) {
            // Didn't find extension name in any of the device layers, error out
            loader_log(this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_validate_device_extensions: Device extension %s not supported by selected physical device "
                       "or enabled layers.",
                       pCreateInfo->ppEnabledExtensionNames[i]);
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }
    return VK_SUCCESS;
}

// Terminator functions for the Instance chain
// All named terminator_<Vulkan API name>
VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateInstance(const VkInstanceCreateInfo* pCreateInfo,
                                                         const VkAllocationCallbacks* pAllocator, VkInstance* pInstance) {
    struct loader_icd_term* icd_term = NULL;
    VkExtensionProperties* prop = NULL;
    char** filtered_extension_names = NULL;
    VkInstanceCreateInfo icd_create_info = {0};
    VkResult res = VK_SUCCESS;
    bool one_icd_successful = false;

    struct loader_instance* ptr_instance = (struct loader_instance*)*pInstance;
    if (NULL == ptr_instance) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT, 0,
                   "terminator_CreateInstance: Loader instance pointer null encountered.  Possibly set by active layer. (Policy "
                   "#LLP_LAYER_21)");
        abort();
    } else if (LOADER_MAGIC_NUMBER != ptr_instance->magic) {
        loader_log(ptr_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "terminator_CreateInstance: Instance pointer (%p) has invalid MAGIC value 0x%08" PRIx64
                   ". Instance value possibly "
                   "corrupted by active layer (Policy #LLP_LAYER_21).  ",
                   ptr_instance, ptr_instance->magic);
        abort();
    }

    // Save the application version if it has been modified - layers sometimes needs features in newer API versions than
    // what the application requested, and thus will increase the instance version to a level that suites their needs.
    if (pCreateInfo->pApplicationInfo && pCreateInfo->pApplicationInfo->apiVersion) {
        loader_api_version altered_version = loader_make_version(pCreateInfo->pApplicationInfo->apiVersion);
        if (altered_version.major != ptr_instance->app_api_version.major ||
            altered_version.minor != ptr_instance->app_api_version.minor) {
            ptr_instance->app_api_version = altered_version;
        }
    }

    memcpy(&icd_create_info, pCreateInfo, sizeof(icd_create_info));

    icd_create_info.enabledLayerCount = 0;
    icd_create_info.ppEnabledLayerNames = NULL;

    // NOTE: Need to filter the extensions to only those supported by the ICD.
    //       No ICD will advertise support for layers. An ICD library could
    //       support a layer, but it would be independent of the actual ICD,
    //       just in the same library.
    uint32_t extension_count = pCreateInfo->enabledExtensionCount;
#if defined(LOADER_ENABLE_LINUX_SORT)
    extension_count += 1;
#endif  // LOADER_ENABLE_LINUX_SORT

    if (extension_count > 0) {
        filtered_extension_names =
            (char**)loader_instance_heap_calloc(ptr_instance, extension_count * sizeof(char*), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (!filtered_extension_names) {
            loader_log(ptr_instance, VULKAN_LOADER_ERROR_BIT, 0,
                       "terminator_CreateInstance: Failed create extension name array for %d extensions", extension_count);
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }
    }
    icd_create_info.ppEnabledExtensionNames = (const char* const*)filtered_extension_names;

    // Determine if Get Physical Device Properties 2 is available to this Instance
    if (pCreateInfo->pApplicationInfo && pCreateInfo->pApplicationInfo->apiVersion >= VK_API_VERSION_1_1) {
        ptr_instance->supports_get_dev_prop_2 = true;
    } else {
        for (uint32_t j = 0; j < pCreateInfo->enabledExtensionCount; j++) {
            if (!strcmp(pCreateInfo->ppEnabledExtensionNames[j], VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
                ptr_instance->supports_get_dev_prop_2 = true;
                break;
            }
        }
    }

    for (uint32_t i = 0; i < ptr_instance->icd_tramp_list.count; i++) {
        icd_term = loader_icd_add(ptr_instance, &ptr_instance->icd_tramp_list.scanned_list[i]);
        if (NULL == icd_term) {
            loader_log(ptr_instance, VULKAN_LOADER_ERROR_BIT, 0,
                       "terminator_CreateInstance: Failed to add ICD %d to ICD trampoline list.", i);
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }

        // If any error happens after here, we need to remove the ICD from the list,
        // because we've already added it, but haven't validated it

        // Make sure that we reset the pApplicationInfo so we don't get an old pointer
        icd_create_info.pApplicationInfo = pCreateInfo->pApplicationInfo;
        icd_create_info.enabledExtensionCount = 0;
        struct loader_extension_list icd_exts = {0};

        // traverse scanned icd list adding non-duplicate extensions to the list
        res = loader_init_generic_list(ptr_instance, (struct loader_generic_list*)&icd_exts, sizeof(VkExtensionProperties));
        if (VK_ERROR_OUT_OF_HOST_MEMORY == res) {
            // If out of memory, bail immediately.
            goto out;
        } else if (VK_SUCCESS != res) {
            // Something bad happened with this ICD, so free it and try the
            // next.
            ptr_instance->icd_terms = icd_term->next;
            icd_term->next = NULL;
            loader_icd_destroy(ptr_instance, icd_term, pAllocator);
            continue;
        }

        res = loader_add_instance_extensions(ptr_instance, icd_term->scanned_icd->EnumerateInstanceExtensionProperties,
                                             icd_term->scanned_icd->lib_name, &icd_exts);
        if (VK_SUCCESS != res) {
            loader_destroy_generic_list(ptr_instance, (struct loader_generic_list*)&icd_exts);
            if (VK_ERROR_OUT_OF_HOST_MEMORY == res) {
                // If out of memory, bail immediately.
                goto out;
            } else {
                // Something bad happened with this ICD, so free it and try the next.
                ptr_instance->icd_terms = icd_term->next;
                icd_term->next = NULL;
                loader_icd_destroy(ptr_instance, icd_term, pAllocator);
                continue;
            }
        }

        for (uint32_t j = 0; j < pCreateInfo->enabledExtensionCount; j++) {
            prop = get_extension_property(pCreateInfo->ppEnabledExtensionNames[j], &icd_exts);
            if (prop && filtered_extension_names) {
                filtered_extension_names[icd_create_info.enabledExtensionCount] = (char*)pCreateInfo->ppEnabledExtensionNames[j];
                icd_create_info.enabledExtensionCount++;
            }
        }
#if defined(LOADER_ENABLE_LINUX_SORT)
        // Force on "VK_KHR_get_physical_device_properties2" for Linux as we use it for GPU sorting.  This
        // should be done if the API version of either the application or the driver does not natively support
        // the core version of vkGetPhysicalDeviceProperties2 entrypoint.
        if ((ptr_instance->app_api_version.major == 1 && ptr_instance->app_api_version.minor == 0) ||
            (VK_API_VERSION_MAJOR(icd_term->scanned_icd->api_version) == 1 &&
             VK_API_VERSION_MINOR(icd_term->scanned_icd->api_version) == 0)) {
            prop = get_extension_property(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, &icd_exts);
            if (prop && filtered_extension_names) {
                filtered_extension_names[icd_create_info.enabledExtensionCount] =
                    (char*)VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME;
                icd_create_info.enabledExtensionCount++;

                // At least one ICD supports this, so the instance should be able to support it
                ptr_instance->supports_get_dev_prop_2 = true;
            }
        }
#endif  // LOADER_ENABLE_LINUX_SORT

        // Determine if vkGetPhysicalDeviceProperties2 is available to this Instance
        // Also determine if VK_EXT_surface_maintenance1 is available on the ICD
        if (icd_term->scanned_icd->api_version >= VK_API_VERSION_1_1) {
            icd_term->enabled_instance_extensions.khr_get_physical_device_properties2 = true;
        }
        fill_out_enabled_instance_extensions(icd_create_info.enabledExtensionCount, (const char* const*)filtered_extension_names,
                                             &icd_term->enabled_instance_extensions);

        loader_destroy_generic_list(ptr_instance, (struct loader_generic_list*)&icd_exts);

        // Get the driver version from vkEnumerateInstanceVersion
        uint32_t icd_version = VK_API_VERSION_1_0;
        VkResult icd_result = VK_SUCCESS;
        if (icd_term->scanned_icd->api_version >= VK_API_VERSION_1_1) {
            PFN_vkEnumerateInstanceVersion icd_enumerate_instance_version =
                (PFN_vkEnumerateInstanceVersion)icd_term->scanned_icd->GetInstanceProcAddr(NULL, "vkEnumerateInstanceVersion");
            if (icd_enumerate_instance_version != NULL) {
                icd_result = icd_enumerate_instance_version(&icd_version);
                if (icd_result != VK_SUCCESS) {
                    icd_version = VK_API_VERSION_1_0;
                    loader_log(ptr_instance, VULKAN_LOADER_DEBUG_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                               "terminator_CreateInstance: ICD \"%s\" vkEnumerateInstanceVersion returned error. The ICD will be "
                               "treated as a 1.0 ICD",
                               icd_term->scanned_icd->lib_name);
                } else if (VK_API_VERSION_MINOR(icd_version) == 0) {
                    loader_log(ptr_instance, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                               "terminator_CreateInstance: Manifest ICD for \"%s\" contained a 1.1 or greater API version, but "
                               "vkEnumerateInstanceVersion returned 1.0, treating as a 1.0 ICD",
                               icd_term->scanned_icd->lib_name);
                }
            } else {
                loader_log(ptr_instance, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                           "terminator_CreateInstance: Manifest ICD for \"%s\" contained a 1.1 or greater API version, but does "
                           "not support vkEnumerateInstanceVersion, treating as a 1.0 ICD",
                           icd_term->scanned_icd->lib_name);
            }
        }

        // Remove the portability enumeration flag bit if the ICD doesn't support the extension
        if ((pCreateInfo->flags & VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR) == 1) {
            bool supports_portability_enumeration = false;
            for (uint32_t j = 0; j < icd_create_info.enabledExtensionCount; j++) {
                if (filtered_extension_names &&
                    strcmp(filtered_extension_names[j], VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0) {
                    supports_portability_enumeration = true;
                    break;
                }
            }
            // If the icd supports the extension, use the flags as given, otherwise remove the portability bit
            icd_create_info.flags = supports_portability_enumeration
                                        ? pCreateInfo->flags
                                        : pCreateInfo->flags & (~VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR);
        }

        // Create an instance, substituting the version to 1.0 if necessary
        VkApplicationInfo icd_app_info = {0};
        const uint32_t api_variant = 0;
        const uint32_t api_version_1_0 = VK_API_VERSION_1_0;
        uint32_t icd_version_nopatch =
            VK_MAKE_API_VERSION(api_variant, VK_API_VERSION_MAJOR(icd_version), VK_API_VERSION_MINOR(icd_version), 0);
        uint32_t requested_version = (pCreateInfo == NULL || pCreateInfo->pApplicationInfo == NULL)
                                         ? api_version_1_0
                                         : pCreateInfo->pApplicationInfo->apiVersion;
        if ((requested_version != 0) && (icd_version_nopatch == api_version_1_0)) {
            if (icd_create_info.pApplicationInfo == NULL) {
                memset(&icd_app_info, 0, sizeof(icd_app_info));
            } else {
                memmove(&icd_app_info, icd_create_info.pApplicationInfo, sizeof(icd_app_info));
            }
            icd_app_info.apiVersion = icd_version;
            icd_create_info.pApplicationInfo = &icd_app_info;
        }

        // If the settings file has device_configurations, we need to raise the ApiVersion drivers use to 1.1 if the driver
        // supports 1.1 or higher. This allows 1.0 apps to use the device_configurations without the app having to set its own
        // ApiVersion to 1.1 on its own.
        if (ptr_instance->settings.settings_active && ptr_instance->settings.device_configurations_active &&
            ptr_instance->settings.device_configuration_count > 0 && icd_version >= VK_API_VERSION_1_1 &&
            requested_version < VK_API_VERSION_1_1) {
            if (NULL != pCreateInfo->pApplicationInfo) {
                memcpy(&icd_app_info, pCreateInfo->pApplicationInfo, sizeof(VkApplicationInfo));
            }
            icd_app_info.apiVersion = VK_API_VERSION_1_1;
            icd_create_info.pApplicationInfo = &icd_app_info;

            loader_log(
                ptr_instance, VULKAN_LOADER_INFO_BIT, 0,
                "terminator_CreateInstance: Raising the VkApplicationInfo::apiVersion from 1.0 to 1.1 on driver \"%s\" so that "
                "the loader settings file is able to use this driver in the device_configuration selection logic.",
                icd_term->scanned_icd->lib_name);
        }

        icd_result =
            ptr_instance->icd_tramp_list.scanned_list[i].CreateInstance(&icd_create_info, pAllocator, &(icd_term->instance));
        if (VK_ERROR_OUT_OF_HOST_MEMORY == icd_result) {
            // If out of memory, bail immediately.
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        } else if (VK_SUCCESS != icd_result) {
            loader_log(ptr_instance, VULKAN_LOADER_WARN_BIT, 0,
                       "terminator_CreateInstance: Received return code %i from call to vkCreateInstance in ICD %s. Skipping "
                       "this driver.",
                       icd_result, icd_term->scanned_icd->lib_name);
            ptr_instance->icd_terms = icd_term->next;
            icd_term->next = NULL;
            loader_icd_destroy(ptr_instance, icd_term, pAllocator);
            continue;
        }

        if (!loader_icd_init_entries(ptr_instance, icd_term)) {
            loader_log(ptr_instance, VULKAN_LOADER_WARN_BIT, 0,
                       "terminator_CreateInstance: Failed to find required entrypoints in ICD %s. Skipping this driver.",
                       icd_term->scanned_icd->lib_name);
            ptr_instance->icd_terms = icd_term->next;
            icd_term->next = NULL;
            loader_icd_destroy(ptr_instance, icd_term, pAllocator);
            continue;
        }

        if (ptr_instance->icd_tramp_list.scanned_list[i].interface_version < 3 &&
            (
#if defined(VK_USE_PLATFORM_XLIB_KHR)
                NULL != icd_term->dispatch.CreateXlibSurfaceKHR ||
#endif  // VK_USE_PLATFORM_XLIB_KHR
#if defined(VK_USE_PLATFORM_XCB_KHR)
                NULL != icd_term->dispatch.CreateXcbSurfaceKHR ||
#endif  // VK_USE_PLATFORM_XCB_KHR
#if defined(VK_USE_PLATFORM_WAYLAND_KHR)
                NULL != icd_term->dispatch.CreateWaylandSurfaceKHR ||
#endif  // VK_USE_PLATFORM_WAYLAND_KHR
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
                NULL != icd_term->dispatch.CreateAndroidSurfaceKHR ||
#endif  // VK_USE_PLATFORM_ANDROID_KHR
#if defined(VK_USE_PLATFORM_WIN32_KHR)
                NULL != icd_term->dispatch.CreateWin32SurfaceKHR ||
#endif  // VK_USE_PLATFORM_WIN32_KHR
                NULL != icd_term->dispatch.DestroySurfaceKHR)) {
            loader_log(ptr_instance, VULKAN_LOADER_WARN_BIT, 0,
                       "terminator_CreateInstance: Driver %s supports interface version %u but still exposes VkSurfaceKHR"
                       " create/destroy entrypoints (Policy #LDP_DRIVER_8)",
                       ptr_instance->icd_tramp_list.scanned_list[i].lib_name,
                       ptr_instance->icd_tramp_list.scanned_list[i].interface_version);
        }

        // If we made it this far, at least one ICD was successful
        one_icd_successful = true;
    }

    // For vkGetPhysicalDeviceProperties2, at least one ICD needs to support the extension for the
    // instance to have it
    if (ptr_instance->enabled_extensions.khr_get_physical_device_properties2) {
        bool at_least_one_supports = false;
        icd_term = ptr_instance->icd_terms;
        while (icd_term != NULL) {
            if (icd_term->enabled_instance_extensions.khr_get_physical_device_properties2) {
                at_least_one_supports = true;
                break;
            }
            icd_term = icd_term->next;
        }
        if (!at_least_one_supports) {
            ptr_instance->enabled_extensions.khr_get_physical_device_properties2 = false;
        }
    }

    // If no ICDs were added to instance list and res is unchanged from it's initial value, the loader was unable to
    // find a suitable ICD.
    if (VK_SUCCESS == res && (ptr_instance->icd_terms == NULL || !one_icd_successful)) {
        loader_log(ptr_instance, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "terminator_CreateInstance: Found no drivers!");
        res = VK_ERROR_INCOMPATIBLE_DRIVER;
    }

out:

    ptr_instance->create_terminator_invalid_extension = false;

    if (VK_SUCCESS != res) {
        if (VK_ERROR_EXTENSION_NOT_PRESENT == res) {
            ptr_instance->create_terminator_invalid_extension = true;
        }

        while (NULL != ptr_instance->icd_terms) {
            icd_term = ptr_instance->icd_terms;
            ptr_instance->icd_terms = icd_term->next;
            if (NULL != icd_term->instance) {
                loader_icd_close_objects(ptr_instance, icd_term);
                icd_term->dispatch.DestroyInstance(icd_term->instance, pAllocator);
            }
            loader_icd_destroy(ptr_instance, icd_term, pAllocator);
        }
    } else {
        // Check for enabled extensions here to setup the loader structures so the loader knows what extensions
        // it needs to worry about.
        // We do it here and again above the layers in the trampoline function since the trampoline function
        // may think different extensions are enabled than what's down here.
        // This is why we don't clear inside of these function calls.
        // The clearing should actually be handled by the overall memset of the pInstance structure in the
        // trampoline.
        fill_out_enabled_instance_extensions(pCreateInfo->enabledExtensionCount, pCreateInfo->ppEnabledExtensionNames,
                                             &ptr_instance->enabled_extensions);
    }

    loader_instance_heap_free(ptr_instance, (void*)filtered_extension_names);

    return res;
}

VKAPI_ATTR void VKAPI_CALL terminator_DestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator) {
    struct loader_instance* ptr_instance = loader_get_instance(instance);
    if (NULL == ptr_instance) {
        return;
    }

    // Remove this instance from the list of instances:
    struct loader_instance* prev = NULL;
    struct loader_instance* next = loader.instances;
    while (next != NULL) {
        if (next == ptr_instance) {
            // Remove this instance from the list:
            if (prev)
                prev->next = next->next;
            else
                loader.instances = next->next;
            break;
        }
        prev = next;
        next = next->next;
    }

    struct loader_icd_term* icd_terms = ptr_instance->icd_terms;
    while (NULL != icd_terms) {
        if (icd_terms->instance) {
            loader_icd_close_objects(ptr_instance, icd_terms);
            icd_terms->dispatch.DestroyInstance(icd_terms->instance, pAllocator);
        }
        struct loader_icd_term* next_icd_term = icd_terms->next;
        icd_terms->instance = VK_NULL_HANDLE;
        loader_icd_destroy(ptr_instance, icd_terms, pAllocator);

        icd_terms = next_icd_term;
    }

    loader_clear_scanned_icd_list(ptr_instance, &ptr_instance->icd_tramp_list);
    loader_destroy_generic_list(ptr_instance, (struct loader_generic_list*)&ptr_instance->ext_list);
    if (NULL != ptr_instance->phys_devs_term) {
        const uint32_t stored_count = ptr_instance->phys_dev_count_term + ptr_instance->removed_phys_dev_count_term;
        for (uint32_t i = 0; i < stored_count; i++) {
            for (uint32_t j = i + 1; j < stored_count; j++) {
                if (ptr_instance->phys_devs_term[i] == ptr_instance->phys_devs_term[j]) {
                    ptr_instance->phys_devs_term[j] = NULL;
                }
            }
        }
        for (uint32_t i = 0; i < stored_count; i++) {
            loader_instance_heap_free(ptr_instance, ptr_instance->phys_devs_term[i]);
        }
        loader_instance_heap_free(ptr_instance, (void*)ptr_instance->phys_devs_term);
    }
    if (NULL != ptr_instance->phys_dev_groups_term) {
        for (uint32_t i = 0; i < ptr_instance->phys_dev_group_count_term; i++) {
            loader_instance_heap_free(ptr_instance, ptr_instance->phys_dev_groups_term[i]);
        }
        loader_instance_heap_free(ptr_instance, (void*)ptr_instance->phys_dev_groups_term);
    }
    loader_free_dev_ext_table(ptr_instance);
    loader_free_phys_dev_ext_table(ptr_instance);

    free_string_list(ptr_instance, &ptr_instance->enabled_layer_names);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo,
                                                       const VkAllocationCallbacks* pAllocator, VkDevice* pDevice) {
    VkResult res = VK_SUCCESS;
    struct loader_physical_device_term* phys_dev_term;
    phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;

    struct loader_device* dev = (struct loader_device*)*pDevice;
    PFN_vkCreateDevice fpCreateDevice = icd_term->dispatch.CreateDevice;
    struct loader_extension_list icd_exts;

    VkBaseOutStructure* caller_dgci_container = NULL;
    VkDeviceGroupDeviceCreateInfo* caller_dgci = NULL;

    VkDeviceGroupDeviceCreateInfo* temp_struct = NULL;
    VkPhysicalDevice* phys_dev_array = NULL;
    char** filtered_extension_names = NULL;

    if (NULL == dev) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "terminator_CreateDevice: Loader device pointer null encountered.  Possibly set by active layer. (Policy "
                   "#LLP_LAYER_22)");
        abort();
    } else if (DEVICE_DISP_TABLE_MAGIC_NUMBER != dev->loader_dispatch.core_dispatch.magic) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "terminator_CreateDevice: Device pointer (%p) has invalid MAGIC value 0x%08" PRIx64
                   ". The expected value is "
                   "0x10ADED040410ADED. Device value possibly "
                   "corrupted by active layer (Policy #LLP_LAYER_22).  ",
                   dev, dev->loader_dispatch.core_dispatch.magic);
        abort();
    }

    dev->phys_dev_term = phys_dev_term;

    icd_exts.list = NULL;

    if (fpCreateDevice == NULL) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "terminator_CreateDevice: No vkCreateDevice command exposed by ICD %s", icd_term->scanned_icd->lib_name);
        res = VK_ERROR_INITIALIZATION_FAILED;
        goto out;
    }

    VkDeviceCreateInfo localCreateInfo;
    memcpy(&localCreateInfo, pCreateInfo, sizeof(localCreateInfo));

    // NOTE: Need to filter the extensions to only those supported by the ICD.
    //       No ICD will advertise support for layers. An ICD library could support a layer,
    //       but it would be independent of the actual ICD, just in the same library.
    if (0 < pCreateInfo->enabledExtensionCount) {
        filtered_extension_names = (char**)loader_calloc_with_instance_fallback(pAllocator, icd_term->this_instance,
                                                                                pCreateInfo->enabledExtensionCount * sizeof(char*),
                                                                                VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (NULL == filtered_extension_names) {
            loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                       "terminator_CreateDevice: Failed to create extension name storage for %d extensions",
                       pCreateInfo->enabledExtensionCount);
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }
    }

    localCreateInfo.enabledLayerCount = 0;
    localCreateInfo.ppEnabledLayerNames = NULL;

    localCreateInfo.enabledExtensionCount = 0;
    localCreateInfo.ppEnabledExtensionNames = (const char* const*)filtered_extension_names;

    // Get the physical device (ICD) extensions
    res = loader_init_generic_list(icd_term->this_instance, (struct loader_generic_list*)&icd_exts, sizeof(VkExtensionProperties));
    if (VK_SUCCESS != res) {
        goto out;
    }

    res = loader_add_device_extensions(icd_term->this_instance, icd_term->dispatch.EnumerateDeviceExtensionProperties,
                                       phys_dev_term->phys_dev, icd_term->scanned_icd->lib_name, &icd_exts);
    if (res != VK_SUCCESS) {
        goto out;
    }

    for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; i++) {
        if (pCreateInfo->ppEnabledExtensionNames == NULL) {
            continue;
        }
        const char* extension_name = pCreateInfo->ppEnabledExtensionNames[i];
        if (extension_name == NULL) {
            continue;
        }
        VkExtensionProperties* prop = get_extension_property(extension_name, &icd_exts);
        if (prop && filtered_extension_names) {
            filtered_extension_names[localCreateInfo.enabledExtensionCount] = (char*)extension_name;
            localCreateInfo.enabledExtensionCount++;
        } else {
            loader_log(icd_term->this_instance, VULKAN_LOADER_DEBUG_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                       "vkCreateDevice extension %s not available for devices associated with ICD %s", extension_name,
                       icd_term->scanned_icd->lib_name);
        }
    }

    // Before we continue, If KHX_device_group is the list of enabled and viable extensions, then we then need to look for the
    // corresponding VkDeviceGroupDeviceCreateInfo struct in the device list and replace all the physical device values (which
    // are really loader physical device terminator values) with the ICD versions.
    // if (icd_term->this_instance->enabled_extensions.khr_device_group_creation == 1) {
    {
        VkBaseOutStructure* pNext = (VkBaseOutStructure*)localCreateInfo.pNext;
        VkBaseOutStructure* pPrev = (VkBaseOutStructure*)&localCreateInfo;
        while (NULL != pNext) {
            if (VK_STRUCTURE_TYPE_DEVICE_GROUP_DEVICE_CREATE_INFO == pNext->sType) {
                VkDeviceGroupDeviceCreateInfo* cur_struct = (VkDeviceGroupDeviceCreateInfo*)pNext;
                if (0 < cur_struct->physicalDeviceCount && NULL != cur_struct->pPhysicalDevices) {
                    temp_struct = loader_calloc_with_instance_fallback(pAllocator, icd_term->this_instance,
                                                                       sizeof(VkDeviceGroupDeviceCreateInfo),
                                                                       VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
                    if (NULL == temp_struct) {
                        res = VK_ERROR_OUT_OF_HOST_MEMORY;
                        goto out;
                    }
                    memcpy(temp_struct, cur_struct, sizeof(VkDeviceGroupDeviceCreateInfo));
                    phys_dev_array = (VkPhysicalDevice*)loader_calloc_with_instance_fallback(
                        pAllocator, icd_term->this_instance, sizeof(VkPhysicalDevice) * cur_struct->physicalDeviceCount,
                        VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
                    if (NULL == phys_dev_array) {
                        res = VK_ERROR_OUT_OF_HOST_MEMORY;
                        goto out;
                    }

                    // Before calling down, replace the incoming physical device values (which are really loader terminator
                    // physical devices) with the ICDs physical device values.
                    struct loader_physical_device_term* cur_term;
                    for (uint32_t phys_dev = 0; phys_dev < cur_struct->physicalDeviceCount; phys_dev++) {
                        cur_term = (struct loader_physical_device_term*)cur_struct->pPhysicalDevices[phys_dev];
                        phys_dev_array[phys_dev] = cur_term->phys_dev;
                    }
                    temp_struct->pPhysicalDevices = phys_dev_array;

                    // Keep track of pointers to restore pNext chain before returning
                    caller_dgci_container = pPrev;
                    caller_dgci = cur_struct;

                    // Replace the old struct in the pNext chain with this one.
                    pPrev->pNext = (VkBaseOutStructure*)temp_struct;
                }
                break;
            }

            pPrev = pNext;
            pNext = pNext->pNext;
        }
    }

    // Handle loader emulation for structs that are not supported by the ICD:
    // Presently, the emulation leaves the pNext chain alone. This means that the ICD will receive items in the chain which
    // are not recognized by the ICD. If this causes the ICD to fail, then the items would have to be removed here. The current
    // implementation does not remove them because copying the pNext chain would be impossible if the loader does not recognize
    // the any of the struct types, as the loader would not know the size to allocate and copy.
    // if (icd_term->dispatch.GetPhysicalDeviceFeatures2 == NULL && icd_term->dispatch.GetPhysicalDeviceFeatures2KHR == NULL) {
    {
        const void* pNext = localCreateInfo.pNext;
        while (pNext != NULL) {
            VkBaseInStructure pNext_in_structure = {0};
            memcpy(&pNext_in_structure, pNext, sizeof(VkBaseInStructure));
            switch (pNext_in_structure.sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2: {
                    const VkPhysicalDeviceFeatures2KHR* features = pNext;

                    if (icd_term->dispatch.GetPhysicalDeviceFeatures2 == NULL &&
                        icd_term->dispatch.GetPhysicalDeviceFeatures2KHR == NULL) {
                        loader_log(icd_term->this_instance, VULKAN_LOADER_INFO_BIT, 0,
                                   "vkCreateDevice: Emulating handling of VkPhysicalDeviceFeatures2 in pNext chain for ICD \"%s\"",
                                   icd_term->scanned_icd->lib_name);

                        // Verify that VK_KHR_get_physical_device_properties2 is enabled
                        if (icd_term->this_instance->enabled_extensions.khr_get_physical_device_properties2) {
                            localCreateInfo.pEnabledFeatures = &features->features;
                        }
                    }

                    // Leave this item in the pNext chain for now

                    pNext = features->pNext;
                    break;
                }

                case VK_STRUCTURE_TYPE_DEVICE_GROUP_DEVICE_CREATE_INFO: {
                    const VkDeviceGroupDeviceCreateInfo* group_info = pNext;

                    if (icd_term->dispatch.EnumeratePhysicalDeviceGroups == NULL &&
                        icd_term->dispatch.EnumeratePhysicalDeviceGroupsKHR == NULL) {
                        loader_log(icd_term->this_instance, VULKAN_LOADER_INFO_BIT, 0,
                                   "vkCreateDevice: Emulating handling of VkPhysicalDeviceGroupProperties in pNext chain for "
                                   "ICD \"%s\"",
                                   icd_term->scanned_icd->lib_name);

                        // The group must contain only this one device, since physical device groups aren't actually supported
                        if (group_info->physicalDeviceCount != 1) {
                            loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                                       "vkCreateDevice: Emulation failed to create device from device group info");
                            res = VK_ERROR_INITIALIZATION_FAILED;
                            goto out;
                        }
                    }

                    // Nothing needs to be done here because we're leaving the item in the pNext chain and because the spec
                    // states that the physicalDevice argument must be included in the device group, and we've already checked
                    // that it is

                    pNext = group_info->pNext;
                    break;
                }

                // Multiview properties are also allowed, but since VK_KHX_multiview is a device extension, we'll just let the
                // ICD handle that error when the user enables the extension here
                default: {
                    pNext = pNext_in_structure.pNext;
                    break;
                }
            }
        }
    }

    VkBool32 maintenance5_feature_enabled = false;
    // Look for the VkPhysicalDeviceMaintenance5FeaturesKHR struct to see if the feature was enabled
    {
        const void* pNext = localCreateInfo.pNext;
        while (pNext != NULL) {
            VkBaseInStructure pNext_in_structure = {0};
            memcpy(&pNext_in_structure, pNext, sizeof(VkBaseInStructure));
            switch (pNext_in_structure.sType) {
                case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_FEATURES_KHR: {
                    const VkPhysicalDeviceMaintenance5FeaturesKHR* maintenance_features = pNext;
                    if (maintenance_features->maintenance5 == VK_TRUE) {
                        maintenance5_feature_enabled = true;
                    }
                    pNext = maintenance_features->pNext;
                    break;
                }

                default: {
                    pNext = pNext_in_structure.pNext;
                    break;
                }
            }
        }
    }

    // Every extension that has a loader-defined terminator needs to be marked as enabled or disabled so that we know whether or
    // not to return that terminator when vkGetDeviceProcAddr is called
    for (uint32_t i = 0; i < localCreateInfo.enabledExtensionCount; ++i) {
        if (!strcmp(localCreateInfo.ppEnabledExtensionNames[i], VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
            dev->driver_extensions.khr_swapchain_enabled = true;
        } else if (!strcmp(localCreateInfo.ppEnabledExtensionNames[i], VK_KHR_DISPLAY_SWAPCHAIN_EXTENSION_NAME)) {
            dev->driver_extensions.khr_display_swapchain_enabled = true;
        } else if (!strcmp(localCreateInfo.ppEnabledExtensionNames[i], VK_KHR_DEVICE_GROUP_EXTENSION_NAME)) {
            dev->driver_extensions.khr_device_group_enabled = true;
        } else if (!strcmp(localCreateInfo.ppEnabledExtensionNames[i], VK_EXT_DEBUG_MARKER_EXTENSION_NAME)) {
            dev->driver_extensions.ext_debug_marker_enabled = true;
#if defined(VK_USE_PLATFORM_WIN32_KHR)
        } else if (!strcmp(localCreateInfo.ppEnabledExtensionNames[i], VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME)) {
            dev->driver_extensions.ext_full_screen_exclusive_enabled = true;
#endif
        } else if (!strcmp(localCreateInfo.ppEnabledExtensionNames[i], VK_KHR_MAINTENANCE_5_EXTENSION_NAME) &&
                   maintenance5_feature_enabled) {
            dev->should_ignore_device_commands_from_newer_version = true;
        }
    }
    dev->layer_extensions.ext_debug_utils_enabled = icd_term->this_instance->enabled_extensions.ext_debug_utils;
    dev->driver_extensions.ext_debug_utils_enabled = icd_term->this_instance->enabled_extensions.ext_debug_utils;

    VkPhysicalDeviceProperties properties;
    icd_term->dispatch.GetPhysicalDeviceProperties(phys_dev_term->phys_dev, &properties);

    loader_log(icd_term->this_instance, VULKAN_LOADER_LAYER_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
               "       Using \"%s\" with driver: \"%s\"", properties.deviceName, icd_term->scanned_icd->lib_name);

    res = fpCreateDevice(phys_dev_term->phys_dev, &localCreateInfo, pAllocator, &dev->icd_device);
    if (res != VK_SUCCESS) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "terminator_CreateDevice: Failed in ICD %s vkCreateDevice call", icd_term->scanned_icd->lib_name);
        goto out;
    }

    *pDevice = dev->icd_device;
    loader_add_logical_device(icd_term, dev);

    // Init dispatch pointer in new device object
    loader_init_dispatch(*pDevice, &dev->loader_dispatch);

out:
    if (NULL != icd_exts.list) {
        loader_destroy_generic_list(icd_term->this_instance, (struct loader_generic_list*)&icd_exts);
    }

    // Restore pNext pointer to old VkDeviceGroupDeviceCreateInfo
    // in the chain to maintain consistency for the caller.
    if (caller_dgci_container != NULL) {
        caller_dgci_container->pNext = (VkBaseOutStructure*)caller_dgci;
    }

    loader_free_with_instance_fallback(pAllocator, icd_term->this_instance, (void*)filtered_extension_names);
    loader_free_with_instance_fallback(pAllocator, icd_term->this_instance, temp_struct);
    loader_free_with_instance_fallback(pAllocator, icd_term->this_instance, (void*)phys_dev_array);

    return res;
}

TEST_FUNCTION_EXPORT VkStringErrorFlags vk_string_validate(const int max_length, const char* utf8) {
    VkStringErrorFlags result = VK_STRING_ERROR_NONE;
    int num_char_bytes = 0;
    int i;
    int j;

    if (utf8 == NULL) {
        return VK_STRING_ERROR_NULL_PTR;
    }

    for (i = 0; i <= max_length; i++) {
        if (utf8[i] == 0) {
            break;
        }
        if (i == max_length) {
            result |= VK_STRING_ERROR_LENGTH;
            break;
        }
        if ((utf8[i] >= 0x20) && (utf8[i] < 0x7f)) {
            num_char_bytes = 0;
        } else if ((utf8[i] & UTF8_ONE_BYTE_MASK) == UTF8_ONE_BYTE_CODE) {
            num_char_bytes = 1;
        } else if ((utf8[i] & UTF8_TWO_BYTE_MASK) == UTF8_TWO_BYTE_CODE) {
            num_char_bytes = 2;
        } else if ((utf8[i] & UTF8_THREE_BYTE_MASK) == UTF8_THREE_BYTE_CODE) {
            num_char_bytes = 3;
        } else {
            result = VK_STRING_ERROR_BAD_DATA;
        }

        // Validate the following num_char_bytes of data
        for (j = 0; (j < num_char_bytes) && (i < max_length); j++) {
            if (++i == max_length) {
                result |= VK_STRING_ERROR_LENGTH;
                break;
            }
            if (utf8[i] == 0) {
                // The string terminated in the middle of a multi-byte character. Stop here so the
                // continuation-byte scan doesn't read past the terminator.
                result |= VK_STRING_ERROR_BAD_DATA;
                return result;
            }
            if ((utf8[i] & UTF8_DATA_BYTE_MASK) != UTF8_DATA_BYTE_CODE) {
                result |= VK_STRING_ERROR_BAD_DATA;
            }
        }
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_EnumerateInstanceVersion(uint32_t* pApiVersion) {
    // NOTE: The Vulkan WG doesn't want us checking pApiVersion for NULL, but instead
    // prefers us crashing.
    *pApiVersion = VK_HEADER_VERSION_COMPLETE;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_pre_instance_EnumerateInstanceVersion(const VkEnumerateInstanceVersionChain* chain,
                                                                                uint32_t* pApiVersion) {
    (void)chain;
    return terminator_EnumerateInstanceVersion(pApiVersion);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_EnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount,
                                                                               VkExtensionProperties* pProperties) {
    struct loader_extension_list* global_ext_list = NULL;
    struct loader_layer_list instance_layers;
    struct loader_extension_list local_ext_list;
    struct loader_icd_tramp_list icd_tramp_list;
    uint32_t copy_size;
    VkResult res = VK_SUCCESS;
    struct loader_envvar_all_filters layer_filters = {0};

    memset(&local_ext_list, 0, sizeof(local_ext_list));
    memset(&instance_layers, 0, sizeof(instance_layers));
    memset(&icd_tramp_list, 0, sizeof(icd_tramp_list));

    res = parse_layer_environment_var_filters(NULL, &layer_filters);
    if (VK_SUCCESS != res) {
        goto out;
    }

    // Get layer libraries if needed
    if (pLayerName && strlen(pLayerName) != 0) {
        if (vk_string_validate(MaxLoaderStringLength, pLayerName) != VK_STRING_ERROR_NONE) {
            assert(VK_FALSE && "vkEnumerateInstanceExtensionProperties: pLayerName is too long or is badly formed");
            res = VK_ERROR_EXTENSION_NOT_PRESENT;
            goto out;
        }

        res = loader_scan_for_layers(NULL, &instance_layers, &layer_filters);
        if (VK_SUCCESS != res) {
            goto out;
        }
        for (uint32_t i = 0; i < instance_layers.count; i++) {
            struct loader_layer_properties* props = &instance_layers.list[i];
            if (strcmp(props->info.layerName, pLayerName) == 0) {
                global_ext_list = &props->instance_extension_list;
                break;
            }
        }
    } else {
        // Preload ICD libraries so subsequent calls to EnumerateInstanceExtensionProperties don't have to load them
        loader_preload_icds();

        // Scan/discover all ICD libraries
        res = loader_icd_scan(NULL, &icd_tramp_list, NULL, NULL);
        // EnumerateInstanceExtensionProperties can't return anything other than OOM or VK_ERROR_LAYER_NOT_PRESENT
        if ((VK_SUCCESS != res && icd_tramp_list.count > 0) || res == VK_ERROR_OUT_OF_HOST_MEMORY) {
            goto out;
        }
        // Get extensions from all ICD's, merge so no duplicates
        res = loader_get_icd_loader_instance_extensions(NULL, &icd_tramp_list, &local_ext_list);
        if (VK_SUCCESS != res) {
            goto out;
        }
        loader_clear_scanned_icd_list(NULL, &icd_tramp_list);

        // Append enabled implicit layers.
        res = loader_scan_for_implicit_layers(NULL, &instance_layers, &layer_filters);
        if (VK_SUCCESS != res) {
            goto out;
        }
        for (uint32_t i = 0; i < instance_layers.count; i++) {
            struct loader_extension_list* ext_list = &instance_layers.list[i].instance_extension_list;
            res = loader_add_to_ext_list(NULL, &local_ext_list, ext_list->count, ext_list->list);
            if (VK_SUCCESS != res) {
                goto out;
            }
        }

        global_ext_list = &local_ext_list;
    }

    if (global_ext_list == NULL) {
        res = VK_ERROR_LAYER_NOT_PRESENT;
        goto out;
    }

    if (pProperties == NULL) {
        *pPropertyCount = global_ext_list->count;
        goto out;
    }

    copy_size = *pPropertyCount < global_ext_list->count ? *pPropertyCount : global_ext_list->count;
    for (uint32_t i = 0; i < copy_size; i++) {
        memcpy(&pProperties[i], &global_ext_list->list[i], sizeof(VkExtensionProperties));
    }
    *pPropertyCount = copy_size;

    if (copy_size < global_ext_list->count) {
        res = VK_INCOMPLETE;
        goto out;
    }

out:
    loader_clear_scanned_icd_list(NULL, &icd_tramp_list);
    loader_destroy_generic_list(NULL, (struct loader_generic_list*)&local_ext_list);
    loader_delete_layer_list_and_properties(NULL, &instance_layers);
    return res;
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_pre_instance_EnumerateInstanceExtensionProperties(
    const VkEnumerateInstanceExtensionPropertiesChain* chain, const char* pLayerName, uint32_t* pPropertyCount,
    VkExtensionProperties* pProperties) {
    (void)chain;
    return terminator_EnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_EnumerateInstanceLayerProperties(uint32_t* pPropertyCount,
                                                                           VkLayerProperties* pProperties) {
    VkResult result = VK_SUCCESS;
    struct loader_layer_list instance_layer_list = {0};
    struct loader_envvar_all_filters layer_filters = {0};

    LOADER_PLATFORM_THREAD_ONCE(&once_init, loader_initialize);

    result = parse_layer_environment_var_filters(NULL, &layer_filters);
    if (VK_SUCCESS != result) {
        goto out;
    }

    // Get layer libraries
    result = loader_scan_for_layers(NULL, &instance_layer_list, &layer_filters);
    if (VK_SUCCESS != result) {
        goto out;
    }

    uint32_t layers_to_write_out = 0;
    for (uint32_t i = 0; i < instance_layer_list.count; i++) {
        if (instance_layer_list.list[i].settings_control_value == LOADER_SETTINGS_LAYER_CONTROL_ON ||
            instance_layer_list.list[i].settings_control_value == LOADER_SETTINGS_LAYER_CONTROL_DEFAULT) {
            layers_to_write_out++;
        }
    }

    if (pProperties == NULL) {
        *pPropertyCount = layers_to_write_out;
        goto out;
    }

    uint32_t output_properties_index = 0;
    for (uint32_t i = 0; i < instance_layer_list.count; i++) {
        if (output_properties_index < *pPropertyCount &&
            (instance_layer_list.list[i].settings_control_value == LOADER_SETTINGS_LAYER_CONTROL_ON ||
             instance_layer_list.list[i].settings_control_value == LOADER_SETTINGS_LAYER_CONTROL_DEFAULT)) {
            memcpy(&pProperties[output_properties_index], &instance_layer_list.list[i].info, sizeof(VkLayerProperties));
            output_properties_index++;
        }
    }
    if (output_properties_index < layers_to_write_out) {
        // Indicates that we had more elements to write but ran out of room
        result = VK_INCOMPLETE;
    }

    *pPropertyCount = output_properties_index;

out:

    loader_delete_layer_list_and_properties(NULL, &instance_layer_list);
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_pre_instance_EnumerateInstanceLayerProperties(
    const VkEnumerateInstanceLayerPropertiesChain* chain, uint32_t* pPropertyCount, VkLayerProperties* pProperties) {
    (void)chain;
    return terminator_EnumerateInstanceLayerProperties(pPropertyCount, pProperties);
}
