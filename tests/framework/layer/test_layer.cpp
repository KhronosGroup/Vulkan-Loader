/*
 * Copyright (c) 2021-2022 The Khronos Group Inc.
 * Copyright (c) 2021-2022 Valve Corporation
 * Copyright (c) 2021-2022 LunarG, Inc.
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

#include "test_layer.h"

#include "loader/generated/vk_dispatch_table_helper.h"

// export the enumeration functions instance|device+layer|extension
#ifndef TEST_LAYER_EXPORT_ENUMERATE_FUNCTIONS
#define TEST_LAYER_EXPORT_ENUMERATE_FUNCTIONS 0
#endif

// export test_layer_GetInstanceProcAddr
#ifndef TEST_LAYER_EXPORT_LAYER_NAMED_GIPA
#define TEST_LAYER_EXPORT_LAYER_NAMED_GIPA 0
#endif

// export vkGetInstanceProcAddr
#ifndef TEST_LAYER_EXPORT_LAYER_VK_GIPA
#define TEST_LAYER_EXPORT_LAYER_VK_GIPA 0
#endif

// export test_layer_GetDeviceProcAddr
#ifndef TEST_LAYER_EXPORT_LAYER_NAMED_GDPA
#define TEST_LAYER_EXPORT_LAYER_NAMED_GDPA 0
#endif

// export vkGetDeviceProcAddr
#ifndef TEST_LAYER_EXPORT_LAYER_VK_GDPA
#define TEST_LAYER_EXPORT_LAYER_VK_GDPA 0
#endif

// export GetInstanceProcAddr
#ifndef TEST_LAYER_EXPORT_NO_PREFIX_GIPA
#define TEST_LAYER_EXPORT_NO_PREFIX_GIPA 0
#endif

// export GetDeviceProcAddr
#ifndef TEST_LAYER_EXPORT_NO_PREFIX_GDPA
#define TEST_LAYER_EXPORT_NO_PREFIX_GDPA 0
#endif

// export vk_layerGetPhysicalDeviceProcAddr
#ifndef TEST_LAYER_EXPORT_GET_PHYSICAL_DEVICE_PROC_ADDR
#define TEST_LAYER_EXPORT_GET_PHYSICAL_DEVICE_PROC_ADDR 0
#endif

// export vkNegotiateLoaderLayerInterfaceVersion
#ifndef LAYER_EXPORT_NEGOTIATE_LOADER_LAYER_INTERFACE_VERSION
#define LAYER_EXPORT_NEGOTIATE_LOADER_LAYER_INTERFACE_VERSION 0
#endif

#ifndef TEST_LAYER_NAME
#define TEST_LAYER_NAME "VkLayer_LunarG_test_layer"
#endif

TestLayer layer;
extern "C" {
FRAMEWORK_EXPORT TestLayer* get_test_layer_func() { return &layer; }
FRAMEWORK_EXPORT TestLayer* reset_layer_func() {
    layer.~TestLayer();
    return new (&layer) TestLayer();
}
}

VkLayerInstanceCreateInfo* get_chain_info(const VkInstanceCreateInfo* pCreateInfo, VkLayerFunction func) {
    VkLayerInstanceCreateInfo* chain_info = (VkLayerInstanceCreateInfo*)pCreateInfo->pNext;
    while (chain_info && !(chain_info->sType == VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO && chain_info->function == func)) {
        chain_info = (VkLayerInstanceCreateInfo*)chain_info->pNext;
    }
    assert(chain_info != NULL);
    return chain_info;
}

VkLayerDeviceCreateInfo* get_chain_info(const VkDeviceCreateInfo* pCreateInfo, VkLayerFunction func) {
    VkLayerDeviceCreateInfo* chain_info = (VkLayerDeviceCreateInfo*)pCreateInfo->pNext;
    while (chain_info && !(chain_info->sType == VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO && chain_info->function == func)) {
        chain_info = (VkLayerDeviceCreateInfo*)chain_info->pNext;
    }
    assert(chain_info != NULL);
    return chain_info;
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkEnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties) {
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkEnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount,
                                                                           VkExtensionProperties* pProperties) {
    if (pLayerName && string_eq(pLayerName, TEST_LAYER_NAME)) {
        *pPropertyCount = 0;
        return VK_SUCCESS;
    }
    return layer.instance_dispatch_table.EnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount,
                                                                     VkLayerProperties* pProperties) {
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char* pLayerName,
                                                                         uint32_t* pPropertyCount,
                                                                         VkExtensionProperties* pProperties) {
    if (pLayerName && string_eq(pLayerName, TEST_LAYER_NAME)) {
        *pPropertyCount = 0;
        return VK_SUCCESS;
    }
    return layer.instance_dispatch_table.EnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pPropertyCount,
                                                                            pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkEnumerateInstanceVersion(uint32_t* pApiVersion) {
    if (pApiVersion != nullptr) {
        *pApiVersion = VK_API_VERSION_1_0;
    }
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo,
                                                     const VkAllocationCallbacks* pAllocator, VkInstance* pInstance) {
    VkLayerInstanceCreateInfo* chain_info = get_chain_info(pCreateInfo, VK_LAYER_LINK_INFO);

    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr = chain_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vk_icdGetPhysicalDeviceProcAddr fpGetPhysicalDeviceProcAddr = chain_info->u.pLayerInfo->pfnNextGetPhysicalDeviceProcAddr;
    PFN_vkCreateInstance fpCreateInstance = (PFN_vkCreateInstance)fpGetInstanceProcAddr(NULL, "vkCreateInstance");
    if (fpCreateInstance == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // Advance the link info for the next element of the chain
    chain_info->u.pLayerInfo = chain_info->u.pLayerInfo->pNext;
    layer.next_vkGetInstanceProcAddr = fpGetInstanceProcAddr;

    bool use_modified_create_info = false;
    VkInstanceCreateInfo instance_create_info{};
    VkApplicationInfo application_info{};
    if (pCreateInfo) {
        instance_create_info = *pCreateInfo;
        if (pCreateInfo->pApplicationInfo) {
            application_info = *pCreateInfo->pApplicationInfo;
        }
    }

    // If the test needs to modify the api version, do it before we call down the chain
    if (layer.alter_api_version != VK_API_VERSION_1_0 && pCreateInfo && pCreateInfo->pApplicationInfo) {
        application_info.apiVersion = layer.alter_api_version;
        instance_create_info.pApplicationInfo = &application_info;
        use_modified_create_info = true;
    }
    const VkInstanceCreateInfo* create_info_pointer = use_modified_create_info ? &instance_create_info : pCreateInfo;

    // Continue call down the chain
    VkResult result = fpCreateInstance(create_info_pointer, pAllocator, pInstance);
    if (result != VK_SUCCESS) {
        return result;
    }
    layer.instance_handle = *pInstance;
    if (layer.use_gipa_GetPhysicalDeviceProcAddr) {
        layer.next_GetPhysicalDeviceProcAddr =
            reinterpret_cast<PFN_GetPhysicalDeviceProcAddr>(fpGetInstanceProcAddr(*pInstance, "vk_layerGetPhysicalDeviceProcAddr"));
    } else {
        layer.next_GetPhysicalDeviceProcAddr = fpGetPhysicalDeviceProcAddr;
    }
    // Init layer's dispatch table using GetInstanceProcAddr of
    // next layer in the chain.
    layer_init_instance_dispatch_table(layer.instance_handle, &layer.instance_dispatch_table, fpGetInstanceProcAddr);

    if (layer.create_instance_callback) result = layer.create_instance_callback(layer);

    for (auto& func : layer.custom_physical_device_interception_functions) {
        auto next_func = layer.next_GetPhysicalDeviceProcAddr(*pInstance, func.name.c_str());
        layer.custom_dispatch_functions.at(func.name.c_str()) = next_func;
    }

    for (auto& func : layer.custom_device_interception_functions) {
        auto next_func = layer.next_vkGetInstanceProcAddr(*pInstance, func.name.c_str());
        layer.custom_dispatch_functions.at(func.name.c_str()) = next_func;
    }

    if (layer.do_spurious_allocations_in_create_instance && pAllocator && pAllocator->pfnAllocation) {
        layer.spurious_instance_memory_allocation =
            pAllocator->pfnAllocation(pAllocator->pUserData, 100, 8, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (layer.spurious_instance_memory_allocation == nullptr) {
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }
    }

    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL test_override_vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo,
                                                              const VkAllocationCallbacks* pAllocator, VkInstance* pInstance) {
    return VK_ERROR_INVALID_SHADER_NV;
}

VKAPI_ATTR void VKAPI_CALL test_vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator) {
    if (layer.spurious_instance_memory_allocation && pAllocator && pAllocator->pfnFree) {
        pAllocator->pfnFree(pAllocator->pUserData, layer.spurious_instance_memory_allocation);
        layer.spurious_instance_memory_allocation = nullptr;
    }

    layer.instance_dispatch_table.DestroyInstance(instance, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo,
                                                   const VkAllocationCallbacks* pAllocator, VkDevice* pDevice) {
    VkLayerDeviceCreateInfo* chain_info = get_chain_info(pCreateInfo, VK_LAYER_LINK_INFO);

    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr = chain_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkGetDeviceProcAddr fpGetDeviceProcAddr = chain_info->u.pLayerInfo->pfnNextGetDeviceProcAddr;
    PFN_vkCreateDevice fpCreateDevice = (PFN_vkCreateDevice)fpGetInstanceProcAddr(layer.instance_handle, "vkCreateDevice");
    if (fpCreateDevice == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    layer.next_vkGetDeviceProcAddr = fpGetDeviceProcAddr;

    // Advance the link info for the next element on the chain
    chain_info->u.pLayerInfo = chain_info->u.pLayerInfo->pNext;

    VkResult result = fpCreateDevice(physicalDevice, pCreateInfo, pAllocator, pDevice);
    if (result != VK_SUCCESS) {
        return result;
    }
    TestLayer::Device device{};
    device.device_handle = *pDevice;

    // initialize layer's dispatch table
    layer_init_device_dispatch_table(device.device_handle, &device.dispatch_table, fpGetDeviceProcAddr);

    for (auto& func : layer.custom_device_interception_functions) {
        auto next_func = layer.next_vkGetDeviceProcAddr(*pDevice, func.name.c_str());
        layer.custom_dispatch_functions.at(func.name.c_str()) = next_func;
    }

    if (layer.create_device_callback) {
        result = layer.create_device_callback(layer);
    }

    // Need to add the created devices to the list so it can be freed
    layer.created_devices.push_back(device);

    if (layer.do_spurious_allocations_in_create_device && pAllocator && pAllocator->pfnAllocation) {
        void* allocation = pAllocator->pfnAllocation(pAllocator->pUserData, 110, 8, VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);
        if (allocation == nullptr) {
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        } else {
            layer.spurious_device_memory_allocations.push_back({allocation, device.device_handle});
        }
    }

    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkEnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount,
                                                               VkPhysicalDevice* pPhysicalDevices) {
    if (layer.add_phys_devs || layer.remove_phys_devs || layer.reorder_phys_devs) {
        VkResult res = VK_SUCCESS;

        if (layer.complete_physical_devices.size() == 0) {
            // Get list of all physical devices from lower down
            // NOTE: This only works if we don't test changing the number of devices
            //       underneath us when using this test.
            uint32_t icd_count = 0;
            layer.instance_dispatch_table.EnumeratePhysicalDevices(instance, &icd_count, nullptr);
            std::vector<VkPhysicalDevice> tmp_vector;
            tmp_vector.resize(icd_count);
            layer.instance_dispatch_table.EnumeratePhysicalDevices(instance, &icd_count, tmp_vector.data());
            layer.complete_physical_devices.clear();

            if (layer.remove_phys_devs) {
                // Erase the 3rd and 4th items
                layer.removed_physical_devices.push_back(tmp_vector[3]);
                layer.removed_physical_devices.push_back(tmp_vector[4]);
                tmp_vector.erase(tmp_vector.begin() + 3);
                tmp_vector.erase(tmp_vector.begin() + 3);
            }

            if (layer.add_phys_devs) {
                // Insert a new device in the beginning, middle, and end
                uint32_t middle = static_cast<uint32_t>(tmp_vector.size() / 2);
                VkPhysicalDevice new_phys_dev = reinterpret_cast<VkPhysicalDevice>((size_t)(0xABCD0000));
                layer.added_physical_devices.push_back(new_phys_dev);
                tmp_vector.insert(tmp_vector.begin(), new_phys_dev);
                new_phys_dev = reinterpret_cast<VkPhysicalDevice>((size_t)(0xBADC0000));
                layer.added_physical_devices.push_back(new_phys_dev);
                tmp_vector.insert(tmp_vector.begin() + middle, new_phys_dev);
                new_phys_dev = reinterpret_cast<VkPhysicalDevice>((size_t)(0xDCBA0000));
                layer.added_physical_devices.push_back(new_phys_dev);
                tmp_vector.push_back(new_phys_dev);
            }

            if (layer.reorder_phys_devs) {
                // Flip the order of items
                for (int32_t dev = static_cast<int32_t>(tmp_vector.size() - 1); dev >= 0; --dev) {
                    layer.complete_physical_devices.push_back(tmp_vector[dev]);
                }
            } else {
                // Otherwise, keep the order the same
                for (uint32_t dev = 0; dev < tmp_vector.size(); ++dev) {
                    layer.complete_physical_devices.push_back(tmp_vector[dev]);
                }
            }
        }

        if (nullptr == pPhysicalDevices) {
            *pPhysicalDeviceCount = static_cast<uint32_t>(layer.complete_physical_devices.size());
        } else {
            uint32_t adj_count = static_cast<uint32_t>(layer.complete_physical_devices.size());
            if (*pPhysicalDeviceCount < adj_count) {
                adj_count = *pPhysicalDeviceCount;
                res = VK_INCOMPLETE;
            }
            for (uint32_t dev = 0; dev < adj_count; ++dev) {
                pPhysicalDevices[dev] = layer.complete_physical_devices[dev];
            }
            *pPhysicalDeviceCount = adj_count;
        }

        return res;
    } else {
        return layer.instance_dispatch_table.EnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);
    }
}

VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice,
                                                              VkPhysicalDeviceProperties* pProperties) {
    if (std::find(layer.removed_physical_devices.begin(), layer.removed_physical_devices.end(), physicalDevice) !=
        layer.removed_physical_devices.end()) {
        // Should not get here since the application should not know about those devices
        assert(false);
    } else if (std::find(layer.added_physical_devices.begin(), layer.added_physical_devices.end(), physicalDevice) !=
               layer.added_physical_devices.end()) {
        // Added device so put in some placeholder info we can test against
        pProperties->apiVersion = VK_API_VERSION_1_2;
        pProperties->driverVersion = VK_MAKE_API_VERSION(0, 12, 14, 196);
        pProperties->vendorID = 0xDECAFBAD;
        pProperties->deviceID = 0xDEADBADD;
#if defined(_WIN32)
        strncpy_s(pProperties->deviceName, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE, "physdev_added_xx", 17);
#else
        strncpy(pProperties->deviceName, "physdev_added_xx", VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);
#endif
    } else {
        // Not an affected device so just return
        layer.instance_dispatch_table.GetPhysicalDeviceProperties(physicalDevice, pProperties);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkEnumeratePhysicalDeviceGroups(
    VkInstance instance, uint32_t* pPhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties) {
    if (layer.add_phys_devs || layer.remove_phys_devs || layer.reorder_phys_devs) {
        VkResult res = VK_SUCCESS;

        if (layer.complete_physical_device_groups.size() == 0) {
            uint32_t fake_count = 1000;
            // Call EnumerateDevices to add remove devices as needed
            test_vkEnumeratePhysicalDevices(instance, &fake_count, nullptr);

            // Get list of all physical devices from lower down
            // NOTE: This only works if we don't test changing the number of devices
            //       underneath us when using this test.
            uint32_t icd_group_count = 0;
            layer.instance_dispatch_table.EnumeratePhysicalDeviceGroups(instance, &icd_group_count, nullptr);
            std::vector<VkPhysicalDeviceGroupProperties> tmp_vector(icd_group_count);
            for (uint32_t group = 0; group < icd_group_count; ++group) {
                tmp_vector[group].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
            }
            layer.instance_dispatch_table.EnumeratePhysicalDeviceGroups(instance, &icd_group_count, tmp_vector.data());
            layer.complete_physical_device_groups.clear();

            if (layer.remove_phys_devs) {
                // Now, if a device has been removed, and it was the only group, we need to remove the group as well.
                for (uint32_t rem_dev = 0; rem_dev < layer.removed_physical_devices.size(); ++rem_dev) {
                    for (uint32_t group = 0; group < icd_group_count; ++group) {
                        for (uint32_t grp_dev = 0; grp_dev < tmp_vector[group].physicalDeviceCount; ++grp_dev) {
                            if (tmp_vector[group].physicalDevices[grp_dev] == layer.removed_physical_devices[rem_dev]) {
                                for (uint32_t cp_item = grp_dev + 1; cp_item < tmp_vector[group].physicalDeviceCount; ++cp_item) {
                                    tmp_vector[group].physicalDevices[grp_dev] = tmp_vector[group].physicalDevices[cp_item];
                                }
                                tmp_vector[group].physicalDeviceCount--;
                            }
                        }
                    }
                }
                for (uint32_t group = 0; group < tmp_vector.size(); ++group) {
                    if (tmp_vector[group].physicalDeviceCount == 0) {
                        layer.removed_physical_device_groups.push_back(tmp_vector[group]);
                        tmp_vector.erase(tmp_vector.begin() + group);
                        --group;
                    }
                }
            }

            if (layer.add_phys_devs) {
                // Add a new group for each physical device not associated with a current group
                for (uint32_t dev = 0; dev < layer.added_physical_devices.size(); ++dev) {
                    VkPhysicalDeviceGroupProperties props{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES};
                    props.physicalDeviceCount = 1;
                    props.physicalDevices[0] = layer.added_physical_devices[dev];
                    tmp_vector.push_back(props);
                    layer.added_physical_device_groups.push_back(props);
                }
            }

            if (layer.reorder_phys_devs) {
                // Flip the order of items
                for (int32_t dev = static_cast<int32_t>(tmp_vector.size() - 1); dev >= 0; --dev) {
                    layer.complete_physical_device_groups.push_back(tmp_vector[dev]);
                }
            } else {
                // Otherwise, keep the order the same
                for (uint32_t dev = 0; dev < tmp_vector.size(); ++dev) {
                    layer.complete_physical_device_groups.push_back(tmp_vector[dev]);
                }
            }
        }

        if (nullptr == pPhysicalDeviceGroupProperties) {
            *pPhysicalDeviceGroupCount = static_cast<uint32_t>(layer.complete_physical_device_groups.size());
        } else {
            uint32_t adj_count = static_cast<uint32_t>(layer.complete_physical_device_groups.size());
            if (*pPhysicalDeviceGroupCount < adj_count) {
                adj_count = *pPhysicalDeviceGroupCount;
                res = VK_INCOMPLETE;
            }
            for (uint32_t dev = 0; dev < adj_count; ++dev) {
                pPhysicalDeviceGroupProperties[dev] = layer.complete_physical_device_groups[dev];
            }
            *pPhysicalDeviceGroupCount = adj_count;
        }

        return res;
    } else {
        return layer.instance_dispatch_table.EnumeratePhysicalDeviceGroups(instance, pPhysicalDeviceGroupCount,
                                                                           pPhysicalDeviceGroupProperties);
    }
}

// device functions

VKAPI_ATTR void VKAPI_CALL test_vkDestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator) {
    for (uint32_t i = 0; i < layer.spurious_device_memory_allocations.size();) {
        auto& allocation = layer.spurious_device_memory_allocations[i];
        if (allocation.device == device && pAllocator && pAllocator->pfnFree) {
            pAllocator->pfnFree(pAllocator->pUserData, allocation.allocation);
            layer.spurious_device_memory_allocations.erase(layer.spurious_device_memory_allocations.begin() + i);
        } else {
            i++;
        }
    }

    for (auto& created_device : layer.created_devices) {
        if (created_device.device_handle == device) {
            created_device.dispatch_table.DestroyDevice(device, pAllocator);
            break;
        }
    }
}
// forward declarations needed for trampolines
#if TEST_LAYER_EXPORT_GET_PHYSICAL_DEVICE_PROC_ADDR
extern "C" {
FRAMEWORK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vk_layerGetPhysicalDeviceProcAddr(VkInstance instance, const char* pName);
}
#endif

// trampolines
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL get_device_func(VkDevice device, const char* pName);
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL get_device_func_impl(VkDevice device, const char* pName) {
    if (string_eq(pName, "vkGetDeviceProcAddr")) return to_vkVoidFunction(get_device_func);
    if (string_eq(pName, "vkDestroyDevice")) return to_vkVoidFunction(test_vkDestroyDevice);

    for (auto& func : layer.custom_device_interception_functions) {
        if (func.name == pName) {
            return to_vkVoidFunction(func.function);
        }
    }

    for (auto& func : layer.custom_device_implementation_functions) {
        if (func.name == pName) {
            return to_vkVoidFunction(func.function);
        }
    }

    return nullptr;
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL get_device_func(VkDevice device, const char* pName) {
    PFN_vkVoidFunction ret_dev = get_device_func_impl(device, pName);
    if (ret_dev != nullptr) return ret_dev;

    return layer.next_vkGetDeviceProcAddr(device, pName);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL get_physical_device_func(VkInstance instance, const char* pName) {
    if (string_eq(pName, "vkEnumerateDeviceLayerProperties")) return to_vkVoidFunction(test_vkEnumerateDeviceLayerProperties);
    if (string_eq(pName, "vkEnumerateDeviceExtensionProperties"))
        return to_vkVoidFunction(test_vkEnumerateDeviceExtensionProperties);
    if (string_eq(pName, "vkEnumeratePhysicalDevices")) return (PFN_vkVoidFunction)test_vkEnumeratePhysicalDevices;
    if (string_eq(pName, "vkEnumeratePhysicalDeviceGroups")) return (PFN_vkVoidFunction)test_vkEnumeratePhysicalDeviceGroups;
    if (string_eq(pName, "vkGetPhysicalDeviceProperties")) return (PFN_vkVoidFunction)test_vkGetPhysicalDeviceProperties;

    for (auto& func : layer.custom_physical_device_interception_functions) {
        if (func.name == pName) {
            return to_vkVoidFunction(func.function);
        }
    }

    for (auto& func : layer.custom_physical_device_implementation_functions) {
        if (func.name == pName) {
            return to_vkVoidFunction(func.function);
        }
    }

#if TEST_LAYER_EXPORT_GET_PHYSICAL_DEVICE_PROC_ADDR
    if (string_eq(pName, "vk_layerGetPhysicalDeviceProcAddr")) return to_vkVoidFunction(vk_layerGetPhysicalDeviceProcAddr);
#endif
    return nullptr;
}
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL get_instance_func(VkInstance instance, const char* pName);
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL get_instance_func_impl(VkInstance instance, const char* pName) {
    if (pName == nullptr) return nullptr;
    if (string_eq(pName, "vkGetInstanceProcAddr")) return to_vkVoidFunction(get_instance_func);
    if (string_eq(pName, "vkEnumerateInstanceLayerProperties")) return to_vkVoidFunction(test_vkEnumerateInstanceLayerProperties);
    if (string_eq(pName, "vkEnumerateInstanceExtensionProperties"))
        return to_vkVoidFunction(test_vkEnumerateInstanceExtensionProperties);
    if (string_eq(pName, "vkEnumerateInstanceVersion")) return to_vkVoidFunction(test_vkEnumerateInstanceVersion);
    if (string_eq(pName, "vkCreateInstance")) return to_vkVoidFunction(test_vkCreateInstance);
    if (string_eq(pName, "vkDestroyInstance")) return to_vkVoidFunction(test_vkDestroyInstance);
    if (string_eq(pName, "vkCreateDevice")) return to_vkVoidFunction(test_vkCreateDevice);
    if (string_eq(pName, "vkGetDeviceProcAddr")) return to_vkVoidFunction(get_device_func);

    PFN_vkVoidFunction ret_phys_dev = get_physical_device_func(instance, pName);
    if (ret_phys_dev != nullptr) return ret_phys_dev;

    PFN_vkVoidFunction ret_dev = get_device_func_impl(nullptr, pName);
    if (ret_dev != nullptr) return ret_dev;

    return nullptr;
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL get_instance_func(VkInstance instance, const char* pName) {
    PFN_vkVoidFunction ret_dev = get_instance_func_impl(instance, pName);
    if (ret_dev != nullptr) return ret_dev;

    return layer.next_vkGetInstanceProcAddr(instance, pName);
}

// Exported functions
extern "C" {
#if TEST_LAYER_EXPORT_ENUMERATE_FUNCTIONS

// Pre-instance handling functions

FRAMEWORK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL test_preinst_vkEnumerateInstanceLayerProperties(
    const VkEnumerateInstanceLayerPropertiesChain* pChain, uint32_t* pPropertyCount, VkLayerProperties* pProperties) {
    VkResult res = pChain->pfnNextLayer(pChain->pNextLink, pPropertyCount, pProperties);
    if (nullptr == pProperties) {
        *pPropertyCount = layer.reported_layer_props;
    } else {
        uint32_t count = layer.reported_layer_props;
        if (*pPropertyCount < layer.reported_layer_props) {
            count = *pPropertyCount;
            res = VK_INCOMPLETE;
        }
        for (uint32_t i = 0; i < count; ++i) {
            snprintf(pProperties[i].layerName, VK_MAX_EXTENSION_NAME_SIZE, "%02d_layer", count);
            pProperties[i].specVersion = count;
            pProperties[i].implementationVersion = 0xABCD0000 + count;
        }
    }
    return res;
}

FRAMEWORK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL test_preinst_vkEnumerateInstanceExtensionProperties(
    const VkEnumerateInstanceExtensionPropertiesChain* pChain, const char* pLayerName, uint32_t* pPropertyCount,
    VkExtensionProperties* pProperties) {
    VkResult res = pChain->pfnNextLayer(pChain->pNextLink, pLayerName, pPropertyCount, pProperties);
    if (nullptr == pProperties) {
        *pPropertyCount = layer.reported_extension_props;
    } else {
        uint32_t count = layer.reported_extension_props;
        if (*pPropertyCount < layer.reported_extension_props) {
            count = *pPropertyCount;
            res = VK_INCOMPLETE;
        }
        for (uint32_t i = 0; i < count; ++i) {
            snprintf(pProperties[i].extensionName, VK_MAX_EXTENSION_NAME_SIZE, "%02d_ext", count);
            pProperties[i].specVersion = count;
        }
    }
    return res;
}

FRAMEWORK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
test_preinst_vkEnumerateInstanceVersion(const VkEnumerateInstanceVersionChain* pChain, uint32_t* pApiVersion) {
    VkResult res = pChain->pfnNextLayer(pChain->pNextLink, pApiVersion);
    *pApiVersion = layer.reported_instance_version;
    return res;
}

FRAMEWORK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(uint32_t* pPropertyCount,
                                                                                   VkLayerProperties* pProperties) {
    return test_vkEnumerateInstanceLayerProperties(pPropertyCount, pProperties);
}
FRAMEWORK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(const char* pLayerName,
                                                                                       uint32_t* pPropertyCount,
                                                                                       VkExtensionProperties* pProperties) {
    return test_vkEnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties);
}

FRAMEWORK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice,
                                                                                 uint32_t* pPropertyCount,
                                                                                 VkLayerProperties* pProperties) {
    return test_vkEnumerateDeviceLayerProperties(physicalDevice, pPropertyCount, pProperties);
}
FRAMEWORK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice,
                                                                                     const char* pLayerName,
                                                                                     uint32_t* pPropertyCount,
                                                                                     VkExtensionProperties* pProperties) {
    return test_vkEnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pPropertyCount, pProperties);
}
#endif

#if TEST_LAYER_EXPORT_LAYER_NAMED_GIPA
FRAMEWORK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL test_layer_GetInstanceProcAddr(VkInstance instance, const char* pName) {
    return get_instance_func(instance, pName);
}
FRAMEWORK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL test_override_vkGetInstanceProcAddr(VkInstance instance,
                                                                                              const char* pName) {
    if (string_eq(pName, "vkCreateInstance")) return to_vkVoidFunction(test_override_vkCreateInstance);
    return get_instance_func(instance, pName);
}
#endif

#if TEST_LAYER_EXPORT_LAYER_VK_GIPA
FRAMEWORK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance instance, const char* pName) {
    return get_instance_func(instance, pName);
}
#endif

#if TEST_LAYER_EXPORT_LAYER_NAMED_GDPA
FRAMEWORK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL test_layer_GetDeviceProcAddr(VkDevice device, const char* pName) {
    return get_device_func(device, pName);
}
#endif

#if TEST_LAYER_EXPORT_LAYER_VK_GDPA
FRAMEWORK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(VkDevice device, const char* pName) {
    return get_device_func(device, pName);
}
#endif

#if TEST_LAYER_EXPORT_NO_PREFIX_GIPA
FRAMEWORK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetInstanceProcAddr(VkInstance instance, const char* pName) {
    return get_instance_func(instance, pName);
}
#endif

#if TEST_LAYER_EXPORT_NO_PREFIX_GDPA
FRAMEWORK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetDeviceProcAddr(VkDevice device, const char* pName) {
    return get_device_func(device, pName);
}
#endif

#if TEST_LAYER_EXPORT_GET_PHYSICAL_DEVICE_PROC_ADDR
FRAMEWORK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vk_layerGetPhysicalDeviceProcAddr(VkInstance instance,
                                                                                            const char* pName) {
    auto func = get_physical_device_func(instance, pName);
    if (func != nullptr) return func;
    return layer.next_GetPhysicalDeviceProcAddr(instance, pName);
}
#endif

#if LAYER_EXPORT_NEGOTIATE_LOADER_LAYER_INTERFACE_VERSION
// vk_layer.h has a forward declaration of vkNegotiateLoaderLayerInterfaceVersion, which doesn't have any attributes
// Since FRAMEWORK_EXPORT adds  __declspec(dllexport), we can't do that here, thus we need our own macro
#if (defined(__GNUC__) && (__GNUC__ >= 4)) || (defined(__SUNPRO_C) && (__SUNPRO_C >= 0x590))
#define EXPORT_NEGOTIATE_FUNCTION __attribute__((visibility("default")))
#else
#define EXPORT_NEGOTIATE_FUNCTION
#endif

EXPORT_NEGOTIATE_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkNegotiateLoaderLayerInterfaceVersion(VkNegotiateLayerInterface* pVersionStruct) {
    if (pVersionStruct) {
        if (pVersionStruct->loaderLayerInterfaceVersion < layer.min_implementation_version) {
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        pVersionStruct->loaderLayerInterfaceVersion = layer.implementation_version;
        pVersionStruct->pfnGetInstanceProcAddr = get_instance_func;
        pVersionStruct->pfnGetDeviceProcAddr = get_device_func;
#if TEST_LAYER_EXPORT_GET_PHYSICAL_DEVICE_PROC_ADDR
        pVersionStruct->pfnGetPhysicalDeviceProcAddr = vk_layerGetPhysicalDeviceProcAddr;
#else
        pVersionStruct->pfnGetPhysicalDeviceProcAddr = nullptr;
#endif

        return VK_SUCCESS;
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}
#endif
}  // extern "C"
