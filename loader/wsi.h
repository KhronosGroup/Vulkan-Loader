/*
 * Copyright (c) 2015-2021 The Khronos Group Inc.
 * Copyright (c) 2015-2021 Valve Corporation
 * Copyright (c) 2015-2021 LunarG, Inc.
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
 * Author: Ian Elliott <ian@lunarg.com>
 * Author: Charles Giessen <charles@lunarg.com>
 *
 */

#pragma once

#include "loader_common.h"

// The first ICD/Loader interface that support querying the SurfaceKHR from
// the ICDs.
#define ICD_VER_SUPPORTS_ICD_SURFACE_KHR 3

typedef struct {
    // This union holds the data that drivers which use ICD interface version 2 and before expect. This is so they can dereference
    // VkSurfaceKHR to get this struct and access the creation parameters used in subsequent API calls, such as get surface formats
    // & get surface present modes.
    // Thus, these members need to stay here in order to preserve ABI compatibility.
    union {
#if defined(VK_USE_PLATFORM_WAYLAND_KHR)
        VkIcdSurfaceWayland wayland_surf;
#endif  // VK_USE_PLATFORM_WAYLAND_KHR
#if defined(VK_USE_PLATFORM_WIN32_KHR)
        VkIcdSurfaceWin32 win_surf;
#endif  // VK_USE_PLATFORM_WIN32_KHR
#if defined(VK_USE_PLATFORM_XCB_KHR)
        VkIcdSurfaceXcb xcb_surf;
#endif  // VK_USE_PLATFORM_XCB_KHR
#if defined(VK_USE_PLATFORM_XLIB_KHR)
        VkIcdSurfaceXlib xlib_surf;
#endif  // VK_USE_PLATFORM_XLIB_KHR
#if defined(VK_USE_PLATFORM_DIRECTFB_EXT)
        VkIcdSurfaceDirectFB directfb_surf;
#endif  // VK_USE_PLATFORM_DIRECTFB_EXT
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
        VkIcdSurfaceAndroid android_surf;
#endif  // VK_USE_PLATFORM_ANDROID_KHR
#if defined(VK_USE_PLATFORM_MACOS_MVK)
        VkIcdSurfaceMacOS macos_surf;
#endif  // VK_USE_PLATFORM_MACOS_MVK
#if defined(VK_USE_PLATFORM_IOS_MVK)
        VkIcdSurfaceIOS ios_surf;
#endif  // VK_USE_PLATFORM_MACOS_MVK
#if defined(VK_USE_PLATFORM_GGP)
        VkIcdSurfaceGgp ggp_surf;
#endif  // VK_USE_PLATFORM_GGP
#if defined(VK_USE_PLATFORM_FUCHSIA)
        VkIcdSurfaceImagePipe imagepipe_surf;
#endif  // VK_USE_PLATFORM_FUCHSIA
#if defined(VK_USE_PLATFORM_METAL_EXT)
        VkIcdSurfaceMetal metal_surf;
#endif  // VK_USE_PLATFORM_METAL_EXT
#if defined(VK_USE_PLATFORM_SCREEN_QNX)
        VkIcdSurfaceScreen screen_surf;
#endif  // VK_USE_PLATFORM_SCREEN_QNX
#if defined(VK_USE_PLATFORM_VI_NN)
        VkIcdSurfaceVi vi_surf;
#endif  // VK_USE_PLATFORM_VI_NN
        VkIcdSurfaceDisplay display_surf;
        VkIcdSurfaceHeadless headless_surf;
#if defined(VK_USE_PLATFORM_UBM_SEC)
        VkIcdSurfaceUbm ubm_surf;
#endif  // VK_USE_PLATFORM_UBM_SEC

        // All of the specific ICD surface structures start with the base structure
        VkIcdSurfaceBase base;
    };
    uint32_t base_size;            // Size of VkIcdSurfaceBase
    uint32_t platform_size;        // Size of corresponding VkIcdSurfaceXXX
    uint32_t non_platform_offset;  // Start offset to base_size
    uint32_t entire_size;          // Size of entire VkIcdSurface
    uint32_t surface_index;        // This surface's index into each drivers list of created surfaces

    bool callbacks_valid;             // Need to store if a VkAllocationCallbacks pointer was used.
    VkAllocationCallbacks callbacks;  // Storage for the callbacks

    // The create info parameters captured by the union in VkIcdSurface are insufficient, as they do not
    // capture every single parameter from the create info (e.g. flags that did not have any flags defined
    // at the time) and they also cannot capture extension structures, so we are including a separate
    // pointer that will hold a copy of the original creation structure chain. This chain can only capture
    // known extension structures and preferably should be auto-generated, but the list of extension
    // structures is rather limited (there is only a single display surface creation structure extension
    // at the time of writing).
    uint8_t* create_info;

} VkIcdSurface;

VkResult wsi_unwrap_icd_surface(struct loader_icd_term* icd_term, VkSurfaceKHR* surface);

bool wsi_swapchain_instance_gpa(struct loader_instance* ptr_instance, const char* name, void** addr);

bool wsi_unsupported_instance_extension(const VkExtensionProperties* ext_prop);

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo,
                                                             const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain);

VKAPI_ATTR void VKAPI_CALL terminator_DestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface,
                                                        const VkAllocationCallbacks* pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice,
                                                                             uint32_t queueFamilyIndex, VkSurfaceKHR surface,
                                                                             VkBool32* pSupported);

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice,
                                                                                  VkSurfaceKHR surface,
                                                                                  VkSurfaceCapabilitiesKHR* pSurfaceCapabilities);

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                                                             uint32_t* pSurfaceFormatCount,
                                                                             VkSurfaceFormatKHR* pSurfaceFormats);

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice,
                                                                                  VkSurfaceKHR surface, uint32_t* pPresentModeCount,
                                                                                  VkPresentModeKHR* pPresentModes);

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetDeviceGroupSurfacePresentModesKHR(VkDevice device, VkSurfaceKHR surface,
                                                                               VkDeviceGroupPresentModeFlagsKHR* pModes);

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateSharedSwapchainsKHR(VkDevice device, uint32_t swapchainCount,
                                                                    const VkSwapchainCreateInfoKHR* pCreateInfos,
                                                                    const VkAllocationCallbacks* pAllocator,
                                                                    VkSwapchainKHR* pSwapchains);

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDevicePresentRectanglesKHR(VkPhysicalDevice physicalDevice,
                                                                                VkSurfaceKHR surface, uint32_t* pRectCount,
                                                                                VkRect2D* pRects);

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceDisplayProperties2KHR(VkPhysicalDevice physicalDevice,
                                                                                 uint32_t* pPropertyCount,
                                                                                 VkDisplayProperties2KHR* pProperties);

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceDisplayPlaneProperties2KHR(VkPhysicalDevice physicalDevice,
                                                                                      uint32_t* pPropertyCount,
                                                                                      VkDisplayPlaneProperties2KHR* pProperties);

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetDisplayModeProperties2KHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display,
                                                                       uint32_t* pPropertyCount,
                                                                       VkDisplayModeProperties2KHR* pProperties);

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetDisplayPlaneCapabilities2KHR(VkPhysicalDevice physicalDevice,
                                                                          const VkDisplayPlaneInfo2KHR* pDisplayPlaneInfo,
                                                                          VkDisplayPlaneCapabilities2KHR* pCapabilities);

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceSurfaceCapabilities2KHR(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
    VkSurfaceCapabilities2KHR* pSurfaceCapabilities);

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceSurfaceFormats2KHR(VkPhysicalDevice physicalDevice,
                                                                              const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
                                                                              uint32_t* pSurfaceFormatCount,
                                                                              VkSurfaceFormat2KHR* pSurfaceFormats);

VkResult allocate_icd_surface_struct(struct loader_instance* instance, size_t base_size, size_t platform_size,
                                     const VkAllocationCallbacks* pAllocator, VkIcdSurface** out_icd_surface);

struct loader_struct_type_info {
    VkStructureType stype;
    size_t size;
};

VkResult copy_surface_create_info(struct loader_instance* loader_inst, VkIcdSurface* icd_surface, const void* create_info,
                                  size_t struct_type_info_count, const struct loader_struct_type_info* struct_type_info,
                                  const char* base_struct_name, const VkAllocationCallbacks* pAllocator);

// VK_DEFINE_NON_DISPATCHABLE_HANDLE is a real pointer on 64-bit builds and a uint64_t on 32-bit builds;
// round-tripping through uintptr_t is required to convert a loader-internal pointer into the handle type.
static inline VkSurfaceKHR wrap_surface_handle(void* icd_surface) {
    return (VkSurfaceKHR)(uintptr_t)icd_surface;  // NOLINT(performance-no-int-to-ptr)
}

void cleanup_surface_creation(struct loader_instance* loader_inst, VkResult result, VkIcdSurface* icd_surface,
                              const VkAllocationCallbacks* pAllocator);
