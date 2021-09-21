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

// Load the global function pointers with and without a NULL vkInstance handle.
// Call the function to make sure it is callable, don't care about what is returned.
TEST(GetProcAddr, GlobalFunctions) {
    SingleICDShim env(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    env.get_test_icd().physical_devices.emplace_back("physical_device_0");

    auto& gipa = env.vulkan_functions.vkGetInstanceProcAddr;

    auto EnumerateInstanceExtensionProperties =
        reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(gipa(NULL, "vkEnumerateInstanceExtensionProperties"));
    handle_assert_has_value(EnumerateInstanceExtensionProperties);
    uint32_t ext_count = 0;
    ASSERT_EQ(VK_SUCCESS, EnumerateInstanceExtensionProperties("", &ext_count, nullptr));

    auto EnumerateInstanceLayerProperties =
        reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(gipa(NULL, "vkEnumerateInstanceLayerProperties"));
    handle_assert_has_value(EnumerateInstanceLayerProperties);
    uint32_t layer_count = 0;
    ASSERT_EQ(VK_SUCCESS, EnumerateInstanceLayerProperties(&layer_count, nullptr));

    auto EnumerateInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(gipa(NULL, "vkEnumerateInstanceVersion"));
    handle_assert_has_value(EnumerateInstanceVersion);
    uint32_t api_version = 0;
    EnumerateInstanceVersion(&api_version);

    auto GetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(gipa(NULL, "vkGetInstanceProcAddr"));
    handle_assert_has_value(GetInstanceProcAddr);
    GetInstanceProcAddr(NULL, "vkGetInstanceProcAddr");

    auto CreateInstance = reinterpret_cast<PFN_vkCreateInstance>(gipa(NULL, "vkCreateInstance"));
    handle_assert_has_value(CreateInstance);

    // Now create an instance and query the functions again but with a valid instance, should be NULL
    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    EnumerateInstanceExtensionProperties =
        reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(gipa(inst, "vkEnumerateInstanceExtensionProperties"));
    handle_assert_null(EnumerateInstanceExtensionProperties);

    EnumerateInstanceLayerProperties =
        reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(gipa(inst, "vkEnumerateInstanceLayerProperties"));
    handle_assert_null(EnumerateInstanceLayerProperties);

    EnumerateInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(gipa(inst, "vkEnumerateInstanceVersion"));
    handle_assert_null(EnumerateInstanceVersion);

    CreateInstance = reinterpret_cast<PFN_vkCreateInstance>(gipa(inst, "vkCreateInstance"));
    handle_assert_null(CreateInstance);

    GetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(gipa(inst, "vkGetInstanceProcAddr"));
    handle_assert_null(GetInstanceProcAddr);
}