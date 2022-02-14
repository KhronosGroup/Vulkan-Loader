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

class UnknownFunction : public ::testing::Test {
   protected:
    virtual void SetUp() {
        env = std::unique_ptr<FrameworkEnvironment>(new FrameworkEnvironment());
        env->add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    }

    virtual void TearDown() { env.reset(); }
    std::unique_ptr<FrameworkEnvironment> env;
};

/*
 Creates a TestICD with a function unknown to the loader called vkNotRealFuncTEST. The TestICD, when vk_icdGetPhysicalDeviceProcAddr
 is called, will return the custom_physical_device_function if the function name matches vkNotRealFuncTEST. The test then calls the
 function to verify that the unknown physical device function dispatching is working correctly.
*/

VKAPI_ATTR int VKAPI_CALL custom_physical_device_function(VkPhysicalDevice device, int foo, int bar) { return foo + bar; }
using PFN_custom_physical_device_function = decltype(&custom_physical_device_function);

TEST_F(UnknownFunction, PhysicalDeviceFunction) {
#if defined(__APPLE__)
    GTEST_SKIP() << "Skip this test as currently macOS doesn't fully support unknown functions.";
#endif
    uint32_t function_count = MAX_NUM_UNKNOWN_EXTS;
    auto& driver = env->get_test_icd();
    std::vector<std::string> fake_function_names;

    driver.physical_devices.emplace_back("physical_device_0");
    for (uint32_t i = 0; i < function_count; i++) {
        fake_function_names.push_back(std::string("vkNotRealFuncTEST_") + std::to_string(i));

        driver.custom_physical_device_functions.push_back(
            VulkanFunction{fake_function_names.back(), reinterpret_cast<void*>(custom_physical_device_function)});
    }
    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    VkPhysicalDevice phys_dev = inst.GetPhysDev();
    int i = 0, j = 1000;
    for (const auto& function_name : fake_function_names) {
        PFN_custom_physical_device_function returned_func = env->vulkan_functions.load(inst.inst, function_name.c_str());
        ASSERT_NE(returned_func, nullptr);
        ASSERT_EQ(returned_func(phys_dev, i, j), i + j);
        i++;
        j--;
    }
}

TEST_F(UnknownFunction, PhysicalDeviceFunctionWithImplicitLayer) {
#if defined(__APPLE__)
    GTEST_SKIP() << "Skip this test as currently macOS doesn't fully support unknown functions.";
#endif
    uint32_t function_count = MAX_NUM_UNKNOWN_EXTS;
    auto& driver = env->get_test_icd();
    std::vector<std::string> fake_function_names;

    driver.physical_devices.emplace_back("physical_device_0");
    for (uint32_t i = 0; i < function_count; i++) {
        fake_function_names.push_back(std::string("vkNotRealFuncTEST_") + std::to_string(i));

        driver.custom_physical_device_functions.push_back(
            VulkanFunction{fake_function_names.back(), reinterpret_cast<void*>(custom_physical_device_function)});
    }

    env->add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                          .set_name("VK_LAYER_implicit_layer_unknown_function_intercept")
                                                          .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                          .set_disable_environment("DISABLE_ME")),
                            "implicit_layer_unknown_function_intercept.json");

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    VkPhysicalDevice phys_dev = inst.GetPhysDev();
    int i = 0, j = 1000;
    for (const auto& function_name : fake_function_names) {
        PFN_custom_physical_device_function returned_func = env->vulkan_functions.load(inst.inst, function_name.c_str());
        ASSERT_NE(returned_func, nullptr);
        ASSERT_EQ(returned_func(phys_dev, i, j), i + j);
        i++;
        j--;
    }
}

VKAPI_ATTR int VKAPI_CALL custom_device_function(VkDevice device, int foo, int bar) { return foo + bar; }
using PFN_custom_device_function = decltype(&custom_device_function);

TEST_F(UnknownFunction, DeviceFunctionFromGetInstanceProcAddr) {
#if defined(__APPLE__)
    GTEST_SKIP() << "Skip this test as currently macOS doesn't fully support unknown functions.";
#endif
    uint32_t function_count = MAX_NUM_UNKNOWN_EXTS;
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    std::vector<std::string> fake_function_names;
    for (uint32_t i = 0; i < function_count; i++) {
        fake_function_names.push_back(std::string("vkNotRealFuncTEST_") + std::to_string(i));
        driver.physical_devices.back().add_device_function(
            VulkanFunction{fake_function_names.back(), reinterpret_cast<void*>(custom_device_function)});
    }

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    DeviceWrapper dev{inst};
    dev.CheckCreate(inst.GetPhysDev());
    // auto createCommandPool =
    //    reinterpret_cast<PFN_vkCreateCommandPool>(env->vulkan_functions.vkGetInstanceProcAddr(inst, "vkCreateCommandPool"));
    // createCommandPool(dev.dev, nullptr, nullptr, nullptr);
    int i = 0, j = 1000;
    for (const auto& function_name : fake_function_names) {
        PFN_custom_device_function returned_func = env->vulkan_functions.load(inst.inst, function_name.c_str());
        ASSERT_NE(returned_func, nullptr);
        ASSERT_EQ(returned_func(dev.dev, i, j), i + j);
        i++;
        j--;
    }
}

TEST_F(UnknownFunction, DeviceFunctionFromGetInstanceProcAddrWithImplicitLayer) {
#if defined(__APPLE__)
    GTEST_SKIP() << "Skip this test as currently macOS doesn't fully support unknown functions.";
#endif
    uint32_t function_count = MAX_NUM_UNKNOWN_EXTS;
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    std::vector<std::string> fake_function_names;
    for (uint32_t i = 0; i < function_count; i++) {
        fake_function_names.push_back(std::string("vkNotRealFuncTEST_") + std::to_string(i));
        driver.physical_devices.back().add_device_function(
            VulkanFunction{fake_function_names.back(), reinterpret_cast<void*>(custom_device_function)});
    }

    env->add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                          .set_name("VK_LAYER_implicit_layer_unknown_function_intercept")
                                                          .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                          .set_disable_environment("DISABLE_ME")),
                            "implicit_layer_unknown_function_intercept.json");

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    DeviceWrapper dev{inst};
    dev.CheckCreate(inst.GetPhysDev());
    int i = 0, j = 1000;
    for (const auto& function_name : fake_function_names) {
        PFN_custom_device_function returned_func = env->vulkan_functions.load(inst.inst, function_name.c_str());
        ASSERT_NE(returned_func, nullptr);
        ASSERT_EQ(returned_func(dev.dev, i, j), i + j);
        i++;
        j--;
    }
}

TEST_F(UnknownFunction, DeviceFunctionFromGetDeviceProcAddr) {
    uint32_t function_count = 1000;
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    std::vector<std::string> fake_function_names;
    for (uint32_t i = 0; i < function_count; i++) {
        fake_function_names.push_back(std::string("vkNotRealFuncTEST_") + std::to_string(i));
        driver.physical_devices.back().add_device_function(
            VulkanFunction{fake_function_names.back(), reinterpret_cast<void*>(custom_device_function)});
    }

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    DeviceWrapper dev{inst};
    dev.CheckCreate(inst.GetPhysDev());
    int i = 0, j = 1000;
    for (const auto& function_name : fake_function_names) {
        PFN_custom_device_function returned_func = env->vulkan_functions.load(dev.dev, function_name.c_str());
        ASSERT_NE(returned_func, nullptr);
        ASSERT_EQ(returned_func(dev, i, j), i + j);
        i++;
        j--;
    }
}

TEST_F(UnknownFunction, DeviceFunctionFromGetDeviceProcAddrWithImplicitLayer) {
    uint32_t function_count = 1000;
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    std::vector<std::string> fake_function_names;
    for (uint32_t i = 0; i < function_count; i++) {
        fake_function_names.push_back(std::string("vkNotRealFuncTEST_") + std::to_string(i));
        driver.physical_devices.back().add_device_function(
            VulkanFunction{fake_function_names.back(), reinterpret_cast<void*>(custom_device_function)});
    }

    env->add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                          .set_name("VK_LAYER_implicit_layer_unknown_function_intercept")
                                                          .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                          .set_disable_environment("DISABLE_ME")),
                            "implicit_layer_unknown_function_intercept.json");

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    DeviceWrapper dev{inst};
    dev.CheckCreate(inst.GetPhysDev());
    int i = 0, j = 1000;
    for (const auto& function_name : fake_function_names) {
        PFN_custom_device_function returned_func = env->vulkan_functions.load(dev.dev, function_name.c_str());
        ASSERT_NE(returned_func, nullptr);
        ASSERT_EQ(returned_func(dev, i, j), i + j);
        i++;
        j--;
    }
}

VKAPI_ATTR int VKAPI_CALL custom_command_buffer_function(VkCommandBuffer commandBuffer, int foo, int bar) { return foo + bar; }
using PFN_custom_command_buffer_function = decltype(&custom_command_buffer_function);

// command buffer
// command buffer with implicit layer

TEST_F(UnknownFunction, CommandBufferFunctionFromGetDeviceProcAddr) {
    uint32_t function_count = 1000;
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().add_queue_family_properties({});
    std::vector<std::string> fake_function_names;
    for (uint32_t i = 0; i < function_count; i++) {
        fake_function_names.push_back(std::string("vkNotRealFuncTEST_") + std::to_string(i));
        driver.physical_devices.back().add_device_function(
            VulkanFunction{fake_function_names.back(), reinterpret_cast<void*>(custom_command_buffer_function)});
    }

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    DeviceWrapper dev{inst};
    dev.CheckCreate(inst.GetPhysDev());

    DeviceFunctions funcs{env->vulkan_functions, dev};
    VkCommandPool command_pool;
    VkCommandPoolCreateInfo pool_create_info{};
    funcs.vkCreateCommandPool(dev, &pool_create_info, nullptr, &command_pool);
    VkCommandBuffer command_buffer;
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.commandBufferCount = 1;
    alloc_info.commandPool = command_pool;
    funcs.vkAllocateCommandBuffers(dev, &alloc_info, &command_buffer);

    int i = 0, j = 1000;
    for (const auto& function_name : fake_function_names) {
        PFN_custom_command_buffer_function returned_func = env->vulkan_functions.load(dev.dev, function_name.c_str());
        ASSERT_NE(returned_func, nullptr);
        ASSERT_EQ(returned_func(command_buffer, i, j), i + j);
        i++;
        j--;
    }
}

TEST_F(UnknownFunction, CommandBufferFunctionFromGetDeviceProcAddrWithImplicitLayer) {
    uint32_t function_count = 1000;
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().add_queue_family_properties({});
    std::vector<std::string> fake_function_names;
    for (uint32_t i = 0; i < function_count; i++) {
        fake_function_names.push_back(std::string("vkNotRealFuncTEST_") + std::to_string(i));
        driver.physical_devices.back().add_device_function(
            VulkanFunction{fake_function_names.back(), reinterpret_cast<void*>(custom_command_buffer_function)});
    }

    env->add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                          .set_name("VK_LAYER_implicit_layer_unknown_function_intercept")
                                                          .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                          .set_disable_environment("DISABLE_ME")),
                            "implicit_layer_unknown_function_intercept.json");

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    DeviceWrapper dev{inst};
    dev.CheckCreate(inst.GetPhysDev());

    DeviceFunctions funcs{env->vulkan_functions, dev};
    VkCommandPool command_pool;
    VkCommandPoolCreateInfo pool_create_info{};
    funcs.vkCreateCommandPool(dev, &pool_create_info, nullptr, &command_pool);
    VkCommandBuffer command_buffer;
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.commandBufferCount = 1;
    alloc_info.commandPool = command_pool;
    funcs.vkAllocateCommandBuffers(dev, &alloc_info, &command_buffer);

    int i = 0, j = 1000;
    for (const auto& function_name : fake_function_names) {
        PFN_custom_command_buffer_function returned_func = env->vulkan_functions.load(dev.dev, function_name.c_str());
        ASSERT_NE(returned_func, nullptr);
        ASSERT_EQ(returned_func(command_buffer, i, j), i + j);
        i++;
        j--;
    }
}

TEST_F(UnknownFunction, CommandBufferFunctionFromGetInstanceProcAddr) {
#if defined(__APPLE__)
    GTEST_SKIP() << "Skip this test as currently macOS doesn't fully support unknown functions.";
#endif
    uint32_t function_count = MAX_NUM_UNKNOWN_EXTS;
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().add_queue_family_properties({});
    std::vector<std::string> fake_function_names;
    for (uint32_t i = 0; i < function_count; i++) {
        fake_function_names.push_back(std::string("vkNotRealFuncTEST_") + std::to_string(i));
        driver.physical_devices.back().add_device_function(
            VulkanFunction{fake_function_names.back(), reinterpret_cast<void*>(custom_command_buffer_function)});
    }

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    DeviceWrapper dev{inst};
    dev.CheckCreate(inst.GetPhysDev());

    DeviceFunctions funcs{env->vulkan_functions, dev};
    VkCommandPool command_pool;
    VkCommandPoolCreateInfo pool_create_info{};
    funcs.vkCreateCommandPool(dev, &pool_create_info, nullptr, &command_pool);
    VkCommandBuffer command_buffer;
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.commandBufferCount = 1;
    alloc_info.commandPool = command_pool;
    funcs.vkAllocateCommandBuffers(dev, &alloc_info, &command_buffer);

    int i = 0, j = 1000;
    for (const auto& function_name : fake_function_names) {
        PFN_custom_command_buffer_function returned_func = env->vulkan_functions.load(inst.inst, function_name.c_str());
        ASSERT_NE(returned_func, nullptr);
        ASSERT_EQ(returned_func(command_buffer, i, j), i + j);
        i++;
        j--;
    }
}

TEST_F(UnknownFunction, CommandBufferFunctionFromGetInstanceProcAddrWithImplicitLayer) {
#if defined(__APPLE__)
    GTEST_SKIP() << "Skip this test as currently macOS doesn't fully support unknown functions.";
#endif
    uint32_t function_count = MAX_NUM_UNKNOWN_EXTS;
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().add_queue_family_properties({});
    std::vector<std::string> fake_function_names;
    for (uint32_t i = 0; i < function_count; i++) {
        fake_function_names.push_back(std::string("vkNotRealFuncTEST_") + std::to_string(i));
        driver.physical_devices.back().add_device_function(
            VulkanFunction{fake_function_names.back(), reinterpret_cast<void*>(custom_command_buffer_function)});
    }

    env->add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                          .set_name("VK_LAYER_implicit_layer_unknown_function_intercept")
                                                          .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                          .set_disable_environment("DISABLE_ME")),
                            "implicit_layer_unknown_function_intercept.json");

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    DeviceWrapper dev{inst};
    dev.CheckCreate(inst.GetPhysDev());

    DeviceFunctions funcs{env->vulkan_functions, dev};
    VkCommandPool command_pool;
    VkCommandPoolCreateInfo pool_create_info{};
    funcs.vkCreateCommandPool(dev, &pool_create_info, nullptr, &command_pool);
    VkCommandBuffer command_buffer;
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.commandBufferCount = 1;
    alloc_info.commandPool = command_pool;
    funcs.vkAllocateCommandBuffers(dev, &alloc_info, &command_buffer);

    int i = 0, j = 1000;
    for (const auto& function_name : fake_function_names) {
        PFN_custom_command_buffer_function returned_func = env->vulkan_functions.load(inst.inst, function_name.c_str());
        ASSERT_NE(returned_func, nullptr);
        ASSERT_EQ(returned_func(command_buffer, i, j), i + j);
        i++;
        j--;
    }
}

VKAPI_ATTR int VKAPI_CALL custom_queue_function(VkQueue queue, int foo, int bar) { return foo + bar; }
using PFN_custom_queue_function = decltype(&custom_queue_function);

TEST_F(UnknownFunction, QueueFunctionFromGetDeviceProcAddr) {
    uint32_t function_count = 1000;
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().add_queue_family_properties({});
    std::vector<std::string> fake_function_names;
    for (uint32_t i = 0; i < function_count; i++) {
        fake_function_names.push_back(std::string("vkNotRealFuncTEST_") + std::to_string(i));
        driver.physical_devices.back().add_device_function(
            VulkanFunction{fake_function_names.back(), reinterpret_cast<void*>(custom_queue_function)});
    }

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    DeviceWrapper dev{inst};
    dev.create_info.add_device_queue({});
    dev.CheckCreate(inst.GetPhysDev());
    VkQueue queue{};
    env->vulkan_functions.vkGetDeviceQueue(dev, 0, 0, &queue);

    int i = 0, j = 1000;
    for (const auto& function_name : fake_function_names) {
        PFN_custom_queue_function returned_func = env->vulkan_functions.load(dev.dev, function_name.c_str());
        ASSERT_NE(returned_func, nullptr);
        ASSERT_EQ(returned_func(queue, i, j), i + j);
        i++;
        j--;
    }
}

TEST_F(UnknownFunction, QueueFunctionFromGetDeviceProcAddrWithImplicitLayer) {
    uint32_t function_count = 1000;
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().add_queue_family_properties({});
    std::vector<std::string> fake_function_names;
    for (uint32_t i = 0; i < function_count; i++) {
        fake_function_names.push_back(std::string("vkNotRealFuncTEST_") + std::to_string(i));
        driver.physical_devices.back().add_device_function(
            VulkanFunction{fake_function_names.back(), reinterpret_cast<void*>(custom_queue_function)});
    }

    env->add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                          .set_name("VK_LAYER_implicit_layer_unknown_function_intercept")
                                                          .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                          .set_disable_environment("DISABLE_ME")),
                            "implicit_layer_unknown_function_intercept.json");

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    DeviceWrapper dev{inst};
    dev.create_info.add_device_queue({});
    dev.CheckCreate(inst.GetPhysDev());
    VkQueue queue{};
    env->vulkan_functions.vkGetDeviceQueue(dev, 0, 0, &queue);

    int i = 0, j = 1000;
    for (const auto& function_name : fake_function_names) {
        PFN_custom_queue_function returned_func = env->vulkan_functions.load(dev.dev, function_name.c_str());
        ASSERT_NE(returned_func, nullptr);
        ASSERT_EQ(returned_func(queue, i, j), i + j);
        i++;
        j--;
    }
}

TEST_F(UnknownFunction, QueueFunctionFromGetInstanceProcAddr) {
#if defined(__APPLE__)
    GTEST_SKIP() << "Skip this test as currently macOS doesn't fully support unknown functions.";
#endif
    uint32_t function_count = MAX_NUM_UNKNOWN_EXTS;
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().add_queue_family_properties({});
    std::vector<std::string> fake_function_names;
    for (uint32_t i = 0; i < function_count; i++) {
        fake_function_names.push_back(std::string("vkNotRealFuncTEST_") + std::to_string(i));
        driver.physical_devices.back().add_device_function(
            VulkanFunction{fake_function_names.back(), reinterpret_cast<void*>(custom_queue_function)});
    }

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    DeviceWrapper dev{inst};
    dev.create_info.add_device_queue({});
    dev.CheckCreate(inst.GetPhysDev());
    VkQueue queue{};
    env->vulkan_functions.vkGetDeviceQueue(dev, 0, 0, &queue);

    int i = 0, j = 1000;
    for (const auto& function_name : fake_function_names) {
        PFN_custom_queue_function returned_func = env->vulkan_functions.load(inst.inst, function_name.c_str());
        ASSERT_NE(returned_func, nullptr);
        ASSERT_EQ(returned_func(queue, i, j), i + j);
        i++;
        j--;
    }
}

TEST_F(UnknownFunction, QueueFunctionFromGetInstanceProcAddrWithImplicitLayer) {
#if defined(__APPLE__)
    GTEST_SKIP() << "Skip this test as currently macOS doesn't fully support unknown functions.";
#endif
    uint32_t function_count = MAX_NUM_UNKNOWN_EXTS;
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().add_queue_family_properties({});
    std::vector<std::string> fake_function_names;
    for (uint32_t i = 0; i < function_count; i++) {
        fake_function_names.push_back(std::string("vkNotRealFuncTEST_") + std::to_string(i));
        driver.physical_devices.back().add_device_function(
            VulkanFunction{fake_function_names.back(), reinterpret_cast<void*>(custom_queue_function)});
    }

    env->add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                          .set_name("VK_LAYER_implicit_layer_unknown_function_intercept")
                                                          .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                          .set_disable_environment("DISABLE_ME")),
                            "implicit_layer_unknown_function_intercept.json");

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    DeviceWrapper dev{inst};
    dev.create_info.add_device_queue({});
    dev.CheckCreate(inst.GetPhysDev());
    VkQueue queue{};
    env->vulkan_functions.vkGetDeviceQueue(dev, 0, 0, &queue);

    int i = 0, j = 1000;
    for (const auto& function_name : fake_function_names) {
        PFN_custom_queue_function returned_func = env->vulkan_functions.load(inst.inst, function_name.c_str());
        ASSERT_NE(returned_func, nullptr);
        ASSERT_EQ(returned_func(queue, i, j), i + j);
        i++;
        j--;
    }
}
