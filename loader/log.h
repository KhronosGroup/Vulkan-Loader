/*
 *
 * Copyright (c) 2014-2021 The Khronos Group Inc.
 * Copyright (c) 2014-2021 Valve Corporation
 * Copyright (c) 2014-2021 LunarG, Inc.
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
 * Author: Jon Ashburn <jon@lunarg.com>
 * Author: Courtney Goeltzenleuchter <courtney@LunarG.com>
 * Author: Chia-I Wu <olvaffe@gmail.com>
 * Author: Chia-I Wu <olv@lunarg.com>
 * Author: Mark Lobodzinski <mark@LunarG.com>
 * Author: Lenny Komow <lenny@lunarg.com>
 * Author: Charles Giessen <charles@lunarg.com>
 *
 */

#include "loader_common.h"

enum vulkan_loader_debug_flags {
    VULKAN_LOADER_INFO_BIT = 0x01,
    VULKAN_LOADER_WARN_BIT = 0x02,
    VULKAN_LOADER_PERF_BIT = 0x04,
    VULKAN_LOADER_ERROR_BIT = 0x08,
    VULKAN_LOADER_DEBUG_BIT = 0x10,
    VULKAN_LOADER_LAYER_BIT = 0x20,
    VULKAN_LOADER_DRIVER_BIT = 0x40,
    VULKAN_LOADER_VALIDATION_BIT = 0x80,
};

// Checks for the environment variable VK_LOADER_DEBUG and sets up the current debug level accordingly
// This should be called before any Vulkan API calls, eg in the initialization of the .dll or .so
void loader_debug_init(void);

// Returns a bitmask that indicates the current flags that should be output
uint32_t loader_get_debug_level(void);

// Logs a message to stderr
// May output to DebugUtils if the instance isn't null and the extension is enabled.
void loader_log(const struct loader_instance *inst, VkFlags msg_type, int32_t msg_code, const char *format, ...);
