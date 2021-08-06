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

class UnknownExtension : public ::testing::Test {
   protected:
    virtual void SetUp() {
        env = std::unique_ptr<SingleICDShim>(
            new SingleICDShim(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA, VK_MAKE_VERSION(1, 0, 0))));
    }

    virtual void TearDown() { env.reset(); }
    std::unique_ptr<SingleICDShim> env;
};

/*
 Creates a TestICD with a function unknown to the loader called vkNotRealFuncTEST. The TestICD, when vk_icdGetPhysicalDeviceProcAddr
 is called, will return the custom_physical_device_function if the function name matches vkNotRealFuncTEST. The test then calls the
 function to verify that the unknown physical device function dispatching is working correctly.
*/

VkResult custom_physical_device_function(VkPhysicalDevice device, int foo, int bar) { return VK_SUCCESS; }
using PFN_custom_physical_device_function = decltype(&custom_physical_device_function);

TEST_F(UnknownExtension, ICDKnownExtension) {
    auto& driver = env->get_test_icd();
    const char* fake_function_name = "vkNotRealFuncTEST";

    driver.physical_devices.emplace_back("physical_device_0");
    driver.custom_physical_device_functions.push_back(
        VulkanFunction{fake_function_name, reinterpret_cast<void*>(custom_physical_device_function)});

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    PFN_custom_physical_device_function returned_func = reinterpret_cast<PFN_custom_physical_device_function>(
        env->vulkan_functions.vkGetInstanceProcAddr(inst.inst, fake_function_name));
    ASSERT_NE(returned_func, nullptr);
    ASSERT_EQ(returned_func(phys_dev, 42, 58008), VK_SUCCESS);
}