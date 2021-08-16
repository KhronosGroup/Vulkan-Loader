/*
 * Copyright (c) 2015-2017 The Khronos Group Inc.
 * Copyright (c) 2015-2017 Valve Corporation
 * Copyright (c) 2015-2017 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Jeremy Hayes <jeremy@lunarG.com>
 * Author: Mark Young <marky@lunarG.com>
 */

// Following items are needed for C++ to work with PRIxLEAST64
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include <stdint.h>  // For UINT32_MAX

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "test_common.h"
#include <vulkan/vulkan.h>

namespace VK {

struct InstanceCreateInfo {
    InstanceCreateInfo()
        : info  // MSVC can't handle list initialization, thus explicit construction herein.
          (VkInstanceCreateInfo{
              VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,  // sType
              nullptr,                                 // pNext
              0,                                       // flags
              nullptr,                                 // pApplicationInfo
              0,                                       // enabledLayerCount
              nullptr,                                 // ppEnabledLayerNames
              0,                                       // enabledExtensionCount
              nullptr                                  // ppEnabledExtensionNames
          }) {}

    InstanceCreateInfo &sType(VkStructureType const &sType) {
        info.sType = sType;

        return *this;
    }

    InstanceCreateInfo &pNext(void const *const pNext) {
        info.pNext = pNext;

        return *this;
    }

    InstanceCreateInfo &flags(VkInstanceCreateFlags const &flags) {
        info.flags = flags;

        return *this;
    }

    InstanceCreateInfo &pApplicationInfo(VkApplicationInfo const *const pApplicationInfo) {
        info.pApplicationInfo = pApplicationInfo;

        return *this;
    }

    InstanceCreateInfo &enabledLayerCount(uint32_t const &enabledLayerCount) {
        info.enabledLayerCount = enabledLayerCount;

        return *this;
    }

    InstanceCreateInfo &ppEnabledLayerNames(char const *const *const ppEnabledLayerNames) {
        info.ppEnabledLayerNames = ppEnabledLayerNames;

        return *this;
    }

    InstanceCreateInfo &enabledExtensionCount(uint32_t const &enabledExtensionCount) {
        info.enabledExtensionCount = enabledExtensionCount;

        return *this;
    }

    InstanceCreateInfo &ppEnabledExtensionNames(char const *const *const ppEnabledExtensionNames) {
        info.ppEnabledExtensionNames = ppEnabledExtensionNames;

        return *this;
    }

    operator VkInstanceCreateInfo const *() const { return &info; }

    operator VkInstanceCreateInfo *() { return &info; }

    VkInstanceCreateInfo info;
};

struct DeviceQueueCreateInfo {
    DeviceQueueCreateInfo()
        : info  // MSVC can't handle list initialization, thus explicit construction herein.
          (VkDeviceQueueCreateInfo{
              VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,  // sType
              nullptr,                                     // pNext
              0,                                           // flags
              0,                                           // queueFamilyIndex
              0,                                           // queueCount
              nullptr                                      // pQueuePriorities
          }) {}

    DeviceQueueCreateInfo &sType(VkStructureType const &sType) {
        info.sType = sType;

        return *this;
    }

    DeviceQueueCreateInfo &pNext(void const *const pNext) {
        info.pNext = pNext;

        return *this;
    }

    DeviceQueueCreateInfo &flags(VkDeviceQueueCreateFlags const &flags) {
        info.flags = flags;

        return *this;
    }

    DeviceQueueCreateInfo &queueFamilyIndex(uint32_t const &queueFamilyIndex) {
        info.queueFamilyIndex = queueFamilyIndex;

        return *this;
    }

    DeviceQueueCreateInfo &queueCount(uint32_t const &queueCount) {
        info.queueCount = queueCount;

        return *this;
    }

    DeviceQueueCreateInfo &pQueuePriorities(float const *const pQueuePriorities) {
        info.pQueuePriorities = pQueuePriorities;

        return *this;
    }

    operator VkDeviceQueueCreateInfo() { return info; }

    VkDeviceQueueCreateInfo info;
};

struct DeviceCreateInfo {
    DeviceCreateInfo()
        : info  // MSVC can't handle list initialization, thus explicit construction herein.
          (VkDeviceCreateInfo{
              VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,  // sType
              nullptr,                               // pNext
              0,                                     // flags
              0,                                     // queueCreateInfoCount
              nullptr,                               // pQueueCreateInfos
              0,                                     // enabledLayerCount
              nullptr,                               // ppEnabledLayerNames
              0,                                     // enabledExtensionCount
              nullptr,                               // ppEnabledExtensionNames
              nullptr                                // pEnabledFeatures
          }) {}

    DeviceCreateInfo &sType(VkStructureType const &sType) {
        info.sType = sType;

        return *this;
    }

    DeviceCreateInfo &pNext(void const *const pNext) {
        info.pNext = pNext;

        return *this;
    }

    DeviceCreateInfo &flags(VkDeviceQueueCreateFlags const &flags) {
        info.flags = flags;

        return *this;
    }

    DeviceCreateInfo &queueCreateInfoCount(uint32_t const &queueCreateInfoCount) {
        info.queueCreateInfoCount = queueCreateInfoCount;

        return *this;
    }

    DeviceCreateInfo &pQueueCreateInfos(VkDeviceQueueCreateInfo const *const pQueueCreateInfos) {
        info.pQueueCreateInfos = pQueueCreateInfos;

        return *this;
    }

    DeviceCreateInfo &enabledLayerCount(uint32_t const &enabledLayerCount) {
        info.enabledLayerCount = enabledLayerCount;

        return *this;
    }

    DeviceCreateInfo &ppEnabledLayerNames(char const *const *const ppEnabledLayerNames) {
        info.ppEnabledLayerNames = ppEnabledLayerNames;

        return *this;
    }

    DeviceCreateInfo &enabledExtensionCount(uint32_t const &enabledExtensionCount) {
        info.enabledExtensionCount = enabledExtensionCount;

        return *this;
    }

    DeviceCreateInfo &ppEnabledExtensionNames(char const *const *const ppEnabledExtensionNames) {
        info.ppEnabledExtensionNames = ppEnabledExtensionNames;

        return *this;
    }

    DeviceCreateInfo &pEnabledFeatures(VkPhysicalDeviceFeatures const *const pEnabledFeatures) {
        info.pEnabledFeatures = pEnabledFeatures;

        return *this;
    }

    operator VkDeviceCreateInfo const *() const { return &info; }

    operator VkDeviceCreateInfo *() { return &info; }

    VkDeviceCreateInfo info;
};
}  // namespace VK

struct CommandLine : public ::testing::Test {
    static void Initialize(int argc, char **argv) { arguments.assign(argv, argv + argc); };

    static void SetUpTestCase(){};
    static void TearDownTestCase(){};

    static std::vector<std::string> arguments;
};
std::vector<std::string> CommandLine::arguments;

struct EnumerateInstanceLayerProperties : public CommandLine {};
struct EnumerateInstanceExtensionProperties : public CommandLine {};
struct ImplicitLayer : public CommandLine {};

void test_create_device(VkPhysicalDevice physical) {
    uint32_t familyCount = 0;
    VkResult result;
    vkGetPhysicalDeviceQueueFamilyProperties(physical, &familyCount, nullptr);
    ASSERT_GT(familyCount, 0u);

    std::unique_ptr<VkQueueFamilyProperties[]> family(new VkQueueFamilyProperties[familyCount]);
    vkGetPhysicalDeviceQueueFamilyProperties(physical, &familyCount, family.get());
    ASSERT_GT(familyCount, 0u);

    for (uint32_t q = 0; q < familyCount; ++q) {
        if (~family[q].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            continue;
        }

        float const priorities[] = {0.0f};  // Temporary required due to MSVC bug.
        VkDeviceQueueCreateInfo const queueInfo[1]{
            VK::DeviceQueueCreateInfo().queueFamilyIndex(q).queueCount(1).pQueuePriorities(priorities)};

        auto const deviceInfo = VK::DeviceCreateInfo().queueCreateInfoCount(1).pQueueCreateInfos(queueInfo);

        VkDevice device;
        result = vkCreateDevice(physical, deviceInfo, nullptr, &device);
        ASSERT_EQ(result, VK_SUCCESS);

        vkDestroyDevice(device, nullptr);
    }
}

// Test groups:
// LX = lunar exchange
// LVLGH = loader and validation github
// LVLGL = loader and validation gitlab

// Used by run_loader_tests.sh to test for layer insertion.
TEST(CreateInstance, LayerPresent) {
    char const *const names1[] = {"VK_LAYER_LUNARG_test"};      // Temporary required due to MSVC bug.
    char const *const names2[] = {"VK_LAYER_LUNARG_meta"};      // Temporary required due to MSVC bug.
    char const *const names3[] = {"VK_LAYER_LUNARG_meta_rev"};  // Temporary required due to MSVC bug.
    auto const info1 = VK::InstanceCreateInfo().enabledLayerCount(1).ppEnabledLayerNames(names1);
    VkInstance instance = VK_NULL_HANDLE;
    VkResult result = vkCreateInstance(info1, VK_NULL_HANDLE, &instance);
    ASSERT_EQ(result, VK_SUCCESS);
    vkDestroyInstance(instance, nullptr);

    for (auto names : {names2, names3}) {
        auto const info2 = VK::InstanceCreateInfo().enabledLayerCount(1).ppEnabledLayerNames(names);
        instance = VK_NULL_HANDLE;
        result = vkCreateInstance(info2, VK_NULL_HANDLE, &instance);
        ASSERT_EQ(result, VK_SUCCESS);

        uint32_t deviceCount;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        std::vector<VkPhysicalDevice> devs(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devs.data());
        test_create_device(devs[0]);

        vkDestroyInstance(instance, nullptr);
    }
}

TEST_F(EnumerateInstanceLayerProperties, PropertyCountLessThanAvailable) {
    uint32_t count = 0u;
    VkResult result = vkEnumerateInstanceLayerProperties(&count, nullptr);
    ASSERT_EQ(result, VK_SUCCESS);

    // We need atleast two for the test to be relevant.
    if (count < 2u) {
        return;
    }

    std::unique_ptr<VkLayerProperties[]> properties(new VkLayerProperties[count]);
    count = 1;
    result = vkEnumerateInstanceLayerProperties(&count, properties.get());
    ASSERT_EQ(result, VK_INCOMPLETE);
}

TEST(EnumerateDeviceLayerProperties, PropertyCountLessThanAvailable) {
    VkInstance instance = VK_NULL_HANDLE;
    VkResult result = vkCreateInstance(VK::InstanceCreateInfo(), VK_NULL_HANDLE, &instance);
    ASSERT_EQ(result, VK_SUCCESS);

    uint32_t physicalCount = 0;
    result = vkEnumeratePhysicalDevices(instance, &physicalCount, nullptr);
    ASSERT_EQ(result, VK_SUCCESS);
    ASSERT_GT(physicalCount, 0u);

    std::unique_ptr<VkPhysicalDevice[]> physical(new VkPhysicalDevice[physicalCount]);
    result = vkEnumeratePhysicalDevices(instance, &physicalCount, physical.get());
    ASSERT_EQ(result, VK_SUCCESS);
    ASSERT_GT(physicalCount, 0u);

    for (uint32_t p = 0; p < physicalCount; ++p) {
        uint32_t count = 0u;
        result = vkEnumerateDeviceLayerProperties(physical[p], &count, nullptr);
        ASSERT_EQ(result, VK_SUCCESS);

        // We need atleast two for the test to be relevant.
        if (count < 2u) {
            continue;
        }

        std::unique_ptr<VkLayerProperties[]> properties(new VkLayerProperties[count]);
        count = 1;
        result = vkEnumerateDeviceLayerProperties(physical[p], &count, properties.get());
        ASSERT_EQ(result, VK_INCOMPLETE);
    }

    vkDestroyInstance(instance, nullptr);
}

TEST_F(EnumerateInstanceLayerProperties, Count) {
    uint32_t count = 0u;
    VkResult result = vkEnumerateInstanceLayerProperties(&count, nullptr);
    ASSERT_EQ(result, VK_SUCCESS);

    if (std::find(arguments.begin(), arguments.end(), "count") != arguments.end()) {
        std::cout << "count=" << count << '\n';
    }
}

TEST_F(EnumerateInstanceLayerProperties, OnePass) {
    // Count required for this test.
    if (std::find(arguments.begin(), arguments.end(), "count") == arguments.end()) {
        return;
    }

    uint32_t count = std::stoul(arguments[2]);

    std::unique_ptr<VkLayerProperties[]> properties(new VkLayerProperties[count]);
    VkResult result = vkEnumerateInstanceLayerProperties(&count, properties.get());
    ASSERT_EQ(result, VK_SUCCESS);

    if (std::find(arguments.begin(), arguments.end(), "properties") != arguments.end()) {
        for (uint32_t p = 0; p < count; ++p) {
            std::cout << "properties[" << p << "] =" << ' ' << properties[p].layerName << ' ' << properties[p].specVersion << ' '
                      << properties[p].implementationVersion << ' ' << properties[p].description << '\n';
        }
    }
}

TEST_F(EnumerateInstanceLayerProperties, TwoPass) {
    uint32_t count = 0u;
    VkResult result = vkEnumerateInstanceLayerProperties(&count, nullptr);
    ASSERT_EQ(result, VK_SUCCESS);

    std::unique_ptr<VkLayerProperties[]> properties(new VkLayerProperties[count]);
    result = vkEnumerateInstanceLayerProperties(&count, properties.get());
    ASSERT_EQ(result, VK_SUCCESS);

    if (std::find(arguments.begin(), arguments.end(), "properties") != arguments.end()) {
        for (uint32_t p = 0; p < count; ++p) {
            std::cout << "properties[" << p << "] =" << ' ' << properties[p].layerName << ' ' << properties[p].specVersion << ' '
                      << properties[p].implementationVersion << ' ' << properties[p].description << '\n';
        }
    }
}

TEST_F(ImplicitLayer, Present) {
    auto const info = VK::InstanceCreateInfo();
    VkInstance instance = VK_NULL_HANDLE;
    VkResult result = vkCreateInstance(info, VK_NULL_HANDLE, &instance);
    ASSERT_EQ(result, VK_SUCCESS);

    vkDestroyInstance(instance, nullptr);
}

TEST(WrapObjects, Insert) {
    VkInstance instance = VK_NULL_HANDLE;
    VkResult result = vkCreateInstance(VK::InstanceCreateInfo(), VK_NULL_HANDLE, &instance);
    ASSERT_EQ(result, VK_SUCCESS);

    uint32_t physicalCount = 0;
    result = vkEnumeratePhysicalDevices(instance, &physicalCount, nullptr);
    ASSERT_EQ(result, VK_SUCCESS);
    ASSERT_GT(physicalCount, 0u);

    std::unique_ptr<VkPhysicalDevice[]> physical(new VkPhysicalDevice[physicalCount]);
    result = vkEnumeratePhysicalDevices(instance, &physicalCount, physical.get());
    ASSERT_EQ(result, VK_SUCCESS);
    ASSERT_GT(physicalCount, 0u);

    for (uint32_t p = 0; p < physicalCount; ++p) {
        test_create_device(physical[p]);
    }

    vkDestroyInstance(instance, nullptr);
}

// Used by run_loader_tests.sh to test that calling vkEnumeratePhysicalDeviceGroupsKHR without first querying
// the count, works.  And, that it also returns only physical devices made available by the standard
// enumerate call
TEST(EnumeratePhysicalDeviceGroupsKHR, OneCall) {
    VkInstance instance = VK_NULL_HANDLE;
    char const *const names[] = {VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME};
    auto const info = VK::InstanceCreateInfo().enabledExtensionCount(1).ppEnabledExtensionNames(names);
    uint32_t group;
    uint32_t dev;
    std::vector<std::pair<VkPhysicalDevice, bool>> phys_dev_normal_found;
    std::vector<std::pair<VkPhysicalDevice, bool>> phys_dev_group_found;

    VkResult result = vkCreateInstance(info, VK_NULL_HANDLE, &instance);
    if (result == VK_ERROR_EXTENSION_NOT_PRESENT) {
        // Extension isn't present, just skip this test
        ASSERT_EQ(result, VK_ERROR_EXTENSION_NOT_PRESENT);
        std::cout << "Skipping EnumeratePhysicalDeviceGroupsKHR : OneCall due to Instance lacking support"
                  << " for " << VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME << " extension\n";
        return;
    }

    uint32_t phys_dev_count = 500;
    std::unique_ptr<VkPhysicalDevice[]> phys_devs(new VkPhysicalDevice[phys_dev_count]);
    result = vkEnumeratePhysicalDevices(instance, &phys_dev_count, phys_devs.get());
    ASSERT_EQ(result, VK_SUCCESS);
    ASSERT_GT(phys_dev_count, 0u);

    // Initialize the normal physical device boolean pair array
    for (dev = 0; dev < phys_dev_count; dev++) {
        phys_dev_normal_found.push_back(std::make_pair(phys_devs[dev], false));
    }

    // Get a pointer to the new vkEnumeratePhysicalDeviceGroupsKHR call
    PFN_vkEnumeratePhysicalDeviceGroupsKHR p_vkEnumeratePhysicalDeviceGroupsKHR =
        (PFN_vkEnumeratePhysicalDeviceGroupsKHR)vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDeviceGroupsKHR");

    // Setup the group information in preparation for the call
    uint32_t group_count = 30;
    std::unique_ptr<VkPhysicalDeviceGroupPropertiesKHR[]> phys_dev_groups(new VkPhysicalDeviceGroupPropertiesKHR[group_count]);
    for (group = 0; group < group_count; group++) {
        phys_dev_groups[group].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES_KHR;
        phys_dev_groups[group].pNext = nullptr;
        phys_dev_groups[group].physicalDeviceCount = 0;
        memset(phys_dev_groups[group].physicalDevices, 0, sizeof(VkPhysicalDevice) * VK_MAX_DEVICE_GROUP_SIZE_KHR);
        phys_dev_groups[group].subsetAllocation = VK_FALSE;
    }

    result = p_vkEnumeratePhysicalDeviceGroupsKHR(instance, &group_count, phys_dev_groups.get());
    ASSERT_EQ(result, VK_SUCCESS);
    ASSERT_GT(group_count, 0u);

    // Initialize the group physical device boolean pair array
    for (group = 0; group < group_count; group++) {
        for (dev = 0; dev < phys_dev_groups[group].physicalDeviceCount; dev++) {
            phys_dev_group_found.push_back(std::make_pair(phys_dev_groups[group].physicalDevices[dev], false));
        }
    }

    // Now, make sure we can find each normal and group item in the other list
    for (dev = 0; dev < phys_dev_count; dev++) {
        for (group = 0; group < phys_dev_group_found.size(); group++) {
            if (phys_dev_normal_found[dev].first == phys_dev_group_found[group].first) {
                phys_dev_normal_found[dev].second = true;
                phys_dev_group_found[group].second = true;
                break;
            }
        }
    }

    for (dev = 0; dev < phys_dev_count; dev++) {
        ASSERT_EQ(phys_dev_normal_found[dev].second, true);
    }
    for (dev = 0; dev < phys_dev_group_found.size(); dev++) {
        ASSERT_EQ(phys_dev_group_found[dev].second, true);
    }

    vkDestroyInstance(instance, nullptr);
}

// Used by run_loader_tests.sh to test for the expected usage of the
// vkEnumeratePhysicalDeviceGroupsKHR call in a two call fashion (once with NULL data
// to get count, and then again with data).
TEST(EnumeratePhysicalDeviceGroupsKHR, TwoCall) {
    VkInstance instance = VK_NULL_HANDLE;
    char const *const names[] = {VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME};
    auto const info = VK::InstanceCreateInfo().enabledExtensionCount(1).ppEnabledExtensionNames(names);
    uint32_t group;
    uint32_t group_count;
    uint32_t dev;
    std::vector<std::pair<VkPhysicalDevice, bool>> phys_dev_normal_found;
    std::vector<std::pair<VkPhysicalDevice, bool>> phys_dev_group_found;

    VkResult result = vkCreateInstance(info, VK_NULL_HANDLE, &instance);
    if (result == VK_ERROR_EXTENSION_NOT_PRESENT) {
        // Extension isn't present, just skip this test
        ASSERT_EQ(result, VK_ERROR_EXTENSION_NOT_PRESENT);
        std::cout << "Skipping EnumeratePhysicalDeviceGroupsKHR : TwoCall due to Instance lacking support"
                  << " for " << VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME << " extension\n";
        return;
    }

    // Get a pointer to the new vkEnumeratePhysicalDeviceGroupsKHR call
    PFN_vkEnumeratePhysicalDeviceGroupsKHR p_vkEnumeratePhysicalDeviceGroupsKHR =
        (PFN_vkEnumeratePhysicalDeviceGroupsKHR)vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDeviceGroupsKHR");

    // Setup the group information in preparation for the call
    uint32_t array_group_count = 30;
    std::unique_ptr<VkPhysicalDeviceGroupPropertiesKHR[]> phys_dev_groups(
        new VkPhysicalDeviceGroupPropertiesKHR[array_group_count]);
    for (group = 0; group < array_group_count; group++) {
        phys_dev_groups[group].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES_KHR;
        phys_dev_groups[group].pNext = nullptr;
        phys_dev_groups[group].physicalDeviceCount = 0;
        memset(phys_dev_groups[group].physicalDevices, 0, sizeof(VkPhysicalDevice) * VK_MAX_DEVICE_GROUP_SIZE_KHR);
        phys_dev_groups[group].subsetAllocation = VK_FALSE;
    }

    result = p_vkEnumeratePhysicalDeviceGroupsKHR(instance, &group_count, nullptr);
    ASSERT_EQ(result, VK_SUCCESS);
    ASSERT_GT(group_count, 0u);
    ASSERT_LT(group_count, array_group_count);

    result = p_vkEnumeratePhysicalDeviceGroupsKHR(instance, &group_count, phys_dev_groups.get());
    ASSERT_EQ(result, VK_SUCCESS);
    ASSERT_GT(group_count, 0u);
    ASSERT_LT(group_count, array_group_count);

    // Initialize the group physical device boolean pair array
    for (group = 0; group < group_count; group++) {
        for (dev = 0; dev < phys_dev_groups[group].physicalDeviceCount; dev++) {
            phys_dev_group_found.push_back(std::make_pair(phys_dev_groups[group].physicalDevices[dev], false));
        }
    }

    uint32_t phys_dev_count = 500;
    std::unique_ptr<VkPhysicalDevice[]> phys_devs(new VkPhysicalDevice[phys_dev_count]);
    result = vkEnumeratePhysicalDevices(instance, &phys_dev_count, phys_devs.get());
    ASSERT_EQ(result, VK_SUCCESS);
    ASSERT_GT(phys_dev_count, 0u);

    // Initialize the normal physical device boolean pair array
    for (dev = 0; dev < phys_dev_count; dev++) {
        phys_dev_normal_found.push_back(std::make_pair(phys_devs[dev], false));
    }

    // Now, make sure we can find each normal and group item in the other list
    for (dev = 0; dev < phys_dev_count; dev++) {
        for (group = 0; group < phys_dev_group_found.size(); group++) {
            if (phys_dev_normal_found[dev].first == phys_dev_group_found[group].first) {
                phys_dev_normal_found[dev].second = true;
                phys_dev_group_found[group].second = true;
                break;
            }
        }
    }

    for (dev = 0; dev < phys_dev_count; dev++) {
        ASSERT_EQ(phys_dev_normal_found[dev].second, true);
    }
    for (dev = 0; dev < phys_dev_group_found.size(); dev++) {
        ASSERT_EQ(phys_dev_group_found[dev].second, true);
    }

    vkDestroyInstance(instance, nullptr);
}

// Used by run_loader_tests.sh to test for the expected usage of the EnumeratePhysicalDeviceGroupsKHR
// call if not enough numbers are provided for the final list.
TEST(EnumeratePhysicalDeviceGroupsKHR, TwoCallIncomplete) {
    VkInstance instance = VK_NULL_HANDLE;
    char const *const names[] = {VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME};
    auto const info = VK::InstanceCreateInfo().enabledExtensionCount(1).ppEnabledExtensionNames(names);
    uint32_t group;
    uint32_t group_count;

    VkResult result = vkCreateInstance(info, VK_NULL_HANDLE, &instance);
    if (result == VK_ERROR_EXTENSION_NOT_PRESENT) {
        // Extension isn't present, just skip this test
        ASSERT_EQ(result, VK_ERROR_EXTENSION_NOT_PRESENT);
        std::cout << "Skipping EnumeratePhysicalDeviceGroupsKHR : TwoCallIncomplete due to Instance lacking support"
                  << " for " << VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME << " extension\n";
        return;
    }

    // Get a pointer to the new vkEnumeratePhysicalDeviceGroupsKHR call
    PFN_vkEnumeratePhysicalDeviceGroupsKHR p_vkEnumeratePhysicalDeviceGroupsKHR =
        (PFN_vkEnumeratePhysicalDeviceGroupsKHR)vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDeviceGroupsKHR");

    // Setup the group information in preparation for the call
    uint32_t array_group_count = 30;
    std::unique_ptr<VkPhysicalDeviceGroupPropertiesKHR[]> phys_dev_groups(
        new VkPhysicalDeviceGroupPropertiesKHR[array_group_count]);
    for (group = 0; group < array_group_count; group++) {
        phys_dev_groups[group].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES_KHR;
        phys_dev_groups[group].pNext = nullptr;
        phys_dev_groups[group].physicalDeviceCount = 0;
        memset(phys_dev_groups[group].physicalDevices, 0, sizeof(VkPhysicalDevice) * VK_MAX_DEVICE_GROUP_SIZE_KHR);
        phys_dev_groups[group].subsetAllocation = VK_FALSE;
    }

    result = p_vkEnumeratePhysicalDeviceGroupsKHR(instance, &group_count, nullptr);
    ASSERT_EQ(result, VK_SUCCESS);
    ASSERT_GT(group_count, 0u);
    ASSERT_LT(group_count, array_group_count);

    group_count -= 1;

    result = p_vkEnumeratePhysicalDeviceGroupsKHR(instance, &group_count, phys_dev_groups.get());
    ASSERT_EQ(result, VK_INCOMPLETE);

    vkDestroyInstance(instance, nullptr);
}

int main(int argc, char **argv) {
    int result;

    ::testing::InitGoogleTest(&argc, argv);

    if (argc > 0) {
        CommandLine::Initialize(argc, argv);
    }

    result = RUN_ALL_TESTS();

    return result;
}
