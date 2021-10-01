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

struct ImplicitLayer : public CommandLine {};

void test_create_device(VkPhysicalDevice physical_device) {
    uint32_t familyCount = 0;
    VkResult result;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &familyCount, nullptr);
    ASSERT_GT(familyCount, 0u);

    std::unique_ptr<VkQueueFamilyProperties[]> family(new VkQueueFamilyProperties[familyCount]);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &familyCount, family.get());
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
        result = vkCreateDevice(physical_device, deviceInfo, nullptr, &device);
        ASSERT_EQ(result, VK_SUCCESS);

        vkDestroyDevice(device, nullptr);
    }
}

// KEEP
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

int main(int argc, char **argv) {
    int result;

    ::testing::InitGoogleTest(&argc, argv);

    if (argc > 0) {
        CommandLine::Initialize(argc, argv);
    }

    result = RUN_ALL_TESTS();

    return result;
}
