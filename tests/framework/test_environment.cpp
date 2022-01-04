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

void InstWrapper::CheckCreate(VkResult result_to_check) {
    ASSERT_EQ(result_to_check, functions->vkCreateInstance(create_info.get(), callbacks, &inst));
}

std::vector<VkPhysicalDevice> InstWrapper::GetPhysDevs(uint32_t phys_dev_count, VkResult result_to_check) {
    uint32_t physical_count = phys_dev_count;
    std::vector<VkPhysicalDevice> physical_devices;
    physical_devices.resize(phys_dev_count);
    VkResult res = functions->vkEnumeratePhysicalDevices(inst, &physical_count, physical_devices.data());
    EXPECT_EQ(result_to_check, res);
    return physical_devices;
}

std::vector<VkPhysicalDevice> InstWrapper::GetPhysDevs(VkResult result_to_check) {
    uint32_t physical_count = 0;
    VkResult res = functions->vkEnumeratePhysicalDevices(inst, &physical_count, nullptr);
    std::vector<VkPhysicalDevice> physical_devices;
    physical_devices.resize(physical_count);
    res = functions->vkEnumeratePhysicalDevices(inst, &physical_count, physical_devices.data());
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

std::vector<VkExtensionProperties> EnumerateDeviceExtensions(InstWrapper const& inst, VkPhysicalDevice physical_device) {
    uint32_t ext_count = 1;
    VkResult res = inst.functions->vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &ext_count, nullptr);
    EXPECT_EQ(VK_SUCCESS, res);
    std::vector<VkExtensionProperties> extensions;
    extensions.resize(ext_count);
    res = inst.functions->vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &ext_count, extensions.data());
    EXPECT_EQ(VK_SUCCESS, res);
    extensions.resize(ext_count);
    return extensions;
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
    create_info.instance_info.pNext = logger.get();
}
void FillDebugUtilsCreateDetails(InstanceCreateInfo& create_info, DebugUtilsWrapper& wrapper) {
    create_info.add_extension("VK_EXT_debug_utils");
    create_info.instance_info.pNext = wrapper.get();
}

PlatformShimWrapper::PlatformShimWrapper(DebugMode debug_mode) noexcept : debug_mode(debug_mode) {
#if defined(WIN32) || defined(__APPLE__)
    shim_library = LibraryWrapper(SHIM_LIBRARY_NAME);
    PFN_get_platform_shim get_platform_shim_func = shim_library.get_symbol(GET_PLATFORM_SHIM_STR);
    assert(get_platform_shim_func != NULL && "Must be able to get \"platform_shim\"");
    platform_shim = get_platform_shim_func();
#elif defined(__linux__) || defined(__FreeBSD__)
    platform_shim = get_platform_shim();
#endif
    platform_shim->reset(debug_mode);

    // leave it permanently on at full blast
    set_env_var("VK_LOADER_DEBUG", "all");
}
PlatformShimWrapper::~PlatformShimWrapper() noexcept { platform_shim->reset(debug_mode); }

TestICDHandle::TestICDHandle() noexcept {}
TestICDHandle::TestICDHandle(fs::path const& icd_path) noexcept : icd_library(icd_path) {
    proc_addr_get_test_icd = icd_library.get_symbol(GET_TEST_ICD_FUNC_STR);
    proc_addr_reset_icd = icd_library.get_symbol(RESET_ICD_FUNC_STR);
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
    proc_addr_get_test_layer = layer_library.get_symbol(GET_TEST_LAYER_FUNC_STR);
    proc_addr_reset_layer = layer_library.get_symbol(RESET_LAYER_FUNC_STR);
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

FrameworkEnvironment::FrameworkEnvironment(DebugMode debug_mode, bool override_icds, bool override_layers) noexcept
    : platform_shim(debug_mode),
      null_folder(FRAMEWORK_BUILD_DIRECTORY, "null_dir", debug_mode),
      icd_folder(FRAMEWORK_BUILD_DIRECTORY, "icd_manifests", debug_mode),
      explicit_layer_folder(FRAMEWORK_BUILD_DIRECTORY, "explicit_layer_manifests", debug_mode),
      implicit_layer_folder(FRAMEWORK_BUILD_DIRECTORY, "implicit_layer_manifests", debug_mode),
      vulkan_functions() {
    platform_shim->redirect_all_paths(null_folder.location());

    if (override_icds) {
        platform_shim->set_path(ManifestCategory::icd, null_folder.location());
    } else {
        platform_shim->set_path(ManifestCategory::icd, icd_folder.location());
    }
    if (override_layers) {
        platform_shim->set_path(ManifestCategory::explicit_layer, null_folder.location());
    } else {
        platform_shim->set_path(ManifestCategory::explicit_layer, explicit_layer_folder.location());
    }
    platform_shim->set_path(ManifestCategory::implicit_layer, implicit_layer_folder.location());
}

void FrameworkEnvironment::add_icd(TestICDDetails icd_details) noexcept {
    size_t cur_icd_index = icds.size();
    if (!icd_details.is_fake) {
        fs::path new_driver_name = fs::path(icd_details.icd_path).stem() + "_" + std::to_string(cur_icd_index) +
                                   fs::path(icd_details.icd_path).extension();

        auto new_driver_location = icd_folder.copy_file(icd_details.icd_path, new_driver_name.str());

        icds.push_back(TestICDHandle(new_driver_location));
        icds.back().reset_icd();
        icd_details.icd_path = new_driver_location;
    }
    std::string full_json_name = icd_details.json_name + "_" + std::to_string(cur_icd_index) + ".json";

    auto driver_loc =
        icd_folder.write_manifest(full_json_name, ManifestICD{}
                                                      .set_lib_path(fs::fixup_backslashes_in_path(icd_details.icd_path).str())
                                                      .set_api_version(icd_details.api_version)
                                                      .get_manifest_str());

    if (icd_details.use_env_var_icd_filenames) {
        if (!env_var_vk_icd_filenames.empty()) {
            env_var_vk_icd_filenames += OS_ENV_VAR_LIST_SEPARATOR;
        }
        env_var_vk_icd_filenames += (icd_folder.location() / full_json_name).str();
        set_env_var("VK_DRIVER_FILES", env_var_vk_icd_filenames);
    } else  if (icd_details.use_add_env_var_icd_filenames) {
        if (!add_env_var_vk_icd_filenames.empty()) {
            add_env_var_vk_icd_filenames += OS_ENV_VAR_LIST_SEPARATOR;
        }
        add_env_var_vk_icd_filenames += (icd_folder.location() / full_json_name).str();
        set_env_var("VK_ADD_DRIVER_FILES", add_env_var_vk_icd_filenames);
    } else {
        platform_shim->add_manifest(ManifestCategory::icd, driver_loc);
    }
}
void FrameworkEnvironment::add_implicit_layer(ManifestLayer layer_manifest, const std::string& json_name) noexcept {
    add_layer_impl(TestLayerDetails{layer_manifest, json_name}, implicit_layer_folder, ManifestCategory::implicit_layer);
}
void FrameworkEnvironment::add_explicit_layer(ManifestLayer layer_manifest, const std::string& json_name) noexcept {
    add_layer_impl(TestLayerDetails{layer_manifest, json_name}, explicit_layer_folder, ManifestCategory::explicit_layer);
}
void FrameworkEnvironment::add_fake_implicit_layer(ManifestLayer layer_manifest, const std::string& json_name) noexcept {
    TestLayerDetails fake_details{layer_manifest, json_name};
    fake_details.is_fake = true;
    add_layer_impl(fake_details, implicit_layer_folder, ManifestCategory::implicit_layer);
}
void FrameworkEnvironment::add_fake_explicit_layer(ManifestLayer layer_manifest, const std::string& json_name) noexcept {
    TestLayerDetails fake_details{layer_manifest, json_name};
    fake_details.is_fake = true;
    add_layer_impl(fake_details, explicit_layer_folder, ManifestCategory::explicit_layer);
}
void FrameworkEnvironment::add_implicit_layer(TestLayerDetails layer_details) noexcept {
    add_layer_impl(layer_details,
                   layer_details.destination_folder == nullptr ? implicit_layer_folder : *layer_details.destination_folder,
                   ManifestCategory::implicit_layer);
}
void FrameworkEnvironment::add_explicit_layer(TestLayerDetails layer_details) noexcept {
    add_layer_impl(layer_details,
                   layer_details.destination_folder == nullptr ? explicit_layer_folder : *layer_details.destination_folder,
                   ManifestCategory::explicit_layer);
}

void FrameworkEnvironment::add_layer_impl(TestLayerDetails layer_details, fs::FolderManager& folder_manager,
                                          ManifestCategory category) {
    for (auto& layer : layer_details.layer_manifest.layers) {
        size_t cur_layer_index = layers.size();
        if (!layer.lib_path.str().empty()) {
            std::string new_layer_name = layer.name + "_" + std::to_string(cur_layer_index) + "_" + layer.lib_path.filename().str();

            auto new_layer_location = folder_manager.copy_file(layer.lib_path, new_layer_name);

            // Don't load the layer binary if using any of the wrap objects layers, since it doesn't export the same interface
            // functions
            if (!layer_details.is_fake &&
                layer.lib_path.stem().str().find(fs::path(TEST_LAYER_WRAP_OBJECTS).stem().str()) == std::string::npos) {
                layers.push_back(TestLayerHandle(new_layer_location));
                layers.back().reset_layer();
            }
            layer.lib_path = new_layer_location;
        }
    }
    if (layer_details.add_to_regular_search_paths) {
        auto layer_loc = folder_manager.write_manifest(layer_details.json_name, layer_details.layer_manifest.get_manifest_str());
        platform_shim->add_manifest(category, layer_loc);
    }
}

TestICD& FrameworkEnvironment::get_test_icd(int index) noexcept { return icds[index].get_test_icd(); }
TestICD& FrameworkEnvironment::reset_icd(int index) noexcept { return icds[index].reset_icd(); }
fs::path FrameworkEnvironment::get_test_icd_path(int index) noexcept { return icds[index].get_icd_full_path(); }

TestLayer& FrameworkEnvironment::get_test_layer(int index) noexcept { return layers[index].get_test_layer(); }
TestLayer& FrameworkEnvironment::reset_layer(int index) noexcept { return layers[index].reset_layer(); }