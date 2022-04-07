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
// VK_EXT_debug_report specific tests
// =========================================
//

// Prototype declaration for callback so we can use it in class utility methods
static VkBool32 VKAPI_CALL test_DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
                                                    uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix,
                                                    const char* pMessage, void* pUserData);

class DebugReportTest : public ::testing::Test {
   public:
    void VerifyExpected(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT object_type, const std::string& message) {
        if (object_type == expected_object_type && 0 < (flags | expected_flag)) {
            if (allow_any_message || (std::string::npos != message.find(expected_message))) {
                message_found = true;
            }
        }
    }

   protected:
    virtual void SetUp() {
        env = std::unique_ptr<FrameworkEnvironment>(new FrameworkEnvironment());
        for (uint32_t icd = 0; icd < 3; ++icd) {
            env->add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_0));
            env->get_test_icd(icd).physical_devices.push_back({});
            env->get_test_icd(icd).physical_devices.push_back({});
        }
        // Initialize the expected output
        allow_any_message = false;
        expected_message = "";
        expected_object_type = VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT;
        message_found = false;
    }

    VkResult CreateReportInstance(VkDebugReportFlagsEXT debug_report_flags, VkInstance* inst,
                                  VkApplicationInfo* app_info = nullptr) {
        std::vector<const char*> enabled_extensions;
        enabled_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

        // Setup the debug report struct
        VkDebugReportCallbackCreateInfoEXT debug_report_info{VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT};
        debug_report_info.pNext = nullptr;
        debug_report_info.flags = debug_report_flags;
        debug_report_info.pfnCallback = reinterpret_cast<PFN_vkDebugReportCallbackEXT>(test_DebugReportCallback);
        debug_report_info.pUserData = reinterpret_cast<void*>(this);

        // Pass it into instance create struct
        VkInstanceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pNext = &debug_report_info;
        create_info.pApplicationInfo = app_info;
        create_info.enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size());
        create_info.ppEnabledExtensionNames = enabled_extensions.data();

        return env->vulkan_functions.vkCreateInstance(&create_info, nullptr, inst);
    }

    VkResult CreateReportCallback(VkInstance inst, VkDebugReportFlagsEXT debug_report_flags, VkDebugReportCallbackEXT* callback) {
        PFN_vkCreateDebugReportCallbackEXT create_debug_report = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
            env->vulkan_functions.vkGetInstanceProcAddr(inst, "vkCreateDebugReportCallbackEXT"));
        if (nullptr == create_debug_report) {
            return VK_ERROR_INITIALIZATION_FAILED;
        }
        VkDebugReportCallbackCreateInfoEXT debug_report_info{VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT};
        debug_report_info.pNext = nullptr;
        debug_report_info.flags = debug_report_flags;
        debug_report_info.pfnCallback = reinterpret_cast<PFN_vkDebugReportCallbackEXT>(test_DebugReportCallback);
        debug_report_info.pUserData = reinterpret_cast<void*>(this);
        return create_debug_report(inst, &debug_report_info, nullptr, callback);
    }

    VkResult DestroyReportCallback(VkInstance inst, VkDebugReportCallbackEXT callback) {
        PFN_vkDestroyDebugReportCallbackEXT destroy_debug_report = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
            env->vulkan_functions.vkGetInstanceProcAddr(inst, "vkDestroyDebugReportCallbackEXT"));
        if (nullptr == destroy_debug_report) {
            return VK_ERROR_INITIALIZATION_FAILED;
        }
        destroy_debug_report(inst, callback, nullptr);
        return VK_SUCCESS;
    }

    virtual void TearDown() { env.reset(); }

    std::unique_ptr<FrameworkEnvironment> env;
    bool allow_any_message;
    std::string expected_message;
    VkDebugReportObjectTypeEXT expected_object_type;
    VkDebugReportFlagBitsEXT expected_flag;
    bool message_found;
};

// This is the actual callback prototyped above.
static VkBool32 VKAPI_CALL test_DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
                                                    uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix,
                                                    const char* pMessage, void* pUserData) {
    DebugReportTest* debug_report_test = reinterpret_cast<DebugReportTest*>(pUserData);
    debug_report_test->VerifyExpected(flags, objectType, pMessage);
    return VK_FALSE;
}

class CreateDestroyInstanceReport : public DebugReportTest {};
class SeparateReport : public DebugReportTest {};
class ManualReport : public DebugReportTest {};

// Test creating and destroying instance looking for errors, but none should occur.
TEST_F(CreateDestroyInstanceReport, NoCallback) {
    // Make sure we don't find any errors
    allow_any_message = true;
    expected_object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_flag = VK_DEBUG_REPORT_ERROR_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateReportInstance(VK_DEBUG_REPORT_ERROR_BIT_EXT, &inst));
    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    ASSERT_EQ(false, message_found);
}

// Test report (error) created in vkCreateInstance with warning in vkCreateInstance
TEST_F(CreateDestroyInstanceReport, WarnInCreateIgnored) {
    expected_message = "The API Variant specified";
    expected_object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_flag = VK_DEBUG_REPORT_WARNING_BIT_EXT;

    VkApplicationInfo app_info;
    app_info.apiVersion = VK_MAKE_API_VERSION(1, 1, 0, 0);
    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateReportInstance(VK_DEBUG_REPORT_ERROR_BIT_EXT, &inst, &app_info));
    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should NOT be found (because we only have errors reported in create)
    ASSERT_EQ(false, message_found);
}

// Test creating and destroying instance looking for errors, but none should occur.
TEST_F(CreateDestroyInstanceReport, WarnInCreate) {
    expected_message = "The API Variant specified";
    expected_object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_flag = VK_DEBUG_REPORT_WARNING_BIT_EXT;

    VkApplicationInfo app_info;
    app_info.apiVersion = VK_MAKE_API_VERSION(1, 1, 0, 0);
    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateReportInstance(VK_DEBUG_REPORT_WARNING_BIT_EXT, &inst, &app_info));
    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should be found (because we only have errors reported in create)
    ASSERT_EQ(true, message_found);
}

// Test report (error/warning) created in vkCreateInstance with info in vkDestroyInstance
TEST_F(CreateDestroyInstanceReport, InfoInDestroyIgnored) {
    expected_message = "destroying temporary instance debug report callback";
    expected_object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_flag = VK_DEBUG_REPORT_INFORMATION_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateReportInstance(VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT, &inst));
    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Should be not be found
    ASSERT_EQ(false, message_found);
}

// Test report (info) created in vkCreateInstance with info in vkDestroyInstance
TEST_F(CreateDestroyInstanceReport, InfoInDestroy) {
    expected_message = "destroying temporary instance debug report callback";
    expected_object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_flag = VK_DEBUG_REPORT_INFORMATION_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateReportInstance(VK_DEBUG_REPORT_INFORMATION_BIT_EXT, &inst));
    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should be found
    ASSERT_EQ(true, message_found);
}

// Test report (error/warning) created in vkCreateInstance with error in vkEnumeratePhysicalDevices.
// This should not be logged because we have only defined the debug report logging for vkCreateInstance
// and vkDestroyInstance.
TEST_F(SeparateReport, ErrorInEnumDevsNoCallback) {
    // Look for the invaid count param message
    expected_message = "VUID-vkEnumeratePhysicalDevices-pPhysicalDeviceCount-parameter";
    expected_object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_flag = VK_DEBUG_REPORT_ERROR_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateReportInstance(VK_DEBUG_REPORT_ERROR_BIT_EXT, &inst));

    ASSERT_EQ(env->vulkan_functions.vkEnumeratePhysicalDevices(inst, nullptr, nullptr), VK_ERROR_INITIALIZATION_FAILED);

    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should NOT be found (because we don't have a report callback setup outside of the create/destroy instance chain)
    ASSERT_EQ(false, message_found);
}

// Test report created outside of vkCreateInstance with error in vkEnumeratePhysicalDevices.
// This should be logged now.
TEST_F(SeparateReport, ErrorInEnumDevs) {
    // Look for the invaid count param message
    expected_message = "VUID-vkEnumeratePhysicalDevices-pPhysicalDeviceCount-parameter";
    expected_object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_flag = VK_DEBUG_REPORT_ERROR_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateReportInstance(VK_DEBUG_REPORT_ERROR_BIT_EXT, &inst));

    VkDebugReportCallbackEXT callback;
    ASSERT_EQ(VK_SUCCESS, CreateReportCallback(inst, VK_DEBUG_REPORT_ERROR_BIT_EXT, &callback));

    ASSERT_EQ(env->vulkan_functions.vkEnumeratePhysicalDevices(inst, nullptr, nullptr), VK_ERROR_INITIALIZATION_FAILED);

    ASSERT_EQ(VK_SUCCESS, DestroyReportCallback(inst, callback));

    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should be found
    ASSERT_EQ(true, message_found);
}

// Test report created outside of vkCreateInstance with info in vkEnumeratePhysicalDevices.
// This should not be logged because type is wrong.
TEST_F(SeparateReport, InfoInEnumDevsIgnored) {
    expected_message = "Trimming device count from 6 to 5";
    expected_object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_flag = VK_DEBUG_REPORT_INFORMATION_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateReportInstance(VK_DEBUG_REPORT_ERROR_BIT_EXT, &inst));

    VkDebugReportCallbackEXT callback;
    ASSERT_EQ(VK_SUCCESS, CreateReportCallback(inst, VK_DEBUG_REPORT_ERROR_BIT_EXT, &callback));

    uint32_t max_count = 5;
    std::array<VkPhysicalDevice, 5> devices;
    ASSERT_EQ(env->vulkan_functions.vkEnumeratePhysicalDevices(inst, &max_count, devices.data()), VK_INCOMPLETE);

    ASSERT_EQ(VK_SUCCESS, DestroyReportCallback(inst, callback));

    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should not be found (because it's info)
    ASSERT_EQ(false, message_found);
}

// Test report created outside of vkCreateInstance with info in vkEnumeratePhysicalDevices.
// This should be logged because type is correct.
TEST_F(SeparateReport, InfoInEnumDevs) {
    expected_message = "Trimming device count from 6 to 5";
    expected_object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_flag = VK_DEBUG_REPORT_INFORMATION_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateReportInstance(VK_DEBUG_REPORT_ERROR_BIT_EXT, &inst));

    VkDebugReportCallbackEXT callback;
    ASSERT_EQ(VK_SUCCESS, CreateReportCallback(inst, VK_DEBUG_REPORT_INFORMATION_BIT_EXT, &callback));

    uint32_t max_count = 5;
    std::array<VkPhysicalDevice, 5> devices;
    ASSERT_EQ(env->vulkan_functions.vkEnumeratePhysicalDevices(inst, &max_count, devices.data()), VK_INCOMPLETE);

    ASSERT_EQ(VK_SUCCESS, DestroyReportCallback(inst, callback));

    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should be found
    ASSERT_EQ(true, message_found);
}

// Test report created outside of vkCreateInstance with a manual info message of the wrong message flag type to be logged.
TEST_F(ManualReport, InfoIgnoredWrongType) {
    const char my_message[] = "This is my special message!";
    expected_message = my_message;
    expected_object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_flag = VK_DEBUG_REPORT_INFORMATION_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateReportInstance(VK_DEBUG_REPORT_ERROR_BIT_EXT, &inst));

    VkDebugReportCallbackEXT callback;
    ASSERT_EQ(VK_SUCCESS, CreateReportCallback(inst, VK_DEBUG_REPORT_ERROR_BIT_EXT, &callback));

    PFN_vkDebugReportMessageEXT log_debug_report =
        reinterpret_cast<PFN_vkDebugReportMessageEXT>(env->vulkan_functions.vkGetInstanceProcAddr(inst, "vkDebugReportMessageEXT"));
    ASSERT_NE(nullptr, log_debug_report);
    log_debug_report(inst, VK_DEBUG_REPORT_INFORMATION_BIT_EXT, VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT, (uint64_t)(inst), 0, 0,
                     nullptr, my_message);

    ASSERT_EQ(VK_SUCCESS, DestroyReportCallback(inst, callback));

    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should not be found
    ASSERT_EQ(false, message_found);
}

// Test report created outside of vkCreateInstance with a manual info message of the wrong object type to be logged.
TEST_F(ManualReport, InfoIgnoredWrongObject) {
    const char my_message[] = "This is my special message!";
    expected_message = my_message;
    expected_object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_flag = VK_DEBUG_REPORT_INFORMATION_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateReportInstance(VK_DEBUG_REPORT_ERROR_BIT_EXT, &inst));

    VkDebugReportCallbackEXT callback;
    ASSERT_EQ(VK_SUCCESS, CreateReportCallback(inst, VK_DEBUG_REPORT_INFORMATION_BIT_EXT, &callback));

    PFN_vkDebugReportMessageEXT log_debug_report =
        reinterpret_cast<PFN_vkDebugReportMessageEXT>(env->vulkan_functions.vkGetInstanceProcAddr(inst, "vkDebugReportMessageEXT"));
    ASSERT_NE(nullptr, log_debug_report);
    log_debug_report(inst, VK_DEBUG_REPORT_INFORMATION_BIT_EXT, VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT, (uint64_t)(inst), 0, 0,
                     nullptr, my_message);

    ASSERT_EQ(VK_SUCCESS, DestroyReportCallback(inst, callback));

    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should not be found
    ASSERT_EQ(false, message_found);
}

// Test report created outside of vkCreateInstance with a manual info message to be logged.
TEST_F(ManualReport, InfoMessage) {
    const char my_message[] = "This is my special message!";
    expected_message = my_message;
    expected_object_type = VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT;
    expected_flag = VK_DEBUG_REPORT_INFORMATION_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateReportInstance(VK_DEBUG_REPORT_ERROR_BIT_EXT, &inst));

    VkDebugReportCallbackEXT callback;
    ASSERT_EQ(VK_SUCCESS, CreateReportCallback(inst, VK_DEBUG_REPORT_INFORMATION_BIT_EXT, &callback));

    PFN_vkDebugReportMessageEXT log_debug_report =
        reinterpret_cast<PFN_vkDebugReportMessageEXT>(env->vulkan_functions.vkGetInstanceProcAddr(inst, "vkDebugReportMessageEXT"));
    ASSERT_NE(nullptr, log_debug_report);
    log_debug_report(inst, VK_DEBUG_REPORT_INFORMATION_BIT_EXT, VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT, (uint64_t)(inst), 0, 0,
                     nullptr, my_message);

    ASSERT_EQ(VK_SUCCESS, DestroyReportCallback(inst, callback));

    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should be found
    ASSERT_EQ(true, message_found);
}

//
// VK_EXT_debug_util specific tests
// =========================================
//

// Prototype declaration for callback so we can use it in class utility methods
static VkBool32 VKAPI_CALL test_DebugUtilsCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                   VkDebugUtilsMessageTypeFlagsEXT message_types,
                                                   const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);

class DebugUtilTest : public ::testing::Test {
   public:
    void VerifyExpected(VkDebugUtilsMessageTypeFlagsEXT message_flags, VkDebugUtilsMessageSeverityFlagsEXT severity_flags,
                        const std::string& message, const VkDebugUtilsMessengerCallbackDataEXT* callback_data) {
        if ((0 < (severity_flags | expected_severity_flags)) && (0 < (message_flags | expected_message_flags))) {
            if (allow_any_message || (std::string::npos != message.find(expected_message))) {
                for (uint32_t obj = 0; obj < callback_data->objectCount; ++obj) {
                    if (callback_data->pObjects[obj].objectType == expected_object_type &&
                        (!check_object_handle || callback_data->pObjects[obj].objectHandle == expected_object_handle)) {
                        message_found = true;
                        break;
                    }
                }
            }
        }
    }

   protected:
    virtual void SetUp() {
        env = std::unique_ptr<FrameworkEnvironment>(new FrameworkEnvironment());
        for (uint32_t icd = 0; icd < 3; ++icd) {
            env->add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_0));
            env->get_test_icd(icd).physical_devices.push_back({});
            env->get_test_icd(icd).physical_devices.push_back({});
        }
        // Initialize the expected output
        allow_any_message = false;
        expected_message = "";
        expected_object_type = VK_OBJECT_TYPE_UNKNOWN;
        check_object_handle = false;
        expected_object_handle = 0;
        message_found = false;
    }

    VkResult CreateUtilsInstance(VkDebugUtilsMessageTypeFlagsEXT types, VkDebugUtilsMessageSeverityFlagsEXT severities,
                                 VkInstance* inst, VkApplicationInfo* app_info = nullptr) {
        std::vector<const char*> enabled_extensions;
        enabled_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        // Setup the debug utils struct
        VkDebugUtilsMessengerCreateInfoEXT debug_utils_info{VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
        debug_utils_info.pNext = nullptr;
        debug_utils_info.messageSeverity = severities;
        debug_utils_info.messageType = types;
        debug_utils_info.pfnUserCallback = reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(test_DebugUtilsCallback);
        debug_utils_info.pUserData = reinterpret_cast<void*>(this);

        VkInstanceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pNext = &debug_utils_info;
        create_info.pApplicationInfo = app_info;
        create_info.enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size());
        create_info.ppEnabledExtensionNames = enabled_extensions.data();
        return env->vulkan_functions.vkCreateInstance(&create_info, nullptr, inst);
    }

    VkResult CreateUtilsMessenger(VkInstance inst, VkDebugUtilsMessageTypeFlagsEXT types,
                                  VkDebugUtilsMessageSeverityFlagsEXT severities, VkDebugUtilsMessengerEXT* messenger) {
        PFN_vkCreateDebugUtilsMessengerEXT create_messenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            env->vulkan_functions.vkGetInstanceProcAddr(inst, "vkCreateDebugUtilsMessengerEXT"));
        if (nullptr == create_messenger) {
            return VK_ERROR_INITIALIZATION_FAILED;
        }
        VkDebugUtilsMessengerCreateInfoEXT debug_utils_info{VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
        debug_utils_info.pNext = nullptr;
        debug_utils_info.messageSeverity = severities;
        debug_utils_info.messageType = types;
        debug_utils_info.pfnUserCallback = reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(test_DebugUtilsCallback);
        debug_utils_info.pUserData = reinterpret_cast<void*>(this);
        return create_messenger(inst, &debug_utils_info, nullptr, messenger);
    }

    VkResult DestroyUtilsMessenger(VkInstance inst, VkDebugUtilsMessengerEXT messenger) {
        PFN_vkDestroyDebugUtilsMessengerEXT destroy_messenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            env->vulkan_functions.vkGetInstanceProcAddr(inst, "vkDestroyDebugUtilsMessengerEXT"));
        if (nullptr == destroy_messenger) {
            return VK_ERROR_INITIALIZATION_FAILED;
        }
        destroy_messenger(inst, messenger, nullptr);
        return VK_SUCCESS;
    }

    virtual void TearDown() { env.reset(); }

    std::unique_ptr<FrameworkEnvironment> env;
    bool allow_any_message;
    std::string expected_message;
    VkObjectType expected_object_type;
    bool check_object_handle;
    uint64_t expected_object_handle;
    VkDebugUtilsMessageTypeFlagsEXT expected_message_flags;
    VkDebugUtilsMessageSeverityFlagsEXT expected_severity_flags;
    bool message_found;
};

// This is the actual callback prototyped above.

static VkBool32 VKAPI_CALL test_DebugUtilsCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                   VkDebugUtilsMessageTypeFlagsEXT message_types,
                                                   const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) {
    DebugUtilTest* debug_util_test = reinterpret_cast<DebugUtilTest*>(user_data);
    debug_util_test->VerifyExpected(message_types, message_severity, callback_data->pMessage, callback_data);
    return VK_FALSE;
}

class CreateDestroyInstanceMessenger : public DebugUtilTest {};
class SeparateMessenger : public DebugUtilTest {};
class ManualMessage : public DebugUtilTest {};

// Test creating and destroying instance looking for errors, but none should occur.
TEST_F(CreateDestroyInstanceMessenger, NoCallback) {
    // Make sure we don't find any errors
    allow_any_message = true;
    expected_object_type = VK_OBJECT_TYPE_INSTANCE;
    expected_message_flags = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
    expected_severity_flags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS,
              CreateUtilsInstance(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, &inst));
    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    ASSERT_EQ(false, message_found);
}

// Test debug utils (error) created in vkCreateInstance with warning in vkCreateInstance
TEST_F(CreateDestroyInstanceMessenger, WarnInCreateIgnored) {
    expected_message = "The API Variant specified";
    expected_object_type = VK_OBJECT_TYPE_INSTANCE;
    expected_message_flags = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
    expected_severity_flags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;

    VkApplicationInfo app_info;
    app_info.apiVersion = VK_MAKE_API_VERSION(1, 1, 0, 0);

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateUtilsInstance(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
                                              VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, &inst, &app_info));
    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    ASSERT_EQ(false, message_found);
}

// Test debug utils (error/warning) created in vkCreateInstance with warning in vkCreateInstance
TEST_F(CreateDestroyInstanceMessenger, WarnInCreate) {
    expected_message = "The API Variant specified";
    expected_object_type = VK_OBJECT_TYPE_INSTANCE;
    expected_message_flags = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
    expected_severity_flags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;

    VkApplicationInfo app_info;
    app_info.apiVersion = VK_MAKE_API_VERSION(1, 1, 0, 0);

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateUtilsInstance(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
                                              VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, &inst, &app_info));
    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    ASSERT_EQ(true, message_found);
}

// Test debug utils error/warn created in vkCreateInstance with info in vkDestroyInstance
TEST_F(CreateDestroyInstanceMessenger, DestroyInfoIgnoredSeverity) {
    expected_message = "destroying temporary instance debug util messenger";
    expected_object_type = VK_OBJECT_TYPE_INSTANCE;
    expected_message_flags = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
    expected_severity_flags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

    VkApplicationInfo app_info;
    app_info.apiVersion = VK_MAKE_API_VERSION(1, 1, 0, 0);

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS,
              CreateUtilsInstance(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
                                  &inst, &app_info));
    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    ASSERT_EQ(false, message_found);
}

// Test debug utils info/performance created in vkCreateInstance with info/general in vkDestroyInstance
TEST_F(CreateDestroyInstanceMessenger, DestroyInfoIgnoredType) {
    expected_message = "destroying temporary instance debug util messenger";
    expected_object_type = VK_OBJECT_TYPE_INSTANCE;
    expected_message_flags = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
    expected_severity_flags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

    VkApplicationInfo app_info;
    app_info.apiVersion = VK_MAKE_API_VERSION(1, 1, 0, 0);

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateUtilsInstance(VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                                              VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, &inst, &app_info));
    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    ASSERT_EQ(false, message_found);
}

// Test debug utils info/general created in vkCreateInstance with info/general in vkDestroyInstance
TEST_F(CreateDestroyInstanceMessenger, DestroyInfo) {
    expected_message = "destroying temporary instance debug util messenger";
    expected_object_type = VK_OBJECT_TYPE_INSTANCE;
    expected_message_flags = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
    expected_severity_flags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

    VkApplicationInfo app_info;
    app_info.apiVersion = VK_MAKE_API_VERSION(1, 1, 0, 0);

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateUtilsInstance(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
                                              VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, &inst, &app_info));
    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    ASSERT_EQ(true, message_found);
}

// Test debug utils error created in vkCreateInstance with error in vkEnumeratePhysicalDevices.
// This should not be logged because we have only defined the debug utils logging for vkCreateInstance
// and vkDestroyInstance.
TEST_F(SeparateMessenger, ErrorInEnumDevsNoMessenger) {
    expected_message = "VUID-vkEnumeratePhysicalDevices-pPhysicalDeviceCount-parameter";
    expected_object_type = VK_OBJECT_TYPE_INSTANCE;
    expected_message_flags = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    expected_severity_flags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS,
              CreateUtilsInstance(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, &inst));

    ASSERT_EQ(env->vulkan_functions.vkEnumeratePhysicalDevices(inst, nullptr, nullptr), VK_ERROR_INITIALIZATION_FAILED);

    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should NOT be found (because we don't have a report callback setup outside of the create/destroy instance chain)
    ASSERT_EQ(false, message_found);
}

// Test debug utils created outside of vkCreateInstance with error in vkEnumeratePhysicalDevices, but, with the wrong
// message type so it still won't be logged.
TEST_F(SeparateMessenger, ErrorInEnumDevsWrongType) {
    expected_message = "VUID-vkEnumeratePhysicalDevices-pPhysicalDeviceCount-parameter";
    expected_object_type = VK_OBJECT_TYPE_INSTANCE;
    expected_message_flags = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    expected_severity_flags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS,
              CreateUtilsInstance(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, &inst));

    // Create the debug utils messenger to collect performance warnings and errors
    VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS,
              CreateUtilsMessenger(inst, VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
                                   &messenger));

    ASSERT_EQ(env->vulkan_functions.vkEnumeratePhysicalDevices(inst, nullptr, nullptr), VK_ERROR_INITIALIZATION_FAILED);

    // Now that we should have gotten our message, destroy the messenger
    ASSERT_EQ(VK_SUCCESS, DestroyUtilsMessenger(inst, messenger));

    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    ASSERT_EQ(false, message_found);
}

// Test debug utils created outside of vkCreateInstance with error in vkEnumeratePhysicalDevices, but, with the wrong
// message severity so it still won't be logged.
TEST_F(SeparateMessenger, ErrorInEnumDevsWrongSeverity) {
    expected_message = "VUID-vkEnumeratePhysicalDevices-pPhysicalDeviceCount-parameter";
    expected_object_type = VK_OBJECT_TYPE_INSTANCE;
    expected_message_flags = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    expected_severity_flags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS,
              CreateUtilsInstance(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, &inst));

    // Create the debug utils messenger to collect only validation warnings
    VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateUtilsMessenger(inst, VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
                                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, &messenger));

    ASSERT_EQ(env->vulkan_functions.vkEnumeratePhysicalDevices(inst, nullptr, nullptr), VK_ERROR_INITIALIZATION_FAILED);

    // Now that we should have gotten our message, destroy the messenger
    ASSERT_EQ(VK_SUCCESS, DestroyUtilsMessenger(inst, messenger));

    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    ASSERT_EQ(false, message_found);
}

// Test debug utils created outside of vkCreateInstance with error in vkEnumeratePhysicalDevices with the correct type.
TEST_F(SeparateMessenger, ErrorInEnumDevs) {
    expected_message = "VUID-vkEnumeratePhysicalDevices-pPhysicalDeviceCount-parameter";
    expected_object_type = VK_OBJECT_TYPE_INSTANCE;
    expected_message_flags = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    expected_severity_flags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS,
              CreateUtilsInstance(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, &inst));

    // Create the debug utils messenger to collect validation errors
    VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateUtilsMessenger(inst, VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
                                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, &messenger));

    ASSERT_EQ(env->vulkan_functions.vkEnumeratePhysicalDevices(inst, nullptr, nullptr), VK_ERROR_INITIALIZATION_FAILED);

    // Now that we should have gotten our message, destroy the messenger
    ASSERT_EQ(VK_SUCCESS, DestroyUtilsMessenger(inst, messenger));

    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    ASSERT_EQ(true, message_found);
}

// Test debug utils created outside of vkCreateInstance with info in vkEnumeratePhysicalDevices.
// This should not be logged because type is wrong.
TEST_F(SeparateMessenger, InfoInEnumDevsIgnoredType) {
    expected_message = "Trimming device count from 6 to 5";
    expected_object_type = VK_OBJECT_TYPE_INSTANCE;
    expected_message_flags = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
    expected_severity_flags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS,
              CreateUtilsInstance(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, &inst));

    // Create the debug utils messenger to collect validation info
    VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateUtilsMessenger(inst, VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
                                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, &messenger));

    uint32_t max_count = 5;
    std::array<VkPhysicalDevice, 5> devices;
    ASSERT_EQ(env->vulkan_functions.vkEnumeratePhysicalDevices(inst, &max_count, devices.data()), VK_INCOMPLETE);

    // Now that we should have gotten our message, destroy the messenger
    ASSERT_EQ(VK_SUCCESS, DestroyUtilsMessenger(inst, messenger));

    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should not be found (because it's info)
    ASSERT_EQ(false, message_found);
}

// Test debug utils created outside of vkCreateInstance with info in vkEnumeratePhysicalDevices.
// This should not be logged because severity is wrong.
TEST_F(SeparateMessenger, InfoInEnumDevsIgnoredSeverity) {
    expected_message = "Trimming device count from 6 to 5";
    expected_object_type = VK_OBJECT_TYPE_INSTANCE;
    expected_message_flags = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
    expected_severity_flags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS,
              CreateUtilsInstance(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, &inst));

    // Create the debug utils messenger to collect general errors/warnings
    VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS,
              CreateUtilsMessenger(inst, VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
                                   &messenger));

    uint32_t max_count = 5;
    std::array<VkPhysicalDevice, 5> devices;
    ASSERT_EQ(env->vulkan_functions.vkEnumeratePhysicalDevices(inst, &max_count, devices.data()), VK_INCOMPLETE);

    // Now that we should have gotten our message, destroy the messenger
    ASSERT_EQ(VK_SUCCESS, DestroyUtilsMessenger(inst, messenger));

    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should not be found (because it's info)
    ASSERT_EQ(false, message_found);
}

// Test debug utils created outside of vkCreateInstance with info in vkEnumeratePhysicalDevices.
TEST_F(SeparateMessenger, InfoInEnumDevs) {
    expected_message = "Trimming device count from 6 to 5";
    expected_object_type = VK_OBJECT_TYPE_INSTANCE;
    expected_message_flags = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
    expected_severity_flags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS,
              CreateUtilsInstance(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, &inst));
    ASSERT_EQ(false, message_found);

    // Create the debug utils messenger to collect general errors/warnings
    VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateUtilsMessenger(inst, VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
                                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, &messenger));

    uint32_t max_count = 5;
    std::array<VkPhysicalDevice, 5> devices;
    ASSERT_EQ(env->vulkan_functions.vkEnumeratePhysicalDevices(inst, &max_count, devices.data()), VK_INCOMPLETE);

    // Now that we should have gotten our message, destroy the messenger
    ASSERT_EQ(VK_SUCCESS, DestroyUtilsMessenger(inst, messenger));

    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should be found
    ASSERT_EQ(true, message_found);
}

// Test messenger created outside of vkCreateInstance with a manual info message of the wrong message severity to be logged.
TEST_F(ManualMessage, InfoMessageIgnoredSeverity) {
    const char my_message[] = "This is my special message!";
    expected_message = my_message;
    expected_object_type = VK_OBJECT_TYPE_INSTANCE;
    expected_message_flags = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
    expected_severity_flags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateUtilsInstance(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
                                              VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, &inst));

    // Create the debug utils messenger to collect general errors/warnings
    VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS,
              CreateUtilsMessenger(inst, VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
                                   &messenger));

    // Trigger the message
    PFN_vkSubmitDebugUtilsMessageEXT submit_message = reinterpret_cast<PFN_vkSubmitDebugUtilsMessageEXT>(
        env->vulkan_functions.vkGetInstanceProcAddr(inst, "vkSubmitDebugUtilsMessageEXT"));
    ASSERT_NE(nullptr, submit_message);

    VkDebugUtilsObjectNameInfoEXT object{VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT};
    object.objectType = VK_OBJECT_TYPE_INSTANCE;
    object.objectHandle = (uint64_t)inst;
    VkDebugUtilsMessengerCallbackDataEXT message_data{VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CALLBACK_DATA_EXT};
    message_data.pMessage = my_message;
    message_data.objectCount = 1;
    message_data.pObjects = &object;
    submit_message(inst, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT, &message_data);

    // Now that we should have gotten our message, destroy the messenger
    ASSERT_EQ(VK_SUCCESS, DestroyUtilsMessenger(inst, messenger));

    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should not be found
    ASSERT_EQ(false, message_found);
}

// Test messenger created outside of vkCreateInstance with a manual info message of the wrong object type to be logged.
TEST_F(ManualMessage, InfoMessageIgnoredObject) {
    const char my_message[] = "This is my special message!";
    expected_message = my_message;
    expected_object_type = VK_OBJECT_TYPE_INSTANCE;
    expected_message_flags = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
    expected_severity_flags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateUtilsInstance(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
                                              VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, &inst));

    // Create the debug utils messenger to collect general errors/warnings
    VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateUtilsMessenger(inst, VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
                                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, &messenger));

    // Trigger the message
    PFN_vkSubmitDebugUtilsMessageEXT submit_message = reinterpret_cast<PFN_vkSubmitDebugUtilsMessageEXT>(
        env->vulkan_functions.vkGetInstanceProcAddr(inst, "vkSubmitDebugUtilsMessageEXT"));
    ASSERT_NE(nullptr, submit_message);

    VkDebugUtilsObjectNameInfoEXT object{VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT};
    object.objectType = VK_OBJECT_TYPE_COMMAND_POOL;
    object.objectHandle = (uint64_t)inst;
    VkDebugUtilsMessengerCallbackDataEXT message_data{VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CALLBACK_DATA_EXT};
    message_data.pMessage = my_message;
    message_data.objectCount = 1;
    message_data.pObjects = &object;
    submit_message(inst, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT, &message_data);

    // Now that we should have gotten our message, destroy the messenger
    ASSERT_EQ(VK_SUCCESS, DestroyUtilsMessenger(inst, messenger));

    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should not be found
    ASSERT_EQ(false, message_found);
}

// Test messenger created outside of vkCreateInstance with a manual info message.
TEST_F(ManualMessage, InfoMessage) {
    const char my_message[] = "This is my special message!";
    expected_message = my_message;
    expected_object_type = VK_OBJECT_TYPE_INSTANCE;
    expected_message_flags = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
    expected_severity_flags = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

    VkInstance inst = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateUtilsInstance(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
                                              VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, &inst));

    // Create the debug utils messenger to collect general errors/warnings
    VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
    ASSERT_EQ(VK_SUCCESS, CreateUtilsMessenger(inst, VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
                                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, &messenger));

    // Trigger the message
    PFN_vkSubmitDebugUtilsMessageEXT submit_message = reinterpret_cast<PFN_vkSubmitDebugUtilsMessageEXT>(
        env->vulkan_functions.vkGetInstanceProcAddr(inst, "vkSubmitDebugUtilsMessageEXT"));
    ASSERT_NE(nullptr, submit_message);

    VkDebugUtilsObjectNameInfoEXT object{VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT};
    object.objectType = VK_OBJECT_TYPE_INSTANCE;
    object.objectHandle = (uint64_t)inst;
    VkDebugUtilsMessengerCallbackDataEXT message_data{VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CALLBACK_DATA_EXT};
    message_data.pMessage = my_message;
    message_data.objectCount = 1;
    message_data.pObjects = &object;
    submit_message(inst, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT, &message_data);

    // Now that we should have gotten our message, destroy the messenger
    ASSERT_EQ(VK_SUCCESS, DestroyUtilsMessenger(inst, messenger));

    env->vulkan_functions.vkDestroyInstance(inst, nullptr);

    // Message should be found
    ASSERT_EQ(true, message_found);
}
