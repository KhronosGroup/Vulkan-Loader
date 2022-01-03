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
        env = std::unique_ptr<FrameworkEnvironment>(new FrameworkEnvironment());
        env->add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    }

    virtual void TearDown() { env.reset(); }
    std::unique_ptr<FrameworkEnvironment> env;
};

// Subtyping for organization
class ExplicitLayers : public LayerTests {};
class ImplicitLayers : public LayerTests {};
class LayerExtensions : public LayerTests {};
class MetaLayers : public LayerTests {};
class OverrideMetaLayer : public LayerTests {};
class LayerCreateInstance : public LayerTests {};

void CheckLogForLayerString(FrameworkEnvironment& env, const char* implicit_layer_name, bool check_for_enable) {
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

    env->add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                          .set_name(implicit_layer_name)
                                                          .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                          .set_disable_environment(disable_env_var)
                                                          .set_enable_environment(enable_env_var)),
                            "implicit_test_layer.json");

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

    env->add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                          .set_name(implicit_layer_name)
                                                          .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                          .set_disable_environment(disable_env_var)),
                            "implicit_test_layer.json");

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
    env->add_implicit_layer(ManifestLayer{}
                                .set_file_format_version(ManifestVersion(1, 1, 2))
                                .add_layer(ManifestLayer::LayerDescription{}
                                               .set_name(meta_layer_name)
                                               .add_component_layers({"InvalidLayer1", "InvalidLayer2"})
                                               .set_disable_environment("NotGonnaWork")
                                               .add_instance_extension({"NeverGonnaGiveYouUp"})
                                               .add_device_extension({"NeverGonnaLetYouDown"})),
                            "meta_test_layer.json");

    const char* regular_layer_name = "TestLayer";
    env->add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(regular_layer_name).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "regular_test_layer.json");

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
    env->add_explicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                          .set_name(regular_layer_name)
                                                          .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_1)
                                                          .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))
                                                          .add_device_extension({"NeverGonnaLetYouDown"})),
                            "regular_test_layer.json");

    const char* lunarg_meta_layer_name = "VK_LAYER_LUNARG_override";
    env->add_implicit_layer(ManifestLayer{}
                                .set_file_format_version(ManifestVersion(1, 1, 2))
                                .add_layer(ManifestLayer::LayerDescription{}
                                               .set_name(lunarg_meta_layer_name)
                                               .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))
                                               .add_component_layers({regular_layer_name})
                                               .set_disable_environment("DisableMeIfYouCan")),
                            "meta_test_layer.json");

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
    env->add_explicit_layer(
        ManifestLayer{}
            .set_file_format_version(ManifestVersion(1, 1, 2))
            .add_layer(
                ManifestLayer::LayerDescription{}.set_name(regular_layer_name).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "regular_test_layer.json");

    auto& layer = env->get_test_layer(0);
    layer.set_create_instance_callback([](TestLayer& layer) -> VkResult {
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
    });

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_layer(regular_layer_name).set_api_version(1, 1, 0);
    inst.CheckCreate();
}

TEST_F(LayerCreateInstance, GetPhysicalDeviceProperties2KHR) {
    env->get_test_icd().physical_devices.push_back({});
    env->get_test_icd().add_instance_extension({"VK_KHR_get_physical_device_properties2", 0});

    const char* regular_layer_name = "TestLayer";
    env->add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(regular_layer_name).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "regular_test_layer.json");

    auto& layer = env->get_test_layer(0);
    layer.set_create_instance_callback([](TestLayer& layer) -> VkResult {
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
    });

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_layer(regular_layer_name).add_extension("VK_KHR_get_physical_device_properties2");
    inst.CheckCreate();
}

TEST_F(ExplicitLayers, WrapObjects) {
    auto& driver = env->get_test_icd();

    const char* wrap_objects_name = "VK_LAYER_LUNARG_wrap_objects";
    env->add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(wrap_objects_name).set_lib_path(TEST_LAYER_WRAP_OBJECTS)),
        "wrap_objects_layer.json");

    const char* regular_layer_name_1 = "RegularLayer1";
    env->add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(regular_layer_name_1).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "regular_layer_1.json");

    const char* regular_layer_name_2 = "RegularLayer2";
    env->add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(regular_layer_name_2).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "regular_layer_2.json");

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

TEST_F(LayerExtensions, ImplicitNoAdditionalInstanceExtension) {
    auto& driver = env->get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* implicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    const char* enable_env_var = "ENABLE_ME";
    const char* disable_env_var = "DISABLE_ME";

    env->add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                          .set_name(implicit_layer_name)
                                                          .set_lib_path(TEST_LAYER_WRAP_OBJECTS)
                                                          .set_disable_environment(disable_env_var)
                                                          .set_enable_environment(enable_env_var)),
                            "implicit_wrap_layer_no_ext.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1);

    // // set enable env-var, layer should load
    set_env_var(enable_env_var, "1");
    CheckLogForLayerString(*env, implicit_layer_name, true);

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    if (extension_count > 0) {
        extension_props.resize(extension_count);
        ASSERT_EQ(VK_SUCCESS,
                  env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_props.data()));

        // Make sure the extensions that are implemented only in the test layers is not present.
        for (uint32_t ext = 0; ext < extension_count; ++ext) {
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME));
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME));
        }
    }

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    // Make sure all the function pointers are NULL as well
    ASSERT_EQ(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkReleaseDisplayEXT"));
    ASSERT_EQ(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));

    remove_env_var(enable_env_var);
}

TEST_F(LayerExtensions, ImplicitDirDispModeInstanceExtension) {
    auto& driver = env->get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* implicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    const char* enable_env_var = "ENABLE_ME";
    const char* disable_env_var = "DISABLE_ME";

    env->add_implicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}
                .set_name(implicit_layer_name)
                .set_lib_path(TEST_LAYER_WRAP_OBJECTS_1)
                .set_disable_environment(disable_env_var)
                .set_enable_environment(enable_env_var)
                .add_instance_extension({VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME, 1, {"vkReleaseDisplayEXT"}})),
        "implicit_wrap_layer_dir_disp_mode.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1);

    // // set enable env-var, layer should load
    set_env_var(enable_env_var, "1");
    CheckLogForLayerString(*env, implicit_layer_name, true);

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    ASSERT_GE(extension_count, 1);
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS,
              env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_props.data()));

    // Make sure the extensions that are implemented only in the test layers is not present.
    bool found = false;
    for (uint32_t ext = 0; ext < extension_count; ++ext) {
        if (!strcmp(extension_props[ext].extensionName, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME)) {
            found = true;
        }
        ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME));
    }
    ASSERT_EQ(true, found);

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extension(VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME);
    inst.CheckCreate();

    // Make sure only the appropriate function pointers are NULL as well
    ASSERT_NE(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkReleaseDisplayEXT"));
    ASSERT_EQ(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));

    remove_env_var(enable_env_var);
}

TEST_F(LayerExtensions, ImplicitDispSurfCountInstanceExtension) {
    auto& driver = env->get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* implicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    const char* enable_env_var = "ENABLE_ME";
    const char* disable_env_var = "DISABLE_ME";

    env->add_implicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}
                .set_name(implicit_layer_name)
                .set_lib_path(TEST_LAYER_WRAP_OBJECTS_2)
                .set_disable_environment(disable_env_var)
                .set_enable_environment(enable_env_var)
                .add_instance_extension(
                    {VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME, 1, {"vkGetPhysicalDeviceSurfaceCapabilities2EXT"}})),
        "implicit_wrap_layer_disp_surf_count.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1);

    // // set enable env-var, layer should load
    set_env_var(enable_env_var, "1");
    CheckLogForLayerString(*env, implicit_layer_name, true);

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    ASSERT_GE(extension_count, 1);
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS,
              env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_props.data()));

    // Make sure the extensions that are implemented only in the test layers is not present.
    bool found = false;
    for (uint32_t ext = 0; ext < extension_count; ++ext) {
        if (!strcmp(extension_props[ext].extensionName, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME)) {
            found = true;
        }
        ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME));
    }
    ASSERT_EQ(true, found);

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extension(VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME);
    inst.CheckCreate();

    // Make sure only the appropriate function pointers are NULL as well
    ASSERT_EQ(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkReleaseDisplayEXT"));
    ASSERT_NE(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));

    remove_env_var(enable_env_var);
}

TEST_F(LayerExtensions, ImplicitBothInstanceExtensions) {
    auto& driver = env->get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* implicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    const char* enable_env_var = "ENABLE_ME";
    const char* disable_env_var = "DISABLE_ME";

    env->add_implicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}
                .set_name(implicit_layer_name)
                .set_lib_path(TEST_LAYER_WRAP_OBJECTS_3)
                .set_disable_environment(disable_env_var)
                .set_enable_environment(enable_env_var)
                .add_instance_extension({VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME, 1, {"vkReleaseDisplayEXT"}})
                .add_instance_extension(
                    {VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME, 1, {"vkGetPhysicalDeviceSurfaceCapabilities2EXT"}})),
        "implicit_wrap_layer_both_inst.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1);

    // // set enable env-var, layer should load
    set_env_var(enable_env_var, "1");
    CheckLogForLayerString(*env, implicit_layer_name, true);

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    ASSERT_GE(extension_count, 2);
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS,
              env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_props.data()));

    // Make sure the extensions that are implemented only in the test layers is not present.
    bool found[2] = {false, false};
    for (uint32_t ext = 0; ext < extension_count; ++ext) {
        if (!strcmp(extension_props[ext].extensionName, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME)) {
            found[0] = true;
        }
        if (!strcmp(extension_props[ext].extensionName, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME)) {
            found[1] = true;
        }
    }
    for (uint32_t ext = 0; ext < 2; ++ext) {
        ASSERT_EQ(true, found[ext]);
    }

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extension(VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME)
        .add_extension(VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME);
    inst.CheckCreate();

    // Make sure only the appropriate function pointers are NULL as well
    ASSERT_NE(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkReleaseDisplayEXT"));
    ASSERT_NE(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));

    remove_env_var(enable_env_var);
}

TEST_F(LayerExtensions, ExplicitNoAdditionalInstanceExtension) {
    auto& driver = env->get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* explicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    env->add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(explicit_layer_name).set_lib_path(TEST_LAYER_WRAP_OBJECTS)),
        "explicit_wrap_layer_no_ext.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1);

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    if (extension_count > 0) {
        extension_props.resize(extension_count);
        ASSERT_EQ(VK_SUCCESS,
                  env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_props.data()));

        // Make sure the extensions are not present
        for (uint32_t ext = 0; ext < extension_count; ++ext) {
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME));
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME));
        }
    }

    // Now query by layer name.
    extension_count = 0;
    extension_props.clear();
    ASSERT_EQ(VK_SUCCESS,
              env->vulkan_functions.vkEnumerateInstanceExtensionProperties(explicit_layer_name, &extension_count, nullptr));
    if (extension_count > 0) {
        extension_props.resize(extension_count);
        ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties(explicit_layer_name, &extension_count,
                                                                                           extension_props.data()));

        // Make sure the extensions still aren't present in this layer
        for (uint32_t ext = 0; ext < extension_count; ++ext) {
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME));
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME));
        }
    }

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    // Make sure all the function pointers are NULL as well
    ASSERT_EQ(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkReleaseDisplayEXT"));
    ASSERT_EQ(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));
}

TEST_F(LayerExtensions, ExplicitDirDispModeInstanceExtension) {
    auto& driver = env->get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* explicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    env->add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}
                .set_name(explicit_layer_name)
                .set_lib_path(TEST_LAYER_WRAP_OBJECTS_1)
                .add_instance_extension({VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME, 1, {"vkReleaseDisplayEXT"}})),
        "explicit_wrap_layer_dir_disp_mode.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1);

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    if (extension_count > 0) {
        extension_props.resize(extension_count);
        ASSERT_EQ(VK_SUCCESS,
                  env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_props.data()));

        // Make sure the extensions are not present
        for (uint32_t ext = 0; ext < extension_count; ++ext) {
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME));
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME));
        }
    }

    // Now query by layer name.
    extension_count = 0;
    extension_props.clear();
    ASSERT_EQ(VK_SUCCESS,
              env->vulkan_functions.vkEnumerateInstanceExtensionProperties(explicit_layer_name, &extension_count, nullptr));
    ASSERT_GE(extension_count, 1);
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties(explicit_layer_name, &extension_count,
                                                                                       extension_props.data()));

    // Make sure the extensions still aren't present in this layer
    bool found = false;
    for (uint32_t ext = 0; ext < extension_count; ++ext) {
        if (!strcmp(extension_props[ext].extensionName, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME)) {
            found = true;
        }
        ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME));
    }
    ASSERT_EQ(true, found);

    InstWrapper inst1{env->vulkan_functions};
    inst1.create_info.add_extension(VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME);
    inst1.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    // Make sure only the appropriate function pointers are NULL as well
    ASSERT_EQ(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst1.inst, "vkReleaseDisplayEXT"));
    ASSERT_EQ(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst1.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));

    InstWrapper inst2{env->vulkan_functions};
    inst2.create_info.add_layer(explicit_layer_name).add_extension(VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME);
    inst2.CheckCreate();

    // Make sure only the appropriate function pointers are NULL as well
    ASSERT_NE(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst2.inst, "vkReleaseDisplayEXT"));
    ASSERT_EQ(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst2.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));
}

TEST_F(LayerExtensions, ExplicitDispSurfCountInstanceExtension) {
    auto& driver = env->get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* explicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    env->add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}
                .set_name(explicit_layer_name)
                .set_lib_path(TEST_LAYER_WRAP_OBJECTS_2)
                .add_instance_extension(
                    {VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME, 1, {"vkGetPhysicalDeviceSurfaceCapabilities2EXT"}})),
        "explicit_wrap_layer_disp_surf_count.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1);

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    if (extension_count > 0) {
        extension_props.resize(extension_count);
        ASSERT_EQ(VK_SUCCESS,
                  env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_props.data()));

        // Make sure the extensions are not present
        for (uint32_t ext = 0; ext < extension_count; ++ext) {
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME));
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME));
        }
    }

    // Now query by layer name.
    extension_count = 0;
    extension_props.clear();
    ASSERT_EQ(VK_SUCCESS,
              env->vulkan_functions.vkEnumerateInstanceExtensionProperties(explicit_layer_name, &extension_count, nullptr));
    ASSERT_GE(extension_count, 1);
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties(explicit_layer_name, &extension_count,
                                                                                       extension_props.data()));

    // Make sure the extensions still aren't present in this layer
    bool found = false;
    for (uint32_t ext = 0; ext < extension_count; ++ext) {
        if (!strcmp(extension_props[ext].extensionName, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME)) {
            found = true;
        }
        ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME));
    }
    ASSERT_EQ(true, found);

    InstWrapper inst1{env->vulkan_functions};
    inst1.create_info.add_extension(VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME);
    inst1.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    // Make sure only the appropriate function pointers are NULL as well
    ASSERT_EQ(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst1.inst, "vkReleaseDisplayEXT"));
    ASSERT_EQ(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst1.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));

    InstWrapper inst2{env->vulkan_functions};
    inst2.create_info.add_layer(explicit_layer_name).add_extension(VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME);
    inst2.CheckCreate();

    // Make sure only the appropriate function pointers are NULL as well
    ASSERT_EQ(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst2.inst, "vkReleaseDisplayEXT"));
    ASSERT_NE(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst2.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));
}

TEST_F(LayerExtensions, ExplicitBothInstanceExtensions) {
    auto& driver = env->get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* explicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    env->add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}
                .set_name(explicit_layer_name)
                .set_lib_path(TEST_LAYER_WRAP_OBJECTS_3)
                .add_instance_extension({VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME, 1, {"vkReleaseDisplayEXT"}})
                .add_instance_extension(
                    {VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME, 1, {"vkGetPhysicalDeviceSurfaceCapabilities2EXT"}})),
        "explicit_wrap_layer_both_inst.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1);

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    if (extension_count > 0) {
        extension_props.resize(extension_count);
        ASSERT_EQ(VK_SUCCESS,
                  env->vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_props.data()));

        // Make sure the extensions are not present
        for (uint32_t ext = 0; ext < extension_count; ++ext) {
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME));
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME));
        }
    }

    // Now query by layer name.
    extension_count = 0;
    extension_props.clear();
    ASSERT_EQ(VK_SUCCESS,
              env->vulkan_functions.vkEnumerateInstanceExtensionProperties(explicit_layer_name, &extension_count, nullptr));
    ASSERT_GE(extension_count, 2);
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties(explicit_layer_name, &extension_count,
                                                                                       extension_props.data()));

    // Make sure the extensions still aren't present in this layer
    bool found[2] = {false, false};
    for (uint32_t ext = 0; ext < extension_count; ++ext) {
        if (!strcmp(extension_props[ext].extensionName, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME)) {
            found[0] = true;
        }
        if (!strcmp(extension_props[ext].extensionName, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME)) {
            found[1] = true;
        }
    }
    for (uint32_t ext = 0; ext < 2; ++ext) {
        ASSERT_EQ(true, found[ext]);
    }

    InstWrapper inst1{env->vulkan_functions};
    inst1.create_info.add_extension(VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME)
        .add_extension(VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME);
    inst1.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    // Make sure only the appropriate function pointers are NULL as well
    ASSERT_EQ(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst1.inst, "vkReleaseDisplayEXT"));
    ASSERT_EQ(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst1.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));

    InstWrapper inst2{env->vulkan_functions};
    inst2.create_info.add_layer(explicit_layer_name)
        .add_extension(VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME)
        .add_extension(VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME);
    inst2.CheckCreate();

    // Make sure only the appropriate function pointers are NULL as well
    ASSERT_NE(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst2.inst, "vkReleaseDisplayEXT"));
    ASSERT_NE(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst2.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));
}

TEST_F(LayerExtensions, ImplicitNoAdditionalDeviceExtension) {
    auto& driver = env->get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* implicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    const char* enable_env_var = "ENABLE_ME";
    const char* disable_env_var = "DISABLE_ME";

    env->add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                          .set_name(implicit_layer_name)
                                                          .set_lib_path(TEST_LAYER_WRAP_OBJECTS)
                                                          .set_disable_environment(disable_env_var)
                                                          .set_enable_environment(enable_env_var)),
                            "implicit_wrap_layer_no_ext.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1);

    // // set enable env-var, layer should load
    set_env_var(enable_env_var, "1");
    CheckLogForLayerString(*env, implicit_layer_name, true);

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count, nullptr));
    if (extension_count > 0) {
        extension_props.resize(extension_count);
        ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count,
                                                                                         extension_props.data()));

        // Make sure the extensions that are implemented only in the test layers is not present.
        for (uint32_t ext = 0; ext < extension_count; ++ext) {
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_KHR_MAINTENANCE1_EXTENSION_NAME));
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME));
        }
    }

    DeviceWrapper dev{inst};
    dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(phys_dev);

    // Make sure all the function pointers are NULL as well
    ASSERT_EQ(nullptr, dev->vkGetDeviceProcAddr(dev.dev, "vkTrimCommandPoolKHR"));
    ASSERT_EQ(nullptr, dev->vkGetDeviceProcAddr(dev.dev, "vkGetSwapchainStatusKHR"));
    ASSERT_EQ(nullptr, dev->vkGetDeviceProcAddr(dev.dev, "vkSetDeviceMemoryPriorityEXT"));

    remove_env_var(enable_env_var);
}

TEST_F(LayerExtensions, ImplicitMaintenanceDeviceExtension) {
    auto& driver = env->get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* implicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    const char* enable_env_var = "ENABLE_ME";
    const char* disable_env_var = "DISABLE_ME";

    env->add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                          .set_name(implicit_layer_name)
                                                          .set_lib_path(TEST_LAYER_WRAP_OBJECTS_1)
                                                          .set_disable_environment(disable_env_var)
                                                          .set_enable_environment(enable_env_var)),
                            "implicit_wrap_layer_maint.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1);

    // // set enable env-var, layer should load
    set_env_var(enable_env_var, "1");
    CheckLogForLayerString(*env, implicit_layer_name, true);

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count, nullptr));
    ASSERT_GE(extension_count, 1);
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count,
                                                                                     extension_props.data()));

    // Make sure only the one extension implemented by the enabled implicit layer is present.
    bool found = false;
    for (uint32_t ext = 0; ext < extension_count; ++ext) {
        if (!strcmp(extension_props[ext].extensionName, VK_KHR_MAINTENANCE1_EXTENSION_NAME)) {
            found = true;
        }
        ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME));
    }
    ASSERT_EQ(true, found);

    DeviceWrapper dev{inst};
    dev.create_info.add_extension(VK_KHR_MAINTENANCE1_EXTENSION_NAME).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(phys_dev);

    // Make sure only the appropriate function pointers are NULL as well
    ASSERT_NE(nullptr, dev->vkGetDeviceProcAddr(dev.dev, "vkTrimCommandPoolKHR"));
    ASSERT_EQ(nullptr, dev->vkGetDeviceProcAddr(dev.dev, "vkGetSwapchainStatusKHR"));

    remove_env_var(enable_env_var);
}

TEST_F(LayerExtensions, ImplicitPresentImageDeviceExtension) {
    auto& driver = env->get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* implicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    const char* enable_env_var = "ENABLE_ME";
    const char* disable_env_var = "DISABLE_ME";

    env->add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                          .set_name(implicit_layer_name)
                                                          .set_lib_path(TEST_LAYER_WRAP_OBJECTS_2)
                                                          .set_disable_environment(disable_env_var)
                                                          .set_enable_environment(enable_env_var)),
                            "implicit_wrap_layer_pres.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1);

    // // set enable env-var, layer should load
    set_env_var(enable_env_var, "1");
    CheckLogForLayerString(*env, implicit_layer_name, true);

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count, nullptr));
    ASSERT_GE(extension_count, 1);
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count,
                                                                                     extension_props.data()));

    // Make sure only the one extension implemented by the enabled implicit layer is present.
    bool found = false;
    for (uint32_t ext = 0; ext < extension_count; ++ext) {
        ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_KHR_MAINTENANCE1_EXTENSION_NAME));
        if (!strcmp(extension_props[ext].extensionName, VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME)) {
            found = true;
        }
    }
    ASSERT_EQ(true, found);

    DeviceWrapper dev{inst};
    dev.create_info.add_extension(VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME)
        .add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(phys_dev);

    // Make sure only the appropriate function pointers are NULL as well
    ASSERT_EQ(nullptr, dev->vkGetDeviceProcAddr(dev.dev, "vkTrimCommandPoolKHR"));
    ASSERT_NE(nullptr, dev->vkGetDeviceProcAddr(dev.dev, "vkGetSwapchainStatusKHR"));

    remove_env_var(enable_env_var);
}

TEST_F(LayerExtensions, ImplicitBothDeviceExtensions) {
    auto& driver = env->get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* implicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    const char* enable_env_var = "ENABLE_ME";
    const char* disable_env_var = "DISABLE_ME";

    env->add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                          .set_name(implicit_layer_name)
                                                          .set_lib_path(TEST_LAYER_WRAP_OBJECTS_3)
                                                          .set_disable_environment(disable_env_var)
                                                          .set_enable_environment(enable_env_var)),
                            "implicit_wrap_layer_both_dev.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1);

    // // set enable env-var, layer should load
    set_env_var(enable_env_var, "1");
    CheckLogForLayerString(*env, implicit_layer_name, true);

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count, nullptr));
    ASSERT_GE(extension_count, 2);
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count,
                                                                                     extension_props.data()));

    // Make sure only the one extension implemented by the enabled implicit layer is present.
    bool found[2] = {false, false};
    for (uint32_t ext = 0; ext < extension_count; ++ext) {
        if (!strcmp(extension_props[ext].extensionName, VK_KHR_MAINTENANCE1_EXTENSION_NAME)) {
            found[0] = true;
        }
        if (!strcmp(extension_props[ext].extensionName, VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME)) {
            found[1] = true;
        }
    }
    for (uint32_t ext = 0; ext < 2; ++ext) {
        ASSERT_EQ(true, found[ext]);
    }

    DeviceWrapper dev{inst};
    dev.create_info.add_extension(VK_KHR_MAINTENANCE1_EXTENSION_NAME)
        .add_extension(VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME)
        .add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(phys_dev);

    // Make sure only the appropriate function pointers are NULL as well
    ASSERT_NE(nullptr, dev->vkGetDeviceProcAddr(dev.dev, "vkTrimCommandPoolKHR"));
    ASSERT_NE(nullptr, dev->vkGetDeviceProcAddr(dev.dev, "vkGetSwapchainStatusKHR"));

    remove_env_var(enable_env_var);
}

TEST_F(LayerExtensions, ExplicitNoAdditionalDeviceExtension) {
    auto& driver = env->get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* explicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    env->add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(explicit_layer_name).set_lib_path(TEST_LAYER_WRAP_OBJECTS)),
        "explicit_wrap_layer_no_ext.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1);

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_layer(explicit_layer_name);
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count, nullptr));
    if (extension_count > 0) {
        extension_props.resize(extension_count);
        ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count,
                                                                                         extension_props.data()));

        // Make sure the extensions are not present
        for (uint32_t ext = 0; ext < extension_count; ++ext) {
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_KHR_MAINTENANCE1_EXTENSION_NAME));
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME));
        }
    }

    // Now query by layer name.
    extension_count = 0;
    extension_props.clear();
    ASSERT_EQ(VK_SUCCESS,
              env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, explicit_layer_name, &extension_count, nullptr));
    if (extension_count > 0) {
        extension_props.resize(extension_count);
        ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, explicit_layer_name,
                                                                                         &extension_count, extension_props.data()));

        // Make sure the extensions still aren't present in this layer
        for (uint32_t ext = 0; ext < extension_count; ++ext) {
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_KHR_MAINTENANCE1_EXTENSION_NAME));
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME));
        }
    }

    DeviceWrapper dev{inst};
    dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(phys_dev);

    // Make sure all the function pointers are NULL as well
    ASSERT_EQ(nullptr, dev->vkGetDeviceProcAddr(dev.dev, "vkTrimCommandPoolKHR"));
    ASSERT_EQ(nullptr, dev->vkGetDeviceProcAddr(dev.dev, "vkGetSwapchainStatusKHR"));
    ASSERT_EQ(nullptr, dev->vkGetDeviceProcAddr(dev.dev, "vkSetDeviceMemoryPriorityEXT"));
}

TEST_F(LayerExtensions, ExplicitMaintenanceDeviceExtension) {
    auto& driver = env->get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* explicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    env->add_explicit_layer(
        ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                      .set_name(explicit_layer_name)
                                      .set_lib_path(TEST_LAYER_WRAP_OBJECTS_1)
                                      .set_api_version(VK_MAKE_API_VERSION(0, 1, 0, 0))
                                      .add_device_extension({VK_KHR_MAINTENANCE1_EXTENSION_NAME, 1, {"vkTrimCommandPoolKHR"}})),
        "explicit_wrap_layer_maint.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1);

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_layer(explicit_layer_name);
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count, nullptr));
    if (extension_count > 0) {
        extension_props.resize(extension_count);
        ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count,
                                                                                         extension_props.data()));

        // Make sure the extensions are not present
        for (uint32_t ext = 0; ext < extension_count; ++ext) {
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_KHR_MAINTENANCE1_EXTENSION_NAME));
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME));
        }
    }

    // Now query by layer name.
    extension_count = 0;
    extension_props.clear();
    ASSERT_EQ(VK_SUCCESS,
              env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, explicit_layer_name, &extension_count, nullptr));
    ASSERT_GE(extension_count, 1);
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, explicit_layer_name,
                                                                                     &extension_count, extension_props.data()));

    // Make sure only the one extension implemented by the enabled implicit layer is present.
    bool found = true;
    for (uint32_t ext = 0; ext < extension_count; ++ext) {
        if (!strcmp(extension_props[ext].extensionName, VK_KHR_MAINTENANCE1_EXTENSION_NAME)) {
            found = true;
        }
        ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME));
    }
    ASSERT_EQ(true, found);

    DeviceWrapper dev{inst};
    dev.create_info.add_extension(VK_KHR_MAINTENANCE1_EXTENSION_NAME).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(phys_dev);

    // Make sure only the appropriate function pointers are NULL as well
    ASSERT_NE(nullptr, dev->vkGetDeviceProcAddr(dev.dev, "vkTrimCommandPoolKHR"));
    ASSERT_EQ(nullptr, dev->vkGetDeviceProcAddr(dev.dev, "vkGetSwapchainStatusKHR"));
}

TEST_F(LayerExtensions, ExplicitPresentImageDeviceExtension) {
    auto& driver = env->get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* explicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    env->add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}
                .set_name(explicit_layer_name)
                .set_lib_path(TEST_LAYER_WRAP_OBJECTS_2)
                .set_api_version(VK_MAKE_API_VERSION(0, 1, 0, 0))
                .add_device_extension({VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME, 1, {"vkGetSwapchainStatusKHR"}})),
        "explicit_wrap_layer_pres.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1);

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_layer(explicit_layer_name);
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count, nullptr));
    if (extension_count > 0) {
        extension_props.resize(extension_count);
        ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count,
                                                                                         extension_props.data()));

        // Make sure the extensions are not present
        for (uint32_t ext = 0; ext < extension_count; ++ext) {
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_KHR_MAINTENANCE1_EXTENSION_NAME));
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME));
        }
    }

    // Now query by layer name.
    extension_count = 0;
    extension_props.clear();
    ASSERT_EQ(VK_SUCCESS,
              env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, explicit_layer_name, &extension_count, nullptr));
    ASSERT_GE(extension_count, 1);
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, explicit_layer_name,
                                                                                     &extension_count, extension_props.data()));

    // Make sure only the one extension implemented by the enabled implicit layer is present.
    bool found = false;
    for (uint32_t ext = 0; ext < extension_count; ++ext) {
        ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_KHR_MAINTENANCE1_EXTENSION_NAME));
        if (!strcmp(extension_props[ext].extensionName, VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME)) {
            found = true;
        }
    }
    ASSERT_EQ(true, found);

    DeviceWrapper dev{inst};
    dev.create_info.add_extension(VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME)
        .add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(phys_dev);

    // Make sure only the appropriate function pointers are NULL as well
    ASSERT_EQ(nullptr, dev->vkGetDeviceProcAddr(dev.dev, "vkTrimCommandPoolKHR"));
    ASSERT_NE(nullptr, dev->vkGetDeviceProcAddr(dev.dev, "vkGetSwapchainStatusKHR"));
}

TEST_F(LayerExtensions, ExplicitBothDeviceExtensions) {
    auto& driver = env->get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* explicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    env->add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}
                .set_name(explicit_layer_name)
                .set_lib_path(TEST_LAYER_WRAP_OBJECTS_3)
                .set_api_version(VK_MAKE_API_VERSION(0, 1, 0, 0))
                .add_device_extension({VK_KHR_MAINTENANCE1_EXTENSION_NAME, 1, {"vkTrimCommandPoolKHR"}})
                .add_device_extension({VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME, 1, {"vkGetSwapchainStatusKHR"}})),
        "explicit_wrap_layer_both_dev.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1);

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_layer(explicit_layer_name);
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count, nullptr));
    if (extension_count > 0) {
        extension_props.resize(extension_count);
        ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count,
                                                                                         extension_props.data()));

        // Make sure the extensions are not present
        for (uint32_t ext = 0; ext < extension_count; ++ext) {
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_KHR_MAINTENANCE1_EXTENSION_NAME));
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME));
        }
    }

    // Now query by layer name.
    extension_count = 0;
    extension_props.clear();
    ASSERT_EQ(VK_SUCCESS,
              env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, explicit_layer_name, &extension_count, nullptr));
    ASSERT_GE(extension_count, 1);
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, explicit_layer_name,
                                                                                     &extension_count, extension_props.data()));

    // Make sure only the one extension implemented by the enabled implicit layer is present.
    bool found[2] = {false, false};
    for (uint32_t ext = 0; ext < extension_count; ++ext) {
        if (!strcmp(extension_props[ext].extensionName, VK_KHR_MAINTENANCE1_EXTENSION_NAME)) {
            found[0] = true;
        }
        if (!strcmp(extension_props[ext].extensionName, VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME)) {
            found[1] = true;
        }
    }
    for (uint32_t ext = 0; ext < 2; ++ext) {
        ASSERT_EQ(true, found[ext]);
    }

    DeviceWrapper dev{inst};
    dev.create_info.add_extension(VK_KHR_MAINTENANCE1_EXTENSION_NAME)
        .add_extension(VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME)
        .add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(phys_dev);

    // Make sure only the appropriate function pointers are NULL as well
    ASSERT_NE(nullptr, dev->vkGetDeviceProcAddr(dev.dev, "vkTrimCommandPoolKHR"));
    ASSERT_NE(nullptr, dev->vkGetDeviceProcAddr(dev.dev, "vkGetSwapchainStatusKHR"));
}
