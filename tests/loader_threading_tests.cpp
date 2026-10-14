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

#include <algorithm>
#include <thread>

void create_destroy_instance_loop_with_function_queries(FrameworkEnvironment* env, uint32_t num_loops_create_destroy_instance,
                                                        uint32_t num_loops_try_get_instance_proc_addr,
                                                        uint32_t num_loops_try_get_device_proc_addr) {
    for (uint32_t i = 0; i < num_loops_create_destroy_instance; i++) {
        InstWrapper inst{env->vulkan_functions};
        inst.CheckCreate();
        PFN_vkEnumeratePhysicalDevices enum_pd = nullptr;
        for (uint32_t j = 0; j < num_loops_try_get_instance_proc_addr; j++) {
            enum_pd = inst.load("vkEnumeratePhysicalDevices");
            ASSERT_NE(enum_pd, nullptr);
        }
        VkPhysicalDevice phys_dev = inst.GetPhysDev();

        DeviceWrapper dev{inst};
        dev.CheckCreate(phys_dev);
        for (uint32_t j = 0; j < num_loops_try_get_device_proc_addr; j++) {
            PFN_vkCmdBindPipeline p = dev.load("vkCmdBindPipeline");
            p(VK_NULL_HANDLE, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, VK_NULL_HANDLE);
        }
    }
}

void create_destroy_device_loop(FrameworkEnvironment* env, uint32_t num_loops_create_destroy_device,
                                uint32_t num_loops_try_get_proc_addr) {
    InstWrapper inst{env->vulkan_functions};
    inst.CheckCreate();
    for (uint32_t i = 0; i < num_loops_create_destroy_device; i++) {
        DeviceWrapper dev{inst};
        dev.CheckCreate(inst.GetPhysDev());

        for (uint32_t j = 0; j < num_loops_try_get_proc_addr; j++) {
            PFN_vkCmdBindPipeline p = dev.load("vkCmdBindPipeline");
            PFN_vkCmdBindDescriptorSets d = dev.load("vkCmdBindDescriptorSets");
            PFN_vkCmdBindVertexBuffers vb = dev.load("vkCmdBindVertexBuffers");
            PFN_vkCmdBindIndexBuffer ib = dev.load("vkCmdBindIndexBuffer");
            PFN_vkCmdDraw c = dev.load("vkCmdDraw");
            p(VK_NULL_HANDLE, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, VK_NULL_HANDLE);
            d(VK_NULL_HANDLE, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, VK_NULL_HANDLE, 0, 0, nullptr, 0, nullptr);
            vb(VK_NULL_HANDLE, 0, 0, nullptr, nullptr);
            ib(VK_NULL_HANDLE, 0, 0, VkIndexType::VK_INDEX_TYPE_UINT16);
            c(VK_NULL_HANDLE, 0, 0, 0, 0);
        }
    }
}
VKAPI_ATTR void VKAPI_CALL test_vkCmdBindPipeline(VkCommandBuffer, VkPipelineBindPoint, VkPipeline) {}
VKAPI_ATTR void VKAPI_CALL test_vkCmdBindDescriptorSets(VkCommandBuffer, VkPipelineBindPoint, VkPipelineLayout, uint32_t, uint32_t,
                                                        const VkDescriptorSet*, uint32_t, const uint32_t*) {}
VKAPI_ATTR void VKAPI_CALL test_vkCmdBindVertexBuffers(VkCommandBuffer, uint32_t, uint32_t, const VkBuffer*, const VkDeviceSize*) {}
VKAPI_ATTR void VKAPI_CALL test_vkCmdBindIndexBuffer(VkCommandBuffer, VkBuffer, VkDeviceSize, VkIndexType) {}
VKAPI_ATTR void VKAPI_CALL test_vkCmdDraw(VkCommandBuffer, uint32_t, uint32_t, uint32_t, uint32_t) {}
TEST(Threading, InstanceCreateDestroyLoop) {
    const auto processor_count = std::thread::hardware_concurrency();

    FrameworkEnvironment env{FrameworkSettings{}.set_log_filter("")};
    auto& driver = env.add_icd(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA);
    uint32_t num_loops_create_destroy_instance = 50;
    uint32_t num_loops_try_get_instance_proc_addr = 5;
    uint32_t num_loops_try_get_device_proc_addr = 50;

    driver.add_and_get_physical_device("physical_device_0")
        .known_device_functions.push_back({"vkCmdBindPipeline", to_vkVoidFunction(test_vkCmdBindPipeline)});

    std::vector<std::thread> instance_creation_threads;
    std::vector<std::thread> function_query_threads;
    for (uint32_t i = 0; i < processor_count; i++) {
        instance_creation_threads.emplace_back(create_destroy_instance_loop_with_function_queries, &env,
                                               num_loops_create_destroy_instance, num_loops_try_get_instance_proc_addr,
                                               num_loops_try_get_device_proc_addr);
    }
    for (uint32_t i = 0; i < processor_count; i++) {
        instance_creation_threads[i].join();
    }
}

TEST(Threading, DeviceCreateDestroyLoop) {
    const auto processor_count = std::thread::hardware_concurrency();

    FrameworkEnvironment env{FrameworkSettings{}.set_log_filter("")};
    auto& driver = env.add_icd(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA);

    uint32_t num_loops_create_destroy_device = 100;
    uint32_t num_loops_try_get_device_proc_addr = 5;

    driver.add_and_get_physical_device("physical_device_0").known_device_functions = {
        {"vkCmdBindPipeline", to_vkVoidFunction(test_vkCmdBindPipeline)},
        {"vkCmdBindDescriptorSets", to_vkVoidFunction(test_vkCmdBindDescriptorSets)},
        {"vkCmdBindVertexBuffers", to_vkVoidFunction(test_vkCmdBindVertexBuffers)},
        {"vkCmdBindIndexBuffer", to_vkVoidFunction(test_vkCmdBindIndexBuffer)},
        {"vkCmdDraw", to_vkVoidFunction(test_vkCmdDraw)}};

    std::vector<std::thread> device_creation_threads;

    for (uint32_t i = 0; i < processor_count; i++) {
        device_creation_threads.emplace_back(create_destroy_device_loop, &env, num_loops_create_destroy_device,
                                             num_loops_try_get_device_proc_addr);
    }
    for (uint32_t i = 0; i < processor_count; i++) {
        device_creation_threads[i].join();
    }
}

TEST(Threading, LayersDeviceCreateDestroyLoop) {
    const auto processor_count = std::thread::hardware_concurrency();

    FrameworkEnvironment env{FrameworkSettings{}.set_log_filter("")};
    auto& driver = env.add_icd(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA);

    env.add_implicit_layer({}, ManifestLayer{}.add_layer(ManifestLayer::LayerDescription{}
                                                             .set_name("implicit_layer_name")
                                                             .set_lib_path(TEST_LAYER_PATH_EXPORT_VERSION_2)
                                                             .set_disable_environment("DISABLE_ME")));
    env.get_test_layer(0).set_call_create_and_destroy_device_in_GetQueue(true);

    uint32_t num_loops_create_destroy_device = 100;
    uint32_t num_loops_try_get_device_proc_addr = 5;

    driver.add_and_get_physical_device("physical_device_0").known_device_functions = {
        {"vkCmdBindPipeline", to_vkVoidFunction(test_vkCmdBindPipeline)},
        {"vkCmdBindDescriptorSets", to_vkVoidFunction(test_vkCmdBindDescriptorSets)},
        {"vkCmdBindVertexBuffers", to_vkVoidFunction(test_vkCmdBindVertexBuffers)},
        {"vkCmdBindIndexBuffer", to_vkVoidFunction(test_vkCmdBindIndexBuffer)},
        {"vkCmdDraw", to_vkVoidFunction(test_vkCmdDraw)}};

    std::vector<std::thread> device_creation_threads;

    for (uint32_t i = 0; i < processor_count; i++) {
        device_creation_threads.emplace_back(create_destroy_device_loop, &env, num_loops_create_destroy_device,
                                             num_loops_try_get_device_proc_addr);
    }
    for (uint32_t i = 0; i < processor_count; i++) {
        device_creation_threads[i].join();
    }
}

void set_debug_utils_name_loop(FrameworkEnvironment* env, uint32_t num_loops, InstWrapper* inst,
                               std::vector<VkPhysicalDevice>* phys_devs) {
    auto load_function = [&inst](const char* func_name) { return inst->load(func_name); };

    PFN_vkSetDebugUtilsObjectNameEXT SetDebugUtilsObjectNameEXT = load_function("vkSetDebugUtilsObjectNameEXT");
    PFN_vkSetDebugUtilsObjectTagEXT SetDebugUtilsObjectTagEXT = load_function("vkSetDebugUtilsObjectTagEXT");

    std::vector<DeviceWrapper> devices;
    for (uint32_t i = 0; i < phys_devs->size(); i++) {
        auto& device = devices.emplace_back(*inst);
        device.CheckCreate(phys_devs->at(i));
    }

    for (uint32_t i = 0; i < num_loops; i++) {
        for (uint32_t i = 0; i < phys_devs->size(); i++) {
            VkDebugUtilsObjectNameInfoEXT info{};
            info.pObjectName = "this string is not that interesting\n";
            SetDebugUtilsObjectNameEXT(devices.at(i), &info);
        }
    }
}
VKAPI_ATTR VkBool32 VKAPI_CALL test_threading_debug_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT,
                                                                       VkDebugUtilsMessageTypeFlagsEXT,
                                                                       const VkDebugUtilsMessengerCallbackDataEXT*, void*) {
    return VK_FALSE;
}

void create_destroy_messenger_loop(uint32_t num_loops, VkInstance inst, PFN_vkCreateDebugUtilsMessengerEXT create,
                                   PFN_vkDestroyDebugUtilsMessengerEXT destroy) {
    VkDebugUtilsMessengerCreateInfoEXT info{VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    info.pfnUserCallback = test_threading_debug_messenger_callback;
    for (uint32_t i = 0; i < num_loops; i++) {
        VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
        if (create(inst, &info, nullptr, &messenger) == VK_SUCCESS) {
            destroy(inst, messenger, nullptr);
        }
    }
}

// The emulated GetPhysicalDeviceFeatures2 path (1.1 app on a 1.0 ICD) logs an info message, which walks the
// instance's debug messenger chain, and this trampoline does not hold loader_lock while it does so.
void emulated_features2_query_loop(uint32_t num_loops, VkPhysicalDevice phys_dev, PFN_vkGetPhysicalDeviceFeatures2 query) {
    for (uint32_t i = 0; i < num_loops; i++) {
        VkPhysicalDeviceFeatures2 features{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
        query(phys_dev, &features);
    }
}

// One thread creates and destroys debug messengers (mutating the messenger chain under loader_lock) while another
// thread triggers loader logging that walks that same chain. Run under the thread sanitizer this catches an
// unsynchronised access to the messenger chain if the logging walk is not taken under loader_lock.
TEST(Threading, DebugUtilsMessengerLoggingLoop) {
    const uint32_t thread_pairs = std::max(std::thread::hardware_concurrency() / 2, 1u);
    uint32_t num_loops = 2000;
    FrameworkEnvironment env{FrameworkSettings{}.set_log_filter("")};
    env.add_icd(TEST_ICD_PATH_VERSION_2).add_physical_device("physical_device_0");

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.set_api_version(VK_API_VERSION_1_1).add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    PFN_vkCreateDebugUtilsMessengerEXT create = inst.load("vkCreateDebugUtilsMessengerEXT");
    PFN_vkDestroyDebugUtilsMessengerEXT destroy = inst.load("vkDestroyDebugUtilsMessengerEXT");
    PFN_vkGetPhysicalDeviceFeatures2 query = inst.load("vkGetPhysicalDeviceFeatures2");
    ASSERT_NE(create, nullptr);
    ASSERT_NE(destroy, nullptr);
    ASSERT_NE(query, nullptr);

    std::vector<std::thread> threads;
    for (uint32_t i = 0; i < thread_pairs; i++) {
        threads.emplace_back(create_destroy_messenger_loop, num_loops, inst.inst, create, destroy);
        threads.emplace_back(emulated_features2_query_loop, num_loops, phys_dev, query);
    }
    for (auto& thread : threads) {
        thread.join();
    }
}

TEST(Threading, SetDebugUtilsNameCreateDestroyLoop) {
    const auto processor_count = std::thread::hardware_concurrency();
    uint32_t num_loops = 1000;
    FrameworkEnvironment env{FrameworkSettings{}.set_log_filter("")};
    auto& driver1 = env.add_icd(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA).add_physical_device({});
    auto& driver2 = env.add_icd(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA).add_physical_device({});

    std::vector<std::thread> set_debug_name_threads;

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    inst.CheckCreate();

    auto phys_devs = inst.GetPhysDevs();
    for (uint32_t i = 0; i < processor_count; i++) {
        set_debug_name_threads.emplace_back(set_debug_utils_name_loop, &env, num_loops, &inst, &phys_devs);
    }
    for (uint32_t i = 0; i < processor_count; i++) {
        set_debug_name_threads[i].join();
    }
}

void create_destroy_surface_loop(FrameworkEnvironment* env, uint32_t num_loops, InstWrapper* inst) {
    for (uint32_t i = 0; i < num_loops; i++) {
        VkSurfaceKHR surface{};
        ASSERT_EQ(VK_SUCCESS, create_surface(*inst, surface));
        env->vulkan_functions.vkDestroySurfaceKHR(inst->inst, surface, nullptr);
    }
}

TEST(Threading, SurfaceCreateDestroyLoop) {
    const auto processor_count = std::thread::hardware_concurrency();
    uint32_t num_loops = 100;
    FrameworkEnvironment env{FrameworkSettings{}.set_log_filter("")};
    env.add_icd(TEST_ICD_PATH_VERSION_2).setup_WSI().add_physical_device({});

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.setup_WSI();
    inst.CheckCreate();

    std::vector<std::thread> surface_threads;
    for (uint32_t i = 0; i < processor_count; i++) {
        surface_threads.emplace_back(create_destroy_surface_loop, &env, num_loops, &inst);
    }
    for (uint32_t i = 0; i < processor_count; i++) {
        surface_threads[i].join();
    }
}

// Surface queries unwrap the loader surface by reading the driver's surface_list - the same list that surface creation
// grows (and reallocates) under loader_lock once more surfaces are alive than its current capacity. The query commands
// don't require external synchronization of the surface, so this runs them against one surface while other threads
// create and destroy batches of surfaces. Run under the thread sanitizer this catches an unsynchronised access to the
// surface list if the unwrap is not done under loader_lock.
void query_surface_support_loop(VulkanFunctions* vulkan_functions, VkPhysicalDevice phys_dev, VkSurfaceKHR surface,
                                uint32_t num_loops) {
    for (uint32_t i = 0; i < num_loops; i++) {
        VkBool32 supported = VK_FALSE;
        ASSERT_EQ(VK_SUCCESS, vulkan_functions->vkGetPhysicalDeviceSurfaceSupportKHR(phys_dev, 0, surface, &supported));
    }
}

void create_destroy_surface_batch_loop(FrameworkEnvironment* env, InstWrapper* inst, uint32_t num_loops, uint32_t batch_size) {
    for (uint32_t i = 0; i < num_loops; i++) {
        std::vector<VkSurfaceKHR> surfaces(batch_size, VK_NULL_HANDLE);
        for (auto& surface : surfaces) {
            ASSERT_EQ(VK_SUCCESS, create_surface(*inst, surface));
        }
        for (auto& surface : surfaces) {
            env->vulkan_functions.vkDestroySurfaceKHR(inst->inst, surface, nullptr);
        }
    }
}

TEST(Threading, SurfaceQueryWhileCreateDestroySurfaceLoop) {
    const uint32_t thread_pairs = std::max(std::thread::hardware_concurrency() / 2, 1u);
    const uint32_t num_query_loops = 1000;
    const uint32_t num_create_destroy_loops = 10;
    // Enough surfaces alive at once to grow the driver's surface_list past its initial capacity of 32 entries
    const uint32_t batch_size = 70;

    FrameworkEnvironment env{FrameworkSettings{}.set_log_filter("")};
    auto& driver = env.add_icd(TEST_ICD_PATH_VERSION_2);
    driver.setup_WSI();
    driver.add_and_get_physical_device("physical_device_0")
        .add_queue_family_properties({{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true});

    InstWrapper inst{env.vulkan_functions};
    inst.create_info.setup_WSI();
    inst.CheckCreate();
    VkPhysicalDevice phys_dev = inst.GetPhysDev();

    VkSurfaceKHR query_surface{VK_NULL_HANDLE};
    ASSERT_EQ(VK_SUCCESS, create_surface(inst, query_surface));

    std::vector<std::thread> threads;
    for (uint32_t i = 0; i < thread_pairs; i++) {
        threads.emplace_back(query_surface_support_loop, &env.vulkan_functions, phys_dev, query_surface, num_query_loops);
        threads.emplace_back(create_destroy_surface_batch_loop, &env, &inst, num_create_destroy_loops, batch_size);
    }
    for (auto& thread : threads) {
        thread.join();
    }
    env.vulkan_functions.vkDestroySurfaceKHR(inst.inst, query_surface, nullptr);
}

// The driver reports these through vk_icdGetPhysicalDeviceProcAddr, so vkGetInstanceProcAddr has to append each one to
// the loader's instance wide unknown function tables before it can hand back a trampoline.
VKAPI_ATTR uint32_t VKAPI_CALL test_unknown_phys_dev_function(VkPhysicalDevice, uint32_t foo) { return foo; }

void get_unknown_function_loop(InstWrapper* inst, std::vector<std::string> const* func_names) {
    for (auto const& name : *func_names) {
        PFN_vkVoidFunction func = inst->load(name.c_str());
        ASSERT_NE(func, nullptr);
    }
}

TEST(Threading, GetUnknownPhysicalDeviceFunctionLoop) {
    // Capped so that thread_count * funcs_per_thread stays under MAX_NUM_UNKNOWN_EXTS on a machine with many cores
    uint32_t thread_count = std::thread::hardware_concurrency();
    if (thread_count < 2) thread_count = 2;
    if (thread_count > 8) thread_count = 8;
    const uint32_t funcs_per_thread = 16;

    FrameworkEnvironment env{FrameworkSettings{}.set_log_filter("")};
    auto& phys_dev = env.add_icd(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA).add_and_get_physical_device({});

    // Each thread queries its own set of names, so every query appends a new entry rather than finding an existing one
    std::vector<std::vector<std::string>> func_names{thread_count};
    for (uint32_t i = 0; i < thread_count; i++) {
        for (uint32_t j = 0; j < funcs_per_thread; j++) {
            func_names[i].push_back("vkNotRealFuncTEST_" + std::to_string(i) + "_" + std::to_string(j));
            phys_dev.custom_physical_device_functions.push_back(
                VulkanFunction{func_names[i].back(), to_vkVoidFunction(test_unknown_phys_dev_function)});
        }
    }

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    std::vector<std::thread> function_query_threads;
    for (uint32_t i = 0; i < thread_count; i++) {
        function_query_threads.emplace_back(get_unknown_function_loop, &inst, &func_names[i]);
    }
    for (uint32_t i = 0; i < thread_count; i++) {
        function_query_threads[i].join();
    }
}

// Reported through the driver's vkGetInstanceProcAddr, which puts them on the unknown device function path. Registering
// one walks every driver's logical device list, which vkCreateDevice and vkDestroyDevice are editing in the other threads.
VKAPI_ATTR uint32_t VKAPI_CALL test_unknown_device_function(VkDevice, uint32_t foo) { return foo; }

void create_destroy_device_only_loop(InstWrapper* inst, uint32_t num_loops) {
    for (uint32_t i = 0; i < num_loops; i++) {
        DeviceWrapper dev{*inst};
        dev.CheckCreate(inst->GetPhysDev());
    }
}

TEST(Threading, GetUnknownDeviceFunctionWhileCreatingDevices) {
    const uint32_t thread_count = 4;
    const uint32_t funcs_per_thread = 16;
    const uint32_t num_loops_create_destroy_device = 50;

    FrameworkEnvironment env{FrameworkSettings{}.set_log_filter("")};
    auto& phys_dev = env.add_icd(TEST_ICD_PATH_VERSION_2_EXPORT_ICD_GPDPA).add_and_get_physical_device({});

    std::vector<std::vector<std::string>> func_names{thread_count};
    for (uint32_t i = 0; i < thread_count; i++) {
        for (uint32_t j = 0; j < funcs_per_thread; j++) {
            func_names[i].push_back("vkNotRealDeviceFuncTEST_" + std::to_string(i) + "_" + std::to_string(j));
            phys_dev.known_device_functions.push_back(
                VulkanFunction{func_names[i].back(), to_vkVoidFunction(test_unknown_device_function)});
        }
    }

    InstWrapper inst{env.vulkan_functions};
    inst.CheckCreate();

    std::vector<std::thread> threads;
    for (uint32_t i = 0; i < thread_count; i++) {
        threads.emplace_back(get_unknown_function_loop, &inst, &func_names[i]);
        threads.emplace_back(create_destroy_device_only_loop, &inst, num_loops_create_destroy_device);
    }
    for (auto& thread : threads) {
        thread.join();
    }
}
