/*
 *
 * Copyright (c) 2015-2022 The Khronos Group Inc.
 * Copyright (c) 2015-2022 Valve Corporation
 * Copyright (c) 2015-2022 LunarG, Inc.
 * Copyright (C) 2015 Google Inc.
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
#include "log.h"
#include "vk_loader_platform.h"
#include "vk_loader_extension_utils.h"
#include "wsi.h"
#include <vulkan/vk_icd.h>

// Manual trampoline entrypoints are in this file

/* vkGetInstanceProcAddr: Get global level or instance level entrypoint addressess.
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
 * returned regardless of the value of the instance paramtere. The spec was amended in this version to only allow querying global
 * level entrypoints with a NULL instance. However, as to not break old applications, the new behavior is only applied if the
 * instance passed in is both valid and minor version is greater than 1.2, which was when this change in behavior occurred. Only
 * instances with a newer version will get the new behavior.
 */
LOADER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance instance, const char *pName) {
    // Always should be able to get vkGetInstanceProcAddr if queried, regardless of the value of instance
    if (!strcmp(pName, "vkGetInstanceProcAddr")) return (PFN_vkVoidFunction)vkGetInstanceProcAddr;

    // Get entrypoint addresses that are global (no dispatchable object)
    void *addr = globalGetProcAddr(pName);
    if (addr != VK_NULL_HANDLE) {
        // Always can get a global entrypoint from vkGetInstanceProcAddr with a NULL instance handle
        if (instance == VK_NULL_HANDLE) {
            return addr;
        } else {
            // New behavior only returns a global entrypoint if the instance handle is NULL.
            // Old behavior is to return a global entrypoint regardless of the value of the instance handle.
            // Use new behavior if: The instance is valid and the minor version of the instance is greater than 1.2, which
            // was when the new behavior was added. (eg, it is enforced in the next minor version of vulkan, which will be 1.3)

            // First check if instance is valid - loader_get_instance() returns NULL if it isn't.
            struct loader_instance *ptr_instance = loader_get_instance(instance);
            if (ptr_instance != NULL &&
                loader_check_version_meets_required(loader_combine_version(1, 3, 0), ptr_instance->app_api_version)) {
                // New behavior
                return NULL;
            } else {
                // Old behavior
                return addr;
            }
        }
    } else {
        // All other functions require a valid instance handle to get
        if (instance == VK_NULL_HANDLE) {
            return NULL;
        }
        struct loader_instance *ptr_instance = loader_get_instance(instance);
        // If we've gotten here and the pointer is NULL, it's invalid
        if (ptr_instance == NULL) {
            loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                       "vkGetInstanceProcAddr: Invalid instance [VUID-vkGetInstanceProcAddr-instance-parameter]");
            abort(); /* Intentionally fail so user can correct issue. */
        }
        // Return trampoline code for non-global entrypoints including any extensions.
        // Device extensions are returned if a layer or ICD supports the extension.
        // Instance extensions are returned if the extension is enabled and the
        // loader or someone else supports the extension
        return trampoline_get_proc_addr(ptr_instance, pName);
    }
}

// Get a device level or global level entry point address.
// @param device
// @param pName
// @return
//    If device is valid, returns a device relative entry point for device level
//    entry points both core and extensions.
//    Device relative means call down the device chain.
LOADER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(VkDevice device, const char *pName) {
    void *addr;

    // For entrypoints that loader must handle (ie non-dispatchable or create object)
    // make sure the loader entrypoint is returned
    addr = loader_non_passthrough_gdpa(pName);
    if (addr) {
        return addr;
    }

    // Although CreateDevice is on device chain it's dispatchable object isn't
    // a VkDevice or child of VkDevice so return NULL.
    if (!strcmp(pName, "CreateDevice")) return NULL;

    // Return the dispatch table entrypoint for the fastest case
    const VkLayerDispatchTable *disp_table = *(VkLayerDispatchTable **)device;
    if (disp_table == NULL) return NULL;

    addr = loader_lookup_device_dispatch_table(disp_table, pName);
    if (addr) return addr;

    if (disp_table->GetDeviceProcAddr == NULL) return NULL;
    return disp_table->GetDeviceProcAddr(device, pName);
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(const char *pLayerName,
                                                                                    uint32_t *pPropertyCount,
                                                                                    VkExtensionProperties *pProperties) {
    LOADER_PLATFORM_THREAD_ONCE(&once_init, loader_initialize);

    // We know we need to call at least the terminator
    VkResult res = VK_SUCCESS;
    VkEnumerateInstanceExtensionPropertiesChain chain_tail = {
        .header =
            {
                .type = VK_CHAIN_TYPE_ENUMERATE_INSTANCE_EXTENSION_PROPERTIES,
                .version = VK_CURRENT_CHAIN_VERSION,
                .size = sizeof(chain_tail),
            },
        .pfnNextLayer = &terminator_EnumerateInstanceExtensionProperties,
        .pNextLink = NULL,
    };
    VkEnumerateInstanceExtensionPropertiesChain *chain_head = &chain_tail;

    // Get the implicit layers
    struct loader_layer_list layers;
    memset(&layers, 0, sizeof(layers));
    loader_scan_for_implicit_layers(NULL, &layers);

    // We'll need to save the dl handles so we can close them later
    loader_platform_dl_handle *libs = malloc(sizeof(loader_platform_dl_handle) * layers.count);
    if (libs == NULL && layers.count > 0) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    memset(libs, 0, sizeof(loader_platform_dl_handle) * layers.count);
    size_t lib_count = 0;

    // Prepend layers onto the chain if they implement this entry point
    for (uint32_t i = 0; i < layers.count; ++i) {
        if (!loader_implicit_layer_is_enabled(NULL, layers.list + i) ||
            layers.list[i].pre_instance_functions.enumerate_instance_extension_properties[0] == '\0') {
            continue;
        }

        loader_platform_dl_handle layer_lib = loader_platform_open_library(layers.list[i].lib_name);
        if (layer_lib == NULL) {
            loader_log(NULL, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                       "%s: Unable to load implicit layer library \"%s\"", __FUNCTION__, layers.list[i].lib_name);
            continue;
        }

        libs[lib_count++] = layer_lib;
        void *pfn = loader_platform_get_proc_address(layer_lib,
                                                     layers.list[i].pre_instance_functions.enumerate_instance_extension_properties);
        if (pfn == NULL) {
            loader_log(NULL, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                       "%s: Unable to resolve symbol \"%s\" in implicit layer library \"%s\"", __FUNCTION__,
                       layers.list[i].pre_instance_functions.enumerate_instance_extension_properties, layers.list[i].lib_name);
            continue;
        }

        VkEnumerateInstanceExtensionPropertiesChain *chain_link = malloc(sizeof(VkEnumerateInstanceExtensionPropertiesChain));
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

    // Free up the layers
    loader_delete_layer_list_and_properties(NULL, &layers);

    // Tear down the chain
    while (chain_head != &chain_tail) {
        VkEnumerateInstanceExtensionPropertiesChain *holder = chain_head;
        chain_head = (VkEnumerateInstanceExtensionPropertiesChain *)chain_head->pNextLink;
        free(holder);
    }

    // Close the dl handles
    for (size_t i = 0; i < lib_count; ++i) {
        loader_platform_close_library(libs[i]);
    }
    free(libs);

    return res;
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(uint32_t *pPropertyCount,
                                                                                VkLayerProperties *pProperties) {
    LOADER_PLATFORM_THREAD_ONCE(&once_init, loader_initialize);

    // We know we need to call at least the terminator
    VkResult res = VK_SUCCESS;
    VkEnumerateInstanceLayerPropertiesChain chain_tail = {
        .header =
            {
                .type = VK_CHAIN_TYPE_ENUMERATE_INSTANCE_LAYER_PROPERTIES,
                .version = VK_CURRENT_CHAIN_VERSION,
                .size = sizeof(chain_tail),
            },
        .pfnNextLayer = &terminator_EnumerateInstanceLayerProperties,
        .pNextLink = NULL,
    };
    VkEnumerateInstanceLayerPropertiesChain *chain_head = &chain_tail;

    // Get the implicit layers
    struct loader_layer_list layers;
    memset(&layers, 0, sizeof(layers));
    loader_scan_for_implicit_layers(NULL, &layers);

    // We'll need to save the dl handles so we can close them later
    loader_platform_dl_handle *libs = malloc(sizeof(loader_platform_dl_handle) * layers.count);
    memset(libs, 0, sizeof(loader_platform_dl_handle) * layers.count);
    if (libs == NULL && layers.count > 0) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    size_t lib_count = 0;

    // Prepend layers onto the chain if they implement this entry point
    for (uint32_t i = 0; i < layers.count; ++i) {
        if (!loader_implicit_layer_is_enabled(NULL, layers.list + i) ||
            layers.list[i].pre_instance_functions.enumerate_instance_layer_properties[0] == '\0') {
            continue;
        }

        loader_platform_dl_handle layer_lib = loader_platform_open_library(layers.list[i].lib_name);
        if (layer_lib == NULL) {
            loader_log(NULL, VULKAN_LOADER_WARN_BIT, 0, "%s: Unable to load implicit layer library \"%s\"", __FUNCTION__,
                       layers.list[i].lib_name);
            continue;
        }

        libs[lib_count++] = layer_lib;
        void *pfn =
            loader_platform_get_proc_address(layer_lib, layers.list[i].pre_instance_functions.enumerate_instance_layer_properties);
        if (pfn == NULL) {
            loader_log(NULL, VULKAN_LOADER_WARN_BIT, 0, "%s: Unable to resolve symbol \"%s\" in implicit layer library \"%s\"",
                       __FUNCTION__, layers.list[i].pre_instance_functions.enumerate_instance_layer_properties,
                       layers.list[i].lib_name);
            continue;
        }

        VkEnumerateInstanceLayerPropertiesChain *chain_link = malloc(sizeof(VkEnumerateInstanceLayerPropertiesChain));
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

    // Free up the layers
    loader_delete_layer_list_and_properties(NULL, &layers);

    // Tear down the chain
    while (chain_head != &chain_tail) {
        VkEnumerateInstanceLayerPropertiesChain *holder = chain_head;
        chain_head = (VkEnumerateInstanceLayerPropertiesChain *)chain_head->pNextLink;
        free(holder);
    }

    // Close the dl handles
    for (size_t i = 0; i < lib_count; ++i) {
        loader_platform_close_library(libs[i]);
    }
    free(libs);

    return res;
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceVersion(uint32_t *pApiVersion) {
    LOADER_PLATFORM_THREAD_ONCE(&once_init, loader_initialize);

    if (NULL == pApiVersion) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
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
        .pfnNextLayer = &terminator_EnumerateInstanceVersion,
        .pNextLink = NULL,
    };
    VkEnumerateInstanceVersionChain *chain_head = &chain_tail;

    // Get the implicit layers
    struct loader_layer_list layers;
    memset(&layers, 0, sizeof(layers));
    loader_scan_for_implicit_layers(NULL, &layers);

    // We'll need to save the dl handles so we can close them later
    loader_platform_dl_handle *libs = malloc(sizeof(loader_platform_dl_handle) * layers.count);
    if (libs == NULL && layers.count > 0) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    memset(libs, 0, sizeof(loader_platform_dl_handle) * layers.count);
    size_t lib_count = 0;

    // Prepend layers onto the chain if they implement this entry point
    for (uint32_t i = 0; i < layers.count; ++i) {
        if (!loader_implicit_layer_is_enabled(NULL, layers.list + i) ||
            layers.list[i].pre_instance_functions.enumerate_instance_version[0] == '\0') {
            continue;
        }

        loader_platform_dl_handle layer_lib = loader_platform_open_library(layers.list[i].lib_name);
        if (layer_lib == NULL) {
            loader_log(NULL, VULKAN_LOADER_WARN_BIT, 0, "%s: Unable to load implicit layer library \"%s\"", __FUNCTION__,
                       layers.list[i].lib_name);
            continue;
        }

        libs[lib_count++] = layer_lib;
        void *pfn = loader_platform_get_proc_address(layer_lib, layers.list[i].pre_instance_functions.enumerate_instance_version);
        if (pfn == NULL) {
            loader_log(NULL, VULKAN_LOADER_WARN_BIT, 0, "%s: Unable to resolve symbol \"%s\" in implicit layer library \"%s\"",
                       __FUNCTION__, layers.list[i].pre_instance_functions.enumerate_instance_version, layers.list[i].lib_name);
            continue;
        }

        VkEnumerateInstanceVersionChain *chain_link = malloc(sizeof(VkEnumerateInstanceVersionChain));
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

    // Free up the layers
    loader_delete_layer_list_and_properties(NULL, &layers);

    // Tear down the chain
    while (chain_head != &chain_tail) {
        VkEnumerateInstanceVersionChain *holder = chain_head;
        chain_head = (VkEnumerateInstanceVersionChain *)chain_head->pNextLink;
        free(holder);
    }

    // Close the dl handles
    for (size_t i = 0; i < lib_count; ++i) {
        loader_platform_close_library(libs[i]);
    }
    free(libs);

    return res;
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateInstance(const VkInstanceCreateInfo *pCreateInfo,
                                                              const VkAllocationCallbacks *pAllocator, VkInstance *pInstance) {
    struct loader_instance *ptr_instance = NULL;
    VkInstance created_instance = VK_NULL_HANDLE;
    bool loaderLocked = false;
    VkResult res = VK_ERROR_INITIALIZATION_FAILED;

    LOADER_PLATFORM_THREAD_ONCE(&once_init, loader_initialize);

    if (pCreateInfo == NULL) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateInstance: \'pCreateInfo\' is NULL (VUID-vkCreateInstance-pCreateInfo-parameter)");
        goto out;
    }
    if (pInstance == NULL) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateInstance \'pInstance\' not valid (VUID-vkCreateInstance-pInstance-parameter)");
        goto out;
    }

#if (DEBUG_DISABLE_APP_ALLOCATORS == 1)
    {
#else
    if (pAllocator) {
        ptr_instance = (struct loader_instance *)pAllocator->pfnAllocation(pAllocator->pUserData, sizeof(struct loader_instance),
                                                                           sizeof(int *), VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    } else {
#endif
        ptr_instance = (struct loader_instance *)malloc(sizeof(struct loader_instance));
    }

    VkInstanceCreateInfo ici = *pCreateInfo;

    if (ptr_instance == NULL) {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    loader_platform_thread_lock_mutex(&loader_lock);
    loaderLocked = true;
    memset(ptr_instance, 0, sizeof(struct loader_instance));
    if (pAllocator) {
        ptr_instance->alloc_callbacks = *pAllocator;
    }
    ptr_instance->magic = LOADER_MAGIC_NUMBER;

    // Save the application version
    if (NULL == pCreateInfo->pApplicationInfo || 0 == pCreateInfo->pApplicationInfo->apiVersion) {
        ptr_instance->app_api_version = LOADER_VERSION_1_0_0;
    } else {
        ptr_instance->app_api_version = loader_make_version(pCreateInfo->pApplicationInfo->apiVersion);
        // zero out the patch version since we don't actually want to compare with it
        ptr_instance->app_api_version.patch = 0;
    }

    // Look for one or more VK_EXT_debug_report or VK_EXT_debug_utils create info structures
    // and setup a callback(s) for each one found.
    ptr_instance->num_tmp_report_callbacks = 0;
    ptr_instance->tmp_report_create_infos = NULL;
    ptr_instance->tmp_report_callbacks = NULL;
    ptr_instance->num_tmp_messengers = 0;
    ptr_instance->tmp_messenger_create_infos = NULL;
    ptr_instance->tmp_messengers = NULL;

    // Handle cases of VK_EXT_debug_utils
    if (util_CopyDebugUtilsMessengerCreateInfos(pCreateInfo->pNext, pAllocator, &ptr_instance->num_tmp_messengers,
                                                &ptr_instance->tmp_messenger_create_infos, &ptr_instance->tmp_messengers)) {
        // One or more were found, but allocation failed.  Therefore, clean up and fail this function:
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    } else if (ptr_instance->num_tmp_messengers > 0) {
        // Setup the temporary messenger(s) here to catch early issues:
        if (util_CreateDebugUtilsMessengers(ptr_instance, pAllocator, ptr_instance->num_tmp_messengers,
                                            ptr_instance->tmp_messenger_create_infos, ptr_instance->tmp_messengers)) {
            // Failure of setting up one or more of the messenger.  Therefore, clean up and fail this function:
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }
    }

    // Handle cases of VK_EXT_debug_report
    if (util_CopyDebugReportCreateInfos(pCreateInfo->pNext, pAllocator, &ptr_instance->num_tmp_report_callbacks,
                                        &ptr_instance->tmp_report_create_infos, &ptr_instance->tmp_report_callbacks)) {
        // One or more were found, but allocation failed.  Therefore, clean up and fail this function:
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    } else if (ptr_instance->num_tmp_report_callbacks > 0) {
        // Setup the temporary callback(s) here to catch early issues:
        if (util_CreateDebugReportCallbacks(ptr_instance, pAllocator, ptr_instance->num_tmp_report_callbacks,
                                            ptr_instance->tmp_report_create_infos, ptr_instance->tmp_report_callbacks)) {
            // Failure of setting up one or more of the callback.  Therefore, clean up and fail this function:
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }
    }

    // Make sure the application provided API version has 0 for its variant
    if (NULL != pCreateInfo->pApplicationInfo) {
        uint32_t variant_version = VK_API_VERSION_VARIANT(pCreateInfo->pApplicationInfo->apiVersion);
        if (0 != variant_version) {
            loader_log(ptr_instance, VULKAN_LOADER_WARN_BIT, 0,
                       "vkCreateInstance: The API Variant specified in pCreateInfo->pApplicationInfo.apiVersion is %d instead of "
                       "the expected value of 0.",
                       variant_version);
        }
    }

    // Due to implicit layers need to get layer list even if
    // enabledLayerCount == 0 and VK_INSTANCE_LAYERS is unset. For now always
    // get layer list via loader_scan_for_layers().
    memset(&ptr_instance->instance_layer_list, 0, sizeof(ptr_instance->instance_layer_list));
    loader_scan_for_layers(ptr_instance, &ptr_instance->instance_layer_list);

    // Validate the app requested layers to be enabled
    if (pCreateInfo->enabledLayerCount > 0) {
        res = loader_validate_layers(ptr_instance, pCreateInfo->enabledLayerCount, pCreateInfo->ppEnabledLayerNames,
                                     &ptr_instance->instance_layer_list);
        if (res != VK_SUCCESS) {
            goto out;
        }
    }

    // Scan/discover all ICD libraries
    memset(&ptr_instance->icd_tramp_list, 0, sizeof(ptr_instance->icd_tramp_list));
    res = loader_icd_scan(ptr_instance, &ptr_instance->icd_tramp_list);
    if (res == VK_SUCCESS && ptr_instance->icd_tramp_list.count == 0) {
        // No drivers found
        loader_log(ptr_instance, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_DRIVER_BIT, 0, "vkCreateInstance: Found no drivers!");
        res = VK_ERROR_INCOMPATIBLE_DRIVER;
        goto out;
    }
    if (res != VK_SUCCESS) {
        if (res != VK_ERROR_OUT_OF_HOST_MEMORY && ptr_instance->icd_tramp_list.count == 0) {
            loader_log(ptr_instance, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_DRIVER_BIT, 0, "vkCreateInstance: Found no drivers!");
            res = VK_ERROR_INCOMPATIBLE_DRIVER;
        }
        goto out;
    }

    // Get extensions from all ICD's, merge so no duplicates, then validate
    res = loader_get_icd_loader_instance_extensions(ptr_instance, &ptr_instance->icd_tramp_list, &ptr_instance->ext_list);
    if (res != VK_SUCCESS) {
        goto out;
    }
    res = loader_validate_instance_extensions(ptr_instance, &ptr_instance->ext_list, &ptr_instance->instance_layer_list, &ici);
    if (res != VK_SUCCESS) {
        goto out;
    }

    ptr_instance->disp = loader_instance_heap_alloc(ptr_instance, sizeof(struct loader_instance_dispatch_table),
                                                    VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    if (ptr_instance->disp == NULL) {
        loader_log(ptr_instance, VULKAN_LOADER_ERROR_BIT, 0,
                   "vkCreateInstance:  Failed to allocate Loader's full Instance dispatch table.");
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }
    memcpy(&ptr_instance->disp->layer_inst_disp, &instance_term_disp, sizeof(instance_term_disp));

    ptr_instance->next = loader.instances;
    loader.instances = ptr_instance;

    // Activate any layers on instance chain
    res = loader_enable_instance_layers(ptr_instance, &ici, &ptr_instance->instance_layer_list);
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
        wsi_create_instance(ptr_instance, &ici);
        debug_utils_CreateInstance(ptr_instance, &ici);
        extensions_create_instance(ptr_instance, &ici);

        *pInstance = (VkInstance)ptr_instance;

        // Finally have the layers in place and everyone has seen
        // the CreateInstance command go by. This allows the layer's
        // GetInstanceProcAddr functions to return valid extension functions
        // if enabled.
        loader_activate_instance_layer_extensions(ptr_instance, created_instance);
    } else if (VK_ERROR_EXTENSION_NOT_PRESENT == res && !ptr_instance->create_terminator_invalid_extension) {
        loader_log(ptr_instance, VULKAN_LOADER_WARN_BIT, 0,
                   "vkCreateInstance: Layer returning invalid extension error not triggered by ICD/Loader (Policy #LLP_LAYER_17).");
    }

out:

    if (NULL != ptr_instance) {
        if (res != VK_SUCCESS) {
            if (loader.instances == ptr_instance) {
                loader.instances = ptr_instance->next;
            }
            if (NULL != ptr_instance->disp) {
                loader_instance_heap_free(ptr_instance, ptr_instance->disp);
            }
            if (ptr_instance->num_tmp_report_callbacks > 0) {
                // Remove temporary VK_EXT_debug_report items
                util_DestroyDebugReportCallbacks(ptr_instance, pAllocator, ptr_instance->num_tmp_report_callbacks,
                                                 ptr_instance->tmp_report_callbacks);
                util_FreeDebugReportCreateInfos(pAllocator, ptr_instance->tmp_report_create_infos,
                                                ptr_instance->tmp_report_callbacks);
            }
            if (ptr_instance->num_tmp_messengers > 0) {
                // Remove temporary VK_EXT_debug_utils items
                util_DestroyDebugUtilsMessengers(ptr_instance, pAllocator, ptr_instance->num_tmp_messengers,
                                                 ptr_instance->tmp_messengers);
                util_FreeDebugUtilsMessengerCreateInfos(pAllocator, ptr_instance->tmp_messenger_create_infos,
                                                        ptr_instance->tmp_messengers);
            }

            if (NULL != ptr_instance->expanded_activated_layer_list.list) {
                loader_deactivate_layers(ptr_instance, NULL, &ptr_instance->expanded_activated_layer_list);
            }
            if (NULL != ptr_instance->app_activated_layer_list.list) {
                loader_destroy_layer_list(ptr_instance, NULL, &ptr_instance->app_activated_layer_list);
            }

            loader_delete_layer_list_and_properties(ptr_instance, &ptr_instance->instance_layer_list);
            loader_scanned_icd_clear(ptr_instance, &ptr_instance->icd_tramp_list);
            loader_destroy_generic_list(ptr_instance, (struct loader_generic_list *)&ptr_instance->ext_list);

            loader_instance_heap_free(ptr_instance, ptr_instance);
        } else {
            // Remove temporary VK_EXT_debug_report or VK_EXT_debug_utils items
            util_DestroyDebugUtilsMessengers(ptr_instance, pAllocator, ptr_instance->num_tmp_messengers,
                                             ptr_instance->tmp_messengers);
            util_DestroyDebugReportCallbacks(ptr_instance, pAllocator, ptr_instance->num_tmp_report_callbacks,
                                             ptr_instance->tmp_report_callbacks);
        }

        if (loaderLocked) {
            loader_platform_thread_unlock_mutex(&loader_lock);
        }
    }

    return res;
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks *pAllocator) {
    const VkLayerInstanceDispatchTable *disp;
    struct loader_instance *ptr_instance = NULL;
    bool callback_setup = false;
    bool messenger_setup = false;

    if (instance == VK_NULL_HANDLE) {
        return;
    }
    loader_platform_thread_lock_mutex(&loader_lock);

    ptr_instance = loader_get_instance(instance);
    if (ptr_instance == NULL) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkDestroyInstance: Invalid instance [VUID-vkDestroyInstance-instance-parameter]");
        loader_platform_thread_unlock_mutex(&loader_lock);
        abort(); /* Intentionally fail so user can correct issue. */
    }

    if (pAllocator) {
        ptr_instance->alloc_callbacks = *pAllocator;
    }

    if (ptr_instance->num_tmp_messengers > 0) {
        // Setup the temporary VK_EXT_debug_utils messenger(s) here to catch cleanup issues:
        if (!util_CreateDebugUtilsMessengers(ptr_instance, pAllocator, ptr_instance->num_tmp_messengers,
                                             ptr_instance->tmp_messenger_create_infos, ptr_instance->tmp_messengers)) {
            messenger_setup = true;
        }
    }

    if (ptr_instance->num_tmp_report_callbacks > 0) {
        // Setup the temporary VK_EXT_debug_report callback(s) here to catch cleanup issues:
        if (!util_CreateDebugReportCallbacks(ptr_instance, pAllocator, ptr_instance->num_tmp_report_callbacks,
                                             ptr_instance->tmp_report_create_infos, ptr_instance->tmp_report_callbacks)) {
            callback_setup = true;
        }
    }

    disp = loader_get_instance_layer_dispatch(instance);
    disp->DestroyInstance(ptr_instance->instance, pAllocator);

    if (NULL != ptr_instance->expanded_activated_layer_list.list) {
        loader_deactivate_layers(ptr_instance, NULL, &ptr_instance->expanded_activated_layer_list);
    }
    if (NULL != ptr_instance->app_activated_layer_list.list) {
        loader_destroy_layer_list(ptr_instance, NULL, &ptr_instance->app_activated_layer_list);
    }

    if (ptr_instance->phys_devs_tramp) {
        for (uint32_t i = 0; i < ptr_instance->phys_dev_count_tramp; i++) {
            loader_instance_heap_free(ptr_instance, ptr_instance->phys_devs_tramp[i]);
        }
        loader_instance_heap_free(ptr_instance, ptr_instance->phys_devs_tramp);
    }

    if (messenger_setup) {
        loader_log(ptr_instance, VULKAN_LOADER_INFO_BIT, 0,
                   "vkDestroyInstance: destroying temporary instance debug util messenger");

        util_DestroyDebugUtilsMessengers(ptr_instance, pAllocator, ptr_instance->num_tmp_messengers, ptr_instance->tmp_messengers);
        util_FreeDebugUtilsMessengerCreateInfos(pAllocator, ptr_instance->tmp_messenger_create_infos, ptr_instance->tmp_messengers);
    }

    if (callback_setup) {
        loader_log(ptr_instance, VULKAN_LOADER_INFO_BIT, 0,
                   "vkDestroyInstance: destroying temporary instance debug report callback");

        util_DestroyDebugReportCallbacks(ptr_instance, pAllocator, ptr_instance->num_tmp_report_callbacks,
                                         ptr_instance->tmp_report_callbacks);
        util_FreeDebugReportCreateInfos(pAllocator, ptr_instance->tmp_report_create_infos, ptr_instance->tmp_report_callbacks);
    }

    loader_instance_heap_free(ptr_instance, ptr_instance->disp);
    loader_instance_heap_free(ptr_instance, ptr_instance);
    loader_platform_thread_unlock_mutex(&loader_lock);

    // Unload preloaded layers, so if vkEnumerateInstanceExtensionProperties or vkCreateInstance is called again, the ICD's are up
    // to date
    loader_unload_preloaded_icds();
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDevices(VkInstance instance, uint32_t *pPhysicalDeviceCount,
                                                                        VkPhysicalDevice *pPhysicalDevices) {
    VkResult res = VK_SUCCESS;
    struct loader_instance *inst;

    loader_platform_thread_lock_mutex(&loader_lock);

    inst = loader_get_instance(instance);
    if (NULL == inst) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkEnumeratePhysicalDevices: Invalid instance [VUID-vkEnumeratePhysicalDevices-instance-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }

    if (NULL == pPhysicalDeviceCount) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkEnumeratePhysicalDevices: Received NULL pointer for physical device count return value. "
                   "[VUID-vkEnumeratePhysicalDevices-pPhysicalDeviceCount-parameter]");
        res = VK_ERROR_INITIALIZATION_FAILED;
        goto out;
    }

    // Call down the chain to get the physical device info
    res = inst->disp->layer_inst_disp.EnumeratePhysicalDevices(inst->instance, pPhysicalDeviceCount, pPhysicalDevices);

    if (NULL != pPhysicalDevices && (VK_SUCCESS == res || VK_INCOMPLETE == res)) {
        // Wrap the PhysDev object for loader usage, return wrapped objects
        VkResult update_res = setup_loader_tramp_phys_devs(inst, *pPhysicalDeviceCount, pPhysicalDevices);
        if (VK_SUCCESS != update_res) {
            res = update_res;
        }
    }

out:

    loader_platform_thread_unlock_mutex(&loader_lock);
    return res;
}

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo *pCreateInfo,
                                                            const VkAllocationCallbacks *pAllocator, VkDevice *pDevice) {
    if (VK_NULL_HANDLE == loader_unwrap_physical_device(physicalDevice)) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkCreateDevice: Invalid physicalDevice [VUID-vkCreateDevice-physicalDevice-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }
    loader_platform_thread_lock_mutex(&loader_lock);
    VkResult res = loader_layer_create_device(NULL, physicalDevice, pCreateInfo, pAllocator, pDevice, NULL, NULL);
    loader_platform_thread_unlock_mutex(&loader_lock);
    return res;
}

LOADER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyDevice(VkDevice device, const VkAllocationCallbacks *pAllocator) {
    const VkLayerDispatchTable *disp;

    if (device == VK_NULL_HANDLE) {
        return;
    }
    disp = loader_get_dispatch(device);
    if (NULL == disp) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkDestroyDevice: Invalid device [VUID-vkDestroyDevice-device-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }

    loader_platform_thread_lock_mutex(&loader_lock);

    loader_layer_destroy_device(device, pAllocator, disp->DestroyDevice);

    loader_platform_thread_unlock_mutex(&loader_lock);
}


LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice,
                                                                              uint32_t *pPropertyCount,
                                                                              VkLayerProperties *pProperties) {
    uint32_t copy_size;
    struct loader_physical_device_tramp *phys_dev;
    struct loader_layer_list *enabled_layers, layers_list;
    memset(&layers_list, 0, sizeof(layers_list));
    loader_platform_thread_lock_mutex(&loader_lock);

    // Don't dispatch this call down the instance chain, want all device layers
    // enumerated and instance chain may not contain all device layers
    // TODO re-evaluate the above statement we maybe able to start calling
    // down the chain

    phys_dev = (struct loader_physical_device_tramp *)physicalDevice;
    if (VK_NULL_HANDLE == physicalDevice || PHYS_TRAMP_MAGIC_NUMBER != phys_dev->magic) {
        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
                   "vkEnumerateDeviceLayerProperties: Invalid physicalDevice "
                   "[VUID-vkEnumerateDeviceLayerProperties-physicalDevice-parameter]");
        loader_platform_thread_unlock_mutex(&loader_lock);
        abort(); /* Intentionally fail so user can correct issue. */
    }

    const struct loader_instance *inst = phys_dev->this_instance;

    uint32_t count = inst->app_activated_layer_list.count;
    if (count == 0 || pProperties == NULL) {
        *pPropertyCount = count;
        loader_platform_thread_unlock_mutex(&loader_lock);
        return VK_SUCCESS;
    }
    enabled_layers = (struct loader_layer_list *)&inst->app_activated_layer_list;

    copy_size = (*pPropertyCount < count) ? *pPropertyCount : count;
    for (uint32_t i = 0; i < copy_size; i++) {
        memcpy(&pProperties[i], &(enabled_layers->list[i].info), sizeof(VkLayerProperties));
    }
    *pPropertyCount = copy_size;

    if (copy_size < count) {
        loader_platform_thread_unlock_mutex(&loader_lock);
        return VK_INCOMPLETE;
    }

    loader_platform_thread_unlock_mutex(&loader_lock);
    return VK_SUCCESS;
}


// ---- Vulkan core 1.1 trampolines

LOADER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDeviceGroups(
    VkInstance instance, uint32_t *pPhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties *pPhysicalDeviceGroupProperties) {
    VkResult res = VK_SUCCESS;
    struct loader_instance *inst = NULL;

    loader_platform_thread_lock_mutex(&loader_lock);

    inst = loader_get_instance(instance);
    if (NULL == inst) {
        loader_log(
            NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,
            "vkEnumeratePhysicalDeviceGroupsKHR: Invalid instance [VUID-vkEnumeratePhysicalDeviceGroups-instance-parameter]");
        abort(); /* Intentionally fail so user can correct issue. */
    }

    if (NULL == pPhysicalDeviceGroupCount) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "vkEnumeratePhysicalDeviceGroupsKHR: Received NULL pointer for physical "
                   "device group count return value.");
        res = VK_ERROR_INITIALIZATION_FAILED;
        goto out;
    }

    // Call down the chain to get the physical device group info.
    res = inst->disp->layer_inst_disp.EnumeratePhysicalDeviceGroups(inst->instance, pPhysicalDeviceGroupCount,
                                                                    pPhysicalDeviceGroupProperties);
    if (NULL != pPhysicalDeviceGroupProperties && (VK_SUCCESS == res || VK_INCOMPLETE == res)) {
        // Wrap the PhysDev object for loader usage, return wrapped objects
        VkResult update_res = setup_loader_tramp_phys_dev_groups(inst, *pPhysicalDeviceGroupCount, pPhysicalDeviceGroupProperties);
        if (VK_SUCCESS != update_res) {
            res = update_res;
        }
    }

out:

    loader_platform_thread_unlock_mutex(&loader_lock);
    return res;
}


// -----------------------------------------------------------------------------------------------------------
// -- NOTE: This must be last since it implements dispatch table look-up functions that use all trampolines!!
// -----------------------------------------------------------------------------------------------------------
// Generated loader trampolines are in the following file
#include "vk_loader_trampolines.c"
