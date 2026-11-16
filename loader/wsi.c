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
#include "generated/vk_loader_wsi.h"

// Linux WSI surface extensions are not always compiled into the loader. (Assume
// for Windows the KHR_win32_surface is always compiled into loader). A given
// Linux build environment might not have the headers required for building one
// of the three extensions  (Xlib, Xcb, Wayland).  Thus, need to check if
// the built loader actually supports the particular Linux surface extension.
// If not supported by the built loader it will not be included in the list of
// enumerated instance extensions.  This solves the issue where an ICD or layer
// advertises support for a given Linux surface extension but the loader was not
// built to support the extension.
bool wsi_unsupported_instance_extension(const VkExtensionProperties* ext_prop) {
#if !defined(VK_USE_PLATFORM_WAYLAND_KHR)
    if (!strcmp(ext_prop->extensionName, "VK_KHR_wayland_surface")) return true;
#endif  // VK_USE_PLATFORM_WAYLAND_KHR
#if !defined(VK_USE_PLATFORM_XCB_KHR)
    if (!strcmp(ext_prop->extensionName, "VK_KHR_xcb_surface")) return true;
#endif  // VK_USE_PLATFORM_XCB_KHR
#if !defined(VK_USE_PLATFORM_XLIB_KHR)
    if (!strcmp(ext_prop->extensionName, "VK_KHR_xlib_surface")) return true;
#endif  // VK_USE_PLATFORM_XLIB_KHR
#if !defined(VK_USE_PLATFORM_DIRECTFB_EXT)
    if (!strcmp(ext_prop->extensionName, "VK_EXT_directfb_surface")) return true;
#endif  // VK_USE_PLATFORM_DIRECTFB_EXT
#if !defined(VK_USE_PLATFORM_SCREEN_QNX)
    if (!strcmp(ext_prop->extensionName, "VK_QNX_screen_surface")) return true;
#endif  // VK_USE_PLATFORM_SCREEN_QNX

    return false;
}

// Functions for the VK_KHR_surface extension:

// This is the trampoline entrypoint for DestroySurfaceKHR
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface,
                                                             const VkAllocationCallbacks* pAllocator) {
    struct loader_instance* loader_inst = loader_validate_and_get_instance_dispatch(instance, __FUNCTION__);
    loader_inst->disp->layer_inst_disp.DestroySurfaceKHR(loader_inst->instance, surface, pAllocator);
}

// This is the instance chain terminator function for DestroySurfaceKHR
VKAPI_ATTR void VKAPI_CALL terminator_DestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface,
                                                        const VkAllocationCallbacks* pAllocator) {
    struct loader_instance* loader_inst = loader_get_instance(instance);

    // NOLINTNEXTLINE(performance-no-int-to-ptr) - VkSurfaceKHR handle decode requires round-tripping through uintptr_t
    VkIcdSurface* icd_surface = (VkIcdSurface*)(uintptr_t)surface;
    if (NULL != icd_surface) {
        // Every surface creation terminator holds loader_lock while it reserves a slot in loader_inst->surfaces_list and
        // grows each driver's surface_list, either of which can reallocate the list. vkDestroySurfaceKHR only requires
        // the surface itself to be externally synchronized, so take the same lock before touching that shared state.
        loader_platform_thread_lock_mutex(&loader_lock);
        for (struct loader_icd_term* icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next) {
            if (icd_term->enabled_instance_extensions.khr_surface &&
                icd_term->scanned_icd->interface_version >= ICD_VER_SUPPORTS_ICD_SURFACE_KHR &&
                NULL != icd_term->dispatch.DestroySurfaceKHR && icd_term->surface_list.list[icd_surface->surface_index]) {
                icd_term->dispatch.DestroySurfaceKHR(icd_term->instance, icd_term->surface_list.list[icd_surface->surface_index],
                                                     pAllocator);
                // NOLINTNEXTLINE(performance-no-int-to-ptr) - encoding the NULL non-dispatchable handle, not a real pointer
                icd_term->surface_list.list[icd_surface->surface_index] = (VkSurfaceKHR)(uintptr_t)NULL;

            } else {
                // The real_icd_surface for any ICD not supporting the
                // proper interface version should be NULL.  If not, then
                // we have a problem.
                // NOLINTNEXTLINE(performance-no-int-to-ptr) - encoding the NULL non-dispatchable handle, not a real pointer
                assert(!(icd_term->enabled_instance_extensions.khr_surface &&
                         icd_term->scanned_icd->interface_version >= ICD_VER_SUPPORTS_ICD_SURFACE_KHR) ||
                       (VkSurfaceKHR)(uintptr_t)NULL == icd_term->surface_list.list[icd_surface->surface_index]);
            }
        }
        if (NULL != icd_surface->create_info) {
            loader_instance_heap_free(loader_inst, icd_surface->create_info);
        }
        loader_release_object_from_list(&loader_inst->surfaces_list, icd_surface->surface_index);
        loader_platform_thread_unlock_mutex(&loader_lock);
        // NOLINTNEXTLINE(performance-no-int-to-ptr) - decoding the loader-internal pointer out of the handle
        loader_instance_heap_free(loader_inst, (void*)(uintptr_t)surface);
    }
}

// This is the trampoline entrypoint for GetPhysicalDeviceSurfaceSupportKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                  uint32_t queueFamilyIndex, VkSurfaceKHR surface,
                                                                                  VkBool32* pSupported) {
    const VkLayerInstanceDispatchTable* disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceSurfaceSupportKHR(unwrapped_phys_dev, queueFamilyIndex, surface, pSupported);
}

// This is the instance chain terminator function for
// GetPhysicalDeviceSurfaceSupportKHR
VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice,
                                                                             uint32_t queueFamilyIndex, VkSurfaceKHR surface,
                                                                             VkBool32* pSupported) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    struct loader_instance* loader_inst = (struct loader_instance*)icd_term->this_instance;
    if (!loader_inst->enabled_extensions.khr_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_surface extension not enabled. vkGetPhysicalDeviceSurfaceSupportKHR not executed!");
        return VK_SUCCESS;
    }

    if (NULL == pSupported) {
        loader_log(loader_inst, VULKAN_LOADER_FATAL_ERROR_BIT | VULKAN_LOADER_ERROR_BIT, 0,
                   "NULL pointer passed into vkGetPhysicalDeviceSurfaceSupportKHR for pSupported!");
        abort();
    }
    *pSupported = false;

    if (NULL == icd_term->dispatch.GetPhysicalDeviceSurfaceSupportKHR) {
        // set pSupported to false as this driver doesn't support WSI functionality
        *pSupported = false;
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device does not export vkGetPhysicalDeviceSurfaceSupportKHR!");
        return VK_SUCCESS;
    }

    VkResult res = wsi_unwrap_icd_surface(icd_term, &surface);
    if (res != VK_SUCCESS) {
        // set pSupported to false as this driver doesn't support WSI functionality
        *pSupported = false;
        return VK_SUCCESS;
    }

    return icd_term->dispatch.GetPhysicalDeviceSurfaceSupportKHR(phys_dev_term->phys_dev, queueFamilyIndex, surface, pSupported);
}

// This is the trampoline entrypoint for GetPhysicalDeviceSurfaceCapabilitiesKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities) {
    const VkLayerInstanceDispatchTable* disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceSurfaceCapabilitiesKHR(unwrapped_phys_dev, surface, pSurfaceCapabilities);
}

// This is the instance chain terminator function for
// GetPhysicalDeviceSurfaceCapabilitiesKHR
VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice,
                                                                                  VkSurfaceKHR surface,
                                                                                  VkSurfaceCapabilitiesKHR* pSurfaceCapabilities) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    struct loader_instance* loader_inst = (struct loader_instance*)icd_term->this_instance;
    if (!loader_inst->enabled_extensions.khr_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_surface extension not enabled. vkGetPhysicalDeviceSurfaceCapabilitiesKHR not executed!");
        return VK_SUCCESS;
    }

    if (NULL == pSurfaceCapabilities) {
        loader_log(loader_inst, VULKAN_LOADER_FATAL_ERROR_BIT | VULKAN_LOADER_ERROR_BIT, 0,
                   "NULL pointer passed into vkGetPhysicalDeviceSurfaceCapabilitiesKHR for pSurfaceCapabilities!");
        abort();
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceSurfaceCapabilitiesKHR) {
        // Zero out the capabilities as this driver doesn't support WSI functionality
        memset(pSurfaceCapabilities, 0, sizeof(VkSurfaceCapabilitiesKHR));
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device does not export vkGetPhysicalDeviceSurfaceCapabilitiesKHR!");
        return VK_SUCCESS;
    }

    VkResult res = wsi_unwrap_icd_surface(icd_term, &surface);
    if (res != VK_SUCCESS) {
        return res;
    }

    return icd_term->dispatch.GetPhysicalDeviceSurfaceCapabilitiesKHR(phys_dev_term->phys_dev, surface, pSurfaceCapabilities);
}

// This is the trampoline entrypoint for GetPhysicalDeviceSurfaceFormatsKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice,
                                                                                  VkSurfaceKHR surface,
                                                                                  uint32_t* pSurfaceFormatCount,
                                                                                  VkSurfaceFormatKHR* pSurfaceFormats) {
    const VkLayerInstanceDispatchTable* disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceSurfaceFormatsKHR(unwrapped_phys_dev, surface, pSurfaceFormatCount, pSurfaceFormats);
}

// This is the instance chain terminator function for
// GetPhysicalDeviceSurfaceFormatsKHR
VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                                                             uint32_t* pSurfaceFormatCount,
                                                                             VkSurfaceFormatKHR* pSurfaceFormats) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    struct loader_instance* loader_inst = (struct loader_instance*)icd_term->this_instance;
    if (!loader_inst->enabled_extensions.khr_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_surface extension not enabled. vkGetPhysicalDeviceSurfaceFormatsKHR not executed!");
        return VK_SUCCESS;
    }

    if (NULL == pSurfaceFormatCount) {
        loader_log(loader_inst, VULKAN_LOADER_FATAL_ERROR_BIT | VULKAN_LOADER_ERROR_BIT, 0,
                   "NULL pointer passed into vkGetPhysicalDeviceSurfaceFormatsKHR for pSurfaceFormatCount!");
        abort();
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceSurfaceFormatsKHR) {
        // Zero out the format count as this driver doesn't support WSI functionality
        *pSurfaceFormatCount = 0;
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device does not export vkGetPhysicalDeviceSurfaceFormatsKHR!");
        return VK_SUCCESS;
    }

    if (VK_NULL_HANDLE != surface) {
        VkResult res = wsi_unwrap_icd_surface(icd_term, &surface);
        if (res != VK_SUCCESS) {
            return res;
        }
    }

    return icd_term->dispatch.GetPhysicalDeviceSurfaceFormatsKHR(phys_dev_term->phys_dev, surface, pSurfaceFormatCount,
                                                                 pSurfaceFormats);
}

// This is the trampoline entrypoint for GetPhysicalDeviceSurfacePresentModesKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice,
                                                                                       VkSurfaceKHR surface,
                                                                                       uint32_t* pPresentModeCount,
                                                                                       VkPresentModeKHR* pPresentModes) {
    const VkLayerInstanceDispatchTable* disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceSurfacePresentModesKHR(unwrapped_phys_dev, surface, pPresentModeCount, pPresentModes);
}

// This is the instance chain terminator function for
// GetPhysicalDeviceSurfacePresentModesKHR
VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice,
                                                                                  VkSurfaceKHR surface, uint32_t* pPresentModeCount,
                                                                                  VkPresentModeKHR* pPresentModes) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    struct loader_instance* loader_inst = (struct loader_instance*)icd_term->this_instance;
    if (!loader_inst->enabled_extensions.khr_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_surface extension not enabled. vkGetPhysicalDeviceSurfacePresentModesKHR not executed!");
        return VK_SUCCESS;
    }

    if (NULL == pPresentModeCount) {
        loader_log(loader_inst, VULKAN_LOADER_FATAL_ERROR_BIT | VULKAN_LOADER_ERROR_BIT, 0,
                   "NULL pointer passed into vkGetPhysicalDeviceSurfacePresentModesKHR for pPresentModeCount!");
        abort();
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceSurfacePresentModesKHR) {
        // Zero out the present mode count as this driver doesn't support WSI functionality
        *pPresentModeCount = 0;
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device does not export vkGetPhysicalDeviceSurfacePresentModesKHR!");
        return VK_SUCCESS;
    }

    if (VK_NULL_HANDLE != surface) {
        VkResult res = wsi_unwrap_icd_surface(icd_term, &surface);
        if (res != VK_SUCCESS) {
            return res;
        }
    }

    return icd_term->dispatch.GetPhysicalDeviceSurfacePresentModesKHR(phys_dev_term->phys_dev, surface, pPresentModeCount,
                                                                      pPresentModes);
}

// Functions for the VK_KHR_swapchain extension:

// This is the trampoline entrypoint for CreateSwapchainKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo,
                                                                  const VkAllocationCallbacks* pAllocator,
                                                                  VkSwapchainKHR* pSwapchain) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    if (NULL == disp->CreateSwapchainKHR) {
        struct loader_device* dev = *((struct loader_device**)device);
        loader_log(NULL != dev ? dev->phys_dev_term->this_icd_term->this_instance : NULL,
                   VULKAN_LOADER_FATAL_ERROR_BIT | VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateSwapchainKHR: Driver's function pointer was NULL, returning VK_SUCCESS. Was the VK_KHR_swapchain "
                   "extension enabled?");
        abort();
    }
    return disp->CreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo,
                                                             const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain) {
    struct loader_device* dev;
    struct loader_icd_term* icd_term = loader_get_icd_and_device(device, &dev);
    if (NULL == icd_term || NULL == dev) {
        loader_log(NULL, VULKAN_LOADER_FATAL_ERROR_BIT | VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateSwapchainKHR Terminator: device handle. This is likely the result of a "
                   "layer wrapping device handles and failing to unwrap them in all functions. "
                   "[VUID-vkCreateSwapchainKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    if (NULL == pCreateInfo) {
        loader_log(NULL, VULKAN_LOADER_FATAL_ERROR_BIT | VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateSwapchainKHR: Invalid pCreateInfo pointer [VUID-vkCreateSwapchainKHR-pCreateInfo-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    // Need to gracefully handle the function pointer not being found.
    if (NULL == dev->loader_dispatch.extension_terminator_dispatch.CreateSwapchainKHR) {
        loader_log(NULL, VULKAN_LOADER_FATAL_ERROR_BIT | VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateSwapchainKHR: Driver's function pointer was NULL, returning VK_SUCCESS. Was the VK_KHR_swapchain "
                   "extension enabled?");
        return VK_SUCCESS;
    }

    VkSwapchainCreateInfoKHR create_info_copy = *pCreateInfo;
    VkResult res = wsi_unwrap_icd_surface(icd_term, &create_info_copy.surface);
    if (res != VK_SUCCESS) {
        return res;
    }

    return dev->loader_dispatch.extension_terminator_dispatch.CreateSwapchainKHR(device, &create_info_copy, pAllocator, pSwapchain);
}

// This is the trampoline entrypoint for DestroySwapchainKHR
LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain,
                                                               const VkAllocationCallbacks* pAllocator) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->DestroySwapchainKHR(device, swapchain, pAllocator);
}

// This is the trampoline entrypoint for GetSwapchainImagesKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain,
                                                                     uint32_t* pSwapchainImageCount, VkImage* pSwapchainImages) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->GetSwapchainImagesKHR(device, swapchain, pSwapchainImageCount, pSwapchainImages);
}

// This is the trampoline entrypoint for AcquireNextImageKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkAcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout,
                                                                   VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->AcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, pImageIndex);
}

// This is the trampoline entrypoint for QueuePresentKHR
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_queue_dispatch(queue, __FUNCTION__);
    return disp->QueuePresentKHR(queue, pPresentInfo);
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters) - base_size/platform_size are plain sizes with no safer distinguishing type
VkResult allocate_icd_surface_struct(struct loader_instance* instance, size_t base_size, size_t platform_size,
                                     const VkAllocationCallbacks* pAllocator, VkIcdSurface** out_icd_surface) {
    uint32_t next_index = 0;
    VkIcdSurface* icd_surface = NULL;
    VkResult res = loader_get_next_available_entry(instance, &instance->surfaces_list, &next_index, pAllocator);
    if (res != VK_SUCCESS) {
        return res;
    }

    // Next, if so, proceed with the implementation of this function:
    icd_surface = loader_instance_heap_calloc(instance, sizeof(VkIcdSurface), VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
    if (icd_surface == NULL) {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }
    // Setup the new sizes and offsets so we can grow the structures in the
    // future without having problems
    icd_surface->base_size = (uint32_t)base_size;
    icd_surface->platform_size = (uint32_t)platform_size;
    icd_surface->non_platform_offset = (uint32_t)((uint8_t*)(&icd_surface->base_size) - (uint8_t*)icd_surface);
    icd_surface->entire_size = sizeof(VkIcdSurface);
    icd_surface->surface_index = next_index;
    icd_surface->create_info = NULL;

    for (struct loader_icd_term* icd_term = instance->icd_terms; icd_term != NULL; icd_term = icd_term->next) {
        if (icd_term->enabled_instance_extensions.khr_surface &&
            icd_term->scanned_icd->interface_version >= ICD_VER_SUPPORTS_ICD_SURFACE_KHR) {
            if (icd_term->surface_list.list == NULL) {
                res =
                    loader_init_generic_list(instance, (struct loader_generic_list*)&icd_term->surface_list, sizeof(VkSurfaceKHR));
                if (res != VK_SUCCESS) {
                    goto out;
                }
            } else if (icd_term->surface_list.capacity <= next_index * sizeof(VkSurfaceKHR)) {
                res = loader_resize_generic_list(instance, (struct loader_generic_list*)&icd_term->surface_list);
                if (res != VK_SUCCESS) {
                    goto out;
                }
            }
        }
    }

    *out_icd_surface = icd_surface;
out:
    if (res != VK_SUCCESS) {
        loader_instance_heap_free(instance, icd_surface);
        // Give back the slot reserved above, the caller has no surface to release it through
        loader_release_object_from_list(&instance->surfaces_list, next_index);
        // cleanup of icd_term->surface_list is done during instance destruction
    }
    return res;
}

VkResult copy_surface_create_info(struct loader_instance* loader_inst, VkIcdSurface* icd_surface, const void* create_info,
                                  size_t struct_type_info_count, const struct loader_struct_type_info* struct_type_info,
                                  const char* base_struct_name, const VkAllocationCallbacks* pAllocator) {
    size_t create_info_total_size = 0;
    const void* pnext = create_info;
    while (NULL != pnext) {
        VkBaseInStructure base = {0};
        memcpy(&base, pnext, sizeof(VkBaseInStructure));
        bool known_struct = false;
        for (size_t i = 0; i < struct_type_info_count; ++i) {
            if (base.sType == struct_type_info[i].stype) {
                create_info_total_size += loader_aligned_size(struct_type_info[i].size);
                pnext = base.pNext;
                known_struct = true;
                break;
            }
        }
        if (!known_struct) {
            loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0, "Unsupported extension structure in %s pNext chain.",
                       base_struct_name);
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    icd_surface->create_info = loader_instance_heap_calloc(loader_inst, create_info_total_size, VK_SYSTEM_ALLOCATION_SCOPE_OBJECT);
    if (NULL == icd_surface->create_info) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    uint8_t* dst = icd_surface->create_info;
    VkBaseInStructure* prev_struct = NULL;
    pnext = create_info;
    while (NULL != pnext) {
        VkBaseInStructure base = {0};
        memcpy(&base, pnext, sizeof(VkBaseInStructure));
        for (size_t i = 0; i < struct_type_info_count; ++i) {
            if (base.sType == struct_type_info[i].stype) {
                memcpy(dst, pnext, struct_type_info[i].size);
                if (NULL != prev_struct) {
                    prev_struct->pNext = (const VkBaseInStructure*)dst;
                }
                prev_struct = (VkBaseInStructure*)dst;
                dst += loader_aligned_size(struct_type_info[i].size);
                pnext = base.pNext;
                break;
            }
        }
    }
    if (pAllocator) {
        icd_surface->callbacks_valid = true;
        icd_surface->callbacks = *pAllocator;
    }

    return VK_SUCCESS;
}

void cleanup_surface_creation(struct loader_instance* loader_inst, VkResult result, VkIcdSurface* icd_surface,
                              const VkAllocationCallbacks* pAllocator) {
    if (VK_SUCCESS != result && NULL != icd_surface) {
        for (struct loader_icd_term* icd_term = loader_inst->icd_terms; icd_term != NULL; icd_term = icd_term->next) {
            if (icd_term->enabled_instance_extensions.khr_surface && NULL != icd_term->surface_list.list &&
                icd_term->surface_list.capacity > icd_surface->surface_index * sizeof(VkSurfaceKHR) &&
                icd_term->surface_list.list[icd_surface->surface_index] && NULL != icd_term->dispatch.DestroySurfaceKHR) {
                icd_term->dispatch.DestroySurfaceKHR(icd_term->instance, icd_term->surface_list.list[icd_surface->surface_index],
                                                     pAllocator);
            }
        }
        loader_release_object_from_list(&loader_inst->surfaces_list, icd_surface->surface_index);
        if (NULL != icd_surface->create_info) {
            loader_instance_heap_free(loader_inst, icd_surface->create_info);
        }
        loader_instance_heap_free(loader_inst, icd_surface);
    }
}

// Ensure we are properly setting VK_USE_PLATFORM_METAL_EXT, VK_USE_PLATFORM_IOS_MVK, and VK_USE_PLATFORM_MACOS_MVK.
#if __APPLE__

#ifndef VK_USE_PLATFORM_METAL_EXT
#error "VK_USE_PLATFORM_METAL_EXT not defined!"
#endif

#include <TargetConditionals.h>

#if TARGET_OS_IOS

#ifndef VK_USE_PLATFORM_IOS_MVK
#error "VK_USE_PLATFORM_IOS_MVK not defined!"
#endif

#endif  //  TARGET_OS_IOS

#if TARGET_OS_OSX

#ifndef VK_USE_PLATFORM_MACOS_MVK
#error "VK_USE_PLATFORM_MACOS_MVK not defined!"
#endif

#endif  // TARGET_OS_OSX

#endif  // __APPLE__

// EXT_display_swapchain Extension command

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateSharedSwapchainsKHR(VkDevice device, uint32_t swapchainCount,
                                                                         const VkSwapchainCreateInfoKHR* pCreateInfos,
                                                                         const VkAllocationCallbacks* pAllocator,
                                                                         VkSwapchainKHR* pSwapchains) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->CreateSharedSwapchainsKHR(device, swapchainCount, pCreateInfos, pAllocator, pSwapchains);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateSharedSwapchainsKHR(VkDevice device, uint32_t swapchainCount,
                                                                    const VkSwapchainCreateInfoKHR* pCreateInfos,
                                                                    const VkAllocationCallbacks* pAllocator,
                                                                    VkSwapchainKHR* pSwapchains) {
    struct loader_device* dev;
    struct loader_icd_term* icd_term = loader_get_icd_and_device(device, &dev);
    if (NULL == icd_term || NULL == dev) {
        loader_log(NULL, VULKAN_LOADER_FATAL_ERROR_BIT | VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateSharedSwapchainsKHR Terminator: Invalid device handle. This is likely the result of a "
                   "layer wrapping device handles and failing to unwrap them in all functions. "
                   "[VUID-vkCreateSharedSwapchainsKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    if (NULL == dev->loader_dispatch.extension_terminator_dispatch.CreateSharedSwapchainsKHR) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT, 0,
                   "vkCreateSharedSwapchainsKHR Terminator: Driver's function pointer was NULL, returning VK_SUCCESS. Was the "
                   "VK_KHR_display_swapchain extension enabled?");
        return VK_SUCCESS;
    }
    VkResult res = VK_SUCCESS;
    VkSwapchainCreateInfoKHR* pCreateCopy = loader_calloc_with_instance_fallback(
        pAllocator, icd_term->this_instance, sizeof(VkSwapchainCreateInfoKHR) * swapchainCount, VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
    if (NULL == pCreateCopy) {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }
    memcpy(pCreateCopy, pCreateInfos, sizeof(VkSwapchainCreateInfoKHR) * swapchainCount);
    for (uint32_t sc = 0; sc < swapchainCount; sc++) {
        res = wsi_unwrap_icd_surface(icd_term, &pCreateCopy[sc].surface);
        if (res != VK_SUCCESS) {
            goto out;
        }
    }
    res = dev->loader_dispatch.extension_terminator_dispatch.CreateSharedSwapchainsKHR(device, swapchainCount, pCreateCopy,
                                                                                       pAllocator, pSwapchains);

out:
    loader_free_with_instance_fallback(pAllocator, icd_term->this_instance, pCreateCopy);
    return res;
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkGetDeviceGroupPresentCapabilitiesKHR(VkDevice device, VkDeviceGroupPresentCapabilitiesKHR* pDeviceGroupPresentCapabilities) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->GetDeviceGroupPresentCapabilitiesKHR(device, pDeviceGroupPresentCapabilities);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetDeviceGroupSurfacePresentModesKHR(VkDevice device, VkSurfaceKHR surface,
                                                                                    VkDeviceGroupPresentModeFlagsKHR* pModes) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->GetDeviceGroupSurfacePresentModesKHR(device, surface, pModes);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetDeviceGroupSurfacePresentModesKHR(VkDevice device, VkSurfaceKHR surface,
                                                                               VkDeviceGroupPresentModeFlagsKHR* pModes) {
    struct loader_device* dev;
    struct loader_icd_term* icd_term = loader_get_icd_and_device(device, &dev);
    if (NULL == icd_term || NULL == dev) {
        loader_log(NULL, VULKAN_LOADER_FATAL_ERROR_BIT | VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkGetDeviceGroupSurfacePresentModesKHR: Invalid device "
                   "[VUID-vkGetDeviceGroupSurfacePresentModesKHR-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    if (NULL == dev->loader_dispatch.extension_terminator_dispatch.GetDeviceGroupSurfacePresentModesKHR) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT, 0,
                   "vkGetDeviceGroupSurfacePresentModesKHR: Driver's function pointer was NULL, returning VK_SUCCESS. Was either "
                   "Vulkan 1.1 and VK_KHR_swapchain enabled or both the VK_KHR_device_group and VK_KHR_surface "
                   "extensions enabled when using Vulkan 1.0?");
        return VK_SUCCESS;
    }

    VkResult res = wsi_unwrap_icd_surface(icd_term, &surface);
    if (res != VK_SUCCESS) {
        return res;
    }

    return dev->loader_dispatch.extension_terminator_dispatch.GetDeviceGroupSurfacePresentModesKHR(device, surface, pModes);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDevicePresentRectanglesKHR(VkPhysicalDevice physicalDevice,
                                                                                     VkSurfaceKHR surface, uint32_t* pRectCount,
                                                                                     VkRect2D* pRects) {
    const VkLayerInstanceDispatchTable* disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDevicePresentRectanglesKHR(unwrapped_phys_dev, surface, pRectCount, pRects);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDevicePresentRectanglesKHR(VkPhysicalDevice physicalDevice,
                                                                                VkSurfaceKHR surface, uint32_t* pRectCount,
                                                                                VkRect2D* pRects) {
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.GetPhysicalDevicePresentRectanglesKHR) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support GetPhysicalDevicePresentRectanglesKHX");
        // return as this driver doesn't support WSI functionality
        if (pRectCount) {
            *pRectCount = 0;
        }
        return VK_SUCCESS;
    }

    VkResult res = wsi_unwrap_icd_surface(icd_term, &surface);
    if (res != VK_SUCCESS) {
        return res;
    }

    return icd_term->dispatch.GetPhysicalDevicePresentRectanglesKHR(phys_dev_term->phys_dev, surface, pRectCount, pRects);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkAcquireNextImage2KHR(VkDevice device, const VkAcquireNextImageInfoKHR* pAcquireInfo,
                                                                    uint32_t* pImageIndex) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    return disp->AcquireNextImage2KHR(device, pAcquireInfo, pImageIndex);
}

// ---- VK_KHR_get_display_properties2 extension trampoline/terminators

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceDisplayProperties2KHR(VkPhysicalDevice physicalDevice,
                                                                                      uint32_t* pPropertyCount,
                                                                                      VkDisplayProperties2KHR* pProperties) {
    const VkLayerInstanceDispatchTable* disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceDisplayProperties2KHR(unwrapped_phys_dev, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceDisplayProperties2KHR(VkPhysicalDevice physicalDevice,
                                                                                 uint32_t* pPropertyCount,
                                                                                 VkDisplayProperties2KHR* pProperties) {
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;

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
    VkResult res = VK_SUCCESS;
    // If we do have to write to pProperties, then we need to write to a temporary array of VkDisplayPropertiesKHR and copy it
    uint32_t allocated_count = *pPropertyCount;
    VkDisplayPropertiesKHR* properties = loader_instance_heap_calloc(
        icd_term->this_instance, allocated_count * sizeof(VkDisplayPropertiesKHR), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
    if (properties == NULL) {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }
    res = icd_term->dispatch.GetPhysicalDeviceDisplayPropertiesKHR(phys_dev_term->phys_dev, pPropertyCount, properties);
    if (res < 0) {
        goto out;
    }
    // The driver reports the written count back in pPropertyCount; never copy past the array we sized.
    for (uint32_t i = 0; i < *pPropertyCount && i < allocated_count; ++i) {
        memcpy(&pProperties[i].displayProperties, &properties[i], sizeof(VkDisplayPropertiesKHR));
    }
out:
    loader_instance_heap_free(icd_term->this_instance, properties);
    return res;
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceDisplayPlaneProperties2KHR(
    VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkDisplayPlaneProperties2KHR* pProperties) {
    const VkLayerInstanceDispatchTable* disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceDisplayPlaneProperties2KHR(unwrapped_phys_dev, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceDisplayPlaneProperties2KHR(VkPhysicalDevice physicalDevice,
                                                                                      uint32_t* pPropertyCount,
                                                                                      VkDisplayPlaneProperties2KHR* pProperties) {
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;

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
    VkResult res = VK_SUCCESS;
    // If we do have to write to pProperties, then we need to write to a temporary array of VkDisplayPlanePropertiesKHR and copy it
    uint32_t allocated_count = *pPropertyCount;
    VkDisplayPlanePropertiesKHR* properties = loader_instance_heap_calloc(
        icd_term->this_instance, allocated_count * sizeof(VkDisplayPlanePropertiesKHR), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
    if (properties == NULL) {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }
    res = icd_term->dispatch.GetPhysicalDeviceDisplayPlanePropertiesKHR(phys_dev_term->phys_dev, pPropertyCount, properties);
    if (res < 0) {
        goto out;
    }
    // The driver reports the written count back in pPropertyCount; never copy past the array we sized.
    for (uint32_t i = 0; i < *pPropertyCount && i < allocated_count; ++i) {
        memcpy(&pProperties[i].displayPlaneProperties, &properties[i], sizeof(VkDisplayPlanePropertiesKHR));
    }

out:
    loader_instance_heap_free(icd_term->this_instance, properties);
    return res;
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetDisplayModeProperties2KHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display,
                                                                            uint32_t* pPropertyCount,
                                                                            VkDisplayModeProperties2KHR* pProperties) {
    const VkLayerInstanceDispatchTable* disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetDisplayModeProperties2KHR(unwrapped_phys_dev, display, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetDisplayModeProperties2KHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display,
                                                                       uint32_t* pPropertyCount,
                                                                       VkDisplayModeProperties2KHR* pProperties) {
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;

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
    VkResult res = VK_SUCCESS;
    uint32_t allocated_count = *pPropertyCount;
    VkDisplayModePropertiesKHR* properties = loader_instance_heap_calloc(
        icd_term->this_instance, allocated_count * sizeof(VkDisplayModePropertiesKHR), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
    if (properties == NULL) {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }
    res = icd_term->dispatch.GetDisplayModePropertiesKHR(phys_dev_term->phys_dev, display, pPropertyCount, properties);
    if (res < 0) {
        goto out;
    }
    // The driver reports the written count back in pPropertyCount; never copy past the array we sized.
    for (uint32_t i = 0; i < *pPropertyCount && i < allocated_count; ++i) {
        memcpy(&pProperties[i].displayModeProperties, &properties[i], sizeof(VkDisplayModePropertiesKHR));
    }
out:
    loader_instance_heap_free(icd_term->this_instance, properties);
    return res;
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetDisplayPlaneCapabilities2KHR(VkPhysicalDevice physicalDevice,
                                                                               const VkDisplayPlaneInfo2KHR* pDisplayPlaneInfo,
                                                                               VkDisplayPlaneCapabilities2KHR* pCapabilities) {
    const VkLayerInstanceDispatchTable* disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetDisplayPlaneCapabilities2KHR(unwrapped_phys_dev, pDisplayPlaneInfo, pCapabilities);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetDisplayPlaneCapabilities2KHR(VkPhysicalDevice physicalDevice,
                                                                          const VkDisplayPlaneInfo2KHR* pDisplayPlaneInfo,
                                                                          VkDisplayPlaneCapabilities2KHR* pCapabilities) {
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;

    // If the function is available in the driver, just call into it
    if (icd_term->dispatch.GetDisplayPlaneCapabilities2KHR != NULL) {
        return icd_term->dispatch.GetDisplayPlaneCapabilities2KHR(phys_dev_term->phys_dev, pDisplayPlaneInfo, pCapabilities);
    }

    // We have to emulate the function.
    loader_log(icd_term->this_instance, VULKAN_LOADER_INFO_BIT, 0,
               "vkGetDisplayPlaneCapabilities2KHR: Emulating call in ICD \"%s\"", icd_term->scanned_icd->lib_name);

    // If the icd doesn't support VK_KHR_display, then there are no capabilities
    if (NULL == icd_term->dispatch.GetDisplayPlaneCapabilitiesKHR) {
        if (pCapabilities) {
            memset(&pCapabilities->capabilities, 0, sizeof(VkDisplayPlaneCapabilitiesKHR));
        }
        return VK_SUCCESS;
    }

    // Just call into the old version of the function.
    return icd_term->dispatch.GetDisplayPlaneCapabilitiesKHR(phys_dev_term->phys_dev, pDisplayPlaneInfo->mode,
                                                             pDisplayPlaneInfo->planeIndex, &pCapabilities->capabilities);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceSurfaceCapabilities2KHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
                                           VkSurfaceCapabilities2KHR* pSurfaceCapabilities) {
    const VkLayerInstanceDispatchTable* disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceSurfaceCapabilities2KHR(unwrapped_phys_dev, pSurfaceInfo, pSurfaceCapabilities);
}

void emulate_VK_KHR_surface_maintenance1(const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
                                         VkSurfaceCapabilities2KHR* pSurfaceCapabilities) {
    // Because VK_KHR_surface_maintenance1 is an instance extension, applications will use it to query info on drivers which do
    // not support the extension. Thus we need to emulate the driver filling out the structs in that case.
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_MAX_ENUM_KHR;
    const void* void_pNext = pSurfaceInfo->pNext;
    while (void_pNext) {
        VkBaseOutStructure out_structure = {0};
        memcpy(&out_structure, void_pNext, sizeof(VkBaseOutStructure));
        if (out_structure.sType == VK_STRUCTURE_TYPE_SURFACE_PRESENT_MODE_KHR) {
            VkSurfacePresentModeKHR* surface_present_mode = (VkSurfacePresentModeKHR*)void_pNext;
            present_mode = surface_present_mode->presentMode;
        }
        void_pNext = out_structure.pNext;
    }
    // If no VkSurfacePresentModeKHR was present, return
    if (present_mode == VK_PRESENT_MODE_MAX_ENUM_KHR) {
        return;
    }

    void_pNext = pSurfaceCapabilities->pNext;
    while (void_pNext) {
        VkBaseOutStructure out_structure = {0};
        memcpy(&out_structure, void_pNext, sizeof(VkBaseOutStructure));
        if (out_structure.sType == VK_STRUCTURE_TYPE_SURFACE_PRESENT_MODE_COMPATIBILITY_KHR) {
            VkSurfacePresentModeCompatibilityKHR* surface_present_mode_compatibility =
                (VkSurfacePresentModeCompatibilityKHR*)void_pNext;
            if (surface_present_mode_compatibility->pPresentModes) {
                if (surface_present_mode_compatibility->presentModeCount != 0) {
                    surface_present_mode_compatibility->pPresentModes[0] = present_mode;
                    surface_present_mode_compatibility->presentModeCount = 1;
                }
            } else {
                surface_present_mode_compatibility->presentModeCount = 1;
            }

        } else if (out_structure.sType == VK_STRUCTURE_TYPE_SURFACE_PRESENT_SCALING_CAPABILITIES_KHR) {
            // Because there is no way to fill out the information faithfully, set scaled max/min image extent to the
            // surface capabilities max/min extent and the rest to zero.
            VkSurfacePresentScalingCapabilitiesKHR* surface_present_scaling_capabilities =
                (VkSurfacePresentScalingCapabilitiesKHR*)void_pNext;
            surface_present_scaling_capabilities->supportedPresentScaling = 0;
            surface_present_scaling_capabilities->supportedPresentGravityX = 0;
            surface_present_scaling_capabilities->supportedPresentGravityY = 0;
            surface_present_scaling_capabilities->maxScaledImageExtent = pSurfaceCapabilities->surfaceCapabilities.maxImageExtent;
            surface_present_scaling_capabilities->minScaledImageExtent = pSurfaceCapabilities->surfaceCapabilities.minImageExtent;
        }
        void_pNext = out_structure.pNext;
    }
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceSurfaceCapabilities2KHR(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
    VkSurfaceCapabilities2KHR* pSurfaceCapabilities) {
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    struct loader_instance* loader_inst = (struct loader_instance*)icd_term->this_instance;

    if (!loader_inst->enabled_extensions.khr_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_surface extension not enabled. vkGetPhysicalDeviceSurfaceCapabilities2KHR not executed!");
        return VK_SUCCESS;
    }

    VkSurfaceKHR surface = pSurfaceInfo->surface;
    if (VK_NULL_HANDLE != pSurfaceInfo->surface) {
        VkResult res = wsi_unwrap_icd_surface(icd_term, &surface);
        if (res != VK_SUCCESS) {
            return res;
        }
    }

    if (icd_term->dispatch.GetPhysicalDeviceSurfaceCapabilities2KHR != NULL) {
        void* pNext = pSurfaceCapabilities->pNext;
        while (pNext != NULL) {
            VkBaseOutStructure pNext_out_structure = {0};
            memcpy(&pNext_out_structure, pNext, sizeof(VkBaseOutStructure));
            if (pNext_out_structure.sType == VK_STRUCTURE_TYPE_SURFACE_PROTECTED_CAPABILITIES_KHR) {
                // Not all ICDs may be supporting VK_KHR_surface_protected_capabilities
                // Initialize VkSurfaceProtectedCapabilitiesKHR.supportsProtected to false and
                // if an ICD supports protected surfaces, it will reset it to true accordingly.
                ((VkSurfaceProtectedCapabilitiesKHR*)pNext)->supportsProtected = VK_FALSE;
            }
            pNext = pNext_out_structure.pNext;
        }

        // Pass the call to the driver, possibly unwrapping the ICD surface
        VkPhysicalDeviceSurfaceInfo2KHR info_copy = *pSurfaceInfo;
        info_copy.surface = surface;

        VkResult res =
            icd_term->dispatch.GetPhysicalDeviceSurfaceCapabilities2KHR(phys_dev_term->phys_dev, &info_copy, pSurfaceCapabilities);

        // Because VK_EXT_surface_maintenance1 is an instance extension, applications will use it to query info on drivers which do
        // not support the extension. Thus we need to emulate the driver filling out the structs in that case.
        if (!icd_term->enabled_instance_extensions.khr_surface_maintenance1 &&
            !icd_term->enabled_instance_extensions.ext_surface_maintenance1) {
            emulate_VK_KHR_surface_maintenance1(pSurfaceInfo, pSurfaceCapabilities);
        }

        return res;
    }
    // Emulate the call
    loader_log(icd_term->this_instance, VULKAN_LOADER_INFO_BIT, 0,
               "vkGetPhysicalDeviceSurfaceCapabilities2KHR: Emulating call in ICD \"%s\" using "
               "vkGetPhysicalDeviceSurfaceCapabilitiesKHR",
               icd_term->scanned_icd->lib_name);

    // Write to the VkSurfaceCapabilities2KHR struct

    // If the icd doesn't support VK_KHR_surface, then there are no capabilities
    if (NULL == icd_term->dispatch.GetPhysicalDeviceSurfaceCapabilitiesKHR) {
        if (pSurfaceCapabilities) {
            memset(&pSurfaceCapabilities->surfaceCapabilities, 0, sizeof(VkSurfaceCapabilitiesKHR));
        }
        return VK_SUCCESS;
    }
    VkResult res = icd_term->dispatch.GetPhysicalDeviceSurfaceCapabilitiesKHR(phys_dev_term->phys_dev, surface,
                                                                              &pSurfaceCapabilities->surfaceCapabilities);

    if (!icd_term->enabled_instance_extensions.khr_surface_maintenance1 &&
        !icd_term->enabled_instance_extensions.ext_surface_maintenance1) {
        emulate_VK_KHR_surface_maintenance1(pSurfaceInfo, pSurfaceCapabilities);
    }
    return res;
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceSurfaceFormats2KHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
                                      uint32_t* pSurfaceFormatCount, VkSurfaceFormat2KHR* pSurfaceFormats) {
    const VkLayerInstanceDispatchTable* disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceSurfaceFormats2KHR(unwrapped_phys_dev, pSurfaceInfo, pSurfaceFormatCount, pSurfaceFormats);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceSurfaceFormats2KHR(VkPhysicalDevice physicalDevice,
                                                                              const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
                                                                              uint32_t* pSurfaceFormatCount,
                                                                              VkSurfaceFormat2KHR* pSurfaceFormats) {
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    struct loader_instance* loader_inst = (struct loader_instance*)icd_term->this_instance;

    if (!loader_inst->enabled_extensions.khr_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_surface extension not enabled. vkGetPhysicalDeviceSurfaceFormats2KHR not executed!");
        return VK_SUCCESS;
    }

    VkSurfaceKHR surface = pSurfaceInfo->surface;
    if (VK_NULL_HANDLE != pSurfaceInfo->surface) {
        VkResult res = wsi_unwrap_icd_surface(icd_term, &surface);
        if (res != VK_SUCCESS) {
            return res;
        }
    }

    if (icd_term->dispatch.GetPhysicalDeviceSurfaceFormats2KHR != NULL) {
        // Pass the call to the driver, possibly unwrapping the ICD surface
        VkPhysicalDeviceSurfaceInfo2KHR info_copy = *pSurfaceInfo;
        info_copy.surface = surface;

        return icd_term->dispatch.GetPhysicalDeviceSurfaceFormats2KHR(phys_dev_term->phys_dev, &info_copy, pSurfaceFormatCount,
                                                                      pSurfaceFormats);
    }
    // Emulate the call
    loader_log(icd_term->this_instance, VULKAN_LOADER_INFO_BIT, 0,
               "vkGetPhysicalDeviceSurfaceFormats2KHR: Emulating call in ICD \"%s\" using vkGetPhysicalDeviceSurfaceFormatsKHR",
               icd_term->scanned_icd->lib_name);

    if (pSurfaceInfo->pNext != NULL) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_WARN_BIT, 0,
                   "vkGetPhysicalDeviceSurfaceFormats2KHR: Emulation found unrecognized structure type in pSurfaceInfo->pNext "
                   "- this struct will be ignored");
    }

    // If the icd doesn't support VK_KHR_surface, then there are no formats
    if (NULL == icd_term->dispatch.GetPhysicalDeviceSurfaceFormatsKHR) {
        if (pSurfaceFormatCount) {
            *pSurfaceFormatCount = 0;
        }
        return VK_SUCCESS;
    }

    if (*pSurfaceFormatCount == 0 || pSurfaceFormats == NULL) {
        // Write to pSurfaceFormatCount
        return icd_term->dispatch.GetPhysicalDeviceSurfaceFormatsKHR(phys_dev_term->phys_dev, surface, pSurfaceFormatCount, NULL);
    }
    // Allocate a temporary array for the output of the old function
    VkResult res = VK_SUCCESS;
    uint32_t allocated_count = *pSurfaceFormatCount;
    VkSurfaceFormatKHR* formats = loader_instance_heap_calloc(icd_term->this_instance, allocated_count * sizeof(VkSurfaceFormatKHR),
                                                              VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
    if (formats == NULL) {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    res = icd_term->dispatch.GetPhysicalDeviceSurfaceFormatsKHR(phys_dev_term->phys_dev, surface, pSurfaceFormatCount, formats);
    // The driver reports the written count back in pSurfaceFormatCount; never copy past the array we sized.
    for (uint32_t i = 0; i < *pSurfaceFormatCount && i < allocated_count; ++i) {
        pSurfaceFormats[i].surfaceFormat = formats[i];
        if (pSurfaceFormats[i].pNext != NULL) {
            loader_log(icd_term->this_instance, VULKAN_LOADER_WARN_BIT, 0,
                       "vkGetPhysicalDeviceSurfaceFormats2KHR: Emulation found unrecognized structure type in "
                       "pSurfaceFormats[%d].pNext - this struct will be ignored",
                       i);
        }
    }
out:
    loader_instance_heap_free(icd_term->this_instance, formats);
    return res;
}

bool wsi_swapchain_instance_gpa(struct loader_instance* loader_inst, const char* name, void** addr) {
    *addr = NULL;

    // Functions for the VK_KHR_surface extension:
    if (!strcmp("vkDestroySurfaceKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_surface ? (void*)vkDestroySurfaceKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceSurfaceSupportKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_surface ? (void*)vkGetPhysicalDeviceSurfaceSupportKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceSurfaceCapabilitiesKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_surface ? (void*)vkGetPhysicalDeviceSurfaceCapabilitiesKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceSurfaceFormatsKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_surface ? (void*)vkGetPhysicalDeviceSurfaceFormatsKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceSurfacePresentModesKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_surface ? (void*)vkGetPhysicalDeviceSurfacePresentModesKHR : NULL;
        return true;
    }

    if (!strcmp("vkGetDeviceGroupPresentCapabilitiesKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_surface ? (void*)vkGetDeviceGroupPresentCapabilitiesKHR : NULL;
        return true;
    }

    if (!strcmp("vkGetDeviceGroupSurfacePresentModesKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_surface ? (void*)vkGetDeviceGroupSurfacePresentModesKHR : NULL;
        return true;
    }

    if (!strcmp("vkGetPhysicalDevicePresentRectanglesKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_surface ? (void*)vkGetPhysicalDevicePresentRectanglesKHR : NULL;
        return true;
    }

    // Functions for VK_KHR_get_surface_capabilities2 extension:
    if (!strcmp("vkGetPhysicalDeviceSurfaceCapabilities2KHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_surface ? (void*)vkGetPhysicalDeviceSurfaceCapabilities2KHR : NULL;
        return true;
    }

    if (!strcmp("vkGetPhysicalDeviceSurfaceFormats2KHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_surface ? (void*)vkGetPhysicalDeviceSurfaceFormats2KHR : NULL;
        return true;
    }

    // Functions for the VK_KHR_swapchain extension:

    // Note: This is a device extension, and its functions are statically
    // exported from the loader.  Per Khronos decisions, the loader's GIPA
    // function will return the trampoline function for such device-extension
    // functions, regardless of whether the extension has been enabled.
    if (!strcmp("vkCreateSwapchainKHR", name)) {
        *addr = (void*)vkCreateSwapchainKHR;
        return true;
    }
    if (!strcmp("vkDestroySwapchainKHR", name)) {
        *addr = (void*)vkDestroySwapchainKHR;
        return true;
    }
    if (!strcmp("vkGetSwapchainImagesKHR", name)) {
        *addr = (void*)vkGetSwapchainImagesKHR;
        return true;
    }
    if (!strcmp("vkAcquireNextImageKHR", name)) {
        *addr = (void*)vkAcquireNextImageKHR;
        return true;
    }
    if (!strcmp("vkQueuePresentKHR", name)) {
        *addr = (void*)vkQueuePresentKHR;
        return true;
    }
    if (!strcmp("vkAcquireNextImage2KHR", name)) {
        *addr = (void*)vkAcquireNextImage2KHR;
        return true;
    }

    // Functions for KHR_display_swapchain extension:
    if (!strcmp("vkCreateSharedSwapchainsKHR", name)) {
        *addr = (void*)vkCreateSharedSwapchainsKHR;
        return true;
    }

    // Functions for KHR_get_display_properties2
    if (!strcmp("vkGetPhysicalDeviceDisplayProperties2KHR", name)) {
        *addr =
            loader_inst->enabled_extensions.khr_get_display_properties2 ? (void*)vkGetPhysicalDeviceDisplayProperties2KHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceDisplayPlaneProperties2KHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_get_display_properties2 ? (void*)vkGetPhysicalDeviceDisplayPlaneProperties2KHR
                                                                            : NULL;
        return true;
    }
    if (!strcmp("vkGetDisplayModeProperties2KHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_get_display_properties2 ? (void*)vkGetDisplayModeProperties2KHR : NULL;
        return true;
    }
    if (!strcmp("vkGetDisplayPlaneCapabilities2KHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_get_display_properties2 ? (void*)vkGetDisplayPlaneCapabilities2KHR : NULL;
        return true;
    }

    // Need to query for functions from the generated portion of the wsi code
    return generated_wsi_swapchain_instance_gpa(loader_inst, name, addr);
}
