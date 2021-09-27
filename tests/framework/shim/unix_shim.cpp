/*
 * Copyright (c) 2021 The Khronos Group Inc.
 * Copyright (c) 2021 Valve Corporation
 * Copyright (c) 2021 LunarG, Inc.
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
PlatformShim* get_platform_shim() {
    platform_shim = PlatformShim();
    return &platform_shim;
}
#elif defined(__APPLE__)
FRAMEWORK_EXPORT PlatformShim* get_platform_shim() {
    platform_shim = PlatformShim();
    return &platform_shim;
}
#endif

// Necessary for MacOS function himming
#if defined(__linux__) || defined(__FreeBSD__)
#define OPENDIR_FUNC_NAME opendir
#define ACCESS_FUNC_NAME access
#define FOPEN_FUNC_NAME fopen
#elif defined(__APPLE__)
#define OPENDIR_FUNC_NAME my_opendir
#define ACCESS_FUNC_NAME my_access
#define FOPEN_FUNC_NAME my_fopen
#endif

using PFN_OPENDIR = DIR* (*)(const char* path_name);
using PFN_ACCESS = int (*)(const char* pathname, int mode);
using PFN_FOPEN = FILE* (*)(const char* filename, const char* mode);

static PFN_OPENDIR real_opendir = nullptr;
static PFN_ACCESS real_access = nullptr;
static PFN_FOPEN real_fopen = nullptr;

FRAMEWORK_EXPORT DIR* OPENDIR_FUNC_NAME(const char* path_name) {
    if (!real_opendir) real_opendir = (PFN_OPENDIR)dlsym(RTLD_NEXT, "opendir");

    DIR* dir;
    if (platform_shim.is_fake_path(path_name)) {
        auto fake_path_name = platform_shim.get_fake_path(fs::path(path_name));
        dir = real_opendir(fake_path_name.c_str());
    } else {
        dir = real_opendir(path_name);
    }

    return dir;
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
__attribute__((used)) static Interposer _interpose_access MACOS_ATTRIB = {VOIDP_CAST(my_access), VOIDP_CAST(access)};
__attribute__((used)) static Interposer _interpose_fopen MACOS_ATTRIB = {VOIDP_CAST(my_fopen), VOIDP_CAST(fopen)};
#endif
}  // extern "C"
