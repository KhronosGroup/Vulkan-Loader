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

PlatformShimWrapper::PlatformShimWrapper(std::vector<fs::FolderManager>* folders, bool enable_log) noexcept {
#if defined(WIN32) || defined(__APPLE__)
    shim_library = LibraryWrapper(SHIM_LIBRARY_NAME);
    PFN_get_platform_shim get_platform_shim_func = shim_library.get_symbol(GET_PLATFORM_SHIM_STR);
    assert(get_platform_shim_func != NULL && "Must be able to get \"platform_shim\"");
    platform_shim = get_platform_shim_func(folders);
#elif defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__)
    platform_shim = get_platform_shim(folders);
#endif
    platform_shim->reset();

    if (enable_log) {
        set_env_var("VK_LOADER_DEBUG", "all");
    }
}
PlatformShimWrapper::~PlatformShimWrapper() noexcept { platform_shim->reset(); }

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
fs::path TestICDHandle::get_icd_manifest_path() noexcept { return manifest_path; }

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
fs::path TestLayerHandle::get_layer_manifest_path() noexcept { return manifest_path; }

FrameworkEnvironment::FrameworkEnvironment() noexcept : FrameworkEnvironment(true) {}
FrameworkEnvironment::FrameworkEnvironment(bool enable_log) noexcept : platform_shim(&folders, enable_log), vulkan_functions() {
    // This order is important, it matches the enum ManifestLocation, used to index the folders vector
    folders.emplace_back(FRAMEWORK_BUILD_DIRECTORY, std::string("null_dir"));
    folders.emplace_back(FRAMEWORK_BUILD_DIRECTORY, std::string("icd_manifests"));
    folders.emplace_back(FRAMEWORK_BUILD_DIRECTORY, std::string("icd_env_vars_manifests"));
    folders.emplace_back(FRAMEWORK_BUILD_DIRECTORY, std::string("explicit_layer_manifests"));
    folders.emplace_back(FRAMEWORK_BUILD_DIRECTORY, std::string("explicit_env_var_layer_folder"));
    folders.emplace_back(FRAMEWORK_BUILD_DIRECTORY, std::string("explicit_add_env_var_layer_folder"));
    folders.emplace_back(FRAMEWORK_BUILD_DIRECTORY, std::string("implicit_layer_manifests"));
    folders.emplace_back(FRAMEWORK_BUILD_DIRECTORY, std::string("override_layer_manifests"));
    folders.emplace_back(FRAMEWORK_BUILD_DIRECTORY, std::string("app_package_manifests"));

    platform_shim->redirect_all_paths(get_folder(ManifestLocation::null).location());
    platform_shim->set_path(ManifestCategory::icd, get_folder(ManifestLocation::driver).location());
    platform_shim->set_path(ManifestCategory::explicit_layer, get_folder(ManifestLocation::explicit_layer).location());
    platform_shim->set_path(ManifestCategory::implicit_layer, get_folder(ManifestLocation::implicit_layer).location());
}

void FrameworkEnvironment::add_icd(TestICDDetails icd_details) noexcept {
    size_t cur_icd_index = icds.size();
    fs::FolderManager* folder = &get_folder(ManifestLocation::driver);
    if (icd_details.discovery_type == ManifestDiscoveryType::env_var ||
        icd_details.discovery_type == ManifestDiscoveryType::add_env_var) {
        folder = &get_folder(ManifestLocation::driver_env_var);
    }
    if (icd_details.discovery_type == ManifestDiscoveryType::windows_app_package) {
        folder = &get_folder(ManifestLocation::windows_app_package);
    }
    if (!icd_details.is_fake) {
        fs::path new_driver_name = fs::path(icd_details.icd_manifest.lib_path).stem() + "_" + std::to_string(cur_icd_index) +
                                   fs::path(icd_details.icd_manifest.lib_path).extension();

        auto new_driver_location = folder->copy_file(icd_details.icd_manifest.lib_path, new_driver_name.str());

        icds.push_back(TestICDHandle(new_driver_location));
        icds.back().reset_icd();
        icd_details.icd_manifest.lib_path = new_driver_location.str();
    }
    std::string full_json_name = icd_details.json_name + "_" + std::to_string(cur_icd_index) + ".json";

    icds.back().manifest_path = folder->write_manifest(full_json_name, icd_details.icd_manifest.get_manifest_str());
    switch (icd_details.discovery_type) {
        default:
        case (ManifestDiscoveryType::generic):
            platform_shim->add_manifest(ManifestCategory::icd, icds.back().manifest_path);
            break;
        case (ManifestDiscoveryType::env_var):
            if (!env_var_vk_icd_filenames.empty()) {
                env_var_vk_icd_filenames += OS_ENV_VAR_LIST_SEPARATOR;
            }
            env_var_vk_icd_filenames += (folder->location() / full_json_name).str();
            set_env_var("VK_DRIVER_FILES", env_var_vk_icd_filenames);
            break;
        case (ManifestDiscoveryType::add_env_var):
            if (!add_env_var_vk_icd_filenames.empty()) {
                add_env_var_vk_icd_filenames += OS_ENV_VAR_LIST_SEPARATOR;
            }
            add_env_var_vk_icd_filenames += (folder->location() / full_json_name).str();
            set_env_var("VK_ADD_DRIVER_FILES", add_env_var_vk_icd_filenames);
            break;
        case (ManifestDiscoveryType::none):
            break;
#ifdef _WIN32
        case (ManifestDiscoveryType::windows_app_package):
            platform_shim->set_app_package_path(folder->location());
            break;
#endif
    }
}

void FrameworkEnvironment::add_implicit_layer(ManifestLayer layer_manifest, const std::string& json_name) noexcept {
    add_layer_impl(TestLayerDetails{layer_manifest, json_name}, ManifestCategory::implicit_layer);
}
void FrameworkEnvironment::add_explicit_layer(ManifestLayer layer_manifest, const std::string& json_name) noexcept {
    add_layer_impl(TestLayerDetails{layer_manifest, json_name}, ManifestCategory::explicit_layer);
}
void FrameworkEnvironment::add_fake_implicit_layer(ManifestLayer layer_manifest, const std::string& json_name) noexcept {
    add_layer_impl(TestLayerDetails{layer_manifest, json_name}.set_is_fake(true), ManifestCategory::implicit_layer);
}
void FrameworkEnvironment::add_fake_explicit_layer(ManifestLayer layer_manifest, const std::string& json_name) noexcept {
    add_layer_impl(TestLayerDetails{layer_manifest, json_name}.set_is_fake(true), ManifestCategory::explicit_layer);
}
void FrameworkEnvironment::add_implicit_layer(TestLayerDetails layer_details) noexcept {
    add_layer_impl(layer_details, ManifestCategory::implicit_layer);
}
void FrameworkEnvironment::add_explicit_layer(TestLayerDetails layer_details) noexcept {
    add_layer_impl(layer_details, ManifestCategory::explicit_layer);
}

void FrameworkEnvironment::add_layer_impl(TestLayerDetails layer_details, ManifestCategory category) {
    fs::FolderManager* fs_ptr = &get_folder(ManifestLocation::explicit_layer);
    switch (layer_details.discovery_type) {
        default:
        case (ManifestDiscoveryType::generic):
            if (category == ManifestCategory::implicit_layer) fs_ptr = &get_folder(ManifestLocation::implicit_layer);
            break;
        case (ManifestDiscoveryType::env_var):
            fs_ptr = &get_folder(ManifestLocation::explicit_layer_env_var);
            if (!env_var_vk_layer_paths.empty()) {
                env_var_vk_layer_paths += OS_ENV_VAR_LIST_SEPARATOR;
            }
            env_var_vk_layer_paths += fs_ptr->location().str();
            set_env_var("VK_LAYER_PATH", env_var_vk_layer_paths);
            break;
        case (ManifestDiscoveryType::add_env_var):
            fs_ptr = &get_folder(ManifestLocation::explicit_layer_add_env_var);
            if (!add_env_var_vk_layer_paths.empty()) {
                add_env_var_vk_layer_paths += OS_ENV_VAR_LIST_SEPARATOR;
            }
            add_env_var_vk_layer_paths += fs_ptr->location().str();
            set_env_var("VK_ADD_LAYER_PATH", add_env_var_vk_layer_paths);
            break;
        case (ManifestDiscoveryType::override_folder):
            fs_ptr = &get_folder(ManifestLocation::override_layer);
            break;
        case (ManifestDiscoveryType::none):
            break;
    }
    auto& folder = *fs_ptr;
    size_t new_layers_start = layers.size();
    for (auto& layer : layer_details.layer_manifest.layers) {
        size_t cur_layer_index = layers.size();
        if (!layer.lib_path.str().empty()) {
            std::string new_layer_name = layer.name + "_" + std::to_string(cur_layer_index) + "_" + layer.lib_path.filename().str();

            auto new_layer_location = folder.copy_file(layer.lib_path, new_layer_name);

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
    if (layer_details.discovery_type != ManifestDiscoveryType::none) {
        auto layer_loc = folder.write_manifest(layer_details.json_name, layer_details.layer_manifest.get_manifest_str());
        platform_shim->add_manifest(category, layer_loc);
        for (size_t i = new_layers_start; i < layers.size(); i++) {
            layers.at(i).manifest_path = layer_loc;
        }
    }
}

TestICD& FrameworkEnvironment::get_test_icd(size_t index) noexcept { return icds[index].get_test_icd(); }
TestICD& FrameworkEnvironment::reset_icd(size_t index) noexcept { return icds[index].reset_icd(); }
fs::path FrameworkEnvironment::get_test_icd_path(size_t index) noexcept { return icds[index].get_icd_full_path(); }
fs::path FrameworkEnvironment::get_icd_manifest_path(size_t index) noexcept { return icds[index].get_icd_manifest_path(); }

TestLayer& FrameworkEnvironment::get_test_layer(size_t index) noexcept { return layers[index].get_test_layer(); }
TestLayer& FrameworkEnvironment::reset_layer(size_t index) noexcept { return layers[index].reset_layer(); }
fs::path FrameworkEnvironment::get_test_layer_path(size_t index) noexcept { return layers[index].get_layer_full_path(); }
fs::path FrameworkEnvironment::get_layer_manifest_path(size_t index) noexcept { return layers[index].get_layer_manifest_path(); }

fs::FolderManager& FrameworkEnvironment::get_folder(ManifestLocation location) noexcept {
    // index it directly using the enum location since they will always be in that order
    return folders.at(static_cast<size_t>(location));
}
const char* get_platform_wsi_extension(const char* api_selection) {
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    return "VK_KHR_android_surface";
#elif defined(VK_USE_PLATFORM_DIRECTFB_EXT)
    return "VK_EXT_directfb_surface";
#elif defined(VK_USE_PLATFORM_FUCHSIA)
    return "VK_FUCHSIA_imagepipe_surface";
#elif defined(VK_USE_PLATFORM_GGP)
    return "VK_GGP_stream_descriptor_surface";
#elif defined(VK_USE_PLATFORM_IOS_MVK)
    return "VK_MVK_ios_surface";
#elif defined(VK_USE_PLATFORM_MACOS_MVK) || defined(VK_USE_PLATFORM_METAL_EXT)
#if defined(VK_USE_PLATFORM_MACOS_MVK)
    if (string_eq(api_selection, "VK_USE_PLATFORM_MACOS_MVK")) return "VK_MVK_macos_surface";
#endif
#if defined(VK_USE_PLATFORM_METAL_EXT)
    if (string_eq(api_selection, "VK_USE_PLATFORM_METAL_EXT")) return "VK_EXT_metal_surface";
    return "VK_EXT_metal_surface";
#endif
#elif defined(VK_USE_PLATFORM_SCREEN_QNX)
    return "VK_QNX_screen_surface";
#elif defined(VK_USE_PLATFORM_VI_NN)
    return "VK_NN_vi_surface";
#elif defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR) || defined(VK_USE_PLATFORM_WAYLAND_KHR)
#if defined(VK_USE_PLATFORM_XCB_KHR)
    if (string_eq(api_selection, "VK_USE_PLATFORM_XCB_KHR")) return "VK_KHR_xcb_surface";
#endif
#if defined(VK_USE_PLATFORM_XLIB_KHR)
    if (string_eq(api_selection, "VK_USE_PLATFORM_XLIB_KHR")) return "VK_KHR_xlib_surface";
#endif
#if defined(VK_USE_PLATFORM_WAYLAND_KHR)
    if (string_eq(api_selection, "VK_USE_PLATFORM_WAYLAND_KHR")) return "VK_KHR_wayland_surface";
#endif
#if defined(VK_USE_PLATFORM_XCB_KHR)
    return "VK_KHR_xcb_surface";
#endif
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
    return "VK_KHR_win32_surface";
#else
    return "VK_KHR_display";
#endif
}

void setup_WSI_in_ICD(TestICD& icd, const char* api_selection) {
    icd.enable_icd_wsi = true;
    icd.add_instance_extensions({"VK_KHR_surface", get_platform_wsi_extension(api_selection)});
}
void setup_WSI_in_create_instance(InstWrapper& inst, const char* api_selection) {
    inst.create_info.add_extensions({"VK_KHR_surface", get_platform_wsi_extension(api_selection)});
}

template <typename CreationFunc, typename CreateInfo>
void create_surface_helper(InstWrapper& inst, VkSurfaceKHR& surface, const char* load_func_name) {
    CreationFunc pfn_CreateSurface = inst.load(load_func_name);
    CreateInfo surf_create_info{};
    ASSERT_EQ(VK_SUCCESS, pfn_CreateSurface(inst, &surf_create_info, nullptr, &surface));
}
void create_surface(InstWrapper& inst, VkSurfaceKHR& surface, const char* api_selection) {
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    create_surface_helper<PFN_vkCreateAndroidSurfaceKHR, VkAndroidSurfaceCreateInfoKHR>(inst, surface, "vkCreateAndroidSurfaceKHR");
#elif defined(VK_USE_PLATFORM_DIRECTFB_EXT)
    create_surface_helper<PFN_vkCreateDirectFBSurfaceEXT, VkDirectFBSurfaceCreateInfoEXT>(inst, surface,
                                                                                          "vkCreateDirectFBSurfaceEXT");
#elif defined(VK_USE_PLATFORM_FUCHSIA)
    create_surface_helper<PFN_vkCreateImagePipeSurfaceFUCHSIA, VkImagePipeSurfaceCreateInfoFUCHSIA>(
        inst, surface, "vkCreateImagePipeSurfaceFUCHSIA");
#elif defined(VK_USE_PLATFORM_GGP)
    create_surface_helper<PFN__vkCreateStreamDescriptorSurfaceGGP, VkStreamDescriptorSurfaceCreateInfoGGP>(
        inst, surface, "vkCreateStreamDescriptorSurfaceGGP");
#elif defined(VK_USE_PLATFORM_IOS_MVK)
    create_surface_helper<PFN_vkCreateIOSSurfaceMVK, VkIOSSurfaceCreateInfoMVK>(inst, surface, "vkCreateIOSSurfaceMVK");
#elif defined(VK_USE_PLATFORM_MACOS_MVK) || defined(VK_USE_PLATFORM_METAL_EXT)
#if defined(VK_USE_PLATFORM_MACOS_MVK)
    if (api_selection != nullptr && string_eq(api_selection, "VK_USE_PLATFORM_MACOS_MVK"))
        create_surface_helper<PFN_vkCreateMacOSSurfaceMVK, VkMacOSSurfaceCreateInfoMVK>(inst, surface, "vkCreateMacOSSurfaceMVK");
#endif
#if defined(VK_USE_PLATFORM_METAL_EXT)
    if (api_selection == nullptr || (api_selection != nullptr && string_eq(api_selection, "VK_USE_PLATFORM_METAL_EXT")))
        create_surface_helper<PFN_vkCreateMetalSurfaceEXT, VkMetalSurfaceCreateInfoEXT>(inst, surface, "vkCreateMetalSurfaceEXT");
#endif
#elif defined(VK_USE_PLATFORM_SCREEN_QNX)
    create_surface_helper<PFN_vkCreateScreenSurfaceQNX, VkScreenSurfaceCreateInfoQNX>(inst, surface, "vkCreateScreenSurfaceQNX");
#elif defined(VK_USE_PLATFORM_VI_NN)
    create_surface_helper<PFN_vkCreateViSurfaceNN, VkViSurfaceCreateInfoNN>(inst, surface, "vkCreateViSurfaceNN");
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
    create_surface_helper<PFN_vkCreateWin32SurfaceKHR, VkWin32SurfaceCreateInfoKHR>(inst, surface, "vkCreateWin32SurfaceKHR");
#elif defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR) || defined(VK_USE_PLATFORM_WAYLAND_KHR)
#if defined(VK_USE_PLATFORM_XLIB_KHR)
    if (string_eq(api_selection, "VK_USE_PLATFORM_XLIB_KHR"))
        create_surface_helper<PFN_vkCreateXlibSurfaceKHR, VkXlibSurfaceCreateInfoKHR>(inst, surface, "vkCreateXlibSurfaceKHR");
#endif
#if defined(VK_USE_PLATFORM_WAYLAND_KHR)
    if (string_eq(api_selection, "VK_USE_PLATFORM_WAYLAND_KHR"))
        create_surface_helper<PFN_vkCreateWaylandSurfaceKHR, VkWaylandSurfaceCreateInfoKHR>(inst, surface,
                                                                                            "vkCreateWaylandSurfaceKHR");
#endif
#if defined(VK_USE_PLATFORM_XCB_KHR)
    if (api_selection == nullptr || string_eq(api_selection, "VK_USE_PLATFORM_XCB_KHR"))
        create_surface_helper<PFN_vkCreateXcbSurfaceKHR, VkXcbSurfaceCreateInfoKHR>(inst, surface, "vkCreateXcbSurfaceKHR");
#endif
#else
    create_surface_helper<PFN_vkCreateDisplayPlaneSurfaceKHR, VkDisplaySurfaceCreateInfoKHR>(inst, surface,
                                                                                             "vkCreateDisplayPlaneSurfaceKHR");
#endif
    assert(surface != VK_NULL_HANDLE);
}

extern "C" {
void __ubsan_on_report() { FAIL() << "Encountered an undefined behavior sanitizer error"; }
void __asan_on_error() { FAIL() << "Encountered an address sanitizer error"; }
void __tsan_on_report() { FAIL() << "Encountered a thread sanitizer error"; }
}  // extern "C"
