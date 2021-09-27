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

InstWrapper::InstWrapper(VulkanFunctions& functions, VkAllocationCallbacks* callbacks) noexcept
    : functions(&functions), callbacks(callbacks) {}
InstWrapper::InstWrapper(VulkanFunctions& functions, VkInstance inst, VkAllocationCallbacks* callbacks) noexcept
    : functions(&functions), inst(inst), callbacks(callbacks) {}
InstWrapper::~InstWrapper() noexcept {
    if (inst != VK_NULL_HANDLE) functions->vkDestroyInstance(inst, callbacks);
}

InstWrapper::InstWrapper(InstWrapper&& other) noexcept {
    functions = other.functions;
    inst = other.inst;
    callbacks = other.callbacks;
    create_info = other.create_info;
    other.inst = VK_NULL_HANDLE;
}
InstWrapper& InstWrapper::operator=(InstWrapper&& other) noexcept {
    functions->vkDestroyInstance(inst, callbacks);
    functions = other.functions;
    inst = other.inst;
    callbacks = other.callbacks;
    create_info = other.create_info;
    other.inst = VK_NULL_HANDLE;
    return *this;
}

testing::AssertionResult InstWrapper::CheckCreate(VkResult result_to_check) {
    VkResult res = functions->vkCreateInstance(create_info.get(), callbacks, &inst);
    if (res == result_to_check)
        return testing::AssertionSuccess();
    else
        return testing::AssertionFailure() << " Expected VkCreateInstance to return " << result_to_check << " but got " << res;
}

std::vector<VkPhysicalDevice> InstWrapper::GetPhysDevs(uint32_t phys_dev_count, VkResult result_to_check) {
    uint32_t physical_count = phys_dev_count;
    std::vector<VkPhysicalDevice> physical_devices;
    physical_devices.resize(phys_dev_count);
    VkResult res = functions->vkEnumeratePhysicalDevices(inst, &physical_count, physical_devices.data());
    EXPECT_EQ(result_to_check, res);
    return physical_devices;
}

VkPhysicalDevice InstWrapper::GetPhysDev(VkResult result_to_check) {
    uint32_t physical_count = 1;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkResult res = this->functions->vkEnumeratePhysicalDevices(inst, &physical_count, &physical_device);
    EXPECT_EQ(result_to_check, res);
    return physical_device;
}

DeviceWrapper::DeviceWrapper(InstWrapper& inst_wrapper, VkAllocationCallbacks* callbacks) noexcept
    : functions(inst_wrapper.functions), callbacks(callbacks){};
DeviceWrapper::DeviceWrapper(VulkanFunctions& functions, VkDevice device, VkAllocationCallbacks* callbacks) noexcept
    : functions(&functions), dev(device), callbacks(callbacks){};
DeviceWrapper::~DeviceWrapper() noexcept { functions->vkDestroyDevice(dev, callbacks); }

DeviceWrapper::DeviceWrapper(DeviceWrapper&& other) noexcept {
    functions = other.functions;
    dev = other.dev;
    callbacks = other.callbacks;
    create_info = other.create_info;
    other.dev = VK_NULL_HANDLE;
}
DeviceWrapper& DeviceWrapper::operator=(DeviceWrapper&& other) noexcept {
    functions->vkDestroyDevice(dev, callbacks);
    functions = other.functions;
    dev = other.dev;
    callbacks = other.callbacks;
    create_info = other.create_info;
    other.dev = VK_NULL_HANDLE;
    return *this;
}

void DeviceWrapper::CheckCreate(VkPhysicalDevice phys_dev, VkResult result_to_check) {
    ASSERT_EQ(result_to_check, functions->vkCreateDevice(phys_dev, create_info.get(), callbacks, &dev));
}

VkResult CreateDebugUtilsMessenger(DebugUtilsWrapper& debug_utils) {
    return debug_utils.vkCreateDebugUtilsMessengerEXT(debug_utils.inst, debug_utils.get(), debug_utils.callbacks,
                                                      &debug_utils.messenger);
}

void FillDebugUtilsCreateDetails(InstanceCreateInfo& create_info, DebugUtilsLogger& logger) {
    create_info.add_extension("VK_EXT_debug_utils");
    create_info.inst_info.pNext = logger.get();
}
void FillDebugUtilsCreateDetails(InstanceCreateInfo& create_info, DebugUtilsWrapper& wrapper) {
    create_info.add_extension("VK_EXT_debug_utils");
    create_info.inst_info.pNext = wrapper.get();
}

PlatformShimWrapper::PlatformShimWrapper(DebugMode debug_mode) noexcept : debug_mode(debug_mode) {
#if defined(WIN32) || defined(__APPLE__)
    shim_library = LibraryWrapper(SHIM_LIBRARY_NAME);
    auto get_platform_shim_func = shim_library.get_symbol<PFN_get_platform_shim>(GET_PLATFORM_SHIM_STR);
    assert(get_platform_shim_func != NULL && "Must be able to get \"platform_shim\"");
    platform_shim = get_platform_shim_func();
#elif defined(__linux__) || defined(__FreeBSD__)
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
    proc_addr_reset_icd = icd_library.get_symbol<GetNewTestICDFunc>(RESET_ICD_FUNC_STR);
}
TestICD& TestICDHandle::get_test_icd() noexcept {
    assert(proc_addr_get_test_icd != NULL && "symbol must be loaded before use");
    return *proc_addr_get_test_icd();
}
TestICD& TestICDHandle::reset_icd() noexcept {
    assert(proc_addr_reset_icd != NULL && "symbol must be loaded before use");
    return *proc_addr_reset_icd();
}
fs::path TestICDHandle::get_icd_full_path() noexcept { return icd_library.lib_path; }

TestLayerHandle::TestLayerHandle() noexcept {}
TestLayerHandle::TestLayerHandle(fs::path const& layer_path) noexcept : layer_library(layer_path) {
    proc_addr_get_test_layer = layer_library.get_symbol<GetNewTestLayerFunc>(GET_TEST_LAYER_FUNC_STR);
    proc_addr_reset_layer = layer_library.get_symbol<GetNewTestLayerFunc>(RESET_LAYER_FUNC_STR);
}
TestLayer& TestLayerHandle::get_test_layer() noexcept {
    assert(proc_addr_get_test_layer != NULL && "symbol must be loaded before use");
    return *proc_addr_get_test_layer();
}
TestLayer& TestLayerHandle::reset_layer() noexcept {
    assert(proc_addr_reset_layer != NULL && "symbol must be loaded before use");
    return *proc_addr_reset_layer();
}
fs::path TestLayerHandle::get_layer_full_path() noexcept { return layer_library.lib_path; }

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
    for (auto& layer : layer_manifest.layers) {
        if (!layer.lib_path.str().empty()) {
            std::string new_layer_name = layer.name + "_" + layer.lib_path.filename().str();

            auto new_layer_location = implicit_layer_folder.copy_file(layer.lib_path, new_layer_name);
            layer.lib_path = new_layer_location;
        }
    }

    auto layer_loc = implicit_layer_folder.write(json_name, layer_manifest);
    platform_shim->add_manifest(ManifestCategory::implicit_layer, layer_loc);
}
void FrameworkEnvironment::AddExplicitLayer(ManifestLayer layer_manifest, const std::string& json_name) noexcept {
    for (auto& layer : layer_manifest.layers) {
        if (!layer.lib_path.str().empty()) {
            std::string new_layer_name = layer.name + "_" + layer.lib_path.filename().str();

            auto new_layer_location = explicit_layer_folder.copy_file(layer.lib_path, new_layer_name);
            layer.lib_path = new_layer_location;
        }
    }

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

    reset_icd = driver_wrapper.get_symbol<GetNewTestICDFunc>(RESET_ICD_FUNC_STR);
}

SingleICDShim::SingleICDShim(TestICDDetails icd_details, DebugMode debug_mode) noexcept : FrameworkEnvironment(debug_mode) {
    icd_handle = TestICDHandle(icd_details.icd_path);
    icd_handle.reset_icd();

    AddICD(icd_details, "test_icd.json");
}
TestICD& SingleICDShim::get_test_icd() noexcept { return icd_handle.get_test_icd(); }
TestICD& SingleICDShim::reset_icd() noexcept { return icd_handle.reset_icd(); }
fs::path SingleICDShim::get_test_icd_path() noexcept { return icd_handle.get_icd_full_path(); }

MultipleICDShim::MultipleICDShim(std::vector<TestICDDetails> icd_details_vector, DebugMode debug_mode) noexcept
    : FrameworkEnvironment(debug_mode) {
    uint32_t i = 0;
    for (auto& test_icd_detail : icd_details_vector) {
        fs::path new_driver_name =
            fs::path(test_icd_detail.icd_path).stem() + "_" + std::to_string(i) + fs::path(test_icd_detail.icd_path).extension();

        auto new_driver_location = icd_folder.copy_file(test_icd_detail.icd_path, new_driver_name.str());

        icds.push_back(TestICDHandle(new_driver_location));
        icds.back().reset_icd();
        test_icd_detail.icd_path = new_driver_location.c_str();
        AddICD(test_icd_detail, std::string("test_icd_") + std::to_string(i) + ".json");
        i++;
    }
}
TestICD& MultipleICDShim::get_test_icd(int index) noexcept { return icds[index].get_test_icd(); }
TestICD& MultipleICDShim::reset_icd(int index) noexcept { return icds[index].reset_icd(); }
fs::path MultipleICDShim::get_test_icd_path(int index) noexcept { return icds[index].get_icd_full_path(); }

FakeBinaryICDShim::FakeBinaryICDShim(TestICDDetails read_icd_details, TestICDDetails fake_icd_details,
                                     DebugMode debug_mode) noexcept
    : FrameworkEnvironment(debug_mode) {
    real_icd = TestICDHandle(read_icd_details.icd_path);
    real_icd.reset_icd();

    // Must use name that isn't a substring of eachother, otherwise loader removes the other ICD
    // EX test_icd.json is fully contained in fake_test_icd.json, causing test_icd.json to not be loaded
    AddICD(fake_icd_details, "test_fake_icd.json");
    AddICD(read_icd_details, "test_icd.json");
}
TestICD& FakeBinaryICDShim::get_test_icd() noexcept { return real_icd.get_test_icd(); }
TestICD& FakeBinaryICDShim::reset_icd() noexcept { return real_icd.reset_icd(); }
fs::path FakeBinaryICDShim::get_test_icd_path() noexcept { return real_icd.get_icd_full_path(); }