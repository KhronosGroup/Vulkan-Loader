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

class RegressionTests : public ::testing::Test {
   protected:
    virtual void SetUp() {
        env = std::unique_ptr<FrameworkEnvironment>(new FrameworkEnvironment());
        env->add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    }

    virtual void TearDown() { env.reset(); }
    std::unique_ptr<FrameworkEnvironment> env;
};

// Subtyping for organization
class CreateInstance : public RegressionTests {};
class EnumerateInstanceVersion : public RegressionTests {};
class EnumerateInstanceLayerProperties : public RegressionTests {};
class EnumerateInstanceExtensionProperties : public RegressionTests {};
class EnumerateDeviceLayerProperties : public RegressionTests {};
class EnumerateDeviceExtensionProperties : public RegressionTests {};
class EnumeratePhysicalDevices : public RegressionTests {};
class SortedPhysicalDevices : public RegressionTests {};
class CreateDevice : public RegressionTests {};
class EnumeratePhysicalDeviceGroups : public RegressionTests {};
class WrapObjects : public RegressionTests {};

TEST_F(CreateInstance, BasicRun) {
    auto& driver = env->get_test_icd();
    driver.set_min_icd_interface_version(5);

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();
}

// LX435
TEST_F(CreateInstance, ConstInstanceInfo) {
    VkInstance inst = VK_NULL_HANDLE;
    VkInstanceCreateInfo const info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, 0, nullptr, 0, nullptr, 0, nullptr};
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(&info, VK_NULL_HANDLE, &inst), VK_SUCCESS);
    // Must clean up
    env->vulkan_functions.vkDestroyInstance(inst, nullptr);
}

// VUID-vkDestroyInstance-instance-parameter, VUID-vkDestroyInstance-pAllocator-parameter
TEST_F(CreateInstance, DestroyInstanceNullHandle) { env->vulkan_functions.vkDestroyInstance(VK_NULL_HANDLE, nullptr); }

// VUID-vkDestroyDevice-device-parameter, VUID-vkDestroyDevice-pAllocator-parameter
TEST_F(CreateInstance, DestroyDeviceNullHandle) { env->vulkan_functions.vkDestroyDevice(VK_NULL_HANDLE, nullptr); }

// VUID-vkCreateInstance-ppEnabledExtensionNames-01388
TEST_F(CreateInstance, ExtensionNotPresent) {
    {
        InstWrapper inst{env->vulkan_functions};
        inst.create_info.add_extension("VK_EXT_validation_features");  // test icd won't report this as supported
        inst.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);
    }
    {
        InstWrapper inst{env->vulkan_functions};
        inst.create_info.add_extension("Non_existant_extension");  // unknown instance extension
        inst.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);
    }
}

TEST_F(CreateInstance, LayerNotPresent) {
    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_layer("VK_NON_EXISTANT_LAYER");
    inst.CheckCreate(VK_ERROR_LAYER_NOT_PRESENT);
}

TEST_F(CreateInstance, LayerPresent) {
    const char* layer_name = "TestLayer";
    env->add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(layer_name).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "test_layer.json");

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_layer(layer_name);
    inst.CheckCreate();
}

TEST(NoDrivers, CreateInstance) {
    FrameworkEnvironment env{};
    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate(VK_ERROR_INCOMPATIBLE_DRIVER);
}

TEST_F(EnumerateInstanceLayerProperties, UsageChecks) {
    const char* layer_name_1 = "TestLayer1";
    const char* layer_name_2 = "TestLayer1";

    env->add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(layer_name_1).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "test_layer_1.json");

    env->add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(layer_name_2).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "test_layer_2.json");

    {  // OnePass
        VkLayerProperties layer_props[2] = {};
        uint32_t layer_count = 2;
        ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props));
        ASSERT_EQ(layer_count, 2);
        ASSERT_TRUE(string_eq(layer_name_1, layer_props[0].layerName));
        ASSERT_TRUE(string_eq(layer_name_2, layer_props[1].layerName));
    }
    {  // OnePass
        uint32_t layer_count = 0;
        ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr));
        ASSERT_EQ(layer_count, 2);

        VkLayerProperties layer_props[2] = {};
        ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props));
        ASSERT_EQ(layer_count, 2);
        ASSERT_TRUE(string_eq(layer_name_1, layer_props[0].layerName));
        ASSERT_TRUE(string_eq(layer_name_2, layer_props[1].layerName));
    }
    {  // PropertyCountLessThanAvailable
        VkLayerProperties layer_props{};
        uint32_t layer_count = 1;
        ASSERT_EQ(VK_INCOMPLETE, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, &layer_props));
        ASSERT_TRUE(string_eq(layer_name_1, layer_props.layerName));
    }
}

TEST_F(EnumerateInstanceExtensionProperties, UsageChecks) {
    Extension first_ext{"VK_EXT_validation_features"};  // known instance extensions
    Extension second_ext{"VK_EXT_headless_surface"};
    env->reset_icd().add_instance_extensions({first_ext, second_ext});

    {  // One Pass
        uint32_t extension_count = 4;
        std::array<VkExtensionProperties, 4> extensions;
        ASSERT_EQ(VK_SUCCESS,
                  env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data()));
        ASSERT_EQ(extension_count, 4);  // return debug report & debug utils + our two extensions

        // loader always adds the debug report & debug utils extensions
        ASSERT_TRUE(first_ext.extensionName == extensions[0].extensionName);
        ASSERT_TRUE(second_ext.extensionName == extensions[1].extensionName);
        ASSERT_TRUE(string_eq("VK_EXT_debug_report", extensions[2].extensionName));
        ASSERT_TRUE(string_eq("VK_EXT_debug_utils", extensions[3].extensionName));
    }
    {  // Two Pass
        uint32_t extension_count = 0;
        std::array<VkExtensionProperties, 4> extensions;
        ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
        ASSERT_EQ(extension_count, 4);  // return debug report & debug utils + our two extensions

        ASSERT_EQ(VK_SUCCESS,
                  env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data()));
        ASSERT_EQ(extension_count, 4);
        // loader always adds the debug report & debug utils extensions
        ASSERT_TRUE(first_ext.extensionName == extensions[0].extensionName);
        ASSERT_TRUE(second_ext.extensionName == extensions[1].extensionName);
        ASSERT_TRUE(string_eq("VK_EXT_debug_report", extensions[2].extensionName));
        ASSERT_TRUE(string_eq("VK_EXT_debug_utils", extensions[3].extensionName));
    }
}

TEST_F(EnumerateInstanceExtensionProperties, PropertyCountLessThanAvailable) {
    uint32_t extension_count = 0;
    std::array<VkExtensionProperties, 2> extensions;
    {  // use nullptr for null string
        ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
        ASSERT_EQ(extension_count, 2);  // return debug report & debug utils
        extension_count = 1;            // artificially remove one extension

        ASSERT_EQ(VK_INCOMPLETE,
                  env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data()));
        ASSERT_EQ(extension_count, 1);
        // loader always adds the debug report & debug utils extensions
        ASSERT_TRUE(string_eq(extensions[0].extensionName, "VK_EXT_debug_report"));
    }
    {  // use "" for null string
        ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, nullptr));
        ASSERT_EQ(extension_count, 2);  // return debug report & debug utils
        extension_count = 1;            // artificially remove one extension

        ASSERT_EQ(VK_INCOMPLETE,
                  env->vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, extensions.data()));
        ASSERT_EQ(extension_count, 1);
        // loader always adds the debug report & debug utils extensions
        ASSERT_TRUE(string_eq(extensions[0].extensionName, "VK_EXT_debug_report"));
    }
}

TEST_F(EnumerateInstanceExtensionProperties, FilterUnkownInstanceExtensions) {
    Extension first_ext{"FirstTestExtension"};  // unknown instance extensions
    Extension second_ext{"SecondTestExtension"};
    env->reset_icd().add_instance_extensions({first_ext, second_ext});
    {
        uint32_t extension_count = 0;
        ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, nullptr));
        ASSERT_EQ(extension_count, 2);  // return debug report & debug utils

        std::array<VkExtensionProperties, 2> extensions;
        ASSERT_EQ(VK_SUCCESS,
                  env->vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, extensions.data()));
        ASSERT_EQ(extension_count, 2);
        // loader always adds the debug report & debug utils extensions
        ASSERT_TRUE(string_eq(extensions[0].extensionName, "VK_EXT_debug_report"));
        ASSERT_TRUE(string_eq(extensions[1].extensionName, "VK_EXT_debug_utils"));
    }
    {  // Disable unknown instance extension filtering
        set_env_var("VK_LOADER_DISABLE_INST_EXT_FILTER", "1");

        uint32_t extension_count = 0;
        ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, nullptr));
        ASSERT_EQ(extension_count, 4);

        std::array<VkExtensionProperties, 4> extensions;
        ASSERT_EQ(VK_SUCCESS,
                  env->vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, extensions.data()));
        ASSERT_EQ(extension_count, 4);

        ASSERT_EQ(extensions[0], first_ext.get());
        ASSERT_EQ(extensions[1], second_ext.get());
        // Loader always adds these two extensions
        ASSERT_TRUE(string_eq(extensions[2].extensionName, "VK_EXT_debug_report"));
        ASSERT_TRUE(string_eq(extensions[3].extensionName, "VK_EXT_debug_utils"));
    }
}

TEST_F(EnumerateDeviceLayerProperties, LayersMatch) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    const char* layer_name = "TestLayer";
    env->add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(layer_name).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "test_layer.json");

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_layer(layer_name);
    inst.CheckCreate();

    VkPhysicalDevice phys_dev = inst.GetPhysDev();
    {  // LayersMatch

        uint32_t layer_count = 0;
        ASSERT_EQ(env->vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &layer_count, nullptr), VK_SUCCESS);
        ASSERT_EQ(layer_count, 1);
        VkLayerProperties layer_props;
        ASSERT_EQ(env->vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &layer_count, &layer_props), VK_SUCCESS);
        ASSERT_EQ(layer_count, 1);
        ASSERT_TRUE(string_eq(layer_props.layerName, layer_name));
    }
    {  // Property count less than available
        VkLayerProperties layer_props;
        uint32_t layer_count = 0;
        ASSERT_EQ(VK_INCOMPLETE, env->vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &layer_count, &layer_props));
        ASSERT_EQ(layer_count, 0);
    }
}

TEST_F(EnumerateDeviceExtensionProperties, DeviceExtensionEnumerated) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    std::array<Extension, 2> device_extensions = {Extension{"MyExtension0", 4}, Extension{"MyExtension1", 7}};
    for (auto& ext : device_extensions) {
        driver.physical_devices.front().extensions.push_back(ext);
    }
    InstWrapper inst{env->vulkan_functions};
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

TEST_F(EnumerateDeviceExtensionProperties, PropertyCountLessThanAvailable) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    std::array<Extension, 2> device_extensions = {Extension{"MyExtension0", 4}, Extension{"MyExtension1", 7}};
    for (auto& ext : device_extensions) {
        driver.physical_devices.front().extensions.push_back(ext);
    }
    InstWrapper inst{env->vulkan_functions};
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

TEST_F(EnumeratePhysicalDevices, OneCall) {
    auto& driver = env->get_test_icd().set_min_icd_interface_version(5);

    driver.physical_devices.emplace_back("physical_device_0", 1);
    driver.physical_devices.emplace_back("physical_device_1", 2);
    driver.physical_devices.emplace_back("physical_device_2", 3);
    driver.physical_devices.emplace_back("physical_device_3", 4);

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    uint32_t physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    uint32_t returned_physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    std::vector<VkPhysicalDevice> physical_device_handles = std::vector<VkPhysicalDevice>(physical_count);
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_device_handles.data()));
    ASSERT_EQ(physical_count, returned_physical_count);
}

TEST_F(EnumeratePhysicalDevices, TwoCall) {
    auto& driver = env->get_test_icd().set_min_icd_interface_version(5);
    Extension first_ext{VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME};  // known instance extensions
    env->reset_icd().add_instance_extension(first_ext);

    const uint32_t real_device_count = 2;
    for (size_t i = 0; i < real_device_count; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i), i + 1);
        driver.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    }

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    uint32_t physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    uint32_t returned_physical_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, nullptr));
    ASSERT_EQ(physical_count, returned_physical_count);

    std::array<VkPhysicalDevice, real_device_count> physical_device_handles;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count,
                                                                           physical_device_handles.data()));
    ASSERT_EQ(physical_count, returned_physical_count);
}

TEST_F(EnumeratePhysicalDevices, MatchOneAndTwoCallNumbers) {
    auto& driver = env->get_test_icd();
    driver.set_min_icd_interface_version(5);
    Extension first_ext{VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME};  // known instance extensions
    env->reset_icd().add_instance_extension(first_ext);

    const uint32_t real_device_count = 3;
    for (size_t i = 0; i < real_device_count; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i), i + 1);
        driver.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    }

    InstWrapper inst1{env->vulkan_functions};
    inst1.CheckCreate();

    uint32_t physical_count_one_call = static_cast<uint32_t>(driver.physical_devices.size());
    std::array<VkPhysicalDevice, real_device_count> physical_device_handles_one_call;
    ASSERT_EQ(VK_SUCCESS,
              inst1->vkEnumeratePhysicalDevices(inst1, &physical_count_one_call, physical_device_handles_one_call.data()));
    ASSERT_EQ(real_device_count, physical_count_one_call);

    InstWrapper inst2{env->vulkan_functions};
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

TEST_F(EnumeratePhysicalDevices, TwoCallIncomplete) {
    auto& driver = env->get_test_icd().set_min_icd_interface_version(5);
    Extension first_ext{VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME};  // known instance extensions
    env->reset_icd().add_instance_extension(first_ext);

    const uint32_t real_device_count = 2;
    for (size_t i = 0; i < real_device_count; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i), i + 1);
        driver.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    }

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    uint32_t physical_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &physical_count, nullptr));
    ASSERT_EQ(physical_count, driver.physical_devices.size());

    std::array<VkPhysicalDevice, real_device_count> physical;

    // Remove one from the physical device count so we can get the VK_INCOMPLETE message
    physical_count = 1;

    ASSERT_EQ(VK_INCOMPLETE, inst->vkEnumeratePhysicalDevices(inst, &physical_count, physical.data()));
    ASSERT_EQ(physical_count, 1);
}

TEST_F(EnumeratePhysicalDevices, ZeroPhysicalDevicesAfterCreateInstance) {
    auto& driver = env->get_test_icd().set_min_icd_interface_version(5);
    InstWrapper inst{env->vulkan_functions};
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

TEST_F(CreateDevice, ExtensionNotPresent) {
    auto& driver = env->get_test_icd();

    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    uint32_t familyCount = 0;
    inst->vkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &familyCount, nullptr);
    ASSERT_EQ(familyCount, 1);

    VkQueueFamilyProperties families;
    inst->vkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &familyCount, &families);
    ASSERT_EQ(familyCount, 1);
    ASSERT_EQ(families, family_props.properties);

    DeviceWrapper dev{inst};
    dev.create_info.add_extension("NotPresent").add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));

    dev.CheckCreate(phys_dev, VK_ERROR_EXTENSION_NOT_PRESENT);
}

// LX535 / MI-76: Device layers are deprecated.
// Ensure that no errors occur if a bogus device layer list is passed to vkCreateDevice.
// https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#extendingvulkan-layers-devicelayerdeprecation
TEST_F(CreateDevice, LayersNotPresent) {
    auto& driver = env->get_test_icd();

    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    uint32_t familyCount = 0;
    inst->vkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &familyCount, nullptr);
    ASSERT_EQ(familyCount, 1);

    VkQueueFamilyProperties families;
    inst->vkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &familyCount, &families);
    ASSERT_EQ(familyCount, 1);
    ASSERT_EQ(families, family_props.properties);

    DeviceWrapper dev{inst};
    DeviceCreateInfo dev_create_info;
    dev.create_info.add_layer("NotPresent").add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));

    dev.CheckCreate(phys_dev);
}

TEST(TryLoadWrongBinaries, WrongICD) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    env.add_icd(TestICDDetails(CURRENT_PLATFORM_DUMMY_BINARY).set_is_fake(true));
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
    ASSERT_EQ(driver_count, 1);
}

TEST(TryLoadWrongBinaries, WrongExplicitAndImplicit) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    env.get_test_icd().physical_devices.emplace_back("physical_device_0");

    const char* layer_name_0 = "DummyLayerExplicit";
    auto layer_0 = ManifestLayer{}.add_layer(
        ManifestLayer::LayerDescription{}.set_name(layer_name_0).set_lib_path(CURRENT_PLATFORM_DUMMY_BINARY));

    auto layer_loc_0 = env.explicit_layer_folder.write("dummy_test_layer_0.json", layer_0);
    env.platform_shim->add_manifest(ManifestCategory::explicit_layer, layer_loc_0);

    const char* layer_name_1 = "DummyLayerImplicit";
    auto layer_1 = ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                 .set_name(layer_name_1)
                                                 .set_lib_path(CURRENT_PLATFORM_DUMMY_BINARY)
                                                 .set_disable_environment("DISABLE_ENV"));

    auto layer_loc_1 = env.implicit_layer_folder.write("dummy_test_layer_1.json", layer_1);
    env.platform_shim->add_manifest(ManifestCategory::implicit_layer, layer_loc_1);

    uint32_t layer_count = 0;
    ASSERT_EQ(env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr), VK_SUCCESS);
    ASSERT_EQ(layer_count, 2);

    std::array<VkLayerProperties, 2> layer_props;
    ASSERT_EQ(env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props.data()), VK_SUCCESS);
    ASSERT_EQ(layer_count, 2);

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer(layer_name_0).add_layer(layer_name_1);
    // "According to all known laws of aviation, there is no way that this should return VK_SUCCESS"
    // This by accounts *should* return VK_ERROR_LAYER_NOT_PRESENT but due to a confluence of bad choices and backwards
    // compatibility guarantee, returns VK_SUCCESS.
    // REASON: To be able to 'load' a library in either 32 or 64 bit apps, the loader will just try to load both and ignore
    // whichever library didn't match the current architecture. Because of this, the loader actually just flat out ignores
    // errors and pretends they didn't load at all.
    // TODO: add 32/64 bit field to layer manifests so that this issue doesn't occur, then implement logic to make the loader
    // smart enough to tell when a layer that failed to load was due to the old behavior or not. (eg, don't report an error if
    // a layer with the same name successfully loaded)
    inst.CheckCreate();
}

TEST_F(EnumeratePhysicalDeviceGroups, OneCall) {
    auto& driver = env->get_test_icd().set_min_icd_interface_version(5);
    Extension first_ext{VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME};  // known instance extensions
    env->reset_icd().add_instance_extension(first_ext);

    // ICD contains 2 devices
    driver.physical_devices.emplace_back("PhysicalDevice0", 12);
    driver.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    driver.physical_devices.emplace_back("PhysicalDevice1", 24);
    driver.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    // ICD contains 1 group, which contains both devices
    driver.physical_device_groups.push_back({});
    driver.physical_device_groups.back()
        .use_physical_device(driver.physical_devices[0])
        .use_physical_device(driver.physical_devices[1]);
    uint32_t physical_device_count = 2;

    // Core function
    {
        InstWrapper inst{env->vulkan_functions};
        inst.create_info.set_api_version(VK_API_VERSION_1_1);
        inst.CheckCreate();

        auto physical_devices = std::vector<VkPhysicalDevice>(physical_device_count);
        uint32_t returned_phys_dev_count = physical_device_count;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_phys_dev_count, physical_devices.data()));
        handle_assert_has_values(physical_devices);

        uint32_t group_count = static_cast<uint32_t>(driver.physical_device_groups.size());
        uint32_t returned_group_count = group_count;
        VkPhysicalDeviceGroupProperties group_props{};
        group_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, &group_props));
        ASSERT_EQ(group_count, returned_group_count);
    }
    driver.add_instance_extension({"VK_KHR_device_group_creation"});
    // Extension
    {
        InstWrapper inst{env->vulkan_functions};
        inst.create_info.add_extension("VK_KHR_device_group_creation");
        inst.CheckCreate();

        auto vkEnumeratePhysicalDeviceGroupsKHR = reinterpret_cast<PFN_vkEnumeratePhysicalDeviceGroupsKHR>(
            env->vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkEnumeratePhysicalDeviceGroupsKHR"));

        auto physical_devices = std::vector<VkPhysicalDevice>(physical_device_count);
        uint32_t returned_phys_dev_count = physical_device_count;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_phys_dev_count, physical_devices.data()));
        handle_assert_has_values(physical_devices);

        uint32_t group_count = static_cast<uint32_t>(driver.physical_device_groups.size());
        uint32_t returned_group_count = group_count;
        VkPhysicalDeviceGroupPropertiesKHR group_props{};
        group_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES_KHR;
        ASSERT_EQ(VK_SUCCESS, vkEnumeratePhysicalDeviceGroupsKHR(inst, &returned_group_count, &group_props));
        ASSERT_EQ(group_count, returned_group_count);
    }
}

TEST_F(EnumeratePhysicalDeviceGroups, TwoCall) {
    auto& driver = env->get_test_icd().set_min_icd_interface_version(5);
    Extension first_ext{VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME};  // known instance extensions
    env->reset_icd().add_instance_extension(first_ext);

    // ICD contains 2 devices
    driver.physical_devices.emplace_back("PhysicalDevice0", 12);
    driver.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    driver.physical_devices.emplace_back("PhysicalDevice1", 24);
    driver.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    // ICD contains 1 group, which contains both devices
    driver.physical_device_groups.push_back({});
    driver.physical_device_groups.back()
        .use_physical_device(driver.physical_devices[0])
        .use_physical_device(driver.physical_devices[1]);
    uint32_t physical_device_count = 2;

    // Core function
    {
        InstWrapper inst{env->vulkan_functions};
        inst.create_info.set_api_version(VK_API_VERSION_1_1);
        inst.CheckCreate();

        auto physical_devices = std::vector<VkPhysicalDevice>(physical_device_count);
        uint32_t returned_phys_dev_count = physical_device_count;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_phys_dev_count, physical_devices.data()));
        handle_assert_has_values(physical_devices);

        uint32_t group_count = static_cast<uint32_t>(driver.physical_device_groups.size());
        uint32_t returned_group_count = 0;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, nullptr));
        ASSERT_EQ(group_count, returned_group_count);

        VkPhysicalDeviceGroupProperties group_props{};
        group_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, &group_props));
        ASSERT_EQ(group_count, returned_group_count);
    }
    driver.add_instance_extension({"VK_KHR_device_group_creation"});
    // Extension
    {
        InstWrapper inst{env->vulkan_functions};
        inst.create_info.add_extension("VK_KHR_device_group_creation");
        inst.CheckCreate();

        auto physical_devices = std::vector<VkPhysicalDevice>(physical_device_count);
        uint32_t returned_phys_dev_count = physical_device_count;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_phys_dev_count, physical_devices.data()));
        handle_assert_has_values(physical_devices);

        auto vkEnumeratePhysicalDeviceGroupsKHR = reinterpret_cast<PFN_vkEnumeratePhysicalDeviceGroupsKHR>(
            env->vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkEnumeratePhysicalDeviceGroupsKHR"));

        uint32_t group_count = static_cast<uint32_t>(driver.physical_device_groups.size());
        uint32_t returned_group_count = 0;
        ASSERT_EQ(VK_SUCCESS, vkEnumeratePhysicalDeviceGroupsKHR(inst, &returned_group_count, nullptr));
        ASSERT_EQ(group_count, returned_group_count);

        VkPhysicalDeviceGroupPropertiesKHR group_props{};
        group_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES_KHR;
        ASSERT_EQ(VK_SUCCESS, vkEnumeratePhysicalDeviceGroupsKHR(inst, &returned_group_count, &group_props));
        ASSERT_EQ(group_count, returned_group_count);
    }
}

TEST_F(EnumeratePhysicalDeviceGroups, TwoCallIncomplete) {
    auto& driver = env->get_test_icd().set_min_icd_interface_version(5);
    Extension first_ext{VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME};  // known instance extensions
    env->reset_icd().add_instance_extension(first_ext);

    // ICD contains 2 devices
    driver.physical_devices.emplace_back("PhysicalDevice0", 12);
    driver.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    driver.physical_devices.emplace_back("PhysicalDevice1", 24);
    driver.physical_devices.back().extensions.push_back({VK_EXT_PCI_BUS_INFO_EXTENSION_NAME, 0});
    // ICD contains 1 group, which contains both devices
    driver.physical_device_groups.push_back({});
    driver.physical_device_groups.back()
        .use_physical_device(driver.physical_devices[0])
        .use_physical_device(driver.physical_devices[1]);

    // Core function
    {
        InstWrapper inst{env->vulkan_functions};
        inst.create_info.set_api_version(VK_API_VERSION_1_1);
        inst.CheckCreate();

        uint32_t group_count = static_cast<uint32_t>(driver.physical_device_groups.size());
        uint32_t returned_group_count = 0;
        ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, nullptr));
        ASSERT_EQ(group_count, returned_group_count);
        returned_group_count = 0;

        VkPhysicalDeviceGroupProperties group_props{};
        group_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
        ASSERT_EQ(VK_INCOMPLETE, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, &group_props));
        ASSERT_EQ(0, returned_group_count);
        handle_assert_no_values(returned_group_count, group_props.physicalDevices);
    }
    driver.add_instance_extension({"VK_KHR_device_group_creation"});
    // Extension
    {
        InstWrapper inst{env->vulkan_functions};
        inst.create_info.add_extension("VK_KHR_device_group_creation");
        inst.CheckCreate();

        auto vkEnumeratePhysicalDeviceGroupsKHR = reinterpret_cast<PFN_vkEnumeratePhysicalDeviceGroupsKHR>(
            env->vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkEnumeratePhysicalDeviceGroupsKHR"));

        uint32_t group_count = static_cast<uint32_t>(driver.physical_device_groups.size());
        uint32_t returned_group_count = 0;
        ASSERT_EQ(VK_SUCCESS, vkEnumeratePhysicalDeviceGroupsKHR(inst, &returned_group_count, nullptr));
        ASSERT_EQ(group_count, returned_group_count);
        returned_group_count = 0;

        VkPhysicalDeviceGroupPropertiesKHR group_props{};
        group_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES_KHR;
        ASSERT_EQ(VK_INCOMPLETE, vkEnumeratePhysicalDeviceGroupsKHR(inst, &returned_group_count, &group_props));
        ASSERT_EQ(0, returned_group_count);
        handle_assert_no_values(returned_group_count, group_props.physicalDevices);
    }
}

#if defined(__linux__) || defined(__FreeBSD__)
// Make sure the loader reports the correct message based on if USE_UNSAFE_FILE_SEARCH is set or not
TEST(EnvironmentVariables, NonSecureEnvVarLookup) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd().physical_devices.emplace_back("physical_device_0");

    DebugUtilsLogger log{VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    InstWrapper inst{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst.create_info, log);
    inst.CheckCreate();

#if !defined(USE_UNSAFE_FILE_SEARCH)
    ASSERT_FALSE(log.find("Loader is using non-secure environment variable lookup for"));
#else
    ASSERT_TRUE(log.find("Loader is using non-secure environment variable lookup for"));
#endif
}

// Check for proper handling of paths specified via environment variables.
TEST(EnvironmentVariables, XDG) {
    // Set up a layer path that includes default and user-specified locations,
    // so that the test app can find them.  Include some badly specified elements as well.
    // Need to redirect the 'home' directory
    fs::path HOME = "/home/fake_home";
    set_env_var("HOME", HOME.str());
    set_env_var("XDG_CONFIG_DIRS", ":/tmp/goober:::::/tmp/goober/::::");
    set_env_var("XDG_CONFIG_HOME", ":/tmp/goober:::::/tmp/goober2/::::");
    set_env_var("XDG_DATA_DIRS", "::::/tmp/goober3:/tmp/goober4/with spaces:::");
    set_env_var("XDG_DATA_HOME", "::::/tmp/goober3:/tmp/goober4/with spaces:::");

    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd().physical_devices.push_back({});

    InstWrapper inst{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
    inst.CheckCreate();

    auto check_paths = [](DebugUtilsLogger const& debug_log, ManifestCategory category, fs::path const& HOME) {
        EXPECT_TRUE(debug_log.find((fs::path("/tmp/goober/vulkan") / category_path_name(category)).str()));
        EXPECT_TRUE(debug_log.find((fs::path("/tmp/goober2/vulkan") / category_path_name(category)).str()));
        EXPECT_TRUE(debug_log.find((fs::path("/tmp/goober3/vulkan") / category_path_name(category)).str()));
        EXPECT_TRUE(debug_log.find((fs::path("/tmp/goober4/with spaces/vulkan") / category_path_name(category)).str()));
    };
    check_paths(env.debug_log, ManifestCategory::icd, HOME);
    check_paths(env.debug_log, ManifestCategory::implicit_layer, HOME);
    check_paths(env.debug_log, ManifestCategory::explicit_layer, HOME);
}

// Check for proper handling of paths specified via environment variables.
TEST(EnvironmentVariables, VK_LAYER_PATH) {
    // Set up a layer path that includes default and user-specified locations,
    // so that the test app can find them.  Include some badly specified elements as well.
    // Need to redirect the 'home' directory
    fs::path HOME = "/home/fake_home";
    set_env_var("HOME", HOME.str());
    std::string vk_layer_path = ":/tmp/carol::::/:";
    vk_layer_path += (HOME / "/ with spaces/:::::/tandy:").str();
    set_env_var("VK_LAYER_PATH", vk_layer_path);

    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd().physical_devices.push_back({});
    env.platform_shim->redirect_path("/tmp/carol", env.explicit_layer_folder.location());

    const char* layer_name = "TestLayer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(layer_name).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "test_layer.json");

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer(layer_name);
    FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
    inst.CheckCreate();

    // look for VK_LAYER_PATHS
    EXPECT_TRUE(env.debug_log.find("/tmp/carol"));
    EXPECT_TRUE(env.debug_log.find("/tandy"));
    EXPECT_TRUE(env.debug_log.find((HOME / "/ with spaces/").str()));
}
#endif

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
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
        env.get_test_icd().physical_devices.push_back({});

        InstWrapper inst{env.vulkan_functions};
        inst.CheckCreate();

        auto phys_dev = inst.GetPhysDev();

        auto getToolProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceToolPropertiesEXT>(
            inst.functions->vkGetInstanceProcAddr(inst, "vkGetPhysicalDeviceToolPropertiesEXT"));
        handle_assert_has_value(getToolProperties);

        uint32_t tool_count = 0;
        ASSERT_EQ(VK_SUCCESS, getToolProperties(phys_dev, &tool_count, nullptr));
        ASSERT_EQ(tool_count, 0);
    }
    {  // extension is supported in driver
        FrameworkEnvironment env{};
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
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
        ASSERT_EQ(tool_count, 1);
        VkPhysicalDeviceToolPropertiesEXT props{};
        ASSERT_EQ(VK_SUCCESS, getToolProperties(phys_dev, &tool_count, &props));
        ASSERT_EQ(tool_count, 1);
        string_eq(props.name, icd_tool_props.name);
    }
}

TEST_F(CreateInstance, InstanceNullLayerPtr) {
    VkInstance inst = VK_NULL_HANDLE;
    VkInstanceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.enabledLayerCount = 1;

    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(&info, VK_NULL_HANDLE, &inst), VK_ERROR_LAYER_NOT_PRESENT);
}
TEST_F(CreateInstance, InstanceNullExtensionPtr) {
    VkInstance inst = VK_NULL_HANDLE;
    VkInstanceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.enabledExtensionCount = 1;

    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(&info, VK_NULL_HANDLE, &inst), VK_ERROR_EXTENSION_NOT_PRESENT);
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

#if defined(__linux__) || defined(__FreeBSD__)
// NOTE: Sort order only affects Linux
TEST(SortedPhysicalDevices, DevicesSortEnabled) {
    FrameworkEnvironment env{};
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
                ASSERT_EQ(pci_bus_info.pciBus, 1);
                break;
            case 1:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
                ASSERT_EQ(true, !strcmp("pd4", props.deviceName));
                ASSERT_EQ(props.vendorID, 75);
                ASSERT_EQ(props.deviceID, 0xCCCC002);
                ASSERT_EQ(pci_bus_info.pciBus, 4);
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
                ASSERT_EQ(pci_bus_info.pciBus, 3);
                break;
            case 4:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU);
                ASSERT_EQ(true, !strcmp("pd5", props.deviceName));
                ASSERT_EQ(props.vendorID, 6940);
                ASSERT_EQ(props.deviceID, 0xDDDD001);
                ASSERT_EQ(pci_bus_info.pciBus, 0);
                break;
            case 5:
                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_CPU);
                ASSERT_EQ(true, !strcmp("pd2", props.deviceName));
                ASSERT_EQ(props.vendorID, 1);
                ASSERT_EQ(props.deviceID, 0xBBBB001);
                ASSERT_EQ(pci_bus_info.pciBus, 0);
                break;
            default:
                ASSERT_EQ(false, true);
        }
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
    }
    ASSERT_EQ(false, sorted);
    remove_env_var("VK_LOADER_DISABLE_SELECT");
}

#if 0  // TODO: Enable check on physical device group sorting to make sure proper order returned.
       //       This is disabled because the test framework needs a little more work for this.
TEST(SortedPhysicalDevices, DeviceGroupsSortedEnabled) {
    FrameworkEnvironment env{};

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    auto& cur_icd_0 = env.get_test_icd(0);
    cur_icd_0.set_icd_api_version(VK_API_VERSION_1_1);
    cur_icd_0.physical_devices.push_back({"pd0", 7});
    FillInRandomDeviceProps(cur_icd_0.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_API_VERSION_1_1,
                            888, 0xAAA001);
    cur_icd_0.physical_devices.push_back({"pd1", 3});
    FillInRandomDeviceProps(cur_icd_0.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
                            VK_API_VERSION_1_1, 888, 0xAAA002);
    cur_icd_0.physical_devices.push_back({"pd2", 6});
    FillInRandomDeviceProps(cur_icd_0.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_API_VERSION_1_1,
                            888, 0xAAA003);
    cur_icd_0.physical_device_groups.push_back({});
    cur_icd_0.physical_device_groups.back()
        .use_physical_device(cur_icd_0.physical_devices[0])
        .use_physical_device(cur_icd_0.physical_devices[2]);
    cur_icd_0.physical_device_groups.push_back({});
    cur_icd_0.physical_device_groups.back().use_physical_device(cur_icd_0.physical_devices[1]);

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    auto& cur_icd_1 = env.get_test_icd(1);
    cur_icd_1.set_icd_api_version(VK_API_VERSION_1_1);
    cur_icd_1.physical_devices.push_back({"pd3", 0});
    FillInRandomDeviceProps(cur_icd_0.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_CPU, VK_API_VERSION_1_1, 1,
                            0xBBBB001);

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    auto& cur_icd_2 = env.get_test_icd(2);
    cur_icd_2.set_icd_api_version(VK_API_VERSION_1_1);
    cur_icd_2.physical_devices.push_back({"pd4", 1});
    FillInRandomDeviceProps(cur_icd_0.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_API_VERSION_1_1,
                            75, 0xCCCC001);
    cur_icd_2.physical_devices.push_back({"pd5", 4});
    FillInRandomDeviceProps(cur_icd_0.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_API_VERSION_1_1,
                            75, 0xCCCC002);
    cur_icd_2.physical_devices.push_back({"pd6", 2});
    FillInRandomDeviceProps(cur_icd_0.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, VK_API_VERSION_1_1,
                            75, 0xCCCC003);
    cur_icd_2.physical_device_groups.push_back({});
    cur_icd_2.physical_device_groups.back()
        .use_physical_device(cur_icd_2.physical_devices[1])
        .use_physical_device(cur_icd_2.physical_devices[2]);
    cur_icd_2.physical_device_groups.push_back({});
    cur_icd_2.physical_device_groups.back().use_physical_device(cur_icd_2.physical_devices[0]);

    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_API_VERSION_1_1));
    auto& cur_icd_3 = env.get_test_icd(3);
    cur_icd_3.set_icd_api_version(VK_API_VERSION_1_1);
    cur_icd_3.physical_devices.push_back({"pd7", 0});
    FillInRandomDeviceProps(cur_icd_0.physical_devices.back().properties, VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU, VK_API_VERSION_1_1,
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

    const uint32_t max_phys_dev_groups = 8;
    uint32_t group_count = max_phys_dev_groups;
    std::array<VkPhysicalDeviceGroupProperties, max_phys_dev_groups> physical_device_groups{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES};
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &group_count, physical_device_groups.data()));
    ASSERT_EQ(group_count, max_phys_dev_groups);

    uint32_t cur_dev = 0;
    for (uint32_t group = 0; group < max_phys_dev_groups; ++group) {
        for (uint32_t dev = 0; dev < physical_device_groups[group].physicalDeviceCount; ++dev) {
            VkPhysicalDeviceProperties props{};
            inst->vkGetPhysicalDeviceProperties(physical_devices[dev], &props);
            VkPhysicalDeviceProperties2 props2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
            VkPhysicalDevicePCIBusInfoPropertiesEXT pci_bus_info{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT};
            props2.pNext = &pci_bus_info;
            GetPhysDevProps2(physical_devices[dev], &props2);
            /*
                        switch (++cur_dev) {
                            case 0:
                                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
                                ASSERT_EQ(true, !strcmp("pd3", props.deviceName));
                                ASSERT_EQ(props.vendorID, 75);
                                ASSERT_EQ(props.deviceID, 0xCCCC001);
                                ASSERT_EQ(pci_bus_info.pciBus, 1);
                                break;
                            case 1:
                                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
                                ASSERT_EQ(true, !strcmp("pd4", props.deviceName));
                                ASSERT_EQ(props.vendorID, 75);
                                ASSERT_EQ(props.deviceID, 0xCCCC002);
                                ASSERT_EQ(pci_bus_info.pciBus, 4);
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
                                ASSERT_EQ(pci_bus_info.pciBus, 3);
                                break;
                            case 4:
                                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU);
                                ASSERT_EQ(true, !strcmp("pd5", props.deviceName));
                                ASSERT_EQ(props.vendorID, 6940);
                                ASSERT_EQ(props.deviceID, 0xDDDD001);
                                ASSERT_EQ(pci_bus_info.pciBus, 0);
                                break;
                            case 5:
                                ASSERT_EQ(props.deviceType, VK_PHYSICAL_DEVICE_TYPE_CPU);
                                ASSERT_EQ(true, !strcmp("pd2", props.deviceName));
                                ASSERT_EQ(props.vendorID, 1);
                                ASSERT_EQ(props.deviceID, 0xBBBB001);
                                ASSERT_EQ(pci_bus_info.pciBus, 0);
                                break;
                            default:
                                ASSERT_EQ(false, true);
                        }
            */
        }
    }
}
#endif
#endif  // __linux__ || __FreeBSD__
