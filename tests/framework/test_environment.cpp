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

namespace detail {
PlatformShimWrapper::PlatformShimWrapper(DebugMode debug_mode) noexcept : debug_mode(debug_mode) {
#if defined(WIN32) || defined(__APPLE__)
    shim_library = LibraryWrapper(SHIM_LIBRARY_NAME);
    auto get_platform_shim_func = shim_library.get_symbol<PFN_get_platform_shim>(GET_PLATFORM_SHIM_STR);
    assert(get_platform_shim_func != NULL && "Must be able to get \"platform_shim\"");
    platform_shim = get_platform_shim_func();
#elif defined(__linux__)
    platform_shim = get_platform_shim();
#endif
    platform_shim->setup_override(debug_mode);
    platform_shim->reset(debug_mode);

    // leave it permanently on at full blast
    set_env_var("VK_LOADER_DEBUG", "all");
}
PlatformShimWrapper::~PlatformShimWrapper() noexcept {
    platform_shim->reset(debug_mode);
    platform_shim->clear_override(debug_mode);
}

TestICDHandle::TestICDHandle() noexcept {}
TestICDHandle::TestICDHandle(fs::path const& icd_path) noexcept : icd_library(icd_path) {
    proc_addr_get_test_icd = icd_library.get_symbol<GetNewTestICDFunc>(GET_TEST_ICD_FUNC_STR);
    proc_addr_get_new_test_icd = icd_library.get_symbol<GetNewTestICDFunc>(GET_NEW_TEST_ICD_FUNC_STR);
}
TestICD& TestICDHandle::get_test_icd() noexcept {
    assert(proc_addr_get_test_icd != NULL && "symbol must be loaded before use");
    return *proc_addr_get_test_icd();
}
TestICD& TestICDHandle::get_new_test_icd() noexcept {
    assert(proc_addr_get_new_test_icd != NULL && "symbol must be loaded before use");
    return *proc_addr_get_new_test_icd();
}
fs::path TestICDHandle::get_icd_full_path() noexcept { return icd_library.lib_path; }
}  // namespace detail

FrameworkEnvironment::FrameworkEnvironment(DebugMode debug_mode) noexcept
    : platform_shim(debug_mode),
      null_folder(FRAMEWORK_BUILD_DIRECTORY, "null_dir", debug_mode),
      icd_folder(FRAMEWORK_BUILD_DIRECTORY, "icd_manifests", debug_mode),
      explicit_layer_folder(FRAMEWORK_BUILD_DIRECTORY, "explicit_layer_manifests", debug_mode),
      implicit_layer_folder(FRAMEWORK_BUILD_DIRECTORY, "implicit_layer_manifests", debug_mode),
      vulkan_functions() {
    platform_shim->redirect_all_paths(null_folder.location());

    platform_shim->set_path(ManifestCategory::icd, icd_folder.location());
    platform_shim->set_path(ManifestCategory::explicit_layer, explicit_layer_folder.location());
    platform_shim->set_path(ManifestCategory::implicit_layer, implicit_layer_folder.location());
}

void FrameworkEnvironment::AddICD(TestICDDetails icd_details, const std::string& json_name) noexcept {
    ManifestICD icd_manifest;
    icd_manifest.lib_path = fs::fixup_backslashes_in_path(icd_details.icd_path);
    icd_manifest.api_version = icd_details.api_version;
    auto driver_loc = icd_folder.write(json_name, icd_manifest);
    platform_shim->add_manifest(ManifestCategory::icd, driver_loc);
}
void FrameworkEnvironment::AddImplicitLayer(ManifestLayer layer_manifest, const std::string& json_name) noexcept {
    auto layer_loc = implicit_layer_folder.write(json_name, layer_manifest);
    platform_shim->add_manifest(ManifestCategory::implicit_layer, layer_loc);
}
void FrameworkEnvironment::AddExplicitLayer(ManifestLayer layer_manifest, const std::string& json_name) noexcept {
    auto layer_loc = explicit_layer_folder.write(json_name, layer_manifest);
    platform_shim->add_manifest(ManifestCategory::explicit_layer, layer_loc);
}

EnvVarICDOverrideShim::EnvVarICDOverrideShim(DebugMode debug_mode) noexcept : FrameworkEnvironment(debug_mode) {}

void EnvVarICDOverrideShim::SetEnvOverrideICD(const char* icd_path, const char* manifest_name) noexcept {
    ManifestICD icd_manifest;
    icd_manifest.lib_path = icd_path;
    icd_manifest.api_version = VK_MAKE_VERSION(1, 0, 0);

    icd_folder.write(manifest_name, icd_manifest);
    set_env_var("VK_ICD_FILENAMES", (icd_folder.location() / manifest_name).str());

    driver_wrapper = LibraryWrapper(fs::path(icd_path));

    get_new_test_icd = driver_wrapper.get_symbol<GetNewTestICDFunc>(GET_NEW_TEST_ICD_FUNC_STR);
}

SingleICDShim::SingleICDShim(TestICDDetails icd_details, DebugMode debug_mode) noexcept : FrameworkEnvironment(debug_mode) {
    icd_handle = detail::TestICDHandle(icd_details.icd_path);
    icd_handle.get_new_test_icd();

    AddICD(icd_details, "test_icd.json");
}
TestICD& SingleICDShim::get_test_icd() noexcept { return icd_handle.get_test_icd(); }
TestICD& SingleICDShim::get_new_test_icd() noexcept { return icd_handle.get_new_test_icd(); }
fs::path SingleICDShim::get_test_icd_path() noexcept { return icd_handle.get_icd_full_path(); }

MultipleICDShim::MultipleICDShim(std::vector<TestICDDetails> icd_details_vector, DebugMode debug_mode) noexcept
    : FrameworkEnvironment(debug_mode) {
    uint32_t i = 0;
    for (auto& test_icd_detail : icd_details_vector) {
        fs::path new_driver_name =
            fs::path(test_icd_detail.icd_path).stem() + "_" + std::to_string(i) + fs::path(test_icd_detail.icd_path).extension();

        auto new_driver_location = icd_folder.copy_file(test_icd_detail.icd_path, new_driver_name.str());

        icds.push_back(detail::TestICDHandle(new_driver_location));
        icds.back().get_new_test_icd();
        test_icd_detail.icd_path = new_driver_location.c_str();
        AddICD(test_icd_detail, std::string("test_icd_") + std::to_string(i) + ".json");
        i++;
    }
}
TestICD& MultipleICDShim::get_test_icd(int index) noexcept { return icds[index].get_test_icd(); }
TestICD& MultipleICDShim::get_new_test_icd(int index) noexcept { return icds[index].get_new_test_icd(); }
fs::path MultipleICDShim::get_test_icd_path(int index) noexcept { return icds[index].get_icd_full_path(); }

FakeBinaryICDShim::FakeBinaryICDShim(TestICDDetails read_icd_details, TestICDDetails fake_icd_details,
                                     DebugMode debug_mode) noexcept
    : FrameworkEnvironment(debug_mode) {
    real_icd = detail::TestICDHandle(read_icd_details.icd_path);
    real_icd.get_new_test_icd();

    // Must use name that isn't a substring of eachother, otherwise loader removes the other ICD
    // EX test_icd.json is fully contained in fake_test_icd.json, causing test_icd.json to not be loaded
    AddICD(fake_icd_details, "test_fake_icd.json");
    AddICD(read_icd_details, "test_icd.json");
}
TestICD& FakeBinaryICDShim::get_test_icd() noexcept { return real_icd.get_test_icd(); }
TestICD& FakeBinaryICDShim::get_new_test_icd() noexcept { return real_icd.get_new_test_icd(); }
fs::path FakeBinaryICDShim::get_test_icd_path() noexcept { return real_icd.get_icd_full_path(); }