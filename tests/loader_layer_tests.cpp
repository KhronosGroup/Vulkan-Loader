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

class LayerTests : public ::testing::Test {
   protected:
    virtual void SetUp() {
        env = std::unique_ptr<SingleICDShim>(new SingleICDShim(TestICDDetails(TEST_ICD_PATH_VERSION_6, VK_MAKE_VERSION(1, 0, 0))));
    }

    virtual void TearDown() { env.reset(); }
    std::unique_ptr<SingleICDShim> env;
};

// Subtyping for organization
class ExplicitLayers : public LayerTests {};
class ImplicitLayers : public LayerTests {};
class MetaLayers : public LayerTests {};
class OverrideMetaLayer : public LayerTests {};
class LayerCreateInstance : public LayerTests {};

void CheckLogForLayerString(SingleICDShim& env, const char* implicit_layer_name, bool check_for_enable) {
    {
        InstWrapper inst{env.vulkan_functions};
        FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
        inst.CheckCreate(VK_SUCCESS);
        if (check_for_enable) {
            ASSERT_TRUE(env.debug_log.find(std::string("Insert instance layer ") + implicit_layer_name));
        } else {
            ASSERT_FALSE(env.debug_log.find(std::string("Insert instance layer ") + implicit_layer_name));
        }
    }
    env.debug_log.clear();
}

TEST_F(ImplicitLayers, WithEnableAndDisableEnvVar) {
    const char* implicit_layer_name = "ImplicitTestLayer";
    const char* enable_env_var = "ENABLE_ME";
    const char* disable_env_var = "DISABLE_ME";

    ManifestLayer::LayerDescription description{};
    description.name = implicit_layer_name;
    description.lib_path = TEST_LAYER_PATH_EXPORT_VERSION_2;
    description.enable_environment = enable_env_var;
    description.disable_environment = disable_env_var;

    ManifestLayer implicit_layer;
    implicit_layer.layers.push_back(description);
    env->AddImplicitLayer(implicit_layer, "implicit_test_layer.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1);

    // didn't set enable env-var, layer should not load
    CheckLogForLayerString(*env, implicit_layer_name, false);

    // set enable env-var to 0, no layer should be found
    set_env_var(enable_env_var, "0");
    CheckLogForLayerString(*env, implicit_layer_name, false);

    // // set enable env-var, layer should load
    set_env_var(enable_env_var, "1");
    CheckLogForLayerString(*env, implicit_layer_name, true);

    remove_env_var(enable_env_var);

    // set disable env-var to 0, layer should not load
    set_env_var(disable_env_var, "0");
    CheckLogForLayerString(*env, implicit_layer_name, false);

    // set disable env-var to 1, layer should not load
    set_env_var(disable_env_var, "1");
    CheckLogForLayerString(*env, implicit_layer_name, false);

    // set both enable and disable env-var, layer should not load
    set_env_var(enable_env_var, "1");
    set_env_var(disable_env_var, "1");
    CheckLogForLayerString(*env, implicit_layer_name, false);

    remove_env_var(enable_env_var);
    remove_env_var(disable_env_var);
}

TEST_F(ImplicitLayers, OnlyDisableEnvVar) {
    const char* implicit_layer_name = "ImplicitTestLayer";
    const char* disable_env_var = "DISABLE_ME";

    ManifestLayer::LayerDescription description{};
    description.name = implicit_layer_name;
    description.lib_path = TEST_LAYER_PATH_EXPORT_VERSION_2;
    description.disable_environment = disable_env_var;

    ManifestLayer implicit_layer;
    implicit_layer.layers.push_back(description);
    env->AddImplicitLayer(implicit_layer, "implicit_test_layer.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1);

    // don't set disable env-var, layer should load
    CheckLogForLayerString(*env, implicit_layer_name, true);

    // set disable env-var to 0, layer should not load
    set_env_var(disable_env_var, "0");
    CheckLogForLayerString(*env, implicit_layer_name, false);

    // set disable env-var to 1, layer should not load
    set_env_var(disable_env_var, "1");
    CheckLogForLayerString(*env, implicit_layer_name, false);

    {
        InstWrapper inst{env->vulkan_functions};
        FillDebugUtilsCreateDetails(inst.create_info, env->debug_log);
        inst.create_info.add_layer(implicit_layer_name);
        inst.CheckCreate(VK_SUCCESS);
        ASSERT_TRUE(env->debug_log.find(std::string("Insert instance layer ") + implicit_layer_name));
    }
    remove_env_var(disable_env_var);
}

TEST_F(MetaLayers, InvalidComponentLayer) {
    const char* meta_layer_name = "MetaTestLayer";
    ManifestLayer::LayerDescription description{};
    description.name = meta_layer_name;
    description.component_layers = {"InvalidLayer1", "InvalidLayer2"};
    description.disable_environment = "NotGonnaWork";
    description.instance_extensions.push_back({"NeverGonnaGiveYouUp"});
    description.device_extensions.push_back({"NeverGonnaLetYouDown"});

    ManifestLayer meta_layer;
    meta_layer.file_format_version = ManifestVersion(1, 1, 2);
    meta_layer.layers.push_back(description);
    env->AddImplicitLayer(meta_layer, "meta_test_layer.json");

    const char* regular_layer_name = "TestLayer";
    ManifestLayer::LayerDescription regular_description{};
    regular_description.name = regular_layer_name;
    regular_description.lib_path = TEST_LAYER_PATH_EXPORT_VERSION_1;

    ManifestLayer regular_layer;
    regular_layer.layers.push_back(regular_description);
    env->AddExplicitLayer(regular_layer, "regular_test_layer.json");

    // should find 1, the 'regular' layer
    uint32_t layer_count = 1;
    EXPECT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr));
    EXPECT_EQ(layer_count, 1);

    VkLayerProperties layer_props;
    EXPECT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, &layer_props));
    EXPECT_EQ(layer_count, 1);
    EXPECT_TRUE(string_eq(layer_props.layerName, regular_layer_name));

    uint32_t extension_count = 0;
    std::array<VkExtensionProperties, 2> extensions;
    EXPECT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    EXPECT_EQ(extension_count, 2);  // return debug report & debug utils + our two extensions

    EXPECT_EQ(VK_SUCCESS,
              env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data()));
    EXPECT_EQ(extension_count, 2);

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_layer(meta_layer_name);
    inst.CheckCreate(VK_ERROR_LAYER_NOT_PRESENT);
}

TEST_F(OverrideMetaLayer, InvalidDisableEnvironment) {
    const char* regular_layer_name = "TestLayer";
    ManifestLayer::LayerDescription regular_description{};
    regular_description.name = regular_layer_name;
    regular_description.api_version = VK_MAKE_API_VERSION(0, 1, 1, 0);
    regular_description.lib_path = TEST_LAYER_PATH_EXPORT_VERSION_1;
    regular_description.device_extensions.push_back({"NeverGonnaLetYouDown"});

    ManifestLayer regular_layer;
    regular_layer.layers.push_back(regular_description);
    env->AddExplicitLayer(regular_layer, "regular_test_layer.json");

    const char* lunarg_meta_layer_name = "VK_LAYER_LUNARG_override";
    ManifestLayer::LayerDescription description{};
    description.name = lunarg_meta_layer_name;
    description.api_version = VK_MAKE_API_VERSION(0, 1, 1, 0);
    description.component_layers = {regular_layer_name};
    description.disable_environment = "DisableMeIfYouCan";

    ManifestLayer lunarg_meta_layer;
    lunarg_meta_layer.file_format_version = ManifestVersion(1, 1, 2);
    lunarg_meta_layer.layers.push_back(description);
    env->AddImplicitLayer(lunarg_meta_layer, "meta_test_layer.json");

    // should find 1, the 'regular' layer
    uint32_t layer_count = 2;
    EXPECT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr));
    EXPECT_EQ(layer_count, 2);

    std::array<VkLayerProperties, 2> layer_props;
    EXPECT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props.data()));
    EXPECT_EQ(layer_count, 2);
    EXPECT_TRUE(string_eq(layer_props[0].layerName, lunarg_meta_layer_name));
    EXPECT_TRUE(string_eq(layer_props[1].layerName, regular_layer_name));

    uint32_t extension_count = 0;
    std::array<VkExtensionProperties, 2> extensions;
    EXPECT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    EXPECT_EQ(extension_count, 2);  // return debug report & debug utils + our two extensions

    EXPECT_EQ(VK_SUCCESS,
              env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data()));
    EXPECT_EQ(extension_count, 2);

    InstWrapper inst{env->vulkan_functions};
    // inst.create_info.add_layer();
    inst.CheckCreate();
}

// This test makes sure that any layer calling GetPhysicalDeviceProperties2 inside of CreateInstance
// succeeds and doesn't crash.
TEST_F(LayerCreateInstance, GetPhysicalDeviceProperties2) {
    env->get_test_icd().physical_devices.push_back({});
    env->get_test_icd().icd_api_version = VK_MAKE_API_VERSION(0, 1, 1, 0);
    const char* regular_layer_name = "TestLayer";
    ManifestLayer::LayerDescription regular_description{};
    regular_description.name = regular_layer_name;
    regular_description.lib_path = TEST_LAYER_PATH_EXPORT_VERSION_2;

    ManifestLayer regular_layer;
    regular_layer.file_format_version = ManifestVersion(1, 1, 2);
    regular_layer.layers.push_back(regular_description);
    env->AddExplicitLayer(regular_layer, "regular_test_layer.json");

    TestLayerHandle layer_handle{regular_description.lib_path};
    auto& layer = layer_handle.get_test_layer();
    layer.SetCreateInstanceCallback(
        [](TestLayer& layer, void* data) -> VkResult {
            uint32_t phys_dev_count = 0;
            VkResult res = layer.instance_dispatch_table.EnumeratePhysicalDevices(layer.instance_handle, &phys_dev_count, nullptr);
            if (res != VK_SUCCESS || phys_dev_count > 1) {
                return VK_ERROR_INITIALIZATION_FAILED;  // expecting only a single physical device.
            }
            VkPhysicalDevice phys_dev{};
            res = layer.instance_dispatch_table.EnumeratePhysicalDevices(layer.instance_handle, &phys_dev_count, &phys_dev);
            if (res != VK_SUCCESS) {
                return VK_ERROR_INITIALIZATION_FAILED;
            }
            VkPhysicalDeviceProperties2 props2{};
            props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
            layer.instance_dispatch_table.GetPhysicalDeviceProperties2(phys_dev, &props2);

            VkPhysicalDeviceFeatures2 features2{};
            features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            layer.instance_dispatch_table.GetPhysicalDeviceFeatures2(phys_dev, &features2);
            return VK_SUCCESS;
        },
        nullptr);

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_layer(regular_layer_name).set_api_version(1, 1, 0);
    inst.CheckCreate();
}

TEST_F(LayerCreateInstance, GetPhysicalDeviceProperties2KHR) {
    env->get_test_icd().physical_devices.push_back({});
    env->get_test_icd().AddInstanceExtension({"VK_KHR_get_physical_device_properties2", 0});
    const char* regular_layer_name = "TestLayer";
    ManifestLayer::LayerDescription regular_description{};
    regular_description.name = regular_layer_name;
    regular_description.lib_path = TEST_LAYER_PATH_EXPORT_VERSION_2;

    ManifestLayer regular_layer;
    regular_layer.layers.push_back(regular_description);
    env->AddExplicitLayer(regular_layer, "regular_test_layer.json");

    TestLayerHandle layer_handle{regular_description.lib_path};
    auto& layer = layer_handle.reset_layer();
    layer.SetCreateInstanceCallback(
        [](TestLayer& layer, void* data) -> VkResult {
            uint32_t phys_dev_count = 1;
            VkPhysicalDevice phys_dev{};
            layer.instance_dispatch_table.EnumeratePhysicalDevices(layer.instance_handle, &phys_dev_count, &phys_dev);

            VkPhysicalDeviceProperties2KHR props2{};
            props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR;
            layer.instance_dispatch_table.GetPhysicalDeviceProperties2KHR(phys_dev, &props2);

            VkPhysicalDeviceFeatures2KHR features2{};
            features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;
            layer.instance_dispatch_table.GetPhysicalDeviceFeatures2KHR(phys_dev, &features2);
            return VK_SUCCESS;
        },
        nullptr);

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_layer(regular_layer_name).add_extension("VK_KHR_get_physical_device_properties2");
    inst.CheckCreate();
}

TEST_F(ExplicitLayers, WrapObjects) {
    auto& driver = env->get_test_icd();

    const char* wrap_objects_name = "WrapObjectsLayer";
    ManifestLayer::LayerDescription wrap_objects_description{};
    wrap_objects_description.name = wrap_objects_name;
    wrap_objects_description.lib_path = TEST_LAYER_WRAP_OBJECTS;

    ManifestLayer wrap_objects_layer;
    wrap_objects_layer.layers.push_back(wrap_objects_description);
    env->AddExplicitLayer(wrap_objects_layer, "wrap_objects_layer.json");

    const char* regular_layer_name_1 = "RegularLayer1";
    ManifestLayer::LayerDescription regular_description_1{};
    regular_description_1.name = regular_layer_name_1;
    regular_description_1.lib_path = TEST_LAYER_PATH_EXPORT_VERSION_2;

    ManifestLayer regular_layer_1;
    regular_layer_1.layers.push_back(regular_description_1);
    env->AddExplicitLayer(regular_layer_1, "regular_layer_1.json");

    const char* regular_layer_name_2 = "RegularLayer2";
    ManifestLayer::LayerDescription regular_description_2{};
    regular_description_2.name = regular_layer_name_2;
    regular_description_2.lib_path = TEST_LAYER_PATH_EXPORT_VERSION_2;

    ManifestLayer regular_layer_2;
    regular_layer_2.layers.push_back(regular_description_2);
    env->AddExplicitLayer(regular_layer_2, "regular_layer_2.json");

    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);
    {  // just the wrap layer
        InstWrapper inst{env->vulkan_functions};
        inst.create_info.add_layer(wrap_objects_name);
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();

        DeviceWrapper dev{inst};
        dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
        dev.CheckCreate(phys_dev);
    }
    {  // wrap layer first
        InstWrapper inst{env->vulkan_functions};
        inst.create_info.add_layer(wrap_objects_name).add_layer(regular_layer_name_1);
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();

        DeviceWrapper dev{inst};
        dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
        dev.CheckCreate(phys_dev);
    }
    {  // wrap layer last
        InstWrapper inst{env->vulkan_functions};
        inst.create_info.add_layer(regular_layer_name_1).add_layer(wrap_objects_name);
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();

        DeviceWrapper dev{inst};
        dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
        dev.CheckCreate(phys_dev);
    }
    {  // wrap layer last
        InstWrapper inst{env->vulkan_functions};
        inst.create_info.add_layer(regular_layer_name_1).add_layer(wrap_objects_name).add_layer(regular_layer_name_2);
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();

        DeviceWrapper dev{inst};
        dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
        dev.CheckCreate(phys_dev);
    }
}