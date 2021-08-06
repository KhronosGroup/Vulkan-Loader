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

// Move only type because it holds a DispatchableHandle<VkPhysicalDevice>
struct PhysicalDevice {
    PhysicalDevice() {}
    PhysicalDevice(std::string name) : deviceName(name) {}
    PhysicalDevice(const char* name) : deviceName(name) {}
    PhysicalDevice& set_properties(VkPhysicalDeviceProperties properties) {
        this->properties = properties;
        return *this;
    }
    PhysicalDevice& set_features(VkPhysicalDeviceFeatures features) {
        this->features = features;
        return *this;
    }
    PhysicalDevice& set_memory_properties(VkPhysicalDeviceMemoryProperties memory_properties) {
        this->memory_properties = memory_properties;
        return *this;
    }
    PhysicalDevice& add_queue_family_properties(VkQueueFamilyProperties properties, bool support_present = true) {
        queue_family_properties.push_back(MockQueueFamilyProperties(properties, support_present));
        return *this;
    }
    PhysicalDevice& add_queue_family_properties(MockQueueFamilyProperties properties) {
        queue_family_properties.push_back(properties);
        return *this;
    }
    DispatchableHandle<VkPhysicalDevice> vk_physical_device;
    std::string deviceName;
    VkPhysicalDeviceProperties properties{};
    VkPhysicalDeviceFeatures features{};
    VkPhysicalDeviceMemoryProperties memory_properties{};
    std::vector<MockQueueFamilyProperties> queue_family_properties;
    std::vector<VkFormatProperties> format_properties;

    std::vector<Extension> extensions;

    VkSurfaceCapabilitiesKHR surface_capabilities;
    std::vector<VkSurfaceFormatKHR> surface_formats;
    std::vector<VkPresentModeKHR> surface_present_modes;

    // VkDevice handles created from this physical device
    std::vector<VkDevice> device_handles;

    // List of function names which are 'known' to the physical device but have test defined implementations
    // The purpose of this list is so that vkGetDeviceProcAddr returns 'a real function pointer' in tests
    // without actually implementing any of the logic inside of it.
    std::vector<VulkanFunction> known_device_functions;
};

struct PhysicalDeviceGroup {
    PhysicalDeviceGroup& use_physical_device(PhysicalDevice const& physical_device) {
        physical_device_handles.push_back(&physical_device);
        return *this;
    }

    std::vector<PhysicalDevice const*> physical_device_handles;
    VkBool32 subset_allocation = false;
};