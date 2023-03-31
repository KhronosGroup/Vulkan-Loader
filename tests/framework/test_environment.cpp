/*
 * Copyright (c) 2021-2023 The Khronos Group Inc.
 * Copyright (c) 2021-2023 Valve Corporation
 * Copyright (c) 2021-2023 LunarG, Inc.
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

fs::path get_loader_path() {
    auto loader_path = fs::path(FRAMEWORK_VULKAN_LIBRARY_PATH);
    auto env_var_res = get_env_var("VK_LOADER_TEST_LOADER_PATH", false);
    if (!env_var_res.empty()) {
        loader_path = fs::path(env_var_res);
    }
    return loader_path;
}

void init_vulkan_functions(VulkanFunctions& funcs) {
#if defined(BUILD_STATIC_LOADER)
#define GPA(name) name
#else
#define GPA(name) funcs.loader.get_symbol(#name)
#endif

    // clang-format off
    funcs.vkGetInstanceProcAddr = GPA(vkGetInstanceProcAddr);
    funcs.vkEnumerateInstanceExtensionProperties = GPA(vkEnumerateInstanceExtensionProperties);
    funcs.vkEnumerateInstanceLayerProperties = GPA(vkEnumerateInstanceLayerProperties);
    funcs.vkEnumerateInstanceVersion = GPA(vkEnumerateInstanceVersion);
    funcs.vkCreateInstance = GPA(vkCreateInstance);
    funcs.vkDestroyInstance = GPA(vkDestroyInstance);
    funcs.vkEnumeratePhysicalDevices = GPA(vkEnumeratePhysicalDevices);
    funcs.vkEnumeratePhysicalDeviceGroups = GPA(vkEnumeratePhysicalDeviceGroups);
    funcs.vkGetPhysicalDeviceFeatures = GPA(vkGetPhysicalDeviceFeatures);
    funcs.vkGetPhysicalDeviceFeatures2 = GPA(vkGetPhysicalDeviceFeatures2);
    funcs.vkGetPhysicalDeviceFormatProperties = GPA(vkGetPhysicalDeviceFormatProperties);
    funcs.vkGetPhysicalDeviceFormatProperties2 = GPA(vkGetPhysicalDeviceFormatProperties2);
    funcs.vkGetPhysicalDeviceImageFormatProperties = GPA(vkGetPhysicalDeviceImageFormatProperties);
    funcs.vkGetPhysicalDeviceImageFormatProperties2 = GPA(vkGetPhysicalDeviceImageFormatProperties2);
    funcs.vkGetPhysicalDeviceSparseImageFormatProperties = GPA(vkGetPhysicalDeviceSparseImageFormatProperties);
    funcs.vkGetPhysicalDeviceSparseImageFormatProperties2 = GPA(vkGetPhysicalDeviceSparseImageFormatProperties2);
    funcs.vkGetPhysicalDeviceProperties = GPA(vkGetPhysicalDeviceProperties);
    funcs.vkGetPhysicalDeviceProperties2 = GPA(vkGetPhysicalDeviceProperties2);
    funcs.vkGetPhysicalDeviceQueueFamilyProperties = GPA(vkGetPhysicalDeviceQueueFamilyProperties);
    funcs.vkGetPhysicalDeviceQueueFamilyProperties2 = GPA(vkGetPhysicalDeviceQueueFamilyProperties2);
    funcs.vkGetPhysicalDeviceMemoryProperties = GPA(vkGetPhysicalDeviceMemoryProperties);
    funcs.vkGetPhysicalDeviceMemoryProperties2 = GPA(vkGetPhysicalDeviceMemoryProperties2);
    funcs.vkGetPhysicalDeviceSurfaceSupportKHR = GPA(vkGetPhysicalDeviceSurfaceSupportKHR);
    funcs.vkGetPhysicalDeviceSurfaceFormatsKHR = GPA(vkGetPhysicalDeviceSurfaceFormatsKHR);
    funcs.vkGetPhysicalDeviceSurfacePresentModesKHR = GPA(vkGetPhysicalDeviceSurfacePresentModesKHR);
    funcs.vkGetPhysicalDeviceSurfaceCapabilitiesKHR = GPA(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    funcs.vkEnumerateDeviceExtensionProperties = GPA(vkEnumerateDeviceExtensionProperties);
    funcs.vkEnumerateDeviceLayerProperties = GPA(vkEnumerateDeviceLayerProperties);
    funcs.vkGetPhysicalDeviceExternalBufferProperties = GPA(vkGetPhysicalDeviceExternalBufferProperties);
    funcs.vkGetPhysicalDeviceExternalFenceProperties = GPA(vkGetPhysicalDeviceExternalFenceProperties);
    funcs.vkGetPhysicalDeviceExternalSemaphoreProperties = GPA(vkGetPhysicalDeviceExternalSemaphoreProperties);

    funcs.vkDestroySurfaceKHR = GPA(vkDestroySurfaceKHR);
    funcs.vkGetDeviceProcAddr = GPA(vkGetDeviceProcAddr);
    funcs.vkCreateDevice = GPA(vkCreateDevice);

    funcs.vkCreateHeadlessSurfaceEXT = GPA(vkCreateHeadlessSurfaceEXT);
    funcs.vkCreateDisplayPlaneSurfaceKHR = GPA(vkCreateDisplayPlaneSurfaceKHR);
    funcs.vkGetPhysicalDeviceDisplayPropertiesKHR = GPA(vkGetPhysicalDeviceDisplayPropertiesKHR);
    funcs.vkGetPhysicalDeviceDisplayPlanePropertiesKHR = GPA(vkGetPhysicalDeviceDisplayPlanePropertiesKHR);
    funcs.vkGetDisplayPlaneSupportedDisplaysKHR = GPA(vkGetDisplayPlaneSupportedDisplaysKHR);
    funcs.vkGetDisplayModePropertiesKHR = GPA(vkGetDisplayModePropertiesKHR);
    funcs.vkCreateDisplayModeKHR = GPA(vkCreateDisplayModeKHR);
    funcs.vkGetDisplayPlaneCapabilitiesKHR = GPA(vkGetDisplayPlaneCapabilitiesKHR);
    funcs.vkGetPhysicalDevicePresentRectanglesKHR = GPA(vkGetPhysicalDevicePresentRectanglesKHR);
    funcs.vkGetPhysicalDeviceDisplayProperties2KHR = GPA(vkGetPhysicalDeviceDisplayProperties2KHR);
    funcs.vkGetPhysicalDeviceDisplayPlaneProperties2KHR = GPA(vkGetPhysicalDeviceDisplayPlaneProperties2KHR);
    funcs.vkGetDisplayModeProperties2KHR = GPA(vkGetDisplayModeProperties2KHR);
    funcs.vkGetDisplayPlaneCapabilities2KHR = GPA(vkGetDisplayPlaneCapabilities2KHR);
    funcs.vkGetPhysicalDeviceSurfaceCapabilities2KHR = GPA(vkGetPhysicalDeviceSurfaceCapabilities2KHR);
    funcs.vkGetPhysicalDeviceSurfaceFormats2KHR = GPA(vkGetPhysicalDeviceSurfaceFormats2KHR);

#ifdef VK_USE_PLATFORM_ANDROID_KHR
    funcs.vkCreateAndroidSurfaceKHR = GPA(vkCreateAndroidSurfaceKHR);
#endif  // VK_USE_PLATFORM_ANDROID_KHR
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
    funcs.vkCreateDirectFBSurfaceEXT = GPA(vkCreateDirectFBSurfaceEXT);
    funcs.vkGetPhysicalDeviceDirectFBPresentationSupportEXT = GPA(vkGetPhysicalDeviceDirectFBPresentationSupportEXT);
#endif  // VK_USE_PLATFORM_DIRECTFB_EXT
#ifdef VK_USE_PLATFORM_FUCHSIA
    funcs.vkCreateImagePipeSurfaceFUCHSIA = GPA(vkCreateImagePipeSurfaceFUCHSIA);
#endif  // VK_USE_PLATFORM_FUCHSIA
#ifdef VK_USE_PLATFORM_GGP
    funcs.vkCreateStreamDescriptorSurfaceGGP = GPA(vkCreateStreamDescriptorSurfaceGGP);
#endif  // VK_USE_PLATFORM_GGP
#ifdef VK_USE_PLATFORM_IOS_MVK
    funcs.vkCreateIOSSurfaceMVK = GPA(vkCreateIOSSurfaceMVK);
#endif  // VK_USE_PLATFORM_IOS_MVK
#ifdef VK_USE_PLATFORM_MACOS_MVK
    funcs.vkCreateMacOSSurfaceMVK = GPA(vkCreateMacOSSurfaceMVK);
#endif  // VK_USE_PLATFORM_MACOS_MVK
#ifdef VK_USE_PLATFORM_METAL_EXT
    funcs.vkCreateMetalSurfaceEXT = GPA(vkCreateMetalSurfaceEXT);
#endif  // VK_USE_PLATFORM_METAL_EXT
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    funcs.vkCreateScreenSurfaceQNX = GPA(vkCreateScreenSurfaceQNX);
    funcs.vkGetPhysicalDeviceScreenPresentationSupportQNX = GPA(vkGetPhysicalDeviceScreenPresentationSupportQNX);
#endif  // VK_USE_PLATFORM_SCREEN_QNX
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    funcs.vkCreateWaylandSurfaceKHR = GPA(vkCreateWaylandSurfaceKHR);
    funcs.vkGetPhysicalDeviceWaylandPresentationSupportKHR = GPA(vkGetPhysicalDeviceWaylandPresentationSupportKHR);
#endif  // VK_USE_PLATFORM_WAYLAND_KHR
#ifdef VK_USE_PLATFORM_XCB_KHR
    funcs.vkCreateXcbSurfaceKHR = GPA(vkCreateXcbSurfaceKHR);
    funcs.vkGetPhysicalDeviceXcbPresentationSupportKHR = GPA(vkGetPhysicalDeviceXcbPresentationSupportKHR);
#endif  // VK_USE_PLATFORM_XCB_KHR
#ifdef VK_USE_PLATFORM_XLIB_KHR
    funcs.vkCreateXlibSurfaceKHR = GPA(vkCreateXlibSurfaceKHR);
    funcs.vkGetPhysicalDeviceXlibPresentationSupportKHR = GPA(vkGetPhysicalDeviceXlibPresentationSupportKHR);
#endif  // VK_USE_PLATFORM_XLIB_KHR
#ifdef VK_USE_PLATFORM_WIN32_KHR
    funcs.vkCreateWin32SurfaceKHR = GPA(vkCreateWin32SurfaceKHR);
    funcs.vkGetPhysicalDeviceWin32PresentationSupportKHR = GPA(vkGetPhysicalDeviceWin32PresentationSupportKHR);
#endif  // VK_USE_PLATFORM_WIN32_KHR

    funcs.vkDestroyDevice = GPA(vkDestroyDevice);
    funcs.vkGetDeviceQueue = GPA(vkGetDeviceQueue);
#undef GPA
    // clang-format on
}

#if defined(BUILD_STATIC_LOADER)
VulkanFunctions::VulkanFunctions() {
#else
VulkanFunctions::VulkanFunctions() : loader(get_loader_path()) {
#endif
    init_vulkan_functions(*this);
}

DeviceFunctions::DeviceFunctions(const VulkanFunctions& vulkan_functions, VkDevice device) {
    vkGetDeviceProcAddr = vulkan_functions.vkGetDeviceProcAddr;
    vkDestroyDevice = load(device, "vkDestroyDevice");
    vkGetDeviceQueue = load(device, "vkGetDeviceQueue");
    vkCreateCommandPool = load(device, "vkCreateCommandPool");
    vkAllocateCommandBuffers = load(device, "vkAllocateCommandBuffers");
    vkDestroyCommandPool = load(device, "vkDestroyCommandPool");
    vkCreateSwapchainKHR = load(device, "vkCreateSwapchainKHR");
    vkGetSwapchainImagesKHR = load(device, "vkGetSwapchainImagesKHR");
    vkDestroySwapchainKHR = load(device, "vkDestroySwapchainKHR");
}

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

void InstWrapper::CheckCreateWithInfo(InstanceCreateInfo& create_info, VkResult result_to_check) {
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

// Look through the event log. If you find a line containing the prefix we're interested in, look for the end of
// line character, and then see if the postfix occurs in it as well.
bool DebugUtilsLogger::find_prefix_then_postfix(const char* prefix, const char* postfix) const {
    size_t new_start = 0;
    size_t postfix_index = 0;
    size_t next_eol = 0;
    while ((new_start = returned_output.find(prefix, new_start)) != std::string::npos) {
        next_eol = returned_output.find("\n", new_start);
        if ((postfix_index = returned_output.find(postfix, new_start)) != std::string::npos) {
            if (postfix_index < next_eol) {
                return true;
            }
        }
        new_start = next_eol + 1;
    }
    return false;
}

bool FindPrefixPostfixStringOnLine(DebugUtilsLogger const& env_log, const char* prefix, const char* postfix) {
    return env_log.find_prefix_then_postfix(prefix, postfix);
}

PlatformShimWrapper::PlatformShimWrapper(std::vector<fs::FolderManager>* folders, bool enable_log) noexcept
    : loader_logging{"VK_LOADER_DEBUG"} {
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
        loader_logging.set_new_value("all");
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

FrameworkEnvironment::FrameworkEnvironment() noexcept : FrameworkEnvironment(true, true) {}
FrameworkEnvironment::FrameworkEnvironment(bool enable_log) noexcept : FrameworkEnvironment(enable_log, true) {}
FrameworkEnvironment::FrameworkEnvironment(bool enable_log, bool set_default_search_paths) noexcept
    : platform_shim(&folders, enable_log),
      vulkan_functions(),
      env_var_vk_icd_filenames("VK_DRIVER_FILES"),
      add_env_var_vk_icd_filenames("VK_ADD_DRIVER_FILES"),
      env_var_vk_layer_paths("VK_LAYER_PATH"),
      add_env_var_vk_layer_paths("VK_ADD_LAYER_PATH") {
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
    folders.emplace_back(FRAMEWORK_BUILD_DIRECTORY, std::string("macos_bundle"));

    platform_shim->redirect_all_paths(get_folder(ManifestLocation::null).location());
    if (set_default_search_paths) {
        platform_shim->set_fake_path(ManifestCategory::icd, get_folder(ManifestLocation::driver).location());
        platform_shim->set_fake_path(ManifestCategory::explicit_layer, get_folder(ManifestLocation::explicit_layer).location());
        platform_shim->set_fake_path(ManifestCategory::implicit_layer, get_folder(ManifestLocation::implicit_layer).location());
    }
#if defined(__APPLE__)
    // Necessary since bundles look in sub folders for manifests, not the test framework folder itself
    auto bundle_location = get_folder(ManifestLocation::macos_bundle).location();
    platform_shim->redirect_path(bundle_location / "vulkan/icd.d", bundle_location);
    platform_shim->redirect_path(bundle_location / "vulkan/explicit_layer.d", bundle_location);
    platform_shim->redirect_path(bundle_location / "vulkan/implicit_layer.d", bundle_location);
#endif
}

FrameworkEnvironment::~FrameworkEnvironment() {
    // This is necessary to prevent the folder manager from using dead memory during destruction.
    // What happens is that each folder manager tries to cleanup itself. Except, folders that were never called did not have their
    // DirEntry array's filled out. So when that folder calls delete_folder, which calls readdir, the shim tries to order the files.
    // Except, the list of files is in a object that is currently being destroyed.
    platform_shim->is_during_destruction = true;
}

TestICDHandle& FrameworkEnvironment::add_icd(TestICDDetails icd_details) noexcept {
    size_t cur_icd_index = icds.size();
    fs::FolderManager* folder = &get_folder(ManifestLocation::driver);
    if (icd_details.discovery_type == ManifestDiscoveryType::env_var ||
        icd_details.discovery_type == ManifestDiscoveryType::add_env_var) {
        folder = &get_folder(ManifestLocation::driver_env_var);
    }
    if (icd_details.discovery_type == ManifestDiscoveryType::windows_app_package) {
        folder = &get_folder(ManifestLocation::windows_app_package);
    }
    if (icd_details.discovery_type == ManifestDiscoveryType::macos_bundle) {
        folder = &get_folder(ManifestLocation::macos_bundle);
    }
    if (icd_details.discovery_type == ManifestDiscoveryType::null_dir ||
        icd_details.discovery_type == ManifestDiscoveryType::none) {
        folder = &get_folder(ManifestLocation::null);
    }
    if (!icd_details.is_fake) {
        fs::path new_driver_name = fs::path(icd_details.icd_manifest.lib_path).stem() + "_" + std::to_string(cur_icd_index) +
                                   fs::path(icd_details.icd_manifest.lib_path).extension();

        auto new_driver_location = folder->copy_file(icd_details.icd_manifest.lib_path, new_driver_name.str());

        icds.push_back(TestICDHandle(new_driver_location));
        icds.back().reset_icd();
        icd_details.icd_manifest.lib_path = new_driver_location.str();
    }
    if (icd_details.discovery_type != ManifestDiscoveryType::none) {
        std::string full_json_name = icd_details.json_name;
        if (!icd_details.disable_icd_inc) {
            full_json_name += "_" + std::to_string(cur_icd_index);
        }
        full_json_name += ".json";
        icds.back().manifest_path = folder->write_manifest(full_json_name, icd_details.icd_manifest.get_manifest_str());
        switch (icd_details.discovery_type) {
            default:
            case (ManifestDiscoveryType::generic):
                platform_shim->add_manifest(ManifestCategory::icd, icds.back().manifest_path);
                break;
            case (ManifestDiscoveryType::env_var):
                env_var_vk_icd_filenames.add_to_list((folder->location() / full_json_name).str());
                platform_shim->add_known_path(folder->location());
                break;
            case (ManifestDiscoveryType::add_env_var):
                add_env_var_vk_icd_filenames.add_to_list((folder->location() / full_json_name).str());
                platform_shim->add_known_path(folder->location());
                break;
            case (ManifestDiscoveryType::macos_bundle):
                platform_shim->add_manifest(ManifestCategory::icd, icds.back().manifest_path);
            case (ManifestDiscoveryType::null_dir):
                break;
#ifdef _WIN32
            case (ManifestDiscoveryType::windows_app_package):
                platform_shim->set_app_package_path(folder->location());
                break;
#endif
        }
    }
    return icds.back();
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
            if (layer_details.is_dir) {
                env_var_vk_layer_paths.add_to_list(fs_ptr->location().str());
            } else {
                env_var_vk_layer_paths.add_to_list((fs_ptr->location() / layer_details.json_name).str());
            }
            platform_shim->add_known_path(fs_ptr->location());
            break;
        case (ManifestDiscoveryType::add_env_var):
            fs_ptr = &get_folder(ManifestLocation::explicit_layer_add_env_var);
            if (layer_details.is_dir) {
                add_env_var_vk_layer_paths.add_to_list(fs_ptr->location().str());
            } else {
                add_env_var_vk_layer_paths.add_to_list((fs_ptr->location() / layer_details.json_name).str());
            }
            platform_shim->add_known_path(fs_ptr->location());
            break;
        case (ManifestDiscoveryType::override_folder):
            fs_ptr = &get_folder(ManifestLocation::override_layer);
            break;
        case (ManifestDiscoveryType::macos_bundle):
            fs_ptr = &(get_folder(ManifestLocation::macos_bundle));
            break;
        case (ManifestDiscoveryType::none):
        case (ManifestDiscoveryType::null_dir):
            fs_ptr = &(get_folder(ManifestLocation::null));
            break;
    }
    auto& folder = *fs_ptr;
    size_t new_layers_start = layers.size();
    for (auto& layer : layer_details.layer_manifest.layers) {
        if (!layer.lib_path.str().empty()) {
            std::string layer_binary_name = layer.lib_path.filename().str() + "_" + std::to_string(layers.size());

            auto new_layer_location = folder.copy_file(layer.lib_path, layer_binary_name);

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
        // Write a manifest file to a folder as long as the discovery type isn't none
        auto layer_loc = folder.write_manifest(layer_details.json_name, layer_details.layer_manifest.get_manifest_str());
        // only add the manifest to the registry if its a system location (as if it was installed)
        if (layer_details.discovery_type == ManifestDiscoveryType::generic) {
            platform_shim->add_manifest(category, layer_loc);
        }
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
#if defined(__APPLE__)
void FrameworkEnvironment::setup_macos_bundle() noexcept {
    platform_shim->bundle_contents = get_folder(ManifestLocation::macos_bundle).location().str();
}
#endif
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
    icd.min_icd_interface_version = std::max(icd.min_icd_interface_version, 3U);
}
void setup_WSI_in_create_instance(InstWrapper& inst, const char* api_selection) {
    inst.create_info.add_extensions({"VK_KHR_surface", get_platform_wsi_extension(api_selection)});
}

template <typename CreationFunc, typename CreateInfo>
testing::AssertionResult create_surface_helper(InstWrapper& inst, VkSurfaceKHR& surface, const char* load_func_name) {
    CreationFunc pfn_CreateSurface = inst.load(load_func_name);
    if (!pfn_CreateSurface) return testing::AssertionFailure();
    CreateInfo surf_create_info{};
    VkResult res = pfn_CreateSurface(inst, &surf_create_info, nullptr, &surface);
    return res == VK_SUCCESS ? testing::AssertionSuccess() : testing::AssertionFailure();
}
testing::AssertionResult create_surface(InstWrapper& inst, VkSurfaceKHR& surface, const char* api_selection) {
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    return create_surface_helper<PFN_vkCreateAndroidSurfaceKHR, VkAndroidSurfaceCreateInfoKHR>(inst, surface,
                                                                                               "vkCreateAndroidSurfaceKHR");
#elif defined(VK_USE_PLATFORM_DIRECTFB_EXT)
    return create_surface_helper<PFN_vkCreateDirectFBSurfaceEXT, VkDirectFBSurfaceCreateInfoEXT>(inst, surface,
                                                                                                 "vkCreateDirectFBSurfaceEXT");
#elif defined(VK_USE_PLATFORM_FUCHSIA)
    return create_surface_helper<PFN_vkCreateImagePipeSurfaceFUCHSIA, VkImagePipeSurfaceCreateInfoFUCHSIA>(
        inst, surface, "vkCreateImagePipeSurfaceFUCHSIA");
#elif defined(VK_USE_PLATFORM_GGP)
    return create_surface_helper<PFN__vkCreateStreamDescriptorSurfaceGGP, VkStreamDescriptorSurfaceCreateInfoGGP>(
        inst, surface, "vkCreateStreamDescriptorSurfaceGGP");
#elif defined(VK_USE_PLATFORM_IOS_MVK)
    return create_surface_helper<PFN_vkCreateIOSSurfaceMVK, VkIOSSurfaceCreateInfoMVK>(inst, surface, "vkCreateIOSSurfaceMVK");
#elif defined(VK_USE_PLATFORM_MACOS_MVK) || defined(VK_USE_PLATFORM_METAL_EXT)
#if defined(VK_USE_PLATFORM_MACOS_MVK)
    if (api_selection != nullptr && string_eq(api_selection, "VK_USE_PLATFORM_MACOS_MVK"))
        return create_surface_helper<PFN_vkCreateMacOSSurfaceMVK, VkMacOSSurfaceCreateInfoMVK>(inst, surface,
                                                                                               "vkCreateMacOSSurfaceMVK");
#endif
#if defined(VK_USE_PLATFORM_METAL_EXT)
    if (api_selection == nullptr || (api_selection != nullptr && string_eq(api_selection, "VK_USE_PLATFORM_METAL_EXT")))
        return create_surface_helper<PFN_vkCreateMetalSurfaceEXT, VkMetalSurfaceCreateInfoEXT>(inst, surface,
                                                                                               "vkCreateMetalSurfaceEXT");
#endif
    return testing::AssertionFailure();
#elif defined(VK_USE_PLATFORM_SCREEN_QNX)
    return create_surface_helper<PFN_vkCreateScreenSurfaceQNX, VkScreenSurfaceCreateInfoQNX>(inst, surface,
                                                                                             "vkCreateScreenSurfaceQNX");
#elif defined(VK_USE_PLATFORM_VI_NN)
    return create_surface_helper<PFN_vkCreateViSurfaceNN, VkViSurfaceCreateInfoNN>(inst, surface, "vkCreateViSurfaceNN");
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
    return create_surface_helper<PFN_vkCreateWin32SurfaceKHR, VkWin32SurfaceCreateInfoKHR>(inst, surface,
                                                                                           "vkCreateWin32SurfaceKHR");
#elif defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR) || defined(VK_USE_PLATFORM_WAYLAND_KHR)
#if defined(VK_USE_PLATFORM_XLIB_KHR)
    if (string_eq(api_selection, "VK_USE_PLATFORM_XLIB_KHR"))
        return create_surface_helper<PFN_vkCreateXlibSurfaceKHR, VkXlibSurfaceCreateInfoKHR>(inst, surface,
                                                                                             "vkCreateXlibSurfaceKHR");
#endif
#if defined(VK_USE_PLATFORM_WAYLAND_KHR)
    if (string_eq(api_selection, "VK_USE_PLATFORM_WAYLAND_KHR"))
        return create_surface_helper<PFN_vkCreateWaylandSurfaceKHR, VkWaylandSurfaceCreateInfoKHR>(inst, surface,
                                                                                                   "vkCreateWaylandSurfaceKHR");
#endif
#if defined(VK_USE_PLATFORM_XCB_KHR)
    if (api_selection == nullptr || string_eq(api_selection, "VK_USE_PLATFORM_XCB_KHR"))
        return create_surface_helper<PFN_vkCreateXcbSurfaceKHR, VkXcbSurfaceCreateInfoKHR>(inst, surface, "vkCreateXcbSurfaceKHR");
#endif
    return testing::AssertionFailure();
#else
    return create_surface_helper<PFN_vkCreateDisplayPlaneSurfaceKHR, VkDisplaySurfaceCreateInfoKHR>(
        inst, surface, "vkCreateDisplayPlaneSurfaceKHR");
#endif
}

extern "C" {
void __ubsan_on_report() { FAIL() << "Encountered an undefined behavior sanitizer error"; }
void __asan_on_error() { FAIL() << "Encountered an address sanitizer error"; }
void __tsan_on_report() { FAIL() << "Encountered a thread sanitizer error"; }
}  // extern "C"
