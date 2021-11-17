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
class CreateInstance : public RegressionTests {};
class EnumerateInstanceVersion : public RegressionTests {};
class EnumerateInstanceLayerProperties : public RegressionTests {};
class EnumerateInstanceExtensionProperties : public RegressionTests {};
class EnumerateDeviceLayerProperties : public RegressionTests {};
class EnumerateDeviceExtensionProperties : public RegressionTests {};
class EnumeratePhysicalDevices : public RegressionTests {};
class CreateDevice : public RegressionTests {};
class EnumeratePhysicalDeviceGroups : public RegressionTests {};
class WrapObjects : public RegressionTests {};

TEST_F(CreateInstance, BasicRun) {
    auto& driver = env->get_test_icd();
    driver.SetMinICDInterfaceVersion(5);

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
    ManifestLayer::LayerDescription description{};
    description.name = layer_name;
    description.lib_path = TEST_LAYER_PATH_EXPORT_VERSION_1;

    ManifestLayer layer;
    layer.layers.push_back(description);
    env->AddExplicitLayer(layer, "test_layer.json");

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_layer(layer_name);
    inst.CheckCreate();
}

TEST_F(EnumerateInstanceLayerProperties, UsageChecks) {
    const char* layer_name_1 = "TestLayer1";
    const char* layer_name_2 = "TestLayer1";

    ManifestLayer::LayerDescription description1{};
    description1.name = layer_name_1;
    description1.lib_path = TEST_LAYER_PATH_EXPORT_VERSION_2;

    ManifestLayer layer1;
    layer1.layers.push_back(description1);
    env->AddExplicitLayer(layer1, "test_layer_1.json");

    ManifestLayer::LayerDescription description2{};
    description2.name = layer_name_1;
    description2.lib_path = TEST_LAYER_PATH_EXPORT_VERSION_2;

    ManifestLayer layer2;
    layer2.layers.push_back(description2);
    env->AddExplicitLayer(layer2, "test_layer_2.json");

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
    env->reset_icd().AddInstanceExtensions({first_ext, second_ext});

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
    env->reset_icd().AddInstanceExtensions({first_ext, second_ext});
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
    ManifestLayer::LayerDescription description{};
    description.name = layer_name;
    description.lib_path = TEST_LAYER_PATH_EXPORT_VERSION_2;

    ManifestLayer layer;
    layer.layers.push_back(description);
    env->AddExplicitLayer(layer, "test_layer.json");

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
    auto& driver = env->get_test_icd().SetMinICDInterfaceVersion(5);

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.emplace_back("physical_device_1");
    driver.physical_devices.emplace_back("physical_device_2");
    driver.physical_devices.emplace_back("physical_device_3");

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    uint32_t physical_count = static_cast<uint32_t>(driver.physical_devices.size());
    uint32_t returned_physical_count = static_cast<uint32_t>(driver.physical_devices.size());
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
    driver.SetMinICDInterfaceVersion(5);

    const uint32_t real_device_count = 3;
    for (size_t i = 0; i < real_device_count; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i));
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
    auto& driver = env->get_test_icd().SetMinICDInterfaceVersion(5);

    const uint32_t real_device_count = 2;
    for (size_t i = 0; i < real_device_count; i++) {
        driver.physical_devices.emplace_back(std::string("physical_device_") + std::to_string(i));
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
    FakeBinaryICDShim env(TestICDDetails(TEST_ICD_PATH_VERSION_2), TestICDDetails(CURRENT_PLATFORM_DUMMY_BINARY));
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
    auto& driver = env->get_test_icd().SetMinICDInterfaceVersion(5);
    // ICD contains 2 devices
    driver.physical_devices.emplace_back("PhysicalDevice0");
    driver.physical_devices.emplace_back("PhysicalDevice1");
    // ICD contains 1 group, which contains both devices
    driver.physical_device_groups.push_back({});
    driver.physical_device_groups.back()
        .use_physical_device(driver.physical_devices[0])
        .use_physical_device(driver.physical_devices[1]);
    uint32_t physical_device_count = 2;

    // Core function
    {
        InstWrapper inst{env->vulkan_functions};
        inst.create_info.set_api_version(1, 1, 0);
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
        handle_assert_equal(group_props.physicalDevices[0], physical_devices[0]);
        handle_assert_equal(group_props.physicalDevices[1], physical_devices[1]);
    }
    driver.AddInstanceExtension({"VK_KHR_device_group_creation"});
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
        handle_assert_equal(group_props.physicalDevices[0], physical_devices[0]);
        handle_assert_equal(group_props.physicalDevices[1], physical_devices[1]);
    }
}

TEST_F(EnumeratePhysicalDeviceGroups, TwoCall) {
    auto& driver = env->get_test_icd().SetMinICDInterfaceVersion(5);
    // ICD contains 2 devices
    driver.physical_devices.emplace_back("PhysicalDevice0");
    driver.physical_devices.emplace_back("PhysicalDevice1");
    // ICD contains 1 group, which contains both devices
    driver.physical_device_groups.push_back({});
    driver.physical_device_groups.back()
        .use_physical_device(driver.physical_devices[0])
        .use_physical_device(driver.physical_devices[1]);
    uint32_t physical_device_count = 2;

    // Core function
    {
        InstWrapper inst{env->vulkan_functions};
        inst.create_info.set_api_version(1, 1, 0);
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
        handle_assert_equal(group_props.physicalDevices[0], physical_devices[0]);
        handle_assert_equal(group_props.physicalDevices[1], physical_devices[1]);
    }
    driver.AddInstanceExtension({"VK_KHR_device_group_creation"});
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
        handle_assert_equal(group_props.physicalDevices[0], physical_devices[0]);
        handle_assert_equal(group_props.physicalDevices[1], physical_devices[1]);
    }
}

TEST_F(EnumeratePhysicalDeviceGroups, TwoCallIncomplete) {
    auto& driver = env->get_test_icd().SetMinICDInterfaceVersion(5);
    // ICD contains 2 devices
    driver.physical_devices.emplace_back("PhysicalDevice0");
    driver.physical_devices.emplace_back("PhysicalDevice1");
    // ICD contains 1 group, which contains both devices
    driver.physical_device_groups.push_back({});
    driver.physical_device_groups.back()
        .use_physical_device(driver.physical_devices[0])
        .use_physical_device(driver.physical_devices[1]);

    // Core function
    {
        InstWrapper inst{env->vulkan_functions};
        inst.create_info.set_api_version(1, 1, 0);
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
    driver.AddInstanceExtension({"VK_KHR_device_group_creation"});
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
    SingleICDShim env(TestICDDetails(TEST_ICD_PATH_VERSION_6));
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

    SingleICDShim env{TestICDDetails{TEST_ICD_PATH_VERSION_6}};
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

    SingleICDShim env{TestICDDetails{TEST_ICD_PATH_VERSION_6}};
    env.get_test_icd().physical_devices.push_back({});
    env.platform_shim->redirect_path("/tmp/carol", env.explicit_layer_folder.location());
    const char* layer_name = "TestLayer";

    ManifestLayer::LayerDescription description{};
    description.name = layer_name;
    description.lib_path = TEST_LAYER_PATH_EXPORT_VERSION_2;

    ManifestLayer layer;
    layer.layers.push_back(description);
    env.AddExplicitLayer(layer, "test_layer.json");

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
        SingleICDShim env{TestICDDetails{TEST_ICD_PATH_VERSION_6}};
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
        SingleICDShim env{TestICDDetails{TEST_ICD_PATH_VERSION_6}};
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