// *** THIS FILE IS GENERATED - DO NOT EDIT ***
// See loader_tramp_term_generator.py for modifications

/*
 * Copyright (c) 2015-2022 The Khronos Group Inc.
 * Copyright (c) 2015-2022 Valve Corporation
 * Copyright (c) 2015-2022 LunarG, Inc.
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
 * Author: Mark Lobodzinski <mark@lunarg.com>
 * Author: Mark Young <marky@lunarg.com>
 */

// clang-format off
#pragma once


// Define types as externally available structs where necessary
struct loader_instance;
struct loader_dev_dispatch_table;


// Extension interception for vkCreateInstance function, so we can properly detect and
// enable any instance extension information for extensions we know about.
void extensions_create_instance(struct loader_instance *ptr_instance, const VkInstanceCreateInfo *pCreateInfo);

// Extension interception for vkCreateDevice function, so we can properly detect and
// enable any device extension information for extensions we know about.
void extensions_create_device(struct loader_device *dev, struct loader_icd_term *icd_term, VkPhysicalDevice phys_dev, const VkDeviceCreateInfo *pCreateInfo);

// Array of extension strings for instance extensions we support.
extern const char *const LOADER_INSTANCE_EXTENSIONS[];

// Init Device function pointer dispatch table with core commands
VKAPI_ATTR void VKAPI_CALL loader_init_device_dispatch_table(struct loader_dev_dispatch_table *dev_table, PFN_vkGetDeviceProcAddr gdpa,
                                                             VkDevice dev);

// Init Device function pointer dispatch table with extension commands
VKAPI_ATTR void VKAPI_CALL loader_init_device_extension_dispatch_table(struct loader_dev_dispatch_table *dev_table,
                                                                       PFN_vkGetInstanceProcAddr gipa,
                                                                       PFN_vkGetDeviceProcAddr gdpa,
                                                                       VkInstance inst,
                                                                       VkDevice dev);

// Init Instance function pointer dispatch table with core commands
VKAPI_ATTR void VKAPI_CALL loader_init_instance_core_dispatch_table(VkLayerInstanceDispatchTable *table, PFN_vkGetInstanceProcAddr gipa,
                                                                    VkInstance inst);

// Init Instance function pointer dispatch table with core commands
VKAPI_ATTR void VKAPI_CALL loader_init_instance_extension_dispatch_table(VkLayerInstanceDispatchTable *table, PFN_vkGetInstanceProcAddr gipa,
                                                                         VkInstance inst);

// Instance command lookup function
VKAPI_ATTR void* VKAPI_CALL loader_lookup_instance_dispatch_table(const VkLayerInstanceDispatchTable *table, const char *name,
                                                                  bool *found_name);

// Device command lookup function
VKAPI_ATTR void* VKAPI_CALL loader_lookup_device_dispatch_table(const VkLayerDispatchTable *table, const char *name);

// Union of all known instance extensions that the loader may need to refer to when
// making function pointer decisions.
union loader_instance_extension_enables {
    struct {
        uint8_t khr_surface : 1;
        uint8_t khr_swapchain : 1;
        uint8_t khr_display : 1;
        uint8_t khr_xlib_surface : 1;
        uint8_t khr_xcb_surface : 1;
        uint8_t khr_wayland_surface : 1;
        uint8_t khr_win32_surface : 1;
        uint8_t khr_video_queue : 1;
        uint8_t khr_get_physical_device_properties2 : 1;
        uint8_t khr_device_group_creation : 1;
        uint8_t khr_external_memory_capabilities : 1;
        uint8_t khr_external_semaphore_capabilities : 1;
        uint8_t khr_external_fence_capabilities : 1;
        uint8_t khr_performance_query : 1;
        uint8_t khr_get_surface_capabilities2 : 1;
        uint8_t khr_get_display_properties2 : 1;
        uint8_t khr_fragment_shading_rate : 1;
        uint8_t khr_surface_protected_capabilities : 1;
        uint8_t khr_portability_enumeration : 1;
        uint8_t ext_debug_report : 1;
        uint8_t ggp_stream_descriptor_surface : 1;
        uint8_t nv_external_memory_capabilities : 1;
        uint8_t ext_validation_flags : 1;
        uint8_t nn_vi_surface : 1;
        uint8_t ext_direct_mode_display : 1;
        uint8_t ext_acquire_xlib_display : 1;
        uint8_t ext_display_surface_counter : 1;
        uint8_t ext_swapchain_colorspace : 1;
        uint8_t mvk_ios_surface : 1;
        uint8_t mvk_macos_surface : 1;
        uint8_t ext_debug_utils : 1;
        uint8_t ext_sample_locations : 1;
        uint8_t ext_calibrated_timestamps : 1;
        uint8_t fuchsia_imagepipe_surface : 1;
        uint8_t ext_metal_surface : 1;
        uint8_t ext_tooling_info : 1;
        uint8_t ext_validation_features : 1;
        uint8_t nv_cooperative_matrix : 1;
        uint8_t nv_coverage_reduction_mode : 1;
        uint8_t ext_full_screen_exclusive : 1;
        uint8_t ext_headless_surface : 1;
        uint8_t ext_acquire_drm_display : 1;
        uint8_t nv_acquire_winrt_display : 1;
        uint8_t ext_directfb_surface : 1;
        uint8_t qnx_screen_surface : 1;
        uint8_t google_surfaceless_query : 1;
    };
    uint64_t padding[4];
};

// Union of all device extensions that the loader has to override at least one command for
union loader_device_extension_enables {
    struct {
        uint8_t khr_swapchain : 1;
        uint8_t khr_display_swapchain : 1;
        uint8_t khr_device_group : 1;
        uint8_t ext_debug_marker : 1;
        uint8_t ext_debug_utils : 1;
        uint8_t ext_full_screen_exclusive : 1;
    };
    uint64_t padding[4];
};

// clang-format on

