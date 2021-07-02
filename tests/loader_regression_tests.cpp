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
        env = std::unique_ptr<SingleICDShim>(
            new SingleICDShim(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_MAKE_VERSION(1, 0, 0))));
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
    VkInstanceCreateInfo const info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, 0, nullptr, 0, nullptr, 0, nullptr};

    VkInstance instance = VK_NULL_HANDLE;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(&info, VK_NULL_HANDLE, &instance), VK_SUCCESS);

    env->vulkan_functions.vkDestroyInstance(instance, nullptr);
}

// VUID-vkDestroyInstance-instance-parameter, VUID-vkDestroyInstance-pAllocator-parameter
TEST_F(CreateInstance, DestroyInstanceNullHandle) { env->vulkan_functions.vkDestroyInstance(VK_NULL_HANDLE, nullptr); }

// VUID-vkDestroyDevice-device-parameter, VUID-vkDestroyDevice-pAllocator-parameter
TEST_F(CreateInstance, DestroyDeviceNullHandle) { env->vulkan_functions.vkDestroyDevice(VK_NULL_HANDLE, nullptr); }

TEST_F(CreateInstance, ExtensionNotPresent) {
    auto& driver = env->get_test_icd();
    {
        VkInstance inst = VK_NULL_HANDLE;
        auto inst_info = driver.GetVkInstanceCreateInfo();
        inst_info.add_extension("VK_EXT_validation_features");  // test icd won't report this as supported
        ASSERT_EQ(VK_ERROR_EXTENSION_NOT_PRESENT, env->vulkan_functions.vkCreateInstance(inst_info.get(), VK_NULL_HANDLE, &inst));
    }
    {
        VkInstance inst = VK_NULL_HANDLE;
        auto inst_info = driver.GetVkInstanceCreateInfo();
        inst_info.add_extension("Non_existant_extension");  // unknown instance extension
        ASSERT_EQ(VK_ERROR_EXTENSION_NOT_PRESENT, env->vulkan_functions.vkCreateInstance(inst_info.get(), VK_NULL_HANDLE, &inst));
    }
}

TEST_F(EnumeratePhysicalDevices, OneCall) {
    auto& driver = env->get_test_icd();
    driver.SetMinICDInterfaceVersion(5);

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
    auto& driver = env->get_test_icd();
    driver.SetMinICDInterfaceVersion(5);

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.emplace_back("physical_device_1");

    InstWrapper inst{env->vulkan_functions};
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);

    uint32_t physical_count = driver.physical_devices.size();
    uint32_t returned_physical_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, nullptr));
    ASSERT_EQ(physical_count, returned_physical_count);

    std::unique_ptr<VkPhysicalDevice[]> physical_device_handles(new VkPhysicalDevice[physical_count]);
    ASSERT_EQ(VK_SUCCESS,
              env->vulkan_functions.vkEnumeratePhysicalDevices(inst.inst, &returned_physical_count, physical_device_handles.get()));
    ASSERT_EQ(physical_count, returned_physical_count);
}

TEST_F(EnumeratePhysicalDevices, MatchOneAndTwoCallNumbers) {
    auto& driver = env->get_test_icd();
    driver.SetMinICDInterfaceVersion(5);

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.emplace_back("physical_device_1");
    driver.physical_devices.emplace_back("physical_device_2");

    InstWrapper inst1{env->vulkan_functions};
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst1, inst_create_info), VK_SUCCESS);

    uint32_t physical_count_one_call = driver.physical_devices.size();
    uint32_t returned_physical_count_one_call = driver.physical_devices.size();
    std::unique_ptr<VkPhysicalDevice[]> physical_device_handles_one_call(new VkPhysicalDevice[physical_count_one_call]);
    ASSERT_EQ(VK_SUCCESS,
              inst1->vkEnumeratePhysicalDevices(inst1, &returned_physical_count_one_call, physical_device_handles_one_call.get()));
    ASSERT_EQ(physical_count_one_call, returned_physical_count_one_call);

    InstWrapper inst2{env->vulkan_functions};
    ASSERT_EQ(CreateInst(inst2, inst_create_info), VK_SUCCESS);

    uint32_t physical_count = driver.physical_devices.size();
    uint32_t returned_physical_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst2->vkEnumeratePhysicalDevices(inst2, &returned_physical_count, nullptr));
    ASSERT_EQ(physical_count, returned_physical_count);

    std::unique_ptr<VkPhysicalDevice[]> physical_device_handles(new VkPhysicalDevice[physical_count]);
    ASSERT_EQ(VK_SUCCESS, inst2->vkEnumeratePhysicalDevices(inst2, &returned_physical_count, physical_device_handles.get()));
    ASSERT_EQ(physical_count, returned_physical_count);

    ASSERT_EQ(returned_physical_count, returned_physical_count);
}

TEST_F(EnumeratePhysicalDevices, TwoCallIncomplete) {
    auto& driver = env->get_test_icd();
    driver.SetMinICDInterfaceVersion(5);

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.emplace_back("physical_device_1");

    InstWrapper inst{env->vulkan_functions};
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);

    uint32_t physical_count = 0;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &physical_count, nullptr));
    ASSERT_EQ(physical_count, driver.physical_devices.size());

    std::unique_ptr<VkPhysicalDevice[]> physical(new VkPhysicalDevice[physical_count]);

    // Remove one from the physical device count so we can get the VK_INCOMPLETE message
    physical_count = 1;

    ASSERT_EQ(VK_INCOMPLETE, inst->vkEnumeratePhysicalDevices(inst, &physical_count, physical.get()));
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

TEST_F(EnumerateInstanceExtensionProperties, PropertyCountLessThanAvailable) {
    uint32_t extension_count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, nullptr));
    ASSERT_EQ(extension_count, 2); //return debug report & debug utils
    extension_count = 1;  // artificially remove one extension

    std::array<VkExtensionProperties, 2> extensions;
    ASSERT_EQ(VK_INCOMPLETE, env->vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, extensions.data()));
    ASSERT_EQ(extension_count, 1);
    // loader always adds the debug report & debug utils extensions
    ASSERT_EQ(strcmp(extensions[0].extensionName, "VK_EXT_debug_report"), 0);
}


TEST_F(EnumerateInstanceExtensionProperties, FilterUnkownInstanceExtensions) {
    Extension first_ext{"FirstTestExtension"};
    Extension second_ext{"SecondTestExtension"};
    env->get_new_test_icd().AddInstanceExtensions({first_ext, second_ext});

    uint32_t extension_count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, nullptr));
    ASSERT_EQ(extension_count, 2); //return debug report & debug utils

    std::array<VkExtensionProperties, 2> extensions;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, extensions.data()));
    ASSERT_EQ(extension_count, 2);
    // loader always adds the debug report & debug utils extensions
    ASSERT_EQ(strcmp(extensions[0].extensionName, "VK_EXT_debug_report"), 0);
    ASSERT_EQ(strcmp(extensions[1].extensionName, "VK_EXT_debug_utils"), 0);
}

TEST_F(EnumerateInstanceExtensionProperties, DisableUnknownInstanceExtensionFiltering) {
    Extension first_ext{"FirstTestExtension"};
    Extension second_ext{"SecondTestExtension"};
    env->get_new_test_icd().AddInstanceExtensions({first_ext, second_ext});

    set_env_var("VK_LOADER_DISABLE_INST_EXT_FILTER","1");

    uint32_t extension_count = 0;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, nullptr));
    ASSERT_EQ(extension_count, 4);

    std::array<VkExtensionProperties, 4> extensions;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkEnumerateInstanceExtensionProperties("", &extension_count, extensions.data()));
    ASSERT_EQ(extension_count, 4);

    ASSERT_EQ(extensions[0], first_ext.get());
    ASSERT_EQ(extensions[1], second_ext.get());
    //Loader always adds these two extensions
    ASSERT_EQ(strcmp(extensions[2].extensionName, "VK_EXT_debug_report"), 0);
    ASSERT_EQ(strcmp(extensions[3].extensionName, "VK_EXT_debug_utils"), 0);
}