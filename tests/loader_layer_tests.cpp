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
    const char* layer_name = "TestLayer";
    ManifestLayer::LayerDescription description{};
    description.name = layer_name;
    description.component_layers = {"InvalidLayer1", "InvalidLayer2"};

    ManifestLayer meta_layer;
    meta_layer.layers.push_back(description);
    env->AddExplicitLayer(meta_layer, "meta_test_layer.json");

    InstWrapper inst{env->vulkan_functions};
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_layer(layer_name);
    ASSERT_EQ(VK_ERROR_LAYER_NOT_PRESENT, CreateInst(inst, inst_create_info));
}
