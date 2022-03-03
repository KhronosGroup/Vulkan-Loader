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

#include "test_environment.h"
#include "loader/generated/vk_dispatch_table_helper.h"

// Test for Vulkan Core 1.0
TEST(BasicEntrypointTest, VulkanCore_1_0) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_0;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkFormat var_vkformat{};
    VkFormatProperties var_vkformatproperties{};
    inst_disp_table.GetPhysicalDeviceFormatProperties(var_vkphysicaldevice, var_vkformat, &var_vkformatproperties);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceFormatProperties"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceFormatProperties"));
    log.logger.clear();

    VkQueueFamilyProperties var_vkqueuefamilyproperties{};
    inst_disp_table.GetPhysicalDeviceQueueFamilyProperties(var_vkphysicaldevice, &var_uint32_t, &var_vkqueuefamilyproperties);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceQueueFamilyProperties"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceQueueFamilyProperties"));
    log.logger.clear();

    VkImageType var_vkimagetype{};
    VkSampleCountFlagBits var_vksamplecountflagbits{};
    VkImageUsageFlags var_vkimageusageflags{};
    VkImageTiling var_vkimagetiling{};
    VkSparseImageFormatProperties var_vksparseimageformatproperties{};
    inst_disp_table.GetPhysicalDeviceSparseImageFormatProperties(var_vkphysicaldevice, var_vkformat, var_vkimagetype, var_vksamplecountflagbits, var_vkimageusageflags, var_vkimagetiling, &var_uint32_t, &var_vksparseimageformatproperties);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceSparseImageFormatProperties"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceSparseImageFormatProperties"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    device_disp_table.DeviceWaitIdle(dev.dev);
    ASSERT_TRUE(log.find("Generated Layer vkDeviceWaitIdle"));
    ASSERT_TRUE(log.find("Generated Driver vkDeviceWaitIdle"));
    log.logger.clear();

    VkDeviceMemory var_vkdevicememory{};
    VkDeviceSize var_vkdevicesize{};
    VkMemoryMapFlags var_vkmemorymapflags{};
    uint64_t big_chunk_of_mem[512][512];

    device_disp_table.MapMemory(dev.dev, var_vkdevicememory, var_vkdevicesize, var_vkdevicesize, var_vkmemorymapflags, reinterpret_cast<void**>(&big_chunk_of_mem));
    ASSERT_TRUE(log.find("Generated Layer vkMapMemory"));
    ASSERT_TRUE(log.find("Generated Driver vkMapMemory"));
    log.logger.clear();

    device_disp_table.UnmapMemory(dev.dev, var_vkdevicememory);
    ASSERT_TRUE(log.find("Generated Layer vkUnmapMemory"));
    ASSERT_TRUE(log.find("Generated Driver vkUnmapMemory"));
    log.logger.clear();

    VkMappedMemoryRange var_vkmappedmemoryrange{VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE};
    device_disp_table.FlushMappedMemoryRanges(dev.dev, var_uint32_t, &var_vkmappedmemoryrange);
    ASSERT_TRUE(log.find("Generated Layer vkFlushMappedMemoryRanges"));
    ASSERT_TRUE(log.find("Generated Driver vkFlushMappedMemoryRanges"));
    log.logger.clear();

    device_disp_table.InvalidateMappedMemoryRanges(dev.dev, var_uint32_t, &var_vkmappedmemoryrange);
    ASSERT_TRUE(log.find("Generated Layer vkInvalidateMappedMemoryRanges"));
    ASSERT_TRUE(log.find("Generated Driver vkInvalidateMappedMemoryRanges"));
    log.logger.clear();

    VkBuffer var_vkbuffer{};
    device_disp_table.BindBufferMemory(dev.dev, var_vkbuffer, var_vkdevicememory, var_vkdevicesize);
    ASSERT_TRUE(log.find("Generated Layer vkBindBufferMemory"));
    ASSERT_TRUE(log.find("Generated Driver vkBindBufferMemory"));
    log.logger.clear();

    VkImage var_vkimage{};
    device_disp_table.BindImageMemory(dev.dev, var_vkimage, var_vkdevicememory, var_vkdevicesize);
    ASSERT_TRUE(log.find("Generated Layer vkBindImageMemory"));
    ASSERT_TRUE(log.find("Generated Driver vkBindImageMemory"));
    log.logger.clear();

    VkMemoryRequirements var_vkmemoryrequirements{};
    device_disp_table.GetBufferMemoryRequirements(dev.dev, var_vkbuffer, &var_vkmemoryrequirements);
    ASSERT_TRUE(log.find("Generated Layer vkGetBufferMemoryRequirements"));
    ASSERT_TRUE(log.find("Generated Driver vkGetBufferMemoryRequirements"));
    log.logger.clear();

    device_disp_table.GetImageMemoryRequirements(dev.dev, var_vkimage, &var_vkmemoryrequirements);
    ASSERT_TRUE(log.find("Generated Layer vkGetImageMemoryRequirements"));
    ASSERT_TRUE(log.find("Generated Driver vkGetImageMemoryRequirements"));
    log.logger.clear();

    VkSparseImageMemoryRequirements var_vksparseimagememoryrequirements{};
    device_disp_table.GetImageSparseMemoryRequirements(dev.dev, var_vkimage, &var_uint32_t, &var_vksparseimagememoryrequirements);
    ASSERT_TRUE(log.find("Generated Layer vkGetImageSparseMemoryRequirements"));
    ASSERT_TRUE(log.find("Generated Driver vkGetImageSparseMemoryRequirements"));
    log.logger.clear();

    VkFence var_vkfence{};
    device_disp_table.ResetFences(dev.dev, var_uint32_t, &var_vkfence);
    ASSERT_TRUE(log.find("Generated Layer vkResetFences"));
    ASSERT_TRUE(log.find("Generated Driver vkResetFences"));
    log.logger.clear();

    device_disp_table.GetFenceStatus(dev.dev, var_vkfence);
    ASSERT_TRUE(log.find("Generated Layer vkGetFenceStatus"));
    ASSERT_TRUE(log.find("Generated Driver vkGetFenceStatus"));
    log.logger.clear();

    VkBool32 var_vkbool32{};
    uint64_t var_uint64_t = 1;
    device_disp_table.WaitForFences(dev.dev, var_uint32_t, &var_vkfence, var_vkbool32, var_uint64_t);
    ASSERT_TRUE(log.find("Generated Layer vkWaitForFences"));
    ASSERT_TRUE(log.find("Generated Driver vkWaitForFences"));
    log.logger.clear();

    VkEvent var_vkevent{};
    device_disp_table.GetEventStatus(dev.dev, var_vkevent);
    ASSERT_TRUE(log.find("Generated Layer vkGetEventStatus"));
    ASSERT_TRUE(log.find("Generated Driver vkGetEventStatus"));
    log.logger.clear();

    device_disp_table.SetEvent(dev.dev, var_vkevent);
    ASSERT_TRUE(log.find("Generated Layer vkSetEvent"));
    ASSERT_TRUE(log.find("Generated Driver vkSetEvent"));
    log.logger.clear();

    device_disp_table.ResetEvent(dev.dev, var_vkevent);
    ASSERT_TRUE(log.find("Generated Layer vkResetEvent"));
    ASSERT_TRUE(log.find("Generated Driver vkResetEvent"));
    log.logger.clear();

    VkQueryPool var_vkquerypool{};
    size_t var_size_t{};
    VkQueryResultFlags var_vkqueryresultflags{};
    device_disp_table.GetQueryPoolResults(dev.dev, var_vkquerypool, var_uint32_t, var_uint32_t, var_size_t, reinterpret_cast<void**>(big_chunk_of_mem), var_vkdevicesize, var_vkqueryresultflags);
    ASSERT_TRUE(log.find("Generated Layer vkGetQueryPoolResults"));
    ASSERT_TRUE(log.find("Generated Driver vkGetQueryPoolResults"));
    log.logger.clear();

    VkImageSubresource var_vkimagesubresource{};
    VkSubresourceLayout var_vksubresourcelayout{};
    device_disp_table.GetImageSubresourceLayout(dev.dev, var_vkimage, &var_vkimagesubresource, &var_vksubresourcelayout);
    ASSERT_TRUE(log.find("Generated Layer vkGetImageSubresourceLayout"));
    ASSERT_TRUE(log.find("Generated Driver vkGetImageSubresourceLayout"));
    log.logger.clear();

    VkPipelineCache var_vkpipelinecache{};
    device_disp_table.GetPipelineCacheData(dev.dev, var_vkpipelinecache, &var_size_t, reinterpret_cast<void**>(big_chunk_of_mem));
    ASSERT_TRUE(log.find("Generated Layer vkGetPipelineCacheData"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPipelineCacheData"));
    log.logger.clear();

    device_disp_table.MergePipelineCaches(dev.dev, var_vkpipelinecache, var_uint32_t, &var_vkpipelinecache);
    ASSERT_TRUE(log.find("Generated Layer vkMergePipelineCaches"));
    ASSERT_TRUE(log.find("Generated Driver vkMergePipelineCaches"));
    log.logger.clear();

    VkDescriptorPool var_vkdescriptorpool{};
    VkDescriptorPoolResetFlags var_vkdescriptorpoolresetflags{};
    device_disp_table.ResetDescriptorPool(dev.dev, var_vkdescriptorpool, var_vkdescriptorpoolresetflags);
    ASSERT_TRUE(log.find("Generated Layer vkResetDescriptorPool"));
    ASSERT_TRUE(log.find("Generated Driver vkResetDescriptorPool"));
    log.logger.clear();

    VkWriteDescriptorSet var_vkwritedescriptorset{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
    VkCopyDescriptorSet var_vkcopydescriptorset{VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET};
    device_disp_table.UpdateDescriptorSets(dev.dev, var_uint32_t, &var_vkwritedescriptorset, var_uint32_t, &var_vkcopydescriptorset);
    ASSERT_TRUE(log.find("Generated Layer vkUpdateDescriptorSets"));
    ASSERT_TRUE(log.find("Generated Driver vkUpdateDescriptorSets"));
    log.logger.clear();

    VkRenderPass var_vkrenderpass{};
    VkExtent2D var_vkextent2d{};
    device_disp_table.GetRenderAreaGranularity(dev.dev, var_vkrenderpass, &var_vkextent2d);
    ASSERT_TRUE(log.find("Generated Layer vkGetRenderAreaGranularity"));
    ASSERT_TRUE(log.find("Generated Driver vkGetRenderAreaGranularity"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    VkCommandPoolResetFlags var_vkcommandpoolresetflags{};
    device_disp_table.ResetCommandPool(dev.dev, var_vkcommandpool, var_vkcommandpoolresetflags);
    ASSERT_TRUE(log.find("Generated Layer vkResetCommandPool"));
    ASSERT_TRUE(log.find("Generated Driver vkResetCommandPool"));
    log.logger.clear();

    VkQueue var_vkqueue{};
    device_disp_table.GetDeviceQueue(dev.dev, var_uint32_t, var_uint32_t, &var_vkqueue);
    ASSERT_TRUE(log.find("Generated Layer vkGetDeviceQueue"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDeviceQueue"));
    log.logger.clear();

    VkSubmitInfo var_vksubmitinfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    device_disp_table.QueueSubmit(var_vkqueue, var_uint32_t, &var_vksubmitinfo, var_vkfence);
    ASSERT_TRUE(log.find("Generated Layer vkQueueSubmit"));
    ASSERT_TRUE(log.find("Generated Driver vkQueueSubmit"));
    log.logger.clear();

    device_disp_table.QueueWaitIdle(var_vkqueue);
    ASSERT_TRUE(log.find("Generated Layer vkQueueWaitIdle"));
    ASSERT_TRUE(log.find("Generated Driver vkQueueWaitIdle"));
    log.logger.clear();

    VkBindSparseInfo var_vkbindsparseinfo{VK_STRUCTURE_TYPE_BIND_SPARSE_INFO};
    device_disp_table.QueueBindSparse(var_vkqueue, var_uint32_t, &var_vkbindsparseinfo, var_vkfence);
    ASSERT_TRUE(log.find("Generated Layer vkQueueBindSparse"));
    ASSERT_TRUE(log.find("Generated Driver vkQueueBindSparse"));
    log.logger.clear();

    VkMemoryAllocateInfo var_vkmemoryallocateinfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    device_disp_table.AllocateMemory(dev.dev, &var_vkmemoryallocateinfo, nullptr, &var_vkdevicememory);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateMemory"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateMemory"));
    log.logger.clear();

    device_disp_table.FreeMemory(dev.dev, var_vkdevicememory, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkFreeMemory"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeMemory"));
    log.logger.clear();

    VkFenceCreateInfo var_vkfencecreateinfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    device_disp_table.CreateFence(dev.dev, &var_vkfencecreateinfo, nullptr, &var_vkfence);
    ASSERT_TRUE(log.find("Generated Layer vkCreateFence"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateFence"));
    log.logger.clear();

    device_disp_table.DestroyFence(dev.dev, var_vkfence, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyFence"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyFence"));
    log.logger.clear();

    VkEventCreateInfo var_vkeventcreateinfo{VK_STRUCTURE_TYPE_EVENT_CREATE_INFO};
    device_disp_table.CreateEvent(dev.dev, &var_vkeventcreateinfo, nullptr, &var_vkevent);
    ASSERT_TRUE(log.find("Generated Layer vkCreateEvent"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateEvent"));
    log.logger.clear();

    device_disp_table.DestroyEvent(dev.dev, var_vkevent, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyEvent"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyEvent"));
    log.logger.clear();

    VkQueryPoolCreateInfo var_vkquerypoolcreateinfo{VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO};
    device_disp_table.CreateQueryPool(dev.dev, &var_vkquerypoolcreateinfo, nullptr, &var_vkquerypool);
    ASSERT_TRUE(log.find("Generated Layer vkCreateQueryPool"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateQueryPool"));
    log.logger.clear();

    device_disp_table.DestroyQueryPool(dev.dev, var_vkquerypool, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyQueryPool"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyQueryPool"));
    log.logger.clear();

    VkBufferCreateInfo var_vkbuffercreateinfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    device_disp_table.CreateBuffer(dev.dev, &var_vkbuffercreateinfo, nullptr, &var_vkbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkCreateBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateBuffer"));
    log.logger.clear();

    device_disp_table.DestroyBuffer(dev.dev, var_vkbuffer, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyBuffer"));
    log.logger.clear();

    VkImageCreateInfo var_vkimagecreateinfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    device_disp_table.CreateImage(dev.dev, &var_vkimagecreateinfo, nullptr, &var_vkimage);
    ASSERT_TRUE(log.find("Generated Layer vkCreateImage"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateImage"));
    log.logger.clear();

    device_disp_table.DestroyImage(dev.dev, var_vkimage, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyImage"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyImage"));
    log.logger.clear();

    VkPipelineCacheCreateInfo var_vkpipelinecachecreateinfo{VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO};
    device_disp_table.CreatePipelineCache(dev.dev, &var_vkpipelinecachecreateinfo, nullptr, &var_vkpipelinecache);
    ASSERT_TRUE(log.find("Generated Layer vkCreatePipelineCache"));
    ASSERT_TRUE(log.find("Generated Driver vkCreatePipelineCache"));
    log.logger.clear();

    device_disp_table.DestroyPipelineCache(dev.dev, var_vkpipelinecache, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyPipelineCache"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyPipelineCache"));
    log.logger.clear();

    VkGraphicsPipelineCreateInfo var_vkgraphicspipelinecreateinfo{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    VkPipeline var_vkpipeline{};
    device_disp_table.CreateGraphicsPipelines(dev.dev, var_vkpipelinecache, var_uint32_t, &var_vkgraphicspipelinecreateinfo, nullptr, &var_vkpipeline);
    ASSERT_TRUE(log.find("Generated Layer vkCreateGraphicsPipelines"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateGraphicsPipelines"));
    log.logger.clear();

    device_disp_table.DestroyPipeline(dev.dev, var_vkpipeline, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyPipeline"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyPipeline"));
    log.logger.clear();

    VkPipelineLayoutCreateInfo var_vkpipelinelayoutcreateinfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    VkPipelineLayout var_vkpipelinelayout{};
    device_disp_table.CreatePipelineLayout(dev.dev, &var_vkpipelinelayoutcreateinfo, nullptr, &var_vkpipelinelayout);
    ASSERT_TRUE(log.find("Generated Layer vkCreatePipelineLayout"));
    ASSERT_TRUE(log.find("Generated Driver vkCreatePipelineLayout"));
    log.logger.clear();

    device_disp_table.DestroyPipelineLayout(dev.dev, var_vkpipelinelayout, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyPipelineLayout"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyPipelineLayout"));
    log.logger.clear();

    VkDescriptorPoolCreateInfo var_vkdescriptorpoolcreateinfo{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    device_disp_table.CreateDescriptorPool(dev.dev, &var_vkdescriptorpoolcreateinfo, nullptr, &var_vkdescriptorpool);
    ASSERT_TRUE(log.find("Generated Layer vkCreateDescriptorPool"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateDescriptorPool"));
    log.logger.clear();

    device_disp_table.DestroyDescriptorPool(dev.dev, var_vkdescriptorpool, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyDescriptorPool"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyDescriptorPool"));
    log.logger.clear();

    VkDescriptorSetAllocateInfo var_vkdescriptorsetallocateinfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    VkDescriptorSet var_vkdescriptorset{};
    device_disp_table.AllocateDescriptorSets(dev.dev, &var_vkdescriptorsetallocateinfo, &var_vkdescriptorset);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateDescriptorSets"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateDescriptorSets"));
    log.logger.clear();

    device_disp_table.FreeDescriptorSets(dev.dev, var_vkdescriptorpool, var_uint32_t, &var_vkdescriptorset);
    ASSERT_TRUE(log.find("Generated Layer vkFreeDescriptorSets"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeDescriptorSets"));
    log.logger.clear();

    VkRenderPassCreateInfo var_vkrenderpasscreateinfo{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    device_disp_table.CreateRenderPass(dev.dev, &var_vkrenderpasscreateinfo, nullptr, &var_vkrenderpass);
    ASSERT_TRUE(log.find("Generated Layer vkCreateRenderPass"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateRenderPass"));
    log.logger.clear();

    device_disp_table.DestroyRenderPass(dev.dev, var_vkrenderpass, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyRenderPass"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyRenderPass"));
    log.logger.clear();

    VkCommandPoolCreateInfo var_vkcommandpoolcreateinfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    device_disp_table.CreateCommandPool(dev.dev, &var_vkcommandpoolcreateinfo, nullptr, &var_vkcommandpool);
    ASSERT_TRUE(log.find("Generated Layer vkCreateCommandPool"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateCommandPool"));
    log.logger.clear();

    device_disp_table.DestroyCommandPool(dev.dev, var_vkcommandpool, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyCommandPool"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyCommandPool"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkCommandBufferBeginInfo var_vkcommandbufferbegininfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    device_disp_table.BeginCommandBuffer(var_vkcommandbuffer, &var_vkcommandbufferbegininfo);
    ASSERT_TRUE(log.find("Generated Layer vkBeginCommandBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkBeginCommandBuffer"));
    log.logger.clear();

    device_disp_table.EndCommandBuffer(var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkEndCommandBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkEndCommandBuffer"));
    log.logger.clear();

    VkCommandBufferResetFlags var_vkcommandbufferresetflags{};
    device_disp_table.ResetCommandBuffer(var_vkcommandbuffer, var_vkcommandbufferresetflags);
    ASSERT_TRUE(log.find("Generated Layer vkResetCommandBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkResetCommandBuffer"));
    log.logger.clear();

    VkPipelineBindPoint var_vkpipelinebindpoint{};
    device_disp_table.CmdBindPipeline(var_vkcommandbuffer, var_vkpipelinebindpoint, var_vkpipeline);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBindPipeline"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBindPipeline"));
    log.logger.clear();

    VkViewport var_vkviewport{};
    device_disp_table.CmdSetViewport(var_vkcommandbuffer, var_uint32_t, var_uint32_t, &var_vkviewport);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetViewport"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetViewport"));
    log.logger.clear();

    VkRect2D var_vkrect2d{};
    device_disp_table.CmdSetScissor(var_vkcommandbuffer, var_uint32_t, var_uint32_t, &var_vkrect2d);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetScissor"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetScissor"));
    log.logger.clear();

    float var_float{};
    device_disp_table.CmdSetLineWidth(var_vkcommandbuffer, var_float);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetLineWidth"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetLineWidth"));
    log.logger.clear();

    device_disp_table.CmdSetDepthBias(var_vkcommandbuffer, var_float, var_float, var_float);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetDepthBias"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetDepthBias"));
    log.logger.clear();

    float var_float_1d_4[4]{};
    device_disp_table.CmdSetBlendConstants(var_vkcommandbuffer, var_float_1d_4);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetBlendConstants"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetBlendConstants"));
    log.logger.clear();

    device_disp_table.CmdSetDepthBounds(var_vkcommandbuffer, var_float, var_float);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetDepthBounds"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetDepthBounds"));
    log.logger.clear();

    VkStencilFaceFlags var_vkstencilfaceflags{};
    device_disp_table.CmdSetStencilCompareMask(var_vkcommandbuffer, var_vkstencilfaceflags, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetStencilCompareMask"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetStencilCompareMask"));
    log.logger.clear();

    device_disp_table.CmdSetStencilWriteMask(var_vkcommandbuffer, var_vkstencilfaceflags, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetStencilWriteMask"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetStencilWriteMask"));
    log.logger.clear();

    device_disp_table.CmdSetStencilReference(var_vkcommandbuffer, var_vkstencilfaceflags, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetStencilReference"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetStencilReference"));
    log.logger.clear();

    device_disp_table.CmdBindDescriptorSets(var_vkcommandbuffer, var_vkpipelinebindpoint, var_vkpipelinelayout, var_uint32_t, var_uint32_t, &var_vkdescriptorset, var_uint32_t, &var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBindDescriptorSets"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBindDescriptorSets"));
    log.logger.clear();

    VkIndexType var_vkindextype{};
    device_disp_table.CmdBindIndexBuffer(var_vkcommandbuffer, var_vkbuffer, var_vkdevicesize, var_vkindextype);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBindIndexBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBindIndexBuffer"));
    log.logger.clear();

    device_disp_table.CmdBindVertexBuffers(var_vkcommandbuffer, var_uint32_t, var_uint32_t, &var_vkbuffer, &var_vkdevicesize);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBindVertexBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBindVertexBuffers"));
    log.logger.clear();

    device_disp_table.CmdDraw(var_vkcommandbuffer, var_uint32_t, var_uint32_t, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDraw"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDraw"));
    log.logger.clear();

    int32_t var_int32_t{};
    device_disp_table.CmdDrawIndexed(var_vkcommandbuffer, var_uint32_t, var_uint32_t, var_uint32_t, var_int32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDrawIndexed"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDrawIndexed"));
    log.logger.clear();

    device_disp_table.CmdDrawIndirect(var_vkcommandbuffer, var_vkbuffer, var_vkdevicesize, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDrawIndirect"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDrawIndirect"));
    log.logger.clear();

    device_disp_table.CmdDrawIndexedIndirect(var_vkcommandbuffer, var_vkbuffer, var_vkdevicesize, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDrawIndexedIndirect"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDrawIndexedIndirect"));
    log.logger.clear();

    device_disp_table.CmdDispatch(var_vkcommandbuffer, var_uint32_t, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDispatch"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDispatch"));
    log.logger.clear();

    device_disp_table.CmdDispatchIndirect(var_vkcommandbuffer, var_vkbuffer, var_vkdevicesize);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDispatchIndirect"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDispatchIndirect"));
    log.logger.clear();

    VkBufferCopy var_vkbuffercopy{};
    device_disp_table.CmdCopyBuffer(var_vkcommandbuffer, var_vkbuffer, var_vkbuffer, var_uint32_t, &var_vkbuffercopy);
    ASSERT_TRUE(log.find("Generated Layer vkCmdCopyBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdCopyBuffer"));
    log.logger.clear();

    VkImageLayout var_vkimagelayout{};
    VkImageCopy var_vkimagecopy{};
    device_disp_table.CmdCopyImage(var_vkcommandbuffer, var_vkimage, var_vkimagelayout, var_vkimage, var_vkimagelayout, var_uint32_t, &var_vkimagecopy);
    ASSERT_TRUE(log.find("Generated Layer vkCmdCopyImage"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdCopyImage"));
    log.logger.clear();

    VkImageBlit var_vkimageblit{};
    VkFilter var_vkfilter{};
    device_disp_table.CmdBlitImage(var_vkcommandbuffer, var_vkimage, var_vkimagelayout, var_vkimage, var_vkimagelayout, var_uint32_t, &var_vkimageblit, var_vkfilter);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBlitImage"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBlitImage"));
    log.logger.clear();

    VkBufferImageCopy var_vkbufferimagecopy{};
    device_disp_table.CmdCopyBufferToImage(var_vkcommandbuffer, var_vkbuffer, var_vkimage, var_vkimagelayout, var_uint32_t, &var_vkbufferimagecopy);
    ASSERT_TRUE(log.find("Generated Layer vkCmdCopyBufferToImage"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdCopyBufferToImage"));
    log.logger.clear();

    device_disp_table.CmdCopyImageToBuffer(var_vkcommandbuffer, var_vkimage, var_vkimagelayout, var_vkbuffer, var_uint32_t, &var_vkbufferimagecopy);
    ASSERT_TRUE(log.find("Generated Layer vkCmdCopyImageToBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdCopyImageToBuffer"));
    log.logger.clear();

    device_disp_table.CmdUpdateBuffer(var_vkcommandbuffer, var_vkbuffer, var_vkdevicesize, var_vkdevicesize, reinterpret_cast<void**>(big_chunk_of_mem));
    ASSERT_TRUE(log.find("Generated Layer vkCmdUpdateBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdUpdateBuffer"));
    log.logger.clear();

    device_disp_table.CmdFillBuffer(var_vkcommandbuffer, var_vkbuffer, var_vkdevicesize, var_vkdevicesize, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdFillBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdFillBuffer"));
    log.logger.clear();

    VkClearColorValue var_vkclearcolorvalue{};
    VkImageSubresourceRange var_vkimagesubresourcerange{};
    device_disp_table.CmdClearColorImage(var_vkcommandbuffer, var_vkimage, var_vkimagelayout, &var_vkclearcolorvalue, var_uint32_t, &var_vkimagesubresourcerange);
    ASSERT_TRUE(log.find("Generated Layer vkCmdClearColorImage"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdClearColorImage"));
    log.logger.clear();

    VkClearDepthStencilValue var_vkcleardepthstencilvalue{};
    device_disp_table.CmdClearDepthStencilImage(var_vkcommandbuffer, var_vkimage, var_vkimagelayout, &var_vkcleardepthstencilvalue, var_uint32_t, &var_vkimagesubresourcerange);
    ASSERT_TRUE(log.find("Generated Layer vkCmdClearDepthStencilImage"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdClearDepthStencilImage"));
    log.logger.clear();

    VkClearAttachment var_vkclearattachment{};
    VkClearRect var_vkclearrect{};
    device_disp_table.CmdClearAttachments(var_vkcommandbuffer, var_uint32_t, &var_vkclearattachment, var_uint32_t, &var_vkclearrect);
    ASSERT_TRUE(log.find("Generated Layer vkCmdClearAttachments"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdClearAttachments"));
    log.logger.clear();

    VkImageResolve var_vkimageresolve{};
    device_disp_table.CmdResolveImage(var_vkcommandbuffer, var_vkimage, var_vkimagelayout, var_vkimage, var_vkimagelayout, var_uint32_t, &var_vkimageresolve);
    ASSERT_TRUE(log.find("Generated Layer vkCmdResolveImage"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdResolveImage"));
    log.logger.clear();

    VkPipelineStageFlags var_vkpipelinestageflags{};
    device_disp_table.CmdSetEvent(var_vkcommandbuffer, var_vkevent, var_vkpipelinestageflags);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetEvent"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetEvent"));
    log.logger.clear();

    device_disp_table.CmdResetEvent(var_vkcommandbuffer, var_vkevent, var_vkpipelinestageflags);
    ASSERT_TRUE(log.find("Generated Layer vkCmdResetEvent"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdResetEvent"));
    log.logger.clear();

    VkMemoryBarrier var_vkmemorybarrier{VK_STRUCTURE_TYPE_MEMORY_BARRIER};
    VkBufferMemoryBarrier var_vkbuffermemorybarrier{VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};
    VkImageMemoryBarrier var_vkimagememorybarrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    device_disp_table.CmdWaitEvents(var_vkcommandbuffer, var_uint32_t, &var_vkevent, var_vkpipelinestageflags, var_vkpipelinestageflags, var_uint32_t, &var_vkmemorybarrier, var_uint32_t, &var_vkbuffermemorybarrier, var_uint32_t, &var_vkimagememorybarrier);
    ASSERT_TRUE(log.find("Generated Layer vkCmdWaitEvents"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdWaitEvents"));
    log.logger.clear();

    VkDependencyFlags var_vkdependencyflags{};
    device_disp_table.CmdPipelineBarrier(var_vkcommandbuffer, var_vkpipelinestageflags, var_vkpipelinestageflags, var_vkdependencyflags, var_uint32_t, &var_vkmemorybarrier, var_uint32_t, &var_vkbuffermemorybarrier, var_uint32_t, &var_vkimagememorybarrier);
    ASSERT_TRUE(log.find("Generated Layer vkCmdPipelineBarrier"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdPipelineBarrier"));
    log.logger.clear();

    VkQueryControlFlags var_vkquerycontrolflags{};
    device_disp_table.CmdBeginQuery(var_vkcommandbuffer, var_vkquerypool, var_uint32_t, var_vkquerycontrolflags);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBeginQuery"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBeginQuery"));
    log.logger.clear();

    device_disp_table.CmdEndQuery(var_vkcommandbuffer, var_vkquerypool, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdEndQuery"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdEndQuery"));
    log.logger.clear();

    device_disp_table.CmdResetQueryPool(var_vkcommandbuffer, var_vkquerypool, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdResetQueryPool"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdResetQueryPool"));
    log.logger.clear();

    VkPipelineStageFlagBits var_vkpipelinestageflagbits{};
    device_disp_table.CmdWriteTimestamp(var_vkcommandbuffer, var_vkpipelinestageflagbits, var_vkquerypool, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdWriteTimestamp"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdWriteTimestamp"));
    log.logger.clear();

    device_disp_table.CmdCopyQueryPoolResults(var_vkcommandbuffer, var_vkquerypool, var_uint32_t, var_uint32_t, var_vkbuffer, var_vkdevicesize, var_vkdevicesize, var_vkqueryresultflags);
    ASSERT_TRUE(log.find("Generated Layer vkCmdCopyQueryPoolResults"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdCopyQueryPoolResults"));
    log.logger.clear();

    VkShaderStageFlags var_vkshaderstageflags{};
    device_disp_table.CmdPushConstants(var_vkcommandbuffer, var_vkpipelinelayout, var_vkshaderstageflags, var_uint32_t, var_uint32_t, reinterpret_cast<void**>(big_chunk_of_mem));
    ASSERT_TRUE(log.find("Generated Layer vkCmdPushConstants"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdPushConstants"));
    log.logger.clear();

    VkRenderPassBeginInfo var_vkrenderpassbegininfo{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    VkSubpassContents var_vksubpasscontents{};
    device_disp_table.CmdBeginRenderPass(var_vkcommandbuffer, &var_vkrenderpassbegininfo, var_vksubpasscontents);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBeginRenderPass"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBeginRenderPass"));
    log.logger.clear();

    device_disp_table.CmdNextSubpass(var_vkcommandbuffer, var_vksubpasscontents);
    ASSERT_TRUE(log.find("Generated Layer vkCmdNextSubpass"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdNextSubpass"));
    log.logger.clear();

    device_disp_table.CmdEndRenderPass(var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkCmdEndRenderPass"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdEndRenderPass"));
    log.logger.clear();

    device_disp_table.CmdExecuteCommands(var_vkcommandbuffer, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkCmdExecuteCommands"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdExecuteCommands"));
    log.logger.clear();

    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for Vulkan Core 1.0

// Test for Vulkan Core 1.1
TEST(BasicEntrypointTest, VulkanCore_1_1) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_1;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkFormat var_vkformat{};
    VkFormatProperties2 var_vkformatproperties2{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2};
    inst_disp_table.GetPhysicalDeviceFormatProperties2(var_vkphysicaldevice, var_vkformat, &var_vkformatproperties2);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceFormatProperties2"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceFormatProperties2"));
    log.logger.clear();

    VkPhysicalDeviceSparseImageFormatInfo2 var_vkphysicaldevicesparseimageformatinfo2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SPARSE_IMAGE_FORMAT_INFO_2};
    VkSparseImageFormatProperties2 var_vksparseimageformatproperties2{VK_STRUCTURE_TYPE_SPARSE_IMAGE_FORMAT_PROPERTIES_2};
    inst_disp_table.GetPhysicalDeviceSparseImageFormatProperties2(var_vkphysicaldevice, &var_vkphysicaldevicesparseimageformatinfo2, &var_uint32_t, &var_vksparseimageformatproperties2);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceSparseImageFormatProperties2"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceSparseImageFormatProperties2"));
    log.logger.clear();

    VkPhysicalDeviceExternalBufferInfo var_vkphysicaldeviceexternalbufferinfo{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_BUFFER_INFO};
    VkExternalBufferProperties var_vkexternalbufferproperties{VK_STRUCTURE_TYPE_EXTERNAL_BUFFER_PROPERTIES};
    inst_disp_table.GetPhysicalDeviceExternalBufferProperties(var_vkphysicaldevice, &var_vkphysicaldeviceexternalbufferinfo, &var_vkexternalbufferproperties);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceExternalBufferProperties"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceExternalBufferProperties"));
    log.logger.clear();

    VkPhysicalDeviceExternalFenceInfo var_vkphysicaldeviceexternalfenceinfo{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_FENCE_INFO};
    VkExternalFenceProperties var_vkexternalfenceproperties{VK_STRUCTURE_TYPE_EXTERNAL_FENCE_PROPERTIES};
    inst_disp_table.GetPhysicalDeviceExternalFenceProperties(var_vkphysicaldevice, &var_vkphysicaldeviceexternalfenceinfo, &var_vkexternalfenceproperties);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceExternalFenceProperties"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceExternalFenceProperties"));
    log.logger.clear();

    VkPhysicalDeviceExternalSemaphoreInfo var_vkphysicaldeviceexternalsemaphoreinfo{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_SEMAPHORE_INFO};
    VkExternalSemaphoreProperties var_vkexternalsemaphoreproperties{VK_STRUCTURE_TYPE_EXTERNAL_SEMAPHORE_PROPERTIES};
    inst_disp_table.GetPhysicalDeviceExternalSemaphoreProperties(var_vkphysicaldevice, &var_vkphysicaldeviceexternalsemaphoreinfo, &var_vkexternalsemaphoreproperties);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceExternalSemaphoreProperties"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceExternalSemaphoreProperties"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkBindBufferMemoryInfo var_vkbindbuffermemoryinfo{VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO};
    device_disp_table.BindBufferMemory2(dev.dev, var_uint32_t, &var_vkbindbuffermemoryinfo);
    ASSERT_TRUE(log.find("Generated Layer vkBindBufferMemory2"));
    ASSERT_TRUE(log.find("Generated Driver vkBindBufferMemory2"));
    log.logger.clear();

    VkBindImageMemoryInfo var_vkbindimagememoryinfo{VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO};
    device_disp_table.BindImageMemory2(dev.dev, var_uint32_t, &var_vkbindimagememoryinfo);
    ASSERT_TRUE(log.find("Generated Layer vkBindImageMemory2"));
    ASSERT_TRUE(log.find("Generated Driver vkBindImageMemory2"));
    log.logger.clear();

    VkPeerMemoryFeatureFlags var_vkpeermemoryfeatureflags{};
    device_disp_table.GetDeviceGroupPeerMemoryFeatures(dev.dev, var_uint32_t, var_uint32_t, var_uint32_t, &var_vkpeermemoryfeatureflags);
    ASSERT_TRUE(log.find("Generated Layer vkGetDeviceGroupPeerMemoryFeatures"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDeviceGroupPeerMemoryFeatures"));
    log.logger.clear();

    VkImageMemoryRequirementsInfo2 var_vkimagememoryrequirementsinfo2{VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2};
    VkMemoryRequirements2 var_vkmemoryrequirements2{VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2};
    device_disp_table.GetImageMemoryRequirements2(dev.dev, &var_vkimagememoryrequirementsinfo2, &var_vkmemoryrequirements2);
    ASSERT_TRUE(log.find("Generated Layer vkGetImageMemoryRequirements2"));
    ASSERT_TRUE(log.find("Generated Driver vkGetImageMemoryRequirements2"));
    log.logger.clear();

    VkBufferMemoryRequirementsInfo2 var_vkbuffermemoryrequirementsinfo2{VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2};
    device_disp_table.GetBufferMemoryRequirements2(dev.dev, &var_vkbuffermemoryrequirementsinfo2, &var_vkmemoryrequirements2);
    ASSERT_TRUE(log.find("Generated Layer vkGetBufferMemoryRequirements2"));
    ASSERT_TRUE(log.find("Generated Driver vkGetBufferMemoryRequirements2"));
    log.logger.clear();

    VkImageSparseMemoryRequirementsInfo2 var_vkimagesparsememoryrequirementsinfo2{VK_STRUCTURE_TYPE_IMAGE_SPARSE_MEMORY_REQUIREMENTS_INFO_2};
    VkSparseImageMemoryRequirements2 var_vksparseimagememoryrequirements2{VK_STRUCTURE_TYPE_SPARSE_IMAGE_MEMORY_REQUIREMENTS_2};
    device_disp_table.GetImageSparseMemoryRequirements2(dev.dev, &var_vkimagesparsememoryrequirementsinfo2, &var_uint32_t, &var_vksparseimagememoryrequirements2);
    ASSERT_TRUE(log.find("Generated Layer vkGetImageSparseMemoryRequirements2"));
    ASSERT_TRUE(log.find("Generated Driver vkGetImageSparseMemoryRequirements2"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    VkCommandPoolTrimFlags var_vkcommandpooltrimflags{};
    device_disp_table.TrimCommandPool(dev.dev, var_vkcommandpool, var_vkcommandpooltrimflags);
    ASSERT_TRUE(log.find("Generated Layer vkTrimCommandPool"));
    ASSERT_TRUE(log.find("Generated Driver vkTrimCommandPool"));
    log.logger.clear();

    VkDescriptorSet var_vkdescriptorset{};
    VkDescriptorUpdateTemplate var_vkdescriptorupdatetemplate{};
    uint64_t big_chunk_of_mem[512][512];

    device_disp_table.UpdateDescriptorSetWithTemplate(dev.dev, var_vkdescriptorset, var_vkdescriptorupdatetemplate, reinterpret_cast<void**>(big_chunk_of_mem));
    ASSERT_TRUE(log.find("Generated Layer vkUpdateDescriptorSetWithTemplate"));
    ASSERT_TRUE(log.find("Generated Driver vkUpdateDescriptorSetWithTemplate"));
    log.logger.clear();

    VkDescriptorSetAllocateInfo var_vkdescriptorsetallocateinfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    device_disp_table.AllocateDescriptorSets(dev.dev, &var_vkdescriptorsetallocateinfo, &var_vkdescriptorset);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateDescriptorSets"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateDescriptorSets"));
    log.logger.clear();

    VkDescriptorPool var_vkdescriptorpool{};
    device_disp_table.FreeDescriptorSets(dev.dev, var_vkdescriptorpool, var_uint32_t, &var_vkdescriptorset);
    ASSERT_TRUE(log.find("Generated Layer vkFreeDescriptorSets"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeDescriptorSets"));
    log.logger.clear();

    VkCommandPoolCreateInfo var_vkcommandpoolcreateinfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    device_disp_table.CreateCommandPool(dev.dev, &var_vkcommandpoolcreateinfo, nullptr, &var_vkcommandpool);
    ASSERT_TRUE(log.find("Generated Layer vkCreateCommandPool"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateCommandPool"));
    log.logger.clear();

    device_disp_table.DestroyCommandPool(dev.dev, var_vkcommandpool, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyCommandPool"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyCommandPool"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    device_disp_table.CmdSetDeviceMask(var_vkcommandbuffer, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetDeviceMask"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetDeviceMask"));
    log.logger.clear();

    device_disp_table.CmdDispatchBase(var_vkcommandbuffer, var_uint32_t, var_uint32_t, var_uint32_t, var_uint32_t, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDispatchBase"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDispatchBase"));
    log.logger.clear();

    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

    VkDescriptorUpdateTemplateCreateInfo var_vkdescriptorupdatetemplatecreateinfo{VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO};
    device_disp_table.CreateDescriptorUpdateTemplate(dev.dev, &var_vkdescriptorupdatetemplatecreateinfo, nullptr, &var_vkdescriptorupdatetemplate);
    ASSERT_TRUE(log.find("Generated Layer vkCreateDescriptorUpdateTemplate"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateDescriptorUpdateTemplate"));
    log.logger.clear();

    device_disp_table.DestroyDescriptorUpdateTemplate(dev.dev, var_vkdescriptorupdatetemplate, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyDescriptorUpdateTemplate"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyDescriptorUpdateTemplate"));
    log.logger.clear();

} // Test for Vulkan Core 1.1

// Test for Vulkan Core 1.2
TEST(BasicEntrypointTest, VulkanCore_1_2) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_2;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkQueryPool var_vkquerypool{};
    device_disp_table.ResetQueryPool(dev.dev, var_vkquerypool, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkResetQueryPool"));
    ASSERT_TRUE(log.find("Generated Driver vkResetQueryPool"));
    log.logger.clear();

    VkSemaphore var_vksemaphore{};
    uint64_t var_uint64_t = 1;
    device_disp_table.GetSemaphoreCounterValue(dev.dev, var_vksemaphore, &var_uint64_t);
    ASSERT_TRUE(log.find("Generated Layer vkGetSemaphoreCounterValue"));
    ASSERT_TRUE(log.find("Generated Driver vkGetSemaphoreCounterValue"));
    log.logger.clear();

    VkSemaphoreWaitInfo var_vksemaphorewaitinfo{VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO};
    device_disp_table.WaitSemaphores(dev.dev, &var_vksemaphorewaitinfo, var_uint64_t);
    ASSERT_TRUE(log.find("Generated Layer vkWaitSemaphores"));
    ASSERT_TRUE(log.find("Generated Driver vkWaitSemaphores"));
    log.logger.clear();

    VkSemaphoreSignalInfo var_vksemaphoresignalinfo{VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO};
    device_disp_table.SignalSemaphore(dev.dev, &var_vksemaphoresignalinfo);
    ASSERT_TRUE(log.find("Generated Layer vkSignalSemaphore"));
    ASSERT_TRUE(log.find("Generated Driver vkSignalSemaphore"));
    log.logger.clear();

    VkBufferDeviceAddressInfo var_vkbufferdeviceaddressinfo{VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO};
    device_disp_table.GetBufferDeviceAddress(dev.dev, &var_vkbufferdeviceaddressinfo);
    ASSERT_TRUE(log.find("Generated Layer vkGetBufferDeviceAddress"));
    ASSERT_TRUE(log.find("Generated Driver vkGetBufferDeviceAddress"));
    log.logger.clear();

    device_disp_table.GetBufferOpaqueCaptureAddress(dev.dev, &var_vkbufferdeviceaddressinfo);
    ASSERT_TRUE(log.find("Generated Layer vkGetBufferOpaqueCaptureAddress"));
    ASSERT_TRUE(log.find("Generated Driver vkGetBufferOpaqueCaptureAddress"));
    log.logger.clear();

    VkDeviceMemoryOpaqueCaptureAddressInfo var_vkdevicememoryopaquecaptureaddressinfo{VK_STRUCTURE_TYPE_DEVICE_MEMORY_OPAQUE_CAPTURE_ADDRESS_INFO};
    device_disp_table.GetDeviceMemoryOpaqueCaptureAddress(dev.dev, &var_vkdevicememoryopaquecaptureaddressinfo);
    ASSERT_TRUE(log.find("Generated Layer vkGetDeviceMemoryOpaqueCaptureAddress"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDeviceMemoryOpaqueCaptureAddress"));
    log.logger.clear();

    VkSemaphoreCreateInfo var_vksemaphorecreateinfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    device_disp_table.CreateSemaphore(dev.dev, &var_vksemaphorecreateinfo, nullptr, &var_vksemaphore);
    ASSERT_TRUE(log.find("Generated Layer vkCreateSemaphore"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateSemaphore"));
    log.logger.clear();

    device_disp_table.DestroySemaphore(dev.dev, var_vksemaphore, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySemaphore"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySemaphore"));
    log.logger.clear();

    VkQueryPoolCreateInfo var_vkquerypoolcreateinfo{VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO};
    device_disp_table.CreateQueryPool(dev.dev, &var_vkquerypoolcreateinfo, nullptr, &var_vkquerypool);
    ASSERT_TRUE(log.find("Generated Layer vkCreateQueryPool"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateQueryPool"));
    log.logger.clear();

    device_disp_table.DestroyQueryPool(dev.dev, var_vkquerypool, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyQueryPool"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyQueryPool"));
    log.logger.clear();

    VkBufferCreateInfo var_vkbuffercreateinfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    VkBuffer var_vkbuffer{};
    device_disp_table.CreateBuffer(dev.dev, &var_vkbuffercreateinfo, nullptr, &var_vkbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkCreateBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateBuffer"));
    log.logger.clear();

    device_disp_table.DestroyBuffer(dev.dev, var_vkbuffer, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyBuffer"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkDeviceSize var_vkdevicesize{};
    device_disp_table.CmdDrawIndirectCount(var_vkcommandbuffer, var_vkbuffer, var_vkdevicesize, var_vkbuffer, var_vkdevicesize, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDrawIndirectCount"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDrawIndirectCount"));
    log.logger.clear();

    device_disp_table.CmdDrawIndexedIndirectCount(var_vkcommandbuffer, var_vkbuffer, var_vkdevicesize, var_vkbuffer, var_vkdevicesize, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDrawIndexedIndirectCount"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDrawIndexedIndirectCount"));
    log.logger.clear();

    VkRenderPassBeginInfo var_vkrenderpassbegininfo{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    VkSubpassBeginInfo var_vksubpassbegininfo{VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO};
    device_disp_table.CmdBeginRenderPass2(var_vkcommandbuffer, &var_vkrenderpassbegininfo, &var_vksubpassbegininfo);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBeginRenderPass2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBeginRenderPass2"));
    log.logger.clear();

    VkSubpassEndInfo var_vksubpassendinfo{VK_STRUCTURE_TYPE_SUBPASS_END_INFO};
    device_disp_table.CmdNextSubpass2(var_vkcommandbuffer, &var_vksubpassbegininfo, &var_vksubpassendinfo);
    ASSERT_TRUE(log.find("Generated Layer vkCmdNextSubpass2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdNextSubpass2"));
    log.logger.clear();

    device_disp_table.CmdEndRenderPass2(var_vkcommandbuffer, &var_vksubpassendinfo);
    ASSERT_TRUE(log.find("Generated Layer vkCmdEndRenderPass2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdEndRenderPass2"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for Vulkan Core 1.2

// Test for Vulkan Core 1.3
TEST(BasicEntrypointTest, VulkanCore_1_3) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkObjectType var_vkobjecttype{};
    uint64_t var_uint64_t = 1;
    VkPrivateDataSlot var_vkprivatedataslot{};
    device_disp_table.SetPrivateData(dev.dev, var_vkobjecttype, var_uint64_t, var_vkprivatedataslot, var_uint64_t);
    ASSERT_TRUE(log.find("Generated Layer vkSetPrivateData"));
    ASSERT_TRUE(log.find("Generated Driver vkSetPrivateData"));
    log.logger.clear();

    device_disp_table.GetPrivateData(dev.dev, var_vkobjecttype, var_uint64_t, var_vkprivatedataslot, &var_uint64_t);
    ASSERT_TRUE(log.find("Generated Layer vkGetPrivateData"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPrivateData"));
    log.logger.clear();

    VkDeviceBufferMemoryRequirements var_vkdevicebuffermemoryrequirements{VK_STRUCTURE_TYPE_DEVICE_BUFFER_MEMORY_REQUIREMENTS};
    VkMemoryRequirements2 var_vkmemoryrequirements2{VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2};
    device_disp_table.GetDeviceBufferMemoryRequirements(dev.dev, &var_vkdevicebuffermemoryrequirements, &var_vkmemoryrequirements2);
    ASSERT_TRUE(log.find("Generated Layer vkGetDeviceBufferMemoryRequirements"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDeviceBufferMemoryRequirements"));
    log.logger.clear();

    VkDeviceImageMemoryRequirements var_vkdeviceimagememoryrequirements{VK_STRUCTURE_TYPE_DEVICE_IMAGE_MEMORY_REQUIREMENTS};
    device_disp_table.GetDeviceImageMemoryRequirements(dev.dev, &var_vkdeviceimagememoryrequirements, &var_vkmemoryrequirements2);
    ASSERT_TRUE(log.find("Generated Layer vkGetDeviceImageMemoryRequirements"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDeviceImageMemoryRequirements"));
    log.logger.clear();

    VkSparseImageMemoryRequirements2 var_vksparseimagememoryrequirements2{VK_STRUCTURE_TYPE_SPARSE_IMAGE_MEMORY_REQUIREMENTS_2};
    device_disp_table.GetDeviceImageSparseMemoryRequirements(dev.dev, &var_vkdeviceimagememoryrequirements, &var_uint32_t, &var_vksparseimagememoryrequirements2);
    ASSERT_TRUE(log.find("Generated Layer vkGetDeviceImageSparseMemoryRequirements"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDeviceImageSparseMemoryRequirements"));
    log.logger.clear();

    VkDeviceQueueInfo2 var_vkdevicequeueinfo2{VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2};
    VkQueue var_vkqueue{};
    device_disp_table.GetDeviceQueue2(dev.dev, &var_vkdevicequeueinfo2, &var_vkqueue);
    ASSERT_TRUE(log.find("Generated Layer vkGetDeviceQueue2"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDeviceQueue2"));
    log.logger.clear();

    VkSubmitInfo2 var_vksubmitinfo2{VK_STRUCTURE_TYPE_SUBMIT_INFO_2};
    VkFence var_vkfence{};
    device_disp_table.QueueSubmit2(var_vkqueue, var_uint32_t, &var_vksubmitinfo2, var_vkfence);
    ASSERT_TRUE(log.find("Generated Layer vkQueueSubmit2"));
    ASSERT_TRUE(log.find("Generated Driver vkQueueSubmit2"));
    log.logger.clear();

    VkFenceCreateInfo var_vkfencecreateinfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    device_disp_table.CreateFence(dev.dev, &var_vkfencecreateinfo, nullptr, &var_vkfence);
    ASSERT_TRUE(log.find("Generated Layer vkCreateFence"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateFence"));
    log.logger.clear();

    device_disp_table.DestroyFence(dev.dev, var_vkfence, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyFence"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyFence"));
    log.logger.clear();

    VkEventCreateInfo var_vkeventcreateinfo{VK_STRUCTURE_TYPE_EVENT_CREATE_INFO};
    VkEvent var_vkevent{};
    device_disp_table.CreateEvent(dev.dev, &var_vkeventcreateinfo, nullptr, &var_vkevent);
    ASSERT_TRUE(log.find("Generated Layer vkCreateEvent"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateEvent"));
    log.logger.clear();

    device_disp_table.DestroyEvent(dev.dev, var_vkevent, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyEvent"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyEvent"));
    log.logger.clear();

    VkQueryPoolCreateInfo var_vkquerypoolcreateinfo{VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO};
    VkQueryPool var_vkquerypool{};
    device_disp_table.CreateQueryPool(dev.dev, &var_vkquerypoolcreateinfo, nullptr, &var_vkquerypool);
    ASSERT_TRUE(log.find("Generated Layer vkCreateQueryPool"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateQueryPool"));
    log.logger.clear();

    device_disp_table.DestroyQueryPool(dev.dev, var_vkquerypool, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyQueryPool"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyQueryPool"));
    log.logger.clear();

    VkBufferCreateInfo var_vkbuffercreateinfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    VkBuffer var_vkbuffer{};
    device_disp_table.CreateBuffer(dev.dev, &var_vkbuffercreateinfo, nullptr, &var_vkbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkCreateBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateBuffer"));
    log.logger.clear();

    device_disp_table.DestroyBuffer(dev.dev, var_vkbuffer, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyBuffer"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkDependencyInfo var_vkdependencyinfo{VK_STRUCTURE_TYPE_DEPENDENCY_INFO};
    device_disp_table.CmdSetEvent2(var_vkcommandbuffer, var_vkevent, &var_vkdependencyinfo);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetEvent2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetEvent2"));
    log.logger.clear();

    VkPipelineStageFlags2 var_vkpipelinestageflags2{};
    device_disp_table.CmdResetEvent2(var_vkcommandbuffer, var_vkevent, var_vkpipelinestageflags2);
    ASSERT_TRUE(log.find("Generated Layer vkCmdResetEvent2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdResetEvent2"));
    log.logger.clear();

    device_disp_table.CmdWaitEvents2(var_vkcommandbuffer, var_uint32_t, &var_vkevent, &var_vkdependencyinfo);
    ASSERT_TRUE(log.find("Generated Layer vkCmdWaitEvents2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdWaitEvents2"));
    log.logger.clear();

    device_disp_table.CmdPipelineBarrier2(var_vkcommandbuffer, &var_vkdependencyinfo);
    ASSERT_TRUE(log.find("Generated Layer vkCmdPipelineBarrier2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdPipelineBarrier2"));
    log.logger.clear();

    device_disp_table.CmdWriteTimestamp2(var_vkcommandbuffer, var_vkpipelinestageflags2, var_vkquerypool, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdWriteTimestamp2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdWriteTimestamp2"));
    log.logger.clear();

    VkCopyBufferInfo2 var_vkcopybufferinfo2{VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2};
    device_disp_table.CmdCopyBuffer2(var_vkcommandbuffer, &var_vkcopybufferinfo2);
    ASSERT_TRUE(log.find("Generated Layer vkCmdCopyBuffer2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdCopyBuffer2"));
    log.logger.clear();

    VkCopyImageInfo2 var_vkcopyimageinfo2{VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2};
    device_disp_table.CmdCopyImage2(var_vkcommandbuffer, &var_vkcopyimageinfo2);
    ASSERT_TRUE(log.find("Generated Layer vkCmdCopyImage2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdCopyImage2"));
    log.logger.clear();

    VkCopyBufferToImageInfo2 var_vkcopybuffertoimageinfo2{VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2};
    device_disp_table.CmdCopyBufferToImage2(var_vkcommandbuffer, &var_vkcopybuffertoimageinfo2);
    ASSERT_TRUE(log.find("Generated Layer vkCmdCopyBufferToImage2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdCopyBufferToImage2"));
    log.logger.clear();

    VkCopyImageToBufferInfo2 var_vkcopyimagetobufferinfo2{VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2};
    device_disp_table.CmdCopyImageToBuffer2(var_vkcommandbuffer, &var_vkcopyimagetobufferinfo2);
    ASSERT_TRUE(log.find("Generated Layer vkCmdCopyImageToBuffer2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdCopyImageToBuffer2"));
    log.logger.clear();

    VkBlitImageInfo2 var_vkblitimageinfo2{VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2};
    device_disp_table.CmdBlitImage2(var_vkcommandbuffer, &var_vkblitimageinfo2);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBlitImage2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBlitImage2"));
    log.logger.clear();

    VkResolveImageInfo2 var_vkresolveimageinfo2{VK_STRUCTURE_TYPE_RESOLVE_IMAGE_INFO_2};
    device_disp_table.CmdResolveImage2(var_vkcommandbuffer, &var_vkresolveimageinfo2);
    ASSERT_TRUE(log.find("Generated Layer vkCmdResolveImage2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdResolveImage2"));
    log.logger.clear();

    VkRenderingInfo var_vkrenderinginfo{VK_STRUCTURE_TYPE_RENDERING_INFO};
    device_disp_table.CmdBeginRendering(var_vkcommandbuffer, &var_vkrenderinginfo);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBeginRendering"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBeginRendering"));
    log.logger.clear();

    device_disp_table.CmdEndRendering(var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkCmdEndRendering"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdEndRendering"));
    log.logger.clear();

    VkCullModeFlags var_vkcullmodeflags{};
    device_disp_table.CmdSetCullMode(var_vkcommandbuffer, var_vkcullmodeflags);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetCullMode"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetCullMode"));
    log.logger.clear();

    VkFrontFace var_vkfrontface{};
    device_disp_table.CmdSetFrontFace(var_vkcommandbuffer, var_vkfrontface);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetFrontFace"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetFrontFace"));
    log.logger.clear();

    VkPrimitiveTopology var_vkprimitivetopology{};
    device_disp_table.CmdSetPrimitiveTopology(var_vkcommandbuffer, var_vkprimitivetopology);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetPrimitiveTopology"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetPrimitiveTopology"));
    log.logger.clear();

    VkViewport var_vkviewport{};
    device_disp_table.CmdSetViewportWithCount(var_vkcommandbuffer, var_uint32_t, &var_vkviewport);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetViewportWithCount"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetViewportWithCount"));
    log.logger.clear();

    VkRect2D var_vkrect2d{};
    device_disp_table.CmdSetScissorWithCount(var_vkcommandbuffer, var_uint32_t, &var_vkrect2d);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetScissorWithCount"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetScissorWithCount"));
    log.logger.clear();

    VkDeviceSize var_vkdevicesize{};
    device_disp_table.CmdBindVertexBuffers2(var_vkcommandbuffer, var_uint32_t, var_uint32_t, &var_vkbuffer, &var_vkdevicesize, &var_vkdevicesize, &var_vkdevicesize);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBindVertexBuffers2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBindVertexBuffers2"));
    log.logger.clear();

    VkBool32 var_vkbool32{};
    device_disp_table.CmdSetDepthTestEnable(var_vkcommandbuffer, var_vkbool32);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetDepthTestEnable"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetDepthTestEnable"));
    log.logger.clear();

    device_disp_table.CmdSetDepthWriteEnable(var_vkcommandbuffer, var_vkbool32);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetDepthWriteEnable"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetDepthWriteEnable"));
    log.logger.clear();

    VkCompareOp var_vkcompareop{};
    device_disp_table.CmdSetDepthCompareOp(var_vkcommandbuffer, var_vkcompareop);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetDepthCompareOp"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetDepthCompareOp"));
    log.logger.clear();

    device_disp_table.CmdSetDepthBoundsTestEnable(var_vkcommandbuffer, var_vkbool32);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetDepthBoundsTestEnable"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetDepthBoundsTestEnable"));
    log.logger.clear();

    device_disp_table.CmdSetStencilTestEnable(var_vkcommandbuffer, var_vkbool32);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetStencilTestEnable"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetStencilTestEnable"));
    log.logger.clear();

    VkStencilFaceFlags var_vkstencilfaceflags{};
    VkStencilOp var_vkstencilop{};
    device_disp_table.CmdSetStencilOp(var_vkcommandbuffer, var_vkstencilfaceflags, var_vkstencilop, var_vkstencilop, var_vkstencilop, var_vkcompareop);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetStencilOp"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetStencilOp"));
    log.logger.clear();

    device_disp_table.CmdSetRasterizerDiscardEnable(var_vkcommandbuffer, var_vkbool32);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetRasterizerDiscardEnable"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetRasterizerDiscardEnable"));
    log.logger.clear();

    device_disp_table.CmdSetDepthBiasEnable(var_vkcommandbuffer, var_vkbool32);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetDepthBiasEnable"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetDepthBiasEnable"));
    log.logger.clear();

    device_disp_table.CmdSetPrimitiveRestartEnable(var_vkcommandbuffer, var_vkbool32);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetPrimitiveRestartEnable"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetPrimitiveRestartEnable"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

    VkPrivateDataSlotCreateInfo var_vkprivatedataslotcreateinfo{VK_STRUCTURE_TYPE_PRIVATE_DATA_SLOT_CREATE_INFO};
    device_disp_table.CreatePrivateDataSlot(dev.dev, &var_vkprivatedataslotcreateinfo, nullptr, &var_vkprivatedataslot);
    ASSERT_TRUE(log.find("Generated Layer vkCreatePrivateDataSlot"));
    ASSERT_TRUE(log.find("Generated Driver vkCreatePrivateDataSlot"));
    log.logger.clear();

    device_disp_table.DestroyPrivateDataSlot(dev.dev, var_vkprivatedataslot, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyPrivateDataSlot"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyPrivateDataSlot"));
    log.logger.clear();

} // Test for Vulkan Core 1.3

// clang-format on

