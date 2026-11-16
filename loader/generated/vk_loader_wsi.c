// *** THIS FILE IS GENERATED - DO NOT EDIT ***
// See loader_wsi_generator.py for modifications

/*
 * Copyright (c) 2026 The Khronos Group Inc.
 * Copyright (c) 2026 Valve Corporation
 * Copyright (c) 2026 LunarG, Inc.
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

#include "vk_loader_wsi.h"

#include <vulkan/vk_icd.h>

#include "loader.h"
#include "log.h"
#include "vk_loader_platform.h"
#include "wsi.h"

// This function unwraps the application-provided surface handle into the ICD-specific surface handle
// corresponding to the specified physical device. If the ICD-specific surface handle does not exist
// yet, then this function also creates of the ICD-specific surface object. This enables lazy creation
// of ICD-specific surface objects and therefore avoids creating surfaces for ICDs that may not be able
// to or should not create the specific type of surface (one example is the VK_KHR_display extension
// where the VkDisplayModeKHR handles the surfaces are created from are physical-device-specific
// non-dispatchable handles and therefore they should not be passed down to a foreign ICD).
VkResult wsi_unwrap_icd_surface(struct loader_icd_term* icd_term, VkSurfaceKHR* surface) {
    // NOLINTNEXTLINE(performance-no-int-to-ptr) - VkSurfaceKHR handle decode requires round-tripping through uintptr_t
    VkIcdSurface* icd_surface = (VkIcdSurface*)(uintptr_t)(*surface);

    // The surface creation terminators grow each driver's surface_list under loader_lock, which can reallocate the
    // array this function reads, and terminator_DestroySurfaceKHR clears slots in it under the same lock. The surface
    // query commands that land here don't require external synchronization of the surface, so take the lock before
    // touching the list. loader_lock is recursive, so callers already holding it are unaffected.
    loader_platform_thread_lock_mutex(&loader_lock);

    if (NULL == icd_term->surface_list.list ||
        icd_term->surface_list.capacity <= icd_surface->surface_index * sizeof(VkSurfaceKHR)) {
        // This surface handle is not one that was created by the loader, therefore we simply return
        // the input surface handle unmodified. This matches legacy behavior.
        loader_platform_thread_unlock_mutex(&loader_lock);
        return VK_SUCCESS;
    }

    VkResult result = VK_SUCCESS;
    if (icd_term->scanned_icd->interface_version >= ICD_VER_SUPPORTS_ICD_SURFACE_KHR) {
        VkAllocationCallbacks* pAllocator = icd_surface->callbacks_valid ? &icd_surface->callbacks : NULL;
        if (VK_NULL_HANDLE == icd_term->surface_list.list[icd_surface->surface_index]) {
            // If the surface does not exist yet for the target ICD, then create it lazily
            switch (icd_surface->base.platform) {
                case VK_ICD_WSI_PLATFORM_DISPLAY:
                    if (NULL != icd_term->dispatch.CreateDisplayPlaneSurfaceKHR &&
                        icd_term->enabled_instance_extensions.khr_display) {
                        result = icd_term->dispatch.CreateDisplayPlaneSurfaceKHR(
                            icd_term->instance, (const VkDisplaySurfaceCreateInfoKHR*)icd_surface->create_info, pAllocator,
                            &icd_term->surface_list.list[icd_surface->surface_index]);
                    } else {
                        result = VK_ERROR_EXTENSION_NOT_PRESENT;
                    }
                    break;

#if defined(VK_USE_PLATFORM_XLIB_KHR)
                case VK_ICD_WSI_PLATFORM_XLIB:
                    if (NULL != icd_term->dispatch.CreateXlibSurfaceKHR && icd_term->enabled_instance_extensions.khr_xlib_surface) {
                        result = icd_term->dispatch.CreateXlibSurfaceKHR(
                            icd_term->instance, (const VkXlibSurfaceCreateInfoKHR*)icd_surface->create_info, pAllocator,
                            &icd_term->surface_list.list[icd_surface->surface_index]);
                    } else {
                        result = VK_ERROR_EXTENSION_NOT_PRESENT;
                    }
                    break;
#endif  // VK_USE_PLATFORM_XLIB_KHR

#if defined(VK_USE_PLATFORM_XCB_KHR)
                case VK_ICD_WSI_PLATFORM_XCB:
                    if (NULL != icd_term->dispatch.CreateXcbSurfaceKHR && icd_term->enabled_instance_extensions.khr_xcb_surface) {
                        result = icd_term->dispatch.CreateXcbSurfaceKHR(
                            icd_term->instance, (const VkXcbSurfaceCreateInfoKHR*)icd_surface->create_info, pAllocator,
                            &icd_term->surface_list.list[icd_surface->surface_index]);
                    } else {
                        result = VK_ERROR_EXTENSION_NOT_PRESENT;
                    }
                    break;
#endif  // VK_USE_PLATFORM_XCB_KHR

#if defined(VK_USE_PLATFORM_WAYLAND_KHR)
                case VK_ICD_WSI_PLATFORM_WAYLAND:
                    if (NULL != icd_term->dispatch.CreateWaylandSurfaceKHR &&
                        icd_term->enabled_instance_extensions.khr_wayland_surface) {
                        result = icd_term->dispatch.CreateWaylandSurfaceKHR(
                            icd_term->instance, (const VkWaylandSurfaceCreateInfoKHR*)icd_surface->create_info, pAllocator,
                            &icd_term->surface_list.list[icd_surface->surface_index]);
                    } else {
                        result = VK_ERROR_EXTENSION_NOT_PRESENT;
                    }
                    break;
#endif  // VK_USE_PLATFORM_WAYLAND_KHR

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
                case VK_ICD_WSI_PLATFORM_ANDROID:
                    if (NULL != icd_term->dispatch.CreateAndroidSurfaceKHR &&
                        icd_term->enabled_instance_extensions.khr_android_surface) {
                        result = icd_term->dispatch.CreateAndroidSurfaceKHR(
                            icd_term->instance, (const VkAndroidSurfaceCreateInfoKHR*)icd_surface->create_info, pAllocator,
                            &icd_term->surface_list.list[icd_surface->surface_index]);
                    } else {
                        result = VK_ERROR_EXTENSION_NOT_PRESENT;
                    }
                    break;
#endif  // VK_USE_PLATFORM_ANDROID_KHR

#if defined(VK_USE_PLATFORM_WIN32_KHR)
                case VK_ICD_WSI_PLATFORM_WIN32:
                    if (NULL != icd_term->dispatch.CreateWin32SurfaceKHR &&
                        icd_term->enabled_instance_extensions.khr_win32_surface) {
                        result = icd_term->dispatch.CreateWin32SurfaceKHR(
                            icd_term->instance, (const VkWin32SurfaceCreateInfoKHR*)icd_surface->create_info, pAllocator,
                            &icd_term->surface_list.list[icd_surface->surface_index]);
                    } else {
                        result = VK_ERROR_EXTENSION_NOT_PRESENT;
                    }
                    break;
#endif  // VK_USE_PLATFORM_WIN32_KHR

#if defined(VK_USE_PLATFORM_GGP)
                case VK_ICD_WSI_PLATFORM_GGP:
                    if (NULL != icd_term->dispatch.CreateStreamDescriptorSurfaceGGP &&
                        icd_term->enabled_instance_extensions.ggp_stream_descriptor_surface) {
                        result = icd_term->dispatch.CreateStreamDescriptorSurfaceGGP(
                            icd_term->instance, (const VkStreamDescriptorSurfaceCreateInfoGGP*)icd_surface->create_info, pAllocator,
                            &icd_term->surface_list.list[icd_surface->surface_index]);
                    } else {
                        result = VK_ERROR_EXTENSION_NOT_PRESENT;
                    }
                    break;
#endif  // VK_USE_PLATFORM_GGP

#if defined(VK_USE_PLATFORM_VI_NN)
                case VK_ICD_WSI_PLATFORM_VI:
                    if (NULL != icd_term->dispatch.CreateViSurfaceNN && icd_term->enabled_instance_extensions.nn_vi_surface) {
                        result = icd_term->dispatch.CreateViSurfaceNN(
                            icd_term->instance, (const VkViSurfaceCreateInfoNN*)icd_surface->create_info, pAllocator,
                            &icd_term->surface_list.list[icd_surface->surface_index]);
                    } else {
                        result = VK_ERROR_EXTENSION_NOT_PRESENT;
                    }
                    break;
#endif  // VK_USE_PLATFORM_VI_NN

#if defined(VK_USE_PLATFORM_IOS_MVK)
                case VK_ICD_WSI_PLATFORM_IOS:
                    if (NULL != icd_term->dispatch.CreateIOSSurfaceMVK && icd_term->enabled_instance_extensions.mvk_ios_surface) {
                        result = icd_term->dispatch.CreateIOSSurfaceMVK(
                            icd_term->instance, (const VkIOSSurfaceCreateInfoMVK*)icd_surface->create_info, pAllocator,
                            &icd_term->surface_list.list[icd_surface->surface_index]);
                    } else {
                        result = VK_ERROR_EXTENSION_NOT_PRESENT;
                    }
                    break;
#endif  // VK_USE_PLATFORM_IOS_MVK

#if defined(VK_USE_PLATFORM_MACOS_MVK)
                case VK_ICD_WSI_PLATFORM_MACOS:
                    if (NULL != icd_term->dispatch.CreateMacOSSurfaceMVK &&
                        icd_term->enabled_instance_extensions.mvk_macos_surface) {
                        result = icd_term->dispatch.CreateMacOSSurfaceMVK(
                            icd_term->instance, (const VkMacOSSurfaceCreateInfoMVK*)icd_surface->create_info, pAllocator,
                            &icd_term->surface_list.list[icd_surface->surface_index]);
                    } else {
                        result = VK_ERROR_EXTENSION_NOT_PRESENT;
                    }
                    break;
#endif  // VK_USE_PLATFORM_MACOS_MVK

#if defined(VK_USE_PLATFORM_FUCHSIA)
                case VK_ICD_WSI_PLATFORM_FUCHSIA:
                    if (NULL != icd_term->dispatch.CreateImagePipeSurfaceFUCHSIA &&
                        icd_term->enabled_instance_extensions.fuchsia_imagepipe_surface) {
                        result = icd_term->dispatch.CreateImagePipeSurfaceFUCHSIA(
                            icd_term->instance, (const VkImagePipeSurfaceCreateInfoFUCHSIA*)icd_surface->create_info, pAllocator,
                            &icd_term->surface_list.list[icd_surface->surface_index]);
                    } else {
                        result = VK_ERROR_EXTENSION_NOT_PRESENT;
                    }
                    break;
#endif  // VK_USE_PLATFORM_FUCHSIA

#if defined(VK_USE_PLATFORM_METAL_EXT)
                case VK_ICD_WSI_PLATFORM_METAL:
                    if (NULL != icd_term->dispatch.CreateMetalSurfaceEXT &&
                        icd_term->enabled_instance_extensions.ext_metal_surface) {
                        result = icd_term->dispatch.CreateMetalSurfaceEXT(
                            icd_term->instance, (const VkMetalSurfaceCreateInfoEXT*)icd_surface->create_info, pAllocator,
                            &icd_term->surface_list.list[icd_surface->surface_index]);
                    } else {
                        result = VK_ERROR_EXTENSION_NOT_PRESENT;
                    }
                    break;
#endif  // VK_USE_PLATFORM_METAL_EXT

                case VK_ICD_WSI_PLATFORM_HEADLESS:
                    if (NULL != icd_term->dispatch.CreateHeadlessSurfaceEXT &&
                        icd_term->enabled_instance_extensions.ext_headless_surface) {
                        result = icd_term->dispatch.CreateHeadlessSurfaceEXT(
                            icd_term->instance, (const VkHeadlessSurfaceCreateInfoEXT*)icd_surface->create_info, pAllocator,
                            &icd_term->surface_list.list[icd_surface->surface_index]);
                    } else {
                        result = VK_ERROR_EXTENSION_NOT_PRESENT;
                    }
                    break;

#if defined(VK_USE_PLATFORM_DIRECTFB_EXT)
                case VK_ICD_WSI_PLATFORM_DIRECTFB:
                    if (NULL != icd_term->dispatch.CreateDirectFBSurfaceEXT &&
                        icd_term->enabled_instance_extensions.ext_directfb_surface) {
                        result = icd_term->dispatch.CreateDirectFBSurfaceEXT(
                            icd_term->instance, (const VkDirectFBSurfaceCreateInfoEXT*)icd_surface->create_info, pAllocator,
                            &icd_term->surface_list.list[icd_surface->surface_index]);
                    } else {
                        result = VK_ERROR_EXTENSION_NOT_PRESENT;
                    }
                    break;
#endif  // VK_USE_PLATFORM_DIRECTFB_EXT

#if defined(VK_USE_PLATFORM_SCREEN_QNX)
                case VK_ICD_WSI_PLATFORM_SCREEN:
                    if (NULL != icd_term->dispatch.CreateScreenSurfaceQNX &&
                        icd_term->enabled_instance_extensions.qnx_screen_surface) {
                        result = icd_term->dispatch.CreateScreenSurfaceQNX(
                            icd_term->instance, (const VkScreenSurfaceCreateInfoQNX*)icd_surface->create_info, pAllocator,
                            &icd_term->surface_list.list[icd_surface->surface_index]);
                    } else {
                        result = VK_ERROR_EXTENSION_NOT_PRESENT;
                    }
                    break;
#endif  // VK_USE_PLATFORM_SCREEN_QNX

#if defined(VK_USE_PLATFORM_UBM_SEC)
                case VK_ICD_WSI_PLATFORM_UBM:
                    if (NULL != icd_term->dispatch.CreateUbmSurfaceSEC && icd_term->enabled_instance_extensions.sec_ubm_surface) {
                        result = icd_term->dispatch.CreateUbmSurfaceSEC(
                            icd_term->instance, (const VkUbmSurfaceCreateInfoSEC*)icd_surface->create_info, pAllocator,
                            &icd_term->surface_list.list[icd_surface->surface_index]);
                    } else {
                        result = VK_ERROR_EXTENSION_NOT_PRESENT;
                    }
                    break;
#endif  // VK_USE_PLATFORM_UBM_SEC

                default:
                    // Not supported
                    result = VK_ERROR_EXTENSION_NOT_PRESENT;
                    break;
            }
        }

        *surface = icd_term->surface_list.list[icd_surface->surface_index];
    }

    loader_platform_thread_unlock_mutex(&loader_lock);
    return result;
}

// ---- VK_KHR_display extension commands

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceDisplayPropertiesKHR(VkPhysicalDevice physicalDevice,
                                                                                     uint32_t* pPropertyCount,
                                                                                     VkDisplayPropertiesKHR* pProperties) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, "vkGetPhysicalDeviceDisplayPropertiesKHR");
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceDisplayPropertiesKHR(unwrapped_phys_dev, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceDisplayPropertiesKHR(VkPhysicalDevice physicalDevice,
                                                                                uint32_t* pPropertyCount,
                                                                                VkDisplayPropertiesKHR* pProperties) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    struct loader_instance* loader_inst = (struct loader_instance*)icd_term->this_instance;
    if (!loader_inst->enabled_extensions.khr_display) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_display extension not enabled. vkGetPhysicalDeviceDisplayPropertiesKHR not executed!");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceDisplayPropertiesKHR) {
        loader_log(loader_inst, VULKAN_LOADER_WARN_BIT, 0,
                   "ICD for selected physical device does not export vkGetPhysicalDeviceDisplayPropertiesKHR!");
        // return 0 for property count as this driver doesn't support WSI functionality
        if (pPropertyCount) {
            *pPropertyCount = 0;
        }
        return VK_SUCCESS;
    }

    return icd_term->dispatch.GetPhysicalDeviceDisplayPropertiesKHR(phys_dev_term->phys_dev, pPropertyCount, pProperties);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceDisplayPlanePropertiesKHR(
    VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkDisplayPlanePropertiesKHR* pProperties) {
    VkPhysicalDevice unwrapped_phys_dev =
        loader_unwrap_physical_device(physicalDevice, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR");
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceDisplayPlanePropertiesKHR(unwrapped_phys_dev, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceDisplayPlanePropertiesKHR(VkPhysicalDevice physicalDevice,
                                                                                     uint32_t* pPropertyCount,
                                                                                     VkDisplayPlanePropertiesKHR* pProperties) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    struct loader_instance* loader_inst = (struct loader_instance*)icd_term->this_instance;
    if (!loader_inst->enabled_extensions.khr_display) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_display extension not enabled. vkGetPhysicalDeviceDisplayPlanePropertiesKHR not executed!");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceDisplayPlanePropertiesKHR) {
        loader_log(loader_inst, VULKAN_LOADER_WARN_BIT, 0,
                   "ICD for selected physical device does not export vkGetPhysicalDeviceDisplayPlanePropertiesKHR!");
        // return 0 for property count as this driver doesn't support WSI functionality
        if (pPropertyCount) {
            *pPropertyCount = 0;
        }
        return VK_SUCCESS;
    }

    return icd_term->dispatch.GetPhysicalDeviceDisplayPlanePropertiesKHR(phys_dev_term->phys_dev, pPropertyCount, pProperties);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetDisplayPlaneSupportedDisplaysKHR(VkPhysicalDevice physicalDevice,
                                                                                   uint32_t planeIndex, uint32_t* pDisplayCount,
                                                                                   VkDisplayKHR* pDisplays) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, "vkGetDisplayPlaneSupportedDisplaysKHR");
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetDisplayPlaneSupportedDisplaysKHR(unwrapped_phys_dev, planeIndex, pDisplayCount, pDisplays);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetDisplayPlaneSupportedDisplaysKHR(VkPhysicalDevice physicalDevice, uint32_t planeIndex,
                                                                              uint32_t* pDisplayCount, VkDisplayKHR* pDisplays) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    struct loader_instance* loader_inst = (struct loader_instance*)icd_term->this_instance;
    if (!loader_inst->enabled_extensions.khr_display) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_display extension not enabled. vkGetDisplayPlaneSupportedDisplaysKHR not executed!");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    if (NULL == icd_term->dispatch.GetDisplayPlaneSupportedDisplaysKHR) {
        loader_log(loader_inst, VULKAN_LOADER_WARN_BIT, 0,
                   "ICD for selected physical device does not export vkGetDisplayPlaneSupportedDisplaysKHR!");
        // return 0 for property count as this driver doesn't support WSI functionality
        if (pDisplayCount) {
            *pDisplayCount = 0;
        }
        return VK_SUCCESS;
    }

    return icd_term->dispatch.GetDisplayPlaneSupportedDisplaysKHR(phys_dev_term->phys_dev, planeIndex, pDisplayCount, pDisplays);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetDisplayModePropertiesKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display,
                                                                           uint32_t* pPropertyCount,
                                                                           VkDisplayModePropertiesKHR* pProperties) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, "vkGetDisplayModePropertiesKHR");
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetDisplayModePropertiesKHR(unwrapped_phys_dev, display, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetDisplayModePropertiesKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display,
                                                                      uint32_t* pPropertyCount,
                                                                      VkDisplayModePropertiesKHR* pProperties) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    struct loader_instance* loader_inst = (struct loader_instance*)icd_term->this_instance;
    if (!loader_inst->enabled_extensions.khr_display) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_display extension not enabled. vkGetDisplayModePropertiesKHR not executed!");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    if (NULL == icd_term->dispatch.GetDisplayModePropertiesKHR) {
        loader_log(loader_inst, VULKAN_LOADER_WARN_BIT, 0,
                   "ICD for selected physical device does not export vkGetDisplayModePropertiesKHR!");
        // return 0 for property count as this driver doesn't support WSI functionality
        if (pPropertyCount) {
            *pPropertyCount = 0;
        }
        return VK_SUCCESS;
    }

    return icd_term->dispatch.GetDisplayModePropertiesKHR(phys_dev_term->phys_dev, display, pPropertyCount, pProperties);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateDisplayModeKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display,
                                                                    const VkDisplayModeCreateInfoKHR* pCreateInfo,
                                                                    const VkAllocationCallbacks* pAllocator,
                                                                    VkDisplayModeKHR* pMode) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, "vkCreateDisplayModeKHR");
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->CreateDisplayModeKHR(unwrapped_phys_dev, display, pCreateInfo, pAllocator, pMode);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateDisplayModeKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display,
                                                               const VkDisplayModeCreateInfoKHR* pCreateInfo,
                                                               const VkAllocationCallbacks* pAllocator, VkDisplayModeKHR* pMode) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    struct loader_instance* loader_inst = (struct loader_instance*)icd_term->this_instance;
    if (!loader_inst->enabled_extensions.khr_display) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_display extension not enabled. vkCreateDisplayModeKHR not executed!");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    if (NULL == icd_term->dispatch.CreateDisplayModeKHR) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device does not export vkCreateDisplayModeKHR!");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    return icd_term->dispatch.CreateDisplayModeKHR(phys_dev_term->phys_dev, display, pCreateInfo, pAllocator, pMode);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetDisplayPlaneCapabilitiesKHR(VkPhysicalDevice physicalDevice,
                                                                              VkDisplayModeKHR mode, uint32_t planeIndex,
                                                                              VkDisplayPlaneCapabilitiesKHR* pCapabilities) {
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, "vkGetDisplayPlaneCapabilitiesKHR");
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetDisplayPlaneCapabilitiesKHR(unwrapped_phys_dev, mode, planeIndex, pCapabilities);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetDisplayPlaneCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkDisplayModeKHR mode,
                                                                         uint32_t planeIndex,
                                                                         VkDisplayPlaneCapabilitiesKHR* pCapabilities) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    struct loader_instance* loader_inst = (struct loader_instance*)icd_term->this_instance;
    if (!loader_inst->enabled_extensions.khr_display) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_display extension not enabled. vkGetDisplayPlaneCapabilitiesKHR not executed!");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    if (NULL == icd_term->dispatch.GetDisplayPlaneCapabilitiesKHR) {
        loader_log(loader_inst, VULKAN_LOADER_WARN_BIT, 0,
                   "ICD for selected physical device does not export vkGetDisplayPlaneCapabilitiesKHR!");
        if (pCapabilities) {
            memset(pCapabilities, 0, sizeof(VkDisplayPlaneCapabilitiesKHR));
        }
        return VK_SUCCESS;
    }

    return icd_term->dispatch.GetDisplayPlaneCapabilitiesKHR(phys_dev_term->phys_dev, mode, planeIndex, pCapabilities);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateDisplayPlaneSurfaceKHR(VkInstance instance,
                                                                            const VkDisplaySurfaceCreateInfoKHR* pCreateInfo,
                                                                            const VkAllocationCallbacks* pAllocator,
                                                                            VkSurfaceKHR* pSurface) {
    struct loader_instance* loader_inst = loader_validate_and_get_instance_dispatch(instance, "vkCreateDisplayPlaneSurfaceKHR");
    return loader_inst->disp->layer_inst_disp.CreateDisplayPlaneSurfaceKHR(loader_inst->instance, pCreateInfo, pAllocator,
                                                                           pSurface);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateDisplayPlaneSurfaceKHR(VkInstance instance,
                                                                       const VkDisplaySurfaceCreateInfoKHR* pCreateInfo,
                                                                       const VkAllocationCallbacks* pAllocator,
                                                                       VkSurfaceKHR* pSurface) {
    VkResult result = VK_SUCCESS;
    VkIcdSurface* icd_surface = NULL;
    loader_platform_thread_lock_mutex(&loader_lock);

    // Initialize pSurface to NULL just to be safe.
    *pSurface = VK_NULL_HANDLE;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance* loader_inst = loader_get_instance(instance);
    if (!loader_inst->enabled_extensions.khr_display) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_display extension not enabled. vkCreateDisplayPlaneSurfaceKHR not executed!");
        result = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    result = allocate_icd_surface_struct(loader_inst, sizeof(icd_surface->display_surf.base), sizeof(icd_surface->display_surf),
                                         pAllocator, &icd_surface);
    if (VK_SUCCESS != result) {
        goto out;
    }

    icd_surface->display_surf.base.platform = VK_ICD_WSI_PLATFORM_DISPLAY;
    icd_surface->display_surf.displayMode = pCreateInfo->displayMode;
    icd_surface->display_surf.planeIndex = pCreateInfo->planeIndex;
    icd_surface->display_surf.planeStackIndex = pCreateInfo->planeStackIndex;
    icd_surface->display_surf.transform = pCreateInfo->transform;
    icd_surface->display_surf.globalAlpha = pCreateInfo->globalAlpha;
    icd_surface->display_surf.alphaMode = pCreateInfo->alphaMode;
    icd_surface->display_surf.imageExtent = pCreateInfo->imageExtent;

    const struct loader_struct_type_info ci_types[] = {
        {VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR, sizeof(VkDisplaySurfaceCreateInfoKHR)},
        {VK_STRUCTURE_TYPE_DISPLAY_SURFACE_STEREO_CREATE_INFO_NV, sizeof(VkDisplaySurfaceStereoCreateInfoNV)},
    };
    result = copy_surface_create_info(loader_inst, icd_surface, pCreateInfo, sizeof(ci_types) / sizeof(ci_types[0]), ci_types,
                                      "VkDisplaySurfaceCreateInfoKHR", pAllocator);
    if (VK_SUCCESS != result) {
        goto out;
    }

    *pSurface = wrap_surface_handle(icd_surface);

out:
    cleanup_surface_creation(loader_inst, result, icd_surface, pAllocator);
    loader_platform_thread_unlock_mutex(&loader_lock);

    return result;
}

#if defined(VK_USE_PLATFORM_XLIB_KHR)

// ---- VK_KHR_xlib_surface extension commands
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateXlibSurfaceKHR(VkInstance instance,
                                                                    const VkXlibSurfaceCreateInfoKHR* pCreateInfo,
                                                                    const VkAllocationCallbacks* pAllocator,
                                                                    VkSurfaceKHR* pSurface) {
    struct loader_instance* loader_inst = loader_validate_and_get_instance_dispatch(instance, "vkCreateXlibSurfaceKHR");
    return loader_inst->disp->layer_inst_disp.CreateXlibSurfaceKHR(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateXlibSurfaceKHR(VkInstance instance, const VkXlibSurfaceCreateInfoKHR* pCreateInfo,
                                                               const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    VkResult result = VK_SUCCESS;
    VkIcdSurface* icd_surface = NULL;
    loader_platform_thread_lock_mutex(&loader_lock);

    // Initialize pSurface to NULL just to be safe.
    *pSurface = VK_NULL_HANDLE;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance* loader_inst = loader_get_instance(instance);
    if (!loader_inst->enabled_extensions.khr_xlib_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_xlib_surface extension not enabled. vkCreateXlibSurfaceKHR not executed!");
        result = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    result = allocate_icd_surface_struct(loader_inst, sizeof(icd_surface->xlib_surf.base), sizeof(icd_surface->xlib_surf),
                                         pAllocator, &icd_surface);
    if (VK_SUCCESS != result) {
        goto out;
    }

    icd_surface->xlib_surf.base.platform = VK_ICD_WSI_PLATFORM_XLIB;
    icd_surface->xlib_surf.dpy = pCreateInfo->dpy;
    icd_surface->xlib_surf.window = pCreateInfo->window;

    const struct loader_struct_type_info ci_types[] = {
        {VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR, sizeof(VkXlibSurfaceCreateInfoKHR)},
    };
    result = copy_surface_create_info(loader_inst, icd_surface, pCreateInfo, sizeof(ci_types) / sizeof(ci_types[0]), ci_types,
                                      "VkXlibSurfaceCreateInfoKHR", pAllocator);
    if (VK_SUCCESS != result) {
        goto out;
    }

    *pSurface = wrap_surface_handle(icd_surface);

out:
    cleanup_surface_creation(loader_inst, result, icd_surface, pAllocator);
    loader_platform_thread_unlock_mutex(&loader_lock);

    return result;
}

LOADER_EXPORT VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceXlibPresentationSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                           uint32_t queueFamilyIndex, Display* dpy,
                                                                                           VisualID visualID) {
    VkPhysicalDevice unwrapped_phys_dev =
        loader_unwrap_physical_device(physicalDevice, "vkGetPhysicalDeviceXlibPresentationSupportKHR");
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceXlibPresentationSupportKHR(unwrapped_phys_dev, queueFamilyIndex, dpy, visualID);
}

VKAPI_ATTR VkBool32 VKAPI_CALL terminator_GetPhysicalDeviceXlibPresentationSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                      uint32_t queueFamilyIndex, Display* dpy,
                                                                                      VisualID visualID) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    struct loader_instance* loader_inst = (struct loader_instance*)icd_term->this_instance;
    if (!loader_inst->enabled_extensions.khr_xlib_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_xlib_surface extension not enabled. vkGetPhysicalDeviceXlibPresentationSupportKHR not executed!");
        return VK_FALSE;
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceXlibPresentationSupportKHR) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device does not export vkGetPhysicalDeviceXlibPresentationSupportKHR!");
        return VK_FALSE;
    }

    return icd_term->dispatch.GetPhysicalDeviceXlibPresentationSupportKHR(phys_dev_term->phys_dev, queueFamilyIndex, dpy, visualID);
}

#endif  // VK_USE_PLATFORM_XLIB_KHR
#if defined(VK_USE_PLATFORM_XCB_KHR)

// ---- VK_KHR_xcb_surface extension commands
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateXcbSurfaceKHR(VkInstance instance,
                                                                   const VkXcbSurfaceCreateInfoKHR* pCreateInfo,
                                                                   const VkAllocationCallbacks* pAllocator,
                                                                   VkSurfaceKHR* pSurface) {
    struct loader_instance* loader_inst = loader_validate_and_get_instance_dispatch(instance, "vkCreateXcbSurfaceKHR");
    return loader_inst->disp->layer_inst_disp.CreateXcbSurfaceKHR(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateXcbSurfaceKHR(VkInstance instance, const VkXcbSurfaceCreateInfoKHR* pCreateInfo,
                                                              const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    VkResult result = VK_SUCCESS;
    VkIcdSurface* icd_surface = NULL;
    loader_platform_thread_lock_mutex(&loader_lock);

    // Initialize pSurface to NULL just to be safe.
    *pSurface = VK_NULL_HANDLE;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance* loader_inst = loader_get_instance(instance);
    if (!loader_inst->enabled_extensions.khr_xcb_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_xcb_surface extension not enabled. vkCreateXcbSurfaceKHR not executed!");
        result = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    result = allocate_icd_surface_struct(loader_inst, sizeof(icd_surface->xcb_surf.base), sizeof(icd_surface->xcb_surf), pAllocator,
                                         &icd_surface);
    if (VK_SUCCESS != result) {
        goto out;
    }

    icd_surface->xcb_surf.base.platform = VK_ICD_WSI_PLATFORM_XCB;
    icd_surface->xcb_surf.connection = pCreateInfo->connection;
    icd_surface->xcb_surf.window = pCreateInfo->window;

    const struct loader_struct_type_info ci_types[] = {
        {VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR, sizeof(VkXcbSurfaceCreateInfoKHR)},
    };
    result = copy_surface_create_info(loader_inst, icd_surface, pCreateInfo, sizeof(ci_types) / sizeof(ci_types[0]), ci_types,
                                      "VkXcbSurfaceCreateInfoKHR", pAllocator);
    if (VK_SUCCESS != result) {
        goto out;
    }

    *pSurface = wrap_surface_handle(icd_surface);

out:
    cleanup_surface_creation(loader_inst, result, icd_surface, pAllocator);
    loader_platform_thread_unlock_mutex(&loader_lock);

    return result;
}

LOADER_EXPORT VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceXcbPresentationSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                          uint32_t queueFamilyIndex,
                                                                                          xcb_connection_t* connection,
                                                                                          xcb_visualid_t visual_id) {
    VkPhysicalDevice unwrapped_phys_dev =
        loader_unwrap_physical_device(physicalDevice, "vkGetPhysicalDeviceXcbPresentationSupportKHR");
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceXcbPresentationSupportKHR(unwrapped_phys_dev, queueFamilyIndex, connection, visual_id);
}

VKAPI_ATTR VkBool32 VKAPI_CALL terminator_GetPhysicalDeviceXcbPresentationSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                     uint32_t queueFamilyIndex,
                                                                                     xcb_connection_t* connection,
                                                                                     xcb_visualid_t visual_id) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    struct loader_instance* loader_inst = (struct loader_instance*)icd_term->this_instance;
    if (!loader_inst->enabled_extensions.khr_xcb_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_xcb_surface extension not enabled. vkGetPhysicalDeviceXcbPresentationSupportKHR not executed!");
        return VK_FALSE;
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceXcbPresentationSupportKHR) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device does not export vkGetPhysicalDeviceXcbPresentationSupportKHR!");
        return VK_FALSE;
    }

    return icd_term->dispatch.GetPhysicalDeviceXcbPresentationSupportKHR(phys_dev_term->phys_dev, queueFamilyIndex, connection,
                                                                         visual_id);
}

#endif  // VK_USE_PLATFORM_XCB_KHR
#if defined(VK_USE_PLATFORM_WAYLAND_KHR)

// ---- VK_KHR_wayland_surface extension commands
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateWaylandSurfaceKHR(VkInstance instance,
                                                                       const VkWaylandSurfaceCreateInfoKHR* pCreateInfo,
                                                                       const VkAllocationCallbacks* pAllocator,
                                                                       VkSurfaceKHR* pSurface) {
    struct loader_instance* loader_inst = loader_validate_and_get_instance_dispatch(instance, "vkCreateWaylandSurfaceKHR");
    return loader_inst->disp->layer_inst_disp.CreateWaylandSurfaceKHR(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateWaylandSurfaceKHR(VkInstance instance,
                                                                  const VkWaylandSurfaceCreateInfoKHR* pCreateInfo,
                                                                  const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    VkResult result = VK_SUCCESS;
    VkIcdSurface* icd_surface = NULL;
    loader_platform_thread_lock_mutex(&loader_lock);

    // Initialize pSurface to NULL just to be safe.
    *pSurface = VK_NULL_HANDLE;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance* loader_inst = loader_get_instance(instance);
    if (!loader_inst->enabled_extensions.khr_wayland_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_wayland_surface extension not enabled. vkCreateWaylandSurfaceKHR not executed!");
        result = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    result = allocate_icd_surface_struct(loader_inst, sizeof(icd_surface->wayland_surf.base), sizeof(icd_surface->wayland_surf),
                                         pAllocator, &icd_surface);
    if (VK_SUCCESS != result) {
        goto out;
    }

    icd_surface->wayland_surf.base.platform = VK_ICD_WSI_PLATFORM_WAYLAND;
    icd_surface->wayland_surf.display = pCreateInfo->display;
    icd_surface->wayland_surf.surface = pCreateInfo->surface;

    const struct loader_struct_type_info ci_types[] = {
        {VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR, sizeof(VkWaylandSurfaceCreateInfoKHR)},
    };
    result = copy_surface_create_info(loader_inst, icd_surface, pCreateInfo, sizeof(ci_types) / sizeof(ci_types[0]), ci_types,
                                      "VkWaylandSurfaceCreateInfoKHR", pAllocator);
    if (VK_SUCCESS != result) {
        goto out;
    }

    *pSurface = wrap_surface_handle(icd_surface);

out:
    cleanup_surface_creation(loader_inst, result, icd_surface, pAllocator);
    loader_platform_thread_unlock_mutex(&loader_lock);

    return result;
}

LOADER_EXPORT VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceWaylandPresentationSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                              uint32_t queueFamilyIndex,
                                                                                              struct wl_display* display) {
    VkPhysicalDevice unwrapped_phys_dev =
        loader_unwrap_physical_device(physicalDevice, "vkGetPhysicalDeviceWaylandPresentationSupportKHR");
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceWaylandPresentationSupportKHR(unwrapped_phys_dev, queueFamilyIndex, display);
}

VKAPI_ATTR VkBool32 VKAPI_CALL terminator_GetPhysicalDeviceWaylandPresentationSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                         uint32_t queueFamilyIndex,
                                                                                         struct wl_display* display) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    struct loader_instance* loader_inst = (struct loader_instance*)icd_term->this_instance;
    if (!loader_inst->enabled_extensions.khr_wayland_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_wayland_surface extension not enabled. vkGetPhysicalDeviceWaylandPresentationSupportKHR not executed!");
        return VK_FALSE;
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceWaylandPresentationSupportKHR) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device does not export vkGetPhysicalDeviceWaylandPresentationSupportKHR!");
        return VK_FALSE;
    }

    return icd_term->dispatch.GetPhysicalDeviceWaylandPresentationSupportKHR(phys_dev_term->phys_dev, queueFamilyIndex, display);
}

#endif  // VK_USE_PLATFORM_WAYLAND_KHR
#if defined(VK_USE_PLATFORM_ANDROID_KHR)

// ---- VK_KHR_android_surface extension commands
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateAndroidSurfaceKHR(VkInstance instance,
                                                                       const VkAndroidSurfaceCreateInfoKHR* pCreateInfo,
                                                                       const VkAllocationCallbacks* pAllocator,
                                                                       VkSurfaceKHR* pSurface) {
    struct loader_instance* loader_inst = loader_validate_and_get_instance_dispatch(instance, "vkCreateAndroidSurfaceKHR");
    return loader_inst->disp->layer_inst_disp.CreateAndroidSurfaceKHR(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateAndroidSurfaceKHR(VkInstance instance,
                                                                  const VkAndroidSurfaceCreateInfoKHR* pCreateInfo,
                                                                  const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    VkResult result = VK_SUCCESS;
    VkIcdSurface* icd_surface = NULL;
    loader_platform_thread_lock_mutex(&loader_lock);

    // Initialize pSurface to NULL just to be safe.
    *pSurface = VK_NULL_HANDLE;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance* loader_inst = loader_get_instance(instance);
    if (!loader_inst->enabled_extensions.khr_android_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_android_surface extension not enabled. vkCreateAndroidSurfaceKHR not executed!");
        result = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    result = allocate_icd_surface_struct(loader_inst, sizeof(icd_surface->android_surf.base), sizeof(icd_surface->android_surf),
                                         pAllocator, &icd_surface);
    if (VK_SUCCESS != result) {
        goto out;
    }

    icd_surface->android_surf.base.platform = VK_ICD_WSI_PLATFORM_ANDROID;
    icd_surface->android_surf.window = pCreateInfo->window;

    const struct loader_struct_type_info ci_types[] = {
        {VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR, sizeof(VkAndroidSurfaceCreateInfoKHR)},
    };
    result = copy_surface_create_info(loader_inst, icd_surface, pCreateInfo, sizeof(ci_types) / sizeof(ci_types[0]), ci_types,
                                      "VkAndroidSurfaceCreateInfoKHR", pAllocator);
    if (VK_SUCCESS != result) {
        goto out;
    }

    *pSurface = wrap_surface_handle(icd_surface);

out:
    cleanup_surface_creation(loader_inst, result, icd_surface, pAllocator);
    loader_platform_thread_unlock_mutex(&loader_lock);

    return result;
}

#endif  // VK_USE_PLATFORM_ANDROID_KHR
#if defined(VK_USE_PLATFORM_WIN32_KHR)

// ---- VK_KHR_win32_surface extension commands
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateWin32SurfaceKHR(VkInstance instance,
                                                                     const VkWin32SurfaceCreateInfoKHR* pCreateInfo,
                                                                     const VkAllocationCallbacks* pAllocator,
                                                                     VkSurfaceKHR* pSurface) {
    struct loader_instance* loader_inst = loader_validate_and_get_instance_dispatch(instance, "vkCreateWin32SurfaceKHR");
    return loader_inst->disp->layer_inst_disp.CreateWin32SurfaceKHR(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateWin32SurfaceKHR(VkInstance instance, const VkWin32SurfaceCreateInfoKHR* pCreateInfo,
                                                                const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    VkResult result = VK_SUCCESS;
    VkIcdSurface* icd_surface = NULL;
    loader_platform_thread_lock_mutex(&loader_lock);

    // Initialize pSurface to NULL just to be safe.
    *pSurface = VK_NULL_HANDLE;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance* loader_inst = loader_get_instance(instance);
    if (!loader_inst->enabled_extensions.khr_win32_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_win32_surface extension not enabled. vkCreateWin32SurfaceKHR not executed!");
        result = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    result = allocate_icd_surface_struct(loader_inst, sizeof(icd_surface->win_surf.base), sizeof(icd_surface->win_surf), pAllocator,
                                         &icd_surface);
    if (VK_SUCCESS != result) {
        goto out;
    }

    icd_surface->win_surf.base.platform = VK_ICD_WSI_PLATFORM_WIN32;
    icd_surface->win_surf.hinstance = pCreateInfo->hinstance;
    icd_surface->win_surf.hwnd = pCreateInfo->hwnd;

    const struct loader_struct_type_info ci_types[] = {
        {VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR, sizeof(VkWin32SurfaceCreateInfoKHR)},
    };
    result = copy_surface_create_info(loader_inst, icd_surface, pCreateInfo, sizeof(ci_types) / sizeof(ci_types[0]), ci_types,
                                      "VkWin32SurfaceCreateInfoKHR", pAllocator);
    if (VK_SUCCESS != result) {
        goto out;
    }

    *pSurface = wrap_surface_handle(icd_surface);

out:
    cleanup_surface_creation(loader_inst, result, icd_surface, pAllocator);
    loader_platform_thread_unlock_mutex(&loader_lock);

    return result;
}

LOADER_EXPORT VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceWin32PresentationSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                            uint32_t queueFamilyIndex) {
    VkPhysicalDevice unwrapped_phys_dev =
        loader_unwrap_physical_device(physicalDevice, "vkGetPhysicalDeviceWin32PresentationSupportKHR");
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceWin32PresentationSupportKHR(unwrapped_phys_dev, queueFamilyIndex);
}

VKAPI_ATTR VkBool32 VKAPI_CALL terminator_GetPhysicalDeviceWin32PresentationSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                       uint32_t queueFamilyIndex) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    struct loader_instance* loader_inst = (struct loader_instance*)icd_term->this_instance;
    if (!loader_inst->enabled_extensions.khr_win32_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_KHR_win32_surface extension not enabled. vkGetPhysicalDeviceWin32PresentationSupportKHR not executed!");
        return VK_FALSE;
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceWin32PresentationSupportKHR) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device does not export vkGetPhysicalDeviceWin32PresentationSupportKHR!");
        return VK_FALSE;
    }

    return icd_term->dispatch.GetPhysicalDeviceWin32PresentationSupportKHR(phys_dev_term->phys_dev, queueFamilyIndex);
}

#endif  // VK_USE_PLATFORM_WIN32_KHR
#if defined(VK_USE_PLATFORM_GGP)

// ---- VK_GGP_stream_descriptor_surface extension commands
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkCreateStreamDescriptorSurfaceGGP(VkInstance instance, const VkStreamDescriptorSurfaceCreateInfoGGP* pCreateInfo,
                                   const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    struct loader_instance* loader_inst = loader_validate_and_get_instance_dispatch(instance, "vkCreateStreamDescriptorSurfaceGGP");
    return loader_inst->disp->layer_inst_disp.CreateStreamDescriptorSurfaceGGP(loader_inst->instance, pCreateInfo, pAllocator,
                                                                               pSurface);
}

VKAPI_ATTR VkResult VKAPI_CALL
terminator_CreateStreamDescriptorSurfaceGGP(VkInstance instance, const VkStreamDescriptorSurfaceCreateInfoGGP* pCreateInfo,
                                            const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    VkResult result = VK_SUCCESS;
    VkIcdSurface* icd_surface = NULL;
    loader_platform_thread_lock_mutex(&loader_lock);

    // Initialize pSurface to NULL just to be safe.
    *pSurface = VK_NULL_HANDLE;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance* loader_inst = loader_get_instance(instance);
    if (!loader_inst->enabled_extensions.ggp_stream_descriptor_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_GGP_stream_descriptor_surface extension not enabled. vkCreateStreamDescriptorSurfaceGGP not executed!");
        result = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    result = allocate_icd_surface_struct(loader_inst, sizeof(icd_surface->ggp_surf.base), sizeof(icd_surface->ggp_surf), pAllocator,
                                         &icd_surface);
    if (VK_SUCCESS != result) {
        goto out;
    }

    icd_surface->ggp_surf.base.platform = VK_ICD_WSI_PLATFORM_GGP;
    icd_surface->ggp_surf.streamDescriptor = pCreateInfo->streamDescriptor;

    const struct loader_struct_type_info ci_types[] = {
        {VK_STRUCTURE_TYPE_STREAM_DESCRIPTOR_SURFACE_CREATE_INFO_GGP, sizeof(VkStreamDescriptorSurfaceCreateInfoGGP)},
    };
    result = copy_surface_create_info(loader_inst, icd_surface, pCreateInfo, sizeof(ci_types) / sizeof(ci_types[0]), ci_types,
                                      "VkStreamDescriptorSurfaceCreateInfoGGP", pAllocator);
    if (VK_SUCCESS != result) {
        goto out;
    }

    *pSurface = wrap_surface_handle(icd_surface);

out:
    cleanup_surface_creation(loader_inst, result, icd_surface, pAllocator);
    loader_platform_thread_unlock_mutex(&loader_lock);

    return result;
}

#endif  // VK_USE_PLATFORM_GGP
#if defined(VK_USE_PLATFORM_VI_NN)

// ---- VK_NN_vi_surface extension commands
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateViSurfaceNN(VkInstance instance, const VkViSurfaceCreateInfoNN* pCreateInfo,
                                                                 const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    struct loader_instance* loader_inst = loader_validate_and_get_instance_dispatch(instance, "vkCreateViSurfaceNN");
    return loader_inst->disp->layer_inst_disp.CreateViSurfaceNN(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateViSurfaceNN(VkInstance instance, const VkViSurfaceCreateInfoNN* pCreateInfo,
                                                            const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    VkResult result = VK_SUCCESS;
    VkIcdSurface* icd_surface = NULL;
    loader_platform_thread_lock_mutex(&loader_lock);

    // Initialize pSurface to NULL just to be safe.
    *pSurface = VK_NULL_HANDLE;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance* loader_inst = loader_get_instance(instance);
    if (!loader_inst->enabled_extensions.nn_vi_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_NN_vi_surface extension not enabled. vkCreateViSurfaceNN not executed!");
        result = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    result = allocate_icd_surface_struct(loader_inst, sizeof(icd_surface->vi_surf.base), sizeof(icd_surface->vi_surf), pAllocator,
                                         &icd_surface);
    if (VK_SUCCESS != result) {
        goto out;
    }

    icd_surface->vi_surf.base.platform = VK_ICD_WSI_PLATFORM_VI;
    icd_surface->vi_surf.window = pCreateInfo->window;

    const struct loader_struct_type_info ci_types[] = {
        {VK_STRUCTURE_TYPE_VI_SURFACE_CREATE_INFO_NN, sizeof(VkViSurfaceCreateInfoNN)},
    };
    result = copy_surface_create_info(loader_inst, icd_surface, pCreateInfo, sizeof(ci_types) / sizeof(ci_types[0]), ci_types,
                                      "VkViSurfaceCreateInfoNN", pAllocator);
    if (VK_SUCCESS != result) {
        goto out;
    }

    *pSurface = wrap_surface_handle(icd_surface);

out:
    cleanup_surface_creation(loader_inst, result, icd_surface, pAllocator);
    loader_platform_thread_unlock_mutex(&loader_lock);

    return result;
}

#endif  // VK_USE_PLATFORM_VI_NN
#if defined(VK_USE_PLATFORM_IOS_MVK)

// ---- VK_MVK_ios_surface extension commands
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateIOSSurfaceMVK(VkInstance instance,
                                                                   const VkIOSSurfaceCreateInfoMVK* pCreateInfo,
                                                                   const VkAllocationCallbacks* pAllocator,
                                                                   VkSurfaceKHR* pSurface) {
    struct loader_instance* loader_inst = loader_validate_and_get_instance_dispatch(instance, "vkCreateIOSSurfaceMVK");
    return loader_inst->disp->layer_inst_disp.CreateIOSSurfaceMVK(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateIOSSurfaceMVK(VkInstance instance, const VkIOSSurfaceCreateInfoMVK* pCreateInfo,
                                                              const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    VkResult result = VK_SUCCESS;
    VkIcdSurface* icd_surface = NULL;
    loader_platform_thread_lock_mutex(&loader_lock);

    // Initialize pSurface to NULL just to be safe.
    *pSurface = VK_NULL_HANDLE;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance* loader_inst = loader_get_instance(instance);
    if (!loader_inst->enabled_extensions.mvk_ios_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_MVK_ios_surface extension not enabled. vkCreateIOSSurfaceMVK not executed!");
        result = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    result = allocate_icd_surface_struct(loader_inst, sizeof(icd_surface->ios_surf.base), sizeof(icd_surface->ios_surf), pAllocator,
                                         &icd_surface);
    if (VK_SUCCESS != result) {
        goto out;
    }

    icd_surface->ios_surf.base.platform = VK_ICD_WSI_PLATFORM_IOS;
    icd_surface->ios_surf.pView = pCreateInfo->pView;

    const struct loader_struct_type_info ci_types[] = {
        {VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK, sizeof(VkIOSSurfaceCreateInfoMVK)},
    };
    result = copy_surface_create_info(loader_inst, icd_surface, pCreateInfo, sizeof(ci_types) / sizeof(ci_types[0]), ci_types,
                                      "VkIOSSurfaceCreateInfoMVK", pAllocator);
    if (VK_SUCCESS != result) {
        goto out;
    }

    *pSurface = wrap_surface_handle(icd_surface);

out:
    cleanup_surface_creation(loader_inst, result, icd_surface, pAllocator);
    loader_platform_thread_unlock_mutex(&loader_lock);

    return result;
}

#endif  // VK_USE_PLATFORM_IOS_MVK
#if defined(VK_USE_PLATFORM_MACOS_MVK)

// ---- VK_MVK_macos_surface extension commands
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateMacOSSurfaceMVK(VkInstance instance,
                                                                     const VkMacOSSurfaceCreateInfoMVK* pCreateInfo,
                                                                     const VkAllocationCallbacks* pAllocator,
                                                                     VkSurfaceKHR* pSurface) {
    struct loader_instance* loader_inst = loader_validate_and_get_instance_dispatch(instance, "vkCreateMacOSSurfaceMVK");
    return loader_inst->disp->layer_inst_disp.CreateMacOSSurfaceMVK(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateMacOSSurfaceMVK(VkInstance instance, const VkMacOSSurfaceCreateInfoMVK* pCreateInfo,
                                                                const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    VkResult result = VK_SUCCESS;
    VkIcdSurface* icd_surface = NULL;
    loader_platform_thread_lock_mutex(&loader_lock);

    // Initialize pSurface to NULL just to be safe.
    *pSurface = VK_NULL_HANDLE;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance* loader_inst = loader_get_instance(instance);
    if (!loader_inst->enabled_extensions.mvk_macos_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_MVK_macos_surface extension not enabled. vkCreateMacOSSurfaceMVK not executed!");
        result = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    result = allocate_icd_surface_struct(loader_inst, sizeof(icd_surface->macos_surf.base), sizeof(icd_surface->macos_surf),
                                         pAllocator, &icd_surface);
    if (VK_SUCCESS != result) {
        goto out;
    }

    icd_surface->macos_surf.base.platform = VK_ICD_WSI_PLATFORM_MACOS;
    icd_surface->macos_surf.pView = pCreateInfo->pView;

    const struct loader_struct_type_info ci_types[] = {
        {VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK, sizeof(VkMacOSSurfaceCreateInfoMVK)},
    };
    result = copy_surface_create_info(loader_inst, icd_surface, pCreateInfo, sizeof(ci_types) / sizeof(ci_types[0]), ci_types,
                                      "VkMacOSSurfaceCreateInfoMVK", pAllocator);
    if (VK_SUCCESS != result) {
        goto out;
    }

    *pSurface = wrap_surface_handle(icd_surface);

out:
    cleanup_surface_creation(loader_inst, result, icd_surface, pAllocator);
    loader_platform_thread_unlock_mutex(&loader_lock);

    return result;
}

#endif  // VK_USE_PLATFORM_MACOS_MVK
#if defined(VK_USE_PLATFORM_FUCHSIA)

// ---- VK_FUCHSIA_imagepipe_surface extension commands
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateImagePipeSurfaceFUCHSIA(VkInstance instance,
                                                                             const VkImagePipeSurfaceCreateInfoFUCHSIA* pCreateInfo,
                                                                             const VkAllocationCallbacks* pAllocator,
                                                                             VkSurfaceKHR* pSurface) {
    struct loader_instance* loader_inst = loader_validate_and_get_instance_dispatch(instance, "vkCreateImagePipeSurfaceFUCHSIA");
    return loader_inst->disp->layer_inst_disp.CreateImagePipeSurfaceFUCHSIA(loader_inst->instance, pCreateInfo, pAllocator,
                                                                            pSurface);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateImagePipeSurfaceFUCHSIA(VkInstance instance,
                                                                        const VkImagePipeSurfaceCreateInfoFUCHSIA* pCreateInfo,
                                                                        const VkAllocationCallbacks* pAllocator,
                                                                        VkSurfaceKHR* pSurface) {
    VkResult result = VK_SUCCESS;
    VkIcdSurface* icd_surface = NULL;
    loader_platform_thread_lock_mutex(&loader_lock);

    // Initialize pSurface to NULL just to be safe.
    *pSurface = VK_NULL_HANDLE;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance* loader_inst = loader_get_instance(instance);
    if (!loader_inst->enabled_extensions.fuchsia_imagepipe_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_FUCHSIA_imagepipe_surface extension not enabled. vkCreateImagePipeSurfaceFUCHSIA not executed!");
        result = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    result = allocate_icd_surface_struct(loader_inst, sizeof(icd_surface->imagepipe_surf.base), sizeof(icd_surface->imagepipe_surf),
                                         pAllocator, &icd_surface);
    if (VK_SUCCESS != result) {
        goto out;
    }

    icd_surface->imagepipe_surf.base.platform = VK_ICD_WSI_PLATFORM_FUCHSIA;
    const struct loader_struct_type_info ci_types[] = {
        {VK_STRUCTURE_TYPE_IMAGEPIPE_SURFACE_CREATE_INFO_FUCHSIA, sizeof(VkImagePipeSurfaceCreateInfoFUCHSIA)},
    };
    result = copy_surface_create_info(loader_inst, icd_surface, pCreateInfo, sizeof(ci_types) / sizeof(ci_types[0]), ci_types,
                                      "VkImagePipeSurfaceCreateInfoFUCHSIA", pAllocator);
    if (VK_SUCCESS != result) {
        goto out;
    }

    *pSurface = wrap_surface_handle(icd_surface);

out:
    cleanup_surface_creation(loader_inst, result, icd_surface, pAllocator);
    loader_platform_thread_unlock_mutex(&loader_lock);

    return result;
}

#endif  // VK_USE_PLATFORM_FUCHSIA
#if defined(VK_USE_PLATFORM_METAL_EXT)

// ---- VK_EXT_metal_surface extension commands
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateMetalSurfaceEXT(VkInstance instance,
                                                                     const VkMetalSurfaceCreateInfoEXT* pCreateInfo,
                                                                     const VkAllocationCallbacks* pAllocator,
                                                                     VkSurfaceKHR* pSurface) {
    struct loader_instance* loader_inst = loader_validate_and_get_instance_dispatch(instance, "vkCreateMetalSurfaceEXT");
    return loader_inst->disp->layer_inst_disp.CreateMetalSurfaceEXT(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateMetalSurfaceEXT(VkInstance instance, const VkMetalSurfaceCreateInfoEXT* pCreateInfo,
                                                                const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    VkResult result = VK_SUCCESS;
    VkIcdSurface* icd_surface = NULL;
    loader_platform_thread_lock_mutex(&loader_lock);

    // Initialize pSurface to NULL just to be safe.
    *pSurface = VK_NULL_HANDLE;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance* loader_inst = loader_get_instance(instance);
    if (!loader_inst->enabled_extensions.ext_metal_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_EXT_metal_surface extension not enabled. vkCreateMetalSurfaceEXT not executed!");
        result = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    result = allocate_icd_surface_struct(loader_inst, sizeof(icd_surface->metal_surf.base), sizeof(icd_surface->metal_surf),
                                         pAllocator, &icd_surface);
    if (VK_SUCCESS != result) {
        goto out;
    }

    icd_surface->metal_surf.base.platform = VK_ICD_WSI_PLATFORM_METAL;
    icd_surface->metal_surf.pLayer = pCreateInfo->pLayer;

    const struct loader_struct_type_info ci_types[] = {
        {VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT, sizeof(VkMetalSurfaceCreateInfoEXT)},
    };
    result = copy_surface_create_info(loader_inst, icd_surface, pCreateInfo, sizeof(ci_types) / sizeof(ci_types[0]), ci_types,
                                      "VkMetalSurfaceCreateInfoEXT", pAllocator);
    if (VK_SUCCESS != result) {
        goto out;
    }

    *pSurface = wrap_surface_handle(icd_surface);

out:
    cleanup_surface_creation(loader_inst, result, icd_surface, pAllocator);
    loader_platform_thread_unlock_mutex(&loader_lock);

    return result;
}

#endif  // VK_USE_PLATFORM_METAL_EXT

// ---- VK_EXT_headless_surface extension commands
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateHeadlessSurfaceEXT(VkInstance instance,
                                                                        const VkHeadlessSurfaceCreateInfoEXT* pCreateInfo,
                                                                        const VkAllocationCallbacks* pAllocator,
                                                                        VkSurfaceKHR* pSurface) {
    struct loader_instance* loader_inst = loader_validate_and_get_instance_dispatch(instance, "vkCreateHeadlessSurfaceEXT");
    return loader_inst->disp->layer_inst_disp.CreateHeadlessSurfaceEXT(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateHeadlessSurfaceEXT(VkInstance instance,
                                                                   const VkHeadlessSurfaceCreateInfoEXT* pCreateInfo,
                                                                   const VkAllocationCallbacks* pAllocator,
                                                                   VkSurfaceKHR* pSurface) {
    VkResult result = VK_SUCCESS;
    VkIcdSurface* icd_surface = NULL;
    loader_platform_thread_lock_mutex(&loader_lock);

    // Initialize pSurface to NULL just to be safe.
    *pSurface = VK_NULL_HANDLE;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance* loader_inst = loader_get_instance(instance);
    if (!loader_inst->enabled_extensions.ext_headless_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_EXT_headless_surface extension not enabled. vkCreateHeadlessSurfaceEXT not executed!");
        result = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    result = allocate_icd_surface_struct(loader_inst, sizeof(icd_surface->headless_surf.base), sizeof(icd_surface->headless_surf),
                                         pAllocator, &icd_surface);
    if (VK_SUCCESS != result) {
        goto out;
    }

    icd_surface->headless_surf.base.platform = VK_ICD_WSI_PLATFORM_HEADLESS;
    const struct loader_struct_type_info ci_types[] = {
        {VK_STRUCTURE_TYPE_HEADLESS_SURFACE_CREATE_INFO_EXT, sizeof(VkHeadlessSurfaceCreateInfoEXT)},
    };
    result = copy_surface_create_info(loader_inst, icd_surface, pCreateInfo, sizeof(ci_types) / sizeof(ci_types[0]), ci_types,
                                      "VkHeadlessSurfaceCreateInfoEXT", pAllocator);
    if (VK_SUCCESS != result) {
        goto out;
    }

    *pSurface = wrap_surface_handle(icd_surface);

out:
    cleanup_surface_creation(loader_inst, result, icd_surface, pAllocator);
    loader_platform_thread_unlock_mutex(&loader_lock);

    return result;
}

#if defined(VK_USE_PLATFORM_DIRECTFB_EXT)

// ---- VK_EXT_directfb_surface extension commands
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateDirectFBSurfaceEXT(VkInstance instance,
                                                                        const VkDirectFBSurfaceCreateInfoEXT* pCreateInfo,
                                                                        const VkAllocationCallbacks* pAllocator,
                                                                        VkSurfaceKHR* pSurface) {
    struct loader_instance* loader_inst = loader_validate_and_get_instance_dispatch(instance, "vkCreateDirectFBSurfaceEXT");
    return loader_inst->disp->layer_inst_disp.CreateDirectFBSurfaceEXT(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateDirectFBSurfaceEXT(VkInstance instance,
                                                                   const VkDirectFBSurfaceCreateInfoEXT* pCreateInfo,
                                                                   const VkAllocationCallbacks* pAllocator,
                                                                   VkSurfaceKHR* pSurface) {
    VkResult result = VK_SUCCESS;
    VkIcdSurface* icd_surface = NULL;
    loader_platform_thread_lock_mutex(&loader_lock);

    // Initialize pSurface to NULL just to be safe.
    *pSurface = VK_NULL_HANDLE;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance* loader_inst = loader_get_instance(instance);
    if (!loader_inst->enabled_extensions.ext_directfb_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_EXT_directfb_surface extension not enabled. vkCreateDirectFBSurfaceEXT not executed!");
        result = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    result = allocate_icd_surface_struct(loader_inst, sizeof(icd_surface->directfb_surf.base), sizeof(icd_surface->directfb_surf),
                                         pAllocator, &icd_surface);
    if (VK_SUCCESS != result) {
        goto out;
    }

    icd_surface->directfb_surf.base.platform = VK_ICD_WSI_PLATFORM_DIRECTFB;
    icd_surface->directfb_surf.dfb = pCreateInfo->dfb;
    icd_surface->directfb_surf.surface = pCreateInfo->surface;

    const struct loader_struct_type_info ci_types[] = {
        {VK_STRUCTURE_TYPE_DIRECTFB_SURFACE_CREATE_INFO_EXT, sizeof(VkDirectFBSurfaceCreateInfoEXT)},
    };
    result = copy_surface_create_info(loader_inst, icd_surface, pCreateInfo, sizeof(ci_types) / sizeof(ci_types[0]), ci_types,
                                      "VkDirectFBSurfaceCreateInfoEXT", pAllocator);
    if (VK_SUCCESS != result) {
        goto out;
    }

    *pSurface = wrap_surface_handle(icd_surface);

out:
    cleanup_surface_creation(loader_inst, result, icd_surface, pAllocator);
    loader_platform_thread_unlock_mutex(&loader_lock);

    return result;
}

LOADER_EXPORT VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceDirectFBPresentationSupportEXT(VkPhysicalDevice physicalDevice,
                                                                                               uint32_t queueFamilyIndex,
                                                                                               IDirectFB* dfb) {
    VkPhysicalDevice unwrapped_phys_dev =
        loader_unwrap_physical_device(physicalDevice, "vkGetPhysicalDeviceDirectFBPresentationSupportEXT");
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceDirectFBPresentationSupportEXT(unwrapped_phys_dev, queueFamilyIndex, dfb);
}

VKAPI_ATTR VkBool32 VKAPI_CALL terminator_GetPhysicalDeviceDirectFBPresentationSupportEXT(VkPhysicalDevice physicalDevice,
                                                                                          uint32_t queueFamilyIndex,
                                                                                          IDirectFB* dfb) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    struct loader_instance* loader_inst = (struct loader_instance*)icd_term->this_instance;
    if (!loader_inst->enabled_extensions.ext_directfb_surface) {
        loader_log(
            loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
            "VK_EXT_directfb_surface extension not enabled. vkGetPhysicalDeviceDirectFBPresentationSupportEXT not executed!");
        return VK_FALSE;
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceDirectFBPresentationSupportEXT) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device does not export vkGetPhysicalDeviceDirectFBPresentationSupportEXT!");
        return VK_FALSE;
    }

    return icd_term->dispatch.GetPhysicalDeviceDirectFBPresentationSupportEXT(phys_dev_term->phys_dev, queueFamilyIndex, dfb);
}

#endif  // VK_USE_PLATFORM_DIRECTFB_EXT
#if defined(VK_USE_PLATFORM_SCREEN_QNX)

// ---- VK_QNX_screen_surface extension commands
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateScreenSurfaceQNX(VkInstance instance,
                                                                      const VkScreenSurfaceCreateInfoQNX* pCreateInfo,
                                                                      const VkAllocationCallbacks* pAllocator,
                                                                      VkSurfaceKHR* pSurface) {
    struct loader_instance* loader_inst = loader_validate_and_get_instance_dispatch(instance, "vkCreateScreenSurfaceQNX");
    return loader_inst->disp->layer_inst_disp.CreateScreenSurfaceQNX(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateScreenSurfaceQNX(VkInstance instance,
                                                                 const VkScreenSurfaceCreateInfoQNX* pCreateInfo,
                                                                 const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    VkResult result = VK_SUCCESS;
    VkIcdSurface* icd_surface = NULL;
    loader_platform_thread_lock_mutex(&loader_lock);

    // Initialize pSurface to NULL just to be safe.
    *pSurface = VK_NULL_HANDLE;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance* loader_inst = loader_get_instance(instance);
    if (!loader_inst->enabled_extensions.qnx_screen_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_QNX_screen_surface extension not enabled. vkCreateScreenSurfaceQNX not executed!");
        result = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    result = allocate_icd_surface_struct(loader_inst, sizeof(icd_surface->screen_surf.base), sizeof(icd_surface->screen_surf),
                                         pAllocator, &icd_surface);
    if (VK_SUCCESS != result) {
        goto out;
    }

    icd_surface->screen_surf.base.platform = VK_ICD_WSI_PLATFORM_SCREEN;
    icd_surface->screen_surf.context = pCreateInfo->context;
    icd_surface->screen_surf.window = pCreateInfo->window;

    const struct loader_struct_type_info ci_types[] = {
        {VK_STRUCTURE_TYPE_SCREEN_SURFACE_CREATE_INFO_QNX, sizeof(VkScreenSurfaceCreateInfoQNX)},
    };
    result = copy_surface_create_info(loader_inst, icd_surface, pCreateInfo, sizeof(ci_types) / sizeof(ci_types[0]), ci_types,
                                      "VkScreenSurfaceCreateInfoQNX", pAllocator);
    if (VK_SUCCESS != result) {
        goto out;
    }

    *pSurface = wrap_surface_handle(icd_surface);

out:
    cleanup_surface_creation(loader_inst, result, icd_surface, pAllocator);
    loader_platform_thread_unlock_mutex(&loader_lock);

    return result;
}

LOADER_EXPORT VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceScreenPresentationSupportQNX(VkPhysicalDevice physicalDevice,
                                                                                             uint32_t queueFamilyIndex,
                                                                                             struct _screen_window* window) {
    VkPhysicalDevice unwrapped_phys_dev =
        loader_unwrap_physical_device(physicalDevice, "vkGetPhysicalDeviceScreenPresentationSupportQNX");
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceScreenPresentationSupportQNX(unwrapped_phys_dev, queueFamilyIndex, window);
}

VKAPI_ATTR VkBool32 VKAPI_CALL terminator_GetPhysicalDeviceScreenPresentationSupportQNX(VkPhysicalDevice physicalDevice,
                                                                                        uint32_t queueFamilyIndex,
                                                                                        struct _screen_window* window) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    struct loader_instance* loader_inst = (struct loader_instance*)icd_term->this_instance;
    if (!loader_inst->enabled_extensions.qnx_screen_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_QNX_screen_surface extension not enabled. vkGetPhysicalDeviceScreenPresentationSupportQNX not executed!");
        return VK_FALSE;
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceScreenPresentationSupportQNX) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device does not export vkGetPhysicalDeviceScreenPresentationSupportQNX!");
        return VK_FALSE;
    }

    return icd_term->dispatch.GetPhysicalDeviceScreenPresentationSupportQNX(phys_dev_term->phys_dev, queueFamilyIndex, window);
}

#endif  // VK_USE_PLATFORM_SCREEN_QNX
#if defined(VK_USE_PLATFORM_UBM_SEC)

// ---- VK_SEC_ubm_surface extension commands
LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateUbmSurfaceSEC(VkInstance instance,
                                                                   const VkUbmSurfaceCreateInfoSEC* pCreateInfo,
                                                                   const VkAllocationCallbacks* pAllocator,
                                                                   VkSurfaceKHR* pSurface) {
    struct loader_instance* loader_inst = loader_validate_and_get_instance_dispatch(instance, "vkCreateUbmSurfaceSEC");
    return loader_inst->disp->layer_inst_disp.CreateUbmSurfaceSEC(loader_inst->instance, pCreateInfo, pAllocator, pSurface);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_CreateUbmSurfaceSEC(VkInstance instance, const VkUbmSurfaceCreateInfoSEC* pCreateInfo,
                                                              const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    VkResult result = VK_SUCCESS;
    VkIcdSurface* icd_surface = NULL;
    loader_platform_thread_lock_mutex(&loader_lock);

    // Initialize pSurface to NULL just to be safe.
    *pSurface = VK_NULL_HANDLE;

    // First, check to ensure the appropriate extension was enabled:
    struct loader_instance* loader_inst = loader_get_instance(instance);
    if (!loader_inst->enabled_extensions.sec_ubm_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_SEC_ubm_surface extension not enabled. vkCreateUbmSurfaceSEC not executed!");
        result = VK_ERROR_EXTENSION_NOT_PRESENT;
        goto out;
    }

    // Next, if so, proceed with the implementation of this function:
    result = allocate_icd_surface_struct(loader_inst, sizeof(icd_surface->ubm_surf.base), sizeof(icd_surface->ubm_surf), pAllocator,
                                         &icd_surface);
    if (VK_SUCCESS != result) {
        goto out;
    }

    icd_surface->ubm_surf.base.platform = VK_ICD_WSI_PLATFORM_UBM;
    icd_surface->ubm_surf.device = pCreateInfo->device;
    icd_surface->ubm_surf.surface = pCreateInfo->surface;

    const struct loader_struct_type_info ci_types[] = {
        {VK_STRUCTURE_TYPE_UBM_SURFACE_CREATE_INFO_SEC, sizeof(VkUbmSurfaceCreateInfoSEC)},
    };
    result = copy_surface_create_info(loader_inst, icd_surface, pCreateInfo, sizeof(ci_types) / sizeof(ci_types[0]), ci_types,
                                      "VkUbmSurfaceCreateInfoSEC", pAllocator);
    if (VK_SUCCESS != result) {
        goto out;
    }

    *pSurface = wrap_surface_handle(icd_surface);

out:
    cleanup_surface_creation(loader_inst, result, icd_surface, pAllocator);
    loader_platform_thread_unlock_mutex(&loader_lock);

    return result;
}

LOADER_EXPORT VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceUbmPresentationSupportSEC(VkPhysicalDevice physicalDevice,
                                                                                          uint32_t queueFamilyIndex,
                                                                                          struct ubm_device* device) {
    VkPhysicalDevice unwrapped_phys_dev =
        loader_unwrap_physical_device(physicalDevice, "vkGetPhysicalDeviceUbmPresentationSupportSEC");
    const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
    return disp->GetPhysicalDeviceUbmPresentationSupportSEC(unwrapped_phys_dev, queueFamilyIndex, device);
}

VKAPI_ATTR VkBool32 VKAPI_CALL terminator_GetPhysicalDeviceUbmPresentationSupportSEC(VkPhysicalDevice physicalDevice,
                                                                                     uint32_t queueFamilyIndex,
                                                                                     struct ubm_device* device) {
    // First, check to ensure the appropriate extension was enabled:
    struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    struct loader_instance* loader_inst = (struct loader_instance*)icd_term->this_instance;
    if (!loader_inst->enabled_extensions.sec_ubm_surface) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "VK_SEC_ubm_surface extension not enabled. vkGetPhysicalDeviceUbmPresentationSupportSEC not executed!");
        return VK_FALSE;
    }

    if (NULL == icd_term->dispatch.GetPhysicalDeviceUbmPresentationSupportSEC) {
        loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD for selected physical device does not export vkGetPhysicalDeviceUbmPresentationSupportSEC!");
        return VK_FALSE;
    }

    return icd_term->dispatch.GetPhysicalDeviceUbmPresentationSupportSEC(phys_dev_term->phys_dev, queueFamilyIndex, device);
}

#endif  // VK_USE_PLATFORM_UBM_SEC

bool generated_wsi_swapchain_instance_gpa(struct loader_instance* loader_inst, const char* name, void** addr) {
    *addr = NULL;

    // ---- VK_KHR_display extension commands
    if (!strcmp("vkGetPhysicalDeviceDisplayPropertiesKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_display ? (void*)vkGetPhysicalDeviceDisplayPropertiesKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceDisplayPlanePropertiesKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_display ? (void*)vkGetPhysicalDeviceDisplayPlanePropertiesKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetDisplayPlaneSupportedDisplaysKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_display ? (void*)vkGetDisplayPlaneSupportedDisplaysKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetDisplayModePropertiesKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_display ? (void*)vkGetDisplayModePropertiesKHR : NULL;
        return true;
    }
    if (!strcmp("vkCreateDisplayModeKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_display ? (void*)vkCreateDisplayModeKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetDisplayPlaneCapabilitiesKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_display ? (void*)vkGetDisplayPlaneCapabilitiesKHR : NULL;
        return true;
    }
    if (!strcmp("vkCreateDisplayPlaneSurfaceKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_display ? (void*)vkCreateDisplayPlaneSurfaceKHR : NULL;
        return true;
    }
#if defined(VK_USE_PLATFORM_XLIB_KHR)

    // ---- VK_KHR_xlib_surface extension commands
    if (!strcmp("vkCreateXlibSurfaceKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_xlib_surface ? (void*)vkCreateXlibSurfaceKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceXlibPresentationSupportKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_xlib_surface ? (void*)vkGetPhysicalDeviceXlibPresentationSupportKHR : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_XLIB_KHR
#if defined(VK_USE_PLATFORM_XCB_KHR)

    // ---- VK_KHR_xcb_surface extension commands
    if (!strcmp("vkCreateXcbSurfaceKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_xcb_surface ? (void*)vkCreateXcbSurfaceKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceXcbPresentationSupportKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_xcb_surface ? (void*)vkGetPhysicalDeviceXcbPresentationSupportKHR : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_XCB_KHR
#if defined(VK_USE_PLATFORM_WAYLAND_KHR)

    // ---- VK_KHR_wayland_surface extension commands
    if (!strcmp("vkCreateWaylandSurfaceKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_wayland_surface ? (void*)vkCreateWaylandSurfaceKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceWaylandPresentationSupportKHR", name)) {
        *addr =
            loader_inst->enabled_extensions.khr_wayland_surface ? (void*)vkGetPhysicalDeviceWaylandPresentationSupportKHR : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_WAYLAND_KHR
#if defined(VK_USE_PLATFORM_ANDROID_KHR)

    // ---- VK_KHR_android_surface extension commands
    if (!strcmp("vkCreateAndroidSurfaceKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_android_surface ? (void*)vkCreateAndroidSurfaceKHR : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_ANDROID_KHR
#if defined(VK_USE_PLATFORM_WIN32_KHR)

    // ---- VK_KHR_win32_surface extension commands
    if (!strcmp("vkCreateWin32SurfaceKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_win32_surface ? (void*)vkCreateWin32SurfaceKHR : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceWin32PresentationSupportKHR", name)) {
        *addr = loader_inst->enabled_extensions.khr_win32_surface ? (void*)vkGetPhysicalDeviceWin32PresentationSupportKHR : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_WIN32_KHR
#if defined(VK_USE_PLATFORM_GGP)

    // ---- VK_GGP_stream_descriptor_surface extension commands
    if (!strcmp("vkCreateStreamDescriptorSurfaceGGP", name)) {
        *addr = loader_inst->enabled_extensions.ggp_stream_descriptor_surface ? (void*)vkCreateStreamDescriptorSurfaceGGP : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_GGP
#if defined(VK_USE_PLATFORM_VI_NN)

    // ---- VK_NN_vi_surface extension commands
    if (!strcmp("vkCreateViSurfaceNN", name)) {
        *addr = loader_inst->enabled_extensions.nn_vi_surface ? (void*)vkCreateViSurfaceNN : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_VI_NN
#if defined(VK_USE_PLATFORM_IOS_MVK)

    // ---- VK_MVK_ios_surface extension commands
    if (!strcmp("vkCreateIOSSurfaceMVK", name)) {
        *addr = loader_inst->enabled_extensions.mvk_ios_surface ? (void*)vkCreateIOSSurfaceMVK : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_IOS_MVK
#if defined(VK_USE_PLATFORM_MACOS_MVK)

    // ---- VK_MVK_macos_surface extension commands
    if (!strcmp("vkCreateMacOSSurfaceMVK", name)) {
        *addr = loader_inst->enabled_extensions.mvk_macos_surface ? (void*)vkCreateMacOSSurfaceMVK : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_MACOS_MVK
#if defined(VK_USE_PLATFORM_FUCHSIA)

    // ---- VK_FUCHSIA_imagepipe_surface extension commands
    if (!strcmp("vkCreateImagePipeSurfaceFUCHSIA", name)) {
        *addr = loader_inst->enabled_extensions.fuchsia_imagepipe_surface ? (void*)vkCreateImagePipeSurfaceFUCHSIA : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_FUCHSIA
#if defined(VK_USE_PLATFORM_METAL_EXT)

    // ---- VK_EXT_metal_surface extension commands
    if (!strcmp("vkCreateMetalSurfaceEXT", name)) {
        *addr = loader_inst->enabled_extensions.ext_metal_surface ? (void*)vkCreateMetalSurfaceEXT : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_METAL_EXT

    // ---- VK_EXT_headless_surface extension commands
    if (!strcmp("vkCreateHeadlessSurfaceEXT", name)) {
        *addr = loader_inst->enabled_extensions.ext_headless_surface ? (void*)vkCreateHeadlessSurfaceEXT : NULL;
        return true;
    }
#if defined(VK_USE_PLATFORM_DIRECTFB_EXT)

    // ---- VK_EXT_directfb_surface extension commands
    if (!strcmp("vkCreateDirectFBSurfaceEXT", name)) {
        *addr = loader_inst->enabled_extensions.ext_directfb_surface ? (void*)vkCreateDirectFBSurfaceEXT : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceDirectFBPresentationSupportEXT", name)) {
        *addr =
            loader_inst->enabled_extensions.ext_directfb_surface ? (void*)vkGetPhysicalDeviceDirectFBPresentationSupportEXT : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_DIRECTFB_EXT
#if defined(VK_USE_PLATFORM_SCREEN_QNX)

    // ---- VK_QNX_screen_surface extension commands
    if (!strcmp("vkCreateScreenSurfaceQNX", name)) {
        *addr = loader_inst->enabled_extensions.qnx_screen_surface ? (void*)vkCreateScreenSurfaceQNX : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceScreenPresentationSupportQNX", name)) {
        *addr = loader_inst->enabled_extensions.qnx_screen_surface ? (void*)vkGetPhysicalDeviceScreenPresentationSupportQNX : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_SCREEN_QNX
#if defined(VK_USE_PLATFORM_UBM_SEC)

    // ---- VK_SEC_ubm_surface extension commands
    if (!strcmp("vkCreateUbmSurfaceSEC", name)) {
        *addr = loader_inst->enabled_extensions.sec_ubm_surface ? (void*)vkCreateUbmSurfaceSEC : NULL;
        return true;
    }
    if (!strcmp("vkGetPhysicalDeviceUbmPresentationSupportSEC", name)) {
        *addr = loader_inst->enabled_extensions.sec_ubm_surface ? (void*)vkGetPhysicalDeviceUbmPresentationSupportSEC : NULL;
        return true;
    }
#endif  // VK_USE_PLATFORM_UBM_SEC
    return false;
}
