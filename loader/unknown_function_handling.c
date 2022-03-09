/*
 * Copyright (c) 2022 The Khronos Group Inc.
 * Copyright (c) 2022 Valve Corporation
 * Copyright (c) 2022 LunarG, Inc.
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
 * Author: Mark Young <marky@lunarg.com>
 * Author: Lenny Komow <lenny@lunarg.com>
 * Author: Charles Giessen <charles@lunarg.com>
 */

#include "unknown_function_handling.h"

#include "allocation.h"
#include "murmurhash.h"
#include "log.h"

// Forward declarations
void *loader_get_dev_ext_trampoline(uint32_t index);
void *loader_get_phys_dev_ext_tramp(uint32_t index);
void *loader_get_phys_dev_ext_termin(uint32_t index);

// Initialize device_ext dispatch table entry as follows:
// If dev == NULL find all logical devices created within this instance and
//  init the entry (given by idx) in the ext dispatch table.
// If dev != NULL only initialize the entry in the given dev's dispatch table.
// The initialization value is gotten by calling down the device chain with
// GDPA.
// If GDPA returns NULL then don't initialize the dispatch table entry.
void loader_init_dispatch_dev_ext_entry(struct loader_instance *inst, struct loader_device *dev, uint32_t idx, const char *funcName)

{
    void *gdpa_value;
    if (dev != NULL) {
        gdpa_value = dev->loader_dispatch.core_dispatch.GetDeviceProcAddr(dev->chain_device, funcName);
        if (gdpa_value != NULL) dev->loader_dispatch.ext_dispatch.dev_ext[idx] = (PFN_vkDevExt)gdpa_value;
    } else {
        for (struct loader_icd_term *icd_term = inst->icd_terms; icd_term != NULL; icd_term = icd_term->next) {
            struct loader_device *ldev = icd_term->logical_device_list;
            while (ldev) {
                gdpa_value = ldev->loader_dispatch.core_dispatch.GetDeviceProcAddr(ldev->chain_device, funcName);
                if (gdpa_value != NULL) ldev->loader_dispatch.ext_dispatch.dev_ext[idx] = (PFN_vkDevExt)gdpa_value;
                ldev = ldev->next;
            }
        }
    }
}

// Find all dev extension in the hash table  and initialize the dispatch table
// for dev  for each of those extension entrypoints found in hash table.
void loader_init_dispatch_dev_ext(struct loader_instance *inst, struct loader_device *dev) {
    for (uint32_t i = 0; i < MAX_NUM_UNKNOWN_EXTS; i++) {
        if (inst->dev_ext_disp_hash[i].func_name != NULL)
            loader_init_dispatch_dev_ext_entry(inst, dev, i, inst->dev_ext_disp_hash[i].func_name);
    }
}

bool loader_check_icds_for_dev_ext_address(struct loader_instance *inst, const char *funcName) {
    struct loader_icd_term *icd_term;
    icd_term = inst->icd_terms;
    while (NULL != icd_term) {
        if (icd_term->scanned_icd->GetInstanceProcAddr(icd_term->instance, funcName))
            // this icd supports funcName
            return true;
        icd_term = icd_term->next;
    }

    return false;
}

bool loader_check_layer_list_for_dev_ext_address(const struct loader_layer_list *const layers, const char *funcName) {
    // Iterate over the layers.
    for (uint32_t layer = 0; layer < layers->count; ++layer) {
        // Iterate over the extensions.
        const struct loader_device_extension_list *const extensions = &(layers->list[layer].device_extension_list);
        for (uint32_t extension = 0; extension < extensions->count; ++extension) {
            // Iterate over the entry points.
            const struct loader_dev_ext_props *const property = &(extensions->list[extension]);
            for (uint32_t entry = 0; entry < property->entrypoint_count; ++entry) {
                if (strcmp(property->entrypoints[entry], funcName) == 0) {
                    return true;
                }
            }
        }
    }

    return false;
}

void loader_free_dev_ext_table(struct loader_instance *inst) {
    for (uint32_t i = 0; i < MAX_NUM_UNKNOWN_EXTS; i++) {
        loader_instance_heap_free(inst, inst->dev_ext_disp_hash[i].func_name);
        loader_instance_heap_free(inst, inst->dev_ext_disp_hash[i].list.index);
    }
    memset(inst->dev_ext_disp_hash, 0, sizeof(inst->dev_ext_disp_hash));
}

bool loader_add_dev_ext_table(struct loader_instance *inst, uint32_t *ptr_idx, const char *funcName) {
    uint32_t i;
    uint32_t idx = *ptr_idx;
    struct loader_dispatch_hash_list *list = &inst->dev_ext_disp_hash[idx].list;

    if (!inst->dev_ext_disp_hash[idx].func_name) {
        // no entry here at this idx, so use it
        assert(list->capacity == 0);
        inst->dev_ext_disp_hash[idx].func_name =
            (char *)loader_instance_heap_alloc(inst, strlen(funcName) + 1, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (inst->dev_ext_disp_hash[idx].func_name == NULL) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_add_dev_ext_table: Failed to allocate memory for func_name %s",
                       funcName);
            return false;
        }
        strncpy(inst->dev_ext_disp_hash[idx].func_name, funcName, strlen(funcName) + 1);
        return true;
    }

    // check for enough capacity
    if (list->capacity == 0) {
        list->index = loader_instance_heap_alloc(inst, 8 * sizeof(*(list->index)), VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (list->index == NULL) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_add_dev_ext_table: Failed to allocate memory for list index of function %s", funcName);
            return false;
        }
        list->capacity = 8 * sizeof(*(list->index));
    } else if (list->capacity < (list->count + 1) * sizeof(*(list->index))) {
        void *new_ptr = loader_instance_heap_realloc(inst, list->index, list->capacity, list->capacity * 2,
                                                     VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (NULL == new_ptr) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "loader_add_dev_ext_table: Failed to reallocate memory for list index of function %s", funcName);
            return false;
        }
        list->index = new_ptr;
        list->capacity *= 2;
    }

    // find an unused index in the hash table and use it
    i = (idx + 1) % MAX_NUM_UNKNOWN_EXTS;
    do {
        if (!inst->dev_ext_disp_hash[i].func_name) {
            assert(inst->dev_ext_disp_hash[i].list.capacity == 0);
            inst->dev_ext_disp_hash[i].func_name =
                (char *)loader_instance_heap_alloc(inst, strlen(funcName) + 1, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
            if (inst->dev_ext_disp_hash[i].func_name == NULL) {
                loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_add_dev_ext_table: Failed to allocate memory for func_name %s",
                           funcName);
                return false;
            }
            strncpy(inst->dev_ext_disp_hash[i].func_name, funcName, strlen(funcName) + 1);
            list->index[list->count] = i;
            list->count++;
            *ptr_idx = i;
            return true;
        }
        i = (i + 1) % MAX_NUM_UNKNOWN_EXTS;
    } while (i != idx);

    loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_add_dev_ext_table:  Could not insert into hash table; is it full?");

    return false;
}

bool loader_name_in_dev_ext_table(struct loader_instance *inst, uint32_t *idx, const char *funcName) {
    uint32_t alt_idx;
    if (inst->dev_ext_disp_hash[*idx].func_name && !strcmp(inst->dev_ext_disp_hash[*idx].func_name, funcName)) return true;

    // funcName wasn't at the primary spot in the hash table
    // search the list of secondary locations (shallow search, not deep search)
    for (uint32_t i = 0; i < inst->dev_ext_disp_hash[*idx].list.count; i++) {
        alt_idx = inst->dev_ext_disp_hash[*idx].list.index[i];
        if (inst->dev_ext_disp_hash[*idx].func_name && !strcmp(inst->dev_ext_disp_hash[*idx].func_name, funcName)) {
            *idx = alt_idx;
            return true;
        }
    }

    return false;
}

// This function returns generic trampoline code address for unknown entry
// points.
// Presumably, these unknown entry points (as given by funcName) are device
// extension entrypoints.  A hash table is used to keep a list of unknown entry
// points and their mapping to the device extension dispatch table
// (struct loader_dev_ext_dispatch_table).
// \returns
// For a given entry point string (funcName), if an existing mapping is found
// the
// trampoline address for that mapping is returned. Otherwise, this unknown
// entry point
// has not been seen yet. Next check if a layer or ICD supports it.  If so then
// a
// new entry in the hash table is initialized and that trampoline address for
// the new entry is returned. Null is returned if the hash table is full or
// if no discovered layer or ICD returns a non-NULL GetProcAddr for it.
void *loader_dev_ext_gpa(struct loader_instance *inst, const char *funcName) {
    uint32_t idx;
    uint32_t seed = 0;

    idx = murmurhash(funcName, strlen(funcName), seed) % MAX_NUM_UNKNOWN_EXTS;

    if (loader_name_in_dev_ext_table(inst, &idx, funcName))
        // found funcName already in hash
        return loader_get_dev_ext_trampoline(idx);

    // Check if funcName is supported in either ICDs or a layer library
    if (!loader_check_icds_for_dev_ext_address(inst, funcName) &&
        !loader_check_layer_list_for_dev_ext_address(&inst->app_activated_layer_list, funcName)) {
        // if support found in layers continue on
        return NULL;
    }

    if (loader_add_dev_ext_table(inst, &idx, funcName)) {
        // successfully added new table entry
        // init any dev dispatch table entries as needed
        loader_init_dispatch_dev_ext_entry(inst, NULL, idx, funcName);
        return loader_get_dev_ext_trampoline(idx);
    }

    return NULL;
}

bool loader_check_icds_for_phys_dev_ext_address(struct loader_instance *inst, const char *funcName) {
    struct loader_icd_term *icd_term;
    icd_term = inst->icd_terms;
    while (NULL != icd_term) {
        if (icd_term->scanned_icd->interface_version >= MIN_PHYS_DEV_EXTENSION_ICD_INTERFACE_VERSION &&
            icd_term->scanned_icd->GetPhysicalDeviceProcAddr(icd_term->instance, funcName))
            // this icd supports funcName
            return true;
        icd_term = icd_term->next;
    }

    return false;
}

bool loader_check_layer_list_for_phys_dev_ext_address(struct loader_instance *inst, const char *funcName) {
    struct loader_layer_properties *layer_prop_list = inst->expanded_activated_layer_list.list;
    for (uint32_t layer = 0; layer < inst->expanded_activated_layer_list.count; ++layer) {
        // If this layer supports the vk_layerGetPhysicalDeviceProcAddr, then call
        // it and see if it returns a valid pointer for this function name.
        if (layer_prop_list[layer].interface_version > 1) {
            const struct loader_layer_functions *const functions = &(layer_prop_list[layer].functions);
            if (NULL != functions->get_physical_device_proc_addr &&
                NULL != functions->get_physical_device_proc_addr((VkInstance)inst->instance, funcName)) {
                return true;
            }
        }
    }

    return false;
}

void loader_free_phys_dev_ext_table(struct loader_instance *inst) {
    for (uint32_t i = 0; i < MAX_NUM_UNKNOWN_EXTS; i++) {
        loader_instance_heap_free(inst, inst->phys_dev_ext_disp_hash[i].func_name);
        loader_instance_heap_free(inst, inst->phys_dev_ext_disp_hash[i].list.index);
    }
    memset(inst->phys_dev_ext_disp_hash, 0, sizeof(inst->phys_dev_ext_disp_hash));
}

bool loader_add_phys_dev_ext_table(struct loader_instance *inst, uint32_t *ptr_idx, const char *funcName) {
    uint32_t i;
    uint32_t idx = *ptr_idx;
    struct loader_dispatch_hash_list *list = &inst->phys_dev_ext_disp_hash[idx].list;

    if (!inst->phys_dev_ext_disp_hash[idx].func_name) {
        // no entry here at this idx, so use it
        assert(list->capacity == 0);
        inst->phys_dev_ext_disp_hash[idx].func_name =
            (char *)loader_instance_heap_alloc(inst, strlen(funcName) + 1, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (inst->phys_dev_ext_disp_hash[idx].func_name == NULL) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_add_phys_dev_ext_table() can't allocate memory for func_name");
            return false;
        }
        strncpy(inst->phys_dev_ext_disp_hash[idx].func_name, funcName, strlen(funcName) + 1);
        return true;
    }

    // check for enough capacity
    if (list->capacity == 0) {
        list->index = loader_instance_heap_alloc(inst, 8 * sizeof(*(list->index)), VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (list->index == NULL) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_add_phys_dev_ext_table() can't allocate list memory");
            return false;
        }
        list->capacity = 8 * sizeof(*(list->index));
    } else if (list->capacity < (list->count + 1) * sizeof(*(list->index))) {
        void *new_ptr = loader_instance_heap_realloc(inst, list->index, list->capacity, list->capacity * 2,
                                                     VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (NULL == new_ptr) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_add_phys_dev_ext_table() can't reallocate list memory");
            return false;
        }
        list->index = new_ptr;
        list->capacity *= 2;
    }

    // find an unused index in the hash table and use it
    i = (idx + 1) % MAX_NUM_UNKNOWN_EXTS;
    do {
        if (!inst->phys_dev_ext_disp_hash[i].func_name) {
            assert(inst->phys_dev_ext_disp_hash[i].list.capacity == 0);
            inst->phys_dev_ext_disp_hash[i].func_name =
                (char *)loader_instance_heap_alloc(inst, strlen(funcName) + 1, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
            if (inst->phys_dev_ext_disp_hash[i].func_name == NULL) {
                loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_add_phys_dev_ext_table() can't reallocate func_name memory");
                return false;
            }
            strncpy(inst->phys_dev_ext_disp_hash[i].func_name, funcName, strlen(funcName) + 1);
            list->index[list->count] = i;
            list->count++;
            *ptr_idx = i;
            return true;
        }
        i = (i + 1) % MAX_NUM_UNKNOWN_EXTS;
    } while (i != idx);

    loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_add_phys_dev_ext_table() couldn't insert into hash table; is it full?");
    return false;
}

bool loader_name_in_phys_dev_ext_table(struct loader_instance *inst, uint32_t *idx, const char *funcName) {
    uint32_t alt_idx;
    if (inst->phys_dev_ext_disp_hash[*idx].func_name && !strcmp(inst->phys_dev_ext_disp_hash[*idx].func_name, funcName))
        return true;

    // funcName wasn't at the primary spot in the hash table
    // search the list of secondary locations (shallow search, not deep search)
    for (uint32_t i = 0; i < inst->phys_dev_ext_disp_hash[*idx].list.count; i++) {
        alt_idx = inst->phys_dev_ext_disp_hash[*idx].list.index[i];
        if (inst->phys_dev_ext_disp_hash[*idx].func_name && !strcmp(inst->phys_dev_ext_disp_hash[*idx].func_name, funcName)) {
            *idx = alt_idx;
            return true;
        }
    }

    return false;
}

// This function returns a generic trampoline and/or terminator function
// address for any unknown physical device extension commands.  A hash
// table is used to keep a list of unknown entry points and their
// mapping to the physical device extension dispatch table (struct
// loader_phys_dev_ext_dispatch_table).
// For a given entry point string (funcName), if an existing mapping is
// found, then the trampoline address for that mapping is returned in
// tramp_addr (if it is not NULL) and the terminator address for that
// mapping is returned in term_addr (if it is not NULL). Otherwise,
// this unknown entry point has not been seen yet.
// If it has not been seen before, and perform_checking is 'true',
// check if a layer or and ICD supports it.  If so then a new entry in
// the hash table is initialized and the trampoline and/or terminator
// addresses are returned.
// Null is returned if the hash table is full or if no discovered layer or
// ICD returns a non-NULL GetProcAddr for it.
bool loader_phys_dev_ext_gpa(struct loader_instance *inst, const char *funcName, bool perform_checking, void **tramp_addr,
                             void **term_addr) {
    uint32_t idx;
    uint32_t seed = 0;
    bool success = false;

    if (inst == NULL) {
        goto out;
    }

    if (NULL != tramp_addr) {
        *tramp_addr = NULL;
    }
    if (NULL != term_addr) {
        *term_addr = NULL;
    }

    // We should always check to see if any ICD supports it.
    if (!loader_check_icds_for_phys_dev_ext_address(inst, funcName)) {
        // If we're not checking layers, or we are and it's not in a layer, just
        // return
        if (!perform_checking || !loader_check_layer_list_for_phys_dev_ext_address(inst, funcName)) {
            goto out;
        }
    }

    idx = murmurhash(funcName, strlen(funcName), seed) % MAX_NUM_UNKNOWN_EXTS;
    if (perform_checking && !loader_name_in_phys_dev_ext_table(inst, &idx, funcName)) {
        uint32_t i;

        loader_log(inst, VULKAN_LOADER_DEBUG_BIT, 0,
                   "loader_phys_dev_ext_gpa: Found unknown physical function %s, using index %u in table", funcName, idx);

        // Only need to add first one to get index in Instance.  Others will use
        // the same index.
        if (!loader_add_phys_dev_ext_table(inst, &idx, funcName)) {
            // couldn't perform the above function due to insufficient memory available
            goto out;
        }

        // Setup the ICD function pointers
        struct loader_icd_term *icd_term = inst->icd_terms;
        while (NULL != icd_term) {
            if (MIN_PHYS_DEV_EXTENSION_ICD_INTERFACE_VERSION <= icd_term->scanned_icd->interface_version &&
                NULL != icd_term->scanned_icd->GetPhysicalDeviceProcAddr) {
                icd_term->phys_dev_ext[idx] =
                    (PFN_PhysDevExt)icd_term->scanned_icd->GetPhysicalDeviceProcAddr(icd_term->instance, funcName);

                // Make sure we set the instance dispatch to point to the
                // loader's terminator now since we can at least handle it
                // in one ICD.
                inst->disp->phys_dev_ext[idx] = loader_get_phys_dev_ext_termin(idx);

                loader_log(inst, VULKAN_LOADER_DEBUG_BIT, 0, "loader_phys_dev_ext_gpa: Driver %s returned ptr %p for %s",
                           icd_term->scanned_icd->lib_name, inst->disp->phys_dev_ext[idx], funcName);
            } else {
                icd_term->phys_dev_ext[idx] = NULL;
            }

            icd_term = icd_term->next;
        }

        // Now, search for the first layer attached and query using it to get
        // the first entry point.
        for (i = 0; i < inst->expanded_activated_layer_list.count; i++) {
            struct loader_layer_properties *layer_prop = &inst->expanded_activated_layer_list.list[i];
            if (layer_prop->interface_version > 1 && NULL != layer_prop->functions.get_physical_device_proc_addr) {
                inst->disp->phys_dev_ext[idx] =
                    (PFN_PhysDevExt)layer_prop->functions.get_physical_device_proc_addr((VkInstance)inst->instance, funcName);
                if (NULL != inst->disp->phys_dev_ext[idx]) {
                    loader_log(inst, VULKAN_LOADER_DEBUG_BIT, 0, "loader_phys_dev_ext_gpa: Layer %s returned ptr %p for %s",
                               layer_prop->info.layerName, inst->disp->phys_dev_ext[idx], funcName);
                    break;
                }
            }
        }
    }

    if (NULL != tramp_addr) {
        *tramp_addr = loader_get_phys_dev_ext_tramp(idx);
    }

    if (NULL != term_addr) {
        *term_addr = loader_get_phys_dev_ext_termin(idx);
    }

    success = true;

out:
    return success;
}