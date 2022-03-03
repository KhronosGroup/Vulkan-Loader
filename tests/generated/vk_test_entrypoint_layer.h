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

#pragma once

#include "test_util.h"
#include "layer/layer_util.h"
#include "loader/generated/vk_layer_dispatch_table.h"

struct EntrypointTestLayer {
    fs::path manifest_file_path;
    const uint32_t manifest_version = VK_MAKE_API_VERSION(0, 1, 1, 2);
    const uint32_t api_version = VK_API_VERSION_1_3;

    PFN_vkGetInstanceProcAddr next_vkGetInstanceProcAddr = VK_NULL_HANDLE;
    PFN_vkGetDeviceProcAddr next_vkGetDeviceProcAddr = VK_NULL_HANDLE;

    const uint32_t max_icd_interface_version = 6;
    VkInstance instance_handle;
    VkLayerInstanceDispatchTable instance_dispatch_table{};
    uint8_t enabled_instance_major;
    uint8_t enabled_instance_minor;
    std::vector<std::string> enabled_instance_extensions{};

    struct DebugUtilsInfo {
        VkDebugUtilsMessageSeverityFlagsEXT     severities;
        VkDebugUtilsMessageTypeFlagsEXT         types;
        PFN_vkDebugUtilsMessengerCallbackEXT    callback = nullptr;
        void*                                   user_data = nullptr;
    };
    DebugUtilsInfo debug_util_info;
    struct Device {
        VkDevice device_handle;
        VkLayerDispatchTable dispatch_table;
        std::vector<std::string> enabled_extensions;
    };
    std::vector<Device> created_devices;
};

using GetTestLayerFunc = EntrypointTestLayer* (*)();
#define GET_TEST_LAYER_FUNC_STR "get_test_layer_func"

using GetNewTestLayerFunc = EntrypointTestLayer* (*)();
#define RESET_LAYER_FUNC_STR "reset_layer_func"

// clang-format on

