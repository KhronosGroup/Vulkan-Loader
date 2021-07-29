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

class ThreadingTests : public ::testing::Test {
   protected:
    virtual void SetUp() {
        env = std::unique_ptr<SingleICDShim>(new SingleICDShim(TestICDDetails(TEST_ICD_PATH_VERSION_2, VK_MAKE_VERSION(1, 0, 0))));
    }

    virtual void TearDown() { env.reset(); }
    std::unique_ptr<SingleICDShim> env;
};

void create_destroy_device_loop(uint32_t num_loops_create_destroy_device, uint32_t num_loops_try_get_proc_addr, InstWrapper* inst,
                                VkPhysicalDevice phys_dev) {
    for (uint32_t i = 0; i < num_loops_create_destroy_device; i++) {
        DeviceWrapper dev{*inst};
        DeviceCreateInfo dev_create_info{};
        dev_create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(1.0));
        ASSERT_EQ(VK_SUCCESS, CreateDevice(phys_dev, dev, dev_create_info));

        for (uint32_t i = 0; i < num_loops_try_get_proc_addr; i++) {
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

TEST_F(ThreadingTests, ConcurentGetDeviceProcAddr) {
    uint32_t num_threads = 100;
    uint32_t num_loops_create_destroy_device = 10;
    uint32_t num_loops_try_get_proc_addr = 100;
    auto& driver = env->get_test_icd();

    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().known_device_functions_no_implementation.push_back("vkCmdBindPipeline");
    driver.physical_devices.back().known_device_functions_no_implementation.push_back("vkCmdBindDescriptorSets");
    driver.physical_devices.back().known_device_functions_no_implementation.push_back("vkCmdBindVertexBuffers");
    driver.physical_devices.back().known_device_functions_no_implementation.push_back("vkCmdBindIndexBuffer");
    driver.physical_devices.back().known_device_functions_no_implementation.push_back("vkCmdDraw");

    InstWrapper inst{env->vulkan_functions};
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(CreateInst(inst, inst_create_info), VK_SUCCESS);

    VkPhysicalDevice phys_dev;
    ASSERT_EQ(CreatePhysDev(inst, phys_dev), VK_SUCCESS);

    DeviceWrapper dev{inst};
    DeviceCreateInfo dev_create_info{};
    dev_create_info.add_device_queue(DeviceQueueCreateInfo{}.add_priority(1.0));
    ASSERT_EQ(VK_SUCCESS, CreateDevice(phys_dev, dev, dev_create_info));

    std::vector<std::thread> threads;
    for (uint32_t i = 0; i < num_threads; i++) {
        threads.emplace_back(create_destroy_device_loop, num_loops_create_destroy_device, num_loops_try_get_proc_addr, &inst,
                             phys_dev);
    }
    for (uint32_t i = 0; i < num_threads; i++) {
        threads[i].join();
    }
}