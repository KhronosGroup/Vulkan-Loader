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

#include "test_environment.h"

#include <mutex>
#include <thread>

void create_destroy_device_loop(uint32_t num_loops_create_destroy_device, uint32_t num_loops_try_get_proc_addr, InstWrapper* inst,
                                VkPhysicalDevice phys_dev) {
    for (uint32_t i = 0; i < num_loops_create_destroy_device; i++) {
        DeviceWrapper dev{*inst};
        dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(1.0));
        dev.CheckCreate(phys_dev);

        for (uint32_t j = 0; j < num_loops_try_get_proc_addr; j++) {
            PFN_vkCmdBindPipeline p =
                reinterpret_cast<PFN_vkCmdBindPipeline>(dev->vkGetDeviceProcAddr(dev.dev, "vkCmdBindPipeline"));
            ASSERT_NE(p, nullptr);
            PFN_vkCmdBindDescriptorSets d =
                reinterpret_cast<PFN_vkCmdBindDescriptorSets>(dev->vkGetDeviceProcAddr(dev.dev, "vkCmdBindDescriptorSets"));
            ASSERT_NE(d, nullptr);
            PFN_vkCmdBindVertexBuffers vb =
                reinterpret_cast<PFN_vkCmdBindVertexBuffers>(dev->vkGetDeviceProcAddr(dev.dev, "vkCmdBindVertexBuffers"));
            ASSERT_NE(vb, nullptr);
            PFN_vkCmdBindIndexBuffer ib =
                reinterpret_cast<PFN_vkCmdBindIndexBuffer>(dev->vkGetDeviceProcAddr(dev.dev, "vkCmdBindIndexBuffer"));
            ASSERT_NE(ib, nullptr);
            PFN_vkCmdDraw c = reinterpret_cast<PFN_vkCmdDraw>(dev->vkGetDeviceProcAddr(dev.dev, "vkCmdDraw"));
            ASSERT_NE(c, nullptr);
        }
    }
}
VKAPI_ATTR void VKAPI_CALL test_vkCmdBindPipeline(VkCommandBuffer cmd_buf, VkPipelineBindPoint pipelineBindPoint,
                                                  VkPipeline pipeline) {}
VKAPI_ATTR void VKAPI_CALL test_vkCmdBindDescriptorSets(VkCommandBuffer cmd_buf, VkPipelineBindPoint pipelineBindPoint,
                                                        VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount,
                                                        const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount,
                                                        const uint32_t* pDynamicOffsets) {}
VKAPI_ATTR void VKAPI_CALL test_vkCmdBindVertexBuffers(VkCommandBuffer cmd_buf, uint32_t firstBinding, uint32_t bindingCount,
                                                       const VkBuffer* pBuffers, const VkDeviceSize* pOffsets) {}
VKAPI_ATTR void VKAPI_CALL test_vkCmdBindIndexBuffer(VkCommandBuffer cmd_buf, uint32_t firstBinding, uint32_t bindingCount,
                                                     const VkBuffer* pBuffers, const VkDeviceSize* pOffsets) {}
VKAPI_ATTR void VKAPI_CALL test_vkCmdDraw(VkCommandBuffer cmd_buf, uint32_t vertexCount, uint32_t instanceCount,
                                          uint32_t firstVertex, uint32_t firstInstance) {}
TEST(ThreadingTests, ConcurentGetDeviceProcAddr) {
    FrameworkEnvironment env{};
    env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA));
    uint32_t num_threads = 100;
    uint32_t num_loops_create_destroy_device = 10;
    uint32_t num_loops_try_get_proc_addr = 100;
    auto& driver = env.get_test_icd();

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().known_device_functions.push_back(
        {"vkCmdBindPipeline", to_vkVoidFunction(test_vkCmdBindPipeline)});
    driver.physical_devices.back().known_device_functions.push_back(
        {"vkCmdBindDescriptorSets", to_vkVoidFunction(test_vkCmdBindDescriptorSets)});
    driver.physical_devices.back().known_device_functions.push_back(
        {"vkCmdBindVertexBuffers", to_vkVoidFunction(test_vkCmdBindVertexBuffers)});
    driver.physical_devices.back().known_device_functions.push_back(
        {"vkCmdBindIndexBuffer", to_vkVoidFunction(test_vkCmdBindIndexBuffer)});
    driver.physical_devices.back().known_device_functions.push_back({"vkCmdDraw", to_vkVoidFunction(test_vkCmdDraw)});

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    DeviceWrapper dev{inst};
    dev.create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(1.0));
    dev.CheckCreate(phys_dev);

    std::vector<std::thread> threads;
    for (uint32_t i = 0; i < num_threads; i++) {
        threads.emplace_back(create_destroy_device_loop, num_loops_create_destroy_device, num_loops_try_get_proc_addr, &inst,
                             phys_dev);
    }
    for (uint32_t i = 0; i < num_threads; i++) {
        threads[i].join();
    }
}
