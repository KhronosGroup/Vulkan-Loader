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
    if (pLayerName && string_eq(pLayerName, layer.unique_name.c_str())) {
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
    if (pLayerName && string_eq(pLayerName, layer.unique_name.c_str())) {
        *pPropertyCount = 0;
        return VK_SUCCESS;
    }
    return layer.instance_dispatch_table.EnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pPropertyCount,
                                                                            pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkEnumerateInstanceVersion(uint32_t* pApiVersion) {
    if (pApiVersion != nullptr) {
        *pApiVersion = VK_MAKE_VERSION(1, 0, 0);
    }
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo,
                                                     const VkAllocationCallbacks* pAllocator, VkInstance* pInstance) {
    VkLayerInstanceCreateInfo* chain_info = get_chain_info(pCreateInfo, VK_LAYER_LINK_INFO);

    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr = chain_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkCreateInstance fpCreateInstance = (PFN_vkCreateInstance)fpGetInstanceProcAddr(NULL, "vkCreateInstance");
    if (fpCreateInstance == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    layer.next_vkGetInstanceProcAddr = fpGetInstanceProcAddr;

    // Advance the link info for the next element of the chain
    chain_info->u.pLayerInfo = chain_info->u.pLayerInfo->pNext;

    // Continue call down the chain
    VkResult result = fpCreateInstance(pCreateInfo, pAllocator, pInstance);
    if (result != VK_SUCCESS) {
        return result;
    }
    layer.instance_handle = *pInstance;

    // Init layer's dispatch table using GetInstanceProcAddr of
    // next layer in the chain.
    layer_init_instance_dispatch_table(layer.instance_handle, &layer.instance_dispatch_table, fpGetInstanceProcAddr);

    if (layer.create_instance_callback) result = layer.create_instance_callback(layer, layer.create_instance_callback_data);

    return result;
}
VKAPI_ATTR void VKAPI_CALL test_vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator) {
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
    TestLayer::Device device{*pDevice};

    // initialize layer's dispatch table
    layer_init_device_dispatch_table(device.device_handle, &device.dispatch_table, fpGetDeviceProcAddr);

    if (layer.create_device_callback) result = layer.create_device_callback(layer, layer.create_device_callback_data);

    return result;
}

// device functions

VKAPI_ATTR void VKAPI_CALL test_vkDestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator) {
    for (auto& created_device : layer.created_devices) {
        if (created_device.device_handle == device) {
            created_device.dispatch_table.DestroyDevice(device, pAllocator);
            break;
        }
    }
}

// trampolines

#define TO_VOID_PFN(func) reinterpret_cast<PFN_vkVoidFunction>(func)

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL get_device_func(VkDevice device, const char* pName) {
    if (string_eq(pName, "vkDestroyDevice")) return TO_VOID_PFN(test_vkDestroyDevice);

    return layer.next_vkGetDeviceProcAddr(device, pName);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL get_instance_func(VkInstance instance, const char* pName) {
    if (pName == nullptr) return nullptr;
    if (string_eq(pName, "vkGetInstanceProcAddr")) return TO_VOID_PFN(get_instance_func);
    if (string_eq(pName, "vkEnumerateInstanceLayerProperties")) return TO_VOID_PFN(test_vkEnumerateInstanceLayerProperties);
    if (string_eq(pName, "vkEnumerateInstanceExtensionProperties")) return TO_VOID_PFN(test_vkEnumerateInstanceExtensionProperties);
    if (string_eq(pName, "vkEnumerateInstanceVersion")) return TO_VOID_PFN(test_vkEnumerateInstanceVersion);

    if (string_eq(pName, "vkEnumerateDeviceLayerProperties")) return TO_VOID_PFN(test_vkEnumerateDeviceLayerProperties);
    if (string_eq(pName, "vkEnumerateDeviceExtensionProperties")) return TO_VOID_PFN(test_vkEnumerateDeviceExtensionProperties);
    if (string_eq(pName, "vkCreateInstance")) return TO_VOID_PFN(test_vkCreateInstance);
    if (string_eq(pName, "vkDestroyInstance")) return TO_VOID_PFN(test_vkDestroyInstance);
    if (string_eq(pName, "vkCreateDevice")) return TO_VOID_PFN(test_vkCreateDevice);
    if (string_eq(pName, "vkGetDeviceProcAddr")) return TO_VOID_PFN(get_device_func);

    return layer.next_vkGetInstanceProcAddr(instance, pName);
}

// Exported functions
extern "C" {
#if TEST_LAYER_EXPORT_ENUMERATE_FUNCTIONS
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
    return nullptr;
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