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

/*
 * The test_environment is what combines the icd, layer, and shim library into a single object that
 * test fixtures can create and use. Responsible for loading the libraries and establishing the
 * channels for tests to talk with the icd's and layers.
*/
#pragma once


// Must include gtest first to guard against Xlib colliding due to redefinitions of "None" and "Bool"

#ifdef _MSC_VER
#pragma warning(push)
/*
    MSVC warnings 4251 and 4275 have to do with potential dll-interface mismatch
    between library (gtest) and users. Since we build the gtest library
    as part of the test build we know that the dll-interface will match and
    can disable these warnings.
 */
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)
#endif

// GTest and Xlib collide due to redefinitions of "None" and "Bool"
#ifdef VK_USE_PLATFORM_XLIB_KHR
#pragma push_macro("None")
#pragma push_macro("Bool")
#undef None
#undef Bool
#endif

// Use the NDK's header on Android
#include "gtest/gtest.h"

#include "test_util.h"

#include "shim/shim.h"

#include "icd/physical_device.h"
#include "icd/test_icd.h"

namespace detail {
struct PlatformShimWrapper {
    PlatformShimWrapper(DebugMode debug_mode = DebugMode::none);
    ~PlatformShimWrapper();
    PlatformShimWrapper(PlatformShimWrapper const&) = delete;
    PlatformShimWrapper& operator=(PlatformShimWrapper const&) = delete;

    // Convenience
    PlatformShim* operator->() { return platform_shim; }

    LibraryWrapper shim_library;
    PlatformShim* platform_shim;
    DebugMode debug_mode = DebugMode::none;
};

struct TestICDHandle {
    TestICDHandle();
    TestICDHandle(fs::path const& icd_path);
    TestICD& get_new_test_icd();
    TestICD& get_test_icd();
    fs::path get_icd_full_path();

    // Must use statically
    LibraryWrapper icd_library;
    GetTestICDFunc proc_addr_get_test_icd;
    GetNewTestICDFunc proc_addr_get_new_test_icd;
};
}  // namespace detail

struct TestICDDetails {
    TestICDDetails(const char* icd_path, uint32_t api_version = VK_MAKE_VERSION(1, 0, 0))
        : icd_path(icd_path), api_version(api_version) {}
    const char* icd_path = nullptr;
    uint32_t api_version = VK_MAKE_VERSION(1, 0, 0);
};
struct FrameworkEnvironment {
    FrameworkEnvironment(DebugMode debug_mode = DebugMode::none);

    void AddICD(TestICDDetails icd_details, const std::string& json_name);
    void AddImplicitLayer(ManifestLayer layer_manifest, const std::string& json_name);
    void AddExplicitLayer(ManifestLayer layer_manifest, const std::string& json_name);

    detail::PlatformShimWrapper platform_shim;
    fs::FolderManager null_folder;
    fs::FolderManager icd_folder;
    fs::FolderManager explicit_layer_folder;
    fs::FolderManager implicit_layer_folder;
    VulkanFunctions vulkan_functions;
};

struct EnvVarICDOverrideShim : FrameworkEnvironment {
    EnvVarICDOverrideShim(DebugMode debug_mode = DebugMode::none);

    void SetEnvOverrideICD(const char* icd_path, const char* manifest_name);

    LibraryWrapper driver_wrapper;
    GetNewTestICDFunc get_new_test_icd;
};


struct SingleICDShim : FrameworkEnvironment {
    SingleICDShim(TestICDDetails icd_details, DebugMode debug_mode = DebugMode::none);

    TestICD& get_test_icd();
    TestICD& get_new_test_icd();

    fs::path get_test_icd_path();

    detail::TestICDHandle icd_handle;
};

struct MultipleICDShim : FrameworkEnvironment {
    MultipleICDShim(std::vector<TestICDDetails> icd_details_vector, DebugMode debug_mode = DebugMode::none);

    TestICD& get_test_icd(int index);
    TestICD& get_new_test_icd(int index);
    fs::path get_test_icd_path(int index);

    std::vector<detail::TestICDHandle> icds;
};