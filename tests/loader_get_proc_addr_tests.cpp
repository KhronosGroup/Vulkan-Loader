/*
 * Copyright (c) 2021-2022 The Khronos Group Inc.
 * Copyright (c) 2021-2022 Valve Corporation
 * Copyright (c) 2021-2022 LunarG, Inc.
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

// Verify that the various ways to get vkGetInstanceProcAddr return the same value
TEST(GetProcAddr, VerifyGetInstanceProcAddr) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().physical_devices.emplace_back("physical_device_0");
    {
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(VK_API_VERSION_1_1);
        inst.CheckCreate();

        // NOTE: The vulkan_functions are queried using the platform get proc addr from the loader.  So we'll compare
        //       that to what is returned by asking it what the various Vulkan get proc addr functions are.
        PFN_vkGetInstanceProcAddr gipa_loader = env.vulkan_functions.vkGetInstanceProcAddr;
        PFN_vkGetInstanceProcAddr gipa_queried = reinterpret_cast<PFN_vkGetInstanceProcAddr>(
            env.vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkGetInstanceProcAddr"));
        ASSERT_EQ(gipa_loader, gipa_queried);
    }

    {
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(VK_API_VERSION_1_3);
        inst.CheckCreate();

        // NOTE: The vulkan_functions are queried using the platform get proc addr from the loader.  So we'll compare
        //       that to what is returned by asking it what the various Vulkan get proc addr functions are.
        PFN_vkGetInstanceProcAddr gipa_loader = env.vulkan_functions.vkGetInstanceProcAddr;
        PFN_vkGetInstanceProcAddr gipa_queried = reinterpret_cast<PFN_vkGetInstanceProcAddr>(
            env.vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkGetInstanceProcAddr"));
        ASSERT_EQ(gipa_loader, gipa_queried);
    }
}

// Verify that the various ways to get vkGetDeviceProcAddr return the same value
TEST(GetProcAddr, VerifyGetDeviceProcAddr) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().physical_devices.emplace_back("physical_device_0");

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_API_VERSION_1_1);
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    // NOTE: The vulkan_functions are queried using the platform get proc addr from the loader.  So we'll compare
    //       that to what is returned by asking it what the various Vulkan get proc addr functions are.
    PFN_vkGetDeviceProcAddr gdpa_loader = env.vulkan_functions.vkGetDeviceProcAddr;
    PFN_vkGetDeviceProcAddr gdpa_inst_queried =
        reinterpret_cast<PFN_vkGetDeviceProcAddr>(env.vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkGetDeviceProcAddr"));
    ASSERT_EQ(gdpa_loader, gdpa_inst_queried);

    DeviceWrapper dev{inst};
    dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(phys_dev);

    PFN_vkGetDeviceProcAddr gdpa_dev_queried =
        reinterpret_cast<PFN_vkGetDeviceProcAddr>(env.vulkan_functions.vkGetDeviceProcAddr(dev.dev, "vkGetDeviceProcAddr"));
    ASSERT_EQ(gdpa_loader, gdpa_dev_queried);
}

// Load the global function pointers with and without a NULL vkInstance handle.
// Call the function to make sure it is callable, don't care about what is returned.
TEST(GetProcAddr, GlobalFunctions) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().physical_devices.emplace_back("physical_device_0");

    auto& gipa = env.vulkan_functions.vkGetInstanceProcAddr;
    // global entry points with NULL instance handle
    {
        auto EnumerateInstanceExtensionProperties =
            reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(gipa(NULL, "vkEnumerateInstanceExtensionProperties"));
        handle_assert_has_value(EnumerateInstanceExtensionProperties);
        uint32_t ext_count = 0;
        ASSERT_EQ(VK_SUCCESS, EnumerateInstanceExtensionProperties("", &ext_count, nullptr));

        auto EnumerateInstanceLayerProperties =
            reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(gipa(NULL, "vkEnumerateInstanceLayerProperties"));
        handle_assert_has_value(EnumerateInstanceLayerProperties);
        uint32_t layer_count = 0;
        ASSERT_EQ(VK_SUCCESS, EnumerateInstanceLayerProperties(&layer_count, nullptr));

        auto EnumerateInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(gipa(NULL, "vkEnumerateInstanceVersion"));
        handle_assert_has_value(EnumerateInstanceVersion);
        uint32_t api_version = 0;
        EnumerateInstanceVersion(&api_version);

        auto GetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(gipa(NULL, "vkGetInstanceProcAddr"));
        ASSERT_EQ(GetInstanceProcAddr,
                  reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetInstanceProcAddr(NULL, "vkGetInstanceProcAddr")));

        auto CreateInstance = reinterpret_cast<PFN_vkCreateInstance>(gipa(NULL, "vkCreateInstance"));
        handle_assert_has_value(CreateInstance);
    }
    // Now create an instance and query the functions again - should work because the instance version is less than 1.2
    for (int i = 0; i <= 2; i++) {
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.api_version = VK_MAKE_API_VERSION(0, 1, i, 0);
        inst.CheckCreate();

        auto EnumerateInstanceExtensionProperties =
            reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(gipa(inst, "vkEnumerateInstanceExtensionProperties"));
        handle_assert_has_value(EnumerateInstanceExtensionProperties);
        uint32_t ext_count = 0;
        ASSERT_EQ(VK_SUCCESS, EnumerateInstanceExtensionProperties("", &ext_count, nullptr));

        auto EnumerateInstanceLayerProperties =
            reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(gipa(inst, "vkEnumerateInstanceLayerProperties"));
        handle_assert_has_value(EnumerateInstanceLayerProperties);
        uint32_t layer_count = 0;
        ASSERT_EQ(VK_SUCCESS, EnumerateInstanceLayerProperties(&layer_count, nullptr));

        auto EnumerateInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(gipa(inst, "vkEnumerateInstanceVersion"));
        handle_assert_has_value(EnumerateInstanceVersion);
        uint32_t api_version = 0;
        EnumerateInstanceVersion(&api_version);

        auto GetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(gipa(inst, "vkGetInstanceProcAddr"));
        handle_assert_has_value(GetInstanceProcAddr);
        ASSERT_EQ(GetInstanceProcAddr,
                  reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetInstanceProcAddr(inst, "vkGetInstanceProcAddr")));

        auto CreateInstance = reinterpret_cast<PFN_vkCreateInstance>(gipa(inst, "vkCreateInstance"));
        handle_assert_has_value(CreateInstance);
    }
    {
        // Create a 1.3 instance - now everything should return NULL
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.api_version = VK_MAKE_API_VERSION(0, 1, 3, 0);
        inst.CheckCreate();

        auto EnumerateInstanceExtensionProperties =
            reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(gipa(inst, "vkEnumerateInstanceExtensionProperties"));
        handle_assert_null(EnumerateInstanceExtensionProperties);

        auto EnumerateInstanceLayerProperties =
            reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(gipa(inst, "vkEnumerateInstanceLayerProperties"));
        handle_assert_null(EnumerateInstanceLayerProperties);

        auto EnumerateInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(gipa(inst, "vkEnumerateInstanceVersion"));
        handle_assert_null(EnumerateInstanceVersion);

        auto CreateInstance = reinterpret_cast<PFN_vkCreateInstance>(gipa(inst, "vkCreateInstance"));
        handle_assert_null(CreateInstance);

        auto GetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(gipa(inst, "vkGetInstanceProcAddr"));
        handle_assert_equal(env.vulkan_functions.vkGetInstanceProcAddr, GetInstanceProcAddr);
        ASSERT_EQ(GetInstanceProcAddr,
                  reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetInstanceProcAddr(inst, "vkGetInstanceProcAddr")));
        ASSERT_EQ(GetInstanceProcAddr,
                  reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetInstanceProcAddr(NULL, "vkGetInstanceProcAddr")));
        // get a non pre-instance function pointer
        auto EnumeratePhysicalDevices = reinterpret_cast<PFN_vkGetInstanceProcAddr>(gipa(inst, "vkEnumeratePhysicalDevices"));
        handle_assert_has_value(EnumeratePhysicalDevices);

        EnumeratePhysicalDevices = reinterpret_cast<PFN_vkGetInstanceProcAddr>(gipa(NULL, "vkEnumeratePhysicalDevices"));
        handle_assert_null(EnumeratePhysicalDevices);
    }
}

// Swapchain functions which require a terminator in all cases have situations where the driver may have a
// NULL function pointer but the loader shouldn't abort() if that is the case. Rather, it should log a message
// and return VK_SUCCESS to maintain previous behavior.
TEST(GetDeviceProcAddr, SwapchainFuncsWithTerminator) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    setup_WSI_in_ICD(env.get_test_icd());
    env.get_test_icd().physical_devices.emplace_back("physical_device_0");

    InstWrapper inst(env.vulkan_functions);
    inst.create_info.add_extension("VK_EXT_debug_utils");
    setup_WSI_in_create_instance(inst);
    ASSERT_NO_FATAL_FAILURE(inst.CheckCreate());

    VkSurfaceKHR surface{};
    ASSERT_NO_FATAL_FAILURE(create_surface(inst, surface));

    DebugUtilsWrapper log{inst};
    ASSERT_EQ(VK_SUCCESS, CreateDebugUtilsMessenger(log));
    auto phys_dev = inst.GetPhysDev();
    {
        DeviceWrapper dev{inst};
        dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
        ASSERT_NO_FATAL_FAILURE(dev.CheckCreate(phys_dev));
        DeviceFunctions dev_funcs{env.vulkan_functions, dev};

        PFN_vkCreateSwapchainKHR CreateSwapchainKHR = dev.load("vkCreateSwapchainKHR");
        PFN_vkCreateSwapchainKHR inst_CreateSwapchainKHR = inst.load("vkCreateSwapchainKHR");
        PFN_vkGetDeviceGroupSurfacePresentModesKHR GetDeviceGroupSurfacePresentModesKHR =
            dev.load("vkGetDeviceGroupSurfacePresentModesKHR");
        PFN_vkCreateSharedSwapchainsKHR CreateSharedSwapchainsKHR = dev.load("vkCreateSharedSwapchainsKHR");
        ASSERT_FALSE(CreateSwapchainKHR);
        ASSERT_TRUE(inst_CreateSwapchainKHR);
        ASSERT_FALSE(GetDeviceGroupSurfacePresentModesKHR);
        ASSERT_FALSE(CreateSharedSwapchainsKHR);

        VkSwapchainCreateInfoKHR info{};
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.surface = surface;

        VkSwapchainKHR swapchain{};
        if (CreateSwapchainKHR) CreateSwapchainKHR(dev.dev, &info, nullptr, &swapchain);
        ASSERT_FALSE(
            log.find("vkCreateSwapchainKHR: Driver's function pointer was NULL, returning VK_SUCCESS. Was the VK_KHR_swapchain "
                     "extension enabled?"));
        log.logger.clear();
        if (dev_funcs.vkDestroySwapchainKHR) dev_funcs.vkDestroySwapchainKHR(dev.dev, swapchain, nullptr);
        // try to call the vkCreateSwapchainKHR acquired from the instance - this *should* abort due to not enabling the extension
        if (inst_CreateSwapchainKHR) {
            ASSERT_DEATH(inst_CreateSwapchainKHR(dev.dev, &info, nullptr, &swapchain),
                         "vkCreateSwapchainKHR: Driver's function pointer was NULL, returning VK_SUCCESS. Was the VK_KHR_swapchain "
                         "extension enabled?");
        }
        log.logger.clear();
        if (dev_funcs.vkDestroySwapchainKHR) dev_funcs.vkDestroySwapchainKHR(dev.dev, swapchain, nullptr);

        VkDeviceGroupPresentModeFlagsKHR modes{};
        if (GetDeviceGroupSurfacePresentModesKHR) GetDeviceGroupSurfacePresentModesKHR(dev.dev, surface, &modes);

        if (CreateSharedSwapchainsKHR) CreateSharedSwapchainsKHR(dev.dev, 1, &info, nullptr, &swapchain);
    }
    {
        env.get_test_icd().physical_devices.at(0).add_extensions(
            {"VK_KHR_swapchain", "VK_KHR_display_swapchain", "VK_EXT_debug_marker"});

        DeviceWrapper dev{inst};
        dev.create_info.add_extensions({"VK_KHR_swapchain", "VK_KHR_display_swapchain", "VK_EXT_debug_marker"});
        dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
        ASSERT_NO_FATAL_FAILURE(dev.CheckCreate(phys_dev));
        DeviceFunctions dev_funcs{env.vulkan_functions, dev};

        PFN_vkCreateSwapchainKHR CreateSwapchainKHR = dev.load("vkCreateSwapchainKHR");
        PFN_vkCreateSwapchainKHR inst_CreateSwapchainKHR = inst.load("vkCreateSwapchainKHR");
        PFN_vkGetDeviceGroupSurfacePresentModesKHR GetDeviceGroupSurfacePresentModesKHR =
            dev.load("vkGetDeviceGroupSurfacePresentModesKHR");
        PFN_vkCreateSharedSwapchainsKHR CreateSharedSwapchainsKHR = dev.load("vkCreateSharedSwapchainsKHR");
        ASSERT_TRUE(CreateSwapchainKHR);
        ASSERT_TRUE(inst_CreateSwapchainKHR);
        ASSERT_TRUE(GetDeviceGroupSurfacePresentModesKHR);
        ASSERT_TRUE(CreateSharedSwapchainsKHR);

        VkSwapchainCreateInfoKHR info{};
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.surface = surface;

        VkSwapchainKHR swapchain{};
        if (CreateSwapchainKHR) CreateSwapchainKHR(dev.dev, &info, nullptr, &swapchain);
        ASSERT_FALSE(
            log.find("vkCreateSwapchainKHR: Driver's function pointer was NULL, returning VK_SUCCESS. Was the VK_KHR_swapchain "
                     "extension enabled?"));
        log.logger.clear();
        if (dev_funcs.vkDestroySwapchainKHR) dev_funcs.vkDestroySwapchainKHR(dev.dev, swapchain, nullptr);
        if (inst_CreateSwapchainKHR) inst_CreateSwapchainKHR(dev.dev, &info, nullptr, &swapchain);
        ASSERT_FALSE(
            log.find("vkCreateSwapchainKHR: Driver's function pointer was NULL, returning VK_SUCCESS. Was the VK_KHR_swapchain "
                     "extension enabled?"));
        log.logger.clear();
        if (dev_funcs.vkDestroySwapchainKHR) dev_funcs.vkDestroySwapchainKHR(dev.dev, swapchain, nullptr);

        VkDeviceGroupPresentModeFlagsKHR modes{};
        if (GetDeviceGroupSurfacePresentModesKHR) GetDeviceGroupSurfacePresentModesKHR(dev.dev, surface, &modes);

        if (CreateSharedSwapchainsKHR) CreateSharedSwapchainsKHR(dev.dev, 1, &info, nullptr, &swapchain);
    }
    env.vulkan_functions.vkDestroySurfaceKHR(inst.inst, surface, nullptr);
}
