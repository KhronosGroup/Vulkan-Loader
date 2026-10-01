#!/usr/bin/python3 -i
#
# Copyright (c) 2026 The Khronos Group Inc.
# Copyright (c) 2026 Valve Corporation
# Copyright (c) 2026 LunarG, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Author: Charles Giessen <charles@lunarg.com>

DISPATCHABLE_TYPES = ['VkInstance', 'VkPhysicalDevice', 'VkDevice', 'VkQueue', 'VkCommandBuffer']

AVOID_EXT_NAMES = ['VK_EXT_debug_report']

AVOID_CMD_NAMES = ['vkCreateDebugUtilsMessengerEXT',
                   'vkDestroyDebugUtilsMessengerEXT',
                   'vkSubmitDebugUtilsMessageEXT']

WSI_EXT_NAMES = ['VK_KHR_surface',
                 'VK_KHR_display',
                 'VK_KHR_xlib_surface',
                 'VK_KHR_xcb_surface',
                 'VK_KHR_wayland_surface',
                 'VK_EXT_directfb_surface',
                 'VK_KHR_win32_surface',
                 'VK_KHR_android_surface',
                 'VK_GGP_stream_descriptor_surface',
                 'VK_MVK_macos_surface',
                 'VK_MVK_ios_surface',
                 'VK_EXT_headless_surface',
                 'VK_EXT_metal_surface',
                 'VK_FUCHSIA_imagepipe_surface',
                 'VK_KHR_swapchain',
                 'VK_KHR_display_swapchain',
                 'VK_KHR_get_display_properties2',
                 'VK_KHR_get_surface_capabilities2',
                 'VK_QNX_screen_surface',
                 'VK_NN_vi_surface']
