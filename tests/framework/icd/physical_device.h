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

    DispatchableHandle<VkPhysicalDevice> vk_physical_device;
    BUILDER_VALUE(PhysicalDevice, std::string, deviceName, "")
    BUILDER_VALUE(PhysicalDevice, VkPhysicalDeviceProperties, properties, {})
    BUILDER_VALUE(PhysicalDevice, VkPhysicalDeviceFeatures, features, {})
    BUILDER_VALUE(PhysicalDevice, VkPhysicalDeviceMemoryProperties, memory_properties, {})

    BUILDER_VECTOR(PhysicalDevice, MockQueueFamilyProperties, queue_family_properties, queue_family_properties)
    BUILDER_VECTOR(PhysicalDevice, VkFormatProperties, format_properties, format_properties)

    BUILDER_VECTOR(PhysicalDevice, Extension, extensions, extension)

    BUILDER_VALUE(PhysicalDevice, VkSurfaceCapabilitiesKHR, surface_capabilities, {})
    BUILDER_VECTOR(PhysicalDevice, VkSurfaceFormatKHR, surface_formats, surface_format)
    BUILDER_VECTOR(PhysicalDevice, VkPresentModeKHR, surface_present_modes, surface_present_mode)

    // VkDevice handles created from this physical device
    std::vector<VkDevice> device_handles;

    // List of function names which are 'known' to the physical device but have test defined implementations
    // The purpose of this list is so that vkGetDeviceProcAddr returns 'a real function pointer' in tests
    // without actually implementing any of the logic inside of it.
    BUILDER_VECTOR(PhysicalDevice, VulkanFunction, known_device_functions, device_function)
};

struct PhysicalDeviceGroup {
    PhysicalDeviceGroup& use_physical_device(PhysicalDevice const& physical_device) {
        physical_device_handles.push_back(&physical_device);
        return *this;
    }

    std::vector<PhysicalDevice const*> physical_device_handles;
    VkBool32 subset_allocation = false;
};