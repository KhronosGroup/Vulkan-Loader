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

// Test case origin
// LX = lunar exchange
// LVLGH = loader and validation github
// LVLGL = loader and validation gitlab
// VL = Vulkan Loader github
// VVL = Vulkan Validation Layers github

class RegressionTests : public ::testing::Test {
   protected:
    virtual void SetUp() {
        env = std::unique_ptr<SingleICDShim>(new SingleICDShim(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_MAKE_VERSION(1, 0, 0))));
    }

    virtual void TearDown() { env.reset(); }
    std::unique_ptr<SingleICDShim> env;
};

// Subtyping for organization
class EnumerateInstanceVersion : public RegressionTests {};
class EnumerateInstanceLayerProperties : public RegressionTests {};
class EnumerateInstanceExtensionProperties : public RegressionTests {};
class CreateInstance : public RegressionTests {};
class CreateDevice : public RegressionTests {};
class EnumeratePhysicalDevices : public RegressionTests {};
class EnumerateDeviceLayerProperties : public RegressionTests {};
class EnumerateDeviceExtensionProperties : public RegressionTests {};
class ImplicitLayer : public RegressionTests {};
class WrapObjects : public RegressionTests {};
class EnumeratePhysicalDeviceGroupsKHR : public RegressionTests {};

TEST_F(CreateInstance, BasicRun) {
    auto& driver = env->get_test_icd();
    driver.SetMinICDInterfaceVersion(5);

    InstWrapper inst{env->vulkan_functions};
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);
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
        InstanceCreateInfo inst_create_info;
        inst_create_info.add_extension("VK_EXT_validation_features");  // test icd won't report this as supported
        ASSERT_EQ(VK_ERROR_EXTENSION_NOT_PRESENT, CreateInst(inst, inst_create_info));
    }
    {
        InstWrapper inst{env->vulkan_functions};
        InstanceCreateInfo inst_create_info;
        inst_create_info.add_extension("Non_existant_extension");  // unknown instance extension
        ASSERT_EQ(VK_ERROR_EXTENSION_NOT_PRESENT, CreateInst(inst, inst_create_info));
    }
}

TEST_F(CreateInstance, LayerNotPresent) {
    InstWrapper inst{env->vulkan_functions};
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_layer("VK_NON_EXISTANT_LAYER");
    ASSERT_EQ(VK_ERROR_LAYER_NOT_PRESENT, CreateInst(inst, inst_create_info));
}

TEST_F(CreateInstance, LayerPresent) {
    const char* layer_name = "TestLayer";
    ManifestLayer::LayerDescription description{};
    description.name = layer_name;
    description.lib_path = TEST_LAYER_PATH_EXPORT_VERSION_1;

    ManifestLayer layer;
    layer.layers.push_back(description);
    env->AddExplicitLayer(layer, "test_layer.json");

    InstWrapper inst{env->vulkan_functions};
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_layer(layer_name);
    ASSERT_EQ(VK_SUCCESS, CreateInst(inst, inst_create_info));
}

TEST_F(EnumeratePhysicalDevices, OneCall) {
    auto& driver = env->get_test_icd().SetMinICDInterfaceVersion(5);

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.emplace_back("physical_device_1");
    driver.physical_devices.emplace_back("physical_device_2");
    driver.physical_devices.emplace_back("physical_device_3");

    InstWrapper inst{env->vulkan_functions};
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);

    uint32_t physical_count = driver.physical_devices.size();
    uint32_t returned_physical_count = driver.physical_devices.size();
    std::vector<VkPhysicalDevice> physical_device_handles = std::vector<VkPhysicalDevice>(physical_count);
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_physical_count, physical_device_handles.data()));
    ASSERT_EQ(physical_count, returned_physical_count);
}

TEST_F(EnumeratePhysicalDevices, TwoCall) {
    auto& driver = env->get_test_icd().SetMinICDInterfaceVersion(5);

    const uint32_t real_device_count = 2;
    for (size_t i = 0; i < real_device_count; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i));
    }

    InstWrapper inst{env->vulkan_functions};
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);

    uint32_t physical_count = driver.physical_devices.size();
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
    driver.SetMinICDInterfaceVersion(5);

    const uint32_t real_device_count = 3;
    for (size_t i = 0; i < real_device_count; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i));
    }

    InstWrapper inst1{env->vulkan_functions};
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst1, inst_create_info), VK_SUCCESS);

    uint32_t physical_count_one_call = driver.physical_devices.size();
    std::array<VkPhysicalDevice, real_device_count> physical_device_handles_one_call;
    ASSERT_EQ(VK_SUCCESS,
              inst1->vkEnumeratePhysicalDevices(inst1, &physical_count_one_call, physical_device_handles_one_call.data()));
    ASSERT_EQ(real_device_count, physical_count_one_call);

    InstWrapper inst2{env->vulkan_functions};
    ASSERT_EQ(CreateInst(inst2, inst_create_info), VK_SUCCESS);

    uint32_t physical_count = driver.physical_devices.size();
    uint32_t returned_physical_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst2->vkEnumeratePhysicalDevices(inst2, &returned_physical_count, nullptr));
    ASSERT_EQ(physical_count, returned_physical_count);

    std::array<VkPhysicalDevice, real_device_count> physical_device_handles;
    ASSERT_EQ(VK_SUCCESS, inst2->vkEnumeratePhysicalDevices(inst2, &returned_physical_count, physical_device_handles.data()));
    ASSERT_EQ(real_device_count, returned_physical_count);

    ASSERT_EQ(physical_count_one_call, returned_physical_count);
}

TEST_F(EnumeratePhysicalDevices, TwoCallIncomplete) {
    auto& driver = env->get_test_icd().SetMinICDInterfaceVersion(5);

    const uint32_t real_device_count = 2;
    for (size_t i = 0; i < real_device_count; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i));
    }

    InstWrapper inst{env->vulkan_functions};
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);

    uint32_t physical_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &physical_count, nullptr));
    ASSERT_EQ(physical_count, driver.physical_devices.size());

    std::array<VkPhysicalDevice, real_device_count> physical;

    // Remove one from the physical device count so we can get the VK_INCOMPLETE message
    physical_count = 1;

    ASSERT_EQ(VK_INCOMPLETE, inst->vkEnumeratePhysicalDevices(inst, &physical_count, physical.data()));
    ASSERT_EQ(physical_count, 1);
}

TEST_F(CreateDevice, ExtensionNotPresent) {
    auto& driver = env->get_test_icd();

    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    InstWrapper inst{env->vulkan_functions};
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);

    VkPhysicalDevice phys_dev;
    ASSERT_EQ(CreatePhysDev(inst, phys_dev), VK_SUCCESS);

    uint32_t familyCount = 0;
    inst->vkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &familyCount, nullptr);
    ASSERT_EQ(familyCount, 1);

    VkQueueFamilyProperties families;
    inst->vkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &familyCount, &families);
    ASSERT_EQ(familyCount, 1);
    ASSERT_EQ(families, family_props.properties);

    DeviceCreateInfo dev_create_info;
    dev_create_info.add_extension("NotPresent");
    DeviceQueueCreateInfo queue_info;
    queue_info.add_priority(0.0f);
    dev_create_info.add_device_queue(queue_info);

    VkDevice device;
    ASSERT_EQ(VK_ERROR_EXTENSION_NOT_PRESENT, inst->vkCreateDevice(phys_dev, dev_create_info.get(), nullptr, &device));
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
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);

    VkPhysicalDevice phys_dev;
    ASSERT_EQ(CreatePhysDev(inst, phys_dev), VK_SUCCESS);

    uint32_t familyCount = 0;
    inst->vkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &familyCount, nullptr);
    ASSERT_EQ(familyCount, 1);

    VkQueueFamilyProperties families;
    inst->vkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &familyCount, &families);
    ASSERT_EQ(familyCount, 1);
    ASSERT_EQ(families, family_props.properties);

    DeviceCreateInfo dev_create_info;
    dev_create_info.add_layer("NotPresent");
    DeviceQueueCreateInfo queue_info;
    queue_info.add_priority(0.0f);
    dev_create_info.add_device_queue(queue_info);

    VkDevice device;
    ASSERT_EQ(VK_SUCCESS, inst->vkCreateDevice(phys_dev, dev_create_info.get(), nullptr, &device));
}

TEST_F(EnumerateDeviceLayerProperties, LayersMatch) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    const char* layer_name = "TestLayer";
    ManifestLayer::LayerDescription description{};
    description.name = layer_name;
    description.lib_path = TEST_LAYER_PATH_EXPORT_VERSION_2;

    ManifestLayer layer;
    layer.layers.push_back(description);
    env->AddExplicitLayer(layer, "test_layer.json");

    InstWrapper inst{env->vulkan_functions};
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_layer(layer_name);
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);

    VkPhysicalDevice phys_dev;
    ASSERT_EQ(CreatePhysDev(inst, phys_dev), VK_SUCCESS);

    uint32_t layer_count = 0;
    ASSERT_EQ(env->vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &layer_count, nullptr), VK_SUCCESS);
    ASSERT_EQ(layer_count, 1);
    VkLayerProperties layer_props;
    ASSERT_EQ(env->vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &layer_count, &layer_props), VK_SUCCESS);
    ASSERT_EQ(layer_count, 1);
    ASSERT_TRUE(string_eq(layer_props.layerName, layer_name));
}

TEST_F(EnumerateInstanceExtensionProperties, OnePass) {
    Extension first_ext{"VK_EXT_validation_features"};  // known instance extensions
    Extension second_ext{"VK_EXT_headless_surface"};
    env->get_new_test_icd().AddInstanceExtensions({first_ext, second_ext});

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

TEST_F(EnumerateInstanceExtensionProperties, TwoPass) {
    Extension first_ext{"VK_EXT_validation_features"};  // known instance extensions
    Extension second_ext{"VK_EXT_headless_surface"};
    env->get_new_test_icd().AddInstanceExtensions({first_ext, second_ext});

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
    env->get_new_test_icd().AddInstanceExtensions({first_ext, second_ext});

    uint32_t extension_count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, nullptr));
    ASSERT_EQ(extension_count, 2);  // return debug report & debug utils

    std::array<VkExtensionProperties, 2> extensions;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, extensions.data()));
    ASSERT_EQ(extension_count, 2);
    // loader always adds the debug report & debug utils extensions
    ASSERT_TRUE(string_eq(extensions[0].extensionName, "VK_EXT_debug_report"));
    ASSERT_TRUE(string_eq(extensions[1].extensionName, "VK_EXT_debug_utils"));
}

TEST_F(EnumerateInstanceExtensionProperties, DisableUnknownInstanceExtensionFiltering) {
    Extension first_ext{"FirstTestExtension"};  // unknown instance extensions
    Extension second_ext{"SecondTestExtension"};
    env->get_new_test_icd().AddInstanceExtensions({first_ext, second_ext});

    set_env_var("VK_LOADER_DISABLE_INST_EXT_FILTER", "1");

    uint32_t extension_count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, nullptr));
    ASSERT_EQ(extension_count, 4);

    std::array<VkExtensionProperties, 4> extensions;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, extensions.data()));
    ASSERT_EQ(extension_count, 4);

    ASSERT_EQ(extensions[0], first_ext.get());
    ASSERT_EQ(extensions[1], second_ext.get());
    // Loader always adds these two extensions
    ASSERT_TRUE(string_eq(extensions[2].extensionName, "VK_EXT_debug_report"));
    ASSERT_TRUE(string_eq(extensions[3].extensionName, "VK_EXT_debug_utils"));
}

TEST_F(EnumerateDeviceExtensionProperties, DeviceExtensionEnumerated) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    std::array<Extension, 2> device_extensions = {Extension{"MyExtension0", 4}, Extension{"MyExtension1", 7}};
    for (auto& ext : device_extensions) {
        driver.physical_devices.front().extensions.push_back(ext);
    }
    InstWrapper inst{env->vulkan_functions};
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);

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
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);

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

TEST(TryLoadWrongBinaries, WrongICD) {
    FakeBinaryICDShim env(TestICDDetails(TEST_ICD_PATH_VERSION_2), TestICDDetails(CURRENT_PLATFORM_DUMMY_BINARY));
    env.get_test_icd().physical_devices.emplace_back("physical_device_0");

    InstWrapper inst{env.vulkan_functions};
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);

    uint32_t driver_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &driver_count, nullptr));
    ASSERT_EQ(driver_count, 1);
}

TEST(TryLoadWrongBinaries, WrongExplicitAndImplicit) {
    SingleICDShim env(TestICDDetails(TEST_ICD_PATH_VERSION_2));
    env.get_test_icd().physical_devices.emplace_back("physical_device_0");

    const char* layer_name_0 = "DummyLayerExplicit";
    auto description_0 = ManifestLayer::LayerDescription{};
    description_0.name = layer_name_0;
    description_0.lib_path = CURRENT_PLATFORM_DUMMY_BINARY;

    ManifestLayer layer_0;
    layer_0.layers.push_back(description_0);

    auto layer_loc_0 = env.explicit_layer_folder.write("dummy_test_layer_0.json", layer_0);
    env.platform_shim->add_manifest(ManifestCategory::explicit_layer, layer_loc_0);

    const char* layer_name_1 = "DummyLayerImplicit";
    auto description_1 = ManifestLayer::LayerDescription{};
    description_1.name = layer_name_1;
    description_1.lib_path = CURRENT_PLATFORM_DUMMY_BINARY;
    description_1.disable_environment = "DISABLE_ENV";

    ManifestLayer layer_1;
    layer_1.layers.push_back(description_1);

    auto layer_loc_1 = env.implicit_layer_folder.write("dummy_test_layer_1.json", layer_1);
    env.platform_shim->add_manifest(ManifestCategory::implicit_layer, layer_loc_1);

    uint32_t layer_count = 0;
    ASSERT_EQ(env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr), VK_SUCCESS);
    ASSERT_EQ(layer_count, 2);

    std::array<VkLayerProperties, 2> layer_props;
    ASSERT_EQ(env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props.data()), VK_SUCCESS);
    ASSERT_EQ(layer_count, 2);

    InstWrapper inst{env.vulkan_functions};
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_layer(layer_name_0).add_layer(layer_name_1);
    // "According to all known laws of aviation, there is no way that this should return VK_SUCCESS"
    // This by accounts *should* return VK_ERROR_LAYER_NOT_PRESENT but due to a confluence of bad choices and backwards
    // compatibility guarantee, returns VK_SUCCESS.
    // REASON: To be able to 'load' a library in either 32 or 64 bit apps, the loader will just try to load both and ignore
    // whichever library didn't match the current architecture. Because of this, the loader actually just flat out ignores
    // errors and pretends they didn't load at all.
    // TODO: add 32/64 bit field to layer manifests so that this issue doesn't occur, then implement logic to make the loader
    // smart enough to tell when a layer that failed to load was due to the old behavior or not. (eg, don't report an error if
    // a layer with the same name successfully loaded)
    ASSERT_EQ(VK_SUCCESS, CreateInst(inst, inst_create_info));
}
