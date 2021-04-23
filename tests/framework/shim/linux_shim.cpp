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

PlatformShim& get_platform_shim() {
    platform_shim = PlatformShim();
    return platform_shim;
}

extern "C" {

using PFN_OPENDIR = DIR* (*)(const char* path_name);

FRAMEWORK_EXPORT DIR* opendir(const char* path_name) {
    static PFN_OPENDIR real_opendir = nullptr;
    if (!real_opendir) real_opendir = (PFN_OPENDIR)dlsym(RTLD_NEXT, "opendir");

    DIR* dir;
    if (platform_shim.is_fake_path(path_name)) {
        auto fake_path_name = platform_shim.get_fake_path(fs::path(path_name));
        //printf("Open Dir %s as %s\n", path_name, fake_path_name.c_str());
        dir = real_opendir(fake_path_name.c_str());
    } else {
        //  printf("Open Dir %s\n", path_name);
        dir = real_opendir(path_name);
    }

    return dir;
}

using PFN_ACCESS = int (*)(const char* pathname, int mode);

FRAMEWORK_EXPORT int access(const char* in_pathname, int mode) {
    static PFN_ACCESS real_access = nullptr;
    if (!real_access) real_access = (PFN_ACCESS)dlsym(RTLD_NEXT, "access");

    fs::path path{in_pathname};
    if (!path.has_parent_path()) {
        return real_access(in_pathname, mode);
    }

    if (platform_shim.is_fake_path(path.parent_path())) {
        fs::path fake_path = platform_shim.get_fake_path(path.parent_path());
        fake_path /= path.filename();
        // printf("ACCESS: in_pathname %s as %s\n", split.path.c_str(), fake_path.c_str());
        return real_access(fake_path.c_str(), mode);
    }
    return real_access(in_pathname, mode);
}

using PFN_FOPEN = FILE* (*)(const char* filename, const char* mode);

FRAMEWORK_EXPORT FILE* fopen(const char* in_filename, const char* mode) {
    static PFN_FOPEN real_fopen = nullptr;
    if (!real_fopen) real_fopen = (PFN_FOPEN)dlsym(RTLD_NEXT, "fopen");
    //printf("fopen file %s\n", in_filename);

    fs::path path{in_filename};
    if (!path.has_parent_path()) {
        return real_fopen(in_filename, mode);
    }

    // printf("path %s as %s\n", split.path.c_str(), split.name.c_str());

    FILE* f_ptr;
    if (platform_shim.is_fake_path(path.parent_path())) {
        auto fake_path = platform_shim.get_fake_path(path.parent_path()) / path.filename();
        // printf("fopen %s as %s\n", in_filename, fake_path.c_str());
        f_ptr = real_fopen(fake_path.c_str(), mode);
    } else {
        // printf("fopen %s\n", filename);
        f_ptr = real_fopen(in_filename, mode);
    }

    return f_ptr;
}
}  // extern "C"
