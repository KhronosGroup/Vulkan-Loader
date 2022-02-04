/*
 * Copyright (c) 2022 The Khronos Group Inc.
 * Copyright (c) 2022 Valve Corporation
 * Copyright (c) 2022 LunarG, Inc.
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
 * Author: Mark Young <marky@lunarg.com>
 */

#include "test_environment.h"

//
// VK_EXT_debug_report
//

struct ExpectedDebugReportMessage {
    bool any_message = false;
    std::string message;
    VkDebugReportObjectTypeEXT object_type;
    VkDebugReportFlagBitsEXT flag;
    bool found = false;
};
static ExpectedDebugReportMessage expected_debug_report{};

static VkBool32 VKAPI_CALL test_DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
                                                    uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix,
                                                    const char* pMessage, void* pUserData) {
    if (objectType == expected_debug_report.object_type && 0 < (flags | expected_debug_report.flag)) {
        std::string message_str = pMessage;
        if (expected_debug_report.any_message || (std::string::npos != message_str.find(expected_debug_report.message))) {
            expected_debug_report.found = true;
        }
    }
    return VK_FALSE;
}

static void CreateDebugReportEnvironment(FrameworkEnvironment& env, VkDebugReportFlagsEXT flags,
                                         VkInstanceCreateInfo& create_info) {
    for (uint32_t icd = 0; icd < 3; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6, VK_API_VERSION_1_0));
        env.get_test_icd(icd).physical_devices.push_back({});
        env.get_test_icd(icd).physical_devices.push_back({});
    }

    // Initialize the expected output
    expected_debug_report.any_message = false;
    expected_debug_report.message = "";
    expected_debug_report.object_type = VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT;
    expected_debug_report.found = false;

    // Setup the debug report struct
    static VkDebugReportCallbackCreateInfoEXT debug_report_info{VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT};
    debug_report_info.pNext = nullptr;
    debug_report_info.flags = flags;
    debug_report_info.pfnCallback = reinterpret_cast<PFN_vkDebugReportCallbackEXT>(test_DebugReportCallback);
    debug_report_info.pUserData = nullptr;

    static std::vector<const char*> enabled_extensions;
    enabled_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pNext = &debug_report_info;
    create_info.enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size());
    create_info.ppEnabledExtensionNames = enabled_extensions.data();
}

// Test creating and destroying report with errors, but none should occur.
TEST(DebugExtensionTests, NormalCreateDestroy) {
    FrameworkEnvironment env{};
    VkInstanceCreateInfo create_info{};
    CreateDebugReportEnvironment(env, VK_DEBUG_REPORT_ERROR_BIT_EXT, create_info);

    // Make sure we don't find any errors
    expected_debug_report.any_message = true;
    expected_debug_report.object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_debug_report.flag = VK_DEBUG_REPORT_ERROR_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(env.vulkan_functions.vkCreateInstance(&create_info, nullptr, &inst), VK_SUCCESS);
    env.vulkan_functions.vkDestroyInstance(inst, nullptr);

    ASSERT_EQ(false, expected_debug_report.found);
}

// Test report (error/warning) created in vkCreateInstance with warning in vkCreateInstance
TEST(DebugExtensionTests, WarningInCreateInstance) {
    FrameworkEnvironment env{};
    VkInstanceCreateInfo create_info{};
    CreateDebugReportEnvironment(env, VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT, create_info);

    VkApplicationInfo app_info;
    app_info.apiVersion = VK_MAKE_API_VERSION(1, 1, 0, 0);
    create_info.pApplicationInfo = &app_info;

    // Make sure we find the invalid variant message
    expected_debug_report.message = "The API Variant specified";
    expected_debug_report.object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_debug_report.flag = VK_DEBUG_REPORT_WARNING_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(env.vulkan_functions.vkCreateInstance(&create_info, nullptr, &inst), VK_SUCCESS);
    env.vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Should be found because we're reporting errors and warnings
    ASSERT_EQ(true, expected_debug_report.found);
}

// Test report (error) created in vkCreateInstance with warning in vkCreateInstance
TEST(DebugExtensionTests, WarningInCreateInstanceNotLogged) {
    FrameworkEnvironment env{};
    VkInstanceCreateInfo create_info{};
    CreateDebugReportEnvironment(env, VK_DEBUG_REPORT_ERROR_BIT_EXT, create_info);

    VkApplicationInfo app_info;
    app_info.apiVersion = VK_MAKE_API_VERSION(1, 1, 0, 0);
    create_info.pApplicationInfo = &app_info;

    // Make sure we find the invalid variant message
    expected_debug_report.message = "The API Variant specified";
    expected_debug_report.object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_debug_report.flag = VK_DEBUG_REPORT_WARNING_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(env.vulkan_functions.vkCreateInstance(&create_info, nullptr, &inst), VK_SUCCESS);
    env.vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should NOT be found (because we only have errors reported in create)
    ASSERT_EQ(false, expected_debug_report.found);
}

// Test report (error/warning) created in vkCreateInstance with info in vkDestroyInstance
TEST(DebugExtensionTests, InfoInDestroyInstanceNotLogged) {
    FrameworkEnvironment env{};
    VkInstanceCreateInfo create_info{};
    CreateDebugReportEnvironment(env, VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT, create_info);

    expected_debug_report.message = "destroying all debug report callbacks";
    expected_debug_report.object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_debug_report.flag = VK_DEBUG_REPORT_INFORMATION_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(env.vulkan_functions.vkCreateInstance(&create_info, nullptr, &inst), VK_SUCCESS);
    env.vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Should be not be found
    ASSERT_EQ(false, expected_debug_report.found);
}

// Test report (info) created in vkCreateInstance with info in vkDestroyInstance
TEST(DebugExtensionTests, InfoInDestroyInstanceLogged) {
    FrameworkEnvironment env{};
    VkInstanceCreateInfo create_info{};
    CreateDebugReportEnvironment(env, VK_DEBUG_REPORT_INFORMATION_BIT_EXT, create_info);

    expected_debug_report.message = "destroying all debug report callbacks";
    expected_debug_report.object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_debug_report.flag = VK_DEBUG_REPORT_INFORMATION_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(env.vulkan_functions.vkCreateInstance(&create_info, nullptr, &inst), VK_SUCCESS);
    env.vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should be found
    ASSERT_EQ(true, expected_debug_report.found);
}

// Test report (error/warning) created in vkCreateInstance with error in vkEnumeratePhysicalDevices.
// This should not be logged because we have only defined the debug report logging for vkCreateInstance
// and vkDestroyInstance.
TEST(DebugExtensionTests, ErrorInEnumDevsNotLogged) {
    FrameworkEnvironment env{};
    VkInstanceCreateInfo create_info{};
    CreateDebugReportEnvironment(env, VK_DEBUG_REPORT_ERROR_BIT_EXT, create_info);

    // Look for the invaid count param message
    expected_debug_report.message = "VUID-vkEnumeratePhysicalDevices-pPhysicalDeviceCount-parameter";
    expected_debug_report.object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_debug_report.flag = VK_DEBUG_REPORT_ERROR_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(env.vulkan_functions.vkCreateInstance(&create_info, nullptr, &inst), VK_SUCCESS);

    ASSERT_EQ(env.vulkan_functions.vkEnumeratePhysicalDevices(inst, nullptr, nullptr), VK_ERROR_INITIALIZATION_FAILED);

    env.vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should NOT be found (because we don't have a report callback setup outside of the create/destroy instance chain)
    ASSERT_EQ(false, expected_debug_report.found);
}

// Test report created outside of vkCreateInstance with error in vkEnumeratePhysicalDevices.
// This should be logged now.
TEST(DebugExtensionTests, ErrorInEnumDevsLogged) {
    FrameworkEnvironment env{};
    VkInstanceCreateInfo create_info{};
    CreateDebugReportEnvironment(env, VK_DEBUG_REPORT_ERROR_BIT_EXT, create_info);

    // Look for the invaid count param message
    expected_debug_report.message = "VUID-vkEnumeratePhysicalDevices-pPhysicalDeviceCount-parameter";
    expected_debug_report.object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_debug_report.flag = VK_DEBUG_REPORT_ERROR_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(env.vulkan_functions.vkCreateInstance(&create_info, nullptr, &inst), VK_SUCCESS);
    PFN_vkCreateDebugReportCallbackEXT create_debug_report = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
        env.vulkan_functions.vkGetInstanceProcAddr(inst, "vkCreateDebugReportCallbackEXT"));
    PFN_vkDestroyDebugReportCallbackEXT destroy_debug_report = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
        env.vulkan_functions.vkGetInstanceProcAddr(inst, "vkDestroyDebugReportCallbackEXT"));
    ASSERT_NE(nullptr, create_debug_report);
    ASSERT_NE(nullptr, destroy_debug_report);

    // Setup the debug report struct
    static VkDebugReportCallbackCreateInfoEXT debug_report_info{VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT};
    debug_report_info.pNext = nullptr;
    debug_report_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    debug_report_info.pfnCallback = reinterpret_cast<PFN_vkDebugReportCallbackEXT>(test_DebugReportCallback);
    debug_report_info.pUserData = nullptr;

    VkDebugReportCallbackEXT callback = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, create_debug_report(inst, &debug_report_info, nullptr, &callback));
    ASSERT_EQ(env.vulkan_functions.vkEnumeratePhysicalDevices(inst, nullptr, nullptr), VK_ERROR_INITIALIZATION_FAILED);
    destroy_debug_report(inst, callback, nullptr);

    env.vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should be found
    ASSERT_EQ(true, expected_debug_report.found);
}

// Test report created outside of vkCreateInstance with info in vkEnumeratePhysicalDevices.
// This should not be logged because type is wrong.
TEST(DebugExtensionTests, InfoInEnumDevsNotLogged) {
    FrameworkEnvironment env{};
    VkInstanceCreateInfo create_info{};
    CreateDebugReportEnvironment(env, VK_DEBUG_REPORT_ERROR_BIT_EXT, create_info);

    expected_debug_report.message = "Trimming device count down by application request";
    expected_debug_report.object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_debug_report.flag = VK_DEBUG_REPORT_ERROR_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(env.vulkan_functions.vkCreateInstance(&create_info, nullptr, &inst), VK_SUCCESS);
    PFN_vkCreateDebugReportCallbackEXT create_debug_report = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
        env.vulkan_functions.vkGetInstanceProcAddr(inst, "vkCreateDebugReportCallbackEXT"));
    PFN_vkDestroyDebugReportCallbackEXT destroy_debug_report = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
        env.vulkan_functions.vkGetInstanceProcAddr(inst, "vkDestroyDebugReportCallbackEXT"));
    ASSERT_NE(nullptr, create_debug_report);
    ASSERT_NE(nullptr, destroy_debug_report);

    // Setup the debug report struct
    static VkDebugReportCallbackCreateInfoEXT debug_report_info{VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT};
    debug_report_info.pNext = nullptr;
    debug_report_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    debug_report_info.pfnCallback = reinterpret_cast<PFN_vkDebugReportCallbackEXT>(test_DebugReportCallback);
    debug_report_info.pUserData = nullptr;

    VkDebugReportCallbackEXT callback = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, create_debug_report(inst, &debug_report_info, nullptr, &callback));

    uint32_t max_count = 5;
    std::array<VkPhysicalDevice, 5> devices;
    ASSERT_EQ(env.vulkan_functions.vkEnumeratePhysicalDevices(inst, &max_count, devices.data()), VK_INCOMPLETE);
    destroy_debug_report(inst, callback, nullptr);

    env.vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should not be found (because it's info)
    ASSERT_EQ(false, expected_debug_report.found);
}

// Test report created outside of vkCreateInstance with info in vkEnumeratePhysicalDevices.
// This should be logged because type is correct.
TEST(DebugExtensionTests, InfoInEnumDevLogged) {
    FrameworkEnvironment env{};
    VkInstanceCreateInfo create_info{};
    CreateDebugReportEnvironment(env, VK_DEBUG_REPORT_ERROR_BIT_EXT, create_info);

    expected_debug_report.message = "Trimming device count down by application request";
    expected_debug_report.object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_debug_report.flag = VK_DEBUG_REPORT_ERROR_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(env.vulkan_functions.vkCreateInstance(&create_info, nullptr, &inst), VK_SUCCESS);
    PFN_vkCreateDebugReportCallbackEXT create_debug_report = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
        env.vulkan_functions.vkGetInstanceProcAddr(inst, "vkCreateDebugReportCallbackEXT"));
    PFN_vkDestroyDebugReportCallbackEXT destroy_debug_report = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
        env.vulkan_functions.vkGetInstanceProcAddr(inst, "vkDestroyDebugReportCallbackEXT"));
    ASSERT_NE(nullptr, create_debug_report);
    ASSERT_NE(nullptr, destroy_debug_report);

    // Setup the debug report struct
    static VkDebugReportCallbackCreateInfoEXT debug_report_info{VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT};
    debug_report_info.pNext = nullptr;
    debug_report_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_INFORMATION_BIT_EXT;
    debug_report_info.pfnCallback = reinterpret_cast<PFN_vkDebugReportCallbackEXT>(test_DebugReportCallback);
    debug_report_info.pUserData = nullptr;

    VkDebugReportCallbackEXT callback = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, create_debug_report(inst, &debug_report_info, nullptr, &callback));

    uint32_t max_count = 5;
    std::array<VkPhysicalDevice, 5> devices;
    ASSERT_EQ(env.vulkan_functions.vkEnumeratePhysicalDevices(inst, &max_count, devices.data()), VK_INCOMPLETE);
    destroy_debug_report(inst, callback, nullptr);

    env.vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should be found
    ASSERT_EQ(true, expected_debug_report.found);
}

// Test report created outside of vkCreateInstance with a manual info message of the wrong message flag type to be logged.
TEST(DebugExtensionTests, ManualInfoMessageWrongMessageFlag) {
    const char my_message[] = "This is my special message!";
    FrameworkEnvironment env{};
    VkInstanceCreateInfo create_info{};
    CreateDebugReportEnvironment(env, VK_DEBUG_REPORT_ERROR_BIT_EXT, create_info);

    expected_debug_report.message = my_message;
    expected_debug_report.object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_debug_report.flag = VK_DEBUG_REPORT_ERROR_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(env.vulkan_functions.vkCreateInstance(&create_info, nullptr, &inst), VK_SUCCESS);
    PFN_vkCreateDebugReportCallbackEXT create_debug_report = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
        env.vulkan_functions.vkGetInstanceProcAddr(inst, "vkCreateDebugReportCallbackEXT"));
    PFN_vkDestroyDebugReportCallbackEXT destroy_debug_report = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
        env.vulkan_functions.vkGetInstanceProcAddr(inst, "vkDestroyDebugReportCallbackEXT"));
    PFN_vkDebugReportMessageEXT log_debug_report =
        reinterpret_cast<PFN_vkDebugReportMessageEXT>(env.vulkan_functions.vkGetInstanceProcAddr(inst, "vkDebugReportMessageEXT"));
    ASSERT_NE(nullptr, create_debug_report);
    ASSERT_NE(nullptr, destroy_debug_report);
    ASSERT_NE(nullptr, log_debug_report);

    // Setup the debug report struct
    static VkDebugReportCallbackCreateInfoEXT debug_report_info{VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT};
    debug_report_info.pNext = nullptr;
    debug_report_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    debug_report_info.pfnCallback = reinterpret_cast<PFN_vkDebugReportCallbackEXT>(test_DebugReportCallback);
    debug_report_info.pUserData = nullptr;

    VkDebugReportCallbackEXT callback = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, create_debug_report(inst, &debug_report_info, nullptr, &callback));

    log_debug_report(inst, VK_DEBUG_REPORT_INFORMATION_BIT_EXT, VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT, (uint64_t)(inst), 0, 0,
                     nullptr, my_message);

    destroy_debug_report(inst, callback, nullptr);

    env.vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should not be found
    ASSERT_EQ(false, expected_debug_report.found);
}

// Test report created outside of vkCreateInstance with a manual info message of the wrong object type to be logged.
TEST(DebugExtensionTests, ManualInfoMessageWrongObjectType) {
    const char my_message[] = "This is my special message!";
    FrameworkEnvironment env{};
    VkInstanceCreateInfo create_info{};
    CreateDebugReportEnvironment(env, VK_DEBUG_REPORT_ERROR_BIT_EXT, create_info);

    // Look for any message
    expected_debug_report.any_message = true;
    expected_debug_report.message = my_message;
    expected_debug_report.object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_debug_report.flag = VK_DEBUG_REPORT_ERROR_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(env.vulkan_functions.vkCreateInstance(&create_info, nullptr, &inst), VK_SUCCESS);
    PFN_vkCreateDebugReportCallbackEXT create_debug_report = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
        env.vulkan_functions.vkGetInstanceProcAddr(inst, "vkCreateDebugReportCallbackEXT"));
    PFN_vkDestroyDebugReportCallbackEXT destroy_debug_report = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
        env.vulkan_functions.vkGetInstanceProcAddr(inst, "vkDestroyDebugReportCallbackEXT"));
    PFN_vkDebugReportMessageEXT log_debug_report =
        reinterpret_cast<PFN_vkDebugReportMessageEXT>(env.vulkan_functions.vkGetInstanceProcAddr(inst, "vkDebugReportMessageEXT"));
    ASSERT_NE(nullptr, create_debug_report);
    ASSERT_NE(nullptr, destroy_debug_report);
    ASSERT_NE(nullptr, log_debug_report);

    // Setup the debug report struct
    static VkDebugReportCallbackCreateInfoEXT debug_report_info{VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT};
    debug_report_info.pNext = nullptr;
    debug_report_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_INFORMATION_BIT_EXT;
    debug_report_info.pfnCallback = reinterpret_cast<PFN_vkDebugReportCallbackEXT>(test_DebugReportCallback);
    debug_report_info.pUserData = nullptr;

    VkDebugReportCallbackEXT callback = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, create_debug_report(inst, &debug_report_info, nullptr, &callback));

    log_debug_report(inst, VK_DEBUG_REPORT_INFORMATION_BIT_EXT, VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT, (uint64_t)(inst), 0, 0,
                     nullptr, my_message);

    destroy_debug_report(inst, callback, nullptr);

    env.vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should not be found
    ASSERT_EQ(false, expected_debug_report.found);
}

// Test report created outside of vkCreateInstance with a manual info message to be logged.
TEST(DebugExtensionTests, ManualInfoMessageLogged) {
    const char my_message[] = "This is my special message!";
    FrameworkEnvironment env{};
    VkInstanceCreateInfo create_info{};
    CreateDebugReportEnvironment(env, VK_DEBUG_REPORT_ERROR_BIT_EXT, create_info);

    expected_debug_report.message = my_message;
    expected_debug_report.object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_debug_report.flag = VK_DEBUG_REPORT_ERROR_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(env.vulkan_functions.vkCreateInstance(&create_info, nullptr, &inst), VK_SUCCESS);
    PFN_vkCreateDebugReportCallbackEXT create_debug_report = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
        env.vulkan_functions.vkGetInstanceProcAddr(inst, "vkCreateDebugReportCallbackEXT"));
    PFN_vkDestroyDebugReportCallbackEXT destroy_debug_report = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
        env.vulkan_functions.vkGetInstanceProcAddr(inst, "vkDestroyDebugReportCallbackEXT"));
    PFN_vkDebugReportMessageEXT log_debug_report =
        reinterpret_cast<PFN_vkDebugReportMessageEXT>(env.vulkan_functions.vkGetInstanceProcAddr(inst, "vkDebugReportMessageEXT"));
    ASSERT_NE(nullptr, create_debug_report);
    ASSERT_NE(nullptr, destroy_debug_report);
    ASSERT_NE(nullptr, log_debug_report);

    // Setup the debug report struct
    static VkDebugReportCallbackCreateInfoEXT debug_report_info{VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT};
    debug_report_info.pNext = nullptr;
    debug_report_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_INFORMATION_BIT_EXT;
    debug_report_info.pfnCallback = reinterpret_cast<PFN_vkDebugReportCallbackEXT>(test_DebugReportCallback);
    debug_report_info.pUserData = nullptr;

    VkDebugReportCallbackEXT callback = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, create_debug_report(inst, &debug_report_info, nullptr, &callback));

    log_debug_report(inst, VK_DEBUG_REPORT_INFORMATION_BIT_EXT, VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT, (uint64_t)(inst), 0, 0,
                     nullptr, my_message);

    destroy_debug_report(inst, callback, nullptr);

    env.vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should be found
    ASSERT_EQ(true, expected_debug_report.found);
}

//
// VK_EXT_debug_utils
//

// Test creating and destroying report (no logging)

// Test report (error/warning) created in vkCreateInstance with error in create instance (logging)

// Test report (warning) created in vkCreateInstance with error in create instance (no logging)

// Test report (error/warning) created in vkCreateInstance with error in destroy instance (logging)

// Test report (warning) created in vkCreateInstance with error in destroy instance (no logging)

// Test report (error/warning) created in vkCreateInstance with error in vkEnumeratePhysicalDevices (logging)
// (NULL == pPhysicalDeviceCount)

// Test report (error/warning) created in vkCreateInstance with info in vkEnumeratePhysicalDevices (no logging)
// (*pPhysicalDeviceCount < number)

// Test report (error/warning/info) created in vkCreateInstance with info in vkEnumeratePhysicalDevices (logging)
// (*pPhysicalDeviceCount < number)

//
// VK_EXT_debug_utils
//
