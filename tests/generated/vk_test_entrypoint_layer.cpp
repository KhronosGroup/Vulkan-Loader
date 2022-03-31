// *** THIS FILE IS GENERATED - DO NOT EDIT ***
// See loader_test_generator.py for modifications

/*
 * Copyright (c) 2022 The Khronos Group Inc.
 * Copyright (c) 2022 Valve Corporation
 * Copyright (c) 2022 LunarG, Inc.
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
 * Author: Mark Young <marky@lunarg.com>
 */

// clang-format off

#include "vk_test_entrypoint_layer.h"
#include "loader/generated/vk_dispatch_table_helper.h"

EntrypointTestLayer layer;
extern "C" {
FRAMEWORK_EXPORT EntrypointTestLayer* get_test_layer_func() { return &layer; }
FRAMEWORK_EXPORT EntrypointTestLayer* reset_layer_func() {
    layer.~EntrypointTestLayer();
    return new (&layer) EntrypointTestLayer();
}
} // extern "C"

#ifndef TEST_LAYER_NAME
#define TEST_LAYER_NAME "VkLayer_LunarG_entrypoint_layer"
#endif

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

void log_layer_message(const std::string& message) {
    static uint8_t cur_message_index = 0;
    static std::string messages[8];
    if (layer.debug_util_info.callback != nullptr) {
        VkDebugUtilsMessengerCallbackDataEXT callback_data{VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CALLBACK_DATA_EXT};
        messages[cur_message_index] = message.c_str();
        callback_data.pMessage = messages[cur_message_index].c_str();
        if (++cur_message_index >= 8) { cur_message_index = 0; }
        layer.debug_util_info.callback(
               VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
               VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
               &callback_data,
               layer.debug_util_info.user_data);
    }
}

VKAPI_ATTR VkResult VKAPI_CALL layer_EnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties) {
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL layer_EnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount,
                                                                         VkExtensionProperties* pProperties) {
    if (pLayerName && string_eq(pLayerName, TEST_LAYER_NAME)) {
        *pPropertyCount = 0;
        return VK_SUCCESS;
    }
    return layer.instance_dispatch_table.EnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_EnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount,
                                                                   VkLayerProperties* pProperties) {
    log_layer_message("Generated Layer vkEnumerateDeviceLayerProperties");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL layer_EnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char* pLayerName,
                                                                       uint32_t* pPropertyCount,
                                                                       VkExtensionProperties* pProperties) {
    log_layer_message("Generated Layer vkEnumerateDeviceExtensionProperties");
    if (pLayerName && string_eq(pLayerName, TEST_LAYER_NAME)) {
        *pPropertyCount = 0;
        return VK_SUCCESS;
    }
    return layer.instance_dispatch_table.EnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pPropertyCount,
                                                                            pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_EnumerateInstanceVersion(uint32_t* pApiVersion) {
    log_layer_message("Generated Layer vkEnumerateInstanceVersion");
    if (pApiVersion != nullptr) {
        *pApiVersion = VK_API_VERSION_1_3;
    }
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateInstance(const VkInstanceCreateInfo* pCreateInfo,
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

    layer.enabled_instance_major = 1;
    layer.enabled_instance_minor = 0;
    if (pCreateInfo->pApplicationInfo != NULL && pCreateInfo->pApplicationInfo->apiVersion != 0) {
        layer.enabled_instance_major = static_cast<uint8_t>(VK_API_VERSION_MAJOR(pCreateInfo->pApplicationInfo->apiVersion));
        layer.enabled_instance_minor = static_cast<uint8_t>(VK_API_VERSION_MINOR(pCreateInfo->pApplicationInfo->apiVersion));
    }

    for (uint32_t ext = 0; ext < pCreateInfo->enabledExtensionCount; ++ext) {
        layer.enabled_instance_extensions.push_back(pCreateInfo->ppEnabledExtensionNames[ext]);
    }

    // Init layer's dispatch table using GetInstanceProcAddr of next layer in the chain.
    layer_init_instance_dispatch_table(layer.instance_handle, &layer.instance_dispatch_table, fpGetInstanceProcAddr);

    return result;
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator) {
    layer.instance_dispatch_table.DestroyInstance(instance, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                                  const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                                  const VkAllocationCallbacks* pAllocator,
                                                                  VkDebugUtilsMessengerEXT* pMessenger) {
    layer.debug_util_info.severities = pCreateInfo->messageSeverity;
    layer.debug_util_info.types = pCreateInfo->messageType;
    layer.debug_util_info.callback = pCreateInfo->pfnUserCallback;
    layer.debug_util_info.user_data = pCreateInfo->pUserData;
    return layer.instance_dispatch_table.CreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo,
                                                 const VkAllocationCallbacks* pAllocator, VkDevice* pDevice) {
    VkLayerDeviceCreateInfo* chain_info = get_chain_info(pCreateInfo, VK_LAYER_LINK_INFO);

    log_layer_message("Generated Layer vkCreateDevice");

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
    EntrypointTestLayer::Device device{*pDevice};

    for (uint32_t ext = 0; ext < pCreateInfo->enabledExtensionCount; ++ext) {
        device.enabled_extensions.push_back(pCreateInfo->ppEnabledExtensionNames[ext]);
    }

    // initialize layer's dispatch table
    layer_init_device_dispatch_table(device.device_handle, &device.dispatch_table, fpGetDeviceProcAddr);

    // Need to add the created devices to the list so it can be freed
    layer.created_devices.push_back(device);

    return result;
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyDevice");
    for (auto& created_device : layer.created_devices) {
        if (created_device.device_handle == device) {
            created_device.dispatch_table.DestroyDevice(device, pAllocator);
            break;
        }
    }
}
// ----- VK_VERSION_1_0
VKAPI_ATTR VkResult VKAPI_CALL layer_EnumeratePhysicalDevices(
    VkInstance instance,
    uint32_t* pPhysicalDeviceCount,
    VkPhysicalDevice* pPhysicalDevices) {
    log_layer_message("Generated Layer vkEnumeratePhysicalDevices");
    return layer.instance_dispatch_table.EnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceFeatures(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceFeatures* pFeatures) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceFeatures");
   layer.instance_dispatch_table.GetPhysicalDeviceFeatures(physicalDevice, pFeatures);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceFormatProperties(
    VkPhysicalDevice physicalDevice,
    VkFormat format,
    VkFormatProperties* pFormatProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceFormatProperties");
   layer.instance_dispatch_table.GetPhysicalDeviceFormatProperties(physicalDevice, format, pFormatProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceImageFormatProperties(
    VkPhysicalDevice physicalDevice,
    VkFormat format,
    VkImageType type,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkImageCreateFlags flags,
    VkImageFormatProperties* pImageFormatProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceImageFormatProperties");
    return layer.instance_dispatch_table.GetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceProperties(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceProperties* pProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceProperties");
   layer.instance_dispatch_table.GetPhysicalDeviceProperties(physicalDevice, pProperties);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice physicalDevice,
    uint32_t* pQueueFamilyPropertyCount,
    VkQueueFamilyProperties* pQueueFamilyProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceQueueFamilyProperties");
   layer.instance_dispatch_table.GetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceMemoryProperties(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceMemoryProperties* pMemoryProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceMemoryProperties");
   layer.instance_dispatch_table.GetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
}

VKAPI_ATTR void VKAPI_CALL layer_GetDeviceQueue(
    VkDevice device,
    uint32_t queueFamilyIndex,
    uint32_t queueIndex,
    VkQueue* pQueue) {
    log_layer_message("Generated Layer vkGetDeviceQueue");
   layer.created_devices[0].dispatch_table.GetDeviceQueue(device, queueFamilyIndex, queueIndex, pQueue);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_QueueSubmit(
    VkQueue queue,
    uint32_t submitCount,
    const VkSubmitInfo* pSubmits,
    VkFence fence) {
    log_layer_message("Generated Layer vkQueueSubmit");
    return layer.created_devices[0].dispatch_table.QueueSubmit(queue, submitCount, pSubmits, fence);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_QueueWaitIdle(
    VkQueue queue) {
    log_layer_message("Generated Layer vkQueueWaitIdle");
    return layer.created_devices[0].dispatch_table.QueueWaitIdle(queue);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_DeviceWaitIdle(
    VkDevice device) {
    log_layer_message("Generated Layer vkDeviceWaitIdle");
    return layer.created_devices[0].dispatch_table.DeviceWaitIdle(device);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_AllocateMemory(
    VkDevice device,
    const VkMemoryAllocateInfo* pAllocateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDeviceMemory* pMemory) {
    log_layer_message("Generated Layer vkAllocateMemory");
    return layer.created_devices[0].dispatch_table.AllocateMemory(device, pAllocateInfo, pAllocator, pMemory);
}

VKAPI_ATTR void VKAPI_CALL layer_FreeMemory(
    VkDevice device,
    VkDeviceMemory memory,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkFreeMemory");
   layer.created_devices[0].dispatch_table.FreeMemory(device, memory, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_MapMemory(
    VkDevice device,
    VkDeviceMemory memory,
    VkDeviceSize offset,
    VkDeviceSize size,
    VkMemoryMapFlags flags,
    void** ppData) {
    log_layer_message("Generated Layer vkMapMemory");
    return layer.created_devices[0].dispatch_table.MapMemory(device, memory, offset, size, flags, ppData);
}

VKAPI_ATTR void VKAPI_CALL layer_UnmapMemory(
    VkDevice device,
    VkDeviceMemory memory) {
    log_layer_message("Generated Layer vkUnmapMemory");
   layer.created_devices[0].dispatch_table.UnmapMemory(device, memory);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_FlushMappedMemoryRanges(
    VkDevice device,
    uint32_t memoryRangeCount,
    const VkMappedMemoryRange* pMemoryRanges) {
    log_layer_message("Generated Layer vkFlushMappedMemoryRanges");
    return layer.created_devices[0].dispatch_table.FlushMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_InvalidateMappedMemoryRanges(
    VkDevice device,
    uint32_t memoryRangeCount,
    const VkMappedMemoryRange* pMemoryRanges) {
    log_layer_message("Generated Layer vkInvalidateMappedMemoryRanges");
    return layer.created_devices[0].dispatch_table.InvalidateMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
}

VKAPI_ATTR void VKAPI_CALL layer_GetDeviceMemoryCommitment(
    VkDevice device,
    VkDeviceMemory memory,
    VkDeviceSize* pCommittedMemoryInBytes) {
    log_layer_message("Generated Layer vkGetDeviceMemoryCommitment");
   layer.created_devices[0].dispatch_table.GetDeviceMemoryCommitment(device, memory, pCommittedMemoryInBytes);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_BindBufferMemory(
    VkDevice device,
    VkBuffer buffer,
    VkDeviceMemory memory,
    VkDeviceSize memoryOffset) {
    log_layer_message("Generated Layer vkBindBufferMemory");
    return layer.created_devices[0].dispatch_table.BindBufferMemory(device, buffer, memory, memoryOffset);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_BindImageMemory(
    VkDevice device,
    VkImage image,
    VkDeviceMemory memory,
    VkDeviceSize memoryOffset) {
    log_layer_message("Generated Layer vkBindImageMemory");
    return layer.created_devices[0].dispatch_table.BindImageMemory(device, image, memory, memoryOffset);
}

VKAPI_ATTR void VKAPI_CALL layer_GetBufferMemoryRequirements(
    VkDevice device,
    VkBuffer buffer,
    VkMemoryRequirements* pMemoryRequirements) {
    log_layer_message("Generated Layer vkGetBufferMemoryRequirements");
   layer.created_devices[0].dispatch_table.GetBufferMemoryRequirements(device, buffer, pMemoryRequirements);
}

VKAPI_ATTR void VKAPI_CALL layer_GetImageMemoryRequirements(
    VkDevice device,
    VkImage image,
    VkMemoryRequirements* pMemoryRequirements) {
    log_layer_message("Generated Layer vkGetImageMemoryRequirements");
   layer.created_devices[0].dispatch_table.GetImageMemoryRequirements(device, image, pMemoryRequirements);
}

VKAPI_ATTR void VKAPI_CALL layer_GetImageSparseMemoryRequirements(
    VkDevice device,
    VkImage image,
    uint32_t* pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements* pSparseMemoryRequirements) {
    log_layer_message("Generated Layer vkGetImageSparseMemoryRequirements");
   layer.created_devices[0].dispatch_table.GetImageSparseMemoryRequirements(device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceSparseImageFormatProperties(
    VkPhysicalDevice physicalDevice,
    VkFormat format,
    VkImageType type,
    VkSampleCountFlagBits samples,
    VkImageUsageFlags usage,
    VkImageTiling tiling,
    uint32_t* pPropertyCount,
    VkSparseImageFormatProperties* pProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceSparseImageFormatProperties");
   layer.instance_dispatch_table.GetPhysicalDeviceSparseImageFormatProperties(physicalDevice, format, type, samples, usage, tiling, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_QueueBindSparse(
    VkQueue queue,
    uint32_t bindInfoCount,
    const VkBindSparseInfo* pBindInfo,
    VkFence fence) {
    log_layer_message("Generated Layer vkQueueBindSparse");
    return layer.created_devices[0].dispatch_table.QueueBindSparse(queue, bindInfoCount, pBindInfo, fence);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateFence(
    VkDevice device,
    const VkFenceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFence* pFence) {
    log_layer_message("Generated Layer vkCreateFence");
    return layer.created_devices[0].dispatch_table.CreateFence(device, pCreateInfo, pAllocator, pFence);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyFence(
    VkDevice device,
    VkFence fence,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyFence");
   layer.created_devices[0].dispatch_table.DestroyFence(device, fence, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_ResetFences(
    VkDevice device,
    uint32_t fenceCount,
    const VkFence* pFences) {
    log_layer_message("Generated Layer vkResetFences");
    return layer.created_devices[0].dispatch_table.ResetFences(device, fenceCount, pFences);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetFenceStatus(
    VkDevice device,
    VkFence fence) {
    log_layer_message("Generated Layer vkGetFenceStatus");
    return layer.created_devices[0].dispatch_table.GetFenceStatus(device, fence);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_WaitForFences(
    VkDevice device,
    uint32_t fenceCount,
    const VkFence* pFences,
    VkBool32 waitAll,
    uint64_t timeout) {
    log_layer_message("Generated Layer vkWaitForFences");
    return layer.created_devices[0].dispatch_table.WaitForFences(device, fenceCount, pFences, waitAll, timeout);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateSemaphore(
    VkDevice device,
    const VkSemaphoreCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSemaphore* pSemaphore) {
    log_layer_message("Generated Layer vkCreateSemaphore");
    return layer.created_devices[0].dispatch_table.CreateSemaphore(device, pCreateInfo, pAllocator, pSemaphore);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroySemaphore(
    VkDevice device,
    VkSemaphore semaphore,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroySemaphore");
   layer.created_devices[0].dispatch_table.DestroySemaphore(device, semaphore, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateEvent(
    VkDevice device,
    const VkEventCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkEvent* pEvent) {
    log_layer_message("Generated Layer vkCreateEvent");
    return layer.created_devices[0].dispatch_table.CreateEvent(device, pCreateInfo, pAllocator, pEvent);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyEvent(
    VkDevice device,
    VkEvent event,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyEvent");
   layer.created_devices[0].dispatch_table.DestroyEvent(device, event, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetEventStatus(
    VkDevice device,
    VkEvent event) {
    log_layer_message("Generated Layer vkGetEventStatus");
    return layer.created_devices[0].dispatch_table.GetEventStatus(device, event);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_SetEvent(
    VkDevice device,
    VkEvent event) {
    log_layer_message("Generated Layer vkSetEvent");
    return layer.created_devices[0].dispatch_table.SetEvent(device, event);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_ResetEvent(
    VkDevice device,
    VkEvent event) {
    log_layer_message("Generated Layer vkResetEvent");
    return layer.created_devices[0].dispatch_table.ResetEvent(device, event);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateQueryPool(
    VkDevice device,
    const VkQueryPoolCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkQueryPool* pQueryPool) {
    log_layer_message("Generated Layer vkCreateQueryPool");
    return layer.created_devices[0].dispatch_table.CreateQueryPool(device, pCreateInfo, pAllocator, pQueryPool);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyQueryPool(
    VkDevice device,
    VkQueryPool queryPool,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyQueryPool");
   layer.created_devices[0].dispatch_table.DestroyQueryPool(device, queryPool, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetQueryPoolResults(
    VkDevice device,
    VkQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount,
    size_t dataSize,
    void* pData,
    VkDeviceSize stride,
    VkQueryResultFlags flags) {
    log_layer_message("Generated Layer vkGetQueryPoolResults");
    return layer.created_devices[0].dispatch_table.GetQueryPoolResults(device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateBuffer(
    VkDevice device,
    const VkBufferCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkBuffer* pBuffer) {
    log_layer_message("Generated Layer vkCreateBuffer");
    return layer.created_devices[0].dispatch_table.CreateBuffer(device, pCreateInfo, pAllocator, pBuffer);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyBuffer(
    VkDevice device,
    VkBuffer buffer,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyBuffer");
   layer.created_devices[0].dispatch_table.DestroyBuffer(device, buffer, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateBufferView(
    VkDevice device,
    const VkBufferViewCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkBufferView* pView) {
    log_layer_message("Generated Layer vkCreateBufferView");
    return layer.created_devices[0].dispatch_table.CreateBufferView(device, pCreateInfo, pAllocator, pView);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyBufferView(
    VkDevice device,
    VkBufferView bufferView,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyBufferView");
   layer.created_devices[0].dispatch_table.DestroyBufferView(device, bufferView, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateImage(
    VkDevice device,
    const VkImageCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkImage* pImage) {
    log_layer_message("Generated Layer vkCreateImage");
    return layer.created_devices[0].dispatch_table.CreateImage(device, pCreateInfo, pAllocator, pImage);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyImage(
    VkDevice device,
    VkImage image,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyImage");
   layer.created_devices[0].dispatch_table.DestroyImage(device, image, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL layer_GetImageSubresourceLayout(
    VkDevice device,
    VkImage image,
    const VkImageSubresource* pSubresource,
    VkSubresourceLayout* pLayout) {
    log_layer_message("Generated Layer vkGetImageSubresourceLayout");
   layer.created_devices[0].dispatch_table.GetImageSubresourceLayout(device, image, pSubresource, pLayout);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateImageView(
    VkDevice device,
    const VkImageViewCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkImageView* pView) {
    log_layer_message("Generated Layer vkCreateImageView");
    return layer.created_devices[0].dispatch_table.CreateImageView(device, pCreateInfo, pAllocator, pView);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyImageView(
    VkDevice device,
    VkImageView imageView,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyImageView");
   layer.created_devices[0].dispatch_table.DestroyImageView(device, imageView, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateShaderModule(
    VkDevice device,
    const VkShaderModuleCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkShaderModule* pShaderModule) {
    log_layer_message("Generated Layer vkCreateShaderModule");
    return layer.created_devices[0].dispatch_table.CreateShaderModule(device, pCreateInfo, pAllocator, pShaderModule);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyShaderModule(
    VkDevice device,
    VkShaderModule shaderModule,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyShaderModule");
   layer.created_devices[0].dispatch_table.DestroyShaderModule(device, shaderModule, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreatePipelineCache(
    VkDevice device,
    const VkPipelineCacheCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkPipelineCache* pPipelineCache) {
    log_layer_message("Generated Layer vkCreatePipelineCache");
    return layer.created_devices[0].dispatch_table.CreatePipelineCache(device, pCreateInfo, pAllocator, pPipelineCache);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyPipelineCache(
    VkDevice device,
    VkPipelineCache pipelineCache,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyPipelineCache");
   layer.created_devices[0].dispatch_table.DestroyPipelineCache(device, pipelineCache, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetPipelineCacheData(
    VkDevice device,
    VkPipelineCache pipelineCache,
    size_t* pDataSize,
    void* pData) {
    log_layer_message("Generated Layer vkGetPipelineCacheData");
    return layer.created_devices[0].dispatch_table.GetPipelineCacheData(device, pipelineCache, pDataSize, pData);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_MergePipelineCaches(
    VkDevice device,
    VkPipelineCache dstCache,
    uint32_t srcCacheCount,
    const VkPipelineCache* pSrcCaches) {
    log_layer_message("Generated Layer vkMergePipelineCaches");
    return layer.created_devices[0].dispatch_table.MergePipelineCaches(device, dstCache, srcCacheCount, pSrcCaches);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateGraphicsPipelines(
    VkDevice device,
    VkPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const VkGraphicsPipelineCreateInfo* pCreateInfos,
    const VkAllocationCallbacks* pAllocator,
    VkPipeline* pPipelines) {
    log_layer_message("Generated Layer vkCreateGraphicsPipelines");
    return layer.created_devices[0].dispatch_table.CreateGraphicsPipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateComputePipelines(
    VkDevice device,
    VkPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const VkComputePipelineCreateInfo* pCreateInfos,
    const VkAllocationCallbacks* pAllocator,
    VkPipeline* pPipelines) {
    log_layer_message("Generated Layer vkCreateComputePipelines");
    return layer.created_devices[0].dispatch_table.CreateComputePipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyPipeline(
    VkDevice device,
    VkPipeline pipeline,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyPipeline");
   layer.created_devices[0].dispatch_table.DestroyPipeline(device, pipeline, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreatePipelineLayout(
    VkDevice device,
    const VkPipelineLayoutCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkPipelineLayout* pPipelineLayout) {
    log_layer_message("Generated Layer vkCreatePipelineLayout");
    return layer.created_devices[0].dispatch_table.CreatePipelineLayout(device, pCreateInfo, pAllocator, pPipelineLayout);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyPipelineLayout(
    VkDevice device,
    VkPipelineLayout pipelineLayout,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyPipelineLayout");
   layer.created_devices[0].dispatch_table.DestroyPipelineLayout(device, pipelineLayout, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateSampler(
    VkDevice device,
    const VkSamplerCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSampler* pSampler) {
    log_layer_message("Generated Layer vkCreateSampler");
    return layer.created_devices[0].dispatch_table.CreateSampler(device, pCreateInfo, pAllocator, pSampler);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroySampler(
    VkDevice device,
    VkSampler sampler,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroySampler");
   layer.created_devices[0].dispatch_table.DestroySampler(device, sampler, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateDescriptorSetLayout(
    VkDevice device,
    const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDescriptorSetLayout* pSetLayout) {
    log_layer_message("Generated Layer vkCreateDescriptorSetLayout");
    return layer.created_devices[0].dispatch_table.CreateDescriptorSetLayout(device, pCreateInfo, pAllocator, pSetLayout);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyDescriptorSetLayout(
    VkDevice device,
    VkDescriptorSetLayout descriptorSetLayout,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyDescriptorSetLayout");
   layer.created_devices[0].dispatch_table.DestroyDescriptorSetLayout(device, descriptorSetLayout, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateDescriptorPool(
    VkDevice device,
    const VkDescriptorPoolCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDescriptorPool* pDescriptorPool) {
    log_layer_message("Generated Layer vkCreateDescriptorPool");
    return layer.created_devices[0].dispatch_table.CreateDescriptorPool(device, pCreateInfo, pAllocator, pDescriptorPool);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyDescriptorPool(
    VkDevice device,
    VkDescriptorPool descriptorPool,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyDescriptorPool");
   layer.created_devices[0].dispatch_table.DestroyDescriptorPool(device, descriptorPool, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_ResetDescriptorPool(
    VkDevice device,
    VkDescriptorPool descriptorPool,
    VkDescriptorPoolResetFlags flags) {
    log_layer_message("Generated Layer vkResetDescriptorPool");
    return layer.created_devices[0].dispatch_table.ResetDescriptorPool(device, descriptorPool, flags);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_AllocateDescriptorSets(
    VkDevice device,
    const VkDescriptorSetAllocateInfo* pAllocateInfo,
    VkDescriptorSet* pDescriptorSets) {
    log_layer_message("Generated Layer vkAllocateDescriptorSets");
    return layer.created_devices[0].dispatch_table.AllocateDescriptorSets(device, pAllocateInfo, pDescriptorSets);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_FreeDescriptorSets(
    VkDevice device,
    VkDescriptorPool descriptorPool,
    uint32_t descriptorSetCount,
    const VkDescriptorSet* pDescriptorSets) {
    log_layer_message("Generated Layer vkFreeDescriptorSets");
    return layer.created_devices[0].dispatch_table.FreeDescriptorSets(device, descriptorPool, descriptorSetCount, pDescriptorSets);
}

VKAPI_ATTR void VKAPI_CALL layer_UpdateDescriptorSets(
    VkDevice device,
    uint32_t descriptorWriteCount,
    const VkWriteDescriptorSet* pDescriptorWrites,
    uint32_t descriptorCopyCount,
    const VkCopyDescriptorSet* pDescriptorCopies) {
    log_layer_message("Generated Layer vkUpdateDescriptorSets");
   layer.created_devices[0].dispatch_table.UpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateFramebuffer(
    VkDevice device,
    const VkFramebufferCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFramebuffer* pFramebuffer) {
    log_layer_message("Generated Layer vkCreateFramebuffer");
    return layer.created_devices[0].dispatch_table.CreateFramebuffer(device, pCreateInfo, pAllocator, pFramebuffer);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyFramebuffer(
    VkDevice device,
    VkFramebuffer framebuffer,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyFramebuffer");
   layer.created_devices[0].dispatch_table.DestroyFramebuffer(device, framebuffer, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateRenderPass(
    VkDevice device,
    const VkRenderPassCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkRenderPass* pRenderPass) {
    log_layer_message("Generated Layer vkCreateRenderPass");
    return layer.created_devices[0].dispatch_table.CreateRenderPass(device, pCreateInfo, pAllocator, pRenderPass);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyRenderPass(
    VkDevice device,
    VkRenderPass renderPass,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyRenderPass");
   layer.created_devices[0].dispatch_table.DestroyRenderPass(device, renderPass, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL layer_GetRenderAreaGranularity(
    VkDevice device,
    VkRenderPass renderPass,
    VkExtent2D* pGranularity) {
    log_layer_message("Generated Layer vkGetRenderAreaGranularity");
   layer.created_devices[0].dispatch_table.GetRenderAreaGranularity(device, renderPass, pGranularity);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateCommandPool(
    VkDevice device,
    const VkCommandPoolCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkCommandPool* pCommandPool) {
    log_layer_message("Generated Layer vkCreateCommandPool");
    return layer.created_devices[0].dispatch_table.CreateCommandPool(device, pCreateInfo, pAllocator, pCommandPool);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyCommandPool(
    VkDevice device,
    VkCommandPool commandPool,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyCommandPool");
   layer.created_devices[0].dispatch_table.DestroyCommandPool(device, commandPool, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_ResetCommandPool(
    VkDevice device,
    VkCommandPool commandPool,
    VkCommandPoolResetFlags flags) {
    log_layer_message("Generated Layer vkResetCommandPool");
    return layer.created_devices[0].dispatch_table.ResetCommandPool(device, commandPool, flags);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_AllocateCommandBuffers(
    VkDevice device,
    const VkCommandBufferAllocateInfo* pAllocateInfo,
    VkCommandBuffer* pCommandBuffers) {
    log_layer_message("Generated Layer vkAllocateCommandBuffers");
    return layer.created_devices[0].dispatch_table.AllocateCommandBuffers(device, pAllocateInfo, pCommandBuffers);
}

VKAPI_ATTR void VKAPI_CALL layer_FreeCommandBuffers(
    VkDevice device,
    VkCommandPool commandPool,
    uint32_t commandBufferCount,
    const VkCommandBuffer* pCommandBuffers) {
    log_layer_message("Generated Layer vkFreeCommandBuffers");
   layer.created_devices[0].dispatch_table.FreeCommandBuffers(device, commandPool, commandBufferCount, pCommandBuffers);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_BeginCommandBuffer(
    VkCommandBuffer commandBuffer,
    const VkCommandBufferBeginInfo* pBeginInfo) {
    log_layer_message("Generated Layer vkBeginCommandBuffer");
    return layer.created_devices[0].dispatch_table.BeginCommandBuffer(commandBuffer, pBeginInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_EndCommandBuffer(
    VkCommandBuffer commandBuffer) {
    log_layer_message("Generated Layer vkEndCommandBuffer");
    return layer.created_devices[0].dispatch_table.EndCommandBuffer(commandBuffer);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_ResetCommandBuffer(
    VkCommandBuffer commandBuffer,
    VkCommandBufferResetFlags flags) {
    log_layer_message("Generated Layer vkResetCommandBuffer");
    return layer.created_devices[0].dispatch_table.ResetCommandBuffer(commandBuffer, flags);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBindPipeline(
    VkCommandBuffer commandBuffer,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipeline pipeline) {
    log_layer_message("Generated Layer vkCmdBindPipeline");
   layer.created_devices[0].dispatch_table.CmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetViewport(
    VkCommandBuffer commandBuffer,
    uint32_t firstViewport,
    uint32_t viewportCount,
    const VkViewport* pViewports) {
    log_layer_message("Generated Layer vkCmdSetViewport");
   layer.created_devices[0].dispatch_table.CmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetScissor(
    VkCommandBuffer commandBuffer,
    uint32_t firstScissor,
    uint32_t scissorCount,
    const VkRect2D* pScissors) {
    log_layer_message("Generated Layer vkCmdSetScissor");
   layer.created_devices[0].dispatch_table.CmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetLineWidth(
    VkCommandBuffer commandBuffer,
    float lineWidth) {
    log_layer_message("Generated Layer vkCmdSetLineWidth");
   layer.created_devices[0].dispatch_table.CmdSetLineWidth(commandBuffer, lineWidth);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetDepthBias(
    VkCommandBuffer commandBuffer,
    float depthBiasConstantFactor,
    float depthBiasClamp,
    float depthBiasSlopeFactor) {
    log_layer_message("Generated Layer vkCmdSetDepthBias");
   layer.created_devices[0].dispatch_table.CmdSetDepthBias(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetBlendConstants(
    VkCommandBuffer commandBuffer,
    const float blendConstants[4]) {
    log_layer_message("Generated Layer vkCmdSetBlendConstants");
   layer.created_devices[0].dispatch_table.CmdSetBlendConstants(commandBuffer, blendConstants);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetDepthBounds(
    VkCommandBuffer commandBuffer,
    float minDepthBounds,
    float maxDepthBounds) {
    log_layer_message("Generated Layer vkCmdSetDepthBounds");
   layer.created_devices[0].dispatch_table.CmdSetDepthBounds(commandBuffer, minDepthBounds, maxDepthBounds);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetStencilCompareMask(
    VkCommandBuffer commandBuffer,
    VkStencilFaceFlags faceMask,
    uint32_t compareMask) {
    log_layer_message("Generated Layer vkCmdSetStencilCompareMask");
   layer.created_devices[0].dispatch_table.CmdSetStencilCompareMask(commandBuffer, faceMask, compareMask);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetStencilWriteMask(
    VkCommandBuffer commandBuffer,
    VkStencilFaceFlags faceMask,
    uint32_t writeMask) {
    log_layer_message("Generated Layer vkCmdSetStencilWriteMask");
   layer.created_devices[0].dispatch_table.CmdSetStencilWriteMask(commandBuffer, faceMask, writeMask);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetStencilReference(
    VkCommandBuffer commandBuffer,
    VkStencilFaceFlags faceMask,
    uint32_t reference) {
    log_layer_message("Generated Layer vkCmdSetStencilReference");
   layer.created_devices[0].dispatch_table.CmdSetStencilReference(commandBuffer, faceMask, reference);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBindDescriptorSets(
    VkCommandBuffer commandBuffer,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipelineLayout layout,
    uint32_t firstSet,
    uint32_t descriptorSetCount,
    const VkDescriptorSet* pDescriptorSets,
    uint32_t dynamicOffsetCount,
    const uint32_t* pDynamicOffsets) {
    log_layer_message("Generated Layer vkCmdBindDescriptorSets");
   layer.created_devices[0].dispatch_table.CmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBindIndexBuffer(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    VkIndexType indexType) {
    log_layer_message("Generated Layer vkCmdBindIndexBuffer");
   layer.created_devices[0].dispatch_table.CmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBindVertexBuffers(
    VkCommandBuffer commandBuffer,
    uint32_t firstBinding,
    uint32_t bindingCount,
    const VkBuffer* pBuffers,
    const VkDeviceSize* pOffsets) {
    log_layer_message("Generated Layer vkCmdBindVertexBuffers");
   layer.created_devices[0].dispatch_table.CmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdDraw(
    VkCommandBuffer commandBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance) {
    log_layer_message("Generated Layer vkCmdDraw");
   layer.created_devices[0].dispatch_table.CmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdDrawIndexed(
    VkCommandBuffer commandBuffer,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance) {
    log_layer_message("Generated Layer vkCmdDrawIndexed");
   layer.created_devices[0].dispatch_table.CmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdDrawIndirect(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride) {
    log_layer_message("Generated Layer vkCmdDrawIndirect");
   layer.created_devices[0].dispatch_table.CmdDrawIndirect(commandBuffer, buffer, offset, drawCount, stride);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdDrawIndexedIndirect(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride) {
    log_layer_message("Generated Layer vkCmdDrawIndexedIndirect");
   layer.created_devices[0].dispatch_table.CmdDrawIndexedIndirect(commandBuffer, buffer, offset, drawCount, stride);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdDispatch(
    VkCommandBuffer commandBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ) {
    log_layer_message("Generated Layer vkCmdDispatch");
   layer.created_devices[0].dispatch_table.CmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdDispatchIndirect(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset) {
    log_layer_message("Generated Layer vkCmdDispatchIndirect");
   layer.created_devices[0].dispatch_table.CmdDispatchIndirect(commandBuffer, buffer, offset);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdCopyBuffer(
    VkCommandBuffer commandBuffer,
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    uint32_t regionCount,
    const VkBufferCopy* pRegions) {
    log_layer_message("Generated Layer vkCmdCopyBuffer");
   layer.created_devices[0].dispatch_table.CmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdCopyImage(
    VkCommandBuffer commandBuffer,
    VkImage srcImage,
    VkImageLayout srcImageLayout,
    VkImage dstImage,
    VkImageLayout dstImageLayout,
    uint32_t regionCount,
    const VkImageCopy* pRegions) {
    log_layer_message("Generated Layer vkCmdCopyImage");
   layer.created_devices[0].dispatch_table.CmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBlitImage(
    VkCommandBuffer commandBuffer,
    VkImage srcImage,
    VkImageLayout srcImageLayout,
    VkImage dstImage,
    VkImageLayout dstImageLayout,
    uint32_t regionCount,
    const VkImageBlit* pRegions,
    VkFilter filter) {
    log_layer_message("Generated Layer vkCmdBlitImage");
   layer.created_devices[0].dispatch_table.CmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdCopyBufferToImage(
    VkCommandBuffer commandBuffer,
    VkBuffer srcBuffer,
    VkImage dstImage,
    VkImageLayout dstImageLayout,
    uint32_t regionCount,
    const VkBufferImageCopy* pRegions) {
    log_layer_message("Generated Layer vkCmdCopyBufferToImage");
   layer.created_devices[0].dispatch_table.CmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdCopyImageToBuffer(
    VkCommandBuffer commandBuffer,
    VkImage srcImage,
    VkImageLayout srcImageLayout,
    VkBuffer dstBuffer,
    uint32_t regionCount,
    const VkBufferImageCopy* pRegions) {
    log_layer_message("Generated Layer vkCmdCopyImageToBuffer");
   layer.created_devices[0].dispatch_table.CmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdUpdateBuffer(
    VkCommandBuffer commandBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize dstOffset,
    VkDeviceSize dataSize,
    const void* pData) {
    log_layer_message("Generated Layer vkCmdUpdateBuffer");
   layer.created_devices[0].dispatch_table.CmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, pData);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdFillBuffer(
    VkCommandBuffer commandBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize dstOffset,
    VkDeviceSize size,
    uint32_t data) {
    log_layer_message("Generated Layer vkCmdFillBuffer");
   layer.created_devices[0].dispatch_table.CmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdClearColorImage(
    VkCommandBuffer commandBuffer,
    VkImage image,
    VkImageLayout imageLayout,
    const VkClearColorValue* pColor,
    uint32_t rangeCount,
    const VkImageSubresourceRange* pRanges) {
    log_layer_message("Generated Layer vkCmdClearColorImage");
   layer.created_devices[0].dispatch_table.CmdClearColorImage(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdClearDepthStencilImage(
    VkCommandBuffer commandBuffer,
    VkImage image,
    VkImageLayout imageLayout,
    const VkClearDepthStencilValue* pDepthStencil,
    uint32_t rangeCount,
    const VkImageSubresourceRange* pRanges) {
    log_layer_message("Generated Layer vkCmdClearDepthStencilImage");
   layer.created_devices[0].dispatch_table.CmdClearDepthStencilImage(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdClearAttachments(
    VkCommandBuffer commandBuffer,
    uint32_t attachmentCount,
    const VkClearAttachment* pAttachments,
    uint32_t rectCount,
    const VkClearRect* pRects) {
    log_layer_message("Generated Layer vkCmdClearAttachments");
   layer.created_devices[0].dispatch_table.CmdClearAttachments(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdResolveImage(
    VkCommandBuffer commandBuffer,
    VkImage srcImage,
    VkImageLayout srcImageLayout,
    VkImage dstImage,
    VkImageLayout dstImageLayout,
    uint32_t regionCount,
    const VkImageResolve* pRegions) {
    log_layer_message("Generated Layer vkCmdResolveImage");
   layer.created_devices[0].dispatch_table.CmdResolveImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetEvent(
    VkCommandBuffer commandBuffer,
    VkEvent event,
    VkPipelineStageFlags stageMask) {
    log_layer_message("Generated Layer vkCmdSetEvent");
   layer.created_devices[0].dispatch_table.CmdSetEvent(commandBuffer, event, stageMask);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdResetEvent(
    VkCommandBuffer commandBuffer,
    VkEvent event,
    VkPipelineStageFlags stageMask) {
    log_layer_message("Generated Layer vkCmdResetEvent");
   layer.created_devices[0].dispatch_table.CmdResetEvent(commandBuffer, event, stageMask);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdWaitEvents(
    VkCommandBuffer commandBuffer,
    uint32_t eventCount,
    const VkEvent* pEvents,
    VkPipelineStageFlags srcStageMask,
    VkPipelineStageFlags dstStageMask,
    uint32_t memoryBarrierCount,
    const VkMemoryBarrier* pMemoryBarriers,
    uint32_t bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier* pBufferMemoryBarriers,
    uint32_t imageMemoryBarrierCount,
    const VkImageMemoryBarrier* pImageMemoryBarriers) {
    log_layer_message("Generated Layer vkCmdWaitEvents");
   layer.created_devices[0].dispatch_table.CmdWaitEvents(commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdPipelineBarrier(
    VkCommandBuffer commandBuffer,
    VkPipelineStageFlags srcStageMask,
    VkPipelineStageFlags dstStageMask,
    VkDependencyFlags dependencyFlags,
    uint32_t memoryBarrierCount,
    const VkMemoryBarrier* pMemoryBarriers,
    uint32_t bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier* pBufferMemoryBarriers,
    uint32_t imageMemoryBarrierCount,
    const VkImageMemoryBarrier* pImageMemoryBarriers) {
    log_layer_message("Generated Layer vkCmdPipelineBarrier");
   layer.created_devices[0].dispatch_table.CmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBeginQuery(
    VkCommandBuffer commandBuffer,
    VkQueryPool queryPool,
    uint32_t query,
    VkQueryControlFlags flags) {
    log_layer_message("Generated Layer vkCmdBeginQuery");
   layer.created_devices[0].dispatch_table.CmdBeginQuery(commandBuffer, queryPool, query, flags);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdEndQuery(
    VkCommandBuffer commandBuffer,
    VkQueryPool queryPool,
    uint32_t query) {
    log_layer_message("Generated Layer vkCmdEndQuery");
   layer.created_devices[0].dispatch_table.CmdEndQuery(commandBuffer, queryPool, query);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdResetQueryPool(
    VkCommandBuffer commandBuffer,
    VkQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount) {
    log_layer_message("Generated Layer vkCmdResetQueryPool");
   layer.created_devices[0].dispatch_table.CmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdWriteTimestamp(
    VkCommandBuffer commandBuffer,
    VkPipelineStageFlagBits pipelineStage,
    VkQueryPool queryPool,
    uint32_t query) {
    log_layer_message("Generated Layer vkCmdWriteTimestamp");
   layer.created_devices[0].dispatch_table.CmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, query);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdCopyQueryPoolResults(
    VkCommandBuffer commandBuffer,
    VkQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount,
    VkBuffer dstBuffer,
    VkDeviceSize dstOffset,
    VkDeviceSize stride,
    VkQueryResultFlags flags) {
    log_layer_message("Generated Layer vkCmdCopyQueryPoolResults");
   layer.created_devices[0].dispatch_table.CmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdPushConstants(
    VkCommandBuffer commandBuffer,
    VkPipelineLayout layout,
    VkShaderStageFlags stageFlags,
    uint32_t offset,
    uint32_t size,
    const void* pValues) {
    log_layer_message("Generated Layer vkCmdPushConstants");
   layer.created_devices[0].dispatch_table.CmdPushConstants(commandBuffer, layout, stageFlags, offset, size, pValues);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBeginRenderPass(
    VkCommandBuffer commandBuffer,
    const VkRenderPassBeginInfo* pRenderPassBegin,
    VkSubpassContents contents) {
    log_layer_message("Generated Layer vkCmdBeginRenderPass");
   layer.created_devices[0].dispatch_table.CmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdNextSubpass(
    VkCommandBuffer commandBuffer,
    VkSubpassContents contents) {
    log_layer_message("Generated Layer vkCmdNextSubpass");
   layer.created_devices[0].dispatch_table.CmdNextSubpass(commandBuffer, contents);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdEndRenderPass(
    VkCommandBuffer commandBuffer) {
    log_layer_message("Generated Layer vkCmdEndRenderPass");
   layer.created_devices[0].dispatch_table.CmdEndRenderPass(commandBuffer);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdExecuteCommands(
    VkCommandBuffer commandBuffer,
    uint32_t commandBufferCount,
    const VkCommandBuffer* pCommandBuffers) {
    log_layer_message("Generated Layer vkCmdExecuteCommands");
   layer.created_devices[0].dispatch_table.CmdExecuteCommands(commandBuffer, commandBufferCount, pCommandBuffers);
}


// ----- VK_VERSION_1_1
VKAPI_ATTR VkResult VKAPI_CALL layer_BindBufferMemory2(
    VkDevice device,
    uint32_t bindInfoCount,
    const VkBindBufferMemoryInfo* pBindInfos) {
    log_layer_message("Generated Layer vkBindBufferMemory2");
    return layer.created_devices[0].dispatch_table.BindBufferMemory2(device, bindInfoCount, pBindInfos);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_BindImageMemory2(
    VkDevice device,
    uint32_t bindInfoCount,
    const VkBindImageMemoryInfo* pBindInfos) {
    log_layer_message("Generated Layer vkBindImageMemory2");
    return layer.created_devices[0].dispatch_table.BindImageMemory2(device, bindInfoCount, pBindInfos);
}

VKAPI_ATTR void VKAPI_CALL layer_GetDeviceGroupPeerMemoryFeatures(
    VkDevice device,
    uint32_t heapIndex,
    uint32_t localDeviceIndex,
    uint32_t remoteDeviceIndex,
    VkPeerMemoryFeatureFlags* pPeerMemoryFeatures) {
    log_layer_message("Generated Layer vkGetDeviceGroupPeerMemoryFeatures");
   layer.created_devices[0].dispatch_table.GetDeviceGroupPeerMemoryFeatures(device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetDeviceMask(
    VkCommandBuffer commandBuffer,
    uint32_t deviceMask) {
    log_layer_message("Generated Layer vkCmdSetDeviceMask");
   layer.created_devices[0].dispatch_table.CmdSetDeviceMask(commandBuffer, deviceMask);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdDispatchBase(
    VkCommandBuffer commandBuffer,
    uint32_t baseGroupX,
    uint32_t baseGroupY,
    uint32_t baseGroupZ,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ) {
    log_layer_message("Generated Layer vkCmdDispatchBase");
   layer.created_devices[0].dispatch_table.CmdDispatchBase(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_EnumeratePhysicalDeviceGroups(
    VkInstance instance,
    uint32_t* pPhysicalDeviceGroupCount,
    VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties) {
    log_layer_message("Generated Layer vkEnumeratePhysicalDeviceGroups");
    return layer.instance_dispatch_table.EnumeratePhysicalDeviceGroups(instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
}

VKAPI_ATTR void VKAPI_CALL layer_GetImageMemoryRequirements2(
    VkDevice device,
    const VkImageMemoryRequirementsInfo2* pInfo,
    VkMemoryRequirements2* pMemoryRequirements) {
    log_layer_message("Generated Layer vkGetImageMemoryRequirements2");
   layer.created_devices[0].dispatch_table.GetImageMemoryRequirements2(device, pInfo, pMemoryRequirements);
}

VKAPI_ATTR void VKAPI_CALL layer_GetBufferMemoryRequirements2(
    VkDevice device,
    const VkBufferMemoryRequirementsInfo2* pInfo,
    VkMemoryRequirements2* pMemoryRequirements) {
    log_layer_message("Generated Layer vkGetBufferMemoryRequirements2");
   layer.created_devices[0].dispatch_table.GetBufferMemoryRequirements2(device, pInfo, pMemoryRequirements);
}

VKAPI_ATTR void VKAPI_CALL layer_GetImageSparseMemoryRequirements2(
    VkDevice device,
    const VkImageSparseMemoryRequirementsInfo2* pInfo,
    uint32_t* pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2* pSparseMemoryRequirements) {
    log_layer_message("Generated Layer vkGetImageSparseMemoryRequirements2");
   layer.created_devices[0].dispatch_table.GetImageSparseMemoryRequirements2(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceFeatures2(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceFeatures2* pFeatures) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceFeatures2");
   layer.instance_dispatch_table.GetPhysicalDeviceFeatures2(physicalDevice, pFeatures);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceProperties2(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceProperties2* pProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceProperties2");
   layer.instance_dispatch_table.GetPhysicalDeviceProperties2(physicalDevice, pProperties);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceFormatProperties2(
    VkPhysicalDevice physicalDevice,
    VkFormat format,
    VkFormatProperties2* pFormatProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceFormatProperties2");
   layer.instance_dispatch_table.GetPhysicalDeviceFormatProperties2(physicalDevice, format, pFormatProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceImageFormatProperties2(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo,
    VkImageFormatProperties2* pImageFormatProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceImageFormatProperties2");
    return layer.instance_dispatch_table.GetPhysicalDeviceImageFormatProperties2(physicalDevice, pImageFormatInfo, pImageFormatProperties);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceQueueFamilyProperties2(
    VkPhysicalDevice physicalDevice,
    uint32_t* pQueueFamilyPropertyCount,
    VkQueueFamilyProperties2* pQueueFamilyProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceQueueFamilyProperties2");
   layer.instance_dispatch_table.GetPhysicalDeviceQueueFamilyProperties2(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceMemoryProperties2(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceMemoryProperties2* pMemoryProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceMemoryProperties2");
   layer.instance_dispatch_table.GetPhysicalDeviceMemoryProperties2(physicalDevice, pMemoryProperties);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceSparseImageFormatProperties2(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo,
    uint32_t* pPropertyCount,
    VkSparseImageFormatProperties2* pProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceSparseImageFormatProperties2");
   layer.instance_dispatch_table.GetPhysicalDeviceSparseImageFormatProperties2(physicalDevice, pFormatInfo, pPropertyCount, pProperties);
}

VKAPI_ATTR void VKAPI_CALL layer_TrimCommandPool(
    VkDevice device,
    VkCommandPool commandPool,
    VkCommandPoolTrimFlags flags) {
    log_layer_message("Generated Layer vkTrimCommandPool");
   layer.created_devices[0].dispatch_table.TrimCommandPool(device, commandPool, flags);
}

VKAPI_ATTR void VKAPI_CALL layer_GetDeviceQueue2(
    VkDevice device,
    const VkDeviceQueueInfo2* pQueueInfo,
    VkQueue* pQueue) {
    log_layer_message("Generated Layer vkGetDeviceQueue2");
   layer.created_devices[0].dispatch_table.GetDeviceQueue2(device, pQueueInfo, pQueue);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateSamplerYcbcrConversion(
    VkDevice device,
    const VkSamplerYcbcrConversionCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSamplerYcbcrConversion* pYcbcrConversion) {
    log_layer_message("Generated Layer vkCreateSamplerYcbcrConversion");
    return layer.created_devices[0].dispatch_table.CreateSamplerYcbcrConversion(device, pCreateInfo, pAllocator, pYcbcrConversion);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroySamplerYcbcrConversion(
    VkDevice device,
    VkSamplerYcbcrConversion ycbcrConversion,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroySamplerYcbcrConversion");
   layer.created_devices[0].dispatch_table.DestroySamplerYcbcrConversion(device, ycbcrConversion, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateDescriptorUpdateTemplate(
    VkDevice device,
    const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDescriptorUpdateTemplate* pDescriptorUpdateTemplate) {
    log_layer_message("Generated Layer vkCreateDescriptorUpdateTemplate");
    return layer.created_devices[0].dispatch_table.CreateDescriptorUpdateTemplate(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyDescriptorUpdateTemplate(
    VkDevice device,
    VkDescriptorUpdateTemplate descriptorUpdateTemplate,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyDescriptorUpdateTemplate");
   layer.created_devices[0].dispatch_table.DestroyDescriptorUpdateTemplate(device, descriptorUpdateTemplate, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL layer_UpdateDescriptorSetWithTemplate(
    VkDevice device,
    VkDescriptorSet descriptorSet,
    VkDescriptorUpdateTemplate descriptorUpdateTemplate,
    const void* pData) {
    log_layer_message("Generated Layer vkUpdateDescriptorSetWithTemplate");
   layer.created_devices[0].dispatch_table.UpdateDescriptorSetWithTemplate(device, descriptorSet, descriptorUpdateTemplate, pData);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceExternalBufferProperties(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo,
    VkExternalBufferProperties* pExternalBufferProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceExternalBufferProperties");
   layer.instance_dispatch_table.GetPhysicalDeviceExternalBufferProperties(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceExternalFenceProperties(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo,
    VkExternalFenceProperties* pExternalFenceProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceExternalFenceProperties");
   layer.instance_dispatch_table.GetPhysicalDeviceExternalFenceProperties(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceExternalSemaphoreProperties(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo,
    VkExternalSemaphoreProperties* pExternalSemaphoreProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceExternalSemaphoreProperties");
   layer.instance_dispatch_table.GetPhysicalDeviceExternalSemaphoreProperties(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
}

VKAPI_ATTR void VKAPI_CALL layer_GetDescriptorSetLayoutSupport(
    VkDevice device,
    const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
    VkDescriptorSetLayoutSupport* pSupport) {
    log_layer_message("Generated Layer vkGetDescriptorSetLayoutSupport");
   layer.created_devices[0].dispatch_table.GetDescriptorSetLayoutSupport(device, pCreateInfo, pSupport);
}


// ----- VK_VERSION_1_2
VKAPI_ATTR void VKAPI_CALL layer_CmdDrawIndirectCount(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    VkBuffer countBuffer,
    VkDeviceSize countBufferOffset,
    uint32_t maxDrawCount,
    uint32_t stride) {
    log_layer_message("Generated Layer vkCmdDrawIndirectCount");
   layer.created_devices[0].dispatch_table.CmdDrawIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdDrawIndexedIndirectCount(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    VkBuffer countBuffer,
    VkDeviceSize countBufferOffset,
    uint32_t maxDrawCount,
    uint32_t stride) {
    log_layer_message("Generated Layer vkCmdDrawIndexedIndirectCount");
   layer.created_devices[0].dispatch_table.CmdDrawIndexedIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateRenderPass2(
    VkDevice device,
    const VkRenderPassCreateInfo2* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkRenderPass* pRenderPass) {
    log_layer_message("Generated Layer vkCreateRenderPass2");
    return layer.created_devices[0].dispatch_table.CreateRenderPass2(device, pCreateInfo, pAllocator, pRenderPass);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBeginRenderPass2(
    VkCommandBuffer commandBuffer,
    const VkRenderPassBeginInfo* pRenderPassBegin,
    const VkSubpassBeginInfo* pSubpassBeginInfo) {
    log_layer_message("Generated Layer vkCmdBeginRenderPass2");
   layer.created_devices[0].dispatch_table.CmdBeginRenderPass2(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdNextSubpass2(
    VkCommandBuffer commandBuffer,
    const VkSubpassBeginInfo* pSubpassBeginInfo,
    const VkSubpassEndInfo* pSubpassEndInfo) {
    log_layer_message("Generated Layer vkCmdNextSubpass2");
   layer.created_devices[0].dispatch_table.CmdNextSubpass2(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdEndRenderPass2(
    VkCommandBuffer commandBuffer,
    const VkSubpassEndInfo* pSubpassEndInfo) {
    log_layer_message("Generated Layer vkCmdEndRenderPass2");
   layer.created_devices[0].dispatch_table.CmdEndRenderPass2(commandBuffer, pSubpassEndInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_ResetQueryPool(
    VkDevice device,
    VkQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount) {
    log_layer_message("Generated Layer vkResetQueryPool");
   layer.created_devices[0].dispatch_table.ResetQueryPool(device, queryPool, firstQuery, queryCount);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetSemaphoreCounterValue(
    VkDevice device,
    VkSemaphore semaphore,
    uint64_t* pValue) {
    log_layer_message("Generated Layer vkGetSemaphoreCounterValue");
    return layer.created_devices[0].dispatch_table.GetSemaphoreCounterValue(device, semaphore, pValue);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_WaitSemaphores(
    VkDevice device,
    const VkSemaphoreWaitInfo* pWaitInfo,
    uint64_t timeout) {
    log_layer_message("Generated Layer vkWaitSemaphores");
    return layer.created_devices[0].dispatch_table.WaitSemaphores(device, pWaitInfo, timeout);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_SignalSemaphore(
    VkDevice device,
    const VkSemaphoreSignalInfo* pSignalInfo) {
    log_layer_message("Generated Layer vkSignalSemaphore");
    return layer.created_devices[0].dispatch_table.SignalSemaphore(device, pSignalInfo);
}

VKAPI_ATTR VkDeviceAddress VKAPI_CALL layer_GetBufferDeviceAddress(
    VkDevice device,
    const VkBufferDeviceAddressInfo* pInfo) {
    log_layer_message("Generated Layer vkGetBufferDeviceAddress");
    return layer.created_devices[0].dispatch_table.GetBufferDeviceAddress(device, pInfo);
}

VKAPI_ATTR uint64_t VKAPI_CALL layer_GetBufferOpaqueCaptureAddress(
    VkDevice device,
    const VkBufferDeviceAddressInfo* pInfo) {
    log_layer_message("Generated Layer vkGetBufferOpaqueCaptureAddress");
    return layer.created_devices[0].dispatch_table.GetBufferOpaqueCaptureAddress(device, pInfo);
}

VKAPI_ATTR uint64_t VKAPI_CALL layer_GetDeviceMemoryOpaqueCaptureAddress(
    VkDevice device,
    const VkDeviceMemoryOpaqueCaptureAddressInfo* pInfo) {
    log_layer_message("Generated Layer vkGetDeviceMemoryOpaqueCaptureAddress");
    return layer.created_devices[0].dispatch_table.GetDeviceMemoryOpaqueCaptureAddress(device, pInfo);
}


// ----- VK_VERSION_1_3
VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceToolProperties(
    VkPhysicalDevice physicalDevice,
    uint32_t* pToolCount,
    VkPhysicalDeviceToolProperties* pToolProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceToolProperties");
    return layer.instance_dispatch_table.GetPhysicalDeviceToolProperties(physicalDevice, pToolCount, pToolProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreatePrivateDataSlot(
    VkDevice device,
    const VkPrivateDataSlotCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkPrivateDataSlot* pPrivateDataSlot) {
    log_layer_message("Generated Layer vkCreatePrivateDataSlot");
    return layer.created_devices[0].dispatch_table.CreatePrivateDataSlot(device, pCreateInfo, pAllocator, pPrivateDataSlot);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyPrivateDataSlot(
    VkDevice device,
    VkPrivateDataSlot privateDataSlot,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyPrivateDataSlot");
   layer.created_devices[0].dispatch_table.DestroyPrivateDataSlot(device, privateDataSlot, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_SetPrivateData(
    VkDevice device,
    VkObjectType objectType,
    uint64_t objectHandle,
    VkPrivateDataSlot privateDataSlot,
    uint64_t data) {
    log_layer_message("Generated Layer vkSetPrivateData");
    return layer.created_devices[0].dispatch_table.SetPrivateData(device, objectType, objectHandle, privateDataSlot, data);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPrivateData(
    VkDevice device,
    VkObjectType objectType,
    uint64_t objectHandle,
    VkPrivateDataSlot privateDataSlot,
    uint64_t* pData) {
    log_layer_message("Generated Layer vkGetPrivateData");
   layer.created_devices[0].dispatch_table.GetPrivateData(device, objectType, objectHandle, privateDataSlot, pData);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetEvent2(
    VkCommandBuffer commandBuffer,
    VkEvent event,
    const VkDependencyInfo* pDependencyInfo) {
    log_layer_message("Generated Layer vkCmdSetEvent2");
   layer.created_devices[0].dispatch_table.CmdSetEvent2(commandBuffer, event, pDependencyInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdResetEvent2(
    VkCommandBuffer commandBuffer,
    VkEvent event,
    VkPipelineStageFlags2 stageMask) {
    log_layer_message("Generated Layer vkCmdResetEvent2");
   layer.created_devices[0].dispatch_table.CmdResetEvent2(commandBuffer, event, stageMask);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdWaitEvents2(
    VkCommandBuffer commandBuffer,
    uint32_t eventCount,
    const VkEvent* pEvents,
    const VkDependencyInfo* pDependencyInfos) {
    log_layer_message("Generated Layer vkCmdWaitEvents2");
   layer.created_devices[0].dispatch_table.CmdWaitEvents2(commandBuffer, eventCount, pEvents, pDependencyInfos);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdPipelineBarrier2(
    VkCommandBuffer commandBuffer,
    const VkDependencyInfo* pDependencyInfo) {
    log_layer_message("Generated Layer vkCmdPipelineBarrier2");
   layer.created_devices[0].dispatch_table.CmdPipelineBarrier2(commandBuffer, pDependencyInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdWriteTimestamp2(
    VkCommandBuffer commandBuffer,
    VkPipelineStageFlags2 stage,
    VkQueryPool queryPool,
    uint32_t query) {
    log_layer_message("Generated Layer vkCmdWriteTimestamp2");
   layer.created_devices[0].dispatch_table.CmdWriteTimestamp2(commandBuffer, stage, queryPool, query);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_QueueSubmit2(
    VkQueue queue,
    uint32_t submitCount,
    const VkSubmitInfo2* pSubmits,
    VkFence fence) {
    log_layer_message("Generated Layer vkQueueSubmit2");
    return layer.created_devices[0].dispatch_table.QueueSubmit2(queue, submitCount, pSubmits, fence);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdCopyBuffer2(
    VkCommandBuffer commandBuffer,
    const VkCopyBufferInfo2* pCopyBufferInfo) {
    log_layer_message("Generated Layer vkCmdCopyBuffer2");
   layer.created_devices[0].dispatch_table.CmdCopyBuffer2(commandBuffer, pCopyBufferInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdCopyImage2(
    VkCommandBuffer commandBuffer,
    const VkCopyImageInfo2* pCopyImageInfo) {
    log_layer_message("Generated Layer vkCmdCopyImage2");
   layer.created_devices[0].dispatch_table.CmdCopyImage2(commandBuffer, pCopyImageInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdCopyBufferToImage2(
    VkCommandBuffer commandBuffer,
    const VkCopyBufferToImageInfo2* pCopyBufferToImageInfo) {
    log_layer_message("Generated Layer vkCmdCopyBufferToImage2");
   layer.created_devices[0].dispatch_table.CmdCopyBufferToImage2(commandBuffer, pCopyBufferToImageInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdCopyImageToBuffer2(
    VkCommandBuffer commandBuffer,
    const VkCopyImageToBufferInfo2* pCopyImageToBufferInfo) {
    log_layer_message("Generated Layer vkCmdCopyImageToBuffer2");
   layer.created_devices[0].dispatch_table.CmdCopyImageToBuffer2(commandBuffer, pCopyImageToBufferInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBlitImage2(
    VkCommandBuffer commandBuffer,
    const VkBlitImageInfo2* pBlitImageInfo) {
    log_layer_message("Generated Layer vkCmdBlitImage2");
   layer.created_devices[0].dispatch_table.CmdBlitImage2(commandBuffer, pBlitImageInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdResolveImage2(
    VkCommandBuffer commandBuffer,
    const VkResolveImageInfo2* pResolveImageInfo) {
    log_layer_message("Generated Layer vkCmdResolveImage2");
   layer.created_devices[0].dispatch_table.CmdResolveImage2(commandBuffer, pResolveImageInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBeginRendering(
    VkCommandBuffer commandBuffer,
    const VkRenderingInfo* pRenderingInfo) {
    log_layer_message("Generated Layer vkCmdBeginRendering");
   layer.created_devices[0].dispatch_table.CmdBeginRendering(commandBuffer, pRenderingInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdEndRendering(
    VkCommandBuffer commandBuffer) {
    log_layer_message("Generated Layer vkCmdEndRendering");
   layer.created_devices[0].dispatch_table.CmdEndRendering(commandBuffer);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetCullMode(
    VkCommandBuffer commandBuffer,
    VkCullModeFlags cullMode) {
    log_layer_message("Generated Layer vkCmdSetCullMode");
   layer.created_devices[0].dispatch_table.CmdSetCullMode(commandBuffer, cullMode);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetFrontFace(
    VkCommandBuffer commandBuffer,
    VkFrontFace frontFace) {
    log_layer_message("Generated Layer vkCmdSetFrontFace");
   layer.created_devices[0].dispatch_table.CmdSetFrontFace(commandBuffer, frontFace);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetPrimitiveTopology(
    VkCommandBuffer commandBuffer,
    VkPrimitiveTopology primitiveTopology) {
    log_layer_message("Generated Layer vkCmdSetPrimitiveTopology");
   layer.created_devices[0].dispatch_table.CmdSetPrimitiveTopology(commandBuffer, primitiveTopology);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetViewportWithCount(
    VkCommandBuffer commandBuffer,
    uint32_t viewportCount,
    const VkViewport* pViewports) {
    log_layer_message("Generated Layer vkCmdSetViewportWithCount");
   layer.created_devices[0].dispatch_table.CmdSetViewportWithCount(commandBuffer, viewportCount, pViewports);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetScissorWithCount(
    VkCommandBuffer commandBuffer,
    uint32_t scissorCount,
    const VkRect2D* pScissors) {
    log_layer_message("Generated Layer vkCmdSetScissorWithCount");
   layer.created_devices[0].dispatch_table.CmdSetScissorWithCount(commandBuffer, scissorCount, pScissors);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBindVertexBuffers2(
    VkCommandBuffer commandBuffer,
    uint32_t firstBinding,
    uint32_t bindingCount,
    const VkBuffer* pBuffers,
    const VkDeviceSize* pOffsets,
    const VkDeviceSize* pSizes,
    const VkDeviceSize* pStrides) {
    log_layer_message("Generated Layer vkCmdBindVertexBuffers2");
   layer.created_devices[0].dispatch_table.CmdBindVertexBuffers2(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes, pStrides);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetDepthTestEnable(
    VkCommandBuffer commandBuffer,
    VkBool32 depthTestEnable) {
    log_layer_message("Generated Layer vkCmdSetDepthTestEnable");
   layer.created_devices[0].dispatch_table.CmdSetDepthTestEnable(commandBuffer, depthTestEnable);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetDepthWriteEnable(
    VkCommandBuffer commandBuffer,
    VkBool32 depthWriteEnable) {
    log_layer_message("Generated Layer vkCmdSetDepthWriteEnable");
   layer.created_devices[0].dispatch_table.CmdSetDepthWriteEnable(commandBuffer, depthWriteEnable);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetDepthCompareOp(
    VkCommandBuffer commandBuffer,
    VkCompareOp depthCompareOp) {
    log_layer_message("Generated Layer vkCmdSetDepthCompareOp");
   layer.created_devices[0].dispatch_table.CmdSetDepthCompareOp(commandBuffer, depthCompareOp);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetDepthBoundsTestEnable(
    VkCommandBuffer commandBuffer,
    VkBool32 depthBoundsTestEnable) {
    log_layer_message("Generated Layer vkCmdSetDepthBoundsTestEnable");
   layer.created_devices[0].dispatch_table.CmdSetDepthBoundsTestEnable(commandBuffer, depthBoundsTestEnable);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetStencilTestEnable(
    VkCommandBuffer commandBuffer,
    VkBool32 stencilTestEnable) {
    log_layer_message("Generated Layer vkCmdSetStencilTestEnable");
   layer.created_devices[0].dispatch_table.CmdSetStencilTestEnable(commandBuffer, stencilTestEnable);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetStencilOp(
    VkCommandBuffer commandBuffer,
    VkStencilFaceFlags faceMask,
    VkStencilOp failOp,
    VkStencilOp passOp,
    VkStencilOp depthFailOp,
    VkCompareOp compareOp) {
    log_layer_message("Generated Layer vkCmdSetStencilOp");
   layer.created_devices[0].dispatch_table.CmdSetStencilOp(commandBuffer, faceMask, failOp, passOp, depthFailOp, compareOp);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetRasterizerDiscardEnable(
    VkCommandBuffer commandBuffer,
    VkBool32 rasterizerDiscardEnable) {
    log_layer_message("Generated Layer vkCmdSetRasterizerDiscardEnable");
   layer.created_devices[0].dispatch_table.CmdSetRasterizerDiscardEnable(commandBuffer, rasterizerDiscardEnable);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetDepthBiasEnable(
    VkCommandBuffer commandBuffer,
    VkBool32 depthBiasEnable) {
    log_layer_message("Generated Layer vkCmdSetDepthBiasEnable");
   layer.created_devices[0].dispatch_table.CmdSetDepthBiasEnable(commandBuffer, depthBiasEnable);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetPrimitiveRestartEnable(
    VkCommandBuffer commandBuffer,
    VkBool32 primitiveRestartEnable) {
    log_layer_message("Generated Layer vkCmdSetPrimitiveRestartEnable");
   layer.created_devices[0].dispatch_table.CmdSetPrimitiveRestartEnable(commandBuffer, primitiveRestartEnable);
}

VKAPI_ATTR void VKAPI_CALL layer_GetDeviceBufferMemoryRequirements(
    VkDevice device,
    const VkDeviceBufferMemoryRequirements* pInfo,
    VkMemoryRequirements2* pMemoryRequirements) {
    log_layer_message("Generated Layer vkGetDeviceBufferMemoryRequirements");
   layer.created_devices[0].dispatch_table.GetDeviceBufferMemoryRequirements(device, pInfo, pMemoryRequirements);
}

VKAPI_ATTR void VKAPI_CALL layer_GetDeviceImageMemoryRequirements(
    VkDevice device,
    const VkDeviceImageMemoryRequirements* pInfo,
    VkMemoryRequirements2* pMemoryRequirements) {
    log_layer_message("Generated Layer vkGetDeviceImageMemoryRequirements");
   layer.created_devices[0].dispatch_table.GetDeviceImageMemoryRequirements(device, pInfo, pMemoryRequirements);
}

VKAPI_ATTR void VKAPI_CALL layer_GetDeviceImageSparseMemoryRequirements(
    VkDevice device,
    const VkDeviceImageMemoryRequirements* pInfo,
    uint32_t* pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2* pSparseMemoryRequirements) {
    log_layer_message("Generated Layer vkGetDeviceImageSparseMemoryRequirements");
   layer.created_devices[0].dispatch_table.GetDeviceImageSparseMemoryRequirements(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}


// ----- VK_KHR_surface
VKAPI_ATTR void VKAPI_CALL layer_DestroySurfaceKHR(
    VkInstance instance,
    VkSurfaceKHR surface,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroySurfaceKHR");
   layer.instance_dispatch_table.DestroySurfaceKHR(instance, surface, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    VkSurfaceKHR surface,
    VkBool32* pSupported) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceSurfaceSupportKHR");
    return layer.instance_dispatch_table.GetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, pSupported);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    VkSurfaceCapabilitiesKHR* pSurfaceCapabilities) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    return layer.instance_dispatch_table.GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, pSurfaceCapabilities);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    uint32_t* pSurfaceFormatCount,
    VkSurfaceFormatKHR* pSurfaceFormats) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceSurfaceFormatsKHR");
    return layer.instance_dispatch_table.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    uint32_t* pPresentModeCount,
    VkPresentModeKHR* pPresentModes) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceSurfacePresentModesKHR");
    return layer.instance_dispatch_table.GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, pPresentModeCount, pPresentModes);
}


// ----- VK_KHR_swapchain
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateSwapchainKHR(
    VkDevice device,
    const VkSwapchainCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSwapchainKHR* pSwapchain) {
    log_layer_message("Generated Layer vkCreateSwapchainKHR");
    return layer.created_devices[0].dispatch_table.CreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroySwapchainKHR(
    VkDevice device,
    VkSwapchainKHR swapchain,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroySwapchainKHR");
   layer.created_devices[0].dispatch_table.DestroySwapchainKHR(device, swapchain, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetSwapchainImagesKHR(
    VkDevice device,
    VkSwapchainKHR swapchain,
    uint32_t* pSwapchainImageCount,
    VkImage* pSwapchainImages) {
    log_layer_message("Generated Layer vkGetSwapchainImagesKHR");
    return layer.created_devices[0].dispatch_table.GetSwapchainImagesKHR(device, swapchain, pSwapchainImageCount, pSwapchainImages);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_AcquireNextImageKHR(
    VkDevice device,
    VkSwapchainKHR swapchain,
    uint64_t timeout,
    VkSemaphore semaphore,
    VkFence fence,
    uint32_t* pImageIndex) {
    log_layer_message("Generated Layer vkAcquireNextImageKHR");
    return layer.created_devices[0].dispatch_table.AcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, pImageIndex);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_QueuePresentKHR(
    VkQueue queue,
    const VkPresentInfoKHR* pPresentInfo) {
    log_layer_message("Generated Layer vkQueuePresentKHR");
    return layer.created_devices[0].dispatch_table.QueuePresentKHR(queue, pPresentInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetDeviceGroupPresentCapabilitiesKHR(
    VkDevice device,
    VkDeviceGroupPresentCapabilitiesKHR* pDeviceGroupPresentCapabilities) {
    log_layer_message("Generated Layer vkGetDeviceGroupPresentCapabilitiesKHR");
    return layer.created_devices[0].dispatch_table.GetDeviceGroupPresentCapabilitiesKHR(device, pDeviceGroupPresentCapabilities);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetDeviceGroupSurfacePresentModesKHR(
    VkDevice device,
    VkSurfaceKHR surface,
    VkDeviceGroupPresentModeFlagsKHR* pModes) {
    log_layer_message("Generated Layer vkGetDeviceGroupSurfacePresentModesKHR");
    return layer.created_devices[0].dispatch_table.GetDeviceGroupSurfacePresentModesKHR(device, surface, pModes);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDevicePresentRectanglesKHR(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    uint32_t* pRectCount,
    VkRect2D* pRects) {
    log_layer_message("Generated Layer vkGetPhysicalDevicePresentRectanglesKHR");
    return layer.instance_dispatch_table.GetPhysicalDevicePresentRectanglesKHR(physicalDevice, surface, pRectCount, pRects);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_AcquireNextImage2KHR(
    VkDevice device,
    const VkAcquireNextImageInfoKHR* pAcquireInfo,
    uint32_t* pImageIndex) {
    log_layer_message("Generated Layer vkAcquireNextImage2KHR");
    return layer.created_devices[0].dispatch_table.AcquireNextImage2KHR(device, pAcquireInfo, pImageIndex);
}


// ----- VK_KHR_display
VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceDisplayPropertiesKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t* pPropertyCount,
    VkDisplayPropertiesKHR* pProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceDisplayPropertiesKHR");
    return layer.instance_dispatch_table.GetPhysicalDeviceDisplayPropertiesKHR(physicalDevice, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceDisplayPlanePropertiesKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t* pPropertyCount,
    VkDisplayPlanePropertiesKHR* pProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceDisplayPlanePropertiesKHR");
    return layer.instance_dispatch_table.GetPhysicalDeviceDisplayPlanePropertiesKHR(physicalDevice, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetDisplayPlaneSupportedDisplaysKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t planeIndex,
    uint32_t* pDisplayCount,
    VkDisplayKHR* pDisplays) {
    log_layer_message("Generated Layer vkGetDisplayPlaneSupportedDisplaysKHR");
    return layer.instance_dispatch_table.GetDisplayPlaneSupportedDisplaysKHR(physicalDevice, planeIndex, pDisplayCount, pDisplays);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetDisplayModePropertiesKHR(
    VkPhysicalDevice physicalDevice,
    VkDisplayKHR display,
    uint32_t* pPropertyCount,
    VkDisplayModePropertiesKHR* pProperties) {
    log_layer_message("Generated Layer vkGetDisplayModePropertiesKHR");
    return layer.instance_dispatch_table.GetDisplayModePropertiesKHR(physicalDevice, display, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateDisplayModeKHR(
    VkPhysicalDevice physicalDevice,
    VkDisplayKHR display,
    const VkDisplayModeCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDisplayModeKHR* pMode) {
    log_layer_message("Generated Layer vkCreateDisplayModeKHR");
    return layer.instance_dispatch_table.CreateDisplayModeKHR(physicalDevice, display, pCreateInfo, pAllocator, pMode);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetDisplayPlaneCapabilitiesKHR(
    VkPhysicalDevice physicalDevice,
    VkDisplayModeKHR mode,
    uint32_t planeIndex,
    VkDisplayPlaneCapabilitiesKHR* pCapabilities) {
    log_layer_message("Generated Layer vkGetDisplayPlaneCapabilitiesKHR");
    return layer.instance_dispatch_table.GetDisplayPlaneCapabilitiesKHR(physicalDevice, mode, planeIndex, pCapabilities);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateDisplayPlaneSurfaceKHR(
    VkInstance instance,
    const VkDisplaySurfaceCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_layer_message("Generated Layer vkCreateDisplayPlaneSurfaceKHR");
    return layer.instance_dispatch_table.CreateDisplayPlaneSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
}


// ----- VK_KHR_display_swapchain
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateSharedSwapchainsKHR(
    VkDevice device,
    uint32_t swapchainCount,
    const VkSwapchainCreateInfoKHR* pCreateInfos,
    const VkAllocationCallbacks* pAllocator,
    VkSwapchainKHR* pSwapchains) {
    log_layer_message("Generated Layer vkCreateSharedSwapchainsKHR");
    return layer.created_devices[0].dispatch_table.CreateSharedSwapchainsKHR(device, swapchainCount, pCreateInfos, pAllocator, pSwapchains);
}


// ----- VK_KHR_xlib_surface
#ifdef VK_USE_PLATFORM_XLIB_KHR
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateXlibSurfaceKHR(
    VkInstance instance,
    const VkXlibSurfaceCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_layer_message("Generated Layer vkCreateXlibSurfaceKHR");
    return layer.instance_dispatch_table.CreateXlibSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkBool32 VKAPI_CALL layer_GetPhysicalDeviceXlibPresentationSupportKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    Display* dpy,
    VisualID visualID) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceXlibPresentationSupportKHR");
    return layer.instance_dispatch_table.GetPhysicalDeviceXlibPresentationSupportKHR(physicalDevice, queueFamilyIndex, dpy, visualID);
}

#endif // VK_USE_PLATFORM_XLIB_KHR

// ----- VK_KHR_xcb_surface
#ifdef VK_USE_PLATFORM_XCB_KHR
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateXcbSurfaceKHR(
    VkInstance instance,
    const VkXcbSurfaceCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_layer_message("Generated Layer vkCreateXcbSurfaceKHR");
    return layer.instance_dispatch_table.CreateXcbSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkBool32 VKAPI_CALL layer_GetPhysicalDeviceXcbPresentationSupportKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    xcb_connection_t* connection,
    xcb_visualid_t visual_id) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceXcbPresentationSupportKHR");
    return layer.instance_dispatch_table.GetPhysicalDeviceXcbPresentationSupportKHR(physicalDevice, queueFamilyIndex, connection, visual_id);
}

#endif // VK_USE_PLATFORM_XCB_KHR

// ----- VK_KHR_wayland_surface
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateWaylandSurfaceKHR(
    VkInstance instance,
    const VkWaylandSurfaceCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_layer_message("Generated Layer vkCreateWaylandSurfaceKHR");
    return layer.instance_dispatch_table.CreateWaylandSurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkBool32 VKAPI_CALL layer_GetPhysicalDeviceWaylandPresentationSupportKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    struct wl_display* display) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceWaylandPresentationSupportKHR");
    return layer.instance_dispatch_table.GetPhysicalDeviceWaylandPresentationSupportKHR(physicalDevice, queueFamilyIndex, display);
}

#endif // VK_USE_PLATFORM_WAYLAND_KHR

// ----- VK_KHR_win32_surface
#ifdef VK_USE_PLATFORM_WIN32_KHR
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateWin32SurfaceKHR(
    VkInstance instance,
    const VkWin32SurfaceCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_layer_message("Generated Layer vkCreateWin32SurfaceKHR");
    return layer.instance_dispatch_table.CreateWin32SurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkBool32 VKAPI_CALL layer_GetPhysicalDeviceWin32PresentationSupportKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceWin32PresentationSupportKHR");
    return layer.instance_dispatch_table.GetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyIndex);
}

#endif // VK_USE_PLATFORM_WIN32_KHR

// ----- VK_KHR_video_queue
#ifdef VK_ENABLE_BETA_EXTENSIONS
VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceVideoCapabilitiesKHR(
    VkPhysicalDevice physicalDevice,
    const VkVideoProfileKHR* pVideoProfile,
    VkVideoCapabilitiesKHR* pCapabilities) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceVideoCapabilitiesKHR");
    return layer.instance_dispatch_table.GetPhysicalDeviceVideoCapabilitiesKHR(physicalDevice, pVideoProfile, pCapabilities);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceVideoFormatPropertiesKHR(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceVideoFormatInfoKHR* pVideoFormatInfo,
    uint32_t* pVideoFormatPropertyCount,
    VkVideoFormatPropertiesKHR* pVideoFormatProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceVideoFormatPropertiesKHR");
    return layer.instance_dispatch_table.GetPhysicalDeviceVideoFormatPropertiesKHR(physicalDevice, pVideoFormatInfo, pVideoFormatPropertyCount, pVideoFormatProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateVideoSessionKHR(
    VkDevice device,
    const VkVideoSessionCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkVideoSessionKHR* pVideoSession) {
    log_layer_message("Generated Layer vkCreateVideoSessionKHR");
    return layer.created_devices[0].dispatch_table.CreateVideoSessionKHR(device, pCreateInfo, pAllocator, pVideoSession);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyVideoSessionKHR(
    VkDevice device,
    VkVideoSessionKHR videoSession,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyVideoSessionKHR");
   layer.created_devices[0].dispatch_table.DestroyVideoSessionKHR(device, videoSession, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetVideoSessionMemoryRequirementsKHR(
    VkDevice device,
    VkVideoSessionKHR videoSession,
    uint32_t* pVideoSessionMemoryRequirementsCount,
    VkVideoGetMemoryPropertiesKHR* pVideoSessionMemoryRequirements) {
    log_layer_message("Generated Layer vkGetVideoSessionMemoryRequirementsKHR");
    return layer.created_devices[0].dispatch_table.GetVideoSessionMemoryRequirementsKHR(device, videoSession, pVideoSessionMemoryRequirementsCount, pVideoSessionMemoryRequirements);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_BindVideoSessionMemoryKHR(
    VkDevice device,
    VkVideoSessionKHR videoSession,
    uint32_t videoSessionBindMemoryCount,
    const VkVideoBindMemoryKHR* pVideoSessionBindMemories) {
    log_layer_message("Generated Layer vkBindVideoSessionMemoryKHR");
    return layer.created_devices[0].dispatch_table.BindVideoSessionMemoryKHR(device, videoSession, videoSessionBindMemoryCount, pVideoSessionBindMemories);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateVideoSessionParametersKHR(
    VkDevice device,
    const VkVideoSessionParametersCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkVideoSessionParametersKHR* pVideoSessionParameters) {
    log_layer_message("Generated Layer vkCreateVideoSessionParametersKHR");
    return layer.created_devices[0].dispatch_table.CreateVideoSessionParametersKHR(device, pCreateInfo, pAllocator, pVideoSessionParameters);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_UpdateVideoSessionParametersKHR(
    VkDevice device,
    VkVideoSessionParametersKHR videoSessionParameters,
    const VkVideoSessionParametersUpdateInfoKHR* pUpdateInfo) {
    log_layer_message("Generated Layer vkUpdateVideoSessionParametersKHR");
    return layer.created_devices[0].dispatch_table.UpdateVideoSessionParametersKHR(device, videoSessionParameters, pUpdateInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyVideoSessionParametersKHR(
    VkDevice device,
    VkVideoSessionParametersKHR videoSessionParameters,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyVideoSessionParametersKHR");
   layer.created_devices[0].dispatch_table.DestroyVideoSessionParametersKHR(device, videoSessionParameters, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBeginVideoCodingKHR(
    VkCommandBuffer commandBuffer,
    const VkVideoBeginCodingInfoKHR* pBeginInfo) {
    log_layer_message("Generated Layer vkCmdBeginVideoCodingKHR");
   layer.created_devices[0].dispatch_table.CmdBeginVideoCodingKHR(commandBuffer, pBeginInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdEndVideoCodingKHR(
    VkCommandBuffer commandBuffer,
    const VkVideoEndCodingInfoKHR* pEndCodingInfo) {
    log_layer_message("Generated Layer vkCmdEndVideoCodingKHR");
   layer.created_devices[0].dispatch_table.CmdEndVideoCodingKHR(commandBuffer, pEndCodingInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdControlVideoCodingKHR(
    VkCommandBuffer commandBuffer,
    const VkVideoCodingControlInfoKHR* pCodingControlInfo) {
    log_layer_message("Generated Layer vkCmdControlVideoCodingKHR");
   layer.created_devices[0].dispatch_table.CmdControlVideoCodingKHR(commandBuffer, pCodingControlInfo);
}

#endif // VK_ENABLE_BETA_EXTENSIONS

// ----- VK_KHR_video_decode_queue
#ifdef VK_ENABLE_BETA_EXTENSIONS
VKAPI_ATTR void VKAPI_CALL layer_CmdDecodeVideoKHR(
    VkCommandBuffer commandBuffer,
    const VkVideoDecodeInfoKHR* pFrameInfo) {
    log_layer_message("Generated Layer vkCmdDecodeVideoKHR");
   layer.created_devices[0].dispatch_table.CmdDecodeVideoKHR(commandBuffer, pFrameInfo);
}

#endif // VK_ENABLE_BETA_EXTENSIONS

// ----- VK_KHR_dynamic_rendering
VKAPI_ATTR void VKAPI_CALL layer_CmdBeginRenderingKHR(
    VkCommandBuffer commandBuffer,
    const VkRenderingInfo* pRenderingInfo) {
    log_layer_message("Generated Layer vkCmdBeginRenderingKHR");
   layer.created_devices[0].dispatch_table.CmdBeginRenderingKHR(commandBuffer, pRenderingInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdEndRenderingKHR(
    VkCommandBuffer commandBuffer) {
    log_layer_message("Generated Layer vkCmdEndRenderingKHR");
   layer.created_devices[0].dispatch_table.CmdEndRenderingKHR(commandBuffer);
}


// ----- VK_KHR_get_physical_device_properties2
VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceFeatures2KHR(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceFeatures2* pFeatures) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceFeatures2KHR");
   layer.instance_dispatch_table.GetPhysicalDeviceFeatures2KHR(physicalDevice, pFeatures);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceProperties2KHR(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceProperties2* pProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceProperties2KHR");
   layer.instance_dispatch_table.GetPhysicalDeviceProperties2KHR(physicalDevice, pProperties);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceFormatProperties2KHR(
    VkPhysicalDevice physicalDevice,
    VkFormat format,
    VkFormatProperties2* pFormatProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceFormatProperties2KHR");
   layer.instance_dispatch_table.GetPhysicalDeviceFormatProperties2KHR(physicalDevice, format, pFormatProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceImageFormatProperties2KHR(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo,
    VkImageFormatProperties2* pImageFormatProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceImageFormatProperties2KHR");
    return layer.instance_dispatch_table.GetPhysicalDeviceImageFormatProperties2KHR(physicalDevice, pImageFormatInfo, pImageFormatProperties);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceQueueFamilyProperties2KHR(
    VkPhysicalDevice physicalDevice,
    uint32_t* pQueueFamilyPropertyCount,
    VkQueueFamilyProperties2* pQueueFamilyProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceQueueFamilyProperties2KHR");
   layer.instance_dispatch_table.GetPhysicalDeviceQueueFamilyProperties2KHR(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceMemoryProperties2KHR(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceMemoryProperties2* pMemoryProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceMemoryProperties2KHR");
   layer.instance_dispatch_table.GetPhysicalDeviceMemoryProperties2KHR(physicalDevice, pMemoryProperties);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceSparseImageFormatProperties2KHR(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo,
    uint32_t* pPropertyCount,
    VkSparseImageFormatProperties2* pProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceSparseImageFormatProperties2KHR");
   layer.instance_dispatch_table.GetPhysicalDeviceSparseImageFormatProperties2KHR(physicalDevice, pFormatInfo, pPropertyCount, pProperties);
}


// ----- VK_KHR_device_group
VKAPI_ATTR void VKAPI_CALL layer_GetDeviceGroupPeerMemoryFeaturesKHR(
    VkDevice device,
    uint32_t heapIndex,
    uint32_t localDeviceIndex,
    uint32_t remoteDeviceIndex,
    VkPeerMemoryFeatureFlags* pPeerMemoryFeatures) {
    log_layer_message("Generated Layer vkGetDeviceGroupPeerMemoryFeaturesKHR");
   layer.created_devices[0].dispatch_table.GetDeviceGroupPeerMemoryFeaturesKHR(device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetDeviceMaskKHR(
    VkCommandBuffer commandBuffer,
    uint32_t deviceMask) {
    log_layer_message("Generated Layer vkCmdSetDeviceMaskKHR");
   layer.created_devices[0].dispatch_table.CmdSetDeviceMaskKHR(commandBuffer, deviceMask);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdDispatchBaseKHR(
    VkCommandBuffer commandBuffer,
    uint32_t baseGroupX,
    uint32_t baseGroupY,
    uint32_t baseGroupZ,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ) {
    log_layer_message("Generated Layer vkCmdDispatchBaseKHR");
   layer.created_devices[0].dispatch_table.CmdDispatchBaseKHR(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
}


// ----- VK_KHR_maintenance1
VKAPI_ATTR void VKAPI_CALL layer_TrimCommandPoolKHR(
    VkDevice device,
    VkCommandPool commandPool,
    VkCommandPoolTrimFlags flags) {
    log_layer_message("Generated Layer vkTrimCommandPoolKHR");
   layer.created_devices[0].dispatch_table.TrimCommandPoolKHR(device, commandPool, flags);
}


// ----- VK_KHR_device_group_creation
VKAPI_ATTR VkResult VKAPI_CALL layer_EnumeratePhysicalDeviceGroupsKHR(
    VkInstance instance,
    uint32_t* pPhysicalDeviceGroupCount,
    VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties) {
    log_layer_message("Generated Layer vkEnumeratePhysicalDeviceGroupsKHR");
    return layer.instance_dispatch_table.EnumeratePhysicalDeviceGroupsKHR(instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
}


// ----- VK_KHR_external_memory_capabilities
VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceExternalBufferPropertiesKHR(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo,
    VkExternalBufferProperties* pExternalBufferProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceExternalBufferPropertiesKHR");
   layer.instance_dispatch_table.GetPhysicalDeviceExternalBufferPropertiesKHR(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
}


// ----- VK_KHR_external_memory_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
VKAPI_ATTR VkResult VKAPI_CALL layer_GetMemoryWin32HandleKHR(
    VkDevice device,
    const VkMemoryGetWin32HandleInfoKHR* pGetWin32HandleInfo,
    HANDLE* pHandle) {
    log_layer_message("Generated Layer vkGetMemoryWin32HandleKHR");
    return layer.created_devices[0].dispatch_table.GetMemoryWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetMemoryWin32HandlePropertiesKHR(
    VkDevice device,
    VkExternalMemoryHandleTypeFlagBits handleType,
    HANDLE handle,
    VkMemoryWin32HandlePropertiesKHR* pMemoryWin32HandleProperties) {
    log_layer_message("Generated Layer vkGetMemoryWin32HandlePropertiesKHR");
    return layer.created_devices[0].dispatch_table.GetMemoryWin32HandlePropertiesKHR(device, handleType, handle, pMemoryWin32HandleProperties);
}

#endif // VK_USE_PLATFORM_WIN32_KHR

// ----- VK_KHR_external_memory_fd
VKAPI_ATTR VkResult VKAPI_CALL layer_GetMemoryFdKHR(
    VkDevice device,
    const VkMemoryGetFdInfoKHR* pGetFdInfo,
    int* pFd) {
    log_layer_message("Generated Layer vkGetMemoryFdKHR");
    return layer.created_devices[0].dispatch_table.GetMemoryFdKHR(device, pGetFdInfo, pFd);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetMemoryFdPropertiesKHR(
    VkDevice device,
    VkExternalMemoryHandleTypeFlagBits handleType,
    int fd,
    VkMemoryFdPropertiesKHR* pMemoryFdProperties) {
    log_layer_message("Generated Layer vkGetMemoryFdPropertiesKHR");
    return layer.created_devices[0].dispatch_table.GetMemoryFdPropertiesKHR(device, handleType, fd, pMemoryFdProperties);
}


// ----- VK_KHR_external_semaphore_capabilities
VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceExternalSemaphorePropertiesKHR(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo,
    VkExternalSemaphoreProperties* pExternalSemaphoreProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceExternalSemaphorePropertiesKHR");
   layer.instance_dispatch_table.GetPhysicalDeviceExternalSemaphorePropertiesKHR(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
}


// ----- VK_KHR_external_semaphore_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
VKAPI_ATTR VkResult VKAPI_CALL layer_ImportSemaphoreWin32HandleKHR(
    VkDevice device,
    const VkImportSemaphoreWin32HandleInfoKHR* pImportSemaphoreWin32HandleInfo) {
    log_layer_message("Generated Layer vkImportSemaphoreWin32HandleKHR");
    return layer.created_devices[0].dispatch_table.ImportSemaphoreWin32HandleKHR(device, pImportSemaphoreWin32HandleInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetSemaphoreWin32HandleKHR(
    VkDevice device,
    const VkSemaphoreGetWin32HandleInfoKHR* pGetWin32HandleInfo,
    HANDLE* pHandle) {
    log_layer_message("Generated Layer vkGetSemaphoreWin32HandleKHR");
    return layer.created_devices[0].dispatch_table.GetSemaphoreWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
}

#endif // VK_USE_PLATFORM_WIN32_KHR

// ----- VK_KHR_external_semaphore_fd
VKAPI_ATTR VkResult VKAPI_CALL layer_ImportSemaphoreFdKHR(
    VkDevice device,
    const VkImportSemaphoreFdInfoKHR* pImportSemaphoreFdInfo) {
    log_layer_message("Generated Layer vkImportSemaphoreFdKHR");
    return layer.created_devices[0].dispatch_table.ImportSemaphoreFdKHR(device, pImportSemaphoreFdInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetSemaphoreFdKHR(
    VkDevice device,
    const VkSemaphoreGetFdInfoKHR* pGetFdInfo,
    int* pFd) {
    log_layer_message("Generated Layer vkGetSemaphoreFdKHR");
    return layer.created_devices[0].dispatch_table.GetSemaphoreFdKHR(device, pGetFdInfo, pFd);
}


// ----- VK_KHR_push_descriptor
VKAPI_ATTR void VKAPI_CALL layer_CmdPushDescriptorSetKHR(
    VkCommandBuffer commandBuffer,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipelineLayout layout,
    uint32_t set,
    uint32_t descriptorWriteCount,
    const VkWriteDescriptorSet* pDescriptorWrites) {
    log_layer_message("Generated Layer vkCmdPushDescriptorSetKHR");
   layer.created_devices[0].dispatch_table.CmdPushDescriptorSetKHR(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdPushDescriptorSetWithTemplateKHR(
    VkCommandBuffer commandBuffer,
    VkDescriptorUpdateTemplate descriptorUpdateTemplate,
    VkPipelineLayout layout,
    uint32_t set,
    const void* pData) {
    log_layer_message("Generated Layer vkCmdPushDescriptorSetWithTemplateKHR");
   layer.created_devices[0].dispatch_table.CmdPushDescriptorSetWithTemplateKHR(commandBuffer, descriptorUpdateTemplate, layout, set, pData);
}


// ----- VK_KHR_descriptor_update_template
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateDescriptorUpdateTemplateKHR(
    VkDevice device,
    const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDescriptorUpdateTemplate* pDescriptorUpdateTemplate) {
    log_layer_message("Generated Layer vkCreateDescriptorUpdateTemplateKHR");
    return layer.created_devices[0].dispatch_table.CreateDescriptorUpdateTemplateKHR(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyDescriptorUpdateTemplateKHR(
    VkDevice device,
    VkDescriptorUpdateTemplate descriptorUpdateTemplate,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyDescriptorUpdateTemplateKHR");
   layer.created_devices[0].dispatch_table.DestroyDescriptorUpdateTemplateKHR(device, descriptorUpdateTemplate, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL layer_UpdateDescriptorSetWithTemplateKHR(
    VkDevice device,
    VkDescriptorSet descriptorSet,
    VkDescriptorUpdateTemplate descriptorUpdateTemplate,
    const void* pData) {
    log_layer_message("Generated Layer vkUpdateDescriptorSetWithTemplateKHR");
   layer.created_devices[0].dispatch_table.UpdateDescriptorSetWithTemplateKHR(device, descriptorSet, descriptorUpdateTemplate, pData);
}


// ----- VK_KHR_create_renderpass2
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateRenderPass2KHR(
    VkDevice device,
    const VkRenderPassCreateInfo2* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkRenderPass* pRenderPass) {
    log_layer_message("Generated Layer vkCreateRenderPass2KHR");
    return layer.created_devices[0].dispatch_table.CreateRenderPass2KHR(device, pCreateInfo, pAllocator, pRenderPass);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBeginRenderPass2KHR(
    VkCommandBuffer commandBuffer,
    const VkRenderPassBeginInfo* pRenderPassBegin,
    const VkSubpassBeginInfo* pSubpassBeginInfo) {
    log_layer_message("Generated Layer vkCmdBeginRenderPass2KHR");
   layer.created_devices[0].dispatch_table.CmdBeginRenderPass2KHR(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdNextSubpass2KHR(
    VkCommandBuffer commandBuffer,
    const VkSubpassBeginInfo* pSubpassBeginInfo,
    const VkSubpassEndInfo* pSubpassEndInfo) {
    log_layer_message("Generated Layer vkCmdNextSubpass2KHR");
   layer.created_devices[0].dispatch_table.CmdNextSubpass2KHR(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdEndRenderPass2KHR(
    VkCommandBuffer commandBuffer,
    const VkSubpassEndInfo* pSubpassEndInfo) {
    log_layer_message("Generated Layer vkCmdEndRenderPass2KHR");
   layer.created_devices[0].dispatch_table.CmdEndRenderPass2KHR(commandBuffer, pSubpassEndInfo);
}


// ----- VK_KHR_shared_presentable_image
VKAPI_ATTR VkResult VKAPI_CALL layer_GetSwapchainStatusKHR(
    VkDevice device,
    VkSwapchainKHR swapchain) {
    log_layer_message("Generated Layer vkGetSwapchainStatusKHR");
    return layer.created_devices[0].dispatch_table.GetSwapchainStatusKHR(device, swapchain);
}


// ----- VK_KHR_external_fence_capabilities
VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceExternalFencePropertiesKHR(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo,
    VkExternalFenceProperties* pExternalFenceProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceExternalFencePropertiesKHR");
   layer.instance_dispatch_table.GetPhysicalDeviceExternalFencePropertiesKHR(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
}


// ----- VK_KHR_external_fence_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
VKAPI_ATTR VkResult VKAPI_CALL layer_ImportFenceWin32HandleKHR(
    VkDevice device,
    const VkImportFenceWin32HandleInfoKHR* pImportFenceWin32HandleInfo) {
    log_layer_message("Generated Layer vkImportFenceWin32HandleKHR");
    return layer.created_devices[0].dispatch_table.ImportFenceWin32HandleKHR(device, pImportFenceWin32HandleInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetFenceWin32HandleKHR(
    VkDevice device,
    const VkFenceGetWin32HandleInfoKHR* pGetWin32HandleInfo,
    HANDLE* pHandle) {
    log_layer_message("Generated Layer vkGetFenceWin32HandleKHR");
    return layer.created_devices[0].dispatch_table.GetFenceWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
}

#endif // VK_USE_PLATFORM_WIN32_KHR

// ----- VK_KHR_external_fence_fd
VKAPI_ATTR VkResult VKAPI_CALL layer_ImportFenceFdKHR(
    VkDevice device,
    const VkImportFenceFdInfoKHR* pImportFenceFdInfo) {
    log_layer_message("Generated Layer vkImportFenceFdKHR");
    return layer.created_devices[0].dispatch_table.ImportFenceFdKHR(device, pImportFenceFdInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetFenceFdKHR(
    VkDevice device,
    const VkFenceGetFdInfoKHR* pGetFdInfo,
    int* pFd) {
    log_layer_message("Generated Layer vkGetFenceFdKHR");
    return layer.created_devices[0].dispatch_table.GetFenceFdKHR(device, pGetFdInfo, pFd);
}


// ----- VK_KHR_performance_query
VKAPI_ATTR VkResult VKAPI_CALL layer_EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    uint32_t* pCounterCount,
    VkPerformanceCounterKHR* pCounters,
    VkPerformanceCounterDescriptionKHR* pCounterDescriptions) {
    log_layer_message("Generated Layer vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR");
    return layer.instance_dispatch_table.EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR(physicalDevice, queueFamilyIndex, pCounterCount, pCounters, pCounterDescriptions);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR(
    VkPhysicalDevice physicalDevice,
    const VkQueryPoolPerformanceCreateInfoKHR* pPerformanceQueryCreateInfo,
    uint32_t* pNumPasses) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR");
   layer.instance_dispatch_table.GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR(physicalDevice, pPerformanceQueryCreateInfo, pNumPasses);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_AcquireProfilingLockKHR(
    VkDevice device,
    const VkAcquireProfilingLockInfoKHR* pInfo) {
    log_layer_message("Generated Layer vkAcquireProfilingLockKHR");
    return layer.created_devices[0].dispatch_table.AcquireProfilingLockKHR(device, pInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_ReleaseProfilingLockKHR(
    VkDevice device) {
    log_layer_message("Generated Layer vkReleaseProfilingLockKHR");
   layer.created_devices[0].dispatch_table.ReleaseProfilingLockKHR(device);
}


// ----- VK_KHR_get_surface_capabilities2
VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceSurfaceCapabilities2KHR(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
    VkSurfaceCapabilities2KHR* pSurfaceCapabilities) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceSurfaceCapabilities2KHR");
    return layer.instance_dispatch_table.GetPhysicalDeviceSurfaceCapabilities2KHR(physicalDevice, pSurfaceInfo, pSurfaceCapabilities);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceSurfaceFormats2KHR(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
    uint32_t* pSurfaceFormatCount,
    VkSurfaceFormat2KHR* pSurfaceFormats) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceSurfaceFormats2KHR");
    return layer.instance_dispatch_table.GetPhysicalDeviceSurfaceFormats2KHR(physicalDevice, pSurfaceInfo, pSurfaceFormatCount, pSurfaceFormats);
}


// ----- VK_KHR_get_display_properties2
VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceDisplayProperties2KHR(
    VkPhysicalDevice physicalDevice,
    uint32_t* pPropertyCount,
    VkDisplayProperties2KHR* pProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceDisplayProperties2KHR");
    return layer.instance_dispatch_table.GetPhysicalDeviceDisplayProperties2KHR(physicalDevice, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceDisplayPlaneProperties2KHR(
    VkPhysicalDevice physicalDevice,
    uint32_t* pPropertyCount,
    VkDisplayPlaneProperties2KHR* pProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceDisplayPlaneProperties2KHR");
    return layer.instance_dispatch_table.GetPhysicalDeviceDisplayPlaneProperties2KHR(physicalDevice, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetDisplayModeProperties2KHR(
    VkPhysicalDevice physicalDevice,
    VkDisplayKHR display,
    uint32_t* pPropertyCount,
    VkDisplayModeProperties2KHR* pProperties) {
    log_layer_message("Generated Layer vkGetDisplayModeProperties2KHR");
    return layer.instance_dispatch_table.GetDisplayModeProperties2KHR(physicalDevice, display, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetDisplayPlaneCapabilities2KHR(
    VkPhysicalDevice physicalDevice,
    const VkDisplayPlaneInfo2KHR* pDisplayPlaneInfo,
    VkDisplayPlaneCapabilities2KHR* pCapabilities) {
    log_layer_message("Generated Layer vkGetDisplayPlaneCapabilities2KHR");
    return layer.instance_dispatch_table.GetDisplayPlaneCapabilities2KHR(physicalDevice, pDisplayPlaneInfo, pCapabilities);
}


// ----- VK_KHR_get_memory_requirements2
VKAPI_ATTR void VKAPI_CALL layer_GetImageMemoryRequirements2KHR(
    VkDevice device,
    const VkImageMemoryRequirementsInfo2* pInfo,
    VkMemoryRequirements2* pMemoryRequirements) {
    log_layer_message("Generated Layer vkGetImageMemoryRequirements2KHR");
   layer.created_devices[0].dispatch_table.GetImageMemoryRequirements2KHR(device, pInfo, pMemoryRequirements);
}

VKAPI_ATTR void VKAPI_CALL layer_GetBufferMemoryRequirements2KHR(
    VkDevice device,
    const VkBufferMemoryRequirementsInfo2* pInfo,
    VkMemoryRequirements2* pMemoryRequirements) {
    log_layer_message("Generated Layer vkGetBufferMemoryRequirements2KHR");
   layer.created_devices[0].dispatch_table.GetBufferMemoryRequirements2KHR(device, pInfo, pMemoryRequirements);
}

VKAPI_ATTR void VKAPI_CALL layer_GetImageSparseMemoryRequirements2KHR(
    VkDevice device,
    const VkImageSparseMemoryRequirementsInfo2* pInfo,
    uint32_t* pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2* pSparseMemoryRequirements) {
    log_layer_message("Generated Layer vkGetImageSparseMemoryRequirements2KHR");
   layer.created_devices[0].dispatch_table.GetImageSparseMemoryRequirements2KHR(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}


// ----- VK_KHR_sampler_ycbcr_conversion
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateSamplerYcbcrConversionKHR(
    VkDevice device,
    const VkSamplerYcbcrConversionCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSamplerYcbcrConversion* pYcbcrConversion) {
    log_layer_message("Generated Layer vkCreateSamplerYcbcrConversionKHR");
    return layer.created_devices[0].dispatch_table.CreateSamplerYcbcrConversionKHR(device, pCreateInfo, pAllocator, pYcbcrConversion);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroySamplerYcbcrConversionKHR(
    VkDevice device,
    VkSamplerYcbcrConversion ycbcrConversion,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroySamplerYcbcrConversionKHR");
   layer.created_devices[0].dispatch_table.DestroySamplerYcbcrConversionKHR(device, ycbcrConversion, pAllocator);
}


// ----- VK_KHR_bind_memory2
VKAPI_ATTR VkResult VKAPI_CALL layer_BindBufferMemory2KHR(
    VkDevice device,
    uint32_t bindInfoCount,
    const VkBindBufferMemoryInfo* pBindInfos) {
    log_layer_message("Generated Layer vkBindBufferMemory2KHR");
    return layer.created_devices[0].dispatch_table.BindBufferMemory2KHR(device, bindInfoCount, pBindInfos);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_BindImageMemory2KHR(
    VkDevice device,
    uint32_t bindInfoCount,
    const VkBindImageMemoryInfo* pBindInfos) {
    log_layer_message("Generated Layer vkBindImageMemory2KHR");
    return layer.created_devices[0].dispatch_table.BindImageMemory2KHR(device, bindInfoCount, pBindInfos);
}


// ----- VK_KHR_maintenance3
VKAPI_ATTR void VKAPI_CALL layer_GetDescriptorSetLayoutSupportKHR(
    VkDevice device,
    const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
    VkDescriptorSetLayoutSupport* pSupport) {
    log_layer_message("Generated Layer vkGetDescriptorSetLayoutSupportKHR");
   layer.created_devices[0].dispatch_table.GetDescriptorSetLayoutSupportKHR(device, pCreateInfo, pSupport);
}


// ----- VK_KHR_draw_indirect_count
VKAPI_ATTR void VKAPI_CALL layer_CmdDrawIndirectCountKHR(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    VkBuffer countBuffer,
    VkDeviceSize countBufferOffset,
    uint32_t maxDrawCount,
    uint32_t stride) {
    log_layer_message("Generated Layer vkCmdDrawIndirectCountKHR");
   layer.created_devices[0].dispatch_table.CmdDrawIndirectCountKHR(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdDrawIndexedIndirectCountKHR(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    VkBuffer countBuffer,
    VkDeviceSize countBufferOffset,
    uint32_t maxDrawCount,
    uint32_t stride) {
    log_layer_message("Generated Layer vkCmdDrawIndexedIndirectCountKHR");
   layer.created_devices[0].dispatch_table.CmdDrawIndexedIndirectCountKHR(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}


// ----- VK_KHR_timeline_semaphore
VKAPI_ATTR VkResult VKAPI_CALL layer_GetSemaphoreCounterValueKHR(
    VkDevice device,
    VkSemaphore semaphore,
    uint64_t* pValue) {
    log_layer_message("Generated Layer vkGetSemaphoreCounterValueKHR");
    return layer.created_devices[0].dispatch_table.GetSemaphoreCounterValueKHR(device, semaphore, pValue);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_WaitSemaphoresKHR(
    VkDevice device,
    const VkSemaphoreWaitInfo* pWaitInfo,
    uint64_t timeout) {
    log_layer_message("Generated Layer vkWaitSemaphoresKHR");
    return layer.created_devices[0].dispatch_table.WaitSemaphoresKHR(device, pWaitInfo, timeout);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_SignalSemaphoreKHR(
    VkDevice device,
    const VkSemaphoreSignalInfo* pSignalInfo) {
    log_layer_message("Generated Layer vkSignalSemaphoreKHR");
    return layer.created_devices[0].dispatch_table.SignalSemaphoreKHR(device, pSignalInfo);
}


// ----- VK_KHR_fragment_shading_rate
VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceFragmentShadingRatesKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t* pFragmentShadingRateCount,
    VkPhysicalDeviceFragmentShadingRateKHR* pFragmentShadingRates) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceFragmentShadingRatesKHR");
    return layer.instance_dispatch_table.GetPhysicalDeviceFragmentShadingRatesKHR(physicalDevice, pFragmentShadingRateCount, pFragmentShadingRates);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetFragmentShadingRateKHR(
    VkCommandBuffer commandBuffer,
    const VkExtent2D* pFragmentSize,
    const VkFragmentShadingRateCombinerOpKHR combinerOps[2]) {
    log_layer_message("Generated Layer vkCmdSetFragmentShadingRateKHR");
   layer.created_devices[0].dispatch_table.CmdSetFragmentShadingRateKHR(commandBuffer, pFragmentSize, combinerOps);
}


// ----- VK_KHR_present_wait
VKAPI_ATTR VkResult VKAPI_CALL layer_WaitForPresentKHR(
    VkDevice device,
    VkSwapchainKHR swapchain,
    uint64_t presentId,
    uint64_t timeout) {
    log_layer_message("Generated Layer vkWaitForPresentKHR");
    return layer.created_devices[0].dispatch_table.WaitForPresentKHR(device, swapchain, presentId, timeout);
}


// ----- VK_KHR_buffer_device_address
VKAPI_ATTR VkDeviceAddress VKAPI_CALL layer_GetBufferDeviceAddressKHR(
    VkDevice device,
    const VkBufferDeviceAddressInfo* pInfo) {
    log_layer_message("Generated Layer vkGetBufferDeviceAddressKHR");
    return layer.created_devices[0].dispatch_table.GetBufferDeviceAddressKHR(device, pInfo);
}

VKAPI_ATTR uint64_t VKAPI_CALL layer_GetBufferOpaqueCaptureAddressKHR(
    VkDevice device,
    const VkBufferDeviceAddressInfo* pInfo) {
    log_layer_message("Generated Layer vkGetBufferOpaqueCaptureAddressKHR");
    return layer.created_devices[0].dispatch_table.GetBufferOpaqueCaptureAddressKHR(device, pInfo);
}

VKAPI_ATTR uint64_t VKAPI_CALL layer_GetDeviceMemoryOpaqueCaptureAddressKHR(
    VkDevice device,
    const VkDeviceMemoryOpaqueCaptureAddressInfo* pInfo) {
    log_layer_message("Generated Layer vkGetDeviceMemoryOpaqueCaptureAddressKHR");
    return layer.created_devices[0].dispatch_table.GetDeviceMemoryOpaqueCaptureAddressKHR(device, pInfo);
}


// ----- VK_KHR_deferred_host_operations
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateDeferredOperationKHR(
    VkDevice device,
    const VkAllocationCallbacks* pAllocator,
    VkDeferredOperationKHR* pDeferredOperation) {
    log_layer_message("Generated Layer vkCreateDeferredOperationKHR");
    return layer.created_devices[0].dispatch_table.CreateDeferredOperationKHR(device, pAllocator, pDeferredOperation);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyDeferredOperationKHR(
    VkDevice device,
    VkDeferredOperationKHR operation,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyDeferredOperationKHR");
   layer.created_devices[0].dispatch_table.DestroyDeferredOperationKHR(device, operation, pAllocator);
}

VKAPI_ATTR uint32_t VKAPI_CALL layer_GetDeferredOperationMaxConcurrencyKHR(
    VkDevice device,
    VkDeferredOperationKHR operation) {
    log_layer_message("Generated Layer vkGetDeferredOperationMaxConcurrencyKHR");
    return layer.created_devices[0].dispatch_table.GetDeferredOperationMaxConcurrencyKHR(device, operation);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetDeferredOperationResultKHR(
    VkDevice device,
    VkDeferredOperationKHR operation) {
    log_layer_message("Generated Layer vkGetDeferredOperationResultKHR");
    return layer.created_devices[0].dispatch_table.GetDeferredOperationResultKHR(device, operation);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_DeferredOperationJoinKHR(
    VkDevice device,
    VkDeferredOperationKHR operation) {
    log_layer_message("Generated Layer vkDeferredOperationJoinKHR");
    return layer.created_devices[0].dispatch_table.DeferredOperationJoinKHR(device, operation);
}


// ----- VK_KHR_pipeline_executable_properties
VKAPI_ATTR VkResult VKAPI_CALL layer_GetPipelineExecutablePropertiesKHR(
    VkDevice device,
    const VkPipelineInfoKHR* pPipelineInfo,
    uint32_t* pExecutableCount,
    VkPipelineExecutablePropertiesKHR* pProperties) {
    log_layer_message("Generated Layer vkGetPipelineExecutablePropertiesKHR");
    return layer.created_devices[0].dispatch_table.GetPipelineExecutablePropertiesKHR(device, pPipelineInfo, pExecutableCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetPipelineExecutableStatisticsKHR(
    VkDevice device,
    const VkPipelineExecutableInfoKHR* pExecutableInfo,
    uint32_t* pStatisticCount,
    VkPipelineExecutableStatisticKHR* pStatistics) {
    log_layer_message("Generated Layer vkGetPipelineExecutableStatisticsKHR");
    return layer.created_devices[0].dispatch_table.GetPipelineExecutableStatisticsKHR(device, pExecutableInfo, pStatisticCount, pStatistics);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetPipelineExecutableInternalRepresentationsKHR(
    VkDevice device,
    const VkPipelineExecutableInfoKHR* pExecutableInfo,
    uint32_t* pInternalRepresentationCount,
    VkPipelineExecutableInternalRepresentationKHR* pInternalRepresentations) {
    log_layer_message("Generated Layer vkGetPipelineExecutableInternalRepresentationsKHR");
    return layer.created_devices[0].dispatch_table.GetPipelineExecutableInternalRepresentationsKHR(device, pExecutableInfo, pInternalRepresentationCount, pInternalRepresentations);
}


// ----- VK_KHR_video_encode_queue
#ifdef VK_ENABLE_BETA_EXTENSIONS
VKAPI_ATTR void VKAPI_CALL layer_CmdEncodeVideoKHR(
    VkCommandBuffer commandBuffer,
    const VkVideoEncodeInfoKHR* pEncodeInfo) {
    log_layer_message("Generated Layer vkCmdEncodeVideoKHR");
   layer.created_devices[0].dispatch_table.CmdEncodeVideoKHR(commandBuffer, pEncodeInfo);
}

#endif // VK_ENABLE_BETA_EXTENSIONS

// ----- VK_KHR_synchronization2
VKAPI_ATTR void VKAPI_CALL layer_CmdSetEvent2KHR(
    VkCommandBuffer commandBuffer,
    VkEvent event,
    const VkDependencyInfo* pDependencyInfo) {
    log_layer_message("Generated Layer vkCmdSetEvent2KHR");
   layer.created_devices[0].dispatch_table.CmdSetEvent2KHR(commandBuffer, event, pDependencyInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdResetEvent2KHR(
    VkCommandBuffer commandBuffer,
    VkEvent event,
    VkPipelineStageFlags2 stageMask) {
    log_layer_message("Generated Layer vkCmdResetEvent2KHR");
   layer.created_devices[0].dispatch_table.CmdResetEvent2KHR(commandBuffer, event, stageMask);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdWaitEvents2KHR(
    VkCommandBuffer commandBuffer,
    uint32_t eventCount,
    const VkEvent* pEvents,
    const VkDependencyInfo* pDependencyInfos) {
    log_layer_message("Generated Layer vkCmdWaitEvents2KHR");
   layer.created_devices[0].dispatch_table.CmdWaitEvents2KHR(commandBuffer, eventCount, pEvents, pDependencyInfos);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdPipelineBarrier2KHR(
    VkCommandBuffer commandBuffer,
    const VkDependencyInfo* pDependencyInfo) {
    log_layer_message("Generated Layer vkCmdPipelineBarrier2KHR");
   layer.created_devices[0].dispatch_table.CmdPipelineBarrier2KHR(commandBuffer, pDependencyInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdWriteTimestamp2KHR(
    VkCommandBuffer commandBuffer,
    VkPipelineStageFlags2 stage,
    VkQueryPool queryPool,
    uint32_t query) {
    log_layer_message("Generated Layer vkCmdWriteTimestamp2KHR");
   layer.created_devices[0].dispatch_table.CmdWriteTimestamp2KHR(commandBuffer, stage, queryPool, query);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_QueueSubmit2KHR(
    VkQueue queue,
    uint32_t submitCount,
    const VkSubmitInfo2* pSubmits,
    VkFence fence) {
    log_layer_message("Generated Layer vkQueueSubmit2KHR");
    return layer.created_devices[0].dispatch_table.QueueSubmit2KHR(queue, submitCount, pSubmits, fence);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdWriteBufferMarker2AMD(
    VkCommandBuffer commandBuffer,
    VkPipelineStageFlags2 stage,
    VkBuffer dstBuffer,
    VkDeviceSize dstOffset,
    uint32_t marker) {
    log_layer_message("Generated Layer vkCmdWriteBufferMarker2AMD");
   layer.created_devices[0].dispatch_table.CmdWriteBufferMarker2AMD(commandBuffer, stage, dstBuffer, dstOffset, marker);
}

VKAPI_ATTR void VKAPI_CALL layer_GetQueueCheckpointData2NV(
    VkQueue queue,
    uint32_t* pCheckpointDataCount,
    VkCheckpointData2NV* pCheckpointData) {
    log_layer_message("Generated Layer vkGetQueueCheckpointData2NV");
   layer.created_devices[0].dispatch_table.GetQueueCheckpointData2NV(queue, pCheckpointDataCount, pCheckpointData);
}


// ----- VK_KHR_copy_commands2
VKAPI_ATTR void VKAPI_CALL layer_CmdCopyBuffer2KHR(
    VkCommandBuffer commandBuffer,
    const VkCopyBufferInfo2* pCopyBufferInfo) {
    log_layer_message("Generated Layer vkCmdCopyBuffer2KHR");
   layer.created_devices[0].dispatch_table.CmdCopyBuffer2KHR(commandBuffer, pCopyBufferInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdCopyImage2KHR(
    VkCommandBuffer commandBuffer,
    const VkCopyImageInfo2* pCopyImageInfo) {
    log_layer_message("Generated Layer vkCmdCopyImage2KHR");
   layer.created_devices[0].dispatch_table.CmdCopyImage2KHR(commandBuffer, pCopyImageInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdCopyBufferToImage2KHR(
    VkCommandBuffer commandBuffer,
    const VkCopyBufferToImageInfo2* pCopyBufferToImageInfo) {
    log_layer_message("Generated Layer vkCmdCopyBufferToImage2KHR");
   layer.created_devices[0].dispatch_table.CmdCopyBufferToImage2KHR(commandBuffer, pCopyBufferToImageInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdCopyImageToBuffer2KHR(
    VkCommandBuffer commandBuffer,
    const VkCopyImageToBufferInfo2* pCopyImageToBufferInfo) {
    log_layer_message("Generated Layer vkCmdCopyImageToBuffer2KHR");
   layer.created_devices[0].dispatch_table.CmdCopyImageToBuffer2KHR(commandBuffer, pCopyImageToBufferInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBlitImage2KHR(
    VkCommandBuffer commandBuffer,
    const VkBlitImageInfo2* pBlitImageInfo) {
    log_layer_message("Generated Layer vkCmdBlitImage2KHR");
   layer.created_devices[0].dispatch_table.CmdBlitImage2KHR(commandBuffer, pBlitImageInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdResolveImage2KHR(
    VkCommandBuffer commandBuffer,
    const VkResolveImageInfo2* pResolveImageInfo) {
    log_layer_message("Generated Layer vkCmdResolveImage2KHR");
   layer.created_devices[0].dispatch_table.CmdResolveImage2KHR(commandBuffer, pResolveImageInfo);
}


// ----- VK_KHR_maintenance4
VKAPI_ATTR void VKAPI_CALL layer_GetDeviceBufferMemoryRequirementsKHR(
    VkDevice device,
    const VkDeviceBufferMemoryRequirements* pInfo,
    VkMemoryRequirements2* pMemoryRequirements) {
    log_layer_message("Generated Layer vkGetDeviceBufferMemoryRequirementsKHR");
   layer.created_devices[0].dispatch_table.GetDeviceBufferMemoryRequirementsKHR(device, pInfo, pMemoryRequirements);
}

VKAPI_ATTR void VKAPI_CALL layer_GetDeviceImageMemoryRequirementsKHR(
    VkDevice device,
    const VkDeviceImageMemoryRequirements* pInfo,
    VkMemoryRequirements2* pMemoryRequirements) {
    log_layer_message("Generated Layer vkGetDeviceImageMemoryRequirementsKHR");
   layer.created_devices[0].dispatch_table.GetDeviceImageMemoryRequirementsKHR(device, pInfo, pMemoryRequirements);
}

VKAPI_ATTR void VKAPI_CALL layer_GetDeviceImageSparseMemoryRequirementsKHR(
    VkDevice device,
    const VkDeviceImageMemoryRequirements* pInfo,
    uint32_t* pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2* pSparseMemoryRequirements) {
    log_layer_message("Generated Layer vkGetDeviceImageSparseMemoryRequirementsKHR");
   layer.created_devices[0].dispatch_table.GetDeviceImageSparseMemoryRequirementsKHR(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}


// ----- VK_EXT_debug_report
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateDebugReportCallbackEXT(
    VkInstance instance,
    const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugReportCallbackEXT* pCallback) {
    log_layer_message("Generated Layer vkCreateDebugReportCallbackEXT");
    return layer.instance_dispatch_table.CreateDebugReportCallbackEXT(instance, pCreateInfo, pAllocator, pCallback);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyDebugReportCallbackEXT(
    VkInstance instance,
    VkDebugReportCallbackEXT callback,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyDebugReportCallbackEXT");
   layer.instance_dispatch_table.DestroyDebugReportCallbackEXT(instance, callback, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL layer_DebugReportMessageEXT(
    VkInstance instance,
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objectType,
    uint64_t object,
    size_t location,
    int32_t messageCode,
    const char* pLayerPrefix,
    const char* pMessage) {
    log_layer_message("Generated Layer vkDebugReportMessageEXT");
   layer.instance_dispatch_table.DebugReportMessageEXT(instance, flags, objectType, object, location, messageCode, pLayerPrefix, pMessage);
}


// ----- VK_EXT_debug_marker
VKAPI_ATTR VkResult VKAPI_CALL layer_DebugMarkerSetObjectTagEXT(
    VkDevice device,
    const VkDebugMarkerObjectTagInfoEXT* pTagInfo) {
    log_layer_message("Generated Layer vkDebugMarkerSetObjectTagEXT");
    return layer.created_devices[0].dispatch_table.DebugMarkerSetObjectTagEXT(device, pTagInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_DebugMarkerSetObjectNameEXT(
    VkDevice device,
    const VkDebugMarkerObjectNameInfoEXT* pNameInfo) {
    log_layer_message("Generated Layer vkDebugMarkerSetObjectNameEXT");
    return layer.created_devices[0].dispatch_table.DebugMarkerSetObjectNameEXT(device, pNameInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdDebugMarkerBeginEXT(
    VkCommandBuffer commandBuffer,
    const VkDebugMarkerMarkerInfoEXT* pMarkerInfo) {
    log_layer_message("Generated Layer vkCmdDebugMarkerBeginEXT");
   layer.created_devices[0].dispatch_table.CmdDebugMarkerBeginEXT(commandBuffer, pMarkerInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdDebugMarkerEndEXT(
    VkCommandBuffer commandBuffer) {
    log_layer_message("Generated Layer vkCmdDebugMarkerEndEXT");
   layer.created_devices[0].dispatch_table.CmdDebugMarkerEndEXT(commandBuffer);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdDebugMarkerInsertEXT(
    VkCommandBuffer commandBuffer,
    const VkDebugMarkerMarkerInfoEXT* pMarkerInfo) {
    log_layer_message("Generated Layer vkCmdDebugMarkerInsertEXT");
   layer.created_devices[0].dispatch_table.CmdDebugMarkerInsertEXT(commandBuffer, pMarkerInfo);
}


// ----- VK_EXT_transform_feedback
VKAPI_ATTR void VKAPI_CALL layer_CmdBindTransformFeedbackBuffersEXT(
    VkCommandBuffer commandBuffer,
    uint32_t firstBinding,
    uint32_t bindingCount,
    const VkBuffer* pBuffers,
    const VkDeviceSize* pOffsets,
    const VkDeviceSize* pSizes) {
    log_layer_message("Generated Layer vkCmdBindTransformFeedbackBuffersEXT");
   layer.created_devices[0].dispatch_table.CmdBindTransformFeedbackBuffersEXT(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBeginTransformFeedbackEXT(
    VkCommandBuffer commandBuffer,
    uint32_t firstCounterBuffer,
    uint32_t counterBufferCount,
    const VkBuffer* pCounterBuffers,
    const VkDeviceSize* pCounterBufferOffsets) {
    log_layer_message("Generated Layer vkCmdBeginTransformFeedbackEXT");
   layer.created_devices[0].dispatch_table.CmdBeginTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdEndTransformFeedbackEXT(
    VkCommandBuffer commandBuffer,
    uint32_t firstCounterBuffer,
    uint32_t counterBufferCount,
    const VkBuffer* pCounterBuffers,
    const VkDeviceSize* pCounterBufferOffsets) {
    log_layer_message("Generated Layer vkCmdEndTransformFeedbackEXT");
   layer.created_devices[0].dispatch_table.CmdEndTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBeginQueryIndexedEXT(
    VkCommandBuffer commandBuffer,
    VkQueryPool queryPool,
    uint32_t query,
    VkQueryControlFlags flags,
    uint32_t index) {
    log_layer_message("Generated Layer vkCmdBeginQueryIndexedEXT");
   layer.created_devices[0].dispatch_table.CmdBeginQueryIndexedEXT(commandBuffer, queryPool, query, flags, index);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdEndQueryIndexedEXT(
    VkCommandBuffer commandBuffer,
    VkQueryPool queryPool,
    uint32_t query,
    uint32_t index) {
    log_layer_message("Generated Layer vkCmdEndQueryIndexedEXT");
   layer.created_devices[0].dispatch_table.CmdEndQueryIndexedEXT(commandBuffer, queryPool, query, index);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdDrawIndirectByteCountEXT(
    VkCommandBuffer commandBuffer,
    uint32_t instanceCount,
    uint32_t firstInstance,
    VkBuffer counterBuffer,
    VkDeviceSize counterBufferOffset,
    uint32_t counterOffset,
    uint32_t vertexStride) {
    log_layer_message("Generated Layer vkCmdDrawIndirectByteCountEXT");
   layer.created_devices[0].dispatch_table.CmdDrawIndirectByteCountEXT(commandBuffer, instanceCount, firstInstance, counterBuffer, counterBufferOffset, counterOffset, vertexStride);
}


// ----- VK_NVX_binary_import
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateCuModuleNVX(
    VkDevice device,
    const VkCuModuleCreateInfoNVX* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkCuModuleNVX* pModule) {
    log_layer_message("Generated Layer vkCreateCuModuleNVX");
    return layer.created_devices[0].dispatch_table.CreateCuModuleNVX(device, pCreateInfo, pAllocator, pModule);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateCuFunctionNVX(
    VkDevice device,
    const VkCuFunctionCreateInfoNVX* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkCuFunctionNVX* pFunction) {
    log_layer_message("Generated Layer vkCreateCuFunctionNVX");
    return layer.created_devices[0].dispatch_table.CreateCuFunctionNVX(device, pCreateInfo, pAllocator, pFunction);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyCuModuleNVX(
    VkDevice device,
    VkCuModuleNVX module,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyCuModuleNVX");
   layer.created_devices[0].dispatch_table.DestroyCuModuleNVX(device, module, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyCuFunctionNVX(
    VkDevice device,
    VkCuFunctionNVX function,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyCuFunctionNVX");
   layer.created_devices[0].dispatch_table.DestroyCuFunctionNVX(device, function, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdCuLaunchKernelNVX(
    VkCommandBuffer commandBuffer,
    const VkCuLaunchInfoNVX* pLaunchInfo) {
    log_layer_message("Generated Layer vkCmdCuLaunchKernelNVX");
   layer.created_devices[0].dispatch_table.CmdCuLaunchKernelNVX(commandBuffer, pLaunchInfo);
}


// ----- VK_NVX_image_view_handle
VKAPI_ATTR uint32_t VKAPI_CALL layer_GetImageViewHandleNVX(
    VkDevice device,
    const VkImageViewHandleInfoNVX* pInfo) {
    log_layer_message("Generated Layer vkGetImageViewHandleNVX");
    return layer.created_devices[0].dispatch_table.GetImageViewHandleNVX(device, pInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetImageViewAddressNVX(
    VkDevice device,
    VkImageView imageView,
    VkImageViewAddressPropertiesNVX* pProperties) {
    log_layer_message("Generated Layer vkGetImageViewAddressNVX");
    return layer.created_devices[0].dispatch_table.GetImageViewAddressNVX(device, imageView, pProperties);
}


// ----- VK_AMD_draw_indirect_count
VKAPI_ATTR void VKAPI_CALL layer_CmdDrawIndirectCountAMD(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    VkBuffer countBuffer,
    VkDeviceSize countBufferOffset,
    uint32_t maxDrawCount,
    uint32_t stride) {
    log_layer_message("Generated Layer vkCmdDrawIndirectCountAMD");
   layer.created_devices[0].dispatch_table.CmdDrawIndirectCountAMD(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdDrawIndexedIndirectCountAMD(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    VkBuffer countBuffer,
    VkDeviceSize countBufferOffset,
    uint32_t maxDrawCount,
    uint32_t stride) {
    log_layer_message("Generated Layer vkCmdDrawIndexedIndirectCountAMD");
   layer.created_devices[0].dispatch_table.CmdDrawIndexedIndirectCountAMD(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}


// ----- VK_AMD_shader_info
VKAPI_ATTR VkResult VKAPI_CALL layer_GetShaderInfoAMD(
    VkDevice device,
    VkPipeline pipeline,
    VkShaderStageFlagBits shaderStage,
    VkShaderInfoTypeAMD infoType,
    size_t* pInfoSize,
    void* pInfo) {
    log_layer_message("Generated Layer vkGetShaderInfoAMD");
    return layer.created_devices[0].dispatch_table.GetShaderInfoAMD(device, pipeline, shaderStage, infoType, pInfoSize, pInfo);
}


// ----- VK_GGP_stream_descriptor_surface
#ifdef VK_USE_PLATFORM_GGP
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateStreamDescriptorSurfaceGGP(
    VkInstance instance,
    const VkStreamDescriptorSurfaceCreateInfoGGP* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_layer_message("Generated Layer vkCreateStreamDescriptorSurfaceGGP");
    return layer.instance_dispatch_table.CreateStreamDescriptorSurfaceGGP(instance, pCreateInfo, pAllocator, pSurface);
}

#endif // VK_USE_PLATFORM_GGP

// ----- VK_NV_external_memory_capabilities
VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceExternalImageFormatPropertiesNV(
    VkPhysicalDevice physicalDevice,
    VkFormat format,
    VkImageType type,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkImageCreateFlags flags,
    VkExternalMemoryHandleTypeFlagsNV externalHandleType,
    VkExternalImageFormatPropertiesNV* pExternalImageFormatProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceExternalImageFormatPropertiesNV");
    return layer.instance_dispatch_table.GetPhysicalDeviceExternalImageFormatPropertiesNV(physicalDevice, format, type, tiling, usage, flags, externalHandleType, pExternalImageFormatProperties);
}


// ----- VK_NV_external_memory_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
VKAPI_ATTR VkResult VKAPI_CALL layer_GetMemoryWin32HandleNV(
    VkDevice device,
    VkDeviceMemory memory,
    VkExternalMemoryHandleTypeFlagsNV handleType,
    HANDLE* pHandle) {
    log_layer_message("Generated Layer vkGetMemoryWin32HandleNV");
    return layer.created_devices[0].dispatch_table.GetMemoryWin32HandleNV(device, memory, handleType, pHandle);
}

#endif // VK_USE_PLATFORM_WIN32_KHR

// ----- VK_NN_vi_surface
#ifdef VK_USE_PLATFORM_VI_NN
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateViSurfaceNN(
    VkInstance instance,
    const VkViSurfaceCreateInfoNN* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_layer_message("Generated Layer vkCreateViSurfaceNN");
    return layer.instance_dispatch_table.CreateViSurfaceNN(instance, pCreateInfo, pAllocator, pSurface);
}

#endif // VK_USE_PLATFORM_VI_NN

// ----- VK_EXT_conditional_rendering
VKAPI_ATTR void VKAPI_CALL layer_CmdBeginConditionalRenderingEXT(
    VkCommandBuffer commandBuffer,
    const VkConditionalRenderingBeginInfoEXT* pConditionalRenderingBegin) {
    log_layer_message("Generated Layer vkCmdBeginConditionalRenderingEXT");
   layer.created_devices[0].dispatch_table.CmdBeginConditionalRenderingEXT(commandBuffer, pConditionalRenderingBegin);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdEndConditionalRenderingEXT(
    VkCommandBuffer commandBuffer) {
    log_layer_message("Generated Layer vkCmdEndConditionalRenderingEXT");
   layer.created_devices[0].dispatch_table.CmdEndConditionalRenderingEXT(commandBuffer);
}


// ----- VK_NV_clip_space_w_scaling
VKAPI_ATTR void VKAPI_CALL layer_CmdSetViewportWScalingNV(
    VkCommandBuffer commandBuffer,
    uint32_t firstViewport,
    uint32_t viewportCount,
    const VkViewportWScalingNV* pViewportWScalings) {
    log_layer_message("Generated Layer vkCmdSetViewportWScalingNV");
   layer.created_devices[0].dispatch_table.CmdSetViewportWScalingNV(commandBuffer, firstViewport, viewportCount, pViewportWScalings);
}


// ----- VK_EXT_direct_mode_display
VKAPI_ATTR VkResult VKAPI_CALL layer_ReleaseDisplayEXT(
    VkPhysicalDevice physicalDevice,
    VkDisplayKHR display) {
    log_layer_message("Generated Layer vkReleaseDisplayEXT");
    return layer.instance_dispatch_table.ReleaseDisplayEXT(physicalDevice, display);
}


// ----- VK_EXT_acquire_xlib_display
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
VKAPI_ATTR VkResult VKAPI_CALL layer_AcquireXlibDisplayEXT(
    VkPhysicalDevice physicalDevice,
    Display* dpy,
    VkDisplayKHR display) {
    log_layer_message("Generated Layer vkAcquireXlibDisplayEXT");
    return layer.instance_dispatch_table.AcquireXlibDisplayEXT(physicalDevice, dpy, display);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetRandROutputDisplayEXT(
    VkPhysicalDevice physicalDevice,
    Display* dpy,
    RROutput rrOutput,
    VkDisplayKHR* pDisplay) {
    log_layer_message("Generated Layer vkGetRandROutputDisplayEXT");
    return layer.instance_dispatch_table.GetRandROutputDisplayEXT(physicalDevice, dpy, rrOutput, pDisplay);
}

#endif // VK_USE_PLATFORM_XLIB_XRANDR_EXT

// ----- VK_EXT_display_surface_counter
VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceSurfaceCapabilities2EXT(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    VkSurfaceCapabilities2EXT* pSurfaceCapabilities) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceSurfaceCapabilities2EXT");
    return layer.instance_dispatch_table.GetPhysicalDeviceSurfaceCapabilities2EXT(physicalDevice, surface, pSurfaceCapabilities);
}


// ----- VK_EXT_display_control
VKAPI_ATTR VkResult VKAPI_CALL layer_DisplayPowerControlEXT(
    VkDevice device,
    VkDisplayKHR display,
    const VkDisplayPowerInfoEXT* pDisplayPowerInfo) {
    log_layer_message("Generated Layer vkDisplayPowerControlEXT");
    return layer.created_devices[0].dispatch_table.DisplayPowerControlEXT(device, display, pDisplayPowerInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_RegisterDeviceEventEXT(
    VkDevice device,
    const VkDeviceEventInfoEXT* pDeviceEventInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFence* pFence) {
    log_layer_message("Generated Layer vkRegisterDeviceEventEXT");
    return layer.created_devices[0].dispatch_table.RegisterDeviceEventEXT(device, pDeviceEventInfo, pAllocator, pFence);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_RegisterDisplayEventEXT(
    VkDevice device,
    VkDisplayKHR display,
    const VkDisplayEventInfoEXT* pDisplayEventInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFence* pFence) {
    log_layer_message("Generated Layer vkRegisterDisplayEventEXT");
    return layer.created_devices[0].dispatch_table.RegisterDisplayEventEXT(device, display, pDisplayEventInfo, pAllocator, pFence);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetSwapchainCounterEXT(
    VkDevice device,
    VkSwapchainKHR swapchain,
    VkSurfaceCounterFlagBitsEXT counter,
    uint64_t* pCounterValue) {
    log_layer_message("Generated Layer vkGetSwapchainCounterEXT");
    return layer.created_devices[0].dispatch_table.GetSwapchainCounterEXT(device, swapchain, counter, pCounterValue);
}


// ----- VK_GOOGLE_display_timing
VKAPI_ATTR VkResult VKAPI_CALL layer_GetRefreshCycleDurationGOOGLE(
    VkDevice device,
    VkSwapchainKHR swapchain,
    VkRefreshCycleDurationGOOGLE* pDisplayTimingProperties) {
    log_layer_message("Generated Layer vkGetRefreshCycleDurationGOOGLE");
    return layer.created_devices[0].dispatch_table.GetRefreshCycleDurationGOOGLE(device, swapchain, pDisplayTimingProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetPastPresentationTimingGOOGLE(
    VkDevice device,
    VkSwapchainKHR swapchain,
    uint32_t* pPresentationTimingCount,
    VkPastPresentationTimingGOOGLE* pPresentationTimings) {
    log_layer_message("Generated Layer vkGetPastPresentationTimingGOOGLE");
    return layer.created_devices[0].dispatch_table.GetPastPresentationTimingGOOGLE(device, swapchain, pPresentationTimingCount, pPresentationTimings);
}


// ----- VK_EXT_discard_rectangles
VKAPI_ATTR void VKAPI_CALL layer_CmdSetDiscardRectangleEXT(
    VkCommandBuffer commandBuffer,
    uint32_t firstDiscardRectangle,
    uint32_t discardRectangleCount,
    const VkRect2D* pDiscardRectangles) {
    log_layer_message("Generated Layer vkCmdSetDiscardRectangleEXT");
   layer.created_devices[0].dispatch_table.CmdSetDiscardRectangleEXT(commandBuffer, firstDiscardRectangle, discardRectangleCount, pDiscardRectangles);
}


// ----- VK_EXT_hdr_metadata
VKAPI_ATTR void VKAPI_CALL layer_SetHdrMetadataEXT(
    VkDevice device,
    uint32_t swapchainCount,
    const VkSwapchainKHR* pSwapchains,
    const VkHdrMetadataEXT* pMetadata) {
    log_layer_message("Generated Layer vkSetHdrMetadataEXT");
   layer.created_devices[0].dispatch_table.SetHdrMetadataEXT(device, swapchainCount, pSwapchains, pMetadata);
}


// ----- VK_MVK_ios_surface
#ifdef VK_USE_PLATFORM_IOS_MVK
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateIOSSurfaceMVK(
    VkInstance instance,
    const VkIOSSurfaceCreateInfoMVK* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_layer_message("Generated Layer vkCreateIOSSurfaceMVK");
    return layer.instance_dispatch_table.CreateIOSSurfaceMVK(instance, pCreateInfo, pAllocator, pSurface);
}

#endif // VK_USE_PLATFORM_IOS_MVK

// ----- VK_MVK_macos_surface
#ifdef VK_USE_PLATFORM_MACOS_MVK
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateMacOSSurfaceMVK(
    VkInstance instance,
    const VkMacOSSurfaceCreateInfoMVK* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_layer_message("Generated Layer vkCreateMacOSSurfaceMVK");
    return layer.instance_dispatch_table.CreateMacOSSurfaceMVK(instance, pCreateInfo, pAllocator, pSurface);
}

#endif // VK_USE_PLATFORM_MACOS_MVK

// ----- VK_EXT_debug_utils
VKAPI_ATTR VkResult VKAPI_CALL layer_SetDebugUtilsObjectNameEXT(
    VkDevice device,
    const VkDebugUtilsObjectNameInfoEXT* pNameInfo) {
    log_layer_message("Generated Layer vkSetDebugUtilsObjectNameEXT");
    return layer.created_devices[0].dispatch_table.SetDebugUtilsObjectNameEXT(device, pNameInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_SetDebugUtilsObjectTagEXT(
    VkDevice device,
    const VkDebugUtilsObjectTagInfoEXT* pTagInfo) {
    log_layer_message("Generated Layer vkSetDebugUtilsObjectTagEXT");
    return layer.created_devices[0].dispatch_table.SetDebugUtilsObjectTagEXT(device, pTagInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_QueueBeginDebugUtilsLabelEXT(
    VkQueue queue,
    const VkDebugUtilsLabelEXT* pLabelInfo) {
    log_layer_message("Generated Layer vkQueueBeginDebugUtilsLabelEXT");
   layer.created_devices[0].dispatch_table.QueueBeginDebugUtilsLabelEXT(queue, pLabelInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_QueueEndDebugUtilsLabelEXT(
    VkQueue queue) {
    log_layer_message("Generated Layer vkQueueEndDebugUtilsLabelEXT");
   layer.created_devices[0].dispatch_table.QueueEndDebugUtilsLabelEXT(queue);
}

VKAPI_ATTR void VKAPI_CALL layer_QueueInsertDebugUtilsLabelEXT(
    VkQueue queue,
    const VkDebugUtilsLabelEXT* pLabelInfo) {
    log_layer_message("Generated Layer vkQueueInsertDebugUtilsLabelEXT");
   layer.created_devices[0].dispatch_table.QueueInsertDebugUtilsLabelEXT(queue, pLabelInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBeginDebugUtilsLabelEXT(
    VkCommandBuffer commandBuffer,
    const VkDebugUtilsLabelEXT* pLabelInfo) {
    log_layer_message("Generated Layer vkCmdBeginDebugUtilsLabelEXT");
   layer.created_devices[0].dispatch_table.CmdBeginDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdEndDebugUtilsLabelEXT(
    VkCommandBuffer commandBuffer) {
    log_layer_message("Generated Layer vkCmdEndDebugUtilsLabelEXT");
   layer.created_devices[0].dispatch_table.CmdEndDebugUtilsLabelEXT(commandBuffer);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdInsertDebugUtilsLabelEXT(
    VkCommandBuffer commandBuffer,
    const VkDebugUtilsLabelEXT* pLabelInfo) {
    log_layer_message("Generated Layer vkCmdInsertDebugUtilsLabelEXT");
   layer.created_devices[0].dispatch_table.CmdInsertDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT messenger,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyDebugUtilsMessengerEXT");
   layer.instance_dispatch_table.DestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL layer_SubmitDebugUtilsMessageEXT(
    VkInstance instance,
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData) {
    log_layer_message("Generated Layer vkSubmitDebugUtilsMessageEXT");
   layer.instance_dispatch_table.SubmitDebugUtilsMessageEXT(instance, messageSeverity, messageTypes, pCallbackData);
}


// ----- VK_EXT_sample_locations
VKAPI_ATTR void VKAPI_CALL layer_CmdSetSampleLocationsEXT(
    VkCommandBuffer commandBuffer,
    const VkSampleLocationsInfoEXT* pSampleLocationsInfo) {
    log_layer_message("Generated Layer vkCmdSetSampleLocationsEXT");
   layer.created_devices[0].dispatch_table.CmdSetSampleLocationsEXT(commandBuffer, pSampleLocationsInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPhysicalDeviceMultisamplePropertiesEXT(
    VkPhysicalDevice physicalDevice,
    VkSampleCountFlagBits samples,
    VkMultisamplePropertiesEXT* pMultisampleProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceMultisamplePropertiesEXT");
   layer.instance_dispatch_table.GetPhysicalDeviceMultisamplePropertiesEXT(physicalDevice, samples, pMultisampleProperties);
}


// ----- VK_EXT_image_drm_format_modifier
VKAPI_ATTR VkResult VKAPI_CALL layer_GetImageDrmFormatModifierPropertiesEXT(
    VkDevice device,
    VkImage image,
    VkImageDrmFormatModifierPropertiesEXT* pProperties) {
    log_layer_message("Generated Layer vkGetImageDrmFormatModifierPropertiesEXT");
    return layer.created_devices[0].dispatch_table.GetImageDrmFormatModifierPropertiesEXT(device, image, pProperties);
}


// ----- VK_EXT_validation_cache
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateValidationCacheEXT(
    VkDevice device,
    const VkValidationCacheCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkValidationCacheEXT* pValidationCache) {
    log_layer_message("Generated Layer vkCreateValidationCacheEXT");
    return layer.created_devices[0].dispatch_table.CreateValidationCacheEXT(device, pCreateInfo, pAllocator, pValidationCache);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyValidationCacheEXT(
    VkDevice device,
    VkValidationCacheEXT validationCache,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyValidationCacheEXT");
   layer.created_devices[0].dispatch_table.DestroyValidationCacheEXT(device, validationCache, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_MergeValidationCachesEXT(
    VkDevice device,
    VkValidationCacheEXT dstCache,
    uint32_t srcCacheCount,
    const VkValidationCacheEXT* pSrcCaches) {
    log_layer_message("Generated Layer vkMergeValidationCachesEXT");
    return layer.created_devices[0].dispatch_table.MergeValidationCachesEXT(device, dstCache, srcCacheCount, pSrcCaches);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetValidationCacheDataEXT(
    VkDevice device,
    VkValidationCacheEXT validationCache,
    size_t* pDataSize,
    void* pData) {
    log_layer_message("Generated Layer vkGetValidationCacheDataEXT");
    return layer.created_devices[0].dispatch_table.GetValidationCacheDataEXT(device, validationCache, pDataSize, pData);
}


// ----- VK_NV_shading_rate_image
VKAPI_ATTR void VKAPI_CALL layer_CmdBindShadingRateImageNV(
    VkCommandBuffer commandBuffer,
    VkImageView imageView,
    VkImageLayout imageLayout) {
    log_layer_message("Generated Layer vkCmdBindShadingRateImageNV");
   layer.created_devices[0].dispatch_table.CmdBindShadingRateImageNV(commandBuffer, imageView, imageLayout);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetViewportShadingRatePaletteNV(
    VkCommandBuffer commandBuffer,
    uint32_t firstViewport,
    uint32_t viewportCount,
    const VkShadingRatePaletteNV* pShadingRatePalettes) {
    log_layer_message("Generated Layer vkCmdSetViewportShadingRatePaletteNV");
   layer.created_devices[0].dispatch_table.CmdSetViewportShadingRatePaletteNV(commandBuffer, firstViewport, viewportCount, pShadingRatePalettes);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetCoarseSampleOrderNV(
    VkCommandBuffer commandBuffer,
    VkCoarseSampleOrderTypeNV sampleOrderType,
    uint32_t customSampleOrderCount,
    const VkCoarseSampleOrderCustomNV* pCustomSampleOrders) {
    log_layer_message("Generated Layer vkCmdSetCoarseSampleOrderNV");
   layer.created_devices[0].dispatch_table.CmdSetCoarseSampleOrderNV(commandBuffer, sampleOrderType, customSampleOrderCount, pCustomSampleOrders);
}


// ----- VK_NV_ray_tracing
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateAccelerationStructureNV(
    VkDevice device,
    const VkAccelerationStructureCreateInfoNV* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkAccelerationStructureNV* pAccelerationStructure) {
    log_layer_message("Generated Layer vkCreateAccelerationStructureNV");
    return layer.created_devices[0].dispatch_table.CreateAccelerationStructureNV(device, pCreateInfo, pAllocator, pAccelerationStructure);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyAccelerationStructureNV(
    VkDevice device,
    VkAccelerationStructureNV accelerationStructure,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyAccelerationStructureNV");
   layer.created_devices[0].dispatch_table.DestroyAccelerationStructureNV(device, accelerationStructure, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL layer_GetAccelerationStructureMemoryRequirementsNV(
    VkDevice device,
    const VkAccelerationStructureMemoryRequirementsInfoNV* pInfo,
    VkMemoryRequirements2KHR* pMemoryRequirements) {
    log_layer_message("Generated Layer vkGetAccelerationStructureMemoryRequirementsNV");
   layer.created_devices[0].dispatch_table.GetAccelerationStructureMemoryRequirementsNV(device, pInfo, pMemoryRequirements);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_BindAccelerationStructureMemoryNV(
    VkDevice device,
    uint32_t bindInfoCount,
    const VkBindAccelerationStructureMemoryInfoNV* pBindInfos) {
    log_layer_message("Generated Layer vkBindAccelerationStructureMemoryNV");
    return layer.created_devices[0].dispatch_table.BindAccelerationStructureMemoryNV(device, bindInfoCount, pBindInfos);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBuildAccelerationStructureNV(
    VkCommandBuffer commandBuffer,
    const VkAccelerationStructureInfoNV* pInfo,
    VkBuffer instanceData,
    VkDeviceSize instanceOffset,
    VkBool32 update,
    VkAccelerationStructureNV dst,
    VkAccelerationStructureNV src,
    VkBuffer scratch,
    VkDeviceSize scratchOffset) {
    log_layer_message("Generated Layer vkCmdBuildAccelerationStructureNV");
   layer.created_devices[0].dispatch_table.CmdBuildAccelerationStructureNV(commandBuffer, pInfo, instanceData, instanceOffset, update, dst, src, scratch, scratchOffset);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdCopyAccelerationStructureNV(
    VkCommandBuffer commandBuffer,
    VkAccelerationStructureNV dst,
    VkAccelerationStructureNV src,
    VkCopyAccelerationStructureModeKHR mode) {
    log_layer_message("Generated Layer vkCmdCopyAccelerationStructureNV");
   layer.created_devices[0].dispatch_table.CmdCopyAccelerationStructureNV(commandBuffer, dst, src, mode);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdTraceRaysNV(
    VkCommandBuffer commandBuffer,
    VkBuffer raygenShaderBindingTableBuffer,
    VkDeviceSize raygenShaderBindingOffset,
    VkBuffer missShaderBindingTableBuffer,
    VkDeviceSize missShaderBindingOffset,
    VkDeviceSize missShaderBindingStride,
    VkBuffer hitShaderBindingTableBuffer,
    VkDeviceSize hitShaderBindingOffset,
    VkDeviceSize hitShaderBindingStride,
    VkBuffer callableShaderBindingTableBuffer,
    VkDeviceSize callableShaderBindingOffset,
    VkDeviceSize callableShaderBindingStride,
    uint32_t width,
    uint32_t height,
    uint32_t depth) {
    log_layer_message("Generated Layer vkCmdTraceRaysNV");
   layer.created_devices[0].dispatch_table.CmdTraceRaysNV(commandBuffer, raygenShaderBindingTableBuffer, raygenShaderBindingOffset, missShaderBindingTableBuffer, missShaderBindingOffset, missShaderBindingStride, hitShaderBindingTableBuffer, hitShaderBindingOffset, hitShaderBindingStride, callableShaderBindingTableBuffer, callableShaderBindingOffset, callableShaderBindingStride, width, height, depth);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateRayTracingPipelinesNV(
    VkDevice device,
    VkPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const VkRayTracingPipelineCreateInfoNV* pCreateInfos,
    const VkAllocationCallbacks* pAllocator,
    VkPipeline* pPipelines) {
    log_layer_message("Generated Layer vkCreateRayTracingPipelinesNV");
    return layer.created_devices[0].dispatch_table.CreateRayTracingPipelinesNV(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetRayTracingShaderGroupHandlesNV(
    VkDevice device,
    VkPipeline pipeline,
    uint32_t firstGroup,
    uint32_t groupCount,
    size_t dataSize,
    void* pData) {
    log_layer_message("Generated Layer vkGetRayTracingShaderGroupHandlesNV");
    return layer.created_devices[0].dispatch_table.GetRayTracingShaderGroupHandlesNV(device, pipeline, firstGroup, groupCount, dataSize, pData);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetAccelerationStructureHandleNV(
    VkDevice device,
    VkAccelerationStructureNV accelerationStructure,
    size_t dataSize,
    void* pData) {
    log_layer_message("Generated Layer vkGetAccelerationStructureHandleNV");
    return layer.created_devices[0].dispatch_table.GetAccelerationStructureHandleNV(device, accelerationStructure, dataSize, pData);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdWriteAccelerationStructuresPropertiesNV(
    VkCommandBuffer commandBuffer,
    uint32_t accelerationStructureCount,
    const VkAccelerationStructureNV* pAccelerationStructures,
    VkQueryType queryType,
    VkQueryPool queryPool,
    uint32_t firstQuery) {
    log_layer_message("Generated Layer vkCmdWriteAccelerationStructuresPropertiesNV");
   layer.created_devices[0].dispatch_table.CmdWriteAccelerationStructuresPropertiesNV(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CompileDeferredNV(
    VkDevice device,
    VkPipeline pipeline,
    uint32_t shader) {
    log_layer_message("Generated Layer vkCompileDeferredNV");
    return layer.created_devices[0].dispatch_table.CompileDeferredNV(device, pipeline, shader);
}


// ----- VK_EXT_external_memory_host
VKAPI_ATTR VkResult VKAPI_CALL layer_GetMemoryHostPointerPropertiesEXT(
    VkDevice device,
    VkExternalMemoryHandleTypeFlagBits handleType,
    const void* pHostPointer,
    VkMemoryHostPointerPropertiesEXT* pMemoryHostPointerProperties) {
    log_layer_message("Generated Layer vkGetMemoryHostPointerPropertiesEXT");
    return layer.created_devices[0].dispatch_table.GetMemoryHostPointerPropertiesEXT(device, handleType, pHostPointer, pMemoryHostPointerProperties);
}


// ----- VK_AMD_buffer_marker
VKAPI_ATTR void VKAPI_CALL layer_CmdWriteBufferMarkerAMD(
    VkCommandBuffer commandBuffer,
    VkPipelineStageFlagBits pipelineStage,
    VkBuffer dstBuffer,
    VkDeviceSize dstOffset,
    uint32_t marker) {
    log_layer_message("Generated Layer vkCmdWriteBufferMarkerAMD");
   layer.created_devices[0].dispatch_table.CmdWriteBufferMarkerAMD(commandBuffer, pipelineStage, dstBuffer, dstOffset, marker);
}


// ----- VK_EXT_calibrated_timestamps
VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceCalibrateableTimeDomainsEXT(
    VkPhysicalDevice physicalDevice,
    uint32_t* pTimeDomainCount,
    VkTimeDomainEXT* pTimeDomains) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceCalibrateableTimeDomainsEXT");
    return layer.instance_dispatch_table.GetPhysicalDeviceCalibrateableTimeDomainsEXT(physicalDevice, pTimeDomainCount, pTimeDomains);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetCalibratedTimestampsEXT(
    VkDevice device,
    uint32_t timestampCount,
    const VkCalibratedTimestampInfoEXT* pTimestampInfos,
    uint64_t* pTimestamps,
    uint64_t* pMaxDeviation) {
    log_layer_message("Generated Layer vkGetCalibratedTimestampsEXT");
    return layer.created_devices[0].dispatch_table.GetCalibratedTimestampsEXT(device, timestampCount, pTimestampInfos, pTimestamps, pMaxDeviation);
}


// ----- VK_NV_mesh_shader
VKAPI_ATTR void VKAPI_CALL layer_CmdDrawMeshTasksNV(
    VkCommandBuffer commandBuffer,
    uint32_t taskCount,
    uint32_t firstTask) {
    log_layer_message("Generated Layer vkCmdDrawMeshTasksNV");
   layer.created_devices[0].dispatch_table.CmdDrawMeshTasksNV(commandBuffer, taskCount, firstTask);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdDrawMeshTasksIndirectNV(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride) {
    log_layer_message("Generated Layer vkCmdDrawMeshTasksIndirectNV");
   layer.created_devices[0].dispatch_table.CmdDrawMeshTasksIndirectNV(commandBuffer, buffer, offset, drawCount, stride);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdDrawMeshTasksIndirectCountNV(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    VkBuffer countBuffer,
    VkDeviceSize countBufferOffset,
    uint32_t maxDrawCount,
    uint32_t stride) {
    log_layer_message("Generated Layer vkCmdDrawMeshTasksIndirectCountNV");
   layer.created_devices[0].dispatch_table.CmdDrawMeshTasksIndirectCountNV(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}


// ----- VK_NV_scissor_exclusive
VKAPI_ATTR void VKAPI_CALL layer_CmdSetExclusiveScissorNV(
    VkCommandBuffer commandBuffer,
    uint32_t firstExclusiveScissor,
    uint32_t exclusiveScissorCount,
    const VkRect2D* pExclusiveScissors) {
    log_layer_message("Generated Layer vkCmdSetExclusiveScissorNV");
   layer.created_devices[0].dispatch_table.CmdSetExclusiveScissorNV(commandBuffer, firstExclusiveScissor, exclusiveScissorCount, pExclusiveScissors);
}


// ----- VK_NV_device_diagnostic_checkpoints
VKAPI_ATTR void VKAPI_CALL layer_CmdSetCheckpointNV(
    VkCommandBuffer commandBuffer,
    const void* pCheckpointMarker) {
    log_layer_message("Generated Layer vkCmdSetCheckpointNV");
   layer.created_devices[0].dispatch_table.CmdSetCheckpointNV(commandBuffer, pCheckpointMarker);
}

VKAPI_ATTR void VKAPI_CALL layer_GetQueueCheckpointDataNV(
    VkQueue queue,
    uint32_t* pCheckpointDataCount,
    VkCheckpointDataNV* pCheckpointData) {
    log_layer_message("Generated Layer vkGetQueueCheckpointDataNV");
   layer.created_devices[0].dispatch_table.GetQueueCheckpointDataNV(queue, pCheckpointDataCount, pCheckpointData);
}


// ----- VK_INTEL_performance_query
VKAPI_ATTR VkResult VKAPI_CALL layer_InitializePerformanceApiINTEL(
    VkDevice device,
    const VkInitializePerformanceApiInfoINTEL* pInitializeInfo) {
    log_layer_message("Generated Layer vkInitializePerformanceApiINTEL");
    return layer.created_devices[0].dispatch_table.InitializePerformanceApiINTEL(device, pInitializeInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_UninitializePerformanceApiINTEL(
    VkDevice device) {
    log_layer_message("Generated Layer vkUninitializePerformanceApiINTEL");
   layer.created_devices[0].dispatch_table.UninitializePerformanceApiINTEL(device);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CmdSetPerformanceMarkerINTEL(
    VkCommandBuffer commandBuffer,
    const VkPerformanceMarkerInfoINTEL* pMarkerInfo) {
    log_layer_message("Generated Layer vkCmdSetPerformanceMarkerINTEL");
    return layer.created_devices[0].dispatch_table.CmdSetPerformanceMarkerINTEL(commandBuffer, pMarkerInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CmdSetPerformanceStreamMarkerINTEL(
    VkCommandBuffer commandBuffer,
    const VkPerformanceStreamMarkerInfoINTEL* pMarkerInfo) {
    log_layer_message("Generated Layer vkCmdSetPerformanceStreamMarkerINTEL");
    return layer.created_devices[0].dispatch_table.CmdSetPerformanceStreamMarkerINTEL(commandBuffer, pMarkerInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CmdSetPerformanceOverrideINTEL(
    VkCommandBuffer commandBuffer,
    const VkPerformanceOverrideInfoINTEL* pOverrideInfo) {
    log_layer_message("Generated Layer vkCmdSetPerformanceOverrideINTEL");
    return layer.created_devices[0].dispatch_table.CmdSetPerformanceOverrideINTEL(commandBuffer, pOverrideInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_AcquirePerformanceConfigurationINTEL(
    VkDevice device,
    const VkPerformanceConfigurationAcquireInfoINTEL* pAcquireInfo,
    VkPerformanceConfigurationINTEL* pConfiguration) {
    log_layer_message("Generated Layer vkAcquirePerformanceConfigurationINTEL");
    return layer.created_devices[0].dispatch_table.AcquirePerformanceConfigurationINTEL(device, pAcquireInfo, pConfiguration);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_ReleasePerformanceConfigurationINTEL(
    VkDevice device,
    VkPerformanceConfigurationINTEL configuration) {
    log_layer_message("Generated Layer vkReleasePerformanceConfigurationINTEL");
    return layer.created_devices[0].dispatch_table.ReleasePerformanceConfigurationINTEL(device, configuration);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_QueueSetPerformanceConfigurationINTEL(
    VkQueue queue,
    VkPerformanceConfigurationINTEL configuration) {
    log_layer_message("Generated Layer vkQueueSetPerformanceConfigurationINTEL");
    return layer.created_devices[0].dispatch_table.QueueSetPerformanceConfigurationINTEL(queue, configuration);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetPerformanceParameterINTEL(
    VkDevice device,
    VkPerformanceParameterTypeINTEL parameter,
    VkPerformanceValueINTEL* pValue) {
    log_layer_message("Generated Layer vkGetPerformanceParameterINTEL");
    return layer.created_devices[0].dispatch_table.GetPerformanceParameterINTEL(device, parameter, pValue);
}


// ----- VK_AMD_display_native_hdr
VKAPI_ATTR void VKAPI_CALL layer_SetLocalDimmingAMD(
    VkDevice device,
    VkSwapchainKHR swapChain,
    VkBool32 localDimmingEnable) {
    log_layer_message("Generated Layer vkSetLocalDimmingAMD");
   layer.created_devices[0].dispatch_table.SetLocalDimmingAMD(device, swapChain, localDimmingEnable);
}


// ----- VK_FUCHSIA_imagepipe_surface
#ifdef VK_USE_PLATFORM_FUCHSIA
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateImagePipeSurfaceFUCHSIA(
    VkInstance instance,
    const VkImagePipeSurfaceCreateInfoFUCHSIA* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_layer_message("Generated Layer vkCreateImagePipeSurfaceFUCHSIA");
    return layer.instance_dispatch_table.CreateImagePipeSurfaceFUCHSIA(instance, pCreateInfo, pAllocator, pSurface);
}

#endif // VK_USE_PLATFORM_FUCHSIA

// ----- VK_EXT_metal_surface
#ifdef VK_USE_PLATFORM_METAL_EXT
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateMetalSurfaceEXT(
    VkInstance instance,
    const VkMetalSurfaceCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_layer_message("Generated Layer vkCreateMetalSurfaceEXT");
    return layer.instance_dispatch_table.CreateMetalSurfaceEXT(instance, pCreateInfo, pAllocator, pSurface);
}

#endif // VK_USE_PLATFORM_METAL_EXT

// ----- VK_EXT_buffer_device_address
VKAPI_ATTR VkDeviceAddress VKAPI_CALL layer_GetBufferDeviceAddressEXT(
    VkDevice device,
    const VkBufferDeviceAddressInfo* pInfo) {
    log_layer_message("Generated Layer vkGetBufferDeviceAddressEXT");
    return layer.created_devices[0].dispatch_table.GetBufferDeviceAddressEXT(device, pInfo);
}


// ----- VK_EXT_tooling_info
VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceToolPropertiesEXT(
    VkPhysicalDevice physicalDevice,
    uint32_t* pToolCount,
    VkPhysicalDeviceToolProperties* pToolProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceToolPropertiesEXT");
    return layer.instance_dispatch_table.GetPhysicalDeviceToolPropertiesEXT(physicalDevice, pToolCount, pToolProperties);
}


// ----- VK_NV_cooperative_matrix
VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceCooperativeMatrixPropertiesNV(
    VkPhysicalDevice physicalDevice,
    uint32_t* pPropertyCount,
    VkCooperativeMatrixPropertiesNV* pProperties) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceCooperativeMatrixPropertiesNV");
    return layer.instance_dispatch_table.GetPhysicalDeviceCooperativeMatrixPropertiesNV(physicalDevice, pPropertyCount, pProperties);
}


// ----- VK_NV_coverage_reduction_mode
VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV(
    VkPhysicalDevice physicalDevice,
    uint32_t* pCombinationCount,
    VkFramebufferMixedSamplesCombinationNV* pCombinations) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV");
    return layer.instance_dispatch_table.GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV(physicalDevice, pCombinationCount, pCombinations);
}


// ----- VK_EXT_full_screen_exclusive
#ifdef VK_USE_PLATFORM_WIN32_KHR
VKAPI_ATTR VkResult VKAPI_CALL layer_GetPhysicalDeviceSurfacePresentModes2EXT(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
    uint32_t* pPresentModeCount,
    VkPresentModeKHR* pPresentModes) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceSurfacePresentModes2EXT");
    return layer.instance_dispatch_table.GetPhysicalDeviceSurfacePresentModes2EXT(physicalDevice, pSurfaceInfo, pPresentModeCount, pPresentModes);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_AcquireFullScreenExclusiveModeEXT(
    VkDevice device,
    VkSwapchainKHR swapchain) {
    log_layer_message("Generated Layer vkAcquireFullScreenExclusiveModeEXT");
    return layer.created_devices[0].dispatch_table.AcquireFullScreenExclusiveModeEXT(device, swapchain);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_ReleaseFullScreenExclusiveModeEXT(
    VkDevice device,
    VkSwapchainKHR swapchain) {
    log_layer_message("Generated Layer vkReleaseFullScreenExclusiveModeEXT");
    return layer.created_devices[0].dispatch_table.ReleaseFullScreenExclusiveModeEXT(device, swapchain);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetDeviceGroupSurfacePresentModes2EXT(
    VkDevice device,
    const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
    VkDeviceGroupPresentModeFlagsKHR* pModes) {
    log_layer_message("Generated Layer vkGetDeviceGroupSurfacePresentModes2EXT");
    return layer.created_devices[0].dispatch_table.GetDeviceGroupSurfacePresentModes2EXT(device, pSurfaceInfo, pModes);
}

#endif // VK_USE_PLATFORM_WIN32_KHR

// ----- VK_EXT_headless_surface
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateHeadlessSurfaceEXT(
    VkInstance instance,
    const VkHeadlessSurfaceCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_layer_message("Generated Layer vkCreateHeadlessSurfaceEXT");
    return layer.instance_dispatch_table.CreateHeadlessSurfaceEXT(instance, pCreateInfo, pAllocator, pSurface);
}


// ----- VK_EXT_line_rasterization
VKAPI_ATTR void VKAPI_CALL layer_CmdSetLineStippleEXT(
    VkCommandBuffer commandBuffer,
    uint32_t lineStippleFactor,
    uint16_t lineStipplePattern) {
    log_layer_message("Generated Layer vkCmdSetLineStippleEXT");
   layer.created_devices[0].dispatch_table.CmdSetLineStippleEXT(commandBuffer, lineStippleFactor, lineStipplePattern);
}


// ----- VK_EXT_host_query_reset
VKAPI_ATTR void VKAPI_CALL layer_ResetQueryPoolEXT(
    VkDevice device,
    VkQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount) {
    log_layer_message("Generated Layer vkResetQueryPoolEXT");
   layer.created_devices[0].dispatch_table.ResetQueryPoolEXT(device, queryPool, firstQuery, queryCount);
}


// ----- VK_EXT_extended_dynamic_state
VKAPI_ATTR void VKAPI_CALL layer_CmdSetCullModeEXT(
    VkCommandBuffer commandBuffer,
    VkCullModeFlags cullMode) {
    log_layer_message("Generated Layer vkCmdSetCullModeEXT");
   layer.created_devices[0].dispatch_table.CmdSetCullModeEXT(commandBuffer, cullMode);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetFrontFaceEXT(
    VkCommandBuffer commandBuffer,
    VkFrontFace frontFace) {
    log_layer_message("Generated Layer vkCmdSetFrontFaceEXT");
   layer.created_devices[0].dispatch_table.CmdSetFrontFaceEXT(commandBuffer, frontFace);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetPrimitiveTopologyEXT(
    VkCommandBuffer commandBuffer,
    VkPrimitiveTopology primitiveTopology) {
    log_layer_message("Generated Layer vkCmdSetPrimitiveTopologyEXT");
   layer.created_devices[0].dispatch_table.CmdSetPrimitiveTopologyEXT(commandBuffer, primitiveTopology);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetViewportWithCountEXT(
    VkCommandBuffer commandBuffer,
    uint32_t viewportCount,
    const VkViewport* pViewports) {
    log_layer_message("Generated Layer vkCmdSetViewportWithCountEXT");
   layer.created_devices[0].dispatch_table.CmdSetViewportWithCountEXT(commandBuffer, viewportCount, pViewports);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetScissorWithCountEXT(
    VkCommandBuffer commandBuffer,
    uint32_t scissorCount,
    const VkRect2D* pScissors) {
    log_layer_message("Generated Layer vkCmdSetScissorWithCountEXT");
   layer.created_devices[0].dispatch_table.CmdSetScissorWithCountEXT(commandBuffer, scissorCount, pScissors);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBindVertexBuffers2EXT(
    VkCommandBuffer commandBuffer,
    uint32_t firstBinding,
    uint32_t bindingCount,
    const VkBuffer* pBuffers,
    const VkDeviceSize* pOffsets,
    const VkDeviceSize* pSizes,
    const VkDeviceSize* pStrides) {
    log_layer_message("Generated Layer vkCmdBindVertexBuffers2EXT");
   layer.created_devices[0].dispatch_table.CmdBindVertexBuffers2EXT(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes, pStrides);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetDepthTestEnableEXT(
    VkCommandBuffer commandBuffer,
    VkBool32 depthTestEnable) {
    log_layer_message("Generated Layer vkCmdSetDepthTestEnableEXT");
   layer.created_devices[0].dispatch_table.CmdSetDepthTestEnableEXT(commandBuffer, depthTestEnable);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetDepthWriteEnableEXT(
    VkCommandBuffer commandBuffer,
    VkBool32 depthWriteEnable) {
    log_layer_message("Generated Layer vkCmdSetDepthWriteEnableEXT");
   layer.created_devices[0].dispatch_table.CmdSetDepthWriteEnableEXT(commandBuffer, depthWriteEnable);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetDepthCompareOpEXT(
    VkCommandBuffer commandBuffer,
    VkCompareOp depthCompareOp) {
    log_layer_message("Generated Layer vkCmdSetDepthCompareOpEXT");
   layer.created_devices[0].dispatch_table.CmdSetDepthCompareOpEXT(commandBuffer, depthCompareOp);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetDepthBoundsTestEnableEXT(
    VkCommandBuffer commandBuffer,
    VkBool32 depthBoundsTestEnable) {
    log_layer_message("Generated Layer vkCmdSetDepthBoundsTestEnableEXT");
   layer.created_devices[0].dispatch_table.CmdSetDepthBoundsTestEnableEXT(commandBuffer, depthBoundsTestEnable);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetStencilTestEnableEXT(
    VkCommandBuffer commandBuffer,
    VkBool32 stencilTestEnable) {
    log_layer_message("Generated Layer vkCmdSetStencilTestEnableEXT");
   layer.created_devices[0].dispatch_table.CmdSetStencilTestEnableEXT(commandBuffer, stencilTestEnable);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetStencilOpEXT(
    VkCommandBuffer commandBuffer,
    VkStencilFaceFlags faceMask,
    VkStencilOp failOp,
    VkStencilOp passOp,
    VkStencilOp depthFailOp,
    VkCompareOp compareOp) {
    log_layer_message("Generated Layer vkCmdSetStencilOpEXT");
   layer.created_devices[0].dispatch_table.CmdSetStencilOpEXT(commandBuffer, faceMask, failOp, passOp, depthFailOp, compareOp);
}


// ----- VK_NV_device_generated_commands
VKAPI_ATTR void VKAPI_CALL layer_GetGeneratedCommandsMemoryRequirementsNV(
    VkDevice device,
    const VkGeneratedCommandsMemoryRequirementsInfoNV* pInfo,
    VkMemoryRequirements2* pMemoryRequirements) {
    log_layer_message("Generated Layer vkGetGeneratedCommandsMemoryRequirementsNV");
   layer.created_devices[0].dispatch_table.GetGeneratedCommandsMemoryRequirementsNV(device, pInfo, pMemoryRequirements);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdPreprocessGeneratedCommandsNV(
    VkCommandBuffer commandBuffer,
    const VkGeneratedCommandsInfoNV* pGeneratedCommandsInfo) {
    log_layer_message("Generated Layer vkCmdPreprocessGeneratedCommandsNV");
   layer.created_devices[0].dispatch_table.CmdPreprocessGeneratedCommandsNV(commandBuffer, pGeneratedCommandsInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdExecuteGeneratedCommandsNV(
    VkCommandBuffer commandBuffer,
    VkBool32 isPreprocessed,
    const VkGeneratedCommandsInfoNV* pGeneratedCommandsInfo) {
    log_layer_message("Generated Layer vkCmdExecuteGeneratedCommandsNV");
   layer.created_devices[0].dispatch_table.CmdExecuteGeneratedCommandsNV(commandBuffer, isPreprocessed, pGeneratedCommandsInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBindPipelineShaderGroupNV(
    VkCommandBuffer commandBuffer,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipeline pipeline,
    uint32_t groupIndex) {
    log_layer_message("Generated Layer vkCmdBindPipelineShaderGroupNV");
   layer.created_devices[0].dispatch_table.CmdBindPipelineShaderGroupNV(commandBuffer, pipelineBindPoint, pipeline, groupIndex);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateIndirectCommandsLayoutNV(
    VkDevice device,
    const VkIndirectCommandsLayoutCreateInfoNV* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkIndirectCommandsLayoutNV* pIndirectCommandsLayout) {
    log_layer_message("Generated Layer vkCreateIndirectCommandsLayoutNV");
    return layer.created_devices[0].dispatch_table.CreateIndirectCommandsLayoutNV(device, pCreateInfo, pAllocator, pIndirectCommandsLayout);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyIndirectCommandsLayoutNV(
    VkDevice device,
    VkIndirectCommandsLayoutNV indirectCommandsLayout,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyIndirectCommandsLayoutNV");
   layer.created_devices[0].dispatch_table.DestroyIndirectCommandsLayoutNV(device, indirectCommandsLayout, pAllocator);
}


// ----- VK_EXT_acquire_drm_display
VKAPI_ATTR VkResult VKAPI_CALL layer_AcquireDrmDisplayEXT(
    VkPhysicalDevice physicalDevice,
    int32_t drmFd,
    VkDisplayKHR display) {
    log_layer_message("Generated Layer vkAcquireDrmDisplayEXT");
    return layer.instance_dispatch_table.AcquireDrmDisplayEXT(physicalDevice, drmFd, display);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetDrmDisplayEXT(
    VkPhysicalDevice physicalDevice,
    int32_t drmFd,
    uint32_t connectorId,
    VkDisplayKHR* display) {
    log_layer_message("Generated Layer vkGetDrmDisplayEXT");
    return layer.instance_dispatch_table.GetDrmDisplayEXT(physicalDevice, drmFd, connectorId, display);
}


// ----- VK_EXT_private_data
VKAPI_ATTR VkResult VKAPI_CALL layer_CreatePrivateDataSlotEXT(
    VkDevice device,
    const VkPrivateDataSlotCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkPrivateDataSlot* pPrivateDataSlot) {
    log_layer_message("Generated Layer vkCreatePrivateDataSlotEXT");
    return layer.created_devices[0].dispatch_table.CreatePrivateDataSlotEXT(device, pCreateInfo, pAllocator, pPrivateDataSlot);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyPrivateDataSlotEXT(
    VkDevice device,
    VkPrivateDataSlot privateDataSlot,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyPrivateDataSlotEXT");
   layer.created_devices[0].dispatch_table.DestroyPrivateDataSlotEXT(device, privateDataSlot, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_SetPrivateDataEXT(
    VkDevice device,
    VkObjectType objectType,
    uint64_t objectHandle,
    VkPrivateDataSlot privateDataSlot,
    uint64_t data) {
    log_layer_message("Generated Layer vkSetPrivateDataEXT");
    return layer.created_devices[0].dispatch_table.SetPrivateDataEXT(device, objectType, objectHandle, privateDataSlot, data);
}

VKAPI_ATTR void VKAPI_CALL layer_GetPrivateDataEXT(
    VkDevice device,
    VkObjectType objectType,
    uint64_t objectHandle,
    VkPrivateDataSlot privateDataSlot,
    uint64_t* pData) {
    log_layer_message("Generated Layer vkGetPrivateDataEXT");
   layer.created_devices[0].dispatch_table.GetPrivateDataEXT(device, objectType, objectHandle, privateDataSlot, pData);
}


// ----- VK_NV_fragment_shading_rate_enums
VKAPI_ATTR void VKAPI_CALL layer_CmdSetFragmentShadingRateEnumNV(
    VkCommandBuffer commandBuffer,
    VkFragmentShadingRateNV shadingRate,
    const VkFragmentShadingRateCombinerOpKHR combinerOps[2]) {
    log_layer_message("Generated Layer vkCmdSetFragmentShadingRateEnumNV");
   layer.created_devices[0].dispatch_table.CmdSetFragmentShadingRateEnumNV(commandBuffer, shadingRate, combinerOps);
}


// ----- VK_NV_acquire_winrt_display
#ifdef VK_USE_PLATFORM_WIN32_KHR
VKAPI_ATTR VkResult VKAPI_CALL layer_AcquireWinrtDisplayNV(
    VkPhysicalDevice physicalDevice,
    VkDisplayKHR display) {
    log_layer_message("Generated Layer vkAcquireWinrtDisplayNV");
    return layer.instance_dispatch_table.AcquireWinrtDisplayNV(physicalDevice, display);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetWinrtDisplayNV(
    VkPhysicalDevice physicalDevice,
    uint32_t deviceRelativeId,
    VkDisplayKHR* pDisplay) {
    log_layer_message("Generated Layer vkGetWinrtDisplayNV");
    return layer.instance_dispatch_table.GetWinrtDisplayNV(physicalDevice, deviceRelativeId, pDisplay);
}

#endif // VK_USE_PLATFORM_WIN32_KHR

// ----- VK_EXT_directfb_surface
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateDirectFBSurfaceEXT(
    VkInstance instance,
    const VkDirectFBSurfaceCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_layer_message("Generated Layer vkCreateDirectFBSurfaceEXT");
    return layer.instance_dispatch_table.CreateDirectFBSurfaceEXT(instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkBool32 VKAPI_CALL layer_GetPhysicalDeviceDirectFBPresentationSupportEXT(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    IDirectFB* dfb) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceDirectFBPresentationSupportEXT");
    return layer.instance_dispatch_table.GetPhysicalDeviceDirectFBPresentationSupportEXT(physicalDevice, queueFamilyIndex, dfb);
}

#endif // VK_USE_PLATFORM_DIRECTFB_EXT

// ----- VK_EXT_vertex_input_dynamic_state
VKAPI_ATTR void VKAPI_CALL layer_CmdSetVertexInputEXT(
    VkCommandBuffer commandBuffer,
    uint32_t vertexBindingDescriptionCount,
    const VkVertexInputBindingDescription2EXT* pVertexBindingDescriptions,
    uint32_t vertexAttributeDescriptionCount,
    const VkVertexInputAttributeDescription2EXT* pVertexAttributeDescriptions) {
    log_layer_message("Generated Layer vkCmdSetVertexInputEXT");
   layer.created_devices[0].dispatch_table.CmdSetVertexInputEXT(commandBuffer, vertexBindingDescriptionCount, pVertexBindingDescriptions, vertexAttributeDescriptionCount, pVertexAttributeDescriptions);
}


// ----- VK_FUCHSIA_external_memory
#ifdef VK_USE_PLATFORM_FUCHSIA
VKAPI_ATTR VkResult VKAPI_CALL layer_GetMemoryZirconHandleFUCHSIA(
    VkDevice device,
    const VkMemoryGetZirconHandleInfoFUCHSIA* pGetZirconHandleInfo,
    zx_handle_t* pZirconHandle) {
    log_layer_message("Generated Layer vkGetMemoryZirconHandleFUCHSIA");
    return layer.created_devices[0].dispatch_table.GetMemoryZirconHandleFUCHSIA(device, pGetZirconHandleInfo, pZirconHandle);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetMemoryZirconHandlePropertiesFUCHSIA(
    VkDevice device,
    VkExternalMemoryHandleTypeFlagBits handleType,
    zx_handle_t zirconHandle,
    VkMemoryZirconHandlePropertiesFUCHSIA* pMemoryZirconHandleProperties) {
    log_layer_message("Generated Layer vkGetMemoryZirconHandlePropertiesFUCHSIA");
    return layer.created_devices[0].dispatch_table.GetMemoryZirconHandlePropertiesFUCHSIA(device, handleType, zirconHandle, pMemoryZirconHandleProperties);
}

#endif // VK_USE_PLATFORM_FUCHSIA

// ----- VK_FUCHSIA_external_semaphore
#ifdef VK_USE_PLATFORM_FUCHSIA
VKAPI_ATTR VkResult VKAPI_CALL layer_ImportSemaphoreZirconHandleFUCHSIA(
    VkDevice device,
    const VkImportSemaphoreZirconHandleInfoFUCHSIA* pImportSemaphoreZirconHandleInfo) {
    log_layer_message("Generated Layer vkImportSemaphoreZirconHandleFUCHSIA");
    return layer.created_devices[0].dispatch_table.ImportSemaphoreZirconHandleFUCHSIA(device, pImportSemaphoreZirconHandleInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetSemaphoreZirconHandleFUCHSIA(
    VkDevice device,
    const VkSemaphoreGetZirconHandleInfoFUCHSIA* pGetZirconHandleInfo,
    zx_handle_t* pZirconHandle) {
    log_layer_message("Generated Layer vkGetSemaphoreZirconHandleFUCHSIA");
    return layer.created_devices[0].dispatch_table.GetSemaphoreZirconHandleFUCHSIA(device, pGetZirconHandleInfo, pZirconHandle);
}

#endif // VK_USE_PLATFORM_FUCHSIA

// ----- VK_FUCHSIA_buffer_collection
#ifdef VK_USE_PLATFORM_FUCHSIA
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateBufferCollectionFUCHSIA(
    VkDevice device,
    const VkBufferCollectionCreateInfoFUCHSIA* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkBufferCollectionFUCHSIA* pCollection) {
    log_layer_message("Generated Layer vkCreateBufferCollectionFUCHSIA");
    return layer.created_devices[0].dispatch_table.CreateBufferCollectionFUCHSIA(device, pCreateInfo, pAllocator, pCollection);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_SetBufferCollectionImageConstraintsFUCHSIA(
    VkDevice device,
    VkBufferCollectionFUCHSIA collection,
    const VkImageConstraintsInfoFUCHSIA* pImageConstraintsInfo) {
    log_layer_message("Generated Layer vkSetBufferCollectionImageConstraintsFUCHSIA");
    return layer.created_devices[0].dispatch_table.SetBufferCollectionImageConstraintsFUCHSIA(device, collection, pImageConstraintsInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_SetBufferCollectionBufferConstraintsFUCHSIA(
    VkDevice device,
    VkBufferCollectionFUCHSIA collection,
    const VkBufferConstraintsInfoFUCHSIA* pBufferConstraintsInfo) {
    log_layer_message("Generated Layer vkSetBufferCollectionBufferConstraintsFUCHSIA");
    return layer.created_devices[0].dispatch_table.SetBufferCollectionBufferConstraintsFUCHSIA(device, collection, pBufferConstraintsInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyBufferCollectionFUCHSIA(
    VkDevice device,
    VkBufferCollectionFUCHSIA collection,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyBufferCollectionFUCHSIA");
   layer.created_devices[0].dispatch_table.DestroyBufferCollectionFUCHSIA(device, collection, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetBufferCollectionPropertiesFUCHSIA(
    VkDevice device,
    VkBufferCollectionFUCHSIA collection,
    VkBufferCollectionPropertiesFUCHSIA* pProperties) {
    log_layer_message("Generated Layer vkGetBufferCollectionPropertiesFUCHSIA");
    return layer.created_devices[0].dispatch_table.GetBufferCollectionPropertiesFUCHSIA(device, collection, pProperties);
}

#endif // VK_USE_PLATFORM_FUCHSIA

// ----- VK_HUAWEI_subpass_shading
VKAPI_ATTR VkResult VKAPI_CALL layer_GetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI(
    VkDevice device,
    VkRenderPass renderpass,
    VkExtent2D* pMaxWorkgroupSize) {
    log_layer_message("Generated Layer vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI");
    return layer.created_devices[0].dispatch_table.GetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI(device, renderpass, pMaxWorkgroupSize);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSubpassShadingHUAWEI(
    VkCommandBuffer commandBuffer) {
    log_layer_message("Generated Layer vkCmdSubpassShadingHUAWEI");
   layer.created_devices[0].dispatch_table.CmdSubpassShadingHUAWEI(commandBuffer);
}


// ----- VK_HUAWEI_invocation_mask
VKAPI_ATTR void VKAPI_CALL layer_CmdBindInvocationMaskHUAWEI(
    VkCommandBuffer commandBuffer,
    VkImageView imageView,
    VkImageLayout imageLayout) {
    log_layer_message("Generated Layer vkCmdBindInvocationMaskHUAWEI");
   layer.created_devices[0].dispatch_table.CmdBindInvocationMaskHUAWEI(commandBuffer, imageView, imageLayout);
}


// ----- VK_NV_external_memory_rdma
VKAPI_ATTR VkResult VKAPI_CALL layer_GetMemoryRemoteAddressNV(
    VkDevice device,
    const VkMemoryGetRemoteAddressInfoNV* pMemoryGetRemoteAddressInfo,
    VkRemoteAddressNV* pAddress) {
    log_layer_message("Generated Layer vkGetMemoryRemoteAddressNV");
    return layer.created_devices[0].dispatch_table.GetMemoryRemoteAddressNV(device, pMemoryGetRemoteAddressInfo, pAddress);
}


// ----- VK_EXT_extended_dynamic_state2
VKAPI_ATTR void VKAPI_CALL layer_CmdSetPatchControlPointsEXT(
    VkCommandBuffer commandBuffer,
    uint32_t patchControlPoints) {
    log_layer_message("Generated Layer vkCmdSetPatchControlPointsEXT");
   layer.created_devices[0].dispatch_table.CmdSetPatchControlPointsEXT(commandBuffer, patchControlPoints);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetRasterizerDiscardEnableEXT(
    VkCommandBuffer commandBuffer,
    VkBool32 rasterizerDiscardEnable) {
    log_layer_message("Generated Layer vkCmdSetRasterizerDiscardEnableEXT");
   layer.created_devices[0].dispatch_table.CmdSetRasterizerDiscardEnableEXT(commandBuffer, rasterizerDiscardEnable);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetDepthBiasEnableEXT(
    VkCommandBuffer commandBuffer,
    VkBool32 depthBiasEnable) {
    log_layer_message("Generated Layer vkCmdSetDepthBiasEnableEXT");
   layer.created_devices[0].dispatch_table.CmdSetDepthBiasEnableEXT(commandBuffer, depthBiasEnable);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetLogicOpEXT(
    VkCommandBuffer commandBuffer,
    VkLogicOp logicOp) {
    log_layer_message("Generated Layer vkCmdSetLogicOpEXT");
   layer.created_devices[0].dispatch_table.CmdSetLogicOpEXT(commandBuffer, logicOp);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetPrimitiveRestartEnableEXT(
    VkCommandBuffer commandBuffer,
    VkBool32 primitiveRestartEnable) {
    log_layer_message("Generated Layer vkCmdSetPrimitiveRestartEnableEXT");
   layer.created_devices[0].dispatch_table.CmdSetPrimitiveRestartEnableEXT(commandBuffer, primitiveRestartEnable);
}


// ----- VK_QNX_screen_surface
#ifdef VK_USE_PLATFORM_SCREEN_QNX
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateScreenSurfaceQNX(
    VkInstance instance,
    const VkScreenSurfaceCreateInfoQNX* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_layer_message("Generated Layer vkCreateScreenSurfaceQNX");
    return layer.instance_dispatch_table.CreateScreenSurfaceQNX(instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkBool32 VKAPI_CALL layer_GetPhysicalDeviceScreenPresentationSupportQNX(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    struct _screen_window* window) {
    log_layer_message("Generated Layer vkGetPhysicalDeviceScreenPresentationSupportQNX");
    return layer.instance_dispatch_table.GetPhysicalDeviceScreenPresentationSupportQNX(physicalDevice, queueFamilyIndex, window);
}

#endif // VK_USE_PLATFORM_SCREEN_QNX

// ----- VK_EXT_color_write_enable
VKAPI_ATTR void                                    VKAPI_CALL layer_CmdSetColorWriteEnableEXT(
    VkCommandBuffer commandBuffer,
    uint32_t attachmentCount,
    const VkBool32* pColorWriteEnables) {
    log_layer_message("Generated Layer vkCmdSetColorWriteEnableEXT");
   layer.created_devices[0].dispatch_table.CmdSetColorWriteEnableEXT(commandBuffer, attachmentCount, pColorWriteEnables);
}


// ----- VK_EXT_multi_draw
VKAPI_ATTR void VKAPI_CALL layer_CmdDrawMultiEXT(
    VkCommandBuffer commandBuffer,
    uint32_t drawCount,
    const VkMultiDrawInfoEXT* pVertexInfo,
    uint32_t instanceCount,
    uint32_t firstInstance,
    uint32_t stride) {
    log_layer_message("Generated Layer vkCmdDrawMultiEXT");
   layer.created_devices[0].dispatch_table.CmdDrawMultiEXT(commandBuffer, drawCount, pVertexInfo, instanceCount, firstInstance, stride);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdDrawMultiIndexedEXT(
    VkCommandBuffer commandBuffer,
    uint32_t drawCount,
    const VkMultiDrawIndexedInfoEXT* pIndexInfo,
    uint32_t instanceCount,
    uint32_t firstInstance,
    uint32_t stride,
    const int32_t* pVertexOffset) {
    log_layer_message("Generated Layer vkCmdDrawMultiIndexedEXT");
   layer.created_devices[0].dispatch_table.CmdDrawMultiIndexedEXT(commandBuffer, drawCount, pIndexInfo, instanceCount, firstInstance, stride, pVertexOffset);
}


// ----- VK_EXT_pageable_device_local_memory
VKAPI_ATTR void VKAPI_CALL layer_SetDeviceMemoryPriorityEXT(
    VkDevice device,
    VkDeviceMemory memory,
    float priority) {
    log_layer_message("Generated Layer vkSetDeviceMemoryPriorityEXT");
   layer.created_devices[0].dispatch_table.SetDeviceMemoryPriorityEXT(device, memory, priority);
}


// ----- VK_VALVE_descriptor_set_host_mapping
VKAPI_ATTR void VKAPI_CALL layer_GetDescriptorSetLayoutHostMappingInfoVALVE(
    VkDevice device,
    const VkDescriptorSetBindingReferenceVALVE* pBindingReference,
    VkDescriptorSetLayoutHostMappingInfoVALVE* pHostMapping) {
    log_layer_message("Generated Layer vkGetDescriptorSetLayoutHostMappingInfoVALVE");
   layer.created_devices[0].dispatch_table.GetDescriptorSetLayoutHostMappingInfoVALVE(device, pBindingReference, pHostMapping);
}

VKAPI_ATTR void VKAPI_CALL layer_GetDescriptorSetHostMappingVALVE(
    VkDevice device,
    VkDescriptorSet descriptorSet,
    void** ppData) {
    log_layer_message("Generated Layer vkGetDescriptorSetHostMappingVALVE");
   layer.created_devices[0].dispatch_table.GetDescriptorSetHostMappingVALVE(device, descriptorSet, ppData);
}


// ----- VK_KHR_acceleration_structure
VKAPI_ATTR VkResult VKAPI_CALL layer_CreateAccelerationStructureKHR(
    VkDevice device,
    const VkAccelerationStructureCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkAccelerationStructureKHR* pAccelerationStructure) {
    log_layer_message("Generated Layer vkCreateAccelerationStructureKHR");
    return layer.created_devices[0].dispatch_table.CreateAccelerationStructureKHR(device, pCreateInfo, pAllocator, pAccelerationStructure);
}

VKAPI_ATTR void VKAPI_CALL layer_DestroyAccelerationStructureKHR(
    VkDevice device,
    VkAccelerationStructureKHR accelerationStructure,
    const VkAllocationCallbacks* pAllocator) {
    log_layer_message("Generated Layer vkDestroyAccelerationStructureKHR");
   layer.created_devices[0].dispatch_table.DestroyAccelerationStructureKHR(device, accelerationStructure, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBuildAccelerationStructuresKHR(
    VkCommandBuffer commandBuffer,
    uint32_t infoCount,
    const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
    const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos) {
    log_layer_message("Generated Layer vkCmdBuildAccelerationStructuresKHR");
   layer.created_devices[0].dispatch_table.CmdBuildAccelerationStructuresKHR(commandBuffer, infoCount, pInfos, ppBuildRangeInfos);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdBuildAccelerationStructuresIndirectKHR(
    VkCommandBuffer commandBuffer,
    uint32_t infoCount,
    const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
    const VkDeviceAddress* pIndirectDeviceAddresses,
    const uint32_t* pIndirectStrides,
    const uint32_t* const* ppMaxPrimitiveCounts) {
    log_layer_message("Generated Layer vkCmdBuildAccelerationStructuresIndirectKHR");
   layer.created_devices[0].dispatch_table.CmdBuildAccelerationStructuresIndirectKHR(commandBuffer, infoCount, pInfos, pIndirectDeviceAddresses, pIndirectStrides, ppMaxPrimitiveCounts);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_BuildAccelerationStructuresKHR(
    VkDevice device,
    VkDeferredOperationKHR deferredOperation,
    uint32_t infoCount,
    const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
    const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos) {
    log_layer_message("Generated Layer vkBuildAccelerationStructuresKHR");
    return layer.created_devices[0].dispatch_table.BuildAccelerationStructuresKHR(device, deferredOperation, infoCount, pInfos, ppBuildRangeInfos);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CopyAccelerationStructureKHR(
    VkDevice device,
    VkDeferredOperationKHR deferredOperation,
    const VkCopyAccelerationStructureInfoKHR* pInfo) {
    log_layer_message("Generated Layer vkCopyAccelerationStructureKHR");
    return layer.created_devices[0].dispatch_table.CopyAccelerationStructureKHR(device, deferredOperation, pInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CopyAccelerationStructureToMemoryKHR(
    VkDevice device,
    VkDeferredOperationKHR deferredOperation,
    const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo) {
    log_layer_message("Generated Layer vkCopyAccelerationStructureToMemoryKHR");
    return layer.created_devices[0].dispatch_table.CopyAccelerationStructureToMemoryKHR(device, deferredOperation, pInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CopyMemoryToAccelerationStructureKHR(
    VkDevice device,
    VkDeferredOperationKHR deferredOperation,
    const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo) {
    log_layer_message("Generated Layer vkCopyMemoryToAccelerationStructureKHR");
    return layer.created_devices[0].dispatch_table.CopyMemoryToAccelerationStructureKHR(device, deferredOperation, pInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_WriteAccelerationStructuresPropertiesKHR(
    VkDevice device,
    uint32_t accelerationStructureCount,
    const VkAccelerationStructureKHR* pAccelerationStructures,
    VkQueryType queryType,
    size_t dataSize,
    void* pData,
    size_t stride) {
    log_layer_message("Generated Layer vkWriteAccelerationStructuresPropertiesKHR");
    return layer.created_devices[0].dispatch_table.WriteAccelerationStructuresPropertiesKHR(device, accelerationStructureCount, pAccelerationStructures, queryType, dataSize, pData, stride);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdCopyAccelerationStructureKHR(
    VkCommandBuffer commandBuffer,
    const VkCopyAccelerationStructureInfoKHR* pInfo) {
    log_layer_message("Generated Layer vkCmdCopyAccelerationStructureKHR");
   layer.created_devices[0].dispatch_table.CmdCopyAccelerationStructureKHR(commandBuffer, pInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdCopyAccelerationStructureToMemoryKHR(
    VkCommandBuffer commandBuffer,
    const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo) {
    log_layer_message("Generated Layer vkCmdCopyAccelerationStructureToMemoryKHR");
   layer.created_devices[0].dispatch_table.CmdCopyAccelerationStructureToMemoryKHR(commandBuffer, pInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdCopyMemoryToAccelerationStructureKHR(
    VkCommandBuffer commandBuffer,
    const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo) {
    log_layer_message("Generated Layer vkCmdCopyMemoryToAccelerationStructureKHR");
   layer.created_devices[0].dispatch_table.CmdCopyMemoryToAccelerationStructureKHR(commandBuffer, pInfo);
}

VKAPI_ATTR VkDeviceAddress VKAPI_CALL layer_GetAccelerationStructureDeviceAddressKHR(
    VkDevice device,
    const VkAccelerationStructureDeviceAddressInfoKHR* pInfo) {
    log_layer_message("Generated Layer vkGetAccelerationStructureDeviceAddressKHR");
    return layer.created_devices[0].dispatch_table.GetAccelerationStructureDeviceAddressKHR(device, pInfo);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdWriteAccelerationStructuresPropertiesKHR(
    VkCommandBuffer commandBuffer,
    uint32_t accelerationStructureCount,
    const VkAccelerationStructureKHR* pAccelerationStructures,
    VkQueryType queryType,
    VkQueryPool queryPool,
    uint32_t firstQuery) {
    log_layer_message("Generated Layer vkCmdWriteAccelerationStructuresPropertiesKHR");
   layer.created_devices[0].dispatch_table.CmdWriteAccelerationStructuresPropertiesKHR(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
}

VKAPI_ATTR void VKAPI_CALL layer_GetDeviceAccelerationStructureCompatibilityKHR(
    VkDevice device,
    const VkAccelerationStructureVersionInfoKHR* pVersionInfo,
    VkAccelerationStructureCompatibilityKHR* pCompatibility) {
    log_layer_message("Generated Layer vkGetDeviceAccelerationStructureCompatibilityKHR");
   layer.created_devices[0].dispatch_table.GetDeviceAccelerationStructureCompatibilityKHR(device, pVersionInfo, pCompatibility);
}

VKAPI_ATTR void VKAPI_CALL layer_GetAccelerationStructureBuildSizesKHR(
    VkDevice device,
    VkAccelerationStructureBuildTypeKHR buildType,
    const VkAccelerationStructureBuildGeometryInfoKHR* pBuildInfo,
    const uint32_t* pMaxPrimitiveCounts,
    VkAccelerationStructureBuildSizesInfoKHR* pSizeInfo) {
    log_layer_message("Generated Layer vkGetAccelerationStructureBuildSizesKHR");
   layer.created_devices[0].dispatch_table.GetAccelerationStructureBuildSizesKHR(device, buildType, pBuildInfo, pMaxPrimitiveCounts, pSizeInfo);
}


// ----- VK_KHR_ray_tracing_pipeline
VKAPI_ATTR void VKAPI_CALL layer_CmdTraceRaysKHR(
    VkCommandBuffer commandBuffer,
    const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable,
    uint32_t width,
    uint32_t height,
    uint32_t depth) {
    log_layer_message("Generated Layer vkCmdTraceRaysKHR");
   layer.created_devices[0].dispatch_table.CmdTraceRaysKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, width, height, depth);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_CreateRayTracingPipelinesKHR(
    VkDevice device,
    VkDeferredOperationKHR deferredOperation,
    VkPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const VkRayTracingPipelineCreateInfoKHR* pCreateInfos,
    const VkAllocationCallbacks* pAllocator,
    VkPipeline* pPipelines) {
    log_layer_message("Generated Layer vkCreateRayTracingPipelinesKHR");
    return layer.created_devices[0].dispatch_table.CreateRayTracingPipelinesKHR(device, deferredOperation, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetRayTracingShaderGroupHandlesKHR(
    VkDevice device,
    VkPipeline pipeline,
    uint32_t firstGroup,
    uint32_t groupCount,
    size_t dataSize,
    void* pData) {
    log_layer_message("Generated Layer vkGetRayTracingShaderGroupHandlesKHR");
    return layer.created_devices[0].dispatch_table.GetRayTracingShaderGroupHandlesKHR(device, pipeline, firstGroup, groupCount, dataSize, pData);
}

VKAPI_ATTR VkResult VKAPI_CALL layer_GetRayTracingCaptureReplayShaderGroupHandlesKHR(
    VkDevice device,
    VkPipeline pipeline,
    uint32_t firstGroup,
    uint32_t groupCount,
    size_t dataSize,
    void* pData) {
    log_layer_message("Generated Layer vkGetRayTracingCaptureReplayShaderGroupHandlesKHR");
    return layer.created_devices[0].dispatch_table.GetRayTracingCaptureReplayShaderGroupHandlesKHR(device, pipeline, firstGroup, groupCount, dataSize, pData);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdTraceRaysIndirectKHR(
    VkCommandBuffer commandBuffer,
    const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable,
    VkDeviceAddress indirectDeviceAddress) {
    log_layer_message("Generated Layer vkCmdTraceRaysIndirectKHR");
   layer.created_devices[0].dispatch_table.CmdTraceRaysIndirectKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, indirectDeviceAddress);
}

VKAPI_ATTR VkDeviceSize VKAPI_CALL layer_GetRayTracingShaderGroupStackSizeKHR(
    VkDevice device,
    VkPipeline pipeline,
    uint32_t group,
    VkShaderGroupShaderKHR groupShader) {
    log_layer_message("Generated Layer vkGetRayTracingShaderGroupStackSizeKHR");
    return layer.created_devices[0].dispatch_table.GetRayTracingShaderGroupStackSizeKHR(device, pipeline, group, groupShader);
}

VKAPI_ATTR void VKAPI_CALL layer_CmdSetRayTracingPipelineStackSizeKHR(
    VkCommandBuffer commandBuffer,
    uint32_t pipelineStackSize) {
    log_layer_message("Generated Layer vkCmdSetRayTracingPipelineStackSizeKHR");
   layer.created_devices[0].dispatch_table.CmdSetRayTracingPipelineStackSizeKHR(commandBuffer, pipelineStackSize);
}



// Exported functions
extern "C" {

bool IsInstanceExtensionEnabled(const char* extension_name) {
    return layer.enabled_instance_extensions.end() !=
           std::find_if(layer.enabled_instance_extensions.begin(),
                        layer.enabled_instance_extensions.end(),
                        [extension_name](Extension const& ext) { return ext.extensionName == extension_name; });
}

// Prototypes:
FRAMEWORK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL layer_GetDeviceProcAddr(VkDevice device, const char* pName);


FRAMEWORK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL layer_GetInstanceProcAddr(VkInstance instance, const char* pName) {

    // ----- VK_VERSION_1_0
    {
        if (string_eq(pName, "vkCreateInstance")) return to_vkVoidFunction(layer_CreateInstance);
        if (string_eq(pName, "vkDestroyInstance")) return to_vkVoidFunction(layer_DestroyInstance);
        if (string_eq(pName, "vkEnumeratePhysicalDevices")) return to_vkVoidFunction(layer_EnumeratePhysicalDevices);
        if (string_eq(pName, "vkGetPhysicalDeviceFeatures")) return to_vkVoidFunction(layer_GetPhysicalDeviceFeatures);
        if (string_eq(pName, "vkGetPhysicalDeviceFormatProperties")) return to_vkVoidFunction(layer_GetPhysicalDeviceFormatProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceImageFormatProperties")) return to_vkVoidFunction(layer_GetPhysicalDeviceImageFormatProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceProperties")) return to_vkVoidFunction(layer_GetPhysicalDeviceProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyProperties")) return to_vkVoidFunction(layer_GetPhysicalDeviceQueueFamilyProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceMemoryProperties")) return to_vkVoidFunction(layer_GetPhysicalDeviceMemoryProperties);
        if (string_eq(pName, "vkGetInstanceProcAddr")) return to_vkVoidFunction(layer_GetInstanceProcAddr);
        if (string_eq(pName, "vkGetDeviceProcAddr")) return to_vkVoidFunction(layer_GetDeviceProcAddr);
        if (string_eq(pName, "vkCreateDevice")) return to_vkVoidFunction(layer_CreateDevice);
        if (string_eq(pName, "vkDestroyDevice")) return to_vkVoidFunction(layer_DestroyDevice);
        if (string_eq(pName, "vkEnumerateInstanceExtensionProperties")) return to_vkVoidFunction(layer_EnumerateInstanceExtensionProperties);
        if (string_eq(pName, "vkEnumerateDeviceExtensionProperties")) return to_vkVoidFunction(layer_EnumerateDeviceExtensionProperties);
        if (string_eq(pName, "vkEnumerateInstanceLayerProperties")) return to_vkVoidFunction(layer_EnumerateInstanceLayerProperties);
        if (string_eq(pName, "vkEnumerateDeviceLayerProperties")) return to_vkVoidFunction(layer_EnumerateDeviceLayerProperties);
        if (string_eq(pName, "vkGetDeviceQueue")) return to_vkVoidFunction(layer_GetDeviceQueue);
        if (string_eq(pName, "vkQueueSubmit")) return to_vkVoidFunction(layer_QueueSubmit);
        if (string_eq(pName, "vkQueueWaitIdle")) return to_vkVoidFunction(layer_QueueWaitIdle);
        if (string_eq(pName, "vkDeviceWaitIdle")) return to_vkVoidFunction(layer_DeviceWaitIdle);
        if (string_eq(pName, "vkAllocateMemory")) return to_vkVoidFunction(layer_AllocateMemory);
        if (string_eq(pName, "vkFreeMemory")) return to_vkVoidFunction(layer_FreeMemory);
        if (string_eq(pName, "vkMapMemory")) return to_vkVoidFunction(layer_MapMemory);
        if (string_eq(pName, "vkUnmapMemory")) return to_vkVoidFunction(layer_UnmapMemory);
        if (string_eq(pName, "vkFlushMappedMemoryRanges")) return to_vkVoidFunction(layer_FlushMappedMemoryRanges);
        if (string_eq(pName, "vkInvalidateMappedMemoryRanges")) return to_vkVoidFunction(layer_InvalidateMappedMemoryRanges);
        if (string_eq(pName, "vkGetDeviceMemoryCommitment")) return to_vkVoidFunction(layer_GetDeviceMemoryCommitment);
        if (string_eq(pName, "vkBindBufferMemory")) return to_vkVoidFunction(layer_BindBufferMemory);
        if (string_eq(pName, "vkBindImageMemory")) return to_vkVoidFunction(layer_BindImageMemory);
        if (string_eq(pName, "vkGetBufferMemoryRequirements")) return to_vkVoidFunction(layer_GetBufferMemoryRequirements);
        if (string_eq(pName, "vkGetImageMemoryRequirements")) return to_vkVoidFunction(layer_GetImageMemoryRequirements);
        if (string_eq(pName, "vkGetImageSparseMemoryRequirements")) return to_vkVoidFunction(layer_GetImageSparseMemoryRequirements);
        if (string_eq(pName, "vkGetPhysicalDeviceSparseImageFormatProperties")) return to_vkVoidFunction(layer_GetPhysicalDeviceSparseImageFormatProperties);
        if (string_eq(pName, "vkQueueBindSparse")) return to_vkVoidFunction(layer_QueueBindSparse);
        if (string_eq(pName, "vkCreateFence")) return to_vkVoidFunction(layer_CreateFence);
        if (string_eq(pName, "vkDestroyFence")) return to_vkVoidFunction(layer_DestroyFence);
        if (string_eq(pName, "vkResetFences")) return to_vkVoidFunction(layer_ResetFences);
        if (string_eq(pName, "vkGetFenceStatus")) return to_vkVoidFunction(layer_GetFenceStatus);
        if (string_eq(pName, "vkWaitForFences")) return to_vkVoidFunction(layer_WaitForFences);
        if (string_eq(pName, "vkCreateSemaphore")) return to_vkVoidFunction(layer_CreateSemaphore);
        if (string_eq(pName, "vkDestroySemaphore")) return to_vkVoidFunction(layer_DestroySemaphore);
        if (string_eq(pName, "vkCreateEvent")) return to_vkVoidFunction(layer_CreateEvent);
        if (string_eq(pName, "vkDestroyEvent")) return to_vkVoidFunction(layer_DestroyEvent);
        if (string_eq(pName, "vkGetEventStatus")) return to_vkVoidFunction(layer_GetEventStatus);
        if (string_eq(pName, "vkSetEvent")) return to_vkVoidFunction(layer_SetEvent);
        if (string_eq(pName, "vkResetEvent")) return to_vkVoidFunction(layer_ResetEvent);
        if (string_eq(pName, "vkCreateQueryPool")) return to_vkVoidFunction(layer_CreateQueryPool);
        if (string_eq(pName, "vkDestroyQueryPool")) return to_vkVoidFunction(layer_DestroyQueryPool);
        if (string_eq(pName, "vkGetQueryPoolResults")) return to_vkVoidFunction(layer_GetQueryPoolResults);
        if (string_eq(pName, "vkCreateBuffer")) return to_vkVoidFunction(layer_CreateBuffer);
        if (string_eq(pName, "vkDestroyBuffer")) return to_vkVoidFunction(layer_DestroyBuffer);
        if (string_eq(pName, "vkCreateBufferView")) return to_vkVoidFunction(layer_CreateBufferView);
        if (string_eq(pName, "vkDestroyBufferView")) return to_vkVoidFunction(layer_DestroyBufferView);
        if (string_eq(pName, "vkCreateImage")) return to_vkVoidFunction(layer_CreateImage);
        if (string_eq(pName, "vkDestroyImage")) return to_vkVoidFunction(layer_DestroyImage);
        if (string_eq(pName, "vkGetImageSubresourceLayout")) return to_vkVoidFunction(layer_GetImageSubresourceLayout);
        if (string_eq(pName, "vkCreateImageView")) return to_vkVoidFunction(layer_CreateImageView);
        if (string_eq(pName, "vkDestroyImageView")) return to_vkVoidFunction(layer_DestroyImageView);
        if (string_eq(pName, "vkCreateShaderModule")) return to_vkVoidFunction(layer_CreateShaderModule);
        if (string_eq(pName, "vkDestroyShaderModule")) return to_vkVoidFunction(layer_DestroyShaderModule);
        if (string_eq(pName, "vkCreatePipelineCache")) return to_vkVoidFunction(layer_CreatePipelineCache);
        if (string_eq(pName, "vkDestroyPipelineCache")) return to_vkVoidFunction(layer_DestroyPipelineCache);
        if (string_eq(pName, "vkGetPipelineCacheData")) return to_vkVoidFunction(layer_GetPipelineCacheData);
        if (string_eq(pName, "vkMergePipelineCaches")) return to_vkVoidFunction(layer_MergePipelineCaches);
        if (string_eq(pName, "vkCreateGraphicsPipelines")) return to_vkVoidFunction(layer_CreateGraphicsPipelines);
        if (string_eq(pName, "vkCreateComputePipelines")) return to_vkVoidFunction(layer_CreateComputePipelines);
        if (string_eq(pName, "vkDestroyPipeline")) return to_vkVoidFunction(layer_DestroyPipeline);
        if (string_eq(pName, "vkCreatePipelineLayout")) return to_vkVoidFunction(layer_CreatePipelineLayout);
        if (string_eq(pName, "vkDestroyPipelineLayout")) return to_vkVoidFunction(layer_DestroyPipelineLayout);
        if (string_eq(pName, "vkCreateSampler")) return to_vkVoidFunction(layer_CreateSampler);
        if (string_eq(pName, "vkDestroySampler")) return to_vkVoidFunction(layer_DestroySampler);
        if (string_eq(pName, "vkCreateDescriptorSetLayout")) return to_vkVoidFunction(layer_CreateDescriptorSetLayout);
        if (string_eq(pName, "vkDestroyDescriptorSetLayout")) return to_vkVoidFunction(layer_DestroyDescriptorSetLayout);
        if (string_eq(pName, "vkCreateDescriptorPool")) return to_vkVoidFunction(layer_CreateDescriptorPool);
        if (string_eq(pName, "vkDestroyDescriptorPool")) return to_vkVoidFunction(layer_DestroyDescriptorPool);
        if (string_eq(pName, "vkResetDescriptorPool")) return to_vkVoidFunction(layer_ResetDescriptorPool);
        if (string_eq(pName, "vkAllocateDescriptorSets")) return to_vkVoidFunction(layer_AllocateDescriptorSets);
        if (string_eq(pName, "vkFreeDescriptorSets")) return to_vkVoidFunction(layer_FreeDescriptorSets);
        if (string_eq(pName, "vkUpdateDescriptorSets")) return to_vkVoidFunction(layer_UpdateDescriptorSets);
        if (string_eq(pName, "vkCreateFramebuffer")) return to_vkVoidFunction(layer_CreateFramebuffer);
        if (string_eq(pName, "vkDestroyFramebuffer")) return to_vkVoidFunction(layer_DestroyFramebuffer);
        if (string_eq(pName, "vkCreateRenderPass")) return to_vkVoidFunction(layer_CreateRenderPass);
        if (string_eq(pName, "vkDestroyRenderPass")) return to_vkVoidFunction(layer_DestroyRenderPass);
        if (string_eq(pName, "vkGetRenderAreaGranularity")) return to_vkVoidFunction(layer_GetRenderAreaGranularity);
        if (string_eq(pName, "vkCreateCommandPool")) return to_vkVoidFunction(layer_CreateCommandPool);
        if (string_eq(pName, "vkDestroyCommandPool")) return to_vkVoidFunction(layer_DestroyCommandPool);
        if (string_eq(pName, "vkResetCommandPool")) return to_vkVoidFunction(layer_ResetCommandPool);
        if (string_eq(pName, "vkAllocateCommandBuffers")) return to_vkVoidFunction(layer_AllocateCommandBuffers);
        if (string_eq(pName, "vkFreeCommandBuffers")) return to_vkVoidFunction(layer_FreeCommandBuffers);
        if (string_eq(pName, "vkBeginCommandBuffer")) return to_vkVoidFunction(layer_BeginCommandBuffer);
        if (string_eq(pName, "vkEndCommandBuffer")) return to_vkVoidFunction(layer_EndCommandBuffer);
        if (string_eq(pName, "vkResetCommandBuffer")) return to_vkVoidFunction(layer_ResetCommandBuffer);
        if (string_eq(pName, "vkCmdBindPipeline")) return to_vkVoidFunction(layer_CmdBindPipeline);
        if (string_eq(pName, "vkCmdSetViewport")) return to_vkVoidFunction(layer_CmdSetViewport);
        if (string_eq(pName, "vkCmdSetScissor")) return to_vkVoidFunction(layer_CmdSetScissor);
        if (string_eq(pName, "vkCmdSetLineWidth")) return to_vkVoidFunction(layer_CmdSetLineWidth);
        if (string_eq(pName, "vkCmdSetDepthBias")) return to_vkVoidFunction(layer_CmdSetDepthBias);
        if (string_eq(pName, "vkCmdSetBlendConstants")) return to_vkVoidFunction(layer_CmdSetBlendConstants);
        if (string_eq(pName, "vkCmdSetDepthBounds")) return to_vkVoidFunction(layer_CmdSetDepthBounds);
        if (string_eq(pName, "vkCmdSetStencilCompareMask")) return to_vkVoidFunction(layer_CmdSetStencilCompareMask);
        if (string_eq(pName, "vkCmdSetStencilWriteMask")) return to_vkVoidFunction(layer_CmdSetStencilWriteMask);
        if (string_eq(pName, "vkCmdSetStencilReference")) return to_vkVoidFunction(layer_CmdSetStencilReference);
        if (string_eq(pName, "vkCmdBindDescriptorSets")) return to_vkVoidFunction(layer_CmdBindDescriptorSets);
        if (string_eq(pName, "vkCmdBindIndexBuffer")) return to_vkVoidFunction(layer_CmdBindIndexBuffer);
        if (string_eq(pName, "vkCmdBindVertexBuffers")) return to_vkVoidFunction(layer_CmdBindVertexBuffers);
        if (string_eq(pName, "vkCmdDraw")) return to_vkVoidFunction(layer_CmdDraw);
        if (string_eq(pName, "vkCmdDrawIndexed")) return to_vkVoidFunction(layer_CmdDrawIndexed);
        if (string_eq(pName, "vkCmdDrawIndirect")) return to_vkVoidFunction(layer_CmdDrawIndirect);
        if (string_eq(pName, "vkCmdDrawIndexedIndirect")) return to_vkVoidFunction(layer_CmdDrawIndexedIndirect);
        if (string_eq(pName, "vkCmdDispatch")) return to_vkVoidFunction(layer_CmdDispatch);
        if (string_eq(pName, "vkCmdDispatchIndirect")) return to_vkVoidFunction(layer_CmdDispatchIndirect);
        if (string_eq(pName, "vkCmdCopyBuffer")) return to_vkVoidFunction(layer_CmdCopyBuffer);
        if (string_eq(pName, "vkCmdCopyImage")) return to_vkVoidFunction(layer_CmdCopyImage);
        if (string_eq(pName, "vkCmdBlitImage")) return to_vkVoidFunction(layer_CmdBlitImage);
        if (string_eq(pName, "vkCmdCopyBufferToImage")) return to_vkVoidFunction(layer_CmdCopyBufferToImage);
        if (string_eq(pName, "vkCmdCopyImageToBuffer")) return to_vkVoidFunction(layer_CmdCopyImageToBuffer);
        if (string_eq(pName, "vkCmdUpdateBuffer")) return to_vkVoidFunction(layer_CmdUpdateBuffer);
        if (string_eq(pName, "vkCmdFillBuffer")) return to_vkVoidFunction(layer_CmdFillBuffer);
        if (string_eq(pName, "vkCmdClearColorImage")) return to_vkVoidFunction(layer_CmdClearColorImage);
        if (string_eq(pName, "vkCmdClearDepthStencilImage")) return to_vkVoidFunction(layer_CmdClearDepthStencilImage);
        if (string_eq(pName, "vkCmdClearAttachments")) return to_vkVoidFunction(layer_CmdClearAttachments);
        if (string_eq(pName, "vkCmdResolveImage")) return to_vkVoidFunction(layer_CmdResolveImage);
        if (string_eq(pName, "vkCmdSetEvent")) return to_vkVoidFunction(layer_CmdSetEvent);
        if (string_eq(pName, "vkCmdResetEvent")) return to_vkVoidFunction(layer_CmdResetEvent);
        if (string_eq(pName, "vkCmdWaitEvents")) return to_vkVoidFunction(layer_CmdWaitEvents);
        if (string_eq(pName, "vkCmdPipelineBarrier")) return to_vkVoidFunction(layer_CmdPipelineBarrier);
        if (string_eq(pName, "vkCmdBeginQuery")) return to_vkVoidFunction(layer_CmdBeginQuery);
        if (string_eq(pName, "vkCmdEndQuery")) return to_vkVoidFunction(layer_CmdEndQuery);
        if (string_eq(pName, "vkCmdResetQueryPool")) return to_vkVoidFunction(layer_CmdResetQueryPool);
        if (string_eq(pName, "vkCmdWriteTimestamp")) return to_vkVoidFunction(layer_CmdWriteTimestamp);
        if (string_eq(pName, "vkCmdCopyQueryPoolResults")) return to_vkVoidFunction(layer_CmdCopyQueryPoolResults);
        if (string_eq(pName, "vkCmdPushConstants")) return to_vkVoidFunction(layer_CmdPushConstants);
        if (string_eq(pName, "vkCmdBeginRenderPass")) return to_vkVoidFunction(layer_CmdBeginRenderPass);
        if (string_eq(pName, "vkCmdNextSubpass")) return to_vkVoidFunction(layer_CmdNextSubpass);
        if (string_eq(pName, "vkCmdEndRenderPass")) return to_vkVoidFunction(layer_CmdEndRenderPass);
        if (string_eq(pName, "vkCmdExecuteCommands")) return to_vkVoidFunction(layer_CmdExecuteCommands);
    } // End Core 1.0

    // ----- VK_VERSION_1_1
    if (layer.enabled_instance_minor >= 1) {
        if (string_eq(pName, "vkEnumerateInstanceVersion")) return to_vkVoidFunction(layer_EnumerateInstanceVersion);
        if (string_eq(pName, "vkBindBufferMemory2")) return to_vkVoidFunction(layer_BindBufferMemory2);
        if (string_eq(pName, "vkBindImageMemory2")) return to_vkVoidFunction(layer_BindImageMemory2);
        if (string_eq(pName, "vkGetDeviceGroupPeerMemoryFeatures")) return to_vkVoidFunction(layer_GetDeviceGroupPeerMemoryFeatures);
        if (string_eq(pName, "vkCmdSetDeviceMask")) return to_vkVoidFunction(layer_CmdSetDeviceMask);
        if (string_eq(pName, "vkCmdDispatchBase")) return to_vkVoidFunction(layer_CmdDispatchBase);
        if (string_eq(pName, "vkEnumeratePhysicalDeviceGroups")) return to_vkVoidFunction(layer_EnumeratePhysicalDeviceGroups);
        if (string_eq(pName, "vkGetImageMemoryRequirements2")) return to_vkVoidFunction(layer_GetImageMemoryRequirements2);
        if (string_eq(pName, "vkGetBufferMemoryRequirements2")) return to_vkVoidFunction(layer_GetBufferMemoryRequirements2);
        if (string_eq(pName, "vkGetImageSparseMemoryRequirements2")) return to_vkVoidFunction(layer_GetImageSparseMemoryRequirements2);
        if (string_eq(pName, "vkGetPhysicalDeviceFeatures2")) return to_vkVoidFunction(layer_GetPhysicalDeviceFeatures2);
        if (string_eq(pName, "vkGetPhysicalDeviceProperties2")) return to_vkVoidFunction(layer_GetPhysicalDeviceProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceFormatProperties2")) return to_vkVoidFunction(layer_GetPhysicalDeviceFormatProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceImageFormatProperties2")) return to_vkVoidFunction(layer_GetPhysicalDeviceImageFormatProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyProperties2")) return to_vkVoidFunction(layer_GetPhysicalDeviceQueueFamilyProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceMemoryProperties2")) return to_vkVoidFunction(layer_GetPhysicalDeviceMemoryProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceSparseImageFormatProperties2")) return to_vkVoidFunction(layer_GetPhysicalDeviceSparseImageFormatProperties2);
        if (string_eq(pName, "vkTrimCommandPool")) return to_vkVoidFunction(layer_TrimCommandPool);
        if (string_eq(pName, "vkGetDeviceQueue2")) return to_vkVoidFunction(layer_GetDeviceQueue2);
        if (string_eq(pName, "vkCreateSamplerYcbcrConversion")) return to_vkVoidFunction(layer_CreateSamplerYcbcrConversion);
        if (string_eq(pName, "vkDestroySamplerYcbcrConversion")) return to_vkVoidFunction(layer_DestroySamplerYcbcrConversion);
        if (string_eq(pName, "vkCreateDescriptorUpdateTemplate")) return to_vkVoidFunction(layer_CreateDescriptorUpdateTemplate);
        if (string_eq(pName, "vkDestroyDescriptorUpdateTemplate")) return to_vkVoidFunction(layer_DestroyDescriptorUpdateTemplate);
        if (string_eq(pName, "vkUpdateDescriptorSetWithTemplate")) return to_vkVoidFunction(layer_UpdateDescriptorSetWithTemplate);
        if (string_eq(pName, "vkGetPhysicalDeviceExternalBufferProperties")) return to_vkVoidFunction(layer_GetPhysicalDeviceExternalBufferProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceExternalFenceProperties")) return to_vkVoidFunction(layer_GetPhysicalDeviceExternalFenceProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceExternalSemaphoreProperties")) return to_vkVoidFunction(layer_GetPhysicalDeviceExternalSemaphoreProperties);
        if (string_eq(pName, "vkGetDescriptorSetLayoutSupport")) return to_vkVoidFunction(layer_GetDescriptorSetLayoutSupport);
    } // End Core 1.1

    // ----- VK_VERSION_1_2
    if (layer.enabled_instance_minor >= 2) {
        if (string_eq(pName, "vkCmdDrawIndirectCount")) return to_vkVoidFunction(layer_CmdDrawIndirectCount);
        if (string_eq(pName, "vkCmdDrawIndexedIndirectCount")) return to_vkVoidFunction(layer_CmdDrawIndexedIndirectCount);
        if (string_eq(pName, "vkCreateRenderPass2")) return to_vkVoidFunction(layer_CreateRenderPass2);
        if (string_eq(pName, "vkCmdBeginRenderPass2")) return to_vkVoidFunction(layer_CmdBeginRenderPass2);
        if (string_eq(pName, "vkCmdNextSubpass2")) return to_vkVoidFunction(layer_CmdNextSubpass2);
        if (string_eq(pName, "vkCmdEndRenderPass2")) return to_vkVoidFunction(layer_CmdEndRenderPass2);
        if (string_eq(pName, "vkResetQueryPool")) return to_vkVoidFunction(layer_ResetQueryPool);
        if (string_eq(pName, "vkGetSemaphoreCounterValue")) return to_vkVoidFunction(layer_GetSemaphoreCounterValue);
        if (string_eq(pName, "vkWaitSemaphores")) return to_vkVoidFunction(layer_WaitSemaphores);
        if (string_eq(pName, "vkSignalSemaphore")) return to_vkVoidFunction(layer_SignalSemaphore);
        if (string_eq(pName, "vkGetBufferDeviceAddress")) return to_vkVoidFunction(layer_GetBufferDeviceAddress);
        if (string_eq(pName, "vkGetBufferOpaqueCaptureAddress")) return to_vkVoidFunction(layer_GetBufferOpaqueCaptureAddress);
        if (string_eq(pName, "vkGetDeviceMemoryOpaqueCaptureAddress")) return to_vkVoidFunction(layer_GetDeviceMemoryOpaqueCaptureAddress);
    } // End Core 1.2

    // ----- VK_VERSION_1_3
    if (layer.enabled_instance_minor >= 3) {
        if (string_eq(pName, "vkGetPhysicalDeviceToolProperties")) return to_vkVoidFunction(layer_GetPhysicalDeviceToolProperties);
        if (string_eq(pName, "vkCreatePrivateDataSlot")) return to_vkVoidFunction(layer_CreatePrivateDataSlot);
        if (string_eq(pName, "vkDestroyPrivateDataSlot")) return to_vkVoidFunction(layer_DestroyPrivateDataSlot);
        if (string_eq(pName, "vkSetPrivateData")) return to_vkVoidFunction(layer_SetPrivateData);
        if (string_eq(pName, "vkGetPrivateData")) return to_vkVoidFunction(layer_GetPrivateData);
        if (string_eq(pName, "vkCmdSetEvent2")) return to_vkVoidFunction(layer_CmdSetEvent2);
        if (string_eq(pName, "vkCmdResetEvent2")) return to_vkVoidFunction(layer_CmdResetEvent2);
        if (string_eq(pName, "vkCmdWaitEvents2")) return to_vkVoidFunction(layer_CmdWaitEvents2);
        if (string_eq(pName, "vkCmdPipelineBarrier2")) return to_vkVoidFunction(layer_CmdPipelineBarrier2);
        if (string_eq(pName, "vkCmdWriteTimestamp2")) return to_vkVoidFunction(layer_CmdWriteTimestamp2);
        if (string_eq(pName, "vkQueueSubmit2")) return to_vkVoidFunction(layer_QueueSubmit2);
        if (string_eq(pName, "vkCmdCopyBuffer2")) return to_vkVoidFunction(layer_CmdCopyBuffer2);
        if (string_eq(pName, "vkCmdCopyImage2")) return to_vkVoidFunction(layer_CmdCopyImage2);
        if (string_eq(pName, "vkCmdCopyBufferToImage2")) return to_vkVoidFunction(layer_CmdCopyBufferToImage2);
        if (string_eq(pName, "vkCmdCopyImageToBuffer2")) return to_vkVoidFunction(layer_CmdCopyImageToBuffer2);
        if (string_eq(pName, "vkCmdBlitImage2")) return to_vkVoidFunction(layer_CmdBlitImage2);
        if (string_eq(pName, "vkCmdResolveImage2")) return to_vkVoidFunction(layer_CmdResolveImage2);
        if (string_eq(pName, "vkCmdBeginRendering")) return to_vkVoidFunction(layer_CmdBeginRendering);
        if (string_eq(pName, "vkCmdEndRendering")) return to_vkVoidFunction(layer_CmdEndRendering);
        if (string_eq(pName, "vkCmdSetCullMode")) return to_vkVoidFunction(layer_CmdSetCullMode);
        if (string_eq(pName, "vkCmdSetFrontFace")) return to_vkVoidFunction(layer_CmdSetFrontFace);
        if (string_eq(pName, "vkCmdSetPrimitiveTopology")) return to_vkVoidFunction(layer_CmdSetPrimitiveTopology);
        if (string_eq(pName, "vkCmdSetViewportWithCount")) return to_vkVoidFunction(layer_CmdSetViewportWithCount);
        if (string_eq(pName, "vkCmdSetScissorWithCount")) return to_vkVoidFunction(layer_CmdSetScissorWithCount);
        if (string_eq(pName, "vkCmdBindVertexBuffers2")) return to_vkVoidFunction(layer_CmdBindVertexBuffers2);
        if (string_eq(pName, "vkCmdSetDepthTestEnable")) return to_vkVoidFunction(layer_CmdSetDepthTestEnable);
        if (string_eq(pName, "vkCmdSetDepthWriteEnable")) return to_vkVoidFunction(layer_CmdSetDepthWriteEnable);
        if (string_eq(pName, "vkCmdSetDepthCompareOp")) return to_vkVoidFunction(layer_CmdSetDepthCompareOp);
        if (string_eq(pName, "vkCmdSetDepthBoundsTestEnable")) return to_vkVoidFunction(layer_CmdSetDepthBoundsTestEnable);
        if (string_eq(pName, "vkCmdSetStencilTestEnable")) return to_vkVoidFunction(layer_CmdSetStencilTestEnable);
        if (string_eq(pName, "vkCmdSetStencilOp")) return to_vkVoidFunction(layer_CmdSetStencilOp);
        if (string_eq(pName, "vkCmdSetRasterizerDiscardEnable")) return to_vkVoidFunction(layer_CmdSetRasterizerDiscardEnable);
        if (string_eq(pName, "vkCmdSetDepthBiasEnable")) return to_vkVoidFunction(layer_CmdSetDepthBiasEnable);
        if (string_eq(pName, "vkCmdSetPrimitiveRestartEnable")) return to_vkVoidFunction(layer_CmdSetPrimitiveRestartEnable);
        if (string_eq(pName, "vkGetDeviceBufferMemoryRequirements")) return to_vkVoidFunction(layer_GetDeviceBufferMemoryRequirements);
        if (string_eq(pName, "vkGetDeviceImageMemoryRequirements")) return to_vkVoidFunction(layer_GetDeviceImageMemoryRequirements);
        if (string_eq(pName, "vkGetDeviceImageSparseMemoryRequirements")) return to_vkVoidFunction(layer_GetDeviceImageSparseMemoryRequirements);
    } // End Core 1.3

    // ----- VK_KHR_surface
    if (IsInstanceExtensionEnabled("VK_KHR_surface")) {
        if (string_eq(pName, "vkDestroySurfaceKHR")) return to_vkVoidFunction(layer_DestroySurfaceKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceSurfaceSupportKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceSurfaceSupportKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceSurfaceCapabilitiesKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceSurfaceFormatsKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceSurfaceFormatsKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceSurfacePresentModesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceSurfacePresentModesKHR);
    } // VK_KHR_surface

    // ----- VK_KHR_swapchain
    if (string_eq(pName, "vkCreateSwapchainKHR")) return to_vkVoidFunction(layer_CreateSwapchainKHR);
    if (string_eq(pName, "vkDestroySwapchainKHR")) return to_vkVoidFunction(layer_DestroySwapchainKHR);
    if (string_eq(pName, "vkGetSwapchainImagesKHR")) return to_vkVoidFunction(layer_GetSwapchainImagesKHR);
    if (string_eq(pName, "vkAcquireNextImageKHR")) return to_vkVoidFunction(layer_AcquireNextImageKHR);
    if (string_eq(pName, "vkQueuePresentKHR")) return to_vkVoidFunction(layer_QueuePresentKHR);
    if (string_eq(pName, "vkGetDeviceGroupPresentCapabilitiesKHR")) return to_vkVoidFunction(layer_GetDeviceGroupPresentCapabilitiesKHR);
    if (string_eq(pName, "vkGetDeviceGroupSurfacePresentModesKHR")) return to_vkVoidFunction(layer_GetDeviceGroupSurfacePresentModesKHR);
    if (string_eq(pName, "vkGetPhysicalDevicePresentRectanglesKHR")) return to_vkVoidFunction(layer_GetPhysicalDevicePresentRectanglesKHR);
    if (string_eq(pName, "vkAcquireNextImage2KHR")) return to_vkVoidFunction(layer_AcquireNextImage2KHR);

    // ----- VK_KHR_display
    if (IsInstanceExtensionEnabled("VK_KHR_display")) {
        if (string_eq(pName, "vkGetPhysicalDeviceDisplayPropertiesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceDisplayPropertiesKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceDisplayPlanePropertiesKHR);
        if (string_eq(pName, "vkGetDisplayPlaneSupportedDisplaysKHR")) return to_vkVoidFunction(layer_GetDisplayPlaneSupportedDisplaysKHR);
        if (string_eq(pName, "vkGetDisplayModePropertiesKHR")) return to_vkVoidFunction(layer_GetDisplayModePropertiesKHR);
        if (string_eq(pName, "vkCreateDisplayModeKHR")) return to_vkVoidFunction(layer_CreateDisplayModeKHR);
        if (string_eq(pName, "vkGetDisplayPlaneCapabilitiesKHR")) return to_vkVoidFunction(layer_GetDisplayPlaneCapabilitiesKHR);
        if (string_eq(pName, "vkCreateDisplayPlaneSurfaceKHR")) return to_vkVoidFunction(layer_CreateDisplayPlaneSurfaceKHR);
    } // VK_KHR_display

    // ----- VK_KHR_display_swapchain
    if (string_eq(pName, "vkCreateSharedSwapchainsKHR")) return to_vkVoidFunction(layer_CreateSharedSwapchainsKHR);

    // ----- VK_KHR_xlib_surface
#ifdef VK_USE_PLATFORM_XLIB_KHR
    if (IsInstanceExtensionEnabled("VK_KHR_xlib_surface")) {
        if (string_eq(pName, "vkCreateXlibSurfaceKHR")) return to_vkVoidFunction(layer_CreateXlibSurfaceKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceXlibPresentationSupportKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceXlibPresentationSupportKHR);
    } // VK_KHR_xlib_surface
#endif // VK_USE_PLATFORM_XLIB_KHR

    // ----- VK_KHR_xcb_surface
#ifdef VK_USE_PLATFORM_XCB_KHR
    if (IsInstanceExtensionEnabled("VK_KHR_xcb_surface")) {
        if (string_eq(pName, "vkCreateXcbSurfaceKHR")) return to_vkVoidFunction(layer_CreateXcbSurfaceKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceXcbPresentationSupportKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceXcbPresentationSupportKHR);
    } // VK_KHR_xcb_surface
#endif // VK_USE_PLATFORM_XCB_KHR

    // ----- VK_KHR_wayland_surface
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    if (IsInstanceExtensionEnabled("VK_KHR_wayland_surface")) {
        if (string_eq(pName, "vkCreateWaylandSurfaceKHR")) return to_vkVoidFunction(layer_CreateWaylandSurfaceKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceWaylandPresentationSupportKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceWaylandPresentationSupportKHR);
    } // VK_KHR_wayland_surface
#endif // VK_USE_PLATFORM_WAYLAND_KHR

    // ----- VK_KHR_win32_surface
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (IsInstanceExtensionEnabled("VK_KHR_win32_surface")) {
        if (string_eq(pName, "vkCreateWin32SurfaceKHR")) return to_vkVoidFunction(layer_CreateWin32SurfaceKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceWin32PresentationSupportKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceWin32PresentationSupportKHR);
    } // VK_KHR_win32_surface
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_KHR_video_queue
#ifdef VK_ENABLE_BETA_EXTENSIONS
    if (string_eq(pName, "vkGetPhysicalDeviceVideoCapabilitiesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceVideoCapabilitiesKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceVideoFormatPropertiesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceVideoFormatPropertiesKHR);
    if (string_eq(pName, "vkCreateVideoSessionKHR")) return to_vkVoidFunction(layer_CreateVideoSessionKHR);
    if (string_eq(pName, "vkDestroyVideoSessionKHR")) return to_vkVoidFunction(layer_DestroyVideoSessionKHR);
    if (string_eq(pName, "vkGetVideoSessionMemoryRequirementsKHR")) return to_vkVoidFunction(layer_GetVideoSessionMemoryRequirementsKHR);
    if (string_eq(pName, "vkBindVideoSessionMemoryKHR")) return to_vkVoidFunction(layer_BindVideoSessionMemoryKHR);
    if (string_eq(pName, "vkCreateVideoSessionParametersKHR")) return to_vkVoidFunction(layer_CreateVideoSessionParametersKHR);
    if (string_eq(pName, "vkUpdateVideoSessionParametersKHR")) return to_vkVoidFunction(layer_UpdateVideoSessionParametersKHR);
    if (string_eq(pName, "vkDestroyVideoSessionParametersKHR")) return to_vkVoidFunction(layer_DestroyVideoSessionParametersKHR);
    if (string_eq(pName, "vkCmdBeginVideoCodingKHR")) return to_vkVoidFunction(layer_CmdBeginVideoCodingKHR);
    if (string_eq(pName, "vkCmdEndVideoCodingKHR")) return to_vkVoidFunction(layer_CmdEndVideoCodingKHR);
    if (string_eq(pName, "vkCmdControlVideoCodingKHR")) return to_vkVoidFunction(layer_CmdControlVideoCodingKHR);
#endif // VK_ENABLE_BETA_EXTENSIONS

    // ----- VK_KHR_video_decode_queue
#ifdef VK_ENABLE_BETA_EXTENSIONS
    if (string_eq(pName, "vkCmdDecodeVideoKHR")) return to_vkVoidFunction(layer_CmdDecodeVideoKHR);
#endif // VK_ENABLE_BETA_EXTENSIONS

    // ----- VK_KHR_dynamic_rendering
    if (string_eq(pName, "vkCmdBeginRenderingKHR")) return to_vkVoidFunction(layer_CmdBeginRenderingKHR);
    if (string_eq(pName, "vkCmdEndRenderingKHR")) return to_vkVoidFunction(layer_CmdEndRenderingKHR);

    // ----- VK_KHR_get_physical_device_properties2
    if (IsInstanceExtensionEnabled("VK_KHR_get_physical_device_properties2")) {
        if (string_eq(pName, "vkGetPhysicalDeviceFeatures2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceFeatures2KHR);
        if (string_eq(pName, "vkGetPhysicalDeviceProperties2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceProperties2KHR);
        if (string_eq(pName, "vkGetPhysicalDeviceFormatProperties2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceFormatProperties2KHR);
        if (string_eq(pName, "vkGetPhysicalDeviceImageFormatProperties2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceImageFormatProperties2KHR);
        if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyProperties2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceQueueFamilyProperties2KHR);
        if (string_eq(pName, "vkGetPhysicalDeviceMemoryProperties2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceMemoryProperties2KHR);
        if (string_eq(pName, "vkGetPhysicalDeviceSparseImageFormatProperties2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceSparseImageFormatProperties2KHR);
    } // VK_KHR_get_physical_device_properties2

    // ----- VK_KHR_device_group
    if (string_eq(pName, "vkGetDeviceGroupPeerMemoryFeaturesKHR")) return to_vkVoidFunction(layer_GetDeviceGroupPeerMemoryFeaturesKHR);
    if (string_eq(pName, "vkCmdSetDeviceMaskKHR")) return to_vkVoidFunction(layer_CmdSetDeviceMaskKHR);
    if (string_eq(pName, "vkCmdDispatchBaseKHR")) return to_vkVoidFunction(layer_CmdDispatchBaseKHR);

    // ----- VK_KHR_maintenance1
    if (string_eq(pName, "vkTrimCommandPoolKHR")) return to_vkVoidFunction(layer_TrimCommandPoolKHR);

    // ----- VK_KHR_device_group_creation
    if (IsInstanceExtensionEnabled("VK_KHR_device_group_creation")) {
        if (string_eq(pName, "vkEnumeratePhysicalDeviceGroupsKHR")) return to_vkVoidFunction(layer_EnumeratePhysicalDeviceGroupsKHR);
    } // VK_KHR_device_group_creation

    // ----- VK_KHR_external_memory_capabilities
    if (IsInstanceExtensionEnabled("VK_KHR_external_memory_capabilities")) {
        if (string_eq(pName, "vkGetPhysicalDeviceExternalBufferPropertiesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceExternalBufferPropertiesKHR);
    } // VK_KHR_external_memory_capabilities

    // ----- VK_KHR_external_memory_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (string_eq(pName, "vkGetMemoryWin32HandleKHR")) return to_vkVoidFunction(layer_GetMemoryWin32HandleKHR);
    if (string_eq(pName, "vkGetMemoryWin32HandlePropertiesKHR")) return to_vkVoidFunction(layer_GetMemoryWin32HandlePropertiesKHR);
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_KHR_external_memory_fd
    if (string_eq(pName, "vkGetMemoryFdKHR")) return to_vkVoidFunction(layer_GetMemoryFdKHR);
    if (string_eq(pName, "vkGetMemoryFdPropertiesKHR")) return to_vkVoidFunction(layer_GetMemoryFdPropertiesKHR);

    // ----- VK_KHR_external_semaphore_capabilities
    if (IsInstanceExtensionEnabled("VK_KHR_external_semaphore_capabilities")) {
        if (string_eq(pName, "vkGetPhysicalDeviceExternalSemaphorePropertiesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceExternalSemaphorePropertiesKHR);
    } // VK_KHR_external_semaphore_capabilities

    // ----- VK_KHR_external_semaphore_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (string_eq(pName, "vkImportSemaphoreWin32HandleKHR")) return to_vkVoidFunction(layer_ImportSemaphoreWin32HandleKHR);
    if (string_eq(pName, "vkGetSemaphoreWin32HandleKHR")) return to_vkVoidFunction(layer_GetSemaphoreWin32HandleKHR);
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_KHR_external_semaphore_fd
    if (string_eq(pName, "vkImportSemaphoreFdKHR")) return to_vkVoidFunction(layer_ImportSemaphoreFdKHR);
    if (string_eq(pName, "vkGetSemaphoreFdKHR")) return to_vkVoidFunction(layer_GetSemaphoreFdKHR);

    // ----- VK_KHR_push_descriptor
    if (string_eq(pName, "vkCmdPushDescriptorSetKHR")) return to_vkVoidFunction(layer_CmdPushDescriptorSetKHR);
    if (string_eq(pName, "vkCmdPushDescriptorSetWithTemplateKHR")) return to_vkVoidFunction(layer_CmdPushDescriptorSetWithTemplateKHR);

    // ----- VK_KHR_descriptor_update_template
    if (string_eq(pName, "vkCreateDescriptorUpdateTemplateKHR")) return to_vkVoidFunction(layer_CreateDescriptorUpdateTemplateKHR);
    if (string_eq(pName, "vkDestroyDescriptorUpdateTemplateKHR")) return to_vkVoidFunction(layer_DestroyDescriptorUpdateTemplateKHR);
    if (string_eq(pName, "vkUpdateDescriptorSetWithTemplateKHR")) return to_vkVoidFunction(layer_UpdateDescriptorSetWithTemplateKHR);

    // ----- VK_KHR_create_renderpass2
    if (string_eq(pName, "vkCreateRenderPass2KHR")) return to_vkVoidFunction(layer_CreateRenderPass2KHR);
    if (string_eq(pName, "vkCmdBeginRenderPass2KHR")) return to_vkVoidFunction(layer_CmdBeginRenderPass2KHR);
    if (string_eq(pName, "vkCmdNextSubpass2KHR")) return to_vkVoidFunction(layer_CmdNextSubpass2KHR);
    if (string_eq(pName, "vkCmdEndRenderPass2KHR")) return to_vkVoidFunction(layer_CmdEndRenderPass2KHR);

    // ----- VK_KHR_shared_presentable_image
    if (string_eq(pName, "vkGetSwapchainStatusKHR")) return to_vkVoidFunction(layer_GetSwapchainStatusKHR);

    // ----- VK_KHR_external_fence_capabilities
    if (IsInstanceExtensionEnabled("VK_KHR_external_fence_capabilities")) {
        if (string_eq(pName, "vkGetPhysicalDeviceExternalFencePropertiesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceExternalFencePropertiesKHR);
    } // VK_KHR_external_fence_capabilities

    // ----- VK_KHR_external_fence_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (string_eq(pName, "vkImportFenceWin32HandleKHR")) return to_vkVoidFunction(layer_ImportFenceWin32HandleKHR);
    if (string_eq(pName, "vkGetFenceWin32HandleKHR")) return to_vkVoidFunction(layer_GetFenceWin32HandleKHR);
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_KHR_external_fence_fd
    if (string_eq(pName, "vkImportFenceFdKHR")) return to_vkVoidFunction(layer_ImportFenceFdKHR);
    if (string_eq(pName, "vkGetFenceFdKHR")) return to_vkVoidFunction(layer_GetFenceFdKHR);

    // ----- VK_KHR_performance_query
    if (string_eq(pName, "vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR")) return to_vkVoidFunction(layer_EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR);
    if (string_eq(pName, "vkAcquireProfilingLockKHR")) return to_vkVoidFunction(layer_AcquireProfilingLockKHR);
    if (string_eq(pName, "vkReleaseProfilingLockKHR")) return to_vkVoidFunction(layer_ReleaseProfilingLockKHR);

    // ----- VK_KHR_get_surface_capabilities2
    if (IsInstanceExtensionEnabled("VK_KHR_get_surface_capabilities2")) {
        if (string_eq(pName, "vkGetPhysicalDeviceSurfaceCapabilities2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceSurfaceCapabilities2KHR);
        if (string_eq(pName, "vkGetPhysicalDeviceSurfaceFormats2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceSurfaceFormats2KHR);
    } // VK_KHR_get_surface_capabilities2

    // ----- VK_KHR_get_display_properties2
    if (IsInstanceExtensionEnabled("VK_KHR_get_display_properties2")) {
        if (string_eq(pName, "vkGetPhysicalDeviceDisplayProperties2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceDisplayProperties2KHR);
        if (string_eq(pName, "vkGetPhysicalDeviceDisplayPlaneProperties2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceDisplayPlaneProperties2KHR);
        if (string_eq(pName, "vkGetDisplayModeProperties2KHR")) return to_vkVoidFunction(layer_GetDisplayModeProperties2KHR);
        if (string_eq(pName, "vkGetDisplayPlaneCapabilities2KHR")) return to_vkVoidFunction(layer_GetDisplayPlaneCapabilities2KHR);
    } // VK_KHR_get_display_properties2

    // ----- VK_KHR_get_memory_requirements2
    if (string_eq(pName, "vkGetImageMemoryRequirements2KHR")) return to_vkVoidFunction(layer_GetImageMemoryRequirements2KHR);
    if (string_eq(pName, "vkGetBufferMemoryRequirements2KHR")) return to_vkVoidFunction(layer_GetBufferMemoryRequirements2KHR);
    if (string_eq(pName, "vkGetImageSparseMemoryRequirements2KHR")) return to_vkVoidFunction(layer_GetImageSparseMemoryRequirements2KHR);

    // ----- VK_KHR_sampler_ycbcr_conversion
    if (string_eq(pName, "vkCreateSamplerYcbcrConversionKHR")) return to_vkVoidFunction(layer_CreateSamplerYcbcrConversionKHR);
    if (string_eq(pName, "vkDestroySamplerYcbcrConversionKHR")) return to_vkVoidFunction(layer_DestroySamplerYcbcrConversionKHR);

    // ----- VK_KHR_bind_memory2
    if (string_eq(pName, "vkBindBufferMemory2KHR")) return to_vkVoidFunction(layer_BindBufferMemory2KHR);
    if (string_eq(pName, "vkBindImageMemory2KHR")) return to_vkVoidFunction(layer_BindImageMemory2KHR);

    // ----- VK_KHR_maintenance3
    if (string_eq(pName, "vkGetDescriptorSetLayoutSupportKHR")) return to_vkVoidFunction(layer_GetDescriptorSetLayoutSupportKHR);

    // ----- VK_KHR_draw_indirect_count
    if (string_eq(pName, "vkCmdDrawIndirectCountKHR")) return to_vkVoidFunction(layer_CmdDrawIndirectCountKHR);
    if (string_eq(pName, "vkCmdDrawIndexedIndirectCountKHR")) return to_vkVoidFunction(layer_CmdDrawIndexedIndirectCountKHR);

    // ----- VK_KHR_timeline_semaphore
    if (string_eq(pName, "vkGetSemaphoreCounterValueKHR")) return to_vkVoidFunction(layer_GetSemaphoreCounterValueKHR);
    if (string_eq(pName, "vkWaitSemaphoresKHR")) return to_vkVoidFunction(layer_WaitSemaphoresKHR);
    if (string_eq(pName, "vkSignalSemaphoreKHR")) return to_vkVoidFunction(layer_SignalSemaphoreKHR);

    // ----- VK_KHR_fragment_shading_rate
    if (string_eq(pName, "vkGetPhysicalDeviceFragmentShadingRatesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceFragmentShadingRatesKHR);
    if (string_eq(pName, "vkCmdSetFragmentShadingRateKHR")) return to_vkVoidFunction(layer_CmdSetFragmentShadingRateKHR);

    // ----- VK_KHR_present_wait
    if (string_eq(pName, "vkWaitForPresentKHR")) return to_vkVoidFunction(layer_WaitForPresentKHR);

    // ----- VK_KHR_buffer_device_address
    if (string_eq(pName, "vkGetBufferDeviceAddressKHR")) return to_vkVoidFunction(layer_GetBufferDeviceAddressKHR);
    if (string_eq(pName, "vkGetBufferOpaqueCaptureAddressKHR")) return to_vkVoidFunction(layer_GetBufferOpaqueCaptureAddressKHR);
    if (string_eq(pName, "vkGetDeviceMemoryOpaqueCaptureAddressKHR")) return to_vkVoidFunction(layer_GetDeviceMemoryOpaqueCaptureAddressKHR);

    // ----- VK_KHR_deferred_host_operations
    if (string_eq(pName, "vkCreateDeferredOperationKHR")) return to_vkVoidFunction(layer_CreateDeferredOperationKHR);
    if (string_eq(pName, "vkDestroyDeferredOperationKHR")) return to_vkVoidFunction(layer_DestroyDeferredOperationKHR);
    if (string_eq(pName, "vkGetDeferredOperationMaxConcurrencyKHR")) return to_vkVoidFunction(layer_GetDeferredOperationMaxConcurrencyKHR);
    if (string_eq(pName, "vkGetDeferredOperationResultKHR")) return to_vkVoidFunction(layer_GetDeferredOperationResultKHR);
    if (string_eq(pName, "vkDeferredOperationJoinKHR")) return to_vkVoidFunction(layer_DeferredOperationJoinKHR);

    // ----- VK_KHR_pipeline_executable_properties
    if (string_eq(pName, "vkGetPipelineExecutablePropertiesKHR")) return to_vkVoidFunction(layer_GetPipelineExecutablePropertiesKHR);
    if (string_eq(pName, "vkGetPipelineExecutableStatisticsKHR")) return to_vkVoidFunction(layer_GetPipelineExecutableStatisticsKHR);
    if (string_eq(pName, "vkGetPipelineExecutableInternalRepresentationsKHR")) return to_vkVoidFunction(layer_GetPipelineExecutableInternalRepresentationsKHR);

    // ----- VK_KHR_video_encode_queue
#ifdef VK_ENABLE_BETA_EXTENSIONS
    if (string_eq(pName, "vkCmdEncodeVideoKHR")) return to_vkVoidFunction(layer_CmdEncodeVideoKHR);
#endif // VK_ENABLE_BETA_EXTENSIONS

    // ----- VK_KHR_synchronization2
    if (string_eq(pName, "vkCmdSetEvent2KHR")) return to_vkVoidFunction(layer_CmdSetEvent2KHR);
    if (string_eq(pName, "vkCmdResetEvent2KHR")) return to_vkVoidFunction(layer_CmdResetEvent2KHR);
    if (string_eq(pName, "vkCmdWaitEvents2KHR")) return to_vkVoidFunction(layer_CmdWaitEvents2KHR);
    if (string_eq(pName, "vkCmdPipelineBarrier2KHR")) return to_vkVoidFunction(layer_CmdPipelineBarrier2KHR);
    if (string_eq(pName, "vkCmdWriteTimestamp2KHR")) return to_vkVoidFunction(layer_CmdWriteTimestamp2KHR);
    if (string_eq(pName, "vkQueueSubmit2KHR")) return to_vkVoidFunction(layer_QueueSubmit2KHR);
    if (string_eq(pName, "vkCmdWriteBufferMarker2AMD")) return to_vkVoidFunction(layer_CmdWriteBufferMarker2AMD);
    if (string_eq(pName, "vkGetQueueCheckpointData2NV")) return to_vkVoidFunction(layer_GetQueueCheckpointData2NV);

    // ----- VK_KHR_copy_commands2
    if (string_eq(pName, "vkCmdCopyBuffer2KHR")) return to_vkVoidFunction(layer_CmdCopyBuffer2KHR);
    if (string_eq(pName, "vkCmdCopyImage2KHR")) return to_vkVoidFunction(layer_CmdCopyImage2KHR);
    if (string_eq(pName, "vkCmdCopyBufferToImage2KHR")) return to_vkVoidFunction(layer_CmdCopyBufferToImage2KHR);
    if (string_eq(pName, "vkCmdCopyImageToBuffer2KHR")) return to_vkVoidFunction(layer_CmdCopyImageToBuffer2KHR);
    if (string_eq(pName, "vkCmdBlitImage2KHR")) return to_vkVoidFunction(layer_CmdBlitImage2KHR);
    if (string_eq(pName, "vkCmdResolveImage2KHR")) return to_vkVoidFunction(layer_CmdResolveImage2KHR);

    // ----- VK_KHR_maintenance4
    if (string_eq(pName, "vkGetDeviceBufferMemoryRequirementsKHR")) return to_vkVoidFunction(layer_GetDeviceBufferMemoryRequirementsKHR);
    if (string_eq(pName, "vkGetDeviceImageMemoryRequirementsKHR")) return to_vkVoidFunction(layer_GetDeviceImageMemoryRequirementsKHR);
    if (string_eq(pName, "vkGetDeviceImageSparseMemoryRequirementsKHR")) return to_vkVoidFunction(layer_GetDeviceImageSparseMemoryRequirementsKHR);

    // ----- VK_EXT_debug_report
    if (IsInstanceExtensionEnabled("VK_EXT_debug_report")) {
        if (string_eq(pName, "vkCreateDebugReportCallbackEXT")) return to_vkVoidFunction(layer_CreateDebugReportCallbackEXT);
        if (string_eq(pName, "vkDestroyDebugReportCallbackEXT")) return to_vkVoidFunction(layer_DestroyDebugReportCallbackEXT);
        if (string_eq(pName, "vkDebugReportMessageEXT")) return to_vkVoidFunction(layer_DebugReportMessageEXT);
    } // VK_EXT_debug_report

    // ----- VK_EXT_debug_marker
    if (string_eq(pName, "vkDebugMarkerSetObjectTagEXT")) return to_vkVoidFunction(layer_DebugMarkerSetObjectTagEXT);
    if (string_eq(pName, "vkDebugMarkerSetObjectNameEXT")) return to_vkVoidFunction(layer_DebugMarkerSetObjectNameEXT);
    if (string_eq(pName, "vkCmdDebugMarkerBeginEXT")) return to_vkVoidFunction(layer_CmdDebugMarkerBeginEXT);
    if (string_eq(pName, "vkCmdDebugMarkerEndEXT")) return to_vkVoidFunction(layer_CmdDebugMarkerEndEXT);
    if (string_eq(pName, "vkCmdDebugMarkerInsertEXT")) return to_vkVoidFunction(layer_CmdDebugMarkerInsertEXT);

    // ----- VK_EXT_transform_feedback
    if (string_eq(pName, "vkCmdBindTransformFeedbackBuffersEXT")) return to_vkVoidFunction(layer_CmdBindTransformFeedbackBuffersEXT);
    if (string_eq(pName, "vkCmdBeginTransformFeedbackEXT")) return to_vkVoidFunction(layer_CmdBeginTransformFeedbackEXT);
    if (string_eq(pName, "vkCmdEndTransformFeedbackEXT")) return to_vkVoidFunction(layer_CmdEndTransformFeedbackEXT);
    if (string_eq(pName, "vkCmdBeginQueryIndexedEXT")) return to_vkVoidFunction(layer_CmdBeginQueryIndexedEXT);
    if (string_eq(pName, "vkCmdEndQueryIndexedEXT")) return to_vkVoidFunction(layer_CmdEndQueryIndexedEXT);
    if (string_eq(pName, "vkCmdDrawIndirectByteCountEXT")) return to_vkVoidFunction(layer_CmdDrawIndirectByteCountEXT);

    // ----- VK_NVX_binary_import
    if (string_eq(pName, "vkCreateCuModuleNVX")) return to_vkVoidFunction(layer_CreateCuModuleNVX);
    if (string_eq(pName, "vkCreateCuFunctionNVX")) return to_vkVoidFunction(layer_CreateCuFunctionNVX);
    if (string_eq(pName, "vkDestroyCuModuleNVX")) return to_vkVoidFunction(layer_DestroyCuModuleNVX);
    if (string_eq(pName, "vkDestroyCuFunctionNVX")) return to_vkVoidFunction(layer_DestroyCuFunctionNVX);
    if (string_eq(pName, "vkCmdCuLaunchKernelNVX")) return to_vkVoidFunction(layer_CmdCuLaunchKernelNVX);

    // ----- VK_NVX_image_view_handle
    if (string_eq(pName, "vkGetImageViewHandleNVX")) return to_vkVoidFunction(layer_GetImageViewHandleNVX);
    if (string_eq(pName, "vkGetImageViewAddressNVX")) return to_vkVoidFunction(layer_GetImageViewAddressNVX);

    // ----- VK_AMD_draw_indirect_count
    if (string_eq(pName, "vkCmdDrawIndirectCountAMD")) return to_vkVoidFunction(layer_CmdDrawIndirectCountAMD);
    if (string_eq(pName, "vkCmdDrawIndexedIndirectCountAMD")) return to_vkVoidFunction(layer_CmdDrawIndexedIndirectCountAMD);

    // ----- VK_AMD_shader_info
    if (string_eq(pName, "vkGetShaderInfoAMD")) return to_vkVoidFunction(layer_GetShaderInfoAMD);

    // ----- VK_GGP_stream_descriptor_surface
#ifdef VK_USE_PLATFORM_GGP
    if (IsInstanceExtensionEnabled("VK_GGP_stream_descriptor_surface")) {
        if (string_eq(pName, "vkCreateStreamDescriptorSurfaceGGP")) return to_vkVoidFunction(layer_CreateStreamDescriptorSurfaceGGP);
    } // VK_GGP_stream_descriptor_surface
#endif // VK_USE_PLATFORM_GGP

    // ----- VK_NV_external_memory_capabilities
    if (IsInstanceExtensionEnabled("VK_NV_external_memory_capabilities")) {
        if (string_eq(pName, "vkGetPhysicalDeviceExternalImageFormatPropertiesNV")) return to_vkVoidFunction(layer_GetPhysicalDeviceExternalImageFormatPropertiesNV);
    } // VK_NV_external_memory_capabilities

    // ----- VK_NV_external_memory_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (string_eq(pName, "vkGetMemoryWin32HandleNV")) return to_vkVoidFunction(layer_GetMemoryWin32HandleNV);
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_NN_vi_surface
#ifdef VK_USE_PLATFORM_VI_NN
    if (IsInstanceExtensionEnabled("VK_NN_vi_surface")) {
        if (string_eq(pName, "vkCreateViSurfaceNN")) return to_vkVoidFunction(layer_CreateViSurfaceNN);
    } // VK_NN_vi_surface
#endif // VK_USE_PLATFORM_VI_NN

    // ----- VK_EXT_conditional_rendering
    if (string_eq(pName, "vkCmdBeginConditionalRenderingEXT")) return to_vkVoidFunction(layer_CmdBeginConditionalRenderingEXT);
    if (string_eq(pName, "vkCmdEndConditionalRenderingEXT")) return to_vkVoidFunction(layer_CmdEndConditionalRenderingEXT);

    // ----- VK_NV_clip_space_w_scaling
    if (string_eq(pName, "vkCmdSetViewportWScalingNV")) return to_vkVoidFunction(layer_CmdSetViewportWScalingNV);

    // ----- VK_EXT_direct_mode_display
    if (IsInstanceExtensionEnabled("VK_EXT_direct_mode_display")) {
        if (string_eq(pName, "vkReleaseDisplayEXT")) return to_vkVoidFunction(layer_ReleaseDisplayEXT);
    } // VK_EXT_direct_mode_display

    // ----- VK_EXT_acquire_xlib_display
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
    if (IsInstanceExtensionEnabled("VK_EXT_acquire_xlib_display")) {
        if (string_eq(pName, "vkAcquireXlibDisplayEXT")) return to_vkVoidFunction(layer_AcquireXlibDisplayEXT);
        if (string_eq(pName, "vkGetRandROutputDisplayEXT")) return to_vkVoidFunction(layer_GetRandROutputDisplayEXT);
    } // VK_EXT_acquire_xlib_display
#endif // VK_USE_PLATFORM_XLIB_XRANDR_EXT

    // ----- VK_EXT_display_surface_counter
    if (IsInstanceExtensionEnabled("VK_EXT_display_surface_counter")) {
        if (string_eq(pName, "vkGetPhysicalDeviceSurfaceCapabilities2EXT")) return to_vkVoidFunction(layer_GetPhysicalDeviceSurfaceCapabilities2EXT);
    } // VK_EXT_display_surface_counter

    // ----- VK_EXT_display_control
    if (string_eq(pName, "vkDisplayPowerControlEXT")) return to_vkVoidFunction(layer_DisplayPowerControlEXT);
    if (string_eq(pName, "vkRegisterDeviceEventEXT")) return to_vkVoidFunction(layer_RegisterDeviceEventEXT);
    if (string_eq(pName, "vkRegisterDisplayEventEXT")) return to_vkVoidFunction(layer_RegisterDisplayEventEXT);
    if (string_eq(pName, "vkGetSwapchainCounterEXT")) return to_vkVoidFunction(layer_GetSwapchainCounterEXT);

    // ----- VK_GOOGLE_display_timing
    if (string_eq(pName, "vkGetRefreshCycleDurationGOOGLE")) return to_vkVoidFunction(layer_GetRefreshCycleDurationGOOGLE);
    if (string_eq(pName, "vkGetPastPresentationTimingGOOGLE")) return to_vkVoidFunction(layer_GetPastPresentationTimingGOOGLE);

    // ----- VK_EXT_discard_rectangles
    if (string_eq(pName, "vkCmdSetDiscardRectangleEXT")) return to_vkVoidFunction(layer_CmdSetDiscardRectangleEXT);

    // ----- VK_EXT_hdr_metadata
    if (string_eq(pName, "vkSetHdrMetadataEXT")) return to_vkVoidFunction(layer_SetHdrMetadataEXT);

    // ----- VK_MVK_ios_surface
#ifdef VK_USE_PLATFORM_IOS_MVK
    if (IsInstanceExtensionEnabled("VK_MVK_ios_surface")) {
        if (string_eq(pName, "vkCreateIOSSurfaceMVK")) return to_vkVoidFunction(layer_CreateIOSSurfaceMVK);
    } // VK_MVK_ios_surface
#endif // VK_USE_PLATFORM_IOS_MVK

    // ----- VK_MVK_macos_surface
#ifdef VK_USE_PLATFORM_MACOS_MVK
    if (IsInstanceExtensionEnabled("VK_MVK_macos_surface")) {
        if (string_eq(pName, "vkCreateMacOSSurfaceMVK")) return to_vkVoidFunction(layer_CreateMacOSSurfaceMVK);
    } // VK_MVK_macos_surface
#endif // VK_USE_PLATFORM_MACOS_MVK

    // ----- VK_EXT_debug_utils
    if (IsInstanceExtensionEnabled("VK_EXT_debug_utils")) {
        if (string_eq(pName, "vkSetDebugUtilsObjectNameEXT")) return to_vkVoidFunction(layer_SetDebugUtilsObjectNameEXT);
        if (string_eq(pName, "vkSetDebugUtilsObjectTagEXT")) return to_vkVoidFunction(layer_SetDebugUtilsObjectTagEXT);
        if (string_eq(pName, "vkQueueBeginDebugUtilsLabelEXT")) return to_vkVoidFunction(layer_QueueBeginDebugUtilsLabelEXT);
        if (string_eq(pName, "vkQueueEndDebugUtilsLabelEXT")) return to_vkVoidFunction(layer_QueueEndDebugUtilsLabelEXT);
        if (string_eq(pName, "vkQueueInsertDebugUtilsLabelEXT")) return to_vkVoidFunction(layer_QueueInsertDebugUtilsLabelEXT);
        if (string_eq(pName, "vkCmdBeginDebugUtilsLabelEXT")) return to_vkVoidFunction(layer_CmdBeginDebugUtilsLabelEXT);
        if (string_eq(pName, "vkCmdEndDebugUtilsLabelEXT")) return to_vkVoidFunction(layer_CmdEndDebugUtilsLabelEXT);
        if (string_eq(pName, "vkCmdInsertDebugUtilsLabelEXT")) return to_vkVoidFunction(layer_CmdInsertDebugUtilsLabelEXT);
        if (string_eq(pName, "vkCreateDebugUtilsMessengerEXT")) return to_vkVoidFunction(layer_CreateDebugUtilsMessengerEXT);
        if (string_eq(pName, "vkDestroyDebugUtilsMessengerEXT")) return to_vkVoidFunction(layer_DestroyDebugUtilsMessengerEXT);
        if (string_eq(pName, "vkSubmitDebugUtilsMessageEXT")) return to_vkVoidFunction(layer_SubmitDebugUtilsMessageEXT);
    } // VK_EXT_debug_utils

    // ----- VK_EXT_sample_locations
    if (string_eq(pName, "vkCmdSetSampleLocationsEXT")) return to_vkVoidFunction(layer_CmdSetSampleLocationsEXT);
    if (string_eq(pName, "vkGetPhysicalDeviceMultisamplePropertiesEXT")) return to_vkVoidFunction(layer_GetPhysicalDeviceMultisamplePropertiesEXT);

    // ----- VK_EXT_image_drm_format_modifier
    if (string_eq(pName, "vkGetImageDrmFormatModifierPropertiesEXT")) return to_vkVoidFunction(layer_GetImageDrmFormatModifierPropertiesEXT);

    // ----- VK_EXT_validation_cache
    if (string_eq(pName, "vkCreateValidationCacheEXT")) return to_vkVoidFunction(layer_CreateValidationCacheEXT);
    if (string_eq(pName, "vkDestroyValidationCacheEXT")) return to_vkVoidFunction(layer_DestroyValidationCacheEXT);
    if (string_eq(pName, "vkMergeValidationCachesEXT")) return to_vkVoidFunction(layer_MergeValidationCachesEXT);
    if (string_eq(pName, "vkGetValidationCacheDataEXT")) return to_vkVoidFunction(layer_GetValidationCacheDataEXT);

    // ----- VK_NV_shading_rate_image
    if (string_eq(pName, "vkCmdBindShadingRateImageNV")) return to_vkVoidFunction(layer_CmdBindShadingRateImageNV);
    if (string_eq(pName, "vkCmdSetViewportShadingRatePaletteNV")) return to_vkVoidFunction(layer_CmdSetViewportShadingRatePaletteNV);
    if (string_eq(pName, "vkCmdSetCoarseSampleOrderNV")) return to_vkVoidFunction(layer_CmdSetCoarseSampleOrderNV);

    // ----- VK_NV_ray_tracing
    if (string_eq(pName, "vkCreateAccelerationStructureNV")) return to_vkVoidFunction(layer_CreateAccelerationStructureNV);
    if (string_eq(pName, "vkDestroyAccelerationStructureNV")) return to_vkVoidFunction(layer_DestroyAccelerationStructureNV);
    if (string_eq(pName, "vkGetAccelerationStructureMemoryRequirementsNV")) return to_vkVoidFunction(layer_GetAccelerationStructureMemoryRequirementsNV);
    if (string_eq(pName, "vkBindAccelerationStructureMemoryNV")) return to_vkVoidFunction(layer_BindAccelerationStructureMemoryNV);
    if (string_eq(pName, "vkCmdBuildAccelerationStructureNV")) return to_vkVoidFunction(layer_CmdBuildAccelerationStructureNV);
    if (string_eq(pName, "vkCmdCopyAccelerationStructureNV")) return to_vkVoidFunction(layer_CmdCopyAccelerationStructureNV);
    if (string_eq(pName, "vkCmdTraceRaysNV")) return to_vkVoidFunction(layer_CmdTraceRaysNV);
    if (string_eq(pName, "vkCreateRayTracingPipelinesNV")) return to_vkVoidFunction(layer_CreateRayTracingPipelinesNV);
    if (string_eq(pName, "vkGetRayTracingShaderGroupHandlesNV")) return to_vkVoidFunction(layer_GetRayTracingShaderGroupHandlesNV);
    if (string_eq(pName, "vkGetAccelerationStructureHandleNV")) return to_vkVoidFunction(layer_GetAccelerationStructureHandleNV);
    if (string_eq(pName, "vkCmdWriteAccelerationStructuresPropertiesNV")) return to_vkVoidFunction(layer_CmdWriteAccelerationStructuresPropertiesNV);
    if (string_eq(pName, "vkCompileDeferredNV")) return to_vkVoidFunction(layer_CompileDeferredNV);

    // ----- VK_EXT_external_memory_host
    if (string_eq(pName, "vkGetMemoryHostPointerPropertiesEXT")) return to_vkVoidFunction(layer_GetMemoryHostPointerPropertiesEXT);

    // ----- VK_AMD_buffer_marker
    if (string_eq(pName, "vkCmdWriteBufferMarkerAMD")) return to_vkVoidFunction(layer_CmdWriteBufferMarkerAMD);

    // ----- VK_EXT_calibrated_timestamps
    if (string_eq(pName, "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT")) return to_vkVoidFunction(layer_GetPhysicalDeviceCalibrateableTimeDomainsEXT);
    if (string_eq(pName, "vkGetCalibratedTimestampsEXT")) return to_vkVoidFunction(layer_GetCalibratedTimestampsEXT);

    // ----- VK_NV_mesh_shader
    if (string_eq(pName, "vkCmdDrawMeshTasksNV")) return to_vkVoidFunction(layer_CmdDrawMeshTasksNV);
    if (string_eq(pName, "vkCmdDrawMeshTasksIndirectNV")) return to_vkVoidFunction(layer_CmdDrawMeshTasksIndirectNV);
    if (string_eq(pName, "vkCmdDrawMeshTasksIndirectCountNV")) return to_vkVoidFunction(layer_CmdDrawMeshTasksIndirectCountNV);

    // ----- VK_NV_scissor_exclusive
    if (string_eq(pName, "vkCmdSetExclusiveScissorNV")) return to_vkVoidFunction(layer_CmdSetExclusiveScissorNV);

    // ----- VK_NV_device_diagnostic_checkpoints
    if (string_eq(pName, "vkCmdSetCheckpointNV")) return to_vkVoidFunction(layer_CmdSetCheckpointNV);
    if (string_eq(pName, "vkGetQueueCheckpointDataNV")) return to_vkVoidFunction(layer_GetQueueCheckpointDataNV);

    // ----- VK_INTEL_performance_query
    if (string_eq(pName, "vkInitializePerformanceApiINTEL")) return to_vkVoidFunction(layer_InitializePerformanceApiINTEL);
    if (string_eq(pName, "vkUninitializePerformanceApiINTEL")) return to_vkVoidFunction(layer_UninitializePerformanceApiINTEL);
    if (string_eq(pName, "vkCmdSetPerformanceMarkerINTEL")) return to_vkVoidFunction(layer_CmdSetPerformanceMarkerINTEL);
    if (string_eq(pName, "vkCmdSetPerformanceStreamMarkerINTEL")) return to_vkVoidFunction(layer_CmdSetPerformanceStreamMarkerINTEL);
    if (string_eq(pName, "vkCmdSetPerformanceOverrideINTEL")) return to_vkVoidFunction(layer_CmdSetPerformanceOverrideINTEL);
    if (string_eq(pName, "vkAcquirePerformanceConfigurationINTEL")) return to_vkVoidFunction(layer_AcquirePerformanceConfigurationINTEL);
    if (string_eq(pName, "vkReleasePerformanceConfigurationINTEL")) return to_vkVoidFunction(layer_ReleasePerformanceConfigurationINTEL);
    if (string_eq(pName, "vkQueueSetPerformanceConfigurationINTEL")) return to_vkVoidFunction(layer_QueueSetPerformanceConfigurationINTEL);
    if (string_eq(pName, "vkGetPerformanceParameterINTEL")) return to_vkVoidFunction(layer_GetPerformanceParameterINTEL);

    // ----- VK_AMD_display_native_hdr
    if (string_eq(pName, "vkSetLocalDimmingAMD")) return to_vkVoidFunction(layer_SetLocalDimmingAMD);

    // ----- VK_FUCHSIA_imagepipe_surface
#ifdef VK_USE_PLATFORM_FUCHSIA
    if (IsInstanceExtensionEnabled("VK_FUCHSIA_imagepipe_surface")) {
        if (string_eq(pName, "vkCreateImagePipeSurfaceFUCHSIA")) return to_vkVoidFunction(layer_CreateImagePipeSurfaceFUCHSIA);
    } // VK_FUCHSIA_imagepipe_surface
#endif // VK_USE_PLATFORM_FUCHSIA

    // ----- VK_EXT_metal_surface
#ifdef VK_USE_PLATFORM_METAL_EXT
    if (IsInstanceExtensionEnabled("VK_EXT_metal_surface")) {
        if (string_eq(pName, "vkCreateMetalSurfaceEXT")) return to_vkVoidFunction(layer_CreateMetalSurfaceEXT);
    } // VK_EXT_metal_surface
#endif // VK_USE_PLATFORM_METAL_EXT

    // ----- VK_EXT_buffer_device_address
    if (string_eq(pName, "vkGetBufferDeviceAddressEXT")) return to_vkVoidFunction(layer_GetBufferDeviceAddressEXT);

    // ----- VK_EXT_tooling_info
    if (string_eq(pName, "vkGetPhysicalDeviceToolPropertiesEXT")) return to_vkVoidFunction(layer_GetPhysicalDeviceToolPropertiesEXT);

    // ----- VK_NV_cooperative_matrix
    if (string_eq(pName, "vkGetPhysicalDeviceCooperativeMatrixPropertiesNV")) return to_vkVoidFunction(layer_GetPhysicalDeviceCooperativeMatrixPropertiesNV);

    // ----- VK_NV_coverage_reduction_mode
    if (string_eq(pName, "vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV")) return to_vkVoidFunction(layer_GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV);

    // ----- VK_EXT_full_screen_exclusive
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (string_eq(pName, "vkGetPhysicalDeviceSurfacePresentModes2EXT")) return to_vkVoidFunction(layer_GetPhysicalDeviceSurfacePresentModes2EXT);
    if (string_eq(pName, "vkAcquireFullScreenExclusiveModeEXT")) return to_vkVoidFunction(layer_AcquireFullScreenExclusiveModeEXT);
    if (string_eq(pName, "vkReleaseFullScreenExclusiveModeEXT")) return to_vkVoidFunction(layer_ReleaseFullScreenExclusiveModeEXT);
    if (string_eq(pName, "vkGetDeviceGroupSurfacePresentModes2EXT")) return to_vkVoidFunction(layer_GetDeviceGroupSurfacePresentModes2EXT);
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_EXT_headless_surface
    if (IsInstanceExtensionEnabled("VK_EXT_headless_surface")) {
        if (string_eq(pName, "vkCreateHeadlessSurfaceEXT")) return to_vkVoidFunction(layer_CreateHeadlessSurfaceEXT);
    } // VK_EXT_headless_surface

    // ----- VK_EXT_line_rasterization
    if (string_eq(pName, "vkCmdSetLineStippleEXT")) return to_vkVoidFunction(layer_CmdSetLineStippleEXT);

    // ----- VK_EXT_host_query_reset
    if (string_eq(pName, "vkResetQueryPoolEXT")) return to_vkVoidFunction(layer_ResetQueryPoolEXT);

    // ----- VK_EXT_extended_dynamic_state
    if (string_eq(pName, "vkCmdSetCullModeEXT")) return to_vkVoidFunction(layer_CmdSetCullModeEXT);
    if (string_eq(pName, "vkCmdSetFrontFaceEXT")) return to_vkVoidFunction(layer_CmdSetFrontFaceEXT);
    if (string_eq(pName, "vkCmdSetPrimitiveTopologyEXT")) return to_vkVoidFunction(layer_CmdSetPrimitiveTopologyEXT);
    if (string_eq(pName, "vkCmdSetViewportWithCountEXT")) return to_vkVoidFunction(layer_CmdSetViewportWithCountEXT);
    if (string_eq(pName, "vkCmdSetScissorWithCountEXT")) return to_vkVoidFunction(layer_CmdSetScissorWithCountEXT);
    if (string_eq(pName, "vkCmdBindVertexBuffers2EXT")) return to_vkVoidFunction(layer_CmdBindVertexBuffers2EXT);
    if (string_eq(pName, "vkCmdSetDepthTestEnableEXT")) return to_vkVoidFunction(layer_CmdSetDepthTestEnableEXT);
    if (string_eq(pName, "vkCmdSetDepthWriteEnableEXT")) return to_vkVoidFunction(layer_CmdSetDepthWriteEnableEXT);
    if (string_eq(pName, "vkCmdSetDepthCompareOpEXT")) return to_vkVoidFunction(layer_CmdSetDepthCompareOpEXT);
    if (string_eq(pName, "vkCmdSetDepthBoundsTestEnableEXT")) return to_vkVoidFunction(layer_CmdSetDepthBoundsTestEnableEXT);
    if (string_eq(pName, "vkCmdSetStencilTestEnableEXT")) return to_vkVoidFunction(layer_CmdSetStencilTestEnableEXT);
    if (string_eq(pName, "vkCmdSetStencilOpEXT")) return to_vkVoidFunction(layer_CmdSetStencilOpEXT);

    // ----- VK_NV_device_generated_commands
    if (string_eq(pName, "vkGetGeneratedCommandsMemoryRequirementsNV")) return to_vkVoidFunction(layer_GetGeneratedCommandsMemoryRequirementsNV);
    if (string_eq(pName, "vkCmdPreprocessGeneratedCommandsNV")) return to_vkVoidFunction(layer_CmdPreprocessGeneratedCommandsNV);
    if (string_eq(pName, "vkCmdExecuteGeneratedCommandsNV")) return to_vkVoidFunction(layer_CmdExecuteGeneratedCommandsNV);
    if (string_eq(pName, "vkCmdBindPipelineShaderGroupNV")) return to_vkVoidFunction(layer_CmdBindPipelineShaderGroupNV);
    if (string_eq(pName, "vkCreateIndirectCommandsLayoutNV")) return to_vkVoidFunction(layer_CreateIndirectCommandsLayoutNV);
    if (string_eq(pName, "vkDestroyIndirectCommandsLayoutNV")) return to_vkVoidFunction(layer_DestroyIndirectCommandsLayoutNV);

    // ----- VK_EXT_acquire_drm_display
    if (IsInstanceExtensionEnabled("VK_EXT_acquire_drm_display")) {
        if (string_eq(pName, "vkAcquireDrmDisplayEXT")) return to_vkVoidFunction(layer_AcquireDrmDisplayEXT);
        if (string_eq(pName, "vkGetDrmDisplayEXT")) return to_vkVoidFunction(layer_GetDrmDisplayEXT);
    } // VK_EXT_acquire_drm_display

    // ----- VK_EXT_private_data
    if (string_eq(pName, "vkCreatePrivateDataSlotEXT")) return to_vkVoidFunction(layer_CreatePrivateDataSlotEXT);
    if (string_eq(pName, "vkDestroyPrivateDataSlotEXT")) return to_vkVoidFunction(layer_DestroyPrivateDataSlotEXT);
    if (string_eq(pName, "vkSetPrivateDataEXT")) return to_vkVoidFunction(layer_SetPrivateDataEXT);
    if (string_eq(pName, "vkGetPrivateDataEXT")) return to_vkVoidFunction(layer_GetPrivateDataEXT);

    // ----- VK_NV_fragment_shading_rate_enums
    if (string_eq(pName, "vkCmdSetFragmentShadingRateEnumNV")) return to_vkVoidFunction(layer_CmdSetFragmentShadingRateEnumNV);

    // ----- VK_NV_acquire_winrt_display
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (string_eq(pName, "vkAcquireWinrtDisplayNV")) return to_vkVoidFunction(layer_AcquireWinrtDisplayNV);
    if (string_eq(pName, "vkGetWinrtDisplayNV")) return to_vkVoidFunction(layer_GetWinrtDisplayNV);
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_EXT_directfb_surface
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
    if (IsInstanceExtensionEnabled("VK_EXT_directfb_surface")) {
        if (string_eq(pName, "vkCreateDirectFBSurfaceEXT")) return to_vkVoidFunction(layer_CreateDirectFBSurfaceEXT);
        if (string_eq(pName, "vkGetPhysicalDeviceDirectFBPresentationSupportEXT")) return to_vkVoidFunction(layer_GetPhysicalDeviceDirectFBPresentationSupportEXT);
    } // VK_EXT_directfb_surface
#endif // VK_USE_PLATFORM_DIRECTFB_EXT

    // ----- VK_EXT_vertex_input_dynamic_state
    if (string_eq(pName, "vkCmdSetVertexInputEXT")) return to_vkVoidFunction(layer_CmdSetVertexInputEXT);

    // ----- VK_FUCHSIA_external_memory
#ifdef VK_USE_PLATFORM_FUCHSIA
    if (string_eq(pName, "vkGetMemoryZirconHandleFUCHSIA")) return to_vkVoidFunction(layer_GetMemoryZirconHandleFUCHSIA);
    if (string_eq(pName, "vkGetMemoryZirconHandlePropertiesFUCHSIA")) return to_vkVoidFunction(layer_GetMemoryZirconHandlePropertiesFUCHSIA);
#endif // VK_USE_PLATFORM_FUCHSIA

    // ----- VK_FUCHSIA_external_semaphore
#ifdef VK_USE_PLATFORM_FUCHSIA
    if (string_eq(pName, "vkImportSemaphoreZirconHandleFUCHSIA")) return to_vkVoidFunction(layer_ImportSemaphoreZirconHandleFUCHSIA);
    if (string_eq(pName, "vkGetSemaphoreZirconHandleFUCHSIA")) return to_vkVoidFunction(layer_GetSemaphoreZirconHandleFUCHSIA);
#endif // VK_USE_PLATFORM_FUCHSIA

    // ----- VK_FUCHSIA_buffer_collection
#ifdef VK_USE_PLATFORM_FUCHSIA
    if (string_eq(pName, "vkCreateBufferCollectionFUCHSIA")) return to_vkVoidFunction(layer_CreateBufferCollectionFUCHSIA);
    if (string_eq(pName, "vkSetBufferCollectionImageConstraintsFUCHSIA")) return to_vkVoidFunction(layer_SetBufferCollectionImageConstraintsFUCHSIA);
    if (string_eq(pName, "vkSetBufferCollectionBufferConstraintsFUCHSIA")) return to_vkVoidFunction(layer_SetBufferCollectionBufferConstraintsFUCHSIA);
    if (string_eq(pName, "vkDestroyBufferCollectionFUCHSIA")) return to_vkVoidFunction(layer_DestroyBufferCollectionFUCHSIA);
    if (string_eq(pName, "vkGetBufferCollectionPropertiesFUCHSIA")) return to_vkVoidFunction(layer_GetBufferCollectionPropertiesFUCHSIA);
#endif // VK_USE_PLATFORM_FUCHSIA

    // ----- VK_HUAWEI_subpass_shading
    if (string_eq(pName, "vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI")) return to_vkVoidFunction(layer_GetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI);
    if (string_eq(pName, "vkCmdSubpassShadingHUAWEI")) return to_vkVoidFunction(layer_CmdSubpassShadingHUAWEI);

    // ----- VK_HUAWEI_invocation_mask
    if (string_eq(pName, "vkCmdBindInvocationMaskHUAWEI")) return to_vkVoidFunction(layer_CmdBindInvocationMaskHUAWEI);

    // ----- VK_NV_external_memory_rdma
    if (string_eq(pName, "vkGetMemoryRemoteAddressNV")) return to_vkVoidFunction(layer_GetMemoryRemoteAddressNV);

    // ----- VK_EXT_extended_dynamic_state2
    if (string_eq(pName, "vkCmdSetPatchControlPointsEXT")) return to_vkVoidFunction(layer_CmdSetPatchControlPointsEXT);
    if (string_eq(pName, "vkCmdSetRasterizerDiscardEnableEXT")) return to_vkVoidFunction(layer_CmdSetRasterizerDiscardEnableEXT);
    if (string_eq(pName, "vkCmdSetDepthBiasEnableEXT")) return to_vkVoidFunction(layer_CmdSetDepthBiasEnableEXT);
    if (string_eq(pName, "vkCmdSetLogicOpEXT")) return to_vkVoidFunction(layer_CmdSetLogicOpEXT);
    if (string_eq(pName, "vkCmdSetPrimitiveRestartEnableEXT")) return to_vkVoidFunction(layer_CmdSetPrimitiveRestartEnableEXT);

    // ----- VK_QNX_screen_surface
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    if (IsInstanceExtensionEnabled("VK_QNX_screen_surface")) {
        if (string_eq(pName, "vkCreateScreenSurfaceQNX")) return to_vkVoidFunction(layer_CreateScreenSurfaceQNX);
        if (string_eq(pName, "vkGetPhysicalDeviceScreenPresentationSupportQNX")) return to_vkVoidFunction(layer_GetPhysicalDeviceScreenPresentationSupportQNX);
    } // VK_QNX_screen_surface
#endif // VK_USE_PLATFORM_SCREEN_QNX

    // ----- VK_EXT_color_write_enable
    if (string_eq(pName, "vkCmdSetColorWriteEnableEXT")) return to_vkVoidFunction(layer_CmdSetColorWriteEnableEXT);

    // ----- VK_EXT_multi_draw
    if (string_eq(pName, "vkCmdDrawMultiEXT")) return to_vkVoidFunction(layer_CmdDrawMultiEXT);
    if (string_eq(pName, "vkCmdDrawMultiIndexedEXT")) return to_vkVoidFunction(layer_CmdDrawMultiIndexedEXT);

    // ----- VK_EXT_pageable_device_local_memory
    if (string_eq(pName, "vkSetDeviceMemoryPriorityEXT")) return to_vkVoidFunction(layer_SetDeviceMemoryPriorityEXT);

    // ----- VK_VALVE_descriptor_set_host_mapping
    if (string_eq(pName, "vkGetDescriptorSetLayoutHostMappingInfoVALVE")) return to_vkVoidFunction(layer_GetDescriptorSetLayoutHostMappingInfoVALVE);
    if (string_eq(pName, "vkGetDescriptorSetHostMappingVALVE")) return to_vkVoidFunction(layer_GetDescriptorSetHostMappingVALVE);

    // ----- VK_KHR_acceleration_structure
    if (string_eq(pName, "vkCreateAccelerationStructureKHR")) return to_vkVoidFunction(layer_CreateAccelerationStructureKHR);
    if (string_eq(pName, "vkDestroyAccelerationStructureKHR")) return to_vkVoidFunction(layer_DestroyAccelerationStructureKHR);
    if (string_eq(pName, "vkCmdBuildAccelerationStructuresKHR")) return to_vkVoidFunction(layer_CmdBuildAccelerationStructuresKHR);
    if (string_eq(pName, "vkCmdBuildAccelerationStructuresIndirectKHR")) return to_vkVoidFunction(layer_CmdBuildAccelerationStructuresIndirectKHR);
    if (string_eq(pName, "vkBuildAccelerationStructuresKHR")) return to_vkVoidFunction(layer_BuildAccelerationStructuresKHR);
    if (string_eq(pName, "vkCopyAccelerationStructureKHR")) return to_vkVoidFunction(layer_CopyAccelerationStructureKHR);
    if (string_eq(pName, "vkCopyAccelerationStructureToMemoryKHR")) return to_vkVoidFunction(layer_CopyAccelerationStructureToMemoryKHR);
    if (string_eq(pName, "vkCopyMemoryToAccelerationStructureKHR")) return to_vkVoidFunction(layer_CopyMemoryToAccelerationStructureKHR);
    if (string_eq(pName, "vkWriteAccelerationStructuresPropertiesKHR")) return to_vkVoidFunction(layer_WriteAccelerationStructuresPropertiesKHR);
    if (string_eq(pName, "vkCmdCopyAccelerationStructureKHR")) return to_vkVoidFunction(layer_CmdCopyAccelerationStructureKHR);
    if (string_eq(pName, "vkCmdCopyAccelerationStructureToMemoryKHR")) return to_vkVoidFunction(layer_CmdCopyAccelerationStructureToMemoryKHR);
    if (string_eq(pName, "vkCmdCopyMemoryToAccelerationStructureKHR")) return to_vkVoidFunction(layer_CmdCopyMemoryToAccelerationStructureKHR);
    if (string_eq(pName, "vkGetAccelerationStructureDeviceAddressKHR")) return to_vkVoidFunction(layer_GetAccelerationStructureDeviceAddressKHR);
    if (string_eq(pName, "vkCmdWriteAccelerationStructuresPropertiesKHR")) return to_vkVoidFunction(layer_CmdWriteAccelerationStructuresPropertiesKHR);
    if (string_eq(pName, "vkGetDeviceAccelerationStructureCompatibilityKHR")) return to_vkVoidFunction(layer_GetDeviceAccelerationStructureCompatibilityKHR);
    if (string_eq(pName, "vkGetAccelerationStructureBuildSizesKHR")) return to_vkVoidFunction(layer_GetAccelerationStructureBuildSizesKHR);

    // ----- VK_KHR_ray_tracing_pipeline
    if (string_eq(pName, "vkCmdTraceRaysKHR")) return to_vkVoidFunction(layer_CmdTraceRaysKHR);
    if (string_eq(pName, "vkCreateRayTracingPipelinesKHR")) return to_vkVoidFunction(layer_CreateRayTracingPipelinesKHR);
    if (string_eq(pName, "vkGetRayTracingShaderGroupHandlesKHR")) return to_vkVoidFunction(layer_GetRayTracingShaderGroupHandlesKHR);
    if (string_eq(pName, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR")) return to_vkVoidFunction(layer_GetRayTracingCaptureReplayShaderGroupHandlesKHR);
    if (string_eq(pName, "vkCmdTraceRaysIndirectKHR")) return to_vkVoidFunction(layer_CmdTraceRaysIndirectKHR);
    if (string_eq(pName, "vkGetRayTracingShaderGroupStackSizeKHR")) return to_vkVoidFunction(layer_GetRayTracingShaderGroupStackSizeKHR);
    if (string_eq(pName, "vkCmdSetRayTracingPipelineStackSizeKHR")) return to_vkVoidFunction(layer_CmdSetRayTracingPipelineStackSizeKHR);

    return layer.instance_dispatch_table.GetInstanceProcAddr(instance, pName);
}

bool IsDeviceExtensionEnabled(const char* extension_name) {
    return layer.created_devices[0].enabled_extensions.end() !=
           std::find_if(layer.created_devices[0].enabled_extensions.begin(),
                        layer.created_devices[0].enabled_extensions.end(),
                        [extension_name](Extension const& ext) { return ext.extensionName == extension_name; });
}

FRAMEWORK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL layer_GetDeviceProcAddr(VkDevice device, const char* pName) {

    // ----- VK_VERSION_1_0
    {
        if (string_eq(pName, "vkCreateInstance")) return to_vkVoidFunction(layer_CreateInstance);
        if (string_eq(pName, "vkDestroyInstance")) return to_vkVoidFunction(layer_DestroyInstance);
        if (string_eq(pName, "vkEnumeratePhysicalDevices")) return to_vkVoidFunction(layer_EnumeratePhysicalDevices);
        if (string_eq(pName, "vkGetPhysicalDeviceFeatures")) return to_vkVoidFunction(layer_GetPhysicalDeviceFeatures);
        if (string_eq(pName, "vkGetPhysicalDeviceFormatProperties")) return to_vkVoidFunction(layer_GetPhysicalDeviceFormatProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceImageFormatProperties")) return to_vkVoidFunction(layer_GetPhysicalDeviceImageFormatProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceProperties")) return to_vkVoidFunction(layer_GetPhysicalDeviceProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyProperties")) return to_vkVoidFunction(layer_GetPhysicalDeviceQueueFamilyProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceMemoryProperties")) return to_vkVoidFunction(layer_GetPhysicalDeviceMemoryProperties);
        if (string_eq(pName, "vkGetInstanceProcAddr")) return to_vkVoidFunction(layer_GetInstanceProcAddr);
        if (string_eq(pName, "vkGetDeviceProcAddr")) return to_vkVoidFunction(layer_GetDeviceProcAddr);
        if (string_eq(pName, "vkCreateDevice")) return to_vkVoidFunction(layer_CreateDevice);
        if (string_eq(pName, "vkDestroyDevice")) return to_vkVoidFunction(layer_DestroyDevice);
        if (string_eq(pName, "vkEnumerateInstanceExtensionProperties")) return to_vkVoidFunction(layer_EnumerateInstanceExtensionProperties);
        if (string_eq(pName, "vkEnumerateDeviceExtensionProperties")) return to_vkVoidFunction(layer_EnumerateDeviceExtensionProperties);
        if (string_eq(pName, "vkEnumerateInstanceLayerProperties")) return to_vkVoidFunction(layer_EnumerateInstanceLayerProperties);
        if (string_eq(pName, "vkEnumerateDeviceLayerProperties")) return to_vkVoidFunction(layer_EnumerateDeviceLayerProperties);
        if (string_eq(pName, "vkGetDeviceQueue")) return to_vkVoidFunction(layer_GetDeviceQueue);
        if (string_eq(pName, "vkQueueSubmit")) return to_vkVoidFunction(layer_QueueSubmit);
        if (string_eq(pName, "vkQueueWaitIdle")) return to_vkVoidFunction(layer_QueueWaitIdle);
        if (string_eq(pName, "vkDeviceWaitIdle")) return to_vkVoidFunction(layer_DeviceWaitIdle);
        if (string_eq(pName, "vkAllocateMemory")) return to_vkVoidFunction(layer_AllocateMemory);
        if (string_eq(pName, "vkFreeMemory")) return to_vkVoidFunction(layer_FreeMemory);
        if (string_eq(pName, "vkMapMemory")) return to_vkVoidFunction(layer_MapMemory);
        if (string_eq(pName, "vkUnmapMemory")) return to_vkVoidFunction(layer_UnmapMemory);
        if (string_eq(pName, "vkFlushMappedMemoryRanges")) return to_vkVoidFunction(layer_FlushMappedMemoryRanges);
        if (string_eq(pName, "vkInvalidateMappedMemoryRanges")) return to_vkVoidFunction(layer_InvalidateMappedMemoryRanges);
        if (string_eq(pName, "vkGetDeviceMemoryCommitment")) return to_vkVoidFunction(layer_GetDeviceMemoryCommitment);
        if (string_eq(pName, "vkBindBufferMemory")) return to_vkVoidFunction(layer_BindBufferMemory);
        if (string_eq(pName, "vkBindImageMemory")) return to_vkVoidFunction(layer_BindImageMemory);
        if (string_eq(pName, "vkGetBufferMemoryRequirements")) return to_vkVoidFunction(layer_GetBufferMemoryRequirements);
        if (string_eq(pName, "vkGetImageMemoryRequirements")) return to_vkVoidFunction(layer_GetImageMemoryRequirements);
        if (string_eq(pName, "vkGetImageSparseMemoryRequirements")) return to_vkVoidFunction(layer_GetImageSparseMemoryRequirements);
        if (string_eq(pName, "vkGetPhysicalDeviceSparseImageFormatProperties")) return to_vkVoidFunction(layer_GetPhysicalDeviceSparseImageFormatProperties);
        if (string_eq(pName, "vkQueueBindSparse")) return to_vkVoidFunction(layer_QueueBindSparse);
        if (string_eq(pName, "vkCreateFence")) return to_vkVoidFunction(layer_CreateFence);
        if (string_eq(pName, "vkDestroyFence")) return to_vkVoidFunction(layer_DestroyFence);
        if (string_eq(pName, "vkResetFences")) return to_vkVoidFunction(layer_ResetFences);
        if (string_eq(pName, "vkGetFenceStatus")) return to_vkVoidFunction(layer_GetFenceStatus);
        if (string_eq(pName, "vkWaitForFences")) return to_vkVoidFunction(layer_WaitForFences);
        if (string_eq(pName, "vkCreateSemaphore")) return to_vkVoidFunction(layer_CreateSemaphore);
        if (string_eq(pName, "vkDestroySemaphore")) return to_vkVoidFunction(layer_DestroySemaphore);
        if (string_eq(pName, "vkCreateEvent")) return to_vkVoidFunction(layer_CreateEvent);
        if (string_eq(pName, "vkDestroyEvent")) return to_vkVoidFunction(layer_DestroyEvent);
        if (string_eq(pName, "vkGetEventStatus")) return to_vkVoidFunction(layer_GetEventStatus);
        if (string_eq(pName, "vkSetEvent")) return to_vkVoidFunction(layer_SetEvent);
        if (string_eq(pName, "vkResetEvent")) return to_vkVoidFunction(layer_ResetEvent);
        if (string_eq(pName, "vkCreateQueryPool")) return to_vkVoidFunction(layer_CreateQueryPool);
        if (string_eq(pName, "vkDestroyQueryPool")) return to_vkVoidFunction(layer_DestroyQueryPool);
        if (string_eq(pName, "vkGetQueryPoolResults")) return to_vkVoidFunction(layer_GetQueryPoolResults);
        if (string_eq(pName, "vkCreateBuffer")) return to_vkVoidFunction(layer_CreateBuffer);
        if (string_eq(pName, "vkDestroyBuffer")) return to_vkVoidFunction(layer_DestroyBuffer);
        if (string_eq(pName, "vkCreateBufferView")) return to_vkVoidFunction(layer_CreateBufferView);
        if (string_eq(pName, "vkDestroyBufferView")) return to_vkVoidFunction(layer_DestroyBufferView);
        if (string_eq(pName, "vkCreateImage")) return to_vkVoidFunction(layer_CreateImage);
        if (string_eq(pName, "vkDestroyImage")) return to_vkVoidFunction(layer_DestroyImage);
        if (string_eq(pName, "vkGetImageSubresourceLayout")) return to_vkVoidFunction(layer_GetImageSubresourceLayout);
        if (string_eq(pName, "vkCreateImageView")) return to_vkVoidFunction(layer_CreateImageView);
        if (string_eq(pName, "vkDestroyImageView")) return to_vkVoidFunction(layer_DestroyImageView);
        if (string_eq(pName, "vkCreateShaderModule")) return to_vkVoidFunction(layer_CreateShaderModule);
        if (string_eq(pName, "vkDestroyShaderModule")) return to_vkVoidFunction(layer_DestroyShaderModule);
        if (string_eq(pName, "vkCreatePipelineCache")) return to_vkVoidFunction(layer_CreatePipelineCache);
        if (string_eq(pName, "vkDestroyPipelineCache")) return to_vkVoidFunction(layer_DestroyPipelineCache);
        if (string_eq(pName, "vkGetPipelineCacheData")) return to_vkVoidFunction(layer_GetPipelineCacheData);
        if (string_eq(pName, "vkMergePipelineCaches")) return to_vkVoidFunction(layer_MergePipelineCaches);
        if (string_eq(pName, "vkCreateGraphicsPipelines")) return to_vkVoidFunction(layer_CreateGraphicsPipelines);
        if (string_eq(pName, "vkCreateComputePipelines")) return to_vkVoidFunction(layer_CreateComputePipelines);
        if (string_eq(pName, "vkDestroyPipeline")) return to_vkVoidFunction(layer_DestroyPipeline);
        if (string_eq(pName, "vkCreatePipelineLayout")) return to_vkVoidFunction(layer_CreatePipelineLayout);
        if (string_eq(pName, "vkDestroyPipelineLayout")) return to_vkVoidFunction(layer_DestroyPipelineLayout);
        if (string_eq(pName, "vkCreateSampler")) return to_vkVoidFunction(layer_CreateSampler);
        if (string_eq(pName, "vkDestroySampler")) return to_vkVoidFunction(layer_DestroySampler);
        if (string_eq(pName, "vkCreateDescriptorSetLayout")) return to_vkVoidFunction(layer_CreateDescriptorSetLayout);
        if (string_eq(pName, "vkDestroyDescriptorSetLayout")) return to_vkVoidFunction(layer_DestroyDescriptorSetLayout);
        if (string_eq(pName, "vkCreateDescriptorPool")) return to_vkVoidFunction(layer_CreateDescriptorPool);
        if (string_eq(pName, "vkDestroyDescriptorPool")) return to_vkVoidFunction(layer_DestroyDescriptorPool);
        if (string_eq(pName, "vkResetDescriptorPool")) return to_vkVoidFunction(layer_ResetDescriptorPool);
        if (string_eq(pName, "vkAllocateDescriptorSets")) return to_vkVoidFunction(layer_AllocateDescriptorSets);
        if (string_eq(pName, "vkFreeDescriptorSets")) return to_vkVoidFunction(layer_FreeDescriptorSets);
        if (string_eq(pName, "vkUpdateDescriptorSets")) return to_vkVoidFunction(layer_UpdateDescriptorSets);
        if (string_eq(pName, "vkCreateFramebuffer")) return to_vkVoidFunction(layer_CreateFramebuffer);
        if (string_eq(pName, "vkDestroyFramebuffer")) return to_vkVoidFunction(layer_DestroyFramebuffer);
        if (string_eq(pName, "vkCreateRenderPass")) return to_vkVoidFunction(layer_CreateRenderPass);
        if (string_eq(pName, "vkDestroyRenderPass")) return to_vkVoidFunction(layer_DestroyRenderPass);
        if (string_eq(pName, "vkGetRenderAreaGranularity")) return to_vkVoidFunction(layer_GetRenderAreaGranularity);
        if (string_eq(pName, "vkCreateCommandPool")) return to_vkVoidFunction(layer_CreateCommandPool);
        if (string_eq(pName, "vkDestroyCommandPool")) return to_vkVoidFunction(layer_DestroyCommandPool);
        if (string_eq(pName, "vkResetCommandPool")) return to_vkVoidFunction(layer_ResetCommandPool);
        if (string_eq(pName, "vkAllocateCommandBuffers")) return to_vkVoidFunction(layer_AllocateCommandBuffers);
        if (string_eq(pName, "vkFreeCommandBuffers")) return to_vkVoidFunction(layer_FreeCommandBuffers);
        if (string_eq(pName, "vkBeginCommandBuffer")) return to_vkVoidFunction(layer_BeginCommandBuffer);
        if (string_eq(pName, "vkEndCommandBuffer")) return to_vkVoidFunction(layer_EndCommandBuffer);
        if (string_eq(pName, "vkResetCommandBuffer")) return to_vkVoidFunction(layer_ResetCommandBuffer);
        if (string_eq(pName, "vkCmdBindPipeline")) return to_vkVoidFunction(layer_CmdBindPipeline);
        if (string_eq(pName, "vkCmdSetViewport")) return to_vkVoidFunction(layer_CmdSetViewport);
        if (string_eq(pName, "vkCmdSetScissor")) return to_vkVoidFunction(layer_CmdSetScissor);
        if (string_eq(pName, "vkCmdSetLineWidth")) return to_vkVoidFunction(layer_CmdSetLineWidth);
        if (string_eq(pName, "vkCmdSetDepthBias")) return to_vkVoidFunction(layer_CmdSetDepthBias);
        if (string_eq(pName, "vkCmdSetBlendConstants")) return to_vkVoidFunction(layer_CmdSetBlendConstants);
        if (string_eq(pName, "vkCmdSetDepthBounds")) return to_vkVoidFunction(layer_CmdSetDepthBounds);
        if (string_eq(pName, "vkCmdSetStencilCompareMask")) return to_vkVoidFunction(layer_CmdSetStencilCompareMask);
        if (string_eq(pName, "vkCmdSetStencilWriteMask")) return to_vkVoidFunction(layer_CmdSetStencilWriteMask);
        if (string_eq(pName, "vkCmdSetStencilReference")) return to_vkVoidFunction(layer_CmdSetStencilReference);
        if (string_eq(pName, "vkCmdBindDescriptorSets")) return to_vkVoidFunction(layer_CmdBindDescriptorSets);
        if (string_eq(pName, "vkCmdBindIndexBuffer")) return to_vkVoidFunction(layer_CmdBindIndexBuffer);
        if (string_eq(pName, "vkCmdBindVertexBuffers")) return to_vkVoidFunction(layer_CmdBindVertexBuffers);
        if (string_eq(pName, "vkCmdDraw")) return to_vkVoidFunction(layer_CmdDraw);
        if (string_eq(pName, "vkCmdDrawIndexed")) return to_vkVoidFunction(layer_CmdDrawIndexed);
        if (string_eq(pName, "vkCmdDrawIndirect")) return to_vkVoidFunction(layer_CmdDrawIndirect);
        if (string_eq(pName, "vkCmdDrawIndexedIndirect")) return to_vkVoidFunction(layer_CmdDrawIndexedIndirect);
        if (string_eq(pName, "vkCmdDispatch")) return to_vkVoidFunction(layer_CmdDispatch);
        if (string_eq(pName, "vkCmdDispatchIndirect")) return to_vkVoidFunction(layer_CmdDispatchIndirect);
        if (string_eq(pName, "vkCmdCopyBuffer")) return to_vkVoidFunction(layer_CmdCopyBuffer);
        if (string_eq(pName, "vkCmdCopyImage")) return to_vkVoidFunction(layer_CmdCopyImage);
        if (string_eq(pName, "vkCmdBlitImage")) return to_vkVoidFunction(layer_CmdBlitImage);
        if (string_eq(pName, "vkCmdCopyBufferToImage")) return to_vkVoidFunction(layer_CmdCopyBufferToImage);
        if (string_eq(pName, "vkCmdCopyImageToBuffer")) return to_vkVoidFunction(layer_CmdCopyImageToBuffer);
        if (string_eq(pName, "vkCmdUpdateBuffer")) return to_vkVoidFunction(layer_CmdUpdateBuffer);
        if (string_eq(pName, "vkCmdFillBuffer")) return to_vkVoidFunction(layer_CmdFillBuffer);
        if (string_eq(pName, "vkCmdClearColorImage")) return to_vkVoidFunction(layer_CmdClearColorImage);
        if (string_eq(pName, "vkCmdClearDepthStencilImage")) return to_vkVoidFunction(layer_CmdClearDepthStencilImage);
        if (string_eq(pName, "vkCmdClearAttachments")) return to_vkVoidFunction(layer_CmdClearAttachments);
        if (string_eq(pName, "vkCmdResolveImage")) return to_vkVoidFunction(layer_CmdResolveImage);
        if (string_eq(pName, "vkCmdSetEvent")) return to_vkVoidFunction(layer_CmdSetEvent);
        if (string_eq(pName, "vkCmdResetEvent")) return to_vkVoidFunction(layer_CmdResetEvent);
        if (string_eq(pName, "vkCmdWaitEvents")) return to_vkVoidFunction(layer_CmdWaitEvents);
        if (string_eq(pName, "vkCmdPipelineBarrier")) return to_vkVoidFunction(layer_CmdPipelineBarrier);
        if (string_eq(pName, "vkCmdBeginQuery")) return to_vkVoidFunction(layer_CmdBeginQuery);
        if (string_eq(pName, "vkCmdEndQuery")) return to_vkVoidFunction(layer_CmdEndQuery);
        if (string_eq(pName, "vkCmdResetQueryPool")) return to_vkVoidFunction(layer_CmdResetQueryPool);
        if (string_eq(pName, "vkCmdWriteTimestamp")) return to_vkVoidFunction(layer_CmdWriteTimestamp);
        if (string_eq(pName, "vkCmdCopyQueryPoolResults")) return to_vkVoidFunction(layer_CmdCopyQueryPoolResults);
        if (string_eq(pName, "vkCmdPushConstants")) return to_vkVoidFunction(layer_CmdPushConstants);
        if (string_eq(pName, "vkCmdBeginRenderPass")) return to_vkVoidFunction(layer_CmdBeginRenderPass);
        if (string_eq(pName, "vkCmdNextSubpass")) return to_vkVoidFunction(layer_CmdNextSubpass);
        if (string_eq(pName, "vkCmdEndRenderPass")) return to_vkVoidFunction(layer_CmdEndRenderPass);
        if (string_eq(pName, "vkCmdExecuteCommands")) return to_vkVoidFunction(layer_CmdExecuteCommands);
    } // End Core 1.0

    // ----- VK_VERSION_1_1
    if (layer.enabled_instance_minor >= 1) {
        if (string_eq(pName, "vkEnumerateInstanceVersion")) return to_vkVoidFunction(layer_EnumerateInstanceVersion);
        if (string_eq(pName, "vkBindBufferMemory2")) return to_vkVoidFunction(layer_BindBufferMemory2);
        if (string_eq(pName, "vkBindImageMemory2")) return to_vkVoidFunction(layer_BindImageMemory2);
        if (string_eq(pName, "vkGetDeviceGroupPeerMemoryFeatures")) return to_vkVoidFunction(layer_GetDeviceGroupPeerMemoryFeatures);
        if (string_eq(pName, "vkCmdSetDeviceMask")) return to_vkVoidFunction(layer_CmdSetDeviceMask);
        if (string_eq(pName, "vkCmdDispatchBase")) return to_vkVoidFunction(layer_CmdDispatchBase);
        if (string_eq(pName, "vkEnumeratePhysicalDeviceGroups")) return to_vkVoidFunction(layer_EnumeratePhysicalDeviceGroups);
        if (string_eq(pName, "vkGetImageMemoryRequirements2")) return to_vkVoidFunction(layer_GetImageMemoryRequirements2);
        if (string_eq(pName, "vkGetBufferMemoryRequirements2")) return to_vkVoidFunction(layer_GetBufferMemoryRequirements2);
        if (string_eq(pName, "vkGetImageSparseMemoryRequirements2")) return to_vkVoidFunction(layer_GetImageSparseMemoryRequirements2);
        if (string_eq(pName, "vkGetPhysicalDeviceFeatures2")) return to_vkVoidFunction(layer_GetPhysicalDeviceFeatures2);
        if (string_eq(pName, "vkGetPhysicalDeviceProperties2")) return to_vkVoidFunction(layer_GetPhysicalDeviceProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceFormatProperties2")) return to_vkVoidFunction(layer_GetPhysicalDeviceFormatProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceImageFormatProperties2")) return to_vkVoidFunction(layer_GetPhysicalDeviceImageFormatProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyProperties2")) return to_vkVoidFunction(layer_GetPhysicalDeviceQueueFamilyProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceMemoryProperties2")) return to_vkVoidFunction(layer_GetPhysicalDeviceMemoryProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceSparseImageFormatProperties2")) return to_vkVoidFunction(layer_GetPhysicalDeviceSparseImageFormatProperties2);
        if (string_eq(pName, "vkTrimCommandPool")) return to_vkVoidFunction(layer_TrimCommandPool);
        if (string_eq(pName, "vkGetDeviceQueue2")) return to_vkVoidFunction(layer_GetDeviceQueue2);
        if (string_eq(pName, "vkCreateSamplerYcbcrConversion")) return to_vkVoidFunction(layer_CreateSamplerYcbcrConversion);
        if (string_eq(pName, "vkDestroySamplerYcbcrConversion")) return to_vkVoidFunction(layer_DestroySamplerYcbcrConversion);
        if (string_eq(pName, "vkCreateDescriptorUpdateTemplate")) return to_vkVoidFunction(layer_CreateDescriptorUpdateTemplate);
        if (string_eq(pName, "vkDestroyDescriptorUpdateTemplate")) return to_vkVoidFunction(layer_DestroyDescriptorUpdateTemplate);
        if (string_eq(pName, "vkUpdateDescriptorSetWithTemplate")) return to_vkVoidFunction(layer_UpdateDescriptorSetWithTemplate);
        if (string_eq(pName, "vkGetPhysicalDeviceExternalBufferProperties")) return to_vkVoidFunction(layer_GetPhysicalDeviceExternalBufferProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceExternalFenceProperties")) return to_vkVoidFunction(layer_GetPhysicalDeviceExternalFenceProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceExternalSemaphoreProperties")) return to_vkVoidFunction(layer_GetPhysicalDeviceExternalSemaphoreProperties);
        if (string_eq(pName, "vkGetDescriptorSetLayoutSupport")) return to_vkVoidFunction(layer_GetDescriptorSetLayoutSupport);
    } // End Core 1.1

    // ----- VK_VERSION_1_2
    if (layer.enabled_instance_minor >= 2) {
        if (string_eq(pName, "vkCmdDrawIndirectCount")) return to_vkVoidFunction(layer_CmdDrawIndirectCount);
        if (string_eq(pName, "vkCmdDrawIndexedIndirectCount")) return to_vkVoidFunction(layer_CmdDrawIndexedIndirectCount);
        if (string_eq(pName, "vkCreateRenderPass2")) return to_vkVoidFunction(layer_CreateRenderPass2);
        if (string_eq(pName, "vkCmdBeginRenderPass2")) return to_vkVoidFunction(layer_CmdBeginRenderPass2);
        if (string_eq(pName, "vkCmdNextSubpass2")) return to_vkVoidFunction(layer_CmdNextSubpass2);
        if (string_eq(pName, "vkCmdEndRenderPass2")) return to_vkVoidFunction(layer_CmdEndRenderPass2);
        if (string_eq(pName, "vkResetQueryPool")) return to_vkVoidFunction(layer_ResetQueryPool);
        if (string_eq(pName, "vkGetSemaphoreCounterValue")) return to_vkVoidFunction(layer_GetSemaphoreCounterValue);
        if (string_eq(pName, "vkWaitSemaphores")) return to_vkVoidFunction(layer_WaitSemaphores);
        if (string_eq(pName, "vkSignalSemaphore")) return to_vkVoidFunction(layer_SignalSemaphore);
        if (string_eq(pName, "vkGetBufferDeviceAddress")) return to_vkVoidFunction(layer_GetBufferDeviceAddress);
        if (string_eq(pName, "vkGetBufferOpaqueCaptureAddress")) return to_vkVoidFunction(layer_GetBufferOpaqueCaptureAddress);
        if (string_eq(pName, "vkGetDeviceMemoryOpaqueCaptureAddress")) return to_vkVoidFunction(layer_GetDeviceMemoryOpaqueCaptureAddress);
    } // End Core 1.2

    // ----- VK_VERSION_1_3
    if (layer.enabled_instance_minor >= 3) {
        if (string_eq(pName, "vkGetPhysicalDeviceToolProperties")) return to_vkVoidFunction(layer_GetPhysicalDeviceToolProperties);
        if (string_eq(pName, "vkCreatePrivateDataSlot")) return to_vkVoidFunction(layer_CreatePrivateDataSlot);
        if (string_eq(pName, "vkDestroyPrivateDataSlot")) return to_vkVoidFunction(layer_DestroyPrivateDataSlot);
        if (string_eq(pName, "vkSetPrivateData")) return to_vkVoidFunction(layer_SetPrivateData);
        if (string_eq(pName, "vkGetPrivateData")) return to_vkVoidFunction(layer_GetPrivateData);
        if (string_eq(pName, "vkCmdSetEvent2")) return to_vkVoidFunction(layer_CmdSetEvent2);
        if (string_eq(pName, "vkCmdResetEvent2")) return to_vkVoidFunction(layer_CmdResetEvent2);
        if (string_eq(pName, "vkCmdWaitEvents2")) return to_vkVoidFunction(layer_CmdWaitEvents2);
        if (string_eq(pName, "vkCmdPipelineBarrier2")) return to_vkVoidFunction(layer_CmdPipelineBarrier2);
        if (string_eq(pName, "vkCmdWriteTimestamp2")) return to_vkVoidFunction(layer_CmdWriteTimestamp2);
        if (string_eq(pName, "vkQueueSubmit2")) return to_vkVoidFunction(layer_QueueSubmit2);
        if (string_eq(pName, "vkCmdCopyBuffer2")) return to_vkVoidFunction(layer_CmdCopyBuffer2);
        if (string_eq(pName, "vkCmdCopyImage2")) return to_vkVoidFunction(layer_CmdCopyImage2);
        if (string_eq(pName, "vkCmdCopyBufferToImage2")) return to_vkVoidFunction(layer_CmdCopyBufferToImage2);
        if (string_eq(pName, "vkCmdCopyImageToBuffer2")) return to_vkVoidFunction(layer_CmdCopyImageToBuffer2);
        if (string_eq(pName, "vkCmdBlitImage2")) return to_vkVoidFunction(layer_CmdBlitImage2);
        if (string_eq(pName, "vkCmdResolveImage2")) return to_vkVoidFunction(layer_CmdResolveImage2);
        if (string_eq(pName, "vkCmdBeginRendering")) return to_vkVoidFunction(layer_CmdBeginRendering);
        if (string_eq(pName, "vkCmdEndRendering")) return to_vkVoidFunction(layer_CmdEndRendering);
        if (string_eq(pName, "vkCmdSetCullMode")) return to_vkVoidFunction(layer_CmdSetCullMode);
        if (string_eq(pName, "vkCmdSetFrontFace")) return to_vkVoidFunction(layer_CmdSetFrontFace);
        if (string_eq(pName, "vkCmdSetPrimitiveTopology")) return to_vkVoidFunction(layer_CmdSetPrimitiveTopology);
        if (string_eq(pName, "vkCmdSetViewportWithCount")) return to_vkVoidFunction(layer_CmdSetViewportWithCount);
        if (string_eq(pName, "vkCmdSetScissorWithCount")) return to_vkVoidFunction(layer_CmdSetScissorWithCount);
        if (string_eq(pName, "vkCmdBindVertexBuffers2")) return to_vkVoidFunction(layer_CmdBindVertexBuffers2);
        if (string_eq(pName, "vkCmdSetDepthTestEnable")) return to_vkVoidFunction(layer_CmdSetDepthTestEnable);
        if (string_eq(pName, "vkCmdSetDepthWriteEnable")) return to_vkVoidFunction(layer_CmdSetDepthWriteEnable);
        if (string_eq(pName, "vkCmdSetDepthCompareOp")) return to_vkVoidFunction(layer_CmdSetDepthCompareOp);
        if (string_eq(pName, "vkCmdSetDepthBoundsTestEnable")) return to_vkVoidFunction(layer_CmdSetDepthBoundsTestEnable);
        if (string_eq(pName, "vkCmdSetStencilTestEnable")) return to_vkVoidFunction(layer_CmdSetStencilTestEnable);
        if (string_eq(pName, "vkCmdSetStencilOp")) return to_vkVoidFunction(layer_CmdSetStencilOp);
        if (string_eq(pName, "vkCmdSetRasterizerDiscardEnable")) return to_vkVoidFunction(layer_CmdSetRasterizerDiscardEnable);
        if (string_eq(pName, "vkCmdSetDepthBiasEnable")) return to_vkVoidFunction(layer_CmdSetDepthBiasEnable);
        if (string_eq(pName, "vkCmdSetPrimitiveRestartEnable")) return to_vkVoidFunction(layer_CmdSetPrimitiveRestartEnable);
        if (string_eq(pName, "vkGetDeviceBufferMemoryRequirements")) return to_vkVoidFunction(layer_GetDeviceBufferMemoryRequirements);
        if (string_eq(pName, "vkGetDeviceImageMemoryRequirements")) return to_vkVoidFunction(layer_GetDeviceImageMemoryRequirements);
        if (string_eq(pName, "vkGetDeviceImageSparseMemoryRequirements")) return to_vkVoidFunction(layer_GetDeviceImageSparseMemoryRequirements);
    } // End Core 1.3

    // ----- VK_KHR_surface
    if (string_eq(pName, "vkDestroySurfaceKHR")) return to_vkVoidFunction(layer_DestroySurfaceKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceSurfaceSupportKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceSurfaceSupportKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceSurfaceCapabilitiesKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceSurfaceFormatsKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceSurfaceFormatsKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceSurfacePresentModesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceSurfacePresentModesKHR);

    // ----- VK_KHR_swapchain
    if (IsDeviceExtensionEnabled("VK_KHR_swapchain")) {
        if (string_eq(pName, "vkCreateSwapchainKHR")) return to_vkVoidFunction(layer_CreateSwapchainKHR);
        if (string_eq(pName, "vkDestroySwapchainKHR")) return to_vkVoidFunction(layer_DestroySwapchainKHR);
        if (string_eq(pName, "vkGetSwapchainImagesKHR")) return to_vkVoidFunction(layer_GetSwapchainImagesKHR);
        if (string_eq(pName, "vkAcquireNextImageKHR")) return to_vkVoidFunction(layer_AcquireNextImageKHR);
        if (string_eq(pName, "vkQueuePresentKHR")) return to_vkVoidFunction(layer_QueuePresentKHR);
        if (string_eq(pName, "vkGetDeviceGroupPresentCapabilitiesKHR")) return to_vkVoidFunction(layer_GetDeviceGroupPresentCapabilitiesKHR);
        if (string_eq(pName, "vkGetDeviceGroupSurfacePresentModesKHR")) return to_vkVoidFunction(layer_GetDeviceGroupSurfacePresentModesKHR);
        if (string_eq(pName, "vkGetPhysicalDevicePresentRectanglesKHR")) return to_vkVoidFunction(layer_GetPhysicalDevicePresentRectanglesKHR);
        if (string_eq(pName, "vkAcquireNextImage2KHR")) return to_vkVoidFunction(layer_AcquireNextImage2KHR);
    } // VK_KHR_swapchain

    // ----- VK_KHR_display
    if (string_eq(pName, "vkGetPhysicalDeviceDisplayPropertiesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceDisplayPropertiesKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceDisplayPlanePropertiesKHR);
    if (string_eq(pName, "vkGetDisplayPlaneSupportedDisplaysKHR")) return to_vkVoidFunction(layer_GetDisplayPlaneSupportedDisplaysKHR);
    if (string_eq(pName, "vkGetDisplayModePropertiesKHR")) return to_vkVoidFunction(layer_GetDisplayModePropertiesKHR);
    if (string_eq(pName, "vkCreateDisplayModeKHR")) return to_vkVoidFunction(layer_CreateDisplayModeKHR);
    if (string_eq(pName, "vkGetDisplayPlaneCapabilitiesKHR")) return to_vkVoidFunction(layer_GetDisplayPlaneCapabilitiesKHR);
    if (string_eq(pName, "vkCreateDisplayPlaneSurfaceKHR")) return to_vkVoidFunction(layer_CreateDisplayPlaneSurfaceKHR);

    // ----- VK_KHR_display_swapchain
    if (IsDeviceExtensionEnabled("VK_KHR_display_swapchain")) {
        if (string_eq(pName, "vkCreateSharedSwapchainsKHR")) return to_vkVoidFunction(layer_CreateSharedSwapchainsKHR);
    } // VK_KHR_display_swapchain

    // ----- VK_KHR_xlib_surface
#ifdef VK_USE_PLATFORM_XLIB_KHR
    if (string_eq(pName, "vkCreateXlibSurfaceKHR")) return to_vkVoidFunction(layer_CreateXlibSurfaceKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceXlibPresentationSupportKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceXlibPresentationSupportKHR);
#endif // VK_USE_PLATFORM_XLIB_KHR

    // ----- VK_KHR_xcb_surface
#ifdef VK_USE_PLATFORM_XCB_KHR
    if (string_eq(pName, "vkCreateXcbSurfaceKHR")) return to_vkVoidFunction(layer_CreateXcbSurfaceKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceXcbPresentationSupportKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceXcbPresentationSupportKHR);
#endif // VK_USE_PLATFORM_XCB_KHR

    // ----- VK_KHR_wayland_surface
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    if (string_eq(pName, "vkCreateWaylandSurfaceKHR")) return to_vkVoidFunction(layer_CreateWaylandSurfaceKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceWaylandPresentationSupportKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceWaylandPresentationSupportKHR);
#endif // VK_USE_PLATFORM_WAYLAND_KHR

    // ----- VK_KHR_win32_surface
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (string_eq(pName, "vkCreateWin32SurfaceKHR")) return to_vkVoidFunction(layer_CreateWin32SurfaceKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceWin32PresentationSupportKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceWin32PresentationSupportKHR);
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_KHR_video_queue
#ifdef VK_ENABLE_BETA_EXTENSIONS
    if (IsDeviceExtensionEnabled("VK_KHR_video_queue")) {
        if (string_eq(pName, "vkGetPhysicalDeviceVideoCapabilitiesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceVideoCapabilitiesKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceVideoFormatPropertiesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceVideoFormatPropertiesKHR);
        if (string_eq(pName, "vkCreateVideoSessionKHR")) return to_vkVoidFunction(layer_CreateVideoSessionKHR);
        if (string_eq(pName, "vkDestroyVideoSessionKHR")) return to_vkVoidFunction(layer_DestroyVideoSessionKHR);
        if (string_eq(pName, "vkGetVideoSessionMemoryRequirementsKHR")) return to_vkVoidFunction(layer_GetVideoSessionMemoryRequirementsKHR);
        if (string_eq(pName, "vkBindVideoSessionMemoryKHR")) return to_vkVoidFunction(layer_BindVideoSessionMemoryKHR);
        if (string_eq(pName, "vkCreateVideoSessionParametersKHR")) return to_vkVoidFunction(layer_CreateVideoSessionParametersKHR);
        if (string_eq(pName, "vkUpdateVideoSessionParametersKHR")) return to_vkVoidFunction(layer_UpdateVideoSessionParametersKHR);
        if (string_eq(pName, "vkDestroyVideoSessionParametersKHR")) return to_vkVoidFunction(layer_DestroyVideoSessionParametersKHR);
        if (string_eq(pName, "vkCmdBeginVideoCodingKHR")) return to_vkVoidFunction(layer_CmdBeginVideoCodingKHR);
        if (string_eq(pName, "vkCmdEndVideoCodingKHR")) return to_vkVoidFunction(layer_CmdEndVideoCodingKHR);
        if (string_eq(pName, "vkCmdControlVideoCodingKHR")) return to_vkVoidFunction(layer_CmdControlVideoCodingKHR);
    } // VK_KHR_video_queue
#endif // VK_ENABLE_BETA_EXTENSIONS

    // ----- VK_KHR_video_decode_queue
#ifdef VK_ENABLE_BETA_EXTENSIONS
    if (IsDeviceExtensionEnabled("VK_KHR_video_decode_queue")) {
        if (string_eq(pName, "vkCmdDecodeVideoKHR")) return to_vkVoidFunction(layer_CmdDecodeVideoKHR);
    } // VK_KHR_video_decode_queue
#endif // VK_ENABLE_BETA_EXTENSIONS

    // ----- VK_KHR_dynamic_rendering
    if (IsDeviceExtensionEnabled("VK_KHR_dynamic_rendering")) {
        if (string_eq(pName, "vkCmdBeginRenderingKHR")) return to_vkVoidFunction(layer_CmdBeginRenderingKHR);
        if (string_eq(pName, "vkCmdEndRenderingKHR")) return to_vkVoidFunction(layer_CmdEndRenderingKHR);
    } // VK_KHR_dynamic_rendering

    // ----- VK_KHR_get_physical_device_properties2
    if (string_eq(pName, "vkGetPhysicalDeviceFeatures2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceFeatures2KHR);
    if (string_eq(pName, "vkGetPhysicalDeviceProperties2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceProperties2KHR);
    if (string_eq(pName, "vkGetPhysicalDeviceFormatProperties2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceFormatProperties2KHR);
    if (string_eq(pName, "vkGetPhysicalDeviceImageFormatProperties2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceImageFormatProperties2KHR);
    if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyProperties2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceQueueFamilyProperties2KHR);
    if (string_eq(pName, "vkGetPhysicalDeviceMemoryProperties2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceMemoryProperties2KHR);
    if (string_eq(pName, "vkGetPhysicalDeviceSparseImageFormatProperties2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceSparseImageFormatProperties2KHR);

    // ----- VK_KHR_device_group
    if (IsDeviceExtensionEnabled("VK_KHR_device_group")) {
        if (string_eq(pName, "vkGetDeviceGroupPeerMemoryFeaturesKHR")) return to_vkVoidFunction(layer_GetDeviceGroupPeerMemoryFeaturesKHR);
        if (string_eq(pName, "vkCmdSetDeviceMaskKHR")) return to_vkVoidFunction(layer_CmdSetDeviceMaskKHR);
        if (string_eq(pName, "vkCmdDispatchBaseKHR")) return to_vkVoidFunction(layer_CmdDispatchBaseKHR);
    } // VK_KHR_device_group

    // ----- VK_KHR_maintenance1
    if (IsDeviceExtensionEnabled("VK_KHR_maintenance1")) {
        if (string_eq(pName, "vkTrimCommandPoolKHR")) return to_vkVoidFunction(layer_TrimCommandPoolKHR);
    } // VK_KHR_maintenance1

    // ----- VK_KHR_device_group_creation
    if (string_eq(pName, "vkEnumeratePhysicalDeviceGroupsKHR")) return to_vkVoidFunction(layer_EnumeratePhysicalDeviceGroupsKHR);

    // ----- VK_KHR_external_memory_capabilities
    if (string_eq(pName, "vkGetPhysicalDeviceExternalBufferPropertiesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceExternalBufferPropertiesKHR);

    // ----- VK_KHR_external_memory_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (IsDeviceExtensionEnabled("VK_KHR_external_memory_win32")) {
        if (string_eq(pName, "vkGetMemoryWin32HandleKHR")) return to_vkVoidFunction(layer_GetMemoryWin32HandleKHR);
        if (string_eq(pName, "vkGetMemoryWin32HandlePropertiesKHR")) return to_vkVoidFunction(layer_GetMemoryWin32HandlePropertiesKHR);
    } // VK_KHR_external_memory_win32
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_KHR_external_memory_fd
    if (IsDeviceExtensionEnabled("VK_KHR_external_memory_fd")) {
        if (string_eq(pName, "vkGetMemoryFdKHR")) return to_vkVoidFunction(layer_GetMemoryFdKHR);
        if (string_eq(pName, "vkGetMemoryFdPropertiesKHR")) return to_vkVoidFunction(layer_GetMemoryFdPropertiesKHR);
    } // VK_KHR_external_memory_fd

    // ----- VK_KHR_external_semaphore_capabilities
    if (string_eq(pName, "vkGetPhysicalDeviceExternalSemaphorePropertiesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceExternalSemaphorePropertiesKHR);

    // ----- VK_KHR_external_semaphore_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (IsDeviceExtensionEnabled("VK_KHR_external_semaphore_win32")) {
        if (string_eq(pName, "vkImportSemaphoreWin32HandleKHR")) return to_vkVoidFunction(layer_ImportSemaphoreWin32HandleKHR);
        if (string_eq(pName, "vkGetSemaphoreWin32HandleKHR")) return to_vkVoidFunction(layer_GetSemaphoreWin32HandleKHR);
    } // VK_KHR_external_semaphore_win32
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_KHR_external_semaphore_fd
    if (IsDeviceExtensionEnabled("VK_KHR_external_semaphore_fd")) {
        if (string_eq(pName, "vkImportSemaphoreFdKHR")) return to_vkVoidFunction(layer_ImportSemaphoreFdKHR);
        if (string_eq(pName, "vkGetSemaphoreFdKHR")) return to_vkVoidFunction(layer_GetSemaphoreFdKHR);
    } // VK_KHR_external_semaphore_fd

    // ----- VK_KHR_push_descriptor
    if (IsDeviceExtensionEnabled("VK_KHR_push_descriptor")) {
        if (string_eq(pName, "vkCmdPushDescriptorSetKHR")) return to_vkVoidFunction(layer_CmdPushDescriptorSetKHR);
        if (string_eq(pName, "vkCmdPushDescriptorSetWithTemplateKHR")) return to_vkVoidFunction(layer_CmdPushDescriptorSetWithTemplateKHR);
    } // VK_KHR_push_descriptor

    // ----- VK_KHR_descriptor_update_template
    if (IsDeviceExtensionEnabled("VK_KHR_descriptor_update_template")) {
        if (string_eq(pName, "vkCreateDescriptorUpdateTemplateKHR")) return to_vkVoidFunction(layer_CreateDescriptorUpdateTemplateKHR);
        if (string_eq(pName, "vkDestroyDescriptorUpdateTemplateKHR")) return to_vkVoidFunction(layer_DestroyDescriptorUpdateTemplateKHR);
        if (string_eq(pName, "vkUpdateDescriptorSetWithTemplateKHR")) return to_vkVoidFunction(layer_UpdateDescriptorSetWithTemplateKHR);
    } // VK_KHR_descriptor_update_template

    // ----- VK_KHR_create_renderpass2
    if (IsDeviceExtensionEnabled("VK_KHR_create_renderpass2")) {
        if (string_eq(pName, "vkCreateRenderPass2KHR")) return to_vkVoidFunction(layer_CreateRenderPass2KHR);
        if (string_eq(pName, "vkCmdBeginRenderPass2KHR")) return to_vkVoidFunction(layer_CmdBeginRenderPass2KHR);
        if (string_eq(pName, "vkCmdNextSubpass2KHR")) return to_vkVoidFunction(layer_CmdNextSubpass2KHR);
        if (string_eq(pName, "vkCmdEndRenderPass2KHR")) return to_vkVoidFunction(layer_CmdEndRenderPass2KHR);
    } // VK_KHR_create_renderpass2

    // ----- VK_KHR_shared_presentable_image
    if (IsDeviceExtensionEnabled("VK_KHR_shared_presentable_image")) {
        if (string_eq(pName, "vkGetSwapchainStatusKHR")) return to_vkVoidFunction(layer_GetSwapchainStatusKHR);
    } // VK_KHR_shared_presentable_image

    // ----- VK_KHR_external_fence_capabilities
    if (string_eq(pName, "vkGetPhysicalDeviceExternalFencePropertiesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceExternalFencePropertiesKHR);

    // ----- VK_KHR_external_fence_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (IsDeviceExtensionEnabled("VK_KHR_external_fence_win32")) {
        if (string_eq(pName, "vkImportFenceWin32HandleKHR")) return to_vkVoidFunction(layer_ImportFenceWin32HandleKHR);
        if (string_eq(pName, "vkGetFenceWin32HandleKHR")) return to_vkVoidFunction(layer_GetFenceWin32HandleKHR);
    } // VK_KHR_external_fence_win32
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_KHR_external_fence_fd
    if (IsDeviceExtensionEnabled("VK_KHR_external_fence_fd")) {
        if (string_eq(pName, "vkImportFenceFdKHR")) return to_vkVoidFunction(layer_ImportFenceFdKHR);
        if (string_eq(pName, "vkGetFenceFdKHR")) return to_vkVoidFunction(layer_GetFenceFdKHR);
    } // VK_KHR_external_fence_fd

    // ----- VK_KHR_performance_query
    if (IsDeviceExtensionEnabled("VK_KHR_performance_query")) {
        if (string_eq(pName, "vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR")) return to_vkVoidFunction(layer_EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR);
        if (string_eq(pName, "vkAcquireProfilingLockKHR")) return to_vkVoidFunction(layer_AcquireProfilingLockKHR);
        if (string_eq(pName, "vkReleaseProfilingLockKHR")) return to_vkVoidFunction(layer_ReleaseProfilingLockKHR);
    } // VK_KHR_performance_query

    // ----- VK_KHR_get_surface_capabilities2
    if (string_eq(pName, "vkGetPhysicalDeviceSurfaceCapabilities2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceSurfaceCapabilities2KHR);
    if (string_eq(pName, "vkGetPhysicalDeviceSurfaceFormats2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceSurfaceFormats2KHR);

    // ----- VK_KHR_get_display_properties2
    if (string_eq(pName, "vkGetPhysicalDeviceDisplayProperties2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceDisplayProperties2KHR);
    if (string_eq(pName, "vkGetPhysicalDeviceDisplayPlaneProperties2KHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceDisplayPlaneProperties2KHR);
    if (string_eq(pName, "vkGetDisplayModeProperties2KHR")) return to_vkVoidFunction(layer_GetDisplayModeProperties2KHR);
    if (string_eq(pName, "vkGetDisplayPlaneCapabilities2KHR")) return to_vkVoidFunction(layer_GetDisplayPlaneCapabilities2KHR);

    // ----- VK_KHR_get_memory_requirements2
    if (IsDeviceExtensionEnabled("VK_KHR_get_memory_requirements2")) {
        if (string_eq(pName, "vkGetImageMemoryRequirements2KHR")) return to_vkVoidFunction(layer_GetImageMemoryRequirements2KHR);
        if (string_eq(pName, "vkGetBufferMemoryRequirements2KHR")) return to_vkVoidFunction(layer_GetBufferMemoryRequirements2KHR);
        if (string_eq(pName, "vkGetImageSparseMemoryRequirements2KHR")) return to_vkVoidFunction(layer_GetImageSparseMemoryRequirements2KHR);
    } // VK_KHR_get_memory_requirements2

    // ----- VK_KHR_sampler_ycbcr_conversion
    if (IsDeviceExtensionEnabled("VK_KHR_sampler_ycbcr_conversion")) {
        if (string_eq(pName, "vkCreateSamplerYcbcrConversionKHR")) return to_vkVoidFunction(layer_CreateSamplerYcbcrConversionKHR);
        if (string_eq(pName, "vkDestroySamplerYcbcrConversionKHR")) return to_vkVoidFunction(layer_DestroySamplerYcbcrConversionKHR);
    } // VK_KHR_sampler_ycbcr_conversion

    // ----- VK_KHR_bind_memory2
    if (IsDeviceExtensionEnabled("VK_KHR_bind_memory2")) {
        if (string_eq(pName, "vkBindBufferMemory2KHR")) return to_vkVoidFunction(layer_BindBufferMemory2KHR);
        if (string_eq(pName, "vkBindImageMemory2KHR")) return to_vkVoidFunction(layer_BindImageMemory2KHR);
    } // VK_KHR_bind_memory2

    // ----- VK_KHR_maintenance3
    if (IsDeviceExtensionEnabled("VK_KHR_maintenance3")) {
        if (string_eq(pName, "vkGetDescriptorSetLayoutSupportKHR")) return to_vkVoidFunction(layer_GetDescriptorSetLayoutSupportKHR);
    } // VK_KHR_maintenance3

    // ----- VK_KHR_draw_indirect_count
    if (IsDeviceExtensionEnabled("VK_KHR_draw_indirect_count")) {
        if (string_eq(pName, "vkCmdDrawIndirectCountKHR")) return to_vkVoidFunction(layer_CmdDrawIndirectCountKHR);
        if (string_eq(pName, "vkCmdDrawIndexedIndirectCountKHR")) return to_vkVoidFunction(layer_CmdDrawIndexedIndirectCountKHR);
    } // VK_KHR_draw_indirect_count

    // ----- VK_KHR_timeline_semaphore
    if (IsDeviceExtensionEnabled("VK_KHR_timeline_semaphore")) {
        if (string_eq(pName, "vkGetSemaphoreCounterValueKHR")) return to_vkVoidFunction(layer_GetSemaphoreCounterValueKHR);
        if (string_eq(pName, "vkWaitSemaphoresKHR")) return to_vkVoidFunction(layer_WaitSemaphoresKHR);
        if (string_eq(pName, "vkSignalSemaphoreKHR")) return to_vkVoidFunction(layer_SignalSemaphoreKHR);
    } // VK_KHR_timeline_semaphore

    // ----- VK_KHR_fragment_shading_rate
    if (IsDeviceExtensionEnabled("VK_KHR_fragment_shading_rate")) {
        if (string_eq(pName, "vkGetPhysicalDeviceFragmentShadingRatesKHR")) return to_vkVoidFunction(layer_GetPhysicalDeviceFragmentShadingRatesKHR);
        if (string_eq(pName, "vkCmdSetFragmentShadingRateKHR")) return to_vkVoidFunction(layer_CmdSetFragmentShadingRateKHR);
    } // VK_KHR_fragment_shading_rate

    // ----- VK_KHR_present_wait
    if (IsDeviceExtensionEnabled("VK_KHR_present_wait")) {
        if (string_eq(pName, "vkWaitForPresentKHR")) return to_vkVoidFunction(layer_WaitForPresentKHR);
    } // VK_KHR_present_wait

    // ----- VK_KHR_buffer_device_address
    if (IsDeviceExtensionEnabled("VK_KHR_buffer_device_address")) {
        if (string_eq(pName, "vkGetBufferDeviceAddressKHR")) return to_vkVoidFunction(layer_GetBufferDeviceAddressKHR);
        if (string_eq(pName, "vkGetBufferOpaqueCaptureAddressKHR")) return to_vkVoidFunction(layer_GetBufferOpaqueCaptureAddressKHR);
        if (string_eq(pName, "vkGetDeviceMemoryOpaqueCaptureAddressKHR")) return to_vkVoidFunction(layer_GetDeviceMemoryOpaqueCaptureAddressKHR);
    } // VK_KHR_buffer_device_address

    // ----- VK_KHR_deferred_host_operations
    if (IsDeviceExtensionEnabled("VK_KHR_deferred_host_operations")) {
        if (string_eq(pName, "vkCreateDeferredOperationKHR")) return to_vkVoidFunction(layer_CreateDeferredOperationKHR);
        if (string_eq(pName, "vkDestroyDeferredOperationKHR")) return to_vkVoidFunction(layer_DestroyDeferredOperationKHR);
        if (string_eq(pName, "vkGetDeferredOperationMaxConcurrencyKHR")) return to_vkVoidFunction(layer_GetDeferredOperationMaxConcurrencyKHR);
        if (string_eq(pName, "vkGetDeferredOperationResultKHR")) return to_vkVoidFunction(layer_GetDeferredOperationResultKHR);
        if (string_eq(pName, "vkDeferredOperationJoinKHR")) return to_vkVoidFunction(layer_DeferredOperationJoinKHR);
    } // VK_KHR_deferred_host_operations

    // ----- VK_KHR_pipeline_executable_properties
    if (IsDeviceExtensionEnabled("VK_KHR_pipeline_executable_properties")) {
        if (string_eq(pName, "vkGetPipelineExecutablePropertiesKHR")) return to_vkVoidFunction(layer_GetPipelineExecutablePropertiesKHR);
        if (string_eq(pName, "vkGetPipelineExecutableStatisticsKHR")) return to_vkVoidFunction(layer_GetPipelineExecutableStatisticsKHR);
        if (string_eq(pName, "vkGetPipelineExecutableInternalRepresentationsKHR")) return to_vkVoidFunction(layer_GetPipelineExecutableInternalRepresentationsKHR);
    } // VK_KHR_pipeline_executable_properties

    // ----- VK_KHR_video_encode_queue
#ifdef VK_ENABLE_BETA_EXTENSIONS
    if (IsDeviceExtensionEnabled("VK_KHR_video_encode_queue")) {
        if (string_eq(pName, "vkCmdEncodeVideoKHR")) return to_vkVoidFunction(layer_CmdEncodeVideoKHR);
    } // VK_KHR_video_encode_queue
#endif // VK_ENABLE_BETA_EXTENSIONS

    // ----- VK_KHR_synchronization2
    if (IsDeviceExtensionEnabled("VK_KHR_synchronization2")) {
        if (string_eq(pName, "vkCmdSetEvent2KHR")) return to_vkVoidFunction(layer_CmdSetEvent2KHR);
        if (string_eq(pName, "vkCmdResetEvent2KHR")) return to_vkVoidFunction(layer_CmdResetEvent2KHR);
        if (string_eq(pName, "vkCmdWaitEvents2KHR")) return to_vkVoidFunction(layer_CmdWaitEvents2KHR);
        if (string_eq(pName, "vkCmdPipelineBarrier2KHR")) return to_vkVoidFunction(layer_CmdPipelineBarrier2KHR);
        if (string_eq(pName, "vkCmdWriteTimestamp2KHR")) return to_vkVoidFunction(layer_CmdWriteTimestamp2KHR);
        if (string_eq(pName, "vkQueueSubmit2KHR")) return to_vkVoidFunction(layer_QueueSubmit2KHR);
        if (string_eq(pName, "vkCmdWriteBufferMarker2AMD")) return to_vkVoidFunction(layer_CmdWriteBufferMarker2AMD);
        if (string_eq(pName, "vkGetQueueCheckpointData2NV")) return to_vkVoidFunction(layer_GetQueueCheckpointData2NV);
    } // VK_KHR_synchronization2

    // ----- VK_KHR_copy_commands2
    if (IsDeviceExtensionEnabled("VK_KHR_copy_commands2")) {
        if (string_eq(pName, "vkCmdCopyBuffer2KHR")) return to_vkVoidFunction(layer_CmdCopyBuffer2KHR);
        if (string_eq(pName, "vkCmdCopyImage2KHR")) return to_vkVoidFunction(layer_CmdCopyImage2KHR);
        if (string_eq(pName, "vkCmdCopyBufferToImage2KHR")) return to_vkVoidFunction(layer_CmdCopyBufferToImage2KHR);
        if (string_eq(pName, "vkCmdCopyImageToBuffer2KHR")) return to_vkVoidFunction(layer_CmdCopyImageToBuffer2KHR);
        if (string_eq(pName, "vkCmdBlitImage2KHR")) return to_vkVoidFunction(layer_CmdBlitImage2KHR);
        if (string_eq(pName, "vkCmdResolveImage2KHR")) return to_vkVoidFunction(layer_CmdResolveImage2KHR);
    } // VK_KHR_copy_commands2

    // ----- VK_KHR_maintenance4
    if (IsDeviceExtensionEnabled("VK_KHR_maintenance4")) {
        if (string_eq(pName, "vkGetDeviceBufferMemoryRequirementsKHR")) return to_vkVoidFunction(layer_GetDeviceBufferMemoryRequirementsKHR);
        if (string_eq(pName, "vkGetDeviceImageMemoryRequirementsKHR")) return to_vkVoidFunction(layer_GetDeviceImageMemoryRequirementsKHR);
        if (string_eq(pName, "vkGetDeviceImageSparseMemoryRequirementsKHR")) return to_vkVoidFunction(layer_GetDeviceImageSparseMemoryRequirementsKHR);
    } // VK_KHR_maintenance4

    // ----- VK_EXT_debug_report
    if (string_eq(pName, "vkCreateDebugReportCallbackEXT")) return to_vkVoidFunction(layer_CreateDebugReportCallbackEXT);
    if (string_eq(pName, "vkDestroyDebugReportCallbackEXT")) return to_vkVoidFunction(layer_DestroyDebugReportCallbackEXT);
    if (string_eq(pName, "vkDebugReportMessageEXT")) return to_vkVoidFunction(layer_DebugReportMessageEXT);

    // ----- VK_EXT_debug_marker
    if (IsDeviceExtensionEnabled("VK_EXT_debug_marker")) {
        if (string_eq(pName, "vkDebugMarkerSetObjectTagEXT")) return to_vkVoidFunction(layer_DebugMarkerSetObjectTagEXT);
        if (string_eq(pName, "vkDebugMarkerSetObjectNameEXT")) return to_vkVoidFunction(layer_DebugMarkerSetObjectNameEXT);
        if (string_eq(pName, "vkCmdDebugMarkerBeginEXT")) return to_vkVoidFunction(layer_CmdDebugMarkerBeginEXT);
        if (string_eq(pName, "vkCmdDebugMarkerEndEXT")) return to_vkVoidFunction(layer_CmdDebugMarkerEndEXT);
        if (string_eq(pName, "vkCmdDebugMarkerInsertEXT")) return to_vkVoidFunction(layer_CmdDebugMarkerInsertEXT);
    } // VK_EXT_debug_marker

    // ----- VK_EXT_transform_feedback
    if (IsDeviceExtensionEnabled("VK_EXT_transform_feedback")) {
        if (string_eq(pName, "vkCmdBindTransformFeedbackBuffersEXT")) return to_vkVoidFunction(layer_CmdBindTransformFeedbackBuffersEXT);
        if (string_eq(pName, "vkCmdBeginTransformFeedbackEXT")) return to_vkVoidFunction(layer_CmdBeginTransformFeedbackEXT);
        if (string_eq(pName, "vkCmdEndTransformFeedbackEXT")) return to_vkVoidFunction(layer_CmdEndTransformFeedbackEXT);
        if (string_eq(pName, "vkCmdBeginQueryIndexedEXT")) return to_vkVoidFunction(layer_CmdBeginQueryIndexedEXT);
        if (string_eq(pName, "vkCmdEndQueryIndexedEXT")) return to_vkVoidFunction(layer_CmdEndQueryIndexedEXT);
        if (string_eq(pName, "vkCmdDrawIndirectByteCountEXT")) return to_vkVoidFunction(layer_CmdDrawIndirectByteCountEXT);
    } // VK_EXT_transform_feedback

    // ----- VK_NVX_binary_import
    if (IsDeviceExtensionEnabled("VK_NVX_binary_import")) {
        if (string_eq(pName, "vkCreateCuModuleNVX")) return to_vkVoidFunction(layer_CreateCuModuleNVX);
        if (string_eq(pName, "vkCreateCuFunctionNVX")) return to_vkVoidFunction(layer_CreateCuFunctionNVX);
        if (string_eq(pName, "vkDestroyCuModuleNVX")) return to_vkVoidFunction(layer_DestroyCuModuleNVX);
        if (string_eq(pName, "vkDestroyCuFunctionNVX")) return to_vkVoidFunction(layer_DestroyCuFunctionNVX);
        if (string_eq(pName, "vkCmdCuLaunchKernelNVX")) return to_vkVoidFunction(layer_CmdCuLaunchKernelNVX);
    } // VK_NVX_binary_import

    // ----- VK_NVX_image_view_handle
    if (IsDeviceExtensionEnabled("VK_NVX_image_view_handle")) {
        if (string_eq(pName, "vkGetImageViewHandleNVX")) return to_vkVoidFunction(layer_GetImageViewHandleNVX);
        if (string_eq(pName, "vkGetImageViewAddressNVX")) return to_vkVoidFunction(layer_GetImageViewAddressNVX);
    } // VK_NVX_image_view_handle

    // ----- VK_AMD_draw_indirect_count
    if (IsDeviceExtensionEnabled("VK_AMD_draw_indirect_count")) {
        if (string_eq(pName, "vkCmdDrawIndirectCountAMD")) return to_vkVoidFunction(layer_CmdDrawIndirectCountAMD);
        if (string_eq(pName, "vkCmdDrawIndexedIndirectCountAMD")) return to_vkVoidFunction(layer_CmdDrawIndexedIndirectCountAMD);
    } // VK_AMD_draw_indirect_count

    // ----- VK_AMD_shader_info
    if (IsDeviceExtensionEnabled("VK_AMD_shader_info")) {
        if (string_eq(pName, "vkGetShaderInfoAMD")) return to_vkVoidFunction(layer_GetShaderInfoAMD);
    } // VK_AMD_shader_info

    // ----- VK_GGP_stream_descriptor_surface
#ifdef VK_USE_PLATFORM_GGP
    if (string_eq(pName, "vkCreateStreamDescriptorSurfaceGGP")) return to_vkVoidFunction(layer_CreateStreamDescriptorSurfaceGGP);
#endif // VK_USE_PLATFORM_GGP

    // ----- VK_NV_external_memory_capabilities
    if (string_eq(pName, "vkGetPhysicalDeviceExternalImageFormatPropertiesNV")) return to_vkVoidFunction(layer_GetPhysicalDeviceExternalImageFormatPropertiesNV);

    // ----- VK_NV_external_memory_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (IsDeviceExtensionEnabled("VK_NV_external_memory_win32")) {
        if (string_eq(pName, "vkGetMemoryWin32HandleNV")) return to_vkVoidFunction(layer_GetMemoryWin32HandleNV);
    } // VK_NV_external_memory_win32
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_NN_vi_surface
#ifdef VK_USE_PLATFORM_VI_NN
    if (string_eq(pName, "vkCreateViSurfaceNN")) return to_vkVoidFunction(layer_CreateViSurfaceNN);
#endif // VK_USE_PLATFORM_VI_NN

    // ----- VK_EXT_conditional_rendering
    if (IsDeviceExtensionEnabled("VK_EXT_conditional_rendering")) {
        if (string_eq(pName, "vkCmdBeginConditionalRenderingEXT")) return to_vkVoidFunction(layer_CmdBeginConditionalRenderingEXT);
        if (string_eq(pName, "vkCmdEndConditionalRenderingEXT")) return to_vkVoidFunction(layer_CmdEndConditionalRenderingEXT);
    } // VK_EXT_conditional_rendering

    // ----- VK_NV_clip_space_w_scaling
    if (IsDeviceExtensionEnabled("VK_NV_clip_space_w_scaling")) {
        if (string_eq(pName, "vkCmdSetViewportWScalingNV")) return to_vkVoidFunction(layer_CmdSetViewportWScalingNV);
    } // VK_NV_clip_space_w_scaling

    // ----- VK_EXT_direct_mode_display
    if (string_eq(pName, "vkReleaseDisplayEXT")) return to_vkVoidFunction(layer_ReleaseDisplayEXT);

    // ----- VK_EXT_acquire_xlib_display
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
    if (string_eq(pName, "vkAcquireXlibDisplayEXT")) return to_vkVoidFunction(layer_AcquireXlibDisplayEXT);
    if (string_eq(pName, "vkGetRandROutputDisplayEXT")) return to_vkVoidFunction(layer_GetRandROutputDisplayEXT);
#endif // VK_USE_PLATFORM_XLIB_XRANDR_EXT

    // ----- VK_EXT_display_surface_counter
    if (string_eq(pName, "vkGetPhysicalDeviceSurfaceCapabilities2EXT")) return to_vkVoidFunction(layer_GetPhysicalDeviceSurfaceCapabilities2EXT);

    // ----- VK_EXT_display_control
    if (IsDeviceExtensionEnabled("VK_EXT_display_control")) {
        if (string_eq(pName, "vkDisplayPowerControlEXT")) return to_vkVoidFunction(layer_DisplayPowerControlEXT);
        if (string_eq(pName, "vkRegisterDeviceEventEXT")) return to_vkVoidFunction(layer_RegisterDeviceEventEXT);
        if (string_eq(pName, "vkRegisterDisplayEventEXT")) return to_vkVoidFunction(layer_RegisterDisplayEventEXT);
        if (string_eq(pName, "vkGetSwapchainCounterEXT")) return to_vkVoidFunction(layer_GetSwapchainCounterEXT);
    } // VK_EXT_display_control

    // ----- VK_GOOGLE_display_timing
    if (IsDeviceExtensionEnabled("VK_GOOGLE_display_timing")) {
        if (string_eq(pName, "vkGetRefreshCycleDurationGOOGLE")) return to_vkVoidFunction(layer_GetRefreshCycleDurationGOOGLE);
        if (string_eq(pName, "vkGetPastPresentationTimingGOOGLE")) return to_vkVoidFunction(layer_GetPastPresentationTimingGOOGLE);
    } // VK_GOOGLE_display_timing

    // ----- VK_EXT_discard_rectangles
    if (IsDeviceExtensionEnabled("VK_EXT_discard_rectangles")) {
        if (string_eq(pName, "vkCmdSetDiscardRectangleEXT")) return to_vkVoidFunction(layer_CmdSetDiscardRectangleEXT);
    } // VK_EXT_discard_rectangles

    // ----- VK_EXT_hdr_metadata
    if (IsDeviceExtensionEnabled("VK_EXT_hdr_metadata")) {
        if (string_eq(pName, "vkSetHdrMetadataEXT")) return to_vkVoidFunction(layer_SetHdrMetadataEXT);
    } // VK_EXT_hdr_metadata

    // ----- VK_MVK_ios_surface
#ifdef VK_USE_PLATFORM_IOS_MVK
    if (string_eq(pName, "vkCreateIOSSurfaceMVK")) return to_vkVoidFunction(layer_CreateIOSSurfaceMVK);
#endif // VK_USE_PLATFORM_IOS_MVK

    // ----- VK_MVK_macos_surface
#ifdef VK_USE_PLATFORM_MACOS_MVK
    if (string_eq(pName, "vkCreateMacOSSurfaceMVK")) return to_vkVoidFunction(layer_CreateMacOSSurfaceMVK);
#endif // VK_USE_PLATFORM_MACOS_MVK

    // ----- VK_EXT_debug_utils
    if (string_eq(pName, "vkSetDebugUtilsObjectNameEXT")) return to_vkVoidFunction(layer_SetDebugUtilsObjectNameEXT);
    if (string_eq(pName, "vkSetDebugUtilsObjectTagEXT")) return to_vkVoidFunction(layer_SetDebugUtilsObjectTagEXT);
    if (string_eq(pName, "vkQueueBeginDebugUtilsLabelEXT")) return to_vkVoidFunction(layer_QueueBeginDebugUtilsLabelEXT);
    if (string_eq(pName, "vkQueueEndDebugUtilsLabelEXT")) return to_vkVoidFunction(layer_QueueEndDebugUtilsLabelEXT);
    if (string_eq(pName, "vkQueueInsertDebugUtilsLabelEXT")) return to_vkVoidFunction(layer_QueueInsertDebugUtilsLabelEXT);
    if (string_eq(pName, "vkCmdBeginDebugUtilsLabelEXT")) return to_vkVoidFunction(layer_CmdBeginDebugUtilsLabelEXT);
    if (string_eq(pName, "vkCmdEndDebugUtilsLabelEXT")) return to_vkVoidFunction(layer_CmdEndDebugUtilsLabelEXT);
    if (string_eq(pName, "vkCmdInsertDebugUtilsLabelEXT")) return to_vkVoidFunction(layer_CmdInsertDebugUtilsLabelEXT);
    if (string_eq(pName, "vkCreateDebugUtilsMessengerEXT")) return to_vkVoidFunction(layer_CreateDebugUtilsMessengerEXT);
    if (string_eq(pName, "vkDestroyDebugUtilsMessengerEXT")) return to_vkVoidFunction(layer_DestroyDebugUtilsMessengerEXT);
    if (string_eq(pName, "vkSubmitDebugUtilsMessageEXT")) return to_vkVoidFunction(layer_SubmitDebugUtilsMessageEXT);

    // ----- VK_EXT_sample_locations
    if (IsDeviceExtensionEnabled("VK_EXT_sample_locations")) {
        if (string_eq(pName, "vkCmdSetSampleLocationsEXT")) return to_vkVoidFunction(layer_CmdSetSampleLocationsEXT);
        if (string_eq(pName, "vkGetPhysicalDeviceMultisamplePropertiesEXT")) return to_vkVoidFunction(layer_GetPhysicalDeviceMultisamplePropertiesEXT);
    } // VK_EXT_sample_locations

    // ----- VK_EXT_image_drm_format_modifier
    if (IsDeviceExtensionEnabled("VK_EXT_image_drm_format_modifier")) {
        if (string_eq(pName, "vkGetImageDrmFormatModifierPropertiesEXT")) return to_vkVoidFunction(layer_GetImageDrmFormatModifierPropertiesEXT);
    } // VK_EXT_image_drm_format_modifier

    // ----- VK_EXT_validation_cache
    if (IsDeviceExtensionEnabled("VK_EXT_validation_cache")) {
        if (string_eq(pName, "vkCreateValidationCacheEXT")) return to_vkVoidFunction(layer_CreateValidationCacheEXT);
        if (string_eq(pName, "vkDestroyValidationCacheEXT")) return to_vkVoidFunction(layer_DestroyValidationCacheEXT);
        if (string_eq(pName, "vkMergeValidationCachesEXT")) return to_vkVoidFunction(layer_MergeValidationCachesEXT);
        if (string_eq(pName, "vkGetValidationCacheDataEXT")) return to_vkVoidFunction(layer_GetValidationCacheDataEXT);
    } // VK_EXT_validation_cache

    // ----- VK_NV_shading_rate_image
    if (IsDeviceExtensionEnabled("VK_NV_shading_rate_image")) {
        if (string_eq(pName, "vkCmdBindShadingRateImageNV")) return to_vkVoidFunction(layer_CmdBindShadingRateImageNV);
        if (string_eq(pName, "vkCmdSetViewportShadingRatePaletteNV")) return to_vkVoidFunction(layer_CmdSetViewportShadingRatePaletteNV);
        if (string_eq(pName, "vkCmdSetCoarseSampleOrderNV")) return to_vkVoidFunction(layer_CmdSetCoarseSampleOrderNV);
    } // VK_NV_shading_rate_image

    // ----- VK_NV_ray_tracing
    if (IsDeviceExtensionEnabled("VK_NV_ray_tracing")) {
        if (string_eq(pName, "vkCreateAccelerationStructureNV")) return to_vkVoidFunction(layer_CreateAccelerationStructureNV);
        if (string_eq(pName, "vkDestroyAccelerationStructureNV")) return to_vkVoidFunction(layer_DestroyAccelerationStructureNV);
        if (string_eq(pName, "vkGetAccelerationStructureMemoryRequirementsNV")) return to_vkVoidFunction(layer_GetAccelerationStructureMemoryRequirementsNV);
        if (string_eq(pName, "vkBindAccelerationStructureMemoryNV")) return to_vkVoidFunction(layer_BindAccelerationStructureMemoryNV);
        if (string_eq(pName, "vkCmdBuildAccelerationStructureNV")) return to_vkVoidFunction(layer_CmdBuildAccelerationStructureNV);
        if (string_eq(pName, "vkCmdCopyAccelerationStructureNV")) return to_vkVoidFunction(layer_CmdCopyAccelerationStructureNV);
        if (string_eq(pName, "vkCmdTraceRaysNV")) return to_vkVoidFunction(layer_CmdTraceRaysNV);
        if (string_eq(pName, "vkCreateRayTracingPipelinesNV")) return to_vkVoidFunction(layer_CreateRayTracingPipelinesNV);
        if (string_eq(pName, "vkGetRayTracingShaderGroupHandlesNV")) return to_vkVoidFunction(layer_GetRayTracingShaderGroupHandlesNV);
        if (string_eq(pName, "vkGetAccelerationStructureHandleNV")) return to_vkVoidFunction(layer_GetAccelerationStructureHandleNV);
        if (string_eq(pName, "vkCmdWriteAccelerationStructuresPropertiesNV")) return to_vkVoidFunction(layer_CmdWriteAccelerationStructuresPropertiesNV);
        if (string_eq(pName, "vkCompileDeferredNV")) return to_vkVoidFunction(layer_CompileDeferredNV);
    } // VK_NV_ray_tracing

    // ----- VK_EXT_external_memory_host
    if (IsDeviceExtensionEnabled("VK_EXT_external_memory_host")) {
        if (string_eq(pName, "vkGetMemoryHostPointerPropertiesEXT")) return to_vkVoidFunction(layer_GetMemoryHostPointerPropertiesEXT);
    } // VK_EXT_external_memory_host

    // ----- VK_AMD_buffer_marker
    if (IsDeviceExtensionEnabled("VK_AMD_buffer_marker")) {
        if (string_eq(pName, "vkCmdWriteBufferMarkerAMD")) return to_vkVoidFunction(layer_CmdWriteBufferMarkerAMD);
    } // VK_AMD_buffer_marker

    // ----- VK_EXT_calibrated_timestamps
    if (IsDeviceExtensionEnabled("VK_EXT_calibrated_timestamps")) {
        if (string_eq(pName, "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT")) return to_vkVoidFunction(layer_GetPhysicalDeviceCalibrateableTimeDomainsEXT);
        if (string_eq(pName, "vkGetCalibratedTimestampsEXT")) return to_vkVoidFunction(layer_GetCalibratedTimestampsEXT);
    } // VK_EXT_calibrated_timestamps

    // ----- VK_NV_mesh_shader
    if (IsDeviceExtensionEnabled("VK_NV_mesh_shader")) {
        if (string_eq(pName, "vkCmdDrawMeshTasksNV")) return to_vkVoidFunction(layer_CmdDrawMeshTasksNV);
        if (string_eq(pName, "vkCmdDrawMeshTasksIndirectNV")) return to_vkVoidFunction(layer_CmdDrawMeshTasksIndirectNV);
        if (string_eq(pName, "vkCmdDrawMeshTasksIndirectCountNV")) return to_vkVoidFunction(layer_CmdDrawMeshTasksIndirectCountNV);
    } // VK_NV_mesh_shader

    // ----- VK_NV_scissor_exclusive
    if (IsDeviceExtensionEnabled("VK_NV_scissor_exclusive")) {
        if (string_eq(pName, "vkCmdSetExclusiveScissorNV")) return to_vkVoidFunction(layer_CmdSetExclusiveScissorNV);
    } // VK_NV_scissor_exclusive

    // ----- VK_NV_device_diagnostic_checkpoints
    if (IsDeviceExtensionEnabled("VK_NV_device_diagnostic_checkpoints")) {
        if (string_eq(pName, "vkCmdSetCheckpointNV")) return to_vkVoidFunction(layer_CmdSetCheckpointNV);
        if (string_eq(pName, "vkGetQueueCheckpointDataNV")) return to_vkVoidFunction(layer_GetQueueCheckpointDataNV);
    } // VK_NV_device_diagnostic_checkpoints

    // ----- VK_INTEL_performance_query
    if (IsDeviceExtensionEnabled("VK_INTEL_performance_query")) {
        if (string_eq(pName, "vkInitializePerformanceApiINTEL")) return to_vkVoidFunction(layer_InitializePerformanceApiINTEL);
        if (string_eq(pName, "vkUninitializePerformanceApiINTEL")) return to_vkVoidFunction(layer_UninitializePerformanceApiINTEL);
        if (string_eq(pName, "vkCmdSetPerformanceMarkerINTEL")) return to_vkVoidFunction(layer_CmdSetPerformanceMarkerINTEL);
        if (string_eq(pName, "vkCmdSetPerformanceStreamMarkerINTEL")) return to_vkVoidFunction(layer_CmdSetPerformanceStreamMarkerINTEL);
        if (string_eq(pName, "vkCmdSetPerformanceOverrideINTEL")) return to_vkVoidFunction(layer_CmdSetPerformanceOverrideINTEL);
        if (string_eq(pName, "vkAcquirePerformanceConfigurationINTEL")) return to_vkVoidFunction(layer_AcquirePerformanceConfigurationINTEL);
        if (string_eq(pName, "vkReleasePerformanceConfigurationINTEL")) return to_vkVoidFunction(layer_ReleasePerformanceConfigurationINTEL);
        if (string_eq(pName, "vkQueueSetPerformanceConfigurationINTEL")) return to_vkVoidFunction(layer_QueueSetPerformanceConfigurationINTEL);
        if (string_eq(pName, "vkGetPerformanceParameterINTEL")) return to_vkVoidFunction(layer_GetPerformanceParameterINTEL);
    } // VK_INTEL_performance_query

    // ----- VK_AMD_display_native_hdr
    if (IsDeviceExtensionEnabled("VK_AMD_display_native_hdr")) {
        if (string_eq(pName, "vkSetLocalDimmingAMD")) return to_vkVoidFunction(layer_SetLocalDimmingAMD);
    } // VK_AMD_display_native_hdr

    // ----- VK_FUCHSIA_imagepipe_surface
#ifdef VK_USE_PLATFORM_FUCHSIA
    if (string_eq(pName, "vkCreateImagePipeSurfaceFUCHSIA")) return to_vkVoidFunction(layer_CreateImagePipeSurfaceFUCHSIA);
#endif // VK_USE_PLATFORM_FUCHSIA

    // ----- VK_EXT_metal_surface
#ifdef VK_USE_PLATFORM_METAL_EXT
    if (string_eq(pName, "vkCreateMetalSurfaceEXT")) return to_vkVoidFunction(layer_CreateMetalSurfaceEXT);
#endif // VK_USE_PLATFORM_METAL_EXT

    // ----- VK_EXT_buffer_device_address
    if (IsDeviceExtensionEnabled("VK_EXT_buffer_device_address")) {
        if (string_eq(pName, "vkGetBufferDeviceAddressEXT")) return to_vkVoidFunction(layer_GetBufferDeviceAddressEXT);
    } // VK_EXT_buffer_device_address

    // ----- VK_EXT_tooling_info
    if (IsDeviceExtensionEnabled("VK_EXT_tooling_info")) {
        if (string_eq(pName, "vkGetPhysicalDeviceToolPropertiesEXT")) return to_vkVoidFunction(layer_GetPhysicalDeviceToolPropertiesEXT);
    } // VK_EXT_tooling_info

    // ----- VK_NV_cooperative_matrix
    if (IsDeviceExtensionEnabled("VK_NV_cooperative_matrix")) {
        if (string_eq(pName, "vkGetPhysicalDeviceCooperativeMatrixPropertiesNV")) return to_vkVoidFunction(layer_GetPhysicalDeviceCooperativeMatrixPropertiesNV);
    } // VK_NV_cooperative_matrix

    // ----- VK_NV_coverage_reduction_mode
    if (IsDeviceExtensionEnabled("VK_NV_coverage_reduction_mode")) {
        if (string_eq(pName, "vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV")) return to_vkVoidFunction(layer_GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV);
    } // VK_NV_coverage_reduction_mode

    // ----- VK_EXT_full_screen_exclusive
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (IsDeviceExtensionEnabled("VK_EXT_full_screen_exclusive")) {
        if (string_eq(pName, "vkGetPhysicalDeviceSurfacePresentModes2EXT")) return to_vkVoidFunction(layer_GetPhysicalDeviceSurfacePresentModes2EXT);
        if (string_eq(pName, "vkAcquireFullScreenExclusiveModeEXT")) return to_vkVoidFunction(layer_AcquireFullScreenExclusiveModeEXT);
        if (string_eq(pName, "vkReleaseFullScreenExclusiveModeEXT")) return to_vkVoidFunction(layer_ReleaseFullScreenExclusiveModeEXT);
        if (string_eq(pName, "vkGetDeviceGroupSurfacePresentModes2EXT")) return to_vkVoidFunction(layer_GetDeviceGroupSurfacePresentModes2EXT);
    } // VK_EXT_full_screen_exclusive
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_EXT_headless_surface
    if (string_eq(pName, "vkCreateHeadlessSurfaceEXT")) return to_vkVoidFunction(layer_CreateHeadlessSurfaceEXT);

    // ----- VK_EXT_line_rasterization
    if (IsDeviceExtensionEnabled("VK_EXT_line_rasterization")) {
        if (string_eq(pName, "vkCmdSetLineStippleEXT")) return to_vkVoidFunction(layer_CmdSetLineStippleEXT);
    } // VK_EXT_line_rasterization

    // ----- VK_EXT_host_query_reset
    if (IsDeviceExtensionEnabled("VK_EXT_host_query_reset")) {
        if (string_eq(pName, "vkResetQueryPoolEXT")) return to_vkVoidFunction(layer_ResetQueryPoolEXT);
    } // VK_EXT_host_query_reset

    // ----- VK_EXT_extended_dynamic_state
    if (IsDeviceExtensionEnabled("VK_EXT_extended_dynamic_state")) {
        if (string_eq(pName, "vkCmdSetCullModeEXT")) return to_vkVoidFunction(layer_CmdSetCullModeEXT);
        if (string_eq(pName, "vkCmdSetFrontFaceEXT")) return to_vkVoidFunction(layer_CmdSetFrontFaceEXT);
        if (string_eq(pName, "vkCmdSetPrimitiveTopologyEXT")) return to_vkVoidFunction(layer_CmdSetPrimitiveTopologyEXT);
        if (string_eq(pName, "vkCmdSetViewportWithCountEXT")) return to_vkVoidFunction(layer_CmdSetViewportWithCountEXT);
        if (string_eq(pName, "vkCmdSetScissorWithCountEXT")) return to_vkVoidFunction(layer_CmdSetScissorWithCountEXT);
        if (string_eq(pName, "vkCmdBindVertexBuffers2EXT")) return to_vkVoidFunction(layer_CmdBindVertexBuffers2EXT);
        if (string_eq(pName, "vkCmdSetDepthTestEnableEXT")) return to_vkVoidFunction(layer_CmdSetDepthTestEnableEXT);
        if (string_eq(pName, "vkCmdSetDepthWriteEnableEXT")) return to_vkVoidFunction(layer_CmdSetDepthWriteEnableEXT);
        if (string_eq(pName, "vkCmdSetDepthCompareOpEXT")) return to_vkVoidFunction(layer_CmdSetDepthCompareOpEXT);
        if (string_eq(pName, "vkCmdSetDepthBoundsTestEnableEXT")) return to_vkVoidFunction(layer_CmdSetDepthBoundsTestEnableEXT);
        if (string_eq(pName, "vkCmdSetStencilTestEnableEXT")) return to_vkVoidFunction(layer_CmdSetStencilTestEnableEXT);
        if (string_eq(pName, "vkCmdSetStencilOpEXT")) return to_vkVoidFunction(layer_CmdSetStencilOpEXT);
    } // VK_EXT_extended_dynamic_state

    // ----- VK_NV_device_generated_commands
    if (IsDeviceExtensionEnabled("VK_NV_device_generated_commands")) {
        if (string_eq(pName, "vkGetGeneratedCommandsMemoryRequirementsNV")) return to_vkVoidFunction(layer_GetGeneratedCommandsMemoryRequirementsNV);
        if (string_eq(pName, "vkCmdPreprocessGeneratedCommandsNV")) return to_vkVoidFunction(layer_CmdPreprocessGeneratedCommandsNV);
        if (string_eq(pName, "vkCmdExecuteGeneratedCommandsNV")) return to_vkVoidFunction(layer_CmdExecuteGeneratedCommandsNV);
        if (string_eq(pName, "vkCmdBindPipelineShaderGroupNV")) return to_vkVoidFunction(layer_CmdBindPipelineShaderGroupNV);
        if (string_eq(pName, "vkCreateIndirectCommandsLayoutNV")) return to_vkVoidFunction(layer_CreateIndirectCommandsLayoutNV);
        if (string_eq(pName, "vkDestroyIndirectCommandsLayoutNV")) return to_vkVoidFunction(layer_DestroyIndirectCommandsLayoutNV);
    } // VK_NV_device_generated_commands

    // ----- VK_EXT_acquire_drm_display
    if (string_eq(pName, "vkAcquireDrmDisplayEXT")) return to_vkVoidFunction(layer_AcquireDrmDisplayEXT);
    if (string_eq(pName, "vkGetDrmDisplayEXT")) return to_vkVoidFunction(layer_GetDrmDisplayEXT);

    // ----- VK_EXT_private_data
    if (IsDeviceExtensionEnabled("VK_EXT_private_data")) {
        if (string_eq(pName, "vkCreatePrivateDataSlotEXT")) return to_vkVoidFunction(layer_CreatePrivateDataSlotEXT);
        if (string_eq(pName, "vkDestroyPrivateDataSlotEXT")) return to_vkVoidFunction(layer_DestroyPrivateDataSlotEXT);
        if (string_eq(pName, "vkSetPrivateDataEXT")) return to_vkVoidFunction(layer_SetPrivateDataEXT);
        if (string_eq(pName, "vkGetPrivateDataEXT")) return to_vkVoidFunction(layer_GetPrivateDataEXT);
    } // VK_EXT_private_data

    // ----- VK_NV_fragment_shading_rate_enums
    if (IsDeviceExtensionEnabled("VK_NV_fragment_shading_rate_enums")) {
        if (string_eq(pName, "vkCmdSetFragmentShadingRateEnumNV")) return to_vkVoidFunction(layer_CmdSetFragmentShadingRateEnumNV);
    } // VK_NV_fragment_shading_rate_enums

    // ----- VK_NV_acquire_winrt_display
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (IsDeviceExtensionEnabled("VK_NV_acquire_winrt_display")) {
        if (string_eq(pName, "vkAcquireWinrtDisplayNV")) return to_vkVoidFunction(layer_AcquireWinrtDisplayNV);
        if (string_eq(pName, "vkGetWinrtDisplayNV")) return to_vkVoidFunction(layer_GetWinrtDisplayNV);
    } // VK_NV_acquire_winrt_display
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_EXT_directfb_surface
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
    if (string_eq(pName, "vkCreateDirectFBSurfaceEXT")) return to_vkVoidFunction(layer_CreateDirectFBSurfaceEXT);
    if (string_eq(pName, "vkGetPhysicalDeviceDirectFBPresentationSupportEXT")) return to_vkVoidFunction(layer_GetPhysicalDeviceDirectFBPresentationSupportEXT);
#endif // VK_USE_PLATFORM_DIRECTFB_EXT

    // ----- VK_EXT_vertex_input_dynamic_state
    if (IsDeviceExtensionEnabled("VK_EXT_vertex_input_dynamic_state")) {
        if (string_eq(pName, "vkCmdSetVertexInputEXT")) return to_vkVoidFunction(layer_CmdSetVertexInputEXT);
    } // VK_EXT_vertex_input_dynamic_state

    // ----- VK_FUCHSIA_external_memory
#ifdef VK_USE_PLATFORM_FUCHSIA
    if (IsDeviceExtensionEnabled("VK_FUCHSIA_external_memory")) {
        if (string_eq(pName, "vkGetMemoryZirconHandleFUCHSIA")) return to_vkVoidFunction(layer_GetMemoryZirconHandleFUCHSIA);
        if (string_eq(pName, "vkGetMemoryZirconHandlePropertiesFUCHSIA")) return to_vkVoidFunction(layer_GetMemoryZirconHandlePropertiesFUCHSIA);
    } // VK_FUCHSIA_external_memory
#endif // VK_USE_PLATFORM_FUCHSIA

    // ----- VK_FUCHSIA_external_semaphore
#ifdef VK_USE_PLATFORM_FUCHSIA
    if (IsDeviceExtensionEnabled("VK_FUCHSIA_external_semaphore")) {
        if (string_eq(pName, "vkImportSemaphoreZirconHandleFUCHSIA")) return to_vkVoidFunction(layer_ImportSemaphoreZirconHandleFUCHSIA);
        if (string_eq(pName, "vkGetSemaphoreZirconHandleFUCHSIA")) return to_vkVoidFunction(layer_GetSemaphoreZirconHandleFUCHSIA);
    } // VK_FUCHSIA_external_semaphore
#endif // VK_USE_PLATFORM_FUCHSIA

    // ----- VK_FUCHSIA_buffer_collection
#ifdef VK_USE_PLATFORM_FUCHSIA
    if (IsDeviceExtensionEnabled("VK_FUCHSIA_buffer_collection")) {
        if (string_eq(pName, "vkCreateBufferCollectionFUCHSIA")) return to_vkVoidFunction(layer_CreateBufferCollectionFUCHSIA);
        if (string_eq(pName, "vkSetBufferCollectionImageConstraintsFUCHSIA")) return to_vkVoidFunction(layer_SetBufferCollectionImageConstraintsFUCHSIA);
        if (string_eq(pName, "vkSetBufferCollectionBufferConstraintsFUCHSIA")) return to_vkVoidFunction(layer_SetBufferCollectionBufferConstraintsFUCHSIA);
        if (string_eq(pName, "vkDestroyBufferCollectionFUCHSIA")) return to_vkVoidFunction(layer_DestroyBufferCollectionFUCHSIA);
        if (string_eq(pName, "vkGetBufferCollectionPropertiesFUCHSIA")) return to_vkVoidFunction(layer_GetBufferCollectionPropertiesFUCHSIA);
    } // VK_FUCHSIA_buffer_collection
#endif // VK_USE_PLATFORM_FUCHSIA

    // ----- VK_HUAWEI_subpass_shading
    if (IsDeviceExtensionEnabled("VK_HUAWEI_subpass_shading")) {
        if (string_eq(pName, "vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI")) return to_vkVoidFunction(layer_GetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI);
        if (string_eq(pName, "vkCmdSubpassShadingHUAWEI")) return to_vkVoidFunction(layer_CmdSubpassShadingHUAWEI);
    } // VK_HUAWEI_subpass_shading

    // ----- VK_HUAWEI_invocation_mask
    if (IsDeviceExtensionEnabled("VK_HUAWEI_invocation_mask")) {
        if (string_eq(pName, "vkCmdBindInvocationMaskHUAWEI")) return to_vkVoidFunction(layer_CmdBindInvocationMaskHUAWEI);
    } // VK_HUAWEI_invocation_mask

    // ----- VK_NV_external_memory_rdma
    if (IsDeviceExtensionEnabled("VK_NV_external_memory_rdma")) {
        if (string_eq(pName, "vkGetMemoryRemoteAddressNV")) return to_vkVoidFunction(layer_GetMemoryRemoteAddressNV);
    } // VK_NV_external_memory_rdma

    // ----- VK_EXT_extended_dynamic_state2
    if (IsDeviceExtensionEnabled("VK_EXT_extended_dynamic_state2")) {
        if (string_eq(pName, "vkCmdSetPatchControlPointsEXT")) return to_vkVoidFunction(layer_CmdSetPatchControlPointsEXT);
        if (string_eq(pName, "vkCmdSetRasterizerDiscardEnableEXT")) return to_vkVoidFunction(layer_CmdSetRasterizerDiscardEnableEXT);
        if (string_eq(pName, "vkCmdSetDepthBiasEnableEXT")) return to_vkVoidFunction(layer_CmdSetDepthBiasEnableEXT);
        if (string_eq(pName, "vkCmdSetLogicOpEXT")) return to_vkVoidFunction(layer_CmdSetLogicOpEXT);
        if (string_eq(pName, "vkCmdSetPrimitiveRestartEnableEXT")) return to_vkVoidFunction(layer_CmdSetPrimitiveRestartEnableEXT);
    } // VK_EXT_extended_dynamic_state2

    // ----- VK_QNX_screen_surface
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    if (string_eq(pName, "vkCreateScreenSurfaceQNX")) return to_vkVoidFunction(layer_CreateScreenSurfaceQNX);
    if (string_eq(pName, "vkGetPhysicalDeviceScreenPresentationSupportQNX")) return to_vkVoidFunction(layer_GetPhysicalDeviceScreenPresentationSupportQNX);
#endif // VK_USE_PLATFORM_SCREEN_QNX

    // ----- VK_EXT_color_write_enable
    if (IsDeviceExtensionEnabled("VK_EXT_color_write_enable")) {
        if (string_eq(pName, "vkCmdSetColorWriteEnableEXT")) return to_vkVoidFunction(layer_CmdSetColorWriteEnableEXT);
    } // VK_EXT_color_write_enable

    // ----- VK_EXT_multi_draw
    if (IsDeviceExtensionEnabled("VK_EXT_multi_draw")) {
        if (string_eq(pName, "vkCmdDrawMultiEXT")) return to_vkVoidFunction(layer_CmdDrawMultiEXT);
        if (string_eq(pName, "vkCmdDrawMultiIndexedEXT")) return to_vkVoidFunction(layer_CmdDrawMultiIndexedEXT);
    } // VK_EXT_multi_draw

    // ----- VK_EXT_pageable_device_local_memory
    if (IsDeviceExtensionEnabled("VK_EXT_pageable_device_local_memory")) {
        if (string_eq(pName, "vkSetDeviceMemoryPriorityEXT")) return to_vkVoidFunction(layer_SetDeviceMemoryPriorityEXT);
    } // VK_EXT_pageable_device_local_memory

    // ----- VK_VALVE_descriptor_set_host_mapping
    if (IsDeviceExtensionEnabled("VK_VALVE_descriptor_set_host_mapping")) {
        if (string_eq(pName, "vkGetDescriptorSetLayoutHostMappingInfoVALVE")) return to_vkVoidFunction(layer_GetDescriptorSetLayoutHostMappingInfoVALVE);
        if (string_eq(pName, "vkGetDescriptorSetHostMappingVALVE")) return to_vkVoidFunction(layer_GetDescriptorSetHostMappingVALVE);
    } // VK_VALVE_descriptor_set_host_mapping

    // ----- VK_KHR_acceleration_structure
    if (IsDeviceExtensionEnabled("VK_KHR_acceleration_structure")) {
        if (string_eq(pName, "vkCreateAccelerationStructureKHR")) return to_vkVoidFunction(layer_CreateAccelerationStructureKHR);
        if (string_eq(pName, "vkDestroyAccelerationStructureKHR")) return to_vkVoidFunction(layer_DestroyAccelerationStructureKHR);
        if (string_eq(pName, "vkCmdBuildAccelerationStructuresKHR")) return to_vkVoidFunction(layer_CmdBuildAccelerationStructuresKHR);
        if (string_eq(pName, "vkCmdBuildAccelerationStructuresIndirectKHR")) return to_vkVoidFunction(layer_CmdBuildAccelerationStructuresIndirectKHR);
        if (string_eq(pName, "vkBuildAccelerationStructuresKHR")) return to_vkVoidFunction(layer_BuildAccelerationStructuresKHR);
        if (string_eq(pName, "vkCopyAccelerationStructureKHR")) return to_vkVoidFunction(layer_CopyAccelerationStructureKHR);
        if (string_eq(pName, "vkCopyAccelerationStructureToMemoryKHR")) return to_vkVoidFunction(layer_CopyAccelerationStructureToMemoryKHR);
        if (string_eq(pName, "vkCopyMemoryToAccelerationStructureKHR")) return to_vkVoidFunction(layer_CopyMemoryToAccelerationStructureKHR);
        if (string_eq(pName, "vkWriteAccelerationStructuresPropertiesKHR")) return to_vkVoidFunction(layer_WriteAccelerationStructuresPropertiesKHR);
        if (string_eq(pName, "vkCmdCopyAccelerationStructureKHR")) return to_vkVoidFunction(layer_CmdCopyAccelerationStructureKHR);
        if (string_eq(pName, "vkCmdCopyAccelerationStructureToMemoryKHR")) return to_vkVoidFunction(layer_CmdCopyAccelerationStructureToMemoryKHR);
        if (string_eq(pName, "vkCmdCopyMemoryToAccelerationStructureKHR")) return to_vkVoidFunction(layer_CmdCopyMemoryToAccelerationStructureKHR);
        if (string_eq(pName, "vkGetAccelerationStructureDeviceAddressKHR")) return to_vkVoidFunction(layer_GetAccelerationStructureDeviceAddressKHR);
        if (string_eq(pName, "vkCmdWriteAccelerationStructuresPropertiesKHR")) return to_vkVoidFunction(layer_CmdWriteAccelerationStructuresPropertiesKHR);
        if (string_eq(pName, "vkGetDeviceAccelerationStructureCompatibilityKHR")) return to_vkVoidFunction(layer_GetDeviceAccelerationStructureCompatibilityKHR);
        if (string_eq(pName, "vkGetAccelerationStructureBuildSizesKHR")) return to_vkVoidFunction(layer_GetAccelerationStructureBuildSizesKHR);
    } // VK_KHR_acceleration_structure

    // ----- VK_KHR_ray_tracing_pipeline
    if (IsDeviceExtensionEnabled("VK_KHR_ray_tracing_pipeline")) {
        if (string_eq(pName, "vkCmdTraceRaysKHR")) return to_vkVoidFunction(layer_CmdTraceRaysKHR);
        if (string_eq(pName, "vkCreateRayTracingPipelinesKHR")) return to_vkVoidFunction(layer_CreateRayTracingPipelinesKHR);
        if (string_eq(pName, "vkGetRayTracingShaderGroupHandlesKHR")) return to_vkVoidFunction(layer_GetRayTracingShaderGroupHandlesKHR);
        if (string_eq(pName, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR")) return to_vkVoidFunction(layer_GetRayTracingCaptureReplayShaderGroupHandlesKHR);
        if (string_eq(pName, "vkCmdTraceRaysIndirectKHR")) return to_vkVoidFunction(layer_CmdTraceRaysIndirectKHR);
        if (string_eq(pName, "vkGetRayTracingShaderGroupStackSizeKHR")) return to_vkVoidFunction(layer_GetRayTracingShaderGroupStackSizeKHR);
        if (string_eq(pName, "vkCmdSetRayTracingPipelineStackSizeKHR")) return to_vkVoidFunction(layer_CmdSetRayTracingPipelineStackSizeKHR);
    } // VK_KHR_ray_tracing_pipeline

    return layer.created_devices[0].dispatch_table.GetDeviceProcAddr(device, pName);
}


#if (defined(__GNUC__) && (__GNUC__ >= 4)) || (defined(__SUNPRO_C) && (__SUNPRO_C >= 0x590))
#define EXPORT_NEGOTIATE_FUNCTION __attribute__((visibility("default")))
#else
#define EXPORT_NEGOTIATE_FUNCTION
#endif

EXPORT_NEGOTIATE_FUNCTION VKAPI_ATTR VkResult VKAPI_CALL
vkNegotiateLoaderLayerInterfaceVersion(VkNegotiateLayerInterface* pVersionStruct) {
    if (pVersionStruct) {
        pVersionStruct->loaderLayerInterfaceVersion = layer.max_icd_interface_version;
        pVersionStruct->pfnGetInstanceProcAddr = layer_GetInstanceProcAddr;
        pVersionStruct->pfnGetDeviceProcAddr = layer_GetDeviceProcAddr;
        pVersionStruct->pfnGetPhysicalDeviceProcAddr = nullptr;

        return VK_SUCCESS;
    }
    return VK_ERROR_INITIALIZATION_FAILED;
}

}  // extern "C"
// clang-format on

