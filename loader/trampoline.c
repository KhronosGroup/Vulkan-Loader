/*
 *
 * Copyright (c) 2015-2023 The Khronos Group Inc.
 * Copyright (c) 2015-2023 Valve Corporation
 * Copyright (c) 2015-2023 LunarG, Inc.
 * Copyright (C) 2015 Google Inc.
 * Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * Copyright (c) 2023-2023 RasterGrid Kft.
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
 * Author: Courtney Goeltzenleuchter <courtney@lunarg.com>
 * Author: Jon Ashburn <jon@lunarg.com>
 * Author: Tony Barbour <tony@LunarG.com>
 * Author: Chia-I Wu <olv@lunarg.com>
 * Author: Charles Giessen <charles@lunarg.com>
 */

#include <stdlib.h>
#include <string.h>

#include "allocation.h"
#include "debug_utils.h"
#include "gpa_helper.h"
#include "loader.h"
#include "loader_environment.h"
#include "log.h"
#include "settings.h"
#include "vk_loader_extensions.h"
#include "vk_loader_platform.h"

// Trampoline entrypoints are in this file for core Vulkan commands

/* vkGetInstanceProcAddr: Get global level or instance level entrypoint addresses.
 * @param instance
 * @param pName
 * @return
 *    If pName is a global level entrypoint:
 *        If instance == NULL || instance is invalid || (instance is valid && instance.minor_version <= 2):
 *            return global level functions
 *        Else:
 *            return NULL
 *    Else:
 *        If instance is valid:
 *            return a trampoline entry point for all dispatchable Vulkan functions both core and extensions.
 *        Else:
 *            return NULL
 *
 * Note:
 * Vulkan header updated 1.2.193 changed the behavior of vkGetInstanceProcAddr for global entrypoints. They used to always be
 * returned regardless of the value of the instance parameter. The spec was amended in this version to only allow querying global
 * level entrypoints with a NULL instance. However, as to not break old applications, the new behavior is only applied if the
 * instance passed in is both valid and minor version is greater than 1.2, which was when this change in behavior occurred. Only
 * instances with a newer version will get the new behavior.
 */
LOADER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance instance, const char* pName) {
    // Always should be able to get vkGetInstanceProcAddr if queried, regardless of the value of instance
    if (!strcmp(pName, "vkGetInstanceProcAddr")) return (PFN_vkVoidFunction)vkGetInstanceProcAddr;

    // Get entrypoint addresses that are global (no dispatchable object)
    void* addr = globalGetProcAddr(pName);
    if (addr != VK_NULL_HANDLE) {
        // Always can get a global entrypoint from vkGetInstanceProcAddr with a NULL instance handle
        if (instance == VK_NULL_HANDLE) {
            return addr;
        }
        // New behavior only returns a global entrypoint if the instance handle is NULL.
        // Old behavior is to return a global entrypoint regardless of the value of the instance handle.
        // Use new behavior if: The instance is valid and the minor version of the instance is greater than 1.2, which
        // was when the new behavior was added. (eg, it is enforced in the next minor version of vulkan, which will be 1.3)

        // First check if instance is valid - loader_get_instance() returns NULL if it isn't.
        struct loader_instance* ptr_instance = loader_get_instance(instance);
        if (ptr_instance != NULL &&
            loader_check_version_meets_required(loader_combine_version(1, 3, 0), ptr_instance->app_api_version)) {
            // New behavior
            return NULL;
        }
        // Old behavior
        return addr;
    }
    // All other functions require a valid instance handle to get
    if (instance == VK_NULL_HANDLE) {
        return NULL;
    }
    struct loader_instance* ptr_instance = loader_validate_and_get_instance_dispatch(instance, __FUNCTION__);

    // Return trampoline code for non-global entrypoints including any extensions.
    // Device extensions are returned if a layer or ICD supports the extension.
    // Instance extensions are returned if the extension is enabled and the
    // loader or someone else supports the extension
    return trampoline_get_proc_addr(ptr_instance, pName);
}

// Get a device level or global level entry point address.
// @param device
// @param pName
// @return
//    If device is valid, returns a device relative entry point for device level
//    entry points both core and extensions.
//    Device relative means call down the device chain.
LOADER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(VkDevice device, const char* pName) {
    if (!pName || pName[0] != 'v' || pName[1] != 'k') return NULL;

    // For entrypoints that loader must handle (ie non-dispatchable or create object)
    // make sure the loader entrypoint is returned
    const char* name = pName;
    name += 2;
    if (!strcmp(name, "GetDeviceProcAddr")) return (PFN_vkVoidFunction)vkGetDeviceProcAddr;
    if (!strcmp(name, "DestroyDevice")) return (PFN_vkVoidFunction)vkDestroyDevice;
    if (!strcmp(name, "GetDeviceQueue")) return (PFN_vkVoidFunction)vkGetDeviceQueue;
    if (!strcmp(name, "AllocateCommandBuffers")) return (PFN_vkVoidFunction)vkAllocateCommandBuffers;

    // Although CreateDevice is on device chain it's dispatchable object isn't
    // a VkDevice or child of VkDevice so return NULL.
    if (!strcmp(pName, "CreateDevice")) return NULL;

    // Because vkGetDeviceQueue2 is a 1.1 entry point, we need to check if the apiVersion provided during instance creation is
    // sufficient
    if (!strcmp(name, "GetDeviceQueue2")) {
        struct loader_device* dev = NULL;
        struct loader_icd_term* icd_term = loader_get_icd_and_device(device, &dev);
        if (NULL != icd_term && dev != NULL) {
            const struct loader_instance* inst = icd_term->this_instance;
            uint32_t api_version =
                VK_MAKE_API_VERSION(0, inst->app_api_version.major, inst->app_api_version.minor, inst->app_api_version.patch);
            return (dev->should_ignore_device_commands_from_newer_version && api_version < VK_API_VERSION_1_1)
                       ? NULL
                       : (PFN_vkVoidFunction)vkGetDeviceQueue2;
        }
        return NULL;
    }
    // Return the dispatch table entrypoint for the fastest case
    const VkLayerDispatchTable* disp_table = loader_get_dispatch(device);
    if (disp_table == NULL) return NULL;

    bool found_name = false;
    void* addr = loader_lookup_device_dispatch_table(disp_table, pName, &found_name);
    if (found_name) return addr;

    if (disp_table->GetDeviceProcAddr == NULL) return NULL;
    return disp_table->GetDeviceProcAddr(device, pName);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(const char* pLayerName,
                                                                                    uint32_t* pPropertyCount,
                                                                                    VkExtensionProperties* pProperties) {
    LOADER_PLATFORM_THREAD_ONCE(&once_init, loader_initialize);

    update_global_loader_settings();

    // We know we need to call at least the terminator
    VkResult res = VK_SUCCESS;
    VkEnumerateInstanceExtensionPropertiesChain chain_tail = {
        .header =
            {
                .type = VK_CHAIN_TYPE_ENUMERATE_INSTANCE_EXTENSION_PROPERTIES,
                .version = VK_CURRENT_CHAIN_VERSION,
                .size = sizeof(chain_tail),
            },
        .pfnNextLayer = &terminator_pre_instance_EnumerateInstanceExtensionProperties,
        .pNextLink = NULL,
    };
    VkEnumerateInstanceExtensionPropertiesChain* chain_head = &chain_tail;

    // Get the implicit layers
    struct loader_layer_list layers = {0};
    memset(&layers, 0, sizeof(layers));
    struct loader_envvar_all_filters layer_filters = {0};

    res = parse_layer_environment_var_filters(NULL, &layer_filters);
    if (VK_SUCCESS != res) {
        return res;
    }

    res = loader_scan_for_implicit_layers(NULL, &layers, &layer_filters);
    if (VK_SUCCESS != res) {
        return res;
    }

    // Prepend layers onto the chain if they implement this entry point
    for (uint32_t i = 0; i < layers.count; ++i) {
        // Skip this layer if it doesn't expose the entry-point
        if (NULL == layers.list[i].pre_instance_functions.enumerate_instance_extension_properties) {
            continue;
        }
        bool oom = false;
        loader_open_layer_file(NULL, &layers.list[i], &oom);
        if (oom) {
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }
        if (layers.list[i].lib_handle == NULL) {
            continue;
        }

        void* pfn = loader_platform_get_proc_address(layers.list[i].lib_handle,
                                                     layers.list[i].pre_instance_functions.enumerate_instance_extension_properties);
        if (pfn == NULL) {
            loader_log(NULL, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                       "%s: Unable to resolve symbol \"%s\" in implicit layer library \"%s\"", __FUNCTION__,
                       layers.list[i].pre_instance_functions.enumerate_instance_extension_properties, layers.list[i].lib_name);
            continue;
        }

        VkEnumerateInstanceExtensionPropertiesChain* chain_link =
            loader_calloc(NULL, sizeof(VkEnumerateInstanceExtensionPropertiesChain), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (chain_link == NULL) {
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            break;
        }
        memset(chain_link, 0, sizeof(VkEnumerateInstanceLayerPropertiesChain));
        chain_link->header.type = VK_CHAIN_TYPE_ENUMERATE_INSTANCE_EXTENSION_PROPERTIES;
        chain_link->header.version = VK_CURRENT_CHAIN_VERSION;
        chain_link->header.size = sizeof(*chain_link);
        chain_link->pfnNextLayer = pfn;
        chain_link->pNextLink = chain_head;

        chain_head = chain_link;
    }

    // Call down the chain
    if (res == VK_SUCCESS) {
        res = chain_head->pfnNextLayer(chain_head->pNextLink, pLayerName, pPropertyCount, pProperties);
    }

out:
    // Free up the layers
    loader_delete_layer_list_and_properties(NULL, &layers);

    // Tear down the chain
    while (chain_head != &chain_tail) {
        VkEnumerateInstanceExtensionPropertiesChain* holder = chain_head;
        chain_head = (VkEnumerateInstanceExtensionPropertiesChain*)chain_head->pNextLink;
        loader_free(NULL, holder);
    }

    return res;
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(uint32_t* pPropertyCount,
                                                                                VkLayerProperties* pProperties) {
    LOADER_PLATFORM_THREAD_ONCE(&once_init, loader_initialize);

    update_global_loader_settings();

    // We know we need to call at least the terminator
    VkResult res = VK_SUCCESS;
    VkEnumerateInstanceLayerPropertiesChain chain_tail = {
        .header =
            {
                .type = VK_CHAIN_TYPE_ENUMERATE_INSTANCE_LAYER_PROPERTIES,
                .version = VK_CURRENT_CHAIN_VERSION,
                .size = sizeof(chain_tail),
            },
        .pfnNextLayer = &terminator_pre_instance_EnumerateInstanceLayerProperties,
        .pNextLink = NULL,
    };
    VkEnumerateInstanceLayerPropertiesChain* chain_head = &chain_tail;

    // Get the implicit layers
    struct loader_layer_list layers;
    memset(&layers, 0, sizeof(layers));
    struct loader_envvar_all_filters layer_filters = {0};

    res = parse_layer_environment_var_filters(NULL, &layer_filters);
    if (VK_SUCCESS != res) {
        return res;
    }

    res = loader_scan_for_implicit_layers(NULL, &layers, &layer_filters);
    if (VK_SUCCESS != res) {
        return res;
    }

    // Prepend layers onto the chain if they implement this entry point
    for (uint32_t i = 0; i < layers.count; ++i) {
        // Skip this layer if it doesn't expose the entry-point
        if (NULL == layers.list[i].pre_instance_functions.enumerate_instance_layer_properties) {
            continue;
        }

        bool oom = false;
        loader_open_layer_file(NULL, &layers.list[i], &oom);
        if (oom) {
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }
        if (layers.list[i].lib_handle == NULL) {
            continue;
        }

        void* pfn = loader_platform_get_proc_address(layers.list[i].lib_handle,
                                                     layers.list[i].pre_instance_functions.enumerate_instance_layer_properties);
        if (pfn == NULL) {
            loader_log(NULL, VULKAN_LOADER_WARN_BIT, 0, "%s: Unable to resolve symbol \"%s\" in implicit layer library \"%s\"",
                       __FUNCTION__, layers.list[i].pre_instance_functions.enumerate_instance_layer_properties,
                       layers.list[i].lib_name);
            continue;
        }

        VkEnumerateInstanceLayerPropertiesChain* chain_link =
            loader_calloc(NULL, sizeof(VkEnumerateInstanceLayerPropertiesChain), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (chain_link == NULL) {
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            break;
        }
        memset(chain_link, 0, sizeof(VkEnumerateInstanceLayerPropertiesChain));
        chain_link->header.type = VK_CHAIN_TYPE_ENUMERATE_INSTANCE_LAYER_PROPERTIES;
        chain_link->header.version = VK_CURRENT_CHAIN_VERSION;
        chain_link->header.size = sizeof(*chain_link);
        chain_link->pfnNextLayer = pfn;
        chain_link->pNextLink = chain_head;

        chain_head = chain_link;
    }

    // Call down the chain
    if (res == VK_SUCCESS) {
        res = chain_head->pfnNextLayer(chain_head->pNextLink, pPropertyCount, pProperties);
    }

out:
    // Free up the layers
    loader_delete_layer_list_and_properties(NULL, &layers);

    // Tear down the chain
    while (chain_head != &chain_tail) {
        VkEnumerateInstanceLayerPropertiesChain* holder = chain_head;
        chain_head = (VkEnumerateInstanceLayerPropertiesChain*)chain_head->pNextLink;
        loader_free(NULL, holder);
    }

    return res;
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceVersion(uint32_t* pApiVersion) {
    LOADER_PLATFORM_THREAD_ONCE(&once_init, loader_initialize);

    update_global_loader_settings();

    if (NULL == pApiVersion) {
        loader_log(NULL, VULKAN_LOADER_FATAL_ERROR_BIT | VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkEnumerateInstanceVersion: \'pApiVersion\' must not be NULL "
                   "(VUID-vkEnumerateInstanceVersion-pApiVersion-parameter");
        // NOTE: This seems silly, but it's the only allowable failure
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    // We know we need to call at least the terminator
    VkResult res = VK_SUCCESS;
    VkEnumerateInstanceVersionChain chain_tail = {
        .header =
            {
                .type = VK_CHAIN_TYPE_ENUMERATE_INSTANCE_VERSION,
                .version = VK_CURRENT_CHAIN_VERSION,
                .size = sizeof(chain_tail),
            },
        .pfnNextLayer = &terminator_pre_instance_EnumerateInstanceVersion,
        .pNextLink = NULL,
    };
    VkEnumerateInstanceVersionChain* chain_head = &chain_tail;

    // Get the implicit layers
    struct loader_layer_list layers;
    memset(&layers, 0, sizeof(layers));
    struct loader_envvar_all_filters layer_filters = {0};

    res = parse_layer_environment_var_filters(NULL, &layer_filters);
    if (VK_SUCCESS != res) {
        return res;
    }

    res = loader_scan_for_implicit_layers(NULL, &layers, &layer_filters);
    if (VK_SUCCESS != res) {
        return res;
    }

    // Prepend layers onto the chain if they implement this entry point
    for (uint32_t i = 0; i < layers.count; ++i) {
        // Skip this layer if it doesn't expose the entry-point
        if (NULL == layers.list[i].pre_instance_functions.enumerate_instance_version) {
            continue;
        }

        bool oom = false;
        loader_open_layer_file(NULL, &layers.list[i], &oom);
        if (oom) {
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }
        if (layers.list[i].lib_handle == NULL) {
            continue;
        }

        void* pfn = loader_platform_get_proc_address(layers.list[i].lib_handle,
                                                     layers.list[i].pre_instance_functions.enumerate_instance_version);
        if (pfn == NULL) {
            loader_log(NULL, VULKAN_LOADER_WARN_BIT, 0, "%s: Unable to resolve symbol \"%s\" in implicit layer library \"%s\"",
                       __FUNCTION__, layers.list[i].pre_instance_functions.enumerate_instance_version, layers.list[i].lib_name);
            continue;
        }

        VkEnumerateInstanceVersionChain* chain_link =
            loader_calloc(NULL, sizeof(VkEnumerateInstanceVersionChain), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (chain_link == NULL) {
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            break;
        }
        memset(chain_link, 0, sizeof(VkEnumerateInstanceLayerPropertiesChain));
        chain_link->header.type = VK_CHAIN_TYPE_ENUMERATE_INSTANCE_VERSION;
        chain_link->header.version = VK_CURRENT_CHAIN_VERSION;
        chain_link->header.size = sizeof(*chain_link);
        chain_link->pfnNextLayer = pfn;
        chain_link->pNextLink = chain_head;

        chain_head = chain_link;
    }

    // Call down the chain
    if (res == VK_SUCCESS) {
        res = chain_head->pfnNextLayer(chain_head->pNextLink, pApiVersion);
    }

out:
    // Free up the layers
    loader_delete_layer_list_and_properties(NULL, &layers);

    // Tear down the chain
    while (chain_head != &chain_tail) {
        VkEnumerateInstanceVersionChain* holder = chain_head;
        chain_head = (VkEnumerateInstanceVersionChain*)chain_head->pNextLink;
        loader_free(NULL, holder);
    }

    return res;
}

// Add the "instance-only" debug functions to the list of active debug functions
// at the very end.  This way it doesn't get replaced by any new messengers
void loader_add_instance_only_debug_funcs(struct loader_instance* ptr_instance) {
    VkLayerDbgFunctionNode* cur_node = ptr_instance->current_dbg_function_head;
    if (cur_node == NULL) {
        ptr_instance->current_dbg_function_head = ptr_instance->instance_only_dbg_function_head;
    } else {
        while (cur_node != NULL) {
            if (cur_node == ptr_instance->instance_only_dbg_function_head) {
                // Already added
                break;
            }
            // Last item
            if (cur_node->pNext == NULL) {
                cur_node->pNext = ptr_instance->instance_only_dbg_function_head;
            }
            cur_node = cur_node->pNext;
        }
    }
}

// Remove the "instance-only" debug functions from the list of active debug functions.
// It should be added after the last actual debug utils/debug report function.
void loader_remove_instance_only_debug_funcs(struct loader_instance* ptr_instance) {
    VkLayerDbgFunctionNode* cur_node = ptr_instance->current_dbg_function_head;

    // Only thing in list is the instance only head
    if (cur_node == ptr_instance->instance_only_dbg_function_head) {
        ptr_instance->current_dbg_function_head = NULL;
    }
    while (cur_node != NULL) {
        if (cur_node->pNext == ptr_instance->instance_only_dbg_function_head) {
            cur_node->pNext = NULL;
            break;
        }
        cur_node = cur_node->pNext;
    }
}

// Dump the app's VkInstanceCreateInfo under VK_LOADER_DEBUG (names, versions, requested layers/extensions). Handy
// for debugging instance setup and matching driver workarounds to a specific app.
static void loader_log_instance_create_info(const struct loader_instance* inst, const VkInstanceCreateInfo* pCreateInfo) {
    const VkApplicationInfo* app_info = pCreateInfo->pApplicationInfo;
    const char* app_name = (app_info && app_info->pApplicationName) ? app_info->pApplicationName : "";
    const char* engine_name = (app_info && app_info->pEngineName) ? app_info->pEngineName : "";
    uint32_t app_version = app_info ? app_info->applicationVersion : 0;
    uint32_t engine_version = app_info ? app_info->engineVersion : 0;
    uint32_t api_version = app_info ? app_info->apiVersion : 0;

    loader_log(inst, VULKAN_LOADER_INFO_BIT, 0,
               "vkCreateInstance: applicationName: \"%s\", applicationVersion: %u, engineName: \"%s\", engineVersion: %u, "
               "apiVersion: %u.%u.%u",
               app_name, app_version, engine_name, engine_version, VK_API_VERSION_MAJOR(api_version),
               VK_API_VERSION_MINOR(api_version), VK_API_VERSION_PATCH(api_version));

    // Guard the name arrays: this runs before the loader validates pCreateInfo, so the count may be non-zero with a null
    // array (see the null-pointer tests).
    loader_log(inst, VULKAN_LOADER_INFO_BIT, 0,
               "vkCreateInstance: Requested %u instance layer(s):", pCreateInfo->enabledLayerCount);
    if (pCreateInfo->ppEnabledLayerNames) {
        for (uint32_t i = 0; i < pCreateInfo->enabledLayerCount; ++i) {
            if (pCreateInfo->ppEnabledLayerNames[i]) {
                loader_log(inst, VULKAN_LOADER_INFO_BIT, 0, "   %s", pCreateInfo->ppEnabledLayerNames[i]);
            } else {
                loader_log(inst, VULKAN_LOADER_INFO_BIT, 0, "   <NULL>");
            }
        }
    }

    loader_log(inst, VULKAN_LOADER_INFO_BIT, 0,
               "vkCreateInstance: Requested %u instance extension(s):", pCreateInfo->enabledExtensionCount);
    if (pCreateInfo->ppEnabledExtensionNames) {
        for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; ++i) {
            if (pCreateInfo->ppEnabledExtensionNames[i]) {
                loader_log(inst, VULKAN_LOADER_INFO_BIT, 0, "   %s", pCreateInfo->ppEnabledExtensionNames[i]);
            } else {
                loader_log(inst, VULKAN_LOADER_INFO_BIT, 0, "   <NULL>");
            }
        }
    }
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo,
                                                              const VkAllocationCallbacks* pAllocator, VkInstance* pInstance) {
    struct loader_instance* ptr_instance = NULL;
    VkInstance created_instance = VK_NULL_HANDLE;
    VkResult res = VK_ERROR_INITIALIZATION_FAILED;
    VkInstanceCreateInfo ici = {0};
    bool portability_enumeration_flag_bit_set = false;
    bool portability_enumeration_extension_enabled = false;
    struct loader_envvar_all_filters layer_filters = {0};

    LOADER_PLATFORM_THREAD_ONCE(&once_init, loader_initialize);

    if (pCreateInfo == NULL) {
        loader_log(NULL, VULKAN_LOADER_FATAL_ERROR_BIT | VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateInstance: \'pCreateInfo\' is NULL (VUID-vkCreateInstance-pCreateInfo-parameter)");
        goto out;
    }
    ici = *pCreateInfo;

    if (pInstance == NULL) {
        loader_log(NULL, VULKAN_LOADER_FATAL_ERROR_BIT | VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateInstance \'pInstance\' not valid (VUID-vkCreateInstance-pInstance-parameter)");
        goto out;
    }

    ptr_instance =
        (struct loader_instance*)loader_calloc(pAllocator, sizeof(struct loader_instance), VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);

    if (ptr_instance == NULL) {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    loader_platform_thread_lock_mutex(&loader_lock);
    if (pAllocator) {
        ptr_instance->alloc_callbacks = *pAllocator;
    }
    ptr_instance->magic = LOADER_MAGIC_NUMBER;

    // Save the application version
    if (NULL == pCreateInfo->pApplicationInfo || 0 == pCreateInfo->pApplicationInfo->apiVersion) {
        ptr_instance->app_api_version = LOADER_VERSION_1_0_0;
    } else {
        ptr_instance->app_api_version = loader_make_version(pCreateInfo->pApplicationInfo->apiVersion);
    }

    // Look for one or more VK_EXT_debug_report or VK_EXT_debug_utils create info structures
    // and setup a callback(s) for each one found.

    // Handle cases of VK_EXT_debug_utils
    // Setup the temporary messenger(s) here to catch early issues:
    res = util_CreateDebugUtilsMessengers(ptr_instance, pCreateInfo->pNext, pAllocator);
    if (VK_ERROR_OUT_OF_HOST_MEMORY == res) {
        // Failure of setting up one or more of the messenger.
        goto out;
    }

    // Handle cases of VK_EXT_debug_report
    // Setup the temporary callback(s) here to catch early issues:
    res = util_CreateDebugReportCallbacks(ptr_instance, pCreateInfo->pNext, pAllocator);
    if (VK_ERROR_OUT_OF_HOST_MEMORY == res) {
        // Failure of setting up one or more of the callback.
        goto out;
    }

    loader_log_instance_create_info(ptr_instance, pCreateInfo);

    VkResult settings_file_res = get_loader_settings(ptr_instance, &ptr_instance->settings);
    if (settings_file_res == VK_ERROR_OUT_OF_HOST_MEMORY) {
        res = settings_file_res;
        goto out;
    }
    if (ptr_instance->settings.settings_active) {
        log_settings(ptr_instance, &ptr_instance->settings);
    }

    // Providing an apiVersion less than VK_API_VERSION_1_0 but greater than zero prevents the validation layers from starting
    if (pCreateInfo->pApplicationInfo && pCreateInfo->pApplicationInfo->apiVersion != 0u &&
        pCreateInfo->pApplicationInfo->apiVersion < VK_API_VERSION_1_0) {
        loader_log(ptr_instance, VULKAN_LOADER_FATAL_ERROR_BIT | VULKAN_LOADER_ERROR_BIT, 0,
                   "VkInstanceCreateInfo::pApplicationInfo::apiVersion has value of %u which is not permitted. If apiVersion is "
                   "not 0, then it must be "
                   "greater than or equal to the value of VK_API_VERSION_1_0 [VUID-VkApplicationInfo-apiVersion]",
                   pCreateInfo->pApplicationInfo->apiVersion);
    }

    // Check the VkInstanceCreateInfoFlags wether to allow the portability enumeration flag
    if ((pCreateInfo->flags & VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR) == 1) {
        portability_enumeration_flag_bit_set = true;
    }
    // Make sure the portability extension extension has been enabled before enabling portability driver enumeration
    if (pCreateInfo->ppEnabledExtensionNames) {
        for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; i++) {
            if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0) {
                portability_enumeration_extension_enabled = true;
                if (portability_enumeration_flag_bit_set) {
                    ptr_instance->portability_enumeration_enabled = true;
                    loader_log(ptr_instance, VULKAN_LOADER_INFO_BIT, 0,
                               "Portability enumeration bit was set, enumerating portability drivers.");
                }
                break;
            }
        }
    }

    // Make sure the application provided API version has the correct variant
    if (NULL != pCreateInfo->pApplicationInfo) {
        uint32_t variant_version = VK_API_VERSION_VARIANT(pCreateInfo->pApplicationInfo->apiVersion);
        const uint32_t expected_variant = 0;
        if (expected_variant != variant_version) {
            loader_log(ptr_instance, VULKAN_LOADER_WARN_BIT, 0,
                       "vkCreateInstance: The API Variant specified in pCreateInfo->pApplicationInfo.apiVersion is %d instead of "
                       "the expected value of %d.",
                       variant_version, expected_variant);
        }
    }

    res = parse_layer_environment_var_filters(ptr_instance, &layer_filters);
    if (VK_SUCCESS != res) {
        goto out;
    }

    // Due to implicit layers need to get layer list even if
    // enabledLayerCount == 0 and VK_INSTANCE_LAYERS is unset. For now always
    // get layer list via loader_scan_for_layers().
    memset(&ptr_instance->instance_layer_list, 0, sizeof(ptr_instance->instance_layer_list));
    res = loader_scan_for_layers(ptr_instance, &ptr_instance->instance_layer_list, &layer_filters);
    if (VK_SUCCESS != res) {
        goto out;
    }

    // Validate the app requested layers to be enabled
    if (pCreateInfo->enabledLayerCount > 0) {
        res = loader_validate_layers(ptr_instance, pCreateInfo->enabledLayerCount, pCreateInfo->ppEnabledLayerNames,
                                     &ptr_instance->instance_layer_list);
        if (res != VK_SUCCESS) {
            goto out;
        }
    }

    // Scan/discover all System and Environment Variable ICD libraries
    bool skipped_portability_drivers = false;
    res = loader_icd_scan(ptr_instance, &ptr_instance->icd_tramp_list, pCreateInfo, &skipped_portability_drivers);
    if (res == VK_ERROR_OUT_OF_HOST_MEMORY) {
        goto out;
    }

    if (ptr_instance->icd_tramp_list.count == 0) {
        // No drivers found
        if (skipped_portability_drivers) {
            if (portability_enumeration_extension_enabled && !portability_enumeration_flag_bit_set) {
                loader_log(ptr_instance, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                           "vkCreateInstance: Found drivers that contain devices which support the portability subset, but "
                           "the instance does not enumerate portability drivers! Applications that wish to enumerate portability "
                           "drivers must set the VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR bit in the VkInstanceCreateInfo "
                           "flags.");
            } else if (portability_enumeration_flag_bit_set && !portability_enumeration_extension_enabled) {
                loader_log(ptr_instance, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                           "VkInstanceCreateInfo: If flags has the VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR bit set, the "
                           "list of enabled extensions in ppEnabledExtensionNames must contain VK_KHR_portability_enumeration "
                           "[VUID-VkInstanceCreateInfo-flags-06559 ]"
                           "Applications that wish to enumerate portability drivers must enable the VK_KHR_portability_enumeration "
                           "instance extension.");
            } else {
                loader_log(ptr_instance, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                           "vkCreateInstance: Found drivers that contain devices which support the portability subset, but "
                           "the instance does not enumerate portability drivers! Applications that wish to enumerate portability "
                           "drivers must set the VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR bit in the VkInstanceCreateInfo "
                           "flags and enable the VK_KHR_portability_enumeration instance extension.");
            }
        }
        loader_log(ptr_instance, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_DRIVER_BIT, 0, "vkCreateInstance: Found no drivers!");
        res = VK_ERROR_INCOMPATIBLE_DRIVER;
        goto out;
    }

    // Get extensions from all ICD's, merge so no duplicates, then validate
    res = loader_get_icd_loader_instance_extensions(ptr_instance, &ptr_instance->icd_tramp_list, &ptr_instance->ext_list);
    if (res != VK_SUCCESS) {
        goto out;
    }

    ptr_instance->disp = loader_instance_heap_calloc(ptr_instance, sizeof(struct loader_instance_dispatch_table),
                                                     VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    if (ptr_instance->disp == NULL) {
        loader_log(ptr_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "vkCreateInstance:  Failed to allocate Loader's full Instance dispatch table.");
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }
    memcpy(&ptr_instance->disp->layer_inst_disp, &instance_disp, sizeof(instance_disp));

    ptr_instance->next = loader.instances;
    loader.instances = ptr_instance;

    // Activate any layers on instance chain
    res = loader_enable_instance_layers(ptr_instance, &ici, &layer_filters);
    if (res != VK_SUCCESS) {
        goto out;
    }

    res = loader_validate_instance_extensions(ptr_instance, &ptr_instance->ext_list, &ptr_instance->instance_layer_list,
                                              &ptr_instance->expanded_activated_layer_list, &ici);
    if (res != VK_SUCCESS) {
        goto out;
    }

    created_instance = (VkInstance)ptr_instance;
    res = loader_create_instance_chain(&ici, pAllocator, ptr_instance, &created_instance);

    if (VK_SUCCESS == res) {
        // Check for enabled extensions here to setup the loader structures so the loader knows what extensions
        // it needs to worry about.
        // We do it in the terminator and again above the layers here since we may think different extensions
        // are enabled than what's down in the terminator.
        // This is why we don't clear inside of these function calls.
        // The clearing should actually be handled by the overall memset of the pInstance structure above.
        fill_out_enabled_instance_extensions(ici.enabledExtensionCount, ici.ppEnabledExtensionNames,
                                             &ptr_instance->enabled_extensions);

        *pInstance = (VkInstance)ptr_instance;

        // Finally have the layers in place and everyone has seen
        // the CreateInstance command go by. This allows the layer's
        // GetInstanceProcAddr functions to return valid extension functions
        // if enabled.
        loader_activate_instance_layer_extensions(ptr_instance, created_instance);
        ptr_instance->instance_finished_creation = true;
    } else if (VK_ERROR_EXTENSION_NOT_PRESENT == res && !ptr_instance->create_terminator_invalid_extension) {
        loader_log(ptr_instance, VULKAN_LOADER_WARN_BIT, 0,
                   "vkCreateInstance: Layer returning invalid extension error not triggered by ICD/Loader (Policy #LLP_LAYER_17).");
    }

out:

    if (NULL != ptr_instance) {
        if (res != VK_SUCCESS) {
            // error path, should clean everything up
            if (loader.instances == ptr_instance) {
                loader.instances = ptr_instance->next;
            }

            free_loader_settings(ptr_instance, &ptr_instance->settings);

            loader_destroy_generic_list(ptr_instance, (struct loader_generic_list*)&ptr_instance->surfaces_list);
            loader_destroy_generic_list(ptr_instance, (struct loader_generic_list*)&ptr_instance->debug_utils_messengers_list);
            loader_destroy_generic_list(ptr_instance, (struct loader_generic_list*)&ptr_instance->debug_report_callbacks_list);

            loader_instance_heap_free(ptr_instance, ptr_instance->disp);
            // Remove any created VK_EXT_debug_report or VK_EXT_debug_utils items
            destroy_debug_callbacks_chain(ptr_instance, pAllocator);

            loader_destroy_pointer_layer_list(ptr_instance, &ptr_instance->expanded_activated_layer_list);

            loader_delete_layer_list_and_properties(ptr_instance, &ptr_instance->instance_layer_list);
            loader_destroy_generic_list(ptr_instance, (struct loader_generic_list*)&ptr_instance->ext_list);

            // Free any icd_terms that were created.
            // If an OOM occurs from a layer, terminator_CreateInstance won't be reached where this kind of
            // cleanup normally occurs
            struct loader_icd_term* icd_term = NULL;
            while (NULL != ptr_instance->icd_terms) {
                icd_term = ptr_instance->icd_terms;
                // Call destroy Instance on each driver in case we successfully called down the chain but failed on
                // our way back out of it.
                if (icd_term->instance) {
                    loader_icd_close_objects(ptr_instance, icd_term);
                    icd_term->dispatch.DestroyInstance(icd_term->instance, pAllocator);
                }
                icd_term->instance = VK_NULL_HANDLE;
                ptr_instance->icd_terms = icd_term->next;
                loader_icd_destroy(ptr_instance, icd_term, pAllocator);
            }

            loader_clear_scanned_icd_list(ptr_instance, &ptr_instance->icd_tramp_list);
            free_string_list(ptr_instance, &ptr_instance->enabled_layer_names);

            loader_instance_heap_free(ptr_instance, ptr_instance);
        } else {
            // success path, swap out created debug callbacks out so they aren't used until instance destruction
            loader_remove_instance_only_debug_funcs(ptr_instance);
        }
        // Only unlock when ptr_instance isn't NULL, as if it is, the above code didn't make it to when loader_lock was locked.
        loader_platform_thread_unlock_mutex(&loader_lock);
    }

    return res;
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator) {
    const VkLayerInstanceDispatchTable* disp;
    struct loader_instance* ptr_instance = NULL;

    if (instance == VK_NULL_HANDLE) {
        return;
    }
    loader_platform_thread_lock_mutex(&loader_lock);

    ptr_instance = loader_validate_and_get_instance_dispatch(instance, __FUNCTION__);

    if (pAllocator) {
        ptr_instance->alloc_callbacks = *pAllocator;
    }

    // Remove any callbacks that weren't cleaned up by the application
    destroy_debug_callbacks_chain(ptr_instance, pAllocator);

    // Swap in the debug callbacks created during instance creation
    loader_add_instance_only_debug_funcs(ptr_instance);

    disp = loader_get_instance_layer_dispatch(instance);
    disp->DestroyInstance(ptr_instance->instance, pAllocator);

    free_loader_settings(ptr_instance, &ptr_instance->settings);

    loader_destroy_generic_list(ptr_instance, (struct loader_generic_list*)&ptr_instance->surfaces_list);
    loader_destroy_generic_list(ptr_instance, (struct loader_generic_list*)&ptr_instance->debug_utils_messengers_list);
    loader_destroy_generic_list(ptr_instance, (struct loader_generic_list*)&ptr_instance->debug_report_callbacks_list);

    loader_destroy_pointer_layer_list(ptr_instance, &ptr_instance->expanded_activated_layer_list);

    loader_delete_layer_list_and_properties(ptr_instance, &ptr_instance->instance_layer_list);

    free_string_list(ptr_instance, &ptr_instance->enabled_layer_names);

    if (ptr_instance->phys_devs_tramp) {
        for (uint32_t i = 0; i < ptr_instance->phys_dev_count_tramp; i++) {
            loader_instance_heap_free(ptr_instance, ptr_instance->phys_devs_tramp[i]);
        }
        loader_instance_heap_free(ptr_instance, (void*)ptr_instance->phys_devs_tramp);
    }

    // Destroy the debug callbacks created during instance creation
    destroy_debug_callbacks_chain(ptr_instance, pAllocator);

    loader_instance_heap_free(ptr_instance, ptr_instance->disp);
    loader_instance_heap_free(ptr_instance, ptr_instance);
    loader_platform_thread_unlock_mutex(&loader_lock);

    // Unload preloaded layers, so if vkEnumerateInstanceExtensionProperties or vkCreateInstance is called again, the ICD's are
    // up to date
    loader_unload_preloaded_icds();
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount,
                                                                        VkPhysicalDevice* pPhysicalDevices) {
    VkResult res = VK_SUCCESS;
    struct loader_instance* inst;
    VkPhysicalDevice* physical_devices = NULL;

    struct loader_envvar_id_filter device_id_filter = {0, NULL};
    struct loader_envvar_id_filter vendor_id_filter = {0, NULL};
    struct loader_envvar_id_filter driver_id_filter = {0, NULL};

    loader_platform_thread_lock_mutex(&loader_lock);

    inst = loader_validate_and_get_instance_dispatch(instance, __FUNCTION__);

    if (NULL == pPhysicalDeviceCount) {
        loader_log(inst, VULKAN_LOADER_FATAL_ERROR_BIT | VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkEnumeratePhysicalDevices: Received NULL pointer for physical device count return value. "
                   "[VUID-vkEnumeratePhysicalDevices-pPhysicalDeviceCount-parameter]");
        res = VK_ERROR_INITIALIZATION_FAILED;
        goto out;
    }

    res = parse_id_filter_environment_var(inst, VK_DEVICE_ID_FILTER_ENV_VAR, &device_id_filter);
    if (VK_ERROR_OUT_OF_HOST_MEMORY == res) goto out;
    res = parse_id_filter_environment_var(inst, VK_VENDOR_ID_FILTER_ENV_VAR, &vendor_id_filter);
    if (VK_ERROR_OUT_OF_HOST_MEMORY == res) goto out;
    res = parse_id_filter_environment_var(inst, VK_DRIVER_ID_FILTER_ENV_VAR, &driver_id_filter);
    if (VK_ERROR_OUT_OF_HOST_MEMORY == res) goto out;

    // Call down the chain to get the physical device info
    if ((0 == device_id_filter.count) && (0 == vendor_id_filter.count) && (0 == driver_id_filter.count)) {
        res = inst->disp->layer_inst_disp.EnumeratePhysicalDevices(inst->instance, pPhysicalDeviceCount, pPhysicalDevices);
    } else {
        uint32_t physical_device_count = 0;
        res = inst->disp->layer_inst_disp.EnumeratePhysicalDevices(inst->instance, &physical_device_count, NULL);
        if (res != VK_SUCCESS || physical_device_count == 0) {
            *pPhysicalDeviceCount = 0;
            goto out;
        }

        physical_devices = (VkPhysicalDevice*)loader_instance_heap_calloc(inst, physical_device_count * sizeof(VkPhysicalDevice),
                                                                          VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (NULL == physical_devices) {
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }

        res = inst->disp->layer_inst_disp.EnumeratePhysicalDevices(inst->instance, &physical_device_count, physical_devices);
        if (res != VK_SUCCESS) {
            goto out;
        }

        res = loader_filter_enumerated_physical_devices(inst, &device_id_filter, &vendor_id_filter, &driver_id_filter,
                                                        physical_device_count, physical_devices, pPhysicalDeviceCount,
                                                        pPhysicalDevices);
    }

    if (NULL != pPhysicalDevices && (VK_SUCCESS == res || VK_INCOMPLETE == res)) {
        // Wrap the PhysDev object for loader usage, return wrapped objects
        VkResult update_res = setup_loader_tramp_phys_devs(inst, *pPhysicalDeviceCount, pPhysicalDevices);
        if (VK_SUCCESS != update_res) {
            res = update_res;
        }

        // Unloads any drivers that do not expose any physical devices - should save some address space
        unload_drivers_without_physical_devices(inst);
    }

out:
    loader_instance_heap_free(inst, (void*)physical_devices);

    free_id_filters(inst, &device_id_filter);
    free_id_filters(inst, &vendor_id_filter);
    free_id_filters(inst, &driver_id_filter);

    loader_platform_thread_unlock_mutex(&loader_lock);

    return res;
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo,
                                                            const VkAllocationCallbacks* pAllocator, VkDevice* pDevice) {
    // Discard result, just want the validation of the function
    loader_unwrap_physical_device(physicalDevice, __FUNCTION__);
    return loader_layer_create_device(NULL, physicalDevice, pCreateInfo, pAllocator, pDevice, NULL, NULL);
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator) {
    if (device == VK_NULL_HANDLE) {
        return;
    }
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, "vkDestroyDevice");
    loader_layer_destroy_device(device, pAllocator, disp->DestroyDevice);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice,
                                                                                  const char* pLayerName, uint32_t* pPropertyCount,
                                                                                  VkExtensionProperties* pProperties) {
    VkResult res = VK_SUCCESS;
    const VkLayerInstanceDispatchTable* disp;
    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);

    loader_platform_thread_lock_mutex(&loader_lock);

    // always pass this call down the instance chain which will terminate
    // in the ICD. This allows layers to filter the extensions coming back
    // up the chain. In the terminator we look up layer extensions from the
    // manifest file if it wasn't provided by the layer itself.
    disp = loader_get_instance_layer_dispatch(physicalDevice);
    res = disp->EnumerateDeviceExtensionProperties(unwrapped_phys_dev, pLayerName, pPropertyCount, pProperties);

    loader_platform_thread_unlock_mutex(&loader_lock);
    return res;
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice,
                                                                              uint32_t* pPropertyCount,
                                                                              VkLayerProperties* pProperties) {
    uint32_t copy_size;
    loader_platform_thread_lock_mutex(&loader_lock);

    // Don't dispatch this call down the instance chain, want all device layers
    // enumerated and instance chain may not contain all device layers
    // TODO re-evaluate the above statement we maybe able to start calling
    // down the chain

    // Discard result, use the function to validate physicalDevice
    loader_unwrap_physical_device(physicalDevice, __FUNCTION__);

    const struct loader_instance* inst = ((struct loader_physical_device_tramp*)physicalDevice)->this_instance;

    // Only count layers that successfully loaded
    uint32_t count = 0;
    for (uint32_t i = 0; i < inst->expanded_activated_layer_list.count; i++) {
        if (inst->expanded_activated_layer_list.list[i]->lib_status == LOADER_LAYER_LIB_SUCCESS_LOADED) {
            count++;
        }
    }

    if (count == 0 || pProperties == NULL) {
        *pPropertyCount = count;
        loader_platform_thread_unlock_mutex(&loader_lock);
        return VK_SUCCESS;
    }

    copy_size = (*pPropertyCount < count) ? *pPropertyCount : count;
    uint32_t propertiesIter = 0;
    for (uint32_t i = 0; i < inst->expanded_activated_layer_list.count; i++) {
        if (propertiesIter < copy_size &&
            (inst->expanded_activated_layer_list.list[i]->lib_status == LOADER_LAYER_LIB_NOT_LOADED ||
             inst->expanded_activated_layer_list.list[i]->lib_status == LOADER_LAYER_LIB_SUCCESS_LOADED)) {
            memcpy(&pProperties[propertiesIter], &(inst->expanded_activated_layer_list.list[i]->info), sizeof(VkLayerProperties));
            propertiesIter++;
        }
    }
    *pPropertyCount = copy_size;

    if (copy_size < count) {
        loader_platform_thread_unlock_mutex(&loader_lock);
        return VK_INCOMPLETE;
    }

    loader_platform_thread_unlock_mutex(&loader_lock);
    return VK_SUCCESS;
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex,
                                                          VkQueue* pQueue) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->GetDeviceQueue(device, queueFamilyIndex, queueIndex, pQueue);
    if (pQueue != NULL && *pQueue != NULL) {
        loader_set_dispatch(*pQueue, disp);
    }
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkAllocateCommandBuffers(VkDevice device,
                                                                      const VkCommandBufferAllocateInfo* pAllocateInfo,
                                                                      VkCommandBuffer* pCommandBuffers) {
    VkResult res;
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);

    res = disp->AllocateCommandBuffers(device, pAllocateInfo, pCommandBuffers);
    if (res == VK_SUCCESS) {
        for (uint32_t i = 0; i < pAllocateInfo->commandBufferCount; i++) {
            if (pCommandBuffers[i]) {
                loader_set_dispatch(pCommandBuffers[i], disp);
            }
        }
    }

    return res;
}

// ---- Vulkan core 1.1 trampolines

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDeviceGroups(
    VkInstance instance, uint32_t* pPhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties) {
    VkResult res = VK_SUCCESS;
    struct loader_instance* inst = NULL;
    VkPhysicalDeviceGroupProperties* physical_device_group_properties = NULL;

    struct loader_envvar_id_filter device_id_filter = {0, NULL};
    struct loader_envvar_id_filter vendor_id_filter = {0, NULL};
    struct loader_envvar_id_filter driver_id_filter = {0, NULL};

    loader_platform_thread_lock_mutex(&loader_lock);

    inst = loader_validate_and_get_instance_dispatch(instance, __FUNCTION__);

    if (NULL == pPhysicalDeviceGroupCount) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "vkEnumeratePhysicalDeviceGroups: Received NULL pointer for physical "
                   "device group count return value.");
        res = VK_ERROR_INITIALIZATION_FAILED;
        goto out;
    }

    res = parse_id_filter_environment_var(inst, VK_DEVICE_ID_FILTER_ENV_VAR, &device_id_filter);
    if (VK_ERROR_OUT_OF_HOST_MEMORY == res) goto out;
    res = parse_id_filter_environment_var(inst, VK_VENDOR_ID_FILTER_ENV_VAR, &vendor_id_filter);
    if (VK_ERROR_OUT_OF_HOST_MEMORY == res) goto out;
    res = parse_id_filter_environment_var(inst, VK_DRIVER_ID_FILTER_ENV_VAR, &driver_id_filter);
    if (VK_ERROR_OUT_OF_HOST_MEMORY == res) goto out;

    // Call down the chain to get the physical device group info.
    if ((0 == device_id_filter.count) && (0 == vendor_id_filter.count) && (0 == driver_id_filter.count)) {
        res = inst->disp->layer_inst_disp.EnumeratePhysicalDeviceGroups(inst->instance, pPhysicalDeviceGroupCount,
                                                                        pPhysicalDeviceGroupProperties);
    } else {
        uint32_t physical_device_group_count = 0;
        res = inst->disp->layer_inst_disp.EnumeratePhysicalDeviceGroups(inst->instance, &physical_device_group_count, NULL);
        if (res != VK_SUCCESS || physical_device_group_count == 0) {
            *pPhysicalDeviceGroupCount = 0;
            goto out;
        }

        physical_device_group_properties = loader_instance_heap_calloc(
            inst, physical_device_group_count * sizeof(VkPhysicalDeviceGroupProperties), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (NULL == physical_device_group_properties) {
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }

        for (uint32_t i = 0; i < physical_device_group_count; i++) {
            physical_device_group_properties[i].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
        }

        res = inst->disp->layer_inst_disp.EnumeratePhysicalDeviceGroups(inst->instance, &physical_device_group_count,
                                                                        physical_device_group_properties);
        if (res != VK_SUCCESS) {
            goto out;
        }

        res = loader_filter_enumerated_physical_device_groups(inst, &device_id_filter, &vendor_id_filter, &driver_id_filter,
                                                              physical_device_group_count, physical_device_group_properties,
                                                              pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
    }

    if (NULL != pPhysicalDeviceGroupProperties && (VK_SUCCESS == res || VK_INCOMPLETE == res)) {
        // Wrap the PhysDev object for loader usage, return wrapped objects
        VkResult update_res = setup_loader_tramp_phys_dev_groups(inst, *pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
        if (VK_SUCCESS != update_res) {
            res = update_res;
        }
    }

out:
    loader_instance_heap_free(inst, physical_device_group_properties);

    free_id_filters(inst, &device_id_filter);
    free_id_filters(inst, &vendor_id_filter);
    free_id_filters(inst, &driver_id_filter);

    loader_platform_thread_unlock_mutex(&loader_lock);
    return res;
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetDeviceQueue2(VkDevice device, const VkDeviceQueueInfo2* pQueueInfo, VkQueue* pQueue) {
    const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);
    disp->GetDeviceQueue2(device, pQueueInfo, pQueue);
    if (pQueue != NULL && *pQueue != NULL) {
        loader_set_dispatch(*pQueue, disp);
    }
}
