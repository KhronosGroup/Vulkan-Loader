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

// On arm64x the loader has two views of each entry point. An x64 caller (an arm64ec build, or a plain
// x64 process emulated on an Arm64 host) gets an export's x64 fast-forward-sequence thunk back from
// GetProcAddress, while the loader resolves its own name to the native arm64ec function. Same command,
// different address, and the spec doesn't require them to match, so in that case compare behavior
// instead of raw pointer identity. Everywhere else the pointers are expected to be identical.
static bool proc_addr_identity_expected() {
#if defined(_M_ARM64EC)
    return false;
#elif defined(_WIN32)
    USHORT process_machine = 0, native_machine = 0;
    if (IsWow64Process2(GetCurrentProcess(), &process_machine, &native_machine)) {
        // On an Arm64 host vulkan-1.dll may be arm64x, in which case an x64 (emulated) or arm64ec
        // caller gets a fast-forward-sequence thunk that differs from the loader's native pointer.
        // Note x64-on-Arm64 is not classic WOW64, so process_machine is UNKNOWN there, not AMD64;
        // key off the native machine instead.
        return native_machine != IMAGE_FILE_MACHINE_ARM64;
    }
    return true;
#else
    return true;
#endif
}

static void assert_gipa_equivalent(PFN_vkGetInstanceProcAddr from_loader, PFN_vkGetInstanceProcAddr from_query) {
    if (proc_addr_identity_expected()) {
        ASSERT_EQ(from_loader, from_query);
        return;
    }
    ASSERT_NE(from_loader, nullptr);
    ASSERT_NE(from_query, nullptr);
    ASSERT_EQ(from_loader(nullptr, "vkThisCommandDoesNotExist"), nullptr);
    ASSERT_EQ(from_query(nullptr, "vkThisCommandDoesNotExist"), nullptr);
    PFN_vkVoidFunction create_from_loader = from_loader(nullptr, "vkCreateInstance");
    PFN_vkVoidFunction create_from_query = from_query(nullptr, "vkCreateInstance");
    ASSERT_NE(create_from_loader, nullptr);
    ASSERT_EQ(create_from_loader, create_from_query);
}

static void assert_gdpa_equivalent(PFN_vkGetDeviceProcAddr from_loader, PFN_vkGetDeviceProcAddr from_query, VkDevice device) {
    if (proc_addr_identity_expected()) {
        (void)device;
        ASSERT_EQ(from_loader, from_query);
        return;
    }
    ASSERT_NE(from_loader, nullptr);
    ASSERT_NE(from_query, nullptr);
    if (device != VK_NULL_HANDLE) {
        ASSERT_EQ(from_loader(device, "vkThisCommandDoesNotExist"), nullptr);
        ASSERT_EQ(from_query(device, "vkThisCommandDoesNotExist"), nullptr);
        PFN_vkVoidFunction destroy_from_loader = from_loader(device, "vkDestroyDevice");
        PFN_vkVoidFunction destroy_from_query = from_query(device, "vkDestroyDevice");
        ASSERT_NE(destroy_from_loader, nullptr);
        ASSERT_EQ(destroy_from_loader, destroy_from_query);
    }
}

// Verify that the various ways to get vkGetInstanceProcAddr return the same value
TEST(GetProcAddr, VerifyGetInstanceProcAddr) {
    FrameworkEnvironment env{};
    env.add_icd(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA).add_physical_device("physical_device_0");
    {
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(VK_API_VERSION_1_1);
        inst.CheckCreate();

        // NOTE: The vulkan_functions are queried using the platform get proc addr from the loader.  So we'll compare
        //       that to what is returned by asking it what the various Vulkan get proc addr functions are.
        PFN_vkGetInstanceProcAddr gipa_loader = env.vulkan_functions.vkGetInstanceProcAddr;
        PFN_vkGetInstanceProcAddr gipa_queried = reinterpret_cast<PFN_vkGetInstanceProcAddr>(
            env.vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkGetInstanceProcAddr"));
        assert_gipa_equivalent(gipa_loader, gipa_queried);
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
        assert_gipa_equivalent(gipa_loader, gipa_queried);
    }
}

// Verify that the various ways to get vkGetDeviceProcAddr return the same value
TEST(GetProcAddr, VerifyGetDeviceProcAddr) {
    FrameworkEnvironment env{};
    env.add_icd(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA).add_physical_device("physical_device_0");

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_API_VERSION_1_1);
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    // NOTE: The vulkan_functions are queried using the platform get proc addr from the loader.  So we'll compare
    //       that to what is returned by asking it what the various Vulkan get proc addr functions are.
    PFN_vkGetDeviceProcAddr gdpa_loader = env.vulkan_functions.vkGetDeviceProcAddr;
    PFN_vkGetDeviceProcAddr gdpa_inst_queried = inst.load("vkGetDeviceProcAddr");
    assert_gdpa_equivalent(gdpa_loader, gdpa_inst_queried, VK_NULL_HANDLE);

    DeviceWrapper dev{inst};
    dev.CheckCreate(phys_dev);

    PFN_vkGetDeviceProcAddr gdpa_dev_queried = dev.load("vkGetDeviceProcAddr");
    assert_gdpa_equivalent(gdpa_loader, gdpa_dev_queried, dev);
}

// Load the global function pointers with and without a NULL vkInstance handle.
// Call the function to make sure it is callable, don't care about what is returned.
TEST(GetProcAddr, GlobalFunctions) {
    FrameworkEnvironment env{};
    env.add_icd(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA).add_physical_device("physical_device_0");

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

        PFN_vkEnumerateInstanceExtensionProperties EnumerateInstanceExtensionProperties =
            inst.load("vkEnumerateInstanceExtensionProperties");
        handle_assert_has_value(EnumerateInstanceExtensionProperties);
        uint32_t ext_count = 0;
        ASSERT_EQ(VK_SUCCESS, EnumerateInstanceExtensionProperties("", &ext_count, nullptr));

        PFN_vkEnumerateInstanceLayerProperties EnumerateInstanceLayerProperties = inst.load("vkEnumerateInstanceLayerProperties");
        handle_assert_has_value(EnumerateInstanceLayerProperties);
        uint32_t layer_count = 0;
        ASSERT_EQ(VK_SUCCESS, EnumerateInstanceLayerProperties(&layer_count, nullptr));

        PFN_vkEnumerateInstanceVersion EnumerateInstanceVersion = inst.load("vkEnumerateInstanceVersion");
        handle_assert_has_value(EnumerateInstanceVersion);
        uint32_t api_version = 0;
        EnumerateInstanceVersion(&api_version);

        PFN_vkGetInstanceProcAddr GetInstanceProcAddr = inst.load("vkGetInstanceProcAddr");
        handle_assert_has_value(GetInstanceProcAddr);
        ASSERT_EQ(GetInstanceProcAddr,
                  reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetInstanceProcAddr(inst, "vkGetInstanceProcAddr")));

        PFN_vkCreateInstance CreateInstance = inst.load("vkCreateInstance");
        handle_assert_has_value(CreateInstance);
    }
    {
        // Create a 1.3 instance - now everything should return NULL
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.api_version = VK_MAKE_API_VERSION(0, 1, 3, 0);
        inst.CheckCreate();

        PFN_vkEnumerateInstanceExtensionProperties EnumerateInstanceExtensionProperties =
            inst.load("vkEnumerateInstanceExtensionProperties");
        handle_assert_null(EnumerateInstanceExtensionProperties);

        PFN_vkEnumerateInstanceLayerProperties EnumerateInstanceLayerProperties = inst.load("vkEnumerateInstanceLayerProperties");
        handle_assert_null(EnumerateInstanceLayerProperties);

        PFN_vkEnumerateInstanceVersion EnumerateInstanceVersion = inst.load("vkEnumerateInstanceVersion");
        handle_assert_null(EnumerateInstanceVersion);

        PFN_vkCreateInstance CreateInstance = inst.load("vkCreateInstance");
        handle_assert_null(CreateInstance);

        PFN_vkGetInstanceProcAddr GetInstanceProcAddr = inst.load("vkGetInstanceProcAddr");
        assert_gipa_equivalent(env.vulkan_functions.vkGetInstanceProcAddr, GetInstanceProcAddr);
        ASSERT_EQ(GetInstanceProcAddr,
                  reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetInstanceProcAddr(inst, "vkGetInstanceProcAddr")));
        ASSERT_EQ(GetInstanceProcAddr,
                  reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetInstanceProcAddr(NULL, "vkGetInstanceProcAddr")));
        // get a non pre-instance function pointer
        PFN_vkEnumeratePhysicalDevices EnumeratePhysicalDevices = inst.load("vkEnumeratePhysicalDevices");
        handle_assert_has_value(EnumeratePhysicalDevices);

        EnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(gipa(NULL, "vkEnumeratePhysicalDevices"));
        handle_assert_null(EnumeratePhysicalDevices);
    }
}

TEST(GetProcAddr, Verify10FunctionsFailToLoadWithSingleDriver) {
    FrameworkEnvironment env{};
    env.add_icd(TEST_ICD_PATH_VERSION_2).add_physical_device({}).set_can_query_GetPhysicalDeviceFuncs(false);

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate(VK_ERROR_INCOMPATIBLE_DRIVER);
}

TEST(GetProcAddr, Verify10FunctionsLoadWithMultipleDrivers) {
    FrameworkEnvironment env{};
    env.add_icd(TEST_ICD_PATH_VERSION_2).add_physical_device({});
    env.add_icd(TEST_ICD_PATH_VERSION_2).add_physical_device({}).set_can_query_GetPhysicalDeviceFuncs(false);

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    inst.GetPhysDevs(1);
}

// Swapchain functions which require a terminator in all cases have situations where the driver may have a
// NULL function pointer but the loader shouldn't abort() if that is the case. Rather, it should log a message
// and return VK_SUCCESS to maintain previous behavior.
TEST(GetDeviceProcAddr, SwapchainFuncsWithTerminator) {
    FrameworkEnvironment env{};
    auto& test_physical_device =
        env.add_icd(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA).setup_WSI().add_and_get_physical_device("physical_device_0");

    InstWrapper inst(env.vulkan_functions);
    inst.create_info.add_extension("VK_EXT_debug_utils");
    inst.create_info.setup_WSI();
    ASSERT_NO_FATAL_FAILURE(inst.CheckCreate());

    VkSurfaceKHR surface{};
    ASSERT_EQ(VK_SUCCESS, create_surface(inst, surface));

    VkSurfaceKHR surface2{};
    ASSERT_EQ(VK_SUCCESS, create_surface(inst, surface2));

    DebugUtilsWrapper log{inst};
    ASSERT_EQ(VK_SUCCESS, CreateDebugUtilsMessenger(log));
    auto phys_dev = inst.GetPhysDev();
    {
        DeviceWrapper dev{inst};
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
        log.logger.clear();
        ASSERT_FALSE(dev_funcs.vkDestroySwapchainKHR);

        // try to call the vkCreateSwapchainKHR acquired from the instance - this *should* abort due to not enabling the extension
        ASSERT_DEATH(inst_CreateSwapchainKHR(dev.dev, &info, nullptr, &swapchain),
                     "vkCreateSwapchainKHR: Driver's function pointer was NULL, returning VK_SUCCESS. Was the VK_KHR_swapchain "
                     "extension enabled?");

        log.logger.clear();
        ASSERT_FALSE(dev_funcs.vkDestroySwapchainKHR);
    }
    test_physical_device.add_extensions({"VK_KHR_swapchain", "VK_KHR_display_swapchain", "VK_EXT_debug_marker"});
    {
        DeviceWrapper dev{inst};
        dev.create_info.add_extensions({"VK_KHR_swapchain", "VK_KHR_display_swapchain", "VK_EXT_debug_marker"});
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
        ASSERT_TRUE(dev_funcs.vkDestroySwapchainKHR);

        VkSwapchainCreateInfoKHR info{};
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.surface = surface;

        VkSwapchainKHR swapchain{};
        CreateSwapchainKHR(dev.dev, &info, nullptr, &swapchain);
        ASSERT_FALSE(
            log.find("vkCreateSwapchainKHR: Driver's function pointer was NULL, returning VK_SUCCESS. Was the VK_KHR_swapchain "
                     "extension enabled?"));
        log.logger.clear();
        dev_funcs.vkDestroySwapchainKHR(dev.dev, swapchain, nullptr);
        inst_CreateSwapchainKHR(dev.dev, &info, nullptr, &swapchain);
        ASSERT_FALSE(
            log.find("vkCreateSwapchainKHR: Driver's function pointer was NULL, returning VK_SUCCESS. Was the VK_KHR_swapchain "
                     "extension enabled?"));
        log.logger.clear();
        dev_funcs.vkDestroySwapchainKHR(dev.dev, swapchain, nullptr);

        VkDeviceGroupPresentModeFlagsKHR modes{};
        GetDeviceGroupSurfacePresentModesKHR(dev.dev, surface, &modes);

        std::array<VkSwapchainCreateInfoKHR, 2> infos{};
        infos[0] = info;
        infos[1].sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        infos[1].surface = surface2;

        std::array<VkSwapchainKHR, 2> swapchains{};
        ASSERT_EQ(VK_SUCCESS, CreateSharedSwapchainsKHR(dev.dev, static_cast<uint32_t>(swapchains.size()), infos.data(), nullptr,
                                                        swapchains.data()));
        ASSERT_FALSE(log.find("vkCreateSharedSwapchainsKHR Terminator: No VkSurfaceKHR objects were created"));
    }
    env.vulkan_functions.vkDestroySurfaceKHR(inst.inst, surface, nullptr);
    env.vulkan_functions.vkDestroySurfaceKHR(inst.inst, surface2, nullptr);
}

// Verify that the various ways to get vkGetDeviceProcAddr return the same value
TEST(GetProcAddr, PreserveLayerGettingVkCreateDeviceWithNullInstance) {
    FrameworkEnvironment env{};
    env.add_icd(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA).add_physical_device("physical_device_0");

    env.add_implicit_layer({}, ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                             .set_name("VK_LAYER_technically_buggy_layer")
                                                             .set_description("actually_layer_1")
                                                             .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                             .set_disable_environment("if_you_can")));
    env.get_test_layer().set_buggy_query_of_vkCreateDevice(true);
    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_API_VERSION_1_1);
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    DeviceWrapper dev{inst};
    dev.CheckCreate(phys_dev);
}

// The following tests - AppQueries11FunctionsWhileOnlyEnabling10, AppQueries12FunctionsWhileOnlyEnabling11, and
// AppQueries13FunctionsWhileOnlyEnabling12 - check that vkGetDeviceProcAddr only returning functions from core versions up to
// the apiVersion declared in VkApplicationInfo. Function querying should succeed if VK_KHR_maintenance_5 is not enabled, and they
// should return zero when that extension is enabled.

TEST(GetDeviceProcAddr, AppQueries11FunctionsWhileOnlyEnabling10) {
    FrameworkEnvironment env{};
    auto& test_physical_device =
        env.add_icd(TEST_ICD_PATH_VERSION_2, {}, ManifestICD{}.set_api_version(VK_API_VERSION_1_1))
            .set_icd_api_version(VK_API_VERSION_1_1)
            .add_and_get_physical_device(
                PhysicalDevice{}.set_api_version(VK_API_VERSION_1_1).add_extension(VK_KHR_MAINTENANCE_5_EXTENSION_NAME));

    std::vector<const char*> functions = {"vkGetDeviceQueue2", "vkCmdDispatchBase", "vkCreateDescriptorUpdateTemplate"};
    for (const auto& f : functions) {
        test_physical_device.add_device_function(VulkanFunction{f, [] {}});
    }
    {  // doesn't enable the feature or extension
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(1, 0, 0);
        inst.CheckCreate();

        DeviceWrapper dev{inst};
        dev.CheckCreate(inst.GetPhysDev());
        for (const auto& f : functions) {
            ASSERT_NE(nullptr, dev->vkGetDeviceProcAddr(dev.dev, f));
        }
    }
    {  // doesn't enable the feature
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(1, 0, 0);
        inst.CheckCreate();

        DeviceWrapper dev{inst};
        dev.create_info.add_extension(VK_KHR_MAINTENANCE_5_EXTENSION_NAME);
        dev.CheckCreate(inst.GetPhysDev());
        for (const auto& f : functions) {
            ASSERT_NE(nullptr, dev->vkGetDeviceProcAddr(dev.dev, f));
        }
    }
    {  // enables the feature and extension
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(1, 0, 0);
        inst.CheckCreate();

        VkPhysicalDeviceMaintenance5FeaturesKHR features{};
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_FEATURES_KHR;
        features.maintenance5 = VK_TRUE;

        DeviceWrapper dev{inst};
        dev.create_info.add_extension(VK_KHR_MAINTENANCE_5_EXTENSION_NAME);
        dev.create_info.dev.pNext = &features;
        dev.CheckCreate(inst.GetPhysDev());
        for (const auto& f : functions) {
            ASSERT_EQ(nullptr, dev->vkGetDeviceProcAddr(dev.dev, f));
        }
    }
}

TEST(GetDeviceProcAddr, AppQueries12FunctionsWhileOnlyEnabling11) {
    FrameworkEnvironment env{};
    auto& test_physical_device =
        env.add_icd(TEST_ICD_PATH_VERSION_2, {}, ManifestICD{}.set_api_version(VK_API_VERSION_1_2))
            .set_icd_api_version(VK_API_VERSION_1_2)
            .add_and_get_physical_device(
                PhysicalDevice{}.set_api_version(VK_API_VERSION_1_2).add_extension(VK_KHR_MAINTENANCE_5_EXTENSION_NAME));
    std::vector<const char*> functions = {"vkCmdDrawIndirectCount", "vkCmdNextSubpass2", "vkGetBufferDeviceAddress",
                                          "vkGetDeviceMemoryOpaqueCaptureAddress"};
    for (const auto& f : functions) {
        test_physical_device.add_device_function(VulkanFunction{f, [] {}});
    }
    {  // doesn't enable the feature or extension
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(1, 1, 0);
        inst.CheckCreate();

        DeviceWrapper dev{inst};
        dev.CheckCreate(inst.GetPhysDev());

        for (const auto& f : functions) {
            ASSERT_NE(nullptr, dev->vkGetDeviceProcAddr(dev.dev, f));
        }
    }
    {  // doesn't enable the feature
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(1, 1, 0);
        inst.CheckCreate();

        DeviceWrapper dev{inst};
        dev.create_info.add_extension(VK_KHR_MAINTENANCE_5_EXTENSION_NAME);
        dev.CheckCreate(inst.GetPhysDev());

        for (const auto& f : functions) {
            ASSERT_NE(nullptr, dev->vkGetDeviceProcAddr(dev.dev, f));
        }
    }
    {  // enables the feature and extension
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(1, 1, 0);
        inst.CheckCreate();

        VkPhysicalDeviceMaintenance5FeaturesKHR features{};
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_FEATURES_KHR;
        features.maintenance5 = VK_TRUE;

        DeviceWrapper dev{inst};
        dev.create_info.add_extension(VK_KHR_MAINTENANCE_5_EXTENSION_NAME);
        dev.create_info.dev.pNext = &features;
        dev.CheckCreate(inst.GetPhysDev());

        for (const auto& f : functions) {
            ASSERT_EQ(nullptr, dev->vkGetDeviceProcAddr(dev.dev, f));
        }
    }
}

TEST(GetDeviceProcAddr, AppQueries13FunctionsWhileOnlyEnabling12) {
    FrameworkEnvironment env{};
    auto& test_physical_device =
        env.add_icd(TEST_ICD_PATH_VERSION_2, {}, ManifestICD{}.set_api_version(VK_API_VERSION_1_3))
            .set_icd_api_version(VK_API_VERSION_1_3)
            .add_and_get_physical_device(
                PhysicalDevice{}.set_api_version(VK_API_VERSION_1_3).add_extension(VK_KHR_MAINTENANCE_5_EXTENSION_NAME));
    std::vector<const char*> functions = {"vkCreatePrivateDataSlot", "vkGetDeviceBufferMemoryRequirements", "vkCmdWaitEvents2",
                                          "vkGetDeviceImageSparseMemoryRequirements"};

    for (const auto& f : functions) {
        test_physical_device.add_device_function(VulkanFunction{f, [] {}});
    }
    {  // doesn't enable the feature or extension
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(1, 2, 0);
        inst.CheckCreate();

        DeviceWrapper dev{inst};
        dev.CheckCreate(inst.GetPhysDev());

        for (const auto& f : functions) {
            ASSERT_NE(nullptr, dev->vkGetDeviceProcAddr(dev.dev, f));
        }
    }
    {  // doesn't enable the feature
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(1, 2, 0);
        inst.CheckCreate();

        DeviceWrapper dev{inst};
        dev.create_info.add_extension(VK_KHR_MAINTENANCE_5_EXTENSION_NAME);
        dev.CheckCreate(inst.GetPhysDev());

        for (const auto& f : functions) {
            ASSERT_NE(nullptr, dev->vkGetDeviceProcAddr(dev.dev, f));
        }
    }
    {  // enables the feature and extension
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(1, 2, 0);
        inst.CheckCreate();

        VkPhysicalDeviceMaintenance5FeaturesKHR features{};
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_5_FEATURES_KHR;
        features.maintenance5 = VK_TRUE;

        DeviceWrapper dev{inst};
        dev.create_info.add_extension(VK_KHR_MAINTENANCE_5_EXTENSION_NAME);
        dev.create_info.dev.pNext = &features;
        dev.CheckCreate(inst.GetPhysDev());

        for (const auto& f : functions) {
            ASSERT_EQ(nullptr, dev->vkGetDeviceProcAddr(dev.dev, f));
        }
    }
}
