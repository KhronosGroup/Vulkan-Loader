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
#include <functional>

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
template <typename DispatchableHandleType>
struct custom_functions {
    static VKAPI_ATTR uint32_t VKAPI_CALL func_zero(DispatchableHandleType handle, uint32_t foo) { return foo; };
    static VKAPI_ATTR uint32_t VKAPI_CALL func_one(DispatchableHandleType handle, uint32_t foo, uint32_t bar) { return foo + bar; };
    static VKAPI_ATTR float VKAPI_CALL func_two(DispatchableHandleType handle, uint32_t foo, uint32_t bar, float baz) {
        return baz + foo + bar;
    };
    static VKAPI_ATTR int VKAPI_CALL func_three(DispatchableHandleType handle, int* ptr_a, int* ptr_b) { return *ptr_a + *ptr_b; };
    static VKAPI_ATTR float VKAPI_CALL func_four(DispatchableHandleType handle, int* ptr_a, int* ptr_b, int foo, int bar, float k,
                                                 float l, char a, char b, char c) {
        return *ptr_a + *ptr_b + foo + bar + k + l + static_cast<int>(a) + static_cast<int>(b) + static_cast<int>(c);
    };
};

template <typename FunctionStruct>
void fill_custom_functions(std::vector<VulkanFunction>& driver_function_list, std::vector<std::string>& fake_function_names,
                           FunctionStruct const& funcs, uint32_t function_count) {
    for (uint32_t i = 0; i < function_count;) {
        fake_function_names.push_back(std::string("vkNotIntRealFuncTEST_") + std::to_string(i++));
        driver_function_list.push_back(VulkanFunction{fake_function_names.back(), reinterpret_cast<void*>(funcs.func_zero)});

        fake_function_names.push_back(std::string("vkNotIntRealIntFuncTEST_") + std::to_string(i++));
        driver_function_list.push_back(VulkanFunction{fake_function_names.back(), reinterpret_cast<void*>(funcs.func_one)});

        fake_function_names.push_back(std::string("vkIntNotIntRealFloatFuncTEST_") + std::to_string(i++));
        driver_function_list.push_back(VulkanFunction{fake_function_names.back(), reinterpret_cast<void*>(funcs.func_two)});

        fake_function_names.push_back(std::string("vkNotRealFuncPointerPointerTEST_") + std::to_string(i++));
        driver_function_list.push_back(VulkanFunction{fake_function_names.back(), reinterpret_cast<void*>(funcs.func_three)});

        fake_function_names.push_back(std::string("vkNotRealFuncTEST_pointer_pointer_int_int_float_float_char_char_char_") +
                                      std::to_string(i++));
        driver_function_list.push_back(VulkanFunction{fake_function_names.back(), reinterpret_cast<void*>(funcs.func_four)});
    }
}
template <typename FunctionLoader, typename ParentType, typename DispatchableHandleType, typename FunctionStruct>
void check_custom_functions(FunctionLoader& loader, ParentType parent, DispatchableHandleType handle, FunctionStruct const& s,
                            std::vector<std::string>& fake_function_names, uint32_t function_count) {
    for (uint32_t i = 0; i < function_count;) {
        decltype(FunctionStruct::func_zero)* returned_func_i = loader.load(parent, fake_function_names.at(i++).c_str());
        ASSERT_NE(returned_func_i, nullptr);
        EXPECT_EQ(returned_func_i(handle, i * 10), i * 10);

        decltype(FunctionStruct::func_one)* returned_func_ii = loader.load(parent, fake_function_names.at(i++).c_str());
        ASSERT_NE(returned_func_ii, nullptr);
        EXPECT_EQ(returned_func_ii(handle, i * 10, i * 5), i * 10 + i * 5);

        decltype(FunctionStruct::func_two)* returned_func_iif = loader.load(parent, fake_function_names.at(i++).c_str());
        ASSERT_NE(returned_func_iif, nullptr);
        EXPECT_NEAR(returned_func_iif(handle, i * 10, i * 5, 0.1234f), i * 10 + i * 5 + 0.1234f, 0.001);

        int x = 5;
        int y = -505;
        decltype(FunctionStruct::func_three)* returned_func_pp = loader.load(parent, fake_function_names.at(i++).c_str());
        ASSERT_NE(returned_func_pp, nullptr);
        EXPECT_EQ(returned_func_pp(handle, &x, &y), -500);

        decltype(FunctionStruct::func_four)* returned_func_ppiiffccc = loader.load(parent, fake_function_names.at(i++).c_str());
        ASSERT_NE(returned_func_ppiiffccc, nullptr);
        EXPECT_NEAR(returned_func_ppiiffccc(handle, &x, &y, 200, 300, 0.123f, 1001.89f, 'a', 'b', 'c'),
                    -500 + 200 + 300 + 0.123 + 1001.89 + 97 + 98 + 99, 0.001f);
    }
}
using custom_physical_device_functions = custom_functions<VkPhysicalDevice>;

TEST_F(UnknownFunction, PhysicalDeviceFunction) {
#if defined(__APPLE__)
    GTEST_SKIP() << "Skip this test as currently macOS doesn't fully support unknown functions.";
#endif
    uint32_t function_count = MAX_NUM_UNKNOWN_EXTS;
    auto& driver = env->get_test_icd();
    std::vector<std::string> fake_function_names;

    driver.physical_devices.emplace_back("physical_device_0");
    fill_custom_functions(driver.custom_physical_device_functions, fake_function_names, custom_physical_device_functions{},
                          function_count);
    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    VkPhysicalDevice phys_dev = inst.GetPhysDev();
    check_custom_functions(env->vulkan_functions, inst.inst, phys_dev, custom_physical_device_functions{}, fake_function_names,
                           function_count);
}

TEST_F(UnknownFunction, PhysicalDeviceFunctionWithImplicitLayer) {
#if defined(__APPLE__)
    GTEST_SKIP() << "Skip this test as currently macOS doesn't fully support unknown functions.";
#endif
    uint32_t function_count = MAX_NUM_UNKNOWN_EXTS;
    auto& driver = env->get_test_icd();
    std::vector<std::string> fake_function_names;

    driver.physical_devices.emplace_back("physical_device_0");
    fill_custom_functions(driver.custom_physical_device_functions, fake_function_names, custom_physical_device_functions{},
                          function_count);

    env->add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                          .set_name("VK_LAYER_implicit_layer_unknown_function_intercept")
                                                          .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                          .set_disable_environment("DISABLE_ME")),
                            "implicit_layer_unknown_function_intercept.json");

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    VkPhysicalDevice phys_dev = inst.GetPhysDev();
    check_custom_functions(env->vulkan_functions, inst.inst, phys_dev, custom_physical_device_functions{}, fake_function_names,
                           function_count);
}

TEST_F(UnknownFunction, PhysicalDeviceFunctionWithImplicitLayerInterception) {
#if defined(__APPLE__)
    GTEST_SKIP() << "Skip this test as currently macOS doesn't fully support unknown functions.";
#endif
    uint32_t function_count = MAX_NUM_UNKNOWN_EXTS;
    auto& driver = env->get_test_icd();
    std::vector<std::string> fake_function_names;

    driver.physical_devices.emplace_back("physical_device_0");

    env->add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                          .set_name("VK_LAYER_implicit_layer_unknown_function_intercept")
                                                          .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                          .set_disable_environment("DISABLE_ME")),
                            "implicit_layer_unknown_function_intercept.json");
    auto& layer = env->get_test_layer();
    fill_custom_functions(layer.custom_physical_device_functions, fake_function_names, custom_physical_device_functions{},
                          function_count);

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    VkPhysicalDevice phys_dev = inst.GetPhysDev();
    check_custom_functions(env->vulkan_functions, inst.inst, phys_dev, custom_physical_device_functions{}, fake_function_names,
                           function_count);
}

using custom_device_functions = custom_functions<VkDevice>;

TEST_F(UnknownFunction, DeviceFunctionFromGetInstanceProcAddr) {
#if defined(__APPLE__)
    GTEST_SKIP() << "Skip this test as currently macOS doesn't fully support unknown functions.";
#endif
    uint32_t function_count = MAX_NUM_UNKNOWN_EXTS;
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    std::vector<std::string> fake_function_names;

    fill_custom_functions(driver.physical_devices.back().known_device_functions, fake_function_names, custom_device_functions{},
                          function_count);

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    DeviceWrapper dev{inst};
    dev.CheckCreate(inst.GetPhysDev());
    check_custom_functions(env->vulkan_functions, inst.inst, dev.dev, custom_device_functions{}, fake_function_names,
                           function_count);
}

TEST_F(UnknownFunction, DeviceFunctionFromGetInstanceProcAddrWithImplicitLayer) {
#if defined(__APPLE__)
    GTEST_SKIP() << "Skip this test as currently macOS doesn't fully support unknown functions.";
#endif
    uint32_t function_count = MAX_NUM_UNKNOWN_EXTS;
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    std::vector<std::string> fake_function_names;
    fill_custom_functions(driver.physical_devices.back().known_device_functions, fake_function_names, custom_device_functions{},
                          function_count);

    env->add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                          .set_name("VK_LAYER_implicit_layer_unknown_function_intercept")
                                                          .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                          .set_disable_environment("DISABLE_ME")),
                            "implicit_layer_unknown_function_intercept.json");

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    DeviceWrapper dev{inst};
    dev.CheckCreate(inst.GetPhysDev());
    check_custom_functions(env->vulkan_functions, inst.inst, dev.dev, custom_device_functions{}, fake_function_names,
                           function_count);
}

TEST_F(UnknownFunction, DeviceFunctionFromGetDeviceProcAddr) {
    uint32_t function_count = 1000;
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    std::vector<std::string> fake_function_names;
    fill_custom_functions(driver.physical_devices.back().known_device_functions, fake_function_names, custom_device_functions{},
                          function_count);

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    DeviceWrapper dev{inst};
    dev.CheckCreate(inst.GetPhysDev());
    check_custom_functions(env->vulkan_functions, dev.dev, dev.dev, custom_device_functions{}, fake_function_names, function_count);
}

TEST_F(UnknownFunction, DeviceFunctionFromGetDeviceProcAddrWithImplicitLayer) {
    uint32_t function_count = 1000;
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    std::vector<std::string> fake_function_names;
    fill_custom_functions(driver.physical_devices.back().known_device_functions, fake_function_names, custom_device_functions{},
                          function_count);
    env->add_implicit_layer(ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                          .set_name("VK_LAYER_implicit_layer_unknown_function_intercept")
                                                          .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                          .set_disable_environment("DISABLE_ME")),
                            "implicit_layer_unknown_function_intercept.json");

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    DeviceWrapper dev{inst};
    dev.CheckCreate(inst.GetPhysDev());
    check_custom_functions(env->vulkan_functions, dev.dev, dev.dev, custom_device_functions{}, fake_function_names, function_count);
}

using custom_command_buffer_functions = custom_functions<VkCommandBuffer>;

TEST_F(UnknownFunction, CommandBufferFunctionFromGetDeviceProcAddr) {
    uint32_t function_count = 1000;
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().add_queue_family_properties({});
    std::vector<std::string> fake_function_names;

    fill_custom_functions(driver.physical_devices.back().known_device_functions, fake_function_names,
                          custom_command_buffer_functions{}, function_count);

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

    check_custom_functions(env->vulkan_functions, dev.dev, command_buffer, custom_command_buffer_functions{}, fake_function_names,
                           function_count);
}

TEST_F(UnknownFunction, CommandBufferFunctionFromGetDeviceProcAddrWithImplicitLayer) {
    uint32_t function_count = 1000;
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().add_queue_family_properties({});
    std::vector<std::string> fake_function_names;
    fill_custom_functions(driver.physical_devices.back().known_device_functions, fake_function_names,
                          custom_command_buffer_functions{}, function_count);

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

    check_custom_functions(env->vulkan_functions, dev.dev, command_buffer, custom_command_buffer_functions{}, fake_function_names,
                           function_count);
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
    fill_custom_functions(driver.physical_devices.back().known_device_functions, fake_function_names,
                          custom_command_buffer_functions{}, function_count);

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

    check_custom_functions(env->vulkan_functions, inst.inst, command_buffer, custom_command_buffer_functions{}, fake_function_names,
                           function_count);
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
    fill_custom_functions(driver.physical_devices.back().known_device_functions, fake_function_names,
                          custom_command_buffer_functions{}, function_count);

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

    check_custom_functions(env->vulkan_functions, inst.inst, command_buffer, custom_command_buffer_functions{}, fake_function_names,
                           function_count);
}

using custom_queue_functions = custom_functions<VkQueue>;

TEST_F(UnknownFunction, QueueFunctionFromGetDeviceProcAddr) {
    uint32_t function_count = 1000;
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().add_queue_family_properties({});
    std::vector<std::string> fake_function_names;
    fill_custom_functions(driver.physical_devices.back().known_device_functions, fake_function_names, custom_queue_functions{},
                          function_count);

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    DeviceWrapper dev{inst};
    dev.create_info.add_device_queue({});
    dev.CheckCreate(inst.GetPhysDev());
    VkQueue queue{};
    env->vulkan_functions.vkGetDeviceQueue(dev, 0, 0, &queue);

    check_custom_functions(env->vulkan_functions, dev.dev, queue, custom_queue_functions{}, fake_function_names, function_count);
}

TEST_F(UnknownFunction, QueueFunctionFromGetDeviceProcAddrWithImplicitLayer) {
    uint32_t function_count = 1000;
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");
    driver.physical_devices.back().add_queue_family_properties({});
    std::vector<std::string> fake_function_names;
    fill_custom_functions(driver.physical_devices.back().known_device_functions, fake_function_names, custom_queue_functions{},
                          function_count);

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

    check_custom_functions(env->vulkan_functions, dev.dev, queue, custom_queue_functions{}, fake_function_names, function_count);
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
    fill_custom_functions(driver.physical_devices.back().known_device_functions, fake_function_names, custom_queue_functions{},
                          function_count);

    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();

    DeviceWrapper dev{inst};
    dev.create_info.add_device_queue({});
    dev.CheckCreate(inst.GetPhysDev());
    VkQueue queue{};
    env->vulkan_functions.vkGetDeviceQueue(dev, 0, 0, &queue);

    check_custom_functions(env->vulkan_functions, inst.inst, queue, custom_queue_functions{}, fake_function_names, function_count);
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
    fill_custom_functions(driver.physical_devices.back().known_device_functions, fake_function_names, custom_queue_functions{},
                          function_count);

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

    check_custom_functions(env->vulkan_functions, inst.inst, queue, custom_queue_functions{}, fake_function_names, function_count);
}
