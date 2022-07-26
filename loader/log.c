/*
 *
 * Copyright (c) 2014-2022 The Khronos Group Inc.
 * Copyright (c) 2014-2022 Valve Corporation
 * Copyright (c) 2014-2022 LunarG, Inc.
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

#include "log.h"

#include <stdio.h>
#include <stdarg.h>

#include "debug_utils.h"
#include "loader_environment.h"

uint32_t g_loader_debug = 0;

void loader_debug_init(void) {
    char *env, *orig;

    if (g_loader_debug > 0) return;

    g_loader_debug = 0;

    // Parse comma-separated debug options
    orig = env = loader_getenv("VK_LOADER_DEBUG", NULL);
    while (env) {
        char *p = strchr(env, ',');
        size_t len;

        if (p) {
            len = p - env;
        } else {
            len = strlen(env);
        }

        if (len > 0) {
            if (strncmp(env, "all", len) == 0) {
                g_loader_debug = ~0u;
            } else if (strncmp(env, "warn", len) == 0) {
                g_loader_debug |= VULKAN_LOADER_WARN_BIT;
            } else if (strncmp(env, "info", len) == 0) {
                g_loader_debug |= VULKAN_LOADER_INFO_BIT;
            } else if (strncmp(env, "perf", len) == 0) {
                g_loader_debug |= VULKAN_LOADER_PERF_BIT;
            } else if (strncmp(env, "error", len) == 0) {
                g_loader_debug |= VULKAN_LOADER_ERROR_BIT;
            } else if (strncmp(env, "debug", len) == 0) {
                g_loader_debug |= VULKAN_LOADER_DEBUG_BIT;
            } else if (strncmp(env, "layer", len) == 0) {
                g_loader_debug |= VULKAN_LOADER_LAYER_BIT;
            } else if (strncmp(env, "driver", len) == 0 || strncmp(env, "implem", len) == 0 || strncmp(env, "icd", len) == 0) {
                g_loader_debug |= VULKAN_LOADER_DRIVER_BIT;
            }
        }

        if (!p) break;

        env = p + 1;
    }

    loader_free_getenv(orig, NULL);
}

uint32_t loader_get_debug_level(void) { return g_loader_debug; }

void loader_log(const struct loader_instance *inst, VkFlags msg_type, int32_t msg_code, const char *format, ...) {
    char msg[512];
    char cmd_line_msg[512];
    size_t cmd_line_size = sizeof(cmd_line_msg);
    size_t num_used = 0;
    va_list ap;
    int ret;

    va_start(ap, format);
    ret = vsnprintf(msg, sizeof(msg), format, ap);
    if ((ret >= (int)sizeof(msg)) || ret < 0) {
        msg[sizeof(msg) - 1] = '\0';
    }
    va_end(ap);

    if (inst) {
        VkDebugUtilsMessageSeverityFlagBitsEXT severity = 0;
        VkDebugUtilsMessageTypeFlagsEXT type;
        VkDebugUtilsMessengerCallbackDataEXT callback_data;
        VkDebugUtilsObjectNameInfoEXT object_name;

        if ((msg_type & VULKAN_LOADER_INFO_BIT) != 0) {
            severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        } else if ((msg_type & VULKAN_LOADER_WARN_BIT) != 0) {
            severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        } else if ((msg_type & VULKAN_LOADER_ERROR_BIT) != 0) {
            severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        } else if ((msg_type & VULKAN_LOADER_DEBUG_BIT) != 0) {
            severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
        } else if ((msg_type & VULKAN_LOADER_LAYER_BIT) != 0 || (msg_type & VULKAN_LOADER_DRIVER_BIT) != 0) {
            // Just driver or just layer bit should be treated as an info message in debug utils.
            severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        }

        if ((msg_type & VULKAN_LOADER_PERF_BIT) != 0) {
            type = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        } else if ((msg_type & VULKAN_LOADER_VALIDATION_BIT) != 0) {
            // For loader logging, if it's a validation message, we still want to also keep the general flag as well
            // so messages of type validation can still be triggered for general message callbacks.
            type = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        } else {
            type = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
        }

        callback_data.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CALLBACK_DATA_EXT;
        callback_data.pNext = NULL;
        callback_data.flags = 0;
        callback_data.pMessageIdName = "Loader Message";
        callback_data.messageIdNumber = 0;
        callback_data.pMessage = msg;
        callback_data.queueLabelCount = 0;
        callback_data.pQueueLabels = NULL;
        callback_data.cmdBufLabelCount = 0;
        callback_data.pCmdBufLabels = NULL;
        callback_data.objectCount = 1;
        callback_data.pObjects = &object_name;
        object_name.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        object_name.pNext = NULL;
        object_name.objectType = VK_OBJECT_TYPE_INSTANCE;
        object_name.objectHandle = (uint64_t)(uintptr_t)inst;
        object_name.pObjectName = NULL;

        util_SubmitDebugUtilsMessageEXT(inst, severity, type, &callback_data);
    }

    uint32_t filtered_msg_type = (msg_type & g_loader_debug);
    if (0 == filtered_msg_type) {
        return;
    }

    cmd_line_msg[0] = '\0';
    cmd_line_size -= 1;
    num_used = 1;

    if ((msg_type & VULKAN_LOADER_ERROR_BIT) != 0) {
        strncat(cmd_line_msg, "ERROR", cmd_line_size - num_used);
        num_used += 5;
    } else if ((msg_type & VULKAN_LOADER_WARN_BIT) != 0) {
        strncat(cmd_line_msg, "WARNING", cmd_line_size - num_used);
        num_used += 7;
    } else if ((msg_type & VULKAN_LOADER_INFO_BIT) != 0) {
        strncat(cmd_line_msg, "INFO", cmd_line_size - num_used);
        num_used += 4;
    } else if ((msg_type & VULKAN_LOADER_DEBUG_BIT) != 0) {
        strncat(cmd_line_msg, "DEBUG", cmd_line_size - num_used);
        num_used += 5;
    }
    // For the remaining messages, we only want to add any tags that are
    // explicitly enabled by the tools.
    if ((filtered_msg_type & VULKAN_LOADER_PERF_BIT) != 0) {
        if (num_used > 1) {
            strncat(cmd_line_msg, " | ", cmd_line_size - num_used);
            num_used += 3;
        }
        strncat(cmd_line_msg, "PERF", cmd_line_size - num_used);
        num_used += 4;
    }
    if ((filtered_msg_type & VULKAN_LOADER_DRIVER_BIT) != 0) {
        if (num_used > 1) {
            strncat(cmd_line_msg, " | ", cmd_line_size - num_used);
            num_used += 3;
        }
        strncat(cmd_line_msg, "DRIVER", cmd_line_size - num_used);
        num_used += 6;
    }
    if ((filtered_msg_type & VULKAN_LOADER_LAYER_BIT) != 0) {
        if (num_used > 1) {
            strncat(cmd_line_msg, " | ", cmd_line_size - num_used);
            num_used += 3;
        }
        strncat(cmd_line_msg, "LAYER", cmd_line_size - num_used);
        num_used += 5;
    }
    // Add any preceeding spaces so we can have clean output
    if (num_used > 1) {
        strncat(cmd_line_msg, ": ", cmd_line_size - num_used);
        num_used += 2;
    }
    while (num_used < 19) {
        strncat(cmd_line_msg, " ", cmd_line_size - num_used);
        num_used++;
    }

    size_t available_space = cmd_line_size - num_used;
    if (available_space > 0) {
        // If the message is too long, trim it down
        if (strlen(msg) > available_space) {
            msg[available_space - 1] = '\0';
        }
        strncat(cmd_line_msg, msg, cmd_line_size);

#if defined(WIN32)
        OutputDebugString(cmd_line_msg);
        OutputDebugString("\n");
#endif

        fputs(cmd_line_msg, stderr);
        fputc('\n', stderr);
    } else {
        // Shouldn't get here, but check to make sure if we've already overrun
        // the string boundary
        assert(false);
    }
}

void loader_log_asm_function_not_supported(const struct loader_instance *inst, VkFlags msg_type, int32_t msg_code,
                                           const char *func_name) {
    loader_log(inst, msg_type, msg_code, "Function %s not supported for this physical device", func_name);
}
