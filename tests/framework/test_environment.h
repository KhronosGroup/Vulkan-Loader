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

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

// Use the NDK's header on Android
#include "gtest/gtest.h"

#include "test_util.h"

#include "shim/shim.h"

#include "icd/physical_device.h"
#include "icd/test_icd.h"

#include "layer/test_layer.h"

// handle checking
template <typename T>
void handle_assert_has_value(T const& handle) {
    ASSERT_TRUE(handle != VK_NULL_HANDLE);
}
template <typename T>
void handle_assert_null(T const& handle) {
    ASSERT_TRUE(handle == VK_NULL_HANDLE);
}
template <typename T>
void handle_assert_has_values(std::vector<T> const& handles) {
    for (auto const& handle : handles) {
        ASSERT_TRUE(handle != VK_NULL_HANDLE);
    }
}
template <typename T>
void handle_assert_no_values(std::vector<T> const& handles) {
    for (auto const& handle : handles) {
        ASSERT_TRUE(handle == VK_NULL_HANDLE);
    }
}
template <typename T>
void handle_assert_no_values(size_t length, T handles[]) {
    for (size_t i = 0; i < length; i++) {
        ASSERT_TRUE(handles[i] == VK_NULL_HANDLE);
    }
}
template <typename T>
void handle_assert_equal(T const& left, T const& right) {
    ASSERT_EQ(left, right);
}
template <typename T>
void handle_assert_equal(std::vector<T> const& left, std::vector<T> const& right) {
    ASSERT_EQ(left.size(), right.size());
    for (size_t i = 0; i < left.size(); i++) {
        ASSERT_EQ(left[i], right[i]);
    }
}
template <typename T>
void handle_assert_equal(size_t count, T left[], T right[]) {
    for (size_t i = 0; i < count; i++) {
        ASSERT_EQ(left[i], right[i]);
    }
}

// InstWrapper & DeviceWrapper - used to make creating instances & devices easier test writing
struct InstWrapper {
    InstWrapper(VulkanFunctions& functions, VkAllocationCallbacks* callbacks = nullptr) noexcept;
    InstWrapper(VulkanFunctions& functions, VkInstance inst, VkAllocationCallbacks* callbacks = nullptr) noexcept;
    ~InstWrapper() noexcept;

    // Move-nly object
    InstWrapper(InstWrapper const&) = delete;
    InstWrapper& operator=(InstWrapper const&) = delete;
    InstWrapper(InstWrapper&& other) noexcept;
    InstWrapper& operator=(InstWrapper&&) noexcept;

    // Construct this VkInstance using googletest to assert if it succeeded
    void CheckCreate(VkResult result_to_check = VK_SUCCESS);

    // Convenience
    operator VkInstance() { return inst; }
    VulkanFunctions* operator->() { return functions; }

    FromVoidStarFunc load(const char* func_name) { return FromVoidStarFunc(functions->vkGetInstanceProcAddr(inst, func_name)); }

    // Enumerate physical devices using googletest to assert if it succeeded
    std::vector<VkPhysicalDevice> GetPhysDevs(VkResult result_to_check = VK_SUCCESS);  // query all physical devices
    std::vector<VkPhysicalDevice> GetPhysDevs(uint32_t phys_dev_count,
                                              VkResult result_to_check = VK_SUCCESS);  // query only phys_dev_count devices
    // Enumerate a single physical device using googletest to assert if it succeeded
    VkPhysicalDevice GetPhysDev(VkResult result_to_check = VK_SUCCESS);

    VulkanFunctions* functions = nullptr;
    VkInstance inst = VK_NULL_HANDLE;
    VkAllocationCallbacks* callbacks = nullptr;
    InstanceCreateInfo create_info{};
};

std::vector<VkExtensionProperties> EnumerateDeviceExtensions(InstWrapper const& inst, VkPhysicalDevice physical_device);

struct DeviceWrapper {
    DeviceWrapper(InstWrapper& inst_wrapper, VkAllocationCallbacks* callbacks = nullptr) noexcept;
    DeviceWrapper(VulkanFunctions& functions, VkDevice device, VkAllocationCallbacks* callbacks = nullptr) noexcept;
    ~DeviceWrapper() noexcept;

    // Move-only object
    DeviceWrapper(DeviceWrapper const&) = delete;
    DeviceWrapper& operator=(DeviceWrapper const&) = delete;
    DeviceWrapper(DeviceWrapper&&) noexcept;
    DeviceWrapper& operator=(DeviceWrapper&&) noexcept;

    // Construct this VkDevice using googletest to assert if it succeeded
    void CheckCreate(VkPhysicalDevice physical_device, VkResult result_to_check = VK_SUCCESS);

    // Convenience
    operator VkDevice() { return dev; }
    operator VkDevice() const { return dev; }
    VulkanFunctions* operator->() { return functions; }

    FromVoidStarFunc load(const char* func_name) { return FromVoidStarFunc(functions->vkGetDeviceProcAddr(dev, func_name)); }

    VulkanFunctions* functions = nullptr;
    VkDevice dev = VK_NULL_HANDLE;
    VkAllocationCallbacks* callbacks = nullptr;
    DeviceCreateInfo create_info{};
};

struct DebugUtilsLogger {
    static VkBool32 VKAPI_PTR DebugUtilsMessengerLoggerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                                VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                                                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                                void* pUserData) {
        DebugUtilsLogger* debug = reinterpret_cast<DebugUtilsLogger*>(pUserData);
        debug->returned_output += pCallbackData->pMessage;
        debug->returned_output += '\n';
        return VK_FALSE;
    }
    DebugUtilsLogger(VkDebugUtilsMessageSeverityFlagsEXT severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        returned_output.reserve(4096);  // output can be very noisy, reserving should help prevent many small allocations
        create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        create_info.pNext = nullptr;
        create_info.messageSeverity = severity;
        create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        create_info.pfnUserCallback = DebugUtilsMessengerLoggerCallback;
        create_info.pUserData = this;
    }

    // Immoveable object
    DebugUtilsLogger(DebugUtilsLogger const&) = delete;
    DebugUtilsLogger& operator=(DebugUtilsLogger const&) = delete;
    DebugUtilsLogger(DebugUtilsLogger&&) = delete;
    DebugUtilsLogger& operator=(DebugUtilsLogger&&) = delete;
    // Find a string in the log output
    bool find(std::string const& search_text) const { return returned_output.find(search_text) != std::string::npos; }
    // Clear the log
    void clear() { returned_output.clear(); }
    VkDebugUtilsMessengerCreateInfoEXT* get() noexcept { return &create_info; }
    VkDebugUtilsMessengerCreateInfoEXT create_info{};
    std::string returned_output;
};

struct DebugUtilsWrapper {
    DebugUtilsWrapper() noexcept {}
    DebugUtilsWrapper(InstWrapper& inst_wrapper,
                      VkDebugUtilsMessageSeverityFlagsEXT severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                      VkAllocationCallbacks* callbacks = nullptr)
        : logger(severity), inst(inst_wrapper.inst), callbacks(callbacks) {
        vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            inst_wrapper.functions->vkGetInstanceProcAddr(inst_wrapper.inst, "vkCreateDebugUtilsMessengerEXT"));
        vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            inst_wrapper.functions->vkGetInstanceProcAddr(inst_wrapper.inst, "vkDestroyDebugUtilsMessengerEXT"));
    };
    ~DebugUtilsWrapper() noexcept {
        if (messenger) {
            vkDestroyDebugUtilsMessengerEXT(inst, messenger, callbacks);
        }
    }
    // Immoveable object
    DebugUtilsWrapper(DebugUtilsWrapper const&) = delete;
    DebugUtilsWrapper& operator=(DebugUtilsWrapper const&) = delete;
    DebugUtilsWrapper(DebugUtilsWrapper&&) = delete;
    DebugUtilsWrapper& operator=(DebugUtilsWrapper&&) = delete;

    bool find(std::string const& search_text) { return logger.find(search_text); }
    VkDebugUtilsMessengerCreateInfoEXT* get() noexcept { return logger.get(); }

    DebugUtilsLogger logger;
    VkInstance inst = VK_NULL_HANDLE;
    VkAllocationCallbacks* callbacks = nullptr;
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = nullptr;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = nullptr;
    VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
};

VkResult CreateDebugUtilsMessenger(DebugUtilsWrapper& debug_utils);

// Helper that adds the debug utils extension name and sets the pNext chain up
// NOTE: Ignores existing pNext chains
void FillDebugUtilsCreateDetails(InstanceCreateInfo& create_info, DebugUtilsLogger& logger);
void FillDebugUtilsCreateDetails(InstanceCreateInfo& create_info, DebugUtilsWrapper& wrapper);

struct FrameworkEnvironment;  // forward declaration

struct PlatformShimWrapper {
    PlatformShimWrapper(std::vector<fs::FolderManager>* folders) noexcept;
    ~PlatformShimWrapper() noexcept;
    PlatformShimWrapper(PlatformShimWrapper const&) = delete;
    PlatformShimWrapper& operator=(PlatformShimWrapper const&) = delete;

    // Convenience
    PlatformShim* operator->() { return platform_shim; }

    LibraryWrapper shim_library;
    PlatformShim* platform_shim;
};

struct TestICDHandle {
    TestICDHandle() noexcept;
    TestICDHandle(fs::path const& icd_path) noexcept;
    TestICD& reset_icd() noexcept;
    TestICD& get_test_icd() noexcept;
    fs::path get_icd_full_path() noexcept;
    fs::path get_icd_manifest_path() noexcept;

    // Must use statically
    LibraryWrapper icd_library;
    GetTestICDFunc proc_addr_get_test_icd = nullptr;
    GetNewTestICDFunc proc_addr_reset_icd = nullptr;
    fs::path manifest_path;
};
struct TestLayerHandle {
    TestLayerHandle() noexcept;
    TestLayerHandle(fs::path const& layer_path) noexcept;
    TestLayer& reset_layer() noexcept;
    TestLayer& get_test_layer() noexcept;
    fs::path get_layer_full_path() noexcept;
    fs::path get_layer_manifest_path() noexcept;

    // Must use statically
    LibraryWrapper layer_library;
    GetTestLayerFunc proc_addr_get_test_layer = nullptr;
    GetNewTestLayerFunc proc_addr_reset_layer = nullptr;
    fs::path manifest_path;
};

enum class ManifestDiscoveryType {
    generic,              // put the manifest in the regular locations
    none,                 // don't add to regular locations - eg D3DKMT
    env_var,              // use the corresponding env-var for it
    add_env_var,          // use the corresponding add-env-var for it
    override_folder,      // add to a special folder for the override layer to use
    windows_app_package,  // let the app package search find it
};

struct TestICDDetails {
    TestICDDetails(ManifestICD icd_manifest) noexcept : icd_manifest(icd_manifest) {}
    TestICDDetails(fs::path icd_path, uint32_t api_version = VK_API_VERSION_1_0) noexcept {
        icd_manifest.set_lib_path(icd_path.str()).set_api_version(api_version);
    }
    BUILDER_VALUE(TestICDDetails, ManifestICD, icd_manifest, {});
    BUILDER_VALUE(TestICDDetails, std::string, json_name, "test_icd");
    BUILDER_VALUE(TestICDDetails, ManifestDiscoveryType, discovery_type, ManifestDiscoveryType::generic);
    BUILDER_VALUE(TestICDDetails, bool, is_fake, false);
};

struct TestLayerDetails {
    TestLayerDetails(ManifestLayer layer_manifest, const std::string& json_name) noexcept
        : layer_manifest(layer_manifest), json_name(json_name) {}
    BUILDER_VALUE(TestLayerDetails, ManifestLayer, layer_manifest, {});
    BUILDER_VALUE(TestLayerDetails, std::string, json_name, "test_layer");
    BUILDER_VALUE(TestLayerDetails, ManifestDiscoveryType, discovery_type, ManifestDiscoveryType::generic);
    BUILDER_VALUE(TestLayerDetails, bool, is_fake, false);
};

enum class ManifestLocation {
    null = 0,
    driver = 1,
    driver_env_var = 2,
    explicit_layer = 3,
    explicit_layer_env_var = 4,
    explicit_layer_add_env_var = 5,
    implicit_layer = 6,
    override_layer = 7,
    windows_app_package = 8,
};

struct FrameworkEnvironment {
    FrameworkEnvironment() noexcept;

    void add_icd(TestICDDetails icd_details) noexcept;
    void add_implicit_layer(ManifestLayer layer_manifest, const std::string& json_name) noexcept;
    void add_implicit_layer(TestLayerDetails layer_details) noexcept;
    void add_explicit_layer(ManifestLayer layer_manifest, const std::string& json_name) noexcept;
    void add_explicit_layer(TestLayerDetails layer_details) noexcept;
    void add_fake_implicit_layer(ManifestLayer layer_manifest, const std::string& json_name) noexcept;
    void add_fake_explicit_layer(ManifestLayer layer_manifest, const std::string& json_name) noexcept;

    TestICD& get_test_icd(size_t index = 0) noexcept;
    TestICD& reset_icd(size_t index = 0) noexcept;
    fs::path get_test_icd_path(size_t index = 0) noexcept;
    fs::path get_icd_manifest_path(size_t index = 0) noexcept;

    TestLayer& get_test_layer(size_t index = 0) noexcept;
    TestLayer& reset_layer(size_t index = 0) noexcept;
    fs::path get_test_layer_path(size_t index = 0) noexcept;
    fs::path get_layer_manifest_path(size_t index = 0) noexcept;

    fs::FolderManager& get_folder(ManifestLocation location) noexcept;

    PlatformShimWrapper platform_shim;
    std::vector<fs::FolderManager> folders;

    DebugUtilsLogger debug_log;
    VulkanFunctions vulkan_functions;

    std::vector<TestICDHandle> icds;
    std::vector<TestLayerHandle> layers;

    std::string env_var_vk_icd_filenames;
    std::string add_env_var_vk_icd_filenames;

    std::string env_var_vk_layer_paths;
    std::string add_env_var_vk_layer_paths;

   private:
    void add_layer_impl(TestLayerDetails layer_details, ManifestCategory category);
};

// The following helpers setup an icd with the required extensions and setting to use with WSI
// By default they use whatever the set VK_USE_PLATFORM_XXX macros define
void setup_WSI_in_ICD(TestICD& icd);
void setup_WSI_in_create_instance(InstWrapper& inst);
// api_selection: optionally provide a VK_USE_PLATFORM_XXX string to select which API to create a surface with
// Note: MUST provide api_selection on platforms with multiple viable API's, such as linux and MacOS
VkSurfaceKHR create_surface(InstWrapper& inst, const char* api_selection = nullptr);
