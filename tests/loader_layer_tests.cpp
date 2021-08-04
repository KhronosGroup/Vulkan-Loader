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
        env = std::unique_ptr<SingleICDShim>(new SingleICDShim(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_MAKE_VERSION(1, 0, 0))));
    }

    virtual void TearDown() { env.reset(); }
    std::unique_ptr<SingleICDShim> env;
};

// Subtyping for organization
class ExplicitLayers : public LayerTests {};
class ImplicitLayers : public LayerTests {};
class MetaLayers : public LayerTests {};
class OverrideMetaLayer : public LayerTests {};

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
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_layer(meta_layer_name);
    EXPECT_EQ(VK_ERROR_LAYER_NOT_PRESENT, CreateInst(inst, inst_create_info));
}
