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

#include "vk_test_entrypoint_driver.h"

EntrypointTestDriver driver;
extern "C" {
FRAMEWORK_EXPORT EntrypointTestDriver* get_ep_driver_func() { return &driver; }
FRAMEWORK_EXPORT EntrypointTestDriver* reset_ep_driver_func() {
    driver.~EntrypointTestDriver();
    return new (&driver) EntrypointTestDriver();
}
}

// Alias prototypes
//---------------------
VKAPI_ATTR void VKAPI_CALL driver_CmdBeginRendering(VkCommandBuffer commandBuffer, const VkRenderingInfo* pRenderingInfo);
VKAPI_ATTR void VKAPI_CALL driver_CmdEndRendering(VkCommandBuffer commandBuffer);
VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceFeatures2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures2* pFeatures);
VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties2* pProperties);
VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceFormatProperties2(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties2* pFormatProperties);
VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceImageFormatProperties2(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo, VkImageFormatProperties2* pImageFormatProperties);
VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceQueueFamilyProperties2(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties2* pQueueFamilyProperties);
VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceMemoryProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties2* pMemoryProperties);
VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceSparseImageFormatProperties2(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo, uint32_t* pPropertyCount, VkSparseImageFormatProperties2* pProperties);
VKAPI_ATTR void VKAPI_CALL driver_GetDeviceGroupPeerMemoryFeatures(VkDevice device, uint32_t heapIndex, uint32_t localDeviceIndex, uint32_t remoteDeviceIndex, VkPeerMemoryFeatureFlags* pPeerMemoryFeatures);
VKAPI_ATTR void VKAPI_CALL driver_CmdSetDeviceMask(VkCommandBuffer commandBuffer, uint32_t deviceMask);
VKAPI_ATTR void VKAPI_CALL driver_CmdDispatchBase(VkCommandBuffer commandBuffer, uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);
VKAPI_ATTR void VKAPI_CALL driver_TrimCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolTrimFlags flags);
VKAPI_ATTR VkResult VKAPI_CALL driver_EnumeratePhysicalDeviceGroups(VkInstance instance, uint32_t* pPhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties);
VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceExternalBufferProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo, VkExternalBufferProperties* pExternalBufferProperties);
VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceExternalSemaphoreProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo, VkExternalSemaphoreProperties* pExternalSemaphoreProperties);
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateDescriptorUpdateTemplate(VkDevice device, const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorUpdateTemplate* pDescriptorUpdateTemplate);
VKAPI_ATTR void VKAPI_CALL driver_DestroyDescriptorUpdateTemplate(VkDevice device, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const VkAllocationCallbacks* pAllocator);
VKAPI_ATTR void VKAPI_CALL driver_UpdateDescriptorSetWithTemplate(VkDevice device, VkDescriptorSet descriptorSet, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const void* pData);
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateRenderPass2(VkDevice device, const VkRenderPassCreateInfo2* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass);
VKAPI_ATTR void VKAPI_CALL driver_CmdBeginRenderPass2(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, const VkSubpassBeginInfo* pSubpassBeginInfo);
VKAPI_ATTR void VKAPI_CALL driver_CmdNextSubpass2(VkCommandBuffer commandBuffer, const VkSubpassBeginInfo* pSubpassBeginInfo, const VkSubpassEndInfo* pSubpassEndInfo);
VKAPI_ATTR void VKAPI_CALL driver_CmdEndRenderPass2(VkCommandBuffer commandBuffer, const VkSubpassEndInfo* pSubpassEndInfo);
VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceExternalFenceProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo, VkExternalFenceProperties* pExternalFenceProperties);
VKAPI_ATTR void VKAPI_CALL driver_GetImageMemoryRequirements2(VkDevice device, const VkImageMemoryRequirementsInfo2* pInfo, VkMemoryRequirements2* pMemoryRequirements);
VKAPI_ATTR void VKAPI_CALL driver_GetBufferMemoryRequirements2(VkDevice device, const VkBufferMemoryRequirementsInfo2* pInfo, VkMemoryRequirements2* pMemoryRequirements);
VKAPI_ATTR void VKAPI_CALL driver_GetImageSparseMemoryRequirements2(VkDevice device, const VkImageSparseMemoryRequirementsInfo2* pInfo, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements2* pSparseMemoryRequirements);
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateSamplerYcbcrConversion(VkDevice device, const VkSamplerYcbcrConversionCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSamplerYcbcrConversion* pYcbcrConversion);
VKAPI_ATTR void VKAPI_CALL driver_DestroySamplerYcbcrConversion(VkDevice device, VkSamplerYcbcrConversion ycbcrConversion, const VkAllocationCallbacks* pAllocator);
VKAPI_ATTR VkResult VKAPI_CALL driver_BindBufferMemory2(VkDevice device, uint32_t bindInfoCount, const VkBindBufferMemoryInfo* pBindInfos);
VKAPI_ATTR VkResult VKAPI_CALL driver_BindImageMemory2(VkDevice device, uint32_t bindInfoCount, const VkBindImageMemoryInfo* pBindInfos);
VKAPI_ATTR void VKAPI_CALL driver_GetDescriptorSetLayoutSupport(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, VkDescriptorSetLayoutSupport* pSupport);
VKAPI_ATTR void VKAPI_CALL driver_CmdDrawIndirectCount(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride);
VKAPI_ATTR void VKAPI_CALL driver_CmdDrawIndexedIndirectCount(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride);
VKAPI_ATTR VkResult VKAPI_CALL driver_GetSemaphoreCounterValue(VkDevice device, VkSemaphore semaphore, uint64_t* pValue);
VKAPI_ATTR VkResult VKAPI_CALL driver_WaitSemaphores(VkDevice device, const VkSemaphoreWaitInfo* pWaitInfo, uint64_t timeout);
VKAPI_ATTR VkResult VKAPI_CALL driver_SignalSemaphore(VkDevice device, const VkSemaphoreSignalInfo* pSignalInfo);
VKAPI_ATTR VkDeviceAddress VKAPI_CALL driver_GetBufferDeviceAddress(VkDevice device, const VkBufferDeviceAddressInfo* pInfo);
VKAPI_ATTR uint64_t VKAPI_CALL driver_GetBufferOpaqueCaptureAddress(VkDevice device, const VkBufferDeviceAddressInfo* pInfo);
VKAPI_ATTR uint64_t VKAPI_CALL driver_GetDeviceMemoryOpaqueCaptureAddress(VkDevice device, const VkDeviceMemoryOpaqueCaptureAddressInfo* pInfo);
VKAPI_ATTR void VKAPI_CALL driver_CmdSetEvent2(VkCommandBuffer commandBuffer, VkEvent event, const VkDependencyInfo* pDependencyInfo);
VKAPI_ATTR void VKAPI_CALL driver_CmdResetEvent2(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags2 stageMask);
VKAPI_ATTR void VKAPI_CALL driver_CmdWaitEvents2(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent* pEvents, const VkDependencyInfo* pDependencyInfos);
VKAPI_ATTR void VKAPI_CALL driver_CmdPipelineBarrier2(VkCommandBuffer commandBuffer, const VkDependencyInfo* pDependencyInfo);
VKAPI_ATTR void VKAPI_CALL driver_CmdWriteTimestamp2(VkCommandBuffer commandBuffer, VkPipelineStageFlags2 stage, VkQueryPool queryPool, uint32_t query);
VKAPI_ATTR VkResult VKAPI_CALL driver_QueueSubmit2(VkQueue queue, uint32_t submitCount, const VkSubmitInfo2* pSubmits, VkFence fence);
VKAPI_ATTR void VKAPI_CALL driver_CmdCopyBuffer2(VkCommandBuffer commandBuffer, const VkCopyBufferInfo2* pCopyBufferInfo);
VKAPI_ATTR void VKAPI_CALL driver_CmdCopyImage2(VkCommandBuffer commandBuffer, const VkCopyImageInfo2* pCopyImageInfo);
VKAPI_ATTR void VKAPI_CALL driver_CmdCopyBufferToImage2(VkCommandBuffer commandBuffer, const VkCopyBufferToImageInfo2* pCopyBufferToImageInfo);
VKAPI_ATTR void VKAPI_CALL driver_CmdCopyImageToBuffer2(VkCommandBuffer commandBuffer, const VkCopyImageToBufferInfo2* pCopyImageToBufferInfo);
VKAPI_ATTR void VKAPI_CALL driver_CmdBlitImage2(VkCommandBuffer commandBuffer, const VkBlitImageInfo2* pBlitImageInfo);
VKAPI_ATTR void VKAPI_CALL driver_CmdResolveImage2(VkCommandBuffer commandBuffer, const VkResolveImageInfo2* pResolveImageInfo);
VKAPI_ATTR void VKAPI_CALL driver_GetDeviceBufferMemoryRequirements(VkDevice device, const VkDeviceBufferMemoryRequirements* pInfo, VkMemoryRequirements2* pMemoryRequirements);
VKAPI_ATTR void VKAPI_CALL driver_GetDeviceImageMemoryRequirements(VkDevice device, const VkDeviceImageMemoryRequirements* pInfo, VkMemoryRequirements2* pMemoryRequirements);
VKAPI_ATTR void VKAPI_CALL driver_GetDeviceImageSparseMemoryRequirements(VkDevice device, const VkDeviceImageMemoryRequirements* pInfo, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements2* pSparseMemoryRequirements);
VKAPI_ATTR void VKAPI_CALL driver_CmdDrawIndirectCount(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride);
VKAPI_ATTR void VKAPI_CALL driver_CmdDrawIndexedIndirectCount(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride);
VKAPI_ATTR VkResult VKAPI_CALL driver_GetRayTracingShaderGroupHandlesKHR(VkDevice device, VkPipeline pipeline, uint32_t firstGroup, uint32_t groupCount, size_t dataSize, void* pData);
VKAPI_ATTR VkDeviceAddress VKAPI_CALL driver_GetBufferDeviceAddress(VkDevice device, const VkBufferDeviceAddressInfo* pInfo);
VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceToolProperties(VkPhysicalDevice physicalDevice, uint32_t* pToolCount, VkPhysicalDeviceToolProperties* pToolProperties);
VKAPI_ATTR void VKAPI_CALL driver_ResetQueryPool(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount);
VKAPI_ATTR void VKAPI_CALL driver_CmdSetCullMode(VkCommandBuffer commandBuffer, VkCullModeFlags cullMode);
VKAPI_ATTR void VKAPI_CALL driver_CmdSetFrontFace(VkCommandBuffer commandBuffer, VkFrontFace frontFace);
VKAPI_ATTR void VKAPI_CALL driver_CmdSetPrimitiveTopology(VkCommandBuffer commandBuffer, VkPrimitiveTopology primitiveTopology);
VKAPI_ATTR void VKAPI_CALL driver_CmdSetViewportWithCount(VkCommandBuffer commandBuffer, uint32_t viewportCount, const VkViewport* pViewports);
VKAPI_ATTR void VKAPI_CALL driver_CmdSetScissorWithCount(VkCommandBuffer commandBuffer, uint32_t scissorCount, const VkRect2D* pScissors);
VKAPI_ATTR void VKAPI_CALL driver_CmdBindVertexBuffers2(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets, const VkDeviceSize* pSizes, const VkDeviceSize* pStrides);
VKAPI_ATTR void VKAPI_CALL driver_CmdSetDepthTestEnable(VkCommandBuffer commandBuffer, VkBool32 depthTestEnable);
VKAPI_ATTR void VKAPI_CALL driver_CmdSetDepthWriteEnable(VkCommandBuffer commandBuffer, VkBool32 depthWriteEnable);
VKAPI_ATTR void VKAPI_CALL driver_CmdSetDepthCompareOp(VkCommandBuffer commandBuffer, VkCompareOp depthCompareOp);
VKAPI_ATTR void VKAPI_CALL driver_CmdSetDepthBoundsTestEnable(VkCommandBuffer commandBuffer, VkBool32 depthBoundsTestEnable);
VKAPI_ATTR void VKAPI_CALL driver_CmdSetStencilTestEnable(VkCommandBuffer commandBuffer, VkBool32 stencilTestEnable);
VKAPI_ATTR void VKAPI_CALL driver_CmdSetStencilOp(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, VkStencilOp failOp, VkStencilOp passOp, VkStencilOp depthFailOp, VkCompareOp compareOp);
VKAPI_ATTR VkResult VKAPI_CALL driver_CreatePrivateDataSlot(VkDevice device, const VkPrivateDataSlotCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPrivateDataSlot* pPrivateDataSlot);
VKAPI_ATTR void VKAPI_CALL driver_DestroyPrivateDataSlot(VkDevice device, VkPrivateDataSlot privateDataSlot, const VkAllocationCallbacks* pAllocator);
VKAPI_ATTR VkResult VKAPI_CALL driver_SetPrivateData(VkDevice device, VkObjectType objectType, uint64_t objectHandle, VkPrivateDataSlot privateDataSlot, uint64_t data);
VKAPI_ATTR void VKAPI_CALL driver_GetPrivateData(VkDevice device, VkObjectType objectType, uint64_t objectHandle, VkPrivateDataSlot privateDataSlot, uint64_t* pData);
VKAPI_ATTR void VKAPI_CALL driver_CmdSetRasterizerDiscardEnable(VkCommandBuffer commandBuffer, VkBool32 rasterizerDiscardEnable);
VKAPI_ATTR void VKAPI_CALL driver_CmdSetDepthBiasEnable(VkCommandBuffer commandBuffer, VkBool32 depthBiasEnable);
VKAPI_ATTR void VKAPI_CALL driver_CmdSetPrimitiveRestartEnable(VkCommandBuffer commandBuffer, VkBool32 primitiveRestartEnable);


//Temporary device name
const char driver_phys_device_name[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE] = "TestDriverFakePhysicalDevice";

// Instance extensions supported
const char inst_ext_arr[][VK_MAX_EXTENSION_NAME_SIZE] = {
    "VK_KHR_surface",
    "VK_KHR_display",
#ifdef VK_USE_PLATFORM_XLIB_KHR
    "VK_KHR_xlib_surface",
#endif // VK_USE_PLATFORM_XLIB_KHR
#ifdef VK_USE_PLATFORM_XCB_KHR
    "VK_KHR_xcb_surface",
#endif // VK_USE_PLATFORM_XCB_KHR
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    "VK_KHR_wayland_surface",
#endif // VK_USE_PLATFORM_WAYLAND_KHR
#ifdef VK_USE_PLATFORM_WIN32_KHR
    "VK_KHR_win32_surface",
#endif // VK_USE_PLATFORM_WIN32_KHR
    "VK_KHR_get_physical_device_properties2",
    "VK_KHR_device_group_creation",
    "VK_KHR_external_memory_capabilities",
    "VK_KHR_external_semaphore_capabilities",
    "VK_KHR_external_fence_capabilities",
    "VK_KHR_get_surface_capabilities2",
    "VK_KHR_get_display_properties2",
    "VK_KHR_surface_protected_capabilities",
    "VK_KHR_portability_enumeration",
    "VK_EXT_debug_report",
#ifdef VK_USE_PLATFORM_GGP
    "VK_GGP_stream_descriptor_surface",
#endif // VK_USE_PLATFORM_GGP
    "VK_NV_external_memory_capabilities",
    "VK_EXT_validation_flags",
#ifdef VK_USE_PLATFORM_VI_NN
    "VK_NN_vi_surface",
#endif // VK_USE_PLATFORM_VI_NN
    "VK_EXT_direct_mode_display",
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
    "VK_EXT_acquire_xlib_display",
#endif // VK_USE_PLATFORM_XLIB_XRANDR_EXT
    "VK_EXT_display_surface_counter",
    "VK_EXT_swapchain_colorspace",
#ifdef VK_USE_PLATFORM_IOS_MVK
    "VK_MVK_ios_surface",
#endif // VK_USE_PLATFORM_IOS_MVK
#ifdef VK_USE_PLATFORM_MACOS_MVK
    "VK_MVK_macos_surface",
#endif // VK_USE_PLATFORM_MACOS_MVK
    "VK_EXT_debug_utils",
#ifdef VK_USE_PLATFORM_FUCHSIA
    "VK_FUCHSIA_imagepipe_surface",
#endif // VK_USE_PLATFORM_FUCHSIA
#ifdef VK_USE_PLATFORM_METAL_EXT
    "VK_EXT_metal_surface",
#endif // VK_USE_PLATFORM_METAL_EXT
    "VK_EXT_validation_features",
    "VK_EXT_headless_surface",
    "VK_EXT_acquire_drm_display",
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
    "VK_EXT_directfb_surface",
#endif // VK_USE_PLATFORM_DIRECTFB_EXT
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    "VK_QNX_screen_surface",
#endif // VK_USE_PLATFORM_SCREEN_QNX
    "VK_GOOGLE_surfaceless_query",
};

// Device extensions supported
const char dev_ext_arr[][VK_MAX_EXTENSION_NAME_SIZE] = {
    "VK_KHR_swapchain",
    "VK_KHR_display_swapchain",
    "VK_KHR_sampler_mirror_clamp_to_edge",
#ifdef VK_ENABLE_BETA_EXTENSIONS
    "VK_KHR_video_queue",
#endif // VK_ENABLE_BETA_EXTENSIONS
#ifdef VK_ENABLE_BETA_EXTENSIONS
    "VK_KHR_video_decode_queue",
#endif // VK_ENABLE_BETA_EXTENSIONS
    "VK_KHR_dynamic_rendering",
    "VK_KHR_multiview",
    "VK_KHR_device_group",
    "VK_KHR_shader_draw_parameters",
    "VK_KHR_maintenance1",
    "VK_KHR_external_memory",
#ifdef VK_USE_PLATFORM_WIN32_KHR
    "VK_KHR_external_memory_win32",
#endif // VK_USE_PLATFORM_WIN32_KHR
    "VK_KHR_external_memory_fd",
#ifdef VK_USE_PLATFORM_WIN32_KHR
    "VK_KHR_win32_keyed_mutex",
#endif // VK_USE_PLATFORM_WIN32_KHR
    "VK_KHR_external_semaphore",
#ifdef VK_USE_PLATFORM_WIN32_KHR
    "VK_KHR_external_semaphore_win32",
#endif // VK_USE_PLATFORM_WIN32_KHR
    "VK_KHR_external_semaphore_fd",
    "VK_KHR_push_descriptor",
    "VK_KHR_shader_float16_int8",
    "VK_KHR_16bit_storage",
    "VK_KHR_incremental_present",
    "VK_KHR_descriptor_update_template",
    "VK_KHR_imageless_framebuffer",
    "VK_KHR_create_renderpass2",
    "VK_KHR_shared_presentable_image",
    "VK_KHR_external_fence",
#ifdef VK_USE_PLATFORM_WIN32_KHR
    "VK_KHR_external_fence_win32",
#endif // VK_USE_PLATFORM_WIN32_KHR
    "VK_KHR_external_fence_fd",
    "VK_KHR_performance_query",
    "VK_KHR_maintenance2",
    "VK_KHR_variable_pointers",
    "VK_KHR_dedicated_allocation",
    "VK_KHR_storage_buffer_storage_class",
    "VK_KHR_relaxed_block_layout",
    "VK_KHR_get_memory_requirements2",
    "VK_KHR_image_format_list",
    "VK_KHR_sampler_ycbcr_conversion",
    "VK_KHR_bind_memory2",
#ifdef VK_ENABLE_BETA_EXTENSIONS
    "VK_KHR_portability_subset",
#endif // VK_ENABLE_BETA_EXTENSIONS
    "VK_KHR_maintenance3",
    "VK_KHR_draw_indirect_count",
    "VK_KHR_shader_subgroup_extended_types",
    "VK_KHR_8bit_storage",
    "VK_KHR_shader_atomic_int64",
    "VK_KHR_shader_clock",
    "VK_KHR_global_priority",
    "VK_KHR_driver_properties",
    "VK_KHR_shader_float_controls",
    "VK_KHR_depth_stencil_resolve",
    "VK_KHR_swapchain_mutable_format",
    "VK_KHR_timeline_semaphore",
    "VK_KHR_vulkan_memory_model",
    "VK_KHR_shader_terminate_invocation",
    "VK_KHR_fragment_shading_rate",
    "VK_KHR_spirv_1_4",
    "VK_KHR_separate_depth_stencil_layouts",
    "VK_KHR_present_wait",
    "VK_KHR_uniform_buffer_standard_layout",
    "VK_KHR_buffer_device_address",
    "VK_KHR_deferred_host_operations",
    "VK_KHR_pipeline_executable_properties",
    "VK_KHR_shader_integer_dot_product",
    "VK_KHR_pipeline_library",
    "VK_KHR_shader_non_semantic_info",
    "VK_KHR_present_id",
#ifdef VK_ENABLE_BETA_EXTENSIONS
    "VK_KHR_video_encode_queue",
#endif // VK_ENABLE_BETA_EXTENSIONS
    "VK_KHR_synchronization2",
    "VK_KHR_shader_subgroup_uniform_control_flow",
    "VK_KHR_zero_initialize_workgroup_memory",
    "VK_KHR_workgroup_memory_explicit_layout",
    "VK_KHR_copy_commands2",
    "VK_KHR_format_feature_flags2",
    "VK_KHR_maintenance4",
    "VK_NV_glsl_shader",
    "VK_EXT_depth_range_unrestricted",
    "VK_IMG_filter_cubic",
    "VK_AMD_rasterization_order",
    "VK_AMD_shader_trinary_minmax",
    "VK_AMD_shader_explicit_vertex_parameter",
    "VK_EXT_debug_marker",
    "VK_AMD_gcn_shader",
    "VK_NV_dedicated_allocation",
    "VK_EXT_transform_feedback",
    "VK_NVX_binary_import",
    "VK_NVX_image_view_handle",
    "VK_AMD_draw_indirect_count",
    "VK_AMD_negative_viewport_height",
    "VK_AMD_gpu_shader_half_float",
    "VK_AMD_shader_ballot",
#ifdef VK_ENABLE_BETA_EXTENSIONS
    "VK_EXT_video_encode_h264",
#endif // VK_ENABLE_BETA_EXTENSIONS
#ifdef VK_ENABLE_BETA_EXTENSIONS
    "VK_EXT_video_encode_h265",
#endif // VK_ENABLE_BETA_EXTENSIONS
#ifdef VK_ENABLE_BETA_EXTENSIONS
    "VK_EXT_video_decode_h264",
#endif // VK_ENABLE_BETA_EXTENSIONS
    "VK_AMD_texture_gather_bias_lod",
    "VK_AMD_shader_info",
    "VK_AMD_shader_image_load_store_lod",
    "VK_NV_corner_sampled_image",
    "VK_IMG_format_pvrtc",
    "VK_NV_external_memory",
#ifdef VK_USE_PLATFORM_WIN32_KHR
    "VK_NV_external_memory_win32",
#endif // VK_USE_PLATFORM_WIN32_KHR
#ifdef VK_USE_PLATFORM_WIN32_KHR
    "VK_NV_win32_keyed_mutex",
#endif // VK_USE_PLATFORM_WIN32_KHR
    "VK_EXT_shader_subgroup_ballot",
    "VK_EXT_shader_subgroup_vote",
    "VK_EXT_texture_compression_astc_hdr",
    "VK_EXT_astc_decode_mode",
    "VK_EXT_conditional_rendering",
    "VK_NV_clip_space_w_scaling",
    "VK_EXT_display_control",
    "VK_GOOGLE_display_timing",
    "VK_NV_sample_mask_override_coverage",
    "VK_NV_geometry_shader_passthrough",
    "VK_NV_viewport_array2",
    "VK_NVX_multiview_per_view_attributes",
    "VK_NV_viewport_swizzle",
    "VK_EXT_discard_rectangles",
    "VK_EXT_conservative_rasterization",
    "VK_EXT_depth_clip_enable",
    "VK_EXT_hdr_metadata",
    "VK_EXT_external_memory_dma_buf",
    "VK_EXT_queue_family_foreign",
    "VK_EXT_sampler_filter_minmax",
    "VK_AMD_gpu_shader_int16",
    "VK_AMD_mixed_attachment_samples",
    "VK_AMD_shader_fragment_mask",
    "VK_EXT_inline_uniform_block",
    "VK_EXT_shader_stencil_export",
    "VK_EXT_sample_locations",
    "VK_EXT_blend_operation_advanced",
    "VK_NV_fragment_coverage_to_color",
    "VK_NV_framebuffer_mixed_samples",
    "VK_NV_fill_rectangle",
    "VK_NV_shader_sm_builtins",
    "VK_EXT_post_depth_coverage",
    "VK_EXT_image_drm_format_modifier",
    "VK_EXT_validation_cache",
    "VK_EXT_descriptor_indexing",
    "VK_EXT_shader_viewport_index_layer",
    "VK_NV_shading_rate_image",
    "VK_NV_ray_tracing",
    "VK_NV_representative_fragment_test",
    "VK_EXT_filter_cubic",
    "VK_QCOM_render_pass_shader_resolve",
    "VK_EXT_global_priority",
    "VK_EXT_external_memory_host",
    "VK_AMD_buffer_marker",
    "VK_AMD_pipeline_compiler_control",
    "VK_EXT_calibrated_timestamps",
    "VK_AMD_shader_core_properties",
#ifdef VK_ENABLE_BETA_EXTENSIONS
    "VK_EXT_video_decode_h265",
#endif // VK_ENABLE_BETA_EXTENSIONS
    "VK_AMD_memory_overallocation_behavior",
    "VK_EXT_vertex_attribute_divisor",
#ifdef VK_USE_PLATFORM_GGP
    "VK_GGP_frame_token",
#endif // VK_USE_PLATFORM_GGP
    "VK_EXT_pipeline_creation_feedback",
    "VK_NV_shader_subgroup_partitioned",
    "VK_NV_compute_shader_derivatives",
    "VK_NV_mesh_shader",
    "VK_NV_fragment_shader_barycentric",
    "VK_NV_shader_image_footprint",
    "VK_NV_scissor_exclusive",
    "VK_NV_device_diagnostic_checkpoints",
    "VK_INTEL_shader_integer_functions2",
    "VK_INTEL_performance_query",
    "VK_EXT_pci_bus_info",
    "VK_AMD_display_native_hdr",
    "VK_EXT_fragment_density_map",
    "VK_EXT_scalar_block_layout",
    "VK_GOOGLE_hlsl_functionality1",
    "VK_GOOGLE_decorate_string",
    "VK_EXT_subgroup_size_control",
    "VK_AMD_shader_core_properties2",
    "VK_AMD_device_coherent_memory",
    "VK_EXT_shader_image_atomic_int64",
    "VK_EXT_memory_budget",
    "VK_EXT_memory_priority",
    "VK_NV_dedicated_allocation_image_aliasing",
    "VK_EXT_buffer_device_address",
    "VK_EXT_tooling_info",
    "VK_EXT_separate_stencil_usage",
    "VK_NV_cooperative_matrix",
    "VK_NV_coverage_reduction_mode",
    "VK_EXT_fragment_shader_interlock",
    "VK_EXT_ycbcr_image_arrays",
    "VK_EXT_provoking_vertex",
#ifdef VK_USE_PLATFORM_WIN32_KHR
    "VK_EXT_full_screen_exclusive",
#endif // VK_USE_PLATFORM_WIN32_KHR
    "VK_EXT_line_rasterization",
    "VK_EXT_shader_atomic_float",
    "VK_EXT_host_query_reset",
    "VK_EXT_index_type_uint8",
    "VK_EXT_extended_dynamic_state",
    "VK_EXT_shader_atomic_float2",
    "VK_EXT_shader_demote_to_helper_invocation",
    "VK_NV_device_generated_commands",
    "VK_NV_inherited_viewport_scissor",
    "VK_EXT_texel_buffer_alignment",
    "VK_QCOM_render_pass_transform",
    "VK_EXT_device_memory_report",
    "VK_EXT_robustness2",
    "VK_EXT_custom_border_color",
    "VK_GOOGLE_user_type",
    "VK_EXT_private_data",
    "VK_EXT_pipeline_creation_cache_control",
    "VK_NV_device_diagnostics_config",
    "VK_QCOM_render_pass_store_ops",
    "VK_NV_fragment_shading_rate_enums",
    "VK_NV_ray_tracing_motion_blur",
    "VK_EXT_ycbcr_2plane_444_formats",
    "VK_EXT_fragment_density_map2",
    "VK_QCOM_rotated_copy_commands",
    "VK_EXT_image_robustness",
    "VK_EXT_4444_formats",
    "VK_ARM_rasterization_order_attachment_access",
    "VK_EXT_rgba10x6_formats",
#ifdef VK_USE_PLATFORM_WIN32_KHR
    "VK_NV_acquire_winrt_display",
#endif // VK_USE_PLATFORM_WIN32_KHR
    "VK_VALVE_mutable_descriptor_type",
    "VK_EXT_vertex_input_dynamic_state",
    "VK_EXT_physical_device_drm",
    "VK_EXT_depth_clip_control",
    "VK_EXT_primitive_topology_list_restart",
#ifdef VK_USE_PLATFORM_FUCHSIA
    "VK_FUCHSIA_external_memory",
#endif // VK_USE_PLATFORM_FUCHSIA
#ifdef VK_USE_PLATFORM_FUCHSIA
    "VK_FUCHSIA_external_semaphore",
#endif // VK_USE_PLATFORM_FUCHSIA
#ifdef VK_USE_PLATFORM_FUCHSIA
    "VK_FUCHSIA_buffer_collection",
#endif // VK_USE_PLATFORM_FUCHSIA
    "VK_HUAWEI_subpass_shading",
    "VK_HUAWEI_invocation_mask",
    "VK_NV_external_memory_rdma",
    "VK_EXT_extended_dynamic_state2",
    "VK_EXT_color_write_enable",
    "VK_EXT_global_priority_query",
    "VK_EXT_image_view_min_lod",
    "VK_EXT_multi_draw",
    "VK_EXT_load_store_op_none",
    "VK_EXT_border_color_swizzle",
    "VK_EXT_pageable_device_local_memory",
    "VK_VALVE_descriptor_set_host_mapping",
    "VK_QCOM_fragment_density_map_offset",
    "VK_NV_linear_color_attachment",
    "VK_KHR_acceleration_structure",
    "VK_KHR_ray_tracing_pipeline",
    "VK_KHR_ray_query",
};

void driver_initialize_instance_exts() {
    if (driver.instance_extensions_initialized) { return; }
    for(const auto& str : inst_ext_arr){
        driver.instance_extensions.push_back(str);
    }
    driver.instance_extensions_initialized = true;
}

void driver_initialize_device_exts() {
    if (driver.device_extensions_initialized) { return; }
    for(const auto& str : dev_ext_arr){
        driver.device_extensions.push_back(str);
    }
    driver.device_extensions_initialized = true;
}

void log_driver_message(const std::string& message) {
    static uint8_t cur_message_index = 0;
    static std::string messages[8];
    if (driver.debug_util_info.callback != nullptr) {
        VkDebugUtilsMessengerCallbackDataEXT callback_data{VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CALLBACK_DATA_EXT};
        messages[cur_message_index] = message.c_str();
        callback_data.pMessage = messages[cur_message_index].c_str();
        if (++cur_message_index >= 8) { cur_message_index = 0; }
        driver.debug_util_info.callback(
               VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
               VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,
               &callback_data,
               driver.debug_util_info.user_data);
    }
}

//// Instance Functions ////

VKAPI_ATTR VkResult VKAPI_CALL driver_EnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount,
                                                                           VkExtensionProperties* pProperties) {
    driver_initialize_instance_exts();

    VkResult res = VK_SUCCESS;
    if (pLayerName == nullptr) {
        if (pProperties != nullptr) {
            uint32_t count = static_cast<uint32_t>(driver.instance_extensions.size());
            if (*pPropertyCount < count) {
                count = *pPropertyCount;
                res = VK_INCOMPLETE;
            }
            for (uint32_t ext = 0; ext < count; ++ext) {
                pProperties[ext].specVersion = 1;
#if defined(_WIN32)
            strncpy_s(pProperties[ext].extensionName, VK_MAX_EXTENSION_NAME_SIZE, driver.instance_extensions[ext].c_str(), driver.instance_extensions[ext].size() + 1);
#else
            strncpy(pProperties[ext].extensionName, driver.instance_extensions[ext].c_str(), VK_MAX_EXTENSION_NAME_SIZE);
#endif
            }
        } else {
            *pPropertyCount = static_cast<uint32_t>(driver.instance_extensions.size());
        }
    }

    return res;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_EnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties) {
    *pPropertyCount = 0;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_EnumerateInstanceVersion(uint32_t* pApiVersion) {
    if (pApiVersion != nullptr) {
        *pApiVersion = VK_API_VERSION_1_3;
    }
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateInstance(const VkInstanceCreateInfo* pCreateInfo,
                                                     const VkAllocationCallbacks* pAllocator, VkInstance* pInstance) {
    if (pCreateInfo == nullptr || pCreateInfo->pApplicationInfo == nullptr) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    if (pCreateInfo->pApplicationInfo->apiVersion > VK_API_VERSION_1_3) {
        return VK_ERROR_INCOMPATIBLE_DRIVER;
    }

    driver_initialize_instance_exts();

    driver.enabled_instance_major = 1;
    driver.enabled_instance_minor = 0;
    if (pCreateInfo->pApplicationInfo != NULL && pCreateInfo->pApplicationInfo->apiVersion != 0) {
        driver.enabled_instance_minor = VK_API_VERSION_MINOR(pCreateInfo->pApplicationInfo->apiVersion);
    }

    for (uint32_t ext = 0; ext < pCreateInfo->enabledExtensionCount; ++ext) {
        driver.enabled_instance_extensions.push_back(pCreateInfo->ppEnabledExtensionNames[ext]);
    }

    // VK_SUCCESS
    *pInstance = driver.instance_handle.handle;

    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator) {}

VKAPI_ATTR VkResult VKAPI_CALL driver_EnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount,
                                                               VkPhysicalDevice* pPhysicalDevices) {
    log_driver_message("Generated Driver vkEnumeratePhysicalDevices");
    if (pPhysicalDevices != nullptr) {
        pPhysicalDevices[0] = driver.physical_device_handle.handle;
    }
    *pPhysicalDeviceCount = 1;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_EnumeratePhysicalDeviceGroups(
    VkInstance instance, uint32_t* pPhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties) {
    log_driver_message("Generated Driver vkEnumeratePhysicalDeviceGroups");

    if (pPhysicalDeviceGroupProperties != nullptr) {
        pPhysicalDeviceGroupProperties[0].subsetAllocation = false;
        pPhysicalDeviceGroupProperties[0].physicalDeviceCount = 1;
        pPhysicalDeviceGroupProperties[0].physicalDevices[0] = driver.physical_device_handle.handle;
    }
    *pPhysicalDeviceGroupCount = 1;
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice,
                                                              VkPhysicalDeviceProperties* pProperties) {
    if (nullptr != pProperties) {
        pProperties->apiVersion = VK_API_VERSION_1_3;
        pProperties->driverVersion = VK_API_VERSION_1_3;
        pProperties->vendorID = 0xFEEDF00D;
        pProperties->deviceID = 1;
        pProperties->deviceType = VK_PHYSICAL_DEVICE_TYPE_CPU;
#if defined(_WIN32)
        strncpy_s(pProperties->deviceName, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE, driver_phys_device_name, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);
#else
        strncpy(pProperties->deviceName, driver_phys_device_name, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);
#endif
    }
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceProperties2(VkPhysicalDevice physicalDevice,
                                                               VkPhysicalDeviceProperties2* pProperties) {
    if (nullptr != pProperties) {
        driver_GetPhysicalDeviceProperties(physicalDevice, &pProperties->properties);
        VkBaseInStructure* pNext = reinterpret_cast<VkBaseInStructure*>(pProperties->pNext);
        while (pNext) {
            if (pNext->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT) {
                VkPhysicalDevicePCIBusInfoPropertiesEXT* bus_info =
                    reinterpret_cast<VkPhysicalDevicePCIBusInfoPropertiesEXT*>(pNext);
                bus_info->pciBus = rand() % 5;
            }
            pNext = reinterpret_cast<VkBaseInStructure*>(const_cast<VkBaseInStructure*>(pNext->pNext));
        }
    }
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                                   const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                                   const VkAllocationCallbacks* pAllocator,
                                                                   VkDebugUtilsMessengerEXT* pMessenger) {
    *pMessenger = reinterpret_cast<VkDebugUtilsMessengerEXT>(0xdeadbeefdeadbeef);
    driver.debug_util_info.severities = pCreateInfo->messageSeverity;
    driver.debug_util_info.types = pCreateInfo->messageType;
    driver.debug_util_info.callback = pCreateInfo->pfnUserCallback;
    driver.debug_util_info.user_data = pCreateInfo->pUserData;
    return VK_SUCCESS;
}


//// Physical Device functions ////

VKAPI_ATTR VkResult VKAPI_CALL driver_EnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount,
                                                                     VkLayerProperties* pProperties) {
    log_driver_message("Generated Driver vkEnumerateDeviceLayerProperties");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_EnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char* pLayerName,
                                                                         uint32_t* pPropertyCount,
                                                                         VkExtensionProperties* pProperties) {
    log_driver_message("Generated Driver vkEnumerateDeviceExtensionProperties");
    driver_initialize_device_exts();

    VkResult res = VK_SUCCESS;
    if (pLayerName == nullptr) {
        if (pProperties != nullptr) {
            uint32_t count = static_cast<uint32_t>(driver.device_extensions.size());
            if (*pPropertyCount < count) {
                count = *pPropertyCount;
                res = VK_INCOMPLETE;
            }
            for (uint32_t ext = 0; ext < count; ++ext) {
                pProperties[ext].specVersion = 1;
#if defined(_WIN32)
            strncpy_s(pProperties[ext].extensionName, VK_MAX_EXTENSION_NAME_SIZE, driver.device_extensions[ext].c_str(), driver.device_extensions[ext].size() + 1);
#else
            strncpy(pProperties[ext].extensionName, driver.device_extensions[ext].c_str(), VK_MAX_EXTENSION_NAME_SIZE);
#endif
            }
        } else {
            *pPropertyCount = static_cast<uint32_t>(driver.device_extensions.size());
        }
    }

    return res;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo,
                                                   const VkAllocationCallbacks* pAllocator, VkDevice* pDevice) {
    log_driver_message("Generated Driver vkCreateDevice");
    driver_initialize_device_exts();

    DispatchableHandle<VkDevice>* temp_handle = new DispatchableHandle<VkDevice>();
    driver.dev_handles.push_back(temp_handle);
    *pDevice = temp_handle->handle;

    for (uint32_t ext = 0; ext < pCreateInfo->enabledExtensionCount; ++ext) {
        driver.enabled_device_extensions.push_back(pCreateInfo->ppEnabledExtensionNames[ext]);
    }

    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyDevice");
    for (uint32_t ii = 0; ii < static_cast<uint32_t>(driver.dev_handles.size()); ++ii) {
        if (driver.dev_handles[ii]->handle == device) {
            delete driver.dev_handles[ii];
            driver.dev_handles.erase(driver.dev_handles.begin() + ii);
        }
    }
    if (driver.dev_handles.size() == 0) {
        for (uint32_t ii = 0; ii < static_cast<uint32_t>(driver.queue_handles.size()); ++ii) {
            delete driver.queue_handles[ii];
        }
        driver.queue_handles.clear();
        for (uint32_t ii = 0; ii < static_cast<uint32_t>(driver.commandbuffer_handles.size()); ++ii) {
            delete driver.commandbuffer_handles[ii];
        }
        driver.commandbuffer_handles.clear();
    }
}

// ----- VK_VERSION_1_0
VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceFeatures(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceFeatures* pFeatures) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceFeatures");
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceFormatProperties(
    VkPhysicalDevice physicalDevice,
    VkFormat format,
    VkFormatProperties* pFormatProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceFormatProperties");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceImageFormatProperties(
    VkPhysicalDevice physicalDevice,
    VkFormat format,
    VkImageType type,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkImageCreateFlags flags,
    VkImageFormatProperties* pImageFormatProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceImageFormatProperties");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice physicalDevice,
    uint32_t* pQueueFamilyPropertyCount,
    VkQueueFamilyProperties* pQueueFamilyProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceQueueFamilyProperties");
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceMemoryProperties(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceMemoryProperties* pMemoryProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceMemoryProperties");
}

VKAPI_ATTR void VKAPI_CALL driver_GetDeviceQueue(
    VkDevice device,
    uint32_t queueFamilyIndex,
    uint32_t queueIndex,
    VkQueue* pQueue) {
    log_driver_message("Generated Driver vkGetDeviceQueue");
    DispatchableHandle<VkQueue>* temp_handle = new DispatchableHandle<VkQueue>();
    driver.queue_handles.push_back(temp_handle);
    *pQueue = temp_handle->handle;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_QueueSubmit(
    VkQueue queue,
    uint32_t submitCount,
    const VkSubmitInfo* pSubmits,
    VkFence fence) {
    log_driver_message("Generated Driver vkQueueSubmit");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_QueueWaitIdle(
    VkQueue queue) {
    log_driver_message("Generated Driver vkQueueWaitIdle");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_DeviceWaitIdle(
    VkDevice device) {
    log_driver_message("Generated Driver vkDeviceWaitIdle");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_AllocateMemory(
    VkDevice device,
    const VkMemoryAllocateInfo* pAllocateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDeviceMemory* pMemory) {
    log_driver_message("Generated Driver vkAllocateMemory");
    *pMemory = reinterpret_cast<VkDeviceMemory>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_FreeMemory(
    VkDevice device,
    VkDeviceMemory memory,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkFreeMemory");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_MapMemory(
    VkDevice device,
    VkDeviceMemory memory,
    VkDeviceSize offset,
    VkDeviceSize size,
    VkMemoryMapFlags flags,
    void** ppData) {
    log_driver_message("Generated Driver vkMapMemory");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_UnmapMemory(
    VkDevice device,
    VkDeviceMemory memory) {
    log_driver_message("Generated Driver vkUnmapMemory");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_FlushMappedMemoryRanges(
    VkDevice device,
    uint32_t memoryRangeCount,
    const VkMappedMemoryRange* pMemoryRanges) {
    log_driver_message("Generated Driver vkFlushMappedMemoryRanges");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_InvalidateMappedMemoryRanges(
    VkDevice device,
    uint32_t memoryRangeCount,
    const VkMappedMemoryRange* pMemoryRanges) {
    log_driver_message("Generated Driver vkInvalidateMappedMemoryRanges");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_GetDeviceMemoryCommitment(
    VkDevice device,
    VkDeviceMemory memory,
    VkDeviceSize* pCommittedMemoryInBytes) {
    log_driver_message("Generated Driver vkGetDeviceMemoryCommitment");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_BindBufferMemory(
    VkDevice device,
    VkBuffer buffer,
    VkDeviceMemory memory,
    VkDeviceSize memoryOffset) {
    log_driver_message("Generated Driver vkBindBufferMemory");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_BindImageMemory(
    VkDevice device,
    VkImage image,
    VkDeviceMemory memory,
    VkDeviceSize memoryOffset) {
    log_driver_message("Generated Driver vkBindImageMemory");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_GetBufferMemoryRequirements(
    VkDevice device,
    VkBuffer buffer,
    VkMemoryRequirements* pMemoryRequirements) {
    log_driver_message("Generated Driver vkGetBufferMemoryRequirements");
}

VKAPI_ATTR void VKAPI_CALL driver_GetImageMemoryRequirements(
    VkDevice device,
    VkImage image,
    VkMemoryRequirements* pMemoryRequirements) {
    log_driver_message("Generated Driver vkGetImageMemoryRequirements");
}

VKAPI_ATTR void VKAPI_CALL driver_GetImageSparseMemoryRequirements(
    VkDevice device,
    VkImage image,
    uint32_t* pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements* pSparseMemoryRequirements) {
    log_driver_message("Generated Driver vkGetImageSparseMemoryRequirements");
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceSparseImageFormatProperties(
    VkPhysicalDevice physicalDevice,
    VkFormat format,
    VkImageType type,
    VkSampleCountFlagBits samples,
    VkImageUsageFlags usage,
    VkImageTiling tiling,
    uint32_t* pPropertyCount,
    VkSparseImageFormatProperties* pProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceSparseImageFormatProperties");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_QueueBindSparse(
    VkQueue queue,
    uint32_t bindInfoCount,
    const VkBindSparseInfo* pBindInfo,
    VkFence fence) {
    log_driver_message("Generated Driver vkQueueBindSparse");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateFence(
    VkDevice device,
    const VkFenceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFence* pFence) {
    log_driver_message("Generated Driver vkCreateFence");
    *pFence = reinterpret_cast<VkFence>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyFence(
    VkDevice device,
    VkFence fence,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyFence");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_ResetFences(
    VkDevice device,
    uint32_t fenceCount,
    const VkFence* pFences) {
    log_driver_message("Generated Driver vkResetFences");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetFenceStatus(
    VkDevice device,
    VkFence fence) {
    log_driver_message("Generated Driver vkGetFenceStatus");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_WaitForFences(
    VkDevice device,
    uint32_t fenceCount,
    const VkFence* pFences,
    VkBool32 waitAll,
    uint64_t timeout) {
    log_driver_message("Generated Driver vkWaitForFences");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateSemaphore(
    VkDevice device,
    const VkSemaphoreCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSemaphore* pSemaphore) {
    log_driver_message("Generated Driver vkCreateSemaphore");
    *pSemaphore = reinterpret_cast<VkSemaphore>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroySemaphore(
    VkDevice device,
    VkSemaphore semaphore,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroySemaphore");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateEvent(
    VkDevice device,
    const VkEventCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkEvent* pEvent) {
    log_driver_message("Generated Driver vkCreateEvent");
    *pEvent = reinterpret_cast<VkEvent>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyEvent(
    VkDevice device,
    VkEvent event,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyEvent");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetEventStatus(
    VkDevice device,
    VkEvent event) {
    log_driver_message("Generated Driver vkGetEventStatus");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_SetEvent(
    VkDevice device,
    VkEvent event) {
    log_driver_message("Generated Driver vkSetEvent");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_ResetEvent(
    VkDevice device,
    VkEvent event) {
    log_driver_message("Generated Driver vkResetEvent");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateQueryPool(
    VkDevice device,
    const VkQueryPoolCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkQueryPool* pQueryPool) {
    log_driver_message("Generated Driver vkCreateQueryPool");
    *pQueryPool = reinterpret_cast<VkQueryPool>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyQueryPool(
    VkDevice device,
    VkQueryPool queryPool,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyQueryPool");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetQueryPoolResults(
    VkDevice device,
    VkQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount,
    size_t dataSize,
    void* pData,
    VkDeviceSize stride,
    VkQueryResultFlags flags) {
    log_driver_message("Generated Driver vkGetQueryPoolResults");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateBuffer(
    VkDevice device,
    const VkBufferCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkBuffer* pBuffer) {
    log_driver_message("Generated Driver vkCreateBuffer");
    *pBuffer = reinterpret_cast<VkBuffer>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyBuffer(
    VkDevice device,
    VkBuffer buffer,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyBuffer");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateBufferView(
    VkDevice device,
    const VkBufferViewCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkBufferView* pView) {
    log_driver_message("Generated Driver vkCreateBufferView");
    *pView = reinterpret_cast<VkBufferView>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyBufferView(
    VkDevice device,
    VkBufferView bufferView,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyBufferView");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateImage(
    VkDevice device,
    const VkImageCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkImage* pImage) {
    log_driver_message("Generated Driver vkCreateImage");
    *pImage = reinterpret_cast<VkImage>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyImage(
    VkDevice device,
    VkImage image,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyImage");
}

VKAPI_ATTR void VKAPI_CALL driver_GetImageSubresourceLayout(
    VkDevice device,
    VkImage image,
    const VkImageSubresource* pSubresource,
    VkSubresourceLayout* pLayout) {
    log_driver_message("Generated Driver vkGetImageSubresourceLayout");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateImageView(
    VkDevice device,
    const VkImageViewCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkImageView* pView) {
    log_driver_message("Generated Driver vkCreateImageView");
    *pView = reinterpret_cast<VkImageView>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyImageView(
    VkDevice device,
    VkImageView imageView,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyImageView");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateShaderModule(
    VkDevice device,
    const VkShaderModuleCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkShaderModule* pShaderModule) {
    log_driver_message("Generated Driver vkCreateShaderModule");
    *pShaderModule = reinterpret_cast<VkShaderModule>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyShaderModule(
    VkDevice device,
    VkShaderModule shaderModule,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyShaderModule");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreatePipelineCache(
    VkDevice device,
    const VkPipelineCacheCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkPipelineCache* pPipelineCache) {
    log_driver_message("Generated Driver vkCreatePipelineCache");
    *pPipelineCache = reinterpret_cast<VkPipelineCache>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyPipelineCache(
    VkDevice device,
    VkPipelineCache pipelineCache,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyPipelineCache");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetPipelineCacheData(
    VkDevice device,
    VkPipelineCache pipelineCache,
    size_t* pDataSize,
    void* pData) {
    log_driver_message("Generated Driver vkGetPipelineCacheData");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_MergePipelineCaches(
    VkDevice device,
    VkPipelineCache dstCache,
    uint32_t srcCacheCount,
    const VkPipelineCache* pSrcCaches) {
    log_driver_message("Generated Driver vkMergePipelineCaches");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateGraphicsPipelines(
    VkDevice device,
    VkPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const VkGraphicsPipelineCreateInfo* pCreateInfos,
    const VkAllocationCallbacks* pAllocator,
    VkPipeline* pPipelines) {
    log_driver_message("Generated Driver vkCreateGraphicsPipelines");
    *pPipelines = reinterpret_cast<VkPipeline>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateComputePipelines(
    VkDevice device,
    VkPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const VkComputePipelineCreateInfo* pCreateInfos,
    const VkAllocationCallbacks* pAllocator,
    VkPipeline* pPipelines) {
    log_driver_message("Generated Driver vkCreateComputePipelines");
    *pPipelines = reinterpret_cast<VkPipeline>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyPipeline(
    VkDevice device,
    VkPipeline pipeline,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyPipeline");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreatePipelineLayout(
    VkDevice device,
    const VkPipelineLayoutCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkPipelineLayout* pPipelineLayout) {
    log_driver_message("Generated Driver vkCreatePipelineLayout");
    *pPipelineLayout = reinterpret_cast<VkPipelineLayout>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyPipelineLayout(
    VkDevice device,
    VkPipelineLayout pipelineLayout,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyPipelineLayout");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateSampler(
    VkDevice device,
    const VkSamplerCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSampler* pSampler) {
    log_driver_message("Generated Driver vkCreateSampler");
    *pSampler = reinterpret_cast<VkSampler>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroySampler(
    VkDevice device,
    VkSampler sampler,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroySampler");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateDescriptorSetLayout(
    VkDevice device,
    const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDescriptorSetLayout* pSetLayout) {
    log_driver_message("Generated Driver vkCreateDescriptorSetLayout");
    *pSetLayout = reinterpret_cast<VkDescriptorSetLayout>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyDescriptorSetLayout(
    VkDevice device,
    VkDescriptorSetLayout descriptorSetLayout,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyDescriptorSetLayout");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateDescriptorPool(
    VkDevice device,
    const VkDescriptorPoolCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDescriptorPool* pDescriptorPool) {
    log_driver_message("Generated Driver vkCreateDescriptorPool");
    *pDescriptorPool = reinterpret_cast<VkDescriptorPool>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyDescriptorPool(
    VkDevice device,
    VkDescriptorPool descriptorPool,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyDescriptorPool");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_ResetDescriptorPool(
    VkDevice device,
    VkDescriptorPool descriptorPool,
    VkDescriptorPoolResetFlags flags) {
    log_driver_message("Generated Driver vkResetDescriptorPool");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_AllocateDescriptorSets(
    VkDevice device,
    const VkDescriptorSetAllocateInfo* pAllocateInfo,
    VkDescriptorSet* pDescriptorSets) {
    log_driver_message("Generated Driver vkAllocateDescriptorSets");
    *pDescriptorSets = reinterpret_cast<VkDescriptorSet>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_FreeDescriptorSets(
    VkDevice device,
    VkDescriptorPool descriptorPool,
    uint32_t descriptorSetCount,
    const VkDescriptorSet* pDescriptorSets) {
    log_driver_message("Generated Driver vkFreeDescriptorSets");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_UpdateDescriptorSets(
    VkDevice device,
    uint32_t descriptorWriteCount,
    const VkWriteDescriptorSet* pDescriptorWrites,
    uint32_t descriptorCopyCount,
    const VkCopyDescriptorSet* pDescriptorCopies) {
    log_driver_message("Generated Driver vkUpdateDescriptorSets");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateFramebuffer(
    VkDevice device,
    const VkFramebufferCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFramebuffer* pFramebuffer) {
    log_driver_message("Generated Driver vkCreateFramebuffer");
    *pFramebuffer = reinterpret_cast<VkFramebuffer>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyFramebuffer(
    VkDevice device,
    VkFramebuffer framebuffer,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyFramebuffer");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateRenderPass(
    VkDevice device,
    const VkRenderPassCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkRenderPass* pRenderPass) {
    log_driver_message("Generated Driver vkCreateRenderPass");
    *pRenderPass = reinterpret_cast<VkRenderPass>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyRenderPass(
    VkDevice device,
    VkRenderPass renderPass,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyRenderPass");
}

VKAPI_ATTR void VKAPI_CALL driver_GetRenderAreaGranularity(
    VkDevice device,
    VkRenderPass renderPass,
    VkExtent2D* pGranularity) {
    log_driver_message("Generated Driver vkGetRenderAreaGranularity");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateCommandPool(
    VkDevice device,
    const VkCommandPoolCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkCommandPool* pCommandPool) {
    log_driver_message("Generated Driver vkCreateCommandPool");
    *pCommandPool = reinterpret_cast<VkCommandPool>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyCommandPool(
    VkDevice device,
    VkCommandPool commandPool,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyCommandPool");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_ResetCommandPool(
    VkDevice device,
    VkCommandPool commandPool,
    VkCommandPoolResetFlags flags) {
    log_driver_message("Generated Driver vkResetCommandPool");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_AllocateCommandBuffers(
    VkDevice device,
    const VkCommandBufferAllocateInfo* pAllocateInfo,
    VkCommandBuffer* pCommandBuffers) {
    log_driver_message("Generated Driver vkAllocateCommandBuffers");
    DispatchableHandle<VkCommandBuffer>* temp_handle = new DispatchableHandle<VkCommandBuffer>();
    driver.commandbuffer_handles.push_back(temp_handle);
    *pCommandBuffers = temp_handle->handle;
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_FreeCommandBuffers(
    VkDevice device,
    VkCommandPool commandPool,
    uint32_t commandBufferCount,
    const VkCommandBuffer* pCommandBuffers) {
    log_driver_message("Generated Driver vkFreeCommandBuffers");
    for (uint32_t ii = 0; ii < static_cast<uint32_t>(driver.commandbuffer_handles.size()); ++ii) {
        if (driver.commandbuffer_handles[ii]->handle == *pCommandBuffers) {
            delete driver.commandbuffer_handles[ii];
            driver.commandbuffer_handles.erase(driver.commandbuffer_handles.begin() + ii);
        }
    }
}

VKAPI_ATTR VkResult VKAPI_CALL driver_BeginCommandBuffer(
    VkCommandBuffer commandBuffer,
    const VkCommandBufferBeginInfo* pBeginInfo) {
    log_driver_message("Generated Driver vkBeginCommandBuffer");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_EndCommandBuffer(
    VkCommandBuffer commandBuffer) {
    log_driver_message("Generated Driver vkEndCommandBuffer");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_ResetCommandBuffer(
    VkCommandBuffer commandBuffer,
    VkCommandBufferResetFlags flags) {
    log_driver_message("Generated Driver vkResetCommandBuffer");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBindPipeline(
    VkCommandBuffer commandBuffer,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipeline pipeline) {
    log_driver_message("Generated Driver vkCmdBindPipeline");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetViewport(
    VkCommandBuffer commandBuffer,
    uint32_t firstViewport,
    uint32_t viewportCount,
    const VkViewport* pViewports) {
    log_driver_message("Generated Driver vkCmdSetViewport");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetScissor(
    VkCommandBuffer commandBuffer,
    uint32_t firstScissor,
    uint32_t scissorCount,
    const VkRect2D* pScissors) {
    log_driver_message("Generated Driver vkCmdSetScissor");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetLineWidth(
    VkCommandBuffer commandBuffer,
    float lineWidth) {
    log_driver_message("Generated Driver vkCmdSetLineWidth");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetDepthBias(
    VkCommandBuffer commandBuffer,
    float depthBiasConstantFactor,
    float depthBiasClamp,
    float depthBiasSlopeFactor) {
    log_driver_message("Generated Driver vkCmdSetDepthBias");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetBlendConstants(
    VkCommandBuffer commandBuffer,
    const float blendConstants[4]) {
    log_driver_message("Generated Driver vkCmdSetBlendConstants");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetDepthBounds(
    VkCommandBuffer commandBuffer,
    float minDepthBounds,
    float maxDepthBounds) {
    log_driver_message("Generated Driver vkCmdSetDepthBounds");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetStencilCompareMask(
    VkCommandBuffer commandBuffer,
    VkStencilFaceFlags faceMask,
    uint32_t compareMask) {
    log_driver_message("Generated Driver vkCmdSetStencilCompareMask");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetStencilWriteMask(
    VkCommandBuffer commandBuffer,
    VkStencilFaceFlags faceMask,
    uint32_t writeMask) {
    log_driver_message("Generated Driver vkCmdSetStencilWriteMask");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetStencilReference(
    VkCommandBuffer commandBuffer,
    VkStencilFaceFlags faceMask,
    uint32_t reference) {
    log_driver_message("Generated Driver vkCmdSetStencilReference");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBindDescriptorSets(
    VkCommandBuffer commandBuffer,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipelineLayout layout,
    uint32_t firstSet,
    uint32_t descriptorSetCount,
    const VkDescriptorSet* pDescriptorSets,
    uint32_t dynamicOffsetCount,
    const uint32_t* pDynamicOffsets) {
    log_driver_message("Generated Driver vkCmdBindDescriptorSets");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBindIndexBuffer(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    VkIndexType indexType) {
    log_driver_message("Generated Driver vkCmdBindIndexBuffer");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBindVertexBuffers(
    VkCommandBuffer commandBuffer,
    uint32_t firstBinding,
    uint32_t bindingCount,
    const VkBuffer* pBuffers,
    const VkDeviceSize* pOffsets) {
    log_driver_message("Generated Driver vkCmdBindVertexBuffers");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdDraw(
    VkCommandBuffer commandBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance) {
    log_driver_message("Generated Driver vkCmdDraw");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdDrawIndexed(
    VkCommandBuffer commandBuffer,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance) {
    log_driver_message("Generated Driver vkCmdDrawIndexed");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdDrawIndirect(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride) {
    log_driver_message("Generated Driver vkCmdDrawIndirect");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdDrawIndexedIndirect(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride) {
    log_driver_message("Generated Driver vkCmdDrawIndexedIndirect");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdDispatch(
    VkCommandBuffer commandBuffer,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ) {
    log_driver_message("Generated Driver vkCmdDispatch");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdDispatchIndirect(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset) {
    log_driver_message("Generated Driver vkCmdDispatchIndirect");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdCopyBuffer(
    VkCommandBuffer commandBuffer,
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    uint32_t regionCount,
    const VkBufferCopy* pRegions) {
    log_driver_message("Generated Driver vkCmdCopyBuffer");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdCopyImage(
    VkCommandBuffer commandBuffer,
    VkImage srcImage,
    VkImageLayout srcImageLayout,
    VkImage dstImage,
    VkImageLayout dstImageLayout,
    uint32_t regionCount,
    const VkImageCopy* pRegions) {
    log_driver_message("Generated Driver vkCmdCopyImage");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBlitImage(
    VkCommandBuffer commandBuffer,
    VkImage srcImage,
    VkImageLayout srcImageLayout,
    VkImage dstImage,
    VkImageLayout dstImageLayout,
    uint32_t regionCount,
    const VkImageBlit* pRegions,
    VkFilter filter) {
    log_driver_message("Generated Driver vkCmdBlitImage");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdCopyBufferToImage(
    VkCommandBuffer commandBuffer,
    VkBuffer srcBuffer,
    VkImage dstImage,
    VkImageLayout dstImageLayout,
    uint32_t regionCount,
    const VkBufferImageCopy* pRegions) {
    log_driver_message("Generated Driver vkCmdCopyBufferToImage");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdCopyImageToBuffer(
    VkCommandBuffer commandBuffer,
    VkImage srcImage,
    VkImageLayout srcImageLayout,
    VkBuffer dstBuffer,
    uint32_t regionCount,
    const VkBufferImageCopy* pRegions) {
    log_driver_message("Generated Driver vkCmdCopyImageToBuffer");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdUpdateBuffer(
    VkCommandBuffer commandBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize dstOffset,
    VkDeviceSize dataSize,
    const void* pData) {
    log_driver_message("Generated Driver vkCmdUpdateBuffer");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdFillBuffer(
    VkCommandBuffer commandBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize dstOffset,
    VkDeviceSize size,
    uint32_t data) {
    log_driver_message("Generated Driver vkCmdFillBuffer");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdClearColorImage(
    VkCommandBuffer commandBuffer,
    VkImage image,
    VkImageLayout imageLayout,
    const VkClearColorValue* pColor,
    uint32_t rangeCount,
    const VkImageSubresourceRange* pRanges) {
    log_driver_message("Generated Driver vkCmdClearColorImage");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdClearDepthStencilImage(
    VkCommandBuffer commandBuffer,
    VkImage image,
    VkImageLayout imageLayout,
    const VkClearDepthStencilValue* pDepthStencil,
    uint32_t rangeCount,
    const VkImageSubresourceRange* pRanges) {
    log_driver_message("Generated Driver vkCmdClearDepthStencilImage");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdClearAttachments(
    VkCommandBuffer commandBuffer,
    uint32_t attachmentCount,
    const VkClearAttachment* pAttachments,
    uint32_t rectCount,
    const VkClearRect* pRects) {
    log_driver_message("Generated Driver vkCmdClearAttachments");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdResolveImage(
    VkCommandBuffer commandBuffer,
    VkImage srcImage,
    VkImageLayout srcImageLayout,
    VkImage dstImage,
    VkImageLayout dstImageLayout,
    uint32_t regionCount,
    const VkImageResolve* pRegions) {
    log_driver_message("Generated Driver vkCmdResolveImage");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetEvent(
    VkCommandBuffer commandBuffer,
    VkEvent event,
    VkPipelineStageFlags stageMask) {
    log_driver_message("Generated Driver vkCmdSetEvent");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdResetEvent(
    VkCommandBuffer commandBuffer,
    VkEvent event,
    VkPipelineStageFlags stageMask) {
    log_driver_message("Generated Driver vkCmdResetEvent");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdWaitEvents(
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
    log_driver_message("Generated Driver vkCmdWaitEvents");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdPipelineBarrier(
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
    log_driver_message("Generated Driver vkCmdPipelineBarrier");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBeginQuery(
    VkCommandBuffer commandBuffer,
    VkQueryPool queryPool,
    uint32_t query,
    VkQueryControlFlags flags) {
    log_driver_message("Generated Driver vkCmdBeginQuery");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdEndQuery(
    VkCommandBuffer commandBuffer,
    VkQueryPool queryPool,
    uint32_t query) {
    log_driver_message("Generated Driver vkCmdEndQuery");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdResetQueryPool(
    VkCommandBuffer commandBuffer,
    VkQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount) {
    log_driver_message("Generated Driver vkCmdResetQueryPool");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdWriteTimestamp(
    VkCommandBuffer commandBuffer,
    VkPipelineStageFlagBits pipelineStage,
    VkQueryPool queryPool,
    uint32_t query) {
    log_driver_message("Generated Driver vkCmdWriteTimestamp");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdCopyQueryPoolResults(
    VkCommandBuffer commandBuffer,
    VkQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount,
    VkBuffer dstBuffer,
    VkDeviceSize dstOffset,
    VkDeviceSize stride,
    VkQueryResultFlags flags) {
    log_driver_message("Generated Driver vkCmdCopyQueryPoolResults");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdPushConstants(
    VkCommandBuffer commandBuffer,
    VkPipelineLayout layout,
    VkShaderStageFlags stageFlags,
    uint32_t offset,
    uint32_t size,
    const void* pValues) {
    log_driver_message("Generated Driver vkCmdPushConstants");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBeginRenderPass(
    VkCommandBuffer commandBuffer,
    const VkRenderPassBeginInfo* pRenderPassBegin,
    VkSubpassContents contents) {
    log_driver_message("Generated Driver vkCmdBeginRenderPass");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdNextSubpass(
    VkCommandBuffer commandBuffer,
    VkSubpassContents contents) {
    log_driver_message("Generated Driver vkCmdNextSubpass");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdEndRenderPass(
    VkCommandBuffer commandBuffer) {
    log_driver_message("Generated Driver vkCmdEndRenderPass");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdExecuteCommands(
    VkCommandBuffer commandBuffer,
    uint32_t commandBufferCount,
    const VkCommandBuffer* pCommandBuffers) {
    log_driver_message("Generated Driver vkCmdExecuteCommands");
}


// ----- VK_VERSION_1_1
VKAPI_ATTR VkResult VKAPI_CALL driver_BindBufferMemory2(
    VkDevice device,
    uint32_t bindInfoCount,
    const VkBindBufferMemoryInfo* pBindInfos) {
    log_driver_message("Generated Driver vkBindBufferMemory2");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_BindImageMemory2(
    VkDevice device,
    uint32_t bindInfoCount,
    const VkBindImageMemoryInfo* pBindInfos) {
    log_driver_message("Generated Driver vkBindImageMemory2");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_GetDeviceGroupPeerMemoryFeatures(
    VkDevice device,
    uint32_t heapIndex,
    uint32_t localDeviceIndex,
    uint32_t remoteDeviceIndex,
    VkPeerMemoryFeatureFlags* pPeerMemoryFeatures) {
    log_driver_message("Generated Driver vkGetDeviceGroupPeerMemoryFeatures");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetDeviceMask(
    VkCommandBuffer commandBuffer,
    uint32_t deviceMask) {
    log_driver_message("Generated Driver vkCmdSetDeviceMask");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdDispatchBase(
    VkCommandBuffer commandBuffer,
    uint32_t baseGroupX,
    uint32_t baseGroupY,
    uint32_t baseGroupZ,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ) {
    log_driver_message("Generated Driver vkCmdDispatchBase");
}

VKAPI_ATTR void VKAPI_CALL driver_GetImageMemoryRequirements2(
    VkDevice device,
    const VkImageMemoryRequirementsInfo2* pInfo,
    VkMemoryRequirements2* pMemoryRequirements) {
    log_driver_message("Generated Driver vkGetImageMemoryRequirements2");
}

VKAPI_ATTR void VKAPI_CALL driver_GetBufferMemoryRequirements2(
    VkDevice device,
    const VkBufferMemoryRequirementsInfo2* pInfo,
    VkMemoryRequirements2* pMemoryRequirements) {
    log_driver_message("Generated Driver vkGetBufferMemoryRequirements2");
}

VKAPI_ATTR void VKAPI_CALL driver_GetImageSparseMemoryRequirements2(
    VkDevice device,
    const VkImageSparseMemoryRequirementsInfo2* pInfo,
    uint32_t* pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2* pSparseMemoryRequirements) {
    log_driver_message("Generated Driver vkGetImageSparseMemoryRequirements2");
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceFeatures2(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceFeatures2* pFeatures) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceFeatures2");
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceFormatProperties2(
    VkPhysicalDevice physicalDevice,
    VkFormat format,
    VkFormatProperties2* pFormatProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceFormatProperties2");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceImageFormatProperties2(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo,
    VkImageFormatProperties2* pImageFormatProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceImageFormatProperties2");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceQueueFamilyProperties2(
    VkPhysicalDevice physicalDevice,
    uint32_t* pQueueFamilyPropertyCount,
    VkQueueFamilyProperties2* pQueueFamilyProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceQueueFamilyProperties2");
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceMemoryProperties2(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceMemoryProperties2* pMemoryProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceMemoryProperties2");
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceSparseImageFormatProperties2(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo,
    uint32_t* pPropertyCount,
    VkSparseImageFormatProperties2* pProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceSparseImageFormatProperties2");
}

VKAPI_ATTR void VKAPI_CALL driver_TrimCommandPool(
    VkDevice device,
    VkCommandPool commandPool,
    VkCommandPoolTrimFlags flags) {
    log_driver_message("Generated Driver vkTrimCommandPool");
}

VKAPI_ATTR void VKAPI_CALL driver_GetDeviceQueue2(
    VkDevice device,
    const VkDeviceQueueInfo2* pQueueInfo,
    VkQueue* pQueue) {
    log_driver_message("Generated Driver vkGetDeviceQueue2");
    DispatchableHandle<VkQueue>* temp_handle = new DispatchableHandle<VkQueue>();
    driver.queue_handles.push_back(temp_handle);
    *pQueue = temp_handle->handle;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateSamplerYcbcrConversion(
    VkDevice device,
    const VkSamplerYcbcrConversionCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSamplerYcbcrConversion* pYcbcrConversion) {
    log_driver_message("Generated Driver vkCreateSamplerYcbcrConversion");
    *pYcbcrConversion = reinterpret_cast<VkSamplerYcbcrConversion>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroySamplerYcbcrConversion(
    VkDevice device,
    VkSamplerYcbcrConversion ycbcrConversion,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroySamplerYcbcrConversion");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateDescriptorUpdateTemplate(
    VkDevice device,
    const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDescriptorUpdateTemplate* pDescriptorUpdateTemplate) {
    log_driver_message("Generated Driver vkCreateDescriptorUpdateTemplate");
    *pDescriptorUpdateTemplate = reinterpret_cast<VkDescriptorUpdateTemplate>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyDescriptorUpdateTemplate(
    VkDevice device,
    VkDescriptorUpdateTemplate descriptorUpdateTemplate,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyDescriptorUpdateTemplate");
}

VKAPI_ATTR void VKAPI_CALL driver_UpdateDescriptorSetWithTemplate(
    VkDevice device,
    VkDescriptorSet descriptorSet,
    VkDescriptorUpdateTemplate descriptorUpdateTemplate,
    const void* pData) {
    log_driver_message("Generated Driver vkUpdateDescriptorSetWithTemplate");
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceExternalBufferProperties(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo,
    VkExternalBufferProperties* pExternalBufferProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceExternalBufferProperties");
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceExternalFenceProperties(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo,
    VkExternalFenceProperties* pExternalFenceProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceExternalFenceProperties");
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceExternalSemaphoreProperties(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo,
    VkExternalSemaphoreProperties* pExternalSemaphoreProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceExternalSemaphoreProperties");
}

VKAPI_ATTR void VKAPI_CALL driver_GetDescriptorSetLayoutSupport(
    VkDevice device,
    const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
    VkDescriptorSetLayoutSupport* pSupport) {
    log_driver_message("Generated Driver vkGetDescriptorSetLayoutSupport");
}


// ----- VK_VERSION_1_2
VKAPI_ATTR void VKAPI_CALL driver_CmdDrawIndirectCount(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    VkBuffer countBuffer,
    VkDeviceSize countBufferOffset,
    uint32_t maxDrawCount,
    uint32_t stride) {
    log_driver_message("Generated Driver vkCmdDrawIndirectCount");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdDrawIndexedIndirectCount(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    VkBuffer countBuffer,
    VkDeviceSize countBufferOffset,
    uint32_t maxDrawCount,
    uint32_t stride) {
    log_driver_message("Generated Driver vkCmdDrawIndexedIndirectCount");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateRenderPass2(
    VkDevice device,
    const VkRenderPassCreateInfo2* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkRenderPass* pRenderPass) {
    log_driver_message("Generated Driver vkCreateRenderPass2");
    *pRenderPass = reinterpret_cast<VkRenderPass>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBeginRenderPass2(
    VkCommandBuffer commandBuffer,
    const VkRenderPassBeginInfo* pRenderPassBegin,
    const VkSubpassBeginInfo* pSubpassBeginInfo) {
    log_driver_message("Generated Driver vkCmdBeginRenderPass2");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdNextSubpass2(
    VkCommandBuffer commandBuffer,
    const VkSubpassBeginInfo* pSubpassBeginInfo,
    const VkSubpassEndInfo* pSubpassEndInfo) {
    log_driver_message("Generated Driver vkCmdNextSubpass2");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdEndRenderPass2(
    VkCommandBuffer commandBuffer,
    const VkSubpassEndInfo* pSubpassEndInfo) {
    log_driver_message("Generated Driver vkCmdEndRenderPass2");
}

VKAPI_ATTR void VKAPI_CALL driver_ResetQueryPool(
    VkDevice device,
    VkQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount) {
    log_driver_message("Generated Driver vkResetQueryPool");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetSemaphoreCounterValue(
    VkDevice device,
    VkSemaphore semaphore,
    uint64_t* pValue) {
    log_driver_message("Generated Driver vkGetSemaphoreCounterValue");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_WaitSemaphores(
    VkDevice device,
    const VkSemaphoreWaitInfo* pWaitInfo,
    uint64_t timeout) {
    log_driver_message("Generated Driver vkWaitSemaphores");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_SignalSemaphore(
    VkDevice device,
    const VkSemaphoreSignalInfo* pSignalInfo) {
    log_driver_message("Generated Driver vkSignalSemaphore");
    return VK_SUCCESS;
}

VKAPI_ATTR VkDeviceAddress VKAPI_CALL driver_GetBufferDeviceAddress(
    VkDevice device,
    const VkBufferDeviceAddressInfo* pInfo) {
    log_driver_message("Generated Driver vkGetBufferDeviceAddress");
    return static_cast<VkDeviceAddress>(0);
}

VKAPI_ATTR uint64_t VKAPI_CALL driver_GetBufferOpaqueCaptureAddress(
    VkDevice device,
    const VkBufferDeviceAddressInfo* pInfo) {
    log_driver_message("Generated Driver vkGetBufferOpaqueCaptureAddress");
    return static_cast<uint64_t>(0);
}

VKAPI_ATTR uint64_t VKAPI_CALL driver_GetDeviceMemoryOpaqueCaptureAddress(
    VkDevice device,
    const VkDeviceMemoryOpaqueCaptureAddressInfo* pInfo) {
    log_driver_message("Generated Driver vkGetDeviceMemoryOpaqueCaptureAddress");
    return static_cast<uint64_t>(0);
}


// ----- VK_VERSION_1_3
VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceToolProperties(
    VkPhysicalDevice physicalDevice,
    uint32_t* pToolCount,
    VkPhysicalDeviceToolProperties* pToolProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceToolProperties");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreatePrivateDataSlot(
    VkDevice device,
    const VkPrivateDataSlotCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkPrivateDataSlot* pPrivateDataSlot) {
    log_driver_message("Generated Driver vkCreatePrivateDataSlot");
    *pPrivateDataSlot = reinterpret_cast<VkPrivateDataSlot>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyPrivateDataSlot(
    VkDevice device,
    VkPrivateDataSlot privateDataSlot,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyPrivateDataSlot");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_SetPrivateData(
    VkDevice device,
    VkObjectType objectType,
    uint64_t objectHandle,
    VkPrivateDataSlot privateDataSlot,
    uint64_t data) {
    log_driver_message("Generated Driver vkSetPrivateData");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_GetPrivateData(
    VkDevice device,
    VkObjectType objectType,
    uint64_t objectHandle,
    VkPrivateDataSlot privateDataSlot,
    uint64_t* pData) {
    log_driver_message("Generated Driver vkGetPrivateData");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetEvent2(
    VkCommandBuffer commandBuffer,
    VkEvent event,
    const VkDependencyInfo* pDependencyInfo) {
    log_driver_message("Generated Driver vkCmdSetEvent2");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdResetEvent2(
    VkCommandBuffer commandBuffer,
    VkEvent event,
    VkPipelineStageFlags2 stageMask) {
    log_driver_message("Generated Driver vkCmdResetEvent2");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdWaitEvents2(
    VkCommandBuffer commandBuffer,
    uint32_t eventCount,
    const VkEvent* pEvents,
    const VkDependencyInfo* pDependencyInfos) {
    log_driver_message("Generated Driver vkCmdWaitEvents2");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdPipelineBarrier2(
    VkCommandBuffer commandBuffer,
    const VkDependencyInfo* pDependencyInfo) {
    log_driver_message("Generated Driver vkCmdPipelineBarrier2");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdWriteTimestamp2(
    VkCommandBuffer commandBuffer,
    VkPipelineStageFlags2 stage,
    VkQueryPool queryPool,
    uint32_t query) {
    log_driver_message("Generated Driver vkCmdWriteTimestamp2");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_QueueSubmit2(
    VkQueue queue,
    uint32_t submitCount,
    const VkSubmitInfo2* pSubmits,
    VkFence fence) {
    log_driver_message("Generated Driver vkQueueSubmit2");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_CmdCopyBuffer2(
    VkCommandBuffer commandBuffer,
    const VkCopyBufferInfo2* pCopyBufferInfo) {
    log_driver_message("Generated Driver vkCmdCopyBuffer2");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdCopyImage2(
    VkCommandBuffer commandBuffer,
    const VkCopyImageInfo2* pCopyImageInfo) {
    log_driver_message("Generated Driver vkCmdCopyImage2");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdCopyBufferToImage2(
    VkCommandBuffer commandBuffer,
    const VkCopyBufferToImageInfo2* pCopyBufferToImageInfo) {
    log_driver_message("Generated Driver vkCmdCopyBufferToImage2");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdCopyImageToBuffer2(
    VkCommandBuffer commandBuffer,
    const VkCopyImageToBufferInfo2* pCopyImageToBufferInfo) {
    log_driver_message("Generated Driver vkCmdCopyImageToBuffer2");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBlitImage2(
    VkCommandBuffer commandBuffer,
    const VkBlitImageInfo2* pBlitImageInfo) {
    log_driver_message("Generated Driver vkCmdBlitImage2");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdResolveImage2(
    VkCommandBuffer commandBuffer,
    const VkResolveImageInfo2* pResolveImageInfo) {
    log_driver_message("Generated Driver vkCmdResolveImage2");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBeginRendering(
    VkCommandBuffer commandBuffer,
    const VkRenderingInfo* pRenderingInfo) {
    log_driver_message("Generated Driver vkCmdBeginRendering");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdEndRendering(
    VkCommandBuffer commandBuffer) {
    log_driver_message("Generated Driver vkCmdEndRendering");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetCullMode(
    VkCommandBuffer commandBuffer,
    VkCullModeFlags cullMode) {
    log_driver_message("Generated Driver vkCmdSetCullMode");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetFrontFace(
    VkCommandBuffer commandBuffer,
    VkFrontFace frontFace) {
    log_driver_message("Generated Driver vkCmdSetFrontFace");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetPrimitiveTopology(
    VkCommandBuffer commandBuffer,
    VkPrimitiveTopology primitiveTopology) {
    log_driver_message("Generated Driver vkCmdSetPrimitiveTopology");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetViewportWithCount(
    VkCommandBuffer commandBuffer,
    uint32_t viewportCount,
    const VkViewport* pViewports) {
    log_driver_message("Generated Driver vkCmdSetViewportWithCount");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetScissorWithCount(
    VkCommandBuffer commandBuffer,
    uint32_t scissorCount,
    const VkRect2D* pScissors) {
    log_driver_message("Generated Driver vkCmdSetScissorWithCount");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBindVertexBuffers2(
    VkCommandBuffer commandBuffer,
    uint32_t firstBinding,
    uint32_t bindingCount,
    const VkBuffer* pBuffers,
    const VkDeviceSize* pOffsets,
    const VkDeviceSize* pSizes,
    const VkDeviceSize* pStrides) {
    log_driver_message("Generated Driver vkCmdBindVertexBuffers2");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetDepthTestEnable(
    VkCommandBuffer commandBuffer,
    VkBool32 depthTestEnable) {
    log_driver_message("Generated Driver vkCmdSetDepthTestEnable");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetDepthWriteEnable(
    VkCommandBuffer commandBuffer,
    VkBool32 depthWriteEnable) {
    log_driver_message("Generated Driver vkCmdSetDepthWriteEnable");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetDepthCompareOp(
    VkCommandBuffer commandBuffer,
    VkCompareOp depthCompareOp) {
    log_driver_message("Generated Driver vkCmdSetDepthCompareOp");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetDepthBoundsTestEnable(
    VkCommandBuffer commandBuffer,
    VkBool32 depthBoundsTestEnable) {
    log_driver_message("Generated Driver vkCmdSetDepthBoundsTestEnable");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetStencilTestEnable(
    VkCommandBuffer commandBuffer,
    VkBool32 stencilTestEnable) {
    log_driver_message("Generated Driver vkCmdSetStencilTestEnable");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetStencilOp(
    VkCommandBuffer commandBuffer,
    VkStencilFaceFlags faceMask,
    VkStencilOp failOp,
    VkStencilOp passOp,
    VkStencilOp depthFailOp,
    VkCompareOp compareOp) {
    log_driver_message("Generated Driver vkCmdSetStencilOp");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetRasterizerDiscardEnable(
    VkCommandBuffer commandBuffer,
    VkBool32 rasterizerDiscardEnable) {
    log_driver_message("Generated Driver vkCmdSetRasterizerDiscardEnable");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetDepthBiasEnable(
    VkCommandBuffer commandBuffer,
    VkBool32 depthBiasEnable) {
    log_driver_message("Generated Driver vkCmdSetDepthBiasEnable");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetPrimitiveRestartEnable(
    VkCommandBuffer commandBuffer,
    VkBool32 primitiveRestartEnable) {
    log_driver_message("Generated Driver vkCmdSetPrimitiveRestartEnable");
}

VKAPI_ATTR void VKAPI_CALL driver_GetDeviceBufferMemoryRequirements(
    VkDevice device,
    const VkDeviceBufferMemoryRequirements* pInfo,
    VkMemoryRequirements2* pMemoryRequirements) {
    log_driver_message("Generated Driver vkGetDeviceBufferMemoryRequirements");
}

VKAPI_ATTR void VKAPI_CALL driver_GetDeviceImageMemoryRequirements(
    VkDevice device,
    const VkDeviceImageMemoryRequirements* pInfo,
    VkMemoryRequirements2* pMemoryRequirements) {
    log_driver_message("Generated Driver vkGetDeviceImageMemoryRequirements");
}

VKAPI_ATTR void VKAPI_CALL driver_GetDeviceImageSparseMemoryRequirements(
    VkDevice device,
    const VkDeviceImageMemoryRequirements* pInfo,
    uint32_t* pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2* pSparseMemoryRequirements) {
    log_driver_message("Generated Driver vkGetDeviceImageSparseMemoryRequirements");
}


// ----- VK_KHR_surface
VKAPI_ATTR void VKAPI_CALL driver_DestroySurfaceKHR(
    VkInstance instance,
    VkSurfaceKHR surface,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroySurfaceKHR");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    VkSurfaceKHR surface,
    VkBool32* pSupported) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceSurfaceSupportKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    VkSurfaceCapabilitiesKHR* pSurfaceCapabilities) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    uint32_t* pSurfaceFormatCount,
    VkSurfaceFormatKHR* pSurfaceFormats) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceSurfaceFormatsKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    uint32_t* pPresentModeCount,
    VkPresentModeKHR* pPresentModes) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceSurfacePresentModesKHR");
    return VK_SUCCESS;
}


// ----- VK_KHR_swapchain
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateSwapchainKHR(
    VkDevice device,
    const VkSwapchainCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSwapchainKHR* pSwapchain) {
    log_driver_message("Generated Driver vkCreateSwapchainKHR");
    *pSwapchain = reinterpret_cast<VkSwapchainKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroySwapchainKHR(
    VkDevice device,
    VkSwapchainKHR swapchain,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroySwapchainKHR");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetSwapchainImagesKHR(
    VkDevice device,
    VkSwapchainKHR swapchain,
    uint32_t* pSwapchainImageCount,
    VkImage* pSwapchainImages) {
    log_driver_message("Generated Driver vkGetSwapchainImagesKHR");
    *pSwapchainImages = reinterpret_cast<VkImage>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_AcquireNextImageKHR(
    VkDevice device,
    VkSwapchainKHR swapchain,
    uint64_t timeout,
    VkSemaphore semaphore,
    VkFence fence,
    uint32_t* pImageIndex) {
    log_driver_message("Generated Driver vkAcquireNextImageKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_QueuePresentKHR(
    VkQueue queue,
    const VkPresentInfoKHR* pPresentInfo) {
    log_driver_message("Generated Driver vkQueuePresentKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetDeviceGroupPresentCapabilitiesKHR(
    VkDevice device,
    VkDeviceGroupPresentCapabilitiesKHR* pDeviceGroupPresentCapabilities) {
    log_driver_message("Generated Driver vkGetDeviceGroupPresentCapabilitiesKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetDeviceGroupSurfacePresentModesKHR(
    VkDevice device,
    VkSurfaceKHR surface,
    VkDeviceGroupPresentModeFlagsKHR* pModes) {
    log_driver_message("Generated Driver vkGetDeviceGroupSurfacePresentModesKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDevicePresentRectanglesKHR(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    uint32_t* pRectCount,
    VkRect2D* pRects) {
    log_driver_message("Generated Driver vkGetPhysicalDevicePresentRectanglesKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_AcquireNextImage2KHR(
    VkDevice device,
    const VkAcquireNextImageInfoKHR* pAcquireInfo,
    uint32_t* pImageIndex) {
    log_driver_message("Generated Driver vkAcquireNextImage2KHR");
    return VK_SUCCESS;
}


// ----- VK_KHR_display
VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceDisplayPropertiesKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t* pPropertyCount,
    VkDisplayPropertiesKHR* pProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceDisplayPropertiesKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceDisplayPlanePropertiesKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t* pPropertyCount,
    VkDisplayPlanePropertiesKHR* pProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceDisplayPlanePropertiesKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetDisplayPlaneSupportedDisplaysKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t planeIndex,
    uint32_t* pDisplayCount,
    VkDisplayKHR* pDisplays) {
    log_driver_message("Generated Driver vkGetDisplayPlaneSupportedDisplaysKHR");
    *pDisplays = reinterpret_cast<VkDisplayKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetDisplayModePropertiesKHR(
    VkPhysicalDevice physicalDevice,
    VkDisplayKHR display,
    uint32_t* pPropertyCount,
    VkDisplayModePropertiesKHR* pProperties) {
    log_driver_message("Generated Driver vkGetDisplayModePropertiesKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateDisplayModeKHR(
    VkPhysicalDevice physicalDevice,
    VkDisplayKHR display,
    const VkDisplayModeCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDisplayModeKHR* pMode) {
    log_driver_message("Generated Driver vkCreateDisplayModeKHR");
    *pMode = reinterpret_cast<VkDisplayModeKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetDisplayPlaneCapabilitiesKHR(
    VkPhysicalDevice physicalDevice,
    VkDisplayModeKHR mode,
    uint32_t planeIndex,
    VkDisplayPlaneCapabilitiesKHR* pCapabilities) {
    log_driver_message("Generated Driver vkGetDisplayPlaneCapabilitiesKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateDisplayPlaneSurfaceKHR(
    VkInstance instance,
    const VkDisplaySurfaceCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_driver_message("Generated Driver vkCreateDisplayPlaneSurfaceKHR");
    *pSurface = reinterpret_cast<VkSurfaceKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}


// ----- VK_KHR_display_swapchain
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateSharedSwapchainsKHR(
    VkDevice device,
    uint32_t swapchainCount,
    const VkSwapchainCreateInfoKHR* pCreateInfos,
    const VkAllocationCallbacks* pAllocator,
    VkSwapchainKHR* pSwapchains) {
    log_driver_message("Generated Driver vkCreateSharedSwapchainsKHR");
    *pSwapchains = reinterpret_cast<VkSwapchainKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}


// ----- VK_KHR_xlib_surface
#ifdef VK_USE_PLATFORM_XLIB_KHR
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateXlibSurfaceKHR(
    VkInstance instance,
    const VkXlibSurfaceCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_driver_message("Generated Driver vkCreateXlibSurfaceKHR");
    *pSurface = reinterpret_cast<VkSurfaceKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR VkBool32 VKAPI_CALL driver_GetPhysicalDeviceXlibPresentationSupportKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    Display* dpy,
    VisualID visualID) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceXlibPresentationSupportKHR");
    return VK_TRUE;
}

#endif // VK_USE_PLATFORM_XLIB_KHR

// ----- VK_KHR_xcb_surface
#ifdef VK_USE_PLATFORM_XCB_KHR
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateXcbSurfaceKHR(
    VkInstance instance,
    const VkXcbSurfaceCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_driver_message("Generated Driver vkCreateXcbSurfaceKHR");
    *pSurface = reinterpret_cast<VkSurfaceKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR VkBool32 VKAPI_CALL driver_GetPhysicalDeviceXcbPresentationSupportKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    xcb_connection_t* connection,
    xcb_visualid_t visual_id) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceXcbPresentationSupportKHR");
    return VK_TRUE;
}

#endif // VK_USE_PLATFORM_XCB_KHR

// ----- VK_KHR_wayland_surface
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateWaylandSurfaceKHR(
    VkInstance instance,
    const VkWaylandSurfaceCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_driver_message("Generated Driver vkCreateWaylandSurfaceKHR");
    *pSurface = reinterpret_cast<VkSurfaceKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR VkBool32 VKAPI_CALL driver_GetPhysicalDeviceWaylandPresentationSupportKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    struct wl_display* display) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceWaylandPresentationSupportKHR");
    return VK_TRUE;
}

#endif // VK_USE_PLATFORM_WAYLAND_KHR

// ----- VK_KHR_win32_surface
#ifdef VK_USE_PLATFORM_WIN32_KHR
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateWin32SurfaceKHR(
    VkInstance instance,
    const VkWin32SurfaceCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_driver_message("Generated Driver vkCreateWin32SurfaceKHR");
    *pSurface = reinterpret_cast<VkSurfaceKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR VkBool32 VKAPI_CALL driver_GetPhysicalDeviceWin32PresentationSupportKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceWin32PresentationSupportKHR");
    return VK_TRUE;
}

#endif // VK_USE_PLATFORM_WIN32_KHR

// ----- VK_KHR_video_queue
#ifdef VK_ENABLE_BETA_EXTENSIONS
VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceVideoCapabilitiesKHR(
    VkPhysicalDevice physicalDevice,
    const VkVideoProfileKHR* pVideoProfile,
    VkVideoCapabilitiesKHR* pCapabilities) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceVideoCapabilitiesKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceVideoFormatPropertiesKHR(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceVideoFormatInfoKHR* pVideoFormatInfo,
    uint32_t* pVideoFormatPropertyCount,
    VkVideoFormatPropertiesKHR* pVideoFormatProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceVideoFormatPropertiesKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateVideoSessionKHR(
    VkDevice device,
    const VkVideoSessionCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkVideoSessionKHR* pVideoSession) {
    log_driver_message("Generated Driver vkCreateVideoSessionKHR");
    *pVideoSession = reinterpret_cast<VkVideoSessionKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyVideoSessionKHR(
    VkDevice device,
    VkVideoSessionKHR videoSession,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyVideoSessionKHR");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetVideoSessionMemoryRequirementsKHR(
    VkDevice device,
    VkVideoSessionKHR videoSession,
    uint32_t* pVideoSessionMemoryRequirementsCount,
    VkVideoGetMemoryPropertiesKHR* pVideoSessionMemoryRequirements) {
    log_driver_message("Generated Driver vkGetVideoSessionMemoryRequirementsKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_BindVideoSessionMemoryKHR(
    VkDevice device,
    VkVideoSessionKHR videoSession,
    uint32_t videoSessionBindMemoryCount,
    const VkVideoBindMemoryKHR* pVideoSessionBindMemories) {
    log_driver_message("Generated Driver vkBindVideoSessionMemoryKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateVideoSessionParametersKHR(
    VkDevice device,
    const VkVideoSessionParametersCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkVideoSessionParametersKHR* pVideoSessionParameters) {
    log_driver_message("Generated Driver vkCreateVideoSessionParametersKHR");
    *pVideoSessionParameters = reinterpret_cast<VkVideoSessionParametersKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_UpdateVideoSessionParametersKHR(
    VkDevice device,
    VkVideoSessionParametersKHR videoSessionParameters,
    const VkVideoSessionParametersUpdateInfoKHR* pUpdateInfo) {
    log_driver_message("Generated Driver vkUpdateVideoSessionParametersKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyVideoSessionParametersKHR(
    VkDevice device,
    VkVideoSessionParametersKHR videoSessionParameters,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyVideoSessionParametersKHR");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBeginVideoCodingKHR(
    VkCommandBuffer commandBuffer,
    const VkVideoBeginCodingInfoKHR* pBeginInfo) {
    log_driver_message("Generated Driver vkCmdBeginVideoCodingKHR");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdEndVideoCodingKHR(
    VkCommandBuffer commandBuffer,
    const VkVideoEndCodingInfoKHR* pEndCodingInfo) {
    log_driver_message("Generated Driver vkCmdEndVideoCodingKHR");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdControlVideoCodingKHR(
    VkCommandBuffer commandBuffer,
    const VkVideoCodingControlInfoKHR* pCodingControlInfo) {
    log_driver_message("Generated Driver vkCmdControlVideoCodingKHR");
}

#endif // VK_ENABLE_BETA_EXTENSIONS

// ----- VK_KHR_video_decode_queue
#ifdef VK_ENABLE_BETA_EXTENSIONS
VKAPI_ATTR void VKAPI_CALL driver_CmdDecodeVideoKHR(
    VkCommandBuffer commandBuffer,
    const VkVideoDecodeInfoKHR* pFrameInfo) {
    log_driver_message("Generated Driver vkCmdDecodeVideoKHR");
}

#endif // VK_ENABLE_BETA_EXTENSIONS

// ----- VK_KHR_dynamic_rendering
VKAPI_ATTR void VKAPI_CALL driver_CmdBeginRenderingKHR(
    VkCommandBuffer commandBuffer,
    const VkRenderingInfo* pRenderingInfo) {
    log_driver_message("Generated Driver vkCmdBeginRenderingKHR");
    return driver_CmdBeginRendering(commandBuffer, pRenderingInfo);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdEndRenderingKHR(
    VkCommandBuffer commandBuffer) {
    log_driver_message("Generated Driver vkCmdEndRenderingKHR");
    return driver_CmdEndRendering(commandBuffer);
}


// ----- VK_KHR_get_physical_device_properties2
VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceFeatures2KHR(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceFeatures2* pFeatures) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceFeatures2KHR");
    return driver_GetPhysicalDeviceFeatures2(physicalDevice, pFeatures);
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceProperties2KHR(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceProperties2* pProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceProperties2KHR");
    return driver_GetPhysicalDeviceProperties2(physicalDevice, pProperties);
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceFormatProperties2KHR(
    VkPhysicalDevice physicalDevice,
    VkFormat format,
    VkFormatProperties2* pFormatProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceFormatProperties2KHR");
    return driver_GetPhysicalDeviceFormatProperties2(physicalDevice, format, pFormatProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceImageFormatProperties2KHR(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo,
    VkImageFormatProperties2* pImageFormatProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceImageFormatProperties2KHR");
    return driver_GetPhysicalDeviceImageFormatProperties2(physicalDevice, pImageFormatInfo, pImageFormatProperties);
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceQueueFamilyProperties2KHR(
    VkPhysicalDevice physicalDevice,
    uint32_t* pQueueFamilyPropertyCount,
    VkQueueFamilyProperties2* pQueueFamilyProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceQueueFamilyProperties2KHR");
    return driver_GetPhysicalDeviceQueueFamilyProperties2(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceMemoryProperties2KHR(
    VkPhysicalDevice physicalDevice,
    VkPhysicalDeviceMemoryProperties2* pMemoryProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceMemoryProperties2KHR");
    return driver_GetPhysicalDeviceMemoryProperties2(physicalDevice, pMemoryProperties);
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceSparseImageFormatProperties2KHR(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo,
    uint32_t* pPropertyCount,
    VkSparseImageFormatProperties2* pProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceSparseImageFormatProperties2KHR");
    return driver_GetPhysicalDeviceSparseImageFormatProperties2(physicalDevice, pFormatInfo, pPropertyCount, pProperties);
}


// ----- VK_KHR_device_group
VKAPI_ATTR void VKAPI_CALL driver_GetDeviceGroupPeerMemoryFeaturesKHR(
    VkDevice device,
    uint32_t heapIndex,
    uint32_t localDeviceIndex,
    uint32_t remoteDeviceIndex,
    VkPeerMemoryFeatureFlags* pPeerMemoryFeatures) {
    log_driver_message("Generated Driver vkGetDeviceGroupPeerMemoryFeaturesKHR");
    return driver_GetDeviceGroupPeerMemoryFeatures(device, heapIndex, localDeviceIndex, remoteDeviceIndex, pPeerMemoryFeatures);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetDeviceMaskKHR(
    VkCommandBuffer commandBuffer,
    uint32_t deviceMask) {
    log_driver_message("Generated Driver vkCmdSetDeviceMaskKHR");
    return driver_CmdSetDeviceMask(commandBuffer, deviceMask);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdDispatchBaseKHR(
    VkCommandBuffer commandBuffer,
    uint32_t baseGroupX,
    uint32_t baseGroupY,
    uint32_t baseGroupZ,
    uint32_t groupCountX,
    uint32_t groupCountY,
    uint32_t groupCountZ) {
    log_driver_message("Generated Driver vkCmdDispatchBaseKHR");
    return driver_CmdDispatchBase(commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
}


// ----- VK_KHR_maintenance1
VKAPI_ATTR void VKAPI_CALL driver_TrimCommandPoolKHR(
    VkDevice device,
    VkCommandPool commandPool,
    VkCommandPoolTrimFlags flags) {
    log_driver_message("Generated Driver vkTrimCommandPoolKHR");
    return driver_TrimCommandPool(device, commandPool, flags);
}


// ----- VK_KHR_device_group_creation
VKAPI_ATTR VkResult VKAPI_CALL driver_EnumeratePhysicalDeviceGroupsKHR(
    VkInstance instance,
    uint32_t* pPhysicalDeviceGroupCount,
    VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties) {
    log_driver_message("Generated Driver vkEnumeratePhysicalDeviceGroupsKHR");
    return driver_EnumeratePhysicalDeviceGroups(instance, pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
}


// ----- VK_KHR_external_memory_capabilities
VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceExternalBufferPropertiesKHR(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo,
    VkExternalBufferProperties* pExternalBufferProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceExternalBufferPropertiesKHR");
    return driver_GetPhysicalDeviceExternalBufferProperties(physicalDevice, pExternalBufferInfo, pExternalBufferProperties);
}


// ----- VK_KHR_external_memory_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
VKAPI_ATTR VkResult VKAPI_CALL driver_GetMemoryWin32HandleKHR(
    VkDevice device,
    const VkMemoryGetWin32HandleInfoKHR* pGetWin32HandleInfo,
    HANDLE* pHandle) {
    log_driver_message("Generated Driver vkGetMemoryWin32HandleKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetMemoryWin32HandlePropertiesKHR(
    VkDevice device,
    VkExternalMemoryHandleTypeFlagBits handleType,
    HANDLE handle,
    VkMemoryWin32HandlePropertiesKHR* pMemoryWin32HandleProperties) {
    log_driver_message("Generated Driver vkGetMemoryWin32HandlePropertiesKHR");
    return VK_SUCCESS;
}

#endif // VK_USE_PLATFORM_WIN32_KHR

// ----- VK_KHR_external_memory_fd
VKAPI_ATTR VkResult VKAPI_CALL driver_GetMemoryFdKHR(
    VkDevice device,
    const VkMemoryGetFdInfoKHR* pGetFdInfo,
    int* pFd) {
    log_driver_message("Generated Driver vkGetMemoryFdKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetMemoryFdPropertiesKHR(
    VkDevice device,
    VkExternalMemoryHandleTypeFlagBits handleType,
    int fd,
    VkMemoryFdPropertiesKHR* pMemoryFdProperties) {
    log_driver_message("Generated Driver vkGetMemoryFdPropertiesKHR");
    return VK_SUCCESS;
}


// ----- VK_KHR_external_semaphore_capabilities
VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceExternalSemaphorePropertiesKHR(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo,
    VkExternalSemaphoreProperties* pExternalSemaphoreProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceExternalSemaphorePropertiesKHR");
    return driver_GetPhysicalDeviceExternalSemaphoreProperties(physicalDevice, pExternalSemaphoreInfo, pExternalSemaphoreProperties);
}


// ----- VK_KHR_external_semaphore_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
VKAPI_ATTR VkResult VKAPI_CALL driver_ImportSemaphoreWin32HandleKHR(
    VkDevice device,
    const VkImportSemaphoreWin32HandleInfoKHR* pImportSemaphoreWin32HandleInfo) {
    log_driver_message("Generated Driver vkImportSemaphoreWin32HandleKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetSemaphoreWin32HandleKHR(
    VkDevice device,
    const VkSemaphoreGetWin32HandleInfoKHR* pGetWin32HandleInfo,
    HANDLE* pHandle) {
    log_driver_message("Generated Driver vkGetSemaphoreWin32HandleKHR");
    return VK_SUCCESS;
}

#endif // VK_USE_PLATFORM_WIN32_KHR

// ----- VK_KHR_external_semaphore_fd
VKAPI_ATTR VkResult VKAPI_CALL driver_ImportSemaphoreFdKHR(
    VkDevice device,
    const VkImportSemaphoreFdInfoKHR* pImportSemaphoreFdInfo) {
    log_driver_message("Generated Driver vkImportSemaphoreFdKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetSemaphoreFdKHR(
    VkDevice device,
    const VkSemaphoreGetFdInfoKHR* pGetFdInfo,
    int* pFd) {
    log_driver_message("Generated Driver vkGetSemaphoreFdKHR");
    return VK_SUCCESS;
}


// ----- VK_KHR_push_descriptor
VKAPI_ATTR void VKAPI_CALL driver_CmdPushDescriptorSetKHR(
    VkCommandBuffer commandBuffer,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipelineLayout layout,
    uint32_t set,
    uint32_t descriptorWriteCount,
    const VkWriteDescriptorSet* pDescriptorWrites) {
    log_driver_message("Generated Driver vkCmdPushDescriptorSetKHR");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdPushDescriptorSetWithTemplateKHR(
    VkCommandBuffer commandBuffer,
    VkDescriptorUpdateTemplate descriptorUpdateTemplate,
    VkPipelineLayout layout,
    uint32_t set,
    const void* pData) {
    log_driver_message("Generated Driver vkCmdPushDescriptorSetWithTemplateKHR");
}


// ----- VK_KHR_descriptor_update_template
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateDescriptorUpdateTemplateKHR(
    VkDevice device,
    const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDescriptorUpdateTemplate* pDescriptorUpdateTemplate) {
    log_driver_message("Generated Driver vkCreateDescriptorUpdateTemplateKHR");
    return driver_CreateDescriptorUpdateTemplate(device, pCreateInfo, pAllocator, pDescriptorUpdateTemplate);
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyDescriptorUpdateTemplateKHR(
    VkDevice device,
    VkDescriptorUpdateTemplate descriptorUpdateTemplate,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyDescriptorUpdateTemplateKHR");
    return driver_DestroyDescriptorUpdateTemplate(device, descriptorUpdateTemplate, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL driver_UpdateDescriptorSetWithTemplateKHR(
    VkDevice device,
    VkDescriptorSet descriptorSet,
    VkDescriptorUpdateTemplate descriptorUpdateTemplate,
    const void* pData) {
    log_driver_message("Generated Driver vkUpdateDescriptorSetWithTemplateKHR");
    return driver_UpdateDescriptorSetWithTemplate(device, descriptorSet, descriptorUpdateTemplate, pData);
}


// ----- VK_KHR_create_renderpass2
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateRenderPass2KHR(
    VkDevice device,
    const VkRenderPassCreateInfo2* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkRenderPass* pRenderPass) {
    log_driver_message("Generated Driver vkCreateRenderPass2KHR");
    return driver_CreateRenderPass2(device, pCreateInfo, pAllocator, pRenderPass);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBeginRenderPass2KHR(
    VkCommandBuffer commandBuffer,
    const VkRenderPassBeginInfo* pRenderPassBegin,
    const VkSubpassBeginInfo* pSubpassBeginInfo) {
    log_driver_message("Generated Driver vkCmdBeginRenderPass2KHR");
    return driver_CmdBeginRenderPass2(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdNextSubpass2KHR(
    VkCommandBuffer commandBuffer,
    const VkSubpassBeginInfo* pSubpassBeginInfo,
    const VkSubpassEndInfo* pSubpassEndInfo) {
    log_driver_message("Generated Driver vkCmdNextSubpass2KHR");
    return driver_CmdNextSubpass2(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdEndRenderPass2KHR(
    VkCommandBuffer commandBuffer,
    const VkSubpassEndInfo* pSubpassEndInfo) {
    log_driver_message("Generated Driver vkCmdEndRenderPass2KHR");
    return driver_CmdEndRenderPass2(commandBuffer, pSubpassEndInfo);
}


// ----- VK_KHR_shared_presentable_image
VKAPI_ATTR VkResult VKAPI_CALL driver_GetSwapchainStatusKHR(
    VkDevice device,
    VkSwapchainKHR swapchain) {
    log_driver_message("Generated Driver vkGetSwapchainStatusKHR");
    return VK_SUCCESS;
}


// ----- VK_KHR_external_fence_capabilities
VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceExternalFencePropertiesKHR(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo,
    VkExternalFenceProperties* pExternalFenceProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceExternalFencePropertiesKHR");
    return driver_GetPhysicalDeviceExternalFenceProperties(physicalDevice, pExternalFenceInfo, pExternalFenceProperties);
}


// ----- VK_KHR_external_fence_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
VKAPI_ATTR VkResult VKAPI_CALL driver_ImportFenceWin32HandleKHR(
    VkDevice device,
    const VkImportFenceWin32HandleInfoKHR* pImportFenceWin32HandleInfo) {
    log_driver_message("Generated Driver vkImportFenceWin32HandleKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetFenceWin32HandleKHR(
    VkDevice device,
    const VkFenceGetWin32HandleInfoKHR* pGetWin32HandleInfo,
    HANDLE* pHandle) {
    log_driver_message("Generated Driver vkGetFenceWin32HandleKHR");
    return VK_SUCCESS;
}

#endif // VK_USE_PLATFORM_WIN32_KHR

// ----- VK_KHR_external_fence_fd
VKAPI_ATTR VkResult VKAPI_CALL driver_ImportFenceFdKHR(
    VkDevice device,
    const VkImportFenceFdInfoKHR* pImportFenceFdInfo) {
    log_driver_message("Generated Driver vkImportFenceFdKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetFenceFdKHR(
    VkDevice device,
    const VkFenceGetFdInfoKHR* pGetFdInfo,
    int* pFd) {
    log_driver_message("Generated Driver vkGetFenceFdKHR");
    return VK_SUCCESS;
}


// ----- VK_KHR_performance_query
VKAPI_ATTR VkResult VKAPI_CALL driver_EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    uint32_t* pCounterCount,
    VkPerformanceCounterKHR* pCounters,
    VkPerformanceCounterDescriptionKHR* pCounterDescriptions) {
    log_driver_message("Generated Driver vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR(
    VkPhysicalDevice physicalDevice,
    const VkQueryPoolPerformanceCreateInfoKHR* pPerformanceQueryCreateInfo,
    uint32_t* pNumPasses) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_AcquireProfilingLockKHR(
    VkDevice device,
    const VkAcquireProfilingLockInfoKHR* pInfo) {
    log_driver_message("Generated Driver vkAcquireProfilingLockKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_ReleaseProfilingLockKHR(
    VkDevice device) {
    log_driver_message("Generated Driver vkReleaseProfilingLockKHR");
}


// ----- VK_KHR_get_surface_capabilities2
VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceSurfaceCapabilities2KHR(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
    VkSurfaceCapabilities2KHR* pSurfaceCapabilities) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceSurfaceCapabilities2KHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceSurfaceFormats2KHR(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
    uint32_t* pSurfaceFormatCount,
    VkSurfaceFormat2KHR* pSurfaceFormats) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceSurfaceFormats2KHR");
    return VK_SUCCESS;
}


// ----- VK_KHR_get_display_properties2
VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceDisplayProperties2KHR(
    VkPhysicalDevice physicalDevice,
    uint32_t* pPropertyCount,
    VkDisplayProperties2KHR* pProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceDisplayProperties2KHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceDisplayPlaneProperties2KHR(
    VkPhysicalDevice physicalDevice,
    uint32_t* pPropertyCount,
    VkDisplayPlaneProperties2KHR* pProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceDisplayPlaneProperties2KHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetDisplayModeProperties2KHR(
    VkPhysicalDevice physicalDevice,
    VkDisplayKHR display,
    uint32_t* pPropertyCount,
    VkDisplayModeProperties2KHR* pProperties) {
    log_driver_message("Generated Driver vkGetDisplayModeProperties2KHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetDisplayPlaneCapabilities2KHR(
    VkPhysicalDevice physicalDevice,
    const VkDisplayPlaneInfo2KHR* pDisplayPlaneInfo,
    VkDisplayPlaneCapabilities2KHR* pCapabilities) {
    log_driver_message("Generated Driver vkGetDisplayPlaneCapabilities2KHR");
    return VK_SUCCESS;
}


// ----- VK_KHR_get_memory_requirements2
VKAPI_ATTR void VKAPI_CALL driver_GetImageMemoryRequirements2KHR(
    VkDevice device,
    const VkImageMemoryRequirementsInfo2* pInfo,
    VkMemoryRequirements2* pMemoryRequirements) {
    log_driver_message("Generated Driver vkGetImageMemoryRequirements2KHR");
    return driver_GetImageMemoryRequirements2(device, pInfo, pMemoryRequirements);
}

VKAPI_ATTR void VKAPI_CALL driver_GetBufferMemoryRequirements2KHR(
    VkDevice device,
    const VkBufferMemoryRequirementsInfo2* pInfo,
    VkMemoryRequirements2* pMemoryRequirements) {
    log_driver_message("Generated Driver vkGetBufferMemoryRequirements2KHR");
    return driver_GetBufferMemoryRequirements2(device, pInfo, pMemoryRequirements);
}

VKAPI_ATTR void VKAPI_CALL driver_GetImageSparseMemoryRequirements2KHR(
    VkDevice device,
    const VkImageSparseMemoryRequirementsInfo2* pInfo,
    uint32_t* pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2* pSparseMemoryRequirements) {
    log_driver_message("Generated Driver vkGetImageSparseMemoryRequirements2KHR");
    return driver_GetImageSparseMemoryRequirements2(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}


// ----- VK_KHR_sampler_ycbcr_conversion
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateSamplerYcbcrConversionKHR(
    VkDevice device,
    const VkSamplerYcbcrConversionCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSamplerYcbcrConversion* pYcbcrConversion) {
    log_driver_message("Generated Driver vkCreateSamplerYcbcrConversionKHR");
    return driver_CreateSamplerYcbcrConversion(device, pCreateInfo, pAllocator, pYcbcrConversion);
}

VKAPI_ATTR void VKAPI_CALL driver_DestroySamplerYcbcrConversionKHR(
    VkDevice device,
    VkSamplerYcbcrConversion ycbcrConversion,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroySamplerYcbcrConversionKHR");
    return driver_DestroySamplerYcbcrConversion(device, ycbcrConversion, pAllocator);
}


// ----- VK_KHR_bind_memory2
VKAPI_ATTR VkResult VKAPI_CALL driver_BindBufferMemory2KHR(
    VkDevice device,
    uint32_t bindInfoCount,
    const VkBindBufferMemoryInfo* pBindInfos) {
    log_driver_message("Generated Driver vkBindBufferMemory2KHR");
    return driver_BindBufferMemory2(device, bindInfoCount, pBindInfos);
}

VKAPI_ATTR VkResult VKAPI_CALL driver_BindImageMemory2KHR(
    VkDevice device,
    uint32_t bindInfoCount,
    const VkBindImageMemoryInfo* pBindInfos) {
    log_driver_message("Generated Driver vkBindImageMemory2KHR");
    return driver_BindImageMemory2(device, bindInfoCount, pBindInfos);
}


// ----- VK_KHR_maintenance3
VKAPI_ATTR void VKAPI_CALL driver_GetDescriptorSetLayoutSupportKHR(
    VkDevice device,
    const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
    VkDescriptorSetLayoutSupport* pSupport) {
    log_driver_message("Generated Driver vkGetDescriptorSetLayoutSupportKHR");
    return driver_GetDescriptorSetLayoutSupport(device, pCreateInfo, pSupport);
}


// ----- VK_KHR_draw_indirect_count
VKAPI_ATTR void VKAPI_CALL driver_CmdDrawIndirectCountKHR(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    VkBuffer countBuffer,
    VkDeviceSize countBufferOffset,
    uint32_t maxDrawCount,
    uint32_t stride) {
    log_driver_message("Generated Driver vkCmdDrawIndirectCountKHR");
    return driver_CmdDrawIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdDrawIndexedIndirectCountKHR(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    VkBuffer countBuffer,
    VkDeviceSize countBufferOffset,
    uint32_t maxDrawCount,
    uint32_t stride) {
    log_driver_message("Generated Driver vkCmdDrawIndexedIndirectCountKHR");
    return driver_CmdDrawIndexedIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}


// ----- VK_KHR_timeline_semaphore
VKAPI_ATTR VkResult VKAPI_CALL driver_GetSemaphoreCounterValueKHR(
    VkDevice device,
    VkSemaphore semaphore,
    uint64_t* pValue) {
    log_driver_message("Generated Driver vkGetSemaphoreCounterValueKHR");
    return driver_GetSemaphoreCounterValue(device, semaphore, pValue);
}

VKAPI_ATTR VkResult VKAPI_CALL driver_WaitSemaphoresKHR(
    VkDevice device,
    const VkSemaphoreWaitInfo* pWaitInfo,
    uint64_t timeout) {
    log_driver_message("Generated Driver vkWaitSemaphoresKHR");
    return driver_WaitSemaphores(device, pWaitInfo, timeout);
}

VKAPI_ATTR VkResult VKAPI_CALL driver_SignalSemaphoreKHR(
    VkDevice device,
    const VkSemaphoreSignalInfo* pSignalInfo) {
    log_driver_message("Generated Driver vkSignalSemaphoreKHR");
    return driver_SignalSemaphore(device, pSignalInfo);
}


// ----- VK_KHR_fragment_shading_rate
VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceFragmentShadingRatesKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t* pFragmentShadingRateCount,
    VkPhysicalDeviceFragmentShadingRateKHR* pFragmentShadingRates) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceFragmentShadingRatesKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetFragmentShadingRateKHR(
    VkCommandBuffer commandBuffer,
    const VkExtent2D* pFragmentSize,
    const VkFragmentShadingRateCombinerOpKHR combinerOps[2]) {
    log_driver_message("Generated Driver vkCmdSetFragmentShadingRateKHR");
}


// ----- VK_KHR_present_wait
VKAPI_ATTR VkResult VKAPI_CALL driver_WaitForPresentKHR(
    VkDevice device,
    VkSwapchainKHR swapchain,
    uint64_t presentId,
    uint64_t timeout) {
    log_driver_message("Generated Driver vkWaitForPresentKHR");
    return VK_SUCCESS;
}


// ----- VK_KHR_buffer_device_address
VKAPI_ATTR VkDeviceAddress VKAPI_CALL driver_GetBufferDeviceAddressKHR(
    VkDevice device,
    const VkBufferDeviceAddressInfo* pInfo) {
    log_driver_message("Generated Driver vkGetBufferDeviceAddressKHR");
    return driver_GetBufferDeviceAddress(device, pInfo);
}

VKAPI_ATTR uint64_t VKAPI_CALL driver_GetBufferOpaqueCaptureAddressKHR(
    VkDevice device,
    const VkBufferDeviceAddressInfo* pInfo) {
    log_driver_message("Generated Driver vkGetBufferOpaqueCaptureAddressKHR");
    return driver_GetBufferOpaqueCaptureAddress(device, pInfo);
}

VKAPI_ATTR uint64_t VKAPI_CALL driver_GetDeviceMemoryOpaqueCaptureAddressKHR(
    VkDevice device,
    const VkDeviceMemoryOpaqueCaptureAddressInfo* pInfo) {
    log_driver_message("Generated Driver vkGetDeviceMemoryOpaqueCaptureAddressKHR");
    return driver_GetDeviceMemoryOpaqueCaptureAddress(device, pInfo);
}


// ----- VK_KHR_deferred_host_operations
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateDeferredOperationKHR(
    VkDevice device,
    const VkAllocationCallbacks* pAllocator,
    VkDeferredOperationKHR* pDeferredOperation) {
    log_driver_message("Generated Driver vkCreateDeferredOperationKHR");
    *pDeferredOperation = reinterpret_cast<VkDeferredOperationKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyDeferredOperationKHR(
    VkDevice device,
    VkDeferredOperationKHR operation,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyDeferredOperationKHR");
}

VKAPI_ATTR uint32_t VKAPI_CALL driver_GetDeferredOperationMaxConcurrencyKHR(
    VkDevice device,
    VkDeferredOperationKHR operation) {
    log_driver_message("Generated Driver vkGetDeferredOperationMaxConcurrencyKHR");
    return static_cast<uint32_t>(0);
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetDeferredOperationResultKHR(
    VkDevice device,
    VkDeferredOperationKHR operation) {
    log_driver_message("Generated Driver vkGetDeferredOperationResultKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_DeferredOperationJoinKHR(
    VkDevice device,
    VkDeferredOperationKHR operation) {
    log_driver_message("Generated Driver vkDeferredOperationJoinKHR");
    return VK_SUCCESS;
}


// ----- VK_KHR_pipeline_executable_properties
VKAPI_ATTR VkResult VKAPI_CALL driver_GetPipelineExecutablePropertiesKHR(
    VkDevice device,
    const VkPipelineInfoKHR* pPipelineInfo,
    uint32_t* pExecutableCount,
    VkPipelineExecutablePropertiesKHR* pProperties) {
    log_driver_message("Generated Driver vkGetPipelineExecutablePropertiesKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetPipelineExecutableStatisticsKHR(
    VkDevice device,
    const VkPipelineExecutableInfoKHR* pExecutableInfo,
    uint32_t* pStatisticCount,
    VkPipelineExecutableStatisticKHR* pStatistics) {
    log_driver_message("Generated Driver vkGetPipelineExecutableStatisticsKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetPipelineExecutableInternalRepresentationsKHR(
    VkDevice device,
    const VkPipelineExecutableInfoKHR* pExecutableInfo,
    uint32_t* pInternalRepresentationCount,
    VkPipelineExecutableInternalRepresentationKHR* pInternalRepresentations) {
    log_driver_message("Generated Driver vkGetPipelineExecutableInternalRepresentationsKHR");
    return VK_SUCCESS;
}


// ----- VK_KHR_video_encode_queue
#ifdef VK_ENABLE_BETA_EXTENSIONS
VKAPI_ATTR void VKAPI_CALL driver_CmdEncodeVideoKHR(
    VkCommandBuffer commandBuffer,
    const VkVideoEncodeInfoKHR* pEncodeInfo) {
    log_driver_message("Generated Driver vkCmdEncodeVideoKHR");
}

#endif // VK_ENABLE_BETA_EXTENSIONS

// ----- VK_KHR_synchronization2
VKAPI_ATTR void VKAPI_CALL driver_CmdSetEvent2KHR(
    VkCommandBuffer commandBuffer,
    VkEvent event,
    const VkDependencyInfo* pDependencyInfo) {
    log_driver_message("Generated Driver vkCmdSetEvent2KHR");
    return driver_CmdSetEvent2(commandBuffer, event, pDependencyInfo);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdResetEvent2KHR(
    VkCommandBuffer commandBuffer,
    VkEvent event,
    VkPipelineStageFlags2 stageMask) {
    log_driver_message("Generated Driver vkCmdResetEvent2KHR");
    return driver_CmdResetEvent2(commandBuffer, event, stageMask);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdWaitEvents2KHR(
    VkCommandBuffer commandBuffer,
    uint32_t eventCount,
    const VkEvent* pEvents,
    const VkDependencyInfo* pDependencyInfos) {
    log_driver_message("Generated Driver vkCmdWaitEvents2KHR");
    return driver_CmdWaitEvents2(commandBuffer, eventCount, pEvents, pDependencyInfos);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdPipelineBarrier2KHR(
    VkCommandBuffer commandBuffer,
    const VkDependencyInfo* pDependencyInfo) {
    log_driver_message("Generated Driver vkCmdPipelineBarrier2KHR");
    return driver_CmdPipelineBarrier2(commandBuffer, pDependencyInfo);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdWriteTimestamp2KHR(
    VkCommandBuffer commandBuffer,
    VkPipelineStageFlags2 stage,
    VkQueryPool queryPool,
    uint32_t query) {
    log_driver_message("Generated Driver vkCmdWriteTimestamp2KHR");
    return driver_CmdWriteTimestamp2(commandBuffer, stage, queryPool, query);
}

VKAPI_ATTR VkResult VKAPI_CALL driver_QueueSubmit2KHR(
    VkQueue queue,
    uint32_t submitCount,
    const VkSubmitInfo2* pSubmits,
    VkFence fence) {
    log_driver_message("Generated Driver vkQueueSubmit2KHR");
    return driver_QueueSubmit2(queue, submitCount, pSubmits, fence);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdWriteBufferMarker2AMD(
    VkCommandBuffer commandBuffer,
    VkPipelineStageFlags2 stage,
    VkBuffer dstBuffer,
    VkDeviceSize dstOffset,
    uint32_t marker) {
    log_driver_message("Generated Driver vkCmdWriteBufferMarker2AMD");
}

VKAPI_ATTR void VKAPI_CALL driver_GetQueueCheckpointData2NV(
    VkQueue queue,
    uint32_t* pCheckpointDataCount,
    VkCheckpointData2NV* pCheckpointData) {
    log_driver_message("Generated Driver vkGetQueueCheckpointData2NV");
}


// ----- VK_KHR_copy_commands2
VKAPI_ATTR void VKAPI_CALL driver_CmdCopyBuffer2KHR(
    VkCommandBuffer commandBuffer,
    const VkCopyBufferInfo2* pCopyBufferInfo) {
    log_driver_message("Generated Driver vkCmdCopyBuffer2KHR");
    return driver_CmdCopyBuffer2(commandBuffer, pCopyBufferInfo);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdCopyImage2KHR(
    VkCommandBuffer commandBuffer,
    const VkCopyImageInfo2* pCopyImageInfo) {
    log_driver_message("Generated Driver vkCmdCopyImage2KHR");
    return driver_CmdCopyImage2(commandBuffer, pCopyImageInfo);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdCopyBufferToImage2KHR(
    VkCommandBuffer commandBuffer,
    const VkCopyBufferToImageInfo2* pCopyBufferToImageInfo) {
    log_driver_message("Generated Driver vkCmdCopyBufferToImage2KHR");
    return driver_CmdCopyBufferToImage2(commandBuffer, pCopyBufferToImageInfo);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdCopyImageToBuffer2KHR(
    VkCommandBuffer commandBuffer,
    const VkCopyImageToBufferInfo2* pCopyImageToBufferInfo) {
    log_driver_message("Generated Driver vkCmdCopyImageToBuffer2KHR");
    return driver_CmdCopyImageToBuffer2(commandBuffer, pCopyImageToBufferInfo);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBlitImage2KHR(
    VkCommandBuffer commandBuffer,
    const VkBlitImageInfo2* pBlitImageInfo) {
    log_driver_message("Generated Driver vkCmdBlitImage2KHR");
    return driver_CmdBlitImage2(commandBuffer, pBlitImageInfo);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdResolveImage2KHR(
    VkCommandBuffer commandBuffer,
    const VkResolveImageInfo2* pResolveImageInfo) {
    log_driver_message("Generated Driver vkCmdResolveImage2KHR");
    return driver_CmdResolveImage2(commandBuffer, pResolveImageInfo);
}


// ----- VK_KHR_maintenance4
VKAPI_ATTR void VKAPI_CALL driver_GetDeviceBufferMemoryRequirementsKHR(
    VkDevice device,
    const VkDeviceBufferMemoryRequirements* pInfo,
    VkMemoryRequirements2* pMemoryRequirements) {
    log_driver_message("Generated Driver vkGetDeviceBufferMemoryRequirementsKHR");
    return driver_GetDeviceBufferMemoryRequirements(device, pInfo, pMemoryRequirements);
}

VKAPI_ATTR void VKAPI_CALL driver_GetDeviceImageMemoryRequirementsKHR(
    VkDevice device,
    const VkDeviceImageMemoryRequirements* pInfo,
    VkMemoryRequirements2* pMemoryRequirements) {
    log_driver_message("Generated Driver vkGetDeviceImageMemoryRequirementsKHR");
    return driver_GetDeviceImageMemoryRequirements(device, pInfo, pMemoryRequirements);
}

VKAPI_ATTR void VKAPI_CALL driver_GetDeviceImageSparseMemoryRequirementsKHR(
    VkDevice device,
    const VkDeviceImageMemoryRequirements* pInfo,
    uint32_t* pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2* pSparseMemoryRequirements) {
    log_driver_message("Generated Driver vkGetDeviceImageSparseMemoryRequirementsKHR");
    return driver_GetDeviceImageSparseMemoryRequirements(device, pInfo, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
}


// ----- VK_EXT_debug_report
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateDebugReportCallbackEXT(
    VkInstance instance,
    const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugReportCallbackEXT* pCallback) {
    log_driver_message("Generated Driver vkCreateDebugReportCallbackEXT");
    *pCallback = reinterpret_cast<VkDebugReportCallbackEXT>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyDebugReportCallbackEXT(
    VkInstance instance,
    VkDebugReportCallbackEXT callback,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyDebugReportCallbackEXT");
}

VKAPI_ATTR void VKAPI_CALL driver_DebugReportMessageEXT(
    VkInstance instance,
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objectType,
    uint64_t object,
    size_t location,
    int32_t messageCode,
    const char* pLayerPrefix,
    const char* pMessage) {
    log_driver_message("Generated Driver vkDebugReportMessageEXT");
}


// ----- VK_EXT_debug_marker
VKAPI_ATTR VkResult VKAPI_CALL driver_DebugMarkerSetObjectTagEXT(
    VkDevice device,
    const VkDebugMarkerObjectTagInfoEXT* pTagInfo) {
    log_driver_message("Generated Driver vkDebugMarkerSetObjectTagEXT");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_DebugMarkerSetObjectNameEXT(
    VkDevice device,
    const VkDebugMarkerObjectNameInfoEXT* pNameInfo) {
    log_driver_message("Generated Driver vkDebugMarkerSetObjectNameEXT");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_CmdDebugMarkerBeginEXT(
    VkCommandBuffer commandBuffer,
    const VkDebugMarkerMarkerInfoEXT* pMarkerInfo) {
    log_driver_message("Generated Driver vkCmdDebugMarkerBeginEXT");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdDebugMarkerEndEXT(
    VkCommandBuffer commandBuffer) {
    log_driver_message("Generated Driver vkCmdDebugMarkerEndEXT");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdDebugMarkerInsertEXT(
    VkCommandBuffer commandBuffer,
    const VkDebugMarkerMarkerInfoEXT* pMarkerInfo) {
    log_driver_message("Generated Driver vkCmdDebugMarkerInsertEXT");
}


// ----- VK_EXT_transform_feedback
VKAPI_ATTR void VKAPI_CALL driver_CmdBindTransformFeedbackBuffersEXT(
    VkCommandBuffer commandBuffer,
    uint32_t firstBinding,
    uint32_t bindingCount,
    const VkBuffer* pBuffers,
    const VkDeviceSize* pOffsets,
    const VkDeviceSize* pSizes) {
    log_driver_message("Generated Driver vkCmdBindTransformFeedbackBuffersEXT");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBeginTransformFeedbackEXT(
    VkCommandBuffer commandBuffer,
    uint32_t firstCounterBuffer,
    uint32_t counterBufferCount,
    const VkBuffer* pCounterBuffers,
    const VkDeviceSize* pCounterBufferOffsets) {
    log_driver_message("Generated Driver vkCmdBeginTransformFeedbackEXT");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdEndTransformFeedbackEXT(
    VkCommandBuffer commandBuffer,
    uint32_t firstCounterBuffer,
    uint32_t counterBufferCount,
    const VkBuffer* pCounterBuffers,
    const VkDeviceSize* pCounterBufferOffsets) {
    log_driver_message("Generated Driver vkCmdEndTransformFeedbackEXT");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBeginQueryIndexedEXT(
    VkCommandBuffer commandBuffer,
    VkQueryPool queryPool,
    uint32_t query,
    VkQueryControlFlags flags,
    uint32_t index) {
    log_driver_message("Generated Driver vkCmdBeginQueryIndexedEXT");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdEndQueryIndexedEXT(
    VkCommandBuffer commandBuffer,
    VkQueryPool queryPool,
    uint32_t query,
    uint32_t index) {
    log_driver_message("Generated Driver vkCmdEndQueryIndexedEXT");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdDrawIndirectByteCountEXT(
    VkCommandBuffer commandBuffer,
    uint32_t instanceCount,
    uint32_t firstInstance,
    VkBuffer counterBuffer,
    VkDeviceSize counterBufferOffset,
    uint32_t counterOffset,
    uint32_t vertexStride) {
    log_driver_message("Generated Driver vkCmdDrawIndirectByteCountEXT");
}


// ----- VK_NVX_binary_import
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateCuModuleNVX(
    VkDevice device,
    const VkCuModuleCreateInfoNVX* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkCuModuleNVX* pModule) {
    log_driver_message("Generated Driver vkCreateCuModuleNVX");
    *pModule = reinterpret_cast<VkCuModuleNVX>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateCuFunctionNVX(
    VkDevice device,
    const VkCuFunctionCreateInfoNVX* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkCuFunctionNVX* pFunction) {
    log_driver_message("Generated Driver vkCreateCuFunctionNVX");
    *pFunction = reinterpret_cast<VkCuFunctionNVX>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyCuModuleNVX(
    VkDevice device,
    VkCuModuleNVX module,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyCuModuleNVX");
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyCuFunctionNVX(
    VkDevice device,
    VkCuFunctionNVX function,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyCuFunctionNVX");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdCuLaunchKernelNVX(
    VkCommandBuffer commandBuffer,
    const VkCuLaunchInfoNVX* pLaunchInfo) {
    log_driver_message("Generated Driver vkCmdCuLaunchKernelNVX");
}


// ----- VK_NVX_image_view_handle
VKAPI_ATTR uint32_t VKAPI_CALL driver_GetImageViewHandleNVX(
    VkDevice device,
    const VkImageViewHandleInfoNVX* pInfo) {
    log_driver_message("Generated Driver vkGetImageViewHandleNVX");
    return static_cast<uint32_t>(0);
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetImageViewAddressNVX(
    VkDevice device,
    VkImageView imageView,
    VkImageViewAddressPropertiesNVX* pProperties) {
    log_driver_message("Generated Driver vkGetImageViewAddressNVX");
    return VK_SUCCESS;
}


// ----- VK_AMD_draw_indirect_count
VKAPI_ATTR void VKAPI_CALL driver_CmdDrawIndirectCountAMD(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    VkBuffer countBuffer,
    VkDeviceSize countBufferOffset,
    uint32_t maxDrawCount,
    uint32_t stride) {
    log_driver_message("Generated Driver vkCmdDrawIndirectCountAMD");
    return driver_CmdDrawIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdDrawIndexedIndirectCountAMD(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    VkBuffer countBuffer,
    VkDeviceSize countBufferOffset,
    uint32_t maxDrawCount,
    uint32_t stride) {
    log_driver_message("Generated Driver vkCmdDrawIndexedIndirectCountAMD");
    return driver_CmdDrawIndexedIndirectCount(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}


// ----- VK_AMD_shader_info
VKAPI_ATTR VkResult VKAPI_CALL driver_GetShaderInfoAMD(
    VkDevice device,
    VkPipeline pipeline,
    VkShaderStageFlagBits shaderStage,
    VkShaderInfoTypeAMD infoType,
    size_t* pInfoSize,
    void* pInfo) {
    log_driver_message("Generated Driver vkGetShaderInfoAMD");
    return VK_SUCCESS;
}


// ----- VK_GGP_stream_descriptor_surface
#ifdef VK_USE_PLATFORM_GGP
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateStreamDescriptorSurfaceGGP(
    VkInstance instance,
    const VkStreamDescriptorSurfaceCreateInfoGGP* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_driver_message("Generated Driver vkCreateStreamDescriptorSurfaceGGP");
    *pSurface = reinterpret_cast<VkSurfaceKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

#endif // VK_USE_PLATFORM_GGP

// ----- VK_NV_external_memory_capabilities
VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceExternalImageFormatPropertiesNV(
    VkPhysicalDevice physicalDevice,
    VkFormat format,
    VkImageType type,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkImageCreateFlags flags,
    VkExternalMemoryHandleTypeFlagsNV externalHandleType,
    VkExternalImageFormatPropertiesNV* pExternalImageFormatProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceExternalImageFormatPropertiesNV");
    return VK_SUCCESS;
}


// ----- VK_NV_external_memory_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
VKAPI_ATTR VkResult VKAPI_CALL driver_GetMemoryWin32HandleNV(
    VkDevice device,
    VkDeviceMemory memory,
    VkExternalMemoryHandleTypeFlagsNV handleType,
    HANDLE* pHandle) {
    log_driver_message("Generated Driver vkGetMemoryWin32HandleNV");
    return VK_SUCCESS;
}

#endif // VK_USE_PLATFORM_WIN32_KHR

// ----- VK_NN_vi_surface
#ifdef VK_USE_PLATFORM_VI_NN
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateViSurfaceNN(
    VkInstance instance,
    const VkViSurfaceCreateInfoNN* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_driver_message("Generated Driver vkCreateViSurfaceNN");
    *pSurface = reinterpret_cast<VkSurfaceKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

#endif // VK_USE_PLATFORM_VI_NN

// ----- VK_EXT_conditional_rendering
VKAPI_ATTR void VKAPI_CALL driver_CmdBeginConditionalRenderingEXT(
    VkCommandBuffer commandBuffer,
    const VkConditionalRenderingBeginInfoEXT* pConditionalRenderingBegin) {
    log_driver_message("Generated Driver vkCmdBeginConditionalRenderingEXT");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdEndConditionalRenderingEXT(
    VkCommandBuffer commandBuffer) {
    log_driver_message("Generated Driver vkCmdEndConditionalRenderingEXT");
}


// ----- VK_NV_clip_space_w_scaling
VKAPI_ATTR void VKAPI_CALL driver_CmdSetViewportWScalingNV(
    VkCommandBuffer commandBuffer,
    uint32_t firstViewport,
    uint32_t viewportCount,
    const VkViewportWScalingNV* pViewportWScalings) {
    log_driver_message("Generated Driver vkCmdSetViewportWScalingNV");
}


// ----- VK_EXT_direct_mode_display
VKAPI_ATTR VkResult VKAPI_CALL driver_ReleaseDisplayEXT(
    VkPhysicalDevice physicalDevice,
    VkDisplayKHR display) {
    log_driver_message("Generated Driver vkReleaseDisplayEXT");
    return VK_SUCCESS;
}


// ----- VK_EXT_acquire_xlib_display
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
VKAPI_ATTR VkResult VKAPI_CALL driver_AcquireXlibDisplayEXT(
    VkPhysicalDevice physicalDevice,
    Display* dpy,
    VkDisplayKHR display) {
    log_driver_message("Generated Driver vkAcquireXlibDisplayEXT");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetRandROutputDisplayEXT(
    VkPhysicalDevice physicalDevice,
    Display* dpy,
    RROutput rrOutput,
    VkDisplayKHR* pDisplay) {
    log_driver_message("Generated Driver vkGetRandROutputDisplayEXT");
    *pDisplay = reinterpret_cast<VkDisplayKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

#endif // VK_USE_PLATFORM_XLIB_XRANDR_EXT

// ----- VK_EXT_display_surface_counter
VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceSurfaceCapabilities2EXT(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    VkSurfaceCapabilities2EXT* pSurfaceCapabilities) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceSurfaceCapabilities2EXT");
    return VK_SUCCESS;
}


// ----- VK_EXT_display_control
VKAPI_ATTR VkResult VKAPI_CALL driver_DisplayPowerControlEXT(
    VkDevice device,
    VkDisplayKHR display,
    const VkDisplayPowerInfoEXT* pDisplayPowerInfo) {
    log_driver_message("Generated Driver vkDisplayPowerControlEXT");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_RegisterDeviceEventEXT(
    VkDevice device,
    const VkDeviceEventInfoEXT* pDeviceEventInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFence* pFence) {
    log_driver_message("Generated Driver vkRegisterDeviceEventEXT");
    *pFence = reinterpret_cast<VkFence>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_RegisterDisplayEventEXT(
    VkDevice device,
    VkDisplayKHR display,
    const VkDisplayEventInfoEXT* pDisplayEventInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFence* pFence) {
    log_driver_message("Generated Driver vkRegisterDisplayEventEXT");
    *pFence = reinterpret_cast<VkFence>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetSwapchainCounterEXT(
    VkDevice device,
    VkSwapchainKHR swapchain,
    VkSurfaceCounterFlagBitsEXT counter,
    uint64_t* pCounterValue) {
    log_driver_message("Generated Driver vkGetSwapchainCounterEXT");
    return VK_SUCCESS;
}


// ----- VK_GOOGLE_display_timing
VKAPI_ATTR VkResult VKAPI_CALL driver_GetRefreshCycleDurationGOOGLE(
    VkDevice device,
    VkSwapchainKHR swapchain,
    VkRefreshCycleDurationGOOGLE* pDisplayTimingProperties) {
    log_driver_message("Generated Driver vkGetRefreshCycleDurationGOOGLE");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetPastPresentationTimingGOOGLE(
    VkDevice device,
    VkSwapchainKHR swapchain,
    uint32_t* pPresentationTimingCount,
    VkPastPresentationTimingGOOGLE* pPresentationTimings) {
    log_driver_message("Generated Driver vkGetPastPresentationTimingGOOGLE");
    return VK_SUCCESS;
}


// ----- VK_EXT_discard_rectangles
VKAPI_ATTR void VKAPI_CALL driver_CmdSetDiscardRectangleEXT(
    VkCommandBuffer commandBuffer,
    uint32_t firstDiscardRectangle,
    uint32_t discardRectangleCount,
    const VkRect2D* pDiscardRectangles) {
    log_driver_message("Generated Driver vkCmdSetDiscardRectangleEXT");
}


// ----- VK_EXT_hdr_metadata
VKAPI_ATTR void VKAPI_CALL driver_SetHdrMetadataEXT(
    VkDevice device,
    uint32_t swapchainCount,
    const VkSwapchainKHR* pSwapchains,
    const VkHdrMetadataEXT* pMetadata) {
    log_driver_message("Generated Driver vkSetHdrMetadataEXT");
}


// ----- VK_MVK_ios_surface
#ifdef VK_USE_PLATFORM_IOS_MVK
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateIOSSurfaceMVK(
    VkInstance instance,
    const VkIOSSurfaceCreateInfoMVK* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_driver_message("Generated Driver vkCreateIOSSurfaceMVK");
    *pSurface = reinterpret_cast<VkSurfaceKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

#endif // VK_USE_PLATFORM_IOS_MVK

// ----- VK_MVK_macos_surface
#ifdef VK_USE_PLATFORM_MACOS_MVK
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateMacOSSurfaceMVK(
    VkInstance instance,
    const VkMacOSSurfaceCreateInfoMVK* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_driver_message("Generated Driver vkCreateMacOSSurfaceMVK");
    *pSurface = reinterpret_cast<VkSurfaceKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

#endif // VK_USE_PLATFORM_MACOS_MVK

// ----- VK_EXT_debug_utils
VKAPI_ATTR VkResult VKAPI_CALL driver_SetDebugUtilsObjectNameEXT(
    VkDevice device,
    const VkDebugUtilsObjectNameInfoEXT* pNameInfo) {
    log_driver_message("Generated Driver vkSetDebugUtilsObjectNameEXT");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_SetDebugUtilsObjectTagEXT(
    VkDevice device,
    const VkDebugUtilsObjectTagInfoEXT* pTagInfo) {
    log_driver_message("Generated Driver vkSetDebugUtilsObjectTagEXT");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_QueueBeginDebugUtilsLabelEXT(
    VkQueue queue,
    const VkDebugUtilsLabelEXT* pLabelInfo) {
    log_driver_message("Generated Driver vkQueueBeginDebugUtilsLabelEXT");
}

VKAPI_ATTR void VKAPI_CALL driver_QueueEndDebugUtilsLabelEXT(
    VkQueue queue) {
    log_driver_message("Generated Driver vkQueueEndDebugUtilsLabelEXT");
}

VKAPI_ATTR void VKAPI_CALL driver_QueueInsertDebugUtilsLabelEXT(
    VkQueue queue,
    const VkDebugUtilsLabelEXT* pLabelInfo) {
    log_driver_message("Generated Driver vkQueueInsertDebugUtilsLabelEXT");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBeginDebugUtilsLabelEXT(
    VkCommandBuffer commandBuffer,
    const VkDebugUtilsLabelEXT* pLabelInfo) {
    log_driver_message("Generated Driver vkCmdBeginDebugUtilsLabelEXT");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdEndDebugUtilsLabelEXT(
    VkCommandBuffer commandBuffer) {
    log_driver_message("Generated Driver vkCmdEndDebugUtilsLabelEXT");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdInsertDebugUtilsLabelEXT(
    VkCommandBuffer commandBuffer,
    const VkDebugUtilsLabelEXT* pLabelInfo) {
    log_driver_message("Generated Driver vkCmdInsertDebugUtilsLabelEXT");
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT messenger,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyDebugUtilsMessengerEXT");
}

VKAPI_ATTR void VKAPI_CALL driver_SubmitDebugUtilsMessageEXT(
    VkInstance instance,
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData) {
    log_driver_message("Generated Driver vkSubmitDebugUtilsMessageEXT");
}


// ----- VK_EXT_sample_locations
VKAPI_ATTR void VKAPI_CALL driver_CmdSetSampleLocationsEXT(
    VkCommandBuffer commandBuffer,
    const VkSampleLocationsInfoEXT* pSampleLocationsInfo) {
    log_driver_message("Generated Driver vkCmdSetSampleLocationsEXT");
}

VKAPI_ATTR void VKAPI_CALL driver_GetPhysicalDeviceMultisamplePropertiesEXT(
    VkPhysicalDevice physicalDevice,
    VkSampleCountFlagBits samples,
    VkMultisamplePropertiesEXT* pMultisampleProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceMultisamplePropertiesEXT");
}


// ----- VK_EXT_image_drm_format_modifier
VKAPI_ATTR VkResult VKAPI_CALL driver_GetImageDrmFormatModifierPropertiesEXT(
    VkDevice device,
    VkImage image,
    VkImageDrmFormatModifierPropertiesEXT* pProperties) {
    log_driver_message("Generated Driver vkGetImageDrmFormatModifierPropertiesEXT");
    return VK_SUCCESS;
}


// ----- VK_EXT_validation_cache
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateValidationCacheEXT(
    VkDevice device,
    const VkValidationCacheCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkValidationCacheEXT* pValidationCache) {
    log_driver_message("Generated Driver vkCreateValidationCacheEXT");
    *pValidationCache = reinterpret_cast<VkValidationCacheEXT>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyValidationCacheEXT(
    VkDevice device,
    VkValidationCacheEXT validationCache,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyValidationCacheEXT");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_MergeValidationCachesEXT(
    VkDevice device,
    VkValidationCacheEXT dstCache,
    uint32_t srcCacheCount,
    const VkValidationCacheEXT* pSrcCaches) {
    log_driver_message("Generated Driver vkMergeValidationCachesEXT");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetValidationCacheDataEXT(
    VkDevice device,
    VkValidationCacheEXT validationCache,
    size_t* pDataSize,
    void* pData) {
    log_driver_message("Generated Driver vkGetValidationCacheDataEXT");
    return VK_SUCCESS;
}


// ----- VK_NV_shading_rate_image
VKAPI_ATTR void VKAPI_CALL driver_CmdBindShadingRateImageNV(
    VkCommandBuffer commandBuffer,
    VkImageView imageView,
    VkImageLayout imageLayout) {
    log_driver_message("Generated Driver vkCmdBindShadingRateImageNV");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetViewportShadingRatePaletteNV(
    VkCommandBuffer commandBuffer,
    uint32_t firstViewport,
    uint32_t viewportCount,
    const VkShadingRatePaletteNV* pShadingRatePalettes) {
    log_driver_message("Generated Driver vkCmdSetViewportShadingRatePaletteNV");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetCoarseSampleOrderNV(
    VkCommandBuffer commandBuffer,
    VkCoarseSampleOrderTypeNV sampleOrderType,
    uint32_t customSampleOrderCount,
    const VkCoarseSampleOrderCustomNV* pCustomSampleOrders) {
    log_driver_message("Generated Driver vkCmdSetCoarseSampleOrderNV");
}


// ----- VK_NV_ray_tracing
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateAccelerationStructureNV(
    VkDevice device,
    const VkAccelerationStructureCreateInfoNV* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkAccelerationStructureNV* pAccelerationStructure) {
    log_driver_message("Generated Driver vkCreateAccelerationStructureNV");
    *pAccelerationStructure = reinterpret_cast<VkAccelerationStructureNV>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyAccelerationStructureNV(
    VkDevice device,
    VkAccelerationStructureNV accelerationStructure,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyAccelerationStructureNV");
}

VKAPI_ATTR void VKAPI_CALL driver_GetAccelerationStructureMemoryRequirementsNV(
    VkDevice device,
    const VkAccelerationStructureMemoryRequirementsInfoNV* pInfo,
    VkMemoryRequirements2KHR* pMemoryRequirements) {
    log_driver_message("Generated Driver vkGetAccelerationStructureMemoryRequirementsNV");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_BindAccelerationStructureMemoryNV(
    VkDevice device,
    uint32_t bindInfoCount,
    const VkBindAccelerationStructureMemoryInfoNV* pBindInfos) {
    log_driver_message("Generated Driver vkBindAccelerationStructureMemoryNV");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBuildAccelerationStructureNV(
    VkCommandBuffer commandBuffer,
    const VkAccelerationStructureInfoNV* pInfo,
    VkBuffer instanceData,
    VkDeviceSize instanceOffset,
    VkBool32 update,
    VkAccelerationStructureNV dst,
    VkAccelerationStructureNV src,
    VkBuffer scratch,
    VkDeviceSize scratchOffset) {
    log_driver_message("Generated Driver vkCmdBuildAccelerationStructureNV");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdCopyAccelerationStructureNV(
    VkCommandBuffer commandBuffer,
    VkAccelerationStructureNV dst,
    VkAccelerationStructureNV src,
    VkCopyAccelerationStructureModeKHR mode) {
    log_driver_message("Generated Driver vkCmdCopyAccelerationStructureNV");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdTraceRaysNV(
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
    log_driver_message("Generated Driver vkCmdTraceRaysNV");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateRayTracingPipelinesNV(
    VkDevice device,
    VkPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const VkRayTracingPipelineCreateInfoNV* pCreateInfos,
    const VkAllocationCallbacks* pAllocator,
    VkPipeline* pPipelines) {
    log_driver_message("Generated Driver vkCreateRayTracingPipelinesNV");
    *pPipelines = reinterpret_cast<VkPipeline>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetRayTracingShaderGroupHandlesNV(
    VkDevice device,
    VkPipeline pipeline,
    uint32_t firstGroup,
    uint32_t groupCount,
    size_t dataSize,
    void* pData) {
    log_driver_message("Generated Driver vkGetRayTracingShaderGroupHandlesNV");
    return driver_GetRayTracingShaderGroupHandlesKHR(device, pipeline, firstGroup, groupCount, dataSize, pData);
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetAccelerationStructureHandleNV(
    VkDevice device,
    VkAccelerationStructureNV accelerationStructure,
    size_t dataSize,
    void* pData) {
    log_driver_message("Generated Driver vkGetAccelerationStructureHandleNV");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_CmdWriteAccelerationStructuresPropertiesNV(
    VkCommandBuffer commandBuffer,
    uint32_t accelerationStructureCount,
    const VkAccelerationStructureNV* pAccelerationStructures,
    VkQueryType queryType,
    VkQueryPool queryPool,
    uint32_t firstQuery) {
    log_driver_message("Generated Driver vkCmdWriteAccelerationStructuresPropertiesNV");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CompileDeferredNV(
    VkDevice device,
    VkPipeline pipeline,
    uint32_t shader) {
    log_driver_message("Generated Driver vkCompileDeferredNV");
    return VK_SUCCESS;
}


// ----- VK_EXT_external_memory_host
VKAPI_ATTR VkResult VKAPI_CALL driver_GetMemoryHostPointerPropertiesEXT(
    VkDevice device,
    VkExternalMemoryHandleTypeFlagBits handleType,
    const void* pHostPointer,
    VkMemoryHostPointerPropertiesEXT* pMemoryHostPointerProperties) {
    log_driver_message("Generated Driver vkGetMemoryHostPointerPropertiesEXT");
    return VK_SUCCESS;
}


// ----- VK_AMD_buffer_marker
VKAPI_ATTR void VKAPI_CALL driver_CmdWriteBufferMarkerAMD(
    VkCommandBuffer commandBuffer,
    VkPipelineStageFlagBits pipelineStage,
    VkBuffer dstBuffer,
    VkDeviceSize dstOffset,
    uint32_t marker) {
    log_driver_message("Generated Driver vkCmdWriteBufferMarkerAMD");
}


// ----- VK_EXT_calibrated_timestamps
VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceCalibrateableTimeDomainsEXT(
    VkPhysicalDevice physicalDevice,
    uint32_t* pTimeDomainCount,
    VkTimeDomainEXT* pTimeDomains) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceCalibrateableTimeDomainsEXT");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetCalibratedTimestampsEXT(
    VkDevice device,
    uint32_t timestampCount,
    const VkCalibratedTimestampInfoEXT* pTimestampInfos,
    uint64_t* pTimestamps,
    uint64_t* pMaxDeviation) {
    log_driver_message("Generated Driver vkGetCalibratedTimestampsEXT");
    return VK_SUCCESS;
}


// ----- VK_NV_mesh_shader
VKAPI_ATTR void VKAPI_CALL driver_CmdDrawMeshTasksNV(
    VkCommandBuffer commandBuffer,
    uint32_t taskCount,
    uint32_t firstTask) {
    log_driver_message("Generated Driver vkCmdDrawMeshTasksNV");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdDrawMeshTasksIndirectNV(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride) {
    log_driver_message("Generated Driver vkCmdDrawMeshTasksIndirectNV");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdDrawMeshTasksIndirectCountNV(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    VkBuffer countBuffer,
    VkDeviceSize countBufferOffset,
    uint32_t maxDrawCount,
    uint32_t stride) {
    log_driver_message("Generated Driver vkCmdDrawMeshTasksIndirectCountNV");
}


// ----- VK_NV_scissor_exclusive
VKAPI_ATTR void VKAPI_CALL driver_CmdSetExclusiveScissorNV(
    VkCommandBuffer commandBuffer,
    uint32_t firstExclusiveScissor,
    uint32_t exclusiveScissorCount,
    const VkRect2D* pExclusiveScissors) {
    log_driver_message("Generated Driver vkCmdSetExclusiveScissorNV");
}


// ----- VK_NV_device_diagnostic_checkpoints
VKAPI_ATTR void VKAPI_CALL driver_CmdSetCheckpointNV(
    VkCommandBuffer commandBuffer,
    const void* pCheckpointMarker) {
    log_driver_message("Generated Driver vkCmdSetCheckpointNV");
}

VKAPI_ATTR void VKAPI_CALL driver_GetQueueCheckpointDataNV(
    VkQueue queue,
    uint32_t* pCheckpointDataCount,
    VkCheckpointDataNV* pCheckpointData) {
    log_driver_message("Generated Driver vkGetQueueCheckpointDataNV");
}


// ----- VK_INTEL_performance_query
VKAPI_ATTR VkResult VKAPI_CALL driver_InitializePerformanceApiINTEL(
    VkDevice device,
    const VkInitializePerformanceApiInfoINTEL* pInitializeInfo) {
    log_driver_message("Generated Driver vkInitializePerformanceApiINTEL");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_UninitializePerformanceApiINTEL(
    VkDevice device) {
    log_driver_message("Generated Driver vkUninitializePerformanceApiINTEL");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CmdSetPerformanceMarkerINTEL(
    VkCommandBuffer commandBuffer,
    const VkPerformanceMarkerInfoINTEL* pMarkerInfo) {
    log_driver_message("Generated Driver vkCmdSetPerformanceMarkerINTEL");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CmdSetPerformanceStreamMarkerINTEL(
    VkCommandBuffer commandBuffer,
    const VkPerformanceStreamMarkerInfoINTEL* pMarkerInfo) {
    log_driver_message("Generated Driver vkCmdSetPerformanceStreamMarkerINTEL");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CmdSetPerformanceOverrideINTEL(
    VkCommandBuffer commandBuffer,
    const VkPerformanceOverrideInfoINTEL* pOverrideInfo) {
    log_driver_message("Generated Driver vkCmdSetPerformanceOverrideINTEL");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_AcquirePerformanceConfigurationINTEL(
    VkDevice device,
    const VkPerformanceConfigurationAcquireInfoINTEL* pAcquireInfo,
    VkPerformanceConfigurationINTEL* pConfiguration) {
    log_driver_message("Generated Driver vkAcquirePerformanceConfigurationINTEL");
    *pConfiguration = reinterpret_cast<VkPerformanceConfigurationINTEL>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_ReleasePerformanceConfigurationINTEL(
    VkDevice device,
    VkPerformanceConfigurationINTEL configuration) {
    log_driver_message("Generated Driver vkReleasePerformanceConfigurationINTEL");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_QueueSetPerformanceConfigurationINTEL(
    VkQueue queue,
    VkPerformanceConfigurationINTEL configuration) {
    log_driver_message("Generated Driver vkQueueSetPerformanceConfigurationINTEL");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetPerformanceParameterINTEL(
    VkDevice device,
    VkPerformanceParameterTypeINTEL parameter,
    VkPerformanceValueINTEL* pValue) {
    log_driver_message("Generated Driver vkGetPerformanceParameterINTEL");
    return VK_SUCCESS;
}


// ----- VK_AMD_display_native_hdr
VKAPI_ATTR void VKAPI_CALL driver_SetLocalDimmingAMD(
    VkDevice device,
    VkSwapchainKHR swapChain,
    VkBool32 localDimmingEnable) {
    log_driver_message("Generated Driver vkSetLocalDimmingAMD");
}


// ----- VK_FUCHSIA_imagepipe_surface
#ifdef VK_USE_PLATFORM_FUCHSIA
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateImagePipeSurfaceFUCHSIA(
    VkInstance instance,
    const VkImagePipeSurfaceCreateInfoFUCHSIA* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_driver_message("Generated Driver vkCreateImagePipeSurfaceFUCHSIA");
    *pSurface = reinterpret_cast<VkSurfaceKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

#endif // VK_USE_PLATFORM_FUCHSIA

// ----- VK_EXT_metal_surface
#ifdef VK_USE_PLATFORM_METAL_EXT
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateMetalSurfaceEXT(
    VkInstance instance,
    const VkMetalSurfaceCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_driver_message("Generated Driver vkCreateMetalSurfaceEXT");
    *pSurface = reinterpret_cast<VkSurfaceKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

#endif // VK_USE_PLATFORM_METAL_EXT

// ----- VK_EXT_buffer_device_address
VKAPI_ATTR VkDeviceAddress VKAPI_CALL driver_GetBufferDeviceAddressEXT(
    VkDevice device,
    const VkBufferDeviceAddressInfo* pInfo) {
    log_driver_message("Generated Driver vkGetBufferDeviceAddressEXT");
    return driver_GetBufferDeviceAddress(device, pInfo);
}


// ----- VK_EXT_tooling_info
VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceToolPropertiesEXT(
    VkPhysicalDevice physicalDevice,
    uint32_t* pToolCount,
    VkPhysicalDeviceToolProperties* pToolProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceToolPropertiesEXT");
    return driver_GetPhysicalDeviceToolProperties(physicalDevice, pToolCount, pToolProperties);
}


// ----- VK_NV_cooperative_matrix
VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceCooperativeMatrixPropertiesNV(
    VkPhysicalDevice physicalDevice,
    uint32_t* pPropertyCount,
    VkCooperativeMatrixPropertiesNV* pProperties) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceCooperativeMatrixPropertiesNV");
    return VK_SUCCESS;
}


// ----- VK_NV_coverage_reduction_mode
VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV(
    VkPhysicalDevice physicalDevice,
    uint32_t* pCombinationCount,
    VkFramebufferMixedSamplesCombinationNV* pCombinations) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV");
    return VK_SUCCESS;
}


// ----- VK_EXT_full_screen_exclusive
#ifdef VK_USE_PLATFORM_WIN32_KHR
VKAPI_ATTR VkResult VKAPI_CALL driver_GetPhysicalDeviceSurfacePresentModes2EXT(
    VkPhysicalDevice physicalDevice,
    const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
    uint32_t* pPresentModeCount,
    VkPresentModeKHR* pPresentModes) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceSurfacePresentModes2EXT");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_AcquireFullScreenExclusiveModeEXT(
    VkDevice device,
    VkSwapchainKHR swapchain) {
    log_driver_message("Generated Driver vkAcquireFullScreenExclusiveModeEXT");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_ReleaseFullScreenExclusiveModeEXT(
    VkDevice device,
    VkSwapchainKHR swapchain) {
    log_driver_message("Generated Driver vkReleaseFullScreenExclusiveModeEXT");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetDeviceGroupSurfacePresentModes2EXT(
    VkDevice device,
    const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
    VkDeviceGroupPresentModeFlagsKHR* pModes) {
    log_driver_message("Generated Driver vkGetDeviceGroupSurfacePresentModes2EXT");
    return VK_SUCCESS;
}

#endif // VK_USE_PLATFORM_WIN32_KHR

// ----- VK_EXT_headless_surface
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateHeadlessSurfaceEXT(
    VkInstance instance,
    const VkHeadlessSurfaceCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_driver_message("Generated Driver vkCreateHeadlessSurfaceEXT");
    *pSurface = reinterpret_cast<VkSurfaceKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}


// ----- VK_EXT_line_rasterization
VKAPI_ATTR void VKAPI_CALL driver_CmdSetLineStippleEXT(
    VkCommandBuffer commandBuffer,
    uint32_t lineStippleFactor,
    uint16_t lineStipplePattern) {
    log_driver_message("Generated Driver vkCmdSetLineStippleEXT");
}


// ----- VK_EXT_host_query_reset
VKAPI_ATTR void VKAPI_CALL driver_ResetQueryPoolEXT(
    VkDevice device,
    VkQueryPool queryPool,
    uint32_t firstQuery,
    uint32_t queryCount) {
    log_driver_message("Generated Driver vkResetQueryPoolEXT");
    return driver_ResetQueryPool(device, queryPool, firstQuery, queryCount);
}


// ----- VK_EXT_extended_dynamic_state
VKAPI_ATTR void VKAPI_CALL driver_CmdSetCullModeEXT(
    VkCommandBuffer commandBuffer,
    VkCullModeFlags cullMode) {
    log_driver_message("Generated Driver vkCmdSetCullModeEXT");
    return driver_CmdSetCullMode(commandBuffer, cullMode);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetFrontFaceEXT(
    VkCommandBuffer commandBuffer,
    VkFrontFace frontFace) {
    log_driver_message("Generated Driver vkCmdSetFrontFaceEXT");
    return driver_CmdSetFrontFace(commandBuffer, frontFace);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetPrimitiveTopologyEXT(
    VkCommandBuffer commandBuffer,
    VkPrimitiveTopology primitiveTopology) {
    log_driver_message("Generated Driver vkCmdSetPrimitiveTopologyEXT");
    return driver_CmdSetPrimitiveTopology(commandBuffer, primitiveTopology);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetViewportWithCountEXT(
    VkCommandBuffer commandBuffer,
    uint32_t viewportCount,
    const VkViewport* pViewports) {
    log_driver_message("Generated Driver vkCmdSetViewportWithCountEXT");
    return driver_CmdSetViewportWithCount(commandBuffer, viewportCount, pViewports);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetScissorWithCountEXT(
    VkCommandBuffer commandBuffer,
    uint32_t scissorCount,
    const VkRect2D* pScissors) {
    log_driver_message("Generated Driver vkCmdSetScissorWithCountEXT");
    return driver_CmdSetScissorWithCount(commandBuffer, scissorCount, pScissors);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBindVertexBuffers2EXT(
    VkCommandBuffer commandBuffer,
    uint32_t firstBinding,
    uint32_t bindingCount,
    const VkBuffer* pBuffers,
    const VkDeviceSize* pOffsets,
    const VkDeviceSize* pSizes,
    const VkDeviceSize* pStrides) {
    log_driver_message("Generated Driver vkCmdBindVertexBuffers2EXT");
    return driver_CmdBindVertexBuffers2(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets, pSizes, pStrides);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetDepthTestEnableEXT(
    VkCommandBuffer commandBuffer,
    VkBool32 depthTestEnable) {
    log_driver_message("Generated Driver vkCmdSetDepthTestEnableEXT");
    return driver_CmdSetDepthTestEnable(commandBuffer, depthTestEnable);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetDepthWriteEnableEXT(
    VkCommandBuffer commandBuffer,
    VkBool32 depthWriteEnable) {
    log_driver_message("Generated Driver vkCmdSetDepthWriteEnableEXT");
    return driver_CmdSetDepthWriteEnable(commandBuffer, depthWriteEnable);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetDepthCompareOpEXT(
    VkCommandBuffer commandBuffer,
    VkCompareOp depthCompareOp) {
    log_driver_message("Generated Driver vkCmdSetDepthCompareOpEXT");
    return driver_CmdSetDepthCompareOp(commandBuffer, depthCompareOp);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetDepthBoundsTestEnableEXT(
    VkCommandBuffer commandBuffer,
    VkBool32 depthBoundsTestEnable) {
    log_driver_message("Generated Driver vkCmdSetDepthBoundsTestEnableEXT");
    return driver_CmdSetDepthBoundsTestEnable(commandBuffer, depthBoundsTestEnable);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetStencilTestEnableEXT(
    VkCommandBuffer commandBuffer,
    VkBool32 stencilTestEnable) {
    log_driver_message("Generated Driver vkCmdSetStencilTestEnableEXT");
    return driver_CmdSetStencilTestEnable(commandBuffer, stencilTestEnable);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetStencilOpEXT(
    VkCommandBuffer commandBuffer,
    VkStencilFaceFlags faceMask,
    VkStencilOp failOp,
    VkStencilOp passOp,
    VkStencilOp depthFailOp,
    VkCompareOp compareOp) {
    log_driver_message("Generated Driver vkCmdSetStencilOpEXT");
    return driver_CmdSetStencilOp(commandBuffer, faceMask, failOp, passOp, depthFailOp, compareOp);
}


// ----- VK_NV_device_generated_commands
VKAPI_ATTR void VKAPI_CALL driver_GetGeneratedCommandsMemoryRequirementsNV(
    VkDevice device,
    const VkGeneratedCommandsMemoryRequirementsInfoNV* pInfo,
    VkMemoryRequirements2* pMemoryRequirements) {
    log_driver_message("Generated Driver vkGetGeneratedCommandsMemoryRequirementsNV");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdPreprocessGeneratedCommandsNV(
    VkCommandBuffer commandBuffer,
    const VkGeneratedCommandsInfoNV* pGeneratedCommandsInfo) {
    log_driver_message("Generated Driver vkCmdPreprocessGeneratedCommandsNV");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdExecuteGeneratedCommandsNV(
    VkCommandBuffer commandBuffer,
    VkBool32 isPreprocessed,
    const VkGeneratedCommandsInfoNV* pGeneratedCommandsInfo) {
    log_driver_message("Generated Driver vkCmdExecuteGeneratedCommandsNV");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBindPipelineShaderGroupNV(
    VkCommandBuffer commandBuffer,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipeline pipeline,
    uint32_t groupIndex) {
    log_driver_message("Generated Driver vkCmdBindPipelineShaderGroupNV");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateIndirectCommandsLayoutNV(
    VkDevice device,
    const VkIndirectCommandsLayoutCreateInfoNV* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkIndirectCommandsLayoutNV* pIndirectCommandsLayout) {
    log_driver_message("Generated Driver vkCreateIndirectCommandsLayoutNV");
    *pIndirectCommandsLayout = reinterpret_cast<VkIndirectCommandsLayoutNV>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyIndirectCommandsLayoutNV(
    VkDevice device,
    VkIndirectCommandsLayoutNV indirectCommandsLayout,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyIndirectCommandsLayoutNV");
}


// ----- VK_EXT_acquire_drm_display
VKAPI_ATTR VkResult VKAPI_CALL driver_AcquireDrmDisplayEXT(
    VkPhysicalDevice physicalDevice,
    int32_t drmFd,
    VkDisplayKHR display) {
    log_driver_message("Generated Driver vkAcquireDrmDisplayEXT");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetDrmDisplayEXT(
    VkPhysicalDevice physicalDevice,
    int32_t drmFd,
    uint32_t connectorId,
    VkDisplayKHR* display) {
    log_driver_message("Generated Driver vkGetDrmDisplayEXT");
    *display = reinterpret_cast<VkDisplayKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}


// ----- VK_EXT_private_data
VKAPI_ATTR VkResult VKAPI_CALL driver_CreatePrivateDataSlotEXT(
    VkDevice device,
    const VkPrivateDataSlotCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkPrivateDataSlot* pPrivateDataSlot) {
    log_driver_message("Generated Driver vkCreatePrivateDataSlotEXT");
    return driver_CreatePrivateDataSlot(device, pCreateInfo, pAllocator, pPrivateDataSlot);
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyPrivateDataSlotEXT(
    VkDevice device,
    VkPrivateDataSlot privateDataSlot,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyPrivateDataSlotEXT");
    return driver_DestroyPrivateDataSlot(device, privateDataSlot, pAllocator);
}

VKAPI_ATTR VkResult VKAPI_CALL driver_SetPrivateDataEXT(
    VkDevice device,
    VkObjectType objectType,
    uint64_t objectHandle,
    VkPrivateDataSlot privateDataSlot,
    uint64_t data) {
    log_driver_message("Generated Driver vkSetPrivateDataEXT");
    return driver_SetPrivateData(device, objectType, objectHandle, privateDataSlot, data);
}

VKAPI_ATTR void VKAPI_CALL driver_GetPrivateDataEXT(
    VkDevice device,
    VkObjectType objectType,
    uint64_t objectHandle,
    VkPrivateDataSlot privateDataSlot,
    uint64_t* pData) {
    log_driver_message("Generated Driver vkGetPrivateDataEXT");
    return driver_GetPrivateData(device, objectType, objectHandle, privateDataSlot, pData);
}


// ----- VK_NV_fragment_shading_rate_enums
VKAPI_ATTR void VKAPI_CALL driver_CmdSetFragmentShadingRateEnumNV(
    VkCommandBuffer commandBuffer,
    VkFragmentShadingRateNV shadingRate,
    const VkFragmentShadingRateCombinerOpKHR combinerOps[2]) {
    log_driver_message("Generated Driver vkCmdSetFragmentShadingRateEnumNV");
}


// ----- VK_NV_acquire_winrt_display
#ifdef VK_USE_PLATFORM_WIN32_KHR
VKAPI_ATTR VkResult VKAPI_CALL driver_AcquireWinrtDisplayNV(
    VkPhysicalDevice physicalDevice,
    VkDisplayKHR display) {
    log_driver_message("Generated Driver vkAcquireWinrtDisplayNV");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetWinrtDisplayNV(
    VkPhysicalDevice physicalDevice,
    uint32_t deviceRelativeId,
    VkDisplayKHR* pDisplay) {
    log_driver_message("Generated Driver vkGetWinrtDisplayNV");
    *pDisplay = reinterpret_cast<VkDisplayKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

#endif // VK_USE_PLATFORM_WIN32_KHR

// ----- VK_EXT_directfb_surface
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateDirectFBSurfaceEXT(
    VkInstance instance,
    const VkDirectFBSurfaceCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_driver_message("Generated Driver vkCreateDirectFBSurfaceEXT");
    *pSurface = reinterpret_cast<VkSurfaceKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR VkBool32 VKAPI_CALL driver_GetPhysicalDeviceDirectFBPresentationSupportEXT(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    IDirectFB* dfb) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceDirectFBPresentationSupportEXT");
    return VK_TRUE;
}

#endif // VK_USE_PLATFORM_DIRECTFB_EXT

// ----- VK_EXT_vertex_input_dynamic_state
VKAPI_ATTR void VKAPI_CALL driver_CmdSetVertexInputEXT(
    VkCommandBuffer commandBuffer,
    uint32_t vertexBindingDescriptionCount,
    const VkVertexInputBindingDescription2EXT* pVertexBindingDescriptions,
    uint32_t vertexAttributeDescriptionCount,
    const VkVertexInputAttributeDescription2EXT* pVertexAttributeDescriptions) {
    log_driver_message("Generated Driver vkCmdSetVertexInputEXT");
}


// ----- VK_FUCHSIA_external_memory
#ifdef VK_USE_PLATFORM_FUCHSIA
VKAPI_ATTR VkResult VKAPI_CALL driver_GetMemoryZirconHandleFUCHSIA(
    VkDevice device,
    const VkMemoryGetZirconHandleInfoFUCHSIA* pGetZirconHandleInfo,
    zx_handle_t* pZirconHandle) {
    log_driver_message("Generated Driver vkGetMemoryZirconHandleFUCHSIA");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetMemoryZirconHandlePropertiesFUCHSIA(
    VkDevice device,
    VkExternalMemoryHandleTypeFlagBits handleType,
    zx_handle_t zirconHandle,
    VkMemoryZirconHandlePropertiesFUCHSIA* pMemoryZirconHandleProperties) {
    log_driver_message("Generated Driver vkGetMemoryZirconHandlePropertiesFUCHSIA");
    return VK_SUCCESS;
}

#endif // VK_USE_PLATFORM_FUCHSIA

// ----- VK_FUCHSIA_external_semaphore
#ifdef VK_USE_PLATFORM_FUCHSIA
VKAPI_ATTR VkResult VKAPI_CALL driver_ImportSemaphoreZirconHandleFUCHSIA(
    VkDevice device,
    const VkImportSemaphoreZirconHandleInfoFUCHSIA* pImportSemaphoreZirconHandleInfo) {
    log_driver_message("Generated Driver vkImportSemaphoreZirconHandleFUCHSIA");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetSemaphoreZirconHandleFUCHSIA(
    VkDevice device,
    const VkSemaphoreGetZirconHandleInfoFUCHSIA* pGetZirconHandleInfo,
    zx_handle_t* pZirconHandle) {
    log_driver_message("Generated Driver vkGetSemaphoreZirconHandleFUCHSIA");
    return VK_SUCCESS;
}

#endif // VK_USE_PLATFORM_FUCHSIA

// ----- VK_FUCHSIA_buffer_collection
#ifdef VK_USE_PLATFORM_FUCHSIA
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateBufferCollectionFUCHSIA(
    VkDevice device,
    const VkBufferCollectionCreateInfoFUCHSIA* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkBufferCollectionFUCHSIA* pCollection) {
    log_driver_message("Generated Driver vkCreateBufferCollectionFUCHSIA");
    *pCollection = reinterpret_cast<VkBufferCollectionFUCHSIA>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_SetBufferCollectionImageConstraintsFUCHSIA(
    VkDevice device,
    VkBufferCollectionFUCHSIA collection,
    const VkImageConstraintsInfoFUCHSIA* pImageConstraintsInfo) {
    log_driver_message("Generated Driver vkSetBufferCollectionImageConstraintsFUCHSIA");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_SetBufferCollectionBufferConstraintsFUCHSIA(
    VkDevice device,
    VkBufferCollectionFUCHSIA collection,
    const VkBufferConstraintsInfoFUCHSIA* pBufferConstraintsInfo) {
    log_driver_message("Generated Driver vkSetBufferCollectionBufferConstraintsFUCHSIA");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyBufferCollectionFUCHSIA(
    VkDevice device,
    VkBufferCollectionFUCHSIA collection,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyBufferCollectionFUCHSIA");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetBufferCollectionPropertiesFUCHSIA(
    VkDevice device,
    VkBufferCollectionFUCHSIA collection,
    VkBufferCollectionPropertiesFUCHSIA* pProperties) {
    log_driver_message("Generated Driver vkGetBufferCollectionPropertiesFUCHSIA");
    return VK_SUCCESS;
}

#endif // VK_USE_PLATFORM_FUCHSIA

// ----- VK_HUAWEI_subpass_shading
VKAPI_ATTR VkResult VKAPI_CALL driver_GetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI(
    VkDevice device,
    VkRenderPass renderpass,
    VkExtent2D* pMaxWorkgroupSize) {
    log_driver_message("Generated Driver vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSubpassShadingHUAWEI(
    VkCommandBuffer commandBuffer) {
    log_driver_message("Generated Driver vkCmdSubpassShadingHUAWEI");
}


// ----- VK_HUAWEI_invocation_mask
VKAPI_ATTR void VKAPI_CALL driver_CmdBindInvocationMaskHUAWEI(
    VkCommandBuffer commandBuffer,
    VkImageView imageView,
    VkImageLayout imageLayout) {
    log_driver_message("Generated Driver vkCmdBindInvocationMaskHUAWEI");
}


// ----- VK_NV_external_memory_rdma
VKAPI_ATTR VkResult VKAPI_CALL driver_GetMemoryRemoteAddressNV(
    VkDevice device,
    const VkMemoryGetRemoteAddressInfoNV* pMemoryGetRemoteAddressInfo,
    VkRemoteAddressNV* pAddress) {
    log_driver_message("Generated Driver vkGetMemoryRemoteAddressNV");
    return VK_SUCCESS;
}


// ----- VK_EXT_extended_dynamic_state2
VKAPI_ATTR void VKAPI_CALL driver_CmdSetPatchControlPointsEXT(
    VkCommandBuffer commandBuffer,
    uint32_t patchControlPoints) {
    log_driver_message("Generated Driver vkCmdSetPatchControlPointsEXT");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetRasterizerDiscardEnableEXT(
    VkCommandBuffer commandBuffer,
    VkBool32 rasterizerDiscardEnable) {
    log_driver_message("Generated Driver vkCmdSetRasterizerDiscardEnableEXT");
    return driver_CmdSetRasterizerDiscardEnable(commandBuffer, rasterizerDiscardEnable);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetDepthBiasEnableEXT(
    VkCommandBuffer commandBuffer,
    VkBool32 depthBiasEnable) {
    log_driver_message("Generated Driver vkCmdSetDepthBiasEnableEXT");
    return driver_CmdSetDepthBiasEnable(commandBuffer, depthBiasEnable);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetLogicOpEXT(
    VkCommandBuffer commandBuffer,
    VkLogicOp logicOp) {
    log_driver_message("Generated Driver vkCmdSetLogicOpEXT");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetPrimitiveRestartEnableEXT(
    VkCommandBuffer commandBuffer,
    VkBool32 primitiveRestartEnable) {
    log_driver_message("Generated Driver vkCmdSetPrimitiveRestartEnableEXT");
    return driver_CmdSetPrimitiveRestartEnable(commandBuffer, primitiveRestartEnable);
}


// ----- VK_QNX_screen_surface
#ifdef VK_USE_PLATFORM_SCREEN_QNX
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateScreenSurfaceQNX(
    VkInstance instance,
    const VkScreenSurfaceCreateInfoQNX* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSurfaceKHR* pSurface) {
    log_driver_message("Generated Driver vkCreateScreenSurfaceQNX");
    *pSurface = reinterpret_cast<VkSurfaceKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR VkBool32 VKAPI_CALL driver_GetPhysicalDeviceScreenPresentationSupportQNX(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    struct _screen_window* window) {
    log_driver_message("Generated Driver vkGetPhysicalDeviceScreenPresentationSupportQNX");
    return VK_TRUE;
}

#endif // VK_USE_PLATFORM_SCREEN_QNX

// ----- VK_EXT_color_write_enable
VKAPI_ATTR void                                    VKAPI_CALL driver_CmdSetColorWriteEnableEXT(
    VkCommandBuffer commandBuffer,
    uint32_t attachmentCount,
    const VkBool32* pColorWriteEnables) {
    log_driver_message("Generated Driver vkCmdSetColorWriteEnableEXT");
}


// ----- VK_EXT_multi_draw
VKAPI_ATTR void VKAPI_CALL driver_CmdDrawMultiEXT(
    VkCommandBuffer commandBuffer,
    uint32_t drawCount,
    const VkMultiDrawInfoEXT* pVertexInfo,
    uint32_t instanceCount,
    uint32_t firstInstance,
    uint32_t stride) {
    log_driver_message("Generated Driver vkCmdDrawMultiEXT");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdDrawMultiIndexedEXT(
    VkCommandBuffer commandBuffer,
    uint32_t drawCount,
    const VkMultiDrawIndexedInfoEXT* pIndexInfo,
    uint32_t instanceCount,
    uint32_t firstInstance,
    uint32_t stride,
    const int32_t* pVertexOffset) {
    log_driver_message("Generated Driver vkCmdDrawMultiIndexedEXT");
}


// ----- VK_EXT_pageable_device_local_memory
VKAPI_ATTR void VKAPI_CALL driver_SetDeviceMemoryPriorityEXT(
    VkDevice device,
    VkDeviceMemory memory,
    float priority) {
    log_driver_message("Generated Driver vkSetDeviceMemoryPriorityEXT");
}


// ----- VK_VALVE_descriptor_set_host_mapping
VKAPI_ATTR void VKAPI_CALL driver_GetDescriptorSetLayoutHostMappingInfoVALVE(
    VkDevice device,
    const VkDescriptorSetBindingReferenceVALVE* pBindingReference,
    VkDescriptorSetLayoutHostMappingInfoVALVE* pHostMapping) {
    log_driver_message("Generated Driver vkGetDescriptorSetLayoutHostMappingInfoVALVE");
}

VKAPI_ATTR void VKAPI_CALL driver_GetDescriptorSetHostMappingVALVE(
    VkDevice device,
    VkDescriptorSet descriptorSet,
    void** ppData) {
    log_driver_message("Generated Driver vkGetDescriptorSetHostMappingVALVE");
}


// ----- VK_KHR_acceleration_structure
VKAPI_ATTR VkResult VKAPI_CALL driver_CreateAccelerationStructureKHR(
    VkDevice device,
    const VkAccelerationStructureCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkAccelerationStructureKHR* pAccelerationStructure) {
    log_driver_message("Generated Driver vkCreateAccelerationStructureKHR");
    *pAccelerationStructure = reinterpret_cast<VkAccelerationStructureKHR>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_DestroyAccelerationStructureKHR(
    VkDevice device,
    VkAccelerationStructureKHR accelerationStructure,
    const VkAllocationCallbacks* pAllocator) {
    log_driver_message("Generated Driver vkDestroyAccelerationStructureKHR");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBuildAccelerationStructuresKHR(
    VkCommandBuffer commandBuffer,
    uint32_t infoCount,
    const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
    const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos) {
    log_driver_message("Generated Driver vkCmdBuildAccelerationStructuresKHR");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdBuildAccelerationStructuresIndirectKHR(
    VkCommandBuffer commandBuffer,
    uint32_t infoCount,
    const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
    const VkDeviceAddress* pIndirectDeviceAddresses,
    const uint32_t* pIndirectStrides,
    const uint32_t* const* ppMaxPrimitiveCounts) {
    log_driver_message("Generated Driver vkCmdBuildAccelerationStructuresIndirectKHR");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_BuildAccelerationStructuresKHR(
    VkDevice device,
    VkDeferredOperationKHR deferredOperation,
    uint32_t infoCount,
    const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
    const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos) {
    log_driver_message("Generated Driver vkBuildAccelerationStructuresKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CopyAccelerationStructureKHR(
    VkDevice device,
    VkDeferredOperationKHR deferredOperation,
    const VkCopyAccelerationStructureInfoKHR* pInfo) {
    log_driver_message("Generated Driver vkCopyAccelerationStructureKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CopyAccelerationStructureToMemoryKHR(
    VkDevice device,
    VkDeferredOperationKHR deferredOperation,
    const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo) {
    log_driver_message("Generated Driver vkCopyAccelerationStructureToMemoryKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CopyMemoryToAccelerationStructureKHR(
    VkDevice device,
    VkDeferredOperationKHR deferredOperation,
    const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo) {
    log_driver_message("Generated Driver vkCopyMemoryToAccelerationStructureKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_WriteAccelerationStructuresPropertiesKHR(
    VkDevice device,
    uint32_t accelerationStructureCount,
    const VkAccelerationStructureKHR* pAccelerationStructures,
    VkQueryType queryType,
    size_t dataSize,
    void* pData,
    size_t stride) {
    log_driver_message("Generated Driver vkWriteAccelerationStructuresPropertiesKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_CmdCopyAccelerationStructureKHR(
    VkCommandBuffer commandBuffer,
    const VkCopyAccelerationStructureInfoKHR* pInfo) {
    log_driver_message("Generated Driver vkCmdCopyAccelerationStructureKHR");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdCopyAccelerationStructureToMemoryKHR(
    VkCommandBuffer commandBuffer,
    const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo) {
    log_driver_message("Generated Driver vkCmdCopyAccelerationStructureToMemoryKHR");
}

VKAPI_ATTR void VKAPI_CALL driver_CmdCopyMemoryToAccelerationStructureKHR(
    VkCommandBuffer commandBuffer,
    const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo) {
    log_driver_message("Generated Driver vkCmdCopyMemoryToAccelerationStructureKHR");
}

VKAPI_ATTR VkDeviceAddress VKAPI_CALL driver_GetAccelerationStructureDeviceAddressKHR(
    VkDevice device,
    const VkAccelerationStructureDeviceAddressInfoKHR* pInfo) {
    log_driver_message("Generated Driver vkGetAccelerationStructureDeviceAddressKHR");
    return static_cast<VkDeviceAddress>(0);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdWriteAccelerationStructuresPropertiesKHR(
    VkCommandBuffer commandBuffer,
    uint32_t accelerationStructureCount,
    const VkAccelerationStructureKHR* pAccelerationStructures,
    VkQueryType queryType,
    VkQueryPool queryPool,
    uint32_t firstQuery) {
    log_driver_message("Generated Driver vkCmdWriteAccelerationStructuresPropertiesKHR");
}

VKAPI_ATTR void VKAPI_CALL driver_GetDeviceAccelerationStructureCompatibilityKHR(
    VkDevice device,
    const VkAccelerationStructureVersionInfoKHR* pVersionInfo,
    VkAccelerationStructureCompatibilityKHR* pCompatibility) {
    log_driver_message("Generated Driver vkGetDeviceAccelerationStructureCompatibilityKHR");
}

VKAPI_ATTR void VKAPI_CALL driver_GetAccelerationStructureBuildSizesKHR(
    VkDevice device,
    VkAccelerationStructureBuildTypeKHR buildType,
    const VkAccelerationStructureBuildGeometryInfoKHR* pBuildInfo,
    const uint32_t* pMaxPrimitiveCounts,
    VkAccelerationStructureBuildSizesInfoKHR* pSizeInfo) {
    log_driver_message("Generated Driver vkGetAccelerationStructureBuildSizesKHR");
}


// ----- VK_KHR_ray_tracing_pipeline
VKAPI_ATTR void VKAPI_CALL driver_CmdTraceRaysKHR(
    VkCommandBuffer commandBuffer,
    const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable,
    uint32_t width,
    uint32_t height,
    uint32_t depth) {
    log_driver_message("Generated Driver vkCmdTraceRaysKHR");
}

VKAPI_ATTR VkResult VKAPI_CALL driver_CreateRayTracingPipelinesKHR(
    VkDevice device,
    VkDeferredOperationKHR deferredOperation,
    VkPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const VkRayTracingPipelineCreateInfoKHR* pCreateInfos,
    const VkAllocationCallbacks* pAllocator,
    VkPipeline* pPipelines) {
    log_driver_message("Generated Driver vkCreateRayTracingPipelinesKHR");
    *pPipelines = reinterpret_cast<VkPipeline>(0xdeadbeefdeadbeef);
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetRayTracingShaderGroupHandlesKHR(
    VkDevice device,
    VkPipeline pipeline,
    uint32_t firstGroup,
    uint32_t groupCount,
    size_t dataSize,
    void* pData) {
    log_driver_message("Generated Driver vkGetRayTracingShaderGroupHandlesKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL driver_GetRayTracingCaptureReplayShaderGroupHandlesKHR(
    VkDevice device,
    VkPipeline pipeline,
    uint32_t firstGroup,
    uint32_t groupCount,
    size_t dataSize,
    void* pData) {
    log_driver_message("Generated Driver vkGetRayTracingCaptureReplayShaderGroupHandlesKHR");
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL driver_CmdTraceRaysIndirectKHR(
    VkCommandBuffer commandBuffer,
    const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable,
    const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable,
    VkDeviceAddress indirectDeviceAddress) {
    log_driver_message("Generated Driver vkCmdTraceRaysIndirectKHR");
}

VKAPI_ATTR VkDeviceSize VKAPI_CALL driver_GetRayTracingShaderGroupStackSizeKHR(
    VkDevice device,
    VkPipeline pipeline,
    uint32_t group,
    VkShaderGroupShaderKHR groupShader) {
    log_driver_message("Generated Driver vkGetRayTracingShaderGroupStackSizeKHR");
    return static_cast<VkDeviceSize>(0);
}

VKAPI_ATTR void VKAPI_CALL driver_CmdSetRayTracingPipelineStackSizeKHR(
    VkCommandBuffer commandBuffer,
    uint32_t pipelineStackSize) {
    log_driver_message("Generated Driver vkCmdSetRayTracingPipelineStackSizeKHR");
}


// Exported functions
extern "C" {


bool IsInstanceExtensionEnabled(const char* extension_name) {
    return driver.enabled_instance_extensions.end() !=
           std::find_if(driver.enabled_instance_extensions.begin(), driver.enabled_instance_extensions.end(),
                        [extension_name](Extension const& ext) { return ext.extensionName == extension_name; });
}

// Prototypes:
FRAMEWORK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL driver_GetDeviceProcAddr(VkDevice device, const char* pName);

FRAMEWORK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL driver_GetInstanceProcAddr(VkInstance instance, const char* pName) {

    // ----- VK_VERSION_1_0
    {
        if (string_eq(pName, "vkCreateInstance")) return to_vkVoidFunction(driver_CreateInstance);
        if (string_eq(pName, "vkDestroyInstance")) return to_vkVoidFunction(driver_DestroyInstance);
        if (string_eq(pName, "vkEnumeratePhysicalDevices")) return to_vkVoidFunction(driver_EnumeratePhysicalDevices);
        if (string_eq(pName, "vkGetPhysicalDeviceFeatures")) return to_vkVoidFunction(driver_GetPhysicalDeviceFeatures);
        if (string_eq(pName, "vkGetPhysicalDeviceFormatProperties")) return to_vkVoidFunction(driver_GetPhysicalDeviceFormatProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceImageFormatProperties")) return to_vkVoidFunction(driver_GetPhysicalDeviceImageFormatProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceProperties")) return to_vkVoidFunction(driver_GetPhysicalDeviceProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyProperties")) return to_vkVoidFunction(driver_GetPhysicalDeviceQueueFamilyProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceMemoryProperties")) return to_vkVoidFunction(driver_GetPhysicalDeviceMemoryProperties);
        if (string_eq(pName, "vkGetInstanceProcAddr")) return to_vkVoidFunction(driver_GetInstanceProcAddr);
        if (string_eq(pName, "vkGetDeviceProcAddr")) return to_vkVoidFunction(driver_GetDeviceProcAddr);
        if (string_eq(pName, "vkCreateDevice")) return to_vkVoidFunction(driver_CreateDevice);
        if (string_eq(pName, "vkDestroyDevice")) return to_vkVoidFunction(driver_DestroyDevice);
        if (string_eq(pName, "vkEnumerateInstanceExtensionProperties")) return to_vkVoidFunction(driver_EnumerateInstanceExtensionProperties);
        if (string_eq(pName, "vkEnumerateDeviceExtensionProperties")) return to_vkVoidFunction(driver_EnumerateDeviceExtensionProperties);
        if (string_eq(pName, "vkEnumerateInstanceLayerProperties")) return to_vkVoidFunction(driver_EnumerateInstanceLayerProperties);
        if (string_eq(pName, "vkEnumerateDeviceLayerProperties")) return to_vkVoidFunction(driver_EnumerateDeviceLayerProperties);
        if (string_eq(pName, "vkGetDeviceQueue")) return to_vkVoidFunction(driver_GetDeviceQueue);
        if (string_eq(pName, "vkQueueSubmit")) return to_vkVoidFunction(driver_QueueSubmit);
        if (string_eq(pName, "vkQueueWaitIdle")) return to_vkVoidFunction(driver_QueueWaitIdle);
        if (string_eq(pName, "vkDeviceWaitIdle")) return to_vkVoidFunction(driver_DeviceWaitIdle);
        if (string_eq(pName, "vkAllocateMemory")) return to_vkVoidFunction(driver_AllocateMemory);
        if (string_eq(pName, "vkFreeMemory")) return to_vkVoidFunction(driver_FreeMemory);
        if (string_eq(pName, "vkMapMemory")) return to_vkVoidFunction(driver_MapMemory);
        if (string_eq(pName, "vkUnmapMemory")) return to_vkVoidFunction(driver_UnmapMemory);
        if (string_eq(pName, "vkFlushMappedMemoryRanges")) return to_vkVoidFunction(driver_FlushMappedMemoryRanges);
        if (string_eq(pName, "vkInvalidateMappedMemoryRanges")) return to_vkVoidFunction(driver_InvalidateMappedMemoryRanges);
        if (string_eq(pName, "vkGetDeviceMemoryCommitment")) return to_vkVoidFunction(driver_GetDeviceMemoryCommitment);
        if (string_eq(pName, "vkBindBufferMemory")) return to_vkVoidFunction(driver_BindBufferMemory);
        if (string_eq(pName, "vkBindImageMemory")) return to_vkVoidFunction(driver_BindImageMemory);
        if (string_eq(pName, "vkGetBufferMemoryRequirements")) return to_vkVoidFunction(driver_GetBufferMemoryRequirements);
        if (string_eq(pName, "vkGetImageMemoryRequirements")) return to_vkVoidFunction(driver_GetImageMemoryRequirements);
        if (string_eq(pName, "vkGetImageSparseMemoryRequirements")) return to_vkVoidFunction(driver_GetImageSparseMemoryRequirements);
        if (string_eq(pName, "vkGetPhysicalDeviceSparseImageFormatProperties")) return to_vkVoidFunction(driver_GetPhysicalDeviceSparseImageFormatProperties);
        if (string_eq(pName, "vkQueueBindSparse")) return to_vkVoidFunction(driver_QueueBindSparse);
        if (string_eq(pName, "vkCreateFence")) return to_vkVoidFunction(driver_CreateFence);
        if (string_eq(pName, "vkDestroyFence")) return to_vkVoidFunction(driver_DestroyFence);
        if (string_eq(pName, "vkResetFences")) return to_vkVoidFunction(driver_ResetFences);
        if (string_eq(pName, "vkGetFenceStatus")) return to_vkVoidFunction(driver_GetFenceStatus);
        if (string_eq(pName, "vkWaitForFences")) return to_vkVoidFunction(driver_WaitForFences);
        if (string_eq(pName, "vkCreateSemaphore")) return to_vkVoidFunction(driver_CreateSemaphore);
        if (string_eq(pName, "vkDestroySemaphore")) return to_vkVoidFunction(driver_DestroySemaphore);
        if (string_eq(pName, "vkCreateEvent")) return to_vkVoidFunction(driver_CreateEvent);
        if (string_eq(pName, "vkDestroyEvent")) return to_vkVoidFunction(driver_DestroyEvent);
        if (string_eq(pName, "vkGetEventStatus")) return to_vkVoidFunction(driver_GetEventStatus);
        if (string_eq(pName, "vkSetEvent")) return to_vkVoidFunction(driver_SetEvent);
        if (string_eq(pName, "vkResetEvent")) return to_vkVoidFunction(driver_ResetEvent);
        if (string_eq(pName, "vkCreateQueryPool")) return to_vkVoidFunction(driver_CreateQueryPool);
        if (string_eq(pName, "vkDestroyQueryPool")) return to_vkVoidFunction(driver_DestroyQueryPool);
        if (string_eq(pName, "vkGetQueryPoolResults")) return to_vkVoidFunction(driver_GetQueryPoolResults);
        if (string_eq(pName, "vkCreateBuffer")) return to_vkVoidFunction(driver_CreateBuffer);
        if (string_eq(pName, "vkDestroyBuffer")) return to_vkVoidFunction(driver_DestroyBuffer);
        if (string_eq(pName, "vkCreateBufferView")) return to_vkVoidFunction(driver_CreateBufferView);
        if (string_eq(pName, "vkDestroyBufferView")) return to_vkVoidFunction(driver_DestroyBufferView);
        if (string_eq(pName, "vkCreateImage")) return to_vkVoidFunction(driver_CreateImage);
        if (string_eq(pName, "vkDestroyImage")) return to_vkVoidFunction(driver_DestroyImage);
        if (string_eq(pName, "vkGetImageSubresourceLayout")) return to_vkVoidFunction(driver_GetImageSubresourceLayout);
        if (string_eq(pName, "vkCreateImageView")) return to_vkVoidFunction(driver_CreateImageView);
        if (string_eq(pName, "vkDestroyImageView")) return to_vkVoidFunction(driver_DestroyImageView);
        if (string_eq(pName, "vkCreateShaderModule")) return to_vkVoidFunction(driver_CreateShaderModule);
        if (string_eq(pName, "vkDestroyShaderModule")) return to_vkVoidFunction(driver_DestroyShaderModule);
        if (string_eq(pName, "vkCreatePipelineCache")) return to_vkVoidFunction(driver_CreatePipelineCache);
        if (string_eq(pName, "vkDestroyPipelineCache")) return to_vkVoidFunction(driver_DestroyPipelineCache);
        if (string_eq(pName, "vkGetPipelineCacheData")) return to_vkVoidFunction(driver_GetPipelineCacheData);
        if (string_eq(pName, "vkMergePipelineCaches")) return to_vkVoidFunction(driver_MergePipelineCaches);
        if (string_eq(pName, "vkCreateGraphicsPipelines")) return to_vkVoidFunction(driver_CreateGraphicsPipelines);
        if (string_eq(pName, "vkCreateComputePipelines")) return to_vkVoidFunction(driver_CreateComputePipelines);
        if (string_eq(pName, "vkDestroyPipeline")) return to_vkVoidFunction(driver_DestroyPipeline);
        if (string_eq(pName, "vkCreatePipelineLayout")) return to_vkVoidFunction(driver_CreatePipelineLayout);
        if (string_eq(pName, "vkDestroyPipelineLayout")) return to_vkVoidFunction(driver_DestroyPipelineLayout);
        if (string_eq(pName, "vkCreateSampler")) return to_vkVoidFunction(driver_CreateSampler);
        if (string_eq(pName, "vkDestroySampler")) return to_vkVoidFunction(driver_DestroySampler);
        if (string_eq(pName, "vkCreateDescriptorSetLayout")) return to_vkVoidFunction(driver_CreateDescriptorSetLayout);
        if (string_eq(pName, "vkDestroyDescriptorSetLayout")) return to_vkVoidFunction(driver_DestroyDescriptorSetLayout);
        if (string_eq(pName, "vkCreateDescriptorPool")) return to_vkVoidFunction(driver_CreateDescriptorPool);
        if (string_eq(pName, "vkDestroyDescriptorPool")) return to_vkVoidFunction(driver_DestroyDescriptorPool);
        if (string_eq(pName, "vkResetDescriptorPool")) return to_vkVoidFunction(driver_ResetDescriptorPool);
        if (string_eq(pName, "vkAllocateDescriptorSets")) return to_vkVoidFunction(driver_AllocateDescriptorSets);
        if (string_eq(pName, "vkFreeDescriptorSets")) return to_vkVoidFunction(driver_FreeDescriptorSets);
        if (string_eq(pName, "vkUpdateDescriptorSets")) return to_vkVoidFunction(driver_UpdateDescriptorSets);
        if (string_eq(pName, "vkCreateFramebuffer")) return to_vkVoidFunction(driver_CreateFramebuffer);
        if (string_eq(pName, "vkDestroyFramebuffer")) return to_vkVoidFunction(driver_DestroyFramebuffer);
        if (string_eq(pName, "vkCreateRenderPass")) return to_vkVoidFunction(driver_CreateRenderPass);
        if (string_eq(pName, "vkDestroyRenderPass")) return to_vkVoidFunction(driver_DestroyRenderPass);
        if (string_eq(pName, "vkGetRenderAreaGranularity")) return to_vkVoidFunction(driver_GetRenderAreaGranularity);
        if (string_eq(pName, "vkCreateCommandPool")) return to_vkVoidFunction(driver_CreateCommandPool);
        if (string_eq(pName, "vkDestroyCommandPool")) return to_vkVoidFunction(driver_DestroyCommandPool);
        if (string_eq(pName, "vkResetCommandPool")) return to_vkVoidFunction(driver_ResetCommandPool);
        if (string_eq(pName, "vkAllocateCommandBuffers")) return to_vkVoidFunction(driver_AllocateCommandBuffers);
        if (string_eq(pName, "vkFreeCommandBuffers")) return to_vkVoidFunction(driver_FreeCommandBuffers);
        if (string_eq(pName, "vkBeginCommandBuffer")) return to_vkVoidFunction(driver_BeginCommandBuffer);
        if (string_eq(pName, "vkEndCommandBuffer")) return to_vkVoidFunction(driver_EndCommandBuffer);
        if (string_eq(pName, "vkResetCommandBuffer")) return to_vkVoidFunction(driver_ResetCommandBuffer);
        if (string_eq(pName, "vkCmdBindPipeline")) return to_vkVoidFunction(driver_CmdBindPipeline);
        if (string_eq(pName, "vkCmdSetViewport")) return to_vkVoidFunction(driver_CmdSetViewport);
        if (string_eq(pName, "vkCmdSetScissor")) return to_vkVoidFunction(driver_CmdSetScissor);
        if (string_eq(pName, "vkCmdSetLineWidth")) return to_vkVoidFunction(driver_CmdSetLineWidth);
        if (string_eq(pName, "vkCmdSetDepthBias")) return to_vkVoidFunction(driver_CmdSetDepthBias);
        if (string_eq(pName, "vkCmdSetBlendConstants")) return to_vkVoidFunction(driver_CmdSetBlendConstants);
        if (string_eq(pName, "vkCmdSetDepthBounds")) return to_vkVoidFunction(driver_CmdSetDepthBounds);
        if (string_eq(pName, "vkCmdSetStencilCompareMask")) return to_vkVoidFunction(driver_CmdSetStencilCompareMask);
        if (string_eq(pName, "vkCmdSetStencilWriteMask")) return to_vkVoidFunction(driver_CmdSetStencilWriteMask);
        if (string_eq(pName, "vkCmdSetStencilReference")) return to_vkVoidFunction(driver_CmdSetStencilReference);
        if (string_eq(pName, "vkCmdBindDescriptorSets")) return to_vkVoidFunction(driver_CmdBindDescriptorSets);
        if (string_eq(pName, "vkCmdBindIndexBuffer")) return to_vkVoidFunction(driver_CmdBindIndexBuffer);
        if (string_eq(pName, "vkCmdBindVertexBuffers")) return to_vkVoidFunction(driver_CmdBindVertexBuffers);
        if (string_eq(pName, "vkCmdDraw")) return to_vkVoidFunction(driver_CmdDraw);
        if (string_eq(pName, "vkCmdDrawIndexed")) return to_vkVoidFunction(driver_CmdDrawIndexed);
        if (string_eq(pName, "vkCmdDrawIndirect")) return to_vkVoidFunction(driver_CmdDrawIndirect);
        if (string_eq(pName, "vkCmdDrawIndexedIndirect")) return to_vkVoidFunction(driver_CmdDrawIndexedIndirect);
        if (string_eq(pName, "vkCmdDispatch")) return to_vkVoidFunction(driver_CmdDispatch);
        if (string_eq(pName, "vkCmdDispatchIndirect")) return to_vkVoidFunction(driver_CmdDispatchIndirect);
        if (string_eq(pName, "vkCmdCopyBuffer")) return to_vkVoidFunction(driver_CmdCopyBuffer);
        if (string_eq(pName, "vkCmdCopyImage")) return to_vkVoidFunction(driver_CmdCopyImage);
        if (string_eq(pName, "vkCmdBlitImage")) return to_vkVoidFunction(driver_CmdBlitImage);
        if (string_eq(pName, "vkCmdCopyBufferToImage")) return to_vkVoidFunction(driver_CmdCopyBufferToImage);
        if (string_eq(pName, "vkCmdCopyImageToBuffer")) return to_vkVoidFunction(driver_CmdCopyImageToBuffer);
        if (string_eq(pName, "vkCmdUpdateBuffer")) return to_vkVoidFunction(driver_CmdUpdateBuffer);
        if (string_eq(pName, "vkCmdFillBuffer")) return to_vkVoidFunction(driver_CmdFillBuffer);
        if (string_eq(pName, "vkCmdClearColorImage")) return to_vkVoidFunction(driver_CmdClearColorImage);
        if (string_eq(pName, "vkCmdClearDepthStencilImage")) return to_vkVoidFunction(driver_CmdClearDepthStencilImage);
        if (string_eq(pName, "vkCmdClearAttachments")) return to_vkVoidFunction(driver_CmdClearAttachments);
        if (string_eq(pName, "vkCmdResolveImage")) return to_vkVoidFunction(driver_CmdResolveImage);
        if (string_eq(pName, "vkCmdSetEvent")) return to_vkVoidFunction(driver_CmdSetEvent);
        if (string_eq(pName, "vkCmdResetEvent")) return to_vkVoidFunction(driver_CmdResetEvent);
        if (string_eq(pName, "vkCmdWaitEvents")) return to_vkVoidFunction(driver_CmdWaitEvents);
        if (string_eq(pName, "vkCmdPipelineBarrier")) return to_vkVoidFunction(driver_CmdPipelineBarrier);
        if (string_eq(pName, "vkCmdBeginQuery")) return to_vkVoidFunction(driver_CmdBeginQuery);
        if (string_eq(pName, "vkCmdEndQuery")) return to_vkVoidFunction(driver_CmdEndQuery);
        if (string_eq(pName, "vkCmdResetQueryPool")) return to_vkVoidFunction(driver_CmdResetQueryPool);
        if (string_eq(pName, "vkCmdWriteTimestamp")) return to_vkVoidFunction(driver_CmdWriteTimestamp);
        if (string_eq(pName, "vkCmdCopyQueryPoolResults")) return to_vkVoidFunction(driver_CmdCopyQueryPoolResults);
        if (string_eq(pName, "vkCmdPushConstants")) return to_vkVoidFunction(driver_CmdPushConstants);
        if (string_eq(pName, "vkCmdBeginRenderPass")) return to_vkVoidFunction(driver_CmdBeginRenderPass);
        if (string_eq(pName, "vkCmdNextSubpass")) return to_vkVoidFunction(driver_CmdNextSubpass);
        if (string_eq(pName, "vkCmdEndRenderPass")) return to_vkVoidFunction(driver_CmdEndRenderPass);
        if (string_eq(pName, "vkCmdExecuteCommands")) return to_vkVoidFunction(driver_CmdExecuteCommands);
    } // End Core 1.0

    // ----- VK_VERSION_1_1
    {
        if (string_eq(pName, "vkEnumerateInstanceVersion")) return to_vkVoidFunction(driver_EnumerateInstanceVersion);
        if (string_eq(pName, "vkBindBufferMemory2")) return to_vkVoidFunction(driver_BindBufferMemory2);
        if (string_eq(pName, "vkBindImageMemory2")) return to_vkVoidFunction(driver_BindImageMemory2);
        if (string_eq(pName, "vkGetDeviceGroupPeerMemoryFeatures")) return to_vkVoidFunction(driver_GetDeviceGroupPeerMemoryFeatures);
        if (string_eq(pName, "vkCmdSetDeviceMask")) return to_vkVoidFunction(driver_CmdSetDeviceMask);
        if (string_eq(pName, "vkCmdDispatchBase")) return to_vkVoidFunction(driver_CmdDispatchBase);
        if (string_eq(pName, "vkEnumeratePhysicalDeviceGroups")) return to_vkVoidFunction(driver_EnumeratePhysicalDeviceGroups);
        if (string_eq(pName, "vkGetImageMemoryRequirements2")) return to_vkVoidFunction(driver_GetImageMemoryRequirements2);
        if (string_eq(pName, "vkGetBufferMemoryRequirements2")) return to_vkVoidFunction(driver_GetBufferMemoryRequirements2);
        if (string_eq(pName, "vkGetImageSparseMemoryRequirements2")) return to_vkVoidFunction(driver_GetImageSparseMemoryRequirements2);
        if (string_eq(pName, "vkGetPhysicalDeviceFeatures2")) return to_vkVoidFunction(driver_GetPhysicalDeviceFeatures2);
        if (string_eq(pName, "vkGetPhysicalDeviceProperties2")) return to_vkVoidFunction(driver_GetPhysicalDeviceProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceFormatProperties2")) return to_vkVoidFunction(driver_GetPhysicalDeviceFormatProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceImageFormatProperties2")) return to_vkVoidFunction(driver_GetPhysicalDeviceImageFormatProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyProperties2")) return to_vkVoidFunction(driver_GetPhysicalDeviceQueueFamilyProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceMemoryProperties2")) return to_vkVoidFunction(driver_GetPhysicalDeviceMemoryProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceSparseImageFormatProperties2")) return to_vkVoidFunction(driver_GetPhysicalDeviceSparseImageFormatProperties2);
        if (string_eq(pName, "vkTrimCommandPool")) return to_vkVoidFunction(driver_TrimCommandPool);
        if (string_eq(pName, "vkGetDeviceQueue2")) return to_vkVoidFunction(driver_GetDeviceQueue2);
        if (string_eq(pName, "vkCreateSamplerYcbcrConversion")) return to_vkVoidFunction(driver_CreateSamplerYcbcrConversion);
        if (string_eq(pName, "vkDestroySamplerYcbcrConversion")) return to_vkVoidFunction(driver_DestroySamplerYcbcrConversion);
        if (string_eq(pName, "vkCreateDescriptorUpdateTemplate")) return to_vkVoidFunction(driver_CreateDescriptorUpdateTemplate);
        if (string_eq(pName, "vkDestroyDescriptorUpdateTemplate")) return to_vkVoidFunction(driver_DestroyDescriptorUpdateTemplate);
        if (string_eq(pName, "vkUpdateDescriptorSetWithTemplate")) return to_vkVoidFunction(driver_UpdateDescriptorSetWithTemplate);
        if (string_eq(pName, "vkGetPhysicalDeviceExternalBufferProperties")) return to_vkVoidFunction(driver_GetPhysicalDeviceExternalBufferProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceExternalFenceProperties")) return to_vkVoidFunction(driver_GetPhysicalDeviceExternalFenceProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceExternalSemaphoreProperties")) return to_vkVoidFunction(driver_GetPhysicalDeviceExternalSemaphoreProperties);
        if (string_eq(pName, "vkGetDescriptorSetLayoutSupport")) return to_vkVoidFunction(driver_GetDescriptorSetLayoutSupport);
    } // End Core 1.1

    // ----- VK_VERSION_1_2
    {
        if (string_eq(pName, "vkCmdDrawIndirectCount")) return to_vkVoidFunction(driver_CmdDrawIndirectCount);
        if (string_eq(pName, "vkCmdDrawIndexedIndirectCount")) return to_vkVoidFunction(driver_CmdDrawIndexedIndirectCount);
        if (string_eq(pName, "vkCreateRenderPass2")) return to_vkVoidFunction(driver_CreateRenderPass2);
        if (string_eq(pName, "vkCmdBeginRenderPass2")) return to_vkVoidFunction(driver_CmdBeginRenderPass2);
        if (string_eq(pName, "vkCmdNextSubpass2")) return to_vkVoidFunction(driver_CmdNextSubpass2);
        if (string_eq(pName, "vkCmdEndRenderPass2")) return to_vkVoidFunction(driver_CmdEndRenderPass2);
        if (string_eq(pName, "vkResetQueryPool")) return to_vkVoidFunction(driver_ResetQueryPool);
        if (string_eq(pName, "vkGetSemaphoreCounterValue")) return to_vkVoidFunction(driver_GetSemaphoreCounterValue);
        if (string_eq(pName, "vkWaitSemaphores")) return to_vkVoidFunction(driver_WaitSemaphores);
        if (string_eq(pName, "vkSignalSemaphore")) return to_vkVoidFunction(driver_SignalSemaphore);
        if (string_eq(pName, "vkGetBufferDeviceAddress")) return to_vkVoidFunction(driver_GetBufferDeviceAddress);
        if (string_eq(pName, "vkGetBufferOpaqueCaptureAddress")) return to_vkVoidFunction(driver_GetBufferOpaqueCaptureAddress);
        if (string_eq(pName, "vkGetDeviceMemoryOpaqueCaptureAddress")) return to_vkVoidFunction(driver_GetDeviceMemoryOpaqueCaptureAddress);
    } // End Core 1.2

    // ----- VK_VERSION_1_3
    {
        if (string_eq(pName, "vkGetPhysicalDeviceToolProperties")) return to_vkVoidFunction(driver_GetPhysicalDeviceToolProperties);
        if (string_eq(pName, "vkCreatePrivateDataSlot")) return to_vkVoidFunction(driver_CreatePrivateDataSlot);
        if (string_eq(pName, "vkDestroyPrivateDataSlot")) return to_vkVoidFunction(driver_DestroyPrivateDataSlot);
        if (string_eq(pName, "vkSetPrivateData")) return to_vkVoidFunction(driver_SetPrivateData);
        if (string_eq(pName, "vkGetPrivateData")) return to_vkVoidFunction(driver_GetPrivateData);
        if (string_eq(pName, "vkCmdSetEvent2")) return to_vkVoidFunction(driver_CmdSetEvent2);
        if (string_eq(pName, "vkCmdResetEvent2")) return to_vkVoidFunction(driver_CmdResetEvent2);
        if (string_eq(pName, "vkCmdWaitEvents2")) return to_vkVoidFunction(driver_CmdWaitEvents2);
        if (string_eq(pName, "vkCmdPipelineBarrier2")) return to_vkVoidFunction(driver_CmdPipelineBarrier2);
        if (string_eq(pName, "vkCmdWriteTimestamp2")) return to_vkVoidFunction(driver_CmdWriteTimestamp2);
        if (string_eq(pName, "vkQueueSubmit2")) return to_vkVoidFunction(driver_QueueSubmit2);
        if (string_eq(pName, "vkCmdCopyBuffer2")) return to_vkVoidFunction(driver_CmdCopyBuffer2);
        if (string_eq(pName, "vkCmdCopyImage2")) return to_vkVoidFunction(driver_CmdCopyImage2);
        if (string_eq(pName, "vkCmdCopyBufferToImage2")) return to_vkVoidFunction(driver_CmdCopyBufferToImage2);
        if (string_eq(pName, "vkCmdCopyImageToBuffer2")) return to_vkVoidFunction(driver_CmdCopyImageToBuffer2);
        if (string_eq(pName, "vkCmdBlitImage2")) return to_vkVoidFunction(driver_CmdBlitImage2);
        if (string_eq(pName, "vkCmdResolveImage2")) return to_vkVoidFunction(driver_CmdResolveImage2);
        if (string_eq(pName, "vkCmdBeginRendering")) return to_vkVoidFunction(driver_CmdBeginRendering);
        if (string_eq(pName, "vkCmdEndRendering")) return to_vkVoidFunction(driver_CmdEndRendering);
        if (string_eq(pName, "vkCmdSetCullMode")) return to_vkVoidFunction(driver_CmdSetCullMode);
        if (string_eq(pName, "vkCmdSetFrontFace")) return to_vkVoidFunction(driver_CmdSetFrontFace);
        if (string_eq(pName, "vkCmdSetPrimitiveTopology")) return to_vkVoidFunction(driver_CmdSetPrimitiveTopology);
        if (string_eq(pName, "vkCmdSetViewportWithCount")) return to_vkVoidFunction(driver_CmdSetViewportWithCount);
        if (string_eq(pName, "vkCmdSetScissorWithCount")) return to_vkVoidFunction(driver_CmdSetScissorWithCount);
        if (string_eq(pName, "vkCmdBindVertexBuffers2")) return to_vkVoidFunction(driver_CmdBindVertexBuffers2);
        if (string_eq(pName, "vkCmdSetDepthTestEnable")) return to_vkVoidFunction(driver_CmdSetDepthTestEnable);
        if (string_eq(pName, "vkCmdSetDepthWriteEnable")) return to_vkVoidFunction(driver_CmdSetDepthWriteEnable);
        if (string_eq(pName, "vkCmdSetDepthCompareOp")) return to_vkVoidFunction(driver_CmdSetDepthCompareOp);
        if (string_eq(pName, "vkCmdSetDepthBoundsTestEnable")) return to_vkVoidFunction(driver_CmdSetDepthBoundsTestEnable);
        if (string_eq(pName, "vkCmdSetStencilTestEnable")) return to_vkVoidFunction(driver_CmdSetStencilTestEnable);
        if (string_eq(pName, "vkCmdSetStencilOp")) return to_vkVoidFunction(driver_CmdSetStencilOp);
        if (string_eq(pName, "vkCmdSetRasterizerDiscardEnable")) return to_vkVoidFunction(driver_CmdSetRasterizerDiscardEnable);
        if (string_eq(pName, "vkCmdSetDepthBiasEnable")) return to_vkVoidFunction(driver_CmdSetDepthBiasEnable);
        if (string_eq(pName, "vkCmdSetPrimitiveRestartEnable")) return to_vkVoidFunction(driver_CmdSetPrimitiveRestartEnable);
        if (string_eq(pName, "vkGetDeviceBufferMemoryRequirements")) return to_vkVoidFunction(driver_GetDeviceBufferMemoryRequirements);
        if (string_eq(pName, "vkGetDeviceImageMemoryRequirements")) return to_vkVoidFunction(driver_GetDeviceImageMemoryRequirements);
        if (string_eq(pName, "vkGetDeviceImageSparseMemoryRequirements")) return to_vkVoidFunction(driver_GetDeviceImageSparseMemoryRequirements);
    } // End Core 1.3

    // ----- VK_KHR_surface
    if (IsInstanceExtensionEnabled("VK_KHR_surface")) {
        if (string_eq(pName, "vkDestroySurfaceKHR")) return to_vkVoidFunction(driver_DestroySurfaceKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceSurfaceSupportKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceSurfaceSupportKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceSurfaceCapabilitiesKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceSurfaceFormatsKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceSurfaceFormatsKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceSurfacePresentModesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceSurfacePresentModesKHR);
    } // VK_KHR_surface

    // ----- VK_KHR_swapchain
    if (string_eq(pName, "vkCreateSwapchainKHR")) return to_vkVoidFunction(driver_CreateSwapchainKHR);
    if (string_eq(pName, "vkDestroySwapchainKHR")) return to_vkVoidFunction(driver_DestroySwapchainKHR);
    if (string_eq(pName, "vkGetSwapchainImagesKHR")) return to_vkVoidFunction(driver_GetSwapchainImagesKHR);
    if (string_eq(pName, "vkAcquireNextImageKHR")) return to_vkVoidFunction(driver_AcquireNextImageKHR);
    if (string_eq(pName, "vkQueuePresentKHR")) return to_vkVoidFunction(driver_QueuePresentKHR);
    if (string_eq(pName, "vkGetDeviceGroupPresentCapabilitiesKHR")) return to_vkVoidFunction(driver_GetDeviceGroupPresentCapabilitiesKHR);
    if (string_eq(pName, "vkGetDeviceGroupSurfacePresentModesKHR")) return to_vkVoidFunction(driver_GetDeviceGroupSurfacePresentModesKHR);
    if (string_eq(pName, "vkGetPhysicalDevicePresentRectanglesKHR")) return to_vkVoidFunction(driver_GetPhysicalDevicePresentRectanglesKHR);
    if (string_eq(pName, "vkAcquireNextImage2KHR")) return to_vkVoidFunction(driver_AcquireNextImage2KHR);

    // ----- VK_KHR_display
    if (IsInstanceExtensionEnabled("VK_KHR_display")) {
        if (string_eq(pName, "vkGetPhysicalDeviceDisplayPropertiesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceDisplayPropertiesKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceDisplayPlanePropertiesKHR);
        if (string_eq(pName, "vkGetDisplayPlaneSupportedDisplaysKHR")) return to_vkVoidFunction(driver_GetDisplayPlaneSupportedDisplaysKHR);
        if (string_eq(pName, "vkGetDisplayModePropertiesKHR")) return to_vkVoidFunction(driver_GetDisplayModePropertiesKHR);
        if (string_eq(pName, "vkCreateDisplayModeKHR")) return to_vkVoidFunction(driver_CreateDisplayModeKHR);
        if (string_eq(pName, "vkGetDisplayPlaneCapabilitiesKHR")) return to_vkVoidFunction(driver_GetDisplayPlaneCapabilitiesKHR);
        if (string_eq(pName, "vkCreateDisplayPlaneSurfaceKHR")) return to_vkVoidFunction(driver_CreateDisplayPlaneSurfaceKHR);
    } // VK_KHR_display

    // ----- VK_KHR_display_swapchain
    if (string_eq(pName, "vkCreateSharedSwapchainsKHR")) return to_vkVoidFunction(driver_CreateSharedSwapchainsKHR);

    // ----- VK_KHR_xlib_surface
#ifdef VK_USE_PLATFORM_XLIB_KHR
    if (IsInstanceExtensionEnabled("VK_KHR_xlib_surface")) {
        if (string_eq(pName, "vkCreateXlibSurfaceKHR")) return to_vkVoidFunction(driver_CreateXlibSurfaceKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceXlibPresentationSupportKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceXlibPresentationSupportKHR);
    } // VK_KHR_xlib_surface
#endif // VK_USE_PLATFORM_XLIB_KHR

    // ----- VK_KHR_xcb_surface
#ifdef VK_USE_PLATFORM_XCB_KHR
    if (IsInstanceExtensionEnabled("VK_KHR_xcb_surface")) {
        if (string_eq(pName, "vkCreateXcbSurfaceKHR")) return to_vkVoidFunction(driver_CreateXcbSurfaceKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceXcbPresentationSupportKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceXcbPresentationSupportKHR);
    } // VK_KHR_xcb_surface
#endif // VK_USE_PLATFORM_XCB_KHR

    // ----- VK_KHR_wayland_surface
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    if (IsInstanceExtensionEnabled("VK_KHR_wayland_surface")) {
        if (string_eq(pName, "vkCreateWaylandSurfaceKHR")) return to_vkVoidFunction(driver_CreateWaylandSurfaceKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceWaylandPresentationSupportKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceWaylandPresentationSupportKHR);
    } // VK_KHR_wayland_surface
#endif // VK_USE_PLATFORM_WAYLAND_KHR

    // ----- VK_KHR_win32_surface
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (IsInstanceExtensionEnabled("VK_KHR_win32_surface")) {
        if (string_eq(pName, "vkCreateWin32SurfaceKHR")) return to_vkVoidFunction(driver_CreateWin32SurfaceKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceWin32PresentationSupportKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceWin32PresentationSupportKHR);
    } // VK_KHR_win32_surface
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_KHR_video_queue
#ifdef VK_ENABLE_BETA_EXTENSIONS
    if (string_eq(pName, "vkGetPhysicalDeviceVideoCapabilitiesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceVideoCapabilitiesKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceVideoFormatPropertiesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceVideoFormatPropertiesKHR);
    if (string_eq(pName, "vkCreateVideoSessionKHR")) return to_vkVoidFunction(driver_CreateVideoSessionKHR);
    if (string_eq(pName, "vkDestroyVideoSessionKHR")) return to_vkVoidFunction(driver_DestroyVideoSessionKHR);
    if (string_eq(pName, "vkGetVideoSessionMemoryRequirementsKHR")) return to_vkVoidFunction(driver_GetVideoSessionMemoryRequirementsKHR);
    if (string_eq(pName, "vkBindVideoSessionMemoryKHR")) return to_vkVoidFunction(driver_BindVideoSessionMemoryKHR);
    if (string_eq(pName, "vkCreateVideoSessionParametersKHR")) return to_vkVoidFunction(driver_CreateVideoSessionParametersKHR);
    if (string_eq(pName, "vkUpdateVideoSessionParametersKHR")) return to_vkVoidFunction(driver_UpdateVideoSessionParametersKHR);
    if (string_eq(pName, "vkDestroyVideoSessionParametersKHR")) return to_vkVoidFunction(driver_DestroyVideoSessionParametersKHR);
    if (string_eq(pName, "vkCmdBeginVideoCodingKHR")) return to_vkVoidFunction(driver_CmdBeginVideoCodingKHR);
    if (string_eq(pName, "vkCmdEndVideoCodingKHR")) return to_vkVoidFunction(driver_CmdEndVideoCodingKHR);
    if (string_eq(pName, "vkCmdControlVideoCodingKHR")) return to_vkVoidFunction(driver_CmdControlVideoCodingKHR);
#endif // VK_ENABLE_BETA_EXTENSIONS

    // ----- VK_KHR_video_decode_queue
#ifdef VK_ENABLE_BETA_EXTENSIONS
    if (string_eq(pName, "vkCmdDecodeVideoKHR")) return to_vkVoidFunction(driver_CmdDecodeVideoKHR);
#endif // VK_ENABLE_BETA_EXTENSIONS

    // ----- VK_KHR_dynamic_rendering
    if (string_eq(pName, "vkCmdBeginRenderingKHR")) return to_vkVoidFunction(driver_CmdBeginRenderingKHR);
    if (string_eq(pName, "vkCmdEndRenderingKHR")) return to_vkVoidFunction(driver_CmdEndRenderingKHR);

    // ----- VK_KHR_get_physical_device_properties2
    if (IsInstanceExtensionEnabled("VK_KHR_get_physical_device_properties2")) {
        if (string_eq(pName, "vkGetPhysicalDeviceFeatures2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceFeatures2KHR);
        if (string_eq(pName, "vkGetPhysicalDeviceProperties2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceProperties2KHR);
        if (string_eq(pName, "vkGetPhysicalDeviceFormatProperties2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceFormatProperties2KHR);
        if (string_eq(pName, "vkGetPhysicalDeviceImageFormatProperties2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceImageFormatProperties2KHR);
        if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyProperties2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceQueueFamilyProperties2KHR);
        if (string_eq(pName, "vkGetPhysicalDeviceMemoryProperties2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceMemoryProperties2KHR);
        if (string_eq(pName, "vkGetPhysicalDeviceSparseImageFormatProperties2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceSparseImageFormatProperties2KHR);
    } // VK_KHR_get_physical_device_properties2

    // ----- VK_KHR_device_group
    if (string_eq(pName, "vkGetDeviceGroupPeerMemoryFeaturesKHR")) return to_vkVoidFunction(driver_GetDeviceGroupPeerMemoryFeaturesKHR);
    if (string_eq(pName, "vkCmdSetDeviceMaskKHR")) return to_vkVoidFunction(driver_CmdSetDeviceMaskKHR);
    if (string_eq(pName, "vkCmdDispatchBaseKHR")) return to_vkVoidFunction(driver_CmdDispatchBaseKHR);

    // ----- VK_KHR_maintenance1
    if (string_eq(pName, "vkTrimCommandPoolKHR")) return to_vkVoidFunction(driver_TrimCommandPoolKHR);

    // ----- VK_KHR_device_group_creation
    if (IsInstanceExtensionEnabled("VK_KHR_device_group_creation")) {
        if (string_eq(pName, "vkEnumeratePhysicalDeviceGroupsKHR")) return to_vkVoidFunction(driver_EnumeratePhysicalDeviceGroupsKHR);
    } // VK_KHR_device_group_creation

    // ----- VK_KHR_external_memory_capabilities
    if (IsInstanceExtensionEnabled("VK_KHR_external_memory_capabilities")) {
        if (string_eq(pName, "vkGetPhysicalDeviceExternalBufferPropertiesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceExternalBufferPropertiesKHR);
    } // VK_KHR_external_memory_capabilities

    // ----- VK_KHR_external_memory_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (string_eq(pName, "vkGetMemoryWin32HandleKHR")) return to_vkVoidFunction(driver_GetMemoryWin32HandleKHR);
    if (string_eq(pName, "vkGetMemoryWin32HandlePropertiesKHR")) return to_vkVoidFunction(driver_GetMemoryWin32HandlePropertiesKHR);
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_KHR_external_memory_fd
    if (string_eq(pName, "vkGetMemoryFdKHR")) return to_vkVoidFunction(driver_GetMemoryFdKHR);
    if (string_eq(pName, "vkGetMemoryFdPropertiesKHR")) return to_vkVoidFunction(driver_GetMemoryFdPropertiesKHR);

    // ----- VK_KHR_external_semaphore_capabilities
    if (IsInstanceExtensionEnabled("VK_KHR_external_semaphore_capabilities")) {
        if (string_eq(pName, "vkGetPhysicalDeviceExternalSemaphorePropertiesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceExternalSemaphorePropertiesKHR);
    } // VK_KHR_external_semaphore_capabilities

    // ----- VK_KHR_external_semaphore_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (string_eq(pName, "vkImportSemaphoreWin32HandleKHR")) return to_vkVoidFunction(driver_ImportSemaphoreWin32HandleKHR);
    if (string_eq(pName, "vkGetSemaphoreWin32HandleKHR")) return to_vkVoidFunction(driver_GetSemaphoreWin32HandleKHR);
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_KHR_external_semaphore_fd
    if (string_eq(pName, "vkImportSemaphoreFdKHR")) return to_vkVoidFunction(driver_ImportSemaphoreFdKHR);
    if (string_eq(pName, "vkGetSemaphoreFdKHR")) return to_vkVoidFunction(driver_GetSemaphoreFdKHR);

    // ----- VK_KHR_push_descriptor
    if (string_eq(pName, "vkCmdPushDescriptorSetKHR")) return to_vkVoidFunction(driver_CmdPushDescriptorSetKHR);
    if (string_eq(pName, "vkCmdPushDescriptorSetWithTemplateKHR")) return to_vkVoidFunction(driver_CmdPushDescriptorSetWithTemplateKHR);

    // ----- VK_KHR_descriptor_update_template
    if (string_eq(pName, "vkCreateDescriptorUpdateTemplateKHR")) return to_vkVoidFunction(driver_CreateDescriptorUpdateTemplateKHR);
    if (string_eq(pName, "vkDestroyDescriptorUpdateTemplateKHR")) return to_vkVoidFunction(driver_DestroyDescriptorUpdateTemplateKHR);
    if (string_eq(pName, "vkUpdateDescriptorSetWithTemplateKHR")) return to_vkVoidFunction(driver_UpdateDescriptorSetWithTemplateKHR);

    // ----- VK_KHR_create_renderpass2
    if (string_eq(pName, "vkCreateRenderPass2KHR")) return to_vkVoidFunction(driver_CreateRenderPass2KHR);
    if (string_eq(pName, "vkCmdBeginRenderPass2KHR")) return to_vkVoidFunction(driver_CmdBeginRenderPass2KHR);
    if (string_eq(pName, "vkCmdNextSubpass2KHR")) return to_vkVoidFunction(driver_CmdNextSubpass2KHR);
    if (string_eq(pName, "vkCmdEndRenderPass2KHR")) return to_vkVoidFunction(driver_CmdEndRenderPass2KHR);

    // ----- VK_KHR_shared_presentable_image
    if (string_eq(pName, "vkGetSwapchainStatusKHR")) return to_vkVoidFunction(driver_GetSwapchainStatusKHR);

    // ----- VK_KHR_external_fence_capabilities
    if (IsInstanceExtensionEnabled("VK_KHR_external_fence_capabilities")) {
        if (string_eq(pName, "vkGetPhysicalDeviceExternalFencePropertiesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceExternalFencePropertiesKHR);
    } // VK_KHR_external_fence_capabilities

    // ----- VK_KHR_external_fence_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (string_eq(pName, "vkImportFenceWin32HandleKHR")) return to_vkVoidFunction(driver_ImportFenceWin32HandleKHR);
    if (string_eq(pName, "vkGetFenceWin32HandleKHR")) return to_vkVoidFunction(driver_GetFenceWin32HandleKHR);
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_KHR_external_fence_fd
    if (string_eq(pName, "vkImportFenceFdKHR")) return to_vkVoidFunction(driver_ImportFenceFdKHR);
    if (string_eq(pName, "vkGetFenceFdKHR")) return to_vkVoidFunction(driver_GetFenceFdKHR);

    // ----- VK_KHR_performance_query
    if (string_eq(pName, "vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR")) return to_vkVoidFunction(driver_EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR);
    if (string_eq(pName, "vkAcquireProfilingLockKHR")) return to_vkVoidFunction(driver_AcquireProfilingLockKHR);
    if (string_eq(pName, "vkReleaseProfilingLockKHR")) return to_vkVoidFunction(driver_ReleaseProfilingLockKHR);

    // ----- VK_KHR_get_surface_capabilities2
    if (IsInstanceExtensionEnabled("VK_KHR_get_surface_capabilities2")) {
        if (string_eq(pName, "vkGetPhysicalDeviceSurfaceCapabilities2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceSurfaceCapabilities2KHR);
        if (string_eq(pName, "vkGetPhysicalDeviceSurfaceFormats2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceSurfaceFormats2KHR);
    } // VK_KHR_get_surface_capabilities2

    // ----- VK_KHR_get_display_properties2
    if (IsInstanceExtensionEnabled("VK_KHR_get_display_properties2")) {
        if (string_eq(pName, "vkGetPhysicalDeviceDisplayProperties2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceDisplayProperties2KHR);
        if (string_eq(pName, "vkGetPhysicalDeviceDisplayPlaneProperties2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceDisplayPlaneProperties2KHR);
        if (string_eq(pName, "vkGetDisplayModeProperties2KHR")) return to_vkVoidFunction(driver_GetDisplayModeProperties2KHR);
        if (string_eq(pName, "vkGetDisplayPlaneCapabilities2KHR")) return to_vkVoidFunction(driver_GetDisplayPlaneCapabilities2KHR);
    } // VK_KHR_get_display_properties2

    // ----- VK_KHR_get_memory_requirements2
    if (string_eq(pName, "vkGetImageMemoryRequirements2KHR")) return to_vkVoidFunction(driver_GetImageMemoryRequirements2KHR);
    if (string_eq(pName, "vkGetBufferMemoryRequirements2KHR")) return to_vkVoidFunction(driver_GetBufferMemoryRequirements2KHR);
    if (string_eq(pName, "vkGetImageSparseMemoryRequirements2KHR")) return to_vkVoidFunction(driver_GetImageSparseMemoryRequirements2KHR);

    // ----- VK_KHR_sampler_ycbcr_conversion
    if (string_eq(pName, "vkCreateSamplerYcbcrConversionKHR")) return to_vkVoidFunction(driver_CreateSamplerYcbcrConversionKHR);
    if (string_eq(pName, "vkDestroySamplerYcbcrConversionKHR")) return to_vkVoidFunction(driver_DestroySamplerYcbcrConversionKHR);

    // ----- VK_KHR_bind_memory2
    if (string_eq(pName, "vkBindBufferMemory2KHR")) return to_vkVoidFunction(driver_BindBufferMemory2KHR);
    if (string_eq(pName, "vkBindImageMemory2KHR")) return to_vkVoidFunction(driver_BindImageMemory2KHR);

    // ----- VK_KHR_maintenance3
    if (string_eq(pName, "vkGetDescriptorSetLayoutSupportKHR")) return to_vkVoidFunction(driver_GetDescriptorSetLayoutSupportKHR);

    // ----- VK_KHR_draw_indirect_count
    if (string_eq(pName, "vkCmdDrawIndirectCountKHR")) return to_vkVoidFunction(driver_CmdDrawIndirectCountKHR);
    if (string_eq(pName, "vkCmdDrawIndexedIndirectCountKHR")) return to_vkVoidFunction(driver_CmdDrawIndexedIndirectCountKHR);

    // ----- VK_KHR_timeline_semaphore
    if (string_eq(pName, "vkGetSemaphoreCounterValueKHR")) return to_vkVoidFunction(driver_GetSemaphoreCounterValueKHR);
    if (string_eq(pName, "vkWaitSemaphoresKHR")) return to_vkVoidFunction(driver_WaitSemaphoresKHR);
    if (string_eq(pName, "vkSignalSemaphoreKHR")) return to_vkVoidFunction(driver_SignalSemaphoreKHR);

    // ----- VK_KHR_fragment_shading_rate
    if (string_eq(pName, "vkGetPhysicalDeviceFragmentShadingRatesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceFragmentShadingRatesKHR);
    if (string_eq(pName, "vkCmdSetFragmentShadingRateKHR")) return to_vkVoidFunction(driver_CmdSetFragmentShadingRateKHR);

    // ----- VK_KHR_present_wait
    if (string_eq(pName, "vkWaitForPresentKHR")) return to_vkVoidFunction(driver_WaitForPresentKHR);

    // ----- VK_KHR_buffer_device_address
    if (string_eq(pName, "vkGetBufferDeviceAddressKHR")) return to_vkVoidFunction(driver_GetBufferDeviceAddressKHR);
    if (string_eq(pName, "vkGetBufferOpaqueCaptureAddressKHR")) return to_vkVoidFunction(driver_GetBufferOpaqueCaptureAddressKHR);
    if (string_eq(pName, "vkGetDeviceMemoryOpaqueCaptureAddressKHR")) return to_vkVoidFunction(driver_GetDeviceMemoryOpaqueCaptureAddressKHR);

    // ----- VK_KHR_deferred_host_operations
    if (string_eq(pName, "vkCreateDeferredOperationKHR")) return to_vkVoidFunction(driver_CreateDeferredOperationKHR);
    if (string_eq(pName, "vkDestroyDeferredOperationKHR")) return to_vkVoidFunction(driver_DestroyDeferredOperationKHR);
    if (string_eq(pName, "vkGetDeferredOperationMaxConcurrencyKHR")) return to_vkVoidFunction(driver_GetDeferredOperationMaxConcurrencyKHR);
    if (string_eq(pName, "vkGetDeferredOperationResultKHR")) return to_vkVoidFunction(driver_GetDeferredOperationResultKHR);
    if (string_eq(pName, "vkDeferredOperationJoinKHR")) return to_vkVoidFunction(driver_DeferredOperationJoinKHR);

    // ----- VK_KHR_pipeline_executable_properties
    if (string_eq(pName, "vkGetPipelineExecutablePropertiesKHR")) return to_vkVoidFunction(driver_GetPipelineExecutablePropertiesKHR);
    if (string_eq(pName, "vkGetPipelineExecutableStatisticsKHR")) return to_vkVoidFunction(driver_GetPipelineExecutableStatisticsKHR);
    if (string_eq(pName, "vkGetPipelineExecutableInternalRepresentationsKHR")) return to_vkVoidFunction(driver_GetPipelineExecutableInternalRepresentationsKHR);

    // ----- VK_KHR_video_encode_queue
#ifdef VK_ENABLE_BETA_EXTENSIONS
    if (string_eq(pName, "vkCmdEncodeVideoKHR")) return to_vkVoidFunction(driver_CmdEncodeVideoKHR);
#endif // VK_ENABLE_BETA_EXTENSIONS

    // ----- VK_KHR_synchronization2
    if (string_eq(pName, "vkCmdSetEvent2KHR")) return to_vkVoidFunction(driver_CmdSetEvent2KHR);
    if (string_eq(pName, "vkCmdResetEvent2KHR")) return to_vkVoidFunction(driver_CmdResetEvent2KHR);
    if (string_eq(pName, "vkCmdWaitEvents2KHR")) return to_vkVoidFunction(driver_CmdWaitEvents2KHR);
    if (string_eq(pName, "vkCmdPipelineBarrier2KHR")) return to_vkVoidFunction(driver_CmdPipelineBarrier2KHR);
    if (string_eq(pName, "vkCmdWriteTimestamp2KHR")) return to_vkVoidFunction(driver_CmdWriteTimestamp2KHR);
    if (string_eq(pName, "vkQueueSubmit2KHR")) return to_vkVoidFunction(driver_QueueSubmit2KHR);
    if (string_eq(pName, "vkCmdWriteBufferMarker2AMD")) return to_vkVoidFunction(driver_CmdWriteBufferMarker2AMD);
    if (string_eq(pName, "vkGetQueueCheckpointData2NV")) return to_vkVoidFunction(driver_GetQueueCheckpointData2NV);

    // ----- VK_KHR_copy_commands2
    if (string_eq(pName, "vkCmdCopyBuffer2KHR")) return to_vkVoidFunction(driver_CmdCopyBuffer2KHR);
    if (string_eq(pName, "vkCmdCopyImage2KHR")) return to_vkVoidFunction(driver_CmdCopyImage2KHR);
    if (string_eq(pName, "vkCmdCopyBufferToImage2KHR")) return to_vkVoidFunction(driver_CmdCopyBufferToImage2KHR);
    if (string_eq(pName, "vkCmdCopyImageToBuffer2KHR")) return to_vkVoidFunction(driver_CmdCopyImageToBuffer2KHR);
    if (string_eq(pName, "vkCmdBlitImage2KHR")) return to_vkVoidFunction(driver_CmdBlitImage2KHR);
    if (string_eq(pName, "vkCmdResolveImage2KHR")) return to_vkVoidFunction(driver_CmdResolveImage2KHR);

    // ----- VK_KHR_maintenance4
    if (string_eq(pName, "vkGetDeviceBufferMemoryRequirementsKHR")) return to_vkVoidFunction(driver_GetDeviceBufferMemoryRequirementsKHR);
    if (string_eq(pName, "vkGetDeviceImageMemoryRequirementsKHR")) return to_vkVoidFunction(driver_GetDeviceImageMemoryRequirementsKHR);
    if (string_eq(pName, "vkGetDeviceImageSparseMemoryRequirementsKHR")) return to_vkVoidFunction(driver_GetDeviceImageSparseMemoryRequirementsKHR);

    // ----- VK_EXT_debug_report
    if (IsInstanceExtensionEnabled("VK_EXT_debug_report")) {
        if (string_eq(pName, "vkCreateDebugReportCallbackEXT")) return to_vkVoidFunction(driver_CreateDebugReportCallbackEXT);
        if (string_eq(pName, "vkDestroyDebugReportCallbackEXT")) return to_vkVoidFunction(driver_DestroyDebugReportCallbackEXT);
        if (string_eq(pName, "vkDebugReportMessageEXT")) return to_vkVoidFunction(driver_DebugReportMessageEXT);
    } // VK_EXT_debug_report

    // ----- VK_EXT_debug_marker
    if (string_eq(pName, "vkDebugMarkerSetObjectTagEXT")) return to_vkVoidFunction(driver_DebugMarkerSetObjectTagEXT);
    if (string_eq(pName, "vkDebugMarkerSetObjectNameEXT")) return to_vkVoidFunction(driver_DebugMarkerSetObjectNameEXT);
    if (string_eq(pName, "vkCmdDebugMarkerBeginEXT")) return to_vkVoidFunction(driver_CmdDebugMarkerBeginEXT);
    if (string_eq(pName, "vkCmdDebugMarkerEndEXT")) return to_vkVoidFunction(driver_CmdDebugMarkerEndEXT);
    if (string_eq(pName, "vkCmdDebugMarkerInsertEXT")) return to_vkVoidFunction(driver_CmdDebugMarkerInsertEXT);

    // ----- VK_EXT_transform_feedback
    if (string_eq(pName, "vkCmdBindTransformFeedbackBuffersEXT")) return to_vkVoidFunction(driver_CmdBindTransformFeedbackBuffersEXT);
    if (string_eq(pName, "vkCmdBeginTransformFeedbackEXT")) return to_vkVoidFunction(driver_CmdBeginTransformFeedbackEXT);
    if (string_eq(pName, "vkCmdEndTransformFeedbackEXT")) return to_vkVoidFunction(driver_CmdEndTransformFeedbackEXT);
    if (string_eq(pName, "vkCmdBeginQueryIndexedEXT")) return to_vkVoidFunction(driver_CmdBeginQueryIndexedEXT);
    if (string_eq(pName, "vkCmdEndQueryIndexedEXT")) return to_vkVoidFunction(driver_CmdEndQueryIndexedEXT);
    if (string_eq(pName, "vkCmdDrawIndirectByteCountEXT")) return to_vkVoidFunction(driver_CmdDrawIndirectByteCountEXT);

    // ----- VK_NVX_binary_import
    if (string_eq(pName, "vkCreateCuModuleNVX")) return to_vkVoidFunction(driver_CreateCuModuleNVX);
    if (string_eq(pName, "vkCreateCuFunctionNVX")) return to_vkVoidFunction(driver_CreateCuFunctionNVX);
    if (string_eq(pName, "vkDestroyCuModuleNVX")) return to_vkVoidFunction(driver_DestroyCuModuleNVX);
    if (string_eq(pName, "vkDestroyCuFunctionNVX")) return to_vkVoidFunction(driver_DestroyCuFunctionNVX);
    if (string_eq(pName, "vkCmdCuLaunchKernelNVX")) return to_vkVoidFunction(driver_CmdCuLaunchKernelNVX);

    // ----- VK_NVX_image_view_handle
    if (string_eq(pName, "vkGetImageViewHandleNVX")) return to_vkVoidFunction(driver_GetImageViewHandleNVX);
    if (string_eq(pName, "vkGetImageViewAddressNVX")) return to_vkVoidFunction(driver_GetImageViewAddressNVX);

    // ----- VK_AMD_draw_indirect_count
    if (string_eq(pName, "vkCmdDrawIndirectCountAMD")) return to_vkVoidFunction(driver_CmdDrawIndirectCountAMD);
    if (string_eq(pName, "vkCmdDrawIndexedIndirectCountAMD")) return to_vkVoidFunction(driver_CmdDrawIndexedIndirectCountAMD);

    // ----- VK_AMD_shader_info
    if (string_eq(pName, "vkGetShaderInfoAMD")) return to_vkVoidFunction(driver_GetShaderInfoAMD);

    // ----- VK_GGP_stream_descriptor_surface
#ifdef VK_USE_PLATFORM_GGP
    if (IsInstanceExtensionEnabled("VK_GGP_stream_descriptor_surface")) {
        if (string_eq(pName, "vkCreateStreamDescriptorSurfaceGGP")) return to_vkVoidFunction(driver_CreateStreamDescriptorSurfaceGGP);
    } // VK_GGP_stream_descriptor_surface
#endif // VK_USE_PLATFORM_GGP

    // ----- VK_NV_external_memory_capabilities
    if (IsInstanceExtensionEnabled("VK_NV_external_memory_capabilities")) {
        if (string_eq(pName, "vkGetPhysicalDeviceExternalImageFormatPropertiesNV")) return to_vkVoidFunction(driver_GetPhysicalDeviceExternalImageFormatPropertiesNV);
    } // VK_NV_external_memory_capabilities

    // ----- VK_NV_external_memory_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (string_eq(pName, "vkGetMemoryWin32HandleNV")) return to_vkVoidFunction(driver_GetMemoryWin32HandleNV);
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_NN_vi_surface
#ifdef VK_USE_PLATFORM_VI_NN
    if (IsInstanceExtensionEnabled("VK_NN_vi_surface")) {
        if (string_eq(pName, "vkCreateViSurfaceNN")) return to_vkVoidFunction(driver_CreateViSurfaceNN);
    } // VK_NN_vi_surface
#endif // VK_USE_PLATFORM_VI_NN

    // ----- VK_EXT_conditional_rendering
    if (string_eq(pName, "vkCmdBeginConditionalRenderingEXT")) return to_vkVoidFunction(driver_CmdBeginConditionalRenderingEXT);
    if (string_eq(pName, "vkCmdEndConditionalRenderingEXT")) return to_vkVoidFunction(driver_CmdEndConditionalRenderingEXT);

    // ----- VK_NV_clip_space_w_scaling
    if (string_eq(pName, "vkCmdSetViewportWScalingNV")) return to_vkVoidFunction(driver_CmdSetViewportWScalingNV);

    // ----- VK_EXT_direct_mode_display
    if (IsInstanceExtensionEnabled("VK_EXT_direct_mode_display")) {
        if (string_eq(pName, "vkReleaseDisplayEXT")) return to_vkVoidFunction(driver_ReleaseDisplayEXT);
    } // VK_EXT_direct_mode_display

    // ----- VK_EXT_acquire_xlib_display
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
    if (IsInstanceExtensionEnabled("VK_EXT_acquire_xlib_display")) {
        if (string_eq(pName, "vkAcquireXlibDisplayEXT")) return to_vkVoidFunction(driver_AcquireXlibDisplayEXT);
        if (string_eq(pName, "vkGetRandROutputDisplayEXT")) return to_vkVoidFunction(driver_GetRandROutputDisplayEXT);
    } // VK_EXT_acquire_xlib_display
#endif // VK_USE_PLATFORM_XLIB_XRANDR_EXT

    // ----- VK_EXT_display_surface_counter
    if (IsInstanceExtensionEnabled("VK_EXT_display_surface_counter")) {
        if (string_eq(pName, "vkGetPhysicalDeviceSurfaceCapabilities2EXT")) return to_vkVoidFunction(driver_GetPhysicalDeviceSurfaceCapabilities2EXT);
    } // VK_EXT_display_surface_counter

    // ----- VK_EXT_display_control
    if (string_eq(pName, "vkDisplayPowerControlEXT")) return to_vkVoidFunction(driver_DisplayPowerControlEXT);
    if (string_eq(pName, "vkRegisterDeviceEventEXT")) return to_vkVoidFunction(driver_RegisterDeviceEventEXT);
    if (string_eq(pName, "vkRegisterDisplayEventEXT")) return to_vkVoidFunction(driver_RegisterDisplayEventEXT);
    if (string_eq(pName, "vkGetSwapchainCounterEXT")) return to_vkVoidFunction(driver_GetSwapchainCounterEXT);

    // ----- VK_GOOGLE_display_timing
    if (string_eq(pName, "vkGetRefreshCycleDurationGOOGLE")) return to_vkVoidFunction(driver_GetRefreshCycleDurationGOOGLE);
    if (string_eq(pName, "vkGetPastPresentationTimingGOOGLE")) return to_vkVoidFunction(driver_GetPastPresentationTimingGOOGLE);

    // ----- VK_EXT_discard_rectangles
    if (string_eq(pName, "vkCmdSetDiscardRectangleEXT")) return to_vkVoidFunction(driver_CmdSetDiscardRectangleEXT);

    // ----- VK_EXT_hdr_metadata
    if (string_eq(pName, "vkSetHdrMetadataEXT")) return to_vkVoidFunction(driver_SetHdrMetadataEXT);

    // ----- VK_MVK_ios_surface
#ifdef VK_USE_PLATFORM_IOS_MVK
    if (IsInstanceExtensionEnabled("VK_MVK_ios_surface")) {
        if (string_eq(pName, "vkCreateIOSSurfaceMVK")) return to_vkVoidFunction(driver_CreateIOSSurfaceMVK);
    } // VK_MVK_ios_surface
#endif // VK_USE_PLATFORM_IOS_MVK

    // ----- VK_MVK_macos_surface
#ifdef VK_USE_PLATFORM_MACOS_MVK
    if (IsInstanceExtensionEnabled("VK_MVK_macos_surface")) {
        if (string_eq(pName, "vkCreateMacOSSurfaceMVK")) return to_vkVoidFunction(driver_CreateMacOSSurfaceMVK);
    } // VK_MVK_macos_surface
#endif // VK_USE_PLATFORM_MACOS_MVK

    // ----- VK_EXT_debug_utils
    if (IsInstanceExtensionEnabled("VK_EXT_debug_utils")) {
        if (string_eq(pName, "vkSetDebugUtilsObjectNameEXT")) return to_vkVoidFunction(driver_SetDebugUtilsObjectNameEXT);
        if (string_eq(pName, "vkSetDebugUtilsObjectTagEXT")) return to_vkVoidFunction(driver_SetDebugUtilsObjectTagEXT);
        if (string_eq(pName, "vkQueueBeginDebugUtilsLabelEXT")) return to_vkVoidFunction(driver_QueueBeginDebugUtilsLabelEXT);
        if (string_eq(pName, "vkQueueEndDebugUtilsLabelEXT")) return to_vkVoidFunction(driver_QueueEndDebugUtilsLabelEXT);
        if (string_eq(pName, "vkQueueInsertDebugUtilsLabelEXT")) return to_vkVoidFunction(driver_QueueInsertDebugUtilsLabelEXT);
        if (string_eq(pName, "vkCmdBeginDebugUtilsLabelEXT")) return to_vkVoidFunction(driver_CmdBeginDebugUtilsLabelEXT);
        if (string_eq(pName, "vkCmdEndDebugUtilsLabelEXT")) return to_vkVoidFunction(driver_CmdEndDebugUtilsLabelEXT);
        if (string_eq(pName, "vkCmdInsertDebugUtilsLabelEXT")) return to_vkVoidFunction(driver_CmdInsertDebugUtilsLabelEXT);
        if (string_eq(pName, "vkCreateDebugUtilsMessengerEXT")) return to_vkVoidFunction(driver_CreateDebugUtilsMessengerEXT);
        if (string_eq(pName, "vkDestroyDebugUtilsMessengerEXT")) return to_vkVoidFunction(driver_DestroyDebugUtilsMessengerEXT);
        if (string_eq(pName, "vkSubmitDebugUtilsMessageEXT")) return to_vkVoidFunction(driver_SubmitDebugUtilsMessageEXT);
    } // VK_EXT_debug_utils

    // ----- VK_EXT_sample_locations
    if (string_eq(pName, "vkCmdSetSampleLocationsEXT")) return to_vkVoidFunction(driver_CmdSetSampleLocationsEXT);
    if (string_eq(pName, "vkGetPhysicalDeviceMultisamplePropertiesEXT")) return to_vkVoidFunction(driver_GetPhysicalDeviceMultisamplePropertiesEXT);

    // ----- VK_EXT_image_drm_format_modifier
    if (string_eq(pName, "vkGetImageDrmFormatModifierPropertiesEXT")) return to_vkVoidFunction(driver_GetImageDrmFormatModifierPropertiesEXT);

    // ----- VK_EXT_validation_cache
    if (string_eq(pName, "vkCreateValidationCacheEXT")) return to_vkVoidFunction(driver_CreateValidationCacheEXT);
    if (string_eq(pName, "vkDestroyValidationCacheEXT")) return to_vkVoidFunction(driver_DestroyValidationCacheEXT);
    if (string_eq(pName, "vkMergeValidationCachesEXT")) return to_vkVoidFunction(driver_MergeValidationCachesEXT);
    if (string_eq(pName, "vkGetValidationCacheDataEXT")) return to_vkVoidFunction(driver_GetValidationCacheDataEXT);

    // ----- VK_NV_shading_rate_image
    if (string_eq(pName, "vkCmdBindShadingRateImageNV")) return to_vkVoidFunction(driver_CmdBindShadingRateImageNV);
    if (string_eq(pName, "vkCmdSetViewportShadingRatePaletteNV")) return to_vkVoidFunction(driver_CmdSetViewportShadingRatePaletteNV);
    if (string_eq(pName, "vkCmdSetCoarseSampleOrderNV")) return to_vkVoidFunction(driver_CmdSetCoarseSampleOrderNV);

    // ----- VK_NV_ray_tracing
    if (string_eq(pName, "vkCreateAccelerationStructureNV")) return to_vkVoidFunction(driver_CreateAccelerationStructureNV);
    if (string_eq(pName, "vkDestroyAccelerationStructureNV")) return to_vkVoidFunction(driver_DestroyAccelerationStructureNV);
    if (string_eq(pName, "vkGetAccelerationStructureMemoryRequirementsNV")) return to_vkVoidFunction(driver_GetAccelerationStructureMemoryRequirementsNV);
    if (string_eq(pName, "vkBindAccelerationStructureMemoryNV")) return to_vkVoidFunction(driver_BindAccelerationStructureMemoryNV);
    if (string_eq(pName, "vkCmdBuildAccelerationStructureNV")) return to_vkVoidFunction(driver_CmdBuildAccelerationStructureNV);
    if (string_eq(pName, "vkCmdCopyAccelerationStructureNV")) return to_vkVoidFunction(driver_CmdCopyAccelerationStructureNV);
    if (string_eq(pName, "vkCmdTraceRaysNV")) return to_vkVoidFunction(driver_CmdTraceRaysNV);
    if (string_eq(pName, "vkCreateRayTracingPipelinesNV")) return to_vkVoidFunction(driver_CreateRayTracingPipelinesNV);
    if (string_eq(pName, "vkGetRayTracingShaderGroupHandlesNV")) return to_vkVoidFunction(driver_GetRayTracingShaderGroupHandlesNV);
    if (string_eq(pName, "vkGetAccelerationStructureHandleNV")) return to_vkVoidFunction(driver_GetAccelerationStructureHandleNV);
    if (string_eq(pName, "vkCmdWriteAccelerationStructuresPropertiesNV")) return to_vkVoidFunction(driver_CmdWriteAccelerationStructuresPropertiesNV);
    if (string_eq(pName, "vkCompileDeferredNV")) return to_vkVoidFunction(driver_CompileDeferredNV);

    // ----- VK_EXT_external_memory_host
    if (string_eq(pName, "vkGetMemoryHostPointerPropertiesEXT")) return to_vkVoidFunction(driver_GetMemoryHostPointerPropertiesEXT);

    // ----- VK_AMD_buffer_marker
    if (string_eq(pName, "vkCmdWriteBufferMarkerAMD")) return to_vkVoidFunction(driver_CmdWriteBufferMarkerAMD);

    // ----- VK_EXT_calibrated_timestamps
    if (string_eq(pName, "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT")) return to_vkVoidFunction(driver_GetPhysicalDeviceCalibrateableTimeDomainsEXT);
    if (string_eq(pName, "vkGetCalibratedTimestampsEXT")) return to_vkVoidFunction(driver_GetCalibratedTimestampsEXT);

    // ----- VK_NV_mesh_shader
    if (string_eq(pName, "vkCmdDrawMeshTasksNV")) return to_vkVoidFunction(driver_CmdDrawMeshTasksNV);
    if (string_eq(pName, "vkCmdDrawMeshTasksIndirectNV")) return to_vkVoidFunction(driver_CmdDrawMeshTasksIndirectNV);
    if (string_eq(pName, "vkCmdDrawMeshTasksIndirectCountNV")) return to_vkVoidFunction(driver_CmdDrawMeshTasksIndirectCountNV);

    // ----- VK_NV_scissor_exclusive
    if (string_eq(pName, "vkCmdSetExclusiveScissorNV")) return to_vkVoidFunction(driver_CmdSetExclusiveScissorNV);

    // ----- VK_NV_device_diagnostic_checkpoints
    if (string_eq(pName, "vkCmdSetCheckpointNV")) return to_vkVoidFunction(driver_CmdSetCheckpointNV);
    if (string_eq(pName, "vkGetQueueCheckpointDataNV")) return to_vkVoidFunction(driver_GetQueueCheckpointDataNV);

    // ----- VK_INTEL_performance_query
    if (string_eq(pName, "vkInitializePerformanceApiINTEL")) return to_vkVoidFunction(driver_InitializePerformanceApiINTEL);
    if (string_eq(pName, "vkUninitializePerformanceApiINTEL")) return to_vkVoidFunction(driver_UninitializePerformanceApiINTEL);
    if (string_eq(pName, "vkCmdSetPerformanceMarkerINTEL")) return to_vkVoidFunction(driver_CmdSetPerformanceMarkerINTEL);
    if (string_eq(pName, "vkCmdSetPerformanceStreamMarkerINTEL")) return to_vkVoidFunction(driver_CmdSetPerformanceStreamMarkerINTEL);
    if (string_eq(pName, "vkCmdSetPerformanceOverrideINTEL")) return to_vkVoidFunction(driver_CmdSetPerformanceOverrideINTEL);
    if (string_eq(pName, "vkAcquirePerformanceConfigurationINTEL")) return to_vkVoidFunction(driver_AcquirePerformanceConfigurationINTEL);
    if (string_eq(pName, "vkReleasePerformanceConfigurationINTEL")) return to_vkVoidFunction(driver_ReleasePerformanceConfigurationINTEL);
    if (string_eq(pName, "vkQueueSetPerformanceConfigurationINTEL")) return to_vkVoidFunction(driver_QueueSetPerformanceConfigurationINTEL);
    if (string_eq(pName, "vkGetPerformanceParameterINTEL")) return to_vkVoidFunction(driver_GetPerformanceParameterINTEL);

    // ----- VK_AMD_display_native_hdr
    if (string_eq(pName, "vkSetLocalDimmingAMD")) return to_vkVoidFunction(driver_SetLocalDimmingAMD);

    // ----- VK_FUCHSIA_imagepipe_surface
#ifdef VK_USE_PLATFORM_FUCHSIA
    if (IsInstanceExtensionEnabled("VK_FUCHSIA_imagepipe_surface")) {
        if (string_eq(pName, "vkCreateImagePipeSurfaceFUCHSIA")) return to_vkVoidFunction(driver_CreateImagePipeSurfaceFUCHSIA);
    } // VK_FUCHSIA_imagepipe_surface
#endif // VK_USE_PLATFORM_FUCHSIA

    // ----- VK_EXT_metal_surface
#ifdef VK_USE_PLATFORM_METAL_EXT
    if (IsInstanceExtensionEnabled("VK_EXT_metal_surface")) {
        if (string_eq(pName, "vkCreateMetalSurfaceEXT")) return to_vkVoidFunction(driver_CreateMetalSurfaceEXT);
    } // VK_EXT_metal_surface
#endif // VK_USE_PLATFORM_METAL_EXT

    // ----- VK_EXT_buffer_device_address
    if (string_eq(pName, "vkGetBufferDeviceAddressEXT")) return to_vkVoidFunction(driver_GetBufferDeviceAddressEXT);

    // ----- VK_EXT_tooling_info
    if (string_eq(pName, "vkGetPhysicalDeviceToolPropertiesEXT")) return to_vkVoidFunction(driver_GetPhysicalDeviceToolPropertiesEXT);

    // ----- VK_NV_cooperative_matrix
    if (string_eq(pName, "vkGetPhysicalDeviceCooperativeMatrixPropertiesNV")) return to_vkVoidFunction(driver_GetPhysicalDeviceCooperativeMatrixPropertiesNV);

    // ----- VK_NV_coverage_reduction_mode
    if (string_eq(pName, "vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV")) return to_vkVoidFunction(driver_GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV);

    // ----- VK_EXT_full_screen_exclusive
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (string_eq(pName, "vkGetPhysicalDeviceSurfacePresentModes2EXT")) return to_vkVoidFunction(driver_GetPhysicalDeviceSurfacePresentModes2EXT);
    if (string_eq(pName, "vkAcquireFullScreenExclusiveModeEXT")) return to_vkVoidFunction(driver_AcquireFullScreenExclusiveModeEXT);
    if (string_eq(pName, "vkReleaseFullScreenExclusiveModeEXT")) return to_vkVoidFunction(driver_ReleaseFullScreenExclusiveModeEXT);
    if (string_eq(pName, "vkGetDeviceGroupSurfacePresentModes2EXT")) return to_vkVoidFunction(driver_GetDeviceGroupSurfacePresentModes2EXT);
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_EXT_headless_surface
    if (IsInstanceExtensionEnabled("VK_EXT_headless_surface")) {
        if (string_eq(pName, "vkCreateHeadlessSurfaceEXT")) return to_vkVoidFunction(driver_CreateHeadlessSurfaceEXT);
    } // VK_EXT_headless_surface

    // ----- VK_EXT_line_rasterization
    if (string_eq(pName, "vkCmdSetLineStippleEXT")) return to_vkVoidFunction(driver_CmdSetLineStippleEXT);

    // ----- VK_EXT_host_query_reset
    if (string_eq(pName, "vkResetQueryPoolEXT")) return to_vkVoidFunction(driver_ResetQueryPoolEXT);

    // ----- VK_EXT_extended_dynamic_state
    if (string_eq(pName, "vkCmdSetCullModeEXT")) return to_vkVoidFunction(driver_CmdSetCullModeEXT);
    if (string_eq(pName, "vkCmdSetFrontFaceEXT")) return to_vkVoidFunction(driver_CmdSetFrontFaceEXT);
    if (string_eq(pName, "vkCmdSetPrimitiveTopologyEXT")) return to_vkVoidFunction(driver_CmdSetPrimitiveTopologyEXT);
    if (string_eq(pName, "vkCmdSetViewportWithCountEXT")) return to_vkVoidFunction(driver_CmdSetViewportWithCountEXT);
    if (string_eq(pName, "vkCmdSetScissorWithCountEXT")) return to_vkVoidFunction(driver_CmdSetScissorWithCountEXT);
    if (string_eq(pName, "vkCmdBindVertexBuffers2EXT")) return to_vkVoidFunction(driver_CmdBindVertexBuffers2EXT);
    if (string_eq(pName, "vkCmdSetDepthTestEnableEXT")) return to_vkVoidFunction(driver_CmdSetDepthTestEnableEXT);
    if (string_eq(pName, "vkCmdSetDepthWriteEnableEXT")) return to_vkVoidFunction(driver_CmdSetDepthWriteEnableEXT);
    if (string_eq(pName, "vkCmdSetDepthCompareOpEXT")) return to_vkVoidFunction(driver_CmdSetDepthCompareOpEXT);
    if (string_eq(pName, "vkCmdSetDepthBoundsTestEnableEXT")) return to_vkVoidFunction(driver_CmdSetDepthBoundsTestEnableEXT);
    if (string_eq(pName, "vkCmdSetStencilTestEnableEXT")) return to_vkVoidFunction(driver_CmdSetStencilTestEnableEXT);
    if (string_eq(pName, "vkCmdSetStencilOpEXT")) return to_vkVoidFunction(driver_CmdSetStencilOpEXT);

    // ----- VK_NV_device_generated_commands
    if (string_eq(pName, "vkGetGeneratedCommandsMemoryRequirementsNV")) return to_vkVoidFunction(driver_GetGeneratedCommandsMemoryRequirementsNV);
    if (string_eq(pName, "vkCmdPreprocessGeneratedCommandsNV")) return to_vkVoidFunction(driver_CmdPreprocessGeneratedCommandsNV);
    if (string_eq(pName, "vkCmdExecuteGeneratedCommandsNV")) return to_vkVoidFunction(driver_CmdExecuteGeneratedCommandsNV);
    if (string_eq(pName, "vkCmdBindPipelineShaderGroupNV")) return to_vkVoidFunction(driver_CmdBindPipelineShaderGroupNV);
    if (string_eq(pName, "vkCreateIndirectCommandsLayoutNV")) return to_vkVoidFunction(driver_CreateIndirectCommandsLayoutNV);
    if (string_eq(pName, "vkDestroyIndirectCommandsLayoutNV")) return to_vkVoidFunction(driver_DestroyIndirectCommandsLayoutNV);

    // ----- VK_EXT_acquire_drm_display
    if (IsInstanceExtensionEnabled("VK_EXT_acquire_drm_display")) {
        if (string_eq(pName, "vkAcquireDrmDisplayEXT")) return to_vkVoidFunction(driver_AcquireDrmDisplayEXT);
        if (string_eq(pName, "vkGetDrmDisplayEXT")) return to_vkVoidFunction(driver_GetDrmDisplayEXT);
    } // VK_EXT_acquire_drm_display

    // ----- VK_EXT_private_data
    if (string_eq(pName, "vkCreatePrivateDataSlotEXT")) return to_vkVoidFunction(driver_CreatePrivateDataSlotEXT);
    if (string_eq(pName, "vkDestroyPrivateDataSlotEXT")) return to_vkVoidFunction(driver_DestroyPrivateDataSlotEXT);
    if (string_eq(pName, "vkSetPrivateDataEXT")) return to_vkVoidFunction(driver_SetPrivateDataEXT);
    if (string_eq(pName, "vkGetPrivateDataEXT")) return to_vkVoidFunction(driver_GetPrivateDataEXT);

    // ----- VK_NV_fragment_shading_rate_enums
    if (string_eq(pName, "vkCmdSetFragmentShadingRateEnumNV")) return to_vkVoidFunction(driver_CmdSetFragmentShadingRateEnumNV);

    // ----- VK_NV_acquire_winrt_display
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (string_eq(pName, "vkAcquireWinrtDisplayNV")) return to_vkVoidFunction(driver_AcquireWinrtDisplayNV);
    if (string_eq(pName, "vkGetWinrtDisplayNV")) return to_vkVoidFunction(driver_GetWinrtDisplayNV);
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_EXT_directfb_surface
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
    if (IsInstanceExtensionEnabled("VK_EXT_directfb_surface")) {
        if (string_eq(pName, "vkCreateDirectFBSurfaceEXT")) return to_vkVoidFunction(driver_CreateDirectFBSurfaceEXT);
        if (string_eq(pName, "vkGetPhysicalDeviceDirectFBPresentationSupportEXT")) return to_vkVoidFunction(driver_GetPhysicalDeviceDirectFBPresentationSupportEXT);
    } // VK_EXT_directfb_surface
#endif // VK_USE_PLATFORM_DIRECTFB_EXT

    // ----- VK_EXT_vertex_input_dynamic_state
    if (string_eq(pName, "vkCmdSetVertexInputEXT")) return to_vkVoidFunction(driver_CmdSetVertexInputEXT);

    // ----- VK_FUCHSIA_external_memory
#ifdef VK_USE_PLATFORM_FUCHSIA
    if (string_eq(pName, "vkGetMemoryZirconHandleFUCHSIA")) return to_vkVoidFunction(driver_GetMemoryZirconHandleFUCHSIA);
    if (string_eq(pName, "vkGetMemoryZirconHandlePropertiesFUCHSIA")) return to_vkVoidFunction(driver_GetMemoryZirconHandlePropertiesFUCHSIA);
#endif // VK_USE_PLATFORM_FUCHSIA

    // ----- VK_FUCHSIA_external_semaphore
#ifdef VK_USE_PLATFORM_FUCHSIA
    if (string_eq(pName, "vkImportSemaphoreZirconHandleFUCHSIA")) return to_vkVoidFunction(driver_ImportSemaphoreZirconHandleFUCHSIA);
    if (string_eq(pName, "vkGetSemaphoreZirconHandleFUCHSIA")) return to_vkVoidFunction(driver_GetSemaphoreZirconHandleFUCHSIA);
#endif // VK_USE_PLATFORM_FUCHSIA

    // ----- VK_FUCHSIA_buffer_collection
#ifdef VK_USE_PLATFORM_FUCHSIA
    if (string_eq(pName, "vkCreateBufferCollectionFUCHSIA")) return to_vkVoidFunction(driver_CreateBufferCollectionFUCHSIA);
    if (string_eq(pName, "vkSetBufferCollectionImageConstraintsFUCHSIA")) return to_vkVoidFunction(driver_SetBufferCollectionImageConstraintsFUCHSIA);
    if (string_eq(pName, "vkSetBufferCollectionBufferConstraintsFUCHSIA")) return to_vkVoidFunction(driver_SetBufferCollectionBufferConstraintsFUCHSIA);
    if (string_eq(pName, "vkDestroyBufferCollectionFUCHSIA")) return to_vkVoidFunction(driver_DestroyBufferCollectionFUCHSIA);
    if (string_eq(pName, "vkGetBufferCollectionPropertiesFUCHSIA")) return to_vkVoidFunction(driver_GetBufferCollectionPropertiesFUCHSIA);
#endif // VK_USE_PLATFORM_FUCHSIA

    // ----- VK_HUAWEI_subpass_shading
    if (string_eq(pName, "vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI")) return to_vkVoidFunction(driver_GetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI);
    if (string_eq(pName, "vkCmdSubpassShadingHUAWEI")) return to_vkVoidFunction(driver_CmdSubpassShadingHUAWEI);

    // ----- VK_HUAWEI_invocation_mask
    if (string_eq(pName, "vkCmdBindInvocationMaskHUAWEI")) return to_vkVoidFunction(driver_CmdBindInvocationMaskHUAWEI);

    // ----- VK_NV_external_memory_rdma
    if (string_eq(pName, "vkGetMemoryRemoteAddressNV")) return to_vkVoidFunction(driver_GetMemoryRemoteAddressNV);

    // ----- VK_EXT_extended_dynamic_state2
    if (string_eq(pName, "vkCmdSetPatchControlPointsEXT")) return to_vkVoidFunction(driver_CmdSetPatchControlPointsEXT);
    if (string_eq(pName, "vkCmdSetRasterizerDiscardEnableEXT")) return to_vkVoidFunction(driver_CmdSetRasterizerDiscardEnableEXT);
    if (string_eq(pName, "vkCmdSetDepthBiasEnableEXT")) return to_vkVoidFunction(driver_CmdSetDepthBiasEnableEXT);
    if (string_eq(pName, "vkCmdSetLogicOpEXT")) return to_vkVoidFunction(driver_CmdSetLogicOpEXT);
    if (string_eq(pName, "vkCmdSetPrimitiveRestartEnableEXT")) return to_vkVoidFunction(driver_CmdSetPrimitiveRestartEnableEXT);

    // ----- VK_QNX_screen_surface
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    if (IsInstanceExtensionEnabled("VK_QNX_screen_surface")) {
        if (string_eq(pName, "vkCreateScreenSurfaceQNX")) return to_vkVoidFunction(driver_CreateScreenSurfaceQNX);
        if (string_eq(pName, "vkGetPhysicalDeviceScreenPresentationSupportQNX")) return to_vkVoidFunction(driver_GetPhysicalDeviceScreenPresentationSupportQNX);
    } // VK_QNX_screen_surface
#endif // VK_USE_PLATFORM_SCREEN_QNX

    // ----- VK_EXT_color_write_enable
    if (string_eq(pName, "vkCmdSetColorWriteEnableEXT")) return to_vkVoidFunction(driver_CmdSetColorWriteEnableEXT);

    // ----- VK_EXT_multi_draw
    if (string_eq(pName, "vkCmdDrawMultiEXT")) return to_vkVoidFunction(driver_CmdDrawMultiEXT);
    if (string_eq(pName, "vkCmdDrawMultiIndexedEXT")) return to_vkVoidFunction(driver_CmdDrawMultiIndexedEXT);

    // ----- VK_EXT_pageable_device_local_memory
    if (string_eq(pName, "vkSetDeviceMemoryPriorityEXT")) return to_vkVoidFunction(driver_SetDeviceMemoryPriorityEXT);

    // ----- VK_VALVE_descriptor_set_host_mapping
    if (string_eq(pName, "vkGetDescriptorSetLayoutHostMappingInfoVALVE")) return to_vkVoidFunction(driver_GetDescriptorSetLayoutHostMappingInfoVALVE);
    if (string_eq(pName, "vkGetDescriptorSetHostMappingVALVE")) return to_vkVoidFunction(driver_GetDescriptorSetHostMappingVALVE);

    // ----- VK_KHR_acceleration_structure
    if (string_eq(pName, "vkCreateAccelerationStructureKHR")) return to_vkVoidFunction(driver_CreateAccelerationStructureKHR);
    if (string_eq(pName, "vkDestroyAccelerationStructureKHR")) return to_vkVoidFunction(driver_DestroyAccelerationStructureKHR);
    if (string_eq(pName, "vkCmdBuildAccelerationStructuresKHR")) return to_vkVoidFunction(driver_CmdBuildAccelerationStructuresKHR);
    if (string_eq(pName, "vkCmdBuildAccelerationStructuresIndirectKHR")) return to_vkVoidFunction(driver_CmdBuildAccelerationStructuresIndirectKHR);
    if (string_eq(pName, "vkBuildAccelerationStructuresKHR")) return to_vkVoidFunction(driver_BuildAccelerationStructuresKHR);
    if (string_eq(pName, "vkCopyAccelerationStructureKHR")) return to_vkVoidFunction(driver_CopyAccelerationStructureKHR);
    if (string_eq(pName, "vkCopyAccelerationStructureToMemoryKHR")) return to_vkVoidFunction(driver_CopyAccelerationStructureToMemoryKHR);
    if (string_eq(pName, "vkCopyMemoryToAccelerationStructureKHR")) return to_vkVoidFunction(driver_CopyMemoryToAccelerationStructureKHR);
    if (string_eq(pName, "vkWriteAccelerationStructuresPropertiesKHR")) return to_vkVoidFunction(driver_WriteAccelerationStructuresPropertiesKHR);
    if (string_eq(pName, "vkCmdCopyAccelerationStructureKHR")) return to_vkVoidFunction(driver_CmdCopyAccelerationStructureKHR);
    if (string_eq(pName, "vkCmdCopyAccelerationStructureToMemoryKHR")) return to_vkVoidFunction(driver_CmdCopyAccelerationStructureToMemoryKHR);
    if (string_eq(pName, "vkCmdCopyMemoryToAccelerationStructureKHR")) return to_vkVoidFunction(driver_CmdCopyMemoryToAccelerationStructureKHR);
    if (string_eq(pName, "vkGetAccelerationStructureDeviceAddressKHR")) return to_vkVoidFunction(driver_GetAccelerationStructureDeviceAddressKHR);
    if (string_eq(pName, "vkCmdWriteAccelerationStructuresPropertiesKHR")) return to_vkVoidFunction(driver_CmdWriteAccelerationStructuresPropertiesKHR);
    if (string_eq(pName, "vkGetDeviceAccelerationStructureCompatibilityKHR")) return to_vkVoidFunction(driver_GetDeviceAccelerationStructureCompatibilityKHR);
    if (string_eq(pName, "vkGetAccelerationStructureBuildSizesKHR")) return to_vkVoidFunction(driver_GetAccelerationStructureBuildSizesKHR);

    // ----- VK_KHR_ray_tracing_pipeline
    if (string_eq(pName, "vkCmdTraceRaysKHR")) return to_vkVoidFunction(driver_CmdTraceRaysKHR);
    if (string_eq(pName, "vkCreateRayTracingPipelinesKHR")) return to_vkVoidFunction(driver_CreateRayTracingPipelinesKHR);
    if (string_eq(pName, "vkGetRayTracingShaderGroupHandlesKHR")) return to_vkVoidFunction(driver_GetRayTracingShaderGroupHandlesKHR);
    if (string_eq(pName, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR")) return to_vkVoidFunction(driver_GetRayTracingCaptureReplayShaderGroupHandlesKHR);
    if (string_eq(pName, "vkCmdTraceRaysIndirectKHR")) return to_vkVoidFunction(driver_CmdTraceRaysIndirectKHR);
    if (string_eq(pName, "vkGetRayTracingShaderGroupStackSizeKHR")) return to_vkVoidFunction(driver_GetRayTracingShaderGroupStackSizeKHR);
    if (string_eq(pName, "vkCmdSetRayTracingPipelineStackSizeKHR")) return to_vkVoidFunction(driver_CmdSetRayTracingPipelineStackSizeKHR);

    return nullptr;
}

bool IsDeviceExtensionEnabled(const char* extension_name) {
    return driver.enabled_device_extensions.end() !=
           std::find_if(driver.enabled_device_extensions.begin(),
                        driver.enabled_device_extensions.end(),
                        [extension_name](Extension const& ext) { return ext.extensionName == extension_name; });
}

FRAMEWORK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL driver_GetDeviceProcAddr(VkDevice device, const char* pName) {

    // ----- VK_VERSION_1_0
    {
        if (string_eq(pName, "vkCreateInstance")) return to_vkVoidFunction(driver_CreateInstance);
        if (string_eq(pName, "vkDestroyInstance")) return to_vkVoidFunction(driver_DestroyInstance);
        if (string_eq(pName, "vkEnumeratePhysicalDevices")) return to_vkVoidFunction(driver_EnumeratePhysicalDevices);
        if (string_eq(pName, "vkGetPhysicalDeviceFeatures")) return to_vkVoidFunction(driver_GetPhysicalDeviceFeatures);
        if (string_eq(pName, "vkGetPhysicalDeviceFormatProperties")) return to_vkVoidFunction(driver_GetPhysicalDeviceFormatProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceImageFormatProperties")) return to_vkVoidFunction(driver_GetPhysicalDeviceImageFormatProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceProperties")) return to_vkVoidFunction(driver_GetPhysicalDeviceProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyProperties")) return to_vkVoidFunction(driver_GetPhysicalDeviceQueueFamilyProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceMemoryProperties")) return to_vkVoidFunction(driver_GetPhysicalDeviceMemoryProperties);
        if (string_eq(pName, "vkGetInstanceProcAddr")) return to_vkVoidFunction(driver_GetInstanceProcAddr);
        if (string_eq(pName, "vkGetDeviceProcAddr")) return to_vkVoidFunction(driver_GetDeviceProcAddr);
        if (string_eq(pName, "vkCreateDevice")) return to_vkVoidFunction(driver_CreateDevice);
        if (string_eq(pName, "vkDestroyDevice")) return to_vkVoidFunction(driver_DestroyDevice);
        if (string_eq(pName, "vkEnumerateInstanceExtensionProperties")) return to_vkVoidFunction(driver_EnumerateInstanceExtensionProperties);
        if (string_eq(pName, "vkEnumerateDeviceExtensionProperties")) return to_vkVoidFunction(driver_EnumerateDeviceExtensionProperties);
        if (string_eq(pName, "vkEnumerateInstanceLayerProperties")) return to_vkVoidFunction(driver_EnumerateInstanceLayerProperties);
        if (string_eq(pName, "vkEnumerateDeviceLayerProperties")) return to_vkVoidFunction(driver_EnumerateDeviceLayerProperties);
        if (string_eq(pName, "vkGetDeviceQueue")) return to_vkVoidFunction(driver_GetDeviceQueue);
        if (string_eq(pName, "vkQueueSubmit")) return to_vkVoidFunction(driver_QueueSubmit);
        if (string_eq(pName, "vkQueueWaitIdle")) return to_vkVoidFunction(driver_QueueWaitIdle);
        if (string_eq(pName, "vkDeviceWaitIdle")) return to_vkVoidFunction(driver_DeviceWaitIdle);
        if (string_eq(pName, "vkAllocateMemory")) return to_vkVoidFunction(driver_AllocateMemory);
        if (string_eq(pName, "vkFreeMemory")) return to_vkVoidFunction(driver_FreeMemory);
        if (string_eq(pName, "vkMapMemory")) return to_vkVoidFunction(driver_MapMemory);
        if (string_eq(pName, "vkUnmapMemory")) return to_vkVoidFunction(driver_UnmapMemory);
        if (string_eq(pName, "vkFlushMappedMemoryRanges")) return to_vkVoidFunction(driver_FlushMappedMemoryRanges);
        if (string_eq(pName, "vkInvalidateMappedMemoryRanges")) return to_vkVoidFunction(driver_InvalidateMappedMemoryRanges);
        if (string_eq(pName, "vkGetDeviceMemoryCommitment")) return to_vkVoidFunction(driver_GetDeviceMemoryCommitment);
        if (string_eq(pName, "vkBindBufferMemory")) return to_vkVoidFunction(driver_BindBufferMemory);
        if (string_eq(pName, "vkBindImageMemory")) return to_vkVoidFunction(driver_BindImageMemory);
        if (string_eq(pName, "vkGetBufferMemoryRequirements")) return to_vkVoidFunction(driver_GetBufferMemoryRequirements);
        if (string_eq(pName, "vkGetImageMemoryRequirements")) return to_vkVoidFunction(driver_GetImageMemoryRequirements);
        if (string_eq(pName, "vkGetImageSparseMemoryRequirements")) return to_vkVoidFunction(driver_GetImageSparseMemoryRequirements);
        if (string_eq(pName, "vkGetPhysicalDeviceSparseImageFormatProperties")) return to_vkVoidFunction(driver_GetPhysicalDeviceSparseImageFormatProperties);
        if (string_eq(pName, "vkQueueBindSparse")) return to_vkVoidFunction(driver_QueueBindSparse);
        if (string_eq(pName, "vkCreateFence")) return to_vkVoidFunction(driver_CreateFence);
        if (string_eq(pName, "vkDestroyFence")) return to_vkVoidFunction(driver_DestroyFence);
        if (string_eq(pName, "vkResetFences")) return to_vkVoidFunction(driver_ResetFences);
        if (string_eq(pName, "vkGetFenceStatus")) return to_vkVoidFunction(driver_GetFenceStatus);
        if (string_eq(pName, "vkWaitForFences")) return to_vkVoidFunction(driver_WaitForFences);
        if (string_eq(pName, "vkCreateSemaphore")) return to_vkVoidFunction(driver_CreateSemaphore);
        if (string_eq(pName, "vkDestroySemaphore")) return to_vkVoidFunction(driver_DestroySemaphore);
        if (string_eq(pName, "vkCreateEvent")) return to_vkVoidFunction(driver_CreateEvent);
        if (string_eq(pName, "vkDestroyEvent")) return to_vkVoidFunction(driver_DestroyEvent);
        if (string_eq(pName, "vkGetEventStatus")) return to_vkVoidFunction(driver_GetEventStatus);
        if (string_eq(pName, "vkSetEvent")) return to_vkVoidFunction(driver_SetEvent);
        if (string_eq(pName, "vkResetEvent")) return to_vkVoidFunction(driver_ResetEvent);
        if (string_eq(pName, "vkCreateQueryPool")) return to_vkVoidFunction(driver_CreateQueryPool);
        if (string_eq(pName, "vkDestroyQueryPool")) return to_vkVoidFunction(driver_DestroyQueryPool);
        if (string_eq(pName, "vkGetQueryPoolResults")) return to_vkVoidFunction(driver_GetQueryPoolResults);
        if (string_eq(pName, "vkCreateBuffer")) return to_vkVoidFunction(driver_CreateBuffer);
        if (string_eq(pName, "vkDestroyBuffer")) return to_vkVoidFunction(driver_DestroyBuffer);
        if (string_eq(pName, "vkCreateBufferView")) return to_vkVoidFunction(driver_CreateBufferView);
        if (string_eq(pName, "vkDestroyBufferView")) return to_vkVoidFunction(driver_DestroyBufferView);
        if (string_eq(pName, "vkCreateImage")) return to_vkVoidFunction(driver_CreateImage);
        if (string_eq(pName, "vkDestroyImage")) return to_vkVoidFunction(driver_DestroyImage);
        if (string_eq(pName, "vkGetImageSubresourceLayout")) return to_vkVoidFunction(driver_GetImageSubresourceLayout);
        if (string_eq(pName, "vkCreateImageView")) return to_vkVoidFunction(driver_CreateImageView);
        if (string_eq(pName, "vkDestroyImageView")) return to_vkVoidFunction(driver_DestroyImageView);
        if (string_eq(pName, "vkCreateShaderModule")) return to_vkVoidFunction(driver_CreateShaderModule);
        if (string_eq(pName, "vkDestroyShaderModule")) return to_vkVoidFunction(driver_DestroyShaderModule);
        if (string_eq(pName, "vkCreatePipelineCache")) return to_vkVoidFunction(driver_CreatePipelineCache);
        if (string_eq(pName, "vkDestroyPipelineCache")) return to_vkVoidFunction(driver_DestroyPipelineCache);
        if (string_eq(pName, "vkGetPipelineCacheData")) return to_vkVoidFunction(driver_GetPipelineCacheData);
        if (string_eq(pName, "vkMergePipelineCaches")) return to_vkVoidFunction(driver_MergePipelineCaches);
        if (string_eq(pName, "vkCreateGraphicsPipelines")) return to_vkVoidFunction(driver_CreateGraphicsPipelines);
        if (string_eq(pName, "vkCreateComputePipelines")) return to_vkVoidFunction(driver_CreateComputePipelines);
        if (string_eq(pName, "vkDestroyPipeline")) return to_vkVoidFunction(driver_DestroyPipeline);
        if (string_eq(pName, "vkCreatePipelineLayout")) return to_vkVoidFunction(driver_CreatePipelineLayout);
        if (string_eq(pName, "vkDestroyPipelineLayout")) return to_vkVoidFunction(driver_DestroyPipelineLayout);
        if (string_eq(pName, "vkCreateSampler")) return to_vkVoidFunction(driver_CreateSampler);
        if (string_eq(pName, "vkDestroySampler")) return to_vkVoidFunction(driver_DestroySampler);
        if (string_eq(pName, "vkCreateDescriptorSetLayout")) return to_vkVoidFunction(driver_CreateDescriptorSetLayout);
        if (string_eq(pName, "vkDestroyDescriptorSetLayout")) return to_vkVoidFunction(driver_DestroyDescriptorSetLayout);
        if (string_eq(pName, "vkCreateDescriptorPool")) return to_vkVoidFunction(driver_CreateDescriptorPool);
        if (string_eq(pName, "vkDestroyDescriptorPool")) return to_vkVoidFunction(driver_DestroyDescriptorPool);
        if (string_eq(pName, "vkResetDescriptorPool")) return to_vkVoidFunction(driver_ResetDescriptorPool);
        if (string_eq(pName, "vkAllocateDescriptorSets")) return to_vkVoidFunction(driver_AllocateDescriptorSets);
        if (string_eq(pName, "vkFreeDescriptorSets")) return to_vkVoidFunction(driver_FreeDescriptorSets);
        if (string_eq(pName, "vkUpdateDescriptorSets")) return to_vkVoidFunction(driver_UpdateDescriptorSets);
        if (string_eq(pName, "vkCreateFramebuffer")) return to_vkVoidFunction(driver_CreateFramebuffer);
        if (string_eq(pName, "vkDestroyFramebuffer")) return to_vkVoidFunction(driver_DestroyFramebuffer);
        if (string_eq(pName, "vkCreateRenderPass")) return to_vkVoidFunction(driver_CreateRenderPass);
        if (string_eq(pName, "vkDestroyRenderPass")) return to_vkVoidFunction(driver_DestroyRenderPass);
        if (string_eq(pName, "vkGetRenderAreaGranularity")) return to_vkVoidFunction(driver_GetRenderAreaGranularity);
        if (string_eq(pName, "vkCreateCommandPool")) return to_vkVoidFunction(driver_CreateCommandPool);
        if (string_eq(pName, "vkDestroyCommandPool")) return to_vkVoidFunction(driver_DestroyCommandPool);
        if (string_eq(pName, "vkResetCommandPool")) return to_vkVoidFunction(driver_ResetCommandPool);
        if (string_eq(pName, "vkAllocateCommandBuffers")) return to_vkVoidFunction(driver_AllocateCommandBuffers);
        if (string_eq(pName, "vkFreeCommandBuffers")) return to_vkVoidFunction(driver_FreeCommandBuffers);
        if (string_eq(pName, "vkBeginCommandBuffer")) return to_vkVoidFunction(driver_BeginCommandBuffer);
        if (string_eq(pName, "vkEndCommandBuffer")) return to_vkVoidFunction(driver_EndCommandBuffer);
        if (string_eq(pName, "vkResetCommandBuffer")) return to_vkVoidFunction(driver_ResetCommandBuffer);
        if (string_eq(pName, "vkCmdBindPipeline")) return to_vkVoidFunction(driver_CmdBindPipeline);
        if (string_eq(pName, "vkCmdSetViewport")) return to_vkVoidFunction(driver_CmdSetViewport);
        if (string_eq(pName, "vkCmdSetScissor")) return to_vkVoidFunction(driver_CmdSetScissor);
        if (string_eq(pName, "vkCmdSetLineWidth")) return to_vkVoidFunction(driver_CmdSetLineWidth);
        if (string_eq(pName, "vkCmdSetDepthBias")) return to_vkVoidFunction(driver_CmdSetDepthBias);
        if (string_eq(pName, "vkCmdSetBlendConstants")) return to_vkVoidFunction(driver_CmdSetBlendConstants);
        if (string_eq(pName, "vkCmdSetDepthBounds")) return to_vkVoidFunction(driver_CmdSetDepthBounds);
        if (string_eq(pName, "vkCmdSetStencilCompareMask")) return to_vkVoidFunction(driver_CmdSetStencilCompareMask);
        if (string_eq(pName, "vkCmdSetStencilWriteMask")) return to_vkVoidFunction(driver_CmdSetStencilWriteMask);
        if (string_eq(pName, "vkCmdSetStencilReference")) return to_vkVoidFunction(driver_CmdSetStencilReference);
        if (string_eq(pName, "vkCmdBindDescriptorSets")) return to_vkVoidFunction(driver_CmdBindDescriptorSets);
        if (string_eq(pName, "vkCmdBindIndexBuffer")) return to_vkVoidFunction(driver_CmdBindIndexBuffer);
        if (string_eq(pName, "vkCmdBindVertexBuffers")) return to_vkVoidFunction(driver_CmdBindVertexBuffers);
        if (string_eq(pName, "vkCmdDraw")) return to_vkVoidFunction(driver_CmdDraw);
        if (string_eq(pName, "vkCmdDrawIndexed")) return to_vkVoidFunction(driver_CmdDrawIndexed);
        if (string_eq(pName, "vkCmdDrawIndirect")) return to_vkVoidFunction(driver_CmdDrawIndirect);
        if (string_eq(pName, "vkCmdDrawIndexedIndirect")) return to_vkVoidFunction(driver_CmdDrawIndexedIndirect);
        if (string_eq(pName, "vkCmdDispatch")) return to_vkVoidFunction(driver_CmdDispatch);
        if (string_eq(pName, "vkCmdDispatchIndirect")) return to_vkVoidFunction(driver_CmdDispatchIndirect);
        if (string_eq(pName, "vkCmdCopyBuffer")) return to_vkVoidFunction(driver_CmdCopyBuffer);
        if (string_eq(pName, "vkCmdCopyImage")) return to_vkVoidFunction(driver_CmdCopyImage);
        if (string_eq(pName, "vkCmdBlitImage")) return to_vkVoidFunction(driver_CmdBlitImage);
        if (string_eq(pName, "vkCmdCopyBufferToImage")) return to_vkVoidFunction(driver_CmdCopyBufferToImage);
        if (string_eq(pName, "vkCmdCopyImageToBuffer")) return to_vkVoidFunction(driver_CmdCopyImageToBuffer);
        if (string_eq(pName, "vkCmdUpdateBuffer")) return to_vkVoidFunction(driver_CmdUpdateBuffer);
        if (string_eq(pName, "vkCmdFillBuffer")) return to_vkVoidFunction(driver_CmdFillBuffer);
        if (string_eq(pName, "vkCmdClearColorImage")) return to_vkVoidFunction(driver_CmdClearColorImage);
        if (string_eq(pName, "vkCmdClearDepthStencilImage")) return to_vkVoidFunction(driver_CmdClearDepthStencilImage);
        if (string_eq(pName, "vkCmdClearAttachments")) return to_vkVoidFunction(driver_CmdClearAttachments);
        if (string_eq(pName, "vkCmdResolveImage")) return to_vkVoidFunction(driver_CmdResolveImage);
        if (string_eq(pName, "vkCmdSetEvent")) return to_vkVoidFunction(driver_CmdSetEvent);
        if (string_eq(pName, "vkCmdResetEvent")) return to_vkVoidFunction(driver_CmdResetEvent);
        if (string_eq(pName, "vkCmdWaitEvents")) return to_vkVoidFunction(driver_CmdWaitEvents);
        if (string_eq(pName, "vkCmdPipelineBarrier")) return to_vkVoidFunction(driver_CmdPipelineBarrier);
        if (string_eq(pName, "vkCmdBeginQuery")) return to_vkVoidFunction(driver_CmdBeginQuery);
        if (string_eq(pName, "vkCmdEndQuery")) return to_vkVoidFunction(driver_CmdEndQuery);
        if (string_eq(pName, "vkCmdResetQueryPool")) return to_vkVoidFunction(driver_CmdResetQueryPool);
        if (string_eq(pName, "vkCmdWriteTimestamp")) return to_vkVoidFunction(driver_CmdWriteTimestamp);
        if (string_eq(pName, "vkCmdCopyQueryPoolResults")) return to_vkVoidFunction(driver_CmdCopyQueryPoolResults);
        if (string_eq(pName, "vkCmdPushConstants")) return to_vkVoidFunction(driver_CmdPushConstants);
        if (string_eq(pName, "vkCmdBeginRenderPass")) return to_vkVoidFunction(driver_CmdBeginRenderPass);
        if (string_eq(pName, "vkCmdNextSubpass")) return to_vkVoidFunction(driver_CmdNextSubpass);
        if (string_eq(pName, "vkCmdEndRenderPass")) return to_vkVoidFunction(driver_CmdEndRenderPass);
        if (string_eq(pName, "vkCmdExecuteCommands")) return to_vkVoidFunction(driver_CmdExecuteCommands);
    } // End Core 1.0

    // ----- VK_VERSION_1_1
    {
        if (string_eq(pName, "vkEnumerateInstanceVersion")) return to_vkVoidFunction(driver_EnumerateInstanceVersion);
        if (string_eq(pName, "vkBindBufferMemory2")) return to_vkVoidFunction(driver_BindBufferMemory2);
        if (string_eq(pName, "vkBindImageMemory2")) return to_vkVoidFunction(driver_BindImageMemory2);
        if (string_eq(pName, "vkGetDeviceGroupPeerMemoryFeatures")) return to_vkVoidFunction(driver_GetDeviceGroupPeerMemoryFeatures);
        if (string_eq(pName, "vkCmdSetDeviceMask")) return to_vkVoidFunction(driver_CmdSetDeviceMask);
        if (string_eq(pName, "vkCmdDispatchBase")) return to_vkVoidFunction(driver_CmdDispatchBase);
        if (string_eq(pName, "vkEnumeratePhysicalDeviceGroups")) return to_vkVoidFunction(driver_EnumeratePhysicalDeviceGroups);
        if (string_eq(pName, "vkGetImageMemoryRequirements2")) return to_vkVoidFunction(driver_GetImageMemoryRequirements2);
        if (string_eq(pName, "vkGetBufferMemoryRequirements2")) return to_vkVoidFunction(driver_GetBufferMemoryRequirements2);
        if (string_eq(pName, "vkGetImageSparseMemoryRequirements2")) return to_vkVoidFunction(driver_GetImageSparseMemoryRequirements2);
        if (string_eq(pName, "vkGetPhysicalDeviceFeatures2")) return to_vkVoidFunction(driver_GetPhysicalDeviceFeatures2);
        if (string_eq(pName, "vkGetPhysicalDeviceProperties2")) return to_vkVoidFunction(driver_GetPhysicalDeviceProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceFormatProperties2")) return to_vkVoidFunction(driver_GetPhysicalDeviceFormatProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceImageFormatProperties2")) return to_vkVoidFunction(driver_GetPhysicalDeviceImageFormatProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyProperties2")) return to_vkVoidFunction(driver_GetPhysicalDeviceQueueFamilyProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceMemoryProperties2")) return to_vkVoidFunction(driver_GetPhysicalDeviceMemoryProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceSparseImageFormatProperties2")) return to_vkVoidFunction(driver_GetPhysicalDeviceSparseImageFormatProperties2);
        if (string_eq(pName, "vkTrimCommandPool")) return to_vkVoidFunction(driver_TrimCommandPool);
        if (string_eq(pName, "vkGetDeviceQueue2")) return to_vkVoidFunction(driver_GetDeviceQueue2);
        if (string_eq(pName, "vkCreateSamplerYcbcrConversion")) return to_vkVoidFunction(driver_CreateSamplerYcbcrConversion);
        if (string_eq(pName, "vkDestroySamplerYcbcrConversion")) return to_vkVoidFunction(driver_DestroySamplerYcbcrConversion);
        if (string_eq(pName, "vkCreateDescriptorUpdateTemplate")) return to_vkVoidFunction(driver_CreateDescriptorUpdateTemplate);
        if (string_eq(pName, "vkDestroyDescriptorUpdateTemplate")) return to_vkVoidFunction(driver_DestroyDescriptorUpdateTemplate);
        if (string_eq(pName, "vkUpdateDescriptorSetWithTemplate")) return to_vkVoidFunction(driver_UpdateDescriptorSetWithTemplate);
        if (string_eq(pName, "vkGetPhysicalDeviceExternalBufferProperties")) return to_vkVoidFunction(driver_GetPhysicalDeviceExternalBufferProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceExternalFenceProperties")) return to_vkVoidFunction(driver_GetPhysicalDeviceExternalFenceProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceExternalSemaphoreProperties")) return to_vkVoidFunction(driver_GetPhysicalDeviceExternalSemaphoreProperties);
        if (string_eq(pName, "vkGetDescriptorSetLayoutSupport")) return to_vkVoidFunction(driver_GetDescriptorSetLayoutSupport);
    } // End Core 1.1

    // ----- VK_VERSION_1_2
    {
        if (string_eq(pName, "vkCmdDrawIndirectCount")) return to_vkVoidFunction(driver_CmdDrawIndirectCount);
        if (string_eq(pName, "vkCmdDrawIndexedIndirectCount")) return to_vkVoidFunction(driver_CmdDrawIndexedIndirectCount);
        if (string_eq(pName, "vkCreateRenderPass2")) return to_vkVoidFunction(driver_CreateRenderPass2);
        if (string_eq(pName, "vkCmdBeginRenderPass2")) return to_vkVoidFunction(driver_CmdBeginRenderPass2);
        if (string_eq(pName, "vkCmdNextSubpass2")) return to_vkVoidFunction(driver_CmdNextSubpass2);
        if (string_eq(pName, "vkCmdEndRenderPass2")) return to_vkVoidFunction(driver_CmdEndRenderPass2);
        if (string_eq(pName, "vkResetQueryPool")) return to_vkVoidFunction(driver_ResetQueryPool);
        if (string_eq(pName, "vkGetSemaphoreCounterValue")) return to_vkVoidFunction(driver_GetSemaphoreCounterValue);
        if (string_eq(pName, "vkWaitSemaphores")) return to_vkVoidFunction(driver_WaitSemaphores);
        if (string_eq(pName, "vkSignalSemaphore")) return to_vkVoidFunction(driver_SignalSemaphore);
        if (string_eq(pName, "vkGetBufferDeviceAddress")) return to_vkVoidFunction(driver_GetBufferDeviceAddress);
        if (string_eq(pName, "vkGetBufferOpaqueCaptureAddress")) return to_vkVoidFunction(driver_GetBufferOpaqueCaptureAddress);
        if (string_eq(pName, "vkGetDeviceMemoryOpaqueCaptureAddress")) return to_vkVoidFunction(driver_GetDeviceMemoryOpaqueCaptureAddress);
    } // End Core 1.2

    // ----- VK_VERSION_1_3
    {
        if (string_eq(pName, "vkGetPhysicalDeviceToolProperties")) return to_vkVoidFunction(driver_GetPhysicalDeviceToolProperties);
        if (string_eq(pName, "vkCreatePrivateDataSlot")) return to_vkVoidFunction(driver_CreatePrivateDataSlot);
        if (string_eq(pName, "vkDestroyPrivateDataSlot")) return to_vkVoidFunction(driver_DestroyPrivateDataSlot);
        if (string_eq(pName, "vkSetPrivateData")) return to_vkVoidFunction(driver_SetPrivateData);
        if (string_eq(pName, "vkGetPrivateData")) return to_vkVoidFunction(driver_GetPrivateData);
        if (string_eq(pName, "vkCmdSetEvent2")) return to_vkVoidFunction(driver_CmdSetEvent2);
        if (string_eq(pName, "vkCmdResetEvent2")) return to_vkVoidFunction(driver_CmdResetEvent2);
        if (string_eq(pName, "vkCmdWaitEvents2")) return to_vkVoidFunction(driver_CmdWaitEvents2);
        if (string_eq(pName, "vkCmdPipelineBarrier2")) return to_vkVoidFunction(driver_CmdPipelineBarrier2);
        if (string_eq(pName, "vkCmdWriteTimestamp2")) return to_vkVoidFunction(driver_CmdWriteTimestamp2);
        if (string_eq(pName, "vkQueueSubmit2")) return to_vkVoidFunction(driver_QueueSubmit2);
        if (string_eq(pName, "vkCmdCopyBuffer2")) return to_vkVoidFunction(driver_CmdCopyBuffer2);
        if (string_eq(pName, "vkCmdCopyImage2")) return to_vkVoidFunction(driver_CmdCopyImage2);
        if (string_eq(pName, "vkCmdCopyBufferToImage2")) return to_vkVoidFunction(driver_CmdCopyBufferToImage2);
        if (string_eq(pName, "vkCmdCopyImageToBuffer2")) return to_vkVoidFunction(driver_CmdCopyImageToBuffer2);
        if (string_eq(pName, "vkCmdBlitImage2")) return to_vkVoidFunction(driver_CmdBlitImage2);
        if (string_eq(pName, "vkCmdResolveImage2")) return to_vkVoidFunction(driver_CmdResolveImage2);
        if (string_eq(pName, "vkCmdBeginRendering")) return to_vkVoidFunction(driver_CmdBeginRendering);
        if (string_eq(pName, "vkCmdEndRendering")) return to_vkVoidFunction(driver_CmdEndRendering);
        if (string_eq(pName, "vkCmdSetCullMode")) return to_vkVoidFunction(driver_CmdSetCullMode);
        if (string_eq(pName, "vkCmdSetFrontFace")) return to_vkVoidFunction(driver_CmdSetFrontFace);
        if (string_eq(pName, "vkCmdSetPrimitiveTopology")) return to_vkVoidFunction(driver_CmdSetPrimitiveTopology);
        if (string_eq(pName, "vkCmdSetViewportWithCount")) return to_vkVoidFunction(driver_CmdSetViewportWithCount);
        if (string_eq(pName, "vkCmdSetScissorWithCount")) return to_vkVoidFunction(driver_CmdSetScissorWithCount);
        if (string_eq(pName, "vkCmdBindVertexBuffers2")) return to_vkVoidFunction(driver_CmdBindVertexBuffers2);
        if (string_eq(pName, "vkCmdSetDepthTestEnable")) return to_vkVoidFunction(driver_CmdSetDepthTestEnable);
        if (string_eq(pName, "vkCmdSetDepthWriteEnable")) return to_vkVoidFunction(driver_CmdSetDepthWriteEnable);
        if (string_eq(pName, "vkCmdSetDepthCompareOp")) return to_vkVoidFunction(driver_CmdSetDepthCompareOp);
        if (string_eq(pName, "vkCmdSetDepthBoundsTestEnable")) return to_vkVoidFunction(driver_CmdSetDepthBoundsTestEnable);
        if (string_eq(pName, "vkCmdSetStencilTestEnable")) return to_vkVoidFunction(driver_CmdSetStencilTestEnable);
        if (string_eq(pName, "vkCmdSetStencilOp")) return to_vkVoidFunction(driver_CmdSetStencilOp);
        if (string_eq(pName, "vkCmdSetRasterizerDiscardEnable")) return to_vkVoidFunction(driver_CmdSetRasterizerDiscardEnable);
        if (string_eq(pName, "vkCmdSetDepthBiasEnable")) return to_vkVoidFunction(driver_CmdSetDepthBiasEnable);
        if (string_eq(pName, "vkCmdSetPrimitiveRestartEnable")) return to_vkVoidFunction(driver_CmdSetPrimitiveRestartEnable);
        if (string_eq(pName, "vkGetDeviceBufferMemoryRequirements")) return to_vkVoidFunction(driver_GetDeviceBufferMemoryRequirements);
        if (string_eq(pName, "vkGetDeviceImageMemoryRequirements")) return to_vkVoidFunction(driver_GetDeviceImageMemoryRequirements);
        if (string_eq(pName, "vkGetDeviceImageSparseMemoryRequirements")) return to_vkVoidFunction(driver_GetDeviceImageSparseMemoryRequirements);
    } // End Core 1.3

    // ----- VK_KHR_surface
    if (string_eq(pName, "vkDestroySurfaceKHR")) return to_vkVoidFunction(driver_DestroySurfaceKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceSurfaceSupportKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceSurfaceSupportKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceSurfaceCapabilitiesKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceSurfaceFormatsKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceSurfaceFormatsKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceSurfacePresentModesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceSurfacePresentModesKHR);

    // ----- VK_KHR_swapchain
    if (IsDeviceExtensionEnabled("VK_KHR_swapchain")) {
        if (string_eq(pName, "vkCreateSwapchainKHR")) return to_vkVoidFunction(driver_CreateSwapchainKHR);
        if (string_eq(pName, "vkDestroySwapchainKHR")) return to_vkVoidFunction(driver_DestroySwapchainKHR);
        if (string_eq(pName, "vkGetSwapchainImagesKHR")) return to_vkVoidFunction(driver_GetSwapchainImagesKHR);
        if (string_eq(pName, "vkAcquireNextImageKHR")) return to_vkVoidFunction(driver_AcquireNextImageKHR);
        if (string_eq(pName, "vkQueuePresentKHR")) return to_vkVoidFunction(driver_QueuePresentKHR);
        if (string_eq(pName, "vkGetDeviceGroupPresentCapabilitiesKHR")) return to_vkVoidFunction(driver_GetDeviceGroupPresentCapabilitiesKHR);
        if (string_eq(pName, "vkGetDeviceGroupSurfacePresentModesKHR")) return to_vkVoidFunction(driver_GetDeviceGroupSurfacePresentModesKHR);
        if (string_eq(pName, "vkGetPhysicalDevicePresentRectanglesKHR")) return to_vkVoidFunction(driver_GetPhysicalDevicePresentRectanglesKHR);
        if (string_eq(pName, "vkAcquireNextImage2KHR")) return to_vkVoidFunction(driver_AcquireNextImage2KHR);
    } // VK_KHR_swapchain

    // ----- VK_KHR_display
    if (string_eq(pName, "vkGetPhysicalDeviceDisplayPropertiesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceDisplayPropertiesKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceDisplayPlanePropertiesKHR);
    if (string_eq(pName, "vkGetDisplayPlaneSupportedDisplaysKHR")) return to_vkVoidFunction(driver_GetDisplayPlaneSupportedDisplaysKHR);
    if (string_eq(pName, "vkGetDisplayModePropertiesKHR")) return to_vkVoidFunction(driver_GetDisplayModePropertiesKHR);
    if (string_eq(pName, "vkCreateDisplayModeKHR")) return to_vkVoidFunction(driver_CreateDisplayModeKHR);
    if (string_eq(pName, "vkGetDisplayPlaneCapabilitiesKHR")) return to_vkVoidFunction(driver_GetDisplayPlaneCapabilitiesKHR);
    if (string_eq(pName, "vkCreateDisplayPlaneSurfaceKHR")) return to_vkVoidFunction(driver_CreateDisplayPlaneSurfaceKHR);

    // ----- VK_KHR_display_swapchain
    if (IsDeviceExtensionEnabled("VK_KHR_display_swapchain")) {
        if (string_eq(pName, "vkCreateSharedSwapchainsKHR")) return to_vkVoidFunction(driver_CreateSharedSwapchainsKHR);
    } // VK_KHR_display_swapchain

    // ----- VK_KHR_xlib_surface
#ifdef VK_USE_PLATFORM_XLIB_KHR
    if (string_eq(pName, "vkCreateXlibSurfaceKHR")) return to_vkVoidFunction(driver_CreateXlibSurfaceKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceXlibPresentationSupportKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceXlibPresentationSupportKHR);
#endif // VK_USE_PLATFORM_XLIB_KHR

    // ----- VK_KHR_xcb_surface
#ifdef VK_USE_PLATFORM_XCB_KHR
    if (string_eq(pName, "vkCreateXcbSurfaceKHR")) return to_vkVoidFunction(driver_CreateXcbSurfaceKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceXcbPresentationSupportKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceXcbPresentationSupportKHR);
#endif // VK_USE_PLATFORM_XCB_KHR

    // ----- VK_KHR_wayland_surface
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    if (string_eq(pName, "vkCreateWaylandSurfaceKHR")) return to_vkVoidFunction(driver_CreateWaylandSurfaceKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceWaylandPresentationSupportKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceWaylandPresentationSupportKHR);
#endif // VK_USE_PLATFORM_WAYLAND_KHR

    // ----- VK_KHR_win32_surface
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (string_eq(pName, "vkCreateWin32SurfaceKHR")) return to_vkVoidFunction(driver_CreateWin32SurfaceKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceWin32PresentationSupportKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceWin32PresentationSupportKHR);
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_KHR_video_queue
#ifdef VK_ENABLE_BETA_EXTENSIONS
    if (IsDeviceExtensionEnabled("VK_KHR_video_queue")) {
        if (string_eq(pName, "vkGetPhysicalDeviceVideoCapabilitiesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceVideoCapabilitiesKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceVideoFormatPropertiesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceVideoFormatPropertiesKHR);
        if (string_eq(pName, "vkCreateVideoSessionKHR")) return to_vkVoidFunction(driver_CreateVideoSessionKHR);
        if (string_eq(pName, "vkDestroyVideoSessionKHR")) return to_vkVoidFunction(driver_DestroyVideoSessionKHR);
        if (string_eq(pName, "vkGetVideoSessionMemoryRequirementsKHR")) return to_vkVoidFunction(driver_GetVideoSessionMemoryRequirementsKHR);
        if (string_eq(pName, "vkBindVideoSessionMemoryKHR")) return to_vkVoidFunction(driver_BindVideoSessionMemoryKHR);
        if (string_eq(pName, "vkCreateVideoSessionParametersKHR")) return to_vkVoidFunction(driver_CreateVideoSessionParametersKHR);
        if (string_eq(pName, "vkUpdateVideoSessionParametersKHR")) return to_vkVoidFunction(driver_UpdateVideoSessionParametersKHR);
        if (string_eq(pName, "vkDestroyVideoSessionParametersKHR")) return to_vkVoidFunction(driver_DestroyVideoSessionParametersKHR);
        if (string_eq(pName, "vkCmdBeginVideoCodingKHR")) return to_vkVoidFunction(driver_CmdBeginVideoCodingKHR);
        if (string_eq(pName, "vkCmdEndVideoCodingKHR")) return to_vkVoidFunction(driver_CmdEndVideoCodingKHR);
        if (string_eq(pName, "vkCmdControlVideoCodingKHR")) return to_vkVoidFunction(driver_CmdControlVideoCodingKHR);
    } // VK_KHR_video_queue
#endif // VK_ENABLE_BETA_EXTENSIONS

    // ----- VK_KHR_video_decode_queue
#ifdef VK_ENABLE_BETA_EXTENSIONS
    if (IsDeviceExtensionEnabled("VK_KHR_video_decode_queue")) {
        if (string_eq(pName, "vkCmdDecodeVideoKHR")) return to_vkVoidFunction(driver_CmdDecodeVideoKHR);
    } // VK_KHR_video_decode_queue
#endif // VK_ENABLE_BETA_EXTENSIONS

    // ----- VK_KHR_dynamic_rendering
    if (IsDeviceExtensionEnabled("VK_KHR_dynamic_rendering")) {
        if (string_eq(pName, "vkCmdBeginRenderingKHR")) return to_vkVoidFunction(driver_CmdBeginRenderingKHR);
        if (string_eq(pName, "vkCmdEndRenderingKHR")) return to_vkVoidFunction(driver_CmdEndRenderingKHR);
    } // VK_KHR_dynamic_rendering

    // ----- VK_KHR_get_physical_device_properties2
    if (string_eq(pName, "vkGetPhysicalDeviceFeatures2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceFeatures2KHR);
    if (string_eq(pName, "vkGetPhysicalDeviceProperties2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceProperties2KHR);
    if (string_eq(pName, "vkGetPhysicalDeviceFormatProperties2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceFormatProperties2KHR);
    if (string_eq(pName, "vkGetPhysicalDeviceImageFormatProperties2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceImageFormatProperties2KHR);
    if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyProperties2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceQueueFamilyProperties2KHR);
    if (string_eq(pName, "vkGetPhysicalDeviceMemoryProperties2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceMemoryProperties2KHR);
    if (string_eq(pName, "vkGetPhysicalDeviceSparseImageFormatProperties2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceSparseImageFormatProperties2KHR);

    // ----- VK_KHR_device_group
    if (IsDeviceExtensionEnabled("VK_KHR_device_group")) {
        if (string_eq(pName, "vkGetDeviceGroupPeerMemoryFeaturesKHR")) return to_vkVoidFunction(driver_GetDeviceGroupPeerMemoryFeaturesKHR);
        if (string_eq(pName, "vkCmdSetDeviceMaskKHR")) return to_vkVoidFunction(driver_CmdSetDeviceMaskKHR);
        if (string_eq(pName, "vkCmdDispatchBaseKHR")) return to_vkVoidFunction(driver_CmdDispatchBaseKHR);
    } // VK_KHR_device_group

    // ----- VK_KHR_maintenance1
    if (IsDeviceExtensionEnabled("VK_KHR_maintenance1")) {
        if (string_eq(pName, "vkTrimCommandPoolKHR")) return to_vkVoidFunction(driver_TrimCommandPoolKHR);
    } // VK_KHR_maintenance1

    // ----- VK_KHR_device_group_creation
    if (string_eq(pName, "vkEnumeratePhysicalDeviceGroupsKHR")) return to_vkVoidFunction(driver_EnumeratePhysicalDeviceGroupsKHR);

    // ----- VK_KHR_external_memory_capabilities
    if (string_eq(pName, "vkGetPhysicalDeviceExternalBufferPropertiesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceExternalBufferPropertiesKHR);

    // ----- VK_KHR_external_memory_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (IsDeviceExtensionEnabled("VK_KHR_external_memory_win32")) {
        if (string_eq(pName, "vkGetMemoryWin32HandleKHR")) return to_vkVoidFunction(driver_GetMemoryWin32HandleKHR);
        if (string_eq(pName, "vkGetMemoryWin32HandlePropertiesKHR")) return to_vkVoidFunction(driver_GetMemoryWin32HandlePropertiesKHR);
    } // VK_KHR_external_memory_win32
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_KHR_external_memory_fd
    if (IsDeviceExtensionEnabled("VK_KHR_external_memory_fd")) {
        if (string_eq(pName, "vkGetMemoryFdKHR")) return to_vkVoidFunction(driver_GetMemoryFdKHR);
        if (string_eq(pName, "vkGetMemoryFdPropertiesKHR")) return to_vkVoidFunction(driver_GetMemoryFdPropertiesKHR);
    } // VK_KHR_external_memory_fd

    // ----- VK_KHR_external_semaphore_capabilities
    if (string_eq(pName, "vkGetPhysicalDeviceExternalSemaphorePropertiesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceExternalSemaphorePropertiesKHR);

    // ----- VK_KHR_external_semaphore_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (IsDeviceExtensionEnabled("VK_KHR_external_semaphore_win32")) {
        if (string_eq(pName, "vkImportSemaphoreWin32HandleKHR")) return to_vkVoidFunction(driver_ImportSemaphoreWin32HandleKHR);
        if (string_eq(pName, "vkGetSemaphoreWin32HandleKHR")) return to_vkVoidFunction(driver_GetSemaphoreWin32HandleKHR);
    } // VK_KHR_external_semaphore_win32
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_KHR_external_semaphore_fd
    if (IsDeviceExtensionEnabled("VK_KHR_external_semaphore_fd")) {
        if (string_eq(pName, "vkImportSemaphoreFdKHR")) return to_vkVoidFunction(driver_ImportSemaphoreFdKHR);
        if (string_eq(pName, "vkGetSemaphoreFdKHR")) return to_vkVoidFunction(driver_GetSemaphoreFdKHR);
    } // VK_KHR_external_semaphore_fd

    // ----- VK_KHR_push_descriptor
    if (IsDeviceExtensionEnabled("VK_KHR_push_descriptor")) {
        if (string_eq(pName, "vkCmdPushDescriptorSetKHR")) return to_vkVoidFunction(driver_CmdPushDescriptorSetKHR);
        if (string_eq(pName, "vkCmdPushDescriptorSetWithTemplateKHR")) return to_vkVoidFunction(driver_CmdPushDescriptorSetWithTemplateKHR);
    } // VK_KHR_push_descriptor

    // ----- VK_KHR_descriptor_update_template
    if (IsDeviceExtensionEnabled("VK_KHR_descriptor_update_template")) {
        if (string_eq(pName, "vkCreateDescriptorUpdateTemplateKHR")) return to_vkVoidFunction(driver_CreateDescriptorUpdateTemplateKHR);
        if (string_eq(pName, "vkDestroyDescriptorUpdateTemplateKHR")) return to_vkVoidFunction(driver_DestroyDescriptorUpdateTemplateKHR);
        if (string_eq(pName, "vkUpdateDescriptorSetWithTemplateKHR")) return to_vkVoidFunction(driver_UpdateDescriptorSetWithTemplateKHR);
    } // VK_KHR_descriptor_update_template

    // ----- VK_KHR_create_renderpass2
    if (IsDeviceExtensionEnabled("VK_KHR_create_renderpass2")) {
        if (string_eq(pName, "vkCreateRenderPass2KHR")) return to_vkVoidFunction(driver_CreateRenderPass2KHR);
        if (string_eq(pName, "vkCmdBeginRenderPass2KHR")) return to_vkVoidFunction(driver_CmdBeginRenderPass2KHR);
        if (string_eq(pName, "vkCmdNextSubpass2KHR")) return to_vkVoidFunction(driver_CmdNextSubpass2KHR);
        if (string_eq(pName, "vkCmdEndRenderPass2KHR")) return to_vkVoidFunction(driver_CmdEndRenderPass2KHR);
    } // VK_KHR_create_renderpass2

    // ----- VK_KHR_shared_presentable_image
    if (IsDeviceExtensionEnabled("VK_KHR_shared_presentable_image")) {
        if (string_eq(pName, "vkGetSwapchainStatusKHR")) return to_vkVoidFunction(driver_GetSwapchainStatusKHR);
    } // VK_KHR_shared_presentable_image

    // ----- VK_KHR_external_fence_capabilities
    if (string_eq(pName, "vkGetPhysicalDeviceExternalFencePropertiesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceExternalFencePropertiesKHR);

    // ----- VK_KHR_external_fence_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (IsDeviceExtensionEnabled("VK_KHR_external_fence_win32")) {
        if (string_eq(pName, "vkImportFenceWin32HandleKHR")) return to_vkVoidFunction(driver_ImportFenceWin32HandleKHR);
        if (string_eq(pName, "vkGetFenceWin32HandleKHR")) return to_vkVoidFunction(driver_GetFenceWin32HandleKHR);
    } // VK_KHR_external_fence_win32
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_KHR_external_fence_fd
    if (IsDeviceExtensionEnabled("VK_KHR_external_fence_fd")) {
        if (string_eq(pName, "vkImportFenceFdKHR")) return to_vkVoidFunction(driver_ImportFenceFdKHR);
        if (string_eq(pName, "vkGetFenceFdKHR")) return to_vkVoidFunction(driver_GetFenceFdKHR);
    } // VK_KHR_external_fence_fd

    // ----- VK_KHR_performance_query
    if (IsDeviceExtensionEnabled("VK_KHR_performance_query")) {
        if (string_eq(pName, "vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR")) return to_vkVoidFunction(driver_EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR);
        if (string_eq(pName, "vkAcquireProfilingLockKHR")) return to_vkVoidFunction(driver_AcquireProfilingLockKHR);
        if (string_eq(pName, "vkReleaseProfilingLockKHR")) return to_vkVoidFunction(driver_ReleaseProfilingLockKHR);
    } // VK_KHR_performance_query

    // ----- VK_KHR_get_surface_capabilities2
    if (string_eq(pName, "vkGetPhysicalDeviceSurfaceCapabilities2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceSurfaceCapabilities2KHR);
    if (string_eq(pName, "vkGetPhysicalDeviceSurfaceFormats2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceSurfaceFormats2KHR);

    // ----- VK_KHR_get_display_properties2
    if (string_eq(pName, "vkGetPhysicalDeviceDisplayProperties2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceDisplayProperties2KHR);
    if (string_eq(pName, "vkGetPhysicalDeviceDisplayPlaneProperties2KHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceDisplayPlaneProperties2KHR);
    if (string_eq(pName, "vkGetDisplayModeProperties2KHR")) return to_vkVoidFunction(driver_GetDisplayModeProperties2KHR);
    if (string_eq(pName, "vkGetDisplayPlaneCapabilities2KHR")) return to_vkVoidFunction(driver_GetDisplayPlaneCapabilities2KHR);

    // ----- VK_KHR_get_memory_requirements2
    if (IsDeviceExtensionEnabled("VK_KHR_get_memory_requirements2")) {
        if (string_eq(pName, "vkGetImageMemoryRequirements2KHR")) return to_vkVoidFunction(driver_GetImageMemoryRequirements2KHR);
        if (string_eq(pName, "vkGetBufferMemoryRequirements2KHR")) return to_vkVoidFunction(driver_GetBufferMemoryRequirements2KHR);
        if (string_eq(pName, "vkGetImageSparseMemoryRequirements2KHR")) return to_vkVoidFunction(driver_GetImageSparseMemoryRequirements2KHR);
    } // VK_KHR_get_memory_requirements2

    // ----- VK_KHR_sampler_ycbcr_conversion
    if (IsDeviceExtensionEnabled("VK_KHR_sampler_ycbcr_conversion")) {
        if (string_eq(pName, "vkCreateSamplerYcbcrConversionKHR")) return to_vkVoidFunction(driver_CreateSamplerYcbcrConversionKHR);
        if (string_eq(pName, "vkDestroySamplerYcbcrConversionKHR")) return to_vkVoidFunction(driver_DestroySamplerYcbcrConversionKHR);
    } // VK_KHR_sampler_ycbcr_conversion

    // ----- VK_KHR_bind_memory2
    if (IsDeviceExtensionEnabled("VK_KHR_bind_memory2")) {
        if (string_eq(pName, "vkBindBufferMemory2KHR")) return to_vkVoidFunction(driver_BindBufferMemory2KHR);
        if (string_eq(pName, "vkBindImageMemory2KHR")) return to_vkVoidFunction(driver_BindImageMemory2KHR);
    } // VK_KHR_bind_memory2

    // ----- VK_KHR_maintenance3
    if (IsDeviceExtensionEnabled("VK_KHR_maintenance3")) {
        if (string_eq(pName, "vkGetDescriptorSetLayoutSupportKHR")) return to_vkVoidFunction(driver_GetDescriptorSetLayoutSupportKHR);
    } // VK_KHR_maintenance3

    // ----- VK_KHR_draw_indirect_count
    if (IsDeviceExtensionEnabled("VK_KHR_draw_indirect_count")) {
        if (string_eq(pName, "vkCmdDrawIndirectCountKHR")) return to_vkVoidFunction(driver_CmdDrawIndirectCountKHR);
        if (string_eq(pName, "vkCmdDrawIndexedIndirectCountKHR")) return to_vkVoidFunction(driver_CmdDrawIndexedIndirectCountKHR);
    } // VK_KHR_draw_indirect_count

    // ----- VK_KHR_timeline_semaphore
    if (IsDeviceExtensionEnabled("VK_KHR_timeline_semaphore")) {
        if (string_eq(pName, "vkGetSemaphoreCounterValueKHR")) return to_vkVoidFunction(driver_GetSemaphoreCounterValueKHR);
        if (string_eq(pName, "vkWaitSemaphoresKHR")) return to_vkVoidFunction(driver_WaitSemaphoresKHR);
        if (string_eq(pName, "vkSignalSemaphoreKHR")) return to_vkVoidFunction(driver_SignalSemaphoreKHR);
    } // VK_KHR_timeline_semaphore

    // ----- VK_KHR_fragment_shading_rate
    if (IsDeviceExtensionEnabled("VK_KHR_fragment_shading_rate")) {
        if (string_eq(pName, "vkGetPhysicalDeviceFragmentShadingRatesKHR")) return to_vkVoidFunction(driver_GetPhysicalDeviceFragmentShadingRatesKHR);
        if (string_eq(pName, "vkCmdSetFragmentShadingRateKHR")) return to_vkVoidFunction(driver_CmdSetFragmentShadingRateKHR);
    } // VK_KHR_fragment_shading_rate

    // ----- VK_KHR_present_wait
    if (IsDeviceExtensionEnabled("VK_KHR_present_wait")) {
        if (string_eq(pName, "vkWaitForPresentKHR")) return to_vkVoidFunction(driver_WaitForPresentKHR);
    } // VK_KHR_present_wait

    // ----- VK_KHR_buffer_device_address
    if (IsDeviceExtensionEnabled("VK_KHR_buffer_device_address")) {
        if (string_eq(pName, "vkGetBufferDeviceAddressKHR")) return to_vkVoidFunction(driver_GetBufferDeviceAddressKHR);
        if (string_eq(pName, "vkGetBufferOpaqueCaptureAddressKHR")) return to_vkVoidFunction(driver_GetBufferOpaqueCaptureAddressKHR);
        if (string_eq(pName, "vkGetDeviceMemoryOpaqueCaptureAddressKHR")) return to_vkVoidFunction(driver_GetDeviceMemoryOpaqueCaptureAddressKHR);
    } // VK_KHR_buffer_device_address

    // ----- VK_KHR_deferred_host_operations
    if (IsDeviceExtensionEnabled("VK_KHR_deferred_host_operations")) {
        if (string_eq(pName, "vkCreateDeferredOperationKHR")) return to_vkVoidFunction(driver_CreateDeferredOperationKHR);
        if (string_eq(pName, "vkDestroyDeferredOperationKHR")) return to_vkVoidFunction(driver_DestroyDeferredOperationKHR);
        if (string_eq(pName, "vkGetDeferredOperationMaxConcurrencyKHR")) return to_vkVoidFunction(driver_GetDeferredOperationMaxConcurrencyKHR);
        if (string_eq(pName, "vkGetDeferredOperationResultKHR")) return to_vkVoidFunction(driver_GetDeferredOperationResultKHR);
        if (string_eq(pName, "vkDeferredOperationJoinKHR")) return to_vkVoidFunction(driver_DeferredOperationJoinKHR);
    } // VK_KHR_deferred_host_operations

    // ----- VK_KHR_pipeline_executable_properties
    if (IsDeviceExtensionEnabled("VK_KHR_pipeline_executable_properties")) {
        if (string_eq(pName, "vkGetPipelineExecutablePropertiesKHR")) return to_vkVoidFunction(driver_GetPipelineExecutablePropertiesKHR);
        if (string_eq(pName, "vkGetPipelineExecutableStatisticsKHR")) return to_vkVoidFunction(driver_GetPipelineExecutableStatisticsKHR);
        if (string_eq(pName, "vkGetPipelineExecutableInternalRepresentationsKHR")) return to_vkVoidFunction(driver_GetPipelineExecutableInternalRepresentationsKHR);
    } // VK_KHR_pipeline_executable_properties

    // ----- VK_KHR_video_encode_queue
#ifdef VK_ENABLE_BETA_EXTENSIONS
    if (IsDeviceExtensionEnabled("VK_KHR_video_encode_queue")) {
        if (string_eq(pName, "vkCmdEncodeVideoKHR")) return to_vkVoidFunction(driver_CmdEncodeVideoKHR);
    } // VK_KHR_video_encode_queue
#endif // VK_ENABLE_BETA_EXTENSIONS

    // ----- VK_KHR_synchronization2
    if (IsDeviceExtensionEnabled("VK_KHR_synchronization2")) {
        if (string_eq(pName, "vkCmdSetEvent2KHR")) return to_vkVoidFunction(driver_CmdSetEvent2KHR);
        if (string_eq(pName, "vkCmdResetEvent2KHR")) return to_vkVoidFunction(driver_CmdResetEvent2KHR);
        if (string_eq(pName, "vkCmdWaitEvents2KHR")) return to_vkVoidFunction(driver_CmdWaitEvents2KHR);
        if (string_eq(pName, "vkCmdPipelineBarrier2KHR")) return to_vkVoidFunction(driver_CmdPipelineBarrier2KHR);
        if (string_eq(pName, "vkCmdWriteTimestamp2KHR")) return to_vkVoidFunction(driver_CmdWriteTimestamp2KHR);
        if (string_eq(pName, "vkQueueSubmit2KHR")) return to_vkVoidFunction(driver_QueueSubmit2KHR);
        if (string_eq(pName, "vkCmdWriteBufferMarker2AMD")) return to_vkVoidFunction(driver_CmdWriteBufferMarker2AMD);
        if (string_eq(pName, "vkGetQueueCheckpointData2NV")) return to_vkVoidFunction(driver_GetQueueCheckpointData2NV);
    } // VK_KHR_synchronization2

    // ----- VK_KHR_copy_commands2
    if (IsDeviceExtensionEnabled("VK_KHR_copy_commands2")) {
        if (string_eq(pName, "vkCmdCopyBuffer2KHR")) return to_vkVoidFunction(driver_CmdCopyBuffer2KHR);
        if (string_eq(pName, "vkCmdCopyImage2KHR")) return to_vkVoidFunction(driver_CmdCopyImage2KHR);
        if (string_eq(pName, "vkCmdCopyBufferToImage2KHR")) return to_vkVoidFunction(driver_CmdCopyBufferToImage2KHR);
        if (string_eq(pName, "vkCmdCopyImageToBuffer2KHR")) return to_vkVoidFunction(driver_CmdCopyImageToBuffer2KHR);
        if (string_eq(pName, "vkCmdBlitImage2KHR")) return to_vkVoidFunction(driver_CmdBlitImage2KHR);
        if (string_eq(pName, "vkCmdResolveImage2KHR")) return to_vkVoidFunction(driver_CmdResolveImage2KHR);
    } // VK_KHR_copy_commands2

    // ----- VK_KHR_maintenance4
    if (IsDeviceExtensionEnabled("VK_KHR_maintenance4")) {
        if (string_eq(pName, "vkGetDeviceBufferMemoryRequirementsKHR")) return to_vkVoidFunction(driver_GetDeviceBufferMemoryRequirementsKHR);
        if (string_eq(pName, "vkGetDeviceImageMemoryRequirementsKHR")) return to_vkVoidFunction(driver_GetDeviceImageMemoryRequirementsKHR);
        if (string_eq(pName, "vkGetDeviceImageSparseMemoryRequirementsKHR")) return to_vkVoidFunction(driver_GetDeviceImageSparseMemoryRequirementsKHR);
    } // VK_KHR_maintenance4

    // ----- VK_EXT_debug_report
    if (string_eq(pName, "vkCreateDebugReportCallbackEXT")) return to_vkVoidFunction(driver_CreateDebugReportCallbackEXT);
    if (string_eq(pName, "vkDestroyDebugReportCallbackEXT")) return to_vkVoidFunction(driver_DestroyDebugReportCallbackEXT);
    if (string_eq(pName, "vkDebugReportMessageEXT")) return to_vkVoidFunction(driver_DebugReportMessageEXT);

    // ----- VK_EXT_debug_marker
    if (IsDeviceExtensionEnabled("VK_EXT_debug_marker")) {
        if (string_eq(pName, "vkDebugMarkerSetObjectTagEXT")) return to_vkVoidFunction(driver_DebugMarkerSetObjectTagEXT);
        if (string_eq(pName, "vkDebugMarkerSetObjectNameEXT")) return to_vkVoidFunction(driver_DebugMarkerSetObjectNameEXT);
        if (string_eq(pName, "vkCmdDebugMarkerBeginEXT")) return to_vkVoidFunction(driver_CmdDebugMarkerBeginEXT);
        if (string_eq(pName, "vkCmdDebugMarkerEndEXT")) return to_vkVoidFunction(driver_CmdDebugMarkerEndEXT);
        if (string_eq(pName, "vkCmdDebugMarkerInsertEXT")) return to_vkVoidFunction(driver_CmdDebugMarkerInsertEXT);
    } // VK_EXT_debug_marker

    // ----- VK_EXT_transform_feedback
    if (IsDeviceExtensionEnabled("VK_EXT_transform_feedback")) {
        if (string_eq(pName, "vkCmdBindTransformFeedbackBuffersEXT")) return to_vkVoidFunction(driver_CmdBindTransformFeedbackBuffersEXT);
        if (string_eq(pName, "vkCmdBeginTransformFeedbackEXT")) return to_vkVoidFunction(driver_CmdBeginTransformFeedbackEXT);
        if (string_eq(pName, "vkCmdEndTransformFeedbackEXT")) return to_vkVoidFunction(driver_CmdEndTransformFeedbackEXT);
        if (string_eq(pName, "vkCmdBeginQueryIndexedEXT")) return to_vkVoidFunction(driver_CmdBeginQueryIndexedEXT);
        if (string_eq(pName, "vkCmdEndQueryIndexedEXT")) return to_vkVoidFunction(driver_CmdEndQueryIndexedEXT);
        if (string_eq(pName, "vkCmdDrawIndirectByteCountEXT")) return to_vkVoidFunction(driver_CmdDrawIndirectByteCountEXT);
    } // VK_EXT_transform_feedback

    // ----- VK_NVX_binary_import
    if (IsDeviceExtensionEnabled("VK_NVX_binary_import")) {
        if (string_eq(pName, "vkCreateCuModuleNVX")) return to_vkVoidFunction(driver_CreateCuModuleNVX);
        if (string_eq(pName, "vkCreateCuFunctionNVX")) return to_vkVoidFunction(driver_CreateCuFunctionNVX);
        if (string_eq(pName, "vkDestroyCuModuleNVX")) return to_vkVoidFunction(driver_DestroyCuModuleNVX);
        if (string_eq(pName, "vkDestroyCuFunctionNVX")) return to_vkVoidFunction(driver_DestroyCuFunctionNVX);
        if (string_eq(pName, "vkCmdCuLaunchKernelNVX")) return to_vkVoidFunction(driver_CmdCuLaunchKernelNVX);
    } // VK_NVX_binary_import

    // ----- VK_NVX_image_view_handle
    if (IsDeviceExtensionEnabled("VK_NVX_image_view_handle")) {
        if (string_eq(pName, "vkGetImageViewHandleNVX")) return to_vkVoidFunction(driver_GetImageViewHandleNVX);
        if (string_eq(pName, "vkGetImageViewAddressNVX")) return to_vkVoidFunction(driver_GetImageViewAddressNVX);
    } // VK_NVX_image_view_handle

    // ----- VK_AMD_draw_indirect_count
    if (IsDeviceExtensionEnabled("VK_AMD_draw_indirect_count")) {
        if (string_eq(pName, "vkCmdDrawIndirectCountAMD")) return to_vkVoidFunction(driver_CmdDrawIndirectCountAMD);
        if (string_eq(pName, "vkCmdDrawIndexedIndirectCountAMD")) return to_vkVoidFunction(driver_CmdDrawIndexedIndirectCountAMD);
    } // VK_AMD_draw_indirect_count

    // ----- VK_AMD_shader_info
    if (IsDeviceExtensionEnabled("VK_AMD_shader_info")) {
        if (string_eq(pName, "vkGetShaderInfoAMD")) return to_vkVoidFunction(driver_GetShaderInfoAMD);
    } // VK_AMD_shader_info

    // ----- VK_GGP_stream_descriptor_surface
#ifdef VK_USE_PLATFORM_GGP
    if (string_eq(pName, "vkCreateStreamDescriptorSurfaceGGP")) return to_vkVoidFunction(driver_CreateStreamDescriptorSurfaceGGP);
#endif // VK_USE_PLATFORM_GGP

    // ----- VK_NV_external_memory_capabilities
    if (string_eq(pName, "vkGetPhysicalDeviceExternalImageFormatPropertiesNV")) return to_vkVoidFunction(driver_GetPhysicalDeviceExternalImageFormatPropertiesNV);

    // ----- VK_NV_external_memory_win32
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (IsDeviceExtensionEnabled("VK_NV_external_memory_win32")) {
        if (string_eq(pName, "vkGetMemoryWin32HandleNV")) return to_vkVoidFunction(driver_GetMemoryWin32HandleNV);
    } // VK_NV_external_memory_win32
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_NN_vi_surface
#ifdef VK_USE_PLATFORM_VI_NN
    if (string_eq(pName, "vkCreateViSurfaceNN")) return to_vkVoidFunction(driver_CreateViSurfaceNN);
#endif // VK_USE_PLATFORM_VI_NN

    // ----- VK_EXT_conditional_rendering
    if (IsDeviceExtensionEnabled("VK_EXT_conditional_rendering")) {
        if (string_eq(pName, "vkCmdBeginConditionalRenderingEXT")) return to_vkVoidFunction(driver_CmdBeginConditionalRenderingEXT);
        if (string_eq(pName, "vkCmdEndConditionalRenderingEXT")) return to_vkVoidFunction(driver_CmdEndConditionalRenderingEXT);
    } // VK_EXT_conditional_rendering

    // ----- VK_NV_clip_space_w_scaling
    if (IsDeviceExtensionEnabled("VK_NV_clip_space_w_scaling")) {
        if (string_eq(pName, "vkCmdSetViewportWScalingNV")) return to_vkVoidFunction(driver_CmdSetViewportWScalingNV);
    } // VK_NV_clip_space_w_scaling

    // ----- VK_EXT_direct_mode_display
    if (string_eq(pName, "vkReleaseDisplayEXT")) return to_vkVoidFunction(driver_ReleaseDisplayEXT);

    // ----- VK_EXT_acquire_xlib_display
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
    if (string_eq(pName, "vkAcquireXlibDisplayEXT")) return to_vkVoidFunction(driver_AcquireXlibDisplayEXT);
    if (string_eq(pName, "vkGetRandROutputDisplayEXT")) return to_vkVoidFunction(driver_GetRandROutputDisplayEXT);
#endif // VK_USE_PLATFORM_XLIB_XRANDR_EXT

    // ----- VK_EXT_display_surface_counter
    if (string_eq(pName, "vkGetPhysicalDeviceSurfaceCapabilities2EXT")) return to_vkVoidFunction(driver_GetPhysicalDeviceSurfaceCapabilities2EXT);

    // ----- VK_EXT_display_control
    if (IsDeviceExtensionEnabled("VK_EXT_display_control")) {
        if (string_eq(pName, "vkDisplayPowerControlEXT")) return to_vkVoidFunction(driver_DisplayPowerControlEXT);
        if (string_eq(pName, "vkRegisterDeviceEventEXT")) return to_vkVoidFunction(driver_RegisterDeviceEventEXT);
        if (string_eq(pName, "vkRegisterDisplayEventEXT")) return to_vkVoidFunction(driver_RegisterDisplayEventEXT);
        if (string_eq(pName, "vkGetSwapchainCounterEXT")) return to_vkVoidFunction(driver_GetSwapchainCounterEXT);
    } // VK_EXT_display_control

    // ----- VK_GOOGLE_display_timing
    if (IsDeviceExtensionEnabled("VK_GOOGLE_display_timing")) {
        if (string_eq(pName, "vkGetRefreshCycleDurationGOOGLE")) return to_vkVoidFunction(driver_GetRefreshCycleDurationGOOGLE);
        if (string_eq(pName, "vkGetPastPresentationTimingGOOGLE")) return to_vkVoidFunction(driver_GetPastPresentationTimingGOOGLE);
    } // VK_GOOGLE_display_timing

    // ----- VK_EXT_discard_rectangles
    if (IsDeviceExtensionEnabled("VK_EXT_discard_rectangles")) {
        if (string_eq(pName, "vkCmdSetDiscardRectangleEXT")) return to_vkVoidFunction(driver_CmdSetDiscardRectangleEXT);
    } // VK_EXT_discard_rectangles

    // ----- VK_EXT_hdr_metadata
    if (IsDeviceExtensionEnabled("VK_EXT_hdr_metadata")) {
        if (string_eq(pName, "vkSetHdrMetadataEXT")) return to_vkVoidFunction(driver_SetHdrMetadataEXT);
    } // VK_EXT_hdr_metadata

    // ----- VK_MVK_ios_surface
#ifdef VK_USE_PLATFORM_IOS_MVK
    if (string_eq(pName, "vkCreateIOSSurfaceMVK")) return to_vkVoidFunction(driver_CreateIOSSurfaceMVK);
#endif // VK_USE_PLATFORM_IOS_MVK

    // ----- VK_MVK_macos_surface
#ifdef VK_USE_PLATFORM_MACOS_MVK
    if (string_eq(pName, "vkCreateMacOSSurfaceMVK")) return to_vkVoidFunction(driver_CreateMacOSSurfaceMVK);
#endif // VK_USE_PLATFORM_MACOS_MVK

    // ----- VK_EXT_debug_utils
    if (string_eq(pName, "vkSetDebugUtilsObjectNameEXT")) return to_vkVoidFunction(driver_SetDebugUtilsObjectNameEXT);
    if (string_eq(pName, "vkSetDebugUtilsObjectTagEXT")) return to_vkVoidFunction(driver_SetDebugUtilsObjectTagEXT);
    if (string_eq(pName, "vkQueueBeginDebugUtilsLabelEXT")) return to_vkVoidFunction(driver_QueueBeginDebugUtilsLabelEXT);
    if (string_eq(pName, "vkQueueEndDebugUtilsLabelEXT")) return to_vkVoidFunction(driver_QueueEndDebugUtilsLabelEXT);
    if (string_eq(pName, "vkQueueInsertDebugUtilsLabelEXT")) return to_vkVoidFunction(driver_QueueInsertDebugUtilsLabelEXT);
    if (string_eq(pName, "vkCmdBeginDebugUtilsLabelEXT")) return to_vkVoidFunction(driver_CmdBeginDebugUtilsLabelEXT);
    if (string_eq(pName, "vkCmdEndDebugUtilsLabelEXT")) return to_vkVoidFunction(driver_CmdEndDebugUtilsLabelEXT);
    if (string_eq(pName, "vkCmdInsertDebugUtilsLabelEXT")) return to_vkVoidFunction(driver_CmdInsertDebugUtilsLabelEXT);
    if (string_eq(pName, "vkCreateDebugUtilsMessengerEXT")) return to_vkVoidFunction(driver_CreateDebugUtilsMessengerEXT);
    if (string_eq(pName, "vkDestroyDebugUtilsMessengerEXT")) return to_vkVoidFunction(driver_DestroyDebugUtilsMessengerEXT);
    if (string_eq(pName, "vkSubmitDebugUtilsMessageEXT")) return to_vkVoidFunction(driver_SubmitDebugUtilsMessageEXT);

    // ----- VK_EXT_sample_locations
    if (IsDeviceExtensionEnabled("VK_EXT_sample_locations")) {
        if (string_eq(pName, "vkCmdSetSampleLocationsEXT")) return to_vkVoidFunction(driver_CmdSetSampleLocationsEXT);
        if (string_eq(pName, "vkGetPhysicalDeviceMultisamplePropertiesEXT")) return to_vkVoidFunction(driver_GetPhysicalDeviceMultisamplePropertiesEXT);
    } // VK_EXT_sample_locations

    // ----- VK_EXT_image_drm_format_modifier
    if (IsDeviceExtensionEnabled("VK_EXT_image_drm_format_modifier")) {
        if (string_eq(pName, "vkGetImageDrmFormatModifierPropertiesEXT")) return to_vkVoidFunction(driver_GetImageDrmFormatModifierPropertiesEXT);
    } // VK_EXT_image_drm_format_modifier

    // ----- VK_EXT_validation_cache
    if (IsDeviceExtensionEnabled("VK_EXT_validation_cache")) {
        if (string_eq(pName, "vkCreateValidationCacheEXT")) return to_vkVoidFunction(driver_CreateValidationCacheEXT);
        if (string_eq(pName, "vkDestroyValidationCacheEXT")) return to_vkVoidFunction(driver_DestroyValidationCacheEXT);
        if (string_eq(pName, "vkMergeValidationCachesEXT")) return to_vkVoidFunction(driver_MergeValidationCachesEXT);
        if (string_eq(pName, "vkGetValidationCacheDataEXT")) return to_vkVoidFunction(driver_GetValidationCacheDataEXT);
    } // VK_EXT_validation_cache

    // ----- VK_NV_shading_rate_image
    if (IsDeviceExtensionEnabled("VK_NV_shading_rate_image")) {
        if (string_eq(pName, "vkCmdBindShadingRateImageNV")) return to_vkVoidFunction(driver_CmdBindShadingRateImageNV);
        if (string_eq(pName, "vkCmdSetViewportShadingRatePaletteNV")) return to_vkVoidFunction(driver_CmdSetViewportShadingRatePaletteNV);
        if (string_eq(pName, "vkCmdSetCoarseSampleOrderNV")) return to_vkVoidFunction(driver_CmdSetCoarseSampleOrderNV);
    } // VK_NV_shading_rate_image

    // ----- VK_NV_ray_tracing
    if (IsDeviceExtensionEnabled("VK_NV_ray_tracing")) {
        if (string_eq(pName, "vkCreateAccelerationStructureNV")) return to_vkVoidFunction(driver_CreateAccelerationStructureNV);
        if (string_eq(pName, "vkDestroyAccelerationStructureNV")) return to_vkVoidFunction(driver_DestroyAccelerationStructureNV);
        if (string_eq(pName, "vkGetAccelerationStructureMemoryRequirementsNV")) return to_vkVoidFunction(driver_GetAccelerationStructureMemoryRequirementsNV);
        if (string_eq(pName, "vkBindAccelerationStructureMemoryNV")) return to_vkVoidFunction(driver_BindAccelerationStructureMemoryNV);
        if (string_eq(pName, "vkCmdBuildAccelerationStructureNV")) return to_vkVoidFunction(driver_CmdBuildAccelerationStructureNV);
        if (string_eq(pName, "vkCmdCopyAccelerationStructureNV")) return to_vkVoidFunction(driver_CmdCopyAccelerationStructureNV);
        if (string_eq(pName, "vkCmdTraceRaysNV")) return to_vkVoidFunction(driver_CmdTraceRaysNV);
        if (string_eq(pName, "vkCreateRayTracingPipelinesNV")) return to_vkVoidFunction(driver_CreateRayTracingPipelinesNV);
        if (string_eq(pName, "vkGetRayTracingShaderGroupHandlesNV")) return to_vkVoidFunction(driver_GetRayTracingShaderGroupHandlesNV);
        if (string_eq(pName, "vkGetAccelerationStructureHandleNV")) return to_vkVoidFunction(driver_GetAccelerationStructureHandleNV);
        if (string_eq(pName, "vkCmdWriteAccelerationStructuresPropertiesNV")) return to_vkVoidFunction(driver_CmdWriteAccelerationStructuresPropertiesNV);
        if (string_eq(pName, "vkCompileDeferredNV")) return to_vkVoidFunction(driver_CompileDeferredNV);
    } // VK_NV_ray_tracing

    // ----- VK_EXT_external_memory_host
    if (IsDeviceExtensionEnabled("VK_EXT_external_memory_host")) {
        if (string_eq(pName, "vkGetMemoryHostPointerPropertiesEXT")) return to_vkVoidFunction(driver_GetMemoryHostPointerPropertiesEXT);
    } // VK_EXT_external_memory_host

    // ----- VK_AMD_buffer_marker
    if (IsDeviceExtensionEnabled("VK_AMD_buffer_marker")) {
        if (string_eq(pName, "vkCmdWriteBufferMarkerAMD")) return to_vkVoidFunction(driver_CmdWriteBufferMarkerAMD);
    } // VK_AMD_buffer_marker

    // ----- VK_EXT_calibrated_timestamps
    if (IsDeviceExtensionEnabled("VK_EXT_calibrated_timestamps")) {
        if (string_eq(pName, "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT")) return to_vkVoidFunction(driver_GetPhysicalDeviceCalibrateableTimeDomainsEXT);
        if (string_eq(pName, "vkGetCalibratedTimestampsEXT")) return to_vkVoidFunction(driver_GetCalibratedTimestampsEXT);
    } // VK_EXT_calibrated_timestamps

    // ----- VK_NV_mesh_shader
    if (IsDeviceExtensionEnabled("VK_NV_mesh_shader")) {
        if (string_eq(pName, "vkCmdDrawMeshTasksNV")) return to_vkVoidFunction(driver_CmdDrawMeshTasksNV);
        if (string_eq(pName, "vkCmdDrawMeshTasksIndirectNV")) return to_vkVoidFunction(driver_CmdDrawMeshTasksIndirectNV);
        if (string_eq(pName, "vkCmdDrawMeshTasksIndirectCountNV")) return to_vkVoidFunction(driver_CmdDrawMeshTasksIndirectCountNV);
    } // VK_NV_mesh_shader

    // ----- VK_NV_scissor_exclusive
    if (IsDeviceExtensionEnabled("VK_NV_scissor_exclusive")) {
        if (string_eq(pName, "vkCmdSetExclusiveScissorNV")) return to_vkVoidFunction(driver_CmdSetExclusiveScissorNV);
    } // VK_NV_scissor_exclusive

    // ----- VK_NV_device_diagnostic_checkpoints
    if (IsDeviceExtensionEnabled("VK_NV_device_diagnostic_checkpoints")) {
        if (string_eq(pName, "vkCmdSetCheckpointNV")) return to_vkVoidFunction(driver_CmdSetCheckpointNV);
        if (string_eq(pName, "vkGetQueueCheckpointDataNV")) return to_vkVoidFunction(driver_GetQueueCheckpointDataNV);
    } // VK_NV_device_diagnostic_checkpoints

    // ----- VK_INTEL_performance_query
    if (IsDeviceExtensionEnabled("VK_INTEL_performance_query")) {
        if (string_eq(pName, "vkInitializePerformanceApiINTEL")) return to_vkVoidFunction(driver_InitializePerformanceApiINTEL);
        if (string_eq(pName, "vkUninitializePerformanceApiINTEL")) return to_vkVoidFunction(driver_UninitializePerformanceApiINTEL);
        if (string_eq(pName, "vkCmdSetPerformanceMarkerINTEL")) return to_vkVoidFunction(driver_CmdSetPerformanceMarkerINTEL);
        if (string_eq(pName, "vkCmdSetPerformanceStreamMarkerINTEL")) return to_vkVoidFunction(driver_CmdSetPerformanceStreamMarkerINTEL);
        if (string_eq(pName, "vkCmdSetPerformanceOverrideINTEL")) return to_vkVoidFunction(driver_CmdSetPerformanceOverrideINTEL);
        if (string_eq(pName, "vkAcquirePerformanceConfigurationINTEL")) return to_vkVoidFunction(driver_AcquirePerformanceConfigurationINTEL);
        if (string_eq(pName, "vkReleasePerformanceConfigurationINTEL")) return to_vkVoidFunction(driver_ReleasePerformanceConfigurationINTEL);
        if (string_eq(pName, "vkQueueSetPerformanceConfigurationINTEL")) return to_vkVoidFunction(driver_QueueSetPerformanceConfigurationINTEL);
        if (string_eq(pName, "vkGetPerformanceParameterINTEL")) return to_vkVoidFunction(driver_GetPerformanceParameterINTEL);
    } // VK_INTEL_performance_query

    // ----- VK_AMD_display_native_hdr
    if (IsDeviceExtensionEnabled("VK_AMD_display_native_hdr")) {
        if (string_eq(pName, "vkSetLocalDimmingAMD")) return to_vkVoidFunction(driver_SetLocalDimmingAMD);
    } // VK_AMD_display_native_hdr

    // ----- VK_FUCHSIA_imagepipe_surface
#ifdef VK_USE_PLATFORM_FUCHSIA
    if (string_eq(pName, "vkCreateImagePipeSurfaceFUCHSIA")) return to_vkVoidFunction(driver_CreateImagePipeSurfaceFUCHSIA);
#endif // VK_USE_PLATFORM_FUCHSIA

    // ----- VK_EXT_metal_surface
#ifdef VK_USE_PLATFORM_METAL_EXT
    if (string_eq(pName, "vkCreateMetalSurfaceEXT")) return to_vkVoidFunction(driver_CreateMetalSurfaceEXT);
#endif // VK_USE_PLATFORM_METAL_EXT

    // ----- VK_EXT_buffer_device_address
    if (IsDeviceExtensionEnabled("VK_EXT_buffer_device_address")) {
        if (string_eq(pName, "vkGetBufferDeviceAddressEXT")) return to_vkVoidFunction(driver_GetBufferDeviceAddressEXT);
    } // VK_EXT_buffer_device_address

    // ----- VK_EXT_tooling_info
    if (IsDeviceExtensionEnabled("VK_EXT_tooling_info")) {
        if (string_eq(pName, "vkGetPhysicalDeviceToolPropertiesEXT")) return to_vkVoidFunction(driver_GetPhysicalDeviceToolPropertiesEXT);
    } // VK_EXT_tooling_info

    // ----- VK_NV_cooperative_matrix
    if (IsDeviceExtensionEnabled("VK_NV_cooperative_matrix")) {
        if (string_eq(pName, "vkGetPhysicalDeviceCooperativeMatrixPropertiesNV")) return to_vkVoidFunction(driver_GetPhysicalDeviceCooperativeMatrixPropertiesNV);
    } // VK_NV_cooperative_matrix

    // ----- VK_NV_coverage_reduction_mode
    if (IsDeviceExtensionEnabled("VK_NV_coverage_reduction_mode")) {
        if (string_eq(pName, "vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV")) return to_vkVoidFunction(driver_GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV);
    } // VK_NV_coverage_reduction_mode

    // ----- VK_EXT_full_screen_exclusive
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (IsDeviceExtensionEnabled("VK_EXT_full_screen_exclusive")) {
        if (string_eq(pName, "vkGetPhysicalDeviceSurfacePresentModes2EXT")) return to_vkVoidFunction(driver_GetPhysicalDeviceSurfacePresentModes2EXT);
        if (string_eq(pName, "vkAcquireFullScreenExclusiveModeEXT")) return to_vkVoidFunction(driver_AcquireFullScreenExclusiveModeEXT);
        if (string_eq(pName, "vkReleaseFullScreenExclusiveModeEXT")) return to_vkVoidFunction(driver_ReleaseFullScreenExclusiveModeEXT);
        if (string_eq(pName, "vkGetDeviceGroupSurfacePresentModes2EXT")) return to_vkVoidFunction(driver_GetDeviceGroupSurfacePresentModes2EXT);
    } // VK_EXT_full_screen_exclusive
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_EXT_headless_surface
    if (string_eq(pName, "vkCreateHeadlessSurfaceEXT")) return to_vkVoidFunction(driver_CreateHeadlessSurfaceEXT);

    // ----- VK_EXT_line_rasterization
    if (IsDeviceExtensionEnabled("VK_EXT_line_rasterization")) {
        if (string_eq(pName, "vkCmdSetLineStippleEXT")) return to_vkVoidFunction(driver_CmdSetLineStippleEXT);
    } // VK_EXT_line_rasterization

    // ----- VK_EXT_host_query_reset
    if (IsDeviceExtensionEnabled("VK_EXT_host_query_reset")) {
        if (string_eq(pName, "vkResetQueryPoolEXT")) return to_vkVoidFunction(driver_ResetQueryPoolEXT);
    } // VK_EXT_host_query_reset

    // ----- VK_EXT_extended_dynamic_state
    if (IsDeviceExtensionEnabled("VK_EXT_extended_dynamic_state")) {
        if (string_eq(pName, "vkCmdSetCullModeEXT")) return to_vkVoidFunction(driver_CmdSetCullModeEXT);
        if (string_eq(pName, "vkCmdSetFrontFaceEXT")) return to_vkVoidFunction(driver_CmdSetFrontFaceEXT);
        if (string_eq(pName, "vkCmdSetPrimitiveTopologyEXT")) return to_vkVoidFunction(driver_CmdSetPrimitiveTopologyEXT);
        if (string_eq(pName, "vkCmdSetViewportWithCountEXT")) return to_vkVoidFunction(driver_CmdSetViewportWithCountEXT);
        if (string_eq(pName, "vkCmdSetScissorWithCountEXT")) return to_vkVoidFunction(driver_CmdSetScissorWithCountEXT);
        if (string_eq(pName, "vkCmdBindVertexBuffers2EXT")) return to_vkVoidFunction(driver_CmdBindVertexBuffers2EXT);
        if (string_eq(pName, "vkCmdSetDepthTestEnableEXT")) return to_vkVoidFunction(driver_CmdSetDepthTestEnableEXT);
        if (string_eq(pName, "vkCmdSetDepthWriteEnableEXT")) return to_vkVoidFunction(driver_CmdSetDepthWriteEnableEXT);
        if (string_eq(pName, "vkCmdSetDepthCompareOpEXT")) return to_vkVoidFunction(driver_CmdSetDepthCompareOpEXT);
        if (string_eq(pName, "vkCmdSetDepthBoundsTestEnableEXT")) return to_vkVoidFunction(driver_CmdSetDepthBoundsTestEnableEXT);
        if (string_eq(pName, "vkCmdSetStencilTestEnableEXT")) return to_vkVoidFunction(driver_CmdSetStencilTestEnableEXT);
        if (string_eq(pName, "vkCmdSetStencilOpEXT")) return to_vkVoidFunction(driver_CmdSetStencilOpEXT);
    } // VK_EXT_extended_dynamic_state

    // ----- VK_NV_device_generated_commands
    if (IsDeviceExtensionEnabled("VK_NV_device_generated_commands")) {
        if (string_eq(pName, "vkGetGeneratedCommandsMemoryRequirementsNV")) return to_vkVoidFunction(driver_GetGeneratedCommandsMemoryRequirementsNV);
        if (string_eq(pName, "vkCmdPreprocessGeneratedCommandsNV")) return to_vkVoidFunction(driver_CmdPreprocessGeneratedCommandsNV);
        if (string_eq(pName, "vkCmdExecuteGeneratedCommandsNV")) return to_vkVoidFunction(driver_CmdExecuteGeneratedCommandsNV);
        if (string_eq(pName, "vkCmdBindPipelineShaderGroupNV")) return to_vkVoidFunction(driver_CmdBindPipelineShaderGroupNV);
        if (string_eq(pName, "vkCreateIndirectCommandsLayoutNV")) return to_vkVoidFunction(driver_CreateIndirectCommandsLayoutNV);
        if (string_eq(pName, "vkDestroyIndirectCommandsLayoutNV")) return to_vkVoidFunction(driver_DestroyIndirectCommandsLayoutNV);
    } // VK_NV_device_generated_commands

    // ----- VK_EXT_acquire_drm_display
    if (string_eq(pName, "vkAcquireDrmDisplayEXT")) return to_vkVoidFunction(driver_AcquireDrmDisplayEXT);
    if (string_eq(pName, "vkGetDrmDisplayEXT")) return to_vkVoidFunction(driver_GetDrmDisplayEXT);

    // ----- VK_EXT_private_data
    if (IsDeviceExtensionEnabled("VK_EXT_private_data")) {
        if (string_eq(pName, "vkCreatePrivateDataSlotEXT")) return to_vkVoidFunction(driver_CreatePrivateDataSlotEXT);
        if (string_eq(pName, "vkDestroyPrivateDataSlotEXT")) return to_vkVoidFunction(driver_DestroyPrivateDataSlotEXT);
        if (string_eq(pName, "vkSetPrivateDataEXT")) return to_vkVoidFunction(driver_SetPrivateDataEXT);
        if (string_eq(pName, "vkGetPrivateDataEXT")) return to_vkVoidFunction(driver_GetPrivateDataEXT);
    } // VK_EXT_private_data

    // ----- VK_NV_fragment_shading_rate_enums
    if (IsDeviceExtensionEnabled("VK_NV_fragment_shading_rate_enums")) {
        if (string_eq(pName, "vkCmdSetFragmentShadingRateEnumNV")) return to_vkVoidFunction(driver_CmdSetFragmentShadingRateEnumNV);
    } // VK_NV_fragment_shading_rate_enums

    // ----- VK_NV_acquire_winrt_display
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (IsDeviceExtensionEnabled("VK_NV_acquire_winrt_display")) {
        if (string_eq(pName, "vkAcquireWinrtDisplayNV")) return to_vkVoidFunction(driver_AcquireWinrtDisplayNV);
        if (string_eq(pName, "vkGetWinrtDisplayNV")) return to_vkVoidFunction(driver_GetWinrtDisplayNV);
    } // VK_NV_acquire_winrt_display
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ----- VK_EXT_directfb_surface
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
    if (string_eq(pName, "vkCreateDirectFBSurfaceEXT")) return to_vkVoidFunction(driver_CreateDirectFBSurfaceEXT);
    if (string_eq(pName, "vkGetPhysicalDeviceDirectFBPresentationSupportEXT")) return to_vkVoidFunction(driver_GetPhysicalDeviceDirectFBPresentationSupportEXT);
#endif // VK_USE_PLATFORM_DIRECTFB_EXT

    // ----- VK_EXT_vertex_input_dynamic_state
    if (IsDeviceExtensionEnabled("VK_EXT_vertex_input_dynamic_state")) {
        if (string_eq(pName, "vkCmdSetVertexInputEXT")) return to_vkVoidFunction(driver_CmdSetVertexInputEXT);
    } // VK_EXT_vertex_input_dynamic_state

    // ----- VK_FUCHSIA_external_memory
#ifdef VK_USE_PLATFORM_FUCHSIA
    if (IsDeviceExtensionEnabled("VK_FUCHSIA_external_memory")) {
        if (string_eq(pName, "vkGetMemoryZirconHandleFUCHSIA")) return to_vkVoidFunction(driver_GetMemoryZirconHandleFUCHSIA);
        if (string_eq(pName, "vkGetMemoryZirconHandlePropertiesFUCHSIA")) return to_vkVoidFunction(driver_GetMemoryZirconHandlePropertiesFUCHSIA);
    } // VK_FUCHSIA_external_memory
#endif // VK_USE_PLATFORM_FUCHSIA

    // ----- VK_FUCHSIA_external_semaphore
#ifdef VK_USE_PLATFORM_FUCHSIA
    if (IsDeviceExtensionEnabled("VK_FUCHSIA_external_semaphore")) {
        if (string_eq(pName, "vkImportSemaphoreZirconHandleFUCHSIA")) return to_vkVoidFunction(driver_ImportSemaphoreZirconHandleFUCHSIA);
        if (string_eq(pName, "vkGetSemaphoreZirconHandleFUCHSIA")) return to_vkVoidFunction(driver_GetSemaphoreZirconHandleFUCHSIA);
    } // VK_FUCHSIA_external_semaphore
#endif // VK_USE_PLATFORM_FUCHSIA

    // ----- VK_FUCHSIA_buffer_collection
#ifdef VK_USE_PLATFORM_FUCHSIA
    if (IsDeviceExtensionEnabled("VK_FUCHSIA_buffer_collection")) {
        if (string_eq(pName, "vkCreateBufferCollectionFUCHSIA")) return to_vkVoidFunction(driver_CreateBufferCollectionFUCHSIA);
        if (string_eq(pName, "vkSetBufferCollectionImageConstraintsFUCHSIA")) return to_vkVoidFunction(driver_SetBufferCollectionImageConstraintsFUCHSIA);
        if (string_eq(pName, "vkSetBufferCollectionBufferConstraintsFUCHSIA")) return to_vkVoidFunction(driver_SetBufferCollectionBufferConstraintsFUCHSIA);
        if (string_eq(pName, "vkDestroyBufferCollectionFUCHSIA")) return to_vkVoidFunction(driver_DestroyBufferCollectionFUCHSIA);
        if (string_eq(pName, "vkGetBufferCollectionPropertiesFUCHSIA")) return to_vkVoidFunction(driver_GetBufferCollectionPropertiesFUCHSIA);
    } // VK_FUCHSIA_buffer_collection
#endif // VK_USE_PLATFORM_FUCHSIA

    // ----- VK_HUAWEI_subpass_shading
    if (IsDeviceExtensionEnabled("VK_HUAWEI_subpass_shading")) {
        if (string_eq(pName, "vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI")) return to_vkVoidFunction(driver_GetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI);
        if (string_eq(pName, "vkCmdSubpassShadingHUAWEI")) return to_vkVoidFunction(driver_CmdSubpassShadingHUAWEI);
    } // VK_HUAWEI_subpass_shading

    // ----- VK_HUAWEI_invocation_mask
    if (IsDeviceExtensionEnabled("VK_HUAWEI_invocation_mask")) {
        if (string_eq(pName, "vkCmdBindInvocationMaskHUAWEI")) return to_vkVoidFunction(driver_CmdBindInvocationMaskHUAWEI);
    } // VK_HUAWEI_invocation_mask

    // ----- VK_NV_external_memory_rdma
    if (IsDeviceExtensionEnabled("VK_NV_external_memory_rdma")) {
        if (string_eq(pName, "vkGetMemoryRemoteAddressNV")) return to_vkVoidFunction(driver_GetMemoryRemoteAddressNV);
    } // VK_NV_external_memory_rdma

    // ----- VK_EXT_extended_dynamic_state2
    if (IsDeviceExtensionEnabled("VK_EXT_extended_dynamic_state2")) {
        if (string_eq(pName, "vkCmdSetPatchControlPointsEXT")) return to_vkVoidFunction(driver_CmdSetPatchControlPointsEXT);
        if (string_eq(pName, "vkCmdSetRasterizerDiscardEnableEXT")) return to_vkVoidFunction(driver_CmdSetRasterizerDiscardEnableEXT);
        if (string_eq(pName, "vkCmdSetDepthBiasEnableEXT")) return to_vkVoidFunction(driver_CmdSetDepthBiasEnableEXT);
        if (string_eq(pName, "vkCmdSetLogicOpEXT")) return to_vkVoidFunction(driver_CmdSetLogicOpEXT);
        if (string_eq(pName, "vkCmdSetPrimitiveRestartEnableEXT")) return to_vkVoidFunction(driver_CmdSetPrimitiveRestartEnableEXT);
    } // VK_EXT_extended_dynamic_state2

    // ----- VK_QNX_screen_surface
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    if (string_eq(pName, "vkCreateScreenSurfaceQNX")) return to_vkVoidFunction(driver_CreateScreenSurfaceQNX);
    if (string_eq(pName, "vkGetPhysicalDeviceScreenPresentationSupportQNX")) return to_vkVoidFunction(driver_GetPhysicalDeviceScreenPresentationSupportQNX);
#endif // VK_USE_PLATFORM_SCREEN_QNX

    // ----- VK_EXT_color_write_enable
    if (IsDeviceExtensionEnabled("VK_EXT_color_write_enable")) {
        if (string_eq(pName, "vkCmdSetColorWriteEnableEXT")) return to_vkVoidFunction(driver_CmdSetColorWriteEnableEXT);
    } // VK_EXT_color_write_enable

    // ----- VK_EXT_multi_draw
    if (IsDeviceExtensionEnabled("VK_EXT_multi_draw")) {
        if (string_eq(pName, "vkCmdDrawMultiEXT")) return to_vkVoidFunction(driver_CmdDrawMultiEXT);
        if (string_eq(pName, "vkCmdDrawMultiIndexedEXT")) return to_vkVoidFunction(driver_CmdDrawMultiIndexedEXT);
    } // VK_EXT_multi_draw

    // ----- VK_EXT_pageable_device_local_memory
    if (IsDeviceExtensionEnabled("VK_EXT_pageable_device_local_memory")) {
        if (string_eq(pName, "vkSetDeviceMemoryPriorityEXT")) return to_vkVoidFunction(driver_SetDeviceMemoryPriorityEXT);
    } // VK_EXT_pageable_device_local_memory

    // ----- VK_VALVE_descriptor_set_host_mapping
    if (IsDeviceExtensionEnabled("VK_VALVE_descriptor_set_host_mapping")) {
        if (string_eq(pName, "vkGetDescriptorSetLayoutHostMappingInfoVALVE")) return to_vkVoidFunction(driver_GetDescriptorSetLayoutHostMappingInfoVALVE);
        if (string_eq(pName, "vkGetDescriptorSetHostMappingVALVE")) return to_vkVoidFunction(driver_GetDescriptorSetHostMappingVALVE);
    } // VK_VALVE_descriptor_set_host_mapping

    // ----- VK_KHR_acceleration_structure
    if (IsDeviceExtensionEnabled("VK_KHR_acceleration_structure")) {
        if (string_eq(pName, "vkCreateAccelerationStructureKHR")) return to_vkVoidFunction(driver_CreateAccelerationStructureKHR);
        if (string_eq(pName, "vkDestroyAccelerationStructureKHR")) return to_vkVoidFunction(driver_DestroyAccelerationStructureKHR);
        if (string_eq(pName, "vkCmdBuildAccelerationStructuresKHR")) return to_vkVoidFunction(driver_CmdBuildAccelerationStructuresKHR);
        if (string_eq(pName, "vkCmdBuildAccelerationStructuresIndirectKHR")) return to_vkVoidFunction(driver_CmdBuildAccelerationStructuresIndirectKHR);
        if (string_eq(pName, "vkBuildAccelerationStructuresKHR")) return to_vkVoidFunction(driver_BuildAccelerationStructuresKHR);
        if (string_eq(pName, "vkCopyAccelerationStructureKHR")) return to_vkVoidFunction(driver_CopyAccelerationStructureKHR);
        if (string_eq(pName, "vkCopyAccelerationStructureToMemoryKHR")) return to_vkVoidFunction(driver_CopyAccelerationStructureToMemoryKHR);
        if (string_eq(pName, "vkCopyMemoryToAccelerationStructureKHR")) return to_vkVoidFunction(driver_CopyMemoryToAccelerationStructureKHR);
        if (string_eq(pName, "vkWriteAccelerationStructuresPropertiesKHR")) return to_vkVoidFunction(driver_WriteAccelerationStructuresPropertiesKHR);
        if (string_eq(pName, "vkCmdCopyAccelerationStructureKHR")) return to_vkVoidFunction(driver_CmdCopyAccelerationStructureKHR);
        if (string_eq(pName, "vkCmdCopyAccelerationStructureToMemoryKHR")) return to_vkVoidFunction(driver_CmdCopyAccelerationStructureToMemoryKHR);
        if (string_eq(pName, "vkCmdCopyMemoryToAccelerationStructureKHR")) return to_vkVoidFunction(driver_CmdCopyMemoryToAccelerationStructureKHR);
        if (string_eq(pName, "vkGetAccelerationStructureDeviceAddressKHR")) return to_vkVoidFunction(driver_GetAccelerationStructureDeviceAddressKHR);
        if (string_eq(pName, "vkCmdWriteAccelerationStructuresPropertiesKHR")) return to_vkVoidFunction(driver_CmdWriteAccelerationStructuresPropertiesKHR);
        if (string_eq(pName, "vkGetDeviceAccelerationStructureCompatibilityKHR")) return to_vkVoidFunction(driver_GetDeviceAccelerationStructureCompatibilityKHR);
        if (string_eq(pName, "vkGetAccelerationStructureBuildSizesKHR")) return to_vkVoidFunction(driver_GetAccelerationStructureBuildSizesKHR);
    } // VK_KHR_acceleration_structure

    // ----- VK_KHR_ray_tracing_pipeline
    if (IsDeviceExtensionEnabled("VK_KHR_ray_tracing_pipeline")) {
        if (string_eq(pName, "vkCmdTraceRaysKHR")) return to_vkVoidFunction(driver_CmdTraceRaysKHR);
        if (string_eq(pName, "vkCreateRayTracingPipelinesKHR")) return to_vkVoidFunction(driver_CreateRayTracingPipelinesKHR);
        if (string_eq(pName, "vkGetRayTracingShaderGroupHandlesKHR")) return to_vkVoidFunction(driver_GetRayTracingShaderGroupHandlesKHR);
        if (string_eq(pName, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR")) return to_vkVoidFunction(driver_GetRayTracingCaptureReplayShaderGroupHandlesKHR);
        if (string_eq(pName, "vkCmdTraceRaysIndirectKHR")) return to_vkVoidFunction(driver_CmdTraceRaysIndirectKHR);
        if (string_eq(pName, "vkGetRayTracingShaderGroupStackSizeKHR")) return to_vkVoidFunction(driver_GetRayTracingShaderGroupStackSizeKHR);
        if (string_eq(pName, "vkCmdSetRayTracingPipelineStackSizeKHR")) return to_vkVoidFunction(driver_CmdSetRayTracingPipelineStackSizeKHR);
    } // VK_KHR_ray_tracing_pipeline

    return nullptr;
}

extern FRAMEWORK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vk_icdNegotiateLoaderICDInterfaceVersion(uint32_t* pSupportedVersion) {
    *pSupportedVersion = driver.max_icd_interface_version;
    return VK_SUCCESS;
}

FRAMEWORK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vk_icdGetInstanceProcAddr(VkInstance instance, const char* pName) {
    return driver_GetInstanceProcAddr(instance, pName);
}
}  // extern "C"


// clang-format on

