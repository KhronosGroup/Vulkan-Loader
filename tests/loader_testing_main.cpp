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

#include "test_environment.h"

int main(int argc, char** argv) {
#if defined(_WIN32)
    // Avoid "Abort, Retry, Ignore" dialog boxes
    _set_error_mode(_OUT_TO_STDERR);
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif

    // clean up folders from old test
    fs::delete_folder(fs::path(FRAMEWORK_BUILD_DIRECTORY) / "null_dir");
    fs::delete_folder(fs::path(FRAMEWORK_BUILD_DIRECTORY) / "icd_manifests");
    fs::delete_folder(fs::path(FRAMEWORK_BUILD_DIRECTORY) / "portability_icd_manifests");
    fs::delete_folder(fs::path(FRAMEWORK_BUILD_DIRECTORY) / "explicit_layer_manifests");
    fs::delete_folder(fs::path(FRAMEWORK_BUILD_DIRECTORY) / "implicit_layer_manifests");

    // make sure the tests don't find these env-vars if they were set on the system
    remove_env_var("VK_ICD_FILENAMES");
    remove_env_var("VK_LAYER_PATH");
    remove_env_var("VK_INSTANCE_LAYERS");
    remove_env_var("VK_LOADER_DEBUG");
    remove_env_var("VK_LOADER_DISABLE_INST_EXT_FILTER");

#if defined(__linux__) || defined(__FreeBSD__)
    set_env_var("XDG_CONFIG_HOME", "/etc");
    set_env_var("XDG_CONFIG_DIRS", "/etc");
    set_env_var("XDG_DATA_HOME", "/etc");
    set_env_var("XDG_DATA_DIRS", "/etc");
#endif
#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
    set_env_var("HOME", "/home/fake_home");
#endif

    ::testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();
    return result;
}
