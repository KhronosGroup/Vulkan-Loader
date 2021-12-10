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

#pragma once

#include "test_util.h"

#include "layer/layer_util.h"

#include "loader/generated/vk_layer_dispatch_table.h"

/*
Interface Version 0
*/

/*
must export the following: -- always exported
vkEnumerateInstanceLayerProperties
vkEnumerateInstanceExtensionProperties
Must export the following but nothing -- always exported
vkEnumerateDeviceLayerProperties
vkEnumerateDeviceExtensionProperties
*/

// export test_layer_GetInstanceProcAddr(instance, pName)
// TEST_LAYER_EXPORT_LAYER_NAMED_GIPA
// or (single layer binary)
// export vkGetInstanceProcAddr
// TEST_LAYER_EXPORT_NO_NAME_GIPA

// export test_layer_GetDeviceProcAddr(device, pName)
// TEST_LAYER_EXPORT_LAYER_NAMED_GDPA
// or (single layer binary)
// export vkGetDeviceProcAddr
// TEST_LAYER_EXPORT_NO_NAME_GDPA

/*
Interface Version 1
*/
// export GetInstanceProcAddr
// TEST_LAYER_EXPORT_NO_PREFIX_GIPA

// export GetDeviceProcAddr
// TEST_LAYER_EXPORT_NO_PREFIX_GDPA

// Layer Manifest can override the names of the GetInstanceProcAddr and GetDeviceProcAddrfunctions

/*
Interface Version 2
*/
// export vk_layerGetPhysicalDeviceProcAddr
// TEST_LAYER_EXPORT_GET_PHYSICAL_DEVICE_PROC_ADDR

// export vkNegotiateLoaderLayerInterfaceVersion
// TEST_LAYER_EXPORT_NEGOTIATE_LOADER_LAYER_INTERFACE_VERSION

// Added manifest version 1.1.0

struct TestLayer;

// Callbacks allow tests to implement custom functionality without modifying the layer binary
// TestLayer* layer - Access to the TestLayer object itself
// void* data - pointer to test specific thing, used to pass data from the test into the TestLayer
// Returns VkResult - This value will be used as the return value of the function
using FP_layer_callback = VkResult (*)(TestLayer& layer, void* data);

struct TestLayer {
    fs::path manifest_file_path;
    uint32_t manifest_version = VK_MAKE_VERSION(1, 1, 2);

    BUILDER_VALUE(TestLayer, bool, is_meta_layer, false)

    BUILDER_VALUE(TestLayer, std::string, unique_name, {})
    BUILDER_VALUE(TestLayer, uint32_t, api_version, VK_MAKE_VERSION(1, 0, 0))
    BUILDER_VALUE(TestLayer, uint32_t, implementation_version, 2)
    BUILDER_VALUE(TestLayer, uint32_t, min_implementation_version, 0)
    BUILDER_VALUE(TestLayer, std::string, description, {})

    BUILDER_VECTOR(TestLayer, std::string, alternative_function_names, alternative_function_name)

    BUILDER_VECTOR(TestLayer, Extension, instance_extensions, instance_extension)
    BUILDER_VECTOR(TestLayer, Extension, device_extensions, device_extension)

    BUILDER_VALUE(TestLayer, std::string, enable_environment, {});
    BUILDER_VALUE(TestLayer, std::string, disable_environment, {});

    BUILDER_VECTOR(TestLayer, LayerDefinition, meta_component_layers, meta_component_layer);

    BUILDER_VALUE(TestLayer, bool, intercept_vkEnumerateInstanceExtensionProperties, false)
    BUILDER_VALUE(TestLayer, bool, intercept_vkEnumerateInstanceLayerProperties, false)
    // Called in vkCreateInstance after calling down the chain & returning
    BUILDER_VALUE(TestLayer, std::function<VkResult(TestLayer& layer)>, create_instance_callback, {})
    // Called in vkCreateDevice after calling down the chain & returning
    BUILDER_VALUE(TestLayer, std::function<VkResult(TestLayer& layer)>, create_device_callback, {})

    PFN_vkGetInstanceProcAddr next_vkGetInstanceProcAddr = VK_NULL_HANDLE;
    PFN_vkGetDeviceProcAddr next_vkGetDeviceProcAddr = VK_NULL_HANDLE;

    VkInstance instance_handle;
    VkLayerInstanceDispatchTable instance_dispatch_table;

    struct Device {
        VkDevice device_handle;
        VkLayerDispatchTable dispatch_table;
    };
    std::vector<Device> created_devices;
};

using GetTestLayerFunc = TestLayer* (*)();
#define GET_TEST_LAYER_FUNC_STR "get_test_layer_func"

using GetNewTestLayerFunc = TestLayer* (*)();
#define RESET_LAYER_FUNC_STR "reset_layer_func"