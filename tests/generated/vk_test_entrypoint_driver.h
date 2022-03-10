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

struct EntrypointTestDriver {
    fs::path manifest_file_path;

    const uint32_t icd_api_version = VK_API_VERSION_1_3;
    const uint32_t max_icd_interface_version = 6;
    uint8_t enabled_instance_major;
    uint8_t enabled_instance_minor;

    DispatchableHandle<VkInstance> instance_handle;
    bool instance_extensions_initialized = false;
    std::vector<std::string> instance_extensions{};
    bool device_extensions_initialized = false;
    std::vector<std::string> device_extensions{};
    DispatchableHandle<VkPhysicalDevice> physical_device_handle;
    std::vector<std::string> enabled_instance_extensions{};

    struct DebugUtilsInfo {
        VkDebugUtilsMessageSeverityFlagsEXT     severities;
        VkDebugUtilsMessageTypeFlagsEXT         types;
        PFN_vkDebugUtilsMessengerCallbackEXT    callback = nullptr;
        void*                                   user_data = nullptr;
    };
    DebugUtilsInfo debug_util_info;
    std::vector<DispatchableHandle<VkDevice>*> dev_handles;
    std::vector<std::string> enabled_device_extensions;

    std::vector<DispatchableHandle<VkQueue>*> queue_handles;
    std::vector<DispatchableHandle<VkCommandBuffer>*> commandbuffer_handles;
};

using GetEPDriverFunc = EntrypointTestDriver* (*)();
#define GET_EP_DRIVER_STR "get_ep_driver_func"

using GetNewEPDriverFunc = EntrypointTestDriver* (*)();
#define RESET_EP_DRIVER_STR "reset_ep_driver_func"
// clang-format on

