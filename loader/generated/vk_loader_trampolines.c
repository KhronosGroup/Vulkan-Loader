// *** THIS FILE IS GENERATED - DO NOT EDIT ***
// See loader_tramp_term_generator.py for modifications

/*
 * Copyright (c) 2015-2022 The Khronos Group Inc.
 * Copyright (c) 2015-2022 Valve Corporation
 * Copyright (c) 2015-2022 LunarG, Inc.
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
 * Author: Mark Lobodzinski <mark@lunarg.com>
 * Author: Mark Young <marky@lunarg.com>
 */

// clang-format off
#include <stdlib.h>
#include <string.h>

#include "allocation.h"
#include "debug_utils.h"
#include "gpa_helper.h"
#include "loader.h"
#include "log.h"
#include "vk_loader_platform.h"
#include "vk_loader_extension_utils.h"
#include "wsi.h"
#include <vulkan/vk_icd.h>


// --------------------------------------------
// ---- Vulkan API version 1.0 trampolines ----
// --------------------------------------------


LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFeatures(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceFeatures*                   pFeatures) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceFeatures: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceFeatures-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    disp->GetPhysicalDeviceFeatures(unwrapped_phys_dev, pFeatures);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFormatProperties(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkFormatProperties*                         pFormatProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceFormatProperties: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceFormatProperties-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    disp->GetPhysicalDeviceFormatProperties(unwrapped_phys_dev, format, pFormatProperties);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceImageFormatProperties(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkImageType                                 type,
    VkImageTiling                               tiling,
    VkImageUsageFlags                           usage,
    VkImageCreateFlags                          flags,
    VkImageFormatProperties*                    pImageFormatProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceImageFormatProperties: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceImageFormatProperties-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceImageFormatProperties(unwrapped_phys_dev, format, type, tiling, usage, flags, pImageFormatProperties);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceProperties*                 pProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceProperties: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceProperties-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    disp->GetPhysicalDeviceProperties(unwrapped_phys_dev, pProperties);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pQueueFamilyPropertyCount,
    VkQueueFamilyProperties*                    pQueueFamilyProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceQueueFamilyProperties: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceQueueFamilyProperties-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    disp->GetPhysicalDeviceQueueFamilyProperties(unwrapped_phys_dev, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceMemoryProperties(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceMemoryProperties*           pMemoryProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceMemoryProperties: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceMemoryProperties-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    disp->GetPhysicalDeviceMemoryProperties(unwrapped_phys_dev, pMemoryProperties);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(
    VkPhysicalDevice                            physicalDevice,
    const char*                                 pLayerName,
    uint32_t*                                   pPropertyCount,
    VkExtensionProperties*                      pProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkEnumerateDeviceExtensionProperties: Invalid VkPhysicalDevice [VUID-vkEnumerateDeviceExtensionProperties-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->EnumerateDeviceExtensionProperties(unwrapped_phys_dev, pLayerName, pPropertyCount, pProperties);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceSparseImageFormatProperties(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkImageType                                 type,
    VkSampleCountFlagBits                       samples,
    VkImageUsageFlags                           usage,
    VkImageTiling                               tiling,
    uint32_t*                                   pPropertyCount,
    VkSparseImageFormatProperties*              pProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceSparseImageFormatProperties: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceSparseImageFormatProperties-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    disp->GetPhysicalDeviceSparseImageFormatProperties(unwrapped_phys_dev, format, type, samples, usage, tiling, pPropertyCount, pProperties);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetDeviceQueue(
    VkDevice                                    device,
    uint32_t                                    queueFamilyIndex,
    uint32_t                                    queueIndex,
    VkQueue*                                    pQueue) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->GetDeviceQueue(device, queueFamilyIndex, queueIndex, pQueue);
    if (pQueue != NULL && *pQueue != NULL) {
        loader_set_dispatch(*pQueue, disp);
    }
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkQueueSubmit(
    VkQueue                                     queue,
    uint32_t                                    submitCount,
    const VkSubmitInfo*                         pSubmits,
    VkFence                                     fence) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(queue);
    return disp->QueueSubmit(queue, submitCount, pSubmits, fence);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkQueueWaitIdle(
    VkQueue                                     queue) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(queue);
    return disp->QueueWaitIdle(queue);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkDeviceWaitIdle(
    VkDevice                                    device) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->DeviceWaitIdle(device);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkAllocateMemory(
    VkDevice                                    device,
    const VkMemoryAllocateInfo*                 pAllocateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDeviceMemory*                             pMemory) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->AllocateMemory(device, pAllocateInfo, pAllocator, pMemory);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkFreeMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->FreeMemory(device, memory, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkMapMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkDeviceSize                                offset,
    VkDeviceSize                                size,
    VkMemoryMapFlags                            flags,
    void**                                      ppData) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->MapMemory(device, memory, offset, size, flags, ppData);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkUnmapMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->UnmapMemory(device, memory);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkFlushMappedMemoryRanges(
    VkDevice                                    device,
    uint32_t                                    memoryRangeCount,
    const VkMappedMemoryRange*                  pMemoryRanges) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->FlushMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkInvalidateMappedMemoryRanges(
    VkDevice                                    device,
    uint32_t                                    memoryRangeCount,
    const VkMappedMemoryRange*                  pMemoryRanges) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->InvalidateMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetDeviceMemoryCommitment(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkDeviceSize*                               pCommittedMemoryInBytes) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->GetDeviceMemoryCommitment(device, memory, pCommittedMemoryInBytes);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkBindBufferMemory(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    VkDeviceMemory                              memory,
    VkDeviceSize                                memoryOffset) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->BindBufferMemory(device, buffer, memory, memoryOffset);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkBindImageMemory(
    VkDevice                                    device,
    VkImage                                     image,
    VkDeviceMemory                              memory,
    VkDeviceSize                                memoryOffset) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->BindImageMemory(device, image, memory, memoryOffset);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetBufferMemoryRequirements(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    VkMemoryRequirements*                       pMemoryRequirements) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->GetBufferMemoryRequirements(device, buffer, pMemoryRequirements);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetImageMemoryRequirements(
    VkDevice                                    device,
    VkImage                                     image,
    VkMemoryRequirements*                       pMemoryRequirements) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->GetImageMemoryRequirements(device, image, pMemoryRequirements);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetImageSparseMemoryRequirements(
    VkDevice                                    device,
    VkImage                                     image,
    uint32_t*                                   pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements*            pSparseMemoryRequirements) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->GetImageSparseMemoryRequirements(device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkQueueBindSparse(
    VkQueue                                     queue,
    uint32_t                                    bindInfoCount,
    const VkBindSparseInfo*                     pBindInfo,
    VkFence                                     fence) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(queue);
    return disp->QueueBindSparse(queue, bindInfoCount, pBindInfo, fence);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateFence(
    VkDevice                                    device,
    const VkFenceCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreateFence(device, pCreateInfo, pAllocator, pFence);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyFence(
    VkDevice                                    device,
    VkFence                                     fence,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroyFence(device, fence, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkResetFences(
    VkDevice                                    device,
    uint32_t                                    fenceCount,
    const VkFence*                              pFences) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->ResetFences(device, fenceCount, pFences);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetFenceStatus(
    VkDevice                                    device,
    VkFence                                     fence) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->GetFenceStatus(device, fence);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkWaitForFences(
    VkDevice                                    device,
    uint32_t                                    fenceCount,
    const VkFence*                              pFences,
    VkBool32                                    waitAll,
    uint64_t                                    timeout) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->WaitForFences(device, fenceCount, pFences, waitAll, timeout);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateSemaphore(
    VkDevice                                    device,
    const VkSemaphoreCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSemaphore*                                pSemaphore) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreateSemaphore(device, pCreateInfo, pAllocator, pSemaphore);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroySemaphore(
    VkDevice                                    device,
    VkSemaphore                                 semaphore,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroySemaphore(device, semaphore, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateEvent(
    VkDevice                                    device,
    const VkEventCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkEvent*                                    pEvent) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreateEvent(device, pCreateInfo, pAllocator, pEvent);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyEvent(
    VkDevice                                    device,
    VkEvent                                     event,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroyEvent(device, event, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetEventStatus(
    VkDevice                                    device,
    VkEvent                                     event) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->GetEventStatus(device, event);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkSetEvent(
    VkDevice                                    device,
    VkEvent                                     event) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->SetEvent(device, event);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkResetEvent(
    VkDevice                                    device,
    VkEvent                                     event) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->ResetEvent(device, event);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateQueryPool(
    VkDevice                                    device,
    const VkQueryPoolCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkQueryPool*                                pQueryPool) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreateQueryPool(device, pCreateInfo, pAllocator, pQueryPool);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyQueryPool(
    VkDevice                                    device,
    VkQueryPool                                 queryPool,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroyQueryPool(device, queryPool, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetQueryPoolResults(
    VkDevice                                    device,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount,
    size_t                                      dataSize,
    void*                                       pData,
    VkDeviceSize                                stride,
    VkQueryResultFlags                          flags) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->GetQueryPoolResults(device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateBuffer(
    VkDevice                                    device,
    const VkBufferCreateInfo*                   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkBuffer*                                   pBuffer) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreateBuffer(device, pCreateInfo, pAllocator, pBuffer);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyBuffer(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroyBuffer(device, buffer, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateBufferView(
    VkDevice                                    device,
    const VkBufferViewCreateInfo*               pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkBufferView*                               pView) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreateBufferView(device, pCreateInfo, pAllocator, pView);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyBufferView(
    VkDevice                                    device,
    VkBufferView                                bufferView,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroyBufferView(device, bufferView, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateImage(
    VkDevice                                    device,
    const VkImageCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkImage*                                    pImage) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreateImage(device, pCreateInfo, pAllocator, pImage);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyImage(
    VkDevice                                    device,
    VkImage                                     image,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroyImage(device, image, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetImageSubresourceLayout(
    VkDevice                                    device,
    VkImage                                     image,
    const VkImageSubresource*                   pSubresource,
    VkSubresourceLayout*                        pLayout) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->GetImageSubresourceLayout(device, image, pSubresource, pLayout);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateImageView(
    VkDevice                                    device,
    const VkImageViewCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkImageView*                                pView) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreateImageView(device, pCreateInfo, pAllocator, pView);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyImageView(
    VkDevice                                    device,
    VkImageView                                 imageView,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroyImageView(device, imageView, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateShaderModule(
    VkDevice                                    device,
    const VkShaderModuleCreateInfo*             pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkShaderModule*                             pShaderModule) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreateShaderModule(device, pCreateInfo, pAllocator, pShaderModule);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyShaderModule(
    VkDevice                                    device,
    VkShaderModule                              shaderModule,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroyShaderModule(device, shaderModule, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreatePipelineCache(
    VkDevice                                    device,
    const VkPipelineCacheCreateInfo*            pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkPipelineCache*                            pPipelineCache) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreatePipelineCache(device, pCreateInfo, pAllocator, pPipelineCache);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyPipelineCache(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroyPipelineCache(device, pipelineCache, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPipelineCacheData(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    size_t*                                     pDataSize,
    void*                                       pData) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->GetPipelineCacheData(device, pipelineCache, pDataSize, pData);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkMergePipelineCaches(
    VkDevice                                    device,
    VkPipelineCache                             dstCache,
    uint32_t                                    srcCacheCount,
    const VkPipelineCache*                      pSrcCaches) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->MergePipelineCaches(device, dstCache, srcCacheCount, pSrcCaches);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateGraphicsPipelines(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkGraphicsPipelineCreateInfo*         pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreateGraphicsPipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateComputePipelines(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkComputePipelineCreateInfo*          pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreateComputePipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyPipeline(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroyPipeline(device, pipeline, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreatePipelineLayout(
    VkDevice                                    device,
    const VkPipelineLayoutCreateInfo*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkPipelineLayout*                           pPipelineLayout) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreatePipelineLayout(device, pCreateInfo, pAllocator, pPipelineLayout);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyPipelineLayout(
    VkDevice                                    device,
    VkPipelineLayout                            pipelineLayout,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroyPipelineLayout(device, pipelineLayout, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateSampler(
    VkDevice                                    device,
    const VkSamplerCreateInfo*                  pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSampler*                                  pSampler) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreateSampler(device, pCreateInfo, pAllocator, pSampler);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroySampler(
    VkDevice                                    device,
    VkSampler                                   sampler,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroySampler(device, sampler, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorSetLayout(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorSetLayout*                      pSetLayout) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreateDescriptorSetLayout(device, pCreateInfo, pAllocator, pSetLayout);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorSetLayout(
    VkDevice                                    device,
    VkDescriptorSetLayout                       descriptorSetLayout,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroyDescriptorSetLayout(device, descriptorSetLayout, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorPool(
    VkDevice                                    device,
    const VkDescriptorPoolCreateInfo*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorPool*                           pDescriptorPool) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreateDescriptorPool(device, pCreateInfo, pAllocator, pDescriptorPool);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorPool(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroyDescriptorPool(device, descriptorPool, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkResetDescriptorPool(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    VkDescriptorPoolResetFlags                  flags) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->ResetDescriptorPool(device, descriptorPool, flags);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkAllocateDescriptorSets(
    VkDevice                                    device,
    const VkDescriptorSetAllocateInfo*          pAllocateInfo,
    VkDescriptorSet*                            pDescriptorSets) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->AllocateDescriptorSets(device, pAllocateInfo, pDescriptorSets);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkFreeDescriptorSets(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    uint32_t                                    descriptorSetCount,
    const VkDescriptorSet*                      pDescriptorSets) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->FreeDescriptorSets(device, descriptorPool, descriptorSetCount, pDescriptorSets);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkUpdateDescriptorSets(
    VkDevice                                    device,
    uint32_t                                    descriptorWriteCount,
    const VkWriteDescriptorSet*                 pDescriptorWrites,
    uint32_t                                    descriptorCopyCount,
    const VkCopyDescriptorSet*                  pDescriptorCopies) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->UpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateFramebuffer(
    VkDevice                                    device,
    const VkFramebufferCreateInfo*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFramebuffer*                              pFramebuffer) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreateFramebuffer(device, pCreateInfo, pAllocator, pFramebuffer);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyFramebuffer(
    VkDevice                                    device,
    VkFramebuffer                               framebuffer,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroyFramebuffer(device, framebuffer, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateRenderPass(
    VkDevice                                    device,
    const VkRenderPassCreateInfo*               pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkRenderPass*                               pRenderPass) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreateRenderPass(device, pCreateInfo, pAllocator, pRenderPass);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyRenderPass(
    VkDevice                                    device,
    VkRenderPass                                renderPass,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroyRenderPass(device, renderPass, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetRenderAreaGranularity(
    VkDevice                                    device,
    VkRenderPass                                renderPass,
    VkExtent2D*                                 pGranularity) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->GetRenderAreaGranularity(device, renderPass, pGranularity);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateCommandPool(
    VkDevice                                    device,
    const VkCommandPoolCreateInfo*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkCommandPool*                              pCommandPool) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreateCommandPool(device, pCreateInfo, pAllocator, pCommandPool);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyCommandPool(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroyCommandPool(device, commandPool, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkResetCommandPool(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    VkCommandPoolResetFlags                     flags) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->ResetCommandPool(device, commandPool, flags);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkAllocateCommandBuffers(
    VkDevice                                    device,
    const VkCommandBufferAllocateInfo*          pAllocateInfo,
    VkCommandBuffer*                            pCommandBuffers) {
    VkResult temp;
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    temp = disp->AllocateCommandBuffers(device, pAllocateInfo, pCommandBuffers);
    if (VK_SUCCESS == temp) {
        for (uint32_t i = 0; i < pAllocateInfo->commandBufferCount; i++) {
            if (pCommandBuffers != NULL && pCommandBuffers[i] != NULL) {
                loader_set_dispatch(pCommandBuffers[i], disp);
            }
        }
    }
    return temp;
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkFreeCommandBuffers(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    uint32_t                                    commandBufferCount,
    const VkCommandBuffer*                      pCommandBuffers) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->FreeCommandBuffers(device, commandPool, commandBufferCount, pCommandBuffers);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkBeginCommandBuffer(
    VkCommandBuffer                             commandBuffer,
    const VkCommandBufferBeginInfo*             pBeginInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    return disp->BeginCommandBuffer(commandBuffer, pBeginInfo);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEndCommandBuffer(
    VkCommandBuffer                             commandBuffer) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    return disp->EndCommandBuffer(commandBuffer);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkResetCommandBuffer(
    VkCommandBuffer                             commandBuffer,
    VkCommandBufferResetFlags                   flags) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    return disp->ResetCommandBuffer(commandBuffer, flags);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBindPipeline(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipeline                                  pipeline) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetViewport(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstViewport,
    uint32_t                                    viewportCount,
    const VkViewport*                           pViewports) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetScissor(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstScissor,
    uint32_t                                    scissorCount,
    const VkRect2D*                             pScissors) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetLineWidth(
    VkCommandBuffer                             commandBuffer,
    float                                       lineWidth) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetLineWidth(commandBuffer, lineWidth);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBias(
    VkCommandBuffer                             commandBuffer,
    float                                       depthBiasConstantFactor,
    float                                       depthBiasClamp,
    float                                       depthBiasSlopeFactor) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetDepthBias(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetBlendConstants(
    VkCommandBuffer                             commandBuffer,
    const float                                 blendConstants[4]) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetBlendConstants(commandBuffer, blendConstants);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBounds(
    VkCommandBuffer                             commandBuffer,
    float                                       minDepthBounds,
    float                                       maxDepthBounds) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetDepthBounds(commandBuffer, minDepthBounds, maxDepthBounds);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilCompareMask(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    compareMask) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetStencilCompareMask(commandBuffer, faceMask, compareMask);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilWriteMask(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    writeMask) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetStencilWriteMask(commandBuffer, faceMask, writeMask);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilReference(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    reference) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetStencilReference(commandBuffer, faceMask, reference);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBindDescriptorSets(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipelineLayout                            layout,
    uint32_t                                    firstSet,
    uint32_t                                    descriptorSetCount,
    const VkDescriptorSet*                      pDescriptorSets,
    uint32_t                                    dynamicOffsetCount,
    const uint32_t*                             pDynamicOffsets) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBindIndexBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkIndexType                                 indexType) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBindVertexBuffers(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstBinding,
    uint32_t                                    bindingCount,
    const VkBuffer*                             pBuffers,
    const VkDeviceSize*                         pOffsets) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDraw(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    vertexCount,
    uint32_t                                    instanceCount,
    uint32_t                                    firstVertex,
    uint32_t                                    firstInstance) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexed(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    indexCount,
    uint32_t                                    instanceCount,
    uint32_t                                    firstIndex,
    int32_t                                     vertexOffset,
    uint32_t                                    firstInstance) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndirect(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    uint32_t                                    drawCount,
    uint32_t                                    stride) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdDrawIndirect(commandBuffer, buffer, offset, drawCount, stride);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexedIndirect(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    uint32_t                                    drawCount,
    uint32_t                                    stride) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdDrawIndexedIndirect(commandBuffer, buffer, offset, drawCount, stride);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDispatch(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    groupCountX,
    uint32_t                                    groupCountY,
    uint32_t                                    groupCountZ) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDispatchIndirect(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdDispatchIndirect(commandBuffer, buffer, offset);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    srcBuffer,
    VkBuffer                                    dstBuffer,
    uint32_t                                    regionCount,
    const VkBufferCopy*                         pRegions) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkImageCopy*                          pRegions) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBlitImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkImageBlit*                          pRegions,
    VkFilter                                    filter) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyBufferToImage(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    srcBuffer,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkBufferImageCopy*                    pRegions) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyImageToBuffer(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkBuffer                                    dstBuffer,
    uint32_t                                    regionCount,
    const VkBufferImageCopy*                    pRegions) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdUpdateBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                dataSize,
    const void*                                 pData) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, pData);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdFillBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                size,
    uint32_t                                    data) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdClearColorImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     image,
    VkImageLayout                               imageLayout,
    const VkClearColorValue*                    pColor,
    uint32_t                                    rangeCount,
    const VkImageSubresourceRange*              pRanges) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdClearColorImage(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdClearDepthStencilImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     image,
    VkImageLayout                               imageLayout,
    const VkClearDepthStencilValue*             pDepthStencil,
    uint32_t                                    rangeCount,
    const VkImageSubresourceRange*              pRanges) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdClearDepthStencilImage(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdClearAttachments(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    attachmentCount,
    const VkClearAttachment*                    pAttachments,
    uint32_t                                    rectCount,
    const VkClearRect*                          pRects) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdClearAttachments(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdResolveImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkImageResolve*                       pRegions) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdResolveImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetEvent(
    VkCommandBuffer                             commandBuffer,
    VkEvent                                     event,
    VkPipelineStageFlags                        stageMask) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetEvent(commandBuffer, event, stageMask);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdResetEvent(
    VkCommandBuffer                             commandBuffer,
    VkEvent                                     event,
    VkPipelineStageFlags                        stageMask) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdResetEvent(commandBuffer, event, stageMask);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdWaitEvents(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    eventCount,
    const VkEvent*                              pEvents,
    VkPipelineStageFlags                        srcStageMask,
    VkPipelineStageFlags                        dstStageMask,
    uint32_t                                    memoryBarrierCount,
    const VkMemoryBarrier*                      pMemoryBarriers,
    uint32_t                                    bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier*                pBufferMemoryBarriers,
    uint32_t                                    imageMemoryBarrierCount,
    const VkImageMemoryBarrier*                 pImageMemoryBarriers) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdWaitEvents(commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdPipelineBarrier(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlags                        srcStageMask,
    VkPipelineStageFlags                        dstStageMask,
    VkDependencyFlags                           dependencyFlags,
    uint32_t                                    memoryBarrierCount,
    const VkMemoryBarrier*                      pMemoryBarriers,
    uint32_t                                    bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier*                pBufferMemoryBarriers,
    uint32_t                                    imageMemoryBarrierCount,
    const VkImageMemoryBarrier*                 pImageMemoryBarriers) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBeginQuery(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query,
    VkQueryControlFlags                         flags) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdBeginQuery(commandBuffer, queryPool, query, flags);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdEndQuery(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdEndQuery(commandBuffer, queryPool, query);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdResetQueryPool(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdWriteTimestamp(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlagBits                     pipelineStage,
    VkQueryPool                                 queryPool,
    uint32_t                                    query) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, query);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyQueryPoolResults(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                stride,
    VkQueryResultFlags                          flags) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdPushConstants(
    VkCommandBuffer                             commandBuffer,
    VkPipelineLayout                            layout,
    VkShaderStageFlags                          stageFlags,
    uint32_t                                    offset,
    uint32_t                                    size,
    const void*                                 pValues) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdPushConstants(commandBuffer, layout, stageFlags, offset, size, pValues);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBeginRenderPass(
    VkCommandBuffer                             commandBuffer,
    const VkRenderPassBeginInfo*                pRenderPassBegin,
    VkSubpassContents                           contents) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdNextSubpass(
    VkCommandBuffer                             commandBuffer,
    VkSubpassContents                           contents) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdNextSubpass(commandBuffer, contents);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdEndRenderPass(
    VkCommandBuffer                             commandBuffer) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdEndRenderPass(commandBuffer);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdExecuteCommands(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    commandBufferCount,
    const VkCommandBuffer*                      pCommandBuffers) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdExecuteCommands(commandBuffer, commandBufferCount, pCommandBuffers);
}

// --------------------------------------------
// ---- Vulkan API version 1.1 trampolines ----
// --------------------------------------------


LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFeatures2(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceFeatures2*                  pFeatures) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceFeatures2: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceFeatures2-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    const struct loader_instance *inst = ((struct loader_physical_device_tramp *)physicalDevice)->this_instance;
    if (inst != NULL && inst->inst_ext_enables.khr_get_physical_device_properties2) {
        disp->GetPhysicalDeviceFeatures2KHR(unwrapped_phys_dev, pFeatures);
    } else {
        disp->GetPhysicalDeviceFeatures2(unwrapped_phys_dev, pFeatures);
    }
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties2(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceProperties2*                pProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceProperties2: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceProperties2-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    const struct loader_instance *inst = ((struct loader_physical_device_tramp *)physicalDevice)->this_instance;
    if (inst != NULL && inst->inst_ext_enables.khr_get_physical_device_properties2) {
        disp->GetPhysicalDeviceProperties2KHR(unwrapped_phys_dev, pProperties);
    } else {
        disp->GetPhysicalDeviceProperties2(unwrapped_phys_dev, pProperties);
    }
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFormatProperties2(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkFormatProperties2*                        pFormatProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceFormatProperties2: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceFormatProperties2-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    const struct loader_instance *inst = ((struct loader_physical_device_tramp *)physicalDevice)->this_instance;
    if (inst != NULL && inst->inst_ext_enables.khr_get_physical_device_properties2) {
        disp->GetPhysicalDeviceFormatProperties2KHR(unwrapped_phys_dev, format, pFormatProperties);
    } else {
        disp->GetPhysicalDeviceFormatProperties2(unwrapped_phys_dev, format, pFormatProperties);
    }
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceImageFormatProperties2(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceImageFormatInfo2*     pImageFormatInfo,
    VkImageFormatProperties2*                   pImageFormatProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceImageFormatProperties2: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceImageFormatProperties2-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    const struct loader_instance *inst = ((struct loader_physical_device_tramp *)physicalDevice)->this_instance;
    if (inst != NULL && inst->inst_ext_enables.khr_get_physical_device_properties2) {
        return disp->GetPhysicalDeviceImageFormatProperties2KHR(unwrapped_phys_dev, pImageFormatInfo, pImageFormatProperties);
    } else {
        return disp->GetPhysicalDeviceImageFormatProperties2(unwrapped_phys_dev, pImageFormatInfo, pImageFormatProperties);
    }
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyProperties2(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pQueueFamilyPropertyCount,
    VkQueueFamilyProperties2*                   pQueueFamilyProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceQueueFamilyProperties2: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceQueueFamilyProperties2-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    const struct loader_instance *inst = ((struct loader_physical_device_tramp *)physicalDevice)->this_instance;
    if (inst != NULL && inst->inst_ext_enables.khr_get_physical_device_properties2) {
        disp->GetPhysicalDeviceQueueFamilyProperties2KHR(unwrapped_phys_dev, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    } else {
        disp->GetPhysicalDeviceQueueFamilyProperties2(unwrapped_phys_dev, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    }
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceMemoryProperties2(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceMemoryProperties2*          pMemoryProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceMemoryProperties2: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceMemoryProperties2-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    const struct loader_instance *inst = ((struct loader_physical_device_tramp *)physicalDevice)->this_instance;
    if (inst != NULL && inst->inst_ext_enables.khr_get_physical_device_properties2) {
        disp->GetPhysicalDeviceMemoryProperties2KHR(unwrapped_phys_dev, pMemoryProperties);
    } else {
        disp->GetPhysicalDeviceMemoryProperties2(unwrapped_phys_dev, pMemoryProperties);
    }
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceSparseImageFormatProperties2(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo,
    uint32_t*                                   pPropertyCount,
    VkSparseImageFormatProperties2*             pProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceSparseImageFormatProperties2: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceSparseImageFormatProperties2-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    const struct loader_instance *inst = ((struct loader_physical_device_tramp *)physicalDevice)->this_instance;
    if (inst != NULL && inst->inst_ext_enables.khr_get_physical_device_properties2) {
        disp->GetPhysicalDeviceSparseImageFormatProperties2KHR(unwrapped_phys_dev, pFormatInfo, pPropertyCount, pProperties);
    } else {
        disp->GetPhysicalDeviceSparseImageFormatProperties2(unwrapped_phys_dev, pFormatInfo, pPropertyCount, pProperties);
    }
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalBufferProperties(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalBufferInfo*   pExternalBufferInfo,
    VkExternalBufferProperties*                 pExternalBufferProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceExternalBufferProperties: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceExternalBufferProperties-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    const struct loader_instance *inst = ((struct loader_physical_device_tramp *)physicalDevice)->this_instance;
    if (inst != NULL && inst->inst_ext_enables.khr_external_memory_capabilities) {
        disp->GetPhysicalDeviceExternalBufferPropertiesKHR(unwrapped_phys_dev, pExternalBufferInfo, pExternalBufferProperties);
    } else {
        disp->GetPhysicalDeviceExternalBufferProperties(unwrapped_phys_dev, pExternalBufferInfo, pExternalBufferProperties);
    }
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalFenceProperties(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalFenceInfo*    pExternalFenceInfo,
    VkExternalFenceProperties*                  pExternalFenceProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceExternalFenceProperties: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceExternalFenceProperties-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    const struct loader_instance *inst = ((struct loader_physical_device_tramp *)physicalDevice)->this_instance;
    if (inst != NULL && inst->inst_ext_enables.khr_external_fence_capabilities) {
        disp->GetPhysicalDeviceExternalFencePropertiesKHR(unwrapped_phys_dev, pExternalFenceInfo, pExternalFenceProperties);
    } else {
        disp->GetPhysicalDeviceExternalFenceProperties(unwrapped_phys_dev, pExternalFenceInfo, pExternalFenceProperties);
    }
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalSemaphoreProperties(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo,
    VkExternalSemaphoreProperties*              pExternalSemaphoreProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceExternalSemaphoreProperties: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceExternalSemaphoreProperties-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    const struct loader_instance *inst = ((struct loader_physical_device_tramp *)physicalDevice)->this_instance;
    if (inst != NULL && inst->inst_ext_enables.khr_external_semaphore_capabilities) {
        disp->GetPhysicalDeviceExternalSemaphorePropertiesKHR(unwrapped_phys_dev, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
    } else {
        disp->GetPhysicalDeviceExternalSemaphoreProperties(unwrapped_phys_dev, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
    }
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkBindBufferMemory2(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindBufferMemoryInfo*               pBindInfos) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->BindBufferMemory2(device, bindInfoCount, pBindInfos);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkBindImageMemory2(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindImageMemoryInfo*                pBindInfos) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->BindImageMemory2(device, bindInfoCount, pBindInfos);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetDeviceGroupPeerMemoryFeatures(
    VkDevice                                    device,
    uint32_t                                    heapIndex,
    uint32_t                                    localDeviceIndex,
    uint32_t                                    remoteDeviceIndex,
    VkPeerMemoryFeatureFlags*                   pPeerMemoryFeatures) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->GetDeviceGroupPeerMemoryFeatures(device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetDeviceMask(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    deviceMask) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetDeviceMask(commandBuffer, deviceMask);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDispatchBase(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    baseGroupX,
    uint32_t                                    baseGroupY,
    uint32_t                                    baseGroupZ,
    uint32_t                                    groupCountX,
    uint32_t                                    groupCountY,
    uint32_t                                    groupCountZ) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdDispatchBase(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetImageMemoryRequirements2(
    VkDevice                                    device,
    const VkImageMemoryRequirementsInfo2*       pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->GetImageMemoryRequirements2(device, pInfo, pMemoryRequirements);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetBufferMemoryRequirements2(
    VkDevice                                    device,
    const VkBufferMemoryRequirementsInfo2*      pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->GetBufferMemoryRequirements2(device, pInfo, pMemoryRequirements);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetImageSparseMemoryRequirements2(
    VkDevice                                    device,
    const VkImageSparseMemoryRequirementsInfo2* pInfo,
    uint32_t*                                   pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2*           pSparseMemoryRequirements) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->GetImageSparseMemoryRequirements2(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkTrimCommandPool(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    VkCommandPoolTrimFlags                      flags) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->TrimCommandPool(device, commandPool, flags);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetDeviceQueue2(
    VkDevice                                    device,
    const VkDeviceQueueInfo2*                   pQueueInfo,
    VkQueue*                                    pQueue) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->GetDeviceQueue2(device, pQueueInfo, pQueue);
    if (pQueue != NULL && *pQueue != NULL) {
        loader_set_dispatch(*pQueue, disp);
    }
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateSamplerYcbcrConversion(
    VkDevice                                    device,
    const VkSamplerYcbcrConversionCreateInfo*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSamplerYcbcrConversion*                   pYcbcrConversion) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreateSamplerYcbcrConversion(device, pCreateInfo, pAllocator, pYcbcrConversion);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroySamplerYcbcrConversion(
    VkDevice                                    device,
    VkSamplerYcbcrConversion                    ycbcrConversion,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroySamplerYcbcrConversion(device, ycbcrConversion, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorUpdateTemplate(
    VkDevice                                    device,
    const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorUpdateTemplate*                 pDescriptorUpdateTemplate) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreateDescriptorUpdateTemplate(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorUpdateTemplate(
    VkDevice                                    device,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroyDescriptorUpdateTemplate(device, descriptorUpdateTemplate, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkUpdateDescriptorSetWithTemplate(
    VkDevice                                    device,
    VkDescriptorSet                             descriptorSet,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const void*                                 pData) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->UpdateDescriptorSetWithTemplate(device, descriptorSet, descriptorUpdateTemplate, pData);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetDescriptorSetLayoutSupport(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    VkDescriptorSetLayoutSupport*               pSupport) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->GetDescriptorSetLayoutSupport(device, pCreateInfo, pSupport);
}

// --------------------------------------------
// ---- Vulkan API version 1.2 trampolines ----
// --------------------------------------------


LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndirectCount(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdDrawIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexedIndirectCount(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdDrawIndexedIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateRenderPass2(
    VkDevice                                    device,
    const VkRenderPassCreateInfo2*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkRenderPass*                               pRenderPass) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreateRenderPass2(device, pCreateInfo, pAllocator, pRenderPass);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBeginRenderPass2(
    VkCommandBuffer                             commandBuffer,
    const VkRenderPassBeginInfo*                pRenderPassBegin,
    const VkSubpassBeginInfo*                   pSubpassBeginInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdBeginRenderPass2(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdNextSubpass2(
    VkCommandBuffer                             commandBuffer,
    const VkSubpassBeginInfo*                   pSubpassBeginInfo,
    const VkSubpassEndInfo*                     pSubpassEndInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdNextSubpass2(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdEndRenderPass2(
    VkCommandBuffer                             commandBuffer,
    const VkSubpassEndInfo*                     pSubpassEndInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdEndRenderPass2(commandBuffer, pSubpassEndInfo);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkResetQueryPool(
    VkDevice                                    device,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->ResetQueryPool(device, queryPool, firstQuery, queryCount);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetSemaphoreCounterValue(
    VkDevice                                    device,
    VkSemaphore                                 semaphore,
    uint64_t*                                   pValue) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->GetSemaphoreCounterValue(device, semaphore, pValue);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkWaitSemaphores(
    VkDevice                                    device,
    const VkSemaphoreWaitInfo*                  pWaitInfo,
    uint64_t                                    timeout) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->WaitSemaphores(device, pWaitInfo, timeout);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkSignalSemaphore(
    VkDevice                                    device,
    const VkSemaphoreSignalInfo*                pSignalInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->SignalSemaphore(device, pSignalInfo);
}

LOADER_EXPORT VKAPI_ATTR VkDeviceAddress VKAPI_CALL vkGetBufferDeviceAddress(
    VkDevice                                    device,
    const VkBufferDeviceAddressInfo*            pInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->GetBufferDeviceAddress(device, pInfo);
}

LOADER_EXPORT VKAPI_ATTR uint64_t VKAPI_CALL vkGetBufferOpaqueCaptureAddress(
    VkDevice                                    device,
    const VkBufferDeviceAddressInfo*            pInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->GetBufferOpaqueCaptureAddress(device, pInfo);
}

LOADER_EXPORT VKAPI_ATTR uint64_t VKAPI_CALL vkGetDeviceMemoryOpaqueCaptureAddress(
    VkDevice                                    device,
    const VkDeviceMemoryOpaqueCaptureAddressInfo* pInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->GetDeviceMemoryOpaqueCaptureAddress(device, pInfo);
}

// --------------------------------------------
// ---- Vulkan API version 1.3 trampolines ----
// --------------------------------------------


LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceToolProperties(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pToolCount,
    VkPhysicalDeviceToolProperties*             pToolProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceToolProperties: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceToolProperties-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceToolProperties(unwrapped_phys_dev, pToolCount, pToolProperties);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreatePrivateDataSlot(
    VkDevice                                    device,
    const VkPrivateDataSlotCreateInfo*          pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkPrivateDataSlot*                          pPrivateDataSlot) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->CreatePrivateDataSlot(device, pCreateInfo, pAllocator, pPrivateDataSlot);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyPrivateDataSlot(
    VkDevice                                    device,
    VkPrivateDataSlot                           privateDataSlot,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->DestroyPrivateDataSlot(device, privateDataSlot, pAllocator);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkSetPrivateData(
    VkDevice                                    device,
    VkObjectType                                objectType,
    uint64_t                                    objectHandle,
    VkPrivateDataSlot                           privateDataSlot,
    uint64_t                                    data) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    return disp->SetPrivateData(device, objectType, objectHandle, privateDataSlot, data);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPrivateData(
    VkDevice                                    device,
    VkObjectType                                objectType,
    uint64_t                                    objectHandle,
    VkPrivateDataSlot                           privateDataSlot,
    uint64_t*                                   pData) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->GetPrivateData(device, objectType, objectHandle, privateDataSlot, pData);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetEvent2(
    VkCommandBuffer                             commandBuffer,
    VkEvent                                     event,
    const VkDependencyInfo*                     pDependencyInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetEvent2(commandBuffer, event, pDependencyInfo);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdResetEvent2(
    VkCommandBuffer                             commandBuffer,
    VkEvent                                     event,
    VkPipelineStageFlags2                       stageMask) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdResetEvent2(commandBuffer, event, stageMask);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdWaitEvents2(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    eventCount,
    const VkEvent*                              pEvents,
    const VkDependencyInfo*                     pDependencyInfos) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdWaitEvents2(commandBuffer, eventCount, pEvents, pDependencyInfos);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdPipelineBarrier2(
    VkCommandBuffer                             commandBuffer,
    const VkDependencyInfo*                     pDependencyInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdPipelineBarrier2(commandBuffer, pDependencyInfo);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdWriteTimestamp2(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlags2                       stage,
    VkQueryPool                                 queryPool,
    uint32_t                                    query) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdWriteTimestamp2(commandBuffer, stage, queryPool, query);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkQueueSubmit2(
    VkQueue                                     queue,
    uint32_t                                    submitCount,
    const VkSubmitInfo2*                        pSubmits,
    VkFence                                     fence) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(queue);
    return disp->QueueSubmit2(queue, submitCount, pSubmits, fence);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyBuffer2(
    VkCommandBuffer                             commandBuffer,
    const VkCopyBufferInfo2*                    pCopyBufferInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdCopyBuffer2(commandBuffer, pCopyBufferInfo);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyImage2(
    VkCommandBuffer                             commandBuffer,
    const VkCopyImageInfo2*                     pCopyImageInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdCopyImage2(commandBuffer, pCopyImageInfo);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyBufferToImage2(
    VkCommandBuffer                             commandBuffer,
    const VkCopyBufferToImageInfo2*             pCopyBufferToImageInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdCopyBufferToImage2(commandBuffer, pCopyBufferToImageInfo);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyImageToBuffer2(
    VkCommandBuffer                             commandBuffer,
    const VkCopyImageToBufferInfo2*             pCopyImageToBufferInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdCopyImageToBuffer2(commandBuffer, pCopyImageToBufferInfo);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBlitImage2(
    VkCommandBuffer                             commandBuffer,
    const VkBlitImageInfo2*                     pBlitImageInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdBlitImage2(commandBuffer, pBlitImageInfo);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdResolveImage2(
    VkCommandBuffer                             commandBuffer,
    const VkResolveImageInfo2*                  pResolveImageInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdResolveImage2(commandBuffer, pResolveImageInfo);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBeginRendering(
    VkCommandBuffer                             commandBuffer,
    const VkRenderingInfo*                      pRenderingInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdBeginRendering(commandBuffer, pRenderingInfo);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdEndRendering(
    VkCommandBuffer                             commandBuffer) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdEndRendering(commandBuffer);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetCullMode(
    VkCommandBuffer                             commandBuffer,
    VkCullModeFlags                             cullMode) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetCullMode(commandBuffer, cullMode);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetFrontFace(
    VkCommandBuffer                             commandBuffer,
    VkFrontFace                                 frontFace) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetFrontFace(commandBuffer, frontFace);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetPrimitiveTopology(
    VkCommandBuffer                             commandBuffer,
    VkPrimitiveTopology                         primitiveTopology) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetPrimitiveTopology(commandBuffer, primitiveTopology);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetViewportWithCount(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    viewportCount,
    const VkViewport*                           pViewports) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetViewportWithCount(commandBuffer, viewportCount, pViewports);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetScissorWithCount(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    scissorCount,
    const VkRect2D*                             pScissors) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetScissorWithCount(commandBuffer, scissorCount, pScissors);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBindVertexBuffers2(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstBinding,
    uint32_t                                    bindingCount,
    const VkBuffer*                             pBuffers,
    const VkDeviceSize*                         pOffsets,
    const VkDeviceSize*                         pSizes,
    const VkDeviceSize*                         pStrides) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdBindVertexBuffers2(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes, pStrides);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthTestEnable(
    VkCommandBuffer                             commandBuffer,
    VkBool32                                    depthTestEnable) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetDepthTestEnable(commandBuffer, depthTestEnable);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthWriteEnable(
    VkCommandBuffer                             commandBuffer,
    VkBool32                                    depthWriteEnable) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetDepthWriteEnable(commandBuffer, depthWriteEnable);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthCompareOp(
    VkCommandBuffer                             commandBuffer,
    VkCompareOp                                 depthCompareOp) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetDepthCompareOp(commandBuffer, depthCompareOp);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBoundsTestEnable(
    VkCommandBuffer                             commandBuffer,
    VkBool32                                    depthBoundsTestEnable) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetDepthBoundsTestEnable(commandBuffer, depthBoundsTestEnable);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilTestEnable(
    VkCommandBuffer                             commandBuffer,
    VkBool32                                    stencilTestEnable) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetStencilTestEnable(commandBuffer, stencilTestEnable);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilOp(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    VkStencilOp                                 failOp,
    VkStencilOp                                 passOp,
    VkStencilOp                                 depthFailOp,
    VkCompareOp                                 compareOp) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetStencilOp(commandBuffer, faceMask, failOp, passOp, depthFailOp, compareOp);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetRasterizerDiscardEnable(
    VkCommandBuffer                             commandBuffer,
    VkBool32                                    rasterizerDiscardEnable) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetRasterizerDiscardEnable(commandBuffer, rasterizerDiscardEnable);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBiasEnable(
    VkCommandBuffer                             commandBuffer,
    VkBool32                                    depthBiasEnable) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetDepthBiasEnable(commandBuffer, depthBiasEnable);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetPrimitiveRestartEnable(
    VkCommandBuffer                             commandBuffer,
    VkBool32                                    primitiveRestartEnable) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    disp->CmdSetPrimitiveRestartEnable(commandBuffer, primitiveRestartEnable);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetDeviceBufferMemoryRequirements(
    VkDevice                                    device,
    const VkDeviceBufferMemoryRequirements*     pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->GetDeviceBufferMemoryRequirements(device, pInfo, pMemoryRequirements);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetDeviceImageMemoryRequirements(
    VkDevice                                    device,
    const VkDeviceImageMemoryRequirements*      pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->GetDeviceImageMemoryRequirements(device, pInfo, pMemoryRequirements);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetDeviceImageSparseMemoryRequirements(
    VkDevice                                    device,
    const VkDeviceImageMemoryRequirements*      pInfo,
    uint32_t*                                   pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2*           pSparseMemoryRequirements) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    disp->GetDeviceImageSparseMemoryRequirements(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}

// ----------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_video_queue trampolines ----
// ----------------------------------------------------------------

#ifdef VK_ENABLE_BETA_EXTENSIONS

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceVideoCapabilitiesKHR(
    VkPhysicalDevice                            physicalDevice,
    const VkVideoProfileKHR*                    pVideoProfile,
    VkVideoCapabilitiesKHR*                     pCapabilities) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceVideoCapabilitiesKHR: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceVideoCapabilitiesKHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceVideoCapabilitiesKHR(unwrapped_phys_dev, pVideoProfile, pCapabilities);
}

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceVideoFormatPropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceVideoFormatInfoKHR*   pVideoFormatInfo,
    uint32_t*                                   pVideoFormatPropertyCount,
    VkVideoFormatPropertiesKHR*                 pVideoFormatProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceVideoFormatPropertiesKHR: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceVideoFormatPropertiesKHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceVideoFormatPropertiesKHR(unwrapped_phys_dev, pVideoFormatInfo, pVideoFormatPropertyCount, pVideoFormatProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL CreateVideoSessionKHR(
    VkDevice                                    device,
    const VkVideoSessionCreateInfoKHR*          pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkVideoSessionKHR*                          pVideoSession) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateVideoSessionKHR: Invalid VkDevice [VUID-vkCreateVideoSessionKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CreateVideoSessionKHR(device, pCreateInfo, pAllocator, pVideoSession);
}

VKAPI_ATTR void VKAPI_CALL DestroyVideoSessionKHR(
    VkDevice                                    device,
    VkVideoSessionKHR                           videoSession,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkDestroyVideoSessionKHR: Invalid VkDevice [VUID-vkDestroyVideoSessionKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->DestroyVideoSessionKHR(device, videoSession, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL GetVideoSessionMemoryRequirementsKHR(
    VkDevice                                    device,
    VkVideoSessionKHR                           videoSession,
    uint32_t*                                   pVideoSessionMemoryRequirementsCount,
    VkVideoGetMemoryPropertiesKHR*              pVideoSessionMemoryRequirements) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetVideoSessionMemoryRequirementsKHR: Invalid VkDevice [VUID-vkGetVideoSessionMemoryRequirementsKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetVideoSessionMemoryRequirementsKHR(device, videoSession, pVideoSessionMemoryRequirementsCount, pVideoSessionMemoryRequirements);
}

VKAPI_ATTR VkResult VKAPI_CALL BindVideoSessionMemoryKHR(
    VkDevice                                    device,
    VkVideoSessionKHR                           videoSession,
    uint32_t                                    videoSessionBindMemoryCount,
    const VkVideoBindMemoryKHR*                 pVideoSessionBindMemories) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkBindVideoSessionMemoryKHR: Invalid VkDevice [VUID-vkBindVideoSessionMemoryKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->BindVideoSessionMemoryKHR(device, videoSession, videoSessionBindMemoryCount, pVideoSessionBindMemories);
}

VKAPI_ATTR VkResult VKAPI_CALL CreateVideoSessionParametersKHR(
    VkDevice                                    device,
    const VkVideoSessionParametersCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkVideoSessionParametersKHR*                pVideoSessionParameters) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateVideoSessionParametersKHR: Invalid VkDevice [VUID-vkCreateVideoSessionParametersKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CreateVideoSessionParametersKHR(device, pCreateInfo, pAllocator, pVideoSessionParameters);
}

VKAPI_ATTR VkResult VKAPI_CALL UpdateVideoSessionParametersKHR(
    VkDevice                                    device,
    VkVideoSessionParametersKHR                 videoSessionParameters,
    const VkVideoSessionParametersUpdateInfoKHR* pUpdateInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkUpdateVideoSessionParametersKHR: Invalid VkDevice [VUID-vkUpdateVideoSessionParametersKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->UpdateVideoSessionParametersKHR(device, videoSessionParameters, pUpdateInfo);
}

VKAPI_ATTR void VKAPI_CALL DestroyVideoSessionParametersKHR(
    VkDevice                                    device,
    VkVideoSessionParametersKHR                 videoSessionParameters,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkDestroyVideoSessionParametersKHR: Invalid VkDevice [VUID-vkDestroyVideoSessionParametersKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->DestroyVideoSessionParametersKHR(device, videoSessionParameters, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL CmdBeginVideoCodingKHR(
    VkCommandBuffer                             commandBuffer,
    const VkVideoBeginCodingInfoKHR*            pBeginInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdBeginVideoCodingKHR: Invalid VkCommandBuffer [VUID-vkCmdBeginVideoCodingKHR-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdBeginVideoCodingKHR(commandBuffer, pBeginInfo);
}

VKAPI_ATTR void VKAPI_CALL CmdEndVideoCodingKHR(
    VkCommandBuffer                             commandBuffer,
    const VkVideoEndCodingInfoKHR*              pEndCodingInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdEndVideoCodingKHR: Invalid VkCommandBuffer [VUID-vkCmdEndVideoCodingKHR-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdEndVideoCodingKHR(commandBuffer, pEndCodingInfo);
}

VKAPI_ATTR void VKAPI_CALL CmdControlVideoCodingKHR(
    VkCommandBuffer                             commandBuffer,
    const VkVideoCodingControlInfoKHR*          pCodingControlInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdControlVideoCodingKHR: Invalid VkCommandBuffer [VUID-vkCmdControlVideoCodingKHR-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdControlVideoCodingKHR(commandBuffer, pCodingControlInfo);
}
#endif // VK_ENABLE_BETA_EXTENSIONS

// -----------------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_video_decode_queue trampolines ----
// -----------------------------------------------------------------------

#ifdef VK_ENABLE_BETA_EXTENSIONS

VKAPI_ATTR void VKAPI_CALL CmdDecodeVideoKHR(
    VkCommandBuffer                             commandBuffer,
    const VkVideoDecodeInfoKHR*                 pFrameInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdDecodeVideoKHR: Invalid VkCommandBuffer [VUID-vkCmdDecodeVideoKHR-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdDecodeVideoKHR(commandBuffer, pFrameInfo);
}
#endif // VK_ENABLE_BETA_EXTENSIONS

// --------------------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_external_memory_win32 trampolines ----
// --------------------------------------------------------------------------

#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryWin32HandleKHR(
    VkDevice                                    device,
    const VkMemoryGetWin32HandleInfoKHR*        pGetWin32HandleInfo,
    HANDLE*                                     pHandle) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetMemoryWin32HandleKHR: Invalid VkDevice [VUID-vkGetMemoryWin32HandleKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetMemoryWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
}

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryWin32HandlePropertiesKHR(
    VkDevice                                    device,
    VkExternalMemoryHandleTypeFlagBits          handleType,
    HANDLE                                      handle,
    VkMemoryWin32HandlePropertiesKHR*           pMemoryWin32HandleProperties) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetMemoryWin32HandlePropertiesKHR: Invalid VkDevice [VUID-vkGetMemoryWin32HandlePropertiesKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetMemoryWin32HandlePropertiesKHR(device, handleType, handle, pMemoryWin32HandleProperties);
}
#endif // VK_USE_PLATFORM_WIN32_KHR

// -----------------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_external_memory_fd trampolines ----
// -----------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL GetMemoryFdKHR(
    VkDevice                                    device,
    const VkMemoryGetFdInfoKHR*                 pGetFdInfo,
    int*                                        pFd) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetMemoryFdKHR: Invalid VkDevice [VUID-vkGetMemoryFdKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetMemoryFdKHR(device, pGetFdInfo, pFd);
}

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryFdPropertiesKHR(
    VkDevice                                    device,
    VkExternalMemoryHandleTypeFlagBits          handleType,
    int                                         fd,
    VkMemoryFdPropertiesKHR*                    pMemoryFdProperties) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetMemoryFdPropertiesKHR: Invalid VkDevice [VUID-vkGetMemoryFdPropertiesKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetMemoryFdPropertiesKHR(device, handleType, fd, pMemoryFdProperties);
}

// -----------------------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_external_semaphore_win32 trampolines ----
// -----------------------------------------------------------------------------

#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL ImportSemaphoreWin32HandleKHR(
    VkDevice                                    device,
    const VkImportSemaphoreWin32HandleInfoKHR*  pImportSemaphoreWin32HandleInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkImportSemaphoreWin32HandleKHR: Invalid VkDevice [VUID-vkImportSemaphoreWin32HandleKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->ImportSemaphoreWin32HandleKHR(device, pImportSemaphoreWin32HandleInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL GetSemaphoreWin32HandleKHR(
    VkDevice                                    device,
    const VkSemaphoreGetWin32HandleInfoKHR*     pGetWin32HandleInfo,
    HANDLE*                                     pHandle) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetSemaphoreWin32HandleKHR: Invalid VkDevice [VUID-vkGetSemaphoreWin32HandleKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetSemaphoreWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
}
#endif // VK_USE_PLATFORM_WIN32_KHR

// --------------------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_external_semaphore_fd trampolines ----
// --------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL ImportSemaphoreFdKHR(
    VkDevice                                    device,
    const VkImportSemaphoreFdInfoKHR*           pImportSemaphoreFdInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkImportSemaphoreFdKHR: Invalid VkDevice [VUID-vkImportSemaphoreFdKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->ImportSemaphoreFdKHR(device, pImportSemaphoreFdInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL GetSemaphoreFdKHR(
    VkDevice                                    device,
    const VkSemaphoreGetFdInfoKHR*              pGetFdInfo,
    int*                                        pFd) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetSemaphoreFdKHR: Invalid VkDevice [VUID-vkGetSemaphoreFdKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetSemaphoreFdKHR(device, pGetFdInfo, pFd);
}

// --------------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_push_descriptor trampolines ----
// --------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL CmdPushDescriptorSetKHR(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipelineLayout                            layout,
    uint32_t                                    set,
    uint32_t                                    descriptorWriteCount,
    const VkWriteDescriptorSet*                 pDescriptorWrites) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdPushDescriptorSetKHR: Invalid VkCommandBuffer [VUID-vkCmdPushDescriptorSetKHR-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdPushDescriptorSetKHR(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
}

VKAPI_ATTR void VKAPI_CALL CmdPushDescriptorSetWithTemplateKHR(
    VkCommandBuffer                             commandBuffer,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    VkPipelineLayout                            layout,
    uint32_t                                    set,
    const void*                                 pData) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdPushDescriptorSetWithTemplateKHR: Invalid VkCommandBuffer [VUID-vkCmdPushDescriptorSetWithTemplateKHR-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdPushDescriptorSetWithTemplateKHR(commandBuffer, descriptorUpdateTemplate, layout, set, pData);
}

// -----------------------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_shared_presentable_image trampolines ----
// -----------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL GetSwapchainStatusKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetSwapchainStatusKHR: Invalid VkDevice [VUID-vkGetSwapchainStatusKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetSwapchainStatusKHR(device, swapchain);
}

// -------------------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_external_fence_win32 trampolines ----
// -------------------------------------------------------------------------

#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL ImportFenceWin32HandleKHR(
    VkDevice                                    device,
    const VkImportFenceWin32HandleInfoKHR*      pImportFenceWin32HandleInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkImportFenceWin32HandleKHR: Invalid VkDevice [VUID-vkImportFenceWin32HandleKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->ImportFenceWin32HandleKHR(device, pImportFenceWin32HandleInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL GetFenceWin32HandleKHR(
    VkDevice                                    device,
    const VkFenceGetWin32HandleInfoKHR*         pGetWin32HandleInfo,
    HANDLE*                                     pHandle) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetFenceWin32HandleKHR: Invalid VkDevice [VUID-vkGetFenceWin32HandleKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetFenceWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
}
#endif // VK_USE_PLATFORM_WIN32_KHR

// ----------------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_external_fence_fd trampolines ----
// ----------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL ImportFenceFdKHR(
    VkDevice                                    device,
    const VkImportFenceFdInfoKHR*               pImportFenceFdInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkImportFenceFdKHR: Invalid VkDevice [VUID-vkImportFenceFdKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->ImportFenceFdKHR(device, pImportFenceFdInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL GetFenceFdKHR(
    VkDevice                                    device,
    const VkFenceGetFdInfoKHR*                  pGetFdInfo,
    int*                                        pFd) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetFenceFdKHR: Invalid VkDevice [VUID-vkGetFenceFdKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetFenceFdKHR(device, pGetFdInfo, pFd);
}

// ----------------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_performance_query trampolines ----
// ----------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    uint32_t*                                   pCounterCount,
    VkPerformanceCounterKHR*                    pCounters,
    VkPerformanceCounterDescriptionKHR*         pCounterDescriptions) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR: Invalid VkPhysicalDevice [VUID-vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR(unwrapped_phys_dev, queueFamilyIndex, pCounterCount, pCounters, pCounterDescriptions);
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR(
    VkPhysicalDevice                            physicalDevice,
    const VkQueryPoolPerformanceCreateInfoKHR*  pPerformanceQueryCreateInfo,
    uint32_t*                                   pNumPasses) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    disp->GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR(unwrapped_phys_dev, pPerformanceQueryCreateInfo, pNumPasses);
}

VKAPI_ATTR VkResult VKAPI_CALL AcquireProfilingLockKHR(
    VkDevice                                    device,
    const VkAcquireProfilingLockInfoKHR*        pInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkAcquireProfilingLockKHR: Invalid VkDevice [VUID-vkAcquireProfilingLockKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->AcquireProfilingLockKHR(device, pInfo);
}

VKAPI_ATTR void VKAPI_CALL ReleaseProfilingLockKHR(
    VkDevice                                    device) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkReleaseProfilingLockKHR: Invalid VkDevice [VUID-vkReleaseProfilingLockKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->ReleaseProfilingLockKHR(device);
}

// --------------------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_fragment_shading_rate trampolines ----
// --------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceFragmentShadingRatesKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pFragmentShadingRateCount,
    VkPhysicalDeviceFragmentShadingRateKHR*     pFragmentShadingRates) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceFragmentShadingRatesKHR: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceFragmentShadingRatesKHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceFragmentShadingRatesKHR(unwrapped_phys_dev, pFragmentShadingRateCount, pFragmentShadingRates);
}

VKAPI_ATTR void VKAPI_CALL CmdSetFragmentShadingRateKHR(
    VkCommandBuffer                             commandBuffer,
    const VkExtent2D*                           pFragmentSize,
    const VkFragmentShadingRateCombinerOpKHR    combinerOps[2]) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdSetFragmentShadingRateKHR: Invalid VkCommandBuffer [VUID-vkCmdSetFragmentShadingRateKHR-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdSetFragmentShadingRateKHR(commandBuffer, pFragmentSize, combinerOps);
}

// -----------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_present_wait trampolines ----
// -----------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL WaitForPresentKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    uint64_t                                    presentId,
    uint64_t                                    timeout) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkWaitForPresentKHR: Invalid VkDevice [VUID-vkWaitForPresentKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->WaitForPresentKHR(device, swapchain, presentId, timeout);
}

// -----------------------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_deferred_host_operations trampolines ----
// -----------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL CreateDeferredOperationKHR(
    VkDevice                                    device,
    const VkAllocationCallbacks*                pAllocator,
    VkDeferredOperationKHR*                     pDeferredOperation) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateDeferredOperationKHR: Invalid VkDevice [VUID-vkCreateDeferredOperationKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CreateDeferredOperationKHR(device, pAllocator, pDeferredOperation);
}

VKAPI_ATTR void VKAPI_CALL DestroyDeferredOperationKHR(
    VkDevice                                    device,
    VkDeferredOperationKHR                      operation,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkDestroyDeferredOperationKHR: Invalid VkDevice [VUID-vkDestroyDeferredOperationKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->DestroyDeferredOperationKHR(device, operation, pAllocator);
}

VKAPI_ATTR uint32_t VKAPI_CALL GetDeferredOperationMaxConcurrencyKHR(
    VkDevice                                    device,
    VkDeferredOperationKHR                      operation) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetDeferredOperationMaxConcurrencyKHR: Invalid VkDevice [VUID-vkGetDeferredOperationMaxConcurrencyKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetDeferredOperationMaxConcurrencyKHR(device, operation);
}

VKAPI_ATTR VkResult VKAPI_CALL GetDeferredOperationResultKHR(
    VkDevice                                    device,
    VkDeferredOperationKHR                      operation) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetDeferredOperationResultKHR: Invalid VkDevice [VUID-vkGetDeferredOperationResultKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetDeferredOperationResultKHR(device, operation);
}

VKAPI_ATTR VkResult VKAPI_CALL DeferredOperationJoinKHR(
    VkDevice                                    device,
    VkDeferredOperationKHR                      operation) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkDeferredOperationJoinKHR: Invalid VkDevice [VUID-vkDeferredOperationJoinKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->DeferredOperationJoinKHR(device, operation);
}

// -----------------------------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_pipeline_executable_properties trampolines ----
// -----------------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL GetPipelineExecutablePropertiesKHR(
    VkDevice                                    device,
    const VkPipelineInfoKHR*                    pPipelineInfo,
    uint32_t*                                   pExecutableCount,
    VkPipelineExecutablePropertiesKHR*          pProperties) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPipelineExecutablePropertiesKHR: Invalid VkDevice [VUID-vkGetPipelineExecutablePropertiesKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetPipelineExecutablePropertiesKHR(device, pPipelineInfo, pExecutableCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL GetPipelineExecutableStatisticsKHR(
    VkDevice                                    device,
    const VkPipelineExecutableInfoKHR*          pExecutableInfo,
    uint32_t*                                   pStatisticCount,
    VkPipelineExecutableStatisticKHR*           pStatistics) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPipelineExecutableStatisticsKHR: Invalid VkDevice [VUID-vkGetPipelineExecutableStatisticsKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetPipelineExecutableStatisticsKHR(device, pExecutableInfo, pStatisticCount, pStatistics);
}

VKAPI_ATTR VkResult VKAPI_CALL GetPipelineExecutableInternalRepresentationsKHR(
    VkDevice                                    device,
    const VkPipelineExecutableInfoKHR*          pExecutableInfo,
    uint32_t*                                   pInternalRepresentationCount,
    VkPipelineExecutableInternalRepresentationKHR* pInternalRepresentations) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPipelineExecutableInternalRepresentationsKHR: Invalid VkDevice [VUID-vkGetPipelineExecutableInternalRepresentationsKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetPipelineExecutableInternalRepresentationsKHR(device, pExecutableInfo, pInternalRepresentationCount, pInternalRepresentations);
}

// -----------------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_video_encode_queue trampolines ----
// -----------------------------------------------------------------------

#ifdef VK_ENABLE_BETA_EXTENSIONS

VKAPI_ATTR void VKAPI_CALL CmdEncodeVideoKHR(
    VkCommandBuffer                             commandBuffer,
    const VkVideoEncodeInfoKHR*                 pEncodeInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdEncodeVideoKHR: Invalid VkCommandBuffer [VUID-vkCmdEncodeVideoKHR-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdEncodeVideoKHR(commandBuffer, pEncodeInfo);
}
#endif // VK_ENABLE_BETA_EXTENSIONS

// ---------------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_synchronization2 trampolines ----
// ---------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL CmdWriteBufferMarker2AMD(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlags2                       stage,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    uint32_t                                    marker) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdWriteBufferMarker2AMD: Invalid VkCommandBuffer [VUID-vkCmdWriteBufferMarker2AMD-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdWriteBufferMarker2AMD(commandBuffer, stage, dstBuffer, dstOffset, marker);
}

VKAPI_ATTR void VKAPI_CALL GetQueueCheckpointData2NV(
    VkQueue                                     queue,
    uint32_t*                                   pCheckpointDataCount,
    VkCheckpointData2NV*                        pCheckpointData) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(queue);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetQueueCheckpointData2NV: Invalid VkQueue [VUID-vkGetQueueCheckpointData2NV-queue-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->GetQueueCheckpointData2NV(queue, pCheckpointDataCount, pCheckpointData);
}

// -----------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_debug_marker trampolines ----
// -----------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL DebugMarkerSetObjectTagEXT(
    VkDevice                                    device,
    const VkDebugMarkerObjectTagInfoEXT*        pTagInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkDebugMarkerSetObjectTagEXT: Invalid VkDevice [VUID-vkDebugMarkerSetObjectTagEXT-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    VkDebugMarkerObjectTagInfoEXT local_tag_info;
    memcpy(&local_tag_info, pTagInfo, sizeof(VkDebugMarkerObjectTagInfoEXT));
    // If this is a physical device, we have to replace it with the proper one for the next call.
    if (pTagInfo->objectType == VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT) {
        struct loader_physical_device_tramp *phys_dev_tramp = (struct loader_physical_device_tramp *)(uintptr_t)pTagInfo->object;
        local_tag_info.object = (uint64_t)(uintptr_t)phys_dev_tramp->phys_dev;
    }
    return disp->DebugMarkerSetObjectTagEXT(device, &local_tag_info);
}

VKAPI_ATTR VkResult VKAPI_CALL DebugMarkerSetObjectNameEXT(
    VkDevice                                    device,
    const VkDebugMarkerObjectNameInfoEXT*       pNameInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkDebugMarkerSetObjectNameEXT: Invalid VkDevice [VUID-vkDebugMarkerSetObjectNameEXT-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    VkDebugMarkerObjectNameInfoEXT local_name_info;
    memcpy(&local_name_info, pNameInfo, sizeof(VkDebugMarkerObjectNameInfoEXT));
    // If this is a physical device, we have to replace it with the proper one for the next call.
    if (pNameInfo->objectType == VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT) {
        struct loader_physical_device_tramp *phys_dev_tramp = (struct loader_physical_device_tramp *)(uintptr_t)pNameInfo->object;
        local_name_info.object = (uint64_t)(uintptr_t)phys_dev_tramp->phys_dev;
    }
    return disp->DebugMarkerSetObjectNameEXT(device, &local_name_info);
}

VKAPI_ATTR void VKAPI_CALL CmdDebugMarkerBeginEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugMarkerMarkerInfoEXT*           pMarkerInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdDebugMarkerBeginEXT: Invalid VkCommandBuffer [VUID-vkCmdDebugMarkerBeginEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdDebugMarkerBeginEXT(commandBuffer, pMarkerInfo);
}

VKAPI_ATTR void VKAPI_CALL CmdDebugMarkerEndEXT(
    VkCommandBuffer                             commandBuffer) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdDebugMarkerEndEXT: Invalid VkCommandBuffer [VUID-vkCmdDebugMarkerEndEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdDebugMarkerEndEXT(commandBuffer);
}

VKAPI_ATTR void VKAPI_CALL CmdDebugMarkerInsertEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugMarkerMarkerInfoEXT*           pMarkerInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdDebugMarkerInsertEXT: Invalid VkCommandBuffer [VUID-vkCmdDebugMarkerInsertEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdDebugMarkerInsertEXT(commandBuffer, pMarkerInfo);
}

// -----------------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_transform_feedback trampolines ----
// -----------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL CmdBindTransformFeedbackBuffersEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstBinding,
    uint32_t                                    bindingCount,
    const VkBuffer*                             pBuffers,
    const VkDeviceSize*                         pOffsets,
    const VkDeviceSize*                         pSizes) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdBindTransformFeedbackBuffersEXT: Invalid VkCommandBuffer [VUID-vkCmdBindTransformFeedbackBuffersEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdBindTransformFeedbackBuffersEXT(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes);
}

VKAPI_ATTR void VKAPI_CALL CmdBeginTransformFeedbackEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstCounterBuffer,
    uint32_t                                    counterBufferCount,
    const VkBuffer*                             pCounterBuffers,
    const VkDeviceSize*                         pCounterBufferOffsets) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdBeginTransformFeedbackEXT: Invalid VkCommandBuffer [VUID-vkCmdBeginTransformFeedbackEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdBeginTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
}

VKAPI_ATTR void VKAPI_CALL CmdEndTransformFeedbackEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstCounterBuffer,
    uint32_t                                    counterBufferCount,
    const VkBuffer*                             pCounterBuffers,
    const VkDeviceSize*                         pCounterBufferOffsets) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdEndTransformFeedbackEXT: Invalid VkCommandBuffer [VUID-vkCmdEndTransformFeedbackEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdEndTransformFeedbackEXT(commandBuffer, firstCounterBuffer, counterBufferCount, pCounterBuffers, pCounterBufferOffsets);
}

VKAPI_ATTR void VKAPI_CALL CmdBeginQueryIndexedEXT(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query,
    VkQueryControlFlags                         flags,
    uint32_t                                    index) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdBeginQueryIndexedEXT: Invalid VkCommandBuffer [VUID-vkCmdBeginQueryIndexedEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdBeginQueryIndexedEXT(commandBuffer, queryPool, query, flags, index);
}

VKAPI_ATTR void VKAPI_CALL CmdEndQueryIndexedEXT(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query,
    uint32_t                                    index) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdEndQueryIndexedEXT: Invalid VkCommandBuffer [VUID-vkCmdEndQueryIndexedEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdEndQueryIndexedEXT(commandBuffer, queryPool, query, index);
}

VKAPI_ATTR void VKAPI_CALL CmdDrawIndirectByteCountEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    instanceCount,
    uint32_t                                    firstInstance,
    VkBuffer                                    counterBuffer,
    VkDeviceSize                                counterBufferOffset,
    uint32_t                                    counterOffset,
    uint32_t                                    vertexStride) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdDrawIndirectByteCountEXT: Invalid VkCommandBuffer [VUID-vkCmdDrawIndirectByteCountEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdDrawIndirectByteCountEXT(commandBuffer, instanceCount, firstInstance, counterBuffer, counterBufferOffset, counterOffset, vertexStride);
}

// ------------------------------------------------------------------
// ---- Vulkan device extension VK_NVX_binary_import trampolines ----
// ------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL CreateCuModuleNVX(
    VkDevice                                    device,
    const VkCuModuleCreateInfoNVX*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkCuModuleNVX*                              pModule) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateCuModuleNVX: Invalid VkDevice [VUID-vkCreateCuModuleNVX-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CreateCuModuleNVX(device, pCreateInfo, pAllocator, pModule);
}

VKAPI_ATTR VkResult VKAPI_CALL CreateCuFunctionNVX(
    VkDevice                                    device,
    const VkCuFunctionCreateInfoNVX*            pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkCuFunctionNVX*                            pFunction) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateCuFunctionNVX: Invalid VkDevice [VUID-vkCreateCuFunctionNVX-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CreateCuFunctionNVX(device, pCreateInfo, pAllocator, pFunction);
}

VKAPI_ATTR void VKAPI_CALL DestroyCuModuleNVX(
    VkDevice                                    device,
    VkCuModuleNVX                               module,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkDestroyCuModuleNVX: Invalid VkDevice [VUID-vkDestroyCuModuleNVX-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->DestroyCuModuleNVX(device, module, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL DestroyCuFunctionNVX(
    VkDevice                                    device,
    VkCuFunctionNVX                             function,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkDestroyCuFunctionNVX: Invalid VkDevice [VUID-vkDestroyCuFunctionNVX-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->DestroyCuFunctionNVX(device, function, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL CmdCuLaunchKernelNVX(
    VkCommandBuffer                             commandBuffer,
    const VkCuLaunchInfoNVX*                    pLaunchInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdCuLaunchKernelNVX: Invalid VkCommandBuffer [VUID-vkCmdCuLaunchKernelNVX-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdCuLaunchKernelNVX(commandBuffer, pLaunchInfo);
}

// ----------------------------------------------------------------------
// ---- Vulkan device extension VK_NVX_image_view_handle trampolines ----
// ----------------------------------------------------------------------


VKAPI_ATTR uint32_t VKAPI_CALL GetImageViewHandleNVX(
    VkDevice                                    device,
    const VkImageViewHandleInfoNVX*             pInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetImageViewHandleNVX: Invalid VkDevice [VUID-vkGetImageViewHandleNVX-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetImageViewHandleNVX(device, pInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL GetImageViewAddressNVX(
    VkDevice                                    device,
    VkImageView                                 imageView,
    VkImageViewAddressPropertiesNVX*            pProperties) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetImageViewAddressNVX: Invalid VkDevice [VUID-vkGetImageViewAddressNVX-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetImageViewAddressNVX(device, imageView, pProperties);
}

// ----------------------------------------------------------------
// ---- Vulkan device extension VK_AMD_shader_info trampolines ----
// ----------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL GetShaderInfoAMD(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    VkShaderStageFlagBits                       shaderStage,
    VkShaderInfoTypeAMD                         infoType,
    size_t*                                     pInfoSize,
    void*                                       pInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetShaderInfoAMD: Invalid VkDevice [VUID-vkGetShaderInfoAMD-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetShaderInfoAMD(device, pipeline, shaderStage, infoType, pInfoSize, pInfo);
}

// ----------------------------------------------------------------------------------
// ---- Vulkan instance extension VK_NV_external_memory_capabilities trampolines ----
// ----------------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceExternalImageFormatPropertiesNV(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkImageType                                 type,
    VkImageTiling                               tiling,
    VkImageUsageFlags                           usage,
    VkImageCreateFlags                          flags,
    VkExternalMemoryHandleTypeFlagsNV           externalHandleType,
    VkExternalImageFormatPropertiesNV*          pExternalImageFormatProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceExternalImageFormatPropertiesNV: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceExternalImageFormatPropertiesNV-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceExternalImageFormatPropertiesNV(unwrapped_phys_dev, format, type, tiling, usage, flags, externalHandleType, pExternalImageFormatProperties);
}

// -------------------------------------------------------------------------
// ---- Vulkan device extension VK_NV_external_memory_win32 trampolines ----
// -------------------------------------------------------------------------

#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryWin32HandleNV(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkExternalMemoryHandleTypeFlagsNV           handleType,
    HANDLE*                                     pHandle) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetMemoryWin32HandleNV: Invalid VkDevice [VUID-vkGetMemoryWin32HandleNV-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetMemoryWin32HandleNV(device, memory, handleType, pHandle);
}
#endif // VK_USE_PLATFORM_WIN32_KHR

// --------------------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_conditional_rendering trampolines ----
// --------------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL CmdBeginConditionalRenderingEXT(
    VkCommandBuffer                             commandBuffer,
    const VkConditionalRenderingBeginInfoEXT*   pConditionalRenderingBegin) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdBeginConditionalRenderingEXT: Invalid VkCommandBuffer [VUID-vkCmdBeginConditionalRenderingEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdBeginConditionalRenderingEXT(commandBuffer, pConditionalRenderingBegin);
}

VKAPI_ATTR void VKAPI_CALL CmdEndConditionalRenderingEXT(
    VkCommandBuffer                             commandBuffer) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdEndConditionalRenderingEXT: Invalid VkCommandBuffer [VUID-vkCmdEndConditionalRenderingEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdEndConditionalRenderingEXT(commandBuffer);
}

// ------------------------------------------------------------------------
// ---- Vulkan device extension VK_NV_clip_space_w_scaling trampolines ----
// ------------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL CmdSetViewportWScalingNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstViewport,
    uint32_t                                    viewportCount,
    const VkViewportWScalingNV*                 pViewportWScalings) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdSetViewportWScalingNV: Invalid VkCommandBuffer [VUID-vkCmdSetViewportWScalingNV-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdSetViewportWScalingNV(commandBuffer, firstViewport, viewportCount, pViewportWScalings);
}

// --------------------------------------------------------------------------
// ---- Vulkan instance extension VK_EXT_direct_mode_display trampolines ----
// --------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL ReleaseDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkReleaseDisplayEXT: Invalid VkPhysicalDevice [VUID-vkReleaseDisplayEXT-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->ReleaseDisplayEXT(unwrapped_phys_dev, display);
}

// ---------------------------------------------------------------------------
// ---- Vulkan instance extension VK_EXT_acquire_xlib_display trampolines ----
// ---------------------------------------------------------------------------

#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT

VKAPI_ATTR VkResult VKAPI_CALL AcquireXlibDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    Display*                                    dpy,
    VkDisplayKHR                                display) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkAcquireXlibDisplayEXT: Invalid VkPhysicalDevice [VUID-vkAcquireXlibDisplayEXT-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->AcquireXlibDisplayEXT(unwrapped_phys_dev, dpy, display);
}

VKAPI_ATTR VkResult VKAPI_CALL GetRandROutputDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    Display*                                    dpy,
    RROutput                                    rrOutput,
    VkDisplayKHR*                               pDisplay) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetRandROutputDisplayEXT: Invalid VkPhysicalDevice [VUID-vkGetRandROutputDisplayEXT-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetRandROutputDisplayEXT(unwrapped_phys_dev, dpy, rrOutput, pDisplay);
}
#endif // VK_USE_PLATFORM_XLIB_XRANDR_EXT

// ------------------------------------------------------------------------------
// ---- Vulkan instance extension VK_EXT_display_surface_counter trampolines ----
// ------------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceCapabilities2EXT(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    VkSurfaceCapabilities2EXT*                  pSurfaceCapabilities) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceSurfaceCapabilities2EXT: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceSurfaceCapabilities2EXT-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceSurfaceCapabilities2EXT(unwrapped_phys_dev, surface, pSurfaceCapabilities);
}

// --------------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_display_control trampolines ----
// --------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL DisplayPowerControlEXT(
    VkDevice                                    device,
    VkDisplayKHR                                display,
    const VkDisplayPowerInfoEXT*                pDisplayPowerInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkDisplayPowerControlEXT: Invalid VkDevice [VUID-vkDisplayPowerControlEXT-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->DisplayPowerControlEXT(device, display, pDisplayPowerInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL RegisterDeviceEventEXT(
    VkDevice                                    device,
    const VkDeviceEventInfoEXT*                 pDeviceEventInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkRegisterDeviceEventEXT: Invalid VkDevice [VUID-vkRegisterDeviceEventEXT-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->RegisterDeviceEventEXT(device, pDeviceEventInfo, pAllocator, pFence);
}

VKAPI_ATTR VkResult VKAPI_CALL RegisterDisplayEventEXT(
    VkDevice                                    device,
    VkDisplayKHR                                display,
    const VkDisplayEventInfoEXT*                pDisplayEventInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkRegisterDisplayEventEXT: Invalid VkDevice [VUID-vkRegisterDisplayEventEXT-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->RegisterDisplayEventEXT(device, display, pDisplayEventInfo, pAllocator, pFence);
}

VKAPI_ATTR VkResult VKAPI_CALL GetSwapchainCounterEXT(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    VkSurfaceCounterFlagBitsEXT                 counter,
    uint64_t*                                   pCounterValue) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetSwapchainCounterEXT: Invalid VkDevice [VUID-vkGetSwapchainCounterEXT-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetSwapchainCounterEXT(device, swapchain, counter, pCounterValue);
}

// ----------------------------------------------------------------------
// ---- Vulkan device extension VK_GOOGLE_display_timing trampolines ----
// ----------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL GetRefreshCycleDurationGOOGLE(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    VkRefreshCycleDurationGOOGLE*               pDisplayTimingProperties) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetRefreshCycleDurationGOOGLE: Invalid VkDevice [VUID-vkGetRefreshCycleDurationGOOGLE-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetRefreshCycleDurationGOOGLE(device, swapchain, pDisplayTimingProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL GetPastPresentationTimingGOOGLE(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    uint32_t*                                   pPresentationTimingCount,
    VkPastPresentationTimingGOOGLE*             pPresentationTimings) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPastPresentationTimingGOOGLE: Invalid VkDevice [VUID-vkGetPastPresentationTimingGOOGLE-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetPastPresentationTimingGOOGLE(device, swapchain, pPresentationTimingCount, pPresentationTimings);
}

// -----------------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_discard_rectangles trampolines ----
// -----------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL CmdSetDiscardRectangleEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstDiscardRectangle,
    uint32_t                                    discardRectangleCount,
    const VkRect2D*                             pDiscardRectangles) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdSetDiscardRectangleEXT: Invalid VkCommandBuffer [VUID-vkCmdSetDiscardRectangleEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdSetDiscardRectangleEXT(commandBuffer, firstDiscardRectangle, discardRectangleCount, pDiscardRectangles);
}

// -----------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_hdr_metadata trampolines ----
// -----------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL SetHdrMetadataEXT(
    VkDevice                                    device,
    uint32_t                                    swapchainCount,
    const VkSwapchainKHR*                       pSwapchains,
    const VkHdrMetadataEXT*                     pMetadata) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkSetHdrMetadataEXT: Invalid VkDevice [VUID-vkSetHdrMetadataEXT-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->SetHdrMetadataEXT(device, swapchainCount, pSwapchains, pMetadata);
}

// ------------------------------------------------------------------
// ---- Vulkan instance extension VK_EXT_debug_utils trampolines ----
// ------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL SetDebugUtilsObjectNameEXT(
    VkDevice                                    device,
    const VkDebugUtilsObjectNameInfoEXT*        pNameInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkSetDebugUtilsObjectNameEXT: Invalid VkDevice [VUID-vkSetDebugUtilsObjectNameEXT-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    VkDebugUtilsObjectNameInfoEXT local_name_info;
    memcpy(&local_name_info, pNameInfo, sizeof(VkDebugUtilsObjectNameInfoEXT));
    // If this is a physical device, we have to replace it with the proper one for the next call.
    if (pNameInfo->objectType == VK_OBJECT_TYPE_PHYSICAL_DEVICE) {
        struct loader_physical_device_tramp *phys_dev_tramp = (struct loader_physical_device_tramp *)(uintptr_t)pNameInfo->objectHandle;
        local_name_info.objectHandle = (uint64_t)(uintptr_t)phys_dev_tramp->phys_dev;
    }
    if (disp->SetDebugUtilsObjectNameEXT != NULL) {
        return disp->SetDebugUtilsObjectNameEXT(device, &local_name_info);
    } else {
        return VK_SUCCESS;
    }
}

VKAPI_ATTR VkResult VKAPI_CALL SetDebugUtilsObjectTagEXT(
    VkDevice                                    device,
    const VkDebugUtilsObjectTagInfoEXT*         pTagInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkSetDebugUtilsObjectTagEXT: Invalid VkDevice [VUID-vkSetDebugUtilsObjectTagEXT-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    VkDebugUtilsObjectTagInfoEXT local_tag_info;
    memcpy(&local_tag_info, pTagInfo, sizeof(VkDebugUtilsObjectTagInfoEXT));
    // If this is a physical device, we have to replace it with the proper one for the next call.
    if (pTagInfo->objectType == VK_OBJECT_TYPE_PHYSICAL_DEVICE) {
        struct loader_physical_device_tramp *phys_dev_tramp = (struct loader_physical_device_tramp *)(uintptr_t)pTagInfo->objectHandle;
        local_tag_info.objectHandle = (uint64_t)(uintptr_t)phys_dev_tramp->phys_dev;
    }
    if (disp->SetDebugUtilsObjectTagEXT != NULL) {
        return disp->SetDebugUtilsObjectTagEXT(device, &local_tag_info);
    } else {
        return VK_SUCCESS;
    }
}

VKAPI_ATTR void VKAPI_CALL QueueBeginDebugUtilsLabelEXT(
    VkQueue                                     queue,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(queue);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkQueueBeginDebugUtilsLabelEXT: Invalid VkQueue [VUID-vkQueueBeginDebugUtilsLabelEXT-queue-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    if (disp->QueueBeginDebugUtilsLabelEXT != NULL) {
        disp->QueueBeginDebugUtilsLabelEXT(queue, pLabelInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL QueueEndDebugUtilsLabelEXT(
    VkQueue                                     queue) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(queue);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkQueueEndDebugUtilsLabelEXT: Invalid VkQueue [VUID-vkQueueEndDebugUtilsLabelEXT-queue-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    if (disp->QueueEndDebugUtilsLabelEXT != NULL) {
        disp->QueueEndDebugUtilsLabelEXT(queue);
    }
}

VKAPI_ATTR void VKAPI_CALL QueueInsertDebugUtilsLabelEXT(
    VkQueue                                     queue,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(queue);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkQueueInsertDebugUtilsLabelEXT: Invalid VkQueue [VUID-vkQueueInsertDebugUtilsLabelEXT-queue-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    if (disp->QueueInsertDebugUtilsLabelEXT != NULL) {
        disp->QueueInsertDebugUtilsLabelEXT(queue, pLabelInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdBeginDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdBeginDebugUtilsLabelEXT: Invalid VkCommandBuffer [VUID-vkCmdBeginDebugUtilsLabelEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    if (disp->CmdBeginDebugUtilsLabelEXT != NULL) {
        disp->CmdBeginDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdEndDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdEndDebugUtilsLabelEXT: Invalid VkCommandBuffer [VUID-vkCmdEndDebugUtilsLabelEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    if (disp->CmdEndDebugUtilsLabelEXT != NULL) {
        disp->CmdEndDebugUtilsLabelEXT(commandBuffer);
    }
}

VKAPI_ATTR void VKAPI_CALL CmdInsertDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdInsertDebugUtilsLabelEXT: Invalid VkCommandBuffer [VUID-vkCmdInsertDebugUtilsLabelEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    if (disp->CmdInsertDebugUtilsLabelEXT != NULL) {
        disp->CmdInsertDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
    }
}

// ---------------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_sample_locations trampolines ----
// ---------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL CmdSetSampleLocationsEXT(
    VkCommandBuffer                             commandBuffer,
    const VkSampleLocationsInfoEXT*             pSampleLocationsInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdSetSampleLocationsEXT: Invalid VkCommandBuffer [VUID-vkCmdSetSampleLocationsEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdSetSampleLocationsEXT(commandBuffer, pSampleLocationsInfo);
}

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceMultisamplePropertiesEXT(
    VkPhysicalDevice                            physicalDevice,
    VkSampleCountFlagBits                       samples,
    VkMultisamplePropertiesEXT*                 pMultisampleProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceMultisamplePropertiesEXT: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceMultisamplePropertiesEXT-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    disp->GetPhysicalDeviceMultisamplePropertiesEXT(unwrapped_phys_dev, samples, pMultisampleProperties);
}

// ------------------------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_image_drm_format_modifier trampolines ----
// ------------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL GetImageDrmFormatModifierPropertiesEXT(
    VkDevice                                    device,
    VkImage                                     image,
    VkImageDrmFormatModifierPropertiesEXT*      pProperties) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetImageDrmFormatModifierPropertiesEXT: Invalid VkDevice [VUID-vkGetImageDrmFormatModifierPropertiesEXT-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetImageDrmFormatModifierPropertiesEXT(device, image, pProperties);
}

// ---------------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_validation_cache trampolines ----
// ---------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL CreateValidationCacheEXT(
    VkDevice                                    device,
    const VkValidationCacheCreateInfoEXT*       pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkValidationCacheEXT*                       pValidationCache) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateValidationCacheEXT: Invalid VkDevice [VUID-vkCreateValidationCacheEXT-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CreateValidationCacheEXT(device, pCreateInfo, pAllocator, pValidationCache);
}

VKAPI_ATTR void VKAPI_CALL DestroyValidationCacheEXT(
    VkDevice                                    device,
    VkValidationCacheEXT                        validationCache,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkDestroyValidationCacheEXT: Invalid VkDevice [VUID-vkDestroyValidationCacheEXT-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->DestroyValidationCacheEXT(device, validationCache, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL MergeValidationCachesEXT(
    VkDevice                                    device,
    VkValidationCacheEXT                        dstCache,
    uint32_t                                    srcCacheCount,
    const VkValidationCacheEXT*                 pSrcCaches) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkMergeValidationCachesEXT: Invalid VkDevice [VUID-vkMergeValidationCachesEXT-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->MergeValidationCachesEXT(device, dstCache, srcCacheCount, pSrcCaches);
}

VKAPI_ATTR VkResult VKAPI_CALL GetValidationCacheDataEXT(
    VkDevice                                    device,
    VkValidationCacheEXT                        validationCache,
    size_t*                                     pDataSize,
    void*                                       pData) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetValidationCacheDataEXT: Invalid VkDevice [VUID-vkGetValidationCacheDataEXT-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetValidationCacheDataEXT(device, validationCache, pDataSize, pData);
}

// ----------------------------------------------------------------------
// ---- Vulkan device extension VK_NV_shading_rate_image trampolines ----
// ----------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL CmdBindShadingRateImageNV(
    VkCommandBuffer                             commandBuffer,
    VkImageView                                 imageView,
    VkImageLayout                               imageLayout) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdBindShadingRateImageNV: Invalid VkCommandBuffer [VUID-vkCmdBindShadingRateImageNV-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdBindShadingRateImageNV(commandBuffer, imageView, imageLayout);
}

VKAPI_ATTR void VKAPI_CALL CmdSetViewportShadingRatePaletteNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstViewport,
    uint32_t                                    viewportCount,
    const VkShadingRatePaletteNV*               pShadingRatePalettes) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdSetViewportShadingRatePaletteNV: Invalid VkCommandBuffer [VUID-vkCmdSetViewportShadingRatePaletteNV-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdSetViewportShadingRatePaletteNV(commandBuffer, firstViewport, viewportCount, pShadingRatePalettes);
}

VKAPI_ATTR void VKAPI_CALL CmdSetCoarseSampleOrderNV(
    VkCommandBuffer                             commandBuffer,
    VkCoarseSampleOrderTypeNV                   sampleOrderType,
    uint32_t                                    customSampleOrderCount,
    const VkCoarseSampleOrderCustomNV*          pCustomSampleOrders) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdSetCoarseSampleOrderNV: Invalid VkCommandBuffer [VUID-vkCmdSetCoarseSampleOrderNV-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdSetCoarseSampleOrderNV(commandBuffer, sampleOrderType, customSampleOrderCount, pCustomSampleOrders);
}

// ---------------------------------------------------------------
// ---- Vulkan device extension VK_NV_ray_tracing trampolines ----
// ---------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL CreateAccelerationStructureNV(
    VkDevice                                    device,
    const VkAccelerationStructureCreateInfoNV*  pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkAccelerationStructureNV*                  pAccelerationStructure) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateAccelerationStructureNV: Invalid VkDevice [VUID-vkCreateAccelerationStructureNV-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CreateAccelerationStructureNV(device, pCreateInfo, pAllocator, pAccelerationStructure);
}

VKAPI_ATTR void VKAPI_CALL DestroyAccelerationStructureNV(
    VkDevice                                    device,
    VkAccelerationStructureNV                   accelerationStructure,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkDestroyAccelerationStructureNV: Invalid VkDevice [VUID-vkDestroyAccelerationStructureNV-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->DestroyAccelerationStructureNV(device, accelerationStructure, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL GetAccelerationStructureMemoryRequirementsNV(
    VkDevice                                    device,
    const VkAccelerationStructureMemoryRequirementsInfoNV* pInfo,
    VkMemoryRequirements2KHR*                   pMemoryRequirements) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetAccelerationStructureMemoryRequirementsNV: Invalid VkDevice [VUID-vkGetAccelerationStructureMemoryRequirementsNV-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->GetAccelerationStructureMemoryRequirementsNV(device, pInfo, pMemoryRequirements);
}

VKAPI_ATTR VkResult VKAPI_CALL BindAccelerationStructureMemoryNV(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindAccelerationStructureMemoryInfoNV* pBindInfos) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkBindAccelerationStructureMemoryNV: Invalid VkDevice [VUID-vkBindAccelerationStructureMemoryNV-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->BindAccelerationStructureMemoryNV(device, bindInfoCount, pBindInfos);
}

VKAPI_ATTR void VKAPI_CALL CmdBuildAccelerationStructureNV(
    VkCommandBuffer                             commandBuffer,
    const VkAccelerationStructureInfoNV*        pInfo,
    VkBuffer                                    instanceData,
    VkDeviceSize                                instanceOffset,
    VkBool32                                    update,
    VkAccelerationStructureNV                   dst,
    VkAccelerationStructureNV                   src,
    VkBuffer                                    scratch,
    VkDeviceSize                                scratchOffset) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdBuildAccelerationStructureNV: Invalid VkCommandBuffer [VUID-vkCmdBuildAccelerationStructureNV-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdBuildAccelerationStructureNV(commandBuffer, pInfo, instanceData, instanceOffset, update, dst, src, scratch, scratchOffset);
}

VKAPI_ATTR void VKAPI_CALL CmdCopyAccelerationStructureNV(
    VkCommandBuffer                             commandBuffer,
    VkAccelerationStructureNV                   dst,
    VkAccelerationStructureNV                   src,
    VkCopyAccelerationStructureModeKHR          mode) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdCopyAccelerationStructureNV: Invalid VkCommandBuffer [VUID-vkCmdCopyAccelerationStructureNV-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdCopyAccelerationStructureNV(commandBuffer, dst, src, mode);
}

VKAPI_ATTR void VKAPI_CALL CmdTraceRaysNV(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    raygenShaderBindingTableBuffer,
    VkDeviceSize                                raygenShaderBindingOffset,
    VkBuffer                                    missShaderBindingTableBuffer,
    VkDeviceSize                                missShaderBindingOffset,
    VkDeviceSize                                missShaderBindingStride,
    VkBuffer                                    hitShaderBindingTableBuffer,
    VkDeviceSize                                hitShaderBindingOffset,
    VkDeviceSize                                hitShaderBindingStride,
    VkBuffer                                    callableShaderBindingTableBuffer,
    VkDeviceSize                                callableShaderBindingOffset,
    VkDeviceSize                                callableShaderBindingStride,
    uint32_t                                    width,
    uint32_t                                    height,
    uint32_t                                    depth) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdTraceRaysNV: Invalid VkCommandBuffer [VUID-vkCmdTraceRaysNV-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdTraceRaysNV(commandBuffer, raygenShaderBindingTableBuffer, raygenShaderBindingOffset, missShaderBindingTableBuffer, missShaderBindingOffset, missShaderBindingStride, hitShaderBindingTableBuffer, hitShaderBindingOffset, hitShaderBindingStride, callableShaderBindingTableBuffer, callableShaderBindingOffset, callableShaderBindingStride, width, height, depth);
}

VKAPI_ATTR VkResult VKAPI_CALL CreateRayTracingPipelinesNV(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkRayTracingPipelineCreateInfoNV*     pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateRayTracingPipelinesNV: Invalid VkDevice [VUID-vkCreateRayTracingPipelinesNV-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CreateRayTracingPipelinesNV(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}

VKAPI_ATTR VkResult VKAPI_CALL GetAccelerationStructureHandleNV(
    VkDevice                                    device,
    VkAccelerationStructureNV                   accelerationStructure,
    size_t                                      dataSize,
    void*                                       pData) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetAccelerationStructureHandleNV: Invalid VkDevice [VUID-vkGetAccelerationStructureHandleNV-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetAccelerationStructureHandleNV(device, accelerationStructure, dataSize, pData);
}

VKAPI_ATTR void VKAPI_CALL CmdWriteAccelerationStructuresPropertiesNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    accelerationStructureCount,
    const VkAccelerationStructureNV*            pAccelerationStructures,
    VkQueryType                                 queryType,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdWriteAccelerationStructuresPropertiesNV: Invalid VkCommandBuffer [VUID-vkCmdWriteAccelerationStructuresPropertiesNV-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdWriteAccelerationStructuresPropertiesNV(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
}

VKAPI_ATTR VkResult VKAPI_CALL CompileDeferredNV(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    uint32_t                                    shader) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCompileDeferredNV: Invalid VkDevice [VUID-vkCompileDeferredNV-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CompileDeferredNV(device, pipeline, shader);
}

// -------------------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_external_memory_host trampolines ----
// -------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL GetMemoryHostPointerPropertiesEXT(
    VkDevice                                    device,
    VkExternalMemoryHandleTypeFlagBits          handleType,
    const void*                                 pHostPointer,
    VkMemoryHostPointerPropertiesEXT*           pMemoryHostPointerProperties) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetMemoryHostPointerPropertiesEXT: Invalid VkDevice [VUID-vkGetMemoryHostPointerPropertiesEXT-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetMemoryHostPointerPropertiesEXT(device, handleType, pHostPointer, pMemoryHostPointerProperties);
}

// ------------------------------------------------------------------
// ---- Vulkan device extension VK_AMD_buffer_marker trampolines ----
// ------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL CmdWriteBufferMarkerAMD(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlagBits                     pipelineStage,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    uint32_t                                    marker) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdWriteBufferMarkerAMD: Invalid VkCommandBuffer [VUID-vkCmdWriteBufferMarkerAMD-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdWriteBufferMarkerAMD(commandBuffer, pipelineStage, dstBuffer, dstOffset, marker);
}

// --------------------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_calibrated_timestamps trampolines ----
// --------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceCalibrateableTimeDomainsEXT(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pTimeDomainCount,
    VkTimeDomainEXT*                            pTimeDomains) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceCalibrateableTimeDomainsEXT-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceCalibrateableTimeDomainsEXT(unwrapped_phys_dev, pTimeDomainCount, pTimeDomains);
}

VKAPI_ATTR VkResult VKAPI_CALL GetCalibratedTimestampsEXT(
    VkDevice                                    device,
    uint32_t                                    timestampCount,
    const VkCalibratedTimestampInfoEXT*         pTimestampInfos,
    uint64_t*                                   pTimestamps,
    uint64_t*                                   pMaxDeviation) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetCalibratedTimestampsEXT: Invalid VkDevice [VUID-vkGetCalibratedTimestampsEXT-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetCalibratedTimestampsEXT(device, timestampCount, pTimestampInfos, pTimestamps, pMaxDeviation);
}

// ---------------------------------------------------------------
// ---- Vulkan device extension VK_NV_mesh_shader trampolines ----
// ---------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL CmdDrawMeshTasksNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    taskCount,
    uint32_t                                    firstTask) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdDrawMeshTasksNV: Invalid VkCommandBuffer [VUID-vkCmdDrawMeshTasksNV-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdDrawMeshTasksNV(commandBuffer, taskCount, firstTask);
}

VKAPI_ATTR void VKAPI_CALL CmdDrawMeshTasksIndirectNV(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    uint32_t                                    drawCount,
    uint32_t                                    stride) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdDrawMeshTasksIndirectNV: Invalid VkCommandBuffer [VUID-vkCmdDrawMeshTasksIndirectNV-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdDrawMeshTasksIndirectNV(commandBuffer, buffer, offset, drawCount, stride);
}

VKAPI_ATTR void VKAPI_CALL CmdDrawMeshTasksIndirectCountNV(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdDrawMeshTasksIndirectCountNV: Invalid VkCommandBuffer [VUID-vkCmdDrawMeshTasksIndirectCountNV-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdDrawMeshTasksIndirectCountNV(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

// ---------------------------------------------------------------------
// ---- Vulkan device extension VK_NV_scissor_exclusive trampolines ----
// ---------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL CmdSetExclusiveScissorNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstExclusiveScissor,
    uint32_t                                    exclusiveScissorCount,
    const VkRect2D*                             pExclusiveScissors) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdSetExclusiveScissorNV: Invalid VkCommandBuffer [VUID-vkCmdSetExclusiveScissorNV-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdSetExclusiveScissorNV(commandBuffer, firstExclusiveScissor, exclusiveScissorCount, pExclusiveScissors);
}

// ---------------------------------------------------------------------------------
// ---- Vulkan device extension VK_NV_device_diagnostic_checkpoints trampolines ----
// ---------------------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL CmdSetCheckpointNV(
    VkCommandBuffer                             commandBuffer,
    const void*                                 pCheckpointMarker) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdSetCheckpointNV: Invalid VkCommandBuffer [VUID-vkCmdSetCheckpointNV-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdSetCheckpointNV(commandBuffer, pCheckpointMarker);
}

VKAPI_ATTR void VKAPI_CALL GetQueueCheckpointDataNV(
    VkQueue                                     queue,
    uint32_t*                                   pCheckpointDataCount,
    VkCheckpointDataNV*                         pCheckpointData) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(queue);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetQueueCheckpointDataNV: Invalid VkQueue [VUID-vkGetQueueCheckpointDataNV-queue-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->GetQueueCheckpointDataNV(queue, pCheckpointDataCount, pCheckpointData);
}

// ------------------------------------------------------------------------
// ---- Vulkan device extension VK_INTEL_performance_query trampolines ----
// ------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL InitializePerformanceApiINTEL(
    VkDevice                                    device,
    const VkInitializePerformanceApiInfoINTEL*  pInitializeInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkInitializePerformanceApiINTEL: Invalid VkDevice [VUID-vkInitializePerformanceApiINTEL-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->InitializePerformanceApiINTEL(device, pInitializeInfo);
}

VKAPI_ATTR void VKAPI_CALL UninitializePerformanceApiINTEL(
    VkDevice                                    device) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkUninitializePerformanceApiINTEL: Invalid VkDevice [VUID-vkUninitializePerformanceApiINTEL-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->UninitializePerformanceApiINTEL(device);
}

VKAPI_ATTR VkResult VKAPI_CALL CmdSetPerformanceMarkerINTEL(
    VkCommandBuffer                             commandBuffer,
    const VkPerformanceMarkerInfoINTEL*         pMarkerInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdSetPerformanceMarkerINTEL: Invalid VkCommandBuffer [VUID-vkCmdSetPerformanceMarkerINTEL-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CmdSetPerformanceMarkerINTEL(commandBuffer, pMarkerInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL CmdSetPerformanceStreamMarkerINTEL(
    VkCommandBuffer                             commandBuffer,
    const VkPerformanceStreamMarkerInfoINTEL*   pMarkerInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdSetPerformanceStreamMarkerINTEL: Invalid VkCommandBuffer [VUID-vkCmdSetPerformanceStreamMarkerINTEL-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CmdSetPerformanceStreamMarkerINTEL(commandBuffer, pMarkerInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL CmdSetPerformanceOverrideINTEL(
    VkCommandBuffer                             commandBuffer,
    const VkPerformanceOverrideInfoINTEL*       pOverrideInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdSetPerformanceOverrideINTEL: Invalid VkCommandBuffer [VUID-vkCmdSetPerformanceOverrideINTEL-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CmdSetPerformanceOverrideINTEL(commandBuffer, pOverrideInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL AcquirePerformanceConfigurationINTEL(
    VkDevice                                    device,
    const VkPerformanceConfigurationAcquireInfoINTEL* pAcquireInfo,
    VkPerformanceConfigurationINTEL*            pConfiguration) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkAcquirePerformanceConfigurationINTEL: Invalid VkDevice [VUID-vkAcquirePerformanceConfigurationINTEL-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->AcquirePerformanceConfigurationINTEL(device, pAcquireInfo, pConfiguration);
}

VKAPI_ATTR VkResult VKAPI_CALL ReleasePerformanceConfigurationINTEL(
    VkDevice                                    device,
    VkPerformanceConfigurationINTEL             configuration) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkReleasePerformanceConfigurationINTEL: Invalid VkDevice [VUID-vkReleasePerformanceConfigurationINTEL-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->ReleasePerformanceConfigurationINTEL(device, configuration);
}

VKAPI_ATTR VkResult VKAPI_CALL QueueSetPerformanceConfigurationINTEL(
    VkQueue                                     queue,
    VkPerformanceConfigurationINTEL             configuration) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(queue);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkQueueSetPerformanceConfigurationINTEL: Invalid VkQueue [VUID-vkQueueSetPerformanceConfigurationINTEL-queue-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->QueueSetPerformanceConfigurationINTEL(queue, configuration);
}

VKAPI_ATTR VkResult VKAPI_CALL GetPerformanceParameterINTEL(
    VkDevice                                    device,
    VkPerformanceParameterTypeINTEL             parameter,
    VkPerformanceValueINTEL*                    pValue) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPerformanceParameterINTEL: Invalid VkDevice [VUID-vkGetPerformanceParameterINTEL-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetPerformanceParameterINTEL(device, parameter, pValue);
}

// -----------------------------------------------------------------------
// ---- Vulkan device extension VK_AMD_display_native_hdr trampolines ----
// -----------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL SetLocalDimmingAMD(
    VkDevice                                    device,
    VkSwapchainKHR                              swapChain,
    VkBool32                                    localDimmingEnable) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkSetLocalDimmingAMD: Invalid VkDevice [VUID-vkSetLocalDimmingAMD-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->SetLocalDimmingAMD(device, swapChain, localDimmingEnable);
}

// ----------------------------------------------------------------------
// ---- Vulkan device extension VK_NV_cooperative_matrix trampolines ----
// ----------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceCooperativeMatrixPropertiesNV(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkCooperativeMatrixPropertiesNV*            pProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceCooperativeMatrixPropertiesNV: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceCooperativeMatrixPropertiesNV-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceCooperativeMatrixPropertiesNV(unwrapped_phys_dev, pPropertyCount, pProperties);
}

// ---------------------------------------------------------------------------
// ---- Vulkan device extension VK_NV_coverage_reduction_mode trampolines ----
// ---------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pCombinationCount,
    VkFramebufferMixedSamplesCombinationNV*     pCombinations) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV(unwrapped_phys_dev, pCombinationCount, pCombinations);
}

// --------------------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_full_screen_exclusive trampolines ----
// --------------------------------------------------------------------------

#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfacePresentModes2EXT(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceSurfaceInfo2KHR*      pSurfaceInfo,
    uint32_t*                                   pPresentModeCount,
    VkPresentModeKHR*                           pPresentModes) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetPhysicalDeviceSurfacePresentModes2EXT: Invalid VkPhysicalDevice [VUID-vkGetPhysicalDeviceSurfacePresentModes2EXT-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceSurfacePresentModes2EXT(unwrapped_phys_dev, pSurfaceInfo, pPresentModeCount, pPresentModes);
}

VKAPI_ATTR VkResult VKAPI_CALL AcquireFullScreenExclusiveModeEXT(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkAcquireFullScreenExclusiveModeEXT: Invalid VkDevice [VUID-vkAcquireFullScreenExclusiveModeEXT-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->AcquireFullScreenExclusiveModeEXT(device, swapchain);
}

VKAPI_ATTR VkResult VKAPI_CALL ReleaseFullScreenExclusiveModeEXT(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkReleaseFullScreenExclusiveModeEXT: Invalid VkDevice [VUID-vkReleaseFullScreenExclusiveModeEXT-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->ReleaseFullScreenExclusiveModeEXT(device, swapchain);
}

VKAPI_ATTR VkResult VKAPI_CALL GetDeviceGroupSurfacePresentModes2EXT(
    VkDevice                                    device,
    const VkPhysicalDeviceSurfaceInfo2KHR*      pSurfaceInfo,
    VkDeviceGroupPresentModeFlagsKHR*           pModes) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetDeviceGroupSurfacePresentModes2EXT: Invalid VkDevice [VUID-vkGetDeviceGroupSurfacePresentModes2EXT-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetDeviceGroupSurfacePresentModes2EXT(device, pSurfaceInfo, pModes);
}
#endif // VK_USE_PLATFORM_WIN32_KHR

// -----------------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_line_rasterization trampolines ----
// -----------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL CmdSetLineStippleEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    lineStippleFactor,
    uint16_t                                    lineStipplePattern) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdSetLineStippleEXT: Invalid VkCommandBuffer [VUID-vkCmdSetLineStippleEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdSetLineStippleEXT(commandBuffer, lineStippleFactor, lineStipplePattern);
}

// -----------------------------------------------------------------------------
// ---- Vulkan device extension VK_NV_device_generated_commands trampolines ----
// -----------------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL GetGeneratedCommandsMemoryRequirementsNV(
    VkDevice                                    device,
    const VkGeneratedCommandsMemoryRequirementsInfoNV* pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetGeneratedCommandsMemoryRequirementsNV: Invalid VkDevice [VUID-vkGetGeneratedCommandsMemoryRequirementsNV-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->GetGeneratedCommandsMemoryRequirementsNV(device, pInfo, pMemoryRequirements);
}

VKAPI_ATTR void VKAPI_CALL CmdPreprocessGeneratedCommandsNV(
    VkCommandBuffer                             commandBuffer,
    const VkGeneratedCommandsInfoNV*            pGeneratedCommandsInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdPreprocessGeneratedCommandsNV: Invalid VkCommandBuffer [VUID-vkCmdPreprocessGeneratedCommandsNV-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdPreprocessGeneratedCommandsNV(commandBuffer, pGeneratedCommandsInfo);
}

VKAPI_ATTR void VKAPI_CALL CmdExecuteGeneratedCommandsNV(
    VkCommandBuffer                             commandBuffer,
    VkBool32                                    isPreprocessed,
    const VkGeneratedCommandsInfoNV*            pGeneratedCommandsInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdExecuteGeneratedCommandsNV: Invalid VkCommandBuffer [VUID-vkCmdExecuteGeneratedCommandsNV-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdExecuteGeneratedCommandsNV(commandBuffer, isPreprocessed, pGeneratedCommandsInfo);
}

VKAPI_ATTR void VKAPI_CALL CmdBindPipelineShaderGroupNV(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipeline                                  pipeline,
    uint32_t                                    groupIndex) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdBindPipelineShaderGroupNV: Invalid VkCommandBuffer [VUID-vkCmdBindPipelineShaderGroupNV-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdBindPipelineShaderGroupNV(commandBuffer, pipelineBindPoint, pipeline, groupIndex);
}

VKAPI_ATTR VkResult VKAPI_CALL CreateIndirectCommandsLayoutNV(
    VkDevice                                    device,
    const VkIndirectCommandsLayoutCreateInfoNV* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkIndirectCommandsLayoutNV*                 pIndirectCommandsLayout) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateIndirectCommandsLayoutNV: Invalid VkDevice [VUID-vkCreateIndirectCommandsLayoutNV-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CreateIndirectCommandsLayoutNV(device, pCreateInfo, pAllocator, pIndirectCommandsLayout);
}

VKAPI_ATTR void VKAPI_CALL DestroyIndirectCommandsLayoutNV(
    VkDevice                                    device,
    VkIndirectCommandsLayoutNV                  indirectCommandsLayout,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkDestroyIndirectCommandsLayoutNV: Invalid VkDevice [VUID-vkDestroyIndirectCommandsLayoutNV-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->DestroyIndirectCommandsLayoutNV(device, indirectCommandsLayout, pAllocator);
}

// --------------------------------------------------------------------------
// ---- Vulkan instance extension VK_EXT_acquire_drm_display trampolines ----
// --------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL AcquireDrmDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    int32_t                                     drmFd,
    VkDisplayKHR                                display) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkAcquireDrmDisplayEXT: Invalid VkPhysicalDevice [VUID-vkAcquireDrmDisplayEXT-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->AcquireDrmDisplayEXT(unwrapped_phys_dev, drmFd, display);
}

VKAPI_ATTR VkResult VKAPI_CALL GetDrmDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    int32_t                                     drmFd,
    uint32_t                                    connectorId,
    VkDisplayKHR*                               display) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetDrmDisplayEXT: Invalid VkPhysicalDevice [VUID-vkGetDrmDisplayEXT-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetDrmDisplayEXT(unwrapped_phys_dev, drmFd, connectorId, display);
}

// -------------------------------------------------------------------------------
// ---- Vulkan device extension VK_NV_fragment_shading_rate_enums trampolines ----
// -------------------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL CmdSetFragmentShadingRateEnumNV(
    VkCommandBuffer                             commandBuffer,
    VkFragmentShadingRateNV                     shadingRate,
    const VkFragmentShadingRateCombinerOpKHR    combinerOps[2]) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdSetFragmentShadingRateEnumNV: Invalid VkCommandBuffer [VUID-vkCmdSetFragmentShadingRateEnumNV-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdSetFragmentShadingRateEnumNV(commandBuffer, shadingRate, combinerOps);
}

// -------------------------------------------------------------------------
// ---- Vulkan device extension VK_NV_acquire_winrt_display trampolines ----
// -------------------------------------------------------------------------

#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL AcquireWinrtDisplayNV(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkAcquireWinrtDisplayNV: Invalid VkPhysicalDevice [VUID-vkAcquireWinrtDisplayNV-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->AcquireWinrtDisplayNV(unwrapped_phys_dev, display);
}

VKAPI_ATTR VkResult VKAPI_CALL GetWinrtDisplayNV(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    deviceRelativeId,
    VkDisplayKHR*                               pDisplay) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetWinrtDisplayNV: Invalid VkPhysicalDevice [VUID-vkGetWinrtDisplayNV-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetWinrtDisplayNV(unwrapped_phys_dev, deviceRelativeId, pDisplay);
}
#endif // VK_USE_PLATFORM_WIN32_KHR

// -------------------------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_vertex_input_dynamic_state trampolines ----
// -------------------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL CmdSetVertexInputEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    vertexBindingDescriptionCount,
    const VkVertexInputBindingDescription2EXT*  pVertexBindingDescriptions,
    uint32_t                                    vertexAttributeDescriptionCount,
    const VkVertexInputAttributeDescription2EXT* pVertexAttributeDescriptions) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdSetVertexInputEXT: Invalid VkCommandBuffer [VUID-vkCmdSetVertexInputEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdSetVertexInputEXT(commandBuffer, vertexBindingDescriptionCount, pVertexBindingDescriptions, vertexAttributeDescriptionCount, pVertexAttributeDescriptions);
}

// ------------------------------------------------------------------------
// ---- Vulkan device extension VK_FUCHSIA_external_memory trampolines ----
// ------------------------------------------------------------------------

#ifdef VK_USE_PLATFORM_FUCHSIA

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryZirconHandleFUCHSIA(
    VkDevice                                    device,
    const VkMemoryGetZirconHandleInfoFUCHSIA*   pGetZirconHandleInfo,
    zx_handle_t*                                pZirconHandle) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetMemoryZirconHandleFUCHSIA: Invalid VkDevice [VUID-vkGetMemoryZirconHandleFUCHSIA-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetMemoryZirconHandleFUCHSIA(device, pGetZirconHandleInfo, pZirconHandle);
}

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryZirconHandlePropertiesFUCHSIA(
    VkDevice                                    device,
    VkExternalMemoryHandleTypeFlagBits          handleType,
    zx_handle_t                                 zirconHandle,
    VkMemoryZirconHandlePropertiesFUCHSIA*      pMemoryZirconHandleProperties) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetMemoryZirconHandlePropertiesFUCHSIA: Invalid VkDevice [VUID-vkGetMemoryZirconHandlePropertiesFUCHSIA-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetMemoryZirconHandlePropertiesFUCHSIA(device, handleType, zirconHandle, pMemoryZirconHandleProperties);
}
#endif // VK_USE_PLATFORM_FUCHSIA

// ---------------------------------------------------------------------------
// ---- Vulkan device extension VK_FUCHSIA_external_semaphore trampolines ----
// ---------------------------------------------------------------------------

#ifdef VK_USE_PLATFORM_FUCHSIA

VKAPI_ATTR VkResult VKAPI_CALL ImportSemaphoreZirconHandleFUCHSIA(
    VkDevice                                    device,
    const VkImportSemaphoreZirconHandleInfoFUCHSIA* pImportSemaphoreZirconHandleInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkImportSemaphoreZirconHandleFUCHSIA: Invalid VkDevice [VUID-vkImportSemaphoreZirconHandleFUCHSIA-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->ImportSemaphoreZirconHandleFUCHSIA(device, pImportSemaphoreZirconHandleInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL GetSemaphoreZirconHandleFUCHSIA(
    VkDevice                                    device,
    const VkSemaphoreGetZirconHandleInfoFUCHSIA* pGetZirconHandleInfo,
    zx_handle_t*                                pZirconHandle) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetSemaphoreZirconHandleFUCHSIA: Invalid VkDevice [VUID-vkGetSemaphoreZirconHandleFUCHSIA-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetSemaphoreZirconHandleFUCHSIA(device, pGetZirconHandleInfo, pZirconHandle);
}
#endif // VK_USE_PLATFORM_FUCHSIA

// --------------------------------------------------------------------------
// ---- Vulkan device extension VK_FUCHSIA_buffer_collection trampolines ----
// --------------------------------------------------------------------------

#ifdef VK_USE_PLATFORM_FUCHSIA

VKAPI_ATTR VkResult VKAPI_CALL CreateBufferCollectionFUCHSIA(
    VkDevice                                    device,
    const VkBufferCollectionCreateInfoFUCHSIA*  pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkBufferCollectionFUCHSIA*                  pCollection) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateBufferCollectionFUCHSIA: Invalid VkDevice [VUID-vkCreateBufferCollectionFUCHSIA-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CreateBufferCollectionFUCHSIA(device, pCreateInfo, pAllocator, pCollection);
}

VKAPI_ATTR VkResult VKAPI_CALL SetBufferCollectionImageConstraintsFUCHSIA(
    VkDevice                                    device,
    VkBufferCollectionFUCHSIA                   collection,
    const VkImageConstraintsInfoFUCHSIA*        pImageConstraintsInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkSetBufferCollectionImageConstraintsFUCHSIA: Invalid VkDevice [VUID-vkSetBufferCollectionImageConstraintsFUCHSIA-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->SetBufferCollectionImageConstraintsFUCHSIA(device, collection, pImageConstraintsInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL SetBufferCollectionBufferConstraintsFUCHSIA(
    VkDevice                                    device,
    VkBufferCollectionFUCHSIA                   collection,
    const VkBufferConstraintsInfoFUCHSIA*       pBufferConstraintsInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkSetBufferCollectionBufferConstraintsFUCHSIA: Invalid VkDevice [VUID-vkSetBufferCollectionBufferConstraintsFUCHSIA-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->SetBufferCollectionBufferConstraintsFUCHSIA(device, collection, pBufferConstraintsInfo);
}

VKAPI_ATTR void VKAPI_CALL DestroyBufferCollectionFUCHSIA(
    VkDevice                                    device,
    VkBufferCollectionFUCHSIA                   collection,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkDestroyBufferCollectionFUCHSIA: Invalid VkDevice [VUID-vkDestroyBufferCollectionFUCHSIA-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->DestroyBufferCollectionFUCHSIA(device, collection, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL GetBufferCollectionPropertiesFUCHSIA(
    VkDevice                                    device,
    VkBufferCollectionFUCHSIA                   collection,
    VkBufferCollectionPropertiesFUCHSIA*        pProperties) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetBufferCollectionPropertiesFUCHSIA: Invalid VkDevice [VUID-vkGetBufferCollectionPropertiesFUCHSIA-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetBufferCollectionPropertiesFUCHSIA(device, collection, pProperties);
}
#endif // VK_USE_PLATFORM_FUCHSIA

// -----------------------------------------------------------------------
// ---- Vulkan device extension VK_HUAWEI_subpass_shading trampolines ----
// -----------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL GetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI(
    VkDevice                                    device,
    VkRenderPass                                renderpass,
    VkExtent2D*                                 pMaxWorkgroupSize) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI: Invalid VkDevice [VUID-vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI(device, renderpass, pMaxWorkgroupSize);
}

VKAPI_ATTR void VKAPI_CALL CmdSubpassShadingHUAWEI(
    VkCommandBuffer                             commandBuffer) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdSubpassShadingHUAWEI: Invalid VkCommandBuffer [VUID-vkCmdSubpassShadingHUAWEI-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdSubpassShadingHUAWEI(commandBuffer);
}

// -----------------------------------------------------------------------
// ---- Vulkan device extension VK_HUAWEI_invocation_mask trampolines ----
// -----------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL CmdBindInvocationMaskHUAWEI(
    VkCommandBuffer                             commandBuffer,
    VkImageView                                 imageView,
    VkImageLayout                               imageLayout) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdBindInvocationMaskHUAWEI: Invalid VkCommandBuffer [VUID-vkCmdBindInvocationMaskHUAWEI-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdBindInvocationMaskHUAWEI(commandBuffer, imageView, imageLayout);
}

// ------------------------------------------------------------------------
// ---- Vulkan device extension VK_NV_external_memory_rdma trampolines ----
// ------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL GetMemoryRemoteAddressNV(
    VkDevice                                    device,
    const VkMemoryGetRemoteAddressInfoNV*       pMemoryGetRemoteAddressInfo,
    VkRemoteAddressNV*                          pAddress) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetMemoryRemoteAddressNV: Invalid VkDevice [VUID-vkGetMemoryRemoteAddressNV-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetMemoryRemoteAddressNV(device, pMemoryGetRemoteAddressInfo, pAddress);
}

// ----------------------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_extended_dynamic_state2 trampolines ----
// ----------------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL CmdSetPatchControlPointsEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    patchControlPoints) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdSetPatchControlPointsEXT: Invalid VkCommandBuffer [VUID-vkCmdSetPatchControlPointsEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdSetPatchControlPointsEXT(commandBuffer, patchControlPoints);
}

VKAPI_ATTR void VKAPI_CALL CmdSetLogicOpEXT(
    VkCommandBuffer                             commandBuffer,
    VkLogicOp                                   logicOp) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdSetLogicOpEXT: Invalid VkCommandBuffer [VUID-vkCmdSetLogicOpEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdSetLogicOpEXT(commandBuffer, logicOp);
}

// -----------------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_color_write_enable trampolines ----
// -----------------------------------------------------------------------


VKAPI_ATTR void                                    VKAPI_CALL CmdSetColorWriteEnableEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    attachmentCount,
    const VkBool32*                             pColorWriteEnables) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdSetColorWriteEnableEXT: Invalid VkCommandBuffer [VUID-vkCmdSetColorWriteEnableEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdSetColorWriteEnableEXT(commandBuffer, attachmentCount, pColorWriteEnables);
}

// ---------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_multi_draw trampolines ----
// ---------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL CmdDrawMultiEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    drawCount,
    const VkMultiDrawInfoEXT*                   pVertexInfo,
    uint32_t                                    instanceCount,
    uint32_t                                    firstInstance,
    uint32_t                                    stride) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdDrawMultiEXT: Invalid VkCommandBuffer [VUID-vkCmdDrawMultiEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdDrawMultiEXT(commandBuffer, drawCount, pVertexInfo, instanceCount, firstInstance, stride);
}

VKAPI_ATTR void VKAPI_CALL CmdDrawMultiIndexedEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    drawCount,
    const VkMultiDrawIndexedInfoEXT*            pIndexInfo,
    uint32_t                                    instanceCount,
    uint32_t                                    firstInstance,
    uint32_t                                    stride,
    const int32_t*                              pVertexOffset) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdDrawMultiIndexedEXT: Invalid VkCommandBuffer [VUID-vkCmdDrawMultiIndexedEXT-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdDrawMultiIndexedEXT(commandBuffer, drawCount, pIndexInfo, instanceCount, firstInstance, stride, pVertexOffset);
}

// ---------------------------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_pageable_device_local_memory trampolines ----
// ---------------------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL SetDeviceMemoryPriorityEXT(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    float                                       priority) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkSetDeviceMemoryPriorityEXT: Invalid VkDevice [VUID-vkSetDeviceMemoryPriorityEXT-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->SetDeviceMemoryPriorityEXT(device, memory, priority);
}

// ----------------------------------------------------------------------------------
// ---- Vulkan device extension VK_VALVE_descriptor_set_host_mapping trampolines ----
// ----------------------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL GetDescriptorSetLayoutHostMappingInfoVALVE(
    VkDevice                                    device,
    const VkDescriptorSetBindingReferenceVALVE* pBindingReference,
    VkDescriptorSetLayoutHostMappingInfoVALVE*  pHostMapping) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetDescriptorSetLayoutHostMappingInfoVALVE: Invalid VkDevice [VUID-vkGetDescriptorSetLayoutHostMappingInfoVALVE-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->GetDescriptorSetLayoutHostMappingInfoVALVE(device, pBindingReference, pHostMapping);
}

VKAPI_ATTR void VKAPI_CALL GetDescriptorSetHostMappingVALVE(
    VkDevice                                    device,
    VkDescriptorSet                             descriptorSet,
    void**                                      ppData) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetDescriptorSetHostMappingVALVE: Invalid VkDevice [VUID-vkGetDescriptorSetHostMappingVALVE-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->GetDescriptorSetHostMappingVALVE(device, descriptorSet, ppData);
}

// ---------------------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_acceleration_structure trampolines ----
// ---------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL CreateAccelerationStructureKHR(
    VkDevice                                    device,
    const VkAccelerationStructureCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkAccelerationStructureKHR*                 pAccelerationStructure) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateAccelerationStructureKHR: Invalid VkDevice [VUID-vkCreateAccelerationStructureKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CreateAccelerationStructureKHR(device, pCreateInfo, pAllocator, pAccelerationStructure);
}

VKAPI_ATTR void VKAPI_CALL DestroyAccelerationStructureKHR(
    VkDevice                                    device,
    VkAccelerationStructureKHR                  accelerationStructure,
    const VkAllocationCallbacks*                pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkDestroyAccelerationStructureKHR: Invalid VkDevice [VUID-vkDestroyAccelerationStructureKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->DestroyAccelerationStructureKHR(device, accelerationStructure, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL CmdBuildAccelerationStructuresKHR(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    infoCount,
    const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
    const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdBuildAccelerationStructuresKHR: Invalid VkCommandBuffer [VUID-vkCmdBuildAccelerationStructuresKHR-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdBuildAccelerationStructuresKHR(commandBuffer, infoCount, pInfos, ppBuildRangeInfos);
}

VKAPI_ATTR void VKAPI_CALL CmdBuildAccelerationStructuresIndirectKHR(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    infoCount,
    const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
    const VkDeviceAddress*                      pIndirectDeviceAddresses,
    const uint32_t*                             pIndirectStrides,
    const uint32_t* const*                      ppMaxPrimitiveCounts) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdBuildAccelerationStructuresIndirectKHR: Invalid VkCommandBuffer [VUID-vkCmdBuildAccelerationStructuresIndirectKHR-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdBuildAccelerationStructuresIndirectKHR(commandBuffer, infoCount, pInfos, pIndirectDeviceAddresses, pIndirectStrides, ppMaxPrimitiveCounts);
}

VKAPI_ATTR VkResult VKAPI_CALL BuildAccelerationStructuresKHR(
    VkDevice                                    device,
    VkDeferredOperationKHR                      deferredOperation,
    uint32_t                                    infoCount,
    const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
    const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkBuildAccelerationStructuresKHR: Invalid VkDevice [VUID-vkBuildAccelerationStructuresKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->BuildAccelerationStructuresKHR(device, deferredOperation, infoCount, pInfos, ppBuildRangeInfos);
}

VKAPI_ATTR VkResult VKAPI_CALL CopyAccelerationStructureKHR(
    VkDevice                                    device,
    VkDeferredOperationKHR                      deferredOperation,
    const VkCopyAccelerationStructureInfoKHR*   pInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCopyAccelerationStructureKHR: Invalid VkDevice [VUID-vkCopyAccelerationStructureKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CopyAccelerationStructureKHR(device, deferredOperation, pInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL CopyAccelerationStructureToMemoryKHR(
    VkDevice                                    device,
    VkDeferredOperationKHR                      deferredOperation,
    const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCopyAccelerationStructureToMemoryKHR: Invalid VkDevice [VUID-vkCopyAccelerationStructureToMemoryKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CopyAccelerationStructureToMemoryKHR(device, deferredOperation, pInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL CopyMemoryToAccelerationStructureKHR(
    VkDevice                                    device,
    VkDeferredOperationKHR                      deferredOperation,
    const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCopyMemoryToAccelerationStructureKHR: Invalid VkDevice [VUID-vkCopyMemoryToAccelerationStructureKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CopyMemoryToAccelerationStructureKHR(device, deferredOperation, pInfo);
}

VKAPI_ATTR VkResult VKAPI_CALL WriteAccelerationStructuresPropertiesKHR(
    VkDevice                                    device,
    uint32_t                                    accelerationStructureCount,
    const VkAccelerationStructureKHR*           pAccelerationStructures,
    VkQueryType                                 queryType,
    size_t                                      dataSize,
    void*                                       pData,
    size_t                                      stride) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkWriteAccelerationStructuresPropertiesKHR: Invalid VkDevice [VUID-vkWriteAccelerationStructuresPropertiesKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->WriteAccelerationStructuresPropertiesKHR(device, accelerationStructureCount, pAccelerationStructures, queryType, dataSize, pData, stride);
}

VKAPI_ATTR void VKAPI_CALL CmdCopyAccelerationStructureKHR(
    VkCommandBuffer                             commandBuffer,
    const VkCopyAccelerationStructureInfoKHR*   pInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdCopyAccelerationStructureKHR: Invalid VkCommandBuffer [VUID-vkCmdCopyAccelerationStructureKHR-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdCopyAccelerationStructureKHR(commandBuffer, pInfo);
}

VKAPI_ATTR void VKAPI_CALL CmdCopyAccelerationStructureToMemoryKHR(
    VkCommandBuffer                             commandBuffer,
    const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdCopyAccelerationStructureToMemoryKHR: Invalid VkCommandBuffer [VUID-vkCmdCopyAccelerationStructureToMemoryKHR-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdCopyAccelerationStructureToMemoryKHR(commandBuffer, pInfo);
}

VKAPI_ATTR void VKAPI_CALL CmdCopyMemoryToAccelerationStructureKHR(
    VkCommandBuffer                             commandBuffer,
    const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdCopyMemoryToAccelerationStructureKHR: Invalid VkCommandBuffer [VUID-vkCmdCopyMemoryToAccelerationStructureKHR-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdCopyMemoryToAccelerationStructureKHR(commandBuffer, pInfo);
}

VKAPI_ATTR VkDeviceAddress VKAPI_CALL GetAccelerationStructureDeviceAddressKHR(
    VkDevice                                    device,
    const VkAccelerationStructureDeviceAddressInfoKHR* pInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetAccelerationStructureDeviceAddressKHR: Invalid VkDevice [VUID-vkGetAccelerationStructureDeviceAddressKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetAccelerationStructureDeviceAddressKHR(device, pInfo);
}

VKAPI_ATTR void VKAPI_CALL CmdWriteAccelerationStructuresPropertiesKHR(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    accelerationStructureCount,
    const VkAccelerationStructureKHR*           pAccelerationStructures,
    VkQueryType                                 queryType,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdWriteAccelerationStructuresPropertiesKHR: Invalid VkCommandBuffer [VUID-vkCmdWriteAccelerationStructuresPropertiesKHR-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdWriteAccelerationStructuresPropertiesKHR(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
}

VKAPI_ATTR void VKAPI_CALL GetDeviceAccelerationStructureCompatibilityKHR(
    VkDevice                                    device,
    const VkAccelerationStructureVersionInfoKHR* pVersionInfo,
    VkAccelerationStructureCompatibilityKHR*    pCompatibility) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetDeviceAccelerationStructureCompatibilityKHR: Invalid VkDevice [VUID-vkGetDeviceAccelerationStructureCompatibilityKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->GetDeviceAccelerationStructureCompatibilityKHR(device, pVersionInfo, pCompatibility);
}

VKAPI_ATTR void VKAPI_CALL GetAccelerationStructureBuildSizesKHR(
    VkDevice                                    device,
    VkAccelerationStructureBuildTypeKHR         buildType,
    const VkAccelerationStructureBuildGeometryInfoKHR* pBuildInfo,
    const uint32_t*                             pMaxPrimitiveCounts,
    VkAccelerationStructureBuildSizesInfoKHR*   pSizeInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetAccelerationStructureBuildSizesKHR: Invalid VkDevice [VUID-vkGetAccelerationStructureBuildSizesKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->GetAccelerationStructureBuildSizesKHR(device, buildType, pBuildInfo, pMaxPrimitiveCounts, pSizeInfo);
}

// -------------------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_ray_tracing_pipeline trampolines ----
// -------------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL CmdTraceRaysKHR(
    VkCommandBuffer                             commandBuffer,
    const VkStridedDeviceAddressRegionKHR*      pRaygenShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR*      pMissShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR*      pHitShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR*      pCallableShaderBindingTable,
    uint32_t                                    width,
    uint32_t                                    height,
    uint32_t                                    depth) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdTraceRaysKHR: Invalid VkCommandBuffer [VUID-vkCmdTraceRaysKHR-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdTraceRaysKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, width, height, depth);
}

VKAPI_ATTR VkResult VKAPI_CALL CreateRayTracingPipelinesKHR(
    VkDevice                                    device,
    VkDeferredOperationKHR                      deferredOperation,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkRayTracingPipelineCreateInfoKHR*    pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateRayTracingPipelinesKHR: Invalid VkDevice [VUID-vkCreateRayTracingPipelinesKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CreateRayTracingPipelinesKHR(device, deferredOperation, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}

VKAPI_ATTR VkResult VKAPI_CALL GetRayTracingShaderGroupHandlesKHR(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    uint32_t                                    firstGroup,
    uint32_t                                    groupCount,
    size_t                                      dataSize,
    void*                                       pData) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetRayTracingShaderGroupHandlesKHR: Invalid VkDevice [VUID-vkGetRayTracingShaderGroupHandlesKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetRayTracingShaderGroupHandlesKHR(device, pipeline, firstGroup, groupCount, dataSize, pData);
}

VKAPI_ATTR VkResult VKAPI_CALL GetRayTracingCaptureReplayShaderGroupHandlesKHR(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    uint32_t                                    firstGroup,
    uint32_t                                    groupCount,
    size_t                                      dataSize,
    void*                                       pData) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR: Invalid VkDevice [VUID-vkGetRayTracingCaptureReplayShaderGroupHandlesKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetRayTracingCaptureReplayShaderGroupHandlesKHR(device, pipeline, firstGroup, groupCount, dataSize, pData);
}

VKAPI_ATTR void VKAPI_CALL CmdTraceRaysIndirectKHR(
    VkCommandBuffer                             commandBuffer,
    const VkStridedDeviceAddressRegionKHR*      pRaygenShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR*      pMissShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR*      pHitShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR*      pCallableShaderBindingTable,
    VkDeviceAddress                             indirectDeviceAddress) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdTraceRaysIndirectKHR: Invalid VkCommandBuffer [VUID-vkCmdTraceRaysIndirectKHR-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdTraceRaysIndirectKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, indirectDeviceAddress);
}

VKAPI_ATTR VkDeviceSize VKAPI_CALL GetRayTracingShaderGroupStackSizeKHR(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    uint32_t                                    group,
    VkShaderGroupShaderKHR                      groupShader) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetRayTracingShaderGroupStackSizeKHR: Invalid VkDevice [VUID-vkGetRayTracingShaderGroupStackSizeKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetRayTracingShaderGroupStackSizeKHR(device, pipeline, group, groupShader);
}

VKAPI_ATTR void VKAPI_CALL CmdSetRayTracingPipelineStackSizeKHR(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    pipelineStackSize) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(commandBuffer);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCmdSetRayTracingPipelineStackSizeKHR: Invalid VkCommandBuffer [VUID-vkCmdSetRayTracingPipelineStackSizeKHR-commandBuffer-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->CmdSetRayTracingPipelineStackSizeKHR(commandBuffer, pipelineStackSize);
}

// GPA helpers for extensions
bool extension_instance_gpa(struct loader_instance *ptr_instance, const char *name, void **addr) {
    *addr = NULL;


    // ---- VK_KHR_video_queue extension commands
#ifdef VK_ENABLE_BETA_EXTENSIONS
    if (!strcmp("vkGetPhysicalDeviceVideoCapabilitiesKHR", name)) {
        *addr = (void *)GetPhysicalDeviceVideoCapabilitiesKHR;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceVideoFormatPropertiesKHR", name)) {
        *addr = (void *)GetPhysicalDeviceVideoFormatPropertiesKHR;
        return true;
    }
    if (!strcmp("vkCreateVideoSessionKHR", name)) {
        *addr = (void *)CreateVideoSessionKHR;
        return true;
    }
    if (!strcmp("vkDestroyVideoSessionKHR", name)) {
        *addr = (void *)DestroyVideoSessionKHR;
        return true;
    }
    if (!strcmp("vkGetVideoSessionMemoryRequirementsKHR", name)) {
        *addr = (void *)GetVideoSessionMemoryRequirementsKHR;
        return true;
    }
    if (!strcmp("vkBindVideoSessionMemoryKHR", name)) {
        *addr = (void *)BindVideoSessionMemoryKHR;
        return true;
    }
    if (!strcmp("vkCreateVideoSessionParametersKHR", name)) {
        *addr = (void *)CreateVideoSessionParametersKHR;
        return true;
    }
    if (!strcmp("vkUpdateVideoSessionParametersKHR", name)) {
        *addr = (void *)UpdateVideoSessionParametersKHR;
        return true;
    }
    if (!strcmp("vkDestroyVideoSessionParametersKHR", name)) {
        *addr = (void *)DestroyVideoSessionParametersKHR;
        return true;
    }
    if (!strcmp("vkCmdBeginVideoCodingKHR", name)) {
        *addr = (void *)CmdBeginVideoCodingKHR;
        return true;
    }
    if (!strcmp("vkCmdEndVideoCodingKHR", name)) {
        *addr = (void *)CmdEndVideoCodingKHR;
        return true;
    }
    if (!strcmp("vkCmdControlVideoCodingKHR", name)) {
        *addr = (void *)CmdControlVideoCodingKHR;
        return true;
    }
#endif // VK_ENABLE_BETA_EXTENSIONS

    // ---- VK_KHR_video_decode_queue extension commands
#ifdef VK_ENABLE_BETA_EXTENSIONS
    if (!strcmp("vkCmdDecodeVideoKHR", name)) {
        *addr = (void *)CmdDecodeVideoKHR;
        return true;
    }
#endif // VK_ENABLE_BETA_EXTENSIONS

    // ---- VK_KHR_dynamic_rendering extension commands
    if (!strcmp("vkCmdBeginRenderingKHR", name)) {
        *addr = (void *)vkCmdBeginRendering;
        return true;
    }
    if (!strcmp("vkCmdEndRenderingKHR", name)) {
        *addr = (void *)vkCmdEndRendering;
        return true;
    }

    // ---- VK_KHR_get_physical_device_properties2 extension commands
    if (!strcmp("vkGetPhysicalDeviceFeatures2KHR", name)) {
        *addr = (ptr_instance->inst_ext_enables.khr_get_physical_device_properties2 == 1)
                     ? (void *)vkGetPhysicalDeviceFeatures2
                     : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceProperties2KHR", name)) {
        *addr = (ptr_instance->inst_ext_enables.khr_get_physical_device_properties2 == 1)
                     ? (void *)vkGetPhysicalDeviceProperties2
                     : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceFormatProperties2KHR", name)) {
        *addr = (ptr_instance->inst_ext_enables.khr_get_physical_device_properties2 == 1)
                     ? (void *)vkGetPhysicalDeviceFormatProperties2
                     : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceImageFormatProperties2KHR", name)) {
        *addr = (ptr_instance->inst_ext_enables.khr_get_physical_device_properties2 == 1)
                     ? (void *)vkGetPhysicalDeviceImageFormatProperties2
                     : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceQueueFamilyProperties2KHR", name)) {
        *addr = (ptr_instance->inst_ext_enables.khr_get_physical_device_properties2 == 1)
                     ? (void *)vkGetPhysicalDeviceQueueFamilyProperties2
                     : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceMemoryProperties2KHR", name)) {
        *addr = (ptr_instance->inst_ext_enables.khr_get_physical_device_properties2 == 1)
                     ? (void *)vkGetPhysicalDeviceMemoryProperties2
                     : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceSparseImageFormatProperties2KHR", name)) {
        *addr = (ptr_instance->inst_ext_enables.khr_get_physical_device_properties2 == 1)
                     ? (void *)vkGetPhysicalDeviceSparseImageFormatProperties2
                     : NULL;
        return true;
    }

    // ---- VK_KHR_device_group extension commands
    if (!strcmp("vkGetDeviceGroupPeerMemoryFeaturesKHR", name)) {
        *addr = (void *)vkGetDeviceGroupPeerMemoryFeatures;
        return true;
    }
    if (!strcmp("vkCmdSetDeviceMaskKHR", name)) {
        *addr = (void *)vkCmdSetDeviceMask;
        return true;
    }
    if (!strcmp("vkCmdDispatchBaseKHR", name)) {
        *addr = (void *)vkCmdDispatchBase;
        return true;
    }

    // ---- VK_KHR_maintenance1 extension commands
    if (!strcmp("vkTrimCommandPoolKHR", name)) {
        *addr = (void *)vkTrimCommandPool;
        return true;
    }

    // ---- VK_KHR_device_group_creation extension commands
    if (!strcmp("vkEnumeratePhysicalDeviceGroupsKHR", name)) {
        *addr = (ptr_instance->inst_ext_enables.khr_device_group_creation == 1)
                     ? (void *)vkEnumeratePhysicalDeviceGroups
                     : NULL;
        return true;
    }

    // ---- VK_KHR_external_memory_capabilities extension commands
    if (!strcmp("vkGetPhysicalDeviceExternalBufferPropertiesKHR", name)) {
        *addr = (ptr_instance->inst_ext_enables.khr_external_memory_capabilities == 1)
                     ? (void *)vkGetPhysicalDeviceExternalBufferProperties
                     : NULL;
        return true;
    }

    // ---- VK_KHR_external_memory_win32 extension commands
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (!strcmp("vkGetMemoryWin32HandleKHR", name)) {
        *addr = (void *)GetMemoryWin32HandleKHR;
        return true;
    }
    if (!strcmp("vkGetMemoryWin32HandlePropertiesKHR", name)) {
        *addr = (void *)GetMemoryWin32HandlePropertiesKHR;
        return true;
    }
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ---- VK_KHR_external_memory_fd extension commands
    if (!strcmp("vkGetMemoryFdKHR", name)) {
        *addr = (void *)GetMemoryFdKHR;
        return true;
    }
    if (!strcmp("vkGetMemoryFdPropertiesKHR", name)) {
        *addr = (void *)GetMemoryFdPropertiesKHR;
        return true;
    }

    // ---- VK_KHR_external_semaphore_capabilities extension commands
    if (!strcmp("vkGetPhysicalDeviceExternalSemaphorePropertiesKHR", name)) {
        *addr = (ptr_instance->inst_ext_enables.khr_external_semaphore_capabilities == 1)
                     ? (void *)vkGetPhysicalDeviceExternalSemaphoreProperties
                     : NULL;
        return true;
    }

    // ---- VK_KHR_external_semaphore_win32 extension commands
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (!strcmp("vkImportSemaphoreWin32HandleKHR", name)) {
        *addr = (void *)ImportSemaphoreWin32HandleKHR;
        return true;
    }
    if (!strcmp("vkGetSemaphoreWin32HandleKHR", name)) {
        *addr = (void *)GetSemaphoreWin32HandleKHR;
        return true;
    }
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ---- VK_KHR_external_semaphore_fd extension commands
    if (!strcmp("vkImportSemaphoreFdKHR", name)) {
        *addr = (void *)ImportSemaphoreFdKHR;
        return true;
    }
    if (!strcmp("vkGetSemaphoreFdKHR", name)) {
        *addr = (void *)GetSemaphoreFdKHR;
        return true;
    }

    // ---- VK_KHR_push_descriptor extension commands
    if (!strcmp("vkCmdPushDescriptorSetKHR", name)) {
        *addr = (void *)CmdPushDescriptorSetKHR;
        return true;
    }
    if (!strcmp("vkCmdPushDescriptorSetWithTemplateKHR", name)) {
        *addr = (void *)CmdPushDescriptorSetWithTemplateKHR;
        return true;
    }

    // ---- VK_KHR_descriptor_update_template extension commands
    if (!strcmp("vkCreateDescriptorUpdateTemplateKHR", name)) {
        *addr = (void *)vkCreateDescriptorUpdateTemplate;
        return true;
    }
    if (!strcmp("vkDestroyDescriptorUpdateTemplateKHR", name)) {
        *addr = (void *)vkDestroyDescriptorUpdateTemplate;
        return true;
    }
    if (!strcmp("vkUpdateDescriptorSetWithTemplateKHR", name)) {
        *addr = (void *)vkUpdateDescriptorSetWithTemplate;
        return true;
    }

    // ---- VK_KHR_create_renderpass2 extension commands
    if (!strcmp("vkCreateRenderPass2KHR", name)) {
        *addr = (void *)vkCreateRenderPass2;
        return true;
    }
    if (!strcmp("vkCmdBeginRenderPass2KHR", name)) {
        *addr = (void *)vkCmdBeginRenderPass2;
        return true;
    }
    if (!strcmp("vkCmdNextSubpass2KHR", name)) {
        *addr = (void *)vkCmdNextSubpass2;
        return true;
    }
    if (!strcmp("vkCmdEndRenderPass2KHR", name)) {
        *addr = (void *)vkCmdEndRenderPass2;
        return true;
    }

    // ---- VK_KHR_shared_presentable_image extension commands
    if (!strcmp("vkGetSwapchainStatusKHR", name)) {
        *addr = (void *)GetSwapchainStatusKHR;
        return true;
    }

    // ---- VK_KHR_external_fence_capabilities extension commands
    if (!strcmp("vkGetPhysicalDeviceExternalFencePropertiesKHR", name)) {
        *addr = (ptr_instance->inst_ext_enables.khr_external_fence_capabilities == 1)
                     ? (void *)vkGetPhysicalDeviceExternalFenceProperties
                     : NULL;
        return true;
    }

    // ---- VK_KHR_external_fence_win32 extension commands
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (!strcmp("vkImportFenceWin32HandleKHR", name)) {
        *addr = (void *)ImportFenceWin32HandleKHR;
        return true;
    }
    if (!strcmp("vkGetFenceWin32HandleKHR", name)) {
        *addr = (void *)GetFenceWin32HandleKHR;
        return true;
    }
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ---- VK_KHR_external_fence_fd extension commands
    if (!strcmp("vkImportFenceFdKHR", name)) {
        *addr = (void *)ImportFenceFdKHR;
        return true;
    }
    if (!strcmp("vkGetFenceFdKHR", name)) {
        *addr = (void *)GetFenceFdKHR;
        return true;
    }

    // ---- VK_KHR_performance_query extension commands
    if (!strcmp("vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR", name)) {
        *addr = (void *)EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR", name)) {
        *addr = (void *)GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR;
        return true;
    }
    if (!strcmp("vkAcquireProfilingLockKHR", name)) {
        *addr = (void *)AcquireProfilingLockKHR;
        return true;
    }
    if (!strcmp("vkReleaseProfilingLockKHR", name)) {
        *addr = (void *)ReleaseProfilingLockKHR;
        return true;
    }

    // ---- VK_KHR_get_memory_requirements2 extension commands
    if (!strcmp("vkGetImageMemoryRequirements2KHR", name)) {
        *addr = (void *)vkGetImageMemoryRequirements2;
        return true;
    }
    if (!strcmp("vkGetBufferMemoryRequirements2KHR", name)) {
        *addr = (void *)vkGetBufferMemoryRequirements2;
        return true;
    }
    if (!strcmp("vkGetImageSparseMemoryRequirements2KHR", name)) {
        *addr = (void *)vkGetImageSparseMemoryRequirements2;
        return true;
    }

    // ---- VK_KHR_sampler_ycbcr_conversion extension commands
    if (!strcmp("vkCreateSamplerYcbcrConversionKHR", name)) {
        *addr = (void *)vkCreateSamplerYcbcrConversion;
        return true;
    }
    if (!strcmp("vkDestroySamplerYcbcrConversionKHR", name)) {
        *addr = (void *)vkDestroySamplerYcbcrConversion;
        return true;
    }

    // ---- VK_KHR_bind_memory2 extension commands
    if (!strcmp("vkBindBufferMemory2KHR", name)) {
        *addr = (void *)vkBindBufferMemory2;
        return true;
    }
    if (!strcmp("vkBindImageMemory2KHR", name)) {
        *addr = (void *)vkBindImageMemory2;
        return true;
    }

    // ---- VK_KHR_maintenance3 extension commands
    if (!strcmp("vkGetDescriptorSetLayoutSupportKHR", name)) {
        *addr = (void *)vkGetDescriptorSetLayoutSupport;
        return true;
    }

    // ---- VK_KHR_draw_indirect_count extension commands
    if (!strcmp("vkCmdDrawIndirectCountKHR", name)) {
        *addr = (void *)vkCmdDrawIndirectCount;
        return true;
    }
    if (!strcmp("vkCmdDrawIndexedIndirectCountKHR", name)) {
        *addr = (void *)vkCmdDrawIndexedIndirectCount;
        return true;
    }

    // ---- VK_KHR_timeline_semaphore extension commands
    if (!strcmp("vkGetSemaphoreCounterValueKHR", name)) {
        *addr = (void *)vkGetSemaphoreCounterValue;
        return true;
    }
    if (!strcmp("vkWaitSemaphoresKHR", name)) {
        *addr = (void *)vkWaitSemaphores;
        return true;
    }
    if (!strcmp("vkSignalSemaphoreKHR", name)) {
        *addr = (void *)vkSignalSemaphore;
        return true;
    }

    // ---- VK_KHR_fragment_shading_rate extension commands
    if (!strcmp("vkGetPhysicalDeviceFragmentShadingRatesKHR", name)) {
        *addr = (void *)GetPhysicalDeviceFragmentShadingRatesKHR;
        return true;
    }
    if (!strcmp("vkCmdSetFragmentShadingRateKHR", name)) {
        *addr = (void *)CmdSetFragmentShadingRateKHR;
        return true;
    }

    // ---- VK_KHR_present_wait extension commands
    if (!strcmp("vkWaitForPresentKHR", name)) {
        *addr = (void *)WaitForPresentKHR;
        return true;
    }

    // ---- VK_KHR_buffer_device_address extension commands
    if (!strcmp("vkGetBufferDeviceAddressKHR", name)) {
        *addr = (void *)vkGetBufferDeviceAddress;
        return true;
    }
    if (!strcmp("vkGetBufferOpaqueCaptureAddressKHR", name)) {
        *addr = (void *)vkGetBufferOpaqueCaptureAddress;
        return true;
    }
    if (!strcmp("vkGetDeviceMemoryOpaqueCaptureAddressKHR", name)) {
        *addr = (void *)vkGetDeviceMemoryOpaqueCaptureAddress;
        return true;
    }

    // ---- VK_KHR_deferred_host_operations extension commands
    if (!strcmp("vkCreateDeferredOperationKHR", name)) {
        *addr = (void *)CreateDeferredOperationKHR;
        return true;
    }
    if (!strcmp("vkDestroyDeferredOperationKHR", name)) {
        *addr = (void *)DestroyDeferredOperationKHR;
        return true;
    }
    if (!strcmp("vkGetDeferredOperationMaxConcurrencyKHR", name)) {
        *addr = (void *)GetDeferredOperationMaxConcurrencyKHR;
        return true;
    }
    if (!strcmp("vkGetDeferredOperationResultKHR", name)) {
        *addr = (void *)GetDeferredOperationResultKHR;
        return true;
    }
    if (!strcmp("vkDeferredOperationJoinKHR", name)) {
        *addr = (void *)DeferredOperationJoinKHR;
        return true;
    }

    // ---- VK_KHR_pipeline_executable_properties extension commands
    if (!strcmp("vkGetPipelineExecutablePropertiesKHR", name)) {
        *addr = (void *)GetPipelineExecutablePropertiesKHR;
        return true;
    }
    if (!strcmp("vkGetPipelineExecutableStatisticsKHR", name)) {
        *addr = (void *)GetPipelineExecutableStatisticsKHR;
        return true;
    }
    if (!strcmp("vkGetPipelineExecutableInternalRepresentationsKHR", name)) {
        *addr = (void *)GetPipelineExecutableInternalRepresentationsKHR;
        return true;
    }

    // ---- VK_KHR_video_encode_queue extension commands
#ifdef VK_ENABLE_BETA_EXTENSIONS
    if (!strcmp("vkCmdEncodeVideoKHR", name)) {
        *addr = (void *)CmdEncodeVideoKHR;
        return true;
    }
#endif // VK_ENABLE_BETA_EXTENSIONS

    // ---- VK_KHR_synchronization2 extension commands
    if (!strcmp("vkCmdSetEvent2KHR", name)) {
        *addr = (void *)vkCmdSetEvent2;
        return true;
    }
    if (!strcmp("vkCmdResetEvent2KHR", name)) {
        *addr = (void *)vkCmdResetEvent2;
        return true;
    }
    if (!strcmp("vkCmdWaitEvents2KHR", name)) {
        *addr = (void *)vkCmdWaitEvents2;
        return true;
    }
    if (!strcmp("vkCmdPipelineBarrier2KHR", name)) {
        *addr = (void *)vkCmdPipelineBarrier2;
        return true;
    }
    if (!strcmp("vkCmdWriteTimestamp2KHR", name)) {
        *addr = (void *)vkCmdWriteTimestamp2;
        return true;
    }
    if (!strcmp("vkQueueSubmit2KHR", name)) {
        *addr = (void *)vkQueueSubmit2;
        return true;
    }
    if (!strcmp("vkCmdWriteBufferMarker2AMD", name)) {
        *addr = (void *)CmdWriteBufferMarker2AMD;
        return true;
    }
    if (!strcmp("vkGetQueueCheckpointData2NV", name)) {
        *addr = (void *)GetQueueCheckpointData2NV;
        return true;
    }

    // ---- VK_KHR_copy_commands2 extension commands
    if (!strcmp("vkCmdCopyBuffer2KHR", name)) {
        *addr = (void *)vkCmdCopyBuffer2;
        return true;
    }
    if (!strcmp("vkCmdCopyImage2KHR", name)) {
        *addr = (void *)vkCmdCopyImage2;
        return true;
    }
    if (!strcmp("vkCmdCopyBufferToImage2KHR", name)) {
        *addr = (void *)vkCmdCopyBufferToImage2;
        return true;
    }
    if (!strcmp("vkCmdCopyImageToBuffer2KHR", name)) {
        *addr = (void *)vkCmdCopyImageToBuffer2;
        return true;
    }
    if (!strcmp("vkCmdBlitImage2KHR", name)) {
        *addr = (void *)vkCmdBlitImage2;
        return true;
    }
    if (!strcmp("vkCmdResolveImage2KHR", name)) {
        *addr = (void *)vkCmdResolveImage2;
        return true;
    }

    // ---- VK_KHR_maintenance4 extension commands
    if (!strcmp("vkGetDeviceBufferMemoryRequirementsKHR", name)) {
        *addr = (void *)vkGetDeviceBufferMemoryRequirements;
        return true;
    }
    if (!strcmp("vkGetDeviceImageMemoryRequirementsKHR", name)) {
        *addr = (void *)vkGetDeviceImageMemoryRequirements;
        return true;
    }
    if (!strcmp("vkGetDeviceImageSparseMemoryRequirementsKHR", name)) {
        *addr = (void *)vkGetDeviceImageSparseMemoryRequirements;
        return true;
    }

    // ---- VK_EXT_debug_marker extension commands
    if (!strcmp("vkDebugMarkerSetObjectTagEXT", name)) {
        *addr = (void *)DebugMarkerSetObjectTagEXT;
        return true;
    }
    if (!strcmp("vkDebugMarkerSetObjectNameEXT", name)) {
        *addr = (void *)DebugMarkerSetObjectNameEXT;
        return true;
    }
    if (!strcmp("vkCmdDebugMarkerBeginEXT", name)) {
        *addr = (void *)CmdDebugMarkerBeginEXT;
        return true;
    }
    if (!strcmp("vkCmdDebugMarkerEndEXT", name)) {
        *addr = (void *)CmdDebugMarkerEndEXT;
        return true;
    }
    if (!strcmp("vkCmdDebugMarkerInsertEXT", name)) {
        *addr = (void *)CmdDebugMarkerInsertEXT;
        return true;
    }

    // ---- VK_EXT_transform_feedback extension commands
    if (!strcmp("vkCmdBindTransformFeedbackBuffersEXT", name)) {
        *addr = (void *)CmdBindTransformFeedbackBuffersEXT;
        return true;
    }
    if (!strcmp("vkCmdBeginTransformFeedbackEXT", name)) {
        *addr = (void *)CmdBeginTransformFeedbackEXT;
        return true;
    }
    if (!strcmp("vkCmdEndTransformFeedbackEXT", name)) {
        *addr = (void *)CmdEndTransformFeedbackEXT;
        return true;
    }
    if (!strcmp("vkCmdBeginQueryIndexedEXT", name)) {
        *addr = (void *)CmdBeginQueryIndexedEXT;
        return true;
    }
    if (!strcmp("vkCmdEndQueryIndexedEXT", name)) {
        *addr = (void *)CmdEndQueryIndexedEXT;
        return true;
    }
    if (!strcmp("vkCmdDrawIndirectByteCountEXT", name)) {
        *addr = (void *)CmdDrawIndirectByteCountEXT;
        return true;
    }

    // ---- VK_NVX_binary_import extension commands
    if (!strcmp("vkCreateCuModuleNVX", name)) {
        *addr = (void *)CreateCuModuleNVX;
        return true;
    }
    if (!strcmp("vkCreateCuFunctionNVX", name)) {
        *addr = (void *)CreateCuFunctionNVX;
        return true;
    }
    if (!strcmp("vkDestroyCuModuleNVX", name)) {
        *addr = (void *)DestroyCuModuleNVX;
        return true;
    }
    if (!strcmp("vkDestroyCuFunctionNVX", name)) {
        *addr = (void *)DestroyCuFunctionNVX;
        return true;
    }
    if (!strcmp("vkCmdCuLaunchKernelNVX", name)) {
        *addr = (void *)CmdCuLaunchKernelNVX;
        return true;
    }

    // ---- VK_NVX_image_view_handle extension commands
    if (!strcmp("vkGetImageViewHandleNVX", name)) {
        *addr = (void *)GetImageViewHandleNVX;
        return true;
    }
    if (!strcmp("vkGetImageViewAddressNVX", name)) {
        *addr = (void *)GetImageViewAddressNVX;
        return true;
    }

    // ---- VK_AMD_draw_indirect_count extension commands
    if (!strcmp("vkCmdDrawIndirectCountAMD", name)) {
        *addr = (void *)vkCmdDrawIndirectCount;
        return true;
    }
    if (!strcmp("vkCmdDrawIndexedIndirectCountAMD", name)) {
        *addr = (void *)vkCmdDrawIndexedIndirectCount;
        return true;
    }

    // ---- VK_AMD_shader_info extension commands
    if (!strcmp("vkGetShaderInfoAMD", name)) {
        *addr = (void *)GetShaderInfoAMD;
        return true;
    }

    // ---- VK_NV_external_memory_capabilities extension commands
    if (!strcmp("vkGetPhysicalDeviceExternalImageFormatPropertiesNV", name)) {
        *addr = (ptr_instance->inst_ext_enables.nv_external_memory_capabilities == 1)
                     ? (void *)GetPhysicalDeviceExternalImageFormatPropertiesNV
                     : NULL;
        return true;
    }

    // ---- VK_NV_external_memory_win32 extension commands
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (!strcmp("vkGetMemoryWin32HandleNV", name)) {
        *addr = (void *)GetMemoryWin32HandleNV;
        return true;
    }
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ---- VK_EXT_conditional_rendering extension commands
    if (!strcmp("vkCmdBeginConditionalRenderingEXT", name)) {
        *addr = (void *)CmdBeginConditionalRenderingEXT;
        return true;
    }
    if (!strcmp("vkCmdEndConditionalRenderingEXT", name)) {
        *addr = (void *)CmdEndConditionalRenderingEXT;
        return true;
    }

    // ---- VK_NV_clip_space_w_scaling extension commands
    if (!strcmp("vkCmdSetViewportWScalingNV", name)) {
        *addr = (void *)CmdSetViewportWScalingNV;
        return true;
    }

    // ---- VK_EXT_direct_mode_display extension commands
    if (!strcmp("vkReleaseDisplayEXT", name)) {
        *addr = (ptr_instance->inst_ext_enables.ext_direct_mode_display == 1)
                     ? (void *)ReleaseDisplayEXT
                     : NULL;
        return true;
    }

    // ---- VK_EXT_acquire_xlib_display extension commands
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
    if (!strcmp("vkAcquireXlibDisplayEXT", name)) {
        *addr = (ptr_instance->inst_ext_enables.ext_acquire_xlib_display == 1)
                     ? (void *)AcquireXlibDisplayEXT
                     : NULL;
        return true;
    }
    if (!strcmp("vkGetRandROutputDisplayEXT", name)) {
        *addr = (ptr_instance->inst_ext_enables.ext_acquire_xlib_display == 1)
                     ? (void *)GetRandROutputDisplayEXT
                     : NULL;
        return true;
    }
#endif // VK_USE_PLATFORM_XLIB_XRANDR_EXT

    // ---- VK_EXT_display_surface_counter extension commands
    if (!strcmp("vkGetPhysicalDeviceSurfaceCapabilities2EXT", name)) {
        *addr = (ptr_instance->inst_ext_enables.ext_display_surface_counter == 1)
                     ? (void *)GetPhysicalDeviceSurfaceCapabilities2EXT
                     : NULL;
        return true;
    }

    // ---- VK_EXT_display_control extension commands
    if (!strcmp("vkDisplayPowerControlEXT", name)) {
        *addr = (void *)DisplayPowerControlEXT;
        return true;
    }
    if (!strcmp("vkRegisterDeviceEventEXT", name)) {
        *addr = (void *)RegisterDeviceEventEXT;
        return true;
    }
    if (!strcmp("vkRegisterDisplayEventEXT", name)) {
        *addr = (void *)RegisterDisplayEventEXT;
        return true;
    }
    if (!strcmp("vkGetSwapchainCounterEXT", name)) {
        *addr = (void *)GetSwapchainCounterEXT;
        return true;
    }

    // ---- VK_GOOGLE_display_timing extension commands
    if (!strcmp("vkGetRefreshCycleDurationGOOGLE", name)) {
        *addr = (void *)GetRefreshCycleDurationGOOGLE;
        return true;
    }
    if (!strcmp("vkGetPastPresentationTimingGOOGLE", name)) {
        *addr = (void *)GetPastPresentationTimingGOOGLE;
        return true;
    }

    // ---- VK_EXT_discard_rectangles extension commands
    if (!strcmp("vkCmdSetDiscardRectangleEXT", name)) {
        *addr = (void *)CmdSetDiscardRectangleEXT;
        return true;
    }

    // ---- VK_EXT_hdr_metadata extension commands
    if (!strcmp("vkSetHdrMetadataEXT", name)) {
        *addr = (void *)SetHdrMetadataEXT;
        return true;
    }

    // ---- VK_EXT_debug_utils extension commands
    if (!strcmp("vkSetDebugUtilsObjectNameEXT", name)) {
        *addr = (ptr_instance->inst_ext_enables.ext_debug_utils == 1)
                     ? (void *)SetDebugUtilsObjectNameEXT
                     : NULL;
        return true;
    }
    if (!strcmp("vkSetDebugUtilsObjectTagEXT", name)) {
        *addr = (ptr_instance->inst_ext_enables.ext_debug_utils == 1)
                     ? (void *)SetDebugUtilsObjectTagEXT
                     : NULL;
        return true;
    }
    if (!strcmp("vkQueueBeginDebugUtilsLabelEXT", name)) {
        *addr = (ptr_instance->inst_ext_enables.ext_debug_utils == 1)
                     ? (void *)QueueBeginDebugUtilsLabelEXT
                     : NULL;
        return true;
    }
    if (!strcmp("vkQueueEndDebugUtilsLabelEXT", name)) {
        *addr = (ptr_instance->inst_ext_enables.ext_debug_utils == 1)
                     ? (void *)QueueEndDebugUtilsLabelEXT
                     : NULL;
        return true;
    }
    if (!strcmp("vkQueueInsertDebugUtilsLabelEXT", name)) {
        *addr = (ptr_instance->inst_ext_enables.ext_debug_utils == 1)
                     ? (void *)QueueInsertDebugUtilsLabelEXT
                     : NULL;
        return true;
    }
    if (!strcmp("vkCmdBeginDebugUtilsLabelEXT", name)) {
        *addr = (ptr_instance->inst_ext_enables.ext_debug_utils == 1)
                     ? (void *)CmdBeginDebugUtilsLabelEXT
                     : NULL;
        return true;
    }
    if (!strcmp("vkCmdEndDebugUtilsLabelEXT", name)) {
        *addr = (ptr_instance->inst_ext_enables.ext_debug_utils == 1)
                     ? (void *)CmdEndDebugUtilsLabelEXT
                     : NULL;
        return true;
    }
    if (!strcmp("vkCmdInsertDebugUtilsLabelEXT", name)) {
        *addr = (ptr_instance->inst_ext_enables.ext_debug_utils == 1)
                     ? (void *)CmdInsertDebugUtilsLabelEXT
                     : NULL;
        return true;
    }

    // ---- VK_EXT_sample_locations extension commands
    if (!strcmp("vkCmdSetSampleLocationsEXT", name)) {
        *addr = (void *)CmdSetSampleLocationsEXT;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceMultisamplePropertiesEXT", name)) {
        *addr = (void *)GetPhysicalDeviceMultisamplePropertiesEXT;
        return true;
    }

    // ---- VK_EXT_image_drm_format_modifier extension commands
    if (!strcmp("vkGetImageDrmFormatModifierPropertiesEXT", name)) {
        *addr = (void *)GetImageDrmFormatModifierPropertiesEXT;
        return true;
    }

    // ---- VK_EXT_validation_cache extension commands
    if (!strcmp("vkCreateValidationCacheEXT", name)) {
        *addr = (void *)CreateValidationCacheEXT;
        return true;
    }
    if (!strcmp("vkDestroyValidationCacheEXT", name)) {
        *addr = (void *)DestroyValidationCacheEXT;
        return true;
    }
    if (!strcmp("vkMergeValidationCachesEXT", name)) {
        *addr = (void *)MergeValidationCachesEXT;
        return true;
    }
    if (!strcmp("vkGetValidationCacheDataEXT", name)) {
        *addr = (void *)GetValidationCacheDataEXT;
        return true;
    }

    // ---- VK_NV_shading_rate_image extension commands
    if (!strcmp("vkCmdBindShadingRateImageNV", name)) {
        *addr = (void *)CmdBindShadingRateImageNV;
        return true;
    }
    if (!strcmp("vkCmdSetViewportShadingRatePaletteNV", name)) {
        *addr = (void *)CmdSetViewportShadingRatePaletteNV;
        return true;
    }
    if (!strcmp("vkCmdSetCoarseSampleOrderNV", name)) {
        *addr = (void *)CmdSetCoarseSampleOrderNV;
        return true;
    }

    // ---- VK_NV_ray_tracing extension commands
    if (!strcmp("vkCreateAccelerationStructureNV", name)) {
        *addr = (void *)CreateAccelerationStructureNV;
        return true;
    }
    if (!strcmp("vkDestroyAccelerationStructureNV", name)) {
        *addr = (void *)DestroyAccelerationStructureNV;
        return true;
    }
    if (!strcmp("vkGetAccelerationStructureMemoryRequirementsNV", name)) {
        *addr = (void *)GetAccelerationStructureMemoryRequirementsNV;
        return true;
    }
    if (!strcmp("vkBindAccelerationStructureMemoryNV", name)) {
        *addr = (void *)BindAccelerationStructureMemoryNV;
        return true;
    }
    if (!strcmp("vkCmdBuildAccelerationStructureNV", name)) {
        *addr = (void *)CmdBuildAccelerationStructureNV;
        return true;
    }
    if (!strcmp("vkCmdCopyAccelerationStructureNV", name)) {
        *addr = (void *)CmdCopyAccelerationStructureNV;
        return true;
    }
    if (!strcmp("vkCmdTraceRaysNV", name)) {
        *addr = (void *)CmdTraceRaysNV;
        return true;
    }
    if (!strcmp("vkCreateRayTracingPipelinesNV", name)) {
        *addr = (void *)CreateRayTracingPipelinesNV;
        return true;
    }
    if (!strcmp("vkGetRayTracingShaderGroupHandlesNV", name)) {
        *addr = (void *)GetRayTracingShaderGroupHandlesKHR;
        return true;
    }
    if (!strcmp("vkGetAccelerationStructureHandleNV", name)) {
        *addr = (void *)GetAccelerationStructureHandleNV;
        return true;
    }
    if (!strcmp("vkCmdWriteAccelerationStructuresPropertiesNV", name)) {
        *addr = (void *)CmdWriteAccelerationStructuresPropertiesNV;
        return true;
    }
    if (!strcmp("vkCompileDeferredNV", name)) {
        *addr = (void *)CompileDeferredNV;
        return true;
    }

    // ---- VK_EXT_external_memory_host extension commands
    if (!strcmp("vkGetMemoryHostPointerPropertiesEXT", name)) {
        *addr = (void *)GetMemoryHostPointerPropertiesEXT;
        return true;
    }

    // ---- VK_AMD_buffer_marker extension commands
    if (!strcmp("vkCmdWriteBufferMarkerAMD", name)) {
        *addr = (void *)CmdWriteBufferMarkerAMD;
        return true;
    }

    // ---- VK_EXT_calibrated_timestamps extension commands
    if (!strcmp("vkGetPhysicalDeviceCalibrateableTimeDomainsEXT", name)) {
        *addr = (void *)GetPhysicalDeviceCalibrateableTimeDomainsEXT;
        return true;
    }
    if (!strcmp("vkGetCalibratedTimestampsEXT", name)) {
        *addr = (void *)GetCalibratedTimestampsEXT;
        return true;
    }

    // ---- VK_NV_mesh_shader extension commands
    if (!strcmp("vkCmdDrawMeshTasksNV", name)) {
        *addr = (void *)CmdDrawMeshTasksNV;
        return true;
    }
    if (!strcmp("vkCmdDrawMeshTasksIndirectNV", name)) {
        *addr = (void *)CmdDrawMeshTasksIndirectNV;
        return true;
    }
    if (!strcmp("vkCmdDrawMeshTasksIndirectCountNV", name)) {
        *addr = (void *)CmdDrawMeshTasksIndirectCountNV;
        return true;
    }

    // ---- VK_NV_scissor_exclusive extension commands
    if (!strcmp("vkCmdSetExclusiveScissorNV", name)) {
        *addr = (void *)CmdSetExclusiveScissorNV;
        return true;
    }

    // ---- VK_NV_device_diagnostic_checkpoints extension commands
    if (!strcmp("vkCmdSetCheckpointNV", name)) {
        *addr = (void *)CmdSetCheckpointNV;
        return true;
    }
    if (!strcmp("vkGetQueueCheckpointDataNV", name)) {
        *addr = (void *)GetQueueCheckpointDataNV;
        return true;
    }

    // ---- VK_INTEL_performance_query extension commands
    if (!strcmp("vkInitializePerformanceApiINTEL", name)) {
        *addr = (void *)InitializePerformanceApiINTEL;
        return true;
    }
    if (!strcmp("vkUninitializePerformanceApiINTEL", name)) {
        *addr = (void *)UninitializePerformanceApiINTEL;
        return true;
    }
    if (!strcmp("vkCmdSetPerformanceMarkerINTEL", name)) {
        *addr = (void *)CmdSetPerformanceMarkerINTEL;
        return true;
    }
    if (!strcmp("vkCmdSetPerformanceStreamMarkerINTEL", name)) {
        *addr = (void *)CmdSetPerformanceStreamMarkerINTEL;
        return true;
    }
    if (!strcmp("vkCmdSetPerformanceOverrideINTEL", name)) {
        *addr = (void *)CmdSetPerformanceOverrideINTEL;
        return true;
    }
    if (!strcmp("vkAcquirePerformanceConfigurationINTEL", name)) {
        *addr = (void *)AcquirePerformanceConfigurationINTEL;
        return true;
    }
    if (!strcmp("vkReleasePerformanceConfigurationINTEL", name)) {
        *addr = (void *)ReleasePerformanceConfigurationINTEL;
        return true;
    }
    if (!strcmp("vkQueueSetPerformanceConfigurationINTEL", name)) {
        *addr = (void *)QueueSetPerformanceConfigurationINTEL;
        return true;
    }
    if (!strcmp("vkGetPerformanceParameterINTEL", name)) {
        *addr = (void *)GetPerformanceParameterINTEL;
        return true;
    }

    // ---- VK_AMD_display_native_hdr extension commands
    if (!strcmp("vkSetLocalDimmingAMD", name)) {
        *addr = (void *)SetLocalDimmingAMD;
        return true;
    }

    // ---- VK_EXT_buffer_device_address extension commands
    if (!strcmp("vkGetBufferDeviceAddressEXT", name)) {
        *addr = (void *)vkGetBufferDeviceAddress;
        return true;
    }

    // ---- VK_EXT_tooling_info extension commands
    if (!strcmp("vkGetPhysicalDeviceToolPropertiesEXT", name)) {
        *addr = (void *)vkGetPhysicalDeviceToolProperties;
        return true;
    }

    // ---- VK_NV_cooperative_matrix extension commands
    if (!strcmp("vkGetPhysicalDeviceCooperativeMatrixPropertiesNV", name)) {
        *addr = (void *)GetPhysicalDeviceCooperativeMatrixPropertiesNV;
        return true;
    }

    // ---- VK_NV_coverage_reduction_mode extension commands
    if (!strcmp("vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV", name)) {
        *addr = (void *)GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV;
        return true;
    }

    // ---- VK_EXT_full_screen_exclusive extension commands
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (!strcmp("vkGetPhysicalDeviceSurfacePresentModes2EXT", name)) {
        *addr = (void *)GetPhysicalDeviceSurfacePresentModes2EXT;
        return true;
    }
    if (!strcmp("vkAcquireFullScreenExclusiveModeEXT", name)) {
        *addr = (void *)AcquireFullScreenExclusiveModeEXT;
        return true;
    }
    if (!strcmp("vkReleaseFullScreenExclusiveModeEXT", name)) {
        *addr = (void *)ReleaseFullScreenExclusiveModeEXT;
        return true;
    }
    if (!strcmp("vkGetDeviceGroupSurfacePresentModes2EXT", name)) {
        *addr = (void *)GetDeviceGroupSurfacePresentModes2EXT;
        return true;
    }
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ---- VK_EXT_line_rasterization extension commands
    if (!strcmp("vkCmdSetLineStippleEXT", name)) {
        *addr = (void *)CmdSetLineStippleEXT;
        return true;
    }

    // ---- VK_EXT_host_query_reset extension commands
    if (!strcmp("vkResetQueryPoolEXT", name)) {
        *addr = (void *)vkResetQueryPool;
        return true;
    }

    // ---- VK_EXT_extended_dynamic_state extension commands
    if (!strcmp("vkCmdSetCullModeEXT", name)) {
        *addr = (void *)vkCmdSetCullMode;
        return true;
    }
    if (!strcmp("vkCmdSetFrontFaceEXT", name)) {
        *addr = (void *)vkCmdSetFrontFace;
        return true;
    }
    if (!strcmp("vkCmdSetPrimitiveTopologyEXT", name)) {
        *addr = (void *)vkCmdSetPrimitiveTopology;
        return true;
    }
    if (!strcmp("vkCmdSetViewportWithCountEXT", name)) {
        *addr = (void *)vkCmdSetViewportWithCount;
        return true;
    }
    if (!strcmp("vkCmdSetScissorWithCountEXT", name)) {
        *addr = (void *)vkCmdSetScissorWithCount;
        return true;
    }
    if (!strcmp("vkCmdBindVertexBuffers2EXT", name)) {
        *addr = (void *)vkCmdBindVertexBuffers2;
        return true;
    }
    if (!strcmp("vkCmdSetDepthTestEnableEXT", name)) {
        *addr = (void *)vkCmdSetDepthTestEnable;
        return true;
    }
    if (!strcmp("vkCmdSetDepthWriteEnableEXT", name)) {
        *addr = (void *)vkCmdSetDepthWriteEnable;
        return true;
    }
    if (!strcmp("vkCmdSetDepthCompareOpEXT", name)) {
        *addr = (void *)vkCmdSetDepthCompareOp;
        return true;
    }
    if (!strcmp("vkCmdSetDepthBoundsTestEnableEXT", name)) {
        *addr = (void *)vkCmdSetDepthBoundsTestEnable;
        return true;
    }
    if (!strcmp("vkCmdSetStencilTestEnableEXT", name)) {
        *addr = (void *)vkCmdSetStencilTestEnable;
        return true;
    }
    if (!strcmp("vkCmdSetStencilOpEXT", name)) {
        *addr = (void *)vkCmdSetStencilOp;
        return true;
    }

    // ---- VK_NV_device_generated_commands extension commands
    if (!strcmp("vkGetGeneratedCommandsMemoryRequirementsNV", name)) {
        *addr = (void *)GetGeneratedCommandsMemoryRequirementsNV;
        return true;
    }
    if (!strcmp("vkCmdPreprocessGeneratedCommandsNV", name)) {
        *addr = (void *)CmdPreprocessGeneratedCommandsNV;
        return true;
    }
    if (!strcmp("vkCmdExecuteGeneratedCommandsNV", name)) {
        *addr = (void *)CmdExecuteGeneratedCommandsNV;
        return true;
    }
    if (!strcmp("vkCmdBindPipelineShaderGroupNV", name)) {
        *addr = (void *)CmdBindPipelineShaderGroupNV;
        return true;
    }
    if (!strcmp("vkCreateIndirectCommandsLayoutNV", name)) {
        *addr = (void *)CreateIndirectCommandsLayoutNV;
        return true;
    }
    if (!strcmp("vkDestroyIndirectCommandsLayoutNV", name)) {
        *addr = (void *)DestroyIndirectCommandsLayoutNV;
        return true;
    }

    // ---- VK_EXT_acquire_drm_display extension commands
    if (!strcmp("vkAcquireDrmDisplayEXT", name)) {
        *addr = (ptr_instance->inst_ext_enables.ext_acquire_drm_display == 1)
                     ? (void *)AcquireDrmDisplayEXT
                     : NULL;
        return true;
    }
    if (!strcmp("vkGetDrmDisplayEXT", name)) {
        *addr = (ptr_instance->inst_ext_enables.ext_acquire_drm_display == 1)
                     ? (void *)GetDrmDisplayEXT
                     : NULL;
        return true;
    }

    // ---- VK_EXT_private_data extension commands
    if (!strcmp("vkCreatePrivateDataSlotEXT", name)) {
        *addr = (void *)vkCreatePrivateDataSlot;
        return true;
    }
    if (!strcmp("vkDestroyPrivateDataSlotEXT", name)) {
        *addr = (void *)vkDestroyPrivateDataSlot;
        return true;
    }
    if (!strcmp("vkSetPrivateDataEXT", name)) {
        *addr = (void *)vkSetPrivateData;
        return true;
    }
    if (!strcmp("vkGetPrivateDataEXT", name)) {
        *addr = (void *)vkGetPrivateData;
        return true;
    }

    // ---- VK_NV_fragment_shading_rate_enums extension commands
    if (!strcmp("vkCmdSetFragmentShadingRateEnumNV", name)) {
        *addr = (void *)CmdSetFragmentShadingRateEnumNV;
        return true;
    }

    // ---- VK_NV_acquire_winrt_display extension commands
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (!strcmp("vkAcquireWinrtDisplayNV", name)) {
        *addr = (void *)AcquireWinrtDisplayNV;
        return true;
    }
    if (!strcmp("vkGetWinrtDisplayNV", name)) {
        *addr = (void *)GetWinrtDisplayNV;
        return true;
    }
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ---- VK_EXT_vertex_input_dynamic_state extension commands
    if (!strcmp("vkCmdSetVertexInputEXT", name)) {
        *addr = (void *)CmdSetVertexInputEXT;
        return true;
    }

    // ---- VK_FUCHSIA_external_memory extension commands
#ifdef VK_USE_PLATFORM_FUCHSIA
    if (!strcmp("vkGetMemoryZirconHandleFUCHSIA", name)) {
        *addr = (void *)GetMemoryZirconHandleFUCHSIA;
        return true;
    }
    if (!strcmp("vkGetMemoryZirconHandlePropertiesFUCHSIA", name)) {
        *addr = (void *)GetMemoryZirconHandlePropertiesFUCHSIA;
        return true;
    }
#endif // VK_USE_PLATFORM_FUCHSIA

    // ---- VK_FUCHSIA_external_semaphore extension commands
#ifdef VK_USE_PLATFORM_FUCHSIA
    if (!strcmp("vkImportSemaphoreZirconHandleFUCHSIA", name)) {
        *addr = (void *)ImportSemaphoreZirconHandleFUCHSIA;
        return true;
    }
    if (!strcmp("vkGetSemaphoreZirconHandleFUCHSIA", name)) {
        *addr = (void *)GetSemaphoreZirconHandleFUCHSIA;
        return true;
    }
#endif // VK_USE_PLATFORM_FUCHSIA

    // ---- VK_FUCHSIA_buffer_collection extension commands
#ifdef VK_USE_PLATFORM_FUCHSIA
    if (!strcmp("vkCreateBufferCollectionFUCHSIA", name)) {
        *addr = (void *)CreateBufferCollectionFUCHSIA;
        return true;
    }
    if (!strcmp("vkSetBufferCollectionImageConstraintsFUCHSIA", name)) {
        *addr = (void *)SetBufferCollectionImageConstraintsFUCHSIA;
        return true;
    }
    if (!strcmp("vkSetBufferCollectionBufferConstraintsFUCHSIA", name)) {
        *addr = (void *)SetBufferCollectionBufferConstraintsFUCHSIA;
        return true;
    }
    if (!strcmp("vkDestroyBufferCollectionFUCHSIA", name)) {
        *addr = (void *)DestroyBufferCollectionFUCHSIA;
        return true;
    }
    if (!strcmp("vkGetBufferCollectionPropertiesFUCHSIA", name)) {
        *addr = (void *)GetBufferCollectionPropertiesFUCHSIA;
        return true;
    }
#endif // VK_USE_PLATFORM_FUCHSIA

    // ---- VK_HUAWEI_subpass_shading extension commands
    if (!strcmp("vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI", name)) {
        *addr = (void *)GetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI;
        return true;
    }
    if (!strcmp("vkCmdSubpassShadingHUAWEI", name)) {
        *addr = (void *)CmdSubpassShadingHUAWEI;
        return true;
    }

    // ---- VK_HUAWEI_invocation_mask extension commands
    if (!strcmp("vkCmdBindInvocationMaskHUAWEI", name)) {
        *addr = (void *)CmdBindInvocationMaskHUAWEI;
        return true;
    }

    // ---- VK_NV_external_memory_rdma extension commands
    if (!strcmp("vkGetMemoryRemoteAddressNV", name)) {
        *addr = (void *)GetMemoryRemoteAddressNV;
        return true;
    }

    // ---- VK_EXT_extended_dynamic_state2 extension commands
    if (!strcmp("vkCmdSetPatchControlPointsEXT", name)) {
        *addr = (void *)CmdSetPatchControlPointsEXT;
        return true;
    }
    if (!strcmp("vkCmdSetRasterizerDiscardEnableEXT", name)) {
        *addr = (void *)vkCmdSetRasterizerDiscardEnable;
        return true;
    }
    if (!strcmp("vkCmdSetDepthBiasEnableEXT", name)) {
        *addr = (void *)vkCmdSetDepthBiasEnable;
        return true;
    }
    if (!strcmp("vkCmdSetLogicOpEXT", name)) {
        *addr = (void *)CmdSetLogicOpEXT;
        return true;
    }
    if (!strcmp("vkCmdSetPrimitiveRestartEnableEXT", name)) {
        *addr = (void *)vkCmdSetPrimitiveRestartEnable;
        return true;
    }

    // ---- VK_EXT_color_write_enable extension commands
    if (!strcmp("vkCmdSetColorWriteEnableEXT", name)) {
        *addr = (void *)CmdSetColorWriteEnableEXT;
        return true;
    }

    // ---- VK_EXT_multi_draw extension commands
    if (!strcmp("vkCmdDrawMultiEXT", name)) {
        *addr = (void *)CmdDrawMultiEXT;
        return true;
    }
    if (!strcmp("vkCmdDrawMultiIndexedEXT", name)) {
        *addr = (void *)CmdDrawMultiIndexedEXT;
        return true;
    }

    // ---- VK_EXT_pageable_device_local_memory extension commands
    if (!strcmp("vkSetDeviceMemoryPriorityEXT", name)) {
        *addr = (void *)SetDeviceMemoryPriorityEXT;
        return true;
    }

    // ---- VK_VALVE_descriptor_set_host_mapping extension commands
    if (!strcmp("vkGetDescriptorSetLayoutHostMappingInfoVALVE", name)) {
        *addr = (void *)GetDescriptorSetLayoutHostMappingInfoVALVE;
        return true;
    }
    if (!strcmp("vkGetDescriptorSetHostMappingVALVE", name)) {
        *addr = (void *)GetDescriptorSetHostMappingVALVE;
        return true;
    }

    // ---- VK_KHR_acceleration_structure extension commands
    if (!strcmp("vkCreateAccelerationStructureKHR", name)) {
        *addr = (void *)CreateAccelerationStructureKHR;
        return true;
    }
    if (!strcmp("vkDestroyAccelerationStructureKHR", name)) {
        *addr = (void *)DestroyAccelerationStructureKHR;
        return true;
    }
    if (!strcmp("vkCmdBuildAccelerationStructuresKHR", name)) {
        *addr = (void *)CmdBuildAccelerationStructuresKHR;
        return true;
    }
    if (!strcmp("vkCmdBuildAccelerationStructuresIndirectKHR", name)) {
        *addr = (void *)CmdBuildAccelerationStructuresIndirectKHR;
        return true;
    }
    if (!strcmp("vkBuildAccelerationStructuresKHR", name)) {
        *addr = (void *)BuildAccelerationStructuresKHR;
        return true;
    }
    if (!strcmp("vkCopyAccelerationStructureKHR", name)) {
        *addr = (void *)CopyAccelerationStructureKHR;
        return true;
    }
    if (!strcmp("vkCopyAccelerationStructureToMemoryKHR", name)) {
        *addr = (void *)CopyAccelerationStructureToMemoryKHR;
        return true;
    }
    if (!strcmp("vkCopyMemoryToAccelerationStructureKHR", name)) {
        *addr = (void *)CopyMemoryToAccelerationStructureKHR;
        return true;
    }
    if (!strcmp("vkWriteAccelerationStructuresPropertiesKHR", name)) {
        *addr = (void *)WriteAccelerationStructuresPropertiesKHR;
        return true;
    }
    if (!strcmp("vkCmdCopyAccelerationStructureKHR", name)) {
        *addr = (void *)CmdCopyAccelerationStructureKHR;
        return true;
    }
    if (!strcmp("vkCmdCopyAccelerationStructureToMemoryKHR", name)) {
        *addr = (void *)CmdCopyAccelerationStructureToMemoryKHR;
        return true;
    }
    if (!strcmp("vkCmdCopyMemoryToAccelerationStructureKHR", name)) {
        *addr = (void *)CmdCopyMemoryToAccelerationStructureKHR;
        return true;
    }
    if (!strcmp("vkGetAccelerationStructureDeviceAddressKHR", name)) {
        *addr = (void *)GetAccelerationStructureDeviceAddressKHR;
        return true;
    }
    if (!strcmp("vkCmdWriteAccelerationStructuresPropertiesKHR", name)) {
        *addr = (void *)CmdWriteAccelerationStructuresPropertiesKHR;
        return true;
    }
    if (!strcmp("vkGetDeviceAccelerationStructureCompatibilityKHR", name)) {
        *addr = (void *)GetDeviceAccelerationStructureCompatibilityKHR;
        return true;
    }
    if (!strcmp("vkGetAccelerationStructureBuildSizesKHR", name)) {
        *addr = (void *)GetAccelerationStructureBuildSizesKHR;
        return true;
    }

    // ---- VK_KHR_ray_tracing_pipeline extension commands
    if (!strcmp("vkCmdTraceRaysKHR", name)) {
        *addr = (void *)CmdTraceRaysKHR;
        return true;
    }
    if (!strcmp("vkCreateRayTracingPipelinesKHR", name)) {
        *addr = (void *)CreateRayTracingPipelinesKHR;
        return true;
    }
    if (!strcmp("vkGetRayTracingShaderGroupHandlesKHR", name)) {
        *addr = (void *)GetRayTracingShaderGroupHandlesKHR;
        return true;
    }
    if (!strcmp("vkGetRayTracingCaptureReplayShaderGroupHandlesKHR", name)) {
        *addr = (void *)GetRayTracingCaptureReplayShaderGroupHandlesKHR;
        return true;
    }
    if (!strcmp("vkCmdTraceRaysIndirectKHR", name)) {
        *addr = (void *)CmdTraceRaysIndirectKHR;
        return true;
    }
    if (!strcmp("vkGetRayTracingShaderGroupStackSizeKHR", name)) {
        *addr = (void *)GetRayTracingShaderGroupStackSizeKHR;
        return true;
    }
    if (!strcmp("vkCmdSetRayTracingPipelineStackSizeKHR", name)) {
        *addr = (void *)CmdSetRayTracingPipelineStackSizeKHR;
        return true;
    }
    return false;
}

// clang-format on

