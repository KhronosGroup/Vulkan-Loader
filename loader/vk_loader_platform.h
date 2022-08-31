/*
 *
 * Copyright (c) 2015-2022 The Khronos Group Inc.
 * Copyright (c) 2015-2022 Valve Corporation
 * Copyright (c) 2015-2022 LunarG, Inc.
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
 * Author: Ian Elliot <ian@lunarg.com>
 * Author: Jon Ashburn <jon@lunarg.com>
 * Author: Lenny Komow <lenny@lunarg.com>
 * Author: Charles Giessen <charles@lunarg.com>
 *
 */
#pragma once

#if defined(__FreeBSD__) || defined(__OpenBSD__)
#include <sys/types.h>
#include <sys/select.h>
#endif

#if defined(_WIN32)
// WinSock2.h must be included *BEFORE* windows.h
#include <winsock2.h>
#endif  // _WIN32

#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#if defined(__Fuchsia__)
#include "dlopen_fuchsia.h"
#endif  // defined(__Fuchsia__)

#if defined(__linux__) || defined(__APPLE__) || defined(__Fuchsia__) || defined(__QNXNTO__) || defined(__FreeBSD__) || defined(__OpenBSD__)
#include <unistd.h>
// Note: The following file is for dynamic loading:
#include <dlfcn.h>
#include <pthread.h>
#include <stdlib.h>
#include <libgen.h>

#elif defined(_WIN32)  // defined(__linux__)
/* Windows-specific common code: */
// WinBase.h defines CreateSemaphore and synchapi.h defines CreateEvent
//  undefine them to avoid conflicts with VkLayerDispatchTable struct members.
#ifdef CreateSemaphore
#undef CreateSemaphore
#endif
#ifdef CreateEvent
#undef CreateEvent
#endif
#include <stdio.h>
#include <io.h>
#include <shlwapi.h>
#include <direct.h>
#endif  // defined(_WIN32)

#include "stack_allocation.h"

#if defined(__GNUC__) && __GNUC__ >= 4
#define LOADER_EXPORT __attribute__((visibility("default")))
#elif defined(__SUNPRO_C) && (__SUNPRO_C >= 0x590)
#define LOADER_EXPORT __attribute__((visibility("default")))
#else
#define LOADER_EXPORT
#endif

#define MAX_STRING_SIZE 1024

// This is defined in vk_layer.h, but if there's problems we need to create the define
// here.
#ifndef MAX_NUM_UNKNOWN_EXTS
#define MAX_NUM_UNKNOWN_EXTS 250
#endif

// Environment Variable information
#define VK_ICD_FILENAMES_ENV_VAR "VK_ICD_FILENAMES"  // Deprecated
#define VK_DRIVER_FILES_ENV_VAR "VK_DRIVER_FILES"
#define VK_ADDITIONAL_DRIVER_FILES_ENV_VAR "VK_ADD_DRIVER_FILES"
#define VK_LAYER_PATH_ENV_VAR "VK_LAYER_PATH"
#define VK_ADDITIONAL_LAYER_PATH_ENV_VAR "VK_ADD_LAYER_PATH"

// Override layer information
#define VK_OVERRIDE_LAYER_NAME "VK_LAYER_LUNARG_override"

#define LAYERS_PATH_ENV "VK_LAYER_PATH"
#define ENABLED_LAYERS_ENV "VK_INSTANCE_LAYERS"

#if defined(__linux__) || defined(__APPLE__) || defined(__Fuchsia__) || defined(__QNXNTO__) || defined(__FreeBSD__) || defined(__OpenBSD__)
/* Linux-specific common code: */

// VK Library Filenames, Paths, etc.:
#define PATH_SEPARATOR ':'
#define DIRECTORY_SYMBOL '/'

#define VULKAN_DIR "vulkan/"
#define VULKAN_ICDCONF_DIR "icd.d"
#define VULKAN_ICD_DIR "icd"
#define VULKAN_SETTINGSCONF_DIR "settings.d"
#define VULKAN_ELAYERCONF_DIR "explicit_layer.d"
#define VULKAN_ILAYERCONF_DIR "implicit_layer.d"
#define VULKAN_LAYER_DIR "layer"

#define VK_DRIVERS_INFO_RELATIVE_DIR VULKAN_DIR VULKAN_ICDCONF_DIR
#define VK_SETTINGS_INFO_RELATIVE_DIR VULKAN_DIR VULKAN_SETTINGSCONF_DIR
#define VK_ELAYERS_INFO_RELATIVE_DIR VULKAN_DIR VULKAN_ELAYERCONF_DIR
#define VK_ILAYERS_INFO_RELATIVE_DIR VULKAN_DIR VULKAN_ILAYERCONF_DIR

#define VK_DRIVERS_INFO_REGISTRY_LOC ""
#define VK_SETTINGS_INFO_REGISTRY_LOC ""
#define VK_ELAYERS_INFO_REGISTRY_LOC ""
#define VK_ILAYERS_INFO_REGISTRY_LOC ""

#if defined(__QNXNTO__)
#define SYSCONFDIR "/etc"
#endif

// C99:
#define PRINTF_SIZE_T_SPECIFIER "%zu"

// Dynamic Loading of libraries:
typedef void *loader_platform_dl_handle;

// Threads:
typedef pthread_t loader_platform_thread;

// Thread IDs:
typedef pthread_t loader_platform_thread_id;

// Thread mutex:
typedef pthread_mutex_t loader_platform_thread_mutex;

typedef pthread_cond_t loader_platform_thread_cond;

#elif defined(_WIN32)  // defined(__linux__)

// VK Library Filenames, Paths, etc.:
#define PATH_SEPARATOR ';'
#define DIRECTORY_SYMBOL '\\'
#define DEFAULT_VK_REGISTRY_HIVE HKEY_LOCAL_MACHINE
#define DEFAULT_VK_REGISTRY_HIVE_STR "HKEY_LOCAL_MACHINE"
#define SECONDARY_VK_REGISTRY_HIVE HKEY_CURRENT_USER
#define SECONDARY_VK_REGISTRY_HIVE_STR "HKEY_CURRENT_USER"

#define VK_DRIVERS_INFO_RELATIVE_DIR ""
#define VK_SETTINGS_INFO_RELATIVE_DIR ""
#define VK_ELAYERS_INFO_RELATIVE_DIR ""
#define VK_ILAYERS_INFO_RELATIVE_DIR ""

#ifdef _WIN64
#define HKR_VK_DRIVER_NAME API_NAME "DriverName"
#else
#define HKR_VK_DRIVER_NAME API_NAME "DriverNameWow"
#endif
#define VK_DRIVERS_INFO_REGISTRY_LOC "SOFTWARE\\Khronos\\" API_NAME "\\Drivers"
#define VK_SETTINGS_INFO_REGISTRY_LOC "SOFTWARE\\Khronos\\" API_NAME "\\Settings"
#define VK_ELAYERS_INFO_REGISTRY_LOC "SOFTWARE\\Khronos\\" API_NAME "\\ExplicitLayers"
#define VK_ILAYERS_INFO_REGISTRY_LOC "SOFTWARE\\Khronos\\" API_NAME "\\ImplicitLayers"

#define PRINTF_SIZE_T_SPECIFIER "%Iu"

// Dynamic Loading:
typedef HMODULE loader_platform_dl_handle;

// Threads:
typedef HANDLE loader_platform_thread;

// Thread IDs:
typedef DWORD loader_platform_thread_id;

// Thread mutex:
typedef CRITICAL_SECTION loader_platform_thread_mutex;

typedef CONDITION_VARIABLE loader_platform_thread_cond;

#else  // defined(_WIN32)

#error The "vk_loader_platform.h" file must be modified for this OS.

// NOTE: In order to support another OS, an #elif needs to be added (above the
// "#else // defined(_WIN32)") for that OS, and OS-specific versions of the
// contents of this file must be created, or extend one of the existing OS specific
// sections with the necessary changes.

#endif  // defined(_WIN32)

// Returns true if the DIRECTORY_SYMBOL is contained within path
static inline bool loader_platform_is_path(const char *path) { return strchr(path, DIRECTORY_SYMBOL) != NULL; }

// The once init functionality is not used when building a DLL on Windows. This is because there is no way to clean up the
// resources allocated by anything allocated by once init. This isn't a problem for static libraries, but it is for dynamic
// ones. When building a DLL, we use DllMain() instead to allow properly cleaning up resources.

#if defined(__APPLE__) && defined(BUILD_STATIC_LOADER)
static inline void loader_platform_thread_once_fn(pthread_once_t *ctl, void (*func)(void)) {
    assert(func != NULL);
    assert(ctl != NULL);
    pthread_once(ctl, func);
}
#define LOADER_PLATFORM_THREAD_ONCE_DECLARATION(var) pthread_once_t var = PTHREAD_ONCE_INIT;
#define LOADER_PLATFORM_THREAD_ONCE_EXTERN_DEFINITION(var) extern pthread_once_t var;
#define LOADER_PLATFORM_THREAD_ONCE(ctl, func) loader_platform_thread_once_fn(ctl, func);
#else
#define LOADER_PLATFORM_THREAD_ONCE_DECLARATION(var)
#define LOADER_PLATFORM_THREAD_ONCE_EXTERN_DEFINITION(var)
#define LOADER_PLATFORM_THREAD_ONCE(ctl, func)

#endif

#if defined(__linux__) || defined(__APPLE__) || defined(__Fuchsia__) || defined(__QNXNTO__) || defined(__FreeBSD__) || defined(__OpenBSD__)

// File IO
static inline bool loader_platform_file_exists(const char *path) {
    if (access(path, F_OK))
        return false;
    else
        return true;
}

// Returns true if the given string appears to be a relative or absolute
// path, as opposed to a bare filename.
static inline bool loader_platform_is_path_absolute(const char *path) {
    if (path[0] == '/')
        return true;
    else
        return false;
}

static inline char *loader_platform_dirname(char *path) { return dirname(path); }

// loader_platform_executable_path finds application path + name.
// Path cannot be longer than 1024, returns NULL if it is greater than that.
#if defined(__linux__)
static inline char *loader_platform_executable_path(char *buffer, size_t size) {
    ssize_t count = readlink("/proc/self/exe", buffer, size);
    if (count == -1) return NULL;
    if (count == 0) return NULL;
    buffer[count] = '\0';
    return buffer;
}
#elif defined(__APPLE__)  // defined(__linux__)
#include <libproc.h>
static inline char *loader_platform_executable_path(char *buffer, size_t size) {
    pid_t pid = getpid();
    int ret = proc_pidpath(pid, buffer, size);
    if (ret <= 0) return NULL;
    buffer[ret] = '\0';
    return buffer;
}
#elif defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__)
#include <sys/sysctl.h>
static inline char *loader_platform_executable_path(char *buffer, size_t size) {
    int mib[] = {
        CTL_KERN,
#if defined(__NetBSD__)
        KERN_PROC_ARGS,
        -1,
        KERN_PROC_PATHNAME,
#else
        KERN_PROC,
        KERN_PROC_PATHNAME,
        -1,
#endif
    };
    if (sysctl(mib, sizeof(mib) / sizeof(mib[0]), buffer, &size, NULL, 0) < 0) {
        return NULL;
    }

    return buffer;
}
#elif defined(__Fuchsia__) || defined(__OpenBSD__)
static inline char *loader_platform_executable_path(char *buffer, size_t size) { return NULL; }
#elif defined(__QNXNTO__)

#define SYSCONFDIR "/etc"

#include <fcntl.h>
#include <sys/stat.h>

static inline char *loader_platform_executable_path(char *buffer, size_t size) {
    int fd = open("/proc/self/exefile", O_RDONLY);
    size_t rdsize;

    if (fd == -1) {
        return NULL;
    }

    rdsize = read(fd, buffer, size);
    if (rdsize == size) {
        return NULL;
    }
    buffer[rdsize] = 0x00;
    close(fd);

    return buffer;
}
#endif  // defined (__QNXNTO__)

// Compatability with compilers that don't support __has_feature
#ifndef __has_feature
#define __has_feature(x) 0
#endif

#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
#define LOADER_ADDRESS_SANITIZER  // TODO: Add proper build flag for ASAN support
#endif

// When loading the library, we use RTLD_LAZY so that not all symbols have to be
// resolved at this time (which improves performance). Note that if not all symbols
// can be resolved, this could cause crashes later. Use the LD_BIND_NOW environment
// variable to force all symbols to be resolved here.
#define LOADER_DLOPEN_MODE (RTLD_LAZY | RTLD_LOCAL)

#if defined(__Fuchsia__)
static inline loader_platform_dl_handle loader_platform_open_driver(const char *libPath) {
    return dlopen_fuchsia(libPath, LOADER_DLOPEN_MODE, true);
}
static inline loader_platform_dl_handle loader_platform_open_library(const char *libPath) {
    return dlopen_fuchsia(libPath, LOADER_DLOPEN_MODE, false);
}
#else
static inline loader_platform_dl_handle loader_platform_open_library(const char *libPath) {
    return dlopen(libPath, LOADER_DLOPEN_MODE);
}
#endif

static inline const char *loader_platform_open_library_error(const char *libPath) {
#ifdef __Fuchsia__
    return dlerror_fuchsia();
#else
    return dlerror();
#endif
}
static inline void loader_platform_close_library(loader_platform_dl_handle library) { dlclose(library); }
static inline void *loader_platform_get_proc_address(loader_platform_dl_handle library, const char *name) {
    assert(library);
    assert(name);
    return dlsym(library, name);
}
static inline const char *loader_platform_get_proc_address_error(const char *name) { return dlerror(); }

// Thread mutex:
static inline void loader_platform_thread_create_mutex(loader_platform_thread_mutex *pMutex) { pthread_mutex_init(pMutex, NULL); }
static inline void loader_platform_thread_lock_mutex(loader_platform_thread_mutex *pMutex) { pthread_mutex_lock(pMutex); }
static inline void loader_platform_thread_unlock_mutex(loader_platform_thread_mutex *pMutex) { pthread_mutex_unlock(pMutex); }
static inline void loader_platform_thread_delete_mutex(loader_platform_thread_mutex *pMutex) { pthread_mutex_destroy(pMutex); }

#elif defined(_WIN32)  // defined(__linux__)

// Get the key for the plug n play driver registry
// The string returned by this function should NOT be freed
static inline const char *LoaderPnpDriverRegistry() {
    BOOL is_wow;
    IsWow64Process(GetCurrentProcess(), &is_wow);
    return is_wow ? "VulkanDriverNameWow" : "VulkanDriverName";
}
static inline const wchar_t *LoaderPnpDriverRegistryWide() {
    BOOL is_wow;
    IsWow64Process(GetCurrentProcess(), &is_wow);
    return is_wow ? L"VulkanDriverNameWow" : L"VulkanDriverName";
}

// Get the key for the plug 'n play explicit layer registry
// The string returned by this function should NOT be freed
static inline const char *LoaderPnpELayerRegistry() {
    BOOL is_wow;
    IsWow64Process(GetCurrentProcess(), &is_wow);
    return is_wow ? "VulkanExplicitLayersWow" : "VulkanExplicitLayers";
}
static inline const wchar_t *LoaderPnpELayerRegistryWide() {
    BOOL is_wow;
    IsWow64Process(GetCurrentProcess(), &is_wow);
    return is_wow ? L"VulkanExplicitLayersWow" : L"VulkanExplicitLayers";
}

// Get the key for the plug 'n play implicit layer registry
// The string returned by this function should NOT be freed
static inline const char *LoaderPnpILayerRegistry() {
    BOOL is_wow;
    IsWow64Process(GetCurrentProcess(), &is_wow);
    return is_wow ? "VulkanImplicitLayersWow" : "VulkanImplicitLayers";
}
static inline const wchar_t *LoaderPnpILayerRegistryWide() {
    BOOL is_wow;
    IsWow64Process(GetCurrentProcess(), &is_wow);
    return is_wow ? L"VulkanImplicitLayersWow" : L"VulkanImplicitLayers";
}

// File IO
static bool loader_platform_file_exists(const char *path) {
    int path_utf16_size = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
    if (path_utf16_size <= 0) {
        return false;
    }
    wchar_t *path_utf16 = (wchar_t *)loader_stack_alloc(path_utf16_size * sizeof(wchar_t));
    if (MultiByteToWideChar(CP_UTF8, 0, path, -1, path_utf16, path_utf16_size) != path_utf16_size) {
        return false;
    }
    if (_waccess(path_utf16, 0) == -1)
        return false;
    else
        return true;
}

// Returns true if the given string appears to be a relative or absolute
// path, as opposed to a bare filename.
static bool loader_platform_is_path_absolute(const char *path) {
    if (!path || !*path) {
        return false;
    }
    if (*path == DIRECTORY_SYMBOL || path[1] == ':') {
        return true;
    }
    return false;
}

// WIN32 runtime doesn't have dirname().
static inline char *loader_platform_dirname(char *path) {
    char *current, *next;

    // TODO/TBD: Do we need to deal with the Windows's ":" character?

    for (current = path; *current != '\0'; current = next) {
        next = strchr(current, DIRECTORY_SYMBOL);
        if (next == NULL) {
            if (current != path) *(current - 1) = '\0';
            return path;
        } else {
            // Point one character past the DIRECTORY_SYMBOL:
            next++;
        }
    }
    return path;
}

static inline char *loader_platform_executable_path(char *buffer, size_t size) {
    wchar_t *buffer_utf16 = (wchar_t *)loader_stack_alloc(size * sizeof(wchar_t));
    DWORD ret = GetModuleFileNameW(NULL, buffer_utf16, (DWORD)size);
    if (ret == 0) {
        return NULL;
    }
    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        return NULL;
    }
    int buffer_utf8_size = WideCharToMultiByte(CP_UTF8, 0, buffer_utf16, -1, NULL, 0, NULL, NULL);
    if (buffer_utf8_size <= 0 || (size_t)buffer_utf8_size > size) {
        return NULL;
    }
    if (WideCharToMultiByte(CP_UTF8, 0, buffer_utf16, -1, buffer, buffer_utf8_size, NULL, NULL) != buffer_utf8_size) {
        return NULL;
    }
    return buffer;
}

// Dynamic Loading:
static loader_platform_dl_handle loader_platform_open_library(const char *lib_path) {
    int lib_path_utf16_size = MultiByteToWideChar(CP_UTF8, 0, lib_path, -1, NULL, 0);
    if (lib_path_utf16_size <= 0) {
        return NULL;
    }
    wchar_t *lib_path_utf16 = (wchar_t *)loader_stack_alloc(lib_path_utf16_size * sizeof(wchar_t));
    if (MultiByteToWideChar(CP_UTF8, 0, lib_path, -1, lib_path_utf16, lib_path_utf16_size) != lib_path_utf16_size) {
        return NULL;
    }
    // Try loading the library the original way first.
    loader_platform_dl_handle lib_handle = LoadLibraryW(lib_path_utf16);
    if (lib_handle == NULL && GetLastError() == ERROR_MOD_NOT_FOUND) {
        // If that failed, then try loading it with broader search folders.
        lib_handle = LoadLibraryExW(lib_path_utf16, NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);
    }
    return lib_handle;
}
static const char *loader_platform_open_library_error(const char *libPath) {
    static char errorMsg[512];
    (void)snprintf(errorMsg, 511, "Failed to open dynamic library \"%s\" with error %lu", libPath, GetLastError());
    return errorMsg;
}
static void loader_platform_close_library(loader_platform_dl_handle library) { FreeLibrary(library); }
static void *loader_platform_get_proc_address(loader_platform_dl_handle library, const char *name) {
    assert(library);
    assert(name);
    return (void *)GetProcAddress(library, name);
}
static const char *loader_platform_get_proc_address_error(const char *name) {
    static char errorMsg[120];
    (void)snprintf(errorMsg, 119, "Failed to find function \"%s\" in dynamic library", name);
    return errorMsg;
}

// Thread mutex:
static void loader_platform_thread_create_mutex(loader_platform_thread_mutex *pMutex) { InitializeCriticalSection(pMutex); }
static void loader_platform_thread_lock_mutex(loader_platform_thread_mutex *pMutex) { EnterCriticalSection(pMutex); }
static void loader_platform_thread_unlock_mutex(loader_platform_thread_mutex *pMutex) { LeaveCriticalSection(pMutex); }
static void loader_platform_thread_delete_mutex(loader_platform_thread_mutex *pMutex) { DeleteCriticalSection(pMutex); }

#else  // defined(_WIN32)

#error The "vk_loader_platform.h" file must be modified for this OS.

// NOTE: In order to support another OS, an #elif needs to be added (above the
// "#else // defined(_WIN32)") for that OS, and OS-specific versions of the
// contents of this file must be created.

// NOTE: Other OS-specific changes are also needed for this OS.  Search for
// files with "WIN32" in it, as a quick way to find files that must be changed.

#endif  // defined(_WIN32)
