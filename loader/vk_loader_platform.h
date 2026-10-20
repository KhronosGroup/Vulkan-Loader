/*
 *
 * Copyright (c) 2015-2026 The Khronos Group Inc.
 * Copyright (c) 2015-2026 Valve Corporation
 * Copyright (c) 2015-2026 LunarG, Inc.
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

#include <assert.h>
#include <float.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if defined(__Fuchsia__)
#include "dlopen_fuchsia.h"
#endif  // defined(__Fuchsia__)

// Set of platforms with a common set of functionality which is queried throughout the program
#if defined(__linux__) || defined(__APPLE__) || defined(__Fuchsia__) || defined(__QNX__) || defined(__FreeBSD__) || \
    defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__) || defined(__GNU__) || defined(__CYGWIN__)
#define COMMON_UNIX_PLATFORMS 1
#else
#define COMMON_UNIX_PLATFORMS 0
#endif

#if COMMON_UNIX_PLATFORMS
#include <unistd.h>
// Note: The following file is for dynamic loading:
#include <dlfcn.h>
#include <pthread.h>
#include <stdlib.h>
#include <libgen.h>

#elif defined(_WIN32)
// WinBase.h defines CreateSemaphore and synchapi.h defines CreateEvent
//  undefine them to avoid conflicts with VkLayerDispatchTable struct members.
#if defined(CreateSemaphore)
#undef CreateSemaphore
#endif
#if defined(CreateEvent)
#undef CreateEvent
#endif
#include <stdio.h>
#include <io.h>
#include <shlwapi.h>
#include <direct.h>

#endif  // defined(_WIN32)

#if defined(APPLE_STATIC_LOADER) && !defined(__APPLE__)
#error "APPLE_STATIC_LOADER can only be defined on Apple platforms!"
#endif

#if defined(APPLE_STATIC_LOADER)
#define LOADER_EXPORT
#elif defined(__GNUC__) && __GNUC__ >= 4
#define LOADER_EXPORT __attribute__((visibility("default")))
#elif defined(__SUNPRO_C) && (__SUNPRO_C >= 0x590)
#define LOADER_EXPORT __attribute__((visibility("default")))
#else
#define LOADER_EXPORT
#endif

// For testing purposes, we want to expose some functions not normally callable on the library
#if defined(SHOULD_EXPORT_TEST_FUNCTIONS)
#if defined(_WIN32)
#define TEST_FUNCTION_EXPORT __declspec(dllexport)
#else
#define TEST_FUNCTION_EXPORT LOADER_EXPORT
#endif
#else
#define TEST_FUNCTION_EXPORT
#endif

#define MAX_STRING_SIZE 1024

// This is defined in vk_layer.h, but if there's problems we need to create the define
// here.
#if !defined(MAX_NUM_UNKNOWN_EXTS)
#define MAX_NUM_UNKNOWN_EXTS 250
#endif

// Environment Variable information
#define VK_ICD_FILENAMES_ENV_VAR "VK_ICD_FILENAMES"  // Deprecated in v1.3.207 loader
#define VK_DRIVER_FILES_ENV_VAR "VK_DRIVER_FILES"
#define VK_EXPLICIT_LAYER_PATH_ENV_VAR "VK_LAYER_PATH"
// Support added in v1.3.207 loader
#define VK_ADDITIONAL_DRIVER_FILES_ENV_VAR "VK_ADD_DRIVER_FILES"
#define VK_ADDITIONAL_EXPLICIT_LAYER_PATH_ENV_VAR "VK_ADD_LAYER_PATH"
// Support added in v1.3.234 loader
#define VK_LAYERS_ENABLE_ENV_VAR "VK_LOADER_LAYERS_ENABLE"
#define VK_LAYERS_DISABLE_ENV_VAR "VK_LOADER_LAYERS_DISABLE"
#define VK_LAYERS_ALLOW_ENV_VAR "VK_LOADER_LAYERS_ALLOW"
#define VK_DRIVERS_SELECT_ENV_VAR "VK_LOADER_DRIVERS_SELECT"
#define VK_DRIVERS_DISABLE_ENV_VAR "VK_LOADER_DRIVERS_DISABLE"
#define VK_LOADER_DISABLE_ALL_LAYERS_VAR_1 "~all~"
#define VK_LOADER_DISABLE_ALL_LAYERS_VAR_2 "*"
#define VK_LOADER_DISABLE_ALL_LAYERS_VAR_3 "**"
#define VK_LOADER_DISABLE_IMPLICIT_LAYERS_VAR "~implicit~"
#define VK_LOADER_DISABLE_EXPLICIT_LAYERS_VAR "~explicit~"
// Support added in v1.3.295 loader
#define VK_IMPLICIT_LAYER_PATH_ENV_VAR "VK_IMPLICIT_LAYER_PATH"
#define VK_ADDITIONAL_IMPLICIT_LAYER_PATH_ENV_VAR "VK_ADD_IMPLICIT_LAYER_PATH"

#define VK_DEVICE_ID_FILTER_ENV_VAR "VK_LOADER_DEVICE_ID_FILTER"
#define VK_VENDOR_ID_FILTER_ENV_VAR "VK_LOADER_VENDOR_ID_FILTER"
#define VK_DRIVER_ID_FILTER_ENV_VAR "VK_LOADER_DRIVER_ID_FILTER"

#if defined(__APPLE__)
#define VK_LOADER_SEARCH_ONLY_IN_BUNDLE_ENV_VAR "VK_LOADER_SEARCH_ONLY_IN_BUNDLE"
#endif

// Override layer information
#define VK_OVERRIDE_LAYER_NAME "VK_LAYER_LUNARG_override"

// Loader Settings filename
#define VK_LOADER_SETTINGS_FILENAME "vk_loader_settings.json"

#define LAYERS_PATH_ENV "VK_LAYER_PATH"
#define ENABLED_LAYERS_ENV "VK_INSTANCE_LAYERS"

#if COMMON_UNIX_PLATFORMS
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
#define VK_ELAYERS_INFO_REGISTRY_LOC ""
#define VK_ILAYERS_INFO_REGISTRY_LOC ""
#define VK_SETTINGS_INFO_REGISTRY_LOC ""

#if defined(__QNX__)
#ifndef SYSCONFDIR
#define SYSCONFDIR "/etc"
#endif
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

#elif defined(_WIN32)
/* Windows-specific common code: */
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

#define VK_VARIANT_REG_STR ""
#define VK_VARIANT_REG_STR_W L""

#define VK_DRIVERS_INFO_REGISTRY_LOC "SOFTWARE\\Khronos\\Vulkan" VK_VARIANT_REG_STR "\\Drivers"
#define VK_ELAYERS_INFO_REGISTRY_LOC "SOFTWARE\\Khronos\\Vulkan" VK_VARIANT_REG_STR "\\ExplicitLayers"
#define VK_ILAYERS_INFO_REGISTRY_LOC "SOFTWARE\\Khronos\\Vulkan" VK_VARIANT_REG_STR "\\ImplicitLayers"
#define VK_SETTINGS_INFO_REGISTRY_LOC "SOFTWARE\\Khronos\\Vulkan" VK_VARIANT_REG_STR "\\LoaderSettings"

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

#else

#warning The "vk_loader_platform.h" file must be modified for this OS.

// NOTE: In order to support another OS, an #elif needs to be added (above the
// "#else // defined(_WIN32)") for that OS, and OS-specific versions of the
// contents of this file must be created, or extend one of the existing OS specific
// sections with the necessary changes.

#endif

// controls whether loader_platform_close_library() closes the libraries or not - controlled by an environment variables
extern bool loader_disable_dynamic_library_unloading;

// Returns true if the DIRECTORY_SYMBOL is contained within path
static inline bool loader_platform_is_path(const char *path) { return strchr(path, DIRECTORY_SYMBOL) != NULL; }

// The loader has various initialization tasks which it must do before user code can run. This includes initializing synchronization
// objects, determining the log level, writing the version of the loader to the log, and loading dll's (on windows). On linux, the
// solution is simply running the initialization code in  __attribute__((constructor)), which MacOS uses when the loader is
// dynamically linked. When statically linking on MacOS, the setup code instead uses pthread_once to run the logic a single time
// regardless of which API function the application calls first. On Windows, the equivalent way to run code at dll load time is
// DllMain which has many limitations placed upon it. Instead, the Windows code follows MacOS and does initialization in the first
// API call made, using InitOnceExecuteOnce, except for initialization primitives which must be done in DllMain. This is because
// there is no way to clean up the resources allocated by anything allocated by once init.

#if defined(APPLE_STATIC_LOADER)
static inline void loader_platform_thread_once_fn(pthread_once_t *ctl, void (*func)(void)) {
    assert(func != NULL);
    assert(ctl != NULL);
    pthread_once(ctl, func);
}
#define LOADER_PLATFORM_THREAD_ONCE_DECLARATION(var) pthread_once_t var = PTHREAD_ONCE_INIT;
#define LOADER_PLATFORM_THREAD_ONCE_EXTERN_DEFINITION(var) extern pthread_once_t var;
#define LOADER_PLATFORM_THREAD_ONCE(ctl, func) loader_platform_thread_once_fn(ctl, func);
#elif defined(WIN32)
static inline void loader_platform_thread_win32_once_fn(INIT_ONCE *ctl, PINIT_ONCE_FN func) {
    InitOnceExecuteOnce(ctl, func, NULL, NULL);
}
#define LOADER_PLATFORM_THREAD_ONCE_DECLARATION(var) INIT_ONCE var = INIT_ONCE_STATIC_INIT;
#define LOADER_PLATFORM_THREAD_ONCE_EXTERN_DEFINITION(var) extern INIT_ONCE var;
#define LOADER_PLATFORM_THREAD_ONCE(ctl, func) loader_platform_thread_win32_once_fn(ctl, func);
#else
#define LOADER_PLATFORM_THREAD_ONCE_DECLARATION(var)
#define LOADER_PLATFORM_THREAD_ONCE_EXTERN_DEFINITION(var)
#define LOADER_PLATFORM_THREAD_ONCE(ctl, func)

#endif

struct loader_instance;

// File IO
bool loader_platform_file_exists(const struct loader_instance *inst, const char *path, bool *oom);
// Returns true if the given string appears to be a relative or absolute
// path, as opposed to a bare filename.
bool loader_platform_is_path_absolute(const char *path);
// NOLINTNEXTLINE(concurrency-mt-unsafe) - no portable reentrant dirname; the returned buffer is copied by callers immediately
char *loader_platform_dirname(char *path);
// loader_platform_executable_path finds application path + name.
// Allocates memory to store the path and returns it - caller is responsible for freeing
// Sets oom to true if allocation failed. If it didn't fail allocating but didn't succeed getting the exe path, it returns NULL
char *loader_platform_executable_path(const struct loader_instance *inst, bool *oom);

#if defined(__Fuchsia__)
loader_platform_dl_handle loader_platform_open_driver(const char *libPath);
#endif
loader_platform_dl_handle loader_platform_open_library(const struct loader_instance *inst, const char *libPath, bool *oom);

const char *loader_platform_open_library_error(const char *libPath);
void loader_platform_close_library(loader_platform_dl_handle library);
void *loader_platform_get_proc_address(loader_platform_dl_handle library, const char *name);
const char *loader_platform_get_proc_address_error(const char *name);

// Thread mutex:
void loader_platform_thread_create_mutex(loader_platform_thread_mutex *pMutex);
void loader_platform_thread_lock_mutex(loader_platform_thread_mutex *pMutex);
void loader_platform_thread_unlock_mutex(loader_platform_thread_mutex *pMutex);
void loader_platform_thread_delete_mutex(loader_platform_thread_mutex *pMutex);

void *thread_safe_strtok(char *str, const char *delim, char **saveptr);
FILE *loader_fopen(const char *fileName, const char *mode);
char *loader_strncat(char *dest, size_t dest_sz, const char *src, size_t count);
char *loader_strncpy(char *dest, size_t dest_sz, const char *src, size_t count);

#if defined(_WIN32)

// Converts a wchar_t string to multi byte character string (utf-8)
// oom is set to true if allocation failed
// scope must be a valid VkSystemAllocationScope enum value - uint32_t is used to avoid needing to include vulkan.h
char *loader_convert_wchar_to_char(const struct loader_instance *inst, const wchar_t *str_utf16, bool *oom, uint32_t scope);

// Converts a multi byte character string (utf-8) string to  wchar_t string
// oom is set to true if allocation failed
// scope must be a valid VkSystemAllocationScope enum value - uint32_t is used to avoid needing to include vulkan.h
wchar_t *loader_convert_char_to_wchar(const struct loader_instance *inst, const char *str_utf8, bool *oom, uint32_t scope);

// Get the key for the plug n play driver registry
// The string returned by this function should NOT be freed
const char *LoaderPnpDriverRegistry(void);
const wchar_t *LoaderPnpDriverRegistryWide(void);
// Get the key for the plug 'n play explicit layer registry
// The string returned by this function should NOT be freed
const char *LoaderPnpELayerRegistry(void);
const wchar_t *LoaderPnpELayerRegistryWide(void);
// Get the key for the plug 'n play implicit layer registry
// The string returned by this function should NOT be freed
const char *LoaderPnpILayerRegistry(void);
const wchar_t *LoaderPnpILayerRegistryWide(void);

#endif
