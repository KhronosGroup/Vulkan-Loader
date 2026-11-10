// *** THIS FILE IS GENERATED - DO NOT EDIT ***
// See loader_core_generator.py for modifications

/*
 * Copyright (c) 2026 The Khronos Group Inc.
 * Copyright (c) 2026 Valve Corporation
 * Copyright (c) 2026 LunarG, Inc.
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
 * Author: Charles Giessen <charles@lunarg.com>
 */

#include <string.h>
#include <vulkan/vulkan.h>
#include "vk_loader_platform.h"
#include "loader.h"
#include "debug_utils.h"
#include "unknown_function_handling.h"
#include "wsi.h"

// ---- Core Vulkan 1.0 commands
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice,
                                                                     VkPhysicalDeviceFeatures* pFeatures) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    disp->GetPhysicalDeviceFeatures(unwrapped_phys_dev, pFeatures);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format,
                                                                             VkFormatProperties* pFormatProperties) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    disp->GetPhysicalDeviceFormatProperties(unwrapped_phys_dev, format, pFormatProperties);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceImageFormatProperties(
    VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage,
    VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceImageFormatProperties(unwrapped_phys_dev, format, type, tiling, usage, flags,
                                                        pImageFormatProperties);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice,
                                                                       VkPhysicalDeviceProperties* pProperties) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    disp->GetPhysicalDeviceProperties(unwrapped_phys_dev, pProperties);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice,
                                                                                  uint32_t* pQueueFamilyPropertyCount,
                                                                                  VkQueueFamilyProperties* pQueueFamilyProperties) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    disp->GetPhysicalDeviceQueueFamilyProperties(unwrapped_phys_dev, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice,
                                                                             VkPhysicalDeviceMemoryProperties* pMemoryProperties) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    disp->GetPhysicalDeviceMemoryProperties(unwrapped_phys_dev, pMemoryProperties);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits,
                                                           VkFence fence) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_queue_dispatch(queue, __FUNCTION__);
    return disp->QueueSubmit(queue, submitCount, pSubmits, fence);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkQueueWaitIdle(VkQueue queue) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_queue_dispatch(queue, __FUNCTION__);
    return disp->QueueWaitIdle(queue);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkDeviceWaitIdle(VkDevice device) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->DeviceWaitIdle(device);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkAllocateMemory(VkDevice device, const VkMemoryAllocateInfo* pAllocateInfo,
                                                              const VkAllocationCallbacks* pAllocator, VkDeviceMemory* pMemory) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->AllocateMemory(device, pAllocateInfo, pAllocator, pMemory);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkFreeMemory(VkDevice device, VkDeviceMemory memory,
                                                      const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->FreeMemory(device, memory, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset,
                                                         VkDeviceSize size, VkMemoryMapFlags flags, void** ppData) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->MapMemory(device, memory, offset, size, flags, ppData);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkUnmapMemory(VkDevice device, VkDeviceMemory memory) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->UnmapMemory(device, memory);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkFlushMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount,
                                                                       const VkMappedMemoryRange* pMemoryRanges) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->FlushMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkInvalidateMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount,
                                                                            const VkMappedMemoryRange* pMemoryRanges) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->InvalidateMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetDeviceMemoryCommitment(VkDevice device, VkDeviceMemory memory,
                                                                     VkDeviceSize* pCommittedMemoryInBytes) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->GetDeviceMemoryCommitment(device, memory, pCommittedMemoryInBytes);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory,
                                                                VkDeviceSize memoryOffset) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->BindBufferMemory(device, buffer, memory, memoryOffset);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkBindImageMemory(VkDevice device, VkImage image, VkDeviceMemory memory,
                                                               VkDeviceSize memoryOffset) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->BindImageMemory(device, image, memory, memoryOffset);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetBufferMemoryRequirements(VkDevice device, VkBuffer buffer,
                                                                       VkMemoryRequirements* pMemoryRequirements) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->GetBufferMemoryRequirements(device, buffer, pMemoryRequirements);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetImageMemoryRequirements(VkDevice device, VkImage image,
                                                                      VkMemoryRequirements* pMemoryRequirements) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->GetImageMemoryRequirements(device, image, pMemoryRequirements);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkGetImageSparseMemoryRequirements(VkDevice device, VkImage image, uint32_t* pSparseMemoryRequirementCount,
                                   VkSparseImageMemoryRequirements* pSparseMemoryRequirements) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->GetImageSparseMemoryRequirements(device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceSparseImageFormatProperties(
    VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage,
    VkImageTiling tiling, uint32_t* pPropertyCount, VkSparseImageFormatProperties* pProperties) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    disp->GetPhysicalDeviceSparseImageFormatProperties(unwrapped_phys_dev, format, type, samples, usage, tiling, pPropertyCount,
                                                       pProperties);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkQueueBindSparse(VkQueue queue, uint32_t bindInfoCount,
                                                               const VkBindSparseInfo* pBindInfo, VkFence fence) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_queue_dispatch(queue, __FUNCTION__);
    return disp->QueueBindSparse(queue, bindInfoCount, pBindInfo, fence);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateFence(VkDevice device, const VkFenceCreateInfo* pCreateInfo,
                                                           const VkAllocationCallbacks* pAllocator, VkFence* pFence) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateFence(device, pCreateInfo, pAllocator, pFence);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyFence(VkDevice device, VkFence fence, const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroyFence(device, fence, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkResetFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->ResetFences(device, fenceCount, pFences);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetFenceStatus(VkDevice device, VkFence fence) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->GetFenceStatus(device, fence);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkWaitForFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences,
                                                             VkBool32 waitAll, uint64_t timeout) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->WaitForFences(device, fenceCount, pFences, waitAll, timeout);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateSemaphore(VkDevice device, const VkSemaphoreCreateInfo* pCreateInfo,
                                                               const VkAllocationCallbacks* pAllocator, VkSemaphore* pSemaphore) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateSemaphore(device, pCreateInfo, pAllocator, pSemaphore);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroySemaphore(VkDevice device, VkSemaphore semaphore,
                                                            const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroySemaphore(device, semaphore, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateQueryPool(VkDevice device, const VkQueryPoolCreateInfo* pCreateInfo,
                                                               const VkAllocationCallbacks* pAllocator, VkQueryPool* pQueryPool) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateQueryPool(device, pCreateInfo, pAllocator, pQueryPool);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyQueryPool(VkDevice device, VkQueryPool queryPool,
                                                            const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroyQueryPool(device, queryPool, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetQueryPoolResults(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery,
                                                                   uint32_t queryCount, size_t dataSize, void* pData,
                                                                   VkDeviceSize stride, VkQueryResultFlags flags) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->GetQueryPoolResults(device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateBuffer(VkDevice device, const VkBufferCreateInfo* pCreateInfo,
                                                            const VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateBuffer(device, pCreateInfo, pAllocator, pBuffer);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyBuffer(VkDevice device, VkBuffer buffer,
                                                         const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroyBuffer(device, buffer, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateImage(VkDevice device, const VkImageCreateInfo* pCreateInfo,
                                                           const VkAllocationCallbacks* pAllocator, VkImage* pImage) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateImage(device, pCreateInfo, pAllocator, pImage);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyImage(VkDevice device, VkImage image, const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroyImage(device, image, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetImageSubresourceLayout(VkDevice device, VkImage image,
                                                                     const VkImageSubresource* pSubresource,
                                                                     VkSubresourceLayout* pLayout) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->GetImageSubresourceLayout(device, image, pSubresource, pLayout);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateImageView(VkDevice device, const VkImageViewCreateInfo* pCreateInfo,
                                                               const VkAllocationCallbacks* pAllocator, VkImageView* pView) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateImageView(device, pCreateInfo, pAllocator, pView);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyImageView(VkDevice device, VkImageView imageView,
                                                            const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroyImageView(device, imageView, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo,
                                                                 const VkAllocationCallbacks* pAllocator,
                                                                 VkCommandPool* pCommandPool) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateCommandPool(device, pCreateInfo, pAllocator, pCommandPool);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool,
                                                              const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroyCommandPool(device, commandPool, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkResetCommandPool(VkDevice device, VkCommandPool commandPool,
                                                                VkCommandPoolResetFlags flags) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->ResetCommandPool(device, commandPool, flags);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkFreeCommandBuffers(VkDevice device, VkCommandPool commandPool,
                                                              uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->FreeCommandBuffers(device, commandPool, commandBufferCount, pCommandBuffers);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkBeginCommandBuffer(VkCommandBuffer commandBuffer,
                                                                  const VkCommandBufferBeginInfo* pBeginInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    return disp->BeginCommandBuffer(commandBuffer, pBeginInfo);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEndCommandBuffer(VkCommandBuffer commandBuffer) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    return disp->EndCommandBuffer(commandBuffer);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkResetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    return disp->ResetCommandBuffer(commandBuffer, flags);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer,
                                                         uint32_t regionCount, const VkBufferCopy* pRegions) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyImage(VkCommandBuffer commandBuffer, VkImage srcImage,
                                                        VkImageLayout srcImageLayout, VkImage dstImage,
                                                        VkImageLayout dstImageLayout, uint32_t regionCount,
                                                        const VkImageCopy* pRegions) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage,
                                                                VkImageLayout dstImageLayout, uint32_t regionCount,
                                                                const VkBufferImageCopy* pRegions) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage,
                                                                VkImageLayout srcImageLayout, VkBuffer dstBuffer,
                                                                uint32_t regionCount, const VkBufferImageCopy* pRegions) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdUpdateBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer,
                                                           VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, pData);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdFillBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset,
                                                         VkDeviceSize size, uint32_t data) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdPipelineBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask,
                                                              VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags,
                                                              uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers,
                                                              uint32_t bufferMemoryBarrierCount,
                                                              const VkBufferMemoryBarrier* pBufferMemoryBarriers,
                                                              uint32_t imageMemoryBarrierCount,
                                                              const VkImageMemoryBarrier* pImageMemoryBarriers) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers,
                             bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBeginQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query,
                                                         VkQueryControlFlags flags) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdBeginQuery(commandBuffer, queryPool, query, flags);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdEndQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdEndQuery(commandBuffer, queryPool, query);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdResetQueryPool(VkCommandBuffer commandBuffer, VkQueryPool queryPool,
                                                             uint32_t firstQuery, uint32_t queryCount) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdWriteTimestamp(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage,
                                                             VkQueryPool queryPool, uint32_t query) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, query);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyQueryPoolResults(VkCommandBuffer commandBuffer, VkQueryPool queryPool,
                                                                   uint32_t firstQuery, uint32_t queryCount, VkBuffer dstBuffer,
                                                                   VkDeviceSize dstOffset, VkDeviceSize stride,
                                                                   VkQueryResultFlags flags) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdExecuteCommands(VkCommandBuffer commandBuffer, uint32_t commandBufferCount,
                                                              const VkCommandBuffer* pCommandBuffers) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdExecuteCommands(commandBuffer, commandBufferCount, pCommandBuffers);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateEvent(VkDevice device, const VkEventCreateInfo* pCreateInfo,
                                                           const VkAllocationCallbacks* pAllocator, VkEvent* pEvent) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateEvent(device, pCreateInfo, pAllocator, pEvent);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyEvent(VkDevice device, VkEvent event, const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroyEvent(device, event, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetEventStatus(VkDevice device, VkEvent event) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->GetEventStatus(device, event);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkSetEvent(VkDevice device, VkEvent event) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->SetEvent(device, event);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkResetEvent(VkDevice device, VkEvent event) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->ResetEvent(device, event);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateBufferView(VkDevice device, const VkBufferViewCreateInfo* pCreateInfo,
                                                                const VkAllocationCallbacks* pAllocator, VkBufferView* pView) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateBufferView(device, pCreateInfo, pAllocator, pView);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyBufferView(VkDevice device, VkBufferView bufferView,
                                                             const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroyBufferView(device, bufferView, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateShaderModule(VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo,
                                                                  const VkAllocationCallbacks* pAllocator,
                                                                  VkShaderModule* pShaderModule) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateShaderModule(device, pCreateInfo, pAllocator, pShaderModule);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyShaderModule(VkDevice device, VkShaderModule shaderModule,
                                                               const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroyShaderModule(device, shaderModule, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreatePipelineCache(VkDevice device, const VkPipelineCacheCreateInfo* pCreateInfo,
                                                                   const VkAllocationCallbacks* pAllocator,
                                                                   VkPipelineCache* pPipelineCache) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreatePipelineCache(device, pCreateInfo, pAllocator, pPipelineCache);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyPipelineCache(VkDevice device, VkPipelineCache pipelineCache,
                                                                const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroyPipelineCache(device, pipelineCache, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPipelineCacheData(VkDevice device, VkPipelineCache pipelineCache,
                                                                    size_t* pDataSize, void* pData) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->GetPipelineCacheData(device, pipelineCache, pDataSize, pData);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkMergePipelineCaches(VkDevice device, VkPipelineCache dstCache,
                                                                   uint32_t srcCacheCount, const VkPipelineCache* pSrcCaches) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->MergePipelineCaches(device, dstCache, srcCacheCount, pSrcCaches);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateComputePipelines(VkDevice device, VkPipelineCache pipelineCache,
                                                                      uint32_t createInfoCount,
                                                                      const VkComputePipelineCreateInfo* pCreateInfos,
                                                                      const VkAllocationCallbacks* pAllocator,
                                                                      VkPipeline* pPipelines) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateComputePipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyPipeline(VkDevice device, VkPipeline pipeline,
                                                           const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroyPipeline(device, pipeline, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreatePipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo* pCreateInfo,
                                                                    const VkAllocationCallbacks* pAllocator,
                                                                    VkPipelineLayout* pPipelineLayout) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreatePipelineLayout(device, pCreateInfo, pAllocator, pPipelineLayout);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout,
                                                                 const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroyPipelineLayout(device, pipelineLayout, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateSampler(VkDevice device, const VkSamplerCreateInfo* pCreateInfo,
                                                             const VkAllocationCallbacks* pAllocator, VkSampler* pSampler) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateSampler(device, pCreateInfo, pAllocator, pSampler);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroySampler(VkDevice device, VkSampler sampler,
                                                          const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroySampler(device, sampler, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorSetLayout(VkDevice device,
                                                                         const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
                                                                         const VkAllocationCallbacks* pAllocator,
                                                                         VkDescriptorSetLayout* pSetLayout) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateDescriptorSetLayout(device, pCreateInfo, pAllocator, pSetLayout);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout,
                                                                      const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroyDescriptorSetLayout(device, descriptorSetLayout, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo* pCreateInfo,
                                                                    const VkAllocationCallbacks* pAllocator,
                                                                    VkDescriptorPool* pDescriptorPool) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateDescriptorPool(device, pCreateInfo, pAllocator, pDescriptorPool);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool,
                                                                 const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroyDescriptorPool(device, descriptorPool, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkResetDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool,
                                                                   VkDescriptorPoolResetFlags flags) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->ResetDescriptorPool(device, descriptorPool, flags);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkAllocateDescriptorSets(VkDevice device,
                                                                      const VkDescriptorSetAllocateInfo* pAllocateInfo,
                                                                      VkDescriptorSet* pDescriptorSets) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->AllocateDescriptorSets(device, pAllocateInfo, pDescriptorSets);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkFreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool,
                                                                  uint32_t descriptorSetCount,
                                                                  const VkDescriptorSet* pDescriptorSets) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->FreeDescriptorSets(device, descriptorPool, descriptorSetCount, pDescriptorSets);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkUpdateDescriptorSets(VkDevice device, uint32_t descriptorWriteCount,
                                                                const VkWriteDescriptorSet* pDescriptorWrites,
                                                                uint32_t descriptorCopyCount,
                                                                const VkCopyDescriptorSet* pDescriptorCopies) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->UpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint,
                                                           VkPipeline pipeline) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBindDescriptorSets(VkCommandBuffer commandBuffer,
                                                                 VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout,
                                                                 uint32_t firstSet, uint32_t descriptorSetCount,
                                                                 const VkDescriptorSet* pDescriptorSets,
                                                                 uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets,
                                dynamicOffsetCount, pDynamicOffsets);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdClearColorImage(VkCommandBuffer commandBuffer, VkImage image,
                                                              VkImageLayout imageLayout, const VkClearColorValue* pColor,
                                                              uint32_t rangeCount, const VkImageSubresourceRange* pRanges) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdClearColorImage(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDispatch(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY,
                                                       uint32_t groupCountZ) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDispatchIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer,
                                                               VkDeviceSize offset) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdDispatchIndirect(commandBuffer, buffer, offset);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetEvent(VkCommandBuffer commandBuffer, VkEvent event,
                                                       VkPipelineStageFlags stageMask) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetEvent(commandBuffer, event, stageMask);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdResetEvent(VkCommandBuffer commandBuffer, VkEvent event,
                                                         VkPipelineStageFlags stageMask) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdResetEvent(commandBuffer, event, stageMask);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdWaitEvents(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent* pEvents,
                                                         VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                                                         uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers,
                                                         uint32_t bufferMemoryBarrierCount,
                                                         const VkBufferMemoryBarrier* pBufferMemoryBarriers,
                                                         uint32_t imageMemoryBarrierCount,
                                                         const VkImageMemoryBarrier* pImageMemoryBarriers) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdWaitEvents(commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers,
                        bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout layout,
                                                            VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size,
                                                            const void* pValues) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdPushConstants(commandBuffer, layout, stageFlags, offset, size, pValues);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateGraphicsPipelines(VkDevice device, VkPipelineCache pipelineCache,
                                                                       uint32_t createInfoCount,
                                                                       const VkGraphicsPipelineCreateInfo* pCreateInfos,
                                                                       const VkAllocationCallbacks* pAllocator,
                                                                       VkPipeline* pPipelines) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateGraphicsPipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateFramebuffer(VkDevice device, const VkFramebufferCreateInfo* pCreateInfo,
                                                                 const VkAllocationCallbacks* pAllocator,
                                                                 VkFramebuffer* pFramebuffer) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateFramebuffer(device, pCreateInfo, pAllocator, pFramebuffer);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer,
                                                              const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroyFramebuffer(device, framebuffer, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateRenderPass(VkDevice device, const VkRenderPassCreateInfo* pCreateInfo,
                                                                const VkAllocationCallbacks* pAllocator,
                                                                VkRenderPass* pRenderPass) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateRenderPass(device, pCreateInfo, pAllocator, pRenderPass);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyRenderPass(VkDevice device, VkRenderPass renderPass,
                                                             const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroyRenderPass(device, renderPass, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetRenderAreaGranularity(VkDevice device, VkRenderPass renderPass,
                                                                    VkExtent2D* pGranularity) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->GetRenderAreaGranularity(device, renderPass, pGranularity);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport,
                                                          uint32_t viewportCount, const VkViewport* pViewports) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor,
                                                         uint32_t scissorCount, const VkRect2D* pScissors) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetLineWidth(VkCommandBuffer commandBuffer, float lineWidth) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetLineWidth(commandBuffer, lineWidth);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBias(VkCommandBuffer commandBuffer, float depthBiasConstantFactor,
                                                           float depthBiasClamp, float depthBiasSlopeFactor) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetDepthBias(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetBlendConstants(VkCommandBuffer commandBuffer, const float blendConstants[4]) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetBlendConstants(commandBuffer, blendConstants);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBounds(VkCommandBuffer commandBuffer, float minDepthBounds,
                                                             float maxDepthBounds) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetDepthBounds(commandBuffer, minDepthBounds, maxDepthBounds);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilCompareMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask,
                                                                    uint32_t compareMask) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetStencilCompareMask(commandBuffer, faceMask, compareMask);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilWriteMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask,
                                                                  uint32_t writeMask) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetStencilWriteMask(commandBuffer, faceMask, writeMask);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilReference(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask,
                                                                  uint32_t reference) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetStencilReference(commandBuffer, faceMask, reference);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset,
                                                              VkIndexType indexType) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding,
                                                                uint32_t bindingCount, const VkBuffer* pBuffers,
                                                                const VkDeviceSize* pOffsets) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount,
                                                   uint32_t firstVertex, uint32_t firstInstance) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount,
                                                          uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset,
                                                          uint32_t firstInstance) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset,
                                                           uint32_t drawCount, uint32_t stride) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdDrawIndirect(commandBuffer, buffer, offset, drawCount, stride);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer,
                                                                  VkDeviceSize offset, uint32_t drawCount, uint32_t stride) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdDrawIndexedIndirect(commandBuffer, buffer, offset, drawCount, stride);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage,
                                                        VkImageLayout srcImageLayout, VkImage dstImage,
                                                        VkImageLayout dstImageLayout, uint32_t regionCount,
                                                        const VkImageBlit* pRegions, VkFilter filter) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdClearDepthStencilImage(VkCommandBuffer commandBuffer, VkImage image,
                                                                     VkImageLayout imageLayout,
                                                                     const VkClearDepthStencilValue* pDepthStencil,
                                                                     uint32_t rangeCount, const VkImageSubresourceRange* pRanges) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdClearDepthStencilImage(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdClearAttachments(VkCommandBuffer commandBuffer, uint32_t attachmentCount,
                                                               const VkClearAttachment* pAttachments, uint32_t rectCount,
                                                               const VkClearRect* pRects) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdClearAttachments(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdResolveImage(VkCommandBuffer commandBuffer, VkImage srcImage,
                                                           VkImageLayout srcImageLayout, VkImage dstImage,
                                                           VkImageLayout dstImageLayout, uint32_t regionCount,
                                                           const VkImageResolve* pRegions) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdResolveImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBeginRenderPass(VkCommandBuffer commandBuffer,
                                                              const VkRenderPassBeginInfo* pRenderPassBegin,
                                                              VkSubpassContents contents) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdNextSubpass(VkCommandBuffer commandBuffer, VkSubpassContents contents) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdNextSubpass(commandBuffer, contents);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdEndRenderPass(VkCommandBuffer commandBuffer) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdEndRenderPass(commandBuffer);
}

// ---- Core Vulkan 1.1 commands
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkBindBufferMemory2(VkDevice device, uint32_t bindInfoCount,
                                                                 const VkBindBufferMemoryInfo* pBindInfos) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->BindBufferMemory2(device, bindInfoCount, pBindInfos);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkBindImageMemory2(VkDevice device, uint32_t bindInfoCount,
                                                                const VkBindImageMemoryInfo* pBindInfos) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->BindImageMemory2(device, bindInfoCount, pBindInfos);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetDeviceGroupPeerMemoryFeatures(VkDevice device, uint32_t heapIndex,
                                                                            uint32_t localDeviceIndex, uint32_t remoteDeviceIndex,
                                                                            VkPeerMemoryFeatureFlags* pPeerMemoryFeatures) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->GetDeviceGroupPeerMemoryFeatures(device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetDeviceMask(VkCommandBuffer commandBuffer, uint32_t deviceMask) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetDeviceMask(commandBuffer, deviceMask);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetImageMemoryRequirements2(VkDevice device, const VkImageMemoryRequirementsInfo2* pInfo,
                                                                       VkMemoryRequirements2* pMemoryRequirements) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->GetImageMemoryRequirements2(device, pInfo, pMemoryRequirements);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetBufferMemoryRequirements2(VkDevice device,
                                                                        const VkBufferMemoryRequirementsInfo2* pInfo,
                                                                        VkMemoryRequirements2* pMemoryRequirements) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->GetBufferMemoryRequirements2(device, pInfo, pMemoryRequirements);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetImageSparseMemoryRequirements2(
    VkDevice device, const VkImageSparseMemoryRequirementsInfo2* pInfo, uint32_t* pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2* pSparseMemoryRequirements) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->GetImageSparseMemoryRequirements2(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFeatures2(VkPhysicalDevice physicalDevice,
                                                                      VkPhysicalDeviceFeatures2* pFeatures) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    const struct loader_instance* inst = ((struct loader_physical_device_tramp*)physicalDevice)->this_instance;
    if (inst != NULL && inst->enabled_extensions.khr_get_physical_device_properties2) {
        disp->GetPhysicalDeviceFeatures2KHR(unwrapped_phys_dev, pFeatures);
    } else {
        disp->GetPhysicalDeviceFeatures2(unwrapped_phys_dev, pFeatures);
    }
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties2(VkPhysicalDevice physicalDevice,
                                                                        VkPhysicalDeviceProperties2* pProperties) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    const struct loader_instance* inst = ((struct loader_physical_device_tramp*)physicalDevice)->this_instance;
    if (inst != NULL && inst->enabled_extensions.khr_get_physical_device_properties2) {
        disp->GetPhysicalDeviceProperties2KHR(unwrapped_phys_dev, pProperties);
    } else {
        disp->GetPhysicalDeviceProperties2(unwrapped_phys_dev, pProperties);
    }
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFormatProperties2(VkPhysicalDevice physicalDevice, VkFormat format,
                                                                              VkFormatProperties2* pFormatProperties) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    const struct loader_instance* inst = ((struct loader_physical_device_tramp*)physicalDevice)->this_instance;
    if (inst != NULL && inst->enabled_extensions.khr_get_physical_device_properties2) {
        disp->GetPhysicalDeviceFormatProperties2KHR(unwrapped_phys_dev, format, pFormatProperties);
    } else {
        disp->GetPhysicalDeviceFormatProperties2(unwrapped_phys_dev, format, pFormatProperties);
    }
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceImageFormatProperties2(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo,
                                          VkImageFormatProperties2* pImageFormatProperties) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    const struct loader_instance* inst = ((struct loader_physical_device_tramp*)physicalDevice)->this_instance;
    if (inst != NULL && inst->enabled_extensions.khr_get_physical_device_properties2) {
        return disp->GetPhysicalDeviceImageFormatProperties2KHR(unwrapped_phys_dev, pImageFormatInfo, pImageFormatProperties);
    }
    return disp->GetPhysicalDeviceImageFormatProperties2(unwrapped_phys_dev, pImageFormatInfo, pImageFormatProperties);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyProperties2(
    VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties2* pQueueFamilyProperties) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    const struct loader_instance* inst = ((struct loader_physical_device_tramp*)physicalDevice)->this_instance;
    if (inst != NULL && inst->enabled_extensions.khr_get_physical_device_properties2) {
        disp->GetPhysicalDeviceQueueFamilyProperties2KHR(unwrapped_phys_dev, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    } else {
        disp->GetPhysicalDeviceQueueFamilyProperties2(unwrapped_phys_dev, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    }
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceMemoryProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties2* pMemoryProperties) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    const struct loader_instance* inst = ((struct loader_physical_device_tramp*)physicalDevice)->this_instance;
    if (inst != NULL && inst->enabled_extensions.khr_get_physical_device_properties2) {
        disp->GetPhysicalDeviceMemoryProperties2KHR(unwrapped_phys_dev, pMemoryProperties);
    } else {
        disp->GetPhysicalDeviceMemoryProperties2(unwrapped_phys_dev, pMemoryProperties);
    }
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceSparseImageFormatProperties2(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo, uint32_t* pPropertyCount,
    VkSparseImageFormatProperties2* pProperties) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    const struct loader_instance* inst = ((struct loader_physical_device_tramp*)physicalDevice)->this_instance;
    if (inst != NULL && inst->enabled_extensions.khr_get_physical_device_properties2) {
        disp->GetPhysicalDeviceSparseImageFormatProperties2KHR(unwrapped_phys_dev, pFormatInfo, pPropertyCount, pProperties);
    } else {
        disp->GetPhysicalDeviceSparseImageFormatProperties2(unwrapped_phys_dev, pFormatInfo, pPropertyCount, pProperties);
    }
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkTrimCommandPool(VkDevice device, VkCommandPool commandPool,
                                                           VkCommandPoolTrimFlags flags) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->TrimCommandPool(device, commandPool, flags);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalBufferProperties(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo,
    VkExternalBufferProperties* pExternalBufferProperties) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    const struct loader_instance* inst = ((struct loader_physical_device_tramp*)physicalDevice)->this_instance;
    if (inst != NULL && inst->enabled_extensions.khr_external_memory_capabilities) {
        disp->GetPhysicalDeviceExternalBufferPropertiesKHR(unwrapped_phys_dev, pExternalBufferInfo, pExternalBufferProperties);
    } else {
        disp->GetPhysicalDeviceExternalBufferProperties(unwrapped_phys_dev, pExternalBufferInfo, pExternalBufferProperties);
    }
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalFenceProperties(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo,
    VkExternalFenceProperties* pExternalFenceProperties) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    const struct loader_instance* inst = ((struct loader_physical_device_tramp*)physicalDevice)->this_instance;
    if (inst != NULL && inst->enabled_extensions.khr_external_fence_capabilities) {
        disp->GetPhysicalDeviceExternalFencePropertiesKHR(unwrapped_phys_dev, pExternalFenceInfo, pExternalFenceProperties);
    } else {
        disp->GetPhysicalDeviceExternalFenceProperties(unwrapped_phys_dev, pExternalFenceInfo, pExternalFenceProperties);
    }
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceExternalSemaphoreProperties(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo,
    VkExternalSemaphoreProperties* pExternalSemaphoreProperties) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    const struct loader_instance* inst = ((struct loader_physical_device_tramp*)physicalDevice)->this_instance;
    if (inst != NULL && inst->enabled_extensions.khr_external_semaphore_capabilities) {
        disp->GetPhysicalDeviceExternalSemaphorePropertiesKHR(unwrapped_phys_dev, pExternalSemaphoreInfo,
                                                              pExternalSemaphoreProperties);
    } else {
        disp->GetPhysicalDeviceExternalSemaphoreProperties(unwrapped_phys_dev, pExternalSemaphoreInfo,
                                                           pExternalSemaphoreProperties);
    }
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDispatchBase(VkCommandBuffer commandBuffer, uint32_t baseGroupX, uint32_t baseGroupY,
                                                           uint32_t baseGroupZ, uint32_t groupCountX, uint32_t groupCountY,
                                                           uint32_t groupCountZ) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdDispatchBase(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDescriptorUpdateTemplate(VkDevice device, const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo,
                                 const VkAllocationCallbacks* pAllocator, VkDescriptorUpdateTemplate* pDescriptorUpdateTemplate) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateDescriptorUpdateTemplate(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorUpdateTemplate(VkDevice device,
                                                                           VkDescriptorUpdateTemplate descriptorUpdateTemplate,
                                                                           const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroyDescriptorUpdateTemplate(device, descriptorUpdateTemplate, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkUpdateDescriptorSetWithTemplate(VkDevice device, VkDescriptorSet descriptorSet,
                                                                           VkDescriptorUpdateTemplate descriptorUpdateTemplate,
                                                                           const void* pData) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->UpdateDescriptorSetWithTemplate(device, descriptorSet, descriptorUpdateTemplate, pData);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetDescriptorSetLayoutSupport(VkDevice device,
                                                                         const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
                                                                         VkDescriptorSetLayoutSupport* pSupport) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->GetDescriptorSetLayoutSupport(device, pCreateInfo, pSupport);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateSamplerYcbcrConversion(VkDevice device,
                                                                            const VkSamplerYcbcrConversionCreateInfo* pCreateInfo,
                                                                            const VkAllocationCallbacks* pAllocator,
                                                                            VkSamplerYcbcrConversion* pYcbcrConversion) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateSamplerYcbcrConversion(device, pCreateInfo, pAllocator, pYcbcrConversion);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroySamplerYcbcrConversion(VkDevice device, VkSamplerYcbcrConversion ycbcrConversion,
                                                                         const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroySamplerYcbcrConversion(device, ycbcrConversion, pAllocator);
}

// ---- Core Vulkan 1.2 commands
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkResetQueryPool(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery,
                                                          uint32_t queryCount) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->ResetQueryPool(device, queryPool, firstQuery, queryCount);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetSemaphoreCounterValue(VkDevice device, VkSemaphore semaphore, uint64_t* pValue) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->GetSemaphoreCounterValue(device, semaphore, pValue);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkWaitSemaphores(VkDevice device, const VkSemaphoreWaitInfo* pWaitInfo,
                                                              uint64_t timeout) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->WaitSemaphores(device, pWaitInfo, timeout);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkSignalSemaphore(VkDevice device, const VkSemaphoreSignalInfo* pSignalInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->SignalSemaphore(device, pSignalInfo);
}
LOADER_EXPORT VKAPI_ATTR VkDeviceAddress VKAPI_CALL vkGetBufferDeviceAddress(VkDevice device,
                                                                             const VkBufferDeviceAddressInfo* pInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->GetBufferDeviceAddress(device, pInfo);
}
LOADER_EXPORT VKAPI_ATTR uint64_t VKAPI_CALL vkGetBufferOpaqueCaptureAddress(VkDevice device,
                                                                             const VkBufferDeviceAddressInfo* pInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->GetBufferOpaqueCaptureAddress(device, pInfo);
}
LOADER_EXPORT VKAPI_ATTR uint64_t VKAPI_CALL
vkGetDeviceMemoryOpaqueCaptureAddress(VkDevice device, const VkDeviceMemoryOpaqueCaptureAddressInfo* pInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->GetDeviceMemoryOpaqueCaptureAddress(device, pInfo);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndirectCount(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset,
                                                                VkBuffer countBuffer, VkDeviceSize countBufferOffset,
                                                                uint32_t maxDrawCount, uint32_t stride) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdDrawIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexedIndirectCount(VkCommandBuffer commandBuffer, VkBuffer buffer,
                                                                       VkDeviceSize offset, VkBuffer countBuffer,
                                                                       VkDeviceSize countBufferOffset, uint32_t maxDrawCount,
                                                                       uint32_t stride) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdDrawIndexedIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateRenderPass2(VkDevice device, const VkRenderPassCreateInfo2* pCreateInfo,
                                                                 const VkAllocationCallbacks* pAllocator,
                                                                 VkRenderPass* pRenderPass) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateRenderPass2(device, pCreateInfo, pAllocator, pRenderPass);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBeginRenderPass2(VkCommandBuffer commandBuffer,
                                                               const VkRenderPassBeginInfo* pRenderPassBegin,
                                                               const VkSubpassBeginInfo* pSubpassBeginInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdBeginRenderPass2(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdNextSubpass2(VkCommandBuffer commandBuffer,
                                                           const VkSubpassBeginInfo* pSubpassBeginInfo,
                                                           const VkSubpassEndInfo* pSubpassEndInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdNextSubpass2(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdEndRenderPass2(VkCommandBuffer commandBuffer,
                                                             const VkSubpassEndInfo* pSubpassEndInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdEndRenderPass2(commandBuffer, pSubpassEndInfo);
}

// ---- Core Vulkan 1.3 commands
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceToolProperties(VkPhysicalDevice physicalDevice,
                                                                               uint32_t* pToolCount,
                                                                               VkPhysicalDeviceToolProperties* pToolProperties) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceToolProperties(unwrapped_phys_dev, pToolCount, pToolProperties);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreatePrivateDataSlot(VkDevice device,
                                                                     const VkPrivateDataSlotCreateInfo* pCreateInfo,
                                                                     const VkAllocationCallbacks* pAllocator,
                                                                     VkPrivateDataSlot* pPrivateDataSlot) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreatePrivateDataSlot(device, pCreateInfo, pAllocator, pPrivateDataSlot);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyPrivateDataSlot(VkDevice device, VkPrivateDataSlot privateDataSlot,
                                                                  const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroyPrivateDataSlot(device, privateDataSlot, pAllocator);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkSetPrivateData(VkDevice device, VkObjectType objectType, uint64_t objectHandle,
                                                              VkPrivateDataSlot privateDataSlot, uint64_t data) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->SetPrivateData(device, objectType, objectHandle, privateDataSlot, data);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPrivateData(VkDevice device, VkObjectType objectType, uint64_t objectHandle,
                                                          VkPrivateDataSlot privateDataSlot, uint64_t* pData) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->GetPrivateData(device, objectType, objectHandle, privateDataSlot, pData);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdPipelineBarrier2(VkCommandBuffer commandBuffer,
                                                               const VkDependencyInfo* pDependencyInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdPipelineBarrier2(commandBuffer, pDependencyInfo);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdWriteTimestamp2(VkCommandBuffer commandBuffer, VkPipelineStageFlags2 stage,
                                                              VkQueryPool queryPool, uint32_t query) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdWriteTimestamp2(commandBuffer, stage, queryPool, query);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkQueueSubmit2(VkQueue queue, uint32_t submitCount, const VkSubmitInfo2* pSubmits,
                                                            VkFence fence) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_queue_dispatch(queue, __FUNCTION__);
    return disp->QueueSubmit2(queue, submitCount, pSubmits, fence);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyBuffer2(VkCommandBuffer commandBuffer, const VkCopyBufferInfo2* pCopyBufferInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdCopyBuffer2(commandBuffer, pCopyBufferInfo);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyImage2(VkCommandBuffer commandBuffer, const VkCopyImageInfo2* pCopyImageInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdCopyImage2(commandBuffer, pCopyImageInfo);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyBufferToImage2(VkCommandBuffer commandBuffer,
                                                                 const VkCopyBufferToImageInfo2* pCopyBufferToImageInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdCopyBufferToImage2(commandBuffer, pCopyBufferToImageInfo);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyImageToBuffer2(VkCommandBuffer commandBuffer,
                                                                 const VkCopyImageToBufferInfo2* pCopyImageToBufferInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdCopyImageToBuffer2(commandBuffer, pCopyImageToBufferInfo);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetDeviceBufferMemoryRequirements(VkDevice device,
                                                                             const VkDeviceBufferMemoryRequirements* pInfo,
                                                                             VkMemoryRequirements2* pMemoryRequirements) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->GetDeviceBufferMemoryRequirements(device, pInfo, pMemoryRequirements);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetDeviceImageMemoryRequirements(VkDevice device,
                                                                            const VkDeviceImageMemoryRequirements* pInfo,
                                                                            VkMemoryRequirements2* pMemoryRequirements) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->GetDeviceImageMemoryRequirements(device, pInfo, pMemoryRequirements);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetDeviceImageSparseMemoryRequirements(
    VkDevice device, const VkDeviceImageMemoryRequirements* pInfo, uint32_t* pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2* pSparseMemoryRequirements) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->GetDeviceImageSparseMemoryRequirements(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetEvent2(VkCommandBuffer commandBuffer, VkEvent event,
                                                        const VkDependencyInfo* pDependencyInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetEvent2(commandBuffer, event, pDependencyInfo);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdResetEvent2(VkCommandBuffer commandBuffer, VkEvent event,
                                                          VkPipelineStageFlags2 stageMask) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdResetEvent2(commandBuffer, event, stageMask);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdWaitEvents2(VkCommandBuffer commandBuffer, uint32_t eventCount,
                                                          const VkEvent* pEvents, const VkDependencyInfo* pDependencyInfos) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdWaitEvents2(commandBuffer, eventCount, pEvents, pDependencyInfos);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBlitImage2(VkCommandBuffer commandBuffer, const VkBlitImageInfo2* pBlitImageInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdBlitImage2(commandBuffer, pBlitImageInfo);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdResolveImage2(VkCommandBuffer commandBuffer,
                                                            const VkResolveImageInfo2* pResolveImageInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdResolveImage2(commandBuffer, pResolveImageInfo);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBeginRendering(VkCommandBuffer commandBuffer, const VkRenderingInfo* pRenderingInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdBeginRendering(commandBuffer, pRenderingInfo);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdEndRendering(VkCommandBuffer commandBuffer) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdEndRendering(commandBuffer);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetCullMode(VkCommandBuffer commandBuffer, VkCullModeFlags cullMode) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetCullMode(commandBuffer, cullMode);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetFrontFace(VkCommandBuffer commandBuffer, VkFrontFace frontFace) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetFrontFace(commandBuffer, frontFace);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetPrimitiveTopology(VkCommandBuffer commandBuffer,
                                                                   VkPrimitiveTopology primitiveTopology) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetPrimitiveTopology(commandBuffer, primitiveTopology);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetViewportWithCount(VkCommandBuffer commandBuffer, uint32_t viewportCount,
                                                                   const VkViewport* pViewports) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetViewportWithCount(commandBuffer, viewportCount, pViewports);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetScissorWithCount(VkCommandBuffer commandBuffer, uint32_t scissorCount,
                                                                  const VkRect2D* pScissors) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetScissorWithCount(commandBuffer, scissorCount, pScissors);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBindVertexBuffers2(VkCommandBuffer commandBuffer, uint32_t firstBinding,
                                                                 uint32_t bindingCount, const VkBuffer* pBuffers,
                                                                 const VkDeviceSize* pOffsets, const VkDeviceSize* pSizes,
                                                                 const VkDeviceSize* pStrides) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdBindVertexBuffers2(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes, pStrides);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthTestEnable(VkCommandBuffer commandBuffer, VkBool32 depthTestEnable) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetDepthTestEnable(commandBuffer, depthTestEnable);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthWriteEnable(VkCommandBuffer commandBuffer, VkBool32 depthWriteEnable) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetDepthWriteEnable(commandBuffer, depthWriteEnable);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthCompareOp(VkCommandBuffer commandBuffer, VkCompareOp depthCompareOp) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetDepthCompareOp(commandBuffer, depthCompareOp);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBoundsTestEnable(VkCommandBuffer commandBuffer,
                                                                       VkBool32 depthBoundsTestEnable) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetDepthBoundsTestEnable(commandBuffer, depthBoundsTestEnable);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilTestEnable(VkCommandBuffer commandBuffer, VkBool32 stencilTestEnable) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetStencilTestEnable(commandBuffer, stencilTestEnable);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilOp(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask,
                                                           VkStencilOp failOp, VkStencilOp passOp, VkStencilOp depthFailOp,
                                                           VkCompareOp compareOp) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetStencilOp(commandBuffer, faceMask, failOp, passOp, depthFailOp, compareOp);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetRasterizerDiscardEnable(VkCommandBuffer commandBuffer,
                                                                         VkBool32 rasterizerDiscardEnable) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetRasterizerDiscardEnable(commandBuffer, rasterizerDiscardEnable);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBiasEnable(VkCommandBuffer commandBuffer, VkBool32 depthBiasEnable) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetDepthBiasEnable(commandBuffer, depthBiasEnable);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetPrimitiveRestartEnable(VkCommandBuffer commandBuffer,
                                                                        VkBool32 primitiveRestartEnable) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetPrimitiveRestartEnable(commandBuffer, primitiveRestartEnable);
}

// ---- Core Vulkan 1.4 commands
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkMapMemory2(VkDevice device, const VkMemoryMapInfo* pMemoryMapInfo, void** ppData) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->MapMemory2(device, pMemoryMapInfo, ppData);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkUnmapMemory2(VkDevice device, const VkMemoryUnmapInfo* pMemoryUnmapInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->UnmapMemory2(device, pMemoryUnmapInfo);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetDeviceImageSubresourceLayout(VkDevice device,
                                                                           const VkDeviceImageSubresourceInfo* pInfo,
                                                                           VkSubresourceLayout2* pLayout) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->GetDeviceImageSubresourceLayout(device, pInfo, pLayout);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetImageSubresourceLayout2(VkDevice device, VkImage image,
                                                                      const VkImageSubresource2* pSubresource,
                                                                      VkSubresourceLayout2* pLayout) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->GetImageSubresourceLayout2(device, image, pSubresource, pLayout);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCopyMemoryToImage(VkDevice device,
                                                                 const VkCopyMemoryToImageInfo* pCopyMemoryToImageInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CopyMemoryToImage(device, pCopyMemoryToImageInfo);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCopyImageToMemory(VkDevice device,
                                                                 const VkCopyImageToMemoryInfo* pCopyImageToMemoryInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CopyImageToMemory(device, pCopyImageToMemoryInfo);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCopyImageToImage(VkDevice device,
                                                                const VkCopyImageToImageInfo* pCopyImageToImageInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CopyImageToImage(device, pCopyImageToImageInfo);
}
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkTransitionImageLayout(VkDevice device, uint32_t transitionCount,
                                                                     const VkHostImageLayoutTransitionInfo* pTransitions) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->TransitionImageLayout(device, transitionCount, pTransitions);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSet(VkCommandBuffer commandBuffer,
                                                                VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout,
                                                                uint32_t set, uint32_t descriptorWriteCount,
                                                                const VkWriteDescriptorSet* pDescriptorWrites) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdPushDescriptorSet(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSetWithTemplate(VkCommandBuffer commandBuffer,
                                                                            VkDescriptorUpdateTemplate descriptorUpdateTemplate,
                                                                            VkPipelineLayout layout, uint32_t set,
                                                                            const void* pData) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdPushDescriptorSetWithTemplate(commandBuffer, descriptorUpdateTemplate, layout, set, pData);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBindDescriptorSets2(VkCommandBuffer commandBuffer,
                                                                  const VkBindDescriptorSetsInfo* pBindDescriptorSetsInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdBindDescriptorSets2(commandBuffer, pBindDescriptorSetsInfo);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdPushConstants2(VkCommandBuffer commandBuffer,
                                                             const VkPushConstantsInfo* pPushConstantsInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdPushConstants2(commandBuffer, pPushConstantsInfo);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSet2(VkCommandBuffer commandBuffer,
                                                                 const VkPushDescriptorSetInfo* pPushDescriptorSetInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdPushDescriptorSet2(commandBuffer, pPushDescriptorSetInfo);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSetWithTemplate2(
    VkCommandBuffer commandBuffer, const VkPushDescriptorSetWithTemplateInfo* pPushDescriptorSetWithTemplateInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdPushDescriptorSetWithTemplate2(commandBuffer, pPushDescriptorSetWithTemplateInfo);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetLineStipple(VkCommandBuffer commandBuffer, uint32_t lineStippleFactor,
                                                             uint16_t lineStipplePattern) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetLineStipple(commandBuffer, lineStippleFactor, lineStipplePattern);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBindIndexBuffer2(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset,
                                                               VkDeviceSize size, VkIndexType indexType) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdBindIndexBuffer2(commandBuffer, buffer, offset, size, indexType);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetRenderingAreaGranularity(VkDevice device,
                                                                       const VkRenderingAreaInfo* pRenderingAreaInfo,
                                                                       VkExtent2D* pGranularity) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->GetRenderingAreaGranularity(device, pRenderingAreaInfo, pGranularity);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdSetRenderingAttachmentLocations(VkCommandBuffer commandBuffer, const VkRenderingAttachmentLocationInfo* pLocationInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetRenderingAttachmentLocations(commandBuffer, pLocationInfo);
}
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetRenderingInputAttachmentIndices(
    VkCommandBuffer commandBuffer, const VkRenderingInputAttachmentIndexInfo* pInputAttachmentIndexInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);
    disp->CmdSetRenderingInputAttachmentIndices(commandBuffer, pInputAttachmentIndexInfo);
}
// Implementation for loader's vkGetInstanceProcAddr for non-global entrypoints
void* trampoline_get_proc_addr(struct loader_instance* inst, const char* funcName) {
    const uint32_t name_hash = loader_hash_string(funcName);
    switch (name_hash) {
        case 0xa64dcfc3u:
            if (!strcmp(funcName, "vkDestroyInstance")) return vkDestroyInstance;
            break;
        case 0x09612094u:
            if (!strcmp(funcName, "vkEnumeratePhysicalDevices")) return vkEnumeratePhysicalDevices;
            break;
        case 0x3407e5aau:
            if (!strcmp(funcName, "vkGetPhysicalDeviceFeatures")) return vkGetPhysicalDeviceFeatures;
            break;
        case 0xf571955fu:
            if (!strcmp(funcName, "vkGetPhysicalDeviceFormatProperties")) return vkGetPhysicalDeviceFormatProperties;
            break;
        case 0x8bfced20u:
            if (!strcmp(funcName, "vkGetPhysicalDeviceImageFormatProperties")) return vkGetPhysicalDeviceImageFormatProperties;
            break;
        case 0x89b3ebbcu:
            if (!strcmp(funcName, "vkGetPhysicalDeviceProperties")) return vkGetPhysicalDeviceProperties;
            break;
        case 0x3d926eedu:
            if (!strcmp(funcName, "vkGetPhysicalDeviceQueueFamilyProperties")) return vkGetPhysicalDeviceQueueFamilyProperties;
            break;
        case 0x278fa9bbu:
            if (!strcmp(funcName, "vkGetPhysicalDeviceMemoryProperties")) return vkGetPhysicalDeviceMemoryProperties;
            break;
        case 0x9d4599a6u:
            if (!strcmp(funcName, "vkGetInstanceProcAddr")) return vkGetInstanceProcAddr;
            break;
        case 0x228d66e7u:
            if (!strcmp(funcName, "vkGetDeviceProcAddr")) return vkGetDeviceProcAddr;
            break;
        case 0x87b9b4e4u:
            if (!strcmp(funcName, "vkCreateDevice")) return vkCreateDevice;
            break;
        case 0x76ce3712u:
            if (!strcmp(funcName, "vkDestroyDevice")) return vkDestroyDevice;
            break;
        case 0x53cd225eu:
            if (!strcmp(funcName, "vkEnumerateDeviceExtensionProperties")) return vkEnumerateDeviceExtensionProperties;
            break;
        case 0xa37d674au:
            if (!strcmp(funcName, "vkEnumerateDeviceLayerProperties")) return vkEnumerateDeviceLayerProperties;
            break;
        case 0x0ef70c2bu:
            if (!strcmp(funcName, "vkGetDeviceQueue")) return vkGetDeviceQueue;
            break;
        case 0xa702ecf9u:
            if (!strcmp(funcName, "vkQueueSubmit")) return vkQueueSubmit;
            break;
        case 0x33b1f316u:
            if (!strcmp(funcName, "vkQueueWaitIdle")) return vkQueueWaitIdle;
            break;
        case 0xd8f8605fu:
            if (!strcmp(funcName, "vkDeviceWaitIdle")) return vkDeviceWaitIdle;
            break;
        case 0x8fd182beu:
            if (!strcmp(funcName, "vkAllocateMemory")) return vkAllocateMemory;
            break;
        case 0x3486e387u:
            if (!strcmp(funcName, "vkFreeMemory")) return vkFreeMemory;
            break;
        case 0xeade61a3u:
            if (!strcmp(funcName, "vkMapMemory")) return vkMapMemory;
            break;
        case 0x45ae8caau:
            if (!strcmp(funcName, "vkUnmapMemory")) return vkUnmapMemory;
            break;
        case 0xd4cde7deu:
            if (!strcmp(funcName, "vkFlushMappedMemoryRanges")) return vkFlushMappedMemoryRanges;
            break;
        case 0x242f4435u:
            if (!strcmp(funcName, "vkInvalidateMappedMemoryRanges")) return vkInvalidateMappedMemoryRanges;
            break;
        case 0x7edaff3eu:
            if (!strcmp(funcName, "vkGetDeviceMemoryCommitment")) return vkGetDeviceMemoryCommitment;
            break;
        case 0x8fcb92d4u:
            if (!strcmp(funcName, "vkBindBufferMemory")) return vkBindBufferMemory;
            break;
        case 0xb59c1405u:
            if (!strcmp(funcName, "vkBindImageMemory")) return vkBindImageMemory;
            break;
        case 0x5f571cdbu:
            if (!strcmp(funcName, "vkGetBufferMemoryRequirements")) return vkGetBufferMemoryRequirements;
            break;
        case 0x5b923818u:
            if (!strcmp(funcName, "vkGetImageMemoryRequirements")) return vkGetImageMemoryRequirements;
            break;
        case 0x0f6505a4u:
            if (!strcmp(funcName, "vkGetImageSparseMemoryRequirements")) return vkGetImageSparseMemoryRequirements;
            break;
        case 0x763c38a0u:
            if (!strcmp(funcName, "vkGetPhysicalDeviceSparseImageFormatProperties"))
                return vkGetPhysicalDeviceSparseImageFormatProperties;
            break;
        case 0xe4d99d8eu:
            if (!strcmp(funcName, "vkQueueBindSparse")) return vkQueueBindSparse;
            break;
        case 0xb758c6a3u:
            if (!strcmp(funcName, "vkCreateFence")) return vkCreateFence;
            break;
        case 0x60073b51u:
            if (!strcmp(funcName, "vkDestroyFence")) return vkDestroyFence;
            break;
        case 0xdbb395d3u:
            if (!strcmp(funcName, "vkResetFences")) return vkResetFences;
            break;
        case 0xd870b975u:
            if (!strcmp(funcName, "vkGetFenceStatus")) return vkGetFenceStatus;
            break;
        case 0xe7c2c7dcu:
            if (!strcmp(funcName, "vkWaitForFences")) return vkWaitForFences;
            break;
        case 0xd9b8366cu:
            if (!strcmp(funcName, "vkCreateSemaphore")) return vkCreateSemaphore;
            break;
        case 0xaf0834d2u:
            if (!strcmp(funcName, "vkDestroySemaphore")) return vkDestroySemaphore;
            break;
        case 0x0a03b3a4u:
            if (!strcmp(funcName, "vkCreateQueryPool")) return vkCreateQueryPool;
            break;
        case 0xfc8c9742u:
            if (!strcmp(funcName, "vkDestroyQueryPool")) return vkDestroyQueryPool;
            break;
        case 0x300fc14au:
            if (!strcmp(funcName, "vkGetQueryPoolResults")) return vkGetQueryPoolResults;
            break;
        case 0x507f87ecu:
            if (!strcmp(funcName, "vkCreateBuffer")) return vkCreateBuffer;
            break;
        case 0x8b9013b2u:
            if (!strcmp(funcName, "vkDestroyBuffer")) return vkDestroyBuffer;
            break;
        case 0x29aa0da7u:
            if (!strcmp(funcName, "vkCreateImage")) return vkCreateImage;
            break;
        case 0x2d239345u:
            if (!strcmp(funcName, "vkDestroyImage")) return vkDestroyImage;
            break;
        case 0x3bb2e87du:
            if (!strcmp(funcName, "vkGetImageSubresourceLayout")) return vkGetImageSubresourceLayout;
            break;
        case 0xb838b382u:
            if (!strcmp(funcName, "vkCreateImageView")) return vkCreateImageView;
            break;
        case 0xb32c7798u:
            if (!strcmp(funcName, "vkDestroyImageView")) return vkDestroyImageView;
            break;
        case 0x1d0cde71u:
            if (!strcmp(funcName, "vkCreateCommandPool")) return vkCreateCommandPool;
            break;
        case 0x46d07137u:
            if (!strcmp(funcName, "vkDestroyCommandPool")) return vkDestroyCommandPool;
            break;
        case 0x32d3032cu:
            if (!strcmp(funcName, "vkResetCommandPool")) return vkResetCommandPool;
            break;
        case 0x9b476eedu:
            if (!strcmp(funcName, "vkAllocateCommandBuffers")) return vkAllocateCommandBuffers;
            break;
        case 0x60cb1fd0u:
            if (!strcmp(funcName, "vkFreeCommandBuffers")) return vkFreeCommandBuffers;
            break;
        case 0x38f0e672u:
            if (!strcmp(funcName, "vkBeginCommandBuffer")) return vkBeginCommandBuffer;
            break;
        case 0xcf8708eau:
            if (!strcmp(funcName, "vkEndCommandBuffer")) return vkEndCommandBuffer;
            break;
        case 0x6ee4b1d0u:
            if (!strcmp(funcName, "vkResetCommandBuffer")) return vkResetCommandBuffer;
            break;
        case 0x54db4621u:
            if (!strcmp(funcName, "vkCmdCopyBuffer")) return vkCmdCopyBuffer;
            break;
        case 0x7bf994f0u:
            if (!strcmp(funcName, "vkCmdCopyImage")) return vkCmdCopyImage;
            break;
        case 0xb0b1777du:
            if (!strcmp(funcName, "vkCmdCopyBufferToImage")) return vkCmdCopyBufferToImage;
            break;
        case 0x0220566fu:
            if (!strcmp(funcName, "vkCmdCopyImageToBuffer")) return vkCmdCopyImageToBuffer;
            break;
        case 0x4cafab95u:
            if (!strcmp(funcName, "vkCmdUpdateBuffer")) return vkCmdUpdateBuffer;
            break;
        case 0xe2498171u:
            if (!strcmp(funcName, "vkCmdFillBuffer")) return vkCmdFillBuffer;
            break;
        case 0x0223af53u:
            if (!strcmp(funcName, "vkCmdPipelineBarrier")) return vkCmdPipelineBarrier;
            break;
        case 0x976db859u:
            if (!strcmp(funcName, "vkCmdBeginQuery")) return vkCmdBeginQuery;
            break;
        case 0xb93efb21u:
            if (!strcmp(funcName, "vkCmdEndQuery")) return vkCmdEndQuery;
            break;
        case 0x21657da5u:
            if (!strcmp(funcName, "vkCmdResetQueryPool")) return vkCmdResetQueryPool;
            break;
        case 0xf4fa04efu:
            if (!strcmp(funcName, "vkCmdWriteTimestamp")) return vkCmdWriteTimestamp;
            break;
        case 0xb2cd2b3fu:
            if (!strcmp(funcName, "vkCmdCopyQueryPoolResults")) return vkCmdCopyQueryPoolResults;
            break;
        case 0xf5f7e073u:
            if (!strcmp(funcName, "vkCmdExecuteCommands")) return vkCmdExecuteCommands;
            break;
        case 0x42b3f55eu:
            if (!strcmp(funcName, "vkCreateEvent")) return vkCreateEvent;
            break;
        case 0x624815e0u:
            if (!strcmp(funcName, "vkDestroyEvent")) return vkDestroyEvent;
            break;
        case 0x66007620u:
            if (!strcmp(funcName, "vkGetEventStatus")) return vkGetEventStatus;
            break;
        case 0xa73c166au:
            if (!strcmp(funcName, "vkSetEvent")) return vkSetEvent;
            break;
        case 0x7de046a3u:
            if (!strcmp(funcName, "vkResetEvent")) return vkResetEvent;
            break;
        case 0x11b7e8d5u:
            if (!strcmp(funcName, "vkCreateBufferView")) return vkCreateBufferView;
            break;
        case 0x827878bbu:
            if (!strcmp(funcName, "vkDestroyBufferView")) return vkDestroyBufferView;
            break;
        case 0x6f596ac7u:
            if (!strcmp(funcName, "vkCreateShaderModule")) return vkCreateShaderModule;
            break;
        case 0x6892d8b1u:
            if (!strcmp(funcName, "vkDestroyShaderModule")) return vkDestroyShaderModule;
            break;
        case 0x3e71c4c6u:
            if (!strcmp(funcName, "vkCreatePipelineCache")) return vkCreatePipelineCache;
            break;
        case 0x396c2a34u:
            if (!strcmp(funcName, "vkDestroyPipelineCache")) return vkDestroyPipelineCache;
            break;
        case 0x6855c1a6u:
            if (!strcmp(funcName, "vkGetPipelineCacheData")) return vkGetPipelineCacheData;
            break;
        case 0xc71137ebu:
            if (!strcmp(funcName, "vkMergePipelineCaches")) return vkMergePipelineCaches;
            break;
        case 0xc45193c6u:
            if (!strcmp(funcName, "vkCreateComputePipelines")) return vkCreateComputePipelines;
            break;
        case 0x84481076u:
            if (!strcmp(funcName, "vkDestroyPipeline")) return vkDestroyPipeline;
            break;
        case 0xaffa89a2u:
            if (!strcmp(funcName, "vkCreatePipelineLayout")) return vkCreatePipelineLayout;
            break;
        case 0xe5b1e1d0u:
            if (!strcmp(funcName, "vkDestroyPipelineLayout")) return vkDestroyPipelineLayout;
            break;
        case 0x5d12d31eu:
            if (!strcmp(funcName, "vkCreateSampler")) return vkCreateSampler;
            break;
        case 0x2842f278u:
            if (!strcmp(funcName, "vkDestroySampler")) return vkDestroySampler;
            break;
        case 0x452f43a7u:
            if (!strcmp(funcName, "vkCreateDescriptorSetLayout")) return vkCreateDescriptorSetLayout;
            break;
        case 0x123433f5u:
            if (!strcmp(funcName, "vkDestroyDescriptorSetLayout")) return vkDestroyDescriptorSetLayout;
            break;
        case 0x6392b8cdu:
            if (!strcmp(funcName, "vkCreateDescriptorPool")) return vkCreateDescriptorPool;
            break;
        case 0x752f3e6fu:
            if (!strcmp(funcName, "vkDestroyDescriptorPool")) return vkDestroyDescriptorPool;
            break;
        case 0x19c5052au:
            if (!strcmp(funcName, "vkResetDescriptorPool")) return vkResetDescriptorPool;
            break;
        case 0x14a3b709u:
            if (!strcmp(funcName, "vkAllocateDescriptorSets")) return vkAllocateDescriptorSets;
            break;
        case 0x2d791f80u:
            if (!strcmp(funcName, "vkFreeDescriptorSets")) return vkFreeDescriptorSets;
            break;
        case 0x13d7722fu:
            if (!strcmp(funcName, "vkUpdateDescriptorSets")) return vkUpdateDescriptorSets;
            break;
        case 0x3306d993u:
            if (!strcmp(funcName, "vkCmdBindPipeline")) return vkCmdBindPipeline;
            break;
        case 0x4f8ac889u:
            if (!strcmp(funcName, "vkCmdBindDescriptorSets")) return vkCmdBindDescriptorSets;
            break;
        case 0x146f399bu:
            if (!strcmp(funcName, "vkCmdClearColorImage")) return vkCmdClearColorImage;
            break;
        case 0x1f28c0e0u:
            if (!strcmp(funcName, "vkCmdDispatch")) return vkCmdDispatch;
            break;
        case 0x6d599ab6u:
            if (!strcmp(funcName, "vkCmdDispatchIndirect")) return vkCmdDispatchIndirect;
            break;
        case 0x797f43fau:
            if (!strcmp(funcName, "vkCmdSetEvent")) return vkCmdSetEvent;
            break;
        case 0x7231a513u:
            if (!strcmp(funcName, "vkCmdResetEvent")) return vkCmdResetEvent;
            break;
        case 0x18acad2eu:
            if (!strcmp(funcName, "vkCmdWaitEvents")) return vkCmdWaitEvents;
            break;
        case 0xd617972fu:
            if (!strcmp(funcName, "vkCmdPushConstants")) return vkCmdPushConstants;
            break;
        case 0xffdc1506u:
            if (!strcmp(funcName, "vkCreateGraphicsPipelines")) return vkCreateGraphicsPipelines;
            break;
        case 0x8c0b3c69u:
            if (!strcmp(funcName, "vkCreateFramebuffer")) return vkCreateFramebuffer;
            break;
        case 0xf60843dbu:
            if (!strcmp(funcName, "vkDestroyFramebuffer")) return vkDestroyFramebuffer;
            break;
        case 0xe75b2217u:
            if (!strcmp(funcName, "vkCreateRenderPass")) return vkCreateRenderPass;
            break;
        case 0x235b7d71u:
            if (!strcmp(funcName, "vkDestroyRenderPass")) return vkDestroyRenderPass;
            break;
        case 0x0984abf1u:
            if (!strcmp(funcName, "vkGetRenderAreaGranularity")) return vkGetRenderAreaGranularity;
            break;
        case 0x64e0e218u:
            if (!strcmp(funcName, "vkCmdSetViewport")) return vkCmdSetViewport;
            break;
        case 0xbcc3cad4u:
            if (!strcmp(funcName, "vkCmdSetScissor")) return vkCmdSetScissor;
            break;
        case 0x89505460u:
            if (!strcmp(funcName, "vkCmdSetLineWidth")) return vkCmdSetLineWidth;
            break;
        case 0x9da9f23eu:
            if (!strcmp(funcName, "vkCmdSetDepthBias")) return vkCmdSetDepthBias;
            break;
        case 0xb11418dau:
            if (!strcmp(funcName, "vkCmdSetBlendConstants")) return vkCmdSetBlendConstants;
            break;
        case 0x8aad23c2u:
            if (!strcmp(funcName, "vkCmdSetDepthBounds")) return vkCmdSetDepthBounds;
            break;
        case 0xb6ee9565u:
            if (!strcmp(funcName, "vkCmdSetStencilCompareMask")) return vkCmdSetStencilCompareMask;
            break;
        case 0xe559d0e1u:
            if (!strcmp(funcName, "vkCmdSetStencilWriteMask")) return vkCmdSetStencilWriteMask;
            break;
        case 0x1d031387u:
            if (!strcmp(funcName, "vkCmdSetStencilReference")) return vkCmdSetStencilReference;
            break;
        case 0xf1ac7a71u:
            if (!strcmp(funcName, "vkCmdBindIndexBuffer")) return vkCmdBindIndexBuffer;
            break;
        case 0xadd4e8dau:
            if (!strcmp(funcName, "vkCmdBindVertexBuffers")) return vkCmdBindVertexBuffers;
            break;
        case 0xa9fdc2c0u:
            if (!strcmp(funcName, "vkCmdDraw")) return vkCmdDraw;
            break;
        case 0x44b84ecdu:
            if (!strcmp(funcName, "vkCmdDrawIndexed")) return vkCmdDrawIndexed;
            break;
        case 0x570978d6u:
            if (!strcmp(funcName, "vkCmdDrawIndirect")) return vkCmdDrawIndirect;
            break;
        case 0xa2dd69bfu:
            if (!strcmp(funcName, "vkCmdDrawIndexedIndirect")) return vkCmdDrawIndexedIndirect;
            break;
        case 0xef654202u:
            if (!strcmp(funcName, "vkCmdBlitImage")) return vkCmdBlitImage;
            break;
        case 0x13bc9e59u:
            if (!strcmp(funcName, "vkCmdClearDepthStencilImage")) return vkCmdClearDepthStencilImage;
            break;
        case 0x049d5213u:
            if (!strcmp(funcName, "vkCmdClearAttachments")) return vkCmdClearAttachments;
            break;
        case 0xd84ff14bu:
            if (!strcmp(funcName, "vkCmdResolveImage")) return vkCmdResolveImage;
            break;
        case 0x79052222u:
            if (!strcmp(funcName, "vkCmdBeginRenderPass")) return vkCmdBeginRenderPass;
            break;
        case 0x94036722u:
            if (!strcmp(funcName, "vkCmdNextSubpass")) return vkCmdNextSubpass;
            break;
        case 0xeb0917fau:
            if (!strcmp(funcName, "vkCmdEndRenderPass")) return vkCmdEndRenderPass;
            break;
        case 0x43784012u:
            if (!strcmp(funcName, "vkBindBufferMemory2")) return vkBindBufferMemory2;
            break;
        case 0x1bb3d295u:
            if (!strcmp(funcName, "vkBindImageMemory2")) return vkBindImageMemory2;
            break;
        case 0xf5a1feb7u:
            if (!strcmp(funcName, "vkGetDeviceGroupPeerMemoryFeatures")) return vkGetDeviceGroupPeerMemoryFeatures;
            break;
        case 0xf0313ec8u:
            if (!strcmp(funcName, "vkCmdSetDeviceMask")) return vkCmdSetDeviceMask;
            break;
        case 0x297b509fu:
            if (!strcmp(funcName, "vkEnumeratePhysicalDeviceGroups")) return vkEnumeratePhysicalDeviceGroups;
            break;
        case 0x512e6a1eu:
            if (!strcmp(funcName, "vkGetImageMemoryRequirements2")) return vkGetImageMemoryRequirements2;
            break;
        case 0xff2282cbu:
            if (!strcmp(funcName, "vkGetBufferMemoryRequirements2")) return vkGetBufferMemoryRequirements2;
            break;
        case 0xd207cb22u:
            if (!strcmp(funcName, "vkGetImageSparseMemoryRequirements2")) return vkGetImageSparseMemoryRequirements2;
            break;
        case 0x806e6e48u:
            if (!strcmp(funcName, "vkGetPhysicalDeviceFeatures2")) return vkGetPhysicalDeviceFeatures2;
            break;
        case 0x543bd08au:
            if (!strcmp(funcName, "vkGetPhysicalDeviceProperties2")) return vkGetPhysicalDeviceProperties2;
            break;
        case 0xcece3a97u:
            if (!strcmp(funcName, "vkGetPhysicalDeviceFormatProperties2")) return vkGetPhysicalDeviceFormatProperties2;
            break;
        case 0x71293356u:
            if (!strcmp(funcName, "vkGetPhysicalDeviceImageFormatProperties2")) return vkGetPhysicalDeviceImageFormatProperties2;
            break;
        case 0xcc84890du:
            if (!strcmp(funcName, "vkGetPhysicalDeviceQueueFamilyProperties2")) return vkGetPhysicalDeviceQueueFamilyProperties2;
            break;
        case 0xd027e2abu:
            if (!strcmp(funcName, "vkGetPhysicalDeviceMemoryProperties2")) return vkGetPhysicalDeviceMemoryProperties2;
            break;
        case 0xb2cd0dd6u:
            if (!strcmp(funcName, "vkGetPhysicalDeviceSparseImageFormatProperties2"))
                return vkGetPhysicalDeviceSparseImageFormatProperties2;
            break;
        case 0x59c4eabdu:
            if (!strcmp(funcName, "vkTrimCommandPool")) return vkTrimCommandPool;
            break;
        case 0xa7e80b5bu:
            if (!strcmp(funcName, "vkGetDeviceQueue2")) return vkGetDeviceQueue2;
            break;
        case 0x0b1f96f9u:
            if (!strcmp(funcName, "vkGetPhysicalDeviceExternalBufferProperties"))
                return vkGetPhysicalDeviceExternalBufferProperties;
            break;
        case 0x9e415ee2u:
            if (!strcmp(funcName, "vkGetPhysicalDeviceExternalFenceProperties")) return vkGetPhysicalDeviceExternalFenceProperties;
            break;
        case 0xc98f5959u:
            if (!strcmp(funcName, "vkGetPhysicalDeviceExternalSemaphoreProperties"))
                return vkGetPhysicalDeviceExternalSemaphoreProperties;
            break;
        case 0x95d77705u:
            if (!strcmp(funcName, "vkCmdDispatchBase")) return vkCmdDispatchBase;
            break;
        case 0xdde2a868u:
            if (!strcmp(funcName, "vkCreateDescriptorUpdateTemplate")) return vkCreateDescriptorUpdateTemplate;
            break;
        case 0x8769c7c2u:
            if (!strcmp(funcName, "vkDestroyDescriptorUpdateTemplate")) return vkDestroyDescriptorUpdateTemplate;
            break;
        case 0xe9650ec4u:
            if (!strcmp(funcName, "vkUpdateDescriptorSetWithTemplate")) return vkUpdateDescriptorSetWithTemplate;
            break;
        case 0xfd510f82u:
            if (!strcmp(funcName, "vkGetDescriptorSetLayoutSupport")) return vkGetDescriptorSetLayoutSupport;
            break;
        case 0xf850dd41u:
            if (!strcmp(funcName, "vkCreateSamplerYcbcrConversion")) return vkCreateSamplerYcbcrConversion;
            break;
        case 0x8b04a087u:
            if (!strcmp(funcName, "vkDestroySamplerYcbcrConversion")) return vkDestroySamplerYcbcrConversion;
            break;
        case 0x4f91a755u:
            if (!strcmp(funcName, "vkResetQueryPool")) return vkResetQueryPool;
            break;
        case 0x036431c1u:
            if (!strcmp(funcName, "vkGetSemaphoreCounterValue")) return vkGetSemaphoreCounterValue;
            break;
        case 0xc2aca738u:
            if (!strcmp(funcName, "vkWaitSemaphores")) return vkWaitSemaphores;
            break;
        case 0x307d1a20u:
            if (!strcmp(funcName, "vkSignalSemaphore")) return vkSignalSemaphore;
            break;
        case 0x5a76a96eu:
            if (!strcmp(funcName, "vkGetBufferDeviceAddress")) return vkGetBufferDeviceAddress;
            break;
        case 0x7e52a54du:
            if (!strcmp(funcName, "vkGetBufferOpaqueCaptureAddress")) return vkGetBufferOpaqueCaptureAddress;
            break;
        case 0xcf71e086u:
            if (!strcmp(funcName, "vkGetDeviceMemoryOpaqueCaptureAddress")) return vkGetDeviceMemoryOpaqueCaptureAddress;
            break;
        case 0x772b9685u:
            if (!strcmp(funcName, "vkCmdDrawIndirectCount")) return vkCmdDrawIndirectCount;
            break;
        case 0x570f1d66u:
            if (!strcmp(funcName, "vkCmdDrawIndexedIndirectCount")) return vkCmdDrawIndexedIndirectCount;
            break;
        case 0x5976c03fu:
            if (!strcmp(funcName, "vkCreateRenderPass2")) return vkCreateRenderPass2;
            break;
        case 0x93149f30u:
            if (!strcmp(funcName, "vkCmdBeginRenderPass2")) return vkCmdBeginRenderPass2;
            break;
        case 0x115b3e30u:
            if (!strcmp(funcName, "vkCmdNextSubpass2")) return vkCmdNextSubpass2;
            break;
        case 0xc7506fd8u:
            if (!strcmp(funcName, "vkCmdEndRenderPass2")) return vkCmdEndRenderPass2;
            break;
        case 0x2191b19eu:
            if (!strcmp(funcName, "vkGetPhysicalDeviceToolProperties")) return vkGetPhysicalDeviceToolProperties;
            break;
        case 0x26afd535u:
            if (!strcmp(funcName, "vkCreatePrivateDataSlot")) return vkCreatePrivateDataSlot;
            break;
        case 0xd5976337u:
            if (!strcmp(funcName, "vkDestroyPrivateDataSlot")) return vkDestroyPrivateDataSlot;
            break;
        case 0x8ea4745du:
            if (!strcmp(funcName, "vkSetPrivateData")) return vkSetPrivateData;
            break;
        case 0xe9a91971u:
            if (!strcmp(funcName, "vkGetPrivateData")) return vkGetPrivateData;
            break;
        case 0xbf2d15b3u:
            if (!strcmp(funcName, "vkCmdPipelineBarrier2")) return vkCmdPipelineBarrier2;
            break;
        case 0x8295a7e7u:
            if (!strcmp(funcName, "vkCmdWriteTimestamp2")) return vkCmdWriteTimestamp2;
            break;
        case 0xb49ac391u:
            if (!strcmp(funcName, "vkQueueSubmit2")) return vkQueueSubmit2;
            break;
        case 0xa82f4fe9u:
            if (!strcmp(funcName, "vkCmdCopyBuffer2")) return vkCmdCopyBuffer2;
            break;
        case 0xebe52d66u:
            if (!strcmp(funcName, "vkCmdCopyImage2")) return vkCmdCopyImage2;
            break;
        case 0x765ed15du:
            if (!strcmp(funcName, "vkCmdCopyBufferToImage2")) return vkCmdCopyBufferToImage2;
            break;
        case 0xb5e7f467u:
            if (!strcmp(funcName, "vkCmdCopyImageToBuffer2")) return vkCmdCopyImageToBuffer2;
            break;
        case 0xe338387du:
            if (!strcmp(funcName, "vkGetDeviceBufferMemoryRequirements")) return vkGetDeviceBufferMemoryRequirements;
            break;
        case 0xb451de1au:
            if (!strcmp(funcName, "vkGetDeviceImageMemoryRequirements")) return vkGetDeviceImageMemoryRequirements;
            break;
        case 0xbbff0df2u:
            if (!strcmp(funcName, "vkGetDeviceImageSparseMemoryRequirements")) return vkGetDeviceImageSparseMemoryRequirements;
            break;
        case 0x0b57b3d8u:
            if (!strcmp(funcName, "vkCmdSetEvent2")) return vkCmdSetEvent2;
            break;
        case 0xe526f2f3u:
            if (!strcmp(funcName, "vkCmdResetEvent2")) return vkCmdResetEvent2;
            break;
        case 0xf3d48314u:
            if (!strcmp(funcName, "vkCmdWaitEvents2")) return vkCmdWaitEvents2;
            break;
        case 0x0c673190u:
            if (!strcmp(funcName, "vkCmdBlitImage2")) return vkCmdBlitImage2;
            break;
        case 0xfed9217bu:
            if (!strcmp(funcName, "vkCmdResolveImage2")) return vkCmdResolveImage2;
            break;
        case 0xc23aecb9u:
            if (!strcmp(funcName, "vkCmdBeginRendering")) return vkCmdBeginRendering;
            break;
        case 0x504cb441u:
            if (!strcmp(funcName, "vkCmdEndRendering")) return vkCmdEndRendering;
            break;
        case 0xbb1fdbfdu:
            if (!strcmp(funcName, "vkCmdSetCullMode")) return vkCmdSetCullMode;
            break;
        case 0xec690b60u:
            if (!strcmp(funcName, "vkCmdSetFrontFace")) return vkCmdSetFrontFace;
            break;
        case 0xb4387882u:
            if (!strcmp(funcName, "vkCmdSetPrimitiveTopology")) return vkCmdSetPrimitiveTopology;
            break;
        case 0xa0611d7fu:
            if (!strcmp(funcName, "vkCmdSetViewportWithCount")) return vkCmdSetViewportWithCount;
            break;
        case 0xfc5a2533u:
            if (!strcmp(funcName, "vkCmdSetScissorWithCount")) return vkCmdSetScissorWithCount;
            break;
        case 0x8e2aa538u:
            if (!strcmp(funcName, "vkCmdBindVertexBuffers2")) return vkCmdBindVertexBuffers2;
            break;
        case 0x4874399cu:
            if (!strcmp(funcName, "vkCmdSetDepthTestEnable")) return vkCmdSetDepthTestEnable;
            break;
        case 0x48a8eb79u:
            if (!strcmp(funcName, "vkCmdSetDepthWriteEnable")) return vkCmdSetDepthWriteEnable;
            break;
        case 0xeee7ee11u:
            if (!strcmp(funcName, "vkCmdSetDepthCompareOp")) return vkCmdSetDepthCompareOp;
            break;
        case 0x47579071u:
            if (!strcmp(funcName, "vkCmdSetDepthBoundsTestEnable")) return vkCmdSetDepthBoundsTestEnable;
            break;
        case 0xb486d021u:
            if (!strcmp(funcName, "vkCmdSetStencilTestEnable")) return vkCmdSetStencilTestEnable;
            break;
        case 0x4d91cb75u:
            if (!strcmp(funcName, "vkCmdSetStencilOp")) return vkCmdSetStencilOp;
            break;
        case 0x3fb78c34u:
            if (!strcmp(funcName, "vkCmdSetRasterizerDiscardEnable")) return vkCmdSetRasterizerDiscardEnable;
            break;
        case 0x9b2a547du:
            if (!strcmp(funcName, "vkCmdSetDepthBiasEnable")) return vkCmdSetDepthBiasEnable;
            break;
        case 0xd42a0ba9u:
            if (!strcmp(funcName, "vkCmdSetPrimitiveRestartEnable")) return vkCmdSetPrimitiveRestartEnable;
            break;
        case 0x4d139743u:
            if (!strcmp(funcName, "vkMapMemory2")) return vkMapMemory2;
            break;
        case 0x49c75348u:
            if (!strcmp(funcName, "vkUnmapMemory2")) return vkUnmapMemory2;
            break;
        case 0xa108f16bu:
            if (!strcmp(funcName, "vkGetDeviceImageSubresourceLayout")) return vkGetDeviceImageSubresourceLayout;
            break;
        case 0x49a3b45du:
            if (!strcmp(funcName, "vkGetImageSubresourceLayout2")) return vkGetImageSubresourceLayout2;
            break;
        case 0x5fb583a2u:
            if (!strcmp(funcName, "vkCopyMemoryToImage")) return vkCopyMemoryToImage;
            break;
        case 0x6e1e2502u:
            if (!strcmp(funcName, "vkCopyImageToMemory")) return vkCopyImageToMemory;
            break;
        case 0x7118af0eu:
            if (!strcmp(funcName, "vkCopyImageToImage")) return vkCopyImageToImage;
            break;
        case 0xe09b90fcu:
            if (!strcmp(funcName, "vkTransitionImageLayout")) return vkTransitionImageLayout;
            break;
        case 0x05d80881u:
            if (!strcmp(funcName, "vkCmdPushDescriptorSet")) return vkCmdPushDescriptorSet;
            break;
        case 0xcb42dffbu:
            if (!strcmp(funcName, "vkCmdPushDescriptorSetWithTemplate")) return vkCmdPushDescriptorSetWithTemplate;
            break;
        case 0xf279fe61u:
            if (!strcmp(funcName, "vkCmdBindDescriptorSets2")) return vkCmdBindDescriptorSets2;
            break;
        case 0x2422e2a7u:
            if (!strcmp(funcName, "vkCmdPushConstants2")) return vkCmdPushConstants2;
            break;
        case 0xe615b1c9u:
            if (!strcmp(funcName, "vkCmdPushDescriptorSet2")) return vkCmdPushDescriptorSet2;
            break;
        case 0xc346496bu:
            if (!strcmp(funcName, "vkCmdPushDescriptorSetWithTemplate2")) return vkCmdPushDescriptorSetWithTemplate2;
            break;
        case 0x84520929u:
            if (!strcmp(funcName, "vkCmdSetLineStipple")) return vkCmdSetLineStipple;
            break;
        case 0xb5847779u:
            if (!strcmp(funcName, "vkCmdBindIndexBuffer2")) return vkCmdBindIndexBuffer2;
            break;
        case 0x2154c31du:
            if (!strcmp(funcName, "vkGetRenderingAreaGranularity")) return vkGetRenderingAreaGranularity;
            break;
        case 0x4c431a0du:
            if (!strcmp(funcName, "vkCmdSetRenderingAttachmentLocations")) return vkCmdSetRenderingAttachmentLocations;
            break;
        case 0x9a656a9cu:
            if (!strcmp(funcName, "vkCmdSetRenderingInputAttachmentIndices")) return vkCmdSetRenderingInputAttachmentIndices;
            break;
        default:
            break;
    }

    // Instance extensions
    void* addr;
    if (debug_extensions_InstanceGpa(inst, funcName, &addr)) return addr;

    if (wsi_swapchain_instance_gpa(inst, funcName, &addr)) return addr;

    if (extension_instance_gpa(inst, funcName, name_hash, &addr)) return addr;

    // Unknown physical device extensions
    addr = loader_phys_dev_ext_gpa_tramp(inst, funcName);
    if (NULL != addr) return addr;

    // Unknown device extensions
    addr = loader_dev_ext_gpa_tramp(inst, funcName);
    return addr;
}
