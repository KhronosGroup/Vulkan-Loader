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
struct loader_scanned_icd;
struct loader_extension_list;
struct loader_dev_dispatch_table;
struct loader_physical_device_term;


// Check to determine support of instance extensions by a driver.
void instance_extensions_supported_by_driver(struct loader_scanned_icd* scanned_icd, struct loader_extension_list *ext_list);

// Check to determine support of device extensions by a physical device.
VkResult device_extensions_supported_by_physical_device(struct loader_physical_device_term* phys_dev_term);

// Extension interception for vkCreateInstance function, so we can properly detect and
// enable any instance extension information for extensions we know about.
void extensions_create_instance(struct loader_instance *ptr_instance, const VkInstanceCreateInfo *pCreateInfo);

// Extension interception for vkCreateDevice function, so we can properly detect and
// enable any device extension information for extensions we know about.
void extensions_create_device(struct loader_device *dev, const struct loader_physical_device_term *phys_dev_term,
                              const VkDeviceCreateInfo *pCreateInfo);

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

// Struct of all known instance extensions that the loader may need to refer to when
// making function pointer decisions.
struct loader_instance_extension_enables {
    int khr_surface : 2;
    int khr_display : 2;
    int khr_xlib_surface : 2;
    int khr_xcb_surface : 2;
    int khr_wayland_surface : 2;
    int khr_win32_surface : 2;
    int khr_get_physical_device_properties2 : 2;
    int khr_device_group_creation : 2;
    int khr_external_memory_capabilities : 2;
    int khr_external_semaphore_capabilities : 2;
    int khr_external_fence_capabilities : 2;
    int khr_get_surface_capabilities2 : 2;
    int khr_get_display_properties2 : 2;
    int khr_surface_protected_capabilities : 2;
    int khr_portability_enumeration : 2;
    int ext_debug_report : 2;
    int ggp_stream_descriptor_surface : 2;
    int nv_external_memory_capabilities : 2;
    int ext_validation_flags : 2;
    int nn_vi_surface : 2;
    int ext_direct_mode_display : 2;
    int ext_acquire_xlib_display : 2;
    int ext_display_surface_counter : 2;
    int ext_swapchain_colorspace : 2;
    int mvk_ios_surface : 2;
    int mvk_macos_surface : 2;
    int ext_debug_utils : 2;
    int fuchsia_imagepipe_surface : 2;
    int ext_metal_surface : 2;
    int ext_validation_features : 2;
    int ext_headless_surface : 2;
    int ext_acquire_drm_display : 2;
    int ext_directfb_surface : 2;
    int qnx_screen_surface : 2;
    int google_surfaceless_query : 2;
};

// Struct of all device extensions that the loader has to override at least one command for
struct loader_device_extension_enables {
    int khr_swapchain : 2;
    int khr_display_swapchain : 2;
    int khr_device_group : 2;
    int ext_debug_marker : 2;
    int ext_debug_utils : 2;
    int ext_full_screen_exclusive : 2;
};

struct loader_driver_device_extension_enables {
    int khr_swapchain : 2;
    int khr_display_swapchain : 2;
    int khr_device_group : 2;
    int ext_debug_marker : 2;
    int ext_debug_utils : 2;
    int ext_full_screen_exclusive : 2;
    int khr_video_queue : 2;
    int khr_performance_query : 2;
    int khr_fragment_shading_rate : 2;
    int ext_sample_locations : 2;
    int ext_calibrated_timestamps : 2;
    int ext_tooling_info : 2;
    int nv_cooperative_matrix : 2;
    int nv_coverage_reduction_mode : 2;
    int nv_acquire_winrt_display : 2;
};

// clang-format on

