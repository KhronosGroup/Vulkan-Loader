/*
 *
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
 *
 * Author: Charles Giessen <charles@lunarg.com>
 *
 */

#include "physical_device_enumeration.h"

#include "allocation.h"
#include "loader.h"
#include "loader_environment.h"

#if defined(WIN32)
#include "loader_windows.h"
#endif
#if defined(LOADER_ENABLE_LINUX_SORT)
// This header is currently only used when sorting Linux devices, so don't include it otherwise.
#include "loader_linux.h"
#endif  // LOADER_ENABLE_LINUX_SORT

// Closes the library handle in the scanned ICD, free the lib_name string, and zeros out all data
void loader_unload_scanned_icd(struct loader_instance* inst, struct loader_scanned_icd* scanned_icd) {
    if (NULL == scanned_icd) {
        return;
    }
    if (scanned_icd->handle) {
        loader_platform_close_library(scanned_icd->handle);
        scanned_icd->handle = NULL;
    }
    loader_instance_heap_free(inst, scanned_icd->lib_name);
    memset(scanned_icd, 0, sizeof(struct loader_scanned_icd));
}

// Update the trampoline physical devices with the wrapped version.
// We always want to re-use previous physical device pointers since they may be used by an application
// after returning previously.
VkResult setup_loader_tramp_phys_devs(struct loader_instance* inst, uint32_t phys_dev_count, VkPhysicalDevice* phys_devs) {
    VkResult res = VK_SUCCESS;
    uint32_t found_count = 0;
    uint32_t old_count = inst->phys_dev_count_tramp;
    uint32_t new_count = inst->total_gpu_count;
    struct loader_physical_device_tramp** new_phys_devs = NULL;

    int32_t* old_to_new_index = NULL;
    int32_t* new_to_old_index = NULL;

    if (0 == phys_dev_count) {
        return VK_SUCCESS;
    }
    if (phys_dev_count > new_count) {
        new_count = phys_dev_count;
    }

    if (old_count > 0) {
        // We want an old to new index array and a new to old index array
        old_to_new_index =
            (int32_t*)loader_instance_heap_calloc(inst, sizeof(int32_t) * old_count, VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (NULL == old_to_new_index) {
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }
    }

    new_to_old_index = (int32_t*)loader_instance_heap_calloc(inst, sizeof(int32_t) * new_count, VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
    if (NULL == new_to_old_index) {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    // Initialize both
    for (uint32_t cur_idx = 0; cur_idx < old_count; ++cur_idx) {
        old_to_new_index[cur_idx] = -1;
    }
    for (uint32_t cur_idx = 0; cur_idx < new_count; ++cur_idx) {
        new_to_old_index[cur_idx] = -1;
    }

    // Figure out the old->new and new->old indices
    for (uint32_t cur_idx = 0; cur_idx < old_count; ++cur_idx) {
        for (uint32_t new_idx = 0; new_idx < phys_dev_count; ++new_idx) {
            if (inst->phys_devs_tramp[cur_idx]->phys_dev == phys_devs[new_idx]) {
                old_to_new_index[cur_idx] = (int32_t)new_idx;
                new_to_old_index[new_idx] = (int32_t)cur_idx;
                found_count++;
                break;
            }
        }
    }

    // If we found exactly the number of items we were looking for as we had before.  Then everything
    // we already have is good enough and we just need to update the array that was passed in with
    // the loader values.
    if (found_count == phys_dev_count && 0 != old_count && old_count == new_count) {
        for (uint32_t new_idx = 0; new_idx < phys_dev_count; ++new_idx) {
            for (uint32_t cur_idx = 0; cur_idx < old_count; ++cur_idx) {
                if (old_to_new_index[cur_idx] == (int32_t)new_idx) {
                    phys_devs[new_idx] = (VkPhysicalDevice)inst->phys_devs_tramp[cur_idx];
                    break;
                }
            }
        }
        // Nothing else to do for this path
        res = VK_SUCCESS;
    } else {
        // Something is different, so do the full path of checking every device and creating a new array to use.
        // This can happen if a device was added, or removed, or we hadn't previously queried all the data and we
        // have more to store.
        // Leave room to keep the old physical devices which are not enumerated anymore.
        new_phys_devs = (struct loader_physical_device_tramp**)loader_instance_heap_calloc(
            inst, sizeof(struct loader_physical_device_tramp*) * (new_count + old_count), VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (NULL == new_phys_devs) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "setup_loader_tramp_phys_devs:  Failed to allocate new physical device array of size %d", new_count);
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }

        if (new_count > phys_dev_count) {
            found_count = phys_dev_count;
        } else {
            found_count = new_count;
        }

        // First try to see if an old item exists that matches the new item.  If so, just copy it over.
        for (uint32_t new_idx = 0; new_idx < found_count; ++new_idx) {
            bool old_item_found = false;
            for (uint32_t cur_idx = 0; cur_idx < old_count; ++cur_idx) {
                if (old_to_new_index[cur_idx] == (int32_t)new_idx) {
                    // Copy over old item to correct spot in the new array
                    new_phys_devs[new_idx] = inst->phys_devs_tramp[cur_idx];
                    old_item_found = true;
                    break;
                }
            }
            // Something wasn't found, so it's new so add it to the new list
            if (!old_item_found) {
                new_phys_devs[new_idx] = loader_instance_heap_calloc(inst, sizeof(struct loader_physical_device_tramp),
                                                                     VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
                if (NULL == new_phys_devs[new_idx]) {
                    loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                               "setup_loader_tramp_phys_devs:  Failed to allocate new trampoline physical device");
                    res = VK_ERROR_OUT_OF_HOST_MEMORY;
                    goto out;
                }

                // Initialize the new physicalDevice object
                loader_set_dispatch((void*)new_phys_devs[new_idx], inst->disp);
                new_phys_devs[new_idx]->this_instance = inst;
                new_phys_devs[new_idx]->phys_dev = phys_devs[new_idx];
                new_phys_devs[new_idx]->magic = PHYS_TRAMP_MAGIC_NUMBER;
            }

            phys_devs[new_idx] = (VkPhysicalDevice)new_phys_devs[new_idx];
        }

        // We usually get here if the user array is smaller than the total number of devices, so copy the
        // remaining devices we have over to the new array.
        uint32_t start = found_count;
        for (uint32_t new_idx = start; new_idx < new_count; ++new_idx) {
            for (uint32_t cur_idx = 0; cur_idx < old_count; ++cur_idx) {
                if (old_to_new_index[cur_idx] == -1) {
                    new_phys_devs[new_idx] = inst->phys_devs_tramp[cur_idx];
                    old_to_new_index[cur_idx] = (int32_t)new_idx;
                    found_count++;
                    break;
                }
            }
        }
    }

out:

    if (NULL != new_phys_devs) {
        if (VK_SUCCESS != res) {
            for (uint32_t new_idx = 0; new_idx < found_count; ++new_idx) {
                // If an OOM occurred inside the copying of the new physical devices into the existing array
                // will leave some of the old physical devices in the array which may have been copied into
                // the new array, leading to them being freed twice. To avoid this we just make sure to not
                // delete physical devices which were copied.
                bool found = false;
                for (uint32_t cur_idx = 0; cur_idx < inst->phys_dev_count_tramp; cur_idx++) {
                    if (new_phys_devs[new_idx] == inst->phys_devs_tramp[cur_idx]) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    loader_instance_heap_free(inst, new_phys_devs[new_idx]);
                }
            }
            loader_instance_heap_free(inst, (void*)new_phys_devs);
        } else {
            if (new_count > inst->total_gpu_count) {
                inst->total_gpu_count = new_count;
            }
            // Keep everything in the old array that was not copied into the new array, the application may still be using
            // those physical devices. They are freed when the instance is destroyed.
            if (NULL != inst->phys_devs_tramp) {
                for (uint32_t i = 0; i < inst->phys_dev_count_tramp; i++) {
                    bool found = false;
                    for (uint32_t j = 0; j < found_count; j++) {
                        if (inst->phys_devs_tramp[i] == new_phys_devs[j]) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        new_phys_devs[found_count++] = inst->phys_devs_tramp[i];
                    }
                }
                loader_instance_heap_free(inst, (void*)inst->phys_devs_tramp);
            }
            inst->phys_devs_tramp = new_phys_devs;
            inst->phys_dev_count_tramp = found_count;
        }
    }
    if (VK_SUCCESS != res) {
        inst->total_gpu_count = 0;
    }
    loader_instance_heap_free(inst, old_to_new_index);
    loader_instance_heap_free(inst, new_to_old_index);

    return res;
}

#if defined(LOADER_ENABLE_LINUX_SORT)
bool is_linux_sort_enabled(struct loader_instance* inst) {
    bool sort_items = inst->supports_get_dev_prop_2;
    char* env_value = loader_getenv("VK_LOADER_DISABLE_SELECT", inst);
    if (NULL != env_value) {
        int32_t int_env_val = (int32_t)strtol(env_value, NULL, 10);
        loader_free_getenv(env_value, inst);
        if (int_env_val != 0) {
            sort_items = false;
        }
    }
    return sort_items;
}
#endif  // LOADER_ENABLE_LINUX_SORT

// Look for physical_device in the provided phys_devs list, return true if found and put the index into out_idx, otherwise
// return false
bool find_phys_dev(VkPhysicalDevice physical_device, uint32_t phys_devs_count, struct loader_physical_device_term** phys_devs,
                   uint32_t* out_idx) {
    if (NULL == phys_devs) return false;
    for (uint32_t idx = 0; idx < phys_devs_count; idx++) {
        if (NULL != phys_devs[idx] && physical_device == phys_devs[idx]->phys_dev) {
            *out_idx = idx;
            return true;
        }
    }
    return false;
}

// Add physical_device to new_phys_devs
VkResult check_and_add_to_new_phys_devs(struct loader_instance* inst, VkPhysicalDevice physical_device,
                                        struct loader_icd_physical_devices* dev_array, uint32_t* cur_new_phys_dev_count,
                                        struct loader_physical_device_term** new_phys_devs) {
    uint32_t out_idx = 0;
    uint32_t idx = *cur_new_phys_dev_count;
    // Check if the physical_device already exists in the new_phys_devs buffer, that means it was found from both
    // EnumerateAdapterPhysicalDevices and EnumeratePhysicalDevices and we need to skip it.
    if (find_phys_dev(physical_device, idx, new_phys_devs, &out_idx)) {
        return VK_SUCCESS;
    }
    // Check if it was found in a previous call to vkEnumeratePhysicalDevices, we can just copy over the old data.
    if (find_phys_dev(physical_device, inst->phys_dev_count_term + inst->removed_phys_dev_count_term, inst->phys_devs_term,
                      &out_idx)) {
        new_phys_devs[idx] = inst->phys_devs_term[out_idx];
        (*cur_new_phys_dev_count)++;
        return VK_SUCCESS;
    }

    // Exit in case something is already present - this shouldn't happen but better to be safe than overwrite existing data
    // since this code has been refactored a half dozen times.
    if (NULL != new_phys_devs[idx]) {
        return VK_SUCCESS;
    }
    // If this physical device is new, we need to allocate space for it.
    new_phys_devs[idx] =
        loader_instance_heap_calloc(inst, sizeof(struct loader_physical_device_term), VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    if (NULL == new_phys_devs[idx]) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "check_and_add_to_new_phys_devs:  Failed to allocate physical device terminator object %d", idx);
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    loader_set_dispatch((void*)new_phys_devs[idx], inst->disp);
    new_phys_devs[idx]->this_icd_term = dev_array->icd_term;
    new_phys_devs[idx]->phys_dev = physical_device;

    // Increment the count of new physical devices
    (*cur_new_phys_dev_count)++;
    return VK_SUCCESS;
}

/* Enumerate all physical devices from ICDs and add them to inst->phys_devs_term
 *
 * There are two methods to find VkPhysicalDevices - vkEnumeratePhysicalDevices and vkEnumerateAdapterPhysicalDevices
 * The latter is supported on windows only and on devices supporting ICD Interface Version 6 and greater.
 *
 * Once all physical devices are acquired, they need to be pulled into a single list of `loader_physical_device_term`'s.
 * They also need to be setup - the icd_term, icd_index, phys_dev, and disp (dispatch table) all need the correct data.
 * Additionally, we need to keep using already setup physical devices as they may be in use, thus anything enumerated
 * that is already in inst->phys_devs_term will be carried over.
 */

VkResult setup_loader_term_phys_devs(struct loader_instance* inst) {
    VkResult res = VK_SUCCESS;
    struct loader_icd_term* icd_term;
    uint32_t windows_sorted_devices_count = 0;
    struct loader_icd_physical_devices* windows_sorted_devices_array = NULL;
    uint32_t icd_count = 0;
    struct loader_icd_physical_devices* icd_phys_dev_array = NULL;
    uint32_t new_phys_devs_capacity = 0;
    uint32_t new_phys_devs_count = 0;
    struct loader_physical_device_term** new_phys_devs = NULL;
    const uint32_t old_phys_devs_count = inst->phys_dev_count_term + inst->removed_phys_dev_count_term;

    // No ICD's, no problem
    if (inst->icd_terms_count == 0) {
        return VK_SUCCESS;
    }

#if defined(_WIN32)
    // Get the physical devices supported by platform sorting mechanism into a separate list
    res = windows_read_sorted_physical_devices(inst, &windows_sorted_devices_count, &windows_sorted_devices_array);
    if (VK_SUCCESS != res) {
        goto out;
    }
#endif

    icd_count = inst->icd_terms_count;

    // Allocate something to store the physical device characteristics that we read from each ICD.
    icd_phys_dev_array = (struct loader_icd_physical_devices*)loader_instance_heap_calloc(
        inst, sizeof(struct loader_icd_physical_devices) * icd_count, VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
    if (NULL == icd_phys_dev_array) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "setup_loader_term_phys_devs:  Failed to allocate temporary ICD Physical device info array of size %d",
                   icd_count);
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    // For each ICD, query the number of physical devices, and then get an
    // internal value for those physical devices.
    icd_term = inst->icd_terms;
    uint32_t icd_idx = 0;
    while (NULL != icd_term) {
        struct loader_icd_physical_devices* icd_phys_dev = &icd_phys_dev_array[icd_idx];
        res = icd_term->dispatch.EnumeratePhysicalDevices(icd_term->instance, &icd_phys_dev->device_count, NULL);
        if (VK_ERROR_OUT_OF_HOST_MEMORY == res) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "setup_loader_term_phys_devs: Call to \'vkEnumeratePhysicalDevices\' in ICD %s failed with error code "
                       "VK_ERROR_OUT_OF_HOST_MEMORY",
                       icd_term->scanned_icd->lib_name);
            goto out;
        } else if (VK_SUCCESS == res) {
            // Prevents allocating a zero byte array
            if (icd_phys_dev->device_count == 0) {
                icd_phys_dev->physical_devices = 0;
            } else {
                icd_phys_dev->physical_devices = (VkPhysicalDevice*)loader_instance_heap_calloc(
                    inst, icd_phys_dev->device_count * sizeof(VkPhysicalDevice), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
                if (NULL == icd_phys_dev->physical_devices) {
                    loader_log(
                        inst, VULKAN_LOADER_ERROR_BIT, 0,
                        "setup_loader_term_phys_devs: Failed to allocate temporary ICD Physical device array for ICD %s of size %d",
                        icd_term->scanned_icd->lib_name, icd_phys_dev->device_count);
                    res = VK_ERROR_OUT_OF_HOST_MEMORY;
                    goto out;
                }

                uint32_t allocated_count = icd_phys_dev->device_count;
                res = icd_term->dispatch.EnumeratePhysicalDevices(icd_term->instance, &(icd_phys_dev->device_count),
                                                                  icd_phys_dev->physical_devices);
                if (VK_ERROR_OUT_OF_HOST_MEMORY == res) {
                    loader_log(
                        inst, VULKAN_LOADER_ERROR_BIT, 0,
                        "setup_loader_term_phys_devs: Call to \'vkEnumeratePhysicalDevices\' in ICD %s failed with error code "
                        "VK_ERROR_OUT_OF_HOST_MEMORY",
                        icd_term->scanned_icd->lib_name);
                    goto out;
                }
                if (VK_SUCCESS != res) {
                    loader_log(
                        inst, VULKAN_LOADER_ERROR_BIT, 0,
                        "setup_loader_term_phys_devs: Call to \'vkEnumeratePhysicalDevices\' in ICD %s failed with error code %d",
                        icd_term->scanned_icd->lib_name, res);
                    icd_phys_dev->device_count = 0;
                    loader_instance_heap_free(inst, (void*)icd_phys_dev->physical_devices);
                    icd_phys_dev->physical_devices = NULL;
                } else if (icd_phys_dev->device_count > allocated_count) {
                    // The fill call sizes the array, but never read past what we actually allocated.
                    icd_phys_dev->device_count = allocated_count;
                }
            }
        } else {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "setup_loader_term_phys_devs: Call to \'vkEnumeratePhysicalDevices\' in ICD %s failed with error code %d",
                       icd_term->scanned_icd->lib_name, res);
            icd_phys_dev->device_count = 0;
            icd_phys_dev->physical_devices = 0;
        }
        icd_phys_dev->icd_term = icd_term;
        icd_term->physical_device_count = icd_phys_dev->device_count;
        icd_term = icd_term->next;
        ++icd_idx;
    }

    // Add up both the windows sorted and non windows found physical device counts
    for (uint32_t i = 0; i < windows_sorted_devices_count; ++i) {
        new_phys_devs_capacity += windows_sorted_devices_array[i].device_count;
    }
    for (uint32_t i = 0; i < icd_count; ++i) {
        new_phys_devs_capacity += icd_phys_dev_array[i].device_count;
    }

    // Bail out if there are no physical devices reported
    if (0 == new_phys_devs_capacity) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "setup_loader_term_phys_devs:  Failed to detect any valid GPUs in the current config");
        res = VK_ERROR_INITIALIZATION_FAILED;
        goto out;
    }

    // Create an allocation large enough to hold both the windows sorting enumeration and non-windows physical device
    // enumeration, as well as the old physical devices which are not enumerated anymore.
    new_phys_devs = (struct loader_physical_device_term**)loader_instance_heap_calloc(
        inst, sizeof(struct loader_physical_device_term*) * (new_phys_devs_capacity + old_phys_devs_count),
        VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    if (NULL == new_phys_devs) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "setup_loader_term_phys_devs:  Failed to allocate new physical device array of size %d", new_phys_devs_capacity);
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    // Copy over everything found through sorted enumeration
    for (uint32_t i = 0; i < windows_sorted_devices_count; ++i) {
        for (uint32_t j = 0; j < windows_sorted_devices_array[i].device_count; ++j) {
            res = check_and_add_to_new_phys_devs(inst, windows_sorted_devices_array[i].physical_devices[j],
                                                 &windows_sorted_devices_array[i], &new_phys_devs_count, new_phys_devs);
            if (res == VK_ERROR_OUT_OF_HOST_MEMORY) {
                goto out;
            }
        }
    }

// Now go through the rest of the physical devices and add them to new_phys_devs
#if defined(LOADER_ENABLE_LINUX_SORT)

    if (is_linux_sort_enabled(inst)) {
        for (uint32_t dev = new_phys_devs_count; dev < new_phys_devs_capacity; ++dev) {
            new_phys_devs[dev] =
                loader_instance_heap_calloc(inst, sizeof(struct loader_physical_device_term), VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
            if (NULL == new_phys_devs[dev]) {
                loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                           "setup_loader_term_phys_devs:  Failed to allocate physical device terminator object %d", dev);
                res = VK_ERROR_OUT_OF_HOST_MEMORY;
                goto out;
            }
        }

        // Get the physical devices supported by platform sorting mechanism into a separate list
        // Pass in a sublist to the function so it only operates on the correct elements. This means passing in a pointer to the
        // current next element in new_phys_devs and passing in a `count` of currently unwritten elements
        res = linux_read_sorted_physical_devices(inst, icd_count, icd_phys_dev_array, new_phys_devs_capacity - new_phys_devs_count,
                                                 &new_phys_devs[new_phys_devs_count]);
        if (res == VK_ERROR_OUT_OF_HOST_MEMORY) {
            goto out;
        }
        // Keep previously allocated physical device info since apps may already be using that!
        for (uint32_t new_idx = new_phys_devs_count; new_idx < new_phys_devs_capacity; new_idx++) {
            for (uint32_t old_idx = 0; old_idx < old_phys_devs_count; old_idx++) {
                if (new_phys_devs[new_idx]->phys_dev == inst->phys_devs_term[old_idx]->phys_dev) {
                    loader_log(inst, VULKAN_LOADER_DEBUG_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                               "Copying old device %u into new device %u", old_idx, new_idx);
                    // Free the old new_phys_devs info since we're not using it before we assign the new info
                    loader_instance_heap_free(inst, new_phys_devs[new_idx]);
                    new_phys_devs[new_idx] = inst->phys_devs_term[old_idx];
                    break;
                }
            }
        }
        // now set the count to the capacity, as now the list is filled in
        new_phys_devs_count = new_phys_devs_capacity;
        // We want the following code to run if either linux sorting is disabled at compile time or runtime
    } else {
#endif  // LOADER_ENABLE_LINUX_SORT

        // Copy over everything found through the non-sorted means.
        for (uint32_t i = 0; i < icd_count; ++i) {
            for (uint32_t j = 0; j < icd_phys_dev_array[i].device_count; ++j) {
                res = check_and_add_to_new_phys_devs(inst, icd_phys_dev_array[i].physical_devices[j], &icd_phys_dev_array[i],
                                                     &new_phys_devs_count, new_phys_devs);
                if (res == VK_ERROR_OUT_OF_HOST_MEMORY) {
                    goto out;
                }
            }
        }
#if defined(LOADER_ENABLE_LINUX_SORT)
    }
#endif  // LOADER_ENABLE_LINUX_SORT
out:

    if (VK_SUCCESS != res) {
        if (NULL != new_phys_devs) {
            // We've encountered an error, so we should free the new buffers.
            for (uint32_t i = 0; i < new_phys_devs_capacity; i++) {
                // May not have allocated this far, skip it if we hadn't.
                if (new_phys_devs[i] == NULL) continue;

                // If an OOM occurred inside the copying of the new physical devices into the existing array
                // will leave some of the old physical devices in the array which may have been copied into
                // the new array, leading to them being freed twice. To avoid this we just make sure to not
                // delete physical devices which were copied.
                bool found = false;
                if (NULL != inst->phys_devs_term) {
                    for (uint32_t old_idx = 0; old_idx < old_phys_devs_count; old_idx++) {
                        if (new_phys_devs[i] == inst->phys_devs_term[old_idx]) {
                            found = true;
                            break;
                        }
                    }
                }
                if (!found) {
                    loader_instance_heap_free(inst, new_phys_devs[i]);
                }
            }
            loader_instance_heap_free(inst, (void*)new_phys_devs);
        }
        inst->total_gpu_count = 0;
    } else {
        uint32_t removed_phys_devs_count = 0;
        if (NULL != inst->phys_devs_term) {
            // Keep everything in the old array that was not copied into the new array, the application may still be using
            // those physical devices. They are stored after the enumerated ones and freed when the instance is destroyed.
            for (uint32_t i = 0; i < old_phys_devs_count; i++) {
                bool found = false;
                for (uint32_t j = 0; j < new_phys_devs_count + removed_phys_devs_count; j++) {
                    if (new_phys_devs != NULL && inst->phys_devs_term[i] == new_phys_devs[j]) {
                        found = true;
                        break;
                    }
                }
                if (!found && NULL != new_phys_devs) {
                    new_phys_devs[new_phys_devs_count + removed_phys_devs_count++] = inst->phys_devs_term[i];
                }
            }
            loader_instance_heap_free(inst, (void*)inst->phys_devs_term);
        }

        // Swap out old and new devices list
        inst->phys_dev_count_term = new_phys_devs_count;
        inst->removed_phys_dev_count_term = removed_phys_devs_count;
        inst->phys_devs_term = new_phys_devs;
        inst->total_gpu_count = new_phys_devs_count;
    }

    if (windows_sorted_devices_array != NULL) {
        for (uint32_t i = 0; i < windows_sorted_devices_count; ++i) {
            if (windows_sorted_devices_array[i].device_count > 0 && windows_sorted_devices_array[i].physical_devices != NULL) {
                loader_instance_heap_free(inst, (void*)windows_sorted_devices_array[i].physical_devices);
            }
        }
        loader_instance_heap_free(inst, windows_sorted_devices_array);
    }
    if (NULL != icd_phys_dev_array) {
        for (uint32_t i = 0; i < icd_count; i++) {
            loader_instance_heap_free(inst, (void*)icd_phys_dev_array[i].physical_devices);
        }

        loader_instance_heap_free(inst, icd_phys_dev_array);
    }

    return res;
}
/**
 * Iterates through all drivers and unloads any which do not contain physical devices.
 * This saves address space, which for 32 bit applications is scarce.
 * This must only be called after a call to vkEnumeratePhysicalDevices that isn't just querying the count
 */
void unload_drivers_without_physical_devices(struct loader_instance* inst) {
    struct loader_icd_term* cur_icd_term = inst->icd_terms;
    struct loader_icd_term* prev_icd_term = NULL;

    while (NULL != cur_icd_term) {
        struct loader_icd_term* next_icd_term = cur_icd_term->next;
        // A driver can stop reporting physical devices after the application created a VkDevice from one of them. Unloading it
        // then would free the loader_device that VkDevice dispatches through, so wait until its devices are destroyed.
        // The physical devices it reported before are kept until the instance is destroyed and still point at the driver, so
        // it can't be unloaded either.
        bool has_removed_phys_devs = false;
        for (uint32_t i = 0; i < inst->removed_phys_dev_count_term; i++) {
            if (inst->phys_devs_term[inst->phys_dev_count_term + i]->this_icd_term == cur_icd_term) {
                has_removed_phys_devs = true;
                break;
            }
        }
        if (cur_icd_term->physical_device_count == 0 && NULL == cur_icd_term->logical_device_list && !has_removed_phys_devs) {
            uint32_t cur_scanned_icd_index = UINT32_MAX;
            if (inst->icd_tramp_list.scanned_list) {
                for (uint32_t i = 0; i < inst->icd_tramp_list.count; i++) {
                    if (&(inst->icd_tramp_list.scanned_list[i]) == cur_icd_term->scanned_icd) {
                        cur_scanned_icd_index = i;
                        break;
                    }
                }
            }
            if (cur_scanned_icd_index != UINT32_MAX) {
                loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                           "Removing driver %s due to not having any physical devices", cur_icd_term->scanned_icd->lib_name);

                const VkAllocationCallbacks* allocation_callbacks = ignore_null_callback(&(inst->alloc_callbacks));
                if (cur_icd_term->instance) {
                    loader_icd_close_objects(inst, cur_icd_term);
                    cur_icd_term->dispatch.DestroyInstance(cur_icd_term->instance, allocation_callbacks);
                }
                cur_icd_term->instance = VK_NULL_HANDLE;
                loader_icd_destroy(inst, cur_icd_term, allocation_callbacks);
                cur_icd_term = NULL;
                struct loader_scanned_icd* scanned_icd_to_remove = &inst->icd_tramp_list.scanned_list[cur_scanned_icd_index];
                // Iterate through preloaded ICDs and remove the corresponding driver from that list
                loader_platform_thread_lock_mutex(&loader_preload_icd_lock);
                if (NULL != preloaded_icds.scanned_list) {
                    for (uint32_t i = 0; i < preloaded_icds.count; i++) {
                        if (NULL != preloaded_icds.scanned_list[i].lib_name && NULL != scanned_icd_to_remove->lib_name &&
                            strcmp(preloaded_icds.scanned_list[i].lib_name, scanned_icd_to_remove->lib_name) == 0) {
                            loader_unload_scanned_icd(NULL, &preloaded_icds.scanned_list[i]);
                            // condense the list so that it doesn't contain empty elements.
                            if (i < preloaded_icds.count - 1) {
                                memcpy((void*)&preloaded_icds.scanned_list[i],
                                       (void*)&preloaded_icds.scanned_list[preloaded_icds.count - 1],
                                       sizeof(struct loader_scanned_icd));
                                memset((void*)&preloaded_icds.scanned_list[preloaded_icds.count - 1], 0,
                                       sizeof(struct loader_scanned_icd));
                            }
                            if (i > 0) {
                                preloaded_icds.count--;
                            }

                            break;
                        }
                    }
                }
                loader_platform_thread_unlock_mutex(&loader_preload_icd_lock);

                loader_unload_scanned_icd(inst, scanned_icd_to_remove);
            }

            if (NULL == prev_icd_term) {
                inst->icd_terms = next_icd_term;
            } else {
                prev_icd_term->next = next_icd_term;
            }
        } else {
            prev_icd_term = cur_icd_term;
        }
        cur_icd_term = next_icd_term;
    }
}

VkResult setup_loader_tramp_phys_dev_groups(struct loader_instance* inst, uint32_t group_count,
                                            VkPhysicalDeviceGroupProperties* groups) {
    VkResult res = VK_SUCCESS;
    uint32_t cur_idx;
    uint32_t dev_idx;

    if (0 == group_count) {
        return VK_SUCCESS;
    }

    // Generate a list of all the devices and convert them to the loader ID
    uint32_t phys_dev_count = 0;
    for (cur_idx = 0; cur_idx < group_count; ++cur_idx) {
        phys_dev_count += groups[cur_idx].physicalDeviceCount;
    }

    if (phys_dev_count == 0) {
        return VK_SUCCESS;
    }

    VkPhysicalDevice* devices = (VkPhysicalDevice*)loader_instance_heap_calloc(inst, sizeof(VkPhysicalDevice) * phys_dev_count,
                                                                               VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
    if (NULL == devices) {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    uint32_t cur_device = 0;
    for (cur_idx = 0; cur_idx < group_count; ++cur_idx) {
        for (dev_idx = 0; dev_idx < groups[cur_idx].physicalDeviceCount; ++dev_idx) {
            devices[cur_device++] = groups[cur_idx].physicalDevices[dev_idx];
        }
    }

    // Update the devices based on the loader physical device values.
    res = setup_loader_tramp_phys_devs(inst, phys_dev_count, devices);
    if (VK_SUCCESS != res) {
        goto out;
    }

    // Update the devices in the group structures now
    cur_device = 0;
    for (cur_idx = 0; cur_idx < group_count; ++cur_idx) {
        for (dev_idx = 0; dev_idx < groups[cur_idx].physicalDeviceCount; ++dev_idx) {
            groups[cur_idx].physicalDevices[dev_idx] = devices[cur_device++];
        }
    }
out:
    loader_instance_heap_free(inst, (void*)devices);

    return res;
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_EnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount,
                                                                   VkPhysicalDevice* pPhysicalDevices) {
    struct loader_instance* inst = (struct loader_instance*)instance;
    VkResult res = VK_SUCCESS;

    // Always call the setup loader terminator physical devices because they may
    // have changed at any point.
    res = setup_loader_term_phys_devs(inst);
    if (VK_SUCCESS != res) {
        goto out;
    }

    if (inst->settings.settings_active && inst->settings.device_configurations_active) {
        // Use settings file device_configurations if present
        if (NULL == pPhysicalDevices) {
            // take the minimum of the settings configurations count and number of terminators
            *pPhysicalDeviceCount = (inst->settings.device_configuration_count < inst->phys_dev_count_term)
                                        ? inst->settings.device_configuration_count
                                        : inst->phys_dev_count_term;
        } else {
            res = loader_apply_settings_device_configurations(inst, pPhysicalDeviceCount, pPhysicalDevices);
        }
    } else {
        // Otherwise just copy the physical devices up normally and pass it up the chain
        uint32_t copy_count = inst->phys_dev_count_term;
        if (NULL != pPhysicalDevices) {
            if (copy_count > *pPhysicalDeviceCount) {
                copy_count = *pPhysicalDeviceCount;
                loader_log(inst, VULKAN_LOADER_INFO_BIT, 0,
                           "terminator_EnumeratePhysicalDevices : Trimming device count from %d to %d.", inst->phys_dev_count_term,
                           copy_count);
                res = VK_INCOMPLETE;
            }

            for (uint32_t i = 0; i < copy_count; i++) {
                pPhysicalDevices[i] = (VkPhysicalDevice)inst->phys_devs_term[i];
            }
        }

        *pPhysicalDeviceCount = copy_count;
    }

out:

    return res;
}

VkResult check_physical_device_extensions_for_driver_properties_extension(struct loader_physical_device_term* phys_dev_term,
                                                                          bool* supports_driver_properties) {
    *supports_driver_properties = false;
    uint32_t extension_count = 0;
    VkResult res = phys_dev_term->this_icd_term->dispatch.EnumerateDeviceExtensionProperties(phys_dev_term->phys_dev, NULL,
                                                                                             &extension_count, NULL);
    if (res != VK_SUCCESS || extension_count == 0) {
        return VK_SUCCESS;
    }
    VkExtensionProperties* extension_data =
        loader_instance_heap_calloc(phys_dev_term->this_icd_term->this_instance, sizeof(VkExtensionProperties) * extension_count,
                                    VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
    if (NULL == extension_data) {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    uint32_t allocated_count = extension_count;
    res = phys_dev_term->this_icd_term->dispatch.EnumerateDeviceExtensionProperties(phys_dev_term->phys_dev, NULL, &extension_count,
                                                                                    extension_data);
    if (res != VK_SUCCESS) {
        res = VK_SUCCESS;
        goto out;
    }
    // The count returned by the second call sizes the array, but never read past what we actually allocated.
    for (uint32_t j = 0; j < extension_count && j < allocated_count; j++) {
        if (!strcmp(extension_data[j].extensionName, VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME)) {
            *supports_driver_properties = true;
            goto out;
        }
    }
out:
    loader_instance_heap_free(phys_dev_term->this_icd_term->this_instance, extension_data);

    return res;
}

// Helper struct containing the relevant details of a VkPhysicalDevice necessary for applying the loader settings device
// configurations.
typedef struct physical_device_configuration_details {
    bool pd_was_added;
    bool pd_supports_vulkan_11;
    bool pd_supports_driver_properties;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceIDProperties device_id_properties;
    VkPhysicalDeviceDriverProperties driver_properties;

} physical_device_configuration_details;

// Apply the device_configurations in the settings file to the output VkPhysicalDeviceList.
// That means looking up each VkPhysicalDevice's deviceUUID, filtering using that, and putting them in the order of
// device_configurations in the settings file.
VkResult loader_apply_settings_device_configurations(struct loader_instance* inst, uint32_t* pPhysicalDeviceCount,
                                                     VkPhysicalDevice* pPhysicalDevices) {
    // No physical devices to apply settings to
    if (inst->phys_dev_count_term == 0) {
        if (inst->settings.device_configuration_count > 0) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                       "loader_apply_settings_device_configurations: settings file contained device_configurations but no "
                       "VkPhysicalDevices were enumerated");
            return VK_ERROR_INITIALIZATION_FAILED;
        }
        return VK_SUCCESS;
    }

    VkResult res = VK_SUCCESS;
    loader_log(inst, VULKAN_LOADER_INFO_BIT, 0,
               "Selecting and ordering VkPhysicalDevices to match the loader settings device configurations list");

    physical_device_configuration_details* pd_details = loader_instance_heap_calloc(
        inst, inst->phys_dev_count_term * sizeof(physical_device_configuration_details), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
    if (NULL == pd_details) {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    for (uint32_t i = 0; i < inst->phys_dev_count_term; i++) {
        struct loader_physical_device_term* phys_dev_term = inst->phys_devs_term[i];

        phys_dev_term->this_icd_term->dispatch.GetPhysicalDeviceProperties(phys_dev_term->phys_dev, &pd_details[i].properties);
        if (pd_details[i].properties.apiVersion < VK_API_VERSION_1_1) {
            // Device isn't eligible for sorting
            continue;
        }
        pd_details[i].pd_supports_vulkan_11 = true;
        if (pd_details[i].properties.apiVersion >= VK_API_VERSION_1_2) {
            pd_details[i].pd_supports_driver_properties = true;
        }

        // If this physical device isn't 1.2, then we need to check if it supports VK_KHR_driver_properties
        if (!pd_details[i].pd_supports_driver_properties) {
            res = check_physical_device_extensions_for_driver_properties_extension(phys_dev_term,
                                                                                   &pd_details[i].pd_supports_driver_properties);
            if (res == VK_ERROR_OUT_OF_HOST_MEMORY) {
                goto out;
            }
        }

        pd_details[i].device_id_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;
        pd_details[i].driver_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;
        if (pd_details[i].pd_supports_driver_properties) {
            pd_details[i].device_id_properties.pNext = (void*)&pd_details[i].driver_properties;
        }

        VkPhysicalDeviceProperties2 props2 = {0};
        props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        props2.pNext = (void*)&pd_details[i].device_id_properties;
        if (phys_dev_term->this_icd_term->dispatch.GetPhysicalDeviceProperties2) {
            phys_dev_term->this_icd_term->dispatch.GetPhysicalDeviceProperties2(phys_dev_term->phys_dev, &props2);
        }
    }

    // Loop over the setting's device configurations, find each VkPhysicalDevice which matches the deviceUUID given, add to the
    // pPhysicalDevices output list.
    uint32_t written_output_index = 0;

    for (uint32_t i = 0; i < inst->settings.device_configuration_count; i++) {
        uint8_t* current_deviceUUID = inst->settings.device_configurations[i].deviceUUID;
        uint8_t* current_driverUUID = inst->settings.device_configurations[i].driverUUID;
        bool configuration_found = false;
        for (uint32_t j = 0; j < inst->phys_dev_count_term; j++) {
            // Don't compare deviceUUID's if they have nothing, since we require deviceUUID's to effectively sort them.
            // Also skip devices an earlier configuration already selected so the same VkPhysicalDevice isn't output twice.
            if (!pd_details[j].pd_supports_vulkan_11 || pd_details[j].pd_was_added) {
                continue;
            }
            if (memcmp(current_deviceUUID, pd_details[j].device_id_properties.deviceUUID, sizeof(uint8_t) * VK_UUID_SIZE) == 0 &&
                memcmp(current_driverUUID, pd_details[j].device_id_properties.driverUUID, sizeof(uint8_t) * VK_UUID_SIZE) == 0 &&
                inst->settings.device_configurations[i].driverVersion == pd_details[j].properties.driverVersion) {
                configuration_found = true;
                // Catch when there are more device_configurations than space available in the output
                if (written_output_index >= *pPhysicalDeviceCount) {
                    *pPhysicalDeviceCount = written_output_index;  // write out how many were written
                    res = VK_INCOMPLETE;
                    goto out;
                }
                if (pd_details[j].pd_supports_driver_properties) {
                    loader_log(inst, VULKAN_LOADER_DEBUG_BIT, 0,
                               "pPhysicalDevices array index %d is set to \"%s\" (%s, version %u) ", written_output_index,
                               pd_details[j].properties.deviceName, pd_details[j].driver_properties.driverName,
                               pd_details[j].properties.driverVersion);
                } else {
                    loader_log(inst, VULKAN_LOADER_DEBUG_BIT, 0,
                               "pPhysicalDevices array index %d is set to \"%s\" (driver version %u) ", written_output_index,
                               pd_details[j].properties.deviceName, pd_details[j].properties.driverVersion);
                }
                pPhysicalDevices[written_output_index++] = (VkPhysicalDevice)inst->phys_devs_term[j];
                pd_details[j].pd_was_added = true;
                break;
            }
        }
        if (!configuration_found) {
            char device_uuid_str[UUID_STR_LEN] = {0};
            loader_log_generate_uuid_string(current_deviceUUID, device_uuid_str);
            char driver_uuid_str[UUID_STR_LEN] = {0};
            loader_log_generate_uuid_string(current_driverUUID, driver_uuid_str);

            // Log that this configuration was missing.
            if (inst->settings.device_configurations[i].deviceName[0] != '\0' &&
                inst->settings.device_configurations[i].driverName[0] != '\0') {
                loader_log(
                    inst, VULKAN_LOADER_WARN_BIT, 0,
                    "loader_apply_settings_device_configurations: settings file contained device_configuration which does not "
                    "appear in the enumerated VkPhysicalDevices. Missing VkPhysicalDevice with deviceName: \"%s\", "
                    "deviceUUID: %s, driverName: %s, driverUUID: %s, driverVersion: %u",
                    inst->settings.device_configurations[i].deviceName, device_uuid_str,
                    inst->settings.device_configurations[i].driverName, driver_uuid_str,
                    inst->settings.device_configurations[i].driverVersion);
            } else if (inst->settings.device_configurations[i].deviceName[0] != '\0') {
                loader_log(
                    inst, VULKAN_LOADER_WARN_BIT, 0,
                    "loader_apply_settings_device_configurations: settings file contained device_configuration which does not "
                    "appear in the enumerated VkPhysicalDevices. Missing VkPhysicalDevice with deviceName: \"%s\", "
                    "deviceUUID: %s, driverUUID: %s, driverVersion: %u",
                    inst->settings.device_configurations[i].deviceName, device_uuid_str, driver_uuid_str,
                    inst->settings.device_configurations[i].driverVersion);
            } else {
                loader_log(
                    inst, VULKAN_LOADER_WARN_BIT, 0,
                    "loader_apply_settings_device_configurations: settings file contained device_configuration which does not "
                    "appear in the enumerated VkPhysicalDevices. Missing VkPhysicalDevice with deviceUUID: "
                    "%s, driverUUID: %s, driverVersion: %u",
                    device_uuid_str, driver_uuid_str, inst->settings.device_configurations[i].driverVersion);
            }
        }
    }

    for (uint32_t j = 0; j < inst->phys_dev_count_term; j++) {
        if (!pd_details[j].pd_was_added) {
            loader_log(inst, VULKAN_LOADER_INFO_BIT, 0,
                       "VkPhysicalDevice \"%s\" did not appear in the settings file device configurations list, so was not added "
                       "to the pPhysicalDevices array",
                       pd_details[j].properties.deviceName);
        }
    }

    if (written_output_index == 0) {
        loader_log(inst, VULKAN_LOADER_WARN_BIT, 0,
                   "loader_apply_settings_device_configurations: None of the settings file device configurations had "
                   "deviceUUID's that corresponded to enumerated VkPhysicalDevices. Returning VK_ERROR_INITIALIZATION_FAILED");
        res = VK_ERROR_INITIALIZATION_FAILED;
        goto out;
    }

    *pPhysicalDeviceCount = written_output_index;  // update with how many were written

out:
    loader_instance_heap_free(inst, pd_details);

    return res;
}

VKAPI_ATTR VkResult VKAPI_CALL terminator_EnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice,
                                                                             const char* pLayerName, uint32_t* pPropertyCount,
                                                                             VkExtensionProperties* pProperties) {
    if (NULL == pPropertyCount) {
        return VK_INCOMPLETE;
    }

    struct loader_physical_device_term* phys_dev_term;

    // Any layer or trampoline wrapping should be removed at this point in time can just cast to the expected
    // type for VkPhysicalDevice.
    phys_dev_term = (struct loader_physical_device_term*)physicalDevice;

    // if we got here with a non-empty pLayerName, look up the extensions
    // from the json
    if (pLayerName != NULL && strlen(pLayerName) > 0) {
        uint32_t count;
        uint32_t copy_size;
        const struct loader_instance* inst = phys_dev_term->this_icd_term->this_instance;
        struct loader_device_extension_list* dev_ext_list = NULL;
        struct loader_device_extension_list local_ext_list;
        memset(&local_ext_list, 0, sizeof(local_ext_list));
        if (vk_string_validate(MaxLoaderStringLength, pLayerName) == VK_STRING_ERROR_NONE) {
            for (uint32_t i = 0; i < inst->instance_layer_list.count; i++) {
                struct loader_layer_properties* props = &inst->instance_layer_list.list[i];
                if (strcmp(props->info.layerName, pLayerName) == 0) {
                    dev_ext_list = &props->device_extension_list;
                }
            }

            count = (dev_ext_list == NULL) ? 0 : dev_ext_list->count;
            if (pProperties == NULL) {
                *pPropertyCount = count;
                loader_destroy_generic_list(inst, (struct loader_generic_list*)&local_ext_list);
                return VK_SUCCESS;
            }

            copy_size = *pPropertyCount < count ? *pPropertyCount : count;
            for (uint32_t i = 0; i < copy_size; i++) {
                memcpy(&pProperties[i], &dev_ext_list->list[i].props, sizeof(VkExtensionProperties));
            }
            *pPropertyCount = copy_size;

            loader_destroy_generic_list(inst, (struct loader_generic_list*)&local_ext_list);
            if (copy_size < count) {
                return VK_INCOMPLETE;
            }
        } else {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "vkEnumerateDeviceExtensionProperties:  pLayerName is too long or is badly formed");
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        return VK_SUCCESS;
    }

    // user is querying driver extensions and has supplied their own storage - just fill it out
    if (pProperties) {
        struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
        uint32_t written_count = *pPropertyCount;
        VkResult res =
            icd_term->dispatch.EnumerateDeviceExtensionProperties(phys_dev_term->phys_dev, NULL, &written_count, pProperties);
        if (res != VK_SUCCESS) {
            return res;
        }
        // A driver returning VK_SUCCESS must not claim it wrote more entries than the storage it was given. Clamp so the
        // de-duplication scan and the appends below stay inside the caller's pProperties buffer.
        if (written_count > *pPropertyCount) {
            written_count = *pPropertyCount;
        }

        // Iterate over active layers, if they are an implicit layer, add their device extensions
        // After calling into the driver, written_count contains the amount of device extensions written. We can therefore write
        // layer extensions starting at that point in pProperties
        for (uint32_t i = 0; i < icd_term->this_instance->expanded_activated_layer_list.count; i++) {
            struct loader_layer_properties* layer_props = icd_term->this_instance->expanded_activated_layer_list.list[i];
            if (0 == (layer_props->type_flags & VK_LAYER_TYPE_FLAG_EXPLICIT_LAYER)) {
                struct loader_device_extension_list* layer_ext_list = &layer_props->device_extension_list;
                for (uint32_t j = 0; j < layer_ext_list->count; j++) {
                    struct loader_dev_ext_props* cur_ext_props = &layer_ext_list->list[j];
                    // look for duplicates
                    if (has_vk_extension_property_array(&cur_ext_props->props, written_count, pProperties)) {
                        continue;
                    }

                    if (*pPropertyCount <= written_count) {
                        return VK_INCOMPLETE;
                    }

                    memcpy(&pProperties[written_count], &cur_ext_props->props, sizeof(VkExtensionProperties));
                    written_count++;
                }
            }
        }
        // Make sure we update the pPropertyCount with the how many were written
        *pPropertyCount = written_count;
        return res;
    }
    // Use `goto out;` for rest of this function

    // This case is during the call down the instance chain with pLayerName == NULL and pProperties == NULL
    struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
    struct loader_extension_list all_exts = {0};
    VkResult res = VK_SUCCESS;

    // We need to find the count without duplicates. This requires querying the driver for the names of the extensions.
    res = icd_term->dispatch.EnumerateDeviceExtensionProperties(phys_dev_term->phys_dev, NULL, &all_exts.count, NULL);
    if (res != VK_SUCCESS) {
        goto out;
    }
    // Then allocate memory to store the physical device extension list + the extensions layers provide
    // all_exts.count currently is the number of driver extensions
    all_exts.capacity = sizeof(VkExtensionProperties) * (all_exts.count + 20);
    all_exts.list = loader_instance_heap_calloc(icd_term->this_instance, all_exts.capacity, VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
    if (NULL == all_exts.list) {
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    // Get the available device extensions and put them in all_exts.list
    res = icd_term->dispatch.EnumerateDeviceExtensionProperties(phys_dev_term->phys_dev, NULL, &all_exts.count, all_exts.list);
    if (res != VK_SUCCESS) {
        goto out;
    }

    // Iterate over active layers, if they are an implicit layer, add their device extensions to all_exts.list
    for (uint32_t i = 0; i < icd_term->this_instance->expanded_activated_layer_list.count; i++) {
        struct loader_layer_properties* layer_props = icd_term->this_instance->expanded_activated_layer_list.list[i];
        if (0 == (layer_props->type_flags & VK_LAYER_TYPE_FLAG_EXPLICIT_LAYER)) {
            struct loader_device_extension_list* layer_ext_list = &layer_props->device_extension_list;
            for (uint32_t j = 0; j < layer_ext_list->count; j++) {
                res = loader_add_to_ext_list(icd_term->this_instance, &all_exts, 1, &layer_ext_list->list[j].props);
                if (res != VK_SUCCESS) {
                    goto out;
                }
            }
        }
    }

    // Write out the final de-duplicated count to pPropertyCount
    *pPropertyCount = all_exts.count;
    res = VK_SUCCESS;

out:

    loader_destroy_generic_list(icd_term->this_instance, (struct loader_generic_list*)&all_exts);
    return res;
}

// ---- Vulkan Core 1.1 terminators

VKAPI_ATTR VkResult VKAPI_CALL terminator_EnumeratePhysicalDeviceGroups(
    VkInstance instance, uint32_t* pPhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties) {
    struct loader_instance* inst = (struct loader_instance*)instance;

    VkResult res = VK_SUCCESS;
    struct loader_icd_term* icd_term;
    uint32_t total_count = 0;
    // The number of groups actually written into new_phys_dev_groups.  This is
    // less than total_count when groups are skipped, which happens when the
    // settings file hides a device that a group contains.
    uint32_t new_group_count = 0;
    uint32_t cur_icd_group_count = 0;
    VkPhysicalDeviceGroupProperties** new_phys_dev_groups = NULL;
    struct loader_physical_device_group_term* local_phys_dev_groups = NULL;
    size_t existing_phys_dev_array_alloc_size = 0;
    VkPhysicalDevice* phys_dev_array = NULL;
    PFN_vkEnumeratePhysicalDeviceGroups fpEnumeratePhysicalDeviceGroups = NULL;
    struct loader_icd_physical_devices* sorted_phys_dev_array = NULL;
    uint32_t sorted_count = 0;

    size_t tmp_group_props_alloc_size = 0;
    VkPhysicalDeviceGroupProperties* tmp_group_props = NULL;
    VkPhysicalDevice* visible_phys_devs = NULL;

    // For each ICD, query the number of physical device groups, and then get an
    // internal value for those physical devices.
    icd_term = inst->icd_terms;
    while (NULL != icd_term) {
        cur_icd_group_count = 0;

        // Get the function pointer to use to call into the ICD. This could be the core or KHR version
        if (inst->enabled_extensions.khr_device_group_creation) {
            fpEnumeratePhysicalDeviceGroups = icd_term->dispatch.EnumeratePhysicalDeviceGroupsKHR;
        } else {
            fpEnumeratePhysicalDeviceGroups = icd_term->dispatch.EnumeratePhysicalDeviceGroups;
        }

        if (NULL == fpEnumeratePhysicalDeviceGroups) {
            // Treat each ICD's GPU as it's own group if the extension isn't supported
            res = icd_term->dispatch.EnumeratePhysicalDevices(icd_term->instance, &cur_icd_group_count, NULL);
            if (res != VK_SUCCESS) {
                loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                           "terminator_EnumeratePhysicalDeviceGroups:  Failed during dispatch call of \'EnumeratePhysicalDevices\' "
                           "to ICD %s to get plain phys dev count.",
                           icd_term->scanned_icd->lib_name);
                icd_term = icd_term->next;
                continue;
            }
        } else {
            // Query the actual group info
            res = fpEnumeratePhysicalDeviceGroups(icd_term->instance, &cur_icd_group_count, NULL);
            if (res != VK_SUCCESS) {
                loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                           "terminator_EnumeratePhysicalDeviceGroups:  Failed during dispatch call of "
                           "\'EnumeratePhysicalDeviceGroups\' to ICD %s to get count.",
                           icd_term->scanned_icd->lib_name);
                icd_term = icd_term->next;
                continue;
            }
        }
        total_count += cur_icd_group_count;
        icd_term = icd_term->next;
    }

    // If GPUs not sorted yet, look through them and generate list of all available GPUs
    if (0 == total_count || 0 == inst->total_gpu_count) {
        res = setup_loader_term_phys_devs(inst);
        if (VK_SUCCESS != res) {
            goto out;
        }
    }

    if (NULL != pPhysicalDeviceGroupProperties && total_count > 0) {
        // Create an array for the new physical device groups, which will be stored
        // in the instance for the Terminator code.
        new_phys_dev_groups = (VkPhysicalDeviceGroupProperties**)loader_instance_heap_calloc(
            inst, total_count * sizeof(VkPhysicalDeviceGroupProperties*), VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (NULL == new_phys_dev_groups) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "terminator_EnumeratePhysicalDeviceGroups:  Failed to allocate new physical device group array of size %d",
                       total_count);
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }

        local_phys_dev_groups = loader_instance_heap_calloc(inst, sizeof(struct loader_physical_device_group_term) * total_count,
                                                            VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (NULL == local_phys_dev_groups) {
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }

#if defined(_WIN32)
        // Get the physical devices supported by platform sorting mechanism into a separate list
        res = windows_read_sorted_physical_devices(inst, &sorted_count, &sorted_phys_dev_array);
        if (VK_SUCCESS != res) {
            goto out;
        }
#endif

        cur_icd_group_count = 0;
        icd_term = inst->icd_terms;
        while (NULL != icd_term) {
            uint32_t count_this_time = total_count - cur_icd_group_count;

            // Get the function pointer to use to call into the ICD. This could be the core or KHR version
            if (inst->enabled_extensions.khr_device_group_creation) {
                fpEnumeratePhysicalDeviceGroups = icd_term->dispatch.EnumeratePhysicalDeviceGroupsKHR;
            } else {
                fpEnumeratePhysicalDeviceGroups = icd_term->dispatch.EnumeratePhysicalDeviceGroups;
            }

            if (NULL == fpEnumeratePhysicalDeviceGroups) {
                icd_term->dispatch.EnumeratePhysicalDevices(icd_term->instance, &count_this_time, NULL);

                // The driver can report more devices on this query than it did during the counting pass above. The local arrays
                // were sized to total_count, so never let this ICD write past the space still reserved for it.
                if (count_this_time > total_count - cur_icd_group_count) {
                    count_this_time = total_count - cur_icd_group_count;
                }

                if (count_this_time > 0) {
                    void* new_phys_dev_array = loader_instance_heap_realloc(
                        inst, (void*)phys_dev_array, existing_phys_dev_array_alloc_size, sizeof(VkPhysicalDevice) * count_this_time,
                        VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
                    if (NULL == new_phys_dev_array) {
                        loader_log(
                            inst, VULKAN_LOADER_ERROR_BIT, 0,
                            "terminator_EnumeratePhysicalDeviceGroups:  Failed to allocate local physical device array of size %d",
                            count_this_time);
                        res = VK_ERROR_OUT_OF_HOST_MEMORY;
                        goto out;
                    }
                    existing_phys_dev_array_alloc_size = sizeof(VkPhysicalDevice) * count_this_time;
                    phys_dev_array = (VkPhysicalDevice*)new_phys_dev_array;

                    res = icd_term->dispatch.EnumeratePhysicalDevices(icd_term->instance, &count_this_time, phys_dev_array);
                    if (res != VK_SUCCESS) {
                        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                                   "terminator_EnumeratePhysicalDeviceGroups:  Failed during dispatch call of "
                                   "\'EnumeratePhysicalDevices\' to ICD %s to get plain phys dev count.",
                                   icd_term->scanned_icd->lib_name);
                        goto out;
                    }

                    // Add each GPU as it's own group
                    for (uint32_t indiv_gpu = 0; indiv_gpu < count_this_time; indiv_gpu++) {
                        uint32_t cur_index = indiv_gpu + cur_icd_group_count;
                        local_phys_dev_groups[cur_index].this_icd_term = icd_term;
                        local_phys_dev_groups[cur_index].group_props.physicalDeviceCount = 1;
                        local_phys_dev_groups[cur_index].group_props.physicalDevices[0] = phys_dev_array[indiv_gpu];
                    }
                }
            } else {
                res = fpEnumeratePhysicalDeviceGroups(icd_term->instance, &count_this_time, NULL);
                if (res != VK_SUCCESS) {
                    loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                               "terminator_EnumeratePhysicalDeviceGroups:  Failed during dispatch call of "
                               "\'EnumeratePhysicalDeviceGroups\' to ICD %s to get group count.",
                               icd_term->scanned_icd->lib_name);
                    goto out;
                }
                // Same guard as the plain-device path: the re-queried count can exceed the space sized during the counting pass.
                if (count_this_time > total_count - cur_icd_group_count) {
                    count_this_time = total_count - cur_icd_group_count;
                }
                if (cur_icd_group_count + count_this_time < *pPhysicalDeviceGroupCount) {
                    // The total amount is still less than the amount of physical device group data passed in
                    // by the callee.  Therefore, we don't have to allocate any temporary structures and we
                    // can just use the data that was passed in.
                    res = fpEnumeratePhysicalDeviceGroups(icd_term->instance, &count_this_time,
                                                          &pPhysicalDeviceGroupProperties[cur_icd_group_count]);
                    if (res != VK_SUCCESS) {
                        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                                   "terminator_EnumeratePhysicalDeviceGroups:  Failed during dispatch call of "
                                   "\'EnumeratePhysicalDeviceGroups\' to ICD %s to get group information.",
                                   icd_term->scanned_icd->lib_name);
                        goto out;
                    }
                    for (uint32_t group = 0; group < count_this_time; ++group) {
                        uint32_t cur_index = group + cur_icd_group_count;
                        local_phys_dev_groups[cur_index].group_props = pPhysicalDeviceGroupProperties[cur_index];
                        // physicalDevices is a fixed VK_MAX_DEVICE_GROUP_SIZE array; don't trust a larger count from the ICD.
                        if (local_phys_dev_groups[cur_index].group_props.physicalDeviceCount > VK_MAX_DEVICE_GROUP_SIZE) {
                            local_phys_dev_groups[cur_index].group_props.physicalDeviceCount = VK_MAX_DEVICE_GROUP_SIZE;
                        }
                        local_phys_dev_groups[cur_index].this_icd_term = icd_term;
                    }
                } else {
                    if (count_this_time > 0) {
                        // There's not enough space in the callee's allocated pPhysicalDeviceGroupProperties structs,
                        // so we have to allocate temporary versions to collect all the data.  However, we need to make
                        // sure that at least the ones we do query utilize any pNext data in the callee's version.
                        void* new_tmp_group_props = loader_instance_heap_realloc(
                            inst, tmp_group_props, tmp_group_props_alloc_size,
                            count_this_time * sizeof(VkPhysicalDeviceGroupProperties), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
                        if (NULL == new_tmp_group_props) {
                            res = VK_ERROR_OUT_OF_HOST_MEMORY;
                            goto out;
                        }
                        tmp_group_props = (VkPhysicalDeviceGroupProperties*)new_tmp_group_props;
                        tmp_group_props_alloc_size = count_this_time * sizeof(VkPhysicalDeviceGroupProperties);

                        for (uint32_t group = 0; group < count_this_time; group++) {
                            tmp_group_props[group].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;
                            uint32_t cur_index = group + cur_icd_group_count;
                            if (*pPhysicalDeviceGroupCount > cur_index) {
                                tmp_group_props[group].pNext = pPhysicalDeviceGroupProperties[cur_index].pNext;
                            } else {
                                tmp_group_props[group].pNext = NULL;
                            }
                            tmp_group_props[group].subsetAllocation = false;
                        }

                        res = fpEnumeratePhysicalDeviceGroups(icd_term->instance, &count_this_time, tmp_group_props);
                        if (res != VK_SUCCESS) {
                            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                                       "terminator_EnumeratePhysicalDeviceGroups:  Failed during dispatch call of "
                                       "\'EnumeratePhysicalDeviceGroups\' to ICD %s  to get group information for temp data.",
                                       icd_term->scanned_icd->lib_name);
                            goto out;
                        }
                        // Same guard as the other two paths above: the ICD can report a larger completed count here than it did
                        // during the counting pass. tmp_group_props and local_phys_dev_groups were both sized to the earlier
                        // (clamped) count_this_time, so trusting a larger value here would overflow both stack allocations.
                        if (count_this_time > total_count - cur_icd_group_count) {
                            count_this_time = total_count - cur_icd_group_count;
                        }
                        for (uint32_t group = 0; group < count_this_time; ++group) {
                            uint32_t cur_index = group + cur_icd_group_count;
                            local_phys_dev_groups[cur_index].group_props = tmp_group_props[group];
                            // physicalDevices is a fixed VK_MAX_DEVICE_GROUP_SIZE array; don't trust a larger count from the ICD.
                            if (local_phys_dev_groups[cur_index].group_props.physicalDeviceCount > VK_MAX_DEVICE_GROUP_SIZE) {
                                local_phys_dev_groups[cur_index].group_props.physicalDeviceCount = VK_MAX_DEVICE_GROUP_SIZE;
                            }
                            local_phys_dev_groups[cur_index].this_icd_term = icd_term;
                        }
                    }
                }
                if (VK_SUCCESS != res) {
                    loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                               "terminator_EnumeratePhysicalDeviceGroups:  Failed during dispatch call of "
                               "\'EnumeratePhysicalDeviceGroups\' to ICD %s to get content.",
                               icd_term->scanned_icd->lib_name);
                    goto out;
                }
            }

            cur_icd_group_count += count_this_time;
            icd_term = icd_term->next;
        }

#if defined(LOADER_ENABLE_LINUX_SORT)
        if (is_linux_sort_enabled(inst)) {
            // Get the physical devices supported by platform sorting mechanism into a separate list
            res = linux_sort_physical_device_groups(inst, total_count, local_phys_dev_groups);
        }
#elif defined(_WIN32)
        // The Windows sorting information is only on physical devices.  We need to take that and convert it to the group
        // information if it's present.
        if (sorted_count > 0) {
            res =
                windows_sort_physical_device_groups(inst, total_count, local_phys_dev_groups, sorted_count, sorted_phys_dev_array);
        }
#endif  // LOADER_ENABLE_LINUX_SORT

        // Just to be safe, make sure we successfully completed setup_loader_term_phys_devs above
        // before attempting to do the following.  By verifying that setup_loader_term_phys_devs ran
        // first, it guarantees that each physical device will have a loader-specific handle.
        if (NULL != inst->phys_devs_term) {
            for (uint32_t group = 0; group < total_count; group++) {
                for (uint32_t group_gpu = 0; group_gpu < local_phys_dev_groups[group].group_props.physicalDeviceCount;
                     group_gpu++) {
                    bool found = false;
                    for (uint32_t term_gpu = 0; term_gpu < inst->phys_dev_count_term; term_gpu++) {
                        if (local_phys_dev_groups[group].group_props.physicalDevices[group_gpu] ==
                            inst->phys_devs_term[term_gpu]->phys_dev) {
                            local_phys_dev_groups[group].group_props.physicalDevices[group_gpu] =
                                (VkPhysicalDevice)inst->phys_devs_term[term_gpu];
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                                   "terminator_EnumeratePhysicalDeviceGroups:  Failed to find GPU %d in group %d returned by "
                                   "\'EnumeratePhysicalDeviceGroups\' in list returned by \'EnumeratePhysicalDevices\'",
                                   group_gpu, group);
                        res = VK_ERROR_INITIALIZATION_FAILED;
                        goto out;
                    }
                }
            }
        }

        // Apply the settings file's device_configurations to the groups.  If a group
        // contains an excluded VkPhysicalDevice, drop the whole group.
        if (inst->settings.settings_active && inst->settings.device_configurations_active && NULL != inst->phys_devs_term &&
            inst->phys_dev_count_term > 0) {
            uint32_t visible_count = inst->phys_dev_count_term;
            visible_phys_devs = (VkPhysicalDevice*)loader_instance_heap_calloc(inst, visible_count * sizeof(VkPhysicalDevice),
                                                                               VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
            if (NULL == visible_phys_devs) {
                res = VK_ERROR_OUT_OF_HOST_MEMORY;
                goto out;
            }

            // Reuse the same matching the non-group path uses, so the two can't drift apart.
            VkResult settings_res = loader_apply_settings_device_configurations(inst, &visible_count, visible_phys_devs);
            if (VK_SUCCESS != settings_res) {
                res = settings_res;
                goto out;
            }

            for (uint32_t group = 0; group < total_count; group++) {
                bool group_fully_visible = true;
                for (uint32_t group_gpu = 0; group_gpu < local_phys_dev_groups[group].group_props.physicalDeviceCount;
                     group_gpu++) {
                    bool found = false;
                    for (uint32_t vis = 0; vis < visible_count; vis++) {
                        if (local_phys_dev_groups[group].group_props.physicalDevices[group_gpu] == visible_phys_devs[vis]) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        group_fully_visible = false;
                        break;
                    }
                }

                if (!group_fully_visible) {
                    // Drop the whole group rather than removing the hidden device from it.
                    // The devices in a group are physically linked, so a group that has had
                    // a member removed no longer describes the hardware it claims to.
                    loader_log(inst, VULKAN_LOADER_INFO_BIT, 0,
                               "terminator_EnumeratePhysicalDeviceGroups: Physical device group %d contains a VkPhysicalDevice "
                               "which the settings file device configurations exclude, so the group was not reported.",
                               group);
                    local_phys_dev_groups[group].group_props.physicalDeviceCount = 0;
                    memset((void*)local_phys_dev_groups[group].group_props.physicalDevices, 0,
                           sizeof(local_phys_dev_groups[group].group_props.physicalDevices));
                }
            }
        }

        uint32_t idx = 0;

        // Copy or create everything to fill the new array of physical device groups
        for (uint32_t group = 0; group < total_count; group++) {
            // Skip groups which have been included through sorting, and groups the
            // settings file excluded above.
            if (local_phys_dev_groups[group].group_props.physicalDeviceCount == 0) {
                continue;
            }

            // Find the VkPhysicalDeviceGroupProperties object in local_phys_dev_groups
            VkPhysicalDeviceGroupProperties* group_properties = &local_phys_dev_groups[group].group_props;

            // Check if this physical device group with the same contents is already in the old buffer
            for (uint32_t old_idx = 0; old_idx < inst->phys_dev_group_count_term; old_idx++) {
                // Only carry an old group over once. Two identical groups (a driver reporting the same physical device in more
                // than one group) would otherwise both take the same allocation, which is then freed twice.
                bool already_used = false;
                for (uint32_t prev_idx = 0; prev_idx < idx; prev_idx++) {
                    if (new_phys_dev_groups[prev_idx] == inst->phys_dev_groups_term[old_idx]) {
                        already_used = true;
                        break;
                    }
                }
                if (already_used) {
                    continue;
                }
                if (NULL != group_properties && NULL != inst->phys_dev_groups_term[old_idx] &&
                    group_properties->physicalDeviceCount == inst->phys_dev_groups_term[old_idx]->physicalDeviceCount) {
                    bool found_all_gpus = true;
                    for (uint32_t old_gpu = 0; old_gpu < inst->phys_dev_groups_term[old_idx]->physicalDeviceCount; old_gpu++) {
                        bool found_gpu = false;
                        for (uint32_t new_gpu = 0; new_gpu < group_properties->physicalDeviceCount; new_gpu++) {
                            if (group_properties->physicalDevices[new_gpu] ==
                                inst->phys_dev_groups_term[old_idx]->physicalDevices[old_gpu]) {
                                found_gpu = true;
                                break;
                            }
                        }

                        if (!found_gpu) {
                            found_all_gpus = false;
                            break;
                        }
                    }
                    if (!found_all_gpus) {
                        continue;
                    }
                    new_phys_dev_groups[idx] = inst->phys_dev_groups_term[old_idx];
                    break;
                }
            }
            // If this physical device group isn't in the old buffer, create it
            if (group_properties != NULL && NULL == new_phys_dev_groups[idx]) {
                new_phys_dev_groups[idx] = (VkPhysicalDeviceGroupProperties*)loader_instance_heap_calloc(
                    inst, sizeof(VkPhysicalDeviceGroupProperties), VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
                if (NULL == new_phys_dev_groups[idx]) {
                    loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                               "terminator_EnumeratePhysicalDeviceGroups:  Failed to allocate physical device group Terminator "
                               "object %d",
                               idx);
                    total_count = idx;
                    res = VK_ERROR_OUT_OF_HOST_MEMORY;
                    goto out;
                }
                memcpy(new_phys_dev_groups[idx], group_properties, sizeof(VkPhysicalDeviceGroupProperties));
            }

            ++idx;
        }

        // Only idx entries were written; the rest of new_phys_dev_groups is still NULL
        // from the calloc above, so the count must reflect what was actually filled in.
        new_group_count = idx;
    }

out:

    if (NULL != pPhysicalDeviceGroupProperties) {
        if (VK_SUCCESS != res) {
            if (NULL != new_phys_dev_groups) {
                // We've encountered an error, so we should free the new buffers.
                for (uint32_t i = 0; i < total_count; i++) {
                    // If an OOM occurred inside the copying of the new physical device groups into the existing array will
                    // leave some of the old physical device groups in the array which may have been copied into the new array,
                    // leading to them being freed twice. To avoid this we just make sure to not delete physical device groups
                    // which were copied.
                    bool found = false;
                    if (NULL != inst->phys_devs_term) {
                        for (uint32_t old_idx = 0; old_idx < inst->phys_dev_group_count_term; old_idx++) {
                            if (new_phys_dev_groups[i] == inst->phys_dev_groups_term[old_idx]) {
                                found = true;
                                break;
                            }
                        }
                    }
                    if (!found) {
                        loader_instance_heap_free(inst, new_phys_dev_groups[i]);
                    }
                }
                loader_instance_heap_free(inst, (void*)new_phys_dev_groups);
            }
        } else {
            if (NULL != inst->phys_dev_groups_term) {
                // Free everything in the old array that was not copied into the new array
                // here.  We can't attempt to do that before here since the previous loop
                // looking before the "out:" label may hit an out of memory condition resulting
                // in memory leaking.
                for (uint32_t i = 0; i < inst->phys_dev_group_count_term; i++) {
                    bool found = false;
                    for (uint32_t j = 0; j < total_count; j++) {
                        if (inst->phys_dev_groups_term[i] == new_phys_dev_groups[j]) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        loader_instance_heap_free(inst, inst->phys_dev_groups_term[i]);
                    }
                }
                loader_instance_heap_free(inst, (void*)inst->phys_dev_groups_term);
            }

            // Swap in the new physical device group list
            inst->phys_dev_group_count_term = new_group_count;
            inst->phys_dev_groups_term = new_phys_dev_groups;
        }

        if (sorted_phys_dev_array != NULL) {
            for (uint32_t i = 0; i < sorted_count; ++i) {
                if (sorted_phys_dev_array[i].device_count > 0 && sorted_phys_dev_array[i].physical_devices != NULL) {
                    loader_instance_heap_free(inst, (void*)sorted_phys_dev_array[i].physical_devices);
                }
            }
            loader_instance_heap_free(inst, sorted_phys_dev_array);
        }

        uint32_t copy_count = inst->phys_dev_group_count_term;
        if (NULL != pPhysicalDeviceGroupProperties) {
            if (copy_count > *pPhysicalDeviceGroupCount) {
                copy_count = *pPhysicalDeviceGroupCount;
                loader_log(inst, VULKAN_LOADER_INFO_BIT, 0,
                           "terminator_EnumeratePhysicalDeviceGroups : Trimming device count from %d to %d.",
                           inst->phys_dev_group_count_term, copy_count);
                res = VK_INCOMPLETE;
            }

            for (uint32_t i = 0; i < copy_count; i++) {
                memcpy(&pPhysicalDeviceGroupProperties[i], inst->phys_dev_groups_term[i], sizeof(VkPhysicalDeviceGroupProperties));
            }
        }

        *pPhysicalDeviceGroupCount = copy_count;

    } else {
        *pPhysicalDeviceGroupCount = total_count;
    }

    loader_instance_heap_free(inst, local_phys_dev_groups);
    loader_instance_heap_free(inst, (void*)phys_dev_array);
    loader_instance_heap_free(inst, tmp_group_props);
    loader_instance_heap_free(inst, (void*)visible_phys_devs);

    return res;
}

VkResult get_device_driver_id(const struct loader_instance* inst, VkPhysicalDevice physicalDevice, VkDriverId* driverId) {
    // NOLINTNEXTLINE(bugprone-invalid-enum-default-initialization) - VkDriverId is an external enum with no zero value
    VkPhysicalDeviceDriverProperties physical_device_driver_props = {0};
    physical_device_driver_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;

    VkPhysicalDeviceProperties2 props2 = {0};
    props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    props2.pNext = &physical_device_driver_props;

    assert(inst != NULL);

    // physicalDevice was enumerated from the top of the layer chain, so a layer may have wrapped it and it must not be treated as a
    // loader_physical_device_term. Query it through the layer chain instead. This could be the core or KHR version
    PFN_vkGetPhysicalDeviceProperties2 fpGetPhysicalDeviceProperties2 = NULL;
    if (loader_check_version_meets_required(LOADER_VERSION_1_1_0, inst->app_api_version)) {
        fpGetPhysicalDeviceProperties2 = inst->disp->layer_inst_disp.GetPhysicalDeviceProperties2;
    }
    if (fpGetPhysicalDeviceProperties2 == NULL && inst->enabled_extensions.khr_get_physical_device_properties2) {
        fpGetPhysicalDeviceProperties2 = inst->disp->layer_inst_disp.GetPhysicalDeviceProperties2KHR;
    }

    if (fpGetPhysicalDeviceProperties2 == NULL) {
        // NOLINTNEXTLINE(clang-analyzer-optin.core.EnumCastOutOfRange) - VkDriverId is an external enum with no zero value
        *driverId = 0;
        return VK_ERROR_UNKNOWN;
    }

    fpGetPhysicalDeviceProperties2(physicalDevice, &props2);

    *driverId = physical_device_driver_props.driverID;
    return VK_SUCCESS;
}

VkResult loader_filter_enumerated_physical_devices(const struct loader_instance* inst,
                                                   const struct loader_envvar_id_filter* device_id_filter,
                                                   const struct loader_envvar_id_filter* vendor_id_filter,
                                                   const struct loader_envvar_id_filter* driver_id_filter,
                                                   const uint32_t in_PhysicalDeviceCount,
                                                   const VkPhysicalDevice* in_pPhysicalDevices, uint32_t* out_pPhysicalDeviceCount,
                                                   VkPhysicalDevice* out_pPhysicalDevices) {
    uint32_t filtered_physical_device_count = 0;
    for (uint32_t i = 0; i < in_PhysicalDeviceCount; i++) {
        VkPhysicalDeviceProperties dev_props = {0};
        inst->disp->layer_inst_disp.GetPhysicalDeviceProperties(in_pPhysicalDevices[i], &dev_props);

        if ((0 != device_id_filter->count) && !check_id_matches_filter_environment_var(dev_props.deviceID, device_id_filter)) {
            continue;
        }

        if ((0 != vendor_id_filter->count) && !check_id_matches_filter_environment_var(dev_props.vendorID, vendor_id_filter)) {
            continue;
        }

        if (0 != driver_id_filter->count) {
            VkDriverId driver_id;
            VkResult res = get_device_driver_id(inst, in_pPhysicalDevices[i], &driver_id);

            if ((res != VK_SUCCESS) || !check_id_matches_filter_environment_var(driver_id, driver_id_filter)) {
                continue;
            }
        }

        if ((NULL != out_pPhysicalDevices) && (filtered_physical_device_count < *out_pPhysicalDeviceCount)) {
            out_pPhysicalDevices[filtered_physical_device_count] = in_pPhysicalDevices[i];
        }
        filtered_physical_device_count++;
    }

    if ((NULL == out_pPhysicalDevices) || (filtered_physical_device_count < *out_pPhysicalDeviceCount)) {
        *out_pPhysicalDeviceCount = filtered_physical_device_count;
    }

    return (*out_pPhysicalDeviceCount < filtered_physical_device_count) ? VK_INCOMPLETE : VK_SUCCESS;
}

VkResult loader_filter_enumerated_physical_device_groups(
    const struct loader_instance* inst, const struct loader_envvar_id_filter* device_id_filter,
    const struct loader_envvar_id_filter* vendor_id_filter, const struct loader_envvar_id_filter* driver_id_filter,
    const uint32_t in_PhysicalDeviceGroupCount, const VkPhysicalDeviceGroupProperties* in_pPhysicalDeviceGroupProperties,
    uint32_t* out_PhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties* out_pPhysicalDeviceGroupProperties) {
    uint32_t filtered_physical_device_group_count = 0;
    for (uint32_t i = 0; i < in_PhysicalDeviceGroupCount; i++) {
        const VkPhysicalDeviceGroupProperties* device_group = &in_pPhysicalDeviceGroupProperties[i];

        bool skip_group = false;
        for (uint32_t j = 0; j < device_group->physicalDeviceCount; j++) {
            VkPhysicalDeviceProperties dev_props = {0};
            inst->disp->layer_inst_disp.GetPhysicalDeviceProperties(device_group->physicalDevices[j], &dev_props);

            if ((0 != device_id_filter->count) && !check_id_matches_filter_environment_var(dev_props.deviceID, device_id_filter)) {
                skip_group = true;
                break;
            }

            if ((0 != vendor_id_filter->count) && !check_id_matches_filter_environment_var(dev_props.vendorID, vendor_id_filter)) {
                skip_group = true;
                break;
            }

            if (0 != driver_id_filter->count) {
                VkDriverId driver_id;
                VkResult res = get_device_driver_id(inst, device_group->physicalDevices[j], &driver_id);

                if ((res != VK_SUCCESS) || !check_id_matches_filter_environment_var(driver_id, driver_id_filter)) {
                    skip_group = true;
                    break;
                }
            }
        }

        if (skip_group) {
            continue;
        }

        if ((NULL != out_pPhysicalDeviceGroupProperties) &&
            (filtered_physical_device_group_count < *out_PhysicalDeviceGroupCount)) {
            out_pPhysicalDeviceGroupProperties[filtered_physical_device_group_count] = *device_group;
        }

        filtered_physical_device_group_count++;
    }

    if ((NULL == out_pPhysicalDeviceGroupProperties) || (filtered_physical_device_group_count < *out_PhysicalDeviceGroupCount)) {
        *out_PhysicalDeviceGroupCount = filtered_physical_device_group_count;
    }

    return (*out_PhysicalDeviceGroupCount < filtered_physical_device_group_count) ? VK_INCOMPLETE : VK_SUCCESS;
}
