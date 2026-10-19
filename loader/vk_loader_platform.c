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
#include "vk_loader_platform.h"

#if defined(_WIN32)
#include "stack_allocation.h"
#endif

bool loader_disable_dynamic_library_unloading;

#if COMMON_UNIX_PLATFORMS

// File IO
bool loader_platform_file_exists(const char *path) {
    if (access(path, F_OK)) return false;
    return true;
}
// Returns true if the given string appears to be a relative or absolute
// path, as opposed to a bare filename.
bool loader_platform_is_path_absolute(const char *path) {
    if (path[0] == '/') return true;
    return false;
}

// NOLINTNEXTLINE(concurrency-mt-unsafe) - no portable reentrant dirname; the returned buffer is copied by callers immediately
char *loader_platform_dirname(char *path) { return dirname(path); }

// loader_platform_executable_path finds application path + name.
// Path cannot be longer than 1024, returns NULL if it is greater than that.
#if defined(__linux__) || defined(__GNU__) || defined(__CYGWIN__)
char *loader_platform_executable_path(char *buffer, size_t size) {
    ssize_t count = readlink("/proc/self/exe", buffer, size);
    if (count == -1) return NULL;
    if (count == 0) return NULL;
    // readlink does not append a null terminator and returns up to size bytes, so a target of size or more
    // would leave no room for the terminator and buffer[count] would write past the end.
    if ((size_t)count >= size) return NULL;
    buffer[count] = '\0';
    return buffer;
}
#elif defined(__APPLE__)
#include <TargetConditionals.h>
// TARGET_OS_IPHONE isn't just iOS it's also iOS/tvOS/watchOS. See TargetConditionals.h documentation.
#if TARGET_OS_IPHONE
char *loader_platform_executable_path(char *buffer, size_t size) {
    (void)size;
    buffer[0] = '\0';
    return buffer;
}
#endif
#if TARGET_OS_OSX
#include <libproc.h>
char *loader_platform_executable_path(char *buffer, size_t size) {
    // proc_pidpath takes a uint32_t for the buffer size
    if (size > UINT32_MAX) {
        return NULL;
    }
    pid_t pid = getpid();
    int ret = proc_pidpath(pid, buffer, (uint32_t)size);
    if (ret <= 0) {
        return NULL;
    }
    buffer[ret] = '\0';
    return buffer;
}
#endif
#elif defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__)
#include <sys/sysctl.h>
char *loader_platform_executable_path(char *buffer, size_t size) {
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
char *loader_platform_executable_path(char *buffer, size_t size) {
    (void)buffer;
    (void)size;
    return NULL;
}
#elif defined(__QNX__)

#include <fcntl.h>
#include <sys/stat.h>

char *loader_platform_executable_path(char *buffer, size_t size) {
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
#endif  // defined (__QNX__)

// Compatibility with compilers that don't support __has_feature
#if !defined(__has_feature)
#define __has_feature(x) 0
#endif

#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
#define LOADER_ADDRESS_SANITIZER_ACTIVE  // TODO: Add proper build flag for ASAN support
#endif

// When loading the library, we use RTLD_LAZY so that not all symbols have to be
// resolved at this time (which improves performance). Note that if not all symbols
// can be resolved, this could cause crashes later. Use the LD_BIND_NOW environment
// variable to force all symbols to be resolved here.
#define LOADER_DLOPEN_MODE (RTLD_LAZY | RTLD_LOCAL)

#if defined(__Fuchsia__)
loader_platform_dl_handle loader_platform_open_driver(const char *libPath) {
    return dlopen_fuchsia(libPath, LOADER_DLOPEN_MODE, true);
}
loader_platform_dl_handle loader_platform_open_library(const char *libPath) {
    return dlopen_fuchsia(libPath, LOADER_DLOPEN_MODE, false);
}
#else
loader_platform_dl_handle loader_platform_open_library(const char *libPath) { return dlopen(libPath, LOADER_DLOPEN_MODE); }
#endif

const char *loader_platform_open_library_error(const char *libPath) {
    (void)libPath;
#if defined(__Fuchsia__)
    return dlerror_fuchsia();
#else
    // NOLINTNEXTLINE(concurrency-mt-unsafe) - dlerror has no reentrant variant; the message is used immediately, not retained
    return dlerror();
#endif
}
void loader_platform_close_library(loader_platform_dl_handle library) {
    if (!loader_disable_dynamic_library_unloading) {
        dlclose(library);
    } else {
        (void)library;
    }
}
void *loader_platform_get_proc_address(loader_platform_dl_handle library, const char *name) {
    assert(library);
    assert(name);
    return dlsym(library, name);
}
const char *loader_platform_get_proc_address_error(const char *name) {
    (void)name;
    // NOLINTNEXTLINE(concurrency-mt-unsafe) - dlerror has no reentrant variant; the message is used immediately, not retained
    return dlerror();
}

// Thread mutex:
void loader_platform_thread_create_mutex(loader_platform_thread_mutex *pMutex) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(pMutex, &attr);
}
void loader_platform_thread_lock_mutex(loader_platform_thread_mutex *pMutex) { pthread_mutex_lock(pMutex); }
void loader_platform_thread_unlock_mutex(loader_platform_thread_mutex *pMutex) { pthread_mutex_unlock(pMutex); }
void loader_platform_thread_delete_mutex(loader_platform_thread_mutex *pMutex) { pthread_mutex_destroy(pMutex); }

void *thread_safe_strtok(char *str, const char *delim, char **saveptr) { return strtok_r(str, delim, saveptr); }

FILE *loader_fopen(const char *fileName, const char *mode) { return fopen(fileName, mode); }
char *loader_strncat(char *dest, size_t dest_sz, const char *src, size_t count) {
    (void)dest_sz;
    return strncat(dest, src, count);
}
char *loader_strncpy(char *dest, size_t dest_sz, const char *src, size_t count) {
    (void)dest_sz;
    return strncpy(dest, src, count);
}

#elif defined(_WIN32)

// Get the key for the plug n play driver registry
// The string returned by this function should NOT be freed
const char *LoaderPnpDriverRegistry(void) {
    BOOL is_wow;
    IsWow64Process(GetCurrentProcess(), &is_wow);
    return is_wow ? "Vulkan" VK_VARIANT_REG_STR "DriverNameWow" : "Vulkan" VK_VARIANT_REG_STR "DriverName";
}
const wchar_t *LoaderPnpDriverRegistryWide(void) {
    BOOL is_wow;
    IsWow64Process(GetCurrentProcess(), &is_wow);
    return is_wow ? L"Vulkan" VK_VARIANT_REG_STR_W L"DriverNameWow" : L"Vulkan" VK_VARIANT_REG_STR_W L"DriverName";
}

// Get the key for the plug 'n play explicit layer registry
// The string returned by this function should NOT be freed
const char *LoaderPnpELayerRegistry(void) {
    BOOL is_wow;
    IsWow64Process(GetCurrentProcess(), &is_wow);
    return is_wow ? "Vulkan" VK_VARIANT_REG_STR "ExplicitLayersWow" : "Vulkan" VK_VARIANT_REG_STR "ExplicitLayers";
}
const wchar_t *LoaderPnpELayerRegistryWide(void) {
    BOOL is_wow;
    IsWow64Process(GetCurrentProcess(), &is_wow);
    return is_wow ? L"Vulkan" VK_VARIANT_REG_STR_W L"ExplicitLayersWow" : L"Vulkan" VK_VARIANT_REG_STR_W L"ExplicitLayers";
}

// Get the key for the plug 'n play implicit layer registry
// The string returned by this function should NOT be freed
const char *LoaderPnpILayerRegistry(void) {
    BOOL is_wow;
    IsWow64Process(GetCurrentProcess(), &is_wow);
    return is_wow ? "Vulkan" VK_VARIANT_REG_STR "ImplicitLayersWow" : "Vulkan" VK_VARIANT_REG_STR "ImplicitLayers";
}
const wchar_t *LoaderPnpILayerRegistryWide(void) {
    BOOL is_wow;
    IsWow64Process(GetCurrentProcess(), &is_wow);
    return is_wow ? L"Vulkan" VK_VARIANT_REG_STR_W L"ImplicitLayersWow" : L"Vulkan" VK_VARIANT_REG_STR_W L"ImplicitLayers";
}

// File IO
bool loader_platform_file_exists(const char *path) {
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
bool loader_platform_is_path_absolute(const char *path) {
    if (!path || !*path) {
        return false;
    }
    if (*path == DIRECTORY_SYMBOL || path[1] == ':') {
        return true;
    }
    return false;
}

// WIN32 runtime doesn't have dirname().
char *loader_platform_dirname(char *path) {
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

char *loader_platform_executable_path(char *buffer, size_t size) {
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
loader_platform_dl_handle loader_platform_open_library(const char *lib_path) {
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
const char *loader_platform_open_library_error(const char *libPath) {
    static char errorMsg[512];
    (void)snprintf(errorMsg, 511, "Failed to open dynamic library \"%s\" with error %lu", libPath, GetLastError());
    return errorMsg;
}
void loader_platform_close_library(loader_platform_dl_handle library) {
    if (!loader_disable_dynamic_library_unloading) {
        FreeLibrary(library);
    } else {
        (void)library;
    }
}
void *loader_platform_get_proc_address(loader_platform_dl_handle library, const char *name) {
    assert(library);
    assert(name);
    return (void *)GetProcAddress(library, name);
}
const char *loader_platform_get_proc_address_error(const char *name) {
    static char errorMsg[120];
    (void)snprintf(errorMsg, 119, "Failed to find function \"%s\" in dynamic library", name);
    return errorMsg;
}

// Thread mutex:
void loader_platform_thread_create_mutex(loader_platform_thread_mutex *pMutex) { InitializeCriticalSection(pMutex); }
void loader_platform_thread_lock_mutex(loader_platform_thread_mutex *pMutex) { EnterCriticalSection(pMutex); }
void loader_platform_thread_unlock_mutex(loader_platform_thread_mutex *pMutex) { LeaveCriticalSection(pMutex); }
void loader_platform_thread_delete_mutex(loader_platform_thread_mutex *pMutex) { DeleteCriticalSection(pMutex); }

void *thread_safe_strtok(char *str, const char *delimiters, char **context) { return strtok_s(str, delimiters, context); }

FILE *loader_fopen(const char *fileName, const char *mode) {
    FILE *file = NULL;
    errno_t err = fopen_s(&file, fileName, mode);
    if (err != 0) return NULL;
    return file;
}

char *loader_strncat(char *dest, size_t dest_sz, const char *src, size_t count) {
    errno_t err = strncat_s(dest, dest_sz, src, count);
    if (err != 0) return NULL;
    return dest;
}

char *loader_strncpy(char *dest, size_t dest_sz, const char *src, size_t count) {
    errno_t err = strncpy_s(dest, dest_sz, src, count);
    if (err != 0) return NULL;
    return dest;
}

#else  // defined(_WIN32)

#warning The "vk_loader_platform.h" file must be modified for this OS.

// NOTE: In order to support another OS, an #elif needs to be added (above the
// "#else // defined(_WIN32)") for that OS, and OS-specific versions of the
// contents of this file must be created.

// NOTE: Other OS-specific changes are also needed for this OS.  Search for
// files with "WIN32" in it, as a quick way to find files that must be changed.

#endif  // defined(_WIN32)
