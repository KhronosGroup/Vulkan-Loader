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

const char* lunarg_meta_layer_name = "VK_LAYER_LUNARG_override";

TEST(ImplicitLayers, WithEnableAndDisableEnvVar) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    const char* implicit_layer_name = "VK_LAYER_ImplicitTestLayer";
    const char* enable_env_var = "ENABLE_ME";
    const char* disable_env_var = "DISABLE_ME";

    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(implicit_layer_name)
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .set_disable_environment(disable_env_var)
                                                         .set_enable_environment(enable_env_var)),
                           "implicit_test_layer.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1U);

    // didn't set enable env-var, layer should not load
    CheckLogForLayerString(env, implicit_layer_name, false);

    // set enable env-var to 0, no layer should be found
    set_env_var(enable_env_var, "0");
    CheckLogForLayerString(env, implicit_layer_name, false);

    // // set enable env-var, layer should load
    set_env_var(enable_env_var, "1");
    CheckLogForLayerString(env, implicit_layer_name, true);

    remove_env_var(enable_env_var);

    // set disable env-var to 0, layer should not load
    set_env_var(disable_env_var, "0");
    CheckLogForLayerString(env, implicit_layer_name, false);

    // set disable env-var to 1, layer should not load
    set_env_var(disable_env_var, "1");
    CheckLogForLayerString(env, implicit_layer_name, false);

    // set both enable and disable env-var, layer should not load
    set_env_var(enable_env_var, "1");
    set_env_var(disable_env_var, "1");
    CheckLogForLayerString(env, implicit_layer_name, false);

    remove_env_var(enable_env_var);
    remove_env_var(disable_env_var);
}

TEST(ImplicitLayers, OnlyDisableEnvVar) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    const char* implicit_layer_name = "ImplicitTestLayer";
    const char* disable_env_var = "DISABLE_ME";

    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(implicit_layer_name)
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .set_disable_environment(disable_env_var)),
                           "implicit_test_layer.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1U);

    // don't set disable env-var, layer should load
    CheckLogForLayerString(env, implicit_layer_name, true);

    // set disable env-var to 0, layer should load
    set_env_var(disable_env_var, "0");
    CheckLogForLayerString(env, implicit_layer_name, false);

    // set disable env-var to 1, layer should not load
    set_env_var(disable_env_var, "1");
    CheckLogForLayerString(env, implicit_layer_name, false);

    {
        InstWrapper inst{env.vulkan_functions};
        FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
        inst.create_info.add_layer(implicit_layer_name);
        inst.CheckCreate(VK_SUCCESS);
        ASSERT_TRUE(env.debug_log.find(std::string("Insert instance layer ") + implicit_layer_name));
    }
    remove_env_var(disable_env_var);
}

TEST(ImplicitLayers, PreInstanceEnumInstLayerProps) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    const char* implicit_layer_name = "ImplicitTestLayer";
    const char* disable_env_var = "DISABLE_ME";

    env.add_implicit_layer(
        ManifestLayer{}
            .set_file_format_version(ManifestVersion(1, 1, 2))
            .add_layer(ManifestLayer::LayerDescription{}
                           .set_name(implicit_layer_name)
                           .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                           .set_disable_environment(disable_env_var)
                           .add_pre_instance_function(ManifestLayer::LayerDescription::FunctionOverride{}
                                                          .set_vk_func("vkEnumerateInstanceLayerProperties")
                                                          .set_override_name("test_preinst_vkEnumerateInstanceLayerProperties"))),
        "implicit_test_layer.json");

    uint32_t layer_props = 43;
    auto& layer = env.get_test_layer(0);
    layer.set_reported_layer_props(layer_props);

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, layer_props);

    // set disable env-var to 1, layer should not load
    set_env_var(disable_env_var, "1");

    count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_NE(count, 0U);
    ASSERT_NE(count, layer_props);

    remove_env_var(disable_env_var);
}

TEST(ImplicitLayers, PreInstanceEnumInstExtProps) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    const char* implicit_layer_name = "ImplicitTestLayer";
    const char* disable_env_var = "DISABLE_ME";

    env.add_implicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 1, 2))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(implicit_layer_name)
                                              .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                              .set_disable_environment(disable_env_var)
                                              .add_pre_instance_function(
                                                  ManifestLayer::LayerDescription::FunctionOverride{}
                                                      .set_vk_func("vkEnumerateInstanceExtensionProperties")
                                                      .set_override_name("test_preinst_vkEnumerateInstanceExtensionProperties"))),
                           "implicit_test_layer.json");

    uint32_t ext_props = 52;
    auto& layer = env.get_test_layer(0);
    layer.set_reported_extension_props(ext_props);

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));
    ASSERT_EQ(count, ext_props);

    // set disable env-var to 1, layer should not load
    set_env_var(disable_env_var, "1");

    count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));
    ASSERT_NE(count, 0U);
    ASSERT_NE(count, ext_props);

    remove_env_var(disable_env_var);
}

TEST(ImplicitLayers, PreInstanceVersion) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().physical_devices.push_back({});
    env.get_test_icd().icd_api_version = VK_MAKE_API_VERSION(0, 1, 2, 3);

    const char* implicit_layer_name = "ImplicitTestLayer";
    const char* disable_env_var = "DISABLE_ME";

    env.add_implicit_layer(
        ManifestLayer{}
            .set_file_format_version(ManifestVersion(1, 1, 2))
            .add_layer(ManifestLayer::LayerDescription{}
                           .set_name(implicit_layer_name)
                           .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                           .set_api_version(VK_MAKE_API_VERSION(0, 1, 2, 3))
                           .set_disable_environment(disable_env_var)
                           .add_pre_instance_function(ManifestLayer::LayerDescription::FunctionOverride{}
                                                          .set_vk_func("vkEnumerateInstanceVersion")
                                                          .set_override_name("test_preinst_vkEnumerateInstanceVersion"))),
        "implicit_test_layer.json");

    uint32_t layer_version = VK_MAKE_API_VERSION(1, 2, 3, 4);
    auto& layer = env.get_test_layer(0);
    layer.set_reported_instance_version(layer_version);

    uint32_t version = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceVersion(&version));
    ASSERT_EQ(version, layer_version);

    // set disable env-var to 1, layer should not load
    set_env_var(disable_env_var, "1");

    version = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceVersion(&version));
    ASSERT_NE(version, 0U);
    ASSERT_NE(version, layer_version);

    remove_env_var(disable_env_var);
}

// Run with a pre-Negotiate function version of the layer so that it has to query vkCreateInstance using the
// renamed vkGetInstanceProcAddr function which returns one that intentionally fails.  Then disable the
// layer and verify it works.  The non-override version of vkCreateInstance in the layer also works (and is
// tested through behavior above).
TEST(ImplicitLayers, OverrideGetInstanceProcAddr) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().physical_devices.push_back({});

    const char* implicit_layer_name = "ImplicitTestLayer";
    const char* disable_env_var = "DISABLE_ME";

    env.add_implicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 0, 0))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(implicit_layer_name)
                                              .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_1)
                                              .set_disable_environment(disable_env_var)
                                              .add_function(ManifestLayer::LayerDescription::FunctionOverride{}
                                                                .set_vk_func("vkGetInstanceProcAddr")
                                                                .set_override_name("test_override_vkGetInstanceProcAddr"))),
                           "implicit_test_layer.json");

    {
        InstWrapper inst1{env.vulkan_functions};
        inst1.CheckCreate(VK_ERROR_INVALID_SHADER_NV);
    }

    {
        // set disable env-var to 1, layer should not load
        set_env_var(disable_env_var, "1");
        InstWrapper inst2{env.vulkan_functions};
        inst2.CheckCreate();
    }

    remove_env_var(disable_env_var);
}

// Meta layer which contains component layers that do not exist.
TEST(MetaLayers, InvalidComponentLayer) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    const char* meta_layer_name = "VK_LAYER_MetaTestLayer";
    const char* invalid_layer_name_1 = "VK_LAYER_InvalidLayer1";
    const char* invalid_layer_name_2 = "VK_LAYER_InvalidLayer2";
    env.add_implicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 1, 2))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(meta_layer_name)
                                              .add_component_layers({invalid_layer_name_1, invalid_layer_name_2})
                                              .set_disable_environment("NotGonnaWork")
                                              .add_instance_extension({"NeverGonnaGiveYouUp"})
                                              .add_device_extension({"NeverGonnaLetYouDown"})),
                           "meta_test_layer.json");

    const char* regular_layer_name = "TestLayer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(regular_layer_name).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "regular_test_layer.json");

    // should find 1, the 'regular' layer
    uint32_t layer_count = 1;
    EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr));
    EXPECT_EQ(layer_count, 1U);

    VkLayerProperties layer_props;
    EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, &layer_props));
    EXPECT_EQ(layer_count, 1U);
    EXPECT_TRUE(string_eq(layer_props.layerName, regular_layer_name));

    uint32_t extension_count = 0;
    std::array<VkExtensionProperties, 3> extensions;
    EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    EXPECT_EQ(extension_count, 3U);  // return debug report & debug utils & portability enumeration

    EXPECT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data()));
    EXPECT_EQ(extension_count, 3U);

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer(meta_layer_name);
    FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
    inst.CheckCreate(VK_ERROR_LAYER_NOT_PRESENT);
    ASSERT_TRUE(env.debug_log.find(std::string("verify_meta_layer_component_layers: Meta-layer ") + meta_layer_name +
                                   " can't find component layer " + invalid_layer_name_1 + " at index 0.  Skipping this layer."));
}

// Meta layer that is an explicit layer
TEST(MetaLayers, ExplicitMetaLayer) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().add_physical_device({});
    const char* meta_layer_name = "VK_LAYER_MetaTestLayer";
    const char* regular_layer_name = "VK_LAYER_TestLayer";
    env.add_explicit_layer(
        ManifestLayer{}
            .set_file_format_version(ManifestVersion(1, 1, 2))
            .add_layer(ManifestLayer::LayerDescription{}.set_name(meta_layer_name).add_component_layers({regular_layer_name})),
        "meta_test_layer.json");

    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(regular_layer_name).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "regular_test_layer.json");
    {  // global functions
        // should find 1, the 'regular' layer
        uint32_t layer_count = 0;
        EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr));
        EXPECT_EQ(layer_count, 2U);

        std::array<VkLayerProperties, 2> layer_props;
        EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props.data()));
        EXPECT_EQ(layer_count, 2U);
        EXPECT_TRUE(check_permutation({regular_layer_name, meta_layer_name}, layer_props));

        uint32_t extension_count = 0;
        std::array<VkExtensionProperties, 3> extensions;
        EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
        EXPECT_EQ(extension_count, 3U);  // return debug report & debug utils & portability enumeration

        EXPECT_EQ(VK_SUCCESS,
                  env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data()));
        EXPECT_EQ(extension_count, 3U);
    }
    {  // don't enable the layer, shouldn't find any layers when calling vkEnumerateDeviceLayerProperties
        InstWrapper inst{env.vulkan_functions};
        inst.CheckCreate(VK_SUCCESS);
        auto phys_dev = inst.GetPhysDev();
        uint32_t count = 0;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, nullptr);
        ASSERT_EQ(count, 0U);
    }
    {
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.add_layer(meta_layer_name);
        inst.CheckCreate(VK_SUCCESS);
        auto phys_dev = inst.GetPhysDev();
        uint32_t count = 0;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, nullptr);
        ASSERT_EQ(count, 2U);
        std::array<VkLayerProperties, 2> layer_props;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, layer_props.data());
        ASSERT_EQ(count, 2U);
        EXPECT_TRUE(check_permutation({regular_layer_name, meta_layer_name}, layer_props));
    }
}

// Meta layer which adds itself in its list of component layers
TEST(MetaLayers, MetaLayerNameInComponentLayers) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    const char* meta_layer_name = "VK_LAYER_MetaTestLayer";
    const char* regular_layer_name = "VK_LAYER_TestLayer";
    env.add_implicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 1, 2))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(meta_layer_name)
                                              .add_component_layers({meta_layer_name, regular_layer_name})
                                              .set_disable_environment("NotGonnaWork")
                                              .add_instance_extension({"NeverGonnaGiveYouUp"})
                                              .add_device_extension({"NeverGonnaLetYouDown"})),
                           "meta_test_layer.json");

    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(regular_layer_name).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "regular_test_layer.json");

    // should find 1, the 'regular' layer
    uint32_t layer_count = 1;
    EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr));
    EXPECT_EQ(layer_count, 1U);

    VkLayerProperties layer_props;
    EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, &layer_props));
    EXPECT_EQ(layer_count, 1U);
    EXPECT_TRUE(string_eq(layer_props.layerName, regular_layer_name));

    uint32_t extension_count = 0;
    std::array<VkExtensionProperties, 3> extensions;
    EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    EXPECT_EQ(extension_count, 3U);  // return debug report & debug utils & portability enumeration

    EXPECT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data()));
    EXPECT_EQ(extension_count, 3U);

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer(meta_layer_name);
    FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
    inst.CheckCreate(VK_ERROR_LAYER_NOT_PRESENT);
    ASSERT_TRUE(env.debug_log.find(std::string("verify_meta_layer_component_layers: Meta-layer ") + meta_layer_name +
                                   " lists itself in its component layer " + "list at index 0.  Skipping this layer."));
}

// Meta layer which adds another meta layer as a component layer
TEST(MetaLayers, MetaLayerWhichAddsMetaLayer) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    const char* meta_layer_name = "VK_LAYER_MetaTestLayer";
    const char* meta_meta_layer_name = "VK_LAYER_MetaMetaTestLayer";
    const char* regular_layer_name = "VK_LAYER_TestLayer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(regular_layer_name).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "regular_test_layer.json");
    env.add_explicit_layer(
        ManifestLayer{}
            .set_file_format_version(ManifestVersion(1, 1, 2))
            .add_layer(ManifestLayer::LayerDescription{}.set_name(meta_layer_name).add_component_layers({regular_layer_name})),
        "meta_test_layer.json");
    env.add_explicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 1, 2))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(meta_meta_layer_name)
                                              .add_component_layers({meta_layer_name, regular_layer_name})),
                           "meta_meta_test_layer.json");

    uint32_t layer_count = 3;
    EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr));
    EXPECT_EQ(layer_count, 3U);

    std::array<VkLayerProperties, 3> layer_props;
    EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props.data()));
    EXPECT_EQ(layer_count, 3U);
    EXPECT_TRUE(check_permutation({regular_layer_name, meta_layer_name, meta_meta_layer_name}, layer_props));

    uint32_t extension_count = 0;
    std::array<VkExtensionProperties, 3> extensions;
    EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    EXPECT_EQ(extension_count, 3U);  // return debug report & debug utils & portability enumeration

    EXPECT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data()));
    EXPECT_EQ(extension_count, 3U);

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer(meta_layer_name);
    FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
    inst.CheckCreate();
    ASSERT_TRUE(env.debug_log.find(std::string("verify_meta_layer_component_layers: Adding meta-layer ") + meta_meta_layer_name +
                                   " which also contains meta-layer " + meta_layer_name));
}

TEST(MetaLayers, InstanceExtensionInComponentLayer) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().add_physical_device({});
    const char* meta_layer_name = "VK_LAYER_MetaTestLayer";
    const char* regular_layer_name = "VK_LAYER_TestLayer";
    const char* instance_ext_name = "VK_EXT_headless_surface";
    env.add_explicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(regular_layer_name)
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .add_instance_extension({instance_ext_name})),
                           "regular_test_layer.json");
    env.add_explicit_layer(
        ManifestLayer{}
            .set_file_format_version(ManifestVersion(1, 1, 2))
            .add_layer(ManifestLayer::LayerDescription{}.set_name(meta_layer_name).add_component_layers({regular_layer_name})),
        "meta_test_layer.json");

    uint32_t extension_count = 0;
    std::array<VkExtensionProperties, 1> extensions;
    EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(meta_layer_name, &extension_count, nullptr));
    EXPECT_EQ(extension_count, 1U);  // return instance_ext_name

    EXPECT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumerateInstanceExtensionProperties(meta_layer_name, &extension_count, extensions.data()));
    EXPECT_EQ(extension_count, 1U);
    EXPECT_TRUE(string_eq(extensions[0].extensionName, instance_ext_name));
}

TEST(MetaLayers, DeviceExtensionInComponentLayer) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().add_physical_device({});
    const char* meta_layer_name = "VK_LAYER_MetaTestLayer";
    const char* regular_layer_name = "VK_LAYER_TestLayer";
    const char* device_ext_name = "VK_EXT_fake_dev_ext";
    env.add_explicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(regular_layer_name)
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .add_device_extension({device_ext_name})),
                           "regular_test_layer.json");
    env.add_explicit_layer(
        ManifestLayer{}
            .set_file_format_version(ManifestVersion(1, 1, 2))
            .add_layer(ManifestLayer::LayerDescription{}.set_name(meta_layer_name).add_component_layers({regular_layer_name})),
        "meta_test_layer.json");
    {
        uint32_t extension_count = 0;
        EXPECT_EQ(VK_SUCCESS,
                  env.vulkan_functions.vkEnumerateInstanceExtensionProperties(meta_layer_name, &extension_count, nullptr));
        EXPECT_EQ(extension_count, 0U);
    }
    {  // layer is not enabled
        InstWrapper inst{env.vulkan_functions};
        FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
        inst.CheckCreate();
        ASSERT_TRUE(env.debug_log.find(std::string("Meta-layer ") + meta_layer_name + " component layer " + regular_layer_name +
                                       " adding device extension " + device_ext_name));

        auto phys_dev = inst.GetPhysDev();
        uint32_t extension_count = 0;
        std::array<VkExtensionProperties, 1> extensions;
        EXPECT_EQ(VK_SUCCESS,
                  env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, meta_layer_name, &extension_count, nullptr));
        EXPECT_EQ(extension_count, 1U);  // return device_ext_name

        EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, meta_layer_name, &extension_count,
                                                                                        extensions.data()));
        EXPECT_EQ(extension_count, 1U);
        EXPECT_TRUE(string_eq(extensions[0].extensionName, device_ext_name));
    }
    {  // layer is enabled
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.add_layer(meta_layer_name);
        FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
        inst.CheckCreate();
        ASSERT_TRUE(env.debug_log.find(std::string("Meta-layer ") + meta_layer_name + " component layer " + regular_layer_name +
                                       " adding device extension " + device_ext_name));
        auto phys_dev = inst.GetPhysDev();

        uint32_t extension_count = 0;
        std::array<VkExtensionProperties, 1> extensions;
        EXPECT_EQ(VK_SUCCESS,
                  env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, meta_layer_name, &extension_count, nullptr));
        EXPECT_EQ(extension_count, 1U);  // return device_ext_name

        EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, meta_layer_name, &extension_count,
                                                                                        extensions.data()));
        EXPECT_EQ(extension_count, 1U);
        EXPECT_TRUE(string_eq(extensions[0].extensionName, device_ext_name));
    }
}
// Override meta layer missing disable environment variable still enables the layer
TEST(OverrideMetaLayer, InvalidDisableEnvironment) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().add_physical_device({});
    const char* regular_layer_name = "VK_LAYER_TestLayer";
    env.add_explicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(regular_layer_name)
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))
                                                         .add_device_extension({"NeverGonnaLetYouDown"})),
                           "regular_test_layer.json");

    env.add_implicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 1, 2))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(lunarg_meta_layer_name)
                                              .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))
                                              .add_component_layers({regular_layer_name})),
                           "meta_test_layer.json");

    uint32_t layer_count = 0;
    EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr));
    EXPECT_EQ(layer_count, 1U);

    std::array<VkLayerProperties, 1> layer_props;
    EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props.data()));
    EXPECT_EQ(layer_count, 1U);
    EXPECT_TRUE(string_eq(layer_props[0].layerName, regular_layer_name));

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();
}

// Override meta layer whose version is less than the api version of the instance
TEST(OverrideMetaLayer, OlderVersionThanInstance) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().add_physical_device({});
    const char* regular_layer_name = "VK_LAYER_TestLayer";
    env.add_explicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(regular_layer_name)
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))
                                                         .add_device_extension({"NeverGonnaLetYouDown"})),
                           "regular_test_layer.json");

    env.add_implicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 1, 2))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(lunarg_meta_layer_name)
                                              .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))
                                              .set_disable_environment("DisableMeIfYouCan")
                                              .add_component_layers({regular_layer_name})),
                           "meta_test_layer.json");
    {  // global functions
        uint32_t layer_count = 0;
        EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr));
        EXPECT_EQ(layer_count, 2U);

        std::array<VkLayerProperties, 2> layer_props;
        EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props.data()));
        EXPECT_EQ(layer_count, 2U);
        EXPECT_TRUE(check_permutation({regular_layer_name, lunarg_meta_layer_name}, layer_props));
    }
    {  // 1.1 instance
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.api_version = VK_API_VERSION_1_1;
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();

        uint32_t count = 0;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, nullptr);
        ASSERT_EQ(2U, count);
        std::array<VkLayerProperties, 2> layer_props;

        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, layer_props.data());
        ASSERT_EQ(2U, count);
        ASSERT_TRUE(string_eq(layer_props[0].layerName, regular_layer_name));
        ASSERT_TRUE(string_eq(layer_props[1].layerName, lunarg_meta_layer_name));
    }
    {  // 1.2 instance

        InstWrapper inst{env.vulkan_functions};
        FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);

        inst.create_info.api_version = VK_API_VERSION_1_2;
        inst.CheckCreate();
        ASSERT_TRUE(env.debug_log.find(std::string("loader_add_implicit_layer: Disabling implicit layer ") +
                                       lunarg_meta_layer_name +
                                       " for using an old API version 1.1 versus application requested 1.2"));
    }
}

TEST(OverrideMetaLayer, OlderMetaLayerWithNewerInstanceVersion) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().add_physical_device({});

    const char* regular_layer_name = "VK_LAYER_TestLayer";
    env.add_explicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 2, 0))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(regular_layer_name)
                                              .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                              .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))),
                           "regular_test_layer.json");

    env.add_implicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 2, 0))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(lunarg_meta_layer_name)
                                              .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))
                                              .add_component_layers({regular_layer_name})
                                              .set_disable_environment("DisableMeIfYouCan")),
                           "meta_test_layer.json");
    {  // global functions
        uint32_t layer_count = 2;
        EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr));
        EXPECT_EQ(layer_count, 2U);

        std::array<VkLayerProperties, 2> layer_props;
        EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props.data()));
        EXPECT_EQ(layer_count, 2U);
        EXPECT_TRUE(check_permutation({regular_layer_name, lunarg_meta_layer_name}, layer_props));
    }
    {
        // 1.1 instance
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(1, 1, 0);
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();

        uint32_t count = 0;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, nullptr);
        ASSERT_EQ(2U, count);
        std::array<VkLayerProperties, 2> layer_props;

        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, layer_props.data());
        ASSERT_EQ(2U, count);
        ASSERT_TRUE(string_eq(layer_props[0].layerName, regular_layer_name));
        ASSERT_TRUE(string_eq(layer_props[1].layerName, lunarg_meta_layer_name));
    }

    {
        // 1.2 instance
        InstWrapper inst{env.vulkan_functions};
        FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
        inst.create_info.set_api_version(1, 2, 0);
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();
        ASSERT_TRUE(
            env.debug_log.find("loader_add_implicit_layer: Disabling implicit layer VK_LAYER_LUNARG_override for using an old API "
                               "version 1.1 versus application requested 1.2"));
        uint32_t count = 0;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, nullptr);
        ASSERT_EQ(0U, count);
        std::array<VkLayerProperties, 2> layer_props;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, layer_props.data());
        ASSERT_EQ(0U, count);
    }
}

TEST(OverrideMetaLayer, NewerComponentLayerInMetaLayer) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().add_physical_device({});

    const char* regular_layer_name = "VK_LAYER_TestLayer";
    env.add_explicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 2, 0))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(regular_layer_name)
                                              .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                              .set_api_version(VK_MAKE_API_VERSION(0, 1, 2, 0))),
                           "regular_test_layer.json");

    env.add_implicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 2, 0))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(lunarg_meta_layer_name)
                                              .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))
                                              .add_component_layers({regular_layer_name})
                                              .set_disable_environment("DisableMeIfYouCan")),
                           "meta_test_layer.json");
    {  // global functions
        uint32_t layer_count = 0;
        EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr));
        EXPECT_EQ(layer_count, 2U);

        std::array<VkLayerProperties, 2> layer_props;
        EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props.data()));
        EXPECT_EQ(layer_count, 2U);
        // Expect the explicit layer to still be found
        EXPECT_TRUE(check_permutation({regular_layer_name, lunarg_meta_layer_name}, layer_props));
    }
    {
        // 1.1 instance
        InstWrapper inst{env.vulkan_functions};
        FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
        inst.create_info.set_api_version(1, 1, 0);
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();
        // Newer component is allowed now
        EXPECT_TRUE(env.debug_log.find(std::string("Insert instance layer ") + regular_layer_name));
        env.debug_log.clear();
        uint32_t count = 0;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, nullptr);
        EXPECT_EQ(2U, count);
        std::array<VkLayerProperties, 2> layer_props;

        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, layer_props.data());
        EXPECT_EQ(2U, count);
        EXPECT_TRUE(check_permutation({regular_layer_name, lunarg_meta_layer_name}, layer_props));
    }

    {
        // 1.2 instance
        InstWrapper inst{env.vulkan_functions};
        FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
        inst.create_info.set_api_version(1, 2, 0);
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();
        // Meta layer should be disabled since its less than the apiVersion of the instance
        EXPECT_TRUE(
            env.debug_log.find("loader_add_implicit_layer: Disabling implicit layer VK_LAYER_LUNARG_override for using an old API "
                               "version 1.1 versus application requested 1.2"));
        env.debug_log.clear();
        uint32_t count = 0;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, nullptr);
        EXPECT_EQ(0U, count);
        std::array<VkLayerProperties, 2> layer_props;
        count = 2;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, layer_props.data());
        EXPECT_EQ(0U, count);
    }
}

TEST(OverrideMetaLayer, OlderComponentLayerInMetaLayer) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().add_physical_device({});

    const char* regular_layer_name = "VK_LAYER_TestLayer";
    env.add_explicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 2, 0))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(regular_layer_name)
                                              .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                              .set_api_version(VK_MAKE_API_VERSION(0, 1, 0, 0))),
                           "regular_test_layer.json");

    env.add_implicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 2, 0))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(lunarg_meta_layer_name)
                                              .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))
                                              .add_component_layers({regular_layer_name})
                                              .set_disable_environment("DisableMeIfYouCan")),
                           "meta_test_layer.json");
    {  // global functions
        uint32_t layer_count = 0;
        EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr));
        EXPECT_EQ(layer_count, 1U);

        std::array<VkLayerProperties, 1> layer_props;
        EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props.data()));
        EXPECT_EQ(layer_count, 1U);
        EXPECT_TRUE(string_eq(layer_props[0].layerName, regular_layer_name));
    }
    {
        // 1.1 instance
        InstWrapper inst{env.vulkan_functions};
        FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
        inst.create_info.set_api_version(1, 1, 0);
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();
        EXPECT_TRUE(
            env.debug_log.find("verify_meta_layer_component_layers: Meta-layer uses API version 1.1, but component layer 0 has API "
                               "version 1.0 that is lower.  Skipping this layer."));
        env.debug_log.clear();
        uint32_t count = 0;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, nullptr);
        ASSERT_EQ(0U, count);
        std::array<VkLayerProperties, 2> layer_props;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, layer_props.data());
        ASSERT_EQ(0U, count);
    }

    {
        // 1.2 instance
        InstWrapper inst{env.vulkan_functions};
        FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
        inst.create_info.set_api_version(1, 2, 0);
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();
        ASSERT_TRUE(
            env.debug_log.find("verify_meta_layer_component_layers: Meta-layer uses API version 1.1, but component layer 0 has API "
                               "version 1.0 that is lower.  Skipping this layer."));
        env.debug_log.clear();
        uint32_t count = 0;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, nullptr);
        ASSERT_EQ(0U, count);
        std::array<VkLayerProperties, 2> layer_props;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, layer_props.data());
        ASSERT_EQ(0U, count);
    }
}

TEST(OverrideMetaLayer, ApplicationEnabledLayerInBlacklist) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().add_physical_device({});

    const char* automatic_regular_layer_name = "VK_LAYER_TestLayer_1";
    const char* manual_regular_layer_name = "VK_LAYER_TestLayer_2";
    env.add_explicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(automatic_regular_layer_name)
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))),
                           "regular_test_layer_1.json");
    env.add_explicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(manual_regular_layer_name)
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))),
                           "regular_test_layer_2.json");
    env.add_implicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 2, 0))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(lunarg_meta_layer_name)
                                              .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))
                                              .add_component_layer(automatic_regular_layer_name)
                                              .add_blacklisted_layer(manual_regular_layer_name)
                                              .set_disable_environment("DisableMeIfYouCan")),
                           "meta_test_layer.json");

    {  // enable the layer in the blacklist
        InstWrapper inst{env.vulkan_functions};
        FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
        inst.create_info.add_layer(manual_regular_layer_name);
        inst.CheckCreate(VK_ERROR_LAYER_NOT_PRESENT);
        ASSERT_TRUE(env.debug_log.find(std::string("loader_remove_layers_in_blacklist: Override layer is active and layer ") +
                                       manual_regular_layer_name +
                                       " is in the blacklist inside of it. Removing that layer from current layer list."));
        env.debug_log.clear();
    }
    {  // dont enable the layer in the blacklist
        InstWrapper inst{env.vulkan_functions};
        FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();
        ASSERT_TRUE(env.debug_log.find(std::string("loader_remove_layers_in_blacklist: Override layer is active and layer ") +
                                       manual_regular_layer_name +
                                       " is in the blacklist inside of it. Removing that layer from current layer list."));
        env.debug_log.clear();
        uint32_t count = 0;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, nullptr);
        ASSERT_EQ(2U, count);
        std::array<VkLayerProperties, 2> layer_props;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, layer_props.data());
        ASSERT_EQ(2U, count);
        ASSERT_TRUE(check_permutation({automatic_regular_layer_name, lunarg_meta_layer_name}, layer_props));
    }
}

TEST(OverrideMetaLayer, BasicOverridePaths) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().add_physical_device({});
    fs::FolderManager override_layer_folder{FRAMEWORK_BUILD_DIRECTORY, "override_layer_folder"};

    const char* regular_layer_name = "VK_LAYER_TestLayer_1";
    override_layer_folder.write_manifest("regular_test_layer.json",
                                         ManifestLayer{}
                                             .add_layer(ManifestLayer::LayerDescription{}
                                                            .set_name(regular_layer_name)
                                                            .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                            .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0)))
                                             .get_manifest_str());
    auto override_folder_location = fs::make_native(override_layer_folder.location().str());
    env.add_implicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 2, 0))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(lunarg_meta_layer_name)
                                              .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))
                                              .add_component_layer(regular_layer_name)
                                              .set_disable_environment("DisableMeIfYouCan")
                                              .add_override_path(override_folder_location)),
                           "meta_test_layer.json");

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(1, 1, 0);
    FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
    inst.CheckCreate();
    ASSERT_TRUE(env.debug_log.find(std::string("Insert instance layer ") + regular_layer_name));
    env.layers.clear();
}

TEST(OverrideMetaLayer, BasicOverridePathsIgnoreOtherLayers) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().add_physical_device({});
    fs::FolderManager override_layer_folder{FRAMEWORK_BUILD_DIRECTORY, "override_layer_folder"};

    const char* regular_layer_name = "VK_LAYER_TestLayer";
    env.add_explicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 2, 0))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(regular_layer_name)
                                              .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                              .set_api_version(VK_MAKE_API_VERSION(0, 1, 0, 0))),
                           "regular_test_layer.json");

    const char* special_layer_name = "VK_LAYER_TestLayer_1";
    override_layer_folder.write_manifest("regular_test_layer.json",
                                         ManifestLayer{}
                                             .add_layer(ManifestLayer::LayerDescription{}
                                                            .set_name(special_layer_name)
                                                            .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                            .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0)))
                                             .get_manifest_str());
    auto override_folder_location = fs::make_native(override_layer_folder.location().str());
    env.add_implicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 2, 0))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(lunarg_meta_layer_name)
                                              .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))
                                              .add_component_layer(special_layer_name)
                                              .set_disable_environment("DisableMeIfYouCan")
                                              .add_override_path(override_folder_location)),
                           "meta_test_layer.json");

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(1, 1, 0);
    inst.create_info.add_layer(regular_layer_name);
    FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
    inst.CheckCreate(VK_ERROR_LAYER_NOT_PRESENT);
    ASSERT_FALSE(env.debug_log.find(std::string("Insert instance layer ") + regular_layer_name));
    env.layers.clear();
}

TEST(OverrideMetaLayer, OverridePathsInteractionWithVK_LAYER_PATH) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().add_physical_device({});

    // add explicit layer to VK_LAYER_PATH folder
    const char* env_var_layer_name = "VK_LAYER_env_var_set_path";
    env.add_explicit_layer(TestLayerDetails{ManifestLayer{}
                                                .set_file_format_version(ManifestVersion(1, 2, 0))
                                                .add_layer(ManifestLayer::LayerDescription{}
                                                               .set_name(env_var_layer_name)
                                                               .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                               .set_api_version(VK_MAKE_API_VERSION(0, 1, 0, 0))),
                                            "regular_test_layer.json"}
                               .set_discovery_type(ManifestDiscoveryType::env_var));

    // add layer to regular explicit layer folder
    const char* regular_layer_name = "VK_LAYER_regular_layer_path";
    env.add_explicit_layer(TestLayerDetails{ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                                          .set_name(regular_layer_name)
                                                                          .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                                          .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))),
                                            "regular_test_layer.json"}
                               .set_discovery_type(ManifestDiscoveryType::override_folder));

    env.add_implicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 2, 0))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(lunarg_meta_layer_name)
                                              .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))
                                              .add_component_layer(regular_layer_name)
                                              .set_disable_environment("DisableMeIfYouCan")
                                              .add_override_path(env.override_layer_folder.location().str())),
                           "meta_test_layer.json");

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(1, 1, 0);
    inst.create_info.add_layer(env_var_layer_name);
    FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
    inst.CheckCreate(VK_ERROR_LAYER_NOT_PRESENT);
    ASSERT_FALSE(env.debug_log.find(std::string("Insert instance layer ") + env_var_layer_name));

    env.layers.clear();
    remove_env_var("VK_LAYER_PATH");
}

// Make sure that implicit layers not in the override paths aren't found by mistake
TEST(OverrideMetaLayer, OverridePathsEnableImplicitLayerInDefaultPaths) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().add_physical_device({});
    fs::FolderManager override_layer_folder{FRAMEWORK_BUILD_DIRECTORY, "override_layer_folder"};

    const char* implicit_layer_name = "VK_LAYER_ImplicitLayer";
    env.add_implicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 2, 0))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(implicit_layer_name)
                                              .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                              .set_api_version(VK_MAKE_API_VERSION(0, 1, 0, 0))),
                           "implicit_test_layer.json");

    const char* regular_layer_name = "VK_LAYER_TestLayer_1";
    override_layer_folder.write_manifest("regular_test_layer.json",
                                         ManifestLayer{}
                                             .add_layer(ManifestLayer::LayerDescription{}
                                                            .set_name(regular_layer_name)
                                                            .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                            .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0)))
                                             .get_manifest_str());
    auto override_folder_location = fs::make_native(override_layer_folder.location().str());
    env.add_implicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 2, 0))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(lunarg_meta_layer_name)
                                              .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))
                                              .add_component_layers({regular_layer_name, implicit_layer_name})
                                              .set_disable_environment("DisableMeIfYouCan")
                                              .add_override_path(override_folder_location)),
                           "meta_test_layer.json");

    InstWrapper inst{env.vulkan_functions};
    FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
    inst.create_info.set_api_version(1, 1, 0);
    inst.CheckCreate();
    ASSERT_FALSE(env.debug_log.find(std::string("Insert instance layer ") + implicit_layer_name));
    ASSERT_TRUE(
        env.debug_log.find("Removing meta-layer VK_LAYER_LUNARG_override from instance layer list since it appears invalid."));
    env.layers.clear();
}

TEST(OverrideMetaLayer, ManifestFileFormatVersionTooOld) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().add_physical_device({});
    fs::FolderManager override_layer_folder{FRAMEWORK_BUILD_DIRECTORY, "override_layer_folder"};

    const char* regular_layer_name = "VK_LAYER_TestLayer_1";
    override_layer_folder.write_manifest("regular_test_layer.json",
                                         ManifestLayer{}
                                             .add_layer(ManifestLayer::LayerDescription{}
                                                            .set_name(regular_layer_name)
                                                            .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                            .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0)))
                                             .get_manifest_str());
    auto override_folder_location = fs::make_native(override_layer_folder.location().str());
    env.add_implicit_layer(ManifestLayer{}
                               .set_file_format_version(ManifestVersion(1, 0, 0))
                               .add_layer(ManifestLayer::LayerDescription{}
                                              .set_name(lunarg_meta_layer_name)
                                              .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))
                                              .add_component_layer(regular_layer_name)
                                              .set_disable_environment("DisableMeIfYouCan")
                                              .add_override_path(override_folder_location)),
                           "meta_test_layer.json");

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(1, 1, 0);
    FillDebugUtilsCreateDetails(inst.create_info, env.debug_log);
    inst.CheckCreate();
    ASSERT_TRUE(env.debug_log.find(std::string("Insert instance layer ") + regular_layer_name));
    ASSERT_TRUE(env.debug_log.find("Indicating meta-layer-specific override paths, but using older JSON file version."));
    env.layers.clear();
}

// This test makes sure that any layer calling GetPhysicalDeviceProperties2 inside of CreateInstance
// succeeds and doesn't crash.
TEST(LayerCreateInstance, GetPhysicalDeviceProperties2) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().physical_devices.push_back({});
    env.get_test_icd().icd_api_version = VK_MAKE_API_VERSION(0, 1, 1, 0);

    const char* regular_layer_name = "TestLayer";
    env.add_explicit_layer(
        ManifestLayer{}
            .set_file_format_version(ManifestVersion(1, 1, 2))
            .add_layer(
                ManifestLayer::LayerDescription{}.set_name(regular_layer_name).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "regular_test_layer.json");

    auto& layer = env.get_test_layer(0);
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

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer(regular_layer_name).set_api_version(1, 1, 0);
    inst.CheckCreate();
}

TEST(LayerCreateInstance, GetPhysicalDeviceProperties2KHR) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.get_test_icd().physical_devices.push_back({});
    env.get_test_icd().add_instance_extension({"VK_KHR_get_physical_device_properties2", 0});

    const char* regular_layer_name = "VK_LAYER_TestLayer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(regular_layer_name).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "regular_test_layer.json");

    auto& layer = env.get_test_layer(0);
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

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer(regular_layer_name).add_extension("VK_KHR_get_physical_device_properties2");
    inst.CheckCreate();
}

TEST(ExplicitLayers, WrapObjects) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    auto& driver = env.get_test_icd();

    const char* wrap_objects_name = "VK_LAYER_LUNARG_wrap_objects";
    env.add_explicit_layer(ManifestLayer{}.add_layer(
                               ManifestLayer::LayerDescription{}.set_name(wrap_objects_name).set_lib_path(TEST_LAYER_WRAP_OBJECTS)),
                           "wrap_objects_layer.json");

    const char* regular_layer_name_1 = "RegularLayer1";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(regular_layer_name_1).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "regular_layer_1.json");

    const char* regular_layer_name_2 = "RegularLayer2";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(regular_layer_name_2).set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)),
        "regular_layer_2.json");

    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);
    {  // just the wrap layer
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.add_layer(wrap_objects_name);
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();

        DeviceWrapper dev{inst};
        dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
        dev.CheckCreate(phys_dev);
    }
    {  // wrap layer first
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.add_layer(wrap_objects_name).add_layer(regular_layer_name_1);
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();

        DeviceWrapper dev{inst};
        dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
        dev.CheckCreate(phys_dev);
    }
    {  // wrap layer last
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.add_layer(regular_layer_name_1).add_layer(wrap_objects_name);
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();

        DeviceWrapper dev{inst};
        dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
        dev.CheckCreate(phys_dev);
    }
    {  // wrap layer last
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.add_layer(regular_layer_name_1).add_layer(wrap_objects_name).add_layer(regular_layer_name_2);
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();

        DeviceWrapper dev{inst};
        dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
        dev.CheckCreate(phys_dev);
    }
}

TEST(LayerExtensions, ImplicitNoAdditionalInstanceExtension) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    auto& driver = env.get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* implicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    const char* enable_env_var = "ENABLE_ME";
    const char* disable_env_var = "DISABLE_ME";

    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(implicit_layer_name)
                                                         .set_lib_path(TEST_LAYER_WRAP_OBJECTS)
                                                         .set_disable_environment(disable_env_var)
                                                         .set_enable_environment(enable_env_var)),
                           "implicit_wrap_layer_no_ext.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1U);

    // set enable env-var, layer should load
    set_env_var(enable_env_var, "1");
    CheckLogForLayerString(env, implicit_layer_name, true);

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));

    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_props.data()));
    ASSERT_EQ(extension_count, 3U);  // debug_utils, debug_report, and portability enumeration

    // Make sure the extensions that are implemented only in the test layers is not present.
    ASSERT_FALSE(contains(extension_props, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME));
    ASSERT_FALSE(contains(extension_props, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME));

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    // Make sure all the function pointers are NULL as well
    handle_assert_null(env.vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkReleaseDisplayEXT"));
    handle_assert_null(env.vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));

    remove_env_var(enable_env_var);
}

TEST(LayerExtensions, ImplicitDirDispModeInstanceExtension) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    auto& driver = env.get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* implicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    const char* enable_env_var = "ENABLE_ME";
    const char* disable_env_var = "DISABLE_ME";

    env.add_implicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}
                .set_name(implicit_layer_name)
                .set_lib_path(TEST_LAYER_WRAP_OBJECTS_1)
                .set_disable_environment(disable_env_var)
                .set_enable_environment(enable_env_var)
                .add_instance_extension({VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME, 1, {"vkReleaseDisplayEXT"}})),
        "implicit_wrap_layer_dir_disp_mode.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1U);

    // // set enable env-var, layer should load
    set_env_var(enable_env_var, "1");
    CheckLogForLayerString(env, implicit_layer_name, true);

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    ASSERT_EQ(extension_count, 4U);  // the instance extension, debug_utils, debug_report, and portability enumeration
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_props.data()));
    ASSERT_EQ(extension_count, 4U);

    // Make sure the extensions that are implemented only in the test layers is not present.
    ASSERT_TRUE(contains(extension_props, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME));
    ASSERT_FALSE(contains(extension_props, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME));

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_extension(VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME);
    inst.CheckCreate();

    // Make sure only the appropriate function pointers are NULL as well
    handle_assert_has_value(env.vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkReleaseDisplayEXT"));
    handle_assert_null(env.vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));

    remove_env_var(enable_env_var);
}

TEST(LayerExtensions, ImplicitDispSurfCountInstanceExtension) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    auto& driver = env.get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* implicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    const char* enable_env_var = "ENABLE_ME";
    const char* disable_env_var = "DISABLE_ME";

    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(implicit_layer_name)
                                                         .set_lib_path(TEST_LAYER_WRAP_OBJECTS_2)
                                                         .set_disable_environment(disable_env_var)
                                                         .set_enable_environment(enable_env_var)
                                                         .add_instance_extension({VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME,
                                                                                  1,
                                                                                  {"vkGetPhysicalDeviceSurfaceCapabilities2EXT"}})),
                           "implicit_wrap_layer_disp_surf_count.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1U);

    // // set enable env-var, layer should load
    set_env_var(enable_env_var, "1");
    CheckLogForLayerString(env, implicit_layer_name, true);

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    ASSERT_EQ(extension_count, 4U);  // the instance extension, debug_utils, debug_report, and portability enumeration
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_props.data()));
    ASSERT_EQ(extension_count, 4U);

    // Make sure the extensions that are implemented only in the test layers is not present.
    ASSERT_FALSE(contains(extension_props, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME));
    ASSERT_TRUE(contains(extension_props, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME));

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_extension(VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME);
    inst.CheckCreate();

    // Make sure only the appropriate function pointers are NULL as well
    handle_assert_null(env.vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkReleaseDisplayEXT"));
    handle_assert_has_value(env.vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));

    remove_env_var(enable_env_var);
}

TEST(LayerExtensions, ImplicitBothInstanceExtensions) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    auto& driver = env.get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* implicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    const char* enable_env_var = "ENABLE_ME";
    const char* disable_env_var = "DISABLE_ME";

    env.add_implicit_layer(
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
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1U);

    // // set enable env-var, layer should load
    set_env_var(enable_env_var, "1");
    CheckLogForLayerString(env, implicit_layer_name, true);

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    ASSERT_EQ(extension_count, 5U);  // the two instance extension plus debug_utils, debug_report, portability enumeration
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_props.data()));
    ASSERT_EQ(extension_count, 5U);

    // Make sure the extensions that are implemented only in the test layers is not present.
    ASSERT_TRUE(contains(extension_props, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME));
    ASSERT_TRUE(contains(extension_props, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME));

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_extension(VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME)
        .add_extension(VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME);
    inst.CheckCreate();

    // Make sure only the appropriate function pointers are NULL as well
    handle_assert_has_value(env.vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkReleaseDisplayEXT"));
    handle_assert_has_value(env.vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));

    remove_env_var(enable_env_var);
}

TEST(LayerExtensions, ExplicitNoAdditionalInstanceExtension) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    auto& driver = env.get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* explicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(explicit_layer_name).set_lib_path(TEST_LAYER_WRAP_OBJECTS)),
        "explicit_wrap_layer_no_ext.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1U);

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    ASSERT_EQ(extension_count, 3U);  // debug utils, debug report, portability enumeration
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_props.data()));
    ASSERT_EQ(extension_count, 3U);

    // Make sure the extensions are not present
    for (const auto& ext : extension_props) {
        ASSERT_FALSE(string_eq(ext.extensionName, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME));
        ASSERT_FALSE(string_eq(ext.extensionName, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME));
    }

    // Now query by layer name.
    extension_count = 0;
    extension_props.clear();
    ASSERT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumerateInstanceExtensionProperties(explicit_layer_name, &extension_count, nullptr));
    ASSERT_EQ(extension_count, 0U);

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    // Make sure all the function pointers are NULL as well
    handle_assert_null(env.vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkReleaseDisplayEXT"));
    handle_assert_null(env.vulkan_functions.vkGetInstanceProcAddr(inst.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));
}

TEST(LayerExtensions, ExplicitDirDispModeInstanceExtension) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    auto& driver = env.get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* explicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}
                .set_name(explicit_layer_name)
                .set_lib_path(TEST_LAYER_WRAP_OBJECTS_1)
                .add_instance_extension({VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME, 1, {"vkReleaseDisplayEXT"}})),
        "explicit_wrap_layer_dir_disp_mode.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1U);

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    ASSERT_EQ(extension_count, 3U);  // debug utils, debug report, portability enumeration
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_props.data()));
    ASSERT_EQ(extension_count, 3U);
    // Make sure the extensions are not present
    for (const auto& ext : extension_props) {
        ASSERT_FALSE(string_eq(ext.extensionName, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME));
        ASSERT_FALSE(string_eq(ext.extensionName, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME));
    }

    // Now query by layer name.
    extension_count = 0;
    extension_props.clear();
    ASSERT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumerateInstanceExtensionProperties(explicit_layer_name, &extension_count, nullptr));
    ASSERT_EQ(extension_count, 1U);
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(explicit_layer_name, &extension_count,
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

    InstWrapper inst1{env.vulkan_functions};
    inst1.create_info.add_extension(VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME);
    inst1.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    // Make sure only the appropriate function pointers are NULL as well
    handle_assert_null(env.vulkan_functions.vkGetInstanceProcAddr(inst1.inst, "vkReleaseDisplayEXT"));
    handle_assert_null(env.vulkan_functions.vkGetInstanceProcAddr(inst1.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));

    InstWrapper inst2{env.vulkan_functions};
    inst2.create_info.add_layer(explicit_layer_name).add_extension(VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME);
    inst2.CheckCreate();

    // Make sure only the appropriate function pointers are NULL as well
    handle_assert_has_value(env.vulkan_functions.vkGetInstanceProcAddr(inst2.inst, "vkReleaseDisplayEXT"));
    handle_assert_null(env.vulkan_functions.vkGetInstanceProcAddr(inst2.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));
}

TEST(LayerExtensions, ExplicitDispSurfCountInstanceExtension) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    auto& driver = env.get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* explicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    env.add_explicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(explicit_layer_name)
                                                         .set_lib_path(TEST_LAYER_WRAP_OBJECTS_2)
                                                         .add_instance_extension({VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME,
                                                                                  1,
                                                                                  {"vkGetPhysicalDeviceSurfaceCapabilities2EXT"}})),
                           "explicit_wrap_layer_disp_surf_count.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1U);

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    if (extension_count > 0) {
        extension_props.resize(extension_count);
        ASSERT_EQ(VK_SUCCESS,
                  env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_props.data()));

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
              env.vulkan_functions.vkEnumerateInstanceExtensionProperties(explicit_layer_name, &extension_count, nullptr));
    ASSERT_EQ(extension_count, 1U);
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(explicit_layer_name, &extension_count,
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

    InstWrapper inst1{env.vulkan_functions};
    inst1.create_info.add_extension(VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME);
    inst1.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    // Make sure only the appropriate function pointers are NULL as well
    handle_assert_null(env.vulkan_functions.vkGetInstanceProcAddr(inst1.inst, "vkReleaseDisplayEXT"));
    handle_assert_null(env.vulkan_functions.vkGetInstanceProcAddr(inst1.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));

    InstWrapper inst2{env.vulkan_functions};
    inst2.create_info.add_layer(explicit_layer_name).add_extension(VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME);
    inst2.CheckCreate();

    // Make sure only the appropriate function pointers are NULL as well
    handle_assert_null(env.vulkan_functions.vkGetInstanceProcAddr(inst2.inst, "vkReleaseDisplayEXT"));
    handle_assert_has_value(env.vulkan_functions.vkGetInstanceProcAddr(inst2.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));
}

TEST(LayerExtensions, ExplicitBothInstanceExtensions) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    auto& driver = env.get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* explicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}
                .set_name(explicit_layer_name)
                .set_lib_path(TEST_LAYER_WRAP_OBJECTS_3)
                .add_instance_extension({VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME, 1, {"vkReleaseDisplayEXT"}})
                .add_instance_extension(
                    {VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME, 1, {"vkGetPhysicalDeviceSurfaceCapabilities2EXT"}})),
        "explicit_wrap_layer_both_inst.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1U);

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
    if (extension_count > 0) {
        extension_props.resize(extension_count);
        ASSERT_EQ(VK_SUCCESS,
                  env.vulkan_functions.vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_props.data()));

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
              env.vulkan_functions.vkEnumerateInstanceExtensionProperties(explicit_layer_name, &extension_count, nullptr));
    ASSERT_EQ(extension_count, 2U);
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceExtensionProperties(explicit_layer_name, &extension_count,
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

    InstWrapper inst1{env.vulkan_functions};
    inst1.create_info.add_extension(VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME)
        .add_extension(VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME);
    inst1.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    // Make sure only the appropriate function pointers are NULL as well
    handle_assert_null(env.vulkan_functions.vkGetInstanceProcAddr(inst1.inst, "vkReleaseDisplayEXT"));
    handle_assert_null(env.vulkan_functions.vkGetInstanceProcAddr(inst1.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));

    InstWrapper inst2{env.vulkan_functions};
    inst2.create_info.add_layer(explicit_layer_name)
        .add_extension(VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME)
        .add_extension(VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME);
    inst2.CheckCreate();
    VkPhysicalDevice phys_dev = inst2.GetPhysDev();

    // Make sure only the appropriate function pointers are NULL as well
    handle_assert_has_value(env.vulkan_functions.vkGetInstanceProcAddr(inst2.inst, "vkReleaseDisplayEXT"));
    PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT pfnGetPhysicalDeviceSurfaceCapabilities2EXT =
        reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT>(
            env.vulkan_functions.vkGetInstanceProcAddr(inst2.inst, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));
    handle_assert_has_value(pfnGetPhysicalDeviceSurfaceCapabilities2EXT);

    VkSurfaceCapabilities2EXT surf_caps{VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_EXT};

    // Call and then check a few things
    ASSERT_EQ(VK_SUCCESS, pfnGetPhysicalDeviceSurfaceCapabilities2EXT(phys_dev, VK_NULL_HANDLE, &surf_caps));
    ASSERT_EQ(7U, surf_caps.minImageCount);
    ASSERT_EQ(12U, surf_caps.maxImageCount);
    ASSERT_EQ(365U, surf_caps.maxImageArrayLayers);
}

TEST(LayerExtensions, ImplicitNoAdditionalDeviceExtension) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    auto& driver = env.get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* implicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    const char* enable_env_var = "ENABLE_ME";
    const char* disable_env_var = "DISABLE_ME";

    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(implicit_layer_name)
                                                         .set_lib_path(TEST_LAYER_WRAP_OBJECTS)
                                                         .set_disable_environment(disable_env_var)
                                                         .set_enable_environment(enable_env_var)),
                           "implicit_wrap_layer_no_ext.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1U);

    // // set enable env-var, layer should load
    set_env_var(enable_env_var, "1");
    CheckLogForLayerString(env, implicit_layer_name, true);

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count, nullptr));
    if (extension_count > 0) {
        extension_props.resize(extension_count);
        ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count,
                                                                                        extension_props.data()));

        // Make sure the extensions that are implemented only in the test layers is not present.
        for (uint32_t ext = 0; ext < extension_count; ++ext) {
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_KHR_MAINTENANCE1_EXTENSION_NAME));
            ASSERT_NE(0, strcmp(extension_props[ext].extensionName, VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME));
        }
    }

    // Device functions queried using vkGetInstanceProcAddr should be non-NULL since this could be available for any attached
    // physical device.
    PFN_vkTrimCommandPoolKHR pfn_vkTrimCommandPool =
        reinterpret_cast<PFN_vkTrimCommandPoolKHR>(inst->vkGetInstanceProcAddr(inst.inst, "vkTrimCommandPoolKHR"));
    PFN_vkGetSwapchainStatusKHR pfn_vkGetSwapchainStatus =
        reinterpret_cast<PFN_vkGetSwapchainStatusKHR>(inst->vkGetInstanceProcAddr(inst.inst, "vkGetSwapchainStatusKHR"));
    PFN_vkSetDeviceMemoryPriorityEXT pfn_vkSetDeviceMemoryPriority =
        reinterpret_cast<PFN_vkSetDeviceMemoryPriorityEXT>(inst->vkGetInstanceProcAddr(inst.inst, "vkSetDeviceMemoryPriorityEXT"));
    handle_assert_has_value(pfn_vkTrimCommandPool);
    handle_assert_has_value(pfn_vkGetSwapchainStatus);
    handle_assert_has_value(pfn_vkSetDeviceMemoryPriority);

    DeviceWrapper dev{inst};
    dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(phys_dev);

    // Query again after create device to make sure the value returned by vkGetInstanceProcAddr did not change
    PFN_vkTrimCommandPoolKHR pfn_vkTrimCommandPool2 =
        reinterpret_cast<PFN_vkTrimCommandPoolKHR>(inst->vkGetInstanceProcAddr(inst.inst, "vkTrimCommandPoolKHR"));
    PFN_vkGetSwapchainStatusKHR pfn_vkGetSwapchainStatus2 =
        reinterpret_cast<PFN_vkGetSwapchainStatusKHR>(inst->vkGetInstanceProcAddr(inst.inst, "vkGetSwapchainStatusKHR"));
    PFN_vkSetDeviceMemoryPriorityEXT pfn_vkSetDeviceMemoryPriority2 =
        reinterpret_cast<PFN_vkSetDeviceMemoryPriorityEXT>(inst->vkGetInstanceProcAddr(inst.inst, "vkSetDeviceMemoryPriorityEXT"));
    ASSERT_EQ(pfn_vkTrimCommandPool, pfn_vkTrimCommandPool2);
    ASSERT_EQ(pfn_vkGetSwapchainStatus, pfn_vkGetSwapchainStatus2);
    ASSERT_EQ(pfn_vkSetDeviceMemoryPriority, pfn_vkSetDeviceMemoryPriority2);

    // Make sure all the function pointers returned by vkGetDeviceProcAddr for non-enabled extensions are NULL
    handle_assert_null(dev->vkGetDeviceProcAddr(dev.dev, "vkTrimCommandPoolKHR"));
    handle_assert_null(dev->vkGetDeviceProcAddr(dev.dev, "vkGetSwapchainStatusKHR"));
    handle_assert_null(dev->vkGetDeviceProcAddr(dev.dev, "vkSetDeviceMemoryPriorityEXT"));

    // Even though the instance functions returned are non-NULL.  They should not work if we haven't enabled the extensions.
    ASSERT_DEATH(pfn_vkTrimCommandPool(dev.dev, VK_NULL_HANDLE, 0), "");
    ASSERT_DEATH(pfn_vkGetSwapchainStatus(dev.dev, VK_NULL_HANDLE), "");
    ASSERT_DEATH(pfn_vkSetDeviceMemoryPriority(dev.dev, VK_NULL_HANDLE, 0.f), "");

    remove_env_var(enable_env_var);
}

TEST(LayerExtensions, ImplicitMaintenanceDeviceExtension) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    auto& driver = env.get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* implicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    const char* enable_env_var = "ENABLE_ME";
    const char* disable_env_var = "DISABLE_ME";

    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(implicit_layer_name)
                                                         .set_lib_path(TEST_LAYER_WRAP_OBJECTS_1)
                                                         .set_disable_environment(disable_env_var)
                                                         .set_enable_environment(enable_env_var)),
                           "implicit_wrap_layer_maint.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1U);

    // // set enable env-var, layer should load
    set_env_var(enable_env_var, "1");
    CheckLogForLayerString(env, implicit_layer_name, true);

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count, nullptr));
    ASSERT_EQ(extension_count, 1U);
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count,
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
    handle_assert_has_value(dev->vkGetDeviceProcAddr(dev.dev, "vkTrimCommandPoolKHR"));
    handle_assert_null(dev->vkGetDeviceProcAddr(dev.dev, "vkGetSwapchainStatusKHR"));

    remove_env_var(enable_env_var);
}

TEST(LayerExtensions, ImplicitPresentImageDeviceExtension) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    auto& driver = env.get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* implicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    const char* enable_env_var = "ENABLE_ME";
    const char* disable_env_var = "DISABLE_ME";

    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(implicit_layer_name)
                                                         .set_lib_path(TEST_LAYER_WRAP_OBJECTS_2)
                                                         .set_disable_environment(disable_env_var)
                                                         .set_enable_environment(enable_env_var)),
                           "implicit_wrap_layer_pres.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1U);

    // // set enable env-var, layer should load
    set_env_var(enable_env_var, "1");
    CheckLogForLayerString(env, implicit_layer_name, true);

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count, nullptr));
    ASSERT_EQ(extension_count, 1U);
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count,
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
    handle_assert_null(dev->vkGetDeviceProcAddr(dev.dev, "vkTrimCommandPoolKHR"));
    handle_assert_has_value(dev->vkGetDeviceProcAddr(dev.dev, "vkGetSwapchainStatusKHR"));

    remove_env_var(enable_env_var);
}

TEST(LayerExtensions, ImplicitBothDeviceExtensions) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    auto& driver = env.get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* implicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    const char* enable_env_var = "ENABLE_ME";
    const char* disable_env_var = "DISABLE_ME";

    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(implicit_layer_name)
                                                         .set_lib_path(TEST_LAYER_WRAP_OBJECTS_3)
                                                         .set_disable_environment(disable_env_var)
                                                         .set_enable_environment(enable_env_var)),
                           "implicit_wrap_layer_both_dev.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1U);

    // // set enable env-var, layer should load
    set_env_var(enable_env_var, "1");
    CheckLogForLayerString(env, implicit_layer_name, true);

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count, nullptr));
    ASSERT_EQ(extension_count, 2U);
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count,
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
    handle_assert_has_value(dev->vkGetDeviceProcAddr(dev.dev, "vkTrimCommandPoolKHR"));
    handle_assert_has_value(dev->vkGetDeviceProcAddr(dev.dev, "vkGetSwapchainStatusKHR"));

    remove_env_var(enable_env_var);
}

TEST(LayerExtensions, ExplicitNoAdditionalDeviceExtension) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    auto& driver = env.get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* explicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(explicit_layer_name).set_lib_path(TEST_LAYER_WRAP_OBJECTS)),
        "explicit_wrap_layer_no_ext.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1U);

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer(explicit_layer_name);
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count, nullptr));
    if (extension_count > 0) {
        extension_props.resize(extension_count);
        ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count,
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
              env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, explicit_layer_name, &extension_count, nullptr));
    if (extension_count > 0) {
        extension_props.resize(extension_count);
        ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, explicit_layer_name,
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
    handle_assert_null(dev->vkGetDeviceProcAddr(dev.dev, "vkTrimCommandPoolKHR"));
    handle_assert_null(dev->vkGetDeviceProcAddr(dev.dev, "vkGetSwapchainStatusKHR"));
    handle_assert_null(dev->vkGetDeviceProcAddr(dev.dev, "vkSetDeviceMemoryPriorityEXT"));
}

TEST(LayerExtensions, ExplicitMaintenanceDeviceExtension) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    auto& driver = env.get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* explicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                      .set_name(explicit_layer_name)
                                      .set_lib_path(TEST_LAYER_WRAP_OBJECTS_1)
                                      .set_api_version(VK_MAKE_API_VERSION(0, 1, 0, 0))
                                      .add_device_extension({VK_KHR_MAINTENANCE1_EXTENSION_NAME, 1, {"vkTrimCommandPoolKHR"}})),
        "explicit_wrap_layer_maint.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1U);

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer(explicit_layer_name);
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count, nullptr));
    if (extension_count > 0) {
        extension_props.resize(extension_count);
        ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count,
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
              env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, explicit_layer_name, &extension_count, nullptr));
    ASSERT_EQ(extension_count, 1U);
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, explicit_layer_name, &extension_count,
                                                                                    extension_props.data()));

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
    handle_assert_has_value(dev->vkGetDeviceProcAddr(dev.dev, "vkTrimCommandPoolKHR"));
    handle_assert_null(dev->vkGetDeviceProcAddr(dev.dev, "vkGetSwapchainStatusKHR"));
}

TEST(LayerExtensions, ExplicitPresentImageDeviceExtension) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    auto& driver = env.get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* explicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}
                .set_name(explicit_layer_name)
                .set_lib_path(TEST_LAYER_WRAP_OBJECTS_2)
                .set_api_version(VK_MAKE_API_VERSION(0, 1, 0, 0))
                .add_device_extension({VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME, 1, {"vkGetSwapchainStatusKHR"}})),
        "explicit_wrap_layer_pres.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1U);

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer(explicit_layer_name);
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count, nullptr));
    if (extension_count > 0) {
        extension_props.resize(extension_count);
        ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count,
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
              env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, explicit_layer_name, &extension_count, nullptr));
    ASSERT_EQ(extension_count, 1U);
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, explicit_layer_name, &extension_count,
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
    handle_assert_null(dev->vkGetDeviceProcAddr(dev.dev, "vkTrimCommandPoolKHR"));
    handle_assert_has_value(dev->vkGetDeviceProcAddr(dev.dev, "vkGetSwapchainStatusKHR"));
}

TEST(LayerExtensions, ExplicitBothDeviceExtensions) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    auto& driver = env.get_test_icd();
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    const char* explicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}
                .set_name(explicit_layer_name)
                .set_lib_path(TEST_LAYER_WRAP_OBJECTS_3)
                .set_api_version(VK_MAKE_API_VERSION(0, 1, 0, 0))
                .add_device_extension({VK_KHR_MAINTENANCE1_EXTENSION_NAME, 1, {"vkTrimCommandPoolKHR"}})
                .add_device_extension({VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME, 1, {"vkGetSwapchainStatusKHR"}})),
        "explicit_wrap_layer_both_dev.json");

    uint32_t count = 0;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&count, nullptr));
    ASSERT_EQ(count, 1U);

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer(explicit_layer_name);
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();
    handle_assert_has_value(phys_dev);

    uint32_t extension_count = 0;
    std::vector<VkExtensionProperties> extension_props;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count, nullptr));
    if (extension_count > 0) {
        extension_props.resize(extension_count);
        ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, nullptr, &extension_count,
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
              env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, explicit_layer_name, &extension_count, nullptr));
    ASSERT_EQ(extension_count, 2U);  // debug_utils, and debug_report
    extension_props.resize(extension_count);
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev, explicit_layer_name, &extension_count,
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
    handle_assert_has_value(dev->vkGetDeviceProcAddr(dev.dev, "vkTrimCommandPoolKHR"));

    PFN_vkGetSwapchainStatusKHR gipa_pfnGetSwapchainStatusKHR =
        reinterpret_cast<PFN_vkGetSwapchainStatusKHR>(inst->vkGetInstanceProcAddr(inst.inst, "vkGetSwapchainStatusKHR"));
    PFN_vkGetSwapchainStatusKHR gdpa_pfnGetSwapchainStatusKHR =
        reinterpret_cast<PFN_vkGetSwapchainStatusKHR>(dev->vkGetDeviceProcAddr(dev.dev, "vkGetSwapchainStatusKHR"));
    handle_assert_has_value(gipa_pfnGetSwapchainStatusKHR);
    handle_assert_has_value(gdpa_pfnGetSwapchainStatusKHR);

    // Make sure both versions (from vkGetInstanceProcAddr and vkGetDeviceProcAddr) return the same value.
    ASSERT_EQ(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, gipa_pfnGetSwapchainStatusKHR(dev.dev, VK_NULL_HANDLE));
    ASSERT_EQ(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, gdpa_pfnGetSwapchainStatusKHR(dev.dev, VK_NULL_HANDLE));
}

TEST(TestLayers, ExplicitlyEnableImplicitLayer) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA, VK_MAKE_API_VERSION(0, 1, 2, 0)));
    env.get_test_icd().icd_api_version = VK_MAKE_API_VERSION(0, 1, 2, 0);
    VkPhysicalDeviceProperties properties{};
    properties.apiVersion = VK_MAKE_API_VERSION(0, 1, 2, 0);
    env.get_test_icd().add_physical_device({});
    env.get_test_icd().physical_devices.back().set_properties(properties);

    const char* regular_layer_name = "VK_LAYER_TestLayer1";
    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(regular_layer_name)
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))
                                                         .set_disable_environment("DisableMeIfYouCan")),
                           "regular_test_layer.json");
    {  // 1.1 instance
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.add_layer(regular_layer_name);
        inst.create_info.set_api_version(1, 1, 0);
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();

        uint32_t count = 0;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, nullptr);
        EXPECT_EQ(1U, count);
        VkLayerProperties layer_props{};
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, &layer_props);
        EXPECT_EQ(1U, count);
        ASSERT_TRUE(string_eq(regular_layer_name, layer_props.layerName));
    }
    {  // 1.2 instance
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.add_layer(regular_layer_name);
        inst.create_info.set_api_version(1, 2, 0);
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();

        uint32_t count = 0;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, nullptr);
        ASSERT_EQ(1U, count);
        VkLayerProperties layer_props{};
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, &layer_props);
        ASSERT_EQ(1U, count);
    }
}

TEST(TestLayers, NewerInstanceVersionThanImplicitLayer) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA, VK_MAKE_API_VERSION(0, 1, 2, 0)));
    env.get_test_icd().icd_api_version = VK_MAKE_API_VERSION(0, 1, 2, 0);
    VkPhysicalDeviceProperties properties{};
    properties.apiVersion = VK_MAKE_API_VERSION(0, 1, 2, 0);
    env.get_test_icd().add_physical_device({});
    env.get_test_icd().physical_devices.back().set_properties(properties);

    const char* regular_layer_name = "VK_LAYER_TestLayer1";
    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(regular_layer_name)
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .set_api_version(VK_MAKE_API_VERSION(0, 1, 1, 0))
                                                         .set_disable_environment("DisableMeIfYouCan")),
                           "regular_test_layer.json");
    {  // global functions
        uint32_t layer_count = 0;
        EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr));
        EXPECT_EQ(layer_count, 1U);

        std::array<VkLayerProperties, 1> layer_props;
        EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props.data()));
        EXPECT_EQ(layer_count, 1U);
        EXPECT_TRUE(string_eq(layer_props[0].layerName, regular_layer_name));
    }
    {  // 1.1 instance - should find the implicit layer
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(1, 1, 0);
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();

        uint32_t count = 0;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, nullptr);
        EXPECT_EQ(1U, count);
        VkLayerProperties layer_props{};
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, &layer_props);
        EXPECT_EQ(1U, count);
        ASSERT_TRUE(string_eq(regular_layer_name, layer_props.layerName));
    }
    {  // 1.2 instance -- instance layer shouldn't be found
        DebugUtilsLogger log;
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(1, 2, 0);
        FillDebugUtilsCreateDetails(inst.create_info, log);
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();

        ASSERT_TRUE(log.find(std::string("loader_add_implicit_layer: Disabling implicit layer ") + regular_layer_name +
                             " for using an old API version 1.1 versus application requested 1.2"));

        uint32_t count = 0;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, nullptr);
        ASSERT_EQ(0U, count);
        VkLayerProperties layer_props{};
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, &layer_props);
        ASSERT_EQ(0U, count);
    }
}

TEST(TestLayers, ImplicitLayerPre10APIVersion) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA, VK_MAKE_API_VERSION(0, 1, 2, 0)));
    env.get_test_icd().icd_api_version = VK_MAKE_API_VERSION(0, 1, 2, 0);
    VkPhysicalDeviceProperties properties{};
    properties.apiVersion = VK_MAKE_API_VERSION(0, 1, 2, 0);
    env.get_test_icd().add_physical_device({});
    env.get_test_icd().physical_devices.back().set_properties(properties);

    const char* regular_layer_name = "VK_LAYER_TestLayer1";
    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name(regular_layer_name)
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .set_api_version(VK_MAKE_API_VERSION(0, 0, 1, 0))
                                                         .set_disable_environment("DisableMeIfYouCan")),
                           "regular_test_layer.json");
    {  // global functions

        uint32_t layer_count = 0;
        EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, nullptr));
        EXPECT_EQ(layer_count, 1U);

        std::array<VkLayerProperties, 1> layer_props;
        EXPECT_EQ(VK_SUCCESS, env.vulkan_functions.vkEnumerateInstanceLayerProperties(&layer_count, layer_props.data()));
        EXPECT_EQ(layer_count, 1U);
        EXPECT_TRUE(string_eq(layer_props[0].layerName, regular_layer_name));
    }
    {  // 1.0 instance -- instance layer should be found
        DebugUtilsLogger log;
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(1, 0, 0);
        FillDebugUtilsCreateDetails(inst.create_info, log);
        inst.CheckCreate();
        ASSERT_TRUE(log.find(std::string("loader_add_implicit_layer: Disabling implicit layer ") + regular_layer_name +
                             " for using an old API version 0.1 versus application requested 1.0"));
        VkPhysicalDevice phys_dev = inst.GetPhysDev();

        uint32_t count = 0;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, nullptr);
        ASSERT_EQ(0U, count);
        VkLayerProperties layer_props{};
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, &layer_props);
        ASSERT_EQ(0U, count);
    }
    {  // 1.1 instance -- instance layer should be found
        DebugUtilsLogger log;
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(1, 1, 0);
        FillDebugUtilsCreateDetails(inst.create_info, log);
        inst.CheckCreate();
        ASSERT_TRUE(log.find(std::string("loader_add_implicit_layer: Disabling implicit layer ") + regular_layer_name +
                             " for using an old API version 0.1 versus application requested 1.1"));
        VkPhysicalDevice phys_dev = inst.GetPhysDev();
        uint32_t count = 0;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, nullptr);
        ASSERT_EQ(0U, count);
        VkLayerProperties layer_props{};
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, &layer_props);
        ASSERT_EQ(0U, count);
    }
    {  // 1.2 instance -- instance layer shouldn't be found
        DebugUtilsLogger log;
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_api_version(1, 2, 0);
        FillDebugUtilsCreateDetails(inst.create_info, log);
        inst.CheckCreate();
        VkPhysicalDevice phys_dev = inst.GetPhysDev();
        ASSERT_TRUE(log.find(std::string("loader_add_implicit_layer: Disabling implicit layer ") + regular_layer_name +
                             " for using an old API version 0.1 versus application requested 1.2"));

        uint32_t count = 0;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, nullptr);
        ASSERT_EQ(0U, count);
        VkLayerProperties layer_props{};
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, &layer_props);
        ASSERT_EQ(0U, count);
    }
    {  // application doesn't state its API version
        DebugUtilsLogger log;
        InstWrapper inst{env.vulkan_functions};
        inst.create_info.set_fill_in_application_info(false);
        FillDebugUtilsCreateDetails(inst.create_info, log);
        inst.CheckCreate();
        EXPECT_TRUE(log.find(std::string("loader_add_implicit_layer: Disabling implicit layer ") + regular_layer_name +
                             " for using an old API version 0.1 versus application requested 1.0"));
        VkPhysicalDevice phys_dev = inst.GetPhysDev();

        uint32_t count = 0;
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, nullptr);
        ASSERT_EQ(0U, count);
        VkLayerProperties layer_props{};
        env.vulkan_functions.vkEnumerateDeviceLayerProperties(phys_dev, &count, &layer_props);
        ASSERT_EQ(0U, count);
    }
}

// Verify that VK_INSTANCE_LAYERS work.  To test this, make sure that an explicit layer does not affect an instance until
// it is set with VK_INSTANCE_LAYERS
TEST(TestLayers, EnvironEnableExplicitLayer) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA, VK_MAKE_API_VERSION(0, 1, 2, 0)));
    env.get_test_icd().icd_api_version = VK_MAKE_API_VERSION(0, 1, 2, 0);
    VkPhysicalDeviceProperties properties{};
    properties.apiVersion = VK_MAKE_API_VERSION(0, 1, 2, 0);
    env.get_test_icd().add_physical_device({});
    env.get_test_icd().physical_devices.back().set_properties(properties);

    const char* explicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}
                .set_name(explicit_layer_name)
                .set_lib_path(TEST_LAYER_WRAP_OBJECTS_3)
                .set_api_version(VK_MAKE_API_VERSION(0, 1, 0, 0))
                .add_device_extension({VK_KHR_MAINTENANCE1_EXTENSION_NAME, 1, {"vkTrimCommandPoolKHR"}})
                .add_device_extension({VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME, 1, {"vkGetSwapchainStatusKHR"}})),
        "explicit_wrap_layer_both_dev.json");

    // First, test an instance/device without the layer forced on.  The extensions shouldn't be present and
    // the function pointers should be NULL.
    InstWrapper inst1{env.vulkan_functions};
    inst1.CheckCreate();
    VkPhysicalDevice phys_dev1 = inst1.GetPhysDev();

    // Make sure the extensions in the layer aren't present
    uint32_t extension_count = 40;
    std::array<VkExtensionProperties, 40> extensions;
    EXPECT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev1, nullptr, &extension_count, extensions.data()));
    for (uint32_t ext = 0; ext < extension_count; ++ext) {
        if (string_eq(extensions[ext].extensionName, VK_KHR_MAINTENANCE1_EXTENSION_NAME) ||
            string_eq(extensions[ext].extensionName, VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME)) {
            FAIL() << "Extension should not be present";
        }
    }

    // Create a device and query the function pointers
    DeviceWrapper dev1{inst1};
    dev1.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev1.CheckCreate(phys_dev1);
    PFN_vkTrimCommandPoolKHR pfn_TrimCommandPoolBefore = dev1.load("vkTrimCommandPoolKHR");
    PFN_vkGetSwapchainStatusKHR pfn_GetSwapchainStatusBefore = dev1.load("vkGetSwapchainStatusKHR");
    handle_assert_null(pfn_TrimCommandPoolBefore);
    handle_assert_null(pfn_GetSwapchainStatusBefore);

    // Now setup the instance layer
    set_env_var("VK_INSTANCE_LAYERS", explicit_layer_name);

    // Now, test an instance/device with the layer forced on.  The extensions should be present and
    // the function pointers should be valid.
    InstWrapper inst2{env.vulkan_functions};
    inst2.CheckCreate();
    VkPhysicalDevice phys_dev2 = inst2.GetPhysDev();

    // Make sure the extensions in the layer are present
    extension_count = 40;
    EXPECT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev2, nullptr, &extension_count, extensions.data()));
    bool maint_found = false;
    bool pres_found = false;
    for (uint32_t ext = 0; ext < extension_count; ++ext) {
        if (string_eq(extensions[ext].extensionName, VK_KHR_MAINTENANCE1_EXTENSION_NAME)) {
            maint_found = true;
        }
        if (string_eq(extensions[ext].extensionName, VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME)) {
            pres_found = true;
        }
    }
    ASSERT_EQ(true, maint_found);
    ASSERT_EQ(true, pres_found);

    DeviceWrapper dev2{inst2};
    dev2.create_info.add_extension(VK_KHR_MAINTENANCE1_EXTENSION_NAME)
        .add_extension(VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME)
        .add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev2.CheckCreate(phys_dev2);

    PFN_vkTrimCommandPoolKHR pfn_TrimCommandPoolAfter = dev2.load("vkTrimCommandPoolKHR");
    PFN_vkGetSwapchainStatusKHR pfn_GetSwapchainStatusAfter = dev2.load("vkGetSwapchainStatusKHR");
    handle_assert_has_value(pfn_TrimCommandPoolAfter);
    handle_assert_has_value(pfn_GetSwapchainStatusAfter);

    ASSERT_EQ(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, pfn_GetSwapchainStatusAfter(dev2.dev, VK_NULL_HANDLE));

    remove_env_var("VK_INSTANCE_LAYERS");
}

// Add a device layer, should not work
TEST(TestLayers, DoNotUseDeviceLayer) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA, VK_MAKE_API_VERSION(0, 1, 2, 0)));
    env.get_test_icd().icd_api_version = VK_MAKE_API_VERSION(0, 1, 2, 0);
    VkPhysicalDeviceProperties properties{};
    properties.apiVersion = VK_MAKE_API_VERSION(0, 1, 2, 0);
    env.get_test_icd().add_physical_device({});
    env.get_test_icd().physical_devices.back().set_properties(properties);

    const char* explicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}
                .set_name(explicit_layer_name)
                .set_lib_path(TEST_LAYER_WRAP_OBJECTS_3)
                .set_api_version(VK_MAKE_API_VERSION(0, 1, 0, 0))
                .add_device_extension({VK_KHR_MAINTENANCE1_EXTENSION_NAME, 1, {"vkTrimCommandPoolKHR"}})
                .add_device_extension({VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME, 1, {"vkGetSwapchainStatusKHR"}})),
        "explicit_wrap_layer_both_dev.json");

    // First, test an instance/device without the layer forced on.  The extensions shouldn't be present and
    // the function pointers should be NULL.
    InstWrapper inst1{env.vulkan_functions};
    inst1.CheckCreate();
    VkPhysicalDevice phys_dev1 = inst1.GetPhysDev();

    // Make sure the extensions in the layer aren't present
    uint32_t extension_count = 40;
    std::array<VkExtensionProperties, 40> extensions;
    EXPECT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev1, nullptr, &extension_count, extensions.data()));
    for (uint32_t ext = 0; ext < extension_count; ++ext) {
        if (string_eq(extensions[ext].extensionName, VK_KHR_MAINTENANCE1_EXTENSION_NAME) ||
            string_eq(extensions[ext].extensionName, VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME)) {
            FAIL() << "Extension should not be present";
        }
    }

    // Create a device and query the function pointers
    DeviceWrapper dev1{inst1};
    dev1.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev1.CheckCreate(phys_dev1);
    PFN_vkTrimCommandPoolKHR pfn_TrimCommandPoolBefore = dev1.load("vkTrimCommandPoolKHR");
    PFN_vkGetSwapchainStatusKHR pfn_GetSwapchainStatusBefore = dev1.load("vkGetSwapchainStatusKHR");
    handle_assert_null(pfn_TrimCommandPoolBefore);
    handle_assert_null(pfn_GetSwapchainStatusBefore);

    // Now, test an instance/device with the layer forced on.  The extensions should be present and
    // the function pointers should be valid.
    InstWrapper inst2{env.vulkan_functions};
    inst2.CheckCreate();
    VkPhysicalDevice phys_dev2 = inst2.GetPhysDev();

    // Make sure the extensions in the layer aren't present
    extension_count = 40;
    EXPECT_EQ(VK_SUCCESS,
              env.vulkan_functions.vkEnumerateDeviceExtensionProperties(phys_dev1, nullptr, &extension_count, extensions.data()));
    for (uint32_t ext = 0; ext < extension_count; ++ext) {
        if (string_eq(extensions[ext].extensionName, VK_KHR_MAINTENANCE1_EXTENSION_NAME) ||
            string_eq(extensions[ext].extensionName, VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME)) {
            FAIL() << "Extension should not be present";
        }
    }

    DeviceWrapper dev2{inst2};
    dev2.create_info.add_extension(VK_KHR_MAINTENANCE1_EXTENSION_NAME)
        .add_extension(VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME)
        .add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f))
        .add_layer(explicit_layer_name);
    dev2.CheckCreate(phys_dev2, VK_ERROR_EXTENSION_NOT_PRESENT);

    DeviceWrapper dev3{inst2};
    dev3.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f)).add_layer(explicit_layer_name);
    dev3.CheckCreate(phys_dev2);

    PFN_vkTrimCommandPoolKHR pfn_TrimCommandPoolAfter = dev3.load("vkTrimCommandPoolKHR");
    PFN_vkGetSwapchainStatusKHR pfn_GetSwapchainStatusAfter = dev3.load("vkGetSwapchainStatusKHR");
    handle_assert_null(pfn_TrimCommandPoolAfter);
    handle_assert_null(pfn_GetSwapchainStatusAfter);
}

// Make sure that a layer enabled as both an instance and device layer works properly.
TEST(TestLayers, InstanceAndDeviceLayer) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA, VK_MAKE_API_VERSION(0, 1, 2, 0)));
    env.get_test_icd().icd_api_version = VK_MAKE_API_VERSION(0, 1, 2, 0);
    VkPhysicalDeviceProperties properties{};
    properties.apiVersion = VK_MAKE_API_VERSION(0, 1, 2, 0);
    env.get_test_icd().add_physical_device({});
    env.get_test_icd().physical_devices.back().set_properties(properties);

    const char* explicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}
                .set_name(explicit_layer_name)
                .set_lib_path(TEST_LAYER_WRAP_OBJECTS_3)
                .set_api_version(VK_MAKE_API_VERSION(0, 1, 0, 0))
                .add_device_extension({VK_KHR_MAINTENANCE1_EXTENSION_NAME, 1, {"vkTrimCommandPoolKHR"}})
                .add_device_extension({VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME, 1, {"vkGetSwapchainStatusKHR"}})),
        "explicit_wrap_layer_both_dev.json");

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_layer(explicit_layer_name);
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    DeviceWrapper dev{inst};
    dev.create_info.add_extension(VK_KHR_MAINTENANCE1_EXTENSION_NAME)
        .add_extension(VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME)
        .add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f))
        .add_layer(explicit_layer_name);
    dev.CheckCreate(phys_dev);

    PFN_vkTrimCommandPoolKHR pfn_TrimCommandPoolAfter = dev.load("vkTrimCommandPoolKHR");
    PFN_vkGetSwapchainStatusKHR pfn_GetSwapchainStatusAfter = dev.load("vkGetSwapchainStatusKHR");
    handle_assert_has_value(pfn_TrimCommandPoolAfter);
    handle_assert_has_value(pfn_GetSwapchainStatusAfter);

    ASSERT_EQ(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, pfn_GetSwapchainStatusAfter(dev.dev, VK_NULL_HANDLE));
}

// Make sure loader does not throw an error for a device layer  that is not present
TEST(TestLayers, DeviceLayerNotPresent) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA, VK_MAKE_API_VERSION(0, 1, 2, 0)));
    env.get_test_icd().icd_api_version = VK_MAKE_API_VERSION(0, 1, 2, 0);
    VkPhysicalDeviceProperties properties{};
    properties.apiVersion = VK_MAKE_API_VERSION(0, 1, 2, 0);
    env.get_test_icd().add_physical_device({});
    env.get_test_icd().physical_devices.back().set_properties(properties);

    const char* explicit_layer_name = "VK_LAYER_LUNARG_wrap_objects";

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    DeviceWrapper dev{inst};
    dev.create_info.add_layer(explicit_layer_name);
    dev.CheckCreate(phys_dev);
}

TEST(LayerPhysDeviceMod, AddPhysicalDevices) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name("VkLayer_LunarG_add_phys_dev")
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .set_api_version(VK_API_VERSION_1_1)
                                                         .set_disable_environment("TEST_DISABLE_ADD_PHYS_DEV")),
                           "test_layer_add.json");

    auto& layer = env.get_test_layer(0);
    layer.set_add_phys_devs(true);

    for (uint32_t icd = 0; icd < 2; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_2;
        VkPhysicalDeviceProperties properties{};
        properties.apiVersion = VK_API_VERSION_1_2;
        properties.vendorID = 0x11000000 + (icd << 6);
        for (uint32_t dev = 0; dev < 3; ++dev) {
            properties.deviceID = properties.vendorID + dev;
            properties.deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
            auto dev_name = std::string("physdev_") + std::to_string(icd) + "_" + std::to_string(dev);
#if defined(_WIN32)
            strncpy_s(properties.deviceName, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE, dev_name.c_str(), dev_name.length() + 1);
#else
            strncpy(properties.deviceName, dev_name.c_str(), VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);
#endif
            cur_icd.add_physical_device({});
            cur_icd.physical_devices.back().set_properties(properties);
        }
        cur_icd.physical_device_groups.emplace_back(cur_icd.physical_devices[0]);
        cur_icd.physical_device_groups.emplace_back(cur_icd.physical_devices[1]);
        cur_icd.physical_device_groups.back().use_physical_device(cur_icd.physical_devices[2]);
    }
    const uint32_t icd_devices = 6;

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_API_VERSION_1_1);
    inst.CheckCreate();

    uint32_t dev_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &dev_count, nullptr));
    ASSERT_GT(dev_count, icd_devices);

    auto not_exp_physical_devices = std::vector<VkPhysicalDevice>(icd_devices);
    uint32_t returned_phys_dev_count = icd_devices;
    ASSERT_EQ(VK_INCOMPLETE, inst->vkEnumeratePhysicalDevices(inst, &returned_phys_dev_count, not_exp_physical_devices.data()));
    ASSERT_EQ(icd_devices, returned_phys_dev_count);

    auto physical_devices = std::vector<VkPhysicalDevice>(dev_count);
    returned_phys_dev_count = dev_count;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_phys_dev_count, physical_devices.data()));
    ASSERT_EQ(dev_count, returned_phys_dev_count);

    uint32_t diff_count = dev_count - icd_devices;
    uint32_t found_incomplete = 0;
    uint32_t found_added_count = 0;
    for (uint32_t dev = 0; dev < dev_count; ++dev) {
        VkPhysicalDeviceProperties props{};
        inst->vkGetPhysicalDeviceProperties(physical_devices[dev], &props);
        if (string_eq(props.deviceName, "physdev_added_xx")) {
            found_added_count++;
        }
        for (uint32_t incomp = 0; incomp < icd_devices; ++incomp) {
            if (not_exp_physical_devices[incomp] == physical_devices[dev]) {
                found_incomplete++;
                break;
            }
        }
    }

    // We should have added the number of diff items, and the incomplete count should match the number of
    // original physical devices.
    ASSERT_EQ(found_added_count, diff_count);
    ASSERT_EQ(found_incomplete, icd_devices);
}

TEST(LayerPhysDeviceMod, RemovePhysicalDevices) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name("VkLayer_LunarG_remove_phys_dev")
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .set_api_version(VK_API_VERSION_1_1)
                                                         .set_disable_environment("TEST_DISABLE_REMOVE_PHYS_DEV")),
                           "test_layer_remove.json");

    auto& layer = env.get_test_layer(0);
    layer.set_remove_phys_devs(true);

    for (uint32_t icd = 0; icd < 2; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_2;
        VkPhysicalDeviceProperties properties{};
        properties.apiVersion = VK_API_VERSION_1_2;
        properties.vendorID = 0x11000000 + (icd << 6);
        for (uint32_t dev = 0; dev < 3; ++dev) {
            properties.deviceID = properties.vendorID + dev;
            properties.deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
            auto dev_name = std::string("physdev_") + std::to_string(icd) + "_" + std::to_string(dev);
#if defined(_WIN32)
            strncpy_s(properties.deviceName, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE, dev_name.c_str(), dev_name.length() + 1);
#else
            strncpy(properties.deviceName, dev_name.c_str(), VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);
#endif
            cur_icd.add_physical_device({});
            cur_icd.physical_devices.back().set_properties(properties);
        }
        cur_icd.physical_device_groups.emplace_back(cur_icd.physical_devices[0]);
        cur_icd.physical_device_groups.emplace_back(cur_icd.physical_devices[1]);
        cur_icd.physical_device_groups.back().use_physical_device(cur_icd.physical_devices[2]);
    }
    const uint32_t icd_devices = 6;

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_API_VERSION_1_1);
    inst.CheckCreate();

    uint32_t dev_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &dev_count, nullptr));
    ASSERT_LT(dev_count, icd_devices);

    auto physical_devices = std::vector<VkPhysicalDevice>(dev_count);
    uint32_t returned_phys_dev_count = dev_count;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_phys_dev_count, physical_devices.data()));
    ASSERT_EQ(dev_count, returned_phys_dev_count);
}

TEST(LayerPhysDeviceMod, ReorderPhysicalDevices) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name("VkLayer_LunarG_reorder_phys_dev")
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .set_api_version(VK_API_VERSION_1_1)
                                                         .set_disable_environment("TEST_DISABLE_REORDER_PHYS_DEV")),
                           "test_layer_reorder.json");

    auto& layer = env.get_test_layer(0);
    layer.set_reorder_phys_devs(true);

    for (uint32_t icd = 0; icd < 2; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_2;
        VkPhysicalDeviceProperties properties{};
        properties.apiVersion = VK_API_VERSION_1_2;
        properties.vendorID = 0x11000000 + (icd << 6);
        for (uint32_t dev = 0; dev < 3; ++dev) {
            properties.deviceID = properties.vendorID + dev;
            properties.deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
            auto dev_name = std::string("physdev_") + std::to_string(icd) + "_" + std::to_string(dev);
#if defined(_WIN32)
            strncpy_s(properties.deviceName, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE, dev_name.c_str(), dev_name.length() + 1);
#else
            strncpy(properties.deviceName, dev_name.c_str(), VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);
#endif
            cur_icd.add_physical_device({});
            cur_icd.physical_devices.back().set_properties(properties);
        }
        cur_icd.physical_device_groups.emplace_back(cur_icd.physical_devices[0]);
        cur_icd.physical_device_groups.emplace_back(cur_icd.physical_devices[1]);
        cur_icd.physical_device_groups.back().use_physical_device(cur_icd.physical_devices[2]);
    }
    const uint32_t icd_devices = 6;

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_API_VERSION_1_1);
    inst.CheckCreate();

    uint32_t dev_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &dev_count, nullptr));
    ASSERT_EQ(dev_count, icd_devices);

    auto physical_devices = std::vector<VkPhysicalDevice>(dev_count);
    uint32_t returned_phys_dev_count = dev_count;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_phys_dev_count, physical_devices.data()));
    ASSERT_EQ(dev_count, returned_phys_dev_count);
}

TEST(LayerPhysDeviceMod, AddRemoveAndReorderPhysicalDevices) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name("VkLayer_LunarG_all_phys_dev")
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .set_api_version(VK_API_VERSION_1_1)
                                                         .set_disable_environment("TEST_DISABLE_ALL_PHYS_DEV")),
                           "test_layer_all.json");

    auto& layer = env.get_test_layer(0);
    layer.set_add_phys_devs(true).set_remove_phys_devs(true).set_reorder_phys_devs(true);

    for (uint32_t icd = 0; icd < 2; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_2;
        VkPhysicalDeviceProperties properties{};
        properties.apiVersion = VK_API_VERSION_1_2;
        properties.vendorID = 0x11000000 + (icd << 6);
        for (uint32_t dev = 0; dev < 3; ++dev) {
            properties.deviceID = properties.vendorID + dev;
            properties.deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
            auto dev_name = std::string("physdev_") + std::to_string(icd) + "_" + std::to_string(dev);
#if defined(_WIN32)
            strncpy_s(properties.deviceName, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE, dev_name.c_str(), dev_name.length() + 1);
#else
            strncpy(properties.deviceName, dev_name.c_str(), VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);
#endif
            cur_icd.add_physical_device({});
            cur_icd.physical_devices.back().set_properties(properties);
        }
        cur_icd.physical_device_groups.emplace_back(cur_icd.physical_devices[0]);
        cur_icd.physical_device_groups.emplace_back(cur_icd.physical_devices[1]);
        cur_icd.physical_device_groups.back().use_physical_device(cur_icd.physical_devices[2]);
    }
    const uint32_t icd_devices = 6;

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_API_VERSION_1_1);
    inst.CheckCreate();

    uint32_t dev_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &dev_count, nullptr));
    ASSERT_GT(dev_count, icd_devices);

    auto physical_devices = std::vector<VkPhysicalDevice>(dev_count);
    uint32_t returned_phys_dev_count = dev_count;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &returned_phys_dev_count, physical_devices.data()));
    ASSERT_EQ(dev_count, returned_phys_dev_count);

    uint32_t found_added_count = 0;
    for (uint32_t dev = 0; dev < dev_count; ++dev) {
        VkPhysicalDeviceProperties props{};
        inst->vkGetPhysicalDeviceProperties(physical_devices[dev], &props);
        if (string_eq(props.deviceName, "physdev_added_xx")) {
            found_added_count++;
        }
    }

    // Should see 2 removed, but 3 added so a diff count of 1
    uint32_t diff_count = dev_count - icd_devices;
    ASSERT_EQ(1U, diff_count);
    ASSERT_EQ(found_added_count, 3U);
}

static bool GroupsAreTheSame(VkPhysicalDeviceGroupProperties a, VkPhysicalDeviceGroupProperties b) {
    if (a.physicalDeviceCount != b.physicalDeviceCount) {
        return false;
    }
    for (uint32_t dev = 0; dev < a.physicalDeviceCount; ++dev) {
        if (a.physicalDevices[dev] != b.physicalDevices[dev]) {
            return false;
        }
    }
    return true;
}

TEST(LayerPhysDeviceMod, AddPhysicalDeviceGroups) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name("VkLayer_LunarG_add_phys_dev")
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .set_api_version(VK_API_VERSION_1_1)
                                                         .set_disable_environment("TEST_DISABLE_ADD_PHYS_DEV")),
                           "test_layer_remove.json");

    auto& layer = env.get_test_layer(0);
    layer.set_add_phys_devs(true);

    for (uint32_t icd = 0; icd < 2; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_2;
        VkPhysicalDeviceProperties properties{};
        properties.apiVersion = VK_API_VERSION_1_2;
        properties.vendorID = 0x11000000 + (icd << 6);
        for (uint32_t dev = 0; dev < 3; ++dev) {
            properties.deviceID = properties.vendorID + dev;
            properties.deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
            auto dev_name = std::string("physdev_") + std::to_string(icd) + "_" + std::to_string(dev);
#if defined(_WIN32)
            strncpy_s(properties.deviceName, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE, dev_name.c_str(), dev_name.length() + 1);
#else
            strncpy(properties.deviceName, dev_name.c_str(), VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);
#endif
            cur_icd.add_physical_device({});
            cur_icd.physical_devices.back().set_properties(properties);
        }
        cur_icd.physical_device_groups.emplace_back(cur_icd.physical_devices[0]);
        cur_icd.physical_device_groups.emplace_back(cur_icd.physical_devices[1]);
        cur_icd.physical_device_groups.back().use_physical_device(cur_icd.physical_devices[2]);
    }
    const uint32_t icd_groups = 4;

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_API_VERSION_1_1);
    inst.CheckCreate();

    uint32_t grp_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &grp_count, nullptr));
    ASSERT_GT(grp_count, icd_groups);

    auto not_exp_phys_dev_groups = std::vector<VkPhysicalDeviceGroupProperties>(icd_groups);
    for (uint32_t group = 0; group < icd_groups; ++group) {
        not_exp_phys_dev_groups[group].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
    }
    uint32_t returned_group_count = icd_groups;
    ASSERT_EQ(VK_INCOMPLETE, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, not_exp_phys_dev_groups.data()));
    ASSERT_EQ(icd_groups, returned_group_count);

    auto phys_dev_groups = std::vector<VkPhysicalDeviceGroupProperties>(grp_count);
    for (uint32_t group = 0; group < grp_count; ++group) {
        phys_dev_groups[group].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
    }
    returned_group_count = grp_count;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, phys_dev_groups.data()));
    ASSERT_EQ(grp_count, returned_group_count);

    uint32_t diff_count = grp_count - icd_groups;
    uint32_t found_incomplete = 0;
    uint32_t found_added_count = 0;
    for (uint32_t grp = 0; grp < grp_count; ++grp) {
        // Shortcut, only groups with 1 device could be added in the newly added count
        if (1 == phys_dev_groups[grp].physicalDeviceCount) {
            for (uint32_t dev = 0; dev < phys_dev_groups[grp].physicalDeviceCount; ++dev) {
                VkPhysicalDeviceProperties props{};
                inst->vkGetPhysicalDeviceProperties(phys_dev_groups[grp].physicalDevices[dev], &props);
                if (string_eq(props.deviceName, "physdev_added_xx")) {
                    found_added_count++;
                }
            }
        }
        for (uint32_t incomp = 0; incomp < icd_groups; ++incomp) {
            if (GroupsAreTheSame(not_exp_phys_dev_groups[incomp], phys_dev_groups[grp])) {
                found_incomplete++;
                break;
            }
        }
    }

    // We should have added the number of diff items, and the incomplete count should match the number of
    // original physical devices.
    ASSERT_EQ(found_added_count, diff_count);
    ASSERT_EQ(found_incomplete, icd_groups);
}

TEST(LayerPhysDeviceMod, RemovePhysicalDeviceGroups) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name("VkLayer_LunarG_remove_phys_dev")
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .set_api_version(VK_API_VERSION_1_1)
                                                         .set_disable_environment("TEST_DISABLE_REMOVE_PHYS_DEV")),
                           "test_layer_remove.json");

    auto& layer = env.get_test_layer(0);
    layer.set_remove_phys_devs(true);

    for (uint32_t icd = 0; icd < 2; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_2;
        VkPhysicalDeviceProperties properties{};
        properties.apiVersion = VK_API_VERSION_1_2;
        properties.vendorID = 0x11000000 + (icd << 6);
        for (uint32_t dev = 0; dev < 3; ++dev) {
            properties.deviceID = properties.vendorID + dev;
            properties.deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
            auto dev_name = std::string("physdev_") + std::to_string(icd) + "_" + std::to_string(dev);
#if defined(_WIN32)
            strncpy_s(properties.deviceName, dev_name.c_str(), VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);
#else
            strncpy(properties.deviceName, dev_name.c_str(), VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);
#endif
            cur_icd.add_physical_device({});
            cur_icd.physical_devices.back().set_properties(properties);
        }
        cur_icd.physical_device_groups.emplace_back(cur_icd.physical_devices[0]);
        cur_icd.physical_device_groups.emplace_back(cur_icd.physical_devices[1]);
        cur_icd.physical_device_groups.back().use_physical_device(cur_icd.physical_devices[2]);
    }
    const uint32_t icd_groups = 4;

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_API_VERSION_1_1);
    inst.CheckCreate();

    uint32_t grp_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &grp_count, nullptr));
    ASSERT_LT(grp_count, icd_groups);

    auto phys_dev_groups = std::vector<VkPhysicalDeviceGroupProperties>(grp_count);
    for (uint32_t group = 0; group < grp_count; ++group) {
        phys_dev_groups[group].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
    }
    uint32_t returned_group_count = grp_count;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, phys_dev_groups.data()));
    ASSERT_EQ(grp_count, returned_group_count);
}

TEST(LayerPhysDeviceMod, ReorderPhysicalDeviceGroups) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name("VkLayer_LunarG_reorder_phys_dev")
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .set_api_version(VK_API_VERSION_1_1)
                                                         .set_disable_environment("TEST_DISABLE_REORDER_PHYS_DEV")),
                           "test_layer_reorder.json");

    auto& layer = env.get_test_layer(0);
    layer.set_reorder_phys_devs(true);

    for (uint32_t icd = 0; icd < 2; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_2;
        VkPhysicalDeviceProperties properties{};
        properties.apiVersion = VK_API_VERSION_1_2;
        properties.vendorID = 0x11000000 + (icd << 6);
        for (uint32_t dev = 0; dev < 3; ++dev) {
            properties.deviceID = properties.vendorID + dev;
            properties.deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
            auto dev_name = std::string("physdev_") + std::to_string(icd) + "_" + std::to_string(dev);
#if defined(_WIN32)
            strncpy_s(properties.deviceName, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE, dev_name.c_str(), dev_name.length() + 1);
#else
            strncpy(properties.deviceName, dev_name.c_str(), VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);
#endif
            cur_icd.add_physical_device({});
            cur_icd.physical_devices.back().set_properties(properties);
        }
        cur_icd.physical_device_groups.emplace_back(cur_icd.physical_devices[0]);
        cur_icd.physical_device_groups.emplace_back(cur_icd.physical_devices[1]);
        cur_icd.physical_device_groups.back().use_physical_device(cur_icd.physical_devices[2]);
    }
    const uint32_t icd_groups = 4;

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_API_VERSION_1_1);
    inst.CheckCreate();

    uint32_t grp_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &grp_count, nullptr));
    ASSERT_EQ(grp_count, icd_groups);

    auto phys_dev_groups = std::vector<VkPhysicalDeviceGroupProperties>(grp_count);
    for (uint32_t group = 0; group < grp_count; ++group) {
        phys_dev_groups[group].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
    }
    uint32_t returned_group_count = grp_count;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, phys_dev_groups.data()));
    ASSERT_EQ(grp_count, returned_group_count);
}

TEST(LayerPhysDeviceMod, AddRemoveAndReorderPhysicalDeviceGroups) {
    FrameworkEnvironment env;
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    env.add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                         .set_name("VkLayer_LunarG_all_phys_dev")
                                                         .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                         .set_api_version(VK_API_VERSION_1_1)
                                                         .set_disable_environment("TEST_DISABLE_ALL_PHYS_DEV")),
                           "test_layer_all.json");

    auto& layer = env.get_test_layer(0);
    layer.set_add_phys_devs(true).set_remove_phys_devs(true).set_reorder_phys_devs(true);

    for (uint32_t icd = 0; icd < 2; ++icd) {
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_2;
        VkPhysicalDeviceProperties properties{};
        properties.apiVersion = VK_API_VERSION_1_2;
        properties.vendorID = 0x11000000 + (icd << 6);
        for (uint32_t dev = 0; dev < 3; ++dev) {
            properties.deviceID = properties.vendorID + dev;
            properties.deviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
            auto dev_name = std::string("physdev_") + std::to_string(icd) + "_" + std::to_string(dev);
#if defined(_WIN32)
            strncpy_s(properties.deviceName, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE, dev_name.c_str(), dev_name.length() + 1);
#else
            strncpy(properties.deviceName, dev_name.c_str(), VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);
#endif
            cur_icd.add_physical_device({});
            cur_icd.physical_devices.back().set_properties(properties);
        }
        cur_icd.physical_device_groups.emplace_back(cur_icd.physical_devices[0]);
        cur_icd.physical_device_groups.emplace_back(cur_icd.physical_devices[1]);
        cur_icd.physical_device_groups.back().use_physical_device(cur_icd.physical_devices[2]);
    }
    const uint32_t icd_groups = 4;

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_API_VERSION_1_1);
    inst.CheckCreate();

    uint32_t grp_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &grp_count, nullptr));
    ASSERT_GT(grp_count, icd_groups);

    auto phys_dev_groups = std::vector<VkPhysicalDeviceGroupProperties>(grp_count);
    for (uint32_t group = 0; group < grp_count; ++group) {
        phys_dev_groups[group].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
    }
    uint32_t returned_group_count = grp_count;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDeviceGroups(inst, &returned_group_count, phys_dev_groups.data()));
    ASSERT_EQ(grp_count, returned_group_count);

    uint32_t diff_count = grp_count - icd_groups;
    uint32_t found_added_count = 0;
    for (uint32_t grp = 0; grp < grp_count; ++grp) {
        // Shortcut, only groups with 1 device could be added in the newly added count
        if (1 == phys_dev_groups[grp].physicalDeviceCount) {
            for (uint32_t dev = 0; dev < phys_dev_groups[grp].physicalDeviceCount; ++dev) {
                VkPhysicalDeviceProperties props{};
                inst->vkGetPhysicalDeviceProperties(phys_dev_groups[grp].physicalDevices[dev], &props);
                if (string_eq(props.deviceName, "physdev_added_xx")) {
                    found_added_count++;
                }
            }
        }
    }

    // Should see 2 devices removed which should result in 1 group removed and since 3
    // devices were added we should have 3 new groups.  So we should have a diff of 2
    // groups and 3 new groups
    ASSERT_EQ(2U, diff_count);
    ASSERT_EQ(found_added_count, 3U);
}
