/*
 * Copyright (c) 2021 The Khronos Group Inc.
 * Copyright (c) 2021 Valve Corporation
 * Copyright (c) 2021 LunarG, Inc.
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

bool IsInstanceExtensionEnabled(const char* extension_name) {
    return icd.instance_extensions.end() !=
           std::find_if(icd.instance_extensions.begin(), icd.instance_extensions.end(),
                        [extension_name](Extension const& ext) { return ext.extensionName == extension_name; });
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
        *pApiVersion = VK_MAKE_VERSION(1, 0, 0);
    }
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo,
                                                     const VkAllocationCallbacks* pAllocator, VkInstance* pInstance) {
    if (pCreateInfo == nullptr || pCreateInfo->pApplicationInfo == nullptr) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    if (icd.icd_api_version < VK_MAKE_VERSION(1, 1, 0)) {
        if (pCreateInfo->pApplicationInfo->apiVersion > VK_MAKE_VERSION(1, 0, 0)) {
            return VK_ERROR_INCOMPATIBLE_DRIVER;
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
    if (pPhysicalDeviceGroupProperties == nullptr) {
        *pPhysicalDeviceGroupCount = static_cast<uint32_t>(icd.physical_device_groups.size());
    } else {
        for (size_t device_group = 0; device_group < icd.physical_device_groups.size(); device_group++) {
            if (device_group >= *pPhysicalDeviceGroupCount) {
                return VK_INCOMPLETE;
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
    return VK_SUCCESS;
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
        auto found_iter = icd.messenger_handles.erase(
            std::remove(icd.messenger_handles.begin(), icd.messenger_handles.end(), fake_msgr_handle), icd.messenger_handles.end());
        if (found_iter == icd.messenger_handles.end()) {
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
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL test_vkDestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator) {
    auto found = std::find(icd.device_handles.begin(), icd.device_handles.end(), device);
    if (found != icd.device_handles.end()) icd.device_handles.erase(found);
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkGetPhysicalDeviceToolPropertiesEXT(VkPhysicalDevice physicalDevice, uint32_t* pToolCount,
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

//// WSI ////
#ifdef VK_USE_PLATFORM_ANDROID_KHR
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateAndroidSurfaceKHR(VkInstance instance, const VkAndroidSurfaceCreateInfoKHR* pCreateInfo,
                                                              const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
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
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateWin32SurfaceKHR(VkInstance instance, const VkWin32SurfaceCreateInfoKHR* pCreateInfo,
                                                            const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
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
#endif  // VK_USE_PLATFORM_WIN32_KHR

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateWaylandSurfaceKHR(VkInstance instance, const VkWaylandSurfaceCreateInfoKHR* pCreateInfo,
                                                              const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
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
#endif  // VK_USE_PLATFORM_WAYLAND_KHR

#ifdef VK_USE_PLATFORM_XCB_KHR
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateXcbSurfaceKHR(VkInstance instance, const VkXcbSurfaceCreateInfoKHR* pCreateInfo,
                                                          const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
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
#endif  // VK_USE_PLATFORM_XCB_KHR

#ifdef VK_USE_PLATFORM_XLIB_KHR
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateXlibSurfaceKHR(VkInstance instance, const VkXlibSurfaceCreateInfoKHR* pCreateInfo,
                                                           const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
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
#endif  // VK_USE_PLATFORM_XLIB_KHR

#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateDirectFBSurfaceEXT(VkInstance instance,
                                                               const VkDirectFBSurfaceCreateInfoEXT* pCreateInfo,
                                                               const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
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
#endif  // VK_USE_PLATFORM_DIRECTFB_EXT

#ifdef VK_USE_PLATFORM_MACOS_MVK
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateMacOSSurfaceMVK(VkInstance instance, const VkMacOSSurfaceCreateInfoMVK* pCreateInfo,
                                                            const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
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
#endif  // VK_USE_PLATFORM_MACOS_MVK

#ifdef VK_USE_PLATFORM_IOS_MVK
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateIOSSurfaceMVK(VkInstance instance, const VkIOSSurfaceCreateInfoMVK* pCreateInfo,
                                                          const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
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
#endif  // VK_USE_PLATFORM_IOS_MVK

#ifdef VK_USE_PLATFORM_GGP
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateStreamDescriptorSurfaceGGP(VkInstance instance,
                                                                       const VkStreamDescriptorSurfaceCreateInfoGGP* pCreateInfo,
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
#endif  // VK_USE_PLATFORM_GGP

#if defined(VK_USE_PLATFORM_METAL_EXT)
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateMetalSurfaceEXT(VkInstance instance, const VkMetalSurfaceCreateInfoEXT* pCreateInfo,
                                                            const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
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
#endif  // VK_USE_PLATFORM_METAL_EXT

#ifdef VK_USE_PLATFORM_SCREEN_QNX
VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateScreenSurfaceQNX(VkInstance instance, const VkScreenSurfaceCreateInfoQNX* pCreateInfo,
                                                             const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
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
#endif  // VK_USE_PLATFORM_SCREEN_QNX

VKAPI_ATTR void VKAPI_CALL test_vkDestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface,
                                                    const VkAllocationCallbacks* pAllocator) {
    if (surface != VK_NULL_HANDLE) {
        uint64_t fake_surf_handle = (uint64_t)(surface);
        auto found_iter = icd.surface_handles.erase(
            std::remove(icd.surface_handles.begin(), icd.surface_handles.end(), fake_surf_handle), icd.surface_handles.end());
        if (found_iter == icd.surface_handles.end()) {
            assert(false && "Surface not found during destroy!");
        }
    }
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo,
                                                         const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain) {
    if (nullptr != pSwapchain) {
        uint64_t fake_swapchain_handle = reinterpret_cast<uint64_t>(new uint8_t);
        icd.swapchain_handles.push_back(fake_swapchain_handle);
#if defined(__LP64__) || defined(_WIN64) || (defined(__x86_64__) && !defined(__ILP32__)) || defined(_M_X64) || defined(__ia64) || \
    defined(_M_IA64) || defined(__aarch64__) || defined(__powerpc64__)
        *pSwapchain = reinterpret_cast<VkSwapchainKHR>(fake_swapchain_handle);
#else
        *pSwapchain = fake_swapchain_handle;
#endif
    }
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL test_vkDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain,
                                                      const VkAllocationCallbacks* pAllocator) {
    if (swapchain != VK_NULL_HANDLE) {
        uint64_t fake_swapchain_handle = (uint64_t)(swapchain);
        auto found_iter = icd.swapchain_handles.erase(
            std::remove(icd.swapchain_handles.begin(), icd.swapchain_handles.end(), fake_swapchain_handle),
            icd.swapchain_handles.end());
        if (found_iter == icd.swapchain_handles.end()) {
            assert(false && "Swapchain not found during destroy!");
        }
    }
}

VKAPI_ATTR VkResult VKAPI_CALL test_vkGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex,
                                                                         VkSurfaceKHR surface, VkBool32* pSupported) {
    if (nullptr != pSupported) {
        *pSupported = icd.GetPhysDevice(physicalDevice).queue_family_properties.at(queueFamilyIndex).support_present;
    }
    return VK_SUCCESS;
}
VKAPI_ATTR VkResult VKAPI_CALL test_vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                                                              VkSurfaceCapabilitiesKHR* pSurfaceCapabilities) {
    if (nullptr != pSurfaceCapabilities) {
        *pSurfaceCapabilities = icd.GetPhysDevice(physicalDevice).surface_capabilities;
    }
    return VK_SUCCESS;
}
VKAPI_ATTR VkResult VKAPI_CALL test_vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                                                         uint32_t* pSurfaceFormatCount,
                                                                         VkSurfaceFormatKHR* pSurfaceFormats) {
    FillCountPtr(icd.GetPhysDevice(physicalDevice).surface_formats, pSurfaceFormatCount, pSurfaceFormats);
    return VK_SUCCESS;
}
VKAPI_ATTR VkResult VKAPI_CALL test_vkGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                                                              uint32_t* pPresentModeCount,
                                                                              VkPresentModeKHR* pPresentModes) {
    FillCountPtr(icd.GetPhysDevice(physicalDevice).surface_present_modes, pPresentModeCount, pPresentModes);
    return VK_SUCCESS;
}

//// stubs
// 1.0
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* pFeatures) {}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice,
                                                              VkPhysicalDeviceProperties* pProperties) {}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice,
                                                                    VkPhysicalDeviceMemoryProperties* pMemoryProperties) {}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceSparseImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format,
                                                                               VkImageType type, VkSampleCountFlagBits samples,
                                                                               VkImageUsageFlags usage, VkImageTiling tiling,
                                                                               uint32_t* pPropertyCount,
                                                                               VkSparseImageFormatProperties* pProperties) {}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format,
                                                                    VkFormatProperties* pFormatProperties) {}
VKAPI_ATTR VkResult VKAPI_CALL test_vkGetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format,
                                                                             VkImageType type, VkImageTiling tiling,
                                                                             VkImageUsageFlags usage, VkImageCreateFlags flags,
                                                                             VkImageFormatProperties* pImageFormatProperties) {
    return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceFeatures2(VkPhysicalDevice physicalDevice,
                                                             VkPhysicalDeviceFeatures2* pFeatures) {}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceProperties2(VkPhysicalDevice physicalDevice,
                                                               VkPhysicalDeviceProperties2* pProperties) {}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceMemoryProperties2(VkPhysicalDevice physicalDevice,
                                                                     VkPhysicalDeviceMemoryProperties2* pMemoryProperties) {}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceQueueFamilyProperties2(VkPhysicalDevice physicalDevice,
                                                                          uint32_t* pQueueFamilyPropertyCount,
                                                                          VkQueueFamilyProperties2* pQueueFamilyProperties) {}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceSparseImageFormatProperties2(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo, uint32_t* pPropertyCount,
    VkSparseImageFormatProperties2* pProperties) {}
VKAPI_ATTR void VKAPI_CALL test_vkGetPhysicalDeviceFormatProperties2(VkPhysicalDevice physicalDevice, VkFormat format,
                                                                     VkFormatProperties2* pFormatProperties) {}
VKAPI_ATTR VkResult VKAPI_CALL test_vkGetPhysicalDeviceImageFormatProperties2(
    VkPhysicalDevice physicalDevice, const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo,
    VkImageFormatProperties2* pImageFormatProperties) {
    return VK_SUCCESS;
}

//// trampolines

#define TO_VOID_PFN(func) reinterpret_cast<PFN_vkVoidFunction>(func)

PFN_vkVoidFunction get_instance_func_ver_1_1(VkInstance instance, const char* pName) {
    if (icd.icd_api_version >= VK_MAKE_VERSION(1, 1, 0)) {
        if (string_eq(pName, "test_vkEnumerateInstanceVersion")) {
            return TO_VOID_PFN(test_vkEnumerateInstanceVersion);
        }
    }
    return nullptr;
}
PFN_vkVoidFunction get_instance_func_ver_1_2(VkInstance instance, const char* pName) {
    if (icd.icd_api_version >= VK_MAKE_VERSION(1, 2, 0)) {
        return nullptr;
    }
    return nullptr;
}

PFN_vkVoidFunction get_physical_device_func_wsi(VkInstance instance, const char* pName) {
    if (string_eq(pName, "vkGetPhysicalDeviceSurfaceSupportKHR")) return TO_VOID_PFN(test_vkGetPhysicalDeviceSurfaceSupportKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"))
        return TO_VOID_PFN(test_vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceSurfaceFormatsKHR")) return TO_VOID_PFN(test_vkGetPhysicalDeviceSurfaceFormatsKHR);
    if (string_eq(pName, "vkGetPhysicalDeviceSurfacePresentModesKHR"))
        return TO_VOID_PFN(test_vkGetPhysicalDeviceSurfacePresentModesKHR);
    return nullptr;
}

PFN_vkVoidFunction get_instance_func_wsi(VkInstance instance, const char* pName) {
    if (icd.min_icd_interface_version >= 3 && icd.enable_icd_wsi == true) {
#ifdef VK_USE_PLATFORM_ANDROID_KHR
        if (string_eq(pName, "vkCreateAndroidSurfaceKHR")) {
            icd.is_using_icd_wsi = UsingICDProvidedWSI::is_using;
            return TO_VOID_PFN(test_vkCreateAndroidSurfaceKHR);
        }
#endif
#ifdef VK_USE_PLATFORM_METAL_EXT
        if (string_eq(pName, "vkCreateMetalSurfaceEXT")) {
            return TO_VOID_PFN(test_vkCreateMetalSurfaceEXT);
        }
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
        if (string_eq(pName, "vkCreateWaylandSurfaceKHR")) {
            return TO_VOID_PFN(test_vkCreateWaylandSurfaceKHR);
        }
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
        if (string_eq(pName, "vkCreateXcbSurfaceKHR")) {
            return TO_VOID_PFN(test_vkCreateXcbSurfaceKHR);
        }
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
        if (string_eq(pName, "vkCreateXlibSurfaceKHR")) {
            return TO_VOID_PFN(test_vkCreateXlibSurfaceKHR);
        }
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
        if (string_eq(pName, "vkCreateWin32SurfaceKHR")) {
            return TO_VOID_PFN(test_vkCreateWin32SurfaceKHR);
        }
#endif
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
        if (string_eq(pName, "vkCreateDirectFBSurfaceEXT")) {
            return TO_VOID_PFN(test_vkCreateDirectFBSurfaceEXT);
        }
#endif  // VK_USE_PLATFORM_DIRECTFB_EXT

#ifdef VK_USE_PLATFORM_MACOS_MVK
        if (string_eq(pName, "vkCreateMacOSSurfaceMVK")) {
            return TO_VOID_PFN(test_vkCreateMacOSSurfaceMVK);
        }
#endif  // VK_USE_PLATFORM_MACOS_MVK

#ifdef VK_USE_PLATFORM_IOS_MVK
        if (string_eq(pName, "vkCreateIOSSurfaceMVK")) {
            return TO_VOID_PFN(test_vkCreateIOSSurfaceMVK);
        }
#endif  // VK_USE_PLATFORM_IOS_MVK

#ifdef VK_USE_PLATFORM_GGP
        if (string_eq(pName, "vkCreateStreamDescriptorSurfaceGGP")) {
            return TO_VOID_PFN(test_vkCreateStreamDescriptorSurfaceGGP);
        }
#endif  // VK_USE_PLATFORM_GGP

#ifdef VK_USE_PLATFORM_SCREEN_QNX
        if (string_eq(pName, "vkCreateScreenSurfaceQNX")) {
            return TO_VOID_PFN(test_vkCreateScreenSurfaceQNX);
        }
#endif  // VK_USE_PLATFORM_SCREEN_QNX

        if (string_eq(pName, "vkDestroySurfaceKHR")) {
            icd.is_using_icd_wsi = UsingICDProvidedWSI::is_using;
            return TO_VOID_PFN(test_vkDestroySurfaceKHR);
        }
    }
    if (IsInstanceExtensionEnabled(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
        if (string_eq(pName, "vkCreateDebugUtilsMessengerEXT")) {
            icd.is_using_icd_wsi = UsingICDProvidedWSI::is_using;
            return TO_VOID_PFN(test_vkCreateDebugUtilsMessengerEXT);
        }
        if (string_eq(pName, "vkDestroyDebugUtilsMessengerEXT")) {
            icd.is_using_icd_wsi = UsingICDProvidedWSI::is_using;
            return TO_VOID_PFN(test_vkDestroyDebugUtilsMessengerEXT);
        }
    }

    PFN_vkVoidFunction ret_phys_dev_wsi = get_physical_device_func_wsi(instance, pName);
    if (ret_phys_dev_wsi != nullptr) return ret_phys_dev_wsi;
    return nullptr;
}
PFN_vkVoidFunction get_physical_device_func(VkInstance instance, const char* pName) {
    if (string_eq(pName, "vkEnumerateDeviceLayerProperties")) return TO_VOID_PFN(test_vkEnumerateDeviceLayerProperties);
    if (string_eq(pName, "vkEnumerateDeviceExtensionProperties")) return TO_VOID_PFN(test_vkEnumerateDeviceExtensionProperties);
    if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyProperties"))
        return TO_VOID_PFN(test_vkGetPhysicalDeviceQueueFamilyProperties);
    if (string_eq(pName, "vkCreateDevice")) return TO_VOID_PFN(test_vkCreateDevice);

    if (string_eq(pName, "vkGetPhysicalDeviceFeatures")) return TO_VOID_PFN(test_vkGetPhysicalDeviceFeatures);
    if (string_eq(pName, "vkGetPhysicalDeviceProperties")) return TO_VOID_PFN(test_vkGetPhysicalDeviceProperties);
    if (string_eq(pName, "vkGetPhysicalDeviceMemoryProperties")) return TO_VOID_PFN(test_vkGetPhysicalDeviceMemoryProperties);
    if (string_eq(pName, "vkGetPhysicalDeviceSparseImageFormatProperties"))
        return TO_VOID_PFN(test_vkGetPhysicalDeviceSparseImageFormatProperties);
    if (string_eq(pName, "vkGetPhysicalDeviceFormatProperties")) return TO_VOID_PFN(test_vkGetPhysicalDeviceFormatProperties);
    if (string_eq(pName, "vkGetPhysicalDeviceImageFormatProperties"))
        return TO_VOID_PFN(test_vkGetPhysicalDeviceImageFormatProperties);

    if (icd.icd_api_version >= VK_MAKE_API_VERSION(0, 1, 1, 0) ||
        IsInstanceExtensionEnabled("VK_KHR_get_physical_device_properties2")) {
        if (string_eq(pName, "vkGetPhysicalDeviceFeatures2")) return TO_VOID_PFN(test_vkGetPhysicalDeviceFeatures2);
        if (string_eq(pName, "vkGetPhysicalDeviceProperties2")) return TO_VOID_PFN(test_vkGetPhysicalDeviceProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceFormatProperties2")) return TO_VOID_PFN(test_vkGetPhysicalDeviceFormatProperties2);
        if (string_eq(pName, "vkGetPhysicalDeviceMemoryProperties2")) return TO_VOID_PFN(test_vkGetPhysicalDeviceMemoryProperties2);

        if (string_eq(pName, "vkGetPhysicalDeviceQueueFamilyProperties2"))
            return TO_VOID_PFN(test_vkGetPhysicalDeviceQueueFamilyProperties2);

        if (string_eq(pName, "vkGetPhysicalDeviceSparseImageFormatProperties2"))
            return TO_VOID_PFN(test_vkGetPhysicalDeviceSparseImageFormatProperties2);

        if (string_eq(pName, "vkGetPhysicalDeviceImageFormatProperties2")) {
            return TO_VOID_PFN(test_vkGetPhysicalDeviceImageFormatProperties2);
        }
    }
    if (icd.supports_tooling_info_ext) {
        if (string_eq(pName, "vkGetPhysicalDeviceToolPropertiesEXT")) return TO_VOID_PFN(test_vkGetPhysicalDeviceToolPropertiesEXT);
    }

    for (auto& func : icd.custom_physical_device_functions) {
        if (func.name == pName) {
            return TO_VOID_PFN(func.function);
        }
    }
    return nullptr;
}

PFN_vkVoidFunction get_instance_func(VkInstance instance, const char* pName) {
    if (string_eq(pName, "vkEnumerateInstanceExtensionProperties")) return TO_VOID_PFN(test_vkEnumerateInstanceExtensionProperties);
    if (string_eq(pName, "vkEnumerateInstanceLayerProperties")) return TO_VOID_PFN(test_vkEnumerateInstanceLayerProperties);
    if (string_eq(pName, "vkCreateInstance")) return TO_VOID_PFN(test_vkCreateInstance);
    if (string_eq(pName, "vkDestroyInstance")) return TO_VOID_PFN(test_vkDestroyInstance);
    if (string_eq(pName, "vkEnumeratePhysicalDevices")) return TO_VOID_PFN(test_vkEnumeratePhysicalDevices);
    if (string_eq(pName, "vkEnumeratePhysicalDeviceGroups") || string_eq(pName, "vkEnumeratePhysicalDeviceGroupsKHR"))
        return TO_VOID_PFN(test_vkEnumeratePhysicalDeviceGroups);

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
            return TO_VOID_PFN(func.function);
        }
    }

    return nullptr;
}

PFN_vkVoidFunction get_device_func(VkDevice device, const char* pName) {
    bool found = false;
    PhysicalDevice* found_phys_dev{};
    for (auto& phys_dev : icd.physical_devices) {
        for (auto& device_handle : phys_dev.device_handles) {
            if (device_handle == device) {
                found = true;
                found_phys_dev = &phys_dev;
                break;
            }
        }
    }
    if (!found) return nullptr;
    if (string_eq(pName, "vkDestroyDevice")) return TO_VOID_PFN(test_vkDestroyDevice);
    if (string_eq(pName, "vkCreateSwapchainKHR")) return TO_VOID_PFN(test_vkCreateSwapchainKHR);
    if (string_eq(pName, "vkDestroySwapchainKHR")) return TO_VOID_PFN(test_vkDestroySwapchainKHR);
    for (auto& function : found_phys_dev->known_device_functions) {
        if (string_eq(pName, function.name)) {
            return reinterpret_cast<PFN_vkVoidFunction>(function.function);
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
        if (string_eq(pName, "vkGetInstanceProcAddr")) return TO_VOID_PFN(test_vkGetInstanceProcAddr);
        if (string_eq(pName, "vkEnumerateInstanceExtensionProperties"))
            return TO_VOID_PFN(test_vkEnumerateInstanceExtensionProperties);
        if (string_eq(pName, "vkEnumerateInstanceLayerProperties")) return TO_VOID_PFN(test_vkEnumerateInstanceLayerProperties);
        if (string_eq(pName, "vkEnumerateInstanceVersion")) return TO_VOID_PFN(test_vkEnumerateInstanceVersion);
    }
    if (string_eq(pName, "vkGetDeviceProcAddr")) return TO_VOID_PFN(test_vkGetDeviceProcAddr);

    auto instance_func_return = get_instance_func(instance, pName);
    if (instance_func_return != nullptr) return instance_func_return;
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
    return nullptr;
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
    icd.called_enumerate_adapter_physical_devices = CalledEnumerateAdapterPhysicalDevices::called;
    return test_vkEnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);

    return VK_SUCCESS;
}
#endif  // defined(WIN32)
#endif  // TEST_ICD_EXPORT_ICD_ENUMERATE_ADAPTER_PHYSICAL_DEVICES

}  // extern "C"