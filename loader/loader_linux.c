/*
 *
 * Copyright (c) 2021 The Khronos Group Inc.
 * Copyright (c) 2021 Valve Corporation
 * Copyright (c) 2021 LunarG, Inc.
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
 * Author: Mark Young <marky@lunarg.com>
 *
 */

// Non-windows and non-apple only header file, guard it so that accidental
// inclusion doesn't cause unknown header include errors
#ifdef LOADER_ENABLE_LINUX_SORT

#include <stdio.h>
#include <stdlib.h>

#include "loader_linux.h"

#include "allocation.h"
#include "get_environment.h"
#include "loader.h"
#include "log.h"

// Determine a priority based on device type with the higher value being higher priority.
static uint32_t determine_priority_type_value(VkPhysicalDeviceType type) {
    switch (type) {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            return 10;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            return 5;
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            return 3;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            return 1;
        case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:  // Not really an enum, but throws warning if it's not here
            break;
    }
    return 0;
}

// Compare the two device types.
// This behaves similar to a qsort compare.
static int32_t device_type_compare(VkPhysicalDeviceType a, VkPhysicalDeviceType b) {
    uint32_t a_value = determine_priority_type_value(a);
    uint32_t b_value = determine_priority_type_value(b);
    if (a_value > b_value) {
        return -1;
    } else if (b_value > a_value) {
        return 1;
    }
    return 0;
}

// Used to compare two devices and determine which one should have priority.  The criteria is
// simple:
//   1) Default device ALWAYS wins
//   2) Sort by type
//   3) Sort by PCI bus ID
//   4) Ties broken by device_ID XOR vendor_ID comparison
int32_t compare_devices(const void *a, const void *b) {
    struct LinuxSortedDeviceInfo *left = (struct LinuxSortedDeviceInfo *)a;
    struct LinuxSortedDeviceInfo *right = (struct LinuxSortedDeviceInfo *)b;

    // Default device always gets priority
    if (left->default_device) {
        return -1;
    } else if (right->default_device) {
        return 1;
    }

    // Order by device type next
    int32_t dev_type_comp = device_type_compare(left->device_type, right->device_type);
    if (0 != dev_type_comp) {
        return dev_type_comp;
    }

    // Sort by PCI info (prioritize devices that have info over those that don't)
    if (left->has_pci_bus_info && !right->has_pci_bus_info) {
        return -1;
    } else if (!left->has_pci_bus_info && right->has_pci_bus_info) {
        return 1;
    } else if (left->has_pci_bus_info && right->has_pci_bus_info) {
        // Sort low to high PCI domain
        if (left->pci_domain < right->pci_domain) {
            return -1;
        } else if (left->pci_domain > right->pci_domain) {
            return 1;
        }
        // Sort low to high PCI bus
        if (left->pci_bus < right->pci_bus) {
            return -1;
        } else if (left->pci_bus > right->pci_bus) {
            return 1;
        }
        // Sort low to high PCI device
        if (left->pci_device < right->pci_device) {
            return -1;
        } else if (left->pci_device > right->pci_device) {
            return 1;
        }
        // Sort low to high PCI function
        if (left->pci_function < right->pci_function) {
            return -1;
        } else if (left->pci_function > right->pci_function) {
            return 1;
        }
    }

    // Somehow we have a tie above, so XOR vendorID and deviceID and compare
    uint32_t left_xord_dev_vend = left->device_id ^ left->vendor_id;
    uint32_t right_xord_dev_vend = right->device_id ^ right->vendor_id;
    if (left_xord_dev_vend < right_xord_dev_vend) {
        return -1;
    } else if (right_xord_dev_vend < left_xord_dev_vend) {
        return 1;
    }
    return 0;
}

// Used to compare two device groups and determine which one should have priority.
// NOTE: This assumes that devices in each group have already been sorted.
// The group sort criteria is simple:
//   1) Group with the default device ALWAYS wins
//   2) Group with the best device type for device 0 wins
//   3) Group with best PCI bus ID for device 0 wins
//   4) Ties broken by group device 0 device_ID XOR vendor_ID comparison
int32_t compare_device_groups(const void *a, const void *b) {
    struct loader_physical_device_group_term *grp_a = (struct loader_physical_device_group_term *)a;
    struct loader_physical_device_group_term *grp_b = (struct loader_physical_device_group_term *)b;

    // Use the first GPU's info from each group to sort the groups by
    struct LinuxSortedDeviceInfo *left = &grp_a->internal_device_info[0];
    struct LinuxSortedDeviceInfo *right = &grp_b->internal_device_info[0];

    // Default device always gets priority
    if (left->default_device) {
        return -1;
    } else if (right->default_device) {
        return 1;
    }

    // Order by device type next
    int32_t dev_type_comp = device_type_compare(left->device_type, right->device_type);
    if (0 != dev_type_comp) {
        return dev_type_comp;
    }

    // Sort by PCI info (prioritize devices that have info over those that don't)
    if (left->has_pci_bus_info && !right->has_pci_bus_info) {
        return -1;
    } else if (!left->has_pci_bus_info && right->has_pci_bus_info) {
        return 1;
    } else if (left->has_pci_bus_info && right->has_pci_bus_info) {
        // Sort low to high PCI domain
        if (left->pci_domain < right->pci_domain) {
            return -1;
        } else if (left->pci_domain > right->pci_domain) {
            return 1;
        }
        // Sort low to high PCI bus
        if (left->pci_bus < right->pci_bus) {
            return -1;
        } else if (left->pci_bus > right->pci_bus) {
            return 1;
        }
        // Sort low to high PCI device
        if (left->pci_device < right->pci_device) {
            return -1;
        } else if (left->pci_device > right->pci_device) {
            return 1;
        }
        // Sort low to high PCI function
        if (left->pci_function < right->pci_function) {
            return -1;
        } else if (left->pci_function > right->pci_function) {
            return 1;
        }
    }

    // Somehow we have a tie above, so XOR vendorID and deviceID and compare
    uint32_t left_xord_dev_vend = left->device_id ^ left->vendor_id;
    uint32_t right_xord_dev_vend = right->device_id ^ right->vendor_id;
    if (left_xord_dev_vend < right_xord_dev_vend) {
        return -1;
    } else if (right_xord_dev_vend < left_xord_dev_vend) {
        return 1;
    }
    return 0;
}

// Search for the default device using the loader environment variable and then the Mesa
// environment variable.  Both are formated the same.  This way, we can start working in
// the same fashion and the layer can "migrate" away over time.
const char loader_device_select_env_var[2][32] = {"VK_LOADER_DEVICE_SELECT", "MESA_VK_DEVICE_SELECT"};
static bool linux_env_var_default_device(struct loader_instance *inst, uint32_t device_count,
                                         struct LinuxSortedDeviceInfo *sorted_device_info) {
    for (uint32_t env_var = 0; env_var < 2; ++env_var) {
        char *selection = loader_getenv(loader_device_select_env_var[env_var], inst);
        if (NULL != selection) {
            loader_log(inst, VULKAN_LOADER_DEBUG_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                       "linux_env_var_default_device:  Found mesa environment variable %s set to %s",
                       loader_device_select_env_var[env_var], selection);

            // The environment variable exists, so grab the vendor ID and device ID of the
            // selected default device
            unsigned vendor_id, device_id;
            int32_t matched = sscanf(selection, "%x:%x", &vendor_id, &device_id);
            if (matched != 2) {
                return false;
            }

            bool found = false;
            for (int32_t i = 0; i < (int32_t)device_count; ++i) {
                if (sorted_device_info[i].vendor_id == vendor_id && sorted_device_info[i].device_id == device_id) {
                    loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                               "linux_env_var_default_device:  Found default at index %u \'%s\'", i,
                               sorted_device_info[i].device_name);
                    sorted_device_info[i].default_device = true;
                    found = true;
                    break;
                }
            }

            loader_free_getenv(selection, inst);
            return found;
        }
    }
    return false;
}

// Search for the default device using DRI_PRIME environment variable for device selection.
// NOTE: If DRI_PRIME="1", the algorithm should choose any non-default device.
const char dri_prime_device_select_env_var[] = "DRI_PRIME";
static bool linux_dri_prime_default_device(struct loader_instance *inst, uint32_t device_count,
                                           struct LinuxSortedDeviceInfo *sorted_device_info) {
    char *selection = loader_getenv(dri_prime_device_select_env_var, inst);
    if (NULL != selection) {
        loader_log(inst, VULKAN_LOADER_DEBUG_BIT, 0, "linux_dri_prime_default_device:  Found DRI environment variable %s set to %s",
                   dri_prime_device_select_env_var, selection);

        bool found = false;
        if (!strcmp("1", selection)) {
            int32_t selected_index = -1;
            for (int32_t i = 0; i < (int32_t)device_count; ++i) {
                if (sorted_device_info[i].has_pci_bus_info) {
                    char *tag = NULL;
                    loader_log(inst, VULKAN_LOADER_DEBUG_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                               "linux_dri_prime_default_device:  Checking \'pci-%04x_%02x_%02x_%1u\'",
                               sorted_device_info[i].pci_domain, sorted_device_info[i].pci_bus, sorted_device_info[i].pci_device,
                               sorted_device_info[i].pci_function);
                    if (asprintf(&tag, "pci-%04x_%02x_%02x_%1u", sorted_device_info[i].pci_domain, sorted_device_info[i].pci_bus,
                                 sorted_device_info[i].pci_device, sorted_device_info[i].pci_function) >= 0) {
                        // If not the device select it, if one is already selected, compare the two
                        if (strcmp(selection, tag)) {
                            if (selected_index < 0) {
                                loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                                           "linux_dri_prime_default_device:  Found default at index %u \'%s\'", i,
                                           sorted_device_info[i].device_name);
                                sorted_device_info[i].default_device = true;
                                selected_index = (int32_t)i;
                            } else {
                                // Since we already selected the previous device, unselect it for now for a good comparison.
                                // It will be reset if it "wins".
                                sorted_device_info[selected_index].default_device = false;
                                int32_t compare = compare_devices(&sorted_device_info[selected_index], &sorted_device_info[i]);
                                if (compare <= 0) {
                                    sorted_device_info[selected_index].default_device = false;
                                } else {
                                    sorted_device_info[i].default_device = false;
                                }
                            }
                        }
                    }
                    free(tag);
                }
            }
            // We might not find a device if the only device is the default device.
            if (selected_index >= 0) {
                found = true;
            }
        } else {
            // The environment variable exists, and isn't 1, so generat e a DRI ID using the PCI bus info and
            // compare
            for (int32_t i = 0; i < (int32_t)device_count; ++i) {
                if (sorted_device_info[i].has_pci_bus_info) {
                    char *tag = NULL;
                    loader_log(inst, VULKAN_LOADER_DEBUG_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                               "linux_dri_prime_default_device:  Checking \'pci-%04x_%02x_%02x_%1u\'",
                               sorted_device_info[i].pci_domain, sorted_device_info[i].pci_bus, sorted_device_info[i].pci_device,
                               sorted_device_info[i].pci_function);
                    if (asprintf(&tag, "pci-%04x_%02x_%02x_%1u", sorted_device_info[i].pci_domain, sorted_device_info[i].pci_bus,
                                 sorted_device_info[i].pci_device, sorted_device_info[i].pci_function) >= 0) {
                        if (!strcmp(selection, tag)) {
                            loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                                       "linux_dri_prime_default_device:  Found default at index %u \'%s\'", i,
                                       sorted_device_info[i].device_name);
                            sorted_device_info[i].default_device = true;
                            found = true;
                            break;
                        }
                    }
                    free(tag);
                }
            }
        }

        loader_free_getenv(selection, inst);
        return found;
    }
    return false;
}

// Search for the default device using the vga boot device information.
static bool linux_vga_boot_default_device(struct loader_instance *inst, uint32_t device_count,
                                          struct LinuxSortedDeviceInfo *sorted_device_info) {
    bool found = false;
    char boot_vga_path[4096];

    for (int32_t i = 0; i < (int32_t)device_count; ++i) {
        if (sorted_device_info[i].has_pci_bus_info) {
            // Probe the boot_vga device information for each device using the PCI info.
            snprintf(boot_vga_path, 4095, "/sys/bus/pci/devices/%04x:%02x:%02x.%x/boot_vga", sorted_device_info[i].pci_domain,
                     sorted_device_info[i].pci_bus, sorted_device_info[i].pci_device, sorted_device_info[i].pci_function);
            loader_log(inst, VULKAN_LOADER_DEBUG_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                       "linux_vga_boot_default_device:  Checking \'%s\'", boot_vga_path);

            FILE *vga_boot_file = fopen(boot_vga_path, "rt");
            // If file starts with a 1, this should be the default device
            if (NULL != vga_boot_file) {
                uint8_t val;
                if (fread(&val, 1, 1, vga_boot_file) == 1 && val == '1') {
                    loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                               "linux_vga_boot_default_device:  Found default device at index %u \'%s\'", i,
                               sorted_device_info[i].device_name);
                    sorted_device_info[i].default_device = true;
                    found = true;
                }
                fclose(vga_boot_file);
            }
            if (found) {
                break;
            }
        }
    }
    return found;
}

// This function allocates an array in sorted_devices which must be freed by the caller if not null
VkResult linux_read_sorted_physical_devices(struct loader_instance *inst, uint32_t icd_count,
                                            struct loader_phys_dev_per_icd *icd_devices,
                                            struct loader_physical_device_term **sorted_device_term) {
    VkResult res = VK_SUCCESS;
    bool is_vulkan_1_1 = false;
    if (inst->app_api_major_version >= 1 && inst->app_api_minor_version >= 1) {
        is_vulkan_1_1 = true;
    }

    struct LinuxSortedDeviceInfo *sorted_device_info = loader_instance_heap_alloc(
        inst, inst->total_gpu_count * sizeof(struct LinuxSortedDeviceInfo), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
    if (NULL == sorted_device_info) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    memset(sorted_device_info, 0, inst->total_gpu_count * sizeof(struct LinuxSortedDeviceInfo));

    loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0, "linux_read_sorted_physical_devices:  Original order:");

    // Grab all the necessary info we can about each device
    uint32_t index = 0;
    for (uint32_t icd_idx = 0; icd_idx < icd_count; ++icd_idx) {
        for (uint32_t phys_dev = 0; phys_dev < icd_devices[icd_idx].count; ++phys_dev) {
            struct loader_icd_term *icd_term = icd_devices[icd_idx].this_icd_term;
            VkPhysicalDeviceProperties dev_props = {};

            sorted_device_info[index].physical_device = icd_devices[icd_idx].phys_devs[phys_dev];
            sorted_device_info[index].icd_index = icd_idx;
            sorted_device_info[index].icd_term = icd_term;
            sorted_device_info[index].has_pci_bus_info = false;

            icd_term->dispatch.GetPhysicalDeviceProperties(sorted_device_info[index].physical_device, &dev_props);
            sorted_device_info[index].device_type = dev_props.deviceType;
            strncpy(sorted_device_info[index].device_name, dev_props.deviceName, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);
            sorted_device_info[index].vendor_id = dev_props.vendorID;
            sorted_device_info[index].device_id = dev_props.deviceID;

            bool device_is_1_1_capable =
                VK_API_VERSION_MAJOR(dev_props.apiVersion) == 1 && VK_API_VERSION_MINOR(dev_props.apiVersion) >= 1;
            sorted_device_info[index].has_pci_bus_info = device_is_1_1_capable;
            if (!sorted_device_info[index].has_pci_bus_info) {
                uint32_t ext_count;
                icd_term->dispatch.EnumerateDeviceExtensionProperties(sorted_device_info[index].physical_device, NULL, &ext_count,
                                                                      NULL);
                if (ext_count > 0) {
                    VkExtensionProperties *ext_props =
                        (VkExtensionProperties *)loader_stack_alloc(sizeof(VkExtensionProperties) * ext_count);
                    if (NULL == ext_props) {
                        return VK_ERROR_OUT_OF_HOST_MEMORY;
                    }
                    icd_term->dispatch.EnumerateDeviceExtensionProperties(sorted_device_info[index].physical_device, NULL,
                                                                          &ext_count, ext_props);
                    for (uint32_t ext = 0; ext < ext_count; ++ext) {
                        if (!strcmp(ext_props[ext].extensionName, VK_EXT_PCI_BUS_INFO_EXTENSION_NAME)) {
                            sorted_device_info[index].has_pci_bus_info = true;
                            break;
                        }
                    }
                }
            }

            if (sorted_device_info[index].has_pci_bus_info) {
                VkPhysicalDevicePCIBusInfoPropertiesEXT pci_props = (VkPhysicalDevicePCIBusInfoPropertiesEXT){
                    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT};
                VkPhysicalDeviceProperties2 dev_props2 = (VkPhysicalDeviceProperties2){
                    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, .pNext = (VkBaseInStructure *)&pci_props};
                if (is_vulkan_1_1 && device_is_1_1_capable) {
                    icd_term->dispatch.GetPhysicalDeviceProperties2(sorted_device_info[index].physical_device, &dev_props2);
                } else {
                    icd_term->dispatch.GetPhysicalDeviceProperties2KHR(sorted_device_info[index].physical_device, &dev_props2);
                }
                sorted_device_info[index].pci_domain = pci_props.pciDomain;
                sorted_device_info[index].pci_bus = pci_props.pciBus;
                sorted_device_info[index].pci_device = pci_props.pciDevice;
                sorted_device_info[index].pci_function = pci_props.pciFunction;
            }
            loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0, "           [%u] %s", index,
                       sorted_device_info[index].device_name);
            index++;
        }
    }

    // Select default device in the following order:
    //   1. If device select environment variable defined, use that
    //   2. If DRI Prime device select environment variable defined, use that
    //   3. If XCB is enabled, try selecting default XCB device.
    //   4. If VGA boot device defined, select that
    if (linux_env_var_default_device(inst, inst->total_gpu_count, sorted_device_info)) {
        // Print out selected
    } else if (linux_dri_prime_default_device(inst, inst->total_gpu_count, sorted_device_info)) {
        // Print out selected
    }
#if defined(VK_USE_PLATFORM_XCB_KHR) && defined(LOADER_ENABLE_DRM)
    else if (inst->wsi_xcb_surface_enabled && linux_find_xcb_default_device(inst, inst->total_gpu_count, sorted_device_info)) {
        // Print out selected
    }
#endif  // defined(VK_USE_PLATFORM_XCB_KHR) && defined(LOADER_ENABLE_DRM)
    else if (linux_vga_boot_default_device(inst, inst->total_gpu_count, sorted_device_info)) {
        // Print out selected
    }

    // Sort devices by PCI info
    qsort(sorted_device_info, inst->total_gpu_count, sizeof(struct LinuxSortedDeviceInfo), compare_devices);

    // If we have a selected index, add that first.
    loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0, "linux_read_sorted_physical_devices:  Order set to:");

    // Add all others after (they've already been sorted)
    for (uint32_t dev = 0; dev < inst->total_gpu_count; ++dev) {
        sorted_device_term[dev]->this_icd_term = sorted_device_info[dev].icd_term;
        sorted_device_term[dev]->icd_index = sorted_device_info[dev].icd_index;
        sorted_device_term[dev]->phys_dev = sorted_device_info[dev].physical_device;
        loader_set_dispatch((void *)sorted_device_term[dev], inst->disp);
        loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0, "           [%u] %s  %s", dev,
                   sorted_device_info[dev].device_name, (sorted_device_info[dev].default_device ? "[default]" : ""));
    }

    if (NULL != sorted_device_info) {
        loader_instance_heap_free(inst, sorted_device_info);
    }

    return res;
}

// This function allocates an array in sorted_devices which must be freed by the caller if not null
VkResult linux_read_sorted_physical_device_groups(struct loader_instance *inst, uint32_t group_count,
                                                  struct loader_physical_device_group_term *sorted_group_term) {
    VkResult res = VK_SUCCESS;
    bool is_vulkan_1_1 = false;
    if (inst->app_api_major_version >= 1 && inst->app_api_minor_version >= 1) {
        is_vulkan_1_1 = true;
    }

    loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
               "linux_read_sorted_physical_device_groups:  Original order:");

    for (uint32_t group = 0; group < group_count; ++group) {
        loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0, "           Group %u", group);

        struct loader_icd_term *icd_term = sorted_group_term[group].this_icd_term;
        for (uint32_t gpu = 0; gpu < sorted_group_term[group].group_props.physicalDeviceCount; ++gpu) {
            VkPhysicalDeviceProperties dev_props = {};

            sorted_group_term[group].internal_device_info[gpu].physical_device =
                sorted_group_term[group].group_props.physicalDevices[gpu];
            sorted_group_term[group].internal_device_info[gpu].has_pci_bus_info = false;

            icd_term->dispatch.GetPhysicalDeviceProperties(sorted_group_term[group].internal_device_info[gpu].physical_device,
                                                           &dev_props);
            sorted_group_term[group].internal_device_info[gpu].device_type = dev_props.deviceType;
            strncpy(sorted_group_term[group].internal_device_info[gpu].device_name, dev_props.deviceName,
                    VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);
            sorted_group_term[group].internal_device_info[gpu].vendor_id = dev_props.vendorID;
            sorted_group_term[group].internal_device_info[gpu].device_id = dev_props.deviceID;

            bool device_is_1_1_capable =
                VK_API_VERSION_MAJOR(dev_props.apiVersion) == 1 && VK_API_VERSION_MINOR(dev_props.apiVersion) >= 1;
            sorted_group_term[group].internal_device_info[gpu].has_pci_bus_info = device_is_1_1_capable;
            if (!sorted_group_term[group].internal_device_info[gpu].has_pci_bus_info) {
                uint32_t ext_count;
                icd_term->dispatch.EnumerateDeviceExtensionProperties(
                    sorted_group_term[group].internal_device_info[gpu].physical_device, NULL, &ext_count, NULL);
                if (ext_count > 0) {
                    VkExtensionProperties *ext_props =
                        (VkExtensionProperties *)loader_stack_alloc(sizeof(VkExtensionProperties) * ext_count);
                    if (NULL == ext_props) {
                        return VK_ERROR_OUT_OF_HOST_MEMORY;
                    }
                    icd_term->dispatch.EnumerateDeviceExtensionProperties(
                        sorted_group_term[group].internal_device_info[gpu].physical_device, NULL, &ext_count, ext_props);
                    for (uint32_t ext = 0; ext < ext_count; ++ext) {
                        if (!strcmp(ext_props[ext].extensionName, VK_EXT_PCI_BUS_INFO_EXTENSION_NAME)) {
                            sorted_group_term[group].internal_device_info[gpu].has_pci_bus_info = true;
                            break;
                        }
                    }
                }
            }

            if (sorted_group_term[group].internal_device_info[gpu].has_pci_bus_info) {
                VkPhysicalDevicePCIBusInfoPropertiesEXT pci_props = (VkPhysicalDevicePCIBusInfoPropertiesEXT){
                    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PCI_BUS_INFO_PROPERTIES_EXT};
                VkPhysicalDeviceProperties2 dev_props2 = (VkPhysicalDeviceProperties2){
                    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, .pNext = (VkBaseInStructure *)&pci_props};

                if (is_vulkan_1_1 && device_is_1_1_capable) {
                    icd_term->dispatch.GetPhysicalDeviceProperties2(
                        sorted_group_term[group].internal_device_info[gpu].physical_device, &dev_props2);
                } else {
                    icd_term->dispatch.GetPhysicalDeviceProperties2KHR(
                        sorted_group_term[group].internal_device_info[gpu].physical_device, &dev_props2);
                }

                sorted_group_term[group].internal_device_info[gpu].pci_domain = pci_props.pciDomain;
                sorted_group_term[group].internal_device_info[gpu].pci_bus = pci_props.pciBus;
                sorted_group_term[group].internal_device_info[gpu].pci_device = pci_props.pciDevice;
                sorted_group_term[group].internal_device_info[gpu].pci_function = pci_props.pciFunction;
            }
            loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0, "               [%u] %s", gpu,
                       sorted_group_term[group].internal_device_info[gpu].device_name);
        }

        // Select default device in the following order:
        //   1. If device select environment variable defined, use that
        //   2. If DRI Prime device select environment variable defined, use that
        //   3. If XCB is enabled, try selecting default XCB device.
        //   4. If VGA boot device defined, select that
        if (linux_env_var_default_device(inst, sorted_group_term[group].group_props.physicalDeviceCount,
                                         sorted_group_term[group].internal_device_info)) {
            // Print out selected
        } else if (linux_dri_prime_default_device(inst, sorted_group_term[group].group_props.physicalDeviceCount,
                                                  sorted_group_term[group].internal_device_info)) {
            // Print out selected
        }
#if defined(VK_USE_PLATFORM_XCB_KHR) && defined(LOADER_ENABLE_DRM)
        else if (inst->wsi_xcb_surface_enabled &&
                 linux_find_xcb_default_device(inst, sorted_group_term[group].group_props.physicalDeviceCount,
                                               sorted_group_term[group].internal_device_info)) {
            // Print out selected
        }
#endif  // defined(VK_USE_PLATFORM_XCB_KHR) && defined(LOADER_ENABLE_DRM)
        else if (linux_vga_boot_default_device(inst, sorted_group_term[group].group_props.physicalDeviceCount,
                                               sorted_group_term[group].internal_device_info)) {
            // Print out selected
        }

        // Sort GPUs in each group
        qsort(sorted_group_term[group].internal_device_info, sorted_group_term[group].group_props.physicalDeviceCount,
              sizeof(struct LinuxSortedDeviceInfo), compare_devices);
    }

    // Sort device groups by PCI info
    qsort(sorted_group_term, group_count, sizeof(struct loader_physical_device_group_term), compare_device_groups);

    if (loader_get_debug_level() & (VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT)) {
        loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "linux_read_sorted_physical_device_groups:  Sorted order:");
        for (uint32_t group = 0; group < group_count; ++group) {
            loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0, "           Group %u", group);
            for (uint32_t gpu = 0; gpu < sorted_group_term[group].group_props.physicalDeviceCount; ++gpu) {
                loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0, "               [%u] %s %s", gpu,
                           sorted_group_term[group].internal_device_info[gpu].device_name,
                           (sorted_group_term[group].internal_device_info[gpu].default_device ? "[default]" : ""));
            }
        }
    }

    return res;
}

#endif  // LOADER_ENABLE_LINUX_SORT