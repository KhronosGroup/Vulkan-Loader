/*
 * Copyright (c) 2021-2022 The Khronos Group Inc.
 * Copyright (c) 2021-2022 Valve Corporation
 * Copyright (c) 2021-2022 LunarG, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and/or associated documentation files (the "Materials"), to
 * deal in the Materials without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Materials, and to permit persons to whom the Materials are
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice(s) and this permission notice shall be included in
 * all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR THE
 * USE OR OTHER DEALINGS IN THE MATERIALS.
 *
 * Author: Charles Giessen <charles@lunarg.com>
 */

#include "shim.h"

static PlatformShim platform_shim;
extern "C" {
#if defined(__linux__) || defined(__FreeBSD__)
PlatformShim* get_platform_shim(std::vector<fs::FolderManager>* folders) {
    platform_shim = PlatformShim(folders);
    return &platform_shim;
}
#elif defined(__APPLE__)
FRAMEWORK_EXPORT PlatformShim* get_platform_shim(std::vector<fs::FolderManager>* folders) {
    platform_shim = PlatformShim(folders);
    return &platform_shim;
}
#endif

// Necessary for MacOS function shimming
#if defined(__linux__) || defined(__FreeBSD__)
#define OPENDIR_FUNC_NAME opendir
#define READDIR_FUNC_NAME readdir
#define CLOSEDIR_FUNC_NAME closedir
#define ACCESS_FUNC_NAME access
#define FOPEN_FUNC_NAME fopen
#define GETEUID_FUNC_NAME geteuid
#define GETEGID_FUNC_NAME getegid
#if defined(HAVE_SECURE_GETENV)
#define SECURE_GETENV_FUNC_NAME secure_getenv
#endif
#if defined(HAVE___SECURE_GETENV)
#define __SECURE_GETENV_FUNC_NAME __secure_getenv
#endif
#elif defined(__APPLE__)
#define OPENDIR_FUNC_NAME my_opendir
#define READDIR_FUNC_NAME my_readdir
#define CLOSEDIR_FUNC_NAME my_closedir
#define ACCESS_FUNC_NAME my_access
#define FOPEN_FUNC_NAME my_fopen
#define GETEUID_FUNC_NAME my_geteuid
#define GETEGID_FUNC_NAME my_getegid
#if defined(HAVE_SECURE_GETENV)
#define SECURE_GETENV_FUNC_NAME my_secure_getenv
#endif
#if defined(HAVE___SECURE_GETENV)
#define __SECURE_GETENV_FUNC_NAME my__secure_getenv
#endif
#endif

using PFN_OPENDIR = DIR* (*)(const char* path_name);
using PFN_READDIR = struct dirent* (*)(DIR* dir_stream);
using PFN_CLOSEDIR = int (*)(DIR* dir_stream);
using PFN_ACCESS = int (*)(const char* pathname, int mode);
using PFN_FOPEN = FILE* (*)(const char* filename, const char* mode);
using PFN_GETEUID = uid_t (*)(void);
using PFN_GETEGID = gid_t (*)(void);
#if defined(HAVE_SECURE_GETENV) || defined(HAVE___SECURE_GETENV)
using PFN_SEC_GETENV = char* (*)(const char* name);
#endif

static PFN_OPENDIR real_opendir = nullptr;
static PFN_READDIR real_readdir = nullptr;
static PFN_CLOSEDIR real_closedir = nullptr;
static PFN_ACCESS real_access = nullptr;
static PFN_FOPEN real_fopen = nullptr;
static PFN_GETEUID real_geteuid = nullptr;
static PFN_GETEGID real_getegid = nullptr;
#if defined(HAVE_SECURE_GETENV)
static PFN_SEC_GETENV real_secure_getenv = nullptr;
#endif
#if defined(HAVE___SECURE_GETENV)
static PFN_SEC_GETENV real__secure_getenv = nullptr;
#endif

FRAMEWORK_EXPORT DIR* OPENDIR_FUNC_NAME(const char* path_name) {
    if (!real_opendir) real_opendir = (PFN_OPENDIR)dlsym(RTLD_NEXT, "opendir");

    DIR* dir;
    if (platform_shim.is_fake_path(path_name)) {
        auto fake_path_name = platform_shim.get_fake_path(fs::path(path_name));
        dir = real_opendir(fake_path_name.c_str());
        platform_shim.dir_entries.push_back(DirEntry{dir, std::string(path_name), {}, false});
    } else {
        dir = real_opendir(path_name);
    }

    return dir;
}

FRAMEWORK_EXPORT struct dirent* READDIR_FUNC_NAME(DIR* dir_stream) {
    if (!real_readdir) real_readdir = (PFN_READDIR)dlsym(RTLD_NEXT, "readdir");
    auto it = std::find_if(platform_shim.dir_entries.begin(), platform_shim.dir_entries.end(),
                           [dir_stream](DirEntry const& entry) { return entry.directory == dir_stream; });

    if (it == platform_shim.dir_entries.end()) {
        return real_readdir(dir_stream);
    }
    // Folder was found but this is the first file to be read from it
    if (it->current_index == 0) {
        std::vector<struct dirent*> folder_contents;
        std::vector<std::string> dirent_filenames;
        while (true) {
            struct dirent* dir_entry = real_readdir(dir_stream);
            if (NULL == dir_entry) {
                break;
            }
            folder_contents.push_back(dir_entry);
            dirent_filenames.push_back(&dir_entry->d_name[0]);
        }
        auto real_path = platform_shim.redirection_map.at(it->folder_path);
        auto filenames = get_folder_contents(platform_shim.folders, real_path.str());

        // Add the dirent structures in the order they appear in the FolderManager
        // Ignore anything which wasn't in the FolderManager
        for (auto const& file : filenames) {
            for (size_t i = 0; i < dirent_filenames.size(); i++) {
                if (file == dirent_filenames.at(i)) {
                    it->contents.push_back(folder_contents.at(i));
                    break;
                }
            }
        }
    }
    if (it->current_index >= it->contents.size()) return nullptr;
    return it->contents.at(it->current_index++);
}

FRAMEWORK_EXPORT int CLOSEDIR_FUNC_NAME(DIR* dir_stream) {
    if (!real_closedir) real_closedir = (PFN_CLOSEDIR)dlsym(RTLD_NEXT, "closedir");

    auto it = std::find_if(platform_shim.dir_entries.begin(), platform_shim.dir_entries.end(),
                           [dir_stream](DirEntry const& entry) { return entry.directory == dir_stream; });

    if (it != platform_shim.dir_entries.end()) {
        platform_shim.dir_entries.erase(it);
    }

    return real_closedir(dir_stream);
}

FRAMEWORK_EXPORT int ACCESS_FUNC_NAME(const char* in_pathname, int mode) {
    if (!real_access) real_access = (PFN_ACCESS)dlsym(RTLD_NEXT, "access");

    fs::path path{in_pathname};
    if (!path.has_parent_path()) {
        return real_access(in_pathname, mode);
    }

    if (platform_shim.is_fake_path(path.parent_path())) {
        fs::path fake_path = platform_shim.get_fake_path(path.parent_path());
        fake_path /= path.filename();
        return real_access(fake_path.c_str(), mode);
    }
    return real_access(in_pathname, mode);
}

FRAMEWORK_EXPORT FILE* FOPEN_FUNC_NAME(const char* in_filename, const char* mode) {
    if (!real_fopen) real_fopen = (PFN_FOPEN)dlsym(RTLD_NEXT, "fopen");

    fs::path path{in_filename};
    if (!path.has_parent_path()) {
        return real_fopen(in_filename, mode);
    }

    FILE* f_ptr;
    if (platform_shim.is_fake_path(path.parent_path())) {
        auto fake_path = platform_shim.get_fake_path(path.parent_path()) / path.filename();
        f_ptr = real_fopen(fake_path.c_str(), mode);
    } else {
        f_ptr = real_fopen(in_filename, mode);
    }

    return f_ptr;
}

FRAMEWORK_EXPORT uid_t GETEUID_FUNC_NAME(void) {
    if (!real_geteuid) real_geteuid = (PFN_GETEUID)dlsym(RTLD_NEXT, "geteuid");

    if (platform_shim.use_fake_elevation) {
        // Root on linux is 0, so force pretending like we're root
        return 0;
    } else {
        return real_geteuid();
    }
}

FRAMEWORK_EXPORT gid_t GETEGID_FUNC_NAME(void) {
    if (!real_getegid) real_getegid = (PFN_GETEGID)dlsym(RTLD_NEXT, "getegid");

    if (platform_shim.use_fake_elevation) {
        // Root on linux is 0, so force pretending like we're root
        return 0;
    } else {
        return real_getegid();
    }
}

#if defined(HAVE_SECURE_GETENV)
FRAMEWORK_EXPORT char* SECURE_GETENV_FUNC_NAME(const char* name) {
    if (!real_secure_getenv) real_secure_getenv = (PFN_SEC_GETENV)dlsym(RTLD_NEXT, "secure_getenv");

    if (platform_shim.use_fake_elevation) {
        return NULL;
    } else {
        return real_secure_getenv(name);
    }
}
#endif
#if defined(HAVE___SECURE_GETENV)
FRAMEWORK_EXPORT char* __SECURE_GETENV_FUNC_NAME(const char* name) {
    if (!real__secure_getenv) real__secure_getenv = (PFN_SEC_GETENV)dlsym(RTLD_NEXT, "__secure_getenv");

    if (platform_shim.use_fake_elevation) {
        return NULL;
    } else {
        return real__secure_getenv(name);
    }
}
#endif

/* Shiming functions on apple is limited by the linker prefering to not use functions in the
 * executable in loaded dylibs. By adding an interposer, we redirect the linker to use our
 * version of the function over the real one, thus shimming the system function.
 */
#if defined(__APPLE__)
#define MACOS_ATTRIB __attribute__((section("__DATA,__interpose")))
#define VOIDP_CAST(_func) reinterpret_cast<const void*>(&_func)

struct Interposer {
    const void* shim_function;
    const void* underlying_function;
};

__attribute__((used)) static Interposer _interpose_opendir MACOS_ATTRIB = {VOIDP_CAST(my_opendir), VOIDP_CAST(opendir)};
// don't intercept readdir as it crashes when using ASAN with macOS
// __attribute__((used)) static Interposer _interpose_readdir MACOS_ATTRIB = {VOIDP_CAST(my_readdir), VOIDP_CAST(readdir)};
__attribute__((used)) static Interposer _interpose_closedir MACOS_ATTRIB = {VOIDP_CAST(my_closedir), VOIDP_CAST(closedir)};
__attribute__((used)) static Interposer _interpose_access MACOS_ATTRIB = {VOIDP_CAST(my_access), VOIDP_CAST(access)};
__attribute__((used)) static Interposer _interpose_fopen MACOS_ATTRIB = {VOIDP_CAST(my_fopen), VOIDP_CAST(fopen)};
__attribute__((used)) static Interposer _interpose_euid MACOS_ATTRIB = {VOIDP_CAST(my_geteuid), VOIDP_CAST(geteuid)};
__attribute__((used)) static Interposer _interpose_egid MACOS_ATTRIB = {VOIDP_CAST(my_getegid), VOIDP_CAST(getegid)};
#if defined(HAVE_SECURE_GETENV)
__attribute__((used)) static Interposer _interpose_secure_getenv MACOS_ATTRIB = {VOIDP_CAST(my_secure_getenv),
                                                                                 VOIDP_CAST(secure_getenv)};
#endif
#if defined(HAVE___SECURE_GETENV)
__attribute__((used)) static Interposer _interpose__secure_getenv MACOS_ATTRIB = {VOIDP_CAST(my__secure_getenv),
                                                                                  VOIDP_CAST(__secure_getenv)};
#endif
#endif
}  // extern "C"
