/*
 * Copyright (c) 2021-2022 The Khronos Group Inc.
 * Copyright (c) 2021-2022 Valve Corporation
 * Copyright (c) 2021-2022 LunarG, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and/or associated documentation files (the "Materials"), to
 * deal in the Materials without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Materials, and to permit persons to whom the Materials are
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice(s) and this permission notice shall be included in
 * all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR THE
 * USE OR OTHER DEALINGS IN THE MATERIALS.
 *
 * Author: Charles Giessen <charles@lunarg.com>
 */

#include "test_icd.h"

// export vk_icdGetInstanceProcAddr
#ifndef TEST_ICD_EXPORT_ICD_GIPA
#define TEST_ICD_EXPORT_ICD_GIPA 0
#endif

// export vk_icdNegotiateLoaderICDInterfaceVersion
#ifndef TEST_ICD_EXPORT_NEGOTIATE_INTERFACE_VERSION
#define TEST_ICD_EXPORT_NEGOTIATE_INTERFACE_VERSION 0
#endif

// export vk_icdGetPhysicalDeviceProcAddr
#ifndef TEST_ICD_EXPORT_ICD_GPDPA
#define TEST_ICD_EXPORT_ICD_GPDPA 0
#endif

// export vk_icdEnumerateAdapterPhysicalDevices
#ifndef TEST_ICD_EXPORT_ICD_ENUMERATE_ADAPTER_PHYSICAL_DEVICES
#define TEST_ICD_EXPORT_ICD_ENUMERATE_ADAPTER_PHYSICAL_DEVICES 0
#endif

TestICD icd;
extern "C" {
FRAMEWORK_EXPORT TestICD* get_test_icd_func() { return &icd; }
FRAMEWORK_EXPORT TestICD* reset_icd_func() {
    icd.~TestICD();
    return new (&icd) TestICD();
}
}

LayerDefinition& FindLayer(std::vector<LayerDefinition>& layers, std::string layerName) {
    for (auto& layer : layers) {
        if (layer.layerName == layerName) return layer;
    }
    assert(false && "Layer name not found!");
    return layers[0];
}
bool CheckLayer(std::vector<LayerDefinition>& layers, std::string layerName) {
    for (auto& layer : layers) {
        if (layer.layerName == layerName) return true;
    }
    return false;
}

bool IsInstanceExtensionSupported(const char* extension_name) {
    return icd.instance_extensions.end() !=
           std::find_if(icd.instance_extensions.begin(), icd.instance_extensions.end(),
                        [extension_name](Extension const& ext) { return string_eq(&ext.extensionName[0], extension_name); });
}

bool IsInstanceExtensionEnabled(const char* extension_name) {
    return icd.enabled_instance_extensions.end() !=
           std::find_if(icd.enabled_instance_extensions.begin(), icd.enabled_instance_extensions.end(),
                        [extension_name](Extension const& ext) { return string_eq(&ext.extensionName[0], extension_name); });
}

bool IsPhysicalDeviceExtensionAvailable(const char* extension_name) {
    for (auto& phys_dev : icd.physical_devices) {
        if (phys_dev.extensions.end() !=
            std::find_if(phys_dev.extensions.begin(), phys_dev.extensions.end(),
                         [extension_name](Extension const& ext) { return ext.extensionName == extension_name; })) {
            return true;
        }
    }
    return false;
}

// typename T must have '.get()' function that returns a type U
template <typename T, typename U>
VkResult FillCountPtr(std::vector<T> const& data_vec, uint32_t* pCount, U* pData) {
    if (pCount == nullptr) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    if (pData == nullptr) {
        *pCount = static_cast<uint32_t>(data_vec.size());
        return VK_SUCCESS;
    }
    uint32_t amount_written = 0;
    uint32_t amount_to_write = static_cast<uint32_t>(data_vec.size());
    if (*pCount < data_vec.size()) {
        amount_to_write = *pCount;
    }
    for (size_t i = 0; i < amount_to_write; i++) {
        pData[i] = data_vec[i].get();
        amount_written++;
    }
    if (*pCount < data_vec.size()) {
        *pCount = amount_written;
        return VK_INCOMPLETE;
    }
    *pCount = amount_written;
    return VK_SUCCESS;
}

template <typename T>
VkResult FillCountPtr(std::vector<T> const& data_vec, uint32_t* pCount, T* pData) {
    if (pCount == nullptr) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    if (pData == nullptr) {
        *pCount = static_cast<uint32_t>(data_vec.size());
        return VK_SUCCESS;
    }
    uint32_t amount_written = 0;
    uint32_t amount_to_write = static_cast<uint32_t>(data_vec.size());
    if (*pCount < data_vec.size()) {
        amount_to_write = *pCount;
    }
    for (size_t i = 0; i < amount_to_write; i++) {
        pData[i] = data_vec[i];
        amount_written++;
    }
    if (*pCount < data_vec.size()) {
        *pCount = amount_written;
        return VK_INCOMPLETE;
    }
    *pCount = amount_written;
    return VK_SUCCESS;
}

//// Instance Functions ////

// VK_SUCCESS,VK_INCOMPLETE
VKAPI_ATTR VkResult VKAPI_CALL test_vkEnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount,
                                                                           VkExtensionProperties* pProperties) {
    if (pLayerName != nullptr) {
        auto& layer = FindLayer(icd.instance_layers, std::string(pLayerName));
        return FillCountPtr(layer.extensions, pPropertyCount, pProperties);
    } else {  // instance extensions
        FillCountPtr(icd.instance_extensions, pPropertyCount, pProperties);
    }

    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkEnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties) {
    return FillCountPtr(icd.instance_layers, pPropertyCount, pProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkEnumerateInstanceVersion(uint32_t* pApiVersion) {
    if (pApiVersion != nullptr) {
        *pApiVersion = VK_API_VERSION_1_0;
    }
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo,
                                                     const VkAllocationCallbacks* pAllocator, VkInstance* pInstance) {
    if (pCreateInfo == nullptr || pCreateInfo->pApplicationInfo == nullptr) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    if (icd.icd_api_version < VK_API_VERSION_1_1) {
        if (pCreateInfo->pApplicationInfo->apiVersion > VK_API_VERSION_1_0) {
            return VK_ERROR_INCOMPATIBLE_DRIVER;
        }
    }

    // Add to the list of enabled extensions only those that the ICD actively supports
    for (uint32_t iii = 0; iii < pCreateInfo->enabledExtensionCount; ++iii) {
        if (IsInstanceExtensionSupported(pCreateInfo->ppEnabledExtensionNames[iii])) {
            icd.add_enabled_instance_extension({pCreateInfo->ppEnabledExtensionNames[iii]});
        }
    }

    // VK_SUCCESS
    *pInstance = icd.instance_handle.handle;

    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL test_vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator) {}

// VK_SUCCESS,VK_INCOMPLETE
VKAPI_ATTR VkResult VKAPI_CALL test_vkEnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount,
                                                               VkPhysicalDevice* pPhysicalDevices) {
    if (pPhysicalDevices == nullptr) {
        *pPhysicalDeviceCount = static_cast<uint32_t>(icd.physical_devices.size());
    } else {
        uint32_t handles_written = 0;
        for (size_t i = 0; i < icd.physical_devices.size(); i++) {
            if (i < *pPhysicalDeviceCount) {
                handles_written++;
                pPhysicalDevices[i] = icd.physical_devices[i].vk_physical_device.handle;
            } else {
                *pPhysicalDeviceCount = handles_written;
                return VK_INCOMPLETE;
            }
        }
        *pPhysicalDeviceCount = handles_written;
    }
    return VK_SUCCESS;
}

// VK_SUCCESS,VK_INCOMPLETE, VK_ERROR_INITIALIZATION_FAILED
VKAPI_ATTR VkResult VKAPI_CALL test_vkEnumeratePhysicalDeviceGroups(
    VkInstance instance, uint32_t* pPhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties) {
    VkResult result = VK_SUCCESS;

    if (pPhysicalDeviceGroupProperties == nullptr) {
        if (0 == icd.physical_device_groups.size()) {
            *pPhysicalDeviceGroupCount = static_cast<uint32_t>(icd.physical_devices.size());
        } else {
            *pPhysicalDeviceGroupCount = static_cast<uint32_t>(icd.physical_device_groups.size());
        }
    } else {
        // NOTE: This is a fake struct to make sure the pNext chain is properly passed down to the ICD
        //       vkEnumeratePhysicalDeviceGroups.
        //       The two versions must match:
        //           "FakePNext" test in loader_regresion_tests.cpp
        //           "test_vkEnumeratePhysicalDeviceGroups" in test_icd.cpp
        struct FakePnextSharedWithICD {
            VkStructureType sType;
            void* pNext;
            uint32_t value;
        };

        uint32_t group_count = 0;
        if (0 == icd.physical_device_groups.size()) {
            group_count = static_cast<uint32_t>(icd.physical_devices.size());
            for (size_t device_group = 0; device_group < icd.physical_devices.size(); device_group++) {
                if (device_group >= *pPhysicalDeviceGroupCount) {
                    group_count = *pPhysicalDeviceGroupCount;
                    result = VK_INCOMPLETE;
                    break;
                }
                pPhysicalDeviceGroupProperties[device_group].subsetAllocation = false;
                pPhysicalDeviceGroupProperties[device_group].physicalDeviceCount = 1;
                pPhysicalDeviceGroupProperties[device_group].physicalDevices[0] =
                    icd.physical_devices[device_group].vk_physical_device.handle;
            }
        } else {
            group_count = static_cast<uint32_t>(icd.physical_device_groups.size());
            for (size_t device_group = 0; device_group < icd.physical_device_groups.size(); device_group++) {
                if (device_group >= *pPhysicalDeviceGroupCount) {
                    group_count = *pPhysicalDeviceGroupCount;
                    result = VK_INCOMPLETE;
                    break;
                }
                pPhysicalDeviceGroupProperties[device_group].subsetAllocation =
                    icd.physical_device_groups[device_group].subset_allocation;
                uint32_t handles_written = 0;
                for (size_t i = 0; i < icd.physical_device_groups[device_group].physical_device_handles.size(); i++) {
                    handles_written++;
                    pPhysicalDeviceGroupProperties[device_group].physicalDevices[i] =
                        icd.physical_device_groups[device_group].physical_device_handles[i]->vk_physical_device.handle;
                }
                pPhysicalDeviceGroupProperties[device_group].physicalDeviceCount = handles_written;
            }
        }
        // NOTE: The following code is purely to test pNext passing in vkEnumeratePhysicalDevice groups
        //       and includes normally invalid information.
        for (size_t device_group = 0; device_group < group_count; device_group++) {
            if (nullptr != pPhysicalDeviceGroupProperties[device_group].pNext) {
                VkBaseInStructure* base = reinterpret_cast<VkBaseInStructure*>(pPhysicalDeviceGroupProperties[device_group].pNext);
                if (base->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTI_DRAW_PROPERTIES_EXT) {
                    FakePnextSharedWithICD* fake = reinterpret_cast<FakePnextSharedWithICD*>(base);
                    fake->value = 0xDECAFBAD;
                }
            }
        }
        *pPhysicalDeviceGroupCount = static_cast<uint32_t>(group_count);
    }
    return result;
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateDebugUtilsMessengerEXT(VkInstance instance,
                                                                   const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                                   const VkAllocationCallbacks* pAllocator,
                                                                   VkDebugUtilsMessengerEXT* pMessenger) {
    if (nullptr != pMessenger) {
        uint64_t fake_msgr_handle = reinterpret_cast<uint64_t>(new uint8_t);
        icd.messenger_handles.push_back(fake_msgr_handle);
#if defined(__LP64__) || defined(_WIN64) || (defined(__x86_64__) && !defined(__ILP32__)) || defined(_M_X64) || defined(__ia64) || \
    defined(_M_IA64) || defined(__aarch64__) || defined(__powerpc64__)
        *pMessenger = reinterpret_cast<VkDebugUtilsMessengerEXT>(fake_msgr_handle);
#else
        *pMessenger = fake_msgr_handle;
#endif
    }
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL test_vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger,
                                                                const VkAllocationCallbacks* pAllocator) {
    if (messenger != VK_NULL_HANDLE) {
        uint64_t fake_msgr_handle = (uint64_t)(messenger);
        auto found_iter = std::find(icd.messenger_handles.begin(), icd.messenger_handles.end(), fake_msgr_handle);
        if (found_iter != icd.messenger_handles.end()) {
            // Remove it from the list
            icd.messenger_handles.erase(found_iter);
            // Delete the handle
            delete (uint8_t*)fake_msgr_handle;
        } else {
            assert(false && "Messenger not found during destroy!");
        }
    }
}

//// Physical Device functions ////

// VK_SUCCESS,VK_INCOMPLETE
VKAPI_ATTR VkResult VKAPI_CALL test_vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount,
                                                                     VkLayerProperties* pProperties) {
    assert(false && "ICD's don't contain layers???");
    return VK_SUCCESS;
}

// VK_SUCCESS, VK_INCOMPLETE, VK_ERROR_LAYER_NOT_PRESENT
VKAPI_ATTR VkResult VKAPI_CALL test_vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char* pLayerName,
                                                                         uint32_t* pPropertyCount,
                                                                         VkExtensionProperties* pProperties) {
    auto& phys_dev = icd.GetPhysDevice(physicalDevice);
    if (pLayerName != nullptr) {
        assert(false && "Drivers don't contain layers???");
        return VK_SUCCESS;
    } else {  // instance extensions
        return FillCountPtr(phys_dev.extensions, pPropertyCount, pProperties);
    }
}

VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice,
                                                                         uint32_t* pQueueFamilyPropertyCount,
                                                                         VkQueueFamilyProperties* pQueueFamilyProperties) {
    auto& phys_dev = icd.GetPhysDevice(physicalDevice);
    FillCountPtr(phys_dev.queue_family_properties, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo,
                                                   const VkAllocationCallbacks* pAllocator, VkDevice* pDevice) {
    // VK_SUCCESS
    auto found = std::find_if(icd.physical_devices.begin(), icd.physical_devices.end(), [physicalDevice](PhysicalDevice& phys_dev) {
        return phys_dev.vk_physical_device.handle == physicalDevice;
    });
    if (found == icd.physical_devices.end()) return VK_ERROR_INITIALIZATION_FAILED;
    auto device_handle = DispatchableHandle<VkDevice>();
    *pDevice = device_handle.handle;
    found->device_handles.push_back(device_handle.handle);
    icd.device_handles.emplace_back(std::move(device_handle));

    for (uint32_t i = 0; i < pCreateInfo->queueCreateInfoCount; i++) {
        found->queue_handles.emplace_back();
    }

    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL test_vkDestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator) {
    auto found = std::find(icd.device_handles.begin(), icd.device_handles.end(), device);
    if (found != icd.device_handles.end()) icd.device_handles.erase(found);
}

VKAPI_ATTR VkResult VKAPI_CALL generic_tool_props_function(VkPhysicalDevice physicalDevice, uint32_t* pToolCount,
                                                           VkPhysicalDeviceToolPropertiesEXT* pToolProperties) {
    if (icd.tooling_properties.size() == 0) {
        return VK_SUCCESS;
    }
    if (pToolProperties == nullptr && pToolCount != nullptr) {
        *pToolCount = static_cast<uint32_t>(icd.tooling_properties.size());
    } else if (pToolCount != nullptr) {
        for (size_t i = 0; i < *pToolCount; i++) {
            if (i >= icd.tooling_properties.size()) {
                return VK_INCOMPLETE;
            }
            pToolProperties[i] = icd.tooling_properties[i];
        }
    }
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkGetPhysicalDeviceToolPropertiesEXT(VkPhysicalDevice physicalDevice, uint32_t* pToolCount,
                                                                         VkPhysicalDeviceToolPropertiesEXT* pToolProperties) {
    return generic_tool_props_function(physicalDevice, pToolCount, pToolProperties);
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkGetPhysicalDeviceToolProperties(VkPhysicalDevice physicalDevice, uint32_t* pToolCount,
                                                                      VkPhysicalDeviceToolPropertiesEXT* pToolProperties) {
    return generic_tool_props_function(physicalDevice, pToolCount, pToolProperties);
}

template <typename T>
T to_nondispatch_handle(uint64_t handle) {
#if defined(__LP64__) || defined(_WIN64) || (defined(__x86_64__) && !defined(__ILP32__)) || defined(_M_X64) || defined(__ia64) || \
    defined(_M_IA64) || defined(__aarch64__) || defined(__powerpc64__)
    return reinterpret_cast<T>(handle);
#else
    return handle;
#endif
}

template <typename T>
uint64_t from_nondispatch_handle(T handle) {
#if defined(__LP64__) || defined(_WIN64) || (defined(__x86_64__) && !defined(__ILP32__)) || defined(_M_X64) || defined(__ia64) || \
    defined(_M_IA64) || defined(__aarch64__) || defined(__powerpc64__)
    return reinterpret_cast<uint64_t>(handle);
#else
    return handle;
#endif
}

//// WSI ////
template <typename HandleType>
void common_nondispatch_handle_creation(std::vector<uint64_t>& handles, HandleType* pHandle) {
    if (nullptr != pHandle) {
        if (handles.size() == 0)
            handles.push_back(800851234);
        else
            handles.push_back(handles.back() + 102030);
        *pHandle = to_nondispatch_handle<HandleType>(handles.back());
    }
}
#ifdef VK_USE_PLATFORM_ANDROID_KHR

VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateAndroidSurfaceKHR(VkInstance instance, const VkAndroidSurfaceCreateInfoKHR* pCreateInfo,
                                                              const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    common_nondispatch_handle_creation(icd.surface_handles, pSurface);
    return VK_SUCCESS;
}
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateWin32SurfaceKHR(VkInstance instance, const VkWin32SurfaceCreateInfoKHR* pCreateInfo,
                                                            const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    common_nondispatch_handle_creation(icd.surface_handles, pSurface);
    return VK_SUCCESS;
}

VKAPI_ATTR VkBool32 VKAPI_CALL test_vkGetPhysicalDeviceWin32PresentationSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                   uint32_t queueFamilyIndex) {
    return VK_TRUE;
}
#endif  // VK_USE_PLATFORM_WIN32_KHR

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateWaylandSurfaceKHR(VkInstance instance, const VkWaylandSurfaceCreateInfoKHR* pCreateInfo,
                                                              const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    common_nondispatch_handle_creation(icd.surface_handles, pSurface);
    return VK_SUCCESS;
}

VKAPI_ATTR VkBool32 VKAPI_CALL test_vkGetPhysicalDeviceWaylandPresentationSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                     uint32_t queueFamilyIndex,
                                                                                     struct wl_display* display) {
    return VK_TRUE;
}
#endif  // VK_USE_PLATFORM_WAYLAND_KHR
#ifdef VK_USE_PLATFORM_XCB_KHR
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateXcbSurfaceKHR(VkInstance instance, const VkXcbSurfaceCreateInfoKHR* pCreateInfo,
                                                          const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    common_nondispatch_handle_creation(icd.surface_handles, pSurface);
    return VK_SUCCESS;
}

VKAPI_ATTR VkBool32 VKAPI_CALL test_vkGetPhysicalDeviceXcbPresentationSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                 uint32_t queueFamilyIndex,
                                                                                 xcb_connection_t* connection,
                                                                                 xcb_visualid_t visual_id) {
    return VK_TRUE;
}
#endif  // VK_USE_PLATFORM_XCB_KHR

#ifdef VK_USE_PLATFORM_XLIB_KHR
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateXlibSurfaceKHR(VkInstance instance, const VkXlibSurfaceCreateInfoKHR* pCreateInfo,
                                                           const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    common_nondispatch_handle_creation(icd.surface_handles, pSurface);
    return VK_SUCCESS;
}

VKAPI_ATTR VkBool32 VKAPI_CALL test_vkGetPhysicalDeviceXlibPresentationSupportKHR(VkPhysicalDevice physicalDevice,
                                                                                  uint32_t queueFamilyIndex, Display* dpy,
                                                                                  VisualID visualID) {
    return VK_TRUE;
}
#endif  // VK_USE_PLATFORM_XLIB_KHR

#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateDirectFBSurfaceEXT(VkInstance instance,
                                                               const VkDirectFBSurfaceCreateInfoEXT* pCreateInfo,
                                                               const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    common_nondispatch_handle_creation(icd.surface_handles, pSurface);
    return VK_SUCCESS;
}

VKAPI_ATTR VkBool32 VKAPI_CALL test_vkGetPhysicalDeviceDirectFBPresentationSupportEXT(VkPhysicalDevice physicalDevice,
                                                                                      uint32_t queueFamilyIndex, IDirectFB* dfb) {
    return VK_TRUE;
}

#endif  // VK_USE_PLATFORM_DIRECTFB_EXT

#ifdef VK_USE_PLATFORM_MACOS_MVK
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateMacOSSurfaceMVK(VkInstance instance, const VkMacOSSurfaceCreateInfoMVK* pCreateInfo,
                                                            const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    common_nondispatch_handle_creation(icd.surface_handles, pSurface);
    return VK_SUCCESS;
}
#endif  // VK_USE_PLATFORM_MACOS_MVK

#ifdef VK_USE_PLATFORM_IOS_MVK
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateIOSSurfaceMVK(VkInstance instance, const VkIOSSurfaceCreateInfoMVK* pCreateInfo,
                                                          const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    common_nondispatch_handle_creation(icd.surface_handles, pSurface);
    return VK_SUCCESS;
}
#endif  // VK_USE_PLATFORM_IOS_MVK

#ifdef VK_USE_PLATFORM_GGP
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateStreamDescriptorSurfaceGGP(VkInstance instance,
                                                                       const VkStreamDescriptorSurfaceCreateInfoGGP* pCreateInfo,
                                                                       const VkAllocationCallbacks* pAllocator,
                                                                       VkSurfaceKHR* pSurface) {
    common_nondispatch_handle_creation(icd.surface_handles, pSurface);
    return VK_SUCCESS;
}
#endif  // VK_USE_PLATFORM_GGP

#if defined(VK_USE_PLATFORM_METAL_EXT)
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateMetalSurfaceEXT(VkInstance instance, const VkMetalSurfaceCreateInfoEXT* pCreateInfo,
                                                            const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    common_nondispatch_handle_creation(icd.surface_handles, pSurface);
    return VK_SUCCESS;
}
#endif  // VK_USE_PLATFORM_METAL_EXT

#ifdef VK_USE_PLATFORM_SCREEN_QNX
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateScreenSurfaceQNX(VkInstance instance, const VkScreenSurfaceCreateInfoQNX* pCreateInfo,
                                                             const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    common_nondispatch_handle_creation(icd.surface_handles, pSurface);
    return VK_SUCCESS;
}

VKAPI_ATTR VkBool32 VKAPI_CALL test_vkGetPhysicalDeviceScreenPresentationSupportQNX(VkPhysicalDevice physicalDevice,
                                                                                    uint32_t queueFamilyIndex,
                                                                                    struct _screen_window* window) {
    return VK_TRUE;
}
#endif  // VK_USE_PLATFORM_SCREEN_QNX

VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateHeadlessSurfaceEXT(VkInstance instance,
                                                               const VkHeadlessSurfaceCreateInfoEXT* pCreateInfo,
                                                               const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
    common_nondispatch_handle_creation(icd.surface_handles, pSurface);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL test_vkDestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface,
                                                    const VkAllocationCallbacks* pAllocator) {
    if (surface != VK_NULL_HANDLE) {
        uint64_t fake_surf_handle = from_nondispatch_handle(surface);
        auto found_iter = std::find(icd.surface_handles.begin(), icd.surface_handles.end(), fake_surf_handle);
        if (found_iter != icd.surface_handles.end()) {
            // Remove it from the list
            icd.surface_handles.erase(found_iter);
        } else {
            assert(false && "Surface not found during destroy!");
        }
    }
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo,
                                                         const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain) {
    common_nondispatch_handle_creation(icd.swapchain_handles, pSwapchain);
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL test_vkDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain,
                                                      const VkAllocationCallbacks* pAllocator) {
    if (swapchain != VK_NULL_HANDLE) {
        uint64_t fake_swapchain_handle = from_nondispatch_handle(swapchain);
        auto found_iter = icd.swapchain_handles.erase(
            std::remove(icd.swapchain_handles.begin(), icd.swapchain_handles.end(), fake_swapchain_handle),
            icd.swapchain_handles.end());
        if (found_iter == icd.swapchain_handles.end()) {
            assert(false && "Swapchain not found during destroy!");
        }
    }
}

// VK_KHR_surface
VKAPI_ATTR VkResult VKAPI_CALL test_vkGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex,
                                                                         VkSurfaceKHR surface, VkBool32* pSupported) {
    if (surface != VK_NULL_HANDLE) {
        uint64_t fake_surf_handle = (uint64_t)(surface);
        auto found_iter = std::find(icd.surface_handles.begin(), icd.surface_handles.end(), fake_surf_handle);
        if (found_iter == icd.surface_handles.end()) {
            assert(false && "Surface not found during GetPhysicalDeviceSurfaceSupportKHR query!");
            return VK_ERROR_UNKNOWN;
        }
    }
    if (nullptr != pSupported) {
        *pSupported = icd.GetPhysDevice(physicalDevice).queue_family_properties.at(queueFamilyIndex).support_present;
    }
    return VK_SUCCESS;
}
VKAPI_ATTR VkResult VKAPI_CALL test_vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                                                              VkSurfaceCapabilitiesKHR* pSurfaceCapabilities) {
    if (surface != VK_NULL_HANDLE) {
        uint64_t fake_surf_handle = (uint64_t)(surface);
        auto found_iter = std::find(icd.surface_handles.begin(), icd.surface_handles.end(), fake_surf_handle);
        if (found_iter == icd.surface_handles.end()) {
            assert(false && "Surface not found during GetPhysicalDeviceSurfaceCapabilitiesKHR query!");
            return VK_ERROR_UNKNOWN;
        }
    }
    if (nullptr != pSurfaceCapabilities) {
        *pSurfaceCapabilities = icd.GetPhysDevice(physicalDevice).surface_capabilities;
    }
    return VK_SUCCESS;
}
VKAPI_ATTR VkResult VKAPI_CALL test_vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                                                         uint32_t* pSurfaceFormatCount,
                                                                         VkSurfaceFormatKHR* pSurfaceFormats) {
    if (surface != VK_NULL_HANDLE) {
        uint64_t fake_surf_handle = (uint64_t)(surface);
        auto found_iter = std::find(icd.surface_handles.begin(), icd.surface_handles.end(), fake_surf_handle);
        if (found_iter == icd.surface_handles.end()) {
            assert(false && "Surface not found during GetPhysicalDeviceSurfaceFormatsKHR query!");
            return VK_ERROR_UNKNOWN;
        }
    }
    FillCountPtr(icd.GetPhysDevice(physicalDevice).surface_formats, pSurfaceFormatCount, pSurfaceFormats);
    return VK_SUCCESS;
}
VKAPI_ATTR VkResult VKAPI_CALL test_vkGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                                                              uint32_t* pPresentModeCount,
                                                                              VkPresentModeKHR* pPresentModes) {
    if (surface != VK_NULL_HANDLE) {
        uint64_t fake_surf_handle = (uint64_t)(surface);
        auto found_iter = std::find(icd.surface_handles.begin(), icd.surface_handles.end(), fake_surf_handle);
        if (found_iter == icd.surface_handles.end()) {
            assert(false && "Surface not found during GetPhysicalDeviceSurfacePresentModesKHR query!");
            return VK_ERROR_UNKNOWN;
        }
    }
    FillCountPtr(icd.GetPhysDevice(physicalDevice).surface_present_modes, pPresentModeCount, pPresentModes);
    return VK_SUCCESS;
}

// VK_KHR_display
VKAPI_ATTR VkResult VKAPI_CALL test_vkGetPhysicalDeviceDisplayPropertiesKHR(VkPhysicalDevice physicalDevice,
                                                                            uint32_t* pPropertyCount,
                                                                            VkDisplayPropertiesKHR* pProperties) {
    FillCountPtr(icd.GetPhysDevice(physicalDevice).display_properties, pPropertyCount, pProperties);
    return VK_SUCCESS;
}
VKAPI_ATTR VkResult VKAPI_CALL test_vkGetPhysicalDeviceDisplayPlanePropertiesKHR(VkPhysicalDevice physicalDevice,
                                                                                 uint32_t* pPropertyCount,
                                                                                 VkDisplayPlanePropertiesKHR* pProperties) {
    FillCountPtr(icd.GetPhysDevice(physicalDevice).display_plane_properties, pPropertyCount, pProperties);
    return VK_SUCCESS;
}
VKAPI_ATTR VkResult VKAPI_CALL test_vkGetDisplayPlaneSupportedDisplaysKHR(VkPhysicalDevice physicalDevice, uint32_t planeIndex,
                                                                          uint32_t* pDisplayCount, VkDisplayKHR* pDisplays) {
    FillCountPtr(icd.GetPhysDevice(physicalDevice).displays, pDisplayCount, pDisplays);
    return VK_SUCCESS;
}
VKAPI_ATTR VkResult VKAPI_CALL test_vkGetDisplayModePropertiesKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display,
                                                                  uint32_t* pPropertyCount,
                                                                  VkDisplayModePropertiesKHR* pProperties) {
    FillCountPtr(icd.GetPhysDevice(physicalDevice).display_mode_properties, pPropertyCount, pProperties);
    return VK_SUCCESS;
}
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateDisplayModeKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display,
                                                           const VkDisplayModeCreateInfoKHR* pCreateInfo,
                                                           const VkAllocationCallbacks* pAllocator, VkDisplayModeKHR* pMode) {
    if (nullptr != pMode) {
        *pMode = icd.GetPhysDevice(physicalDevice).display_mode;
    }
    return VK_SUCCESS;
}
VKAPI_ATTR VkResult VKAPI_CALL test_vkGetDisplayPlaneCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkDisplayModeKHR mode,
                                                                     uint32_t planeIndex,
                                                                     VkDisplayPlaneCapabilitiesKHR* pCapabilities) {
    if (nullptr != pCapabilities) {
        *pCapabilities = icd.GetPhysDevice(physicalDevice).display_plane_capabilities;
    }
    return VK_SUCCESS;
}
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateDisplayPlaneSurfaceKHR(VkInstance instance,
                                                                   const VkDisplaySurfaceCreateInfoKHR* pCreateInfo,
                                                                   const VkAllocationCallbacks* pAllocator,
                                                                   VkSurfaceKHR* pSurface) {
    if (nullptr != pSurface) {
        uint64_t fake_surf_handle = reinterpret_cast<uint64_t>(new uint8_t);
        icd.surface_handles.push_back(fake_surf_handle);
#if defined(__LP64__) || defined(_WIN64) || (defined(__x86_64__) && !defined(__ILP32__)) || defined(_M_X64) || defined(__ia64) || \
    defined(_M_IA64) || defined(__aarch64__) || defined(__powerpc64__)
        *pSurface = reinterpret_cast<VkSurfaceKHR>(fake_surf_handle);
#else
        *pSurface = fake_surf_handle;
#endif
    }
    return VK_SUCCESS;
}

// VK_KHR_get_surface_capabilities2
VKAPI_ATTR VkResult VKAPI_CALL test_vkGetPhysicalDeviceSurfaceCapabilities2KHR(VkPhysicalDevice physicalDevice,
                                                                               const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
                                                                               VkSurfaceCapabilities2KHR* pSurfaceCapabilities) {
    if (nullptr != pSurfaceInfo && nullptr != pSurfaceCapabilities) {
        return test_vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, pSurfaceInfo->surface,
                                                              &pSurfaceCapabilities->surfaceCapabilities);
    }
    return VK_SUCCESS;
}
VKAPI_ATTR VkResult VKAPI_CALL test_vkGetPhysicalDeviceSurfaceFormats2KHR(VkPhysicalDevice physicalDevice,
                                                                          const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo,
                                                                          uint32_t* pSurfaceFormatCount,
                                                                          VkSurfaceFormat2KHR* pSurfaceFormats) {
    if (nullptr != pSurfaceFormatCount) {
        test_vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, pSurfaceInfo->surface, pSurfaceFormatCount, nullptr);
        if (nullptr != pSurfaceFormats) {
            auto& phys_dev = icd.GetPhysDevice(physicalDevice);
            // Since the structures are different, we have to copy each item over seperately.  Since we have multiple, we
            // have to manually copy the data here instead of calling the next function
            for (uint32_t cnt = 0; cnt < *pSurfaceFormatCount; ++cnt) {
                memcpy(&pSurfaceFormats[cnt].surfaceFormat, &phys_dev.surface_formats[cnt], sizeof(VkSurfaceFormatKHR));
            }
        }
    }
    return VK_SUCCESS;
}

//// misc
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo,
                                                        const VkAllocationCallbacks* pAllocator, VkCommandPool* pCommandPool) {
    if (pCommandPool != nullptr) {
        pCommandPool = reinterpret_cast<VkCommandPool*>(0xdeadbeefdeadbeef);
    }
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL test_vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool,
                                                     const VkAllocationCallbacks* pAllocator) {
    // do nothing, leak memory for now
}
VKAPI_ATTR VkResult VKAPI_CALL test_vkAllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo* pAllocateInfo,
                                                             VkCommandBuffer* pCommandBuffers) {
    if (pAllocateInfo != nullptr && pCommandBuffers != nullptr) {
        for (size_t i = 0; i < pAllocateInfo->commandBufferCount; i++) {
            icd.allocated_command_buffers.push_back({});
            pCommandBuffers[i] = icd.allocated_command_buffers.back().handle;
        }
    }
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL test_vkGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue* pQueue) {
    *pQueue = icd.physical_devices.back().queue_handles[queueIndex].handle;
}

// VK_EXT_acquire_drm_display
VKAPI_ATTR VkResult VKAPI_CALL test_vkAcquireDrmDisplayEXT(VkPhysicalDevice physicalDevice, int32_t drmFd, VkDisplayKHR display) {
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkGetDrmDisplayEXT(VkPhysicalDevice physicalDevice, int32_t drmFd, uint32_t connectorId,
                                                       VkDisplayKHR* display) {
    if (nullptr != display && icd.GetPhysDevice(physicalDevice).displays.size() > 0) {
        *display = icd.GetPhysDevice(physicalDevice).displays[0];
    }
    return VK_SUCCESS;
}

//// stubs
// 1.0
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* pFeatures) {
    if (nullptr != pFeatures) {
        memcpy(pFeatures, &icd.GetPhysDevice(physicalDevice).features, sizeof(VkPhysicalDeviceFeatures));
    }
}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice,
                                                              VkPhysicalDeviceProperties* pProperties) {
    if (nullptr != pProperties) {
        auto& phys_dev = icd.GetPhysDevice(physicalDevice);
        memcpy(pProperties, &phys_dev.properties, sizeof(VkPhysicalDeviceProperties));
        size_t max_len = (phys_dev.deviceName.length() > VK_MAX_PHYSICAL_DEVICE_NAME_SIZE) ? VK_MAX_PHYSICAL_DEVICE_NAME_SIZE
                                                                                           : phys_dev.deviceName.length();
        std::copy(phys_dev.deviceName.c_str(), phys_dev.deviceName.c_str() + max_len, pProperties->deviceName);
        pProperties->deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE - 1] = '\0';
    }
}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice,
                                                                    VkPhysicalDeviceMemoryProperties* pMemoryProperties) {
    if (nullptr != pMemoryProperties) {
        memcpy(pMemoryProperties, &icd.GetPhysDevice(physicalDevice).memory_properties, sizeof(VkPhysicalDeviceMemoryProperties));
    }
}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceSparseImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format,
                                                                               VkImageType type, VkSampleCountFlagBits samples,
                                                                               VkImageUsageFlags usage, VkImageTiling tiling,
                                                                               uint32_t* pPropertyCount,
                                                                               VkSparseImageFormatProperties* pProperties) {
    FillCountPtr(icd.GetPhysDevice(physicalDevice).sparse_image_format_properties, pPropertyCount, pProperties);
}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format,
                                                                    VkFormatProperties* pFormatProperties) {
    if (nullptr != pFormatProperties) {
        memcpy(pFormatProperties, &icd.GetPhysDevice(physicalDevice).format_properties[static_cast<uint32_t>(format)],
               sizeof(VkFormatProperties));
    }
}
VKAPI_ATTR VkResult VKAPI_CALL test_vkGetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format,
                                                                             VkImageType type, VkImageTiling tiling,
                                                                             VkImageUsageFlags usage, VkImageCreateFlags flags,
                                                                             VkImageFormatProperties* pImageFormatProperties) {
    if (nullptr != pImageFormatProperties) {
        memcpy(pImageFormatProperties, &icd.GetPhysDevice(physicalDevice).image_format_properties, sizeof(VkImageFormatProperties));
    }
    return VK_SUCCESS;
}

// 1.1
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceFeatures2(VkPhysicalDevice physicalDevice,
                                                             VkPhysicalDeviceFeatures2* pFeatures) {
    if (nullptr != pFeatures) {
        test_vkGetPhysicalDeviceFeatures(physicalDevice, &pFeatures->features);
    }
}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceProperties2(VkPhysicalDevice physicalDevice,
                                                               VkPhysicalDeviceProperties2* pProperties) {
    if (nullptr != pProperties) {
        auto& phys_dev = icd.GetPhysDevice(physicalDevice);
        test_vkGetPhysicalDeviceProperties(physicalDevice, &pProperties->properties);
        VkBaseInStructure* pNext = reinterpret_cast<VkBaseInStructure*>(pProperties->pNext);
        while (pNext) {
            if (pNext->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT) {
                VkPhysicalDevicePCIBusInfoPropertiesEXT* bus_info =
                    reinterpret_cast<VkPhysicalDevicePCIBusInfoPropertiesEXT*>(pNext);
                bus_info->pciBus = phys_dev.pci_bus;
            }
            pNext = reinterpret_cast<VkBaseInStructure*>(const_cast<VkBaseInStructure*>(pNext->pNext));
        }
    }
}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceMemoryProperties2(VkPhysicalDevice physicalDevice,
                                                                     VkPhysicalDeviceMemoryProperties2* pMemoryProperties) {
    if (nullptr != pMemoryProperties) {
        test_vkGetPhysicalDeviceMemoryProperties(physicalDevice, &pMemoryProperties->memoryProperties);
    }
}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceQueueFamilyProperties2(VkPhysicalDevice physicalDevice,
                                                                          uint32_t* pQueueFamilyPropertyCount,
                                                                          VkQueueFamilyProperties2* pQueueFamilyProperties) {
    if (nullptr != pQueueFamilyPropertyCount) {
        test_vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, nullptr);
        if (nullptr != pQueueFamilyProperties) {
            auto& phys_dev = icd.GetPhysDevice(physicalDevice);
            // Since the structures are different, we have to copy each item over seperately.  Since we have multiple, we
            // have to manually copy the data here instead of calling the next function
            for (uint32_t queue = 0; queue < *pQueueFamilyPropertyCount; ++queue) {
                memcpy(&pQueueFamilyProperties[queue].queueFamilyProperties, &phys_dev.queue_family_properties[queue].properties,
                       sizeof(VkQueueFamilyProperties));
            }
        }
    }
}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceSparseImageFormatProperties2(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo, uint32_t* pPropertyCount,
    VkSparseImageFormatProperties2* pProperties) {
    if (nullptr != pPropertyCount) {
        test_vkGetPhysicalDeviceSparseImageFormatProperties(physicalDevice, pFormatInfo->format, pFormatInfo->type,
                                                            pFormatInfo->samples, pFormatInfo->usage, pFormatInfo->tiling,
                                                            pPropertyCount, nullptr);
        if (nullptr != pProperties) {
            auto& phys_dev = icd.GetPhysDevice(physicalDevice);
            // Since the structures are different, we have to copy each item over seperately.  Since we have multiple, we
            // have to manually copy the data here instead of calling the next function
            for (uint32_t cnt = 0; cnt < *pPropertyCount; ++cnt) {
                memcpy(&pProperties[cnt].properties, &phys_dev.sparse_image_format_properties[cnt],
                       sizeof(VkSparseImageFormatProperties));
            }
        }
    }
}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceFormatProperties2(VkPhysicalDevice physicalDevice, VkFormat format,
                                                                     VkFormatProperties2* pFormatProperties) {
    if (nullptr != pFormatProperties) {
        test_vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &pFormatProperties->formatProperties);
    }
}
VKAPI_ATTR VkResult VKAPI_CALL test_vkGetPhysicalDeviceImageFormatProperties2(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo,
    VkImageFormatProperties2* pImageFormatProperties) {
    if (nullptr != pImageFormatInfo) {
        VkImageFormatProperties* ptr = nullptr;
        if (pImageFormatProperties) {
            ptr = &pImageFormatProperties->imageFormatProperties;
        }
        test_vkGetPhysicalDeviceImageFormatProperties(physicalDevice, pImageFormatInfo->format, pImageFormatInfo->type,
                                                      pImageFormatInfo->tiling, pImageFormatInfo->usage, pImageFormatInfo->flags,
                                                      ptr);
    }
    return VK_SUCCESS;
}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceExternalBufferProperties(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo,
    VkExternalBufferProperties* pExternalBufferProperties) {
    if (nullptr != pExternalBufferProperties) {
        auto& phys_dev = icd.GetPhysDevice(physicalDevice);
        memcpy(&pExternalBufferProperties->externalMemoryProperties, &phys_dev.external_memory_properties,
               sizeof(VkExternalMemoryProperties));
    }
}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceExternalSemaphoreProperties(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo,
    VkExternalSemaphoreProperties* pExternalSemaphoreProperties) {
    if (nullptr != pExternalSemaphoreProperties) {
        auto& phys_dev = icd.GetPhysDevice(physicalDevice);
        memcpy(pExternalSemaphoreProperties, &phys_dev.external_semaphore_properties, sizeof(VkExternalSemaphoreProperties));
    }
}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceExternalFenceProperties(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo,
    VkExternalFenceProperties* pExternalFenceProperties) {
    if (nullptr != pExternalFenceProperties) {
        auto& phys_dev = icd.GetPhysDevice(physicalDevice);
        memcpy(pExternalFenceProperties, &phys_dev.external_fence_properties, sizeof(VkExternalFenceProperties));
    }
}
// Entry-points associated with the VK_KHR_performance_query extension
VKAPI_ATTR VkResult VKAPI_CALL test_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR(
    VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, uint32_t* pCounterCount, VkPerformanceCounterKHR* pCounters,
    VkPerformanceCounterDescriptionKHR* pCounterDescriptions) {
    return VK_SUCCESS;
}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR(
    VkPhysicalDevice physicalDevice, const VkQueryPoolPerformanceCreateInfoKHR* pPerformanceQueryCreateInfo, uint32_t* pNumPasses) {
}
VKAPI_ATTR VkResult VKAPI_CALL test_vkAcquireProfilingLockKHR(VkDevice device, const VkAcquireProfilingLockInfoKHR* pInfo) {
    return VK_SUCCESS;
}
VKAPI_ATTR void VKAPI_CALL test_vkReleaseProfilingLockKHR(VkDevice device) {}
// Entry-points associated with the VK_EXT_sample_locations extension
VKAPI_ATTR void VKAPI_CALL test_vkCmdSetSampleLocationsEXT(VkCommandBuffer commandBuffer,
                                                           const VkSampleLocationsInfoEXT* pSampleLocationsInfo) {}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceMultisamplePropertiesEXT(VkPhysicalDevice physicalDevice,
                                                                            VkSampleCountFlagBits samples,
                                                                            VkMultisamplePropertiesEXT* pMultisampleProperties) {}
// Entry-points associated with the VK_EXT_calibrated_timestamps extension
VKAPI_ATTR VkResult VKAPI_CALL test_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT(VkPhysicalDevice physicalDevice,
                                                                                   uint32_t* pTimeDomainCount,
                                                                                   VkTimeDomainEXT* pTimeDomains) {
    return VK_SUCCESS;
}
VKAPI_ATTR VkResult VKAPI_CALL test_vkGetCalibratedTimestampsEXT(VkDevice device, uint32_t timestampCount,
                                                                 const VkCalibratedTimestampInfoEXT* pTimestampInfos,
                                                                 uint64_t* pTimestamps, uint64_t* pMaxDeviation) {
    return VK_SUCCESS;
}

//// trampolines

PFN_vkVoidFunction get_instance_func_ver_1_1(VkInstance instance, const char* pName) {
    if (icd.icd_api_version >= VK_API_VERSION_1_1) {
        if (string_eq(pName, "test_vkEnumerateInstanceVersion")) {
            return to_vkVoidFunction(test_vkEnumerateInstanceVersion);
        }
        if (string_eq(pName, "vkEnumeratePhysicalDeviceGroups")) {
            return to_vkVoidFunction(test_vkEnumeratePhysicalDeviceGroups);
        }
    }
    return nullptr;
}
PFN_vkVoidFunction get_instance_func_ver_1_2(VkInstance instance, const char* pName) {
    if (icd.icd_api_version >= VK_API_VERSION_1_2) {
        return nullptr;
    }
    return nullptr;
}

PFN_vkVoidFunction get_physical_device_func_wsi(VkInstance instance, const char* pName) {
    if (IsInstanceExtensionEnabled("VK_KHR_surface")) {
        if (string_eq(pName, "vkGetPhysicalDeviceSurfaceSupportKHR"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceSurfaceSupportKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceSurfaceFormatsKHR"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceSurfaceFormatsKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceSurfacePresentModesKHR"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceSurfacePresentModesKHR);
    }
    if (IsInstanceExtensionEnabled("VK_KHR_get_surface_capabilities2")) {
        if (string_eq(pName, "vkGetPhysicalDeviceSurfaceCapabilities2KHR"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceSurfaceCapabilities2KHR);
        if (string_eq(pName, "vkGetPhysicalDeviceSurfaceFormats2KHR"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceSurfaceFormats2KHR);
    }
    if (IsInstanceExtensionEnabled("VK_KHR_display")) {
        if (string_eq(pName, "vkGetPhysicalDeviceDisplayPropertiesKHR"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceDisplayPropertiesKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceDisplayPlanePropertiesKHR);
        if (string_eq(pName, "vkGetDisplayPlaneSupportedDisplaysKHR"))
            return to_vkVoidFunction(test_vkGetDisplayPlaneSupportedDisplaysKHR);
        if (string_eq(pName, "vkGetDisplayModePropertiesKHR")) return to_vkVoidFunction(test_vkGetDisplayModePropertiesKHR);
        if (string_eq(pName, "vkCreateDisplayModeKHR")) return to_vkVoidFunction(test_vkCreateDisplayModeKHR);
        if (string_eq(pName, "vkGetDisplayPlaneCapabilitiesKHR")) return to_vkVoidFunction(test_vkGetDisplayPlaneCapabilitiesKHR);
        if (string_eq(pName, "vkCreateDisplayPlaneSurfaceKHR")) return to_vkVoidFunction(test_vkCreateDisplayPlaneSurfaceKHR);
    }
    if (IsInstanceExtensionEnabled("VK_EXT_acquire_drm_display")) {
        if (string_eq(pName, "vkAcquireDrmDisplayEXT")) return to_vkVoidFunction(test_vkAcquireDrmDisplayEXT);
        if (string_eq(pName, "vkGetDrmDisplayEXT")) return to_vkVoidFunction(test_vkGetDrmDisplayEXT);
    }
    return nullptr;
}

PFN_vkVoidFunction get_instance_func_wsi(VkInstance instance, const char* pName) {
    if (icd.min_icd_interface_version >= 3 && icd.enable_icd_wsi == true) {
#ifdef VK_USE_PLATFORM_ANDROID_KHR
        if (string_eq(pName, "vkCreateAndroidSurfaceKHR")) {
            icd.is_using_icd_wsi = UsingICDProvidedWSI::is_using;
            return to_vkVoidFunction(test_vkCreateAndroidSurfaceKHR);
        }
#endif
#ifdef VK_USE_PLATFORM_METAL_EXT
        if (string_eq(pName, "vkCreateMetalSurfaceEXT")) {
            return to_vkVoidFunction(test_vkCreateMetalSurfaceEXT);
        }
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
        if (string_eq(pName, "vkCreateWaylandSurfaceKHR")) {
            return to_vkVoidFunction(test_vkCreateWaylandSurfaceKHR);
        }
        if (string_eq(pName, "vkGetPhysicalDeviceWaylandPresentationSupportKHR")) {
            return to_vkVoidFunction(test_vkGetPhysicalDeviceWaylandPresentationSupportKHR);
        }
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
        if (string_eq(pName, "vkCreateXcbSurfaceKHR")) {
            return to_vkVoidFunction(test_vkCreateXcbSurfaceKHR);
        }
        if (string_eq(pName, "vkGetPhysicalDeviceXcbPresentationSupportKHR")) {
            return to_vkVoidFunction(test_vkGetPhysicalDeviceXcbPresentationSupportKHR);
        }
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
        if (string_eq(pName, "vkCreateXlibSurfaceKHR")) {
            return to_vkVoidFunction(test_vkCreateXlibSurfaceKHR);
        }
        if (string_eq(pName, "vkGetPhysicalDeviceXlibPresentationSupportKHR")) {
            return to_vkVoidFunction(test_vkGetPhysicalDeviceXlibPresentationSupportKHR);
        }
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
        if (string_eq(pName, "vkCreateWin32SurfaceKHR")) {
            return to_vkVoidFunction(test_vkCreateWin32SurfaceKHR);
        }
        if (string_eq(pName, "vkGetPhysicalDeviceWin32PresentationSupportKHR")) {
            return to_vkVoidFunction(test_vkGetPhysicalDeviceWin32PresentationSupportKHR);
        }
#endif
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
        if (string_eq(pName, "vkCreateDirectFBSurfaceEXT")) {
            return to_vkVoidFunction(test_vkCreateDirectFBSurfaceEXT);
        }
        if (string_eq(pName, "vkGetPhysicalDeviceDirectFBPresentationSupportEXT")) {
            return to_vkVoidFunction(test_vkGetPhysicalDeviceDirectFBPresentationSupportEXT);
        }
#endif  // VK_USE_PLATFORM_DIRECTFB_EXT

#ifdef VK_USE_PLATFORM_MACOS_MVK
        if (string_eq(pName, "vkCreateMacOSSurfaceMVK")) {
            return to_vkVoidFunction(test_vkCreateMacOSSurfaceMVK);
        }
#endif  // VK_USE_PLATFORM_MACOS_MVK

#ifdef VK_USE_PLATFORM_IOS_MVK
        if (string_eq(pName, "vkCreateIOSSurfaceMVK")) {
            return to_vkVoidFunction(test_vkCreateIOSSurfaceMVK);
        }
#endif  // VK_USE_PLATFORM_IOS_MVK

#ifdef VK_USE_PLATFORM_GGP
        if (string_eq(pName, "vkCreateStreamDescriptorSurfaceGGP")) {
            return to_vkVoidFunction(test_vkCreateStreamDescriptorSurfaceGGP);
        }
#endif  // VK_USE_PLATFORM_GGP

#ifdef VK_USE_PLATFORM_SCREEN_QNX
        if (string_eq(pName, "vkCreateScreenSurfaceQNX")) {
            return to_vkVoidFunction(test_vkCreateScreenSurfaceQNX);
        }
        if (string_eq(pName, "vkGetPhysicalDeviceScreenPresentationSupportQNX")) {
            return to_vkVoidFunction(test_vkGetPhysicalDeviceScreenPresentationSupportQNX);
        }
#endif  // VK_USE_PLATFORM_SCREEN_QNX

        if (string_eq(pName, "vkCreateHeadlessSurfaceEXT")) {
            return to_vkVoidFunction(test_vkCreateHeadlessSurfaceEXT);
        }

        if (string_eq(pName, "vkDestroySurfaceKHR")) {
            icd.is_using_icd_wsi = UsingICDProvidedWSI::is_using;
            return to_vkVoidFunction(test_vkDestroySurfaceKHR);
        }
    }
    if (IsInstanceExtensionEnabled(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
        if (string_eq(pName, "vkCreateDebugUtilsMessengerEXT")) {
            return to_vkVoidFunction(test_vkCreateDebugUtilsMessengerEXT);
        }
        if (string_eq(pName, "vkDestroyDebugUtilsMessengerEXT")) {
            return to_vkVoidFunction(test_vkDestroyDebugUtilsMessengerEXT);
        }
    }

    PFN_vkVoidFunction ret_phys_dev_wsi = get_physical_device_func_wsi(instance, pName);
    if (ret_phys_dev_wsi != nullptr) return ret_phys_dev_wsi;
    return nullptr;
}
PFN_vkVoidFunction get_physical_device_func(VkInstance instance, const char* pName) {
    if (string_eq(pName, "vkEnumerateDeviceLayerProperties")) return to_vkVoidFunction(test_vkEnumerateDeviceLayerProperties);
    if (string_eq(pName, "vkEnumerateDeviceExtensionProperties"))
        return to_vkVoidFunction(test_vkEnumerateDeviceExtensionProperties);
    if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyProperties"))
        return to_vkVoidFunction(test_vkGetPhysicalDeviceQueueFamilyProperties);
    if (string_eq(pName, "vkCreateDevice")) return to_vkVoidFunction(test_vkCreateDevice);

    if (string_eq(pName, "vkGetPhysicalDeviceFeatures")) return to_vkVoidFunction(test_vkGetPhysicalDeviceFeatures);
    if (string_eq(pName, "vkGetPhysicalDeviceProperties")) return to_vkVoidFunction(test_vkGetPhysicalDeviceProperties);
    if (string_eq(pName, "vkGetPhysicalDeviceMemoryProperties")) return to_vkVoidFunction(test_vkGetPhysicalDeviceMemoryProperties);
    if (string_eq(pName, "vkGetPhysicalDeviceSparseImageFormatProperties"))
        return to_vkVoidFunction(test_vkGetPhysicalDeviceSparseImageFormatProperties);
    if (string_eq(pName, "vkGetPhysicalDeviceFormatProperties")) return to_vkVoidFunction(test_vkGetPhysicalDeviceFormatProperties);
    if (string_eq(pName, "vkGetPhysicalDeviceImageFormatProperties"))
        return to_vkVoidFunction(test_vkGetPhysicalDeviceImageFormatProperties);

    if (IsInstanceExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
        if (string_eq(pName, "vkGetPhysicalDeviceFeatures2KHR")) return to_vkVoidFunction(test_vkGetPhysicalDeviceFeatures2);
        if (string_eq(pName, "vkGetPhysicalDeviceProperties2KHR")) return to_vkVoidFunction(test_vkGetPhysicalDeviceProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceFormatProperties2KHR"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceFormatProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceMemoryProperties2KHR"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceMemoryProperties2);

        if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyProperties2KHR"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceQueueFamilyProperties2);

        if (string_eq(pName, "vkGetPhysicalDeviceSparseImageFormatProperties2KHR"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceSparseImageFormatProperties2);

        if (string_eq(pName, "vkGetPhysicalDeviceImageFormatProperties2KHR")) {
            return to_vkVoidFunction(test_vkGetPhysicalDeviceImageFormatProperties2);
        }
    }
    if (IsInstanceExtensionEnabled(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME)) {
        if (string_eq(pName, "vkGetPhysicalDeviceExternalBufferPropertiesKHR"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceExternalBufferProperties);
    }
    if (IsInstanceExtensionEnabled(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME)) {
        if (string_eq(pName, "vkGetPhysicalDeviceExternalSemaphorePropertiesKHR"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceExternalSemaphoreProperties);
    }
    if (IsInstanceExtensionEnabled(VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME)) {
        if (string_eq(pName, "vkGetPhysicalDeviceExternalFencePropertiesKHR"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceExternalFenceProperties);
    }

    // The following physical device extensions only need 1 device to support them for the ICD to export
    // them
    if (IsPhysicalDeviceExtensionAvailable(VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME)) {
        if (string_eq(pName, "vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR"))
            return to_vkVoidFunction(test_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR);
        if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR);
        if (string_eq(pName, "vkAcquireProfilingLockKHR")) return to_vkVoidFunction(test_vkAcquireProfilingLockKHR);
        if (string_eq(pName, "vkReleaseProfilingLockKHR")) return to_vkVoidFunction(test_vkReleaseProfilingLockKHR);
    }
    if (IsPhysicalDeviceExtensionAvailable(VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME)) {
        if (string_eq(pName, "vkCmdSetSampleLocationsEXT")) return to_vkVoidFunction(test_vkCmdSetSampleLocationsEXT);
        if (string_eq(pName, "vkGetPhysicalDeviceMultisamplePropertiesEXT"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceMultisamplePropertiesEXT);
    }
    if (IsPhysicalDeviceExtensionAvailable(VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME)) {
        if (string_eq(pName, "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT);
        if (string_eq(pName, "vkGetCalibratedTimestampsEXT")) return to_vkVoidFunction(test_vkGetCalibratedTimestampsEXT);
    }

    if (icd.icd_api_version >= VK_MAKE_API_VERSION(0, 1, 1, 0)) {
        if (string_eq(pName, "vkGetPhysicalDeviceFeatures2")) return to_vkVoidFunction(test_vkGetPhysicalDeviceFeatures2);
        if (string_eq(pName, "vkGetPhysicalDeviceProperties2")) return to_vkVoidFunction(test_vkGetPhysicalDeviceProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceFormatProperties2"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceFormatProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceMemoryProperties2"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceMemoryProperties2);

        if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyProperties2"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceQueueFamilyProperties2);

        if (string_eq(pName, "vkGetPhysicalDeviceSparseImageFormatProperties2"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceSparseImageFormatProperties2);

        if (string_eq(pName, "vkGetPhysicalDeviceImageFormatProperties2")) {
            return to_vkVoidFunction(test_vkGetPhysicalDeviceImageFormatProperties2);
        }

        if (string_eq(pName, "vkGetPhysicalDeviceExternalBufferProperties"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceExternalBufferProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceExternalSemaphoreProperties"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceExternalSemaphoreProperties);
        if (string_eq(pName, "vkGetPhysicalDeviceExternalFenceProperties"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceExternalFenceProperties);
    }

    if (icd.supports_tooling_info_core) {
        if (string_eq(pName, "vkGetPhysicalDeviceToolProperties")) return to_vkVoidFunction(test_vkGetPhysicalDeviceToolProperties);
    }
    if (icd.supports_tooling_info_ext) {
        if (string_eq(pName, "vkGetPhysicalDeviceToolPropertiesEXT"))
            return to_vkVoidFunction(test_vkGetPhysicalDeviceToolPropertiesEXT);
    }

    for (auto& func : icd.custom_physical_device_functions) {
        if (func.name == pName) {
            return to_vkVoidFunction(func.function);
        }
    }
    return nullptr;
}

PFN_vkVoidFunction get_instance_func(VkInstance instance, const char* pName) {
    if (string_eq(pName, "vkEnumerateInstanceExtensionProperties"))
        return to_vkVoidFunction(test_vkEnumerateInstanceExtensionProperties);
    if (string_eq(pName, "vkEnumerateInstanceLayerProperties")) return to_vkVoidFunction(test_vkEnumerateInstanceLayerProperties);
    if (string_eq(pName, "vkCreateInstance")) return to_vkVoidFunction(test_vkCreateInstance);
    if (string_eq(pName, "vkDestroyInstance")) return to_vkVoidFunction(test_vkDestroyInstance);
    if (string_eq(pName, "vkEnumeratePhysicalDevices")) return to_vkVoidFunction(test_vkEnumeratePhysicalDevices);

    if (IsInstanceExtensionEnabled(VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME)) {
        if (string_eq(pName, "vkEnumeratePhysicalDeviceGroupsKHR")) return to_vkVoidFunction(test_vkEnumeratePhysicalDeviceGroups);
    }

    PFN_vkVoidFunction ret_phys_dev = get_physical_device_func(instance, pName);
    if (ret_phys_dev != nullptr) return ret_phys_dev;

    PFN_vkVoidFunction ret_1_1 = get_instance_func_ver_1_1(instance, pName);
    if (ret_1_1 != nullptr) return ret_1_1;

    PFN_vkVoidFunction ret_1_2 = get_instance_func_ver_1_2(instance, pName);
    if (ret_1_2 != nullptr) return ret_1_2;

    PFN_vkVoidFunction ret_wsi = get_instance_func_wsi(instance, pName);
    if (ret_wsi != nullptr) return ret_wsi;

    for (auto& func : icd.custom_instance_functions) {
        if (func.name == pName) {
            return to_vkVoidFunction(func.function);
        }
    }

    return nullptr;
}

PFN_vkVoidFunction get_device_func(VkDevice device, const char* pName) {
    if (device != nullptr) {
        if (!std::any_of(icd.physical_devices.begin(), icd.physical_devices.end(), [&](const PhysicalDevice& pd) {
                return std::any_of(pd.device_handles.begin(), pd.device_handles.end(),
                                   [&](const VkDevice& pd_device) { return pd_device == device; });
            })) {
            return nullptr;
        }
    }
    if (string_eq(pName, "vkDestroyDevice")) return to_vkVoidFunction(test_vkDestroyDevice);
    if (string_eq(pName, "vkCreateSwapchainKHR")) return to_vkVoidFunction(test_vkCreateSwapchainKHR);
    if (string_eq(pName, "vkDestroySwapchainKHR")) return to_vkVoidFunction(test_vkDestroySwapchainKHR);
    if (string_eq(pName, "vkCreateCommandPool")) return to_vkVoidFunction(test_vkCreateCommandPool);
    if (string_eq(pName, "vkAllocateCommandBuffers")) return to_vkVoidFunction(test_vkAllocateCommandBuffers);
    if (string_eq(pName, "vkDestroyCommandPool")) return to_vkVoidFunction(test_vkDestroyCommandPool);
    if (string_eq(pName, "vkGetDeviceQueue")) return to_vkVoidFunction(test_vkGetDeviceQueue);
    // look for device functions setup from a test
    for (const auto& phys_dev : icd.physical_devices) {
        for (const auto& function : phys_dev.known_device_functions) {
            if (function.name == pName) {
                return to_vkVoidFunction(function.function);
            }
        }
    }
    return nullptr;
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL test_vkGetInstanceProcAddr(VkInstance instance, const char* pName) {
    return get_instance_func(instance, pName);
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL test_vkGetDeviceProcAddr(VkDevice device, const char* pName) {
    return get_device_func(device, pName);
}

PFN_vkVoidFunction base_get_instance_proc_addr(VkInstance instance, const char* pName) {
    if (pName == nullptr) return nullptr;
    if (instance == NULL) {
        if (string_eq(pName, "vkGetInstanceProcAddr")) return to_vkVoidFunction(test_vkGetInstanceProcAddr);
        if (string_eq(pName, "vkEnumerateInstanceExtensionProperties"))
            return to_vkVoidFunction(test_vkEnumerateInstanceExtensionProperties);
        if (string_eq(pName, "vkEnumerateInstanceLayerProperties"))
            return to_vkVoidFunction(test_vkEnumerateInstanceLayerProperties);
        if (string_eq(pName, "vkEnumerateInstanceVersion")) return to_vkVoidFunction(test_vkEnumerateInstanceVersion);
    }
    if (string_eq(pName, "vkGetDeviceProcAddr")) return to_vkVoidFunction(test_vkGetDeviceProcAddr);

    auto instance_func_return = get_instance_func(instance, pName);
    if (instance_func_return != nullptr) return instance_func_return;

    // Need to return function pointers for device extensions
    auto device_func_return = get_device_func(nullptr, pName);
    if (device_func_return != nullptr) return device_func_return;
    return nullptr;
}

// Exported functions
extern "C" {
#if TEST_ICD_EXPORT_NEGOTIATE_INTERFACE_VERSION
extern FRAMEWORK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vk_icdNegotiateLoaderICDInterfaceVersion(uint32_t* pSupportedVersion) {
    if (icd.called_vk_icd_gipa == CalledICDGIPA::not_called &&
        icd.called_negotiate_interface == CalledNegotiateInterface::not_called)
        icd.called_negotiate_interface = CalledNegotiateInterface::vk_icd_negotiate;
    else if (icd.called_vk_icd_gipa != CalledICDGIPA::not_called)
        icd.called_negotiate_interface = CalledNegotiateInterface::vk_icd_gipa_first;

    // loader puts the minimum it supports in pSupportedVersion, if that is lower than our minimum
    // If the ICD doesn't supports the interface version provided by the loader, report VK_ERROR_INCOMPATIBLE_DRIVER
    if (icd.min_icd_interface_version > *pSupportedVersion) {
        icd.interface_version_check = InterfaceVersionCheck::loader_version_too_old;
        *pSupportedVersion = icd.min_icd_interface_version;
        return VK_ERROR_INCOMPATIBLE_DRIVER;
    }

    // the loader-provided interface version is newer than that supported by the ICD
    if (icd.max_icd_interface_version < *pSupportedVersion) {
        icd.interface_version_check = InterfaceVersionCheck::loader_version_too_new;
        *pSupportedVersion = icd.max_icd_interface_version;
    }
    // ICD interface version is greater than the loader's,  return the loader's version
    else if (icd.max_icd_interface_version > *pSupportedVersion) {
        icd.interface_version_check = InterfaceVersionCheck::icd_version_too_new;
        // don't change *pSupportedVersion
    } else {
        icd.interface_version_check = InterfaceVersionCheck::version_is_supported;
        *pSupportedVersion = icd.max_icd_interface_version;
    }

    return VK_SUCCESS;
}
#endif  // TEST_ICD_EXPORT_NEGOTIATE_INTERFACE_VERSION

#if TEST_ICD_EXPORT_ICD_GPDPA
FRAMEWORK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vk_icdGetPhysicalDeviceProcAddr(VkInstance instance, const char* pName) {
    // std::cout << "icdGetPhysicalDeviceProcAddr: " << pName << "\n";
    return get_physical_device_func(instance, pName);
}
#endif  // TEST_ICD_EXPORT_ICD_GPDPA

#if TEST_ICD_EXPORT_ICD_GIPA
FRAMEWORK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vk_icdGetInstanceProcAddr(VkInstance instance, const char* pName) {
    // std::cout << "icdGetInstanceProcAddr: " << pName << "\n";

    if (icd.called_vk_icd_gipa == CalledICDGIPA::not_called) icd.called_vk_icd_gipa = CalledICDGIPA::vk_icd_gipa;

    return base_get_instance_proc_addr(instance, pName);
}
#else   // !TEST_ICD_EXPORT_ICD_GIPA
FRAMEWORK_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance instance, const char* pName) {
    // std::cout << "icdGetInstanceProcAddr: " << pName << "\n";

    if (icd.called_vk_icd_gipa == CalledICDGIPA::not_called) icd.called_vk_icd_gipa = CalledICDGIPA::vk_gipa;
    return base_get_instance_proc_addr(instance, pName);
}
FRAMEWORK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo,
                                                                 const VkAllocationCallbacks* pAllocator, VkInstance* pInstance) {
    return test_vkCreateInstance(pCreateInfo, pAllocator, pInstance);
}
FRAMEWORK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(const char* pLayerName,
                                                                                       uint32_t* pPropertyCount,
                                                                                       VkExtensionProperties* pProperties) {
    return test_vkEnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties);
}
#endif  // TEST_ICD_EXPORT_ICD_GIPA

#if TEST_ICD_EXPORT_ICD_ENUMERATE_ADAPTER_PHYSICAL_DEVICES
#if defined(WIN32)
FRAMEWORK_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vk_icdEnumerateAdapterPhysicalDevices(VkInstance instance, LUID adapterLUID,
                                                                                      uint32_t* pPhysicalDeviceCount,
                                                                                      VkPhysicalDevice* pPhysicalDevices) {
    if (adapterLUID.LowPart != icd.adapterLUID.LowPart || adapterLUID.HighPart != icd.adapterLUID.HighPart) {
        *pPhysicalDeviceCount = 0;
        return VK_SUCCESS;
    }
    icd.called_enumerate_adapter_physical_devices = true;
    VkResult res = test_vkEnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);
    // For this testing, flip order intentionally
    if (nullptr != pPhysicalDevices) {
        std::reverse(pPhysicalDevices, pPhysicalDevices + *pPhysicalDeviceCount);
    }
    return res;
}
#endif  // defined(WIN32)
#endif  // TEST_ICD_EXPORT_ICD_ENUMERATE_ADAPTER_PHYSICAL_DEVICES

}  // extern "C"