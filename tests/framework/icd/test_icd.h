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

#include "physical_device.h"

enum class CalledICDGIPA { not_called, vk_icd_gipa, vk_gipa };

enum class CalledNegotiateInterface { not_called, vk_icd_negotiate, vk_icd_gipa_first };

enum class InterfaceVersionCheck {
    not_called,
    loader_version_too_old,
    loader_version_too_new,
    icd_version_too_new,
    version_is_supported
};

enum class CalledEnumerateAdapterPhysicalDevices { not_called, called, called_but_not_supported };

enum class UsingICDProvidedWSI { not_using, is_using };

struct TestICD {
    fs::path manifest_file_path;

    CalledICDGIPA called_vk_icd_gipa = CalledICDGIPA::not_called;
    CalledNegotiateInterface called_negotiate_interface = CalledNegotiateInterface::not_called;

    InterfaceVersionCheck interface_version_check = InterfaceVersionCheck::not_called;
    BUILDER_VALUE(TestICD, uint32_t, min_icd_interface_version, 0)
    BUILDER_VALUE(TestICD, uint32_t, max_icd_interface_version, 6)
    uint32_t icd_interface_version_received = 0;

    CalledEnumerateAdapterPhysicalDevices called_enumerate_adapter_physical_devices =
        CalledEnumerateAdapterPhysicalDevices::not_called;

    BUILDER_VALUE(TestICD, bool, enable_icd_wsi, false);
    UsingICDProvidedWSI is_using_icd_wsi = UsingICDProvidedWSI::not_using;

    BUILDER_VALUE(TestICD, uint32_t, icd_api_version, VK_MAKE_VERSION(1, 0, 0))
    BUILDER_VECTOR(TestICD, LayerDefinition, instance_layers, instance_layer)
    BUILDER_VECTOR(TestICD, Extension, instance_extensions, instance_extension)

    BUILDER_VECTOR_MOVE_ONLY(TestICD, PhysicalDevice, physical_devices, physical_device);

    BUILDER_VECTOR(TestICD, PhysicalDeviceGroup, physical_device_groups, physical_device_group);

    DispatchableHandle<VkInstance> instance_handle;
    std::vector<DispatchableHandle<VkDevice>> device_handles;
    std::vector<uint64_t> surface_handles;
    std::vector<uint64_t> messenger_handles;
    std::vector<uint64_t> swapchain_handles;

    // Unknown instance and physical device functions. Add a `VulkanFunction` to this list which will be searched in
    // vkGetInstanceProcAddr for custom_instance_functions and vk_icdGetPhysicalDeviceProcAddr for custom_physical_device_functions.
    // To add unknown device functions, add it to the PhysicalDevice directly (in the known_device_functions member)
    BUILDER_VECTOR(TestICD, VulkanFunction, custom_instance_functions, custom_instance_function)
    BUILDER_VECTOR(TestICD, VulkanFunction, custom_physical_device_functions, custom_physical_device_function)

    // Must explicitely state support for the tooling info extension, that way we can control if vkGetInstanceProcAddr returns a
    // function pointer for vkGetPhysicalDeviceToolPropertiesEXT
    BUILDER_VALUE(TestICD, bool, supports_tooling_info_ext, false)
    // List of tooling properties that this driver 'supports'
    std::vector<VkPhysicalDeviceToolPropertiesEXT> tooling_properties;

    PhysicalDevice& GetPhysDevice(VkPhysicalDevice physicalDevice) {
        for (auto& phys_dev : physical_devices) {
            if (phys_dev.vk_physical_device.handle == physicalDevice) return phys_dev;
        }
        assert(false && "vkPhysicalDevice not found!");
        return physical_devices[0];
    }

    InstanceCreateInfo GetVkInstanceCreateInfo() {
        InstanceCreateInfo info;
        for (auto& layer : instance_layers) info.enabled_layers.push_back(layer.layerName.data());
        for (auto& ext : instance_extensions) info.enabled_extensions.push_back(ext.extensionName.data());
        return info;
    }
};

using GetTestICDFunc = TestICD* (*)();
#define GET_TEST_ICD_FUNC_STR "get_test_icd_func"

using GetNewTestICDFunc = TestICD* (*)();
#define RESET_ICD_FUNC_STR "reset_icd_func"