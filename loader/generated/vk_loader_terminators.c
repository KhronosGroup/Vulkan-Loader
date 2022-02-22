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


// --------------------------------------------
// ---- Vulkan API version 1.0 terminators ----
// --------------------------------------------


VKAPI_ATTR void VKAPI_CALL terminator_GetPhysicalDeviceFeatures(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceFeatures*                   pFeatures) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.GetPhysicalDeviceFeatures) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support GetPhysicalDeviceFeatures");
    }
    icd_term->dispatch.GetPhysicalDeviceFeatures(phys_dev_term->phys_dev, pFeatures);
}

VKAPI_ATTR void VKAPI_CALL terminator_GetPhysicalDeviceFormatProperties(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkFormatProperties*                         pFormatProperties) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.GetPhysicalDeviceFormatProperties) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support GetPhysicalDeviceFormatProperties");
    }
    icd_term->dispatch.GetPhysicalDeviceFormatProperties(phys_dev_term->phys_dev, format, pFormatProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceImageFormatProperties(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkImageType                                 type,
    VkImageTiling                               tiling,
    VkImageUsageFlags                           usage,
    VkImageCreateFlags                          flags,
    VkImageFormatProperties*                    pImageFormatProperties) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.GetPhysicalDeviceImageFormatProperties) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support GetPhysicalDeviceImageFormatProperties");
    }
    return icd_term->dispatch.GetPhysicalDeviceImageFormatProperties(phys_dev_term->phys_dev, format, type, tiling, usage, flags, pImageFormatProperties);
}

VKAPI_ATTR void VKAPI_CALL terminator_GetPhysicalDeviceProperties(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceProperties*                 pProperties) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.GetPhysicalDeviceProperties) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support GetPhysicalDeviceProperties");
    }
    icd_term->dispatch.GetPhysicalDeviceProperties(phys_dev_term->phys_dev, pProperties);
}

VKAPI_ATTR void VKAPI_CALL terminator_GetPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pQueueFamilyPropertyCount,
    VkQueueFamilyProperties*                    pQueueFamilyProperties) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.GetPhysicalDeviceQueueFamilyProperties) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support GetPhysicalDeviceQueueFamilyProperties");
    }
    icd_term->dispatch.GetPhysicalDeviceQueueFamilyProperties(phys_dev_term->phys_dev, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}

VKAPI_ATTR void VKAPI_CALL terminator_GetPhysicalDeviceMemoryProperties(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceMemoryProperties*           pMemoryProperties) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.GetPhysicalDeviceMemoryProperties) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support GetPhysicalDeviceMemoryProperties");
    }
    icd_term->dispatch.GetPhysicalDeviceMemoryProperties(phys_dev_term->phys_dev, pMemoryProperties);
}

VKAPI_ATTR void VKAPI_CALL terminator_GetPhysicalDeviceSparseImageFormatProperties(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkImageType                                 type,
    VkSampleCountFlagBits                       samples,
    VkImageUsageFlags                           usage,
    VkImageTiling                               tiling,
    uint32_t*                                   pPropertyCount,
    VkSparseImageFormatProperties*              pProperties) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.GetPhysicalDeviceSparseImageFormatProperties) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support GetPhysicalDeviceSparseImageFormatProperties");
    }
    icd_term->dispatch.GetPhysicalDeviceSparseImageFormatProperties(phys_dev_term->phys_dev, format, type, samples, usage, tiling, pPropertyCount, pProperties);
}

// ----------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_video_queue terminators ----
// ----------------------------------------------------------------

#ifdef VK_ENABLE_BETA_EXTENSIONS

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceVideoCapabilitiesKHR(
    VkPhysicalDevice                            physicalDevice,
    const VkVideoProfileKHR*                    pVideoProfile,
    VkVideoCapabilitiesKHR*                     pCapabilities) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.GetPhysicalDeviceVideoCapabilitiesKHR) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support GetPhysicalDeviceVideoCapabilitiesKHR");
    }
    return icd_term->dispatch.GetPhysicalDeviceVideoCapabilitiesKHR(phys_dev_term->phys_dev, pVideoProfile, pCapabilities);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceVideoFormatPropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceVideoFormatInfoKHR*   pVideoFormatInfo,
    uint32_t*                                   pVideoFormatPropertyCount,
    VkVideoFormatPropertiesKHR*                 pVideoFormatProperties) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.GetPhysicalDeviceVideoFormatPropertiesKHR) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support GetPhysicalDeviceVideoFormatPropertiesKHR");
    }
    return icd_term->dispatch.GetPhysicalDeviceVideoFormatPropertiesKHR(phys_dev_term->phys_dev, pVideoFormatInfo, pVideoFormatPropertyCount, pVideoFormatProperties);
}
#endif // VK_ENABLE_BETA_EXTENSIONS

// ----------------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_performance_query terminators ----
// ----------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL terminator_EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    uint32_t*                                   pCounterCount,
    VkPerformanceCounterKHR*                    pCounters,
    VkPerformanceCounterDescriptionKHR*         pCounterDescriptions) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR");
    }
    return icd_term->dispatch.EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR(phys_dev_term->phys_dev, queueFamilyIndex, pCounterCount, pCounters, pCounterDescriptions);
}

VKAPI_ATTR void VKAPI_CALL terminator_GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR(
    VkPhysicalDevice                            physicalDevice,
    const VkQueryPoolPerformanceCreateInfoKHR*  pPerformanceQueryCreateInfo,
    uint32_t*                                   pNumPasses) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR");
    }
    icd_term->dispatch.GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR(phys_dev_term->phys_dev, pPerformanceQueryCreateInfo, pNumPasses);
}

// --------------------------------------------------------------------------
// ---- Vulkan device extension VK_KHR_fragment_shading_rate terminators ----
// --------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceFragmentShadingRatesKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pFragmentShadingRateCount,
    VkPhysicalDeviceFragmentShadingRateKHR*     pFragmentShadingRates) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.GetPhysicalDeviceFragmentShadingRatesKHR) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support GetPhysicalDeviceFragmentShadingRatesKHR");
    }
    return icd_term->dispatch.GetPhysicalDeviceFragmentShadingRatesKHR(phys_dev_term->phys_dev, pFragmentShadingRateCount, pFragmentShadingRates);
}

// -----------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_debug_marker terminators ----
// -----------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL terminator_DebugMarkerSetObjectTagEXT(
    VkDevice                                    device,
    const VkDebugMarkerObjectTagInfoEXT*        pTagInfo) {
    uint32_t icd_index = 0;
    struct loader_device *dev;
    struct loader_icd_term *icd_term = loader_get_icd_and_device(device, &dev, &icd_index);
    if (NULL != icd_term && NULL != icd_term->dispatch.DebugMarkerSetObjectTagEXT) {
        VkDebugMarkerObjectTagInfoEXT local_tag_info;
        memcpy(&local_tag_info, pTagInfo, sizeof(VkDebugMarkerObjectTagInfoEXT));
        // If this is a physical device, we have to replace it with the proper one for the next call.
        if (pTagInfo->objectType == VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT) {
            struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)(uintptr_t)pTagInfo->object;
            local_tag_info.object = (uint64_t)(uintptr_t)phys_dev_term->phys_dev;
        // If this is a KHR_surface, and the ICD has created its own, we have to replace it with the proper one for the next call.
        } else if (pTagInfo->objectType == VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT) {
            if (NULL != icd_term && NULL != icd_term->dispatch.CreateSwapchainKHR) {
                VkIcdSurface *icd_surface = (VkIcdSurface *)(uintptr_t)pTagInfo->object;
                if (NULL != icd_surface->real_icd_surfaces) {
                    local_tag_info.object = (uint64_t)icd_surface->real_icd_surfaces[icd_index];
                }
            }
        }
        return icd_term->dispatch.DebugMarkerSetObjectTagEXT(device, &local_tag_info);
    } else {
        return VK_SUCCESS;
    }
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_DebugMarkerSetObjectNameEXT(
    VkDevice                                    device,
    const VkDebugMarkerObjectNameInfoEXT*       pNameInfo) {
    uint32_t icd_index = 0;
    struct loader_device *dev;
    struct loader_icd_term *icd_term = loader_get_icd_and_device(device, &dev, &icd_index);
    if (NULL != icd_term && NULL != icd_term->dispatch.DebugMarkerSetObjectNameEXT) {
        VkDebugMarkerObjectNameInfoEXT local_name_info;
        memcpy(&local_name_info, pNameInfo, sizeof(VkDebugMarkerObjectNameInfoEXT));
        // If this is a physical device, we have to replace it with the proper one for the next call.
        if (pNameInfo->objectType == VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT) {
            struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)(uintptr_t)pNameInfo->object;
            local_name_info.object = (uint64_t)(uintptr_t)phys_dev_term->phys_dev;
        // If this is a KHR_surface, and the ICD has created its own, we have to replace it with the proper one for the next call.
        } else if (pNameInfo->objectType == VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT) {
            if (NULL != icd_term && NULL != icd_term->dispatch.CreateSwapchainKHR) {
                VkIcdSurface *icd_surface = (VkIcdSurface *)(uintptr_t)pNameInfo->object;
                if (NULL != icd_surface->real_icd_surfaces) {
                    local_name_info.object = (uint64_t)icd_surface->real_icd_surfaces[icd_index];
                }
            }
        }
        return icd_term->dispatch.DebugMarkerSetObjectNameEXT(device, &local_name_info);
    } else {
        return VK_SUCCESS;
    }
}

// --------------------------------------------------------------------------
// ---- Vulkan instance extension VK_EXT_direct_mode_display terminators ----
// --------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL terminator_ReleaseDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.ReleaseDisplayEXT) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support ReleaseDisplayEXT");
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    return icd_term->dispatch.ReleaseDisplayEXT(phys_dev_term->phys_dev, display);
}

// ------------------------------------------------------------------
// ---- Vulkan instance extension VK_EXT_debug_utils terminators ----
// ------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL terminator_SetDebugUtilsObjectNameEXT(
    VkDevice                                    device,
    const VkDebugUtilsObjectNameInfoEXT*        pNameInfo) {
    uint32_t icd_index = 0;
    struct loader_device *dev;
    struct loader_icd_term *icd_term = loader_get_icd_and_device(device, &dev, &icd_index);
    if (NULL != icd_term && NULL != icd_term->dispatch.SetDebugUtilsObjectNameEXT) {
        VkDebugUtilsObjectNameInfoEXT local_name_info;
        memcpy(&local_name_info, pNameInfo, sizeof(VkDebugUtilsObjectNameInfoEXT));
        // If this is a physical device, we have to replace it with the proper one for the next call.
        if (pNameInfo->objectType == VK_OBJECT_TYPE_PHYSICAL_DEVICE) {
            struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)(uintptr_t)pNameInfo->objectHandle;
            local_name_info.objectHandle = (uint64_t)(uintptr_t)phys_dev_term->phys_dev;
        // If this is a KHR_surface, and the ICD has created its own, we have to replace it with the proper one for the next call.
        } else if (pNameInfo->objectType == VK_OBJECT_TYPE_SURFACE_KHR) {
            if (NULL != icd_term && NULL != icd_term->dispatch.CreateSwapchainKHR) {
                VkIcdSurface *icd_surface = (VkIcdSurface *)(uintptr_t)pNameInfo->objectHandle;
                if (NULL != icd_surface->real_icd_surfaces) {
                    local_name_info.objectHandle = (uint64_t)icd_surface->real_icd_surfaces[icd_index];
                }
            }
        }
        return icd_term->dispatch.SetDebugUtilsObjectNameEXT(device, &local_name_info);
    } else {
        return VK_SUCCESS;
    }
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_SetDebugUtilsObjectTagEXT(
    VkDevice                                    device,
    const VkDebugUtilsObjectTagInfoEXT*         pTagInfo) {
    uint32_t icd_index = 0;
    struct loader_device *dev;
    struct loader_icd_term *icd_term = loader_get_icd_and_device(device, &dev, &icd_index);
    if (NULL != icd_term && NULL != icd_term->dispatch.SetDebugUtilsObjectTagEXT) {
        VkDebugUtilsObjectTagInfoEXT local_tag_info;
        memcpy(&local_tag_info, pTagInfo, sizeof(VkDebugUtilsObjectTagInfoEXT));
        // If this is a physical device, we have to replace it with the proper one for the next call.
        if (pTagInfo->objectType == VK_OBJECT_TYPE_PHYSICAL_DEVICE) {
            struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)(uintptr_t)pTagInfo->objectHandle;
            local_tag_info.objectHandle = (uint64_t)(uintptr_t)phys_dev_term->phys_dev;
        // If this is a KHR_surface, and the ICD has created its own, we have to replace it with the proper one for the next call.
        } else if (pTagInfo->objectType == VK_OBJECT_TYPE_SURFACE_KHR) {
            if (NULL != icd_term && NULL != icd_term->dispatch.CreateSwapchainKHR) {
                VkIcdSurface *icd_surface = (VkIcdSurface *)(uintptr_t)pTagInfo->objectHandle;
                if (NULL != icd_surface->real_icd_surfaces) {
                    local_tag_info.objectHandle = (uint64_t)icd_surface->real_icd_surfaces[icd_index];
                }
            }
        }
        return icd_term->dispatch.SetDebugUtilsObjectTagEXT(device, &local_tag_info);
    } else {
        return VK_SUCCESS;
    }
}

VKAPI_ATTR void VKAPI_CALL terminator_QueueBeginDebugUtilsLabelEXT(
    VkQueue                                     queue,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    uint32_t icd_index = 0;
    struct loader_device *dev;
    struct loader_icd_term *icd_term = loader_get_icd_and_device(queue, &dev, &icd_index);
    if (NULL != icd_term && NULL != icd_term->dispatch.QueueBeginDebugUtilsLabelEXT) {
        icd_term->dispatch.QueueBeginDebugUtilsLabelEXT(queue, pLabelInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL terminator_QueueEndDebugUtilsLabelEXT(
    VkQueue                                     queue) {
    uint32_t icd_index = 0;
    struct loader_device *dev;
    struct loader_icd_term *icd_term = loader_get_icd_and_device(queue, &dev, &icd_index);
    if (NULL != icd_term && NULL != icd_term->dispatch.QueueEndDebugUtilsLabelEXT) {
        icd_term->dispatch.QueueEndDebugUtilsLabelEXT(queue);
    }
}

VKAPI_ATTR void VKAPI_CALL terminator_QueueInsertDebugUtilsLabelEXT(
    VkQueue                                     queue,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    uint32_t icd_index = 0;
    struct loader_device *dev;
    struct loader_icd_term *icd_term = loader_get_icd_and_device(queue, &dev, &icd_index);
    if (NULL != icd_term && NULL != icd_term->dispatch.QueueInsertDebugUtilsLabelEXT) {
        icd_term->dispatch.QueueInsertDebugUtilsLabelEXT(queue, pLabelInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL terminator_CmdBeginDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    uint32_t icd_index = 0;
    struct loader_device *dev;
    struct loader_icd_term *icd_term = loader_get_icd_and_device(commandBuffer, &dev, &icd_index);
    if (NULL != icd_term && NULL != icd_term->dispatch.CmdBeginDebugUtilsLabelEXT) {
        icd_term->dispatch.CmdBeginDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
    }
}

VKAPI_ATTR void VKAPI_CALL terminator_CmdEndDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer) {
    uint32_t icd_index = 0;
    struct loader_device *dev;
    struct loader_icd_term *icd_term = loader_get_icd_and_device(commandBuffer, &dev, &icd_index);
    if (NULL != icd_term && NULL != icd_term->dispatch.CmdEndDebugUtilsLabelEXT) {
        icd_term->dispatch.CmdEndDebugUtilsLabelEXT(commandBuffer);
    }
}

VKAPI_ATTR void VKAPI_CALL terminator_CmdInsertDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    uint32_t icd_index = 0;
    struct loader_device *dev;
    struct loader_icd_term *icd_term = loader_get_icd_and_device(commandBuffer, &dev, &icd_index);
    if (NULL != icd_term && NULL != icd_term->dispatch.CmdInsertDebugUtilsLabelEXT) {
        icd_term->dispatch.CmdInsertDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
    }
}

// ---------------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_sample_locations terminators ----
// ---------------------------------------------------------------------


VKAPI_ATTR void VKAPI_CALL terminator_GetPhysicalDeviceMultisamplePropertiesEXT(
    VkPhysicalDevice                            physicalDevice,
    VkSampleCountFlagBits                       samples,
    VkMultisamplePropertiesEXT*                 pMultisampleProperties) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.GetPhysicalDeviceMultisamplePropertiesEXT) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support GetPhysicalDeviceMultisamplePropertiesEXT");
    }
    icd_term->dispatch.GetPhysicalDeviceMultisamplePropertiesEXT(phys_dev_term->phys_dev, samples, pMultisampleProperties);
}

// --------------------------------------------------------------------------
// ---- Vulkan device extension VK_EXT_calibrated_timestamps terminators ----
// --------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceCalibrateableTimeDomainsEXT(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pTimeDomainCount,
    VkTimeDomainEXT*                            pTimeDomains) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.GetPhysicalDeviceCalibrateableTimeDomainsEXT) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support GetPhysicalDeviceCalibrateableTimeDomainsEXT");
    }
    return icd_term->dispatch.GetPhysicalDeviceCalibrateableTimeDomainsEXT(phys_dev_term->phys_dev, pTimeDomainCount, pTimeDomains);
}

// ----------------------------------------------------------------------
// ---- Vulkan device extension VK_NV_cooperative_matrix terminators ----
// ----------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceCooperativeMatrixPropertiesNV(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkCooperativeMatrixPropertiesNV*            pProperties) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.GetPhysicalDeviceCooperativeMatrixPropertiesNV) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support GetPhysicalDeviceCooperativeMatrixPropertiesNV");
    }
    return icd_term->dispatch.GetPhysicalDeviceCooperativeMatrixPropertiesNV(phys_dev_term->phys_dev, pPropertyCount, pProperties);
}

// ---------------------------------------------------------------------------
// ---- Vulkan device extension VK_NV_coverage_reduction_mode terminators ----
// ---------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL terminator_GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pCombinationCount,
    VkFramebufferMixedSamplesCombinationNV*     pCombinations) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV");
    }
    return icd_term->dispatch.GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV(phys_dev_term->phys_dev, pCombinationCount, pCombinations);
}

// --------------------------------------------------------------------------
// ---- Vulkan instance extension VK_EXT_acquire_drm_display terminators ----
// --------------------------------------------------------------------------


VKAPI_ATTR VkResult VKAPI_CALL terminator_AcquireDrmDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    int32_t                                     drmFd,
    VkDisplayKHR                                display) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.AcquireDrmDisplayEXT) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support AcquireDrmDisplayEXT");
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    return icd_term->dispatch.AcquireDrmDisplayEXT(phys_dev_term->phys_dev, drmFd, display);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetDrmDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    int32_t                                     drmFd,
    uint32_t                                    connectorId,
    VkDisplayKHR*                               display) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.GetDrmDisplayEXT) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support GetDrmDisplayEXT");
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    return icd_term->dispatch.GetDrmDisplayEXT(phys_dev_term->phys_dev, drmFd, connectorId, display);
}

// -------------------------------------------------------------------------
// ---- Vulkan device extension VK_NV_acquire_winrt_display terminators ----
// -------------------------------------------------------------------------

#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL terminator_AcquireWinrtDisplayNV(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.AcquireWinrtDisplayNV) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support AcquireWinrtDisplayNV");
    }
    return icd_term->dispatch.AcquireWinrtDisplayNV(phys_dev_term->phys_dev, display);
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_GetWinrtDisplayNV(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    deviceRelativeId,
    VkDisplayKHR*                               pDisplay) {
    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)physicalDevice;
    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;
    if (NULL == icd_term->dispatch.GetWinrtDisplayNV) {
        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "ICD associated with VkPhysicalDevice does not support GetWinrtDisplayNV");
    }
    return icd_term->dispatch.GetWinrtDisplayNV(phys_dev_term->phys_dev, deviceRelativeId, pDisplay);
}
#endif // VK_USE_PLATFORM_WIN32_KHR


VKAPI_ATTR bool VKAPI_CALL loader_icd_init_entries(struct loader_icd_term *icd_term, VkInstance inst,
                                                   const PFN_vkGetInstanceProcAddr fp_gipa) {

#define LOOKUP_GIPA(func, required)                                                        \
    do {                                                                                   \
        icd_term->dispatch.func = (PFN_vk##func)fp_gipa(inst, "vk" #func);                 \
        if (!icd_term->dispatch.func && required) {                                        \
            loader_log((struct loader_instance *)inst, VULKAN_LOADER_WARN_BIT, 0, \
                       loader_platform_get_proc_address_error("vk" #func));                \
            return false;                                                                  \
        }                                                                                  \
    } while (0)

    // ---- Vulkan 1.0 commands
    LOOKUP_GIPA(DestroyInstance, true);
    LOOKUP_GIPA(EnumeratePhysicalDevices, true);
    LOOKUP_GIPA(GetPhysicalDeviceFeatures, true);
    LOOKUP_GIPA(GetPhysicalDeviceFormatProperties, true);
    LOOKUP_GIPA(GetPhysicalDeviceImageFormatProperties, true);
    LOOKUP_GIPA(GetPhysicalDeviceProperties, true);
    LOOKUP_GIPA(GetPhysicalDeviceQueueFamilyProperties, true);
    LOOKUP_GIPA(GetPhysicalDeviceMemoryProperties, true);
    LOOKUP_GIPA(CreateDevice, true);
    LOOKUP_GIPA(EnumerateDeviceExtensionProperties, true);
    LOOKUP_GIPA(GetPhysicalDeviceSparseImageFormatProperties, true);
    LOOKUP_GIPA(GetDeviceProcAddr, true);

    // ---- Vulkan 1.1 commands
    LOOKUP_GIPA(EnumeratePhysicalDeviceGroups, false);
    LOOKUP_GIPA(GetPhysicalDeviceFeatures2, false);
    LOOKUP_GIPA(GetPhysicalDeviceProperties2, false);
    LOOKUP_GIPA(GetPhysicalDeviceFormatProperties2, false);
    LOOKUP_GIPA(GetPhysicalDeviceImageFormatProperties2, false);
    LOOKUP_GIPA(GetPhysicalDeviceQueueFamilyProperties2, false);
    LOOKUP_GIPA(GetPhysicalDeviceMemoryProperties2, false);
    LOOKUP_GIPA(GetPhysicalDeviceSparseImageFormatProperties2, false);
    LOOKUP_GIPA(GetPhysicalDeviceExternalBufferProperties, false);
    LOOKUP_GIPA(GetPhysicalDeviceExternalFenceProperties, false);
    LOOKUP_GIPA(GetPhysicalDeviceExternalSemaphoreProperties, false);

    // ---- Vulkan 1.3 commands
    LOOKUP_GIPA(GetPhysicalDeviceToolProperties, false);

    // ---- VK_KHR_surface extension commands
    LOOKUP_GIPA(DestroySurfaceKHR, false);
    LOOKUP_GIPA(GetPhysicalDeviceSurfaceSupportKHR, false);
    LOOKUP_GIPA(GetPhysicalDeviceSurfaceCapabilitiesKHR, false);
    LOOKUP_GIPA(GetPhysicalDeviceSurfaceFormatsKHR, false);
    LOOKUP_GIPA(GetPhysicalDeviceSurfacePresentModesKHR, false);

    // ---- VK_KHR_swapchain extension commands
    LOOKUP_GIPA(CreateSwapchainKHR, false);
    LOOKUP_GIPA(GetDeviceGroupSurfacePresentModesKHR, false);
    LOOKUP_GIPA(GetPhysicalDevicePresentRectanglesKHR, false);

    // ---- VK_KHR_display extension commands
    LOOKUP_GIPA(GetPhysicalDeviceDisplayPropertiesKHR, false);
    LOOKUP_GIPA(GetPhysicalDeviceDisplayPlanePropertiesKHR, false);
    LOOKUP_GIPA(GetDisplayPlaneSupportedDisplaysKHR, false);
    LOOKUP_GIPA(GetDisplayModePropertiesKHR, false);
    LOOKUP_GIPA(CreateDisplayModeKHR, false);
    LOOKUP_GIPA(GetDisplayPlaneCapabilitiesKHR, false);
    LOOKUP_GIPA(CreateDisplayPlaneSurfaceKHR, false);

    // ---- VK_KHR_display_swapchain extension commands
    LOOKUP_GIPA(CreateSharedSwapchainsKHR, false);

    // ---- VK_KHR_xlib_surface extension commands
#ifdef VK_USE_PLATFORM_XLIB_KHR
    LOOKUP_GIPA(CreateXlibSurfaceKHR, false);
    LOOKUP_GIPA(GetPhysicalDeviceXlibPresentationSupportKHR, false);
#endif // VK_USE_PLATFORM_XLIB_KHR

    // ---- VK_KHR_xcb_surface extension commands
#ifdef VK_USE_PLATFORM_XCB_KHR
    LOOKUP_GIPA(CreateXcbSurfaceKHR, false);
    LOOKUP_GIPA(GetPhysicalDeviceXcbPresentationSupportKHR, false);
#endif // VK_USE_PLATFORM_XCB_KHR

    // ---- VK_KHR_wayland_surface extension commands
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    LOOKUP_GIPA(CreateWaylandSurfaceKHR, false);
    LOOKUP_GIPA(GetPhysicalDeviceWaylandPresentationSupportKHR, false);
#endif // VK_USE_PLATFORM_WAYLAND_KHR

    // ---- VK_KHR_win32_surface extension commands
#ifdef VK_USE_PLATFORM_WIN32_KHR
    LOOKUP_GIPA(CreateWin32SurfaceKHR, false);
    LOOKUP_GIPA(GetPhysicalDeviceWin32PresentationSupportKHR, false);
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ---- VK_KHR_video_queue extension commands
#ifdef VK_ENABLE_BETA_EXTENSIONS
    LOOKUP_GIPA(GetPhysicalDeviceVideoCapabilitiesKHR, false);
    LOOKUP_GIPA(GetPhysicalDeviceVideoFormatPropertiesKHR, false);
#endif // VK_ENABLE_BETA_EXTENSIONS

    // ---- VK_KHR_get_physical_device_properties2 extension commands
    LOOKUP_GIPA(GetPhysicalDeviceFeatures2KHR, false);
    LOOKUP_GIPA(GetPhysicalDeviceProperties2KHR, false);
    LOOKUP_GIPA(GetPhysicalDeviceFormatProperties2KHR, false);
    LOOKUP_GIPA(GetPhysicalDeviceImageFormatProperties2KHR, false);
    LOOKUP_GIPA(GetPhysicalDeviceQueueFamilyProperties2KHR, false);
    LOOKUP_GIPA(GetPhysicalDeviceMemoryProperties2KHR, false);
    LOOKUP_GIPA(GetPhysicalDeviceSparseImageFormatProperties2KHR, false);

    // ---- VK_KHR_device_group_creation extension commands
    LOOKUP_GIPA(EnumeratePhysicalDeviceGroupsKHR, false);

    // ---- VK_KHR_external_memory_capabilities extension commands
    LOOKUP_GIPA(GetPhysicalDeviceExternalBufferPropertiesKHR, false);

    // ---- VK_KHR_external_semaphore_capabilities extension commands
    LOOKUP_GIPA(GetPhysicalDeviceExternalSemaphorePropertiesKHR, false);

    // ---- VK_KHR_external_fence_capabilities extension commands
    LOOKUP_GIPA(GetPhysicalDeviceExternalFencePropertiesKHR, false);

    // ---- VK_KHR_performance_query extension commands
    LOOKUP_GIPA(EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR, false);
    LOOKUP_GIPA(GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR, false);

    // ---- VK_KHR_get_surface_capabilities2 extension commands
    LOOKUP_GIPA(GetPhysicalDeviceSurfaceCapabilities2KHR, false);
    LOOKUP_GIPA(GetPhysicalDeviceSurfaceFormats2KHR, false);

    // ---- VK_KHR_get_display_properties2 extension commands
    LOOKUP_GIPA(GetPhysicalDeviceDisplayProperties2KHR, false);
    LOOKUP_GIPA(GetPhysicalDeviceDisplayPlaneProperties2KHR, false);
    LOOKUP_GIPA(GetDisplayModeProperties2KHR, false);
    LOOKUP_GIPA(GetDisplayPlaneCapabilities2KHR, false);

    // ---- VK_KHR_fragment_shading_rate extension commands
    LOOKUP_GIPA(GetPhysicalDeviceFragmentShadingRatesKHR, false);

    // ---- VK_EXT_debug_report extension commands
    LOOKUP_GIPA(CreateDebugReportCallbackEXT, false);
    LOOKUP_GIPA(DestroyDebugReportCallbackEXT, false);
    LOOKUP_GIPA(DebugReportMessageEXT, false);

    // ---- VK_EXT_debug_marker extension commands
    LOOKUP_GIPA(DebugMarkerSetObjectTagEXT, false);
    LOOKUP_GIPA(DebugMarkerSetObjectNameEXT, false);

    // ---- VK_GGP_stream_descriptor_surface extension commands
#ifdef VK_USE_PLATFORM_GGP
    LOOKUP_GIPA(CreateStreamDescriptorSurfaceGGP, false);
#endif // VK_USE_PLATFORM_GGP

    // ---- VK_NV_external_memory_capabilities extension commands
    LOOKUP_GIPA(GetPhysicalDeviceExternalImageFormatPropertiesNV, false);

    // ---- VK_NN_vi_surface extension commands
#ifdef VK_USE_PLATFORM_VI_NN
    LOOKUP_GIPA(CreateViSurfaceNN, false);
#endif // VK_USE_PLATFORM_VI_NN

    // ---- VK_EXT_direct_mode_display extension commands
    LOOKUP_GIPA(ReleaseDisplayEXT, false);

    // ---- VK_EXT_acquire_xlib_display extension commands
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
    LOOKUP_GIPA(AcquireXlibDisplayEXT, false);
    LOOKUP_GIPA(GetRandROutputDisplayEXT, false);
#endif // VK_USE_PLATFORM_XLIB_XRANDR_EXT

    // ---- VK_EXT_display_surface_counter extension commands
    LOOKUP_GIPA(GetPhysicalDeviceSurfaceCapabilities2EXT, false);

    // ---- VK_MVK_ios_surface extension commands
#ifdef VK_USE_PLATFORM_IOS_MVK
    LOOKUP_GIPA(CreateIOSSurfaceMVK, false);
#endif // VK_USE_PLATFORM_IOS_MVK

    // ---- VK_MVK_macos_surface extension commands
#ifdef VK_USE_PLATFORM_MACOS_MVK
    LOOKUP_GIPA(CreateMacOSSurfaceMVK, false);
#endif // VK_USE_PLATFORM_MACOS_MVK

    // ---- VK_EXT_debug_utils extension commands
    LOOKUP_GIPA(SetDebugUtilsObjectNameEXT, false);
    LOOKUP_GIPA(SetDebugUtilsObjectTagEXT, false);
    LOOKUP_GIPA(QueueBeginDebugUtilsLabelEXT, false);
    LOOKUP_GIPA(QueueEndDebugUtilsLabelEXT, false);
    LOOKUP_GIPA(QueueInsertDebugUtilsLabelEXT, false);
    LOOKUP_GIPA(CmdBeginDebugUtilsLabelEXT, false);
    LOOKUP_GIPA(CmdEndDebugUtilsLabelEXT, false);
    LOOKUP_GIPA(CmdInsertDebugUtilsLabelEXT, false);
    LOOKUP_GIPA(CreateDebugUtilsMessengerEXT, false);
    LOOKUP_GIPA(DestroyDebugUtilsMessengerEXT, false);
    LOOKUP_GIPA(SubmitDebugUtilsMessageEXT, false);

    // ---- VK_EXT_sample_locations extension commands
    LOOKUP_GIPA(GetPhysicalDeviceMultisamplePropertiesEXT, false);

    // ---- VK_EXT_calibrated_timestamps extension commands
    LOOKUP_GIPA(GetPhysicalDeviceCalibrateableTimeDomainsEXT, false);

    // ---- VK_FUCHSIA_imagepipe_surface extension commands
#ifdef VK_USE_PLATFORM_FUCHSIA
    LOOKUP_GIPA(CreateImagePipeSurfaceFUCHSIA, false);
#endif // VK_USE_PLATFORM_FUCHSIA

    // ---- VK_EXT_metal_surface extension commands
#ifdef VK_USE_PLATFORM_METAL_EXT
    LOOKUP_GIPA(CreateMetalSurfaceEXT, false);
#endif // VK_USE_PLATFORM_METAL_EXT

    // ---- VK_EXT_tooling_info extension commands
    LOOKUP_GIPA(GetPhysicalDeviceToolPropertiesEXT, false);

    // ---- VK_NV_cooperative_matrix extension commands
    LOOKUP_GIPA(GetPhysicalDeviceCooperativeMatrixPropertiesNV, false);

    // ---- VK_NV_coverage_reduction_mode extension commands
    LOOKUP_GIPA(GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV, false);

    // ---- VK_EXT_full_screen_exclusive extension commands
#ifdef VK_USE_PLATFORM_WIN32_KHR
    LOOKUP_GIPA(GetPhysicalDeviceSurfacePresentModes2EXT, false);
    LOOKUP_GIPA(GetDeviceGroupSurfacePresentModes2EXT, false);
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ---- VK_EXT_headless_surface extension commands
    LOOKUP_GIPA(CreateHeadlessSurfaceEXT, false);

    // ---- VK_EXT_acquire_drm_display extension commands
    LOOKUP_GIPA(AcquireDrmDisplayEXT, false);
    LOOKUP_GIPA(GetDrmDisplayEXT, false);

    // ---- VK_NV_acquire_winrt_display extension commands
#ifdef VK_USE_PLATFORM_WIN32_KHR
    LOOKUP_GIPA(AcquireWinrtDisplayNV, false);
    LOOKUP_GIPA(GetWinrtDisplayNV, false);
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ---- VK_EXT_directfb_surface extension commands
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
    LOOKUP_GIPA(CreateDirectFBSurfaceEXT, false);
    LOOKUP_GIPA(GetPhysicalDeviceDirectFBPresentationSupportEXT, false);
#endif // VK_USE_PLATFORM_DIRECTFB_EXT

    // ---- VK_QNX_screen_surface extension commands
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    LOOKUP_GIPA(CreateScreenSurfaceQNX, false);
    LOOKUP_GIPA(GetPhysicalDeviceScreenPresentationSupportQNX, false);
#endif // VK_USE_PLATFORM_SCREEN_QNX

#undef LOOKUP_GIPA

    return true;
};

// Some device commands still need a terminator because the loader needs to unwrap something about them.
// In many cases, the item needing unwrapping is a VkPhysicalDevice or VkSurfaceKHR object.  But there may be other items
// in the future.
PFN_vkVoidFunction get_extension_device_proc_terminator(struct loader_device *dev, const char *pName) {
    PFN_vkVoidFunction addr = NULL;

    // ---- Vulkan 1.0 commands

    // ---- VK_KHR_swapchain extension commands
    if (dev->dev_ext_enables.khr_swapchain) {
        if(!strcmp(pName, "vkCreateSwapchainKHR")) {
            addr = (PFN_vkVoidFunction)terminator_CreateSwapchainKHR;
        } else if(!strcmp(pName, "vkGetDeviceGroupSurfacePresentModesKHR")) {
            addr = (PFN_vkVoidFunction)terminator_GetDeviceGroupSurfacePresentModesKHR;
        }
    } // VK_KHR_swapchain 

    // ---- VK_KHR_display_swapchain extension commands
    if (dev->dev_ext_enables.khr_display_swapchain && dev->dev_ext_enables.khr_swapchain) {
        if(!strcmp(pName, "vkCreateSharedSwapchainsKHR")) {
            addr = (PFN_vkVoidFunction)terminator_CreateSharedSwapchainsKHR;
        }
    } // VK_KHR_display_swapchain 

    // ---- VK_EXT_debug_marker extension commands
    if (dev->dev_ext_enables.ext_debug_marker) {
        if(!strcmp(pName, "vkDebugMarkerSetObjectTagEXT")) {
            addr = (PFN_vkVoidFunction)terminator_DebugMarkerSetObjectTagEXT;
        } else if(!strcmp(pName, "vkDebugMarkerSetObjectNameEXT")) {
            addr = (PFN_vkVoidFunction)terminator_DebugMarkerSetObjectNameEXT;
        }
    } // VK_EXT_debug_marker 

    // ---- VK_EXT_debug_utils extension commands
    if (dev->dev_ext_enables.ext_debug_utils) {
        if(!strcmp(pName, "vkSetDebugUtilsObjectNameEXT")) {
            addr = (PFN_vkVoidFunction)terminator_SetDebugUtilsObjectNameEXT;
        } else if(!strcmp(pName, "vkSetDebugUtilsObjectTagEXT")) {
            addr = (PFN_vkVoidFunction)terminator_SetDebugUtilsObjectTagEXT;
        } else if(!strcmp(pName, "vkQueueBeginDebugUtilsLabelEXT")) {
            addr = (PFN_vkVoidFunction)terminator_QueueBeginDebugUtilsLabelEXT;
        } else if(!strcmp(pName, "vkQueueEndDebugUtilsLabelEXT")) {
            addr = (PFN_vkVoidFunction)terminator_QueueEndDebugUtilsLabelEXT;
        } else if(!strcmp(pName, "vkQueueInsertDebugUtilsLabelEXT")) {
            addr = (PFN_vkVoidFunction)terminator_QueueInsertDebugUtilsLabelEXT;
        } else if(!strcmp(pName, "vkCmdBeginDebugUtilsLabelEXT")) {
            addr = (PFN_vkVoidFunction)terminator_CmdBeginDebugUtilsLabelEXT;
        } else if(!strcmp(pName, "vkCmdEndDebugUtilsLabelEXT")) {
            addr = (PFN_vkVoidFunction)terminator_CmdEndDebugUtilsLabelEXT;
        } else if(!strcmp(pName, "vkCmdInsertDebugUtilsLabelEXT")) {
            addr = (PFN_vkVoidFunction)terminator_CmdInsertDebugUtilsLabelEXT;
        }
    } // VK_EXT_debug_utils 

    // ---- VK_EXT_full_screen_exclusive extension commands
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (dev->dev_ext_enables.ext_full_screen_exclusive && dev->dev_ext_enables.khr_swapchain) {
        if(!strcmp(pName, "vkGetDeviceGroupSurfacePresentModes2EXT")) {
            addr = (PFN_vkVoidFunction)terminator_GetDeviceGroupSurfacePresentModes2EXT;
        }
    } // VK_EXT_full_screen_exclusive 
#endif // VK_USE_PLATFORM_WIN32_KHR

    return addr;
}


// This table contains the loader's instance dispatch table, which contains
// default functions if no instance layers are activated.  This contains
// pointers to "terminator functions".
const VkLayerInstanceDispatchTable instance_term_disp = {

    // ---- Vulkan 1.0 commands
    .CreateInstance = terminator_CreateInstance,
    .DestroyInstance = terminator_DestroyInstance,
    .EnumeratePhysicalDevices = terminator_EnumeratePhysicalDevices,
    .GetPhysicalDeviceFeatures = terminator_GetPhysicalDeviceFeatures,
    .GetPhysicalDeviceFormatProperties = terminator_GetPhysicalDeviceFormatProperties,
    .GetPhysicalDeviceImageFormatProperties = terminator_GetPhysicalDeviceImageFormatProperties,
    .GetPhysicalDeviceProperties = terminator_GetPhysicalDeviceProperties,
    .GetPhysicalDeviceQueueFamilyProperties = terminator_GetPhysicalDeviceQueueFamilyProperties,
    .GetPhysicalDeviceMemoryProperties = terminator_GetPhysicalDeviceMemoryProperties,
    .GetInstanceProcAddr = vkGetInstanceProcAddr,
    .CreateDevice = terminator_CreateDevice,
    .EnumerateDeviceExtensionProperties = terminator_EnumerateDeviceExtensionProperties,
    .EnumerateDeviceLayerProperties = terminator_EnumerateDeviceLayerProperties,
    .GetPhysicalDeviceSparseImageFormatProperties = terminator_GetPhysicalDeviceSparseImageFormatProperties,

    // ---- Vulkan 1.1 commands
    .EnumeratePhysicalDeviceGroups = terminator_EnumeratePhysicalDeviceGroups,
    .GetPhysicalDeviceFeatures2 = terminator_GetPhysicalDeviceFeatures2,
    .GetPhysicalDeviceProperties2 = terminator_GetPhysicalDeviceProperties2,
    .GetPhysicalDeviceFormatProperties2 = terminator_GetPhysicalDeviceFormatProperties2,
    .GetPhysicalDeviceImageFormatProperties2 = terminator_GetPhysicalDeviceImageFormatProperties2,
    .GetPhysicalDeviceQueueFamilyProperties2 = terminator_GetPhysicalDeviceQueueFamilyProperties2,
    .GetPhysicalDeviceMemoryProperties2 = terminator_GetPhysicalDeviceMemoryProperties2,
    .GetPhysicalDeviceSparseImageFormatProperties2 = terminator_GetPhysicalDeviceSparseImageFormatProperties2,
    .GetPhysicalDeviceExternalBufferProperties = terminator_GetPhysicalDeviceExternalBufferProperties,
    .GetPhysicalDeviceExternalFenceProperties = terminator_GetPhysicalDeviceExternalFenceProperties,
    .GetPhysicalDeviceExternalSemaphoreProperties = terminator_GetPhysicalDeviceExternalSemaphoreProperties,

    // ---- Vulkan 1.3 commands
    .GetPhysicalDeviceToolProperties = terminator_GetPhysicalDeviceToolProperties,

    // ---- VK_KHR_surface extension commands
    .DestroySurfaceKHR = terminator_DestroySurfaceKHR,
    .GetPhysicalDeviceSurfaceSupportKHR = terminator_GetPhysicalDeviceSurfaceSupportKHR,
    .GetPhysicalDeviceSurfaceCapabilitiesKHR = terminator_GetPhysicalDeviceSurfaceCapabilitiesKHR,
    .GetPhysicalDeviceSurfaceFormatsKHR = terminator_GetPhysicalDeviceSurfaceFormatsKHR,
    .GetPhysicalDeviceSurfacePresentModesKHR = terminator_GetPhysicalDeviceSurfacePresentModesKHR,

    // ---- VK_KHR_swapchain extension commands
    .GetPhysicalDevicePresentRectanglesKHR = terminator_GetPhysicalDevicePresentRectanglesKHR,

    // ---- VK_KHR_display extension commands
    .GetPhysicalDeviceDisplayPropertiesKHR = terminator_GetPhysicalDeviceDisplayPropertiesKHR,
    .GetPhysicalDeviceDisplayPlanePropertiesKHR = terminator_GetPhysicalDeviceDisplayPlanePropertiesKHR,
    .GetDisplayPlaneSupportedDisplaysKHR = terminator_GetDisplayPlaneSupportedDisplaysKHR,
    .GetDisplayModePropertiesKHR = terminator_GetDisplayModePropertiesKHR,
    .CreateDisplayModeKHR = terminator_CreateDisplayModeKHR,
    .GetDisplayPlaneCapabilitiesKHR = terminator_GetDisplayPlaneCapabilitiesKHR,
    .CreateDisplayPlaneSurfaceKHR = terminator_CreateDisplayPlaneSurfaceKHR,

    // ---- VK_KHR_xlib_surface extension commands
#ifdef VK_USE_PLATFORM_XLIB_KHR
    .CreateXlibSurfaceKHR = terminator_CreateXlibSurfaceKHR,
    .GetPhysicalDeviceXlibPresentationSupportKHR = terminator_GetPhysicalDeviceXlibPresentationSupportKHR,
#endif // VK_USE_PLATFORM_XLIB_KHR

    // ---- VK_KHR_xcb_surface extension commands
#ifdef VK_USE_PLATFORM_XCB_KHR
    .CreateXcbSurfaceKHR = terminator_CreateXcbSurfaceKHR,
    .GetPhysicalDeviceXcbPresentationSupportKHR = terminator_GetPhysicalDeviceXcbPresentationSupportKHR,
#endif // VK_USE_PLATFORM_XCB_KHR

    // ---- VK_KHR_wayland_surface extension commands
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    .CreateWaylandSurfaceKHR = terminator_CreateWaylandSurfaceKHR,
    .GetPhysicalDeviceWaylandPresentationSupportKHR = terminator_GetPhysicalDeviceWaylandPresentationSupportKHR,
#endif // VK_USE_PLATFORM_WAYLAND_KHR

    // ---- VK_KHR_win32_surface extension commands
#ifdef VK_USE_PLATFORM_WIN32_KHR
    .CreateWin32SurfaceKHR = terminator_CreateWin32SurfaceKHR,
    .GetPhysicalDeviceWin32PresentationSupportKHR = terminator_GetPhysicalDeviceWin32PresentationSupportKHR,
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ---- VK_KHR_video_queue extension commands
#ifdef VK_ENABLE_BETA_EXTENSIONS
    .GetPhysicalDeviceVideoCapabilitiesKHR = terminator_GetPhysicalDeviceVideoCapabilitiesKHR,
    .GetPhysicalDeviceVideoFormatPropertiesKHR = terminator_GetPhysicalDeviceVideoFormatPropertiesKHR,
#endif // VK_ENABLE_BETA_EXTENSIONS

    // ---- VK_KHR_get_physical_device_properties2 extension commands
    .GetPhysicalDeviceFeatures2KHR = terminator_GetPhysicalDeviceFeatures2,
    .GetPhysicalDeviceProperties2KHR = terminator_GetPhysicalDeviceProperties2,
    .GetPhysicalDeviceFormatProperties2KHR = terminator_GetPhysicalDeviceFormatProperties2,
    .GetPhysicalDeviceImageFormatProperties2KHR = terminator_GetPhysicalDeviceImageFormatProperties2,
    .GetPhysicalDeviceQueueFamilyProperties2KHR = terminator_GetPhysicalDeviceQueueFamilyProperties2,
    .GetPhysicalDeviceMemoryProperties2KHR = terminator_GetPhysicalDeviceMemoryProperties2,
    .GetPhysicalDeviceSparseImageFormatProperties2KHR = terminator_GetPhysicalDeviceSparseImageFormatProperties2,

    // ---- VK_KHR_device_group_creation extension commands
    .EnumeratePhysicalDeviceGroupsKHR = terminator_EnumeratePhysicalDeviceGroups,

    // ---- VK_KHR_external_memory_capabilities extension commands
    .GetPhysicalDeviceExternalBufferPropertiesKHR = terminator_GetPhysicalDeviceExternalBufferProperties,

    // ---- VK_KHR_external_semaphore_capabilities extension commands
    .GetPhysicalDeviceExternalSemaphorePropertiesKHR = terminator_GetPhysicalDeviceExternalSemaphoreProperties,

    // ---- VK_KHR_external_fence_capabilities extension commands
    .GetPhysicalDeviceExternalFencePropertiesKHR = terminator_GetPhysicalDeviceExternalFenceProperties,

    // ---- VK_KHR_performance_query extension commands
    .EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR = terminator_EnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR,
    .GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR = terminator_GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR,

    // ---- VK_KHR_get_surface_capabilities2 extension commands
    .GetPhysicalDeviceSurfaceCapabilities2KHR = terminator_GetPhysicalDeviceSurfaceCapabilities2KHR,
    .GetPhysicalDeviceSurfaceFormats2KHR = terminator_GetPhysicalDeviceSurfaceFormats2KHR,

    // ---- VK_KHR_get_display_properties2 extension commands
    .GetPhysicalDeviceDisplayProperties2KHR = terminator_GetPhysicalDeviceDisplayProperties2KHR,
    .GetPhysicalDeviceDisplayPlaneProperties2KHR = terminator_GetPhysicalDeviceDisplayPlaneProperties2KHR,
    .GetDisplayModeProperties2KHR = terminator_GetDisplayModeProperties2KHR,
    .GetDisplayPlaneCapabilities2KHR = terminator_GetDisplayPlaneCapabilities2KHR,

    // ---- VK_KHR_fragment_shading_rate extension commands
    .GetPhysicalDeviceFragmentShadingRatesKHR = terminator_GetPhysicalDeviceFragmentShadingRatesKHR,

    // ---- VK_EXT_debug_report extension commands
    .CreateDebugReportCallbackEXT = terminator_CreateDebugReportCallbackEXT,
    .DestroyDebugReportCallbackEXT = terminator_DestroyDebugReportCallbackEXT,
    .DebugReportMessageEXT = terminator_DebugReportMessageEXT,

    // ---- VK_GGP_stream_descriptor_surface extension commands
#ifdef VK_USE_PLATFORM_GGP
    .CreateStreamDescriptorSurfaceGGP = terminator_CreateStreamDescriptorSurfaceGGP,
#endif // VK_USE_PLATFORM_GGP

    // ---- VK_NV_external_memory_capabilities extension commands
    .GetPhysicalDeviceExternalImageFormatPropertiesNV = terminator_GetPhysicalDeviceExternalImageFormatPropertiesNV,

    // ---- VK_NN_vi_surface extension commands
#ifdef VK_USE_PLATFORM_VI_NN
    .CreateViSurfaceNN = terminator_CreateViSurfaceNN,
#endif // VK_USE_PLATFORM_VI_NN

    // ---- VK_EXT_direct_mode_display extension commands
    .ReleaseDisplayEXT = terminator_ReleaseDisplayEXT,

    // ---- VK_EXT_acquire_xlib_display extension commands
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
    .AcquireXlibDisplayEXT = terminator_AcquireXlibDisplayEXT,
    .GetRandROutputDisplayEXT = terminator_GetRandROutputDisplayEXT,
#endif // VK_USE_PLATFORM_XLIB_XRANDR_EXT

    // ---- VK_EXT_display_surface_counter extension commands
    .GetPhysicalDeviceSurfaceCapabilities2EXT = terminator_GetPhysicalDeviceSurfaceCapabilities2EXT,

    // ---- VK_MVK_ios_surface extension commands
#ifdef VK_USE_PLATFORM_IOS_MVK
    .CreateIOSSurfaceMVK = terminator_CreateIOSSurfaceMVK,
#endif // VK_USE_PLATFORM_IOS_MVK

    // ---- VK_MVK_macos_surface extension commands
#ifdef VK_USE_PLATFORM_MACOS_MVK
    .CreateMacOSSurfaceMVK = terminator_CreateMacOSSurfaceMVK,
#endif // VK_USE_PLATFORM_MACOS_MVK

    // ---- VK_EXT_debug_utils extension commands
    .CreateDebugUtilsMessengerEXT = terminator_CreateDebugUtilsMessengerEXT,
    .DestroyDebugUtilsMessengerEXT = terminator_DestroyDebugUtilsMessengerEXT,
    .SubmitDebugUtilsMessageEXT = terminator_SubmitDebugUtilsMessageEXT,

    // ---- VK_EXT_sample_locations extension commands
    .GetPhysicalDeviceMultisamplePropertiesEXT = terminator_GetPhysicalDeviceMultisamplePropertiesEXT,

    // ---- VK_EXT_calibrated_timestamps extension commands
    .GetPhysicalDeviceCalibrateableTimeDomainsEXT = terminator_GetPhysicalDeviceCalibrateableTimeDomainsEXT,

    // ---- VK_FUCHSIA_imagepipe_surface extension commands
#ifdef VK_USE_PLATFORM_FUCHSIA
    .CreateImagePipeSurfaceFUCHSIA = terminator_CreateImagePipeSurfaceFUCHSIA,
#endif // VK_USE_PLATFORM_FUCHSIA

    // ---- VK_EXT_metal_surface extension commands
#ifdef VK_USE_PLATFORM_METAL_EXT
    .CreateMetalSurfaceEXT = terminator_CreateMetalSurfaceEXT,
#endif // VK_USE_PLATFORM_METAL_EXT

    // ---- VK_EXT_tooling_info extension commands
    .GetPhysicalDeviceToolPropertiesEXT = terminator_GetPhysicalDeviceToolProperties,

    // ---- VK_NV_cooperative_matrix extension commands
    .GetPhysicalDeviceCooperativeMatrixPropertiesNV = terminator_GetPhysicalDeviceCooperativeMatrixPropertiesNV,

    // ---- VK_NV_coverage_reduction_mode extension commands
    .GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV = terminator_GetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV,

    // ---- VK_EXT_full_screen_exclusive extension commands
#ifdef VK_USE_PLATFORM_WIN32_KHR
    .GetPhysicalDeviceSurfacePresentModes2EXT = terminator_GetPhysicalDeviceSurfacePresentModes2EXT,
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ---- VK_EXT_headless_surface extension commands
    .CreateHeadlessSurfaceEXT = terminator_CreateHeadlessSurfaceEXT,

    // ---- VK_EXT_acquire_drm_display extension commands
    .AcquireDrmDisplayEXT = terminator_AcquireDrmDisplayEXT,
    .GetDrmDisplayEXT = terminator_GetDrmDisplayEXT,

    // ---- VK_NV_acquire_winrt_display extension commands
#ifdef VK_USE_PLATFORM_WIN32_KHR
    .AcquireWinrtDisplayNV = terminator_AcquireWinrtDisplayNV,
    .GetWinrtDisplayNV = terminator_GetWinrtDisplayNV,
#endif // VK_USE_PLATFORM_WIN32_KHR

    // ---- VK_EXT_directfb_surface extension commands
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
    .CreateDirectFBSurfaceEXT = terminator_CreateDirectFBSurfaceEXT,
    .GetPhysicalDeviceDirectFBPresentationSupportEXT = terminator_GetPhysicalDeviceDirectFBPresentationSupportEXT,
#endif // VK_USE_PLATFORM_DIRECTFB_EXT

    // ---- VK_QNX_screen_surface extension commands
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    .CreateScreenSurfaceQNX = terminator_CreateScreenSurfaceQNX,
    .GetPhysicalDeviceScreenPresentationSupportQNX = terminator_GetPhysicalDeviceScreenPresentationSupportQNX,
#endif // VK_USE_PLATFORM_SCREEN_QNX
};

// clang-format on

