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
 * Author: Jon Ashburn <jon@lunarg.com>
 * Author: Ian Elliott <ianelliott@google.com>
 * Author: Mark Lobodzinski <mark@lunarg.com>
 * Author: Lenny Komow <lenny@lunarg.com>
 * Author: Charles Giessen <charles@lunarg.com>
 */

#include <stdio.h>
#include <stdlib.h>

#include <vulkan/vk_icd.h>

#include "allocation.h"
#include "loader.h"
#include "log.h"
#include "vk_loader_platform.h"
#include "wsi.h"

// The first ICD/Loader interface that support querying the SurfaceKHR from
// the ICDs.
#define ICD_VER_SUPPORTS_ICD_SURFACE_KHR 3

void wsi_create_instance(struct loader_instance *loader_inst, const VkInstanceCreateInfo *pCreateInfo) {
    for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; i++) {
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_KHR_SURFACE_EXTENSION_NAME) == 0) {
            loader_inst->wsi_surface_enabled = true;
            continue;
        }
#ifdef VK_USE_PLATFORM_WIN32_KHR
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_KHR_WIN32_SURFACE_EXTENSION_NAME) == 0) {
            loader_inst->wsi_win32_surface_enabled = true;
            continue;
        }
#endif  // VK_USE_PLATFORM_WIN32_KHR
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME) == 0) {
            loader_inst->wsi_wayland_surface_enabled = true;
            continue;
        }
#endif  // VK_USE_PLATFORM_WAYLAND_KHR
#ifdef VK_USE_PLATFORM_XCB_KHR
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_KHR_XCB_SURFACE_EXTENSION_NAME) == 0) {
            loader_inst->wsi_xcb_surface_enabled = true;
            continue;
        }
#endif  // VK_USE_PLATFORM_XCB_KHR
#ifdef VK_USE_PLATFORM_XLIB_KHR
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_KHR_XLIB_SURFACE_EXTENSION_NAME) == 0) {
            loader_inst->wsi_xlib_surface_enabled = true;
            continue;
        }
#endif  // VK_USE_PLATFORM_XLIB_KHR
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_EXT_DIRECTFB_SURFACE_EXTENSION_NAME) == 0) {
            loader_inst->wsi_directfb_surface_enabled = true;
            continue;
        }
#endif  // VK_USE_PLATFORM_DIRECTFB_EXT
#ifdef VK_USE_PLATFORM_ANDROID_KHR
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_KHR_ANDROID_SURFACE_EXTENSION_NAME) == 0) {
            loader_inst->wsi_android_surface_enabled = true;
            continue;
        }
#endif  // VK_USE_PLATFORM_ANDROID_KHR
#ifdef VK_USE_PLATFORM_MACOS_MVK
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_MVK_MACOS_SURFACE_EXTENSION_NAME) == 0) {
            loader_inst->wsi_macos_surface_enabled = true;
            continue;
        }
#endif  // VK_USE_PLATFORM_MACOS_MVK
#ifdef VK_USE_PLATFORM_IOS_MVK
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_MVK_IOS_SURFACE_EXTENSION_NAME) == 0) {
            loader_inst->wsi_ios_surface_enabled = true;
            continue;
        }
#endif  // VK_USE_PLATFORM_IOS_MVK
#ifdef VK_USE_PLATFORM_GGP
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_GGP_STREAM_DESCRIPTOR_SURFACE_EXTENSION_NAME) == 0) {
            loader_inst->wsi_ggp_surface_enabled = true;
            continue;
        }
#endif  // VK_USE_PLATFORM_GGP
#ifdef VK_USE_PLATFORM_FUCHSIA
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_FUCHSIA_IMAGEPIPE_SURFACE_EXTENSION_NAME) == 0) {
            loader_inst->wsi_imagepipe_surface_enabled = true;
            continue;
        }
#endif  // VK_USE_PLATFORM_FUCHSIA
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME) == 0) {
            loader_inst->wsi_headless_surface_enabled = true;
            continue;
        }
#if defined(VK_USE_PLATFORM_METAL_EXT)
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_EXT_METAL_SURFACE_EXTENSION_NAME) == 0) {
            loader_inst->wsi_metal_surface_enabled = true;
            continue;
        }
#endif
#if defined(VK_USE_PLATFORM_SCREEN_QNX)
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_QNX_SCREEN_SURFACE_EXTENSION_NAME) == 0) {
            loader_inst->wsi_screen_surface_enabled = true;
            continue;
        }
#endif  // VK_USE_PLATFORM_SCREEN_QNX
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_KHR_DISPLAY_EXTENSION_NAME) == 0) {
            loader_inst->wsi_display_enabled = true;
            continue;
        }
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME) == 0) {
            loader_inst->wsi_display_props2_enabled = true;
            continue;
        }
    }
}

// Linux WSI surface extensions are not always compiled into the loader. (Assume
// for Windows the KHR_win32_surface is always compiled into loader). A given
// Linux build environment might not have the headers required for building one
// of the three extensions  (Xlib, Xcb, Wayland).  Thus, need to check if
// the built loader actually supports the particular Linux surface extension.
// If not supported by the built loader it will not be included in the list of
// enumerated instance extensions.  This solves the issue where an ICD or layer
// advertises support for a given Linux surface extension but the loader was not
// built to support the extension.
bool wsi_unsupported_instance_extension(const VkExtensionProperties *ext_prop) {
#ifndef VK_USE_PLATFORM_WAYLAND_KHR
    if (!strcmp(ext_prop->extensionName, "VK_KHR_wayland_surface")) return true;
#endif  // VK_USE_PLATFORM_WAYLAND_KHR
#ifndef VK_USE_PLATFORM_XCB_KHR
    if (!strcmp(ext_prop->extensionName, "VK_KHR_xcb_surface")) return true;
#endif  // VK_USE_PLATFORM_XCB_KHR
#ifndef VK_USE_PLATFORM_XLIB_KHR
    if (!strcmp(ext_prop->extensionName, "VK_KHR_xlib_surface")) return true;
#endif  // VK_USE_PLATFORM_XLIB_KHR
#ifndef VK_USE_PLATFORM_DIRECTFB_EXT
    if (!strcmp(ext_prop->extensionName, "VK_EXT_directfb_surface")) return true;
#endif  // VK_USE_PLATFORM_DIRECTFB_EXT
#ifndef VK_USE_PLATFORM_SCREEN_QNX
    if (!strcmp(ext_prop->extensionName, "VK_QNX_screen_surface")) return true;
#endif  // VK_USE_PLATFORM_WAYLAND_KHR

    return false;
}

// Functions for the VK_KHR_surface extension:

// This is the trampoline entrypoint for DestroySurfaceKHR
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface,
                                                             const VkAllocationCallbacks *pAllocator) {
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (NULL == loader_inst) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkDestroySurfaceKHR: Invalid instance [VUID-vkDestroySurfaceKHR-instance-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    loader_inst->disp->layer_inst_disp.DestroySurfaceKHR(loader_inst->instance, surface, pAllocator);
}

// TODO probably need to lock around all the loader_get_instance() calls.

// This is the instance chain terminator function for DestroySurfaceKHR
VKAPI_ATTR void VKAPI_CALL terminator_DestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface,
                                                        const VkAllocationCallbacks *pAllocator) {
    struct loader_instance *loader_inst = loader_get_instance(instance);

    VkIcdSurface *icd_surface = (VkIcdSurface *)(uintptr_t)surface;
    if (NULL != icd_surface) {
        if (NULL != icd_surface->real_icd_surfaces) {
            uint32_t i = 0;
            for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
                if (icd_term->scanned_icd->interface_version >= ICD_VER_SUPPORTS_ICD_SURFACE_KHR) {
                    if (NULL != icd_term->dispatch.DestroySurfaceKHR && (VkSurfaceKHR)NULL != icd_surface->real_icd_surfaces[i]) {
                        icd_term->dispatch.DestroySurfaceKHR(icd_term->instance, icd_surface->real_icd_surfaces[i], pAllocator);
                        icd_surface->real_icd_surfaces[i] = (VkSurfaceKHR)NULL;
                    }
                } else {
                    // The real_icd_surface for any ICD not supporting the
                    // proper interface version should be NULL.  If not, then
                    // we have a problem.
                    assert((VkSurfaceKHR)NULL == icd_surface->real_icd_surfaces[i]);
                }
            }
            loader_instance_heap_free(loader_inst, icd_surface->real_icd_surfaces);
        }

        loader_instance_heap_free(loader_inst, (void *)(uintptr_t)surface);
    }
}

// This is the trampoline entrypoint for GetPhysicalDeviceSurfaceSupportKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                  uint32_t queueFamilyIndex, VkSurfaceKHR surface,
                                                                                  VkBool32 *pSupported) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetPhysicalDeviceSurfaceSupportKHR: Invalid physicalDevice "
                   "[VUID-vkGetPhysicalDeviceSurfaceSupportKHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    VkResult res = disp->GetPhysicalDeviceSurfaceSupportKHR(unwrapped_phys_dev, queueFamilyIndex, surface, pSupported);
    return res;
}

// This is the instance chain terminator function for
// GetPhysicalDeviceSurfaceSupportKHR
VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice,
                                                                             uint32_t queueFamilyIndex, VkSurfaceKHR surface,
                                                                             VkBool32 *pSupported) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    struct loader_instance *loader_inst = (struct loader_instance *)icd_term->this_instance;
    if (!loader_inst->wsi_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_surface extension not enabled. vkGetPhysicalDeviceSurfaceSupportKHR not executed!\n");
        return VK_SUCCESS;
    }

    if (NULL == pSupported) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "NULL pointer passed into vkGetPhysicalDeviceSurfaceSupportKHR for pSupported!\n");
        abort();
    }
    *pSupported = false;

    if (NULL == icd_term->dispatch.GetPhysicalDeviceSurfaceSupportKHR) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device is not exporting vkGetPhysicalDeviceSurfaceSupportKHR!\n");
        abort();
    }

    VkIcdSurface *icd_surface = (VkIcdSurface *)(uintptr_t)surface;
    if (NULL != icd_surface->real_icd_surfaces && (VkSurfaceKHR)NULL != icd_surface->real_icd_surfaces[phys_dev_term->icd_index]) {
        return icd_term->dispatch.GetPhysicalDeviceSurfaceSupportKHR(
            phys_dev_term->phys_dev, queueFamilyIndex, icd_surface->real_icd_surfaces[phys_dev_term->icd_index], pSupported);
    }

    return icd_term->dispatch.GetPhysicalDeviceSurfaceSupportKHR(phys_dev_term->phys_dev, queueFamilyIndex, surface, pSupported);
}

// This is the trampoline entrypoint for GetPhysicalDeviceSurfaceCapabilitiesKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR *pSurfaceCapabilities) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetPhysicalDeviceSurfaceCapabilitiesKHR: Invalid physicalDevice "
                   "[VUID-vkGetPhysicalDeviceSurfaceCapabilitiesKHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    VkResult res = disp->GetPhysicalDeviceSurfaceCapabilitiesKHR(unwrapped_phys_dev, surface, pSurfaceCapabilities);
    return res;
}

// This is the instance chain terminator function for
// GetPhysicalDeviceSurfaceCapabilitiesKHR
VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice,
                                                                                  VkSurfaceKHR surface,
                                                                                  VkSurfaceCapabilitiesKHR *pSurfaceCapabilities) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    struct loader_instance *loader_inst = (struct loader_instance *)icd_term->this_instance;
    if (!loader_inst->wsi_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_surface extension not enabled. vkGetPhysicalDeviceSurfaceCapabilitiesKHR not executed!\n");
        return VK_SUCCESS;
    }

    if (NULL == pSurfaceCapabilities) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "NULL pointer passed into vkGetPhysicalDeviceSurfaceCapabilitiesKHR for pSurfaceCapabilities!\n");
        abort();
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceSurfaceCapabilitiesKHR) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device is not exporting vkGetPhysicalDeviceSurfaceCapabilitiesKHR!\n");
        abort();
    }

    VkIcdSurface *icd_surface = (VkIcdSurface *)(uintptr_t)surface;
    if (NULL != icd_surface->real_icd_surfaces && (VkSurfaceKHR)NULL != icd_surface->real_icd_surfaces[phys_dev_term->icd_index]) {
        return icd_term->dispatch.GetPhysicalDeviceSurfaceCapabilitiesKHR(
            phys_dev_term->phys_dev, icd_surface->real_icd_surfaces[phys_dev_term->icd_index], pSurfaceCapabilities);
    }

    return icd_term->dispatch.GetPhysicalDeviceSurfaceCapabilitiesKHR(phys_dev_term->phys_dev, surface, pSurfaceCapabilities);
}

// This is the trampoline entrypoint for GetPhysicalDeviceSurfaceFormatsKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice,
                                                                                  VkSurfaceKHR surface,
                                                                                  uint32_t *pSurfaceFormatCount,
                                                                                  VkSurfaceFormatKHR *pSurfaceFormats) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetPhysicalDeviceSurfaceFormatsKHR: Invalid physicalDevice "
                   "[VUID-vkGetPhysicalDeviceSurfaceFormatsKHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    VkResult res = disp->GetPhysicalDeviceSurfaceFormatsKHR(unwrapped_phys_dev, surface, pSurfaceFormatCount, pSurfaceFormats);
    return res;
}

// This is the instance chain terminator function for
// GetPhysicalDeviceSurfaceFormatsKHR
VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                                                             uint32_t *pSurfaceFormatCount,
                                                                             VkSurfaceFormatKHR *pSurfaceFormats) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    struct loader_instance *loader_inst = (struct loader_instance *)icd_term->this_instance;
    if (!loader_inst->wsi_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_surface extension not enabled. vkGetPhysicalDeviceSurfaceFormatsKHR not executed!\n");
        return VK_SUCCESS;
    }

    if (NULL == pSurfaceFormatCount) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "NULL pointer passed into vkGetPhysicalDeviceSurfaceFormatsKHR for pSurfaceFormatCount!\n");
        abort();
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceSurfaceFormatsKHR) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device is not exporting vkGetPhysicalDeviceSurfaceCapabilitiesKHR!\n");
        abort();
    }

    VkIcdSurface *icd_surface = (VkIcdSurface *)(uintptr_t)surface;
    if (NULL != icd_surface->real_icd_surfaces && (VkSurfaceKHR)NULL != icd_surface->real_icd_surfaces[phys_dev_term->icd_index]) {
        return icd_term->dispatch.GetPhysicalDeviceSurfaceFormatsKHR(phys_dev_term->phys_dev,
                                                                     icd_surface->real_icd_surfaces[phys_dev_term->icd_index],
                                                                     pSurfaceFormatCount, pSurfaceFormats);
    }

    return icd_term->dispatch.GetPhysicalDeviceSurfaceFormatsKHR(phys_dev_term->phys_dev, surface, pSurfaceFormatCount,
                                                                 pSurfaceFormats);
}

// This is the trampoline entrypoint for GetPhysicalDeviceSurfacePresentModesKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice,
                                                                                       VkSurfaceKHR surface,
                                                                                       uint32_t *pPresentModeCount,
                                                                                       VkPresentModeKHR *pPresentModes) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetPhysicalDeviceSurfacePresentModesKHR: Invalid physicalDevice "
                   "[VUID-vkGetPhysicalDeviceSurfacePresentModesKHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    VkResult res = disp->GetPhysicalDeviceSurfacePresentModesKHR(unwrapped_phys_dev, surface, pPresentModeCount, pPresentModes);
    return res;
}

// This is the instance chain terminator function for
// GetPhysicalDeviceSurfacePresentModesKHR
VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice,
                                                                                  VkSurfaceKHR surface, uint32_t *pPresentModeCount,
                                                                                  VkPresentModeKHR *pPresentModes) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    struct loader_instance *loader_inst = (struct loader_instance *)icd_term->this_instance;
    if (!loader_inst->wsi_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_surface extension not enabled. vkGetPhysicalDeviceSurfacePresentModesKHR not executed!\n");
        return VK_SUCCESS;
    }

    if (NULL == pPresentModeCount) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "NULL pointer passed into vkGetPhysicalDeviceSurfacePresentModesKHR for pPresentModeCount!\n");
        abort();
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceSurfacePresentModesKHR) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device is not exporting vkGetPhysicalDeviceSurfacePresentModesKHR!\n");
        abort();
    }

    VkIcdSurface *icd_surface = (VkIcdSurface *)(uintptr_t)surface;
    if (NULL != icd_surface->real_icd_surfaces && (VkSurfaceKHR)NULL != icd_surface->real_icd_surfaces[phys_dev_term->icd_index]) {
        return icd_term->dispatch.GetPhysicalDeviceSurfacePresentModesKHR(
            phys_dev_term->phys_dev, icd_surface->real_icd_surfaces[phys_dev_term->icd_index], pPresentModeCount, pPresentModes);
    }

    return icd_term->dispatch.GetPhysicalDeviceSurfacePresentModesKHR(phys_dev_term->phys_dev, surface, pPresentModeCount,
                                                                      pPresentModes);
}

// Functions for the VK_KHR_swapchain extension:

// This is the trampoline entrypoint for CreateSwapchainKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR *pCreateInfo,
                                                                  const VkAllocationCallbacks *pAllocator,
                                                                  VkSwapchainKHR *pSwapchain) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkCreateSwapchainKHR: Invalid device [VUID-vkCreateSwapchainKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR *pCreateInfo,
                                                             const VkAllocationCallbacks *pAllocator, VkSwapchainKHR *pSwapchain) {
    uint32_t icd_index = 0;
    struct loader_device *dev;
    struct loader_icd_term *icd_term = loader_get_icd_and_device(device, &dev, &icd_index);
    if (NULL != icd_term && NULL != icd_term->dispatch.CreateSwapchainKHR) {
        VkIcdSurface *icd_surface = (VkIcdSurface *)(uintptr_t)pCreateInfo->surface;
        if (NULL != icd_surface->real_icd_surfaces) {
            if ((VkSurfaceKHR)NULL != icd_surface->real_icd_surfaces[icd_index]) {
                // We found the ICD, and there is an ICD KHR surface
                // associated with it, so copy the CreateInfo struct
                // and point it at the ICD's surface.
                VkSwapchainCreateInfoKHR *pCreateCopy = loader_stack_alloc(sizeof(VkSwapchainCreateInfoKHR));
                if (NULL == pCreateCopy) {
                    return VK_ERROR_OUT_OF_HOST_MEMORY;
                }
                memcpy(pCreateCopy, pCreateInfo, sizeof(VkSwapchainCreateInfoKHR));
                pCreateCopy->surface = icd_surface->real_icd_surfaces[icd_index];
                return icd_term->dispatch.CreateSwapchainKHR(device, pCreateCopy, pAllocator, pSwapchain);
            }
        }
        return icd_term->dispatch.CreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain);
    }
    return VK_SUCCESS;
}

// This is the trampoline entrypoint for DestroySwapchainKHR
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain,
                                                               const VkAllocationCallbacks *pAllocator) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkDestroySwapchainKHR: Invalid device [VUID-vkDestroySwapchainKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp->DestroySwapchainKHR(device, swapchain, pAllocator);
}

// This is the trampoline entrypoint for GetSwapchainImagesKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain,
                                                                     uint32_t *pSwapchainImageCount, VkImage *pSwapchainImages) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetSwapchainImagesKHR: Invalid device [VUID-vkGetSwapchainImagesKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetSwapchainImagesKHR(device, swapchain, pSwapchainImageCount, pSwapchainImages);
}

// This is the trampoline entrypoint for AcquireNextImageKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkAcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout,
                                                                   VkSemaphore semaphore, VkFence fence, uint32_t *pImageIndex) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkAcquireNextImageKHR: Invalid device [VUID-vkAcquireNextImageKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->AcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, pImageIndex);
}

// This is the trampoline entrypoint for QueuePresentKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR *pPresentInfo) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(queue);
    if (NULL == disp) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkQueuePresentKHR: Invalid queue [VUID-vkQueuePresentKHR-queue-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->QueuePresentKHR(queue, pPresentInfo);
}

static VkIcdSurface *AllocateIcdSurfaceStruct(struct loader_instance *instance, size_t base_size, size_t platform_size) {
    // Next, if so, proceed with the implementation of this function:
    VkIcdSurface *pIcdSurface = loader_instance_heap_alloc(instance, sizeof(VkIcdSurface), VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
    if (pIcdSurface != NULL) {
        // Setup the new sizes and offsets so we can grow the structures in the
        // future without having problems
        pIcdSurface->base_size = (uint32_t)base_size;
        pIcdSurface->platform_size = (uint32_t)platform_size;
        pIcdSurface->non_platform_offset = (uint32_t)((uint8_t *)(&pIcdSurface->base_size) - (uint8_t *)pIcdSurface);
        pIcdSurface->entire_size = sizeof(VkIcdSurface);

        pIcdSurface->real_icd_surfaces = loader_instance_heap_alloc(instance, sizeof(VkSurfaceKHR) * instance->total_icd_count,
                                                                    VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
        if (pIcdSurface->real_icd_surfaces == NULL) {
            loader_instance_heap_free(instance, pIcdSurface);
            pIcdSurface = NULL;
        } else {
            memset(pIcdSurface->real_icd_surfaces, 0, sizeof(VkSurfaceKHR) * instance->total_icd_count);
        }
    }
    return pIcdSurface;
}

#ifdef VK_USE_PLATFORM_WIN32_KHR

// Functions for the VK_KHR_win32_surface extension:

// This is the trampoline entrypoint for CreateWin32SurfaceKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateWin32SurfaceKHR(VkInstance instance,
                                                                     const VkWin32SurfaceCreateInfoKHR *pCreateInfo,
                                                                     const VkAllocationCallbacks *pAllocator,
                                                                     VkSurfaceKHR *pSurface) {
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (NULL == loader_inst) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkCreateWin32SurfaceKHR: Invalid instance [VUID-vkCreateWin32SurfaceKHR-instance-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return loader_inst->disp->layer_inst_disp.CreateWin32SurfaceKHR(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

// This is the instance chain terminator function for CreateWin32SurfaceKHR
VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateWin32SurfaceKHR(VkInstance instance, const VkWin32SurfaceCreateInfoKHR *pCreateInfo,
                                                                const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface) {
    VkResult vkRes = VK_SUCCESS;
    VkIcdSurface *pIcdSurface = NULL;
    uint32_t i = 0;

    // Initialize pSurface to NULL just to be safe.
    *pSurface = VK_NULL_HANDLE;
    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (!loader_inst->wsi_win32_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_win32_surface extension not enabled. vkCreateWin32SurfaceKHR not executed!\n");
        vkRes = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    pIcdSurface = AllocateIcdSurfaceStruct(loader_inst, sizeof(pIcdSurface->win_surf.base), sizeof(pIcdSurface->win_surf));
    if (pIcdSurface == NULL) {
        vkRes = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    pIcdSurface->win_surf.base.platform = VK_ICD_WSI_PLATFORM_WIN32;
    pIcdSurface->win_surf.hinstance = pCreateInfo->hinstance;
    pIcdSurface->win_surf.hwnd = pCreateInfo->hwnd;

    // Loop through each ICD and determine if they need to create a surface
    for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
        if (icd_term->scanned_icd->interface_version >= ICD_VER_SUPPORTS_ICD_SURFACE_KHR) {
            if (NULL != icd_term->dispatch.CreateWin32SurfaceKHR) {
                vkRes = icd_term->dispatch.CreateWin32SurfaceKHR(icd_term->instance, pCreateInfo, pAllocator,
                                                                 &pIcdSurface->real_icd_surfaces[i]);
                if (VK_SUCCESS != vkRes) {
                    goto out;
                }
            }
        }
    }

    *pSurface = (VkSurfaceKHR)(pIcdSurface);

out:

    if (VK_SUCCESS != vkRes && NULL != pIcdSurface) {
        if (NULL != pIcdSurface->real_icd_surfaces) {
            i = 0;
            for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
                if ((VkSurfaceKHR)NULL != pIcdSurface->real_icd_surfaces[i] && NULL != icd_term->dispatch.DestroySurfaceKHR) {
                    icd_term->dispatch.DestroySurfaceKHR(icd_term->instance, pIcdSurface->real_icd_surfaces[i], pAllocator);
                }
            }
            loader_instance_heap_free(loader_inst, pIcdSurface->real_icd_surfaces);
        }
        loader_instance_heap_free(loader_inst, pIcdSurface);
    }

    return vkRes;
}

// This is the trampoline entrypoint for
// GetPhysicalDeviceWin32PresentationSupportKHR
LOADER_EXPORT VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceWin32PresentationSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                            uint32_t queueFamilyIndex) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetPhysicalDeviceWin32PresentationSupportKHR: Invalid physicalDevice "
                   "[VUID-vkGetPhysicalDeviceWin32PresentationSupportKHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceWin32PresentationSupportKHR(unwrapped_phys_dev, queueFamilyIndex);
}

// This is the instance chain terminator function for
// GetPhysicalDeviceWin32PresentationSupportKHR
VKAPI_ATTR VkBool32 VKAPI_CALL terminator_GetPhysicalDeviceWin32PresentationSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                       uint32_t queueFamilyIndex) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    struct loader_instance *loader_inst = (struct loader_instance *)icd_term->this_instance;
    if (!loader_inst->wsi_win32_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_win32_surface extension not enabled. vkGetPhysicalDeviceWin32PresentationSupportKHR not executed!\n");
        return VK_SUCCESS;
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceWin32PresentationSupportKHR) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device is not exporting vkGetPhysicalDeviceWin32PresentationSupportKHR!\n");
        abort();
    }

    return icd_term->dispatch.GetPhysicalDeviceWin32PresentationSupportKHR(phys_dev_term->phys_dev, queueFamilyIndex);
}
#endif  // VK_USE_PLATFORM_WIN32_KHR

#ifdef VK_USE_PLATFORM_WAYLAND_KHR

// This is the trampoline entrypoint for CreateWaylandSurfaceKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateWaylandSurfaceKHR(VkInstance instance,
                                                                       const VkWaylandSurfaceCreateInfoKHR *pCreateInfo,
                                                                       const VkAllocationCallbacks *pAllocator,
                                                                       VkSurfaceKHR *pSurface) {
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (NULL == loader_inst) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkCreateWaylandSurfaceKHR: Invalid instance [VUID-vkCreateWaylandSurfaceKHR-instance-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return loader_inst->disp->layer_inst_disp.CreateWaylandSurfaceKHR(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

// This is the instance chain terminator function for CreateWaylandSurfaceKHR
VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateWaylandSurfaceKHR(VkInstance instance,
                                                                  const VkWaylandSurfaceCreateInfoKHR *pCreateInfo,
                                                                  const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface) {
    VkResult vkRes = VK_SUCCESS;
    VkIcdSurface *pIcdSurface = NULL;
    uint32_t i = 0;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (!loader_inst->wsi_wayland_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_wayland_surface extension not enabled. vkCreateWaylandSurfaceKHR not executed!\n");
        vkRes = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    pIcdSurface = AllocateIcdSurfaceStruct(loader_inst, sizeof(pIcdSurface->wayland_surf.base), sizeof(pIcdSurface->wayland_surf));
    if (pIcdSurface == NULL) {
        vkRes = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    pIcdSurface->wayland_surf.base.platform = VK_ICD_WSI_PLATFORM_WAYLAND;
    pIcdSurface->wayland_surf.display = pCreateInfo->display;
    pIcdSurface->wayland_surf.surface = pCreateInfo->surface;

    // Loop through each ICD and determine if they need to create a surface
    for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
        if (icd_term->scanned_icd->interface_version >= ICD_VER_SUPPORTS_ICD_SURFACE_KHR) {
            if (NULL != icd_term->dispatch.CreateWaylandSurfaceKHR) {
                vkRes = icd_term->dispatch.CreateWaylandSurfaceKHR(icd_term->instance, pCreateInfo, pAllocator,
                                                                   &pIcdSurface->real_icd_surfaces[i]);
                if (VK_SUCCESS != vkRes) {
                    goto out;
                }
            }
        }
    }

    *pSurface = (VkSurfaceKHR)pIcdSurface;

out:

    if (VK_SUCCESS != vkRes && NULL != pIcdSurface) {
        if (NULL != pIcdSurface->real_icd_surfaces) {
            i = 0;
            for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
                if ((VkSurfaceKHR)NULL != pIcdSurface->real_icd_surfaces[i] && NULL != icd_term->dispatch.DestroySurfaceKHR) {
                    icd_term->dispatch.DestroySurfaceKHR(icd_term->instance, pIcdSurface->real_icd_surfaces[i], pAllocator);
                }
            }
            loader_instance_heap_free(loader_inst, pIcdSurface->real_icd_surfaces);
        }
        loader_instance_heap_free(loader_inst, pIcdSurface);
    }

    return vkRes;
}

// This is the trampoline entrypoint for
// GetPhysicalDeviceWaylandPresentationSupportKHR
LOADER_EXPORT VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceWaylandPresentationSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                              uint32_t queueFamilyIndex,
                                                                                              struct wl_display *display) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetPhysicalDeviceWaylandPresentationSupportKHR: Invalid physicalDevice "
                   "[VUID-vkGetPhysicalDeviceWaylandPresentationSupportKHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceWaylandPresentationSupportKHR(unwrapped_phys_dev, queueFamilyIndex, display);
}

// This is the instance chain terminator function for
// GetPhysicalDeviceWaylandPresentationSupportKHR
VKAPI_ATTR VkBool32 VKAPI_CALL terminator_GetPhysicalDeviceWaylandPresentationSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                         uint32_t queueFamilyIndex,
                                                                                         struct wl_display *display) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    struct loader_instance *loader_inst = (struct loader_instance *)icd_term->this_instance;
    if (!loader_inst->wsi_wayland_surface_enabled) {
        loader_log(
            loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
            "VK_KHR_wayland_surface extension not enabled. vkGetPhysicalDeviceWaylandPresentationSupportKHR not executed!\n");
        return VK_SUCCESS;
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceWaylandPresentationSupportKHR) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device is not exporting vkGetPhysicalDeviceWaylandPresentationSupportKHR!\n");
        abort();
    }

    return icd_term->dispatch.GetPhysicalDeviceWaylandPresentationSupportKHR(phys_dev_term->phys_dev, queueFamilyIndex, display);
}
#endif  // VK_USE_PLATFORM_WAYLAND_KHR

#ifdef VK_USE_PLATFORM_XCB_KHR

// Functions for the VK_KHR_xcb_surface extension:

// This is the trampoline entrypoint for CreateXcbSurfaceKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateXcbSurfaceKHR(VkInstance instance,
                                                                   const VkXcbSurfaceCreateInfoKHR *pCreateInfo,
                                                                   const VkAllocationCallbacks *pAllocator,
                                                                   VkSurfaceKHR *pSurface) {
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (NULL == loader_inst) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkCreateXcbSurfaceKHR: Invalid instance [VUID-vkCreateXcbSurfaceKHR-instance-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return loader_inst->disp->layer_inst_disp.CreateXcbSurfaceKHR(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

// This is the instance chain terminator function for CreateXcbSurfaceKHR
VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateXcbSurfaceKHR(VkInstance instance, const VkXcbSurfaceCreateInfoKHR *pCreateInfo,
                                                              const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface) {
    VkResult vkRes = VK_SUCCESS;
    VkIcdSurface *pIcdSurface = NULL;
    uint32_t i = 0;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (!loader_inst->wsi_xcb_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_xcb_surface extension not enabled. vkCreateXcbSurfaceKHR not executed!\n");
        vkRes = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    pIcdSurface = AllocateIcdSurfaceStruct(loader_inst, sizeof(pIcdSurface->xcb_surf.base), sizeof(pIcdSurface->xcb_surf));
    if (pIcdSurface == NULL) {
        vkRes = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    pIcdSurface->xcb_surf.base.platform = VK_ICD_WSI_PLATFORM_XCB;
    pIcdSurface->xcb_surf.connection = pCreateInfo->connection;
    pIcdSurface->xcb_surf.window = pCreateInfo->window;

    // Loop through each ICD and determine if they need to create a surface
    for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
        if (icd_term->scanned_icd->interface_version >= ICD_VER_SUPPORTS_ICD_SURFACE_KHR) {
            if (NULL != icd_term->dispatch.CreateXcbSurfaceKHR) {
                vkRes = icd_term->dispatch.CreateXcbSurfaceKHR(icd_term->instance, pCreateInfo, pAllocator,
                                                               &pIcdSurface->real_icd_surfaces[i]);
                if (VK_SUCCESS != vkRes) {
                    goto out;
                }
            }
        }
    }

    *pSurface = (VkSurfaceKHR)pIcdSurface;

out:

    if (VK_SUCCESS != vkRes && NULL != pIcdSurface) {
        if (NULL != pIcdSurface->real_icd_surfaces) {
            i = 0;
            for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
                if ((VkSurfaceKHR)NULL != pIcdSurface->real_icd_surfaces[i] && NULL != icd_term->dispatch.DestroySurfaceKHR) {
                    icd_term->dispatch.DestroySurfaceKHR(icd_term->instance, pIcdSurface->real_icd_surfaces[i], pAllocator);
                }
            }
            loader_instance_heap_free(loader_inst, pIcdSurface->real_icd_surfaces);
        }
        loader_instance_heap_free(loader_inst, pIcdSurface);
    }

    return vkRes;
}

// This is the trampoline entrypoint for
// GetPhysicalDeviceXcbPresentationSupportKHR
LOADER_EXPORT VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceXcbPresentationSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                          uint32_t queueFamilyIndex,
                                                                                          xcb_connection_t *connection,
                                                                                          xcb_visualid_t visual_id) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetPhysicalDeviceXcbPresentationSupportKHR: Invalid physicalDevice "
                   "[VUID-vkGetPhysicalDeviceXcbPresentationSupportKHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceXcbPresentationSupportKHR(unwrapped_phys_dev, queueFamilyIndex, connection, visual_id);
}

// This is the instance chain terminator function for
// GetPhysicalDeviceXcbPresentationSupportKHR
VKAPI_ATTR VkBool32 VKAPI_CALL terminator_GetPhysicalDeviceXcbPresentationSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                     uint32_t queueFamilyIndex,
                                                                                     xcb_connection_t *connection,
                                                                                     xcb_visualid_t visual_id) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    struct loader_instance *loader_inst = (struct loader_instance *)icd_term->this_instance;
    if (!loader_inst->wsi_xcb_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_xcb_surface extension not enabled. vkGetPhysicalDeviceXcbPresentationSupportKHR not executed!\n");
        return VK_SUCCESS;
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceXcbPresentationSupportKHR) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device is not exporting vkGetPhysicalDeviceXcbPresentationSupportKHR!\n");
        abort();
    }

    return icd_term->dispatch.GetPhysicalDeviceXcbPresentationSupportKHR(phys_dev_term->phys_dev, queueFamilyIndex, connection,
                                                                         visual_id);
}
#endif  // VK_USE_PLATFORM_XCB_KHR

#ifdef VK_USE_PLATFORM_XLIB_KHR

// Functions for the VK_KHR_xlib_surface extension:

// This is the trampoline entrypoint for CreateXlibSurfaceKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateXlibSurfaceKHR(VkInstance instance,
                                                                    const VkXlibSurfaceCreateInfoKHR *pCreateInfo,
                                                                    const VkAllocationCallbacks *pAllocator,
                                                                    VkSurfaceKHR *pSurface) {
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (NULL == loader_inst) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkCreateXlibSurfaceKHR: Invalid instance [VUID-vkCreateXlibSurfaceKHR-instance-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return loader_inst->disp->layer_inst_disp.CreateXlibSurfaceKHR(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

// This is the instance chain terminator function for CreateXlibSurfaceKHR
VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateXlibSurfaceKHR(VkInstance instance, const VkXlibSurfaceCreateInfoKHR *pCreateInfo,
                                                               const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface) {
    VkResult vkRes = VK_SUCCESS;
    VkIcdSurface *pIcdSurface = NULL;
    uint32_t i = 0;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (!loader_inst->wsi_xlib_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_xlib_surface extension not enabled. vkCreateXlibSurfaceKHR not executed!\n");
        vkRes = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    pIcdSurface = AllocateIcdSurfaceStruct(loader_inst, sizeof(pIcdSurface->xlib_surf.base), sizeof(pIcdSurface->xlib_surf));
    if (pIcdSurface == NULL) {
        vkRes = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    pIcdSurface->xlib_surf.base.platform = VK_ICD_WSI_PLATFORM_XLIB;
    pIcdSurface->xlib_surf.dpy = pCreateInfo->dpy;
    pIcdSurface->xlib_surf.window = pCreateInfo->window;

    // Loop through each ICD and determine if they need to create a surface
    for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
        if (icd_term->scanned_icd->interface_version >= ICD_VER_SUPPORTS_ICD_SURFACE_KHR) {
            if (NULL != icd_term->dispatch.CreateXlibSurfaceKHR) {
                vkRes = icd_term->dispatch.CreateXlibSurfaceKHR(icd_term->instance, pCreateInfo, pAllocator,
                                                                &pIcdSurface->real_icd_surfaces[i]);
                if (VK_SUCCESS != vkRes) {
                    goto out;
                }
            }
        }
    }

    *pSurface = (VkSurfaceKHR)pIcdSurface;

out:

    if (VK_SUCCESS != vkRes && NULL != pIcdSurface) {
        if (NULL != pIcdSurface->real_icd_surfaces) {
            i = 0;
            for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
                if ((VkSurfaceKHR)NULL != pIcdSurface->real_icd_surfaces[i] && NULL != icd_term->dispatch.DestroySurfaceKHR) {
                    icd_term->dispatch.DestroySurfaceKHR(icd_term->instance, pIcdSurface->real_icd_surfaces[i], pAllocator);
                }
            }
            loader_instance_heap_free(loader_inst, pIcdSurface->real_icd_surfaces);
        }
        loader_instance_heap_free(loader_inst, pIcdSurface);
    }

    return vkRes;
}

// This is the trampoline entrypoint for
// GetPhysicalDeviceXlibPresentationSupportKHR
LOADER_EXPORT VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceXlibPresentationSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                           uint32_t queueFamilyIndex, Display *dpy,
                                                                                           VisualID visualID) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetPhysicalDeviceXlibPresentationSupportKHR: Invalid physicalDevice "
                   "[VUID-vkGetPhysicalDeviceXlibPresentationSupportKHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceXlibPresentationSupportKHR(unwrapped_phys_dev, queueFamilyIndex, dpy, visualID);
}

// This is the instance chain terminator function for
// GetPhysicalDeviceXlibPresentationSupportKHR
VKAPI_ATTR VkBool32 VKAPI_CALL terminator_GetPhysicalDeviceXlibPresentationSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                      uint32_t queueFamilyIndex, Display *dpy,
                                                                                      VisualID visualID) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    struct loader_instance *loader_inst = (struct loader_instance *)icd_term->this_instance;
    if (!loader_inst->wsi_xlib_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_xlib_surface extension not enabled. vkGetPhysicalDeviceXlibPresentationSupportKHR not executed!\n");
        return VK_SUCCESS;
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceXlibPresentationSupportKHR) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device is not exporting vkGetPhysicalDeviceXlibPresentationSupportKHR!\n");
        abort();
    }

    return icd_term->dispatch.GetPhysicalDeviceXlibPresentationSupportKHR(phys_dev_term->phys_dev, queueFamilyIndex, dpy, visualID);
}
#endif  // VK_USE_PLATFORM_XLIB_KHR

#ifdef VK_USE_PLATFORM_DIRECTFB_EXT

// Functions for the VK_EXT_directfb_surface extension:

// This is the trampoline entrypoint for CreateDirectFBSurfaceEXT
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateDirectFBSurfaceEXT(VkInstance instance,
                                                                        const VkDirectFBSurfaceCreateInfoEXT *pCreateInfo,
                                                                        const VkAllocationCallbacks *pAllocator,
                                                                        VkSurfaceKHR *pSurface) {
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (NULL == loader_inst) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkCreateDirectFBSurfaceEXT: Invalid instance [VUID-vkCreateDirectFBSurfaceEXT-instance-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return loader_inst->disp->layer_inst_disp.CreateDirectFBSurfaceEXT(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

// This is the instance chain terminator function for CreateDirectFBSurfaceEXT
VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateDirectFBSurfaceEXT(VkInstance instance,
                                                                   const VkDirectFBSurfaceCreateInfoEXT *pCreateInfo,
                                                                   const VkAllocationCallbacks *pAllocator,
                                                                   VkSurfaceKHR *pSurface) {
    VkResult vkRes = VK_SUCCESS;
    VkIcdSurface *pIcdSurface = NULL;
    uint32_t i = 0;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (!loader_inst->wsi_directfb_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_EXT_directfb_surface extension not enabled. vkCreateDirectFBSurfaceEXT not executed!\n");
        vkRes = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    pIcdSurface =
        AllocateIcdSurfaceStruct(loader_inst, sizeof(pIcdSurface->directfb_surf.base), sizeof(pIcdSurface->directfb_surf));
    if (pIcdSurface == NULL) {
        vkRes = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    pIcdSurface->directfb_surf.base.platform = VK_ICD_WSI_PLATFORM_DIRECTFB;
    pIcdSurface->directfb_surf.dfb = pCreateInfo->dfb;
    pIcdSurface->directfb_surf.surface = pCreateInfo->surface;

    // Loop through each ICD and determine if they need to create a surface
    for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
        if (icd_term->scanned_icd->interface_version >= ICD_VER_SUPPORTS_ICD_SURFACE_KHR) {
            if (NULL != icd_term->dispatch.CreateDirectFBSurfaceEXT) {
                vkRes = icd_term->dispatch.CreateDirectFBSurfaceEXT(icd_term->instance, pCreateInfo, pAllocator,
                                                                    &pIcdSurface->real_icd_surfaces[i]);
                if (VK_SUCCESS != vkRes) {
                    goto out;
                }
            }
        }
    }

    *pSurface = (VkSurfaceKHR)pIcdSurface;

out:

    if (VK_SUCCESS != vkRes && NULL != pIcdSurface) {
        if (NULL != pIcdSurface->real_icd_surfaces) {
            i = 0;
            for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
                if ((VkSurfaceKHR)NULL != pIcdSurface->real_icd_surfaces[i] && NULL != icd_term->dispatch.DestroySurfaceKHR) {
                    icd_term->dispatch.DestroySurfaceKHR(icd_term->instance, pIcdSurface->real_icd_surfaces[i], pAllocator);
                }
            }
            loader_instance_heap_free(loader_inst, pIcdSurface->real_icd_surfaces);
        }
        loader_instance_heap_free(loader_inst, pIcdSurface);
    }

    return vkRes;
}

// This is the trampoline entrypoint for
// GetPhysicalDeviceDirectFBPresentationSupportEXT
LOADER_EXPORT VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceDirectFBPresentationSupportEXT(VkPhysicalDevice physicalDevice,
                                                                                               uint32_t queueFamilyIndex,
                                                                                               IDirectFB *dfb) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetPhysicalDeviceDirectFBPresentationSupportEXT: Invalid physicalDevice "
                   "[VUID-vkGetPhysicalDeviceDirectFBPresentationSupportEXT-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceDirectFBPresentationSupportEXT(unwrapped_phys_dev, queueFamilyIndex, dfb);
}

// This is the instance chain terminator function for
// GetPhysicalDeviceDirectFBPresentationSupportEXT
VKAPI_ATTR VkBool32 VKAPI_CALL terminator_GetPhysicalDeviceDirectFBPresentationSupportEXT(VkPhysicalDevice physicalDevice,
                                                                                          uint32_t queueFamilyIndex,
                                                                                          IDirectFB *dfb) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    struct loader_instance *loader_inst = (struct loader_instance *)icd_term->this_instance;
    if (!loader_inst->wsi_directfb_surface_enabled) {
        loader_log(
            loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
            "VK_EXT_directfb_surface extension not enabled. vkGetPhysicalDeviceWaylandPresentationSupportKHR not executed!\n");
        return VK_SUCCESS;
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceDirectFBPresentationSupportEXT) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device is not exporting vkGetPhysicalDeviceDirectFBPresentationSupportEXT!\n");
        abort();
    }

    return icd_term->dispatch.GetPhysicalDeviceDirectFBPresentationSupportEXT(phys_dev_term->phys_dev, queueFamilyIndex, dfb);
}

#endif  // VK_USE_PLATFORM_DIRECTFB_EXT

#ifdef VK_USE_PLATFORM_ANDROID_KHR

// Functions for the VK_KHR_android_surface extension:

// This is the trampoline entrypoint for CreateAndroidSurfaceKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateAndroidSurfaceKHR(VkInstance instance,
                                                                       const VkAndroidSurfaceCreateInfoKHR *pCreateInfo,
                                                                       const VkAllocationCallbacks *pAllocator,
                                                                       VkSurfaceKHR *pSurface) {
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (NULL == loader_inst) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkCreateAndroidSurfaceKHR: Invalid instance [VUID-vkCreateAndroidSurfaceKHR-instance-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return loader_inst->disp->layer_inst_disp.CreateAndroidSurfaceKHR(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

// This is the instance chain terminator function for CreateAndroidSurfaceKHR
VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateAndroidSurfaceKHR(VkInstance instance,
                                                                  const VkAndroidSurfaceCreateInfoKHR *pCreateInfo,
                                                                  const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (!loader_inst->wsi_display_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_display extension not enabled. vkCreateAndroidSurfaceKHR not executed!\n");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    // Next, if so, proceed with the implementation of this function:
    VkIcdSurfaceAndroid *pIcdSurface =
        loader_instance_heap_alloc(loader_inst, sizeof(VkIcdSurfaceAndroid), VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
    if (pIcdSurface == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    pIcdSurface->base.platform = VK_ICD_WSI_PLATFORM_ANDROID;
    pIcdSurface->window = pCreateInfo->window;

    *pSurface = (VkSurfaceKHR)pIcdSurface;

    return VK_SUCCESS;
}

#endif  // VK_USE_PLATFORM_ANDROID_KHR

// Functions for the VK_EXT_headless_surface extension:

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateHeadlessSurfaceEXT(VkInstance instance,
                                                                        const VkHeadlessSurfaceCreateInfoEXT *pCreateInfo,
                                                                        const VkAllocationCallbacks *pAllocator,
                                                                        VkSurfaceKHR *pSurface) {
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (NULL == loader_inst) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkCreateHeadlessSurfaceEXT: Invalid instance [VUID-vkCreateHeadlessSurfaceEXT-instance-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return loader_inst->disp->layer_inst_disp.CreateHeadlessSurfaceEXT(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateHeadlessSurfaceEXT(VkInstance instance,
                                                                   const VkHeadlessSurfaceCreateInfoEXT *pCreateInfo,
                                                                   const VkAllocationCallbacks *pAllocator,
                                                                   VkSurfaceKHR *pSurface) {
    struct loader_instance *inst = loader_get_instance(instance);
    VkIcdSurface *pIcdSurface = NULL;
    VkResult vkRes = VK_SUCCESS;
    uint32_t i = 0;

    if (!inst->wsi_headless_surface_enabled) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_EXT_headless_surface extension not enabled.  "
                   "vkCreateHeadlessSurfaceEXT not executed!\n");
        return VK_SUCCESS;
    }

    // Next, if so, proceed with the implementation of this function:
    pIcdSurface = AllocateIcdSurfaceStruct(inst, sizeof(pIcdSurface->headless_surf.base), sizeof(pIcdSurface->headless_surf));
    if (pIcdSurface == NULL) {
        vkRes = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    pIcdSurface->headless_surf.base.platform = VK_ICD_WSI_PLATFORM_HEADLESS;
    // Loop through each ICD and determine if they need to create a surface
    for (struct loader_icd_term *icd_term = inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
        if (icd_term->scanned_icd->interface_version >= ICD_VER_SUPPORTS_ICD_SURFACE_KHR) {
            if (NULL != icd_term->dispatch.CreateHeadlessSurfaceEXT) {
                vkRes = icd_term->dispatch.CreateHeadlessSurfaceEXT(icd_term->instance, pCreateInfo, pAllocator,
                                                                    &pIcdSurface->real_icd_surfaces[i]);
                if (VK_SUCCESS != vkRes) {
                    goto out;
                }
            }
        }
    }

    *pSurface = (VkSurfaceKHR)pIcdSurface;

out:

    if (VK_SUCCESS != vkRes && NULL != pIcdSurface) {
        if (NULL != pIcdSurface->real_icd_surfaces) {
            i = 0;
            for (struct loader_icd_term *icd_term = inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
                if ((VkSurfaceKHR)NULL != pIcdSurface->real_icd_surfaces[i] && NULL != icd_term->dispatch.DestroySurfaceKHR) {
                    icd_term->dispatch.DestroySurfaceKHR(icd_term->instance, pIcdSurface->real_icd_surfaces[i], pAllocator);
                }
            }
            loader_instance_heap_free(inst, pIcdSurface->real_icd_surfaces);
        }
        loader_instance_heap_free(inst, pIcdSurface);
    }

    return vkRes;
}

#ifdef VK_USE_PLATFORM_MACOS_MVK

// Functions for the VK_MVK_macos_surface extension:

// This is the trampoline entrypoint for CreateMacOSSurfaceMVK
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateMacOSSurfaceMVK(VkInstance instance,
                                                                     const VkMacOSSurfaceCreateInfoMVK *pCreateInfo,
                                                                     const VkAllocationCallbacks *pAllocator,
                                                                     VkSurfaceKHR *pSurface) {
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (NULL == loader_inst) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkCreateMacOSSurfaceMVK: Invalid instance [VUID-vkCreateMacOSSurfaceMVK-instance-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return loader_inst->disp->layer_inst_disp.CreateMacOSSurfaceMVK(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

// This is the instance chain terminator function for CreateMacOSSurfaceKHR
VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateMacOSSurfaceMVK(VkInstance instance, const VkMacOSSurfaceCreateInfoMVK *pCreateInfo,
                                                                const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface) {
    VkResult vkRes = VK_SUCCESS;
    VkIcdSurface *pIcdSurface = NULL;
    uint32_t i = 0;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (!loader_inst->wsi_macos_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_MVK_macos_surface extension not enabled. vkCreateMacOSSurfaceMVK not executed!\n");
        vkRes = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    pIcdSurface = AllocateIcdSurfaceStruct(loader_inst, sizeof(pIcdSurface->macos_surf.base), sizeof(pIcdSurface->macos_surf));
    if (pIcdSurface == NULL) {
        vkRes = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    pIcdSurface->macos_surf.base.platform = VK_ICD_WSI_PLATFORM_MACOS;
    pIcdSurface->macos_surf.pView = pCreateInfo->pView;

    // Loop through each ICD and determine if they need to create a surface
    for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
        if (icd_term->scanned_icd->interface_version >= ICD_VER_SUPPORTS_ICD_SURFACE_KHR) {
            if (NULL != icd_term->dispatch.CreateMacOSSurfaceMVK) {
                vkRes = icd_term->dispatch.CreateMacOSSurfaceMVK(icd_term->instance, pCreateInfo, pAllocator,
                                                                 &pIcdSurface->real_icd_surfaces[i]);
                if (VK_SUCCESS != vkRes) {
                    goto out;
                }
            }
        }
    }

    *pSurface = (VkSurfaceKHR)pIcdSurface;

out:

    if (VK_SUCCESS != vkRes && NULL != pIcdSurface) {
        if (NULL != pIcdSurface->real_icd_surfaces) {
            i = 0;
            for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
                if ((VkSurfaceKHR)NULL != pIcdSurface->real_icd_surfaces[i] && NULL != icd_term->dispatch.DestroySurfaceKHR) {
                    icd_term->dispatch.DestroySurfaceKHR(icd_term->instance, pIcdSurface->real_icd_surfaces[i], pAllocator);
                }
            }
            loader_instance_heap_free(loader_inst, pIcdSurface->real_icd_surfaces);
        }
        loader_instance_heap_free(loader_inst, pIcdSurface);
    }

    return vkRes;
}

#endif  // VK_USE_PLATFORM_MACOS_MVK

#ifdef VK_USE_PLATFORM_IOS_MVK

// Functions for the VK_MVK_ios_surface extension:

// This is the trampoline entrypoint for CreateIOSSurfaceMVK
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateIOSSurfaceMVK(VkInstance instance,
                                                                   const VkIOSSurfaceCreateInfoMVK *pCreateInfo,
                                                                   const VkAllocationCallbacks *pAllocator,
                                                                   VkSurfaceKHR *pSurface) {
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (NULL == loader_inst) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkCreateIOSSurfaceMVK: Invalid instance [VUID-vkCreateIOSSurfaceMVK-instance-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return loader_inst->disp->layer_inst_disp.CreateIOSSurfaceMVK(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

// This is the instance chain terminator function for CreateIOSSurfaceKHR
VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateIOSSurfaceMVK(VkInstance instance, const VkIOSSurfaceCreateInfoMVK *pCreateInfo,
                                                              const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (!loader_inst->wsi_ios_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_MVK_ios_surface extension not enabled. vkCreateIOSSurfaceMVK not executed!\n");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    // Next, if so, proceed with the implementation of this function:
    VkIcdSurfaceIOS *pIcdSurface =
        loader_instance_heap_alloc(loader_inst, sizeof(VkIcdSurfaceIOS), VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
    if (pIcdSurface == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    pIcdSurface->base.platform = VK_ICD_WSI_PLATFORM_IOS;
    pIcdSurface->pView = pCreateInfo->pView;

    *pSurface = (VkSurfaceKHR)pIcdSurface;

    return VK_SUCCESS;
}

#endif  // VK_USE_PLATFORM_IOS_MVK

#ifdef VK_USE_PLATFORM_GGP

// Functions for the VK_GGP_stream_descriptor_surface extension:

// This is the trampoline entrypoint for CreateStreamDescriptorSurfaceGGP
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkCreateStreamDescriptorSurfaceGGP(VkInstance instance, const VkStreamDescriptorSurfaceCreateInfoGGP *pCreateInfo,
                                   const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface) {
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (NULL == loader_inst) {
        loader_log(
            NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
            "vkCreateStreamDescriptorSurfaceGGP: Invalid instance [VUID-vkCreateStreamDescriptorSurfaceGGP-instance-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return loader_inst->disp->layer_inst_disp.CreateStreamDescriptorSurfaceGGP(loader_inst->instance, pCreateInfo, pAllocator,
                                                                               pSurface);
}

// This is the instance chain terminator function for CreateStreamDescriptorSurfaceGGP
VKAPI_ATTR VkResult VKAPI_CALL
terminator_CreateStreamDescriptorSurfaceGGP(VkInstance instance, const VkStreamDescriptorSurfaceCreateInfoGGP *pCreateInfo,
                                            const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface) {
    VkResult vkRes = VK_SUCCESS;
    VkIcdSurface *pIcdSurface = NULL;
    uint32_t i = 0;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (!loader_inst->wsi_ggp_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_GGP_stream_descriptor_surface extension not enabled. vkCreateStreamDescriptorSurfaceGGP not executed!\n");
        vkRes = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    pIcdSurface = AllocateIcdSurfaceStruct(loader_inst, sizeof(pIcdSurface->ggp_surf.base), sizeof(pIcdSurface->ggp_surf));
    if (pIcdSurface == NULL) {
        vkRes = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    pIcdSurface->ggp_surf.base.platform = VK_ICD_WSI_PLATFORM_GGP;
    pIcdSurface->ggp_surf.streamDescriptor = pCreateInfo->streamDescriptor;

    // Loop through each ICD and determine if they need to create a surface
    for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
        if (icd_term->scanned_icd->interface_version >= ICD_VER_SUPPORTS_ICD_SURFACE_KHR) {
            if (NULL != icd_term->dispatch.CreateStreamDescriptorSurfaceGGP) {
                vkRes = icd_term->dispatch.CreateStreamDescriptorSurfaceGGP(icd_term->instance, pCreateInfo, pAllocator,
                                                                            &pIcdSurface->real_icd_surfaces[i]);
                if (VK_SUCCESS != vkRes) {
                    goto out;
                }
            }
        }
    }

    *pSurface = (VkSurfaceKHR)pIcdSurface;

out:

    if (VK_SUCCESS != vkRes && NULL != pIcdSurface) {
        if (NULL != pIcdSurface->real_icd_surfaces) {
            i = 0;
            for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
                if ((VkSurfaceKHR)NULL != pIcdSurface->real_icd_surfaces[i] && NULL != icd_term->dispatch.DestroySurfaceKHR) {
                    icd_term->dispatch.DestroySurfaceKHR(icd_term->instance, pIcdSurface->real_icd_surfaces[i], pAllocator);
                }
            }
            loader_instance_heap_free(loader_inst, pIcdSurface->real_icd_surfaces);
        }
        loader_instance_heap_free(loader_inst, pIcdSurface);
    }
    return vkRes;
}

#endif  // VK_USE_PLATFORM_GGP

#if defined(VK_USE_PLATFORM_METAL_EXT)

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateMetalSurfaceEXT(VkInstance instance,
                                                                     const VkMetalSurfaceCreateInfoEXT *pCreateInfo,
                                                                     const VkAllocationCallbacks *pAllocator,
                                                                     VkSurfaceKHR *pSurface) {
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (NULL == loader_inst) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkCreateMetalSurfaceEXT: Invalid instance [VUID-vkCreateMetalSurfaceEXT-instance-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return loader_inst->disp->layer_inst_disp.CreateMetalSurfaceEXT(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateMetalSurfaceEXT(VkInstance instance, const VkMetalSurfaceCreateInfoEXT *pCreateInfo,
                                                                const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface) {
    VkResult result = VK_SUCCESS;
    VkIcdSurface *icd_surface = NULL;
    uint32_t i;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (!loader_inst->wsi_metal_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_EXT_metal_surface extension not enabled. vkCreateMetalSurfaceEXT will not be executed.\n");
    }

    // Next, if so, proceed with the implementation of this function:
    icd_surface = AllocateIcdSurfaceStruct(loader_inst, sizeof(icd_surface->metal_surf.base), sizeof(icd_surface->metal_surf));
    if (icd_surface == NULL) {
        result = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    icd_surface->metal_surf.base.platform = VK_ICD_WSI_PLATFORM_METAL;
    icd_surface->metal_surf.pLayer = pCreateInfo->pLayer;

    // Loop through each ICD and determine if they need to create a surface
    i = 0;
    for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, ++i) {
        if (icd_term->scanned_icd->interface_version >= ICD_VER_SUPPORTS_ICD_SURFACE_KHR) {
            if (icd_term->dispatch.CreateMetalSurfaceEXT != NULL) {
                result = icd_term->dispatch.CreateMetalSurfaceEXT(icd_term->instance, pCreateInfo, pAllocator,
                                                                  &icd_surface->real_icd_surfaces[i]);
                if (result != VK_SUCCESS) {
                    goto out;
                }
            }
        }
    }
    *pSurface = (VkSurfaceKHR)icd_surface;

out:
    if (result != VK_SUCCESS && icd_surface != NULL) {
        if (icd_surface->real_icd_surfaces != NULL) {
            i = 0;
            for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, ++i) {
                if (icd_surface->real_icd_surfaces[i] == VK_NULL_HANDLE && icd_term->dispatch.DestroySurfaceKHR != NULL) {
                    icd_term->dispatch.DestroySurfaceKHR(icd_term->instance, icd_surface->real_icd_surfaces[i], pAllocator);
                }
            }
            loader_instance_heap_free(loader_inst, icd_surface->real_icd_surfaces);
        }
        loader_instance_heap_free(loader_inst, icd_surface);
    }
    return result;
}

#endif  // VK_USE_PLATFORM_METAL_EXT

#ifdef VK_USE_PLATFORM_SCREEN_QNX

// This is the trampoline entrypoint for CreateScrenSurfaceQNX
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateScreenSurfaceQNX(VkInstance instance,
                                                                      const VkScreenSurfaceCreateInfoQNX *pCreateInfo,
                                                                      const VkAllocationCallbacks *pAllocator,
                                                                      VkSurfaceKHR *pSurface) {
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (NULL == loader_inst) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkCreateScreenSurfaceQNX: Invalid instance [VUID-vkCreateScreenSurfaceQNX-instance-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return loader_inst->disp->layer_inst_disp.CreateScreenSurfaceQNX(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

// This is the instance chain terminator function for CreateScreenSurfaceQNX
VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateScreenSurfaceQNX(VkInstance instance,
                                                                 const VkScreenSurfaceCreateInfoQNX *pCreateInfo,
                                                                 const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface) {
    VkResult vkRes = VK_SUCCESS;
    VkIcdSurface *pIcdSurface = NULL;
    uint32_t i = 0;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (!loader_inst->wsi_screen_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_QNX_screen_surface extension not enabled. vkCreateScreenSurfaceQNX not executed!\n");
        vkRes = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    pIcdSurface = AllocateIcdSurfaceStruct(loader_inst, sizeof(pIcdSurface->screen_surf.base), sizeof(pIcdSurface->screen_surf));
    if (pIcdSurface == NULL) {
        vkRes = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    pIcdSurface->screen_surf.base.platform = VK_ICD_WSI_PLATFORM_SCREEN;
    pIcdSurface->screen_surf.context = pCreateInfo->context;
    pIcdSurface->screen_surf.window = pCreateInfo->window;

    // Loop through each ICD and determine if they need to create a surface
    for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
        if (icd_term->scanned_icd->interface_version >= ICD_VER_SUPPORTS_ICD_SURFACE_KHR) {
            if (NULL != icd_term->dispatch.CreateScreenSurfaceQNX) {
                vkRes = icd_term->dispatch.CreateScreenSurfaceQNX(icd_term->instance, pCreateInfo, pAllocator,
                                                                  &pIcdSurface->real_icd_surfaces[i]);
                if (VK_SUCCESS != vkRes) {
                    goto out;
                }
            }
        }
    }

    *pSurface = (VkSurfaceKHR)pIcdSurface;

out:

    if (VK_SUCCESS != vkRes && NULL != pIcdSurface) {
        if (NULL != pIcdSurface->real_icd_surfaces) {
            i = 0;
            for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
                if ((VkSurfaceKHR)NULL != pIcdSurface->real_icd_surfaces[i] && NULL != icd_term->dispatch.DestroySurfaceKHR) {
                    icd_term->dispatch.DestroySurfaceKHR(icd_term->instance, pIcdSurface->real_icd_surfaces[i], pAllocator);
                }
            }
            loader_instance_heap_free(loader_inst, pIcdSurface->real_icd_surfaces);
        }
        loader_instance_heap_free(loader_inst, pIcdSurface);
    }

    return vkRes;
}

// This is the trampoline entrypoint for
// GetPhysicalDeviceScreenPresentationSupportQNX
LOADER_EXPORT VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceScreenPresentationSupportQNX(VkPhysicalDevice physicalDevice,
                                                                                             uint32_t queueFamilyIndex,
                                                                                             struct _screen_window *window) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetPhysicalDeviceScreenPresentationSupportQNX: Invalid physicalDevice "
                   "[VUID-vkGetPhysicalDeviceScreenPresentationSupportQNX-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    const VkLayerInstanceDispatchTable *disp = loader_get_instance_layer_dispatch(physicalDevice);
    VkBool32 res = disp->GetPhysicalDeviceScreenPresentationSupportQNX(unwrapped_phys_dev, queueFamilyIndex, window);
    return res;
}

// This is the instance chain terminator function for
// GetPhysicalDeviceScreenPresentationSupportQNX
VKAPI_ATTR VkBool32 VKAPI_CALL terminator_GetPhysicalDeviceScreenPresentationSupportQNX(VkPhysicalDevice physicalDevice,
                                                                                        uint32_t queueFamilyIndex,
                                                                                        struct _screen_window *window) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    struct loader_instance *loader_inst = (struct loader_instance *)icd_term->this_instance;
    if (!loader_inst->wsi_screen_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_QNX_screen_surface extension not enabled. vkGetPhysicalDeviceScreenPresentationSupportQNX not executed!\n");
        return VK_SUCCESS;
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceScreenPresentationSupportQNX) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device is not exporting vkGetPhysicalDeviceScreenPresentationSupportQNX!\n");
        abort();
    }

    return icd_term->dispatch.GetPhysicalDeviceScreenPresentationSupportQNX(phys_dev_term->phys_dev, queueFamilyIndex, window);
}
#endif  // VK_USE_PLATFORM_SCREEN_QNX

// Functions for the VK_KHR_display instance extension:
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceDisplayPropertiesKHR(VkPhysicalDevice physicalDevice,
                                                                                     uint32_t *pPropertyCount,
                                                                                     VkDisplayPropertiesKHR *pProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetPhysicalDeviceDisplayPropertiesKHR: Invalid physicalDevice "
                   "[VUID-vkGetPhysicalDeviceDisplayPropertiesKHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    VkResult res = disp->GetPhysicalDeviceDisplayPropertiesKHR(unwrapped_phys_dev, pPropertyCount, pProperties);
    return res;
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceDisplayPropertiesKHR(VkPhysicalDevice physicalDevice,
                                                                                uint32_t *pPropertyCount,
                                                                                VkDisplayPropertiesKHR *pProperties) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    struct loader_instance *loader_inst = (struct loader_instance *)icd_term->this_instance;
    if (!loader_inst->wsi_display_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_display extension not enabled. vkGetPhysicalDeviceDisplayPropertiesKHR not executed!\n");
        return VK_SUCCESS;
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceDisplayPropertiesKHR) {
        *pPropertyCount = 0;
        return VK_SUCCESS;
    }

    return icd_term->dispatch.GetPhysicalDeviceDisplayPropertiesKHR(phys_dev_term->phys_dev, pPropertyCount, pProperties);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceDisplayPlanePropertiesKHR(
    VkPhysicalDevice physicalDevice, uint32_t *pPropertyCount, VkDisplayPlanePropertiesKHR *pProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetPhysicalDeviceDisplayPlanePropertiesKHR: Invalid physicalDevice "
                   "[VUID-vkGetPhysicalDeviceDisplayPlanePropertiesKHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    VkResult res = disp->GetPhysicalDeviceDisplayPlanePropertiesKHR(unwrapped_phys_dev, pPropertyCount, pProperties);
    return res;
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceDisplayPlanePropertiesKHR(VkPhysicalDevice physicalDevice,
                                                                                     uint32_t *pPropertyCount,
                                                                                     VkDisplayPlanePropertiesKHR *pProperties) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    struct loader_instance *loader_inst = (struct loader_instance *)icd_term->this_instance;
    if (!loader_inst->wsi_display_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_display extension not enabled. vkGetPhysicalDeviceDisplayPlanePropertiesKHR not executed!\n");
        return VK_SUCCESS;
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceDisplayPlanePropertiesKHR) {
        *pPropertyCount = 0;
        return VK_SUCCESS;
    }

    return icd_term->dispatch.GetPhysicalDeviceDisplayPlanePropertiesKHR(phys_dev_term->phys_dev, pPropertyCount, pProperties);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetDisplayPlaneSupportedDisplaysKHR(VkPhysicalDevice physicalDevice,
                                                                                   uint32_t planeIndex, uint32_t *pDisplayCount,
                                                                                   VkDisplayKHR *pDisplays) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetDisplayPlaneSupportedDisplaysKHR: Invalid physicalDevice "
                   "[VUID-vkGetDisplayPlaneSupportedDisplaysKHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    VkResult res = disp->GetDisplayPlaneSupportedDisplaysKHR(unwrapped_phys_dev, planeIndex, pDisplayCount, pDisplays);
    return res;
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetDisplayPlaneSupportedDisplaysKHR(VkPhysicalDevice physicalDevice, uint32_t planeIndex,
                                                                              uint32_t *pDisplayCount, VkDisplayKHR *pDisplays) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    struct loader_instance *loader_inst = (struct loader_instance *)icd_term->this_instance;
    if (!loader_inst->wsi_display_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_display extension not enabled. vkGetDisplayPlaneSupportedDisplaysKHR not executed!\n");
        return VK_SUCCESS;
    }

    if (NULL == icd_term->dispatch.GetDisplayPlaneSupportedDisplaysKHR) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device is not exporting vkGetDisplayPlaneSupportedDisplaysKHR!\n");
        abort();
    }

    return icd_term->dispatch.GetDisplayPlaneSupportedDisplaysKHR(phys_dev_term->phys_dev, planeIndex, pDisplayCount, pDisplays);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetDisplayModePropertiesKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display,
                                                                           uint32_t *pPropertyCount,
                                                                           VkDisplayModePropertiesKHR *pProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetDisplayModePropertiesKHR: Invalid physicalDevice "
                   "[VUID-vkGetDisplayModePropertiesKHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    VkResult res = disp->GetDisplayModePropertiesKHR(unwrapped_phys_dev, display, pPropertyCount, pProperties);
    return res;
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetDisplayModePropertiesKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display,
                                                                      uint32_t *pPropertyCount,
                                                                      VkDisplayModePropertiesKHR *pProperties) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    struct loader_instance *loader_inst = (struct loader_instance *)icd_term->this_instance;
    if (!loader_inst->wsi_display_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_display extension not enabled. vkGetDisplayModePropertiesKHR not executed!\n");
        return VK_SUCCESS;
    }

    if (NULL == icd_term->dispatch.GetDisplayModePropertiesKHR) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device is not exporting vkGetDisplayModePropertiesKHR!\n");
        abort();
    }

    return icd_term->dispatch.GetDisplayModePropertiesKHR(phys_dev_term->phys_dev, display, pPropertyCount, pProperties);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateDisplayModeKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display,
                                                                    const VkDisplayModeCreateInfoKHR *pCreateInfo,
                                                                    const VkAllocationCallbacks *pAllocator,
                                                                    VkDisplayModeKHR *pMode) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkCreateDisplayModeKHR: Invalid physicalDevice "
                   "[VUID-vkCreateDisplayModeKHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    VkResult res = disp->CreateDisplayModeKHR(unwrapped_phys_dev, display, pCreateInfo, pAllocator, pMode);
    return res;
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateDisplayModeKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display,
                                                               const VkDisplayModeCreateInfoKHR *pCreateInfo,
                                                               const VkAllocationCallbacks *pAllocator, VkDisplayModeKHR *pMode) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    struct loader_instance *loader_inst = (struct loader_instance *)icd_term->this_instance;
    if (!loader_inst->wsi_display_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_display extension not enabled. vkCreateDisplayModeKHR not executed!\n");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    if (NULL == icd_term->dispatch.CreateDisplayModeKHR) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device is not exporting vkCreateDisplayModeKHR!\n");
        abort();
    }

    return icd_term->dispatch.CreateDisplayModeKHR(phys_dev_term->phys_dev, display, pCreateInfo, pAllocator, pMode);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetDisplayPlaneCapabilitiesKHR(VkPhysicalDevice physicalDevice,
                                                                              VkDisplayModeKHR mode, uint32_t planeIndex,
                                                                              VkDisplayPlaneCapabilitiesKHR *pCapabilities) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetDisplayPlaneCapabilitiesKHR: Invalid physicalDevice "
                   "[VUID-vkGetDisplayPlaneCapabilitiesKHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    VkResult res = disp->GetDisplayPlaneCapabilitiesKHR(unwrapped_phys_dev, mode, planeIndex, pCapabilities);
    return res;
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetDisplayPlaneCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkDisplayModeKHR mode,
                                                                         uint32_t planeIndex,
                                                                         VkDisplayPlaneCapabilitiesKHR *pCapabilities) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    struct loader_instance *loader_inst = (struct loader_instance *)icd_term->this_instance;
    if (!loader_inst->wsi_display_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_display extension not enabled. vkGetDisplayPlaneCapabilitiesKHR not executed!\n");
        return VK_SUCCESS;
    }

    if (NULL == icd_term->dispatch.GetDisplayPlaneCapabilitiesKHR) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device is not exporting vkGetDisplayPlaneCapabilitiesKHR!\n");
        abort();
    }

    return icd_term->dispatch.GetDisplayPlaneCapabilitiesKHR(phys_dev_term->phys_dev, mode, planeIndex, pCapabilities);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateDisplayPlaneSurfaceKHR(VkInstance instance,
                                                                            const VkDisplaySurfaceCreateInfoKHR *pCreateInfo,
                                                                            const VkAllocationCallbacks *pAllocator,
                                                                            VkSurfaceKHR *pSurface) {
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (NULL == loader_inst) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkCreateDisplayPlaneSurfaceKHR: Invalid instance [VUID-vkCreateDisplayPlaneSurfaceKHR-instance-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return loader_inst->disp->layer_inst_disp.CreateDisplayPlaneSurfaceKHR(loader_inst->instance, pCreateInfo, pAllocator,
                                                                           pSurface);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateDisplayPlaneSurfaceKHR(VkInstance instance,
                                                                       const VkDisplaySurfaceCreateInfoKHR *pCreateInfo,
                                                                       const VkAllocationCallbacks *pAllocator,
                                                                       VkSurfaceKHR *pSurface) {
    struct loader_instance *inst = loader_get_instance(instance);
    VkIcdSurface *pIcdSurface = NULL;
    VkResult vkRes = VK_SUCCESS;
    uint32_t i = 0;

    if (!inst->wsi_display_enabled) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_surface extension not enabled. vkCreateDisplayPlaneSurfaceKHR not executed!\n");
        vkRes = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    pIcdSurface = AllocateIcdSurfaceStruct(inst, sizeof(pIcdSurface->display_surf.base), sizeof(pIcdSurface->display_surf));
    if (pIcdSurface == NULL) {
        vkRes = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    pIcdSurface->display_surf.base.platform = VK_ICD_WSI_PLATFORM_DISPLAY;
    pIcdSurface->display_surf.displayMode = pCreateInfo->displayMode;
    pIcdSurface->display_surf.planeIndex = pCreateInfo->planeIndex;
    pIcdSurface->display_surf.planeStackIndex = pCreateInfo->planeStackIndex;
    pIcdSurface->display_surf.transform = pCreateInfo->transform;
    pIcdSurface->display_surf.globalAlpha = pCreateInfo->globalAlpha;
    pIcdSurface->display_surf.alphaMode = pCreateInfo->alphaMode;
    pIcdSurface->display_surf.imageExtent = pCreateInfo->imageExtent;

    // Loop through each ICD and determine if they need to create a surface
    for (struct loader_icd_term *icd_term = inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
        if (icd_term->scanned_icd->interface_version >= ICD_VER_SUPPORTS_ICD_SURFACE_KHR) {
            if (NULL != icd_term->dispatch.CreateDisplayPlaneSurfaceKHR) {
                vkRes = icd_term->dispatch.CreateDisplayPlaneSurfaceKHR(icd_term->instance, pCreateInfo, pAllocator,
                                                                        &pIcdSurface->real_icd_surfaces[i]);
                if (VK_SUCCESS != vkRes) {
                    goto out;
                }
            }
        }
    }

    *pSurface = (VkSurfaceKHR)pIcdSurface;

out:

    if (VK_SUCCESS != vkRes && NULL != pIcdSurface) {
        if (NULL != pIcdSurface->real_icd_surfaces) {
            i = 0;
            for (struct loader_icd_term *icd_term = inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
                if ((VkSurfaceKHR)NULL != pIcdSurface->real_icd_surfaces[i] && NULL != icd_term->dispatch.DestroySurfaceKHR) {
                    icd_term->dispatch.DestroySurfaceKHR(icd_term->instance, pIcdSurface->real_icd_surfaces[i], pAllocator);
                }
            }
            loader_instance_heap_free(inst, pIcdSurface->real_icd_surfaces);
        }
        loader_instance_heap_free(inst, pIcdSurface);
    }

    return vkRes;
}

// EXT_display_swapchain Extension command

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateSharedSwapchainsKHR(VkDevice device, uint32_t swapchainCount,
                                                                         const VkSwapchainCreateInfoKHR *pCreateInfos,
                                                                         const VkAllocationCallbacks *pAllocator,
                                                                         VkSwapchainKHR *pSwapchains) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkCreateSharedSwapchainsKHR: Invalid device [VUID-vkCreateSharedSwapchainsKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->CreateSharedSwapchainsKHR(device, swapchainCount, pCreateInfos, pAllocator, pSwapchains);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateSharedSwapchainsKHR(VkDevice device, uint32_t swapchainCount,
                                                                    const VkSwapchainCreateInfoKHR *pCreateInfos,
                                                                    const VkAllocationCallbacks *pAllocator,
                                                                    VkSwapchainKHR *pSwapchains) {
    uint32_t icd_index = 0;
    struct loader_device *dev;
    struct loader_icd_term *icd_term = loader_get_icd_and_device(device, &dev, &icd_index);
    if (NULL != icd_term && NULL != icd_term->dispatch.CreateSharedSwapchainsKHR) {
        VkIcdSurface *icd_surface = (VkIcdSurface *)(uintptr_t)pCreateInfos->surface;
        if (NULL != icd_surface->real_icd_surfaces) {
            if ((VkSurfaceKHR)NULL != icd_surface->real_icd_surfaces[icd_index]) {
                // We found the ICD, and there is an ICD KHR surface
                // associated with it, so copy the CreateInfo struct
                // and point it at the ICD's surface.
                VkSwapchainCreateInfoKHR *pCreateCopy = loader_stack_alloc(sizeof(VkSwapchainCreateInfoKHR) * swapchainCount);
                if (NULL == pCreateCopy) {
                    return VK_ERROR_OUT_OF_HOST_MEMORY;
                }
                memcpy(pCreateCopy, pCreateInfos, sizeof(VkSwapchainCreateInfoKHR) * swapchainCount);
                for (uint32_t sc = 0; sc < swapchainCount; sc++) {
                    pCreateCopy[sc].surface = icd_surface->real_icd_surfaces[icd_index];
                }
                return icd_term->dispatch.CreateSharedSwapchainsKHR(device, swapchainCount, pCreateCopy, pAllocator, pSwapchains);
            }
        }
        return icd_term->dispatch.CreateSharedSwapchainsKHR(device, swapchainCount, pCreateInfos, pAllocator, pSwapchains);
    }
    return VK_SUCCESS;
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkGetDeviceGroupPresentCapabilitiesKHR(VkDevice device, VkDeviceGroupPresentCapabilitiesKHR *pDeviceGroupPresentCapabilities) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetDeviceGroupPresentCapabilitiesKHR: Invalid device "
                   "[VUID-vkGetDeviceGroupPresentCapabilitiesKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetDeviceGroupPresentCapabilitiesKHR(device, pDeviceGroupPresentCapabilities);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetDeviceGroupSurfacePresentModesKHR(VkDevice device, VkSurfaceKHR surface,
                                                                                    VkDeviceGroupPresentModeFlagsKHR *pModes) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetDeviceGroupSurfacePresentModesKHR: Invalid device "
                   "[VUID-vkGetDeviceGroupSurfacePresentModesKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->GetDeviceGroupSurfacePresentModesKHR(device, surface, pModes);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetDeviceGroupSurfacePresentModesKHR(VkDevice device, VkSurfaceKHR surface,
                                                                               VkDeviceGroupPresentModeFlagsKHR *pModes) {
    uint32_t icd_index = 0;
    struct loader_device *dev;
    struct loader_icd_term *icd_term = loader_get_icd_and_device(device, &dev, &icd_index);
    if (NULL != icd_term && NULL != icd_term->dispatch.GetDeviceGroupSurfacePresentModesKHR) {
        VkIcdSurface *icd_surface = (VkIcdSurface *)(uintptr_t)surface;
        if (NULL != icd_surface->real_icd_surfaces && (VkSurfaceKHR)NULL != icd_surface->real_icd_surfaces[icd_index]) {
            return icd_term->dispatch.GetDeviceGroupSurfacePresentModesKHR(device, icd_surface->real_icd_surfaces[icd_index],
                                                                           pModes);
        }
        return icd_term->dispatch.GetDeviceGroupSurfacePresentModesKHR(device, surface, pModes);
    }
    return VK_SUCCESS;
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDevicePresentRectanglesKHR(VkPhysicalDevice physicalDevice,
                                                                                     VkSurfaceKHR surface, uint32_t *pRectCount,
                                                                                     VkRect2D *pRects) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetPhysicalDevicePresentRectanglesKHR: Invalid physicalDevice "
                   "[VUID-vkGetPhysicalDevicePresentRectanglesKHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDevicePresentRectanglesKHR(unwrapped_phys_dev, surface, pRectCount, pRects);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDevicePresentRectanglesKHR(VkPhysicalDevice physicalDevice,
                                                                                VkSurfaceKHR surface, uint32_t *pRectCount,
                                                                                VkRect2D *pRects) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.GetPhysicalDevicePresentRectanglesKHR) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support GetPhysicalDevicePresentRectanglesKHX");
        abort();
    }
    VkIcdSurface *icd_surface = (VkIcdSurface *)(surface);
    uint8_t icd_index = phys_dev_term->icd_index;
    if (NULL != icd_surface->real_icd_surfaces && NULL != (void *)icd_surface->real_icd_surfaces[icd_index]) {
        return icd_term->dispatch.GetPhysicalDevicePresentRectanglesKHR(
            phys_dev_term->phys_dev, icd_surface->real_icd_surfaces[icd_index], pRectCount, pRects);
    }
    return icd_term->dispatch.GetPhysicalDevicePresentRectanglesKHR(phys_dev_term->phys_dev, surface, pRectCount, pRects);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkAcquireNextImage2KHR(VkDevice device, const VkAcquireNextImageInfoKHR *pAcquireInfo,
                                                                    uint32_t *pImageIndex) {
    const VkLayerDispatchTable *disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkAcquireNextImage2KHR: Invalid device [VUID-vkAcquireNextImage2KHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return disp->AcquireNextImage2KHR(device, pAcquireInfo, pImageIndex);
}

// ---- VK_KHR_get_display_properties2 extension trampoline/terminators

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceDisplayProperties2KHR(VkPhysicalDevice physicalDevice,
                                                                                      uint32_t *pPropertyCount,
                                                                                      VkDisplayProperties2KHR *pProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetPhysicalDeviceDisplayProperties2KHR: Invalid physicalDevice "
                   "[VUID-vkGetPhysicalDeviceDisplayProperties2KHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceDisplayProperties2KHR(unwrapped_phys_dev, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceDisplayProperties2KHR(VkPhysicalDevice physicalDevice,
                                                                                 uint32_t *pPropertyCount,
                                                                                 VkDisplayProperties2KHR *pProperties) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;

    // If the function is available in the driver, just call into it
    if (icd_term->dispatch.GetPhysicalDeviceDisplayProperties2KHR != NULL) {
        return icd_term->dispatch.GetPhysicalDeviceDisplayProperties2KHR(phys_dev_term->phys_dev, pPropertyCount, pProperties);
    }

    // We have to emulate the function.
    loader_log(icd_term->this_instance, VULKAN_LOADER_INFO_BIT, 0,
               "vkGetPhysicalDeviceDisplayProperties2KHR: Emulating call in ICD \"%s\"", icd_term->scanned_icd->lib_name);

    // If the icd doesn't support VK_KHR_display, then no properties are available
    if (icd_term->dispatch.GetPhysicalDeviceDisplayPropertiesKHR == NULL) {
        *pPropertyCount = 0;
        return VK_SUCCESS;
    }

    // If we aren't writing to pProperties, then emulation is straightforward
    if (pProperties == NULL || *pPropertyCount == 0) {
        return icd_term->dispatch.GetPhysicalDeviceDisplayPropertiesKHR(phys_dev_term->phys_dev, pPropertyCount, NULL);
    }

    // If we do have to write to pProperties, then we need to write to a temporary array of VkDisplayPropertiesKHR and copy it
    VkDisplayPropertiesKHR *properties = loader_stack_alloc(*pPropertyCount * sizeof(VkDisplayPropertiesKHR));
    if (properties == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    VkResult res = icd_term->dispatch.GetPhysicalDeviceDisplayPropertiesKHR(phys_dev_term->phys_dev, pPropertyCount, properties);
    if (res < 0) {
        return res;
    }
    for (uint32_t i = 0; i < *pPropertyCount; ++i) {
        memcpy(&pProperties[i].displayProperties, &properties[i], sizeof(VkDisplayPropertiesKHR));
    }
    return res;
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceDisplayPlaneProperties2KHR(
    VkPhysicalDevice physicalDevice, uint32_t *pPropertyCount, VkDisplayPlaneProperties2KHR *pProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetPhysicalDeviceDisplayPlaneProperties2KHR: Invalid physicalDevice "
                   "[VUID-vkGetPhysicalDeviceDisplayPlaneProperties2KHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceDisplayPlaneProperties2KHR(unwrapped_phys_dev, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceDisplayPlaneProperties2KHR(VkPhysicalDevice physicalDevice,
                                                                                      uint32_t *pPropertyCount,
                                                                                      VkDisplayPlaneProperties2KHR *pProperties) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;

    // If the function is available in the driver, just call into it
    if (icd_term->dispatch.GetPhysicalDeviceDisplayPlaneProperties2KHR != NULL) {
        return icd_term->dispatch.GetPhysicalDeviceDisplayPlaneProperties2KHR(phys_dev_term->phys_dev, pPropertyCount, pProperties);
    }

    // We have to emulate the function.
    loader_log(icd_term->this_instance, VULKAN_LOADER_INFO_BIT, 0,
               "vkGetPhysicalDeviceDisplayPlaneProperties2KHR: Emulating call in ICD \"%s\"", icd_term->scanned_icd->lib_name);

    // If the icd doesn't support VK_KHR_display, then no properties are available
    if (icd_term->dispatch.GetPhysicalDeviceDisplayPlanePropertiesKHR == NULL) {
        *pPropertyCount = 0;
        return VK_SUCCESS;
    }

    // If we aren't writing to pProperties, then emulation is straightforward
    if (pProperties == NULL || *pPropertyCount == 0) {
        return icd_term->dispatch.GetPhysicalDeviceDisplayPlanePropertiesKHR(phys_dev_term->phys_dev, pPropertyCount, NULL);
    }

    // If we do have to write to pProperties, then we need to write to a temporary array of VkDisplayPlanePropertiesKHR and copy it
    VkDisplayPlanePropertiesKHR *properties = loader_stack_alloc(*pPropertyCount * sizeof(VkDisplayPlanePropertiesKHR));
    if (properties == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    VkResult res =
        icd_term->dispatch.GetPhysicalDeviceDisplayPlanePropertiesKHR(phys_dev_term->phys_dev, pPropertyCount, properties);
    if (res < 0) {
        return res;
    }
    for (uint32_t i = 0; i < *pPropertyCount; ++i) {
        memcpy(&pProperties[i].displayPlaneProperties, &properties[i], sizeof(VkDisplayPlanePropertiesKHR));
    }
    return res;
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetDisplayModeProperties2KHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display,
                                                                            uint32_t *pPropertyCount,
                                                                            VkDisplayModeProperties2KHR *pProperties) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetDisplayModeProperties2KHR: Invalid physicalDevice "
                   "[VUID-vkGetDisplayModeProperties2KHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetDisplayModeProperties2KHR(unwrapped_phys_dev, display, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetDisplayModeProperties2KHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display,
                                                                       uint32_t *pPropertyCount,
                                                                       VkDisplayModeProperties2KHR *pProperties) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;

    // If the function is available in the driver, just call into it
    if (icd_term->dispatch.GetDisplayModeProperties2KHR != NULL) {
        return icd_term->dispatch.GetDisplayModeProperties2KHR(phys_dev_term->phys_dev, display, pPropertyCount, pProperties);
    }

    // We have to emulate the function.
    loader_log(icd_term->this_instance, VULKAN_LOADER_INFO_BIT, 0, "vkGetDisplayModeProperties2KHR: Emulating call in ICD \"%s\"",
               icd_term->scanned_icd->lib_name);

    // If the icd doesn't support VK_KHR_display, then no properties are available
    if (icd_term->dispatch.GetDisplayModePropertiesKHR == NULL) {
        *pPropertyCount = 0;
        return VK_SUCCESS;
    }

    // If we aren't writing to pProperties, then emulation is straightforward
    if (pProperties == NULL || *pPropertyCount == 0) {
        return icd_term->dispatch.GetDisplayModePropertiesKHR(phys_dev_term->phys_dev, display, pPropertyCount, NULL);
    }

    // If we do have to write to pProperties, then we need to write to a temporary array of VkDisplayModePropertiesKHR and copy it
    VkDisplayModePropertiesKHR *properties = loader_stack_alloc(*pPropertyCount * sizeof(VkDisplayModePropertiesKHR));
    if (properties == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    VkResult res = icd_term->dispatch.GetDisplayModePropertiesKHR(phys_dev_term->phys_dev, display, pPropertyCount, properties);
    if (res < 0) {
        return res;
    }
    for (uint32_t i = 0; i < *pPropertyCount; ++i) {
        memcpy(&pProperties[i].displayModeProperties, &properties[i], sizeof(VkDisplayModePropertiesKHR));
    }
    return res;
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetDisplayPlaneCapabilities2KHR(VkPhysicalDevice physicalDevice,
                                                                               const VkDisplayPlaneInfo2KHR *pDisplayPlaneInfo,
                                                                               VkDisplayPlaneCapabilities2KHR *pCapabilities) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetDisplayPlaneCapabilities2KHR: Invalid physicalDevice "
                   "[VUID-vkGetDisplayPlaneCapabilities2KHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetDisplayPlaneCapabilities2KHR(unwrapped_phys_dev, pDisplayPlaneInfo, pCapabilities);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetDisplayPlaneCapabilities2KHR(VkPhysicalDevice physicalDevice,
                                                                          const VkDisplayPlaneInfo2KHR *pDisplayPlaneInfo,
                                                                          VkDisplayPlaneCapabilities2KHR *pCapabilities) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;

    // If the function is abailable in the driver, just call into it
    if (icd_term->dispatch.GetDisplayPlaneCapabilities2KHR != NULL) {
        return icd_term->dispatch.GetDisplayPlaneCapabilities2KHR(phys_dev_term->phys_dev, pDisplayPlaneInfo, pCapabilities);
    }

    // We have to emulate the function.
    loader_log(icd_term->this_instance, VULKAN_LOADER_INFO_BIT, 0,
               "vkGetDisplayPlaneCapabilities2KHR: Emulating call in ICD \"%s\"", icd_term->scanned_icd->lib_name);

    // Just call into the old version of the function.
    // If the icd doesn't support VK_KHR_display, there are zero planes and this call is invalid (and will crash)
    return icd_term->dispatch.GetDisplayPlaneCapabilitiesKHR(phys_dev_term->phys_dev, pDisplayPlaneInfo->mode,
                                                             pDisplayPlaneInfo->planeIndex, &pCapabilities->capabilities);
}

#ifdef VK_USE_PLATFORM_FUCHSIA

// This is the trampoline entrypoint for CreateImagePipeSurfaceFUCHSIA
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateImagePipeSurfaceFUCHSIA(VkInstance instance,
                                                                             const VkImagePipeSurfaceCreateInfoFUCHSIA *pCreateInfo,
                                                                             const VkAllocationCallbacks *pAllocator,
                                                                             VkSurfaceKHR *pSurface) {
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (NULL == loader_inst) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkCreateImagePipeSurfaceFUCHSIA: Invalid instance [VUID-vkCreateImagePipeSurfaceFUCHSIA-instance-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    return loader_inst->disp->layer_inst_disp.CreateImagePipeSurfaceFUCHSIA(loader_inst->instance, pCreateInfo, pAllocator,
                                                                            pSurface);
}

// This is the instance chain terminator function for CreateImagePipeSurfaceFUCHSIA
VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateImagePipeSurfaceFUCHSIA(VkInstance instance,
                                                                        const VkImagePipeSurfaceCreateInfoFUCHSIA *pCreateInfo,
                                                                        const VkAllocationCallbacks *pAllocator,
                                                                        VkSurfaceKHR *pSurface) {
    VkResult vkRes = VK_SUCCESS;
    VkIcdSurface *pIcdSurface = NULL;
    uint32_t i = 0;

    // Initialize pSurface to NULL just to be safe.
    *pSurface = VK_NULL_HANDLE;
    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance *loader_inst = loader_get_instance(instance);
    if (!loader_inst->wsi_imagepipe_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_FUCHSIA_imagepipe_surface extension not enabled.  "
                   "vkCreateImagePipeSurfaceFUCHSIA not executed!\n");
        vkRes = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    pIcdSurface =
        AllocateIcdSurfaceStruct(loader_inst, sizeof(pIcdSurface->imagepipe_surf.base), sizeof(pIcdSurface->imagepipe_surf));
    if (pIcdSurface == NULL) {
        vkRes = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    pIcdSurface->imagepipe_surf.base.platform = VK_ICD_WSI_PLATFORM_FUCHSIA;

    // Loop through each ICD and determine if they need to create a surface
    for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
        if (icd_term->scanned_icd->interface_version >= ICD_VER_SUPPORTS_ICD_SURFACE_KHR) {
            if (NULL != icd_term->dispatch.CreateImagePipeSurfaceFUCHSIA) {
                vkRes = icd_term->dispatch.CreateImagePipeSurfaceFUCHSIA(icd_term->instance, pCreateInfo, pAllocator,
                                                                         &pIcdSurface->real_icd_surfaces[i]);
                if (VK_SUCCESS != vkRes) {
                    goto out;
                }
            }
        }
    }

    *pSurface = (VkSurfaceKHR)(pIcdSurface);

out:

    if (VK_SUCCESS != vkRes && NULL != pIcdSurface) {
        if (NULL != pIcdSurface->real_icd_surfaces) {
            i = 0;
            for (struct loader_icd_term *icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next, i++) {
                if ((VkSurfaceKHR)NULL != pIcdSurface->real_icd_surfaces[i] && NULL != icd_term->dispatch.DestroySurfaceKHR) {
                    icd_term->dispatch.DestroySurfaceKHR(icd_term->instance, pIcdSurface->real_icd_surfaces[i], pAllocator);
                }
            }
            loader_instance_heap_free(loader_inst, pIcdSurface->real_icd_surfaces);
        }
        loader_instance_heap_free(loader_inst, pIcdSurface);
    }

    return vkRes;
}
#endif  // VK_USE_PLATFORM_FUCHSIA

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceSurfaceCapabilities2KHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSurfaceInfo2KHR *pSurfaceInfo,
                                           VkSurfaceCapabilities2KHR *pSurfaceCapabilities) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetPhysicalDeviceSurfaceCapabilities2KHR: Invalid physicalDevice "
                   "[VUID-vkGetPhysicalDeviceSurfaceCapabilities2KHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceSurfaceCapabilities2KHR(unwrapped_phys_dev, pSurfaceInfo, pSurfaceCapabilities);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceSurfaceCapabilities2KHR(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSurfaceInfo2KHR *pSurfaceInfo,
    VkSurfaceCapabilities2KHR *pSurfaceCapabilities) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    struct loader_instance *loader_inst = (struct loader_instance *)icd_term->this_instance;

    if (!loader_inst->wsi_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_surface extension not enabled. vkGetPhysicalDeviceSurfaceCapabilities2KHR not executed!\n");
        return VK_SUCCESS;
    }

    VkIcdSurface *icd_surface = (VkIcdSurface *)(pSurfaceInfo->surface);
    uint8_t icd_index = phys_dev_term->icd_index;

    if (icd_term->dispatch.GetPhysicalDeviceSurfaceCapabilities2KHR != NULL) {
        VkBaseOutStructure *pNext = (VkBaseOutStructure *)pSurfaceCapabilities->pNext;
        while (pNext != NULL) {
            if ((int)pNext->sType == VK_STRUCTURE_TYPE_SURFACE_PROTECTED_CAPABILITIES_KHR) {
                // Not all ICDs may be supporting VK_KHR_surface_protected_capabilities
                // Initialize VkSurfaceProtectedCapabilitiesKHR.supportsProtected to false and
                // if an ICD supports protected surfaces, it will reset it to true accordingly.
                ((VkSurfaceProtectedCapabilitiesKHR *)pNext)->supportsProtected = VK_FALSE;
            }
            pNext = (VkBaseOutStructure *)pNext->pNext;
        }

        // Pass the call to the driver, possibly unwrapping the ICD surface
        if (icd_surface->real_icd_surfaces != NULL && (void *)icd_surface->real_icd_surfaces[icd_index] != NULL) {
            VkPhysicalDeviceSurfaceInfo2KHR info_copy = *pSurfaceInfo;
            info_copy.surface = icd_surface->real_icd_surfaces[icd_index];
            return icd_term->dispatch.GetPhysicalDeviceSurfaceCapabilities2KHR(phys_dev_term->phys_dev, &info_copy,
                                                                               pSurfaceCapabilities);
        } else {
            return icd_term->dispatch.GetPhysicalDeviceSurfaceCapabilities2KHR(phys_dev_term->phys_dev, pSurfaceInfo,
                                                                               pSurfaceCapabilities);
        }
    } else {
        // Emulate the call
        loader_log(icd_term->this_instance, VULKAN_LOADER_INFO_BIT, 0,
                   "vkGetPhysicalDeviceSurfaceCapabilities2KHR: Emulating call in ICD \"%s\" using "
                   "vkGetPhysicalDeviceSurfaceCapabilitiesKHR",
                   icd_term->scanned_icd->lib_name);

        if (pSurfaceInfo->pNext != NULL) {
            loader_log(icd_term->this_instance, VULKAN_LOADER_WARN_BIT, 0,
                       "vkGetPhysicalDeviceSurfaceCapabilities2KHR: Emulation found unrecognized structure type in "
                       "pSurfaceInfo->pNext - this struct will be ignored");
        }

        // Write to the VkSurfaceCapabilities2KHR struct
        VkSurfaceKHR surface = pSurfaceInfo->surface;
        if (icd_surface->real_icd_surfaces != NULL && (void *)icd_surface->real_icd_surfaces[icd_index] != NULL) {
            surface = icd_surface->real_icd_surfaces[icd_index];
        }
        VkResult res = icd_term->dispatch.GetPhysicalDeviceSurfaceCapabilitiesKHR(phys_dev_term->phys_dev, surface,
                                                                                  &pSurfaceCapabilities->surfaceCapabilities);

        if (pSurfaceCapabilities->pNext != NULL) {
            loader_log(icd_term->this_instance, VULKAN_LOADER_WARN_BIT, 0,
                       "vkGetPhysicalDeviceSurfaceCapabilities2KHR: Emulation found unrecognized structure type in "
                       "pSurfaceCapabilities->pNext - this struct will be ignored");
        }
        return res;
    }
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceSurfaceFormats2KHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSurfaceInfo2KHR *pSurfaceInfo,
                                      uint32_t *pSurfaceFormatCount, VkSurfaceFormat2KHR *pSurfaceFormats) {
    const VkLayerInstanceDispatchTable *disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice);
    if (VK_NULL_HANDLE == unwrapped_phys_dev) {
        loader_log(NULL, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "vkGetPhysicalDeviceSurfaceFormats2KHR: Invalid physicalDevice "
                   "[VUID-vkGetPhysicalDeviceSurfaceFormats2KHR-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceSurfaceFormats2KHR(unwrapped_phys_dev, pSurfaceInfo, pSurfaceFormatCount, pSurfaceFormats);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceSurfaceFormats2KHR(VkPhysicalDevice physicalDevice,
                                                                              const VkPhysicalDeviceSurfaceInfo2KHR *pSurfaceInfo,
                                                                              uint32_t *pSurfaceFormatCount,
                                                                              VkSurfaceFormat2KHR *pSurfaceFormats) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    struct loader_instance *loader_inst = (struct loader_instance *)icd_term->this_instance;

    if (!loader_inst->wsi_surface_enabled) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_surface extension not enabled. vkGetPhysicalDeviceSurfaceFormats2KHR not executed!\n");
        return VK_SUCCESS;
    }

    VkIcdSurface *icd_surface = (VkIcdSurface *)(pSurfaceInfo->surface);
    uint8_t icd_index = phys_dev_term->icd_index;

    if (icd_term->dispatch.GetPhysicalDeviceSurfaceFormats2KHR != NULL) {
        // Pass the call to the driver, possibly unwrapping the ICD surface
        if (icd_surface->real_icd_surfaces != NULL && (void *)icd_surface->real_icd_surfaces[icd_index] != NULL) {
            VkPhysicalDeviceSurfaceInfo2KHR info_copy = *pSurfaceInfo;
            info_copy.surface = icd_surface->real_icd_surfaces[icd_index];
            return icd_term->dispatch.GetPhysicalDeviceSurfaceFormats2KHR(phys_dev_term->phys_dev, &info_copy, pSurfaceFormatCount,
                                                                          pSurfaceFormats);
        } else {
            return icd_term->dispatch.GetPhysicalDeviceSurfaceFormats2KHR(phys_dev_term->phys_dev, pSurfaceInfo,
                                                                          pSurfaceFormatCount, pSurfaceFormats);
        }
    } else {
        // Emulate the call
        loader_log(icd_term->this_instance, VULKAN_LOADER_INFO_BIT, 0,
                   "vkGetPhysicalDeviceSurfaceFormats2KHR: Emulating call in ICD \"%s\" using vkGetPhysicalDeviceSurfaceFormatsKHR",
                   icd_term->scanned_icd->lib_name);

        if (pSurfaceInfo->pNext != NULL) {
            loader_log(icd_term->this_instance, VULKAN_LOADER_WARN_BIT, 0,
                       "vkGetPhysicalDeviceSurfaceFormats2KHR: Emulation found unrecognized structure type in pSurfaceInfo->pNext "
                       "- this struct will be ignored");
        }

        VkSurfaceKHR surface = pSurfaceInfo->surface;
        if (icd_surface->real_icd_surfaces != NULL && (void *)icd_surface->real_icd_surfaces[icd_index] != NULL) {
            surface = icd_surface->real_icd_surfaces[icd_index];
        }

        if (*pSurfaceFormatCount == 0 || pSurfaceFormats == NULL) {
            // Write to pSurfaceFormatCount
            return icd_term->dispatch.GetPhysicalDeviceSurfaceFormatsKHR(phys_dev_term->phys_dev, surface, pSurfaceFormatCount,
                                                                         NULL);
        } else {
            // Allocate a temporary array for the output of the old function
            VkSurfaceFormatKHR *formats = loader_stack_alloc(*pSurfaceFormatCount * sizeof(VkSurfaceFormatKHR));
            if (formats == NULL) {
                return VK_ERROR_OUT_OF_HOST_MEMORY;
            }

            VkResult res = icd_term->dispatch.GetPhysicalDeviceSurfaceFormatsKHR(phys_dev_term->phys_dev, surface,
                                                                                 pSurfaceFormatCount, formats);
            for (uint32_t i = 0; i < *pSurfaceFormatCount; ++i) {
                pSurfaceFormats[i].surfaceFormat = formats[i];
                if (pSurfaceFormats[i].pNext != NULL) {
                    loader_log(icd_term->this_instance, VULKAN_LOADER_WARN_BIT, 0,
                               "vkGetPhysicalDeviceSurfaceFormats2KHR: Emulation found unrecognized structure type in "
                               "pSurfaceFormats[%d].pNext - this struct will be ignored",
                               i);
                }
            }
            return res;
        }
    }
}

bool wsi_swapchain_instance_gpa(struct loader_instance *loader_inst, const char *name, void **addr) {
    *addr = NULL;

    // Functions for the VK_KHR_surface extension:
    if (!strcmp("vkDestroySurfaceKHR", name)) {
        *addr = loader_inst->wsi_surface_enabled ? (void *)vkDestroySurfaceKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceSurfaceSupportKHR", name)) {
        *addr = loader_inst->wsi_surface_enabled ? (void *)vkGetPhysicalDeviceSurfaceSupportKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceSurfaceCapabilitiesKHR", name)) {
        *addr = loader_inst->wsi_surface_enabled ? (void *)vkGetPhysicalDeviceSurfaceCapabilitiesKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceSurfaceFormatsKHR", name)) {
        *addr = loader_inst->wsi_surface_enabled ? (void *)vkGetPhysicalDeviceSurfaceFormatsKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceSurfacePresentModesKHR", name)) {
        *addr = loader_inst->wsi_surface_enabled ? (void *)vkGetPhysicalDeviceSurfacePresentModesKHR : NULL;
        return true;
    }

    if (!strcmp("vkGetDeviceGroupPresentCapabilitiesKHR", name)) {
        *addr = loader_inst->wsi_surface_enabled ? (void *)vkGetDeviceGroupPresentCapabilitiesKHR : NULL;
        return true;
    }

    if (!strcmp("vkGetDeviceGroupSurfacePresentModesKHR", name)) {
        *addr = loader_inst->wsi_surface_enabled ? (void *)vkGetDeviceGroupSurfacePresentModesKHR : NULL;
        return true;
    }

    if (!strcmp("vkGetPhysicalDevicePresentRectanglesKHR", name)) {
        *addr = loader_inst->wsi_surface_enabled ? (void *)vkGetPhysicalDevicePresentRectanglesKHR : NULL;
        return true;
    }

    // Functions for VK_KHR_get_surface_capabilities2 extension:
    if (!strcmp("vkGetPhysicalDeviceSurfaceCapabilities2KHR", name)) {
        *addr = loader_inst->wsi_surface_enabled ? (void *)vkGetPhysicalDeviceSurfaceCapabilities2KHR : NULL;
        return true;
    }

    if (!strcmp("vkGetPhysicalDeviceSurfaceFormats2KHR", name)) {
        *addr = loader_inst->wsi_surface_enabled ? (void *)vkGetPhysicalDeviceSurfaceFormats2KHR : NULL;
        return true;
    }

    // Functions for the VK_KHR_swapchain extension:

    // Note: This is a device extension, and its functions are statically
    // exported from the loader.  Per Khronos decisions, the loader's GIPA
    // function will return the trampoline function for such device-extension
    // functions, regardless of whether the extension has been enabled.
    if (!strcmp("vkCreateSwapchainKHR", name)) {
        *addr = (void *)vkCreateSwapchainKHR;
        return true;
    }
    if (!strcmp("vkDestroySwapchainKHR", name)) {
        *addr = (void *)vkDestroySwapchainKHR;
        return true;
    }
    if (!strcmp("vkGetSwapchainImagesKHR", name)) {
        *addr = (void *)vkGetSwapchainImagesKHR;
        return true;
    }
    if (!strcmp("vkAcquireNextImageKHR", name)) {
        *addr = (void *)vkAcquireNextImageKHR;
        return true;
    }
    if (!strcmp("vkQueuePresentKHR", name)) {
        *addr = (void *)vkQueuePresentKHR;
        return true;
    }
    if (!strcmp("vkAcquireNextImage2KHR", name)) {
        *addr = (void *)vkAcquireNextImage2KHR;
        return true;
    }

#ifdef VK_USE_PLATFORM_WIN32_KHR

    // Functions for the VK_KHR_win32_surface extension:
    if (!strcmp("vkCreateWin32SurfaceKHR", name)) {
        *addr = loader_inst->wsi_win32_surface_enabled ? (void *)vkCreateWin32SurfaceKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceWin32PresentationSupportKHR", name)) {
        *addr = loader_inst->wsi_win32_surface_enabled ? (void *)vkGetPhysicalDeviceWin32PresentationSupportKHR : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_WIN32_KHR
#ifdef VK_USE_PLATFORM_WAYLAND_KHR

    // Functions for the VK_KHR_wayland_surface extension:
    if (!strcmp("vkCreateWaylandSurfaceKHR", name)) {
        *addr = loader_inst->wsi_wayland_surface_enabled ? (void *)vkCreateWaylandSurfaceKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceWaylandPresentationSupportKHR", name)) {
        *addr = loader_inst->wsi_wayland_surface_enabled ? (void *)vkGetPhysicalDeviceWaylandPresentationSupportKHR : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_WAYLAND_KHR
#ifdef VK_USE_PLATFORM_XCB_KHR

    // Functions for the VK_KHR_xcb_surface extension:
    if (!strcmp("vkCreateXcbSurfaceKHR", name)) {
        *addr = loader_inst->wsi_xcb_surface_enabled ? (void *)vkCreateXcbSurfaceKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceXcbPresentationSupportKHR", name)) {
        *addr = loader_inst->wsi_xcb_surface_enabled ? (void *)vkGetPhysicalDeviceXcbPresentationSupportKHR : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_XCB_KHR
#ifdef VK_USE_PLATFORM_XLIB_KHR

    // Functions for the VK_KHR_xlib_surface extension:
    if (!strcmp("vkCreateXlibSurfaceKHR", name)) {
        *addr = loader_inst->wsi_xlib_surface_enabled ? (void *)vkCreateXlibSurfaceKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceXlibPresentationSupportKHR", name)) {
        *addr = loader_inst->wsi_xlib_surface_enabled ? (void *)vkGetPhysicalDeviceXlibPresentationSupportKHR : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_XLIB_KHR
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT

    // Functions for the VK_EXT_directfb_surface extension:
    if (!strcmp("vkCreateDirectFBSurfaceEXT", name)) {
        *addr = loader_inst->wsi_directfb_surface_enabled ? (void *)vkCreateDirectFBSurfaceEXT : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceDirectFBPresentationSupportEXT", name)) {
        *addr = loader_inst->wsi_directfb_surface_enabled ? (void *)vkGetPhysicalDeviceDirectFBPresentationSupportEXT : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_DIRECTFB_EXT
#ifdef VK_USE_PLATFORM_ANDROID_KHR

    // Functions for the VK_KHR_android_surface extension:
    if (!strcmp("vkCreateAndroidSurfaceKHR", name)) {
        *addr = loader_inst->wsi_android_surface_enabled ? (void *)vkCreateAndroidSurfaceKHR : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_ANDROID_KHR

#ifdef VK_USE_PLATFORM_MACOS_MVK

    // Functions for the VK_MVK_macos_surface extension:
    if (!strcmp("vkCreateMacOSSurfaceMVK", name)) {
        *addr = loader_inst->wsi_macos_surface_enabled ? (void *)vkCreateMacOSSurfaceMVK : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_MACOS_MVK
#ifdef VK_USE_PLATFORM_IOS_MVK

    // Functions for the VK_MVK_ios_surface extension:
    if (!strcmp("vkCreateIOSSurfaceMVK", name)) {
        *addr = loader_inst->wsi_ios_surface_enabled ? (void *)vkCreateIOSSurfaceMVK : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_IOS_MVK
#ifdef VK_USE_PLATFORM_GGP

    // Functions for the VK_GGP_stream_descriptor_surface extension:
    if (!strcmp("vkCreateStreamDescriptorSurfaceGGP", name)) {
        *addr = loader_inst->wsi_ggp_surface_enabled ? (void *)vkCreateStreamDescriptorSurfaceGGP : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_GGP
#ifdef VK_USE_PLATFORM_FUCHSIA

    // Functions for the VK_FUCHSIA_imagepipe_surface extension:
    if (!strcmp("vkCreateImagePipeSurfaceFUCHSIA", name)) {
        *addr = loader_inst->wsi_imagepipe_surface_enabled ? (void *)vkCreateImagePipeSurfaceFUCHSIA : NULL;
        return true;
    }

#endif  // VK_USE_PLATFORM_FUCHSIA

    // Functions for the VK_EXT_headless_surface extension:
    if (!strcmp("vkCreateHeadlessSurfaceEXT", name)) {
        *addr = loader_inst->wsi_headless_surface_enabled ? (void *)vkCreateHeadlessSurfaceEXT : NULL;
        return true;
    }

#if defined(VK_USE_PLATFORM_METAL_EXT)
    // Functions for the VK_MVK_macos_surface extension:
    if (!strcmp("vkCreateMetalSurfaceEXT", name)) {
        *addr = loader_inst->wsi_metal_surface_enabled ? (void *)vkCreateMetalSurfaceEXT : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_METAL_EXT

#ifdef VK_USE_PLATFORM_SCREEN_QNX

    // Functions for the VK_QNX_screen_surface extension:
    if (!strcmp("vkCreateScreenSurfaceQNX", name)) {
        *addr = loader_inst->wsi_screen_surface_enabled ? (void *)vkCreateScreenSurfaceQNX : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceScreenPresentationSupportQNX", name)) {
        *addr = loader_inst->wsi_screen_surface_enabled ? (void *)vkGetPhysicalDeviceScreenPresentationSupportQNX : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_SCREEN_QNX

    // Functions for VK_KHR_display extension:
    if (!strcmp("vkGetPhysicalDeviceDisplayPropertiesKHR", name)) {
        *addr = loader_inst->wsi_display_enabled ? (void *)vkGetPhysicalDeviceDisplayPropertiesKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceDisplayPlanePropertiesKHR", name)) {
        *addr = loader_inst->wsi_display_enabled ? (void *)vkGetPhysicalDeviceDisplayPlanePropertiesKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetDisplayPlaneSupportedDisplaysKHR", name)) {
        *addr = loader_inst->wsi_display_enabled ? (void *)vkGetDisplayPlaneSupportedDisplaysKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetDisplayModePropertiesKHR", name)) {
        *addr = loader_inst->wsi_display_enabled ? (void *)vkGetDisplayModePropertiesKHR : NULL;
        return true;
    }
    if (!strcmp("vkCreateDisplayModeKHR", name)) {
        *addr = loader_inst->wsi_display_enabled ? (void *)vkCreateDisplayModeKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetDisplayPlaneCapabilitiesKHR", name)) {
        *addr = loader_inst->wsi_display_enabled ? (void *)vkGetDisplayPlaneCapabilitiesKHR : NULL;
        return true;
    }
    if (!strcmp("vkCreateDisplayPlaneSurfaceKHR", name)) {
        *addr = loader_inst->wsi_display_enabled ? (void *)vkCreateDisplayPlaneSurfaceKHR : NULL;
        return true;
    }

    // Functions for KHR_display_swapchain extension:
    if (!strcmp("vkCreateSharedSwapchainsKHR", name)) {
        *addr = (void *)vkCreateSharedSwapchainsKHR;
        return true;
    }

    // Functions for KHR_get_display_properties2
    if (!strcmp("vkGetPhysicalDeviceDisplayProperties2KHR", name)) {
        *addr = loader_inst->wsi_display_props2_enabled ? (void *)vkGetPhysicalDeviceDisplayProperties2KHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceDisplayPlaneProperties2KHR", name)) {
        *addr = loader_inst->wsi_display_props2_enabled ? (void *)vkGetPhysicalDeviceDisplayPlaneProperties2KHR : NULL;
        return true;
    }
    if (!strcmp("vkGetDisplayModeProperties2KHR", name)) {
        *addr = loader_inst->wsi_display_props2_enabled ? (void *)vkGetDisplayModeProperties2KHR : NULL;
        return true;
    }
    if (!strcmp("vkGetDisplayPlaneCapabilities2KHR", name)) {
        *addr = loader_inst->wsi_display_props2_enabled ? (void *)vkGetDisplayPlaneCapabilities2KHR : NULL;
        return true;
    }

    return false;
}
