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

// Test case origin
// LX = lunar exchange
// LVLGH = loader and validation github
// LVLGL = loader and validation gitlab
// VL = Vulkan Loader github
// VVL = Vulkan Validation Layers github

TEST(CreateInstance, BasicRun) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    auto& driver = env.get_test_icd();
    driver.set_min_icd_interface_version(5);

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();
}

// LX435
TEST(CreateInstance, ConstInstanceInfo) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    VkInstance inst = VK_NULL_HANDLE;
    VkInstanceCreateInfo const info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, 0, nullptr, 0, nullptr, 0, nullptr};
    ASSERT_EQ(env.vulkan_functions.vkCreateInstance(&info, VK_NULL_HANDLE, &inst), VK_SUCCESS);
    // Must clean up
    env.vulkan_functions.vkDestroyInstance(inst, nullptr);
}

// VUID-vkDestroyInstance-instance-parameter, VUID-vkDestroyInstance-pAllocator-parameter
TEST(CreateInstance, DestroyInstanceNullHandle) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    env.vulkan_functions.vkDestroyInstance(VK_NULL_HANDLE, nullptr);
}

// VUID-vkDestroyDevice-device-parameter, VUID-vkDestroyDevice-pAllocator-parameter
TEST(CreateInstance, DestroyDeviceNullHandle) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    env.vulkan_functions.vkDestroyDevice(VK_NULL_HANDLE, nullptr);
}

// VUID-vkCreateInstance-ppEnabledExtensionNames-01388
TEST(CreateInstance, ExtensionNotPresent) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    {
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.add_extension("VK_EXT_validation_features");  // test icd won't report this as supported
        inst.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);
    }
    {
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.add_extension("Non_existant_extension");  // unknown instance extension
        inst.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);
    }
}

TEST(CreateInstance, LayerNotPresent) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer("VK_NON_EXISTANT_LAYER");
    inst.CheckCreate(VK_ERROR_LAYER_NOT_PRESENT);
}

TEST(CreateInstance, LayerPresent) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    const char* layer_name = "TestLayer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(layer_name).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "test_layer.json");

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer(layer_name);
    inst.CheckCreate();
}

TEST(CreateInstance, ConsecutiveCreate) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    for (uint32_t i = 0; i < 100; i++) {
        InstWrapper inst{env.vulkan_functions};
        inst.CheckCreate();
    }
}

TEST(CreateInstance, ConsecutiveCreateWithoutDestruction) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    std::vector<InstWrapper> instances;
    for (uint32_t i = 0; i < 100; i++) {
        instances.emplace_back(env.vulkan_functions);
        instances.back().CheckCreate();
    }
}

TEST(NoDrivers, CreateInstance) {
    FrameworkEnvironment env{};
    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate(VK_ERROR_INCOMPATIBLE_DRIVER);
}

TEST(EnumerateInstanceLayerProperties, UsageChecks) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    const char* layer_name_1 = "TestLayer1";
    const char* layer_name_2 = "TestLayer1";

    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(layer_name_1).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "test_layer_1.json");

    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(layer_name_2).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "test_layer_2.json");

    {  // OnePass
        VkLayerProperties layer_props[2] = {};
        uint32_t layer_count = 2;
        ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props));
        ASSERT_EQ(layer_count, 2U);
        ASSERT_TRUE(string_eq(layer_name_1, layer_props[0].layerName));
        ASSERT_TRUE(string_eq(layer_name_2, layer_props[1].layerName));
    }
    {  // OnePass
        uint32_t layer_count = 0;
        ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr));
        ASSERT_EQ(layer_count, 2U);

        VkLayerProperties layer_props[2] = {};
        ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props));
        ASSERT_EQ(layer_count, 2U);
        ASSERT_TRUE(string_eq(layer_name_1, layer_props[0].layerName));
        ASSERT_TRUE(string_eq(layer_name_2, layer_props[1].layerName));
    }
    {  // PropertyCountLessThanAvailable
        VkLayerProperties layer_props{};
        uint32_t layer_count = 1;
        ASSERT_EQ(VK_INCOMPLETE, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, &layer_props));
        ASSERT_TRUE(string_eq(layer_name_1, layer_props.layerName));
    }
}

TEST(EnumerateInstanceExtensionProperties, UsageChecks) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    Extension first_ext{"VK_EXT_validation_features"};  // known instance extensions
    Extension second_ext{"VK_EXT_headless_surface"};
    env.reset_icd().add_instance_extensions({first_ext, second_ext});

    {  // One Pass
        uint32_t extension_count = 5;
        std::array<VkExtensionProperties, 5> extensions;
        ASSERT_EQ(VK_SUCCESS,
                  env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data()));
        ASSERT_EQ(extension_count, 5U);  // return debug report & debug utils & portability enumeration + our two extensions

        // loader always adds the debug report & debug utils extensions
        ASSERT_TRUE(first_ext.extensionName == extensions[0].extensionName);
        ASSERT_TRUE(second_ext.extensionName == extensions[1].extensionName);
        ASSERT_TRUE(string_eq("VK_EXT_debug_report", extensions[2].extensionName));
        ASSERT_TRUE(string_eq("VK_EXT_debug_utils", extensions[3].extensionName));
        ASSERT_TRUE(string_eq("VK_KHR_portability_enumeration", extensions[4].extensionName));
    }
    {  // Two Pass
        uint32_t extension_count = 0;
        std::array<VkExtensionProperties, 5> extensions;
        ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
        ASSERT_EQ(extension_count, 5U);  // return debug report & debug utils + our two extensions

        ASSERT_EQ(VK_SUCCESS,
                  env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data()));
        ASSERT_EQ(extension_count, 5U);
        // loader always adds the debug report & debug utils extensions
        ASSERT_TRUE(first_ext.extensionName == extensions[0].extensionName);
        ASSERT_TRUE(second_ext.extensionName == extensions[1].extensionName);
        ASSERT_TRUE(string_eq("VK_EXT_debug_report", extensions[2].extensionName));
        ASSERT_TRUE(string_eq("VK_EXT_debug_utils", extensions[3].extensionName));
        ASSERT_TRUE(string_eq("VK_KHR_portability_enumeration", extensions[4].extensionName));
    }
}

TEST(EnumerateInstanceExtensionProperties, PropertyCountLessThanAvailable) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    uint32_t extension_count = 0;
    std::array<VkExtensionProperties, 2> extensions;
    {  // use nullptr for null string
        ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
        ASSERT_EQ(extension_count, 3U);  // return debug report & debug utils & portability enumeration
        extension_count = 1;             // artificially remove one extension

        ASSERT_EQ(VK_INCOMPLETE,
                  env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data()));
        ASSERT_EQ(extension_count, 1U);
        // loader always adds the debug report & debug utils extensions
        ASSERT_TRUE(string_eq(extensions[0].extensionName, "VK_EXT_debug_report"));
    }
    {  // use "" for null string
        ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, nullptr));
        ASSERT_EQ(extension_count, 3U);  // return debug report & debug utils & portability enumeration
        extension_count = 1;             // artificially remove one extension

        ASSERT_EQ(VK_INCOMPLETE,
                  env.vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, extensions.data()));
        ASSERT_EQ(extension_count, 1U);
        // loader always adds the debug report & debug utils extensions
        ASSERT_TRUE(string_eq(extensions[0].extensionName, "VK_EXT_debug_report"));
    }
}

TEST(EnumerateInstanceExtensionProperties, FilterUnkownInstanceExtensions) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    Extension first_ext{"FirstTestExtension"};  // unknown instance extensions
    Extension second_ext{"SecondTestExtension"};
    env.reset_icd().add_instance_extensions({first_ext, second_ext});
    {
        uint32_t extension_count = 0;
        ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, nullptr));
        ASSERT_EQ(extension_count, 3U);  // return debug report & debug utils & portability enumeration

        std::array<VkExtensionProperties, 3> extensions;
        ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, extensions.data()));
        ASSERT_EQ(extension_count, 3U);
        // loader always adds the debug report & debug utils extensions
        ASSERT_TRUE(string_eq(extensions[0].extensionName, "VK_EXT_debug_report"));
        ASSERT_TRUE(string_eq(extensions[1].extensionName, "VK_EXT_debug_utils"));
        ASSERT_TRUE(string_eq(extensions[2].extensionName, "VK_KHR_portability_enumeration"));
    }
    {  // Disable unknown instance extension filtering
        set_env_var("VK_LOADER_DISABLE_INST_EXT_FILTER", "1");

        uint32_t extension_count = 0;
        ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, nullptr));
        ASSERT_EQ(extension_count, 5U);

        std::array<VkExtensionProperties, 5> extensions;
        ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, extensions.data()));
        ASSERT_EQ(extension_count, 5U);

        ASSERT_EQ(extensions[0], first_ext.get());
        ASSERT_EQ(extensions[1], second_ext.get());
        // Loader always adds these two extensions
        ASSERT_TRUE(string_eq(extensions[2].extensionName, "VK_EXT_debug_report"));
        ASSERT_TRUE(string_eq(extensions[3].extensionName, "VK_EXT_debug_utils"));
        ASSERT_TRUE(string_eq(extensions[4].extensionName, "VK_KHR_portability_enumeration"));
    }
}

TEST(EnumerateDeviceLayerProperties, LayersMatch) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    auto& driver = env.get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    const char* layer_name = "TestLayer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(layer_name).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "test_layer.json");

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer(layer_name);
    inst.CheckCreate();

    VkPhysicalDevice phys_dev = inst.GetPhysDev();
    {  // LayersMatch

        uint32_t layer_count = 0;
        ASSERT_EQ(env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &layer_count, nullptr), VK_SUCCESS);
        ASSERT_EQ(layer_count, 1U);
        VkLayerProperties layer_props;
        ASSERT_EQ(env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &layer_count, &layer_props), VK_SUCCESS);
        ASSERT_EQ(layer_count, 1U);
        ASSERT_TRUE(string_eq(layer_props.layerName, layer_name));
    }
    {  // Property count less than available
        VkLayerProperties layer_props;
        uint32_t layer_count = 0;
        ASSERT_EQ(VK_INCOMPLETE, env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &layer_count, &layer_props));
        ASSERT_EQ(layer_count, 0U);
    }
}

TEST(EnumerateDeviceExtensionProperties, DeviceExtensionEnumerated) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    auto& driver = env.get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    std::array<Extension, 2> device_extensions = {Extension{"MyExtension0", 4}, Extension{"MyExtension1", 7}};
    for (auto& ext : device_extensions) {
        driver.physical_devices.front().extensions.push_back(ext);
    }
    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &driver_count, &physical_device));

    uint32_t extension_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr));
    ASSERT_EQ(extension_count, device_extensions.size());

    std::array<VkExtensionProperties, 2> enumerated_device_exts;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count,
                                                                     enumerated_device_exts.data()));
    ASSERT_EQ(extension_count, device_extensions.size());
    ASSERT_TRUE(device_extensions[0].extensionName == enumerated_device_exts[0].extensionName);
    ASSERT_TRUE(device_extensions[0].specVersion == enumerated_device_exts[0].specVersion);
}

TEST(EnumerateDeviceExtensionProperties, PropertyCountLessThanAvailable) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    auto& driver = env.get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    std::array<Extension, 2> device_extensions = {Extension{"MyExtension0", 4}, Extension{"MyExtension1", 7}};
    for (auto& ext : device_extensions) {
        driver.physical_devices.front().extensions.push_back(ext);
    }
    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &driver_count, &physical_device));

    uint32_t extension_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumerateDeviceExtensionProperties(physical_device, "", &extension_count, nullptr));
    ASSERT_EQ(extension_count, device_extensions.size());
    extension_count -= 1;

    std::array<VkExtensionProperties, 2> enumerated_device_exts;
    ASSERT_EQ(VK_INCOMPLETE,
              inst->vkEnumerateDeviceExtensionProperties(physical_device, "", &extension_count, enumerated_device_exts.data()));
    ASSERT_EQ(extension_count, device_extensions.size() - 1);
    ASSERT_TRUE(device_extensions[0].extensionName == enumerated_device_exts[0].extensionName);
    ASSERT_TRUE(device_extensions[0].specVersion == enumerated_device_exts[0].specVersion);
}

TEST(EnumerateDeviceExtensionProperties, ZeroPhysicalDeviceExtensions) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    env.get_test_icd().add_physical_device({});
    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0));
    inst.CheckCreate(VK_SUCCESS);

    auto phys_dev = inst.GetPhysDev();
    DeviceWrapper dev{inst};
    dev.CheckCreate(phys_dev);

    uint32_t ext_count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &ext_count, nullptr));
    ASSERT_EQ(ext_count, 0U);
    VkExtensionProperties ext_props{};
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &ext_count, &ext_props));
    ASSERT_EQ(ext_count, 0U);
}

TEST(EnumeratePhysicalDevices, OneCall) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    auto& driver = env.get_test_icd().set_min_icd_interface_version(5);

    driver.physical_devices.emplace_back("physical_device_0", 1);
    driver.physical_devices.emplace_back("physical_device_1", 2);
    driver.physical_devices.emplace_back("physical_device_2", 3);
    driver.physical_devices.emplace_back("physical_device_3", 4);

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    uint32_t physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    uint32_t returned_physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    std::vector<VkPhysicalDevice> physical_device_handles = std::vector<VkPhysicalDevice>(physical_count);
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_device_handles.data()));
    ASSERT_EQ(physical_count, returned_physical_count);
}

TEST(EnumeratePhysicalDevices, TwoCall) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));

    auto& driver = env.get_test_icd().set_min_icd_interface_version(5);
    driver.add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});

    const uint32_t real_device_count = 2;
    for (uint32_t i = 0; i < real_device_count; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i), i + 1);
        driver.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    }

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    uint32_t physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    uint32_t returned_physical_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, nullptr));
    ASSERT_EQ(physical_count, returned_physical_count);

    std::array<VkPhysicalDevice, real_device_count> physical_device_handles;
    ASSERT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, physical_device_handles.data()));
    ASSERT_EQ(physical_count, returned_physical_count);
}

TEST(EnumeratePhysicalDevices, MatchOneAndTwoCallNumbers) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd();
    driver.set_min_icd_interface_version(5);
    driver.add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});

    const uint32_t real_device_count = 3;
    for (uint32_t i = 0; i < real_device_count; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i), i + 1);
        driver.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    }

    InstWrapper inst1{env.vulkan_functions};
    inst1.CheckCreate();

    uint32_t physical_count_one_call = static_cast<uint32_t>(driver.physical_devices.size());
    std::array<VkPhysicalDevice, real_device_count> physical_device_handles_one_call;
    ASSERT_EQ(VK_SUCCESS,
              inst1->vkEnumeratePhysicalDevices(inst1, &physical_count_one_call, physical_device_handles_one_call.data()));
    ASSERT_EQ(real_device_count, physical_count_one_call);

    InstWrapper inst2{env.vulkan_functions};
    inst2.CheckCreate();

    uint32_t physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    uint32_t returned_physical_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst2->vkEnumeratePhysicalDevices(inst2, &returned_physical_count, nullptr));
    ASSERT_EQ(physical_count, returned_physical_count);

    std::array<VkPhysicalDevice, real_device_count> physical_device_handles;
    ASSERT_EQ(VK_SUCCESS, inst2->vkEnumeratePhysicalDevices(inst2, &returned_physical_count, physical_device_handles.data()));
    ASSERT_EQ(real_device_count, returned_physical_count);

    ASSERT_EQ(physical_count_one_call, returned_physical_count);
}

TEST(EnumeratePhysicalDevices, TwoCallIncomplete) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd().set_min_icd_interface_version(5);
    driver.add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});

    const uint32_t real_device_count = 2;
    for (uint32_t i = 0; i < real_device_count; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i), i + 1);
        driver.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    }

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    uint32_t physical_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &physical_count, nullptr));
    ASSERT_EQ(physical_count, driver.physical_devices.size());

    std::array<VkPhysicalDevice, real_device_count> physical;

    auto temp_ptr = std::unique_ptr<int>(new int());
    physical[0] = reinterpret_cast<VkPhysicalDevice>(temp_ptr.get());
    physical[1] = reinterpret_cast<VkPhysicalDevice>(temp_ptr.get());

    // Use zero for the device count so we can get the VK_INCOMPLETE message and verify nothing was written into physical
    physical_count = 0;
    ASSERT_EQ(VK_INCOMPLETE, inst->vkEnumeratePhysicalDevices(inst, &physical_count, physical.data()));
    ASSERT_EQ(physical_count, 0U);
    ASSERT_EQ(static_cast<void*>(physical[0]), static_cast<void*>(temp_ptr.get()));
    ASSERT_EQ(static_cast<void*>(physical[1]), static_cast<void*>(temp_ptr.get()));

    // Remove one from the physical device count so we can get the VK_INCOMPLETE message
    physical_count = 1;
    ASSERT_EQ(VK_INCOMPLETE, inst->vkEnumeratePhysicalDevices(inst, &physical_count, physical.data()));
    ASSERT_EQ(physical_count, 1U);
    ASSERT_EQ(static_cast<void*>(physical[1]), static_cast<void*>(temp_ptr.get()));

    physical_count = 2;
    std::array<VkPhysicalDevice, real_device_count> physical_2;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &physical_count, physical_2.data()));

    // Verify that the first physical device shows up in the list of the second ones
    ASSERT_TRUE(std::find(physical_2.begin(), physical_2.end(), physical[0]) != physical_2.end());
}

TEST(EnumeratePhysicalDevices, ZeroPhysicalDevices) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0));
    inst.CheckCreate(VK_SUCCESS);

    uint32_t count = 0;
    ASSERT_EQ(VK_ERROR_INITIALIZATION_FAILED, env.vulkan_functions.vkEnumeratePhysicalDevices(inst, &count, nullptr));
    ASSERT_EQ(count, 0U);
}

TEST(EnumeratePhysicalDevices, ZeroPhysicalDevicesAfterCreateInstance) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd().set_min_icd_interface_version(5);
    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_API_VERSION_1_1);
    inst.CheckCreate();
    driver.physical_devices.clear();

    uint32_t physical_device_count = 1000;  // not zero starting value
    VkPhysicalDevice physical_device{};

    EXPECT_EQ(VK_ERROR_INITIALIZATION_FAILED, inst->vkEnumeratePhysicalDevices(inst, &physical_device_count, nullptr));
    EXPECT_EQ(VK_ERROR_INITIALIZATION_FAILED, inst->vkEnumeratePhysicalDevices(inst, &physical_device_count, &physical_device));

    uint32_t physical_device_group_count = 1000;  // not zero starting value
    VkPhysicalDeviceGroupProperties physical_device_group_properties{};

    EXPECT_EQ(VK_ERROR_INITIALIZATION_FAILED, inst->vkEnumeratePhysicalDeviceGroups(inst, &physical_device_group_count, nullptr));
    EXPECT_EQ(VK_ERROR_INITIALIZATION_FAILED,
              inst->vkEnumeratePhysicalDeviceGroups(inst, &physical_device_group_count, &physical_device_group_properties));
}

TEST(EnumeratePhysicalDevices, CallTwiceNormal) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd().set_min_icd_interface_version(5);

    for (size_t i = 0; i < 4; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i));
    }

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    // Call twice in a row and make sure nothing bad happened
    uint32_t physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    uint32_t returned_physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    std::vector<VkPhysicalDevice> physical_device_handles_1 = std::vector<VkPhysicalDevice>(physical_count);
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_device_handles_1.data()));
    ASSERT_EQ(physical_count, returned_physical_count);
    std::vector<VkPhysicalDevice> physical_device_handles_2 = std::vector<VkPhysicalDevice>(physical_count);
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_device_handles_2.data()));
    ASSERT_EQ(physical_count, returned_physical_count);
    // Make sure devices are same between the two
    for (uint32_t count = 0; count < driver.physical_devices.size(); ++count) {
        ASSERT_EQ(physical_device_handles_1[count], physical_device_handles_2[count]);
    }
}

TEST(EnumeratePhysicalDevices, CallTwiceIncompleteOnceNormal) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd().set_min_icd_interface_version(5);

    for (size_t i = 0; i < 8; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i));
    }

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    // Query 3, then 5, then all
    uint32_t physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    uint32_t returned_physical_count = 3;
    std::vector<VkPhysicalDevice> physical_device_handles_1 = std::vector<VkPhysicalDevice>(returned_physical_count);
    ASSERT_EQ(VK_INCOMPLETE, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_device_handles_1.data()));
    ASSERT_EQ(3U, returned_physical_count);
    returned_physical_count = 5;
    std::vector<VkPhysicalDevice> physical_device_handles_2 = std::vector<VkPhysicalDevice>(returned_physical_count);
    ASSERT_EQ(VK_INCOMPLETE, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_device_handles_2.data()));
    ASSERT_EQ(5U, returned_physical_count);
    returned_physical_count = physical_count;
    std::vector<VkPhysicalDevice> physical_device_handles_3 = std::vector<VkPhysicalDevice>(returned_physical_count);
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_device_handles_3.data()));
    ASSERT_EQ(physical_count, returned_physical_count);
    // Make sure devices are same between the three
    for (uint32_t count = 0; count < driver.physical_devices.size(); ++count) {
        if (count < physical_device_handles_1.size()) {
            ASSERT_EQ(physical_device_handles_1[count], physical_device_handles_3[count]);
        }
        if (count < physical_device_handles_2.size()) {
            ASSERT_EQ(physical_device_handles_2[count], physical_device_handles_3[count]);
        }
    }
}

TEST(EnumeratePhysicalDevices, CallThriceSuccessReduce) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd().set_min_icd_interface_version(5);

    for (size_t i = 0; i < 8; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i));
    }

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    // Query all at first, then 5, then 3
    uint32_t physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    uint32_t returned_physical_count = physical_count;
    std::vector<VkPhysicalDevice> physical_device_handles_1 = std::vector<VkPhysicalDevice>(physical_count);
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_device_handles_1.data()));
    ASSERT_EQ(physical_count, returned_physical_count);
    returned_physical_count = 5;
    std::vector<VkPhysicalDevice> physical_device_handles_2 = std::vector<VkPhysicalDevice>(returned_physical_count);
    ASSERT_EQ(VK_INCOMPLETE, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_device_handles_2.data()));
    ASSERT_EQ(5U, returned_physical_count);
    returned_physical_count = 3;
    std::vector<VkPhysicalDevice> physical_device_handles_3 = std::vector<VkPhysicalDevice>(returned_physical_count);
    ASSERT_EQ(VK_INCOMPLETE, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_device_handles_3.data()));
    ASSERT_EQ(3U, returned_physical_count);
    // Make sure devices are same between the three
    for (uint32_t count = 0; count < driver.physical_devices.size(); ++count) {
        if (count < physical_device_handles_2.size()) {
            ASSERT_EQ(physical_device_handles_2[count], physical_device_handles_1[count]);
        }
        if (count < physical_device_handles_3.size()) {
            ASSERT_EQ(physical_device_handles_3[count], physical_device_handles_1[count]);
        }
    }
}

TEST(EnumeratePhysicalDevices, CallThriceAddInBetween) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd().set_min_icd_interface_version(5);

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.emplace_back("physical_device_1");

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    uint32_t physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    uint32_t returned_physical_count = physical_count;
    std::vector<VkPhysicalDevice> physical_device_handles_1 = std::vector<VkPhysicalDevice>(physical_count);
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_device_handles_1.data()));
    ASSERT_EQ(physical_count, returned_physical_count);

    driver.physical_devices.emplace_back("physical_device_2");
    driver.physical_devices.emplace_back("physical_device_3");

    std::vector<VkPhysicalDevice> physical_device_handles_2 = std::vector<VkPhysicalDevice>(returned_physical_count);
    ASSERT_EQ(VK_INCOMPLETE, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_device_handles_2.data()));
    ASSERT_EQ(physical_count, returned_physical_count);

    physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    returned_physical_count = physical_count;
    std::vector<VkPhysicalDevice> physical_device_handles_3 = std::vector<VkPhysicalDevice>(returned_physical_count);
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_device_handles_3.data()));
    ASSERT_EQ(physical_count, returned_physical_count);
    // Make sure devices are same between the three
    for (uint32_t count = 0; count < physical_device_handles_3.size(); ++count) {
        if (count < physical_device_handles_1.size()) {
            ASSERT_EQ(physical_device_handles_1[count], physical_device_handles_3[count]);
        }
        if (count < physical_device_handles_2.size()) {
            ASSERT_EQ(physical_device_handles_2[count], physical_device_handles_3[count]);
        }
    }
}

TEST(EnumeratePhysicalDevices, CallThriceRemoveInBetween) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd().set_min_icd_interface_version(5);

    for (size_t i = 0; i < 4; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i));
    }

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    uint32_t physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    uint32_t returned_physical_count = physical_count;
    std::vector<VkPhysicalDevice> physical_device_handles_1 = std::vector<VkPhysicalDevice>(physical_count);
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_device_handles_1.data()));
    ASSERT_EQ(physical_count, returned_physical_count);

    // Delete the 2nd physical device
    driver.physical_devices.erase(std::next(driver.physical_devices.begin()));

    physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    std::vector<VkPhysicalDevice> physical_device_handles_2 = std::vector<VkPhysicalDevice>(returned_physical_count);
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_device_handles_2.data()));
    ASSERT_EQ(physical_count, returned_physical_count);
    physical_device_handles_2.resize(returned_physical_count);

    returned_physical_count = physical_count;
    std::vector<VkPhysicalDevice> physical_device_handles_3 = std::vector<VkPhysicalDevice>(returned_physical_count);
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_device_handles_3.data()));
    ASSERT_EQ(physical_count, returned_physical_count);

    // Make sure one has 1 more device that two or three
    ASSERT_EQ(physical_device_handles_1.size(), physical_device_handles_2.size() + 1);
    ASSERT_EQ(physical_device_handles_1.size(), physical_device_handles_3.size() + 1);

    // Make sure the devices in two and three are all found in one
    uint32_t two_found = 0;
    uint32_t three_found = 0;
    for (uint32_t count = 0; count < physical_device_handles_1.size(); ++count) {
        for (uint32_t int_count = 0; int_count < physical_device_handles_2.size(); ++int_count) {
            if (physical_device_handles_2[int_count] == physical_device_handles_1[count]) {
                two_found++;
                break;
            }
        }
        for (uint32_t int_count = 0; int_count < physical_device_handles_3.size(); ++int_count) {
            if (physical_device_handles_3[int_count] == physical_device_handles_1[count]) {
                three_found++;
                break;
            }
        }
    }
    ASSERT_EQ(two_found, returned_physical_count);
    ASSERT_EQ(three_found, returned_physical_count);
}

TEST(EnumeratePhysicalDevices, MultipleAddRemoves) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd().set_min_icd_interface_version(5);

    for (size_t i = 0; i < 4; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i));
    }
    std::array<std::vector<VkPhysicalDevice>, 8> physical_dev_handles;

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    uint32_t physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    uint32_t returned_physical_count = physical_count;
    physical_dev_handles[0].resize(physical_count);
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_dev_handles[0].data()));
    ASSERT_EQ(physical_count, returned_physical_count);

    // Delete the 2nd physical device (0, 2, 3)
    driver.physical_devices.erase(std::next(driver.physical_devices.begin()));

    // Query using old number from last call (4), but it should only return 3
    physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    physical_dev_handles[1].resize(returned_physical_count);
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_dev_handles[1].data()));
    ASSERT_EQ(physical_count, returned_physical_count);
    physical_dev_handles[1].resize(returned_physical_count);

    // Add two new physical devices to the front (A, B, 0, 2, 3)
    driver.physical_devices.emplace(driver.physical_devices.begin(), "physical_device_B");
    driver.physical_devices.emplace(driver.physical_devices.begin(), "physical_device_A");

    // Query using old number from last call (3), but it should be 5
    physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    physical_dev_handles[2].resize(returned_physical_count);
    ASSERT_EQ(VK_INCOMPLETE, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_dev_handles[2].data()));
    ASSERT_EQ(physical_count - 2, returned_physical_count);
    physical_dev_handles[2].resize(returned_physical_count);

    // Query again to get all 5
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, nullptr));
    physical_dev_handles[3].resize(returned_physical_count);
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_dev_handles[3].data()));
    ASSERT_EQ(physical_count, returned_physical_count);

    // Delete last two physical devices (A, B, 0, 2)
    driver.physical_devices.pop_back();

    // Query using old number from last call (5), but it should be 4
    physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    physical_dev_handles[4].resize(returned_physical_count);
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_dev_handles[4].data()));
    ASSERT_EQ(physical_count, returned_physical_count);
    physical_dev_handles[4].resize(returned_physical_count);
    // Adjust size and query again, should be the same
    physical_dev_handles[5].resize(returned_physical_count);
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_dev_handles[5].data()));

    // Insert a new physical device (A, B, C, 0, 2)
    driver.physical_devices.insert(driver.physical_devices.begin() + 2, "physical_device_C");

    // Query using old number from last call (4), but it should be 5
    physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    physical_dev_handles[6].resize(returned_physical_count);
    ASSERT_EQ(VK_INCOMPLETE, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_dev_handles[6].data()));
    ASSERT_EQ(physical_count - 1, returned_physical_count);
    // Query again to get all 5
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, nullptr));
    physical_dev_handles[7].resize(returned_physical_count);
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_dev_handles[7].data()));

    // Check final results
    // One   [4] - 0, 1, 2, 3
    // Two   [3] - 0, 2, 3
    // Three [3] - A, B, 0
    // Four  [5] - A, B, 0, 2, 3
    // Five  [4] - A, B, 0, 2
    // Six   [4] - A, B, 0, 2
    // Seven [4] - A, B, C, 0
    // Eight [5] - A, B, C, 0, 2
    ASSERT_EQ(4U, physical_dev_handles[0].size());
    ASSERT_EQ(3U, physical_dev_handles[1].size());
    ASSERT_EQ(3U, physical_dev_handles[2].size());
    ASSERT_EQ(5U, physical_dev_handles[3].size());
    ASSERT_EQ(4U, physical_dev_handles[4].size());
    ASSERT_EQ(4U, physical_dev_handles[5].size());
    ASSERT_EQ(4U, physical_dev_handles[6].size());
    ASSERT_EQ(5U, physical_dev_handles[7].size());

    // Make sure the devices in two and three are all found in one
    uint32_t found_items[8]{};
    for (uint32_t handle = 1; handle < 8; ++handle) {
        for (uint32_t count = 0; count < physical_dev_handles[0].size(); ++count) {
            for (uint32_t int_count = 0; int_count < physical_dev_handles[handle].size(); ++int_count) {
                if (physical_dev_handles[handle][int_count] == physical_dev_handles[0][count]) {
                    found_items[handle]++;
                    break;
                }
            }
        }
    }
    // Items matching from first call (must be >= since handle re-use does occur)
    ASSERT_EQ(found_items[1], 3U);
    ASSERT_GE(found_items[2], 1U);
    ASSERT_GE(found_items[3], 3U);
    ASSERT_GE(found_items[4], 2U);
    ASSERT_GE(found_items[5], 2U);
    ASSERT_GE(found_items[6], 1U);
    ASSERT_GE(found_items[7], 2U);

    memset(found_items, 0, 8 * sizeof(uint32_t));
    for (uint32_t handle = 0; handle < 7; ++handle) {
        for (uint32_t count = 0; count < physical_dev_handles[7].size(); ++count) {
            for (uint32_t int_count = 0; int_count < physical_dev_handles[handle].size(); ++int_count) {
                if (physical_dev_handles[handle][int_count] == physical_dev_handles[7][count]) {
                    found_items[handle]++;
                    break;
                }
            }
        }
    }
    // Items matching from last call (must be >= since handle re-use does occur)
    ASSERT_GE(found_items[0], 2U);
    ASSERT_GE(found_items[1], 2U);
    ASSERT_GE(found_items[2], 3U);
    ASSERT_GE(found_items[3], 4U);
    ASSERT_GE(found_items[4], 4U);
    ASSERT_GE(found_items[5], 4U);
    ASSERT_GE(found_items[6], 4U);
}

TEST(CreateDevice, ExtensionNotPresent) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd();

    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    uint32_t familyCount = 0;
    inst->vkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &familyCount, nullptr);
    ASSERT_EQ(familyCount, 1U);

    VkQueueFamilyProperties families;
    inst->vkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &familyCount, &families);
    ASSERT_EQ(familyCount, 1U);
    ASSERT_EQ(families, family_props.properties);

    DeviceWrapper dev{inst};
    dev.create_info.add_extension("NotPresent").add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));

    dev.CheckCreate(phys_dev, VK_ERROR_EXTENSION_NOT_PRESENT);
}

// LX535 / MI-76: Device layers are deprecated.
// Ensure that no errors occur if a bogus device layer list is passed to vkCreateDevice.
// https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#extendingvulkan-layers-devicelayerdeprecation
TEST(CreateDevice, LayersNotPresent) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd();

    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    uint32_t familyCount = 0;
    inst->vkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &familyCount, nullptr);
    ASSERT_EQ(familyCount, 1U);

    VkQueueFamilyProperties families;
    inst->vkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &familyCount, &families);
    ASSERT_EQ(familyCount, 1U);
    ASSERT_EQ(families, family_props.properties);

    DeviceWrapper dev{inst};
    dev.create_info.add_layer("NotPresent").add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));

    dev.CheckCreate(phys_dev);
}

TEST(CreateDevice, ConsecutiveCreate) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd();

    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};
    for (uint32_t i = 0; i < 100; i++) {
        driver.physical_devices.emplace_back("physical_device_0");
        driver.physical_devices.back().queue_family_properties.push_back(family_props);
    }
    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    auto phys_devs = inst.GetPhysDevs(100);
    for (uint32_t i = 0; i < 100; i++) {
        DeviceWrapper dev{inst};
        dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
        dev.CheckCreate(phys_devs[i]);
    }
}

TEST(CreateDevice, ConsecutiveCreateWithoutDestruction) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd();

    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};
    for (uint32_t i = 0; i < 100; i++) {
        driver.physical_devices.emplace_back("physical_device_0");
        driver.physical_devices.back().queue_family_properties.push_back(family_props);
    }
    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    auto phys_devs = inst.GetPhysDevs(100);

    std::vector<DeviceWrapper> devices;
    for (uint32_t i = 0; i < 100; i++) {
        devices.emplace_back(inst);
        DeviceWrapper& dev = devices.back();
        dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));

        dev.CheckCreate(phys_devs[i]);
    }
}

TEST(TryLoadWrongBinaries, WrongICD) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    env.add_icd(TestICDDetails(CURRENT_PLATFORM_DUMMY_BINARY_WRONG_TYPE).set_is_fake(true));
    env.get_test_icd().physical_devices.emplace_back("physical_device_0");

    DebugUtilsLogger log{VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    InstWrapper inst{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst.create_info, log);
    inst.CheckCreate();

#if _WIN32 || _WIN64
    ASSERT_TRUE(log.find("Failed to open dynamic library"));
#endif
#if defined(__linux__) || defined(__FreeBSD__)
#if defined(__x86_64__)
    ASSERT_TRUE(log.find("wrong ELF class: ELFCLASS32"));
#else
    ASSERT_TRUE(log.find("wrong ELF class: ELFCLASS64"));
#endif
#endif

    uint32_t driver_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &driver_count, nullptr));
    ASSERT_EQ(driver_count, 1U);
}

TEST(TryLoadWrongBinaries, WrongExplicit) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    env.get_test_icd().physical_devices.emplace_back("physical_device_0");

    const char* layer_name = "DummyLayerExplicit";
    env.add_fake_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(layer_name).set_lib_path(CURRENT_PLATFORM_DUMMY_BINARY_WRONG_TYPE)),
        "dummy_test_layer.json");

    uint32_t layer_count = 0;
    ASSERT_EQ(env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr), VK_SUCCESS);
    ASSERT_EQ(layer_count, 1U);

    std::array<VkLayerProperties, 2> layer_props;
    ASSERT_EQ(env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props.data()), VK_SUCCESS);
    ASSERT_EQ(layer_count, 1U);

    DebugUtilsLogger log{VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT};
    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer(layer_name);
    FillDebugUtilsCreateDetails(inst.create_info, log);

    // Explicit layer not found should generate a VK_ERROR_LAYER_NOT_PRESENT error message.
    inst.CheckCreate(VK_ERROR_LAYER_NOT_PRESENT);

    // Should get an error message for the explicit layer
#ifndef __APPLE__
    ASSERT_TRUE(log.find(std::string("Requested layer ") + std::string(layer_name) + std::string(" was wrong bit-type!")));
#else   // __APPLE__
    // Apple only throws a wrong library type of error
    ASSERT_TRUE(log.find(std::string("Requested layer ") + std::string(layer_name) + std::string(" failed to load!")));
#endif  // __APPLE__
}

TEST(TryLoadWrongBinaries, WrongImplicit) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    env.get_test_icd().physical_devices.emplace_back("physical_device_0");

    const char* layer_name = "DummyLayerImplicit0";
    env.add_fake_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                              .set_name(layer_name)
                                                              .set_lib_path(CURRENT_PLATFORM_DUMMY_BINARY_WRONG_TYPE)
                                                              .set_disable_environment("DISABLE_ENV")),
                                "dummy_test_layer.json");

    uint32_t layer_count = 0;
    ASSERT_EQ(env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr), VK_SUCCESS);
    ASSERT_EQ(layer_count, 1U);

    std::array<VkLayerProperties, 1> layer_props;
    ASSERT_EQ(env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props.data()), VK_SUCCESS);
    ASSERT_EQ(layer_count, 1U);

    DebugUtilsLogger log{VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    InstWrapper inst{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst.create_info, log);

    // We don't want to return VK_ERROR_LAYER_NOT_PRESENT for missing implicit layers because it's not the
    // application asking for them.
    inst.CheckCreate(VK_SUCCESS);

#ifndef __APPLE__
    // Should get an info message for the bad implicit layer
    ASSERT_TRUE(log.find(std::string("Requested layer ") + std::string(layer_name) + std::string(" was wrong bit-type.")));
#else   // __APPLE__
    // Apple only throws a wrong library type of error
    ASSERT_TRUE(log.find(std::string("Requested layer ") + std::string(layer_name) + std::string(" failed to load.")));
#endif  // __APPLE__
}

TEST(TryLoadWrongBinaries, WrongExplicitAndImplicit) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    env.get_test_icd().physical_devices.emplace_back("physical_device_0");

    const char* layer_name_0 = "DummyLayerExplicit";
    env.add_fake_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(layer_name_0).set_lib_path(CURRENT_PLATFORM_DUMMY_BINARY_WRONG_TYPE)),
        "dummy_test_layer_0.json");
    const char* layer_name_1 = "DummyLayerImplicit";
    env.add_fake_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                              .set_name(layer_name_1)
                                                              .set_lib_path(CURRENT_PLATFORM_DUMMY_BINARY_WRONG_TYPE)
                                                              .set_disable_environment("DISABLE_ENV")),
                                "dummy_test_layer_1.json");

    uint32_t layer_count = 0;
    ASSERT_EQ(env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr), VK_SUCCESS);
    ASSERT_EQ(layer_count, 2U);

    std::array<VkLayerProperties, 2> layer_props;
    ASSERT_EQ(env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props.data()), VK_SUCCESS);
    ASSERT_EQ(layer_count, 2U);

    DebugUtilsLogger log{VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer(layer_name_0);
    FillDebugUtilsCreateDetails(inst.create_info, log);

    // Explicit layer not found should generate a VK_ERROR_LAYER_NOT_PRESENT error message.
    inst.CheckCreate(VK_ERROR_LAYER_NOT_PRESENT);

#ifndef __APPLE__
    // Should get error messages for both (the explicit is second and we don't want the implicit to return before the explicit
    // triggers a failure during vkCreateInstance)
    ASSERT_TRUE(log.find(std::string("Requested layer ") + std::string(layer_name_0) + std::string(" was wrong bit-type!")));
    ASSERT_TRUE(log.find(std::string("Requested layer ") + std::string(layer_name_1) + std::string(" was wrong bit-type.")));
#else   // __APPLE__
    // Apple only throws a wrong library type of error
    ASSERT_TRUE(log.find(std::string("Requested layer ") + std::string(layer_name_0) + std::string(" failed to load!")));
    ASSERT_TRUE(log.find(std::string("Requested layer ") + std::string(layer_name_1) + std::string(" failed to load.")));
#endif  // __APPLE__
}

TEST(TryLoadWrongBinaries, WrongExplicitAndImplicitErrorOnly) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    env.get_test_icd().physical_devices.emplace_back("physical_device_0");

    const char* layer_name_0 = "DummyLayerExplicit";
    env.add_fake_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(layer_name_0).set_lib_path(CURRENT_PLATFORM_DUMMY_BINARY_WRONG_TYPE)),
        "dummy_test_layer_0.json");
    const char* layer_name_1 = "DummyLayerImplicit";
    env.add_fake_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                              .set_name(layer_name_1)
                                                              .set_lib_path(CURRENT_PLATFORM_DUMMY_BINARY_WRONG_TYPE)
                                                              .set_disable_environment("DISABLE_ENV")),
                                "dummy_test_layer_1.json");

    uint32_t layer_count = 0;
    ASSERT_EQ(env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr), VK_SUCCESS);
    ASSERT_EQ(layer_count, 2U);

    std::array<VkLayerProperties, 2> layer_props;
    ASSERT_EQ(env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props.data()), VK_SUCCESS);
    ASSERT_EQ(layer_count, 2U);

    DebugUtilsLogger log{VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT};
    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer(layer_name_0);
    FillDebugUtilsCreateDetails(inst.create_info, log);

    // Explicit layer not found should generate a VK_ERROR_LAYER_NOT_PRESENT error message.
    inst.CheckCreate(VK_ERROR_LAYER_NOT_PRESENT);

#ifndef __APPLE__
    // Should not get an error messages for either
    ASSERT_TRUE(log.find(std::string("Requested layer ") + std::string(layer_name_0) + std::string(" was wrong bit-type!")));
    ASSERT_FALSE(log.find(std::string("Requested layer ") + std::string(layer_name_1) + std::string(" was wrong bit-type.")));
#else   // __APPLE__
    // Apple only throws a wrong library type of error
    ASSERT_TRUE(log.find(std::string("Requested layer ") + std::string(layer_name_0) + std::string(" failed to load!")));
    ASSERT_FALSE(log.find(std::string("Requested layer ") + std::string(layer_name_1) + std::string(" failed to load.")));
#endif  // __APPLE__
}

TEST(TryLoadWrongBinaries, BadExplicit) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    env.get_test_icd().physical_devices.emplace_back("physical_device_0");

    const char* layer_name = "DummyLayerExplicit";
    env.add_fake_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(layer_name).set_lib_path(CURRENT_PLATFORM_DUMMY_BINARY_BAD)),
        "dummy_test_layer.json");

    uint32_t layer_count = 0;
    ASSERT_EQ(env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr), VK_SUCCESS);
    ASSERT_EQ(layer_count, 1U);

    std::array<VkLayerProperties, 2> layer_props;
    ASSERT_EQ(env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props.data()), VK_SUCCESS);
    ASSERT_EQ(layer_count, 1U);

    DebugUtilsLogger log{VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT};
    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer(layer_name);
    FillDebugUtilsCreateDetails(inst.create_info, log);

    // Explicit layer not found should generate a VK_ERROR_LAYER_NOT_PRESENT error message.
    inst.CheckCreate(VK_ERROR_LAYER_NOT_PRESENT);

    // Should get an error message for the bad explicit
    ASSERT_TRUE(log.find(std::string("Requested layer ") + std::string(layer_name) + std::string(" failed to load!")));
}

TEST(TryLoadWrongBinaries, BadImplicit) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    env.get_test_icd().physical_devices.emplace_back("physical_device_0");

    const char* layer_name = "DummyLayerImplicit0";
    env.add_fake_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                              .set_name(layer_name)
                                                              .set_lib_path(CURRENT_PLATFORM_DUMMY_BINARY_BAD)
                                                              .set_disable_environment("DISABLE_ENV")),
                                "dummy_test_layer.json");

    uint32_t layer_count = 0;
    ASSERT_EQ(env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr), VK_SUCCESS);
    ASSERT_EQ(layer_count, 1U);

    std::array<VkLayerProperties, 1> layer_props;
    ASSERT_EQ(env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props.data()), VK_SUCCESS);
    ASSERT_EQ(layer_count, 1U);

    DebugUtilsLogger log{VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    InstWrapper inst{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst.create_info, log);

    // We don't want to return VK_ERROR_LAYER_NOT_PRESENT for missing implicit layers because it's not the
    // application asking for them.
    inst.CheckCreate(VK_SUCCESS);

    // Should get an info message for the bad implicit
    ASSERT_TRUE(log.find(std::string("Requested layer ") + std::string(layer_name) + std::string(" failed to load.")));
}

TEST(TryLoadWrongBinaries, BadExplicitAndImplicit) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    env.get_test_icd().physical_devices.emplace_back("physical_device_0");

    const char* layer_name_0 = "DummyLayerExplicit";
    env.add_fake_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(layer_name_0).set_lib_path(CURRENT_PLATFORM_DUMMY_BINARY_BAD)),
        "dummy_test_layer_0.json");
    const char* layer_name_1 = "DummyLayerImplicit0";
    env.add_fake_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                              .set_name(layer_name_1)
                                                              .set_lib_path(CURRENT_PLATFORM_DUMMY_BINARY_BAD)
                                                              .set_disable_environment("DISABLE_ENV")),
                                "dummy_test_layer_1.json");

    uint32_t layer_count = 0;
    ASSERT_EQ(env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr), VK_SUCCESS);
    ASSERT_EQ(layer_count, 2U);

    std::array<VkLayerProperties, 2> layer_props;
    ASSERT_EQ(env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props.data()), VK_SUCCESS);
    ASSERT_EQ(layer_count, 2U);

    DebugUtilsLogger log{VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer(layer_name_0);
    FillDebugUtilsCreateDetails(inst.create_info, log);

    // Explicit layer not found should generate a VK_ERROR_LAYER_NOT_PRESENT error message.
    inst.CheckCreate(VK_ERROR_LAYER_NOT_PRESENT);

    // Apple only throws a wrong library type of error
    ASSERT_TRUE(log.find(std::string("Requested layer ") + std::string(layer_name_0) + std::string(" failed to load!")));
    ASSERT_TRUE(log.find(std::string("Requested layer ") + std::string(layer_name_1) + std::string(" failed to load.")));
}

TEST(EnumeratePhysicalDeviceGroups, OneCall) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd().set_min_icd_interface_version(5).set_icd_api_version(VK_API_VERSION_1_1);
    driver.add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});

    // ICD contains 3 devices in two groups
    for (size_t i = 0; i < 3; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i), rand() % 50 + 3);
        driver.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    }
    driver.physical_device_groups.emplace_back(driver.physical_devices[0]);
    driver.physical_device_groups.back().use_physical_device(driver.physical_devices[1]);
    driver.physical_device_groups.emplace_back(driver.physical_devices[2]);
    const uint32_t max_physical_device_count = 3;

    // Core function
    {
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(VK_API_VERSION_1_1);
        inst.CheckCreate();

        auto physical_devices = std::vector<VkPhysicalDevice>(max_physical_device_count);
        uint32_t returned_phys_dev_count = max_physical_device_count;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_phys_dev_count, physical_devices.data()));
        handle_assert_has_values(physical_devices);

        uint32_t group_count = static_cast<uint32_t>(driver.physical_device_groups.size());
        uint32_t returned_group_count = group_count;
        std::vector<VkPhysicalDeviceGroupProperties> group_props{};
        group_props.resize(group_count, VkPhysicalDeviceGroupProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES});
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, group_props.data()));
        ASSERT_EQ(group_count, returned_group_count);

        // Make sure each physical device shows up in a group, but only once
        std::array<bool, max_physical_device_count> found{false};
        for (uint32_t group = 0; group < group_count; ++group) {
            for (uint32_t g_dev = 0; g_dev < group_props[group].physicalDeviceCount; ++g_dev) {
                for (uint32_t dev = 0; dev < max_physical_device_count; ++dev) {
                    if (physical_devices[dev] == group_props[group].physicalDevices[g_dev]) {
                        ASSERT_EQ(false, found[dev]);
                        found[dev] = true;
                        break;
                    }
                }
            }
        }
        for (uint32_t dev = 0; dev < max_physical_device_count; ++dev) {
            ASSERT_EQ(true, found[dev]);
        }
    }
    driver.add_instance_extension({VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME});
    // Extension
    {
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.add_extension(VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME);
        inst.CheckCreate();

        auto vkEnumeratePhysicalDeviceGroupsKHR = reinterpret_cast<PFN_vkEnumeratePhysicalDeviceGroupsKHR>(
            env.vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkEnumeratePhysicalDeviceGroupsKHR"));

        auto physical_devices = std::vector<VkPhysicalDevice>(max_physical_device_count);
        uint32_t returned_phys_dev_count = max_physical_device_count;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_phys_dev_count, physical_devices.data()));
        handle_assert_has_values(physical_devices);

        uint32_t group_count = static_cast<uint32_t>(driver.physical_device_groups.size());
        uint32_t returned_group_count = group_count;
        std::vector<VkPhysicalDeviceGroupPropertiesKHR> group_props{};
        group_props.resize(group_count, VkPhysicalDeviceGroupPropertiesKHR{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES_KHR});
        ASSERT_EQ(VK_SUCCESS, vkEnumeratePhysicalDeviceGroupsKHR(inst, &returned_group_count, group_props.data()));
        ASSERT_EQ(group_count, returned_group_count);

        // Make sure each physical device shows up in a group, but only once
        std::array<bool, max_physical_device_count> found{false};
        for (uint32_t group = 0; group < group_count; ++group) {
            for (uint32_t g_dev = 0; g_dev < group_props[group].physicalDeviceCount; ++g_dev) {
                for (uint32_t dev = 0; dev < max_physical_device_count; ++dev) {
                    if (physical_devices[dev] == group_props[group].physicalDevices[g_dev]) {
                        ASSERT_EQ(false, found[dev]);
                        found[dev] = true;
                        break;
                    }
                }
            }
        }
        for (uint32_t dev = 0; dev < max_physical_device_count; ++dev) {
            ASSERT_EQ(true, found[dev]);
        }
    }
}

TEST(EnumeratePhysicalDeviceGroups, TwoCall) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd().set_min_icd_interface_version(5).set_icd_api_version(VK_API_VERSION_1_1);
    driver.add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});

    // ICD contains 3 devices in two groups
    for (size_t i = 0; i < 3; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i), rand() % 50 + 3);
        driver.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    }
    driver.physical_device_groups.emplace_back(driver.physical_devices[0]);
    driver.physical_device_groups.back().use_physical_device(driver.physical_devices[1]);
    driver.physical_device_groups.emplace_back(driver.physical_devices[2]);
    const uint32_t max_physical_device_count = 3;

    // Core function
    {
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(VK_API_VERSION_1_1);
        inst.CheckCreate();

        auto physical_devices = std::vector<VkPhysicalDevice>(max_physical_device_count);
        uint32_t returned_phys_dev_count = max_physical_device_count;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_phys_dev_count, physical_devices.data()));
        handle_assert_has_values(physical_devices);

        uint32_t group_count = static_cast<uint32_t>(driver.physical_device_groups.size());
        uint32_t returned_group_count = 0;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, nullptr));
        ASSERT_EQ(group_count, returned_group_count);

        std::vector<VkPhysicalDeviceGroupProperties> group_props{};
        group_props.resize(group_count, VkPhysicalDeviceGroupProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES});
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, group_props.data()));
        ASSERT_EQ(group_count, returned_group_count);

        // Make sure each physical device shows up in a group, but only once
        std::array<bool, max_physical_device_count> found{false};
        for (uint32_t group = 0; group < group_count; ++group) {
            for (uint32_t g_dev = 0; g_dev < group_props[group].physicalDeviceCount; ++g_dev) {
                for (uint32_t dev = 0; dev < max_physical_device_count; ++dev) {
                    if (physical_devices[dev] == group_props[group].physicalDevices[g_dev]) {
                        ASSERT_EQ(false, found[dev]);
                        found[dev] = true;
                        break;
                    }
                }
            }
        }
        for (uint32_t dev = 0; dev < max_physical_device_count; ++dev) {
            ASSERT_EQ(true, found[dev]);
        }
    }
    driver.add_instance_extension({VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME});
    // Extension
    {
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.add_extension("VK_KHR_device_group_creation");
        inst.CheckCreate();

        auto physical_devices = std::vector<VkPhysicalDevice>(max_physical_device_count);
        uint32_t returned_phys_dev_count = max_physical_device_count;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_phys_dev_count, physical_devices.data()));
        handle_assert_has_values(physical_devices);

        auto vkEnumeratePhysicalDeviceGroupsKHR = reinterpret_cast<PFN_vkEnumeratePhysicalDeviceGroupsKHR>(
            env.vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkEnumeratePhysicalDeviceGroupsKHR"));

        uint32_t group_count = static_cast<uint32_t>(driver.physical_device_groups.size());
        uint32_t returned_group_count = 0;
        ASSERT_EQ(VK_SUCCESS, vkEnumeratePhysicalDeviceGroupsKHR(inst, &returned_group_count, nullptr));
        ASSERT_EQ(group_count, returned_group_count);

        std::vector<VkPhysicalDeviceGroupPropertiesKHR> group_props{};
        group_props.resize(group_count, VkPhysicalDeviceGroupPropertiesKHR{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES_KHR});
        ASSERT_EQ(VK_SUCCESS, vkEnumeratePhysicalDeviceGroupsKHR(inst, &returned_group_count, group_props.data()));
        ASSERT_EQ(group_count, returned_group_count);

        // Make sure each physical device shows up in a group, but only once
        std::array<bool, max_physical_device_count> found{false};
        for (uint32_t group = 0; group < group_count; ++group) {
            for (uint32_t g_dev = 0; g_dev < group_props[group].physicalDeviceCount; ++g_dev) {
                for (uint32_t dev = 0; dev < max_physical_device_count; ++dev) {
                    if (physical_devices[dev] == group_props[group].physicalDevices[g_dev]) {
                        ASSERT_EQ(false, found[dev]);
                        found[dev] = true;
                        break;
                    }
                }
            }
        }
        for (uint32_t dev = 0; dev < max_physical_device_count; ++dev) {
            ASSERT_EQ(true, found[dev]);
        }
    }
}

TEST(EnumeratePhysicalDeviceGroups, TwoCallIncomplete) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd().set_min_icd_interface_version(5).set_icd_api_version(VK_API_VERSION_1_1);
    driver.add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});

    // ICD contains 3 devices in two groups
    for (size_t i = 0; i < 3; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i), rand() % 50 + 3);
        driver.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    }
    driver.physical_device_groups.emplace_back(driver.physical_devices[0]);
    driver.physical_device_groups.back().use_physical_device(driver.physical_devices[1]);
    driver.physical_device_groups.emplace_back(driver.physical_devices[2]);

    // Core function
    {
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(VK_API_VERSION_1_1);
        inst.CheckCreate();

        uint32_t group_count = static_cast<uint32_t>(driver.physical_device_groups.size());
        uint32_t returned_group_count = 0;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, nullptr));
        ASSERT_EQ(group_count, returned_group_count);

        returned_group_count = 1;
        std::array<VkPhysicalDeviceGroupProperties, 1> group_props{};
        group_props[0].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
        ASSERT_EQ(VK_INCOMPLETE, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, group_props.data()));
        ASSERT_EQ(1U, returned_group_count);

        returned_group_count = 2;
        std::array<VkPhysicalDeviceGroupProperties, 2> group_props_2{};
        group_props_2[0].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
        group_props_2[1].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, group_props_2.data()));
        ASSERT_EQ(2U, returned_group_count);

        // Make sure the incomplete group items appear in the complete group
        for (uint32_t inc_group = 0; inc_group < 1; ++inc_group) {
            bool found = false;
            for (uint32_t full_group = 0; full_group < 2; ++full_group) {
                if (group_props[inc_group].physicalDeviceCount == group_props_2[full_group].physicalDeviceCount &&
                    group_props[inc_group].physicalDevices[0] == group_props_2[full_group].physicalDevices[0] &&
                    group_props[inc_group].physicalDevices[1] == group_props_2[full_group].physicalDevices[1]) {
                    found = true;
                    break;
                }
            }
            ASSERT_EQ(true, found);
        }
    }
    driver.add_instance_extension({VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME});
    // Extension
    {
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.add_extension("VK_KHR_device_group_creation");
        inst.CheckCreate();

        auto vkEnumeratePhysicalDeviceGroupsKHR = reinterpret_cast<PFN_vkEnumeratePhysicalDeviceGroupsKHR>(
            env.vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkEnumeratePhysicalDeviceGroupsKHR"));

        uint32_t group_count = static_cast<uint32_t>(driver.physical_device_groups.size());
        uint32_t returned_group_count = 0;
        ASSERT_EQ(VK_SUCCESS, vkEnumeratePhysicalDeviceGroupsKHR(inst, &returned_group_count, nullptr));
        ASSERT_EQ(group_count, returned_group_count);

        returned_group_count = 1;
        std::array<VkPhysicalDeviceGroupPropertiesKHR, 1> group_props{};
        group_props[0].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
        ASSERT_EQ(VK_INCOMPLETE, vkEnumeratePhysicalDeviceGroupsKHR(inst, &returned_group_count, group_props.data()));
        ASSERT_EQ(1U, returned_group_count);

        returned_group_count = 2;
        std::array<VkPhysicalDeviceGroupPropertiesKHR, 2> group_props_2{};
        group_props_2[0].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
        group_props_2[1].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
        ASSERT_EQ(VK_SUCCESS, vkEnumeratePhysicalDeviceGroupsKHR(inst, &returned_group_count, group_props_2.data()));
        ASSERT_EQ(2U, returned_group_count);

        // Make sure the incomplete group items appear in the complete group
        for (uint32_t inc_group = 0; inc_group < 1; ++inc_group) {
            bool found = false;
            for (uint32_t full_group = 0; full_group < 2; ++full_group) {
                if (group_props[inc_group].physicalDeviceCount == group_props_2[full_group].physicalDeviceCount &&
                    group_props[inc_group].physicalDevices[0] == group_props_2[full_group].physicalDevices[0] &&
                    group_props[inc_group].physicalDevices[1] == group_props_2[full_group].physicalDevices[1]) {
                    found = true;
                    break;
                }
            }
            ASSERT_EQ(true, found);
        }
    }
}

// Call the core vkEnumeratePhysicalDeviceGroups and the extension
// vkEnumeratePhysicalDeviceGroupsKHR, and make sure they return the same info.
TEST(EnumeratePhysicalDeviceGroups, TestCoreVersusExtensionSameReturns) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd().set_min_icd_interface_version(5).set_icd_api_version(VK_API_VERSION_1_1);
    driver.add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    driver.add_instance_extension({VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME});

    // Generate the devices
    for (size_t i = 0; i < 6; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i));
    }

    // Generate the starting groups
    driver.physical_device_groups.emplace_back(driver.physical_devices[0]);
    driver.physical_device_groups.emplace_back(driver.physical_devices[1]);
    driver.physical_device_groups.back()
        .use_physical_device(driver.physical_devices[2])
        .use_physical_device(driver.physical_devices[3]);
    driver.physical_device_groups.emplace_back(driver.physical_devices[4]);
    driver.physical_device_groups.back().use_physical_device(driver.physical_devices[5]);

    uint32_t expected_counts[3] = {1, 3, 2};
    uint32_t core_group_count = 0;
    std::vector<VkPhysicalDeviceGroupProperties> core_group_props{};
    uint32_t ext_group_count = 0;
    std::vector<VkPhysicalDeviceGroupPropertiesKHR> ext_group_props{};

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(1, 1, 0);
    inst.create_info.add_extension("VK_KHR_device_group_creation");
    inst.CheckCreate();

    // Core function
    core_group_count = static_cast<uint32_t>(driver.physical_device_groups.size());
    uint32_t returned_group_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, nullptr));
    ASSERT_EQ(core_group_count, returned_group_count);

    core_group_props.resize(returned_group_count,
                            VkPhysicalDeviceGroupProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES});
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, core_group_props.data()));
    ASSERT_EQ(core_group_count, returned_group_count);

    auto vkEnumeratePhysicalDeviceGroupsKHR = reinterpret_cast<PFN_vkEnumeratePhysicalDeviceGroupsKHR>(
        env.vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkEnumeratePhysicalDeviceGroupsKHR"));

    ext_group_count = static_cast<uint32_t>(driver.physical_device_groups.size());
    returned_group_count = 0;
    ASSERT_EQ(VK_SUCCESS, vkEnumeratePhysicalDeviceGroupsKHR(inst, &returned_group_count, nullptr));
    ASSERT_EQ(ext_group_count, returned_group_count);

    ext_group_props.resize(returned_group_count,
                           VkPhysicalDeviceGroupPropertiesKHR{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES_KHR});
    ASSERT_EQ(VK_SUCCESS, vkEnumeratePhysicalDeviceGroupsKHR(inst, &returned_group_count, ext_group_props.data()));
    ASSERT_EQ(ext_group_count, returned_group_count);

    // Make sure data from each matches
    ASSERT_EQ(core_group_count, 3U);
    ASSERT_EQ(ext_group_count, 3U);
    for (uint32_t group = 0; group < core_group_count; ++group) {
        ASSERT_EQ(core_group_props[group].physicalDeviceCount, expected_counts[group]);
        ASSERT_EQ(ext_group_props[group].physicalDeviceCount, expected_counts[group]);
        for (uint32_t dev = 0; dev < core_group_props[group].physicalDeviceCount; ++dev) {
            ASSERT_EQ(core_group_props[group].physicalDevices[dev], ext_group_props[group].physicalDevices[dev]);
        }
    }
    // Make sure no physical device appears in more than one group
    for (uint32_t group1 = 0; group1 < core_group_count; ++group1) {
        for (uint32_t group2 = group1 + 1; group2 < core_group_count; ++group2) {
            for (uint32_t dev1 = 0; dev1 < core_group_props[group1].physicalDeviceCount; ++dev1) {
                for (uint32_t dev2 = 0; dev2 < core_group_props[group1].physicalDeviceCount; ++dev2) {
                    ASSERT_NE(core_group_props[group1].physicalDevices[dev1], core_group_props[group2].physicalDevices[dev2]);
                }
            }
        }
    }
}

// Start with 6 devices in 3 different groups, and then add a group,
// querying vkEnumeratePhysicalDeviceGroups before and after the add.
TEST(EnumeratePhysicalDeviceGroups, CallThriceAddGroupInBetween) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd().set_min_icd_interface_version(5).set_icd_api_version(VK_API_VERSION_1_1);

    // Generate the devices
    for (size_t i = 0; i < 7; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i));
    }

    // Generate the starting groups
    driver.physical_device_groups.emplace_back(driver.physical_devices[0]);
    driver.physical_device_groups.emplace_back(driver.physical_devices[1]);
    driver.physical_device_groups.back()
        .use_physical_device(driver.physical_devices[2])
        .use_physical_device(driver.physical_devices[3]);
    driver.physical_device_groups.emplace_back(driver.physical_devices[4]);
    driver.physical_device_groups.back().use_physical_device(driver.physical_devices[5]);

    uint32_t before_expected_counts[3] = {1, 3, 2};
    uint32_t after_expected_counts[4] = {1, 3, 1, 2};
    uint32_t before_group_count = 3;
    uint32_t after_group_count = 4;
    uint32_t returned_group_count = 0;

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(1, 1, 0);
    inst.CheckCreate();

    std::vector<VkPhysicalDeviceGroupProperties> group_props_before{};
    group_props_before.resize(before_group_count,
                              VkPhysicalDeviceGroupProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES});
    returned_group_count = before_group_count;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, group_props_before.data()));
    ASSERT_EQ(before_group_count, returned_group_count);
    for (uint32_t group = 0; group < before_group_count; ++group) {
        ASSERT_EQ(group_props_before[group].physicalDeviceCount, before_expected_counts[group]);
    }

    // Insert new group after first two
    driver.physical_device_groups.insert(driver.physical_device_groups.begin() + 2, driver.physical_devices[6]);

    std::vector<VkPhysicalDeviceGroupProperties> group_props_after{};
    group_props_after.resize(before_group_count,
                             VkPhysicalDeviceGroupProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES});
    ASSERT_EQ(VK_INCOMPLETE, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, group_props_after.data()));
    ASSERT_EQ(before_group_count, returned_group_count);
    for (uint32_t group = 0; group < before_group_count; ++group) {
        ASSERT_EQ(group_props_after[group].physicalDeviceCount, after_expected_counts[group]);
    }

    group_props_after.resize(after_group_count,
                             VkPhysicalDeviceGroupProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES});
    returned_group_count = after_group_count;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, group_props_after.data()));
    ASSERT_EQ(after_group_count, returned_group_count);
    for (uint32_t group = 0; group < after_group_count; ++group) {
        ASSERT_EQ(group_props_after[group].physicalDeviceCount, after_expected_counts[group]);
    }

    // Make sure all devices in the old group info are still found in the new group info
    for (uint32_t group1 = 0; group1 < group_props_before.size(); ++group1) {
        for (uint32_t group2 = 0; group2 < group_props_after.size(); ++group2) {
            if (group_props_before[group1].physicalDeviceCount == group_props_after[group2].physicalDeviceCount) {
                uint32_t found_count = 0;
                bool found;
                for (uint32_t dev1 = 0; dev1 < group_props_before[group1].physicalDeviceCount; ++dev1) {
                    found = false;
                    for (uint32_t dev2 = 0; dev2 < group_props_after[group2].physicalDeviceCount; ++dev2) {
                        if (group_props_before[group1].physicalDevices[dev1] == group_props_after[group2].physicalDevices[dev2]) {
                            found_count++;
                            found = true;
                            break;
                        }
                    }
                }
                ASSERT_EQ(found, found_count == group_props_before[group1].physicalDeviceCount);
            }
        }
    }
}

// Start with 7 devices in 4 different groups, and then remove a group,
// querying vkEnumeratePhysicalDeviceGroups before and after the remove.
TEST(EnumeratePhysicalDeviceGroups, CallTwiceRemoveGroupInBetween) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd().set_min_icd_interface_version(5).set_icd_api_version(VK_API_VERSION_1_1);

    // Generate the devices
    for (size_t i = 0; i < 7; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i));
    }

    // Generate the starting groups
    driver.physical_device_groups.emplace_back(driver.physical_devices[0]);
    driver.physical_device_groups.emplace_back(driver.physical_devices[1]);
    driver.physical_device_groups.back()
        .use_physical_device(driver.physical_devices[2])
        .use_physical_device(driver.physical_devices[3]);
    driver.physical_device_groups.emplace_back(driver.physical_devices[4]);
    driver.physical_device_groups.emplace_back(driver.physical_devices[5]);
    driver.physical_device_groups.back().use_physical_device(driver.physical_devices[6]);

    uint32_t before_expected_counts[4] = {1, 3, 1, 2};
    uint32_t after_expected_counts[3] = {1, 3, 2};
    uint32_t before_group_count = 4;
    uint32_t after_group_count = 3;
    uint32_t returned_group_count = 0;

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(1, 1, 0);
    inst.CheckCreate();

    std::vector<VkPhysicalDeviceGroupProperties> group_props_before{};
    group_props_before.resize(before_group_count,
                              VkPhysicalDeviceGroupProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES});
    returned_group_count = before_group_count;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, group_props_before.data()));
    ASSERT_EQ(before_group_count, returned_group_count);
    for (uint32_t group = 0; group < before_group_count; ++group) {
        ASSERT_EQ(group_props_before[group].physicalDeviceCount, before_expected_counts[group]);
    }

    // Insert new group after first two
    driver.physical_device_groups.erase(driver.physical_device_groups.begin() + 2);

    std::vector<VkPhysicalDeviceGroupProperties> group_props_after{};
    group_props_after.resize(after_group_count,
                             VkPhysicalDeviceGroupProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES});
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, group_props_after.data()));
    ASSERT_EQ(after_group_count, returned_group_count);
    for (uint32_t group = 0; group < after_group_count; ++group) {
        ASSERT_EQ(group_props_after[group].physicalDeviceCount, after_expected_counts[group]);
    }

    // Make sure all devices in the new group info are found in the old group info
    for (uint32_t group1 = 0; group1 < group_props_after.size(); ++group1) {
        for (uint32_t group2 = 0; group2 < group_props_before.size(); ++group2) {
            if (group_props_after[group1].physicalDeviceCount == group_props_before[group2].physicalDeviceCount) {
                uint32_t found_count = 0;
                bool found;
                for (uint32_t dev1 = 0; dev1 < group_props_after[group1].physicalDeviceCount; ++dev1) {
                    found = false;
                    for (uint32_t dev2 = 0; dev2 < group_props_before[group2].physicalDeviceCount; ++dev2) {
                        if (group_props_after[group1].physicalDevices[dev1] == group_props_before[group2].physicalDevices[dev2]) {
                            found_count++;
                            found = true;
                            break;
                        }
                    }
                }
                ASSERT_EQ(found, found_count == group_props_after[group1].physicalDeviceCount);
            }
        }
    }
}

// Start with 6 devices in 3 different groups, and then add a device to the middle group,
// querying vkEnumeratePhysicalDeviceGroups before and after the add.
TEST(EnumeratePhysicalDeviceGroups, CallTwiceAddDeviceInBetween) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd().set_min_icd_interface_version(5).set_icd_api_version(VK_API_VERSION_1_1);

    // Generate the devices
    for (size_t i = 0; i < 7; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i));
    }

    // Generate the starting groups
    driver.physical_device_groups.emplace_back(driver.physical_devices[0]);
    driver.physical_device_groups.emplace_back(driver.physical_devices[1]);
    driver.physical_device_groups.back()
        .use_physical_device(driver.physical_devices[2])
        .use_physical_device(driver.physical_devices[3]);
    driver.physical_device_groups.emplace_back(driver.physical_devices[4]);
    driver.physical_device_groups.back().use_physical_device(driver.physical_devices[5]);

    uint32_t expected_group_count = 3;
    uint32_t before_expected_counts[3] = {1, 3, 2};
    uint32_t after_expected_counts[3] = {1, 4, 2};
    uint32_t returned_group_count = 0;

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(1, 1, 0);
    inst.CheckCreate();

    std::vector<VkPhysicalDeviceGroupProperties> group_props_before{};
    group_props_before.resize(expected_group_count,
                              VkPhysicalDeviceGroupProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES});
    returned_group_count = expected_group_count;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, group_props_before.data()));
    ASSERT_EQ(expected_group_count, returned_group_count);
    for (uint32_t group = 0; group < expected_group_count; ++group) {
        ASSERT_EQ(group_props_before[group].physicalDeviceCount, before_expected_counts[group]);
    }

    // Insert new device to 2nd group
    driver.physical_device_groups[1].use_physical_device(driver.physical_devices[6]);

    std::vector<VkPhysicalDeviceGroupProperties> group_props_after{};
    group_props_after.resize(expected_group_count,
                             VkPhysicalDeviceGroupProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES});
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, group_props_after.data()));
    ASSERT_EQ(expected_group_count, returned_group_count);
    for (uint32_t group = 0; group < expected_group_count; ++group) {
        ASSERT_EQ(group_props_after[group].physicalDeviceCount, after_expected_counts[group]);
    }

    // Make sure all devices in the old group info are still found in the new group info
    for (uint32_t group1 = 0; group1 < group_props_before.size(); ++group1) {
        for (uint32_t group2 = 0; group2 < group_props_after.size(); ++group2) {
            uint32_t found_count = 0;
            bool found;
            for (uint32_t dev1 = 0; dev1 < group_props_before[group1].physicalDeviceCount; ++dev1) {
                found = false;
                for (uint32_t dev2 = 0; dev2 < group_props_after[group2].physicalDeviceCount; ++dev2) {
                    if (group_props_before[group1].physicalDevices[dev1] == group_props_after[group2].physicalDevices[dev2]) {
                        found_count++;
                        found = true;
                        break;
                    }
                }
            }
            ASSERT_EQ(found, found_count != 0 && found_count == before_expected_counts[group1]);
            if (found) {
                break;
            }
        }
    }
}

// Start with 6 devices in 3 different groups, and then remove a device to the middle group,
// querying vkEnumeratePhysicalDeviceGroups before and after the remove.
TEST(EnumeratePhysicalDeviceGroups, CallTwiceRemoveDeviceInBetween) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd().set_min_icd_interface_version(5).set_icd_api_version(VK_API_VERSION_1_1);

    // Generate the devices
    for (size_t i = 0; i < 6; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i));
    }

    // Generate the starting groups
    driver.physical_device_groups.emplace_back(driver.physical_devices[0]);
    driver.physical_device_groups.emplace_back(driver.physical_devices[1]);
    driver.physical_device_groups.back()
        .use_physical_device(driver.physical_devices[2])
        .use_physical_device(driver.physical_devices[3]);
    driver.physical_device_groups.emplace_back(driver.physical_devices[4]);
    driver.physical_device_groups.back().use_physical_device(driver.physical_devices[5]);

    uint32_t before_expected_counts[3] = {1, 3, 2};
    uint32_t after_expected_counts[3] = {1, 2, 2};
    uint32_t expected_group_count = 3;
    uint32_t returned_group_count = 0;

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(1, 1, 0);
    inst.CheckCreate();

    std::vector<VkPhysicalDeviceGroupProperties> group_props_before{};
    group_props_before.resize(expected_group_count,
                              VkPhysicalDeviceGroupProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES});
    returned_group_count = expected_group_count;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, group_props_before.data()));
    ASSERT_EQ(expected_group_count, returned_group_count);
    printf("Before:\n");
    for (uint32_t group = 0; group < expected_group_count; ++group) {
        printf("  Group %u:\n", group);
        ASSERT_EQ(group_props_before[group].physicalDeviceCount, before_expected_counts[group]);
        for (uint32_t dev = 0; dev < group_props_before[group].physicalDeviceCount; ++dev) {
            printf("    Dev %u: %p\n", dev, group_props_before[group].physicalDevices[dev]);
        }
    }

    // Remove middle device in middle group
    driver.physical_device_groups[1].physical_device_handles.erase(
        driver.physical_device_groups[1].physical_device_handles.begin() + 1);

    std::vector<VkPhysicalDeviceGroupProperties> group_props_after{};
    group_props_after.resize(expected_group_count,
                             VkPhysicalDeviceGroupProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES});
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, group_props_after.data()));
    ASSERT_EQ(expected_group_count, returned_group_count);
    printf("After:\n");
    for (uint32_t group = 0; group < expected_group_count; ++group) {
        printf("  Group %u:\n", group);
        ASSERT_EQ(group_props_after[group].physicalDeviceCount, after_expected_counts[group]);
        for (uint32_t dev = 0; dev < group_props_after[group].physicalDeviceCount; ++dev) {
            printf("    Dev %u: %p\n", dev, group_props_after[group].physicalDevices[dev]);
        }
    }

    // Make sure all devices in the new group info are found in the old group info
    for (uint32_t group1 = 0; group1 < group_props_after.size(); ++group1) {
        for (uint32_t group2 = 0; group2 < group_props_before.size(); ++group2) {
            uint32_t found_count = 0;
            bool found;
            for (uint32_t dev1 = 0; dev1 < group_props_after[group1].physicalDeviceCount; ++dev1) {
                found = false;
                for (uint32_t dev2 = 0; dev2 < group_props_before[group2].physicalDeviceCount; ++dev2) {
                    if (group_props_after[group1].physicalDevices[dev1] == group_props_before[group2].physicalDevices[dev2]) {
                        found_count++;
                        found = true;
                        break;
                    }
                }
            }
            ASSERT_EQ(found, found_count != 0 && found_count == after_expected_counts[group1]);
            if (found) {
                break;
            }
        }
    }
}

// Start with 9 devices but only some in 3 different groups, add and remove
// various devices and groups while querying in between.
TEST(EnumeratePhysicalDeviceGroups, MultipleAddRemoves) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    auto& driver = env.get_test_icd().set_min_icd_interface_version(5).set_icd_api_version(VK_API_VERSION_1_1);

    // Generate the devices
    for (size_t i = 0; i < 9; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i));
    }

    // Generate the starting groups
    driver.physical_device_groups.emplace_back(driver.physical_devices[0]);
    driver.physical_device_groups.emplace_back(driver.physical_devices[1]);
    driver.physical_device_groups.back()
        .use_physical_device(driver.physical_devices[2])
        .use_physical_device(driver.physical_devices[3]);
    driver.physical_device_groups.emplace_back(driver.physical_devices[4]);
    driver.physical_device_groups.back().use_physical_device(driver.physical_devices[5]);

    uint32_t before_expected_counts[3] = {1, 3, 2};
    uint32_t after_add_group_expected_counts[4] = {1, 3, 1, 2};
    uint32_t after_remove_dev_expected_counts[4] = {1, 2, 1, 2};
    uint32_t after_remove_group_expected_counts[3] = {2, 1, 2};
    uint32_t after_add_dev_expected_counts[3] = {2, 1, 4};
    uint32_t before_group_count = 3;
    uint32_t after_group_count = 4;
    uint32_t returned_group_count = 0;

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(1, 1, 0);
    inst.CheckCreate();

    // Should be: 3 Groups { { 0 }, { 1, 2, 3 }, { 4, 5 } }
    std::vector<VkPhysicalDeviceGroupProperties> group_props_before{};
    group_props_before.resize(before_group_count,
                              VkPhysicalDeviceGroupProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES});
    returned_group_count = before_group_count;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, group_props_before.data()));
    ASSERT_EQ(before_group_count, returned_group_count);
    for (uint32_t group = 0; group < before_group_count; ++group) {
        ASSERT_EQ(group_props_before[group].physicalDeviceCount, before_expected_counts[group]);
    }

    // Insert new group after first two
    driver.physical_device_groups.insert(driver.physical_device_groups.begin() + 2, driver.physical_devices[6]);

    // Should be: 4 Groups { { 0 }, { 1, 2, 3 }, { 6 }, { 4, 5 } }
    std::vector<VkPhysicalDeviceGroupProperties> group_props_after_add_group{};
    group_props_after_add_group.resize(after_group_count,
                                       VkPhysicalDeviceGroupProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES});
    returned_group_count = after_group_count;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, group_props_after_add_group.data()));
    ASSERT_EQ(after_group_count, returned_group_count);
    for (uint32_t group = 0; group < before_group_count; ++group) {
        ASSERT_EQ(group_props_after_add_group[group].physicalDeviceCount, after_add_group_expected_counts[group]);
    }

    // Remove first device in 2nd group
    driver.physical_device_groups[1].physical_device_handles.erase(
        driver.physical_device_groups[1].physical_device_handles.begin());

    // Should be: 4 Groups { { 0 }, { 2, 3 }, { 6 }, { 4, 5 } }
    std::vector<VkPhysicalDeviceGroupProperties> group_props_after_remove_device{};
    group_props_after_remove_device.resize(after_group_count,
                                           VkPhysicalDeviceGroupProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES});
    returned_group_count = after_group_count;
    ASSERT_EQ(VK_SUCCESS,
              inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, group_props_after_remove_device.data()));
    ASSERT_EQ(after_group_count, returned_group_count);
    for (uint32_t group = 0; group < before_group_count; ++group) {
        ASSERT_EQ(group_props_after_remove_device[group].physicalDeviceCount, after_remove_dev_expected_counts[group]);
    }

    // Remove first group
    driver.physical_device_groups.erase(driver.physical_device_groups.begin());

    // Should be: 3 Groups { { 2, 3 }, { 6 }, { 4, 5 } }
    std::vector<VkPhysicalDeviceGroupProperties> group_props_after_remove_group{};
    group_props_after_remove_group.resize(before_group_count,
                                          VkPhysicalDeviceGroupProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES});
    returned_group_count = before_group_count;
    ASSERT_EQ(VK_SUCCESS,
              inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, group_props_after_remove_group.data()));
    ASSERT_EQ(before_group_count, returned_group_count);
    for (uint32_t group = 0; group < before_group_count; ++group) {
        ASSERT_EQ(group_props_after_remove_group[group].physicalDeviceCount, after_remove_group_expected_counts[group]);
    }

    // Add two devices to last group
    driver.physical_device_groups.back()
        .use_physical_device(driver.physical_devices[7])
        .use_physical_device(driver.physical_devices[8]);

    // Should be: 3 Groups { { 2, 3 }, { 6 }, { 4, 5, 7, 8 } }
    std::vector<VkPhysicalDeviceGroupProperties> group_props_after_add_device{};
    group_props_after_add_device.resize(before_group_count,
                                        VkPhysicalDeviceGroupProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES});
    returned_group_count = before_group_count;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, group_props_after_add_device.data()));
    ASSERT_EQ(before_group_count, returned_group_count);
    for (uint32_t group = 0; group < before_group_count; ++group) {
        ASSERT_EQ(group_props_after_add_device[group].physicalDeviceCount, after_add_dev_expected_counts[group]);
    }
}

// Fill in random but valid data into the device properties struct for the current physical device
static void FillInRandomDeviceProps(VkPhysicalDeviceProperties& props, VkPhysicalDeviceType dev_type, uint32_t api_vers,
                                    uint32_t vendor, uint32_t device) {
    props.apiVersion = api_vers;
    props.vendorID = vendor;
    props.deviceID = device;
    props.deviceType = dev_type;
    for (uint8_t idx = 0; idx < VK_UUID_SIZE; ++idx) {
        props.pipelineCacheUUID[idx] = static_cast<uint8_t>(rand() % 255);
    }
}

// Pass in a PNext that the fake ICD will fill in some data for.
TEST(EnumeratePhysicalDeviceGroups, FakePNext) {
    FrameworkEnvironment env{};

    // ICD 0: Vulkan 1.1
    //   PhysDev 0: pd0, Discrete, Vulkan 1.1, Bus 7
    //   PhysDev 1: pd1, Integrated, Vulkan 1.1, Bus 3
    //   PhysDev 2: pd2, Discrete, Vulkan 1.1, Bus 6
    //   Group 0: PhysDev 0, PhysDev 2
    //   Group 1: PhysDev 1
    // ICD 1: Vulkan 1.1
    //   PhysDev 4: pd4, Discrete, Vulkan 1.1, Bus 1
    //   PhysDev 5: pd5, Discrete, Vulkan 1.1, Bus 4
    //   PhysDev 6: pd6, Discrete, Vulkan 1.1, Bus 2
    //   Group 0: PhysDev 5, PhysDev 6
    //   Group 1: PhysDev 4
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    auto& cur_icd_0 = env.get_test_icd(0);
    cur_icd_0.set_icd_api_version(VK_API_VERSION_1_1);
    cur_icd_0.physical_devices.push_back({"pd0", 7});
    cur_icd_0.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_0.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_API_VERSION_1_1,
                            888, 0xAAA001);
    cur_icd_0.physical_devices.push_back({"pd1", 3});
    cur_icd_0.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_0.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
                            VK_API_VERSION_1_1, 888, 0xAAA002);
    cur_icd_0.physical_devices.push_back({"pd2", 6});
    cur_icd_0.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_0.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_API_VERSION_1_1,
                            888, 0xAAA003);
    cur_icd_0.physical_device_groups.push_back({});
    cur_icd_0.physical_device_groups.back()
        .use_physical_device(cur_icd_0.physical_devices[0])
        .use_physical_device(cur_icd_0.physical_devices[2]);
    cur_icd_0.physical_device_groups.push_back({cur_icd_0.physical_devices[1]});

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    auto& cur_icd_1 = env.get_test_icd(1);
    cur_icd_1.set_icd_api_version(VK_API_VERSION_1_1);
    cur_icd_1.physical_devices.push_back({"pd4", 1});
    cur_icd_1.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_1.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_API_VERSION_1_1,
                            75, 0xCCCC001);
    cur_icd_1.physical_devices.push_back({"pd5", 4});
    cur_icd_1.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_1.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_API_VERSION_1_1,
                            75, 0xCCCC002);
    cur_icd_1.physical_devices.push_back({"pd6", 2});
    cur_icd_1.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_1.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_API_VERSION_1_1,
                            75, 0xCCCC003);
    cur_icd_1.physical_device_groups.push_back({});
    cur_icd_1.physical_device_groups.back()
        .use_physical_device(cur_icd_1.physical_devices[1])
        .use_physical_device(cur_icd_1.physical_devices[2]);
    cur_icd_1.physical_device_groups.push_back({cur_icd_1.physical_devices[0]});

    InstWrapper inst(env.vulkan_functions);
    inst.create_info.set_api_version(VK_API_VERSION_1_1);
    inst.CheckCreate();

    auto GetPhysDevProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(
        inst.functions->vkGetInstanceProcAddr(inst, "vkGetPhysicalDeviceProperties2"));
    ASSERT_NE(GetPhysDevProps2, nullptr);

    // NOTE: This is a fake struct to make sure the pNext chain is properly passed down to the ICD
    //       vkEnumeratePhysicalDeviceGroups.
    //       The two versions must match:
    //           "FakePNext" test in loader_regresion_tests.cpp
    //           "test_vkEnumeratePhysicalDeviceGroups" in test_icd.cpp
    struct FakePnextSharedWithICD {
        VkStructureType sType;
        void* pNext;
        uint32_t value;
    };

    const uint32_t max_phys_dev_groups = 4;
    uint32_t group_count = max_phys_dev_groups;
    std::array<FakePnextSharedWithICD, max_phys_dev_groups> fake_structs;
    std::array<VkPhysicalDeviceGroupProperties, max_phys_dev_groups> physical_device_groups{};
    for (uint32_t group = 0; group < max_phys_dev_groups; ++group) {
        physical_device_groups[group].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
        fake_structs[group].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_PROPERTIES_EXT;
        physical_device_groups[group].pNext = &fake_structs[group];
    }
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &group_count, physical_device_groups.data()));
    ASSERT_EQ(group_count, max_phys_dev_groups);

    // Value should get written to 0xDECAFBADD by the fake ICD
    for (uint32_t group = 0; group < max_phys_dev_groups; ++group) {
        ASSERT_EQ(fake_structs[group].value, 0xDECAFBAD);
    }
}

TEST(ExtensionManual, ToolingProperties) {
    VkPhysicalDeviceToolPropertiesEXT icd_tool_props{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TOOL_PROPERTIES_EXT,
                                                     nullptr,
                                                     "FakeICDTool",
                                                     "version_0_0_0_1.b",
                                                     VK_TOOL_PURPOSE_VALIDATION_BIT_EXT,
                                                     "This tool does not exist",
                                                     "No-Layer"};
    {  // No support in driver
        FrameworkEnvironment env{};
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
        env.get_test_icd().physical_devices.push_back({});

        InstWrapper inst{env.vulkan_functions};
        inst.CheckCreate();

        auto phys_dev = inst.GetPhysDev();

        auto getToolProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceToolPropertiesEXT>(
            inst.functions->vkGetInstanceProcAddr(inst, "vkGetPhysicalDeviceToolPropertiesEXT"));
        handle_assert_has_value(getToolProperties);

        uint32_t tool_count = 0;
        ASSERT_EQ(VK_SUCCESS, getToolProperties(phys_dev, &tool_count, nullptr));
        ASSERT_EQ(tool_count, 0U);
    }
    {  // extension is supported in driver
        FrameworkEnvironment env{};
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
        env.get_test_icd().physical_devices.push_back({});
        env.get_test_icd().supports_tooling_info_ext = true;
        env.get_test_icd().tooling_properties.push_back(icd_tool_props);
        env.get_test_icd().physical_devices.back().extensions.push_back({VK_EXT_TOOLING_INFO_EXTENSION_NAME, 0});

        InstWrapper inst{env.vulkan_functions};
        inst.CheckCreate();

        auto phys_dev = inst.GetPhysDev();

        auto getToolProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceToolPropertiesEXT>(
            inst.functions->vkGetInstanceProcAddr(inst, "vkGetPhysicalDeviceToolPropertiesEXT"));
        handle_assert_has_value(getToolProperties);
        uint32_t tool_count = 0;
        ASSERT_EQ(VK_SUCCESS, getToolProperties(phys_dev, &tool_count, nullptr));
        ASSERT_EQ(tool_count, 1U);
        VkPhysicalDeviceToolPropertiesEXT props{};
        ASSERT_EQ(VK_SUCCESS, getToolProperties(phys_dev, &tool_count, &props));
        ASSERT_EQ(tool_count, 1U);
        string_eq(props.name, icd_tool_props.name);
    }
    {  // core
        FrameworkEnvironment env{};
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
        env.get_test_icd().physical_devices.push_back({});
        env.get_test_icd().physical_devices.back().properties.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);
        env.get_test_icd().supports_tooling_info_core = true;
        env.get_test_icd().tooling_properties.push_back(icd_tool_props);

        InstWrapper inst{env.vulkan_functions};
        inst.CheckCreate();

        auto phys_dev = inst.GetPhysDev();

        auto getToolProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceToolProperties>(
            inst.functions->vkGetInstanceProcAddr(inst, "vkGetPhysicalDeviceToolProperties"));
        handle_assert_has_value(getToolProperties);
        uint32_t tool_count = 0;
        ASSERT_EQ(VK_SUCCESS, getToolProperties(phys_dev, &tool_count, nullptr));
        ASSERT_EQ(tool_count, 1U);
        VkPhysicalDeviceToolProperties props{};
        ASSERT_EQ(VK_SUCCESS, getToolProperties(phys_dev, &tool_count, &props));
        ASSERT_EQ(tool_count, 1U);
        string_eq(props.name, icd_tool_props.name);
    }
}
TEST(CreateInstance, InstanceNullLayerPtr) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    VkInstance inst = VK_NULL_HANDLE;
    VkInstanceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.enabledLayerCount = 1;

    ASSERT_EQ(env.vulkan_functions.vkCreateInstance(&info, VK_NULL_HANDLE, &inst), VK_ERROR_LAYER_NOT_PRESENT);
}
TEST(CreateInstance, InstanceNullExtensionPtr) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    VkInstance inst = VK_NULL_HANDLE;
    VkInstanceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.enabledExtensionCount = 1;

    ASSERT_EQ(env.vulkan_functions.vkCreateInstance(&info, VK_NULL_HANDLE, &inst), VK_ERROR_EXTENSION_NOT_PRESENT);
}

#if defined(__linux__) || defined(__FreeBSD__)
// NOTE: Sort order only affects Linux
TEST(SortedPhysicalDevices, DevicesSortEnabled10NoAppExt) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    env.get_test_icd(0).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({"pd0", 7});
    FillInRandomDeviceProps(env.get_test_icd(0).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
                            VK_API_VERSION_1_1, 888, 0xAAA001);
    env.get_test_icd(0).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    env.get_test_icd(0).physical_devices.push_back({"pd1", 3});
    FillInRandomDeviceProps(env.get_test_icd(0).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
                            VK_API_VERSION_1_1, 888, 0xAAA002);
    env.get_test_icd(0).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_0));
    env.get_test_icd(1).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(1).physical_devices.push_back({"pd2", 0});
    FillInRandomDeviceProps(env.get_test_icd(1).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_CPU, VK_API_VERSION_1_0,
                            1, 0xBBBB001);
    env.get_test_icd(1).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    env.get_test_icd(2).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(2).physical_devices.push_back({"pd3", 1});
    FillInRandomDeviceProps(env.get_test_icd(2).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
                            VK_API_VERSION_1_1, 75, 0xCCCC001);
    env.get_test_icd(2).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    env.get_test_icd(2).physical_devices.push_back({"pd4", 4});
    FillInRandomDeviceProps(env.get_test_icd(2).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
                            VK_API_VERSION_1_0, 75, 0xCCCC002);
    env.get_test_icd(2).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    env.get_test_icd(3).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(3).physical_devices.push_back({"pd5", 0});
    FillInRandomDeviceProps(env.get_test_icd(3).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU,
                            VK_API_VERSION_1_1, 6940, 0xDDDD001);
    env.get_test_icd(3).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});

    InstWrapper instance(env.vulkan_functions);
    instance.CheckCreate();

    const uint32_t max_phys_devs = 6;
    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkPhysicalDeviceProperties props{};
        instance->vkGetPhysicalDeviceProperties(physical_devices[dev], &props);

        switch (dev) {
            case 0:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
                ASSERT_EQ(true, !strcmp("pd3", props.deviceName));
                ASSERT_EQ(props.vendorID, 75);
                ASSERT_EQ(props.deviceID, 0xCCCC001);
                break;
            case 1:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
                ASSERT_EQ(true, !strcmp("pd4", props.deviceName));
                ASSERT_EQ(props.vendorID, 75);
                ASSERT_EQ(props.deviceID, 0xCCCC002);
                break;
            case 2:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
                ASSERT_EQ(true, !strcmp("pd0", props.deviceName));
                ASSERT_EQ(props.vendorID, 888);
                ASSERT_EQ(props.deviceID, 0xAAA001);
                break;
            case 3:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
                ASSERT_EQ(true, !strcmp("pd1", props.deviceName));
                ASSERT_EQ(props.vendorID, 888);
                ASSERT_EQ(props.deviceID, 0xAAA002);
                break;
            case 4:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU);
                ASSERT_EQ(true, !strcmp("pd5", props.deviceName));
                ASSERT_EQ(props.vendorID, 6940U);
                ASSERT_EQ(props.deviceID, 0xDDDD001);
                break;
            case 5:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_CPU);
                ASSERT_EQ(true, !strcmp("pd2", props.deviceName));
                ASSERT_EQ(props.vendorID, 1U);
                ASSERT_EQ(props.deviceID, 0xBBBB001);
                break;
            default:
                ASSERT_EQ(false, true);
        }
    }

    // Make sure if we call enumerate again, the information is the same
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices_again;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices_again.data()));
    ASSERT_EQ(device_count, max_phys_devs);
    for (uint32_t dev = 0; dev < device_count; ++dev) {
        ASSERT_EQ(physical_devices[dev], physical_devices_again[dev]);
    }
}

TEST(SortedPhysicalDevices, DevicesSortEnabled10AppExt) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    env.get_test_icd(0).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({"pd0", 7});
    FillInRandomDeviceProps(env.get_test_icd(0).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
                            VK_API_VERSION_1_1, 888, 0xAAA001);
    env.get_test_icd(0).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    env.get_test_icd(0).physical_devices.push_back({"pd1", 3});
    FillInRandomDeviceProps(env.get_test_icd(0).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
                            VK_API_VERSION_1_1, 888, 0xAAA002);
    env.get_test_icd(0).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_0));
    env.get_test_icd(1).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(1).physical_devices.push_back({"pd2", 0});
    FillInRandomDeviceProps(env.get_test_icd(1).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_CPU, VK_API_VERSION_1_0,
                            1, 0xBBBB001);
    env.get_test_icd(1).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    env.get_test_icd(2).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(2).physical_devices.push_back({"pd3", 1});
    FillInRandomDeviceProps(env.get_test_icd(2).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
                            VK_API_VERSION_1_1, 75, 0xCCCC001);
    env.get_test_icd(2).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    env.get_test_icd(2).physical_devices.push_back({"pd4", 4});
    FillInRandomDeviceProps(env.get_test_icd(2).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
                            VK_API_VERSION_1_0, 75, 0xCCCC002);
    env.get_test_icd(2).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    env.get_test_icd(3).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(3).physical_devices.push_back({"pd5", 0});
    FillInRandomDeviceProps(env.get_test_icd(3).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU,
                            VK_API_VERSION_1_1, 6940, 0xDDDD001);
    env.get_test_icd(3).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instance.CheckCreate();

    auto GetPhysDevProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2KHR>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties2KHR"));
    ASSERT_NE(GetPhysDevProps2, nullptr);

    const uint32_t max_phys_devs = 6;
    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkPhysicalDeviceProperties props{};
        instance->vkGetPhysicalDeviceProperties(physical_devices[dev], &props);
        VkPhysicalDeviceProperties2KHR props2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
        VkPhysicalDevicePCIBusInfoPropertiesEXT pci_bus_info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT};
        props2.pNext = &pci_bus_info;
        GetPhysDevProps2(physical_devices[dev], &props2);

        switch (dev) {
            case 0:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
                ASSERT_EQ(true, !strcmp("pd3", props.deviceName));
                ASSERT_EQ(props.vendorID, 75);
                ASSERT_EQ(props.deviceID, 0xCCCC001);
                ASSERT_EQ(pci_bus_info.pciBus, 1U);
                break;
            case 1:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
                ASSERT_EQ(true, !strcmp("pd4", props.deviceName));
                ASSERT_EQ(props.vendorID, 75);
                ASSERT_EQ(props.deviceID, 0xCCCC002);
                ASSERT_EQ(pci_bus_info.pciBus, 4U);
                break;
            case 2:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
                ASSERT_EQ(true, !strcmp("pd0", props.deviceName));
                ASSERT_EQ(props.vendorID, 888);
                ASSERT_EQ(props.deviceID, 0xAAA001);
                ASSERT_EQ(pci_bus_info.pciBus, 7);
                break;
            case 3:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
                ASSERT_EQ(true, !strcmp("pd1", props.deviceName));
                ASSERT_EQ(props.vendorID, 888);
                ASSERT_EQ(props.deviceID, 0xAAA002);
                ASSERT_EQ(pci_bus_info.pciBus, 3U);
                break;
            case 4:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU);
                ASSERT_EQ(true, !strcmp("pd5", props.deviceName));
                ASSERT_EQ(props.vendorID, 6940U);
                ASSERT_EQ(props.deviceID, 0xDDDD001);
                ASSERT_EQ(pci_bus_info.pciBus, 0U);
                break;
            case 5:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_CPU);
                ASSERT_EQ(true, !strcmp("pd2", props.deviceName));
                ASSERT_EQ(props.vendorID, 1U);
                ASSERT_EQ(props.deviceID, 0xBBBB001);
                ASSERT_EQ(pci_bus_info.pciBus, 0U);
                break;
            default:
                ASSERT_EQ(false, true);
        }
    }

    // Make sure if we call enumerate again, the information is the same
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices_again;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices_again.data()));
    ASSERT_EQ(device_count, max_phys_devs);
    for (uint32_t dev = 0; dev < device_count; ++dev) {
        ASSERT_EQ(physical_devices[dev], physical_devices_again[dev]);
    }
}

TEST(SortedPhysicalDevices, DevicesSortEnabled11) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    env.get_test_icd(0).set_icd_api_version(VK_API_VERSION_1_1);
    env.get_test_icd(0).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({"pd0", 7});
    FillInRandomDeviceProps(env.get_test_icd(0).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
                            VK_API_VERSION_1_0, 888, 0xAAA001);
    env.get_test_icd(0).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    env.get_test_icd(0).physical_devices.push_back({"pd1", 3});
    FillInRandomDeviceProps(env.get_test_icd(0).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
                            VK_API_VERSION_1_0, 888, 0xAAA002);
    env.get_test_icd(0).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    env.get_test_icd(1).set_icd_api_version(VK_API_VERSION_1_1);
    env.get_test_icd(1).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(1).physical_devices.push_back({"pd2", 0});
    FillInRandomDeviceProps(env.get_test_icd(1).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_CPU, VK_API_VERSION_1_0,
                            1, 0xBBBB001);
    env.get_test_icd(1).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    env.get_test_icd(2).set_icd_api_version(VK_API_VERSION_1_1);
    env.get_test_icd(2).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(2).physical_devices.push_back({"pd3", 1});
    FillInRandomDeviceProps(env.get_test_icd(2).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
                            VK_API_VERSION_1_1, 75, 0xCCCC001);
    env.get_test_icd(2).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    env.get_test_icd(2).physical_devices.push_back({"pd4", 4});
    FillInRandomDeviceProps(env.get_test_icd(2).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
                            VK_API_VERSION_1_1, 75, 0xCCCC002);
    env.get_test_icd(2).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    env.get_test_icd(3).set_icd_api_version(VK_API_VERSION_1_1);
    env.get_test_icd(3).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(3).physical_devices.push_back({"pd5", 0});
    FillInRandomDeviceProps(env.get_test_icd(3).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU,
                            VK_API_VERSION_1_1, 6940, 0xDDDD001);
    env.get_test_icd(3).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(VK_API_VERSION_1_1);
    instance.CheckCreate();

    auto GetPhysDevProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(
        instance.functions->vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties2"));
    ASSERT_NE(GetPhysDevProps2, nullptr);

    const uint32_t max_phys_devs = 6;
    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkPhysicalDeviceProperties props{};
        instance->vkGetPhysicalDeviceProperties(physical_devices[dev], &props);
        VkPhysicalDeviceProperties2KHR props2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
        VkPhysicalDevicePCIBusInfoPropertiesEXT pci_bus_info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT};
        props2.pNext = &pci_bus_info;
        GetPhysDevProps2(physical_devices[dev], &props2);

        switch (dev) {
            case 0:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
                ASSERT_EQ(true, !strcmp("pd3", props.deviceName));
                ASSERT_EQ(props.vendorID, 75);
                ASSERT_EQ(props.deviceID, 0xCCCC001);
                ASSERT_EQ(pci_bus_info.pciBus, 1U);
                break;
            case 1:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
                ASSERT_EQ(true, !strcmp("pd4", props.deviceName));
                ASSERT_EQ(props.vendorID, 75);
                ASSERT_EQ(props.deviceID, 0xCCCC002);
                ASSERT_EQ(pci_bus_info.pciBus, 4U);
                break;
            case 2:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
                ASSERT_EQ(true, !strcmp("pd0", props.deviceName));
                ASSERT_EQ(props.vendorID, 888);
                ASSERT_EQ(props.deviceID, 0xAAA001);
                ASSERT_EQ(pci_bus_info.pciBus, 7);
                break;
            case 3:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
                ASSERT_EQ(true, !strcmp("pd1", props.deviceName));
                ASSERT_EQ(props.vendorID, 888);
                ASSERT_EQ(props.deviceID, 0xAAA002);
                ASSERT_EQ(pci_bus_info.pciBus, 3U);
                break;
            case 4:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU);
                ASSERT_EQ(true, !strcmp("pd5", props.deviceName));
                ASSERT_EQ(props.vendorID, 6940U);
                ASSERT_EQ(props.deviceID, 0xDDDD001);
                ASSERT_EQ(pci_bus_info.pciBus, 0U);
                break;
            case 5:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_CPU);
                ASSERT_EQ(true, !strcmp("pd2", props.deviceName));
                ASSERT_EQ(props.vendorID, 1U);
                ASSERT_EQ(props.deviceID, 0xBBBB001);
                ASSERT_EQ(pci_bus_info.pciBus, 0U);
                break;
            default:
                ASSERT_EQ(false, true);
        }
    }

    // Make sure if we call enumerate again, the information is the same
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices_again;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices_again.data()));
    ASSERT_EQ(device_count, max_phys_devs);
    for (uint32_t dev = 0; dev < device_count; ++dev) {
        ASSERT_EQ(physical_devices[dev], physical_devices_again[dev]);
    }
}

TEST(SortedPhysicalDevices, DevicesSortedDisabled) {
    FrameworkEnvironment env{};

    set_env_var("VK_LOADER_DISABLE_SELECT", "1");

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_0));
    env.get_test_icd(0).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(0).physical_devices.push_back({"pd0", 7});
    FillInRandomDeviceProps(env.get_test_icd(0).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
                            VK_API_VERSION_1_0, 888, 0xAAA001);
    env.get_test_icd(0).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    env.get_test_icd(0).physical_devices.push_back({"pd1", 3});
    FillInRandomDeviceProps(env.get_test_icd(0).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
                            VK_API_VERSION_1_0, 888, 0xAAA002);
    env.get_test_icd(0).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_0));
    env.get_test_icd(1).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(1).physical_devices.push_back({"pd2", 0});
    FillInRandomDeviceProps(env.get_test_icd(1).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_CPU, VK_API_VERSION_1_0,
                            1, 0xBBBB001);
    env.get_test_icd(1).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_0));
    env.get_test_icd(2).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(2).physical_devices.push_back({"pd3", 1});
    FillInRandomDeviceProps(env.get_test_icd(2).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
                            VK_API_VERSION_1_0, 75, 0xCCCC001);
    env.get_test_icd(2).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    env.get_test_icd(2).physical_devices.push_back({"pd4", 4});
    FillInRandomDeviceProps(env.get_test_icd(2).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
                            VK_API_VERSION_1_0, 75, 0xCCCC002);
    env.get_test_icd(2).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_0));
    env.get_test_icd(3).add_instance_extension({VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME});
    env.get_test_icd(3).physical_devices.push_back({"pd5", 0});
    FillInRandomDeviceProps(env.get_test_icd(3).physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU,
                            VK_API_VERSION_1_0, 6940, 0xDDDD001);
    env.get_test_icd(3).physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    instance.CheckCreate();

    // Just make sure we have the correct number of devices
    const uint32_t max_phys_devs = 6;
    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    // Make sure the devices are not in the sorted order.  The order is really undefined, but the chances of
    // it being exactly the expected sorted is very low.
    bool sorted = true;
    for (uint32_t dev = 0; dev < device_count; ++dev) {
        VkPhysicalDeviceProperties props{};
        instance->vkGetPhysicalDeviceProperties(physical_devices[dev], &props);

        switch (dev) {
            case 0:
                if (props.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || strcmp("pd3", props.deviceName)) {
                    sorted = false;
                }
                break;
            case 1:
                if (props.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || strcmp("pd4", props.deviceName)) {
                    sorted = false;
                }
                break;
            case 2:
                if (props.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || strcmp("pd0", props.deviceName)) {
                    sorted = false;
                }
                break;
            case 3:
                if (props.deviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU || strcmp("pd1", props.deviceName)) {
                    sorted = false;
                }
                break;
            case 4:
                if (props.deviceType != VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU || strcmp("pd5", props.deviceName)) {
                    sorted = false;
                }
                break;
            case 5:
                if (props.deviceType != VK_PHYSICAL_DEVICE_TYPE_CPU || strcmp("pd2", props.deviceName)) {
                    sorted = false;
                }
                break;
            default:
                ASSERT_EQ(false, true);
        }
        if (!sorted) {
            break;
        }
    }
    ASSERT_EQ(false, sorted);

    // Make sure if we call enumerate again, the information is the same
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices_again;
    ASSERT_EQ(VK_SUCCESS, instance->vkEnumeratePhysicalDevices(instance, &device_count, physical_devices_again.data()));
    ASSERT_EQ(device_count, max_phys_devs);
    for (uint32_t dev = 0; dev < device_count; ++dev) {
        ASSERT_EQ(physical_devices[dev], physical_devices_again[dev]);
    }

    remove_env_var("VK_LOADER_DISABLE_SELECT");
}

TEST(SortedPhysicalDevices, DeviceGroupsSortedEnabled) {
    FrameworkEnvironment env{};

    // ICD 0: Vulkan 1.1
    //   PhysDev 0: pd0, Discrete, Vulkan 1.1, Bus 7
    //   PhysDev 1: pd1, Integrated, Vulkan 1.1, Bus 3
    //   PhysDev 2: pd2, Discrete, Vulkan 1.1, Bus 6
    //   Group 0: PhysDev 0, PhysDev 2
    //   Group 1: PhysDev 1
    // ICD 1: Vulkan 1.1
    //   PhysDev 3: pd3, CPU, Vulkan 1.1, Bus 0
    // ICD 2: Vulkan 1.1
    //   PhysDev 4: pd4, Discrete, Vulkan 1.1, Bus 1
    //   PhysDev 5: pd5, Discrete, Vulkan 1.1, Bus 4
    //   PhysDev 6: pd6, Discrete, Vulkan 1.1, Bus 2
    //   Group 0: PhysDev 5, PhysDev 6
    //   Group 1: PhysDev 4
    // ICD 3: Vulkan 1.1
    //   PhysDev 7: pd7, Virtual, Vulkan 1.1, Bus 0
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    auto& cur_icd_0 = env.get_test_icd(0);
    cur_icd_0.set_icd_api_version(VK_API_VERSION_1_1);
    cur_icd_0.physical_devices.push_back({"pd0", 7});
    cur_icd_0.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_0.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_API_VERSION_1_1,
                            888, 0xAAA001);
    cur_icd_0.physical_devices.push_back({"pd1", 3});
    cur_icd_0.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_0.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
                            VK_API_VERSION_1_1, 888, 0xAAA002);
    cur_icd_0.physical_devices.push_back({"pd2", 6});
    cur_icd_0.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_0.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_API_VERSION_1_1,
                            888, 0xAAA003);
    cur_icd_0.physical_device_groups.push_back({});
    cur_icd_0.physical_device_groups.back()
        .use_physical_device(cur_icd_0.physical_devices[0])
        .use_physical_device(cur_icd_0.physical_devices[2]);
    cur_icd_0.physical_device_groups.push_back({cur_icd_0.physical_devices[1]});

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    auto& cur_icd_1 = env.get_test_icd(1);
    cur_icd_1.set_icd_api_version(VK_API_VERSION_1_1);
    cur_icd_1.physical_devices.push_back({"pd3", 0});
    cur_icd_1.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_1.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_CPU, VK_API_VERSION_1_1, 1,
                            0xBBBB001);

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    auto& cur_icd_2 = env.get_test_icd(2);
    cur_icd_2.set_icd_api_version(VK_API_VERSION_1_1);
    cur_icd_2.physical_devices.push_back({"pd4", 1});
    cur_icd_2.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_2.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_API_VERSION_1_1,
                            75, 0xCCCC001);
    cur_icd_2.physical_devices.push_back({"pd5", 4});
    cur_icd_2.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_2.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_API_VERSION_1_1,
                            75, 0xCCCC002);
    cur_icd_2.physical_devices.push_back({"pd6", 2});
    cur_icd_2.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_2.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_API_VERSION_1_1,
                            75, 0xCCCC003);
    cur_icd_2.physical_device_groups.push_back({});
    cur_icd_2.physical_device_groups.back()
        .use_physical_device(cur_icd_2.physical_devices[1])
        .use_physical_device(cur_icd_2.physical_devices[2]);
    cur_icd_2.physical_device_groups.push_back({cur_icd_2.physical_devices[0]});

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    auto& cur_icd_3 = env.get_test_icd(3);
    cur_icd_3.set_icd_api_version(VK_API_VERSION_1_1);
    cur_icd_3.physical_devices.push_back({"pd7", 0});
    cur_icd_3.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_3.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU, VK_API_VERSION_1_1,
                            6940, 0xDDDD001);

    InstWrapper inst(env.vulkan_functions);
    inst.create_info.set_api_version(VK_API_VERSION_1_1);
    inst.CheckCreate();

    auto GetPhysDevProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(
        inst.functions->vkGetInstanceProcAddr(inst, "vkGetPhysicalDeviceProperties2"));
    ASSERT_NE(GetPhysDevProps2, nullptr);

    const uint32_t max_phys_devs = 8;
    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    const uint32_t max_phys_dev_groups = 6;
    uint32_t group_count = max_phys_dev_groups;
    std::array<VkPhysicalDeviceGroupProperties, max_phys_dev_groups> physical_device_groups{};
    for (auto& group : physical_device_groups) group.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &group_count, physical_device_groups.data()));
    ASSERT_EQ(group_count, max_phys_dev_groups);

    uint32_t cur_dev = 0;
    for (uint32_t group = 0; group < max_phys_dev_groups; ++group) {
        for (uint32_t dev = 0; dev < physical_device_groups[group].physicalDeviceCount; ++dev) {
            VkPhysicalDeviceProperties props{};
            inst->vkGetPhysicalDeviceProperties(physical_device_groups[group].physicalDevices[dev], &props);
            VkPhysicalDeviceProperties2 props2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
            VkPhysicalDevicePCIBusInfoPropertiesEXT pci_bus_info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT};
            props2.pNext = &pci_bus_info;
            GetPhysDevProps2(physical_device_groups[group].physicalDevices[dev], &props2);
            switch (cur_dev++) {
                case 0:
                    ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
                    ASSERT_EQ(true, !strcmp("pd4", props.deviceName));
                    ASSERT_EQ(props.vendorID, 75);
                    ASSERT_EQ(props.deviceID, 0xCCCC001);
                    ASSERT_EQ(pci_bus_info.pciBus, 1U);
                    break;
                case 1:
                    ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
                    ASSERT_EQ(true, !strcmp("pd6", props.deviceName));
                    ASSERT_EQ(props.vendorID, 75);
                    ASSERT_EQ(props.deviceID, 0xCCCC003);
                    ASSERT_EQ(pci_bus_info.pciBus, 2U);
                    break;
                case 2:
                    ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
                    ASSERT_EQ(true, !strcmp("pd5", props.deviceName));
                    ASSERT_EQ(props.vendorID, 75);
                    ASSERT_EQ(props.deviceID, 0xCCCC002);
                    ASSERT_EQ(pci_bus_info.pciBus, 4U);
                    break;
                case 3:
                    ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
                    ASSERT_EQ(true, !strcmp("pd2", props.deviceName));
                    ASSERT_EQ(props.vendorID, 888);
                    ASSERT_EQ(props.deviceID, 0xAAA003);
                    ASSERT_EQ(pci_bus_info.pciBus, 6);
                    break;
                case 4:
                    ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
                    ASSERT_EQ(true, !strcmp("pd0", props.deviceName));
                    ASSERT_EQ(props.vendorID, 888);
                    ASSERT_EQ(props.deviceID, 0xAAA001);
                    ASSERT_EQ(pci_bus_info.pciBus, 7);
                    break;
                case 5:
                    ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
                    ASSERT_EQ(true, !strcmp("pd1", props.deviceName));
                    ASSERT_EQ(props.vendorID, 888);
                    ASSERT_EQ(props.deviceID, 0xAAA002);
                    ASSERT_EQ(pci_bus_info.pciBus, 3U);
                    break;
                case 6:
                    ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU);
                    ASSERT_EQ(true, !strcmp("pd7", props.deviceName));
                    ASSERT_EQ(props.vendorID, 6940U);
                    ASSERT_EQ(props.deviceID, 0xDDDD001);
                    ASSERT_EQ(pci_bus_info.pciBus, 0U);
                    break;
                case 7:
                    ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_CPU);
                    ASSERT_EQ(true, !strcmp("pd3", props.deviceName));
                    ASSERT_EQ(props.vendorID, 1U);
                    ASSERT_EQ(props.deviceID, 0xBBBB001);
                    ASSERT_EQ(pci_bus_info.pciBus, 0U);
                    break;
                default:
                    ASSERT_EQ(false, true);
            }
        }
    }

    // Make sure if we call enumerate again, the information is the same
    std::array<VkPhysicalDeviceGroupProperties, max_phys_dev_groups> physical_device_groups_again{};
    for (auto& group : physical_device_groups_again) group.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;

    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &group_count, physical_device_groups_again.data()));
    ASSERT_EQ(group_count, max_phys_dev_groups);
    for (uint32_t group = 0; group < max_phys_dev_groups; ++group) {
        ASSERT_EQ(physical_device_groups[group].physicalDeviceCount, physical_device_groups_again[group].physicalDeviceCount);
        for (uint32_t dev = 0; dev < physical_device_groups[group].physicalDeviceCount; ++dev) {
            ASSERT_EQ(physical_device_groups[group].physicalDevices[dev], physical_device_groups_again[group].physicalDevices[dev]);
        }
    }
}

TEST(SortedPhysicalDevices, DeviceGroupsSortedDisabled) {
    FrameworkEnvironment env{};

    set_env_var("VK_LOADER_DISABLE_SELECT", "1");

    // ICD 0: Vulkan 1.1
    //   PhysDev 0: pd0, Discrete, Vulkan 1.1, Bus 7
    //   PhysDev 1: pd1, Integrated, Vulkan 1.1, Bus 3
    //   PhysDev 2: pd2, Discrete, Vulkan 1.1, Bus 6
    //   Group 0: PhysDev 0, PhysDev 2
    //   Group 1: PhysDev 1
    // ICD 1: Vulkan 1.1
    //   PhysDev 3: pd3, CPU, Vulkan 1.1, Bus 0
    // ICD 2: Vulkan 1.1
    //   PhysDev 4: pd4, Discrete, Vulkan 1.1, Bus 1
    //   PhysDev 5: pd5, Discrete, Vulkan 1.1, Bus 4
    //   PhysDev 6: pd6, Discrete, Vulkan 1.1, Bus 2
    //   Group 0: PhysDev 5, PhysDev 6
    //   Group 1: PhysDev 4
    // ICD 3: Vulkan 1.1
    //   PhysDev 7: pd7, Virtual, Vulkan 1.1, Bus 0
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    auto& cur_icd_0 = env.get_test_icd(0);
    cur_icd_0.set_icd_api_version(VK_API_VERSION_1_1);
    cur_icd_0.physical_devices.push_back({"pd0", 7});
    cur_icd_0.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_0.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_API_VERSION_1_1,
                            888, 0xAAA001);
    cur_icd_0.physical_devices.push_back({"pd1", 3});
    cur_icd_0.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_0.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
                            VK_API_VERSION_1_1, 888, 0xAAA002);
    cur_icd_0.physical_devices.push_back({"pd2", 6});
    cur_icd_0.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_0.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_API_VERSION_1_1,
                            888, 0xAAA003);
    cur_icd_0.physical_device_groups.push_back({});
    cur_icd_0.physical_device_groups.back()
        .use_physical_device(cur_icd_0.physical_devices[0])
        .use_physical_device(cur_icd_0.physical_devices[2]);
    cur_icd_0.physical_device_groups.push_back({cur_icd_0.physical_devices[1]});

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    auto& cur_icd_1 = env.get_test_icd(1);
    cur_icd_1.set_icd_api_version(VK_API_VERSION_1_1);
    cur_icd_1.physical_devices.push_back({"pd3", 0});
    cur_icd_1.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_1.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_CPU, VK_API_VERSION_1_1, 1,
                            0xBBBB001);

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    auto& cur_icd_2 = env.get_test_icd(2);
    cur_icd_2.set_icd_api_version(VK_API_VERSION_1_1);
    cur_icd_2.physical_devices.push_back({"pd4", 1});
    cur_icd_2.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_2.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_API_VERSION_1_1,
                            75, 0xCCCC001);
    cur_icd_2.physical_devices.push_back({"pd5", 4});
    cur_icd_2.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_2.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_API_VERSION_1_1,
                            75, 0xCCCC002);
    cur_icd_2.physical_devices.push_back({"pd6", 2});
    cur_icd_2.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_2.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_API_VERSION_1_1,
                            75, 0xCCCC003);
    cur_icd_2.physical_device_groups.push_back({});
    cur_icd_2.physical_device_groups.back()
        .use_physical_device(cur_icd_2.physical_devices[1])
        .use_physical_device(cur_icd_2.physical_devices[2]);
    cur_icd_2.physical_device_groups.push_back({cur_icd_2.physical_devices[0]});

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    auto& cur_icd_3 = env.get_test_icd(3);
    cur_icd_3.set_icd_api_version(VK_API_VERSION_1_1);
    cur_icd_3.physical_devices.push_back({"pd7", 0});
    cur_icd_3.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    FillInRandomDeviceProps(cur_icd_3.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU, VK_API_VERSION_1_1,
                            6940, 0xDDDD001);

    InstWrapper inst(env.vulkan_functions);
    inst.create_info.set_api_version(VK_API_VERSION_1_1);
    inst.CheckCreate();

    auto GetPhysDevProps2 = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(
        inst.functions->vkGetInstanceProcAddr(inst, "vkGetPhysicalDeviceProperties2"));
    ASSERT_NE(GetPhysDevProps2, nullptr);

    const uint32_t max_phys_devs = 8;
    uint32_t device_count = max_phys_devs;
    std::array<VkPhysicalDevice, max_phys_devs> physical_devices;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &device_count, physical_devices.data()));
    ASSERT_EQ(device_count, max_phys_devs);

    const uint32_t max_phys_dev_groups = 6;
    uint32_t group_count = max_phys_dev_groups;
    std::array<VkPhysicalDeviceGroupProperties, max_phys_dev_groups> physical_device_groups{};
    for (auto& group : physical_device_groups) group.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;

    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &group_count, physical_device_groups.data()));
    ASSERT_EQ(group_count, max_phys_dev_groups);

    // Make sure the devices are not in the sorted order.  The order is really undefined, but the chances of
    // it being exactly the expected sorted is very low.
    bool sorted = true;
    uint32_t cur_dev = 0;
    for (uint32_t group = 0; group < max_phys_dev_groups; ++group) {
        for (uint32_t dev = 0; dev < physical_device_groups[group].physicalDeviceCount; ++dev) {
            VkPhysicalDeviceProperties props{};
            inst->vkGetPhysicalDeviceProperties(physical_device_groups[group].physicalDevices[dev], &props);
            switch (cur_dev++) {
                case 0:
                    if (props.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || strcmp("pd4", props.deviceName)) {
                        sorted = false;
                    }
                    break;
                case 1:
                    if (props.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || strcmp("pd6", props.deviceName)) {
                        sorted = false;
                    }
                    break;
                case 2:
                    if (props.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || strcmp("pd5", props.deviceName)) {
                        sorted = false;
                    }
                    break;
                case 3:
                    if (props.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || strcmp("pd2", props.deviceName)) {
                        sorted = false;
                    }
                    break;
                case 4:
                    if (props.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || strcmp("pd0", props.deviceName)) {
                        sorted = false;
                    }
                    break;
                case 5:
                    if (props.deviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU || strcmp("pd1", props.deviceName)) {
                        sorted = false;
                    }
                    break;
                case 6:
                    if (props.deviceType != VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU || strcmp("pd7", props.deviceName)) {
                        sorted = false;
                    }
                    break;
                case 7:
                    if (props.deviceType != VK_PHYSICAL_DEVICE_TYPE_CPU || strcmp("pd3", props.deviceName)) {
                        sorted = false;
                    }
                    break;
                default:
                    ASSERT_EQ(false, true);
            }
        }
        if (!sorted) {
            break;
        }
    }
    ASSERT_EQ(false, sorted);

    // Make sure if we call enumerate again, the information is the same
    std::array<VkPhysicalDeviceGroupProperties, max_phys_dev_groups> physical_device_groups_again{};
    for (auto& group : physical_device_groups_again) group.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;

    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &group_count, physical_device_groups_again.data()));
    ASSERT_EQ(group_count, max_phys_dev_groups);
    for (uint32_t group = 0; group < max_phys_dev_groups; ++group) {
        ASSERT_EQ(physical_device_groups[group].physicalDeviceCount, physical_device_groups_again[group].physicalDeviceCount);
        for (uint32_t dev = 0; dev < physical_device_groups[group].physicalDeviceCount; ++dev) {
            ASSERT_EQ(physical_device_groups[group].physicalDevices[dev], physical_device_groups_again[group].physicalDevices[dev]);
        }
    }

    remove_env_var("VK_LOADER_DISABLE_SELECT");
}

#endif  // __linux__ || __FreeBSD__

const char* portability_driver_warning =
    "vkCreateInstance: Found drivers that contain devices which support the portability subset, but the "
    "portability enumeration bit was not set!. Applications that wish to enumerate portability drivers must set the "
    "VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR bit in the VkInstanceCreateInfo flags and"
    "enable the VK_KHR_portability_enumeration instance extension.";

TEST(PortabilityICDConfiguration, PortabilityICDOnly) {
    FrameworkEnvironment env{};
    env.add_icd(
        TestICDDetails(ManifestICD{}.set_lib_path(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA).set_is_portability_driver(true)));

    auto& driver = env.get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    driver.max_icd_interface_version = 1;
    {  // enable portability extension and flag
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        inst.create_info.add_extension("VK_KHR_portability_enumeration");
        inst.create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
        inst.CheckCreate();
        ASSERT_FALSE(env.debug_log.find(portability_driver_warning));

        DebugUtilsWrapper log{inst, VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT};
        CreateDebugUtilsMessenger(log);

        auto phys_dev = inst.GetPhysDev();
        handle_assert_has_value(phys_dev);

        DeviceWrapper dev_info{inst};
        dev_info.CheckCreate(phys_dev);
        ASSERT_FALSE(log.find(portability_driver_warning));
    }
    {  // enable portability flag but not extension - shouldn't be able to create an instance when filtering is enabled
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        inst.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
        inst.CheckCreate(VK_ERROR_INCOMPATIBLE_DRIVER);
        ASSERT_TRUE(env.debug_log.find(portability_driver_warning));
    }
    {  // enable portability extension but not flag - shouldn't be able to create an instance when filtering is enabled
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.add_extension("VK_KHR_portability_enumeration");
        inst.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
        inst.CheckCreate(VK_ERROR_INCOMPATIBLE_DRIVER);
        ASSERT_TRUE(env.debug_log.find(portability_driver_warning));
    }
    {  // enable neither the portability extension or the flag - shouldn't be able to create an instance when filtering is enabled
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.flags = 0;  // make sure its 0 - no portability
        inst.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
        inst.CheckCreate(VK_ERROR_INCOMPATIBLE_DRIVER);
        ASSERT_TRUE(env.debug_log.find(portability_driver_warning));
    }
}

TEST(PortabilityICDConfiguration, PortabilityAndRegularICD) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(ManifestICD{}.set_lib_path(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA)));
    env.add_icd(
        TestICDDetails(ManifestICD{}.set_lib_path(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA).set_is_portability_driver(true)));

    auto& driver0 = env.get_test_icd(0);
    auto& driver1 = env.get_test_icd(1);

    driver0.physical_devices.emplace_back("physical_device_0");
    driver0.max_icd_interface_version = 1;

    driver1.physical_devices.emplace_back("portability_physical_device_1");
    driver1.max_icd_interface_version = 1;
    {  // enable portability extension and flag
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        inst.create_info.add_extension("VK_KHR_portability_enumeration");
        inst.create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
        inst.CheckCreate();
        ASSERT_FALSE(env.debug_log.find(portability_driver_warning));

        DebugUtilsWrapper log{inst, VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT};
        CreateDebugUtilsMessenger(log);

        auto phys_devs = inst.GetPhysDevs(2);
        for (const auto& phys_dev : phys_devs) {
            handle_assert_has_value(phys_dev);
        }
        DeviceWrapper dev_info_0{inst};
        DeviceWrapper dev_info_1{inst};
        dev_info_0.CheckCreate(phys_devs[0]);
        dev_info_1.CheckCreate(phys_devs[1]);
    }
    {  // enable portability extension but not flag - should only enumerate 1 physical device when filtering is enabled
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        inst.create_info.add_extension("VK_KHR_portability_enumeration");
        FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
        inst.CheckCreate();
        ASSERT_FALSE(env.debug_log.find(portability_driver_warning));

        DebugUtilsWrapper log{inst, VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT};
        CreateDebugUtilsMessenger(log);
        auto phys_dev = inst.GetPhysDev();
        handle_assert_has_value(phys_dev);
        DeviceWrapper dev_info_0{inst};
        dev_info_0.CheckCreate(phys_dev);
    }
    {  // enable portability flag but not extension - should only enumerate 1 physical device when filtering is enabled
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        inst.create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
        inst.CheckCreate();
        ASSERT_FALSE(env.debug_log.find(portability_driver_warning));

        DebugUtilsWrapper log{inst, VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT};
        CreateDebugUtilsMessenger(log);
        auto phys_dev = inst.GetPhysDev();
        handle_assert_has_value(phys_dev);
        DeviceWrapper dev_info_0{inst};
        dev_info_0.CheckCreate(phys_dev);
    }
    {  // do not enable portability extension or flag - should only enumerate 1 physical device when filtering is enabled
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
        inst.CheckCreate();
        ASSERT_FALSE(env.debug_log.find(portability_driver_warning));

        DebugUtilsWrapper log{inst, VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT};
        CreateDebugUtilsMessenger(log);
        auto phys_dev = inst.GetPhysDev();
        handle_assert_has_value(phys_dev);
        DeviceWrapper dev_info_0{inst};
        dev_info_0.CheckCreate(phys_dev);
    }
}

TEST(PortabilityICDConfiguration, PortabilityAndRegularICDPreInstanceFunctions) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(ManifestICD{}.set_lib_path(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA)));
    env.add_icd(
        TestICDDetails(ManifestICD{}.set_lib_path(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA).set_is_portability_driver(true)));

    Extension first_ext{"VK_EXT_validation_features"};  // known instance extensions
    Extension second_ext{"VK_EXT_headless_surface"};
    env.get_test_icd().add_instance_extensions({first_ext, second_ext});

    auto& driver0 = env.get_test_icd(0);
    auto& driver1 = env.get_test_icd(1);

    driver0.physical_devices.emplace_back("physical_device_0");
    driver0.max_icd_interface_version = 1;

    driver1.physical_devices.emplace_back("portability_physical_device_1");
    driver1.max_icd_interface_version = 1;
    {
        // check that enumerating instance extensions work with a portability driver present
        uint32_t extension_count = 0;
        std::array<VkExtensionProperties, 5> extensions;
        ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
        ASSERT_EQ(extension_count, 5U);  // return debug report & debug utils + our two extensions

        ASSERT_EQ(VK_SUCCESS,
                  env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data()));
        ASSERT_EQ(extension_count, 5U);
        // loader always adds the debug report & debug utils extensions
        ASSERT_TRUE(first_ext.extensionName == extensions[0].extensionName);
        ASSERT_TRUE(second_ext.extensionName == extensions[1].extensionName);
        ASSERT_TRUE(string_eq("VK_EXT_debug_report", extensions[2].extensionName));
        ASSERT_TRUE(string_eq("VK_EXT_debug_utils", extensions[3].extensionName));
        ASSERT_TRUE(string_eq("VK_KHR_portability_enumeration", extensions[4].extensionName));
    }

    const char* layer_name = "TestLayer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(layer_name).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "test_layer.json");

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer(layer_name);
    inst.CheckCreate();

    VkPhysicalDevice phys_dev = inst.GetPhysDev();
    {  // LayersMatch

        uint32_t layer_count = 0;
        ASSERT_EQ(env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &layer_count, nullptr), VK_SUCCESS);
        ASSERT_EQ(layer_count, 1U);
        VkLayerProperties layer_props;
        ASSERT_EQ(env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &layer_count, &layer_props), VK_SUCCESS);
        ASSERT_EQ(layer_count, 1U);
        ASSERT_TRUE(string_eq(layer_props.layerName, layer_name));
    }
    {  // Property count less than available
        VkLayerProperties layer_props;
        uint32_t layer_count = 0;
        ASSERT_EQ(VK_INCOMPLETE, env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &layer_count, &layer_props));
        ASSERT_EQ(layer_count, 0U);
    }
}
