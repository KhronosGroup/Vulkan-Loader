/*
 *
 * Copyright (c) 2014-2021 The Khronos Group Inc.
 * Copyright (c) 2014-2021 Valve Corporation
 * Copyright (c) 2014-2021 LunarG, Inc.
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
 * Author: Jon Ashburn <jon@lunarg.com>
 * Author: Courtney Goeltzenleuchter <courtney@LunarG.com>
 * Author: Chia-I Wu <olvaffe@gmail.com>
 * Author: Chia-I Wu <olv@lunarg.com>
 * Author: Mark Lobodzinski <mark@LunarG.com>
 * Author: Lenny Komow <lenny@lunarg.com>
 * Author: Charles Giessen <charles@lunarg.com>
 *
 */

#pragma once

#include "loader_common.h"

static inline struct loader_instance *loader_instance(VkInstance instance) { return (struct loader_instance *)instance; }

static inline VkPhysicalDevice loader_unwrap_physical_device(VkPhysicalDevice physicalDevice) {
    struct loader_physical_device_tramp *phys_dev = (struct loader_physical_device_tramp *)physicalDevice;
    if (PHYS_TRAMP_MAGIC_NUMBER != phys_dev->magic) {
        return VK_NULL_HANDLE;
    }
    return phys_dev->phys_dev;
}

static inline void loader_set_dispatch(void *obj, const void *data) { *((const void **)obj) = data; }

static inline VkLayerDispatchTable *loader_get_dispatch(const void *obj) {
    VkLayerDispatchTable *disp = *((VkLayerDispatchTable **)obj);
    if (VK_NULL_HANDLE == obj || DEVICE_DISP_TABLE_MAGIC_NUMBER != disp->magic) {
        return NULL;
    }
    return disp;
}

static inline struct loader_dev_dispatch_table *loader_get_dev_dispatch(const void *obj) {
    return *((struct loader_dev_dispatch_table **)obj);
}

static inline VkLayerInstanceDispatchTable *loader_get_instance_layer_dispatch(const void *obj) {
    return *((VkLayerInstanceDispatchTable **)obj);
}

static inline struct loader_instance_dispatch_table *loader_get_instance_dispatch(const void *obj) {
    return *((struct loader_instance_dispatch_table **)obj);
}

static inline void loader_init_dispatch(void *obj, const void *data) {
#ifdef DEBUG
    assert(valid_loader_magic_value(obj) &&
           "Incompatible ICD, first dword must be initialized to "
           "ICD_LOADER_MAGIC. See loader/README.md for details.");
#endif

    loader_set_dispatch(obj, data);
}

// Global variables used across files
extern struct loader_struct loader;
extern loader_platform_thread_mutex loader_lock;
extern loader_platform_thread_mutex loader_json_lock;
extern loader_platform_thread_mutex loader_preload_icd_lock;

bool compare_vk_extension_properties(const VkExtensionProperties *op1, const VkExtensionProperties *op2);

VkResult loader_validate_layers(const struct loader_instance *inst, const uint32_t layer_count,
                                const char *const *ppEnabledLayerNames, const struct loader_layer_list *list);

VkResult loader_validate_instance_extensions(struct loader_instance *inst, const struct loader_extension_list *icd_exts,
                                             const struct loader_layer_list *instance_layer,
                                             const VkInstanceCreateInfo *pCreateInfo);

void loader_initialize(void);
void loader_release(void);
void loader_preload_icds(void);
void loader_unload_preloaded_icds(void);
bool has_vk_extension_property_array(const VkExtensionProperties *vk_ext_prop, const uint32_t count,
                                     const VkExtensionProperties *ext_array);
bool has_vk_extension_property(const VkExtensionProperties *vk_ext_prop, const struct loader_extension_list *ext_list);

void loader_free_layer_properties(const struct loader_instance *inst, struct loader_layer_properties *layer_properties);
VkResult loader_add_to_ext_list(const struct loader_instance *inst, struct loader_extension_list *ext_list,
                                uint32_t prop_list_count, const VkExtensionProperties *props);
VkResult loader_add_to_dev_ext_list(const struct loader_instance *inst, struct loader_device_extension_list *ext_list,
                                    const VkExtensionProperties *props, uint32_t entry_count, char **entrys);
VkResult loader_add_device_extensions(const struct loader_instance *inst,
                                      PFN_vkEnumerateDeviceExtensionProperties fpEnumerateDeviceExtensionProperties,
                                      VkPhysicalDevice physical_device, const char *lib_name,
                                      struct loader_extension_list *ext_list);
VkResult loader_init_generic_list(const struct loader_instance *inst, struct loader_generic_list *list_info, size_t element_size);
void loader_destroy_generic_list(const struct loader_instance *inst, struct loader_generic_list *list);
void loader_destroy_layer_list(const struct loader_instance *inst, struct loader_device *device,
                               struct loader_layer_list *layer_list);
void loader_delete_layer_list_and_properties(const struct loader_instance *inst, struct loader_layer_list *layer_list);
VkResult loader_add_layer_name_to_list(const struct loader_instance *inst, const char *name, const enum layer_type_flags type_flags,
                                       const struct loader_layer_list *source_list, struct loader_layer_list *target_list,
                                       struct loader_layer_list *expanded_target_list);
void loader_scanned_icd_clear(const struct loader_instance *inst, struct loader_icd_tramp_list *icd_tramp_list);
VkResult loader_icd_scan(const struct loader_instance *inst, struct loader_icd_tramp_list *icd_tramp_list);
void loader_scan_for_layers(struct loader_instance *inst, struct loader_layer_list *instance_layers);
void loader_scan_for_implicit_layers(struct loader_instance *inst, struct loader_layer_list *instance_layers);
bool loader_implicit_layer_is_enabled(const struct loader_instance *inst, const struct loader_layer_properties *prop);
VkResult loader_get_icd_loader_instance_extensions(const struct loader_instance *inst, struct loader_icd_tramp_list *icd_tramp_list,
                                                   struct loader_extension_list *inst_exts);
struct loader_icd_term *loader_get_icd_and_device(const void *device, struct loader_device **found_dev, uint32_t *icd_index);
void loader_init_dispatch_dev_ext(struct loader_instance *inst, struct loader_device *dev);
void *loader_dev_ext_gpa(struct loader_instance *inst, const char *funcName);
void *loader_get_dev_ext_trampoline(uint32_t index);
bool loader_phys_dev_ext_gpa(struct loader_instance *inst, const char *funcName, bool perform_checking, void **tramp_addr,
                             void **term_addr);
void *loader_get_phys_dev_ext_tramp(uint32_t index);
void *loader_get_phys_dev_ext_termin(uint32_t index);
struct loader_instance *loader_get_instance(const VkInstance instance);
void loader_deactivate_layers(const struct loader_instance *instance, struct loader_device *device, struct loader_layer_list *list);
struct loader_device *loader_create_logical_device(const struct loader_instance *inst, const VkAllocationCallbacks *pAllocator);
void loader_add_logical_device(const struct loader_instance *inst, struct loader_icd_term *icd_term,
                               struct loader_device *found_dev);
void loader_remove_logical_device(const struct loader_instance *inst, struct loader_icd_term *icd_term,
                                  struct loader_device *found_dev, const VkAllocationCallbacks *pAllocator);
// NOTE: Outside of loader, this entry-point is only provided for error
// cleanup.
void loader_destroy_logical_device(const struct loader_instance *inst, struct loader_device *dev,
                                   const VkAllocationCallbacks *pAllocator);

VkResult loader_enable_instance_layers(struct loader_instance *inst, const VkInstanceCreateInfo *pCreateInfo,
                                       const struct loader_layer_list *instance_layers);

VkResult loader_create_instance_chain(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
                                      struct loader_instance *inst, VkInstance *created_instance);

void loader_activate_instance_layer_extensions(struct loader_instance *inst, VkInstance created_inst);

VKAPI_ATTR VkResult VKAPI_CALL loader_layer_create_device(VkInstance instance, VkPhysicalDevice physicalDevice,
                                                          const VkDeviceCreateInfo *pCreateInfo,
                                                          const VkAllocationCallbacks *pAllocator, VkDevice *pDevice,
                                                          PFN_vkGetInstanceProcAddr layerGIPA, PFN_vkGetDeviceProcAddr *nextGDPA);
VKAPI_ATTR void VKAPI_CALL loader_layer_destroy_device(VkDevice device, const VkAllocationCallbacks *pAllocator,
                                                       PFN_vkDestroyDevice destroyFunction);

VkResult loader_create_device_chain(const VkPhysicalDevice pd, const VkDeviceCreateInfo *pCreateInfo,
                                    const VkAllocationCallbacks *pAllocator, const struct loader_instance *inst,
                                    struct loader_device *dev, PFN_vkGetInstanceProcAddr callingLayer,
                                    PFN_vkGetDeviceProcAddr *layerNextGDPA);

VkResult loader_validate_device_extensions(struct loader_instance *this_instance,
                                           const struct loader_layer_list *activated_device_layers,
                                           const struct loader_extension_list *icd_exts, const VkDeviceCreateInfo *pCreateInfo);

VkResult setup_loader_tramp_phys_devs(struct loader_instance *inst);
VkResult setup_loader_term_phys_devs(struct loader_instance *inst);

VkStringErrorFlags vk_string_validate(const int max_length, const char *char_array);
char *loader_get_next_path(char *path);
VkResult add_data_files_in_path(const struct loader_instance *inst, char *search_path, bool is_directory_list,
                                struct loader_data_files *out_files, bool use_first_found_manifest);