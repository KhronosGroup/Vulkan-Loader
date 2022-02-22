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

struct loader_device;
struct loader_icd_term;


// Dispatch table properly filled in with appropriate terminators for the
// supported extensions.
extern const VkLayerInstanceDispatchTable instance_term_disp;


VKAPI_ATTR bool VKAPI_CALL loader_icd_init_entries(struct loader_icd_term *icd_term, VkInstance inst,
                                                   const PFN_vkGetInstanceProcAddr fp_gipa);

// Extension interception for vkGetDeviceProcAddr function, so we can return
// an appropriate terminator if this is one of those few device commands requiring
// a terminator.
PFN_vkVoidFunction get_extension_device_proc_terminator(struct loader_device *dev, const char *pName);


// ------------------------------------------------------------------
// ---- Vulkan instance extension VK_EXT_debug_utils terminators ----
// ------------------------------------------------------------------

VKAPI_ATTR VkResult VKAPI_CALL terminator_SetDebugUtilsObjectNameEXT(
    VkDevice                                    device,
    const VkDebugUtilsObjectNameInfoEXT*        pNameInfo);
VKAPI_ATTR VkResult VKAPI_CALL terminator_SetDebugUtilsObjectTagEXT(
    VkDevice                                    device,
    const VkDebugUtilsObjectTagInfoEXT*         pTagInfo);
VKAPI_ATTR void VKAPI_CALL terminator_QueueBeginDebugUtilsLabelEXT(
    VkQueue                                     queue,
    const VkDebugUtilsLabelEXT*                 pLabelInfo);
VKAPI_ATTR void VKAPI_CALL terminator_QueueEndDebugUtilsLabelEXT(
    VkQueue                                     queue);
VKAPI_ATTR void VKAPI_CALL terminator_QueueInsertDebugUtilsLabelEXT(
    VkQueue                                     queue,
    const VkDebugUtilsLabelEXT*                 pLabelInfo);
VKAPI_ATTR void VKAPI_CALL terminator_CmdBeginDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugUtilsLabelEXT*                 pLabelInfo);
VKAPI_ATTR void VKAPI_CALL terminator_CmdEndDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer);
VKAPI_ATTR void VKAPI_CALL terminator_CmdInsertDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugUtilsLabelEXT*                 pLabelInfo);

// ICD function pointer dispatch table
struct loader_icd_term_dispatch {

    // ---- Vulkan API version 1.0  ----
    PFN_vkCreateInstance CreateInstance;
    PFN_vkDestroyInstance DestroyInstance;
    PFN_vkEnumeratePhysicalDevices EnumeratePhysicalDevices;
    PFN_vkGetPhysicalDeviceFeatures GetPhysicalDeviceFeatures;
    PFN_vkGetPhysicalDeviceFormatProperties GetPhysicalDeviceFormatProperties;
    PFN_vkGetPhysicalDeviceImageFormatProperties GetPhysicalDeviceImageFormatProperties;
    PFN_vkGetPhysicalDeviceProperties GetPhysicalDeviceProperties;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties GetPhysicalDeviceQueueFamilyProperties;
    PFN_vkGetPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties;
    PFN_vkCreateDevice CreateDevice;
    PFN_vkEnumerateInstanceExtensionProperties EnumerateInstanceExtensionProperties;
    PFN_vkEnumerateDeviceExtensionProperties EnumerateDeviceExtensionProperties;
    PFN_vkEnumerateInstanceLayerProperties EnumerateInstanceLayerProperties;
    PFN_vkGetPhysicalDeviceSparseImageFormatProperties GetPhysicalDeviceSparseImageFormatProperties;
    PFN_vkGetDeviceProcAddr GetDeviceProcAddr;
    PFN_vkDestroyDevice DestroyDevice;

    // ---- Vulkan API version 1.1  ----
    PFN_vkEnumerateInstanceVersion EnumerateInstanceVersion;
    PFN_vkEnumeratePhysicalDeviceGroups EnumeratePhysicalDeviceGroups;
    PFN_vkGetPhysicalDeviceFeatures2 GetPhysicalDeviceFeatures2;
    PFN_vkGetPhysicalDeviceProperties2 GetPhysicalDeviceProperties2;
    PFN_vkGetPhysicalDeviceFormatProperties2 GetPhysicalDeviceFormatProperties2;
    PFN_vkGetPhysicalDeviceImageFormatProperties2 GetPhysicalDeviceImageFormatProperties2;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties2 GetPhysicalDeviceQueueFamilyProperties2;
    PFN_vkGetPhysicalDeviceMemoryProperties2 GetPhysicalDeviceMemoryProperties2;
    PFN_vkGetPhysicalDeviceSparseImageFormatProperties2 GetPhysicalDeviceSparseImageFormatProperties2;
    PFN_vkGetPhysicalDeviceExternalBufferProperties GetPhysicalDeviceExternalBufferProperties;
    PFN_vkGetPhysicalDeviceExternalFenceProperties GetPhysicalDeviceExternalFenceProperties;
    PFN_vkGetPhysicalDeviceExternalSemaphoreProperties GetPhysicalDeviceExternalSemaphoreProperties;

    // ---- Vulkan API version 1.3  ----
    PFN_vkGetPhysicalDeviceToolProperties GetPhysicalDeviceToolProperties;

    // ---- Vulkan instance extension VK_KHR_surface  ----
    PFN_vkDestroySurfaceKHR DestroySurfaceKHR;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR GetPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR GetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR GetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR GetPhysicalDeviceSurfacePresentModesKHR;

    // ---- Vulkan device extension VK_KHR_swapchain  ----
    PFN_vkCreateSwapchainKHR CreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR DestroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR GetSwapchainImagesKHR;
    PFN_vkAcquireNextImageKHR AcquireNextImageKHR;
    PFN_vkQueuePresentKHR QueuePresentKHR;
    PFN_vkGetDeviceGroupPresentCapabilitiesKHR GetDeviceGroupPresentCapabilitiesKHR;
    PFN_vkGetDeviceGroupSurfacePresentModesKHR GetDeviceGroupSurfacePresentModesKHR;
    PFN_vkGetPhysicalDevicePresentRectanglesKHR GetPhysicalDevicePresentRectanglesKHR;
    PFN_vkAcquireNextImage2KHR AcquireNextImage2KHR;

    // ---- Vulkan instance extension VK_KHR_display  ----
    PFN_vkGetPhysicalDeviceDisplayPropertiesKHR GetPhysicalDeviceDisplayPropertiesKHR;
    PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR GetPhysicalDeviceDisplayPlanePropertiesKHR;
    PFN_vkGetDisplayPlaneSupportedDisplaysKHR GetDisplayPlaneSupportedDisplaysKHR;
    PFN_vkGetDisplayModePropertiesKHR GetDisplayModePropertiesKHR;
    PFN_vkCreateDisplayModeKHR CreateDisplayModeKHR;
    PFN_vkGetDisplayPlaneCapabilitiesKHR GetDisplayPlaneCapabilitiesKHR;
    PFN_vkCreateDisplayPlaneSurfaceKHR CreateDisplayPlaneSurfaceKHR;

    // ---- Vulkan device extension VK_KHR_display_swapchain  ----
    PFN_vkCreateSharedSwapchainsKHR CreateSharedSwapchainsKHR;

    // ---- Vulkan instance extension VK_KHR_xlib_surface  ----
#ifdef VK_USE_PLATFORM_XLIB_KHR
    PFN_vkCreateXlibSurfaceKHR CreateXlibSurfaceKHR;
    PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR GetPhysicalDeviceXlibPresentationSupportKHR;
#endif // VK_USE_PLATFORM_XLIB_KHR

    // ---- Vulkan instance extension VK_KHR_xcb_surface  ----
#ifdef VK_USE_PLATFORM_XCB_KHR
    PFN_vkCreateXcbSurfaceKHR CreateXcbSurfaceKHR;
    PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR GetPhysicalDeviceXcbPresentationSupportKHR;
#endif // VK_USE_PLATFORM_XCB_KHR

    // ---- Vulkan instance extension VK_KHR_wayland_surface  ----
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    PFN_vkCreateWaylandSurfaceKHR CreateWaylandSurfaceKHR;
    PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR GetPhysicalDeviceWaylandPresentationSupportKHR;
#endif // VK_USE_PLATFORM_WAYLAND_KHR

    // ---- Vulkan instance extension VK_KHR_win32_surface  ----
#ifdef VK_USE_PLATFORM_WIN32_KHR
    PFN_vkCreateWin32SurfaceKHR CreateWin32SurfaceKHR;
    PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR GetPhysicalDeviceWin32PresentationSupportKHR;
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ---- Vulkan device extension VK_KHR_video_queue  ----
#ifdef VK_ENABLE_BETA_EXTENSIONS
    PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR GetPhysicalDeviceVideoCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR GetPhysicalDeviceVideoFormatPropertiesKHR;
#endif // VK_ENABLE_BETA_EXTENSIONS

    // ---- Vulkan instance extension VK_KHR_get_physical_device_properties2  ----
    PFN_vkGetPhysicalDeviceFeatures2KHR GetPhysicalDeviceFeatures2KHR;
    PFN_vkGetPhysicalDeviceProperties2KHR GetPhysicalDeviceProperties2KHR;
    PFN_vkGetPhysicalDeviceFormatProperties2KHR GetPhysicalDeviceFormatProperties2KHR;
    PFN_vkGetPhysicalDeviceImageFormatProperties2KHR GetPhysicalDeviceImageFormatProperties2KHR;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR GetPhysicalDeviceQueueFamilyProperties2KHR;
    PFN_vkGetPhysicalDeviceMemoryProperties2KHR GetPhysicalDeviceMemoryProperties2KHR;
    PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR GetPhysicalDeviceSparseImageFormatProperties2KHR;

    // ---- Vulkan instance extension VK_KHR_device_group_creation  ----
    PFN_vkEnumeratePhysicalDeviceGroupsKHR EnumeratePhysicalDeviceGroupsKHR;

    // ---- Vulkan instance extension VK_KHR_external_memory_capabilities  ----
    PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR GetPhysicalDeviceExternalBufferPropertiesKHR;

    // ---- Vulkan instance extension VK_KHR_external_semaphore_capabilities  ----
    PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR GetPhysicalDeviceExternalSemaphorePropertiesKHR;

    // ---- Vulkan instance extension VK_KHR_external_fence_capabilities  ----
    PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR GetPhysicalDeviceExternalFencePropertiesKHR;

    // ---- Vulkan device extension VK_KHR_performance_query  ----
    PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR;
    PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR;

    // ---- Vulkan instance extension VK_KHR_get_surface_capabilities2  ----
    PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR GetPhysicalDeviceSurfaceCapabilities2KHR;
    PFN_vkGetPhysicalDeviceSurfaceFormats2KHR GetPhysicalDeviceSurfaceFormats2KHR;

    // ---- Vulkan instance extension VK_KHR_get_display_properties2  ----
    PFN_vkGetPhysicalDeviceDisplayProperties2KHR GetPhysicalDeviceDisplayProperties2KHR;
    PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR GetPhysicalDeviceDisplayPlaneProperties2KHR;
    PFN_vkGetDisplayModeProperties2KHR GetDisplayModeProperties2KHR;
    PFN_vkGetDisplayPlaneCapabilities2KHR GetDisplayPlaneCapabilities2KHR;

    // ---- Vulkan device extension VK_KHR_fragment_shading_rate  ----
    PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR GetPhysicalDeviceFragmentShadingRatesKHR;

    // ---- Vulkan instance extension VK_EXT_debug_report  ----
    PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallbackEXT;
    PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallbackEXT;
    PFN_vkDebugReportMessageEXT DebugReportMessageEXT;

    // ---- Vulkan device extension VK_EXT_debug_marker  ----
    PFN_vkDebugMarkerSetObjectTagEXT DebugMarkerSetObjectTagEXT;
    PFN_vkDebugMarkerSetObjectNameEXT DebugMarkerSetObjectNameEXT;

    // ---- Vulkan instance extension VK_GGP_stream_descriptor_surface  ----
#ifdef VK_USE_PLATFORM_GGP
    PFN_vkCreateStreamDescriptorSurfaceGGP CreateStreamDescriptorSurfaceGGP;
#endif // VK_USE_PLATFORM_GGP

    // ---- Vulkan instance extension VK_NV_external_memory_capabilities  ----
    PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV GetPhysicalDeviceExternalImageFormatPropertiesNV;

    // ---- Vulkan instance extension VK_NN_vi_surface  ----
#ifdef VK_USE_PLATFORM_VI_NN
    PFN_vkCreateViSurfaceNN CreateViSurfaceNN;
#endif // VK_USE_PLATFORM_VI_NN

    // ---- Vulkan instance extension VK_EXT_direct_mode_display  ----
    PFN_vkReleaseDisplayEXT ReleaseDisplayEXT;

    // ---- Vulkan instance extension VK_EXT_acquire_xlib_display  ----
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
    PFN_vkAcquireXlibDisplayEXT AcquireXlibDisplayEXT;
    PFN_vkGetRandROutputDisplayEXT GetRandROutputDisplayEXT;
#endif // VK_USE_PLATFORM_XLIB_XRANDR_EXT

    // ---- Vulkan instance extension VK_EXT_display_surface_counter  ----
    PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT GetPhysicalDeviceSurfaceCapabilities2EXT;

    // ---- Vulkan instance extension VK_MVK_ios_surface  ----
#ifdef VK_USE_PLATFORM_IOS_MVK
    PFN_vkCreateIOSSurfaceMVK CreateIOSSurfaceMVK;
#endif // VK_USE_PLATFORM_IOS_MVK

    // ---- Vulkan instance extension VK_MVK_macos_surface  ----
#ifdef VK_USE_PLATFORM_MACOS_MVK
    PFN_vkCreateMacOSSurfaceMVK CreateMacOSSurfaceMVK;
#endif // VK_USE_PLATFORM_MACOS_MVK

    // ---- Vulkan instance extension VK_EXT_debug_utils  ----
    PFN_vkSetDebugUtilsObjectNameEXT SetDebugUtilsObjectNameEXT;
    PFN_vkSetDebugUtilsObjectTagEXT SetDebugUtilsObjectTagEXT;
    PFN_vkQueueBeginDebugUtilsLabelEXT QueueBeginDebugUtilsLabelEXT;
    PFN_vkQueueEndDebugUtilsLabelEXT QueueEndDebugUtilsLabelEXT;
    PFN_vkQueueInsertDebugUtilsLabelEXT QueueInsertDebugUtilsLabelEXT;
    PFN_vkCmdBeginDebugUtilsLabelEXT CmdBeginDebugUtilsLabelEXT;
    PFN_vkCmdEndDebugUtilsLabelEXT CmdEndDebugUtilsLabelEXT;
    PFN_vkCmdInsertDebugUtilsLabelEXT CmdInsertDebugUtilsLabelEXT;
    PFN_vkCreateDebugUtilsMessengerEXT CreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessengerEXT;
    PFN_vkSubmitDebugUtilsMessageEXT SubmitDebugUtilsMessageEXT;

    // ---- Vulkan device extension VK_EXT_sample_locations  ----
    PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT GetPhysicalDeviceMultisamplePropertiesEXT;

    // ---- Vulkan device extension VK_EXT_calibrated_timestamps  ----
    PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT GetPhysicalDeviceCalibrateableTimeDomainsEXT;

    // ---- Vulkan instance extension VK_FUCHSIA_imagepipe_surface  ----
#ifdef VK_USE_PLATFORM_FUCHSIA
    PFN_vkCreateImagePipeSurfaceFUCHSIA CreateImagePipeSurfaceFUCHSIA;
#endif // VK_USE_PLATFORM_FUCHSIA

    // ---- Vulkan instance extension VK_EXT_metal_surface  ----
#ifdef VK_USE_PLATFORM_METAL_EXT
    PFN_vkCreateMetalSurfaceEXT CreateMetalSurfaceEXT;
#endif // VK_USE_PLATFORM_METAL_EXT

    // ---- Vulkan device extension VK_EXT_tooling_info  ----
    PFN_vkGetPhysicalDeviceToolPropertiesEXT GetPhysicalDeviceToolPropertiesEXT;

    // ---- Vulkan device extension VK_NV_cooperative_matrix  ----
    PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV GetPhysicalDeviceCooperativeMatrixPropertiesNV;

    // ---- Vulkan device extension VK_NV_coverage_reduction_mode  ----
    PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV;

    // ---- Vulkan device extension VK_EXT_full_screen_exclusive  ----
#ifdef VK_USE_PLATFORM_WIN32_KHR
    PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT GetPhysicalDeviceSurfacePresentModes2EXT;
    PFN_vkAcquireFullScreenExclusiveModeEXT AcquireFullScreenExclusiveModeEXT;
    PFN_vkReleaseFullScreenExclusiveModeEXT ReleaseFullScreenExclusiveModeEXT;
    PFN_vkGetDeviceGroupSurfacePresentModes2EXT GetDeviceGroupSurfacePresentModes2EXT;
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ---- Vulkan instance extension VK_EXT_headless_surface  ----
    PFN_vkCreateHeadlessSurfaceEXT CreateHeadlessSurfaceEXT;

    // ---- Vulkan instance extension VK_EXT_acquire_drm_display  ----
    PFN_vkAcquireDrmDisplayEXT AcquireDrmDisplayEXT;
    PFN_vkGetDrmDisplayEXT GetDrmDisplayEXT;

    // ---- Vulkan device extension VK_NV_acquire_winrt_display  ----
#ifdef VK_USE_PLATFORM_WIN32_KHR
    PFN_vkAcquireWinrtDisplayNV AcquireWinrtDisplayNV;
    PFN_vkGetWinrtDisplayNV GetWinrtDisplayNV;
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ---- Vulkan instance extension VK_EXT_directfb_surface  ----
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
    PFN_vkCreateDirectFBSurfaceEXT CreateDirectFBSurfaceEXT;
    PFN_vkGetPhysicalDeviceDirectFBPresentationSupportEXT GetPhysicalDeviceDirectFBPresentationSupportEXT;
#endif // VK_USE_PLATFORM_DIRECTFB_EXT

    // ---- Vulkan instance extension VK_QNX_screen_surface  ----
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    PFN_vkCreateScreenSurfaceQNX CreateScreenSurfaceQNX;
    PFN_vkGetPhysicalDeviceScreenPresentationSupportQNX GetPhysicalDeviceScreenPresentationSupportQNX;
#endif // VK_USE_PLATFORM_SCREEN_QNX
};

// clang-format on

