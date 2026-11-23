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

#pragma once

#include "loader_common.h"

VkResult setup_loader_tramp_phys_devs(struct loader_instance* inst, uint32_t phys_dev_count, VkPhysicalDevice* phys_devs);
VkResult setup_loader_tramp_phys_dev_groups(struct loader_instance* inst, uint32_t group_count,
                                            VkPhysicalDeviceGroupProperties* groups);
void unload_drivers_without_physical_devices(struct loader_instance* inst);

VkResult loader_apply_settings_device_configurations(struct loader_instance* inst, uint32_t* pPhysicalDeviceCount,
                                                     VkPhysicalDevice* pPhysicalDevices);

VkResult loader_filter_enumerated_physical_devices(const struct loader_instance* inst,
                                                   const struct loader_envvar_id_filter* device_id_filter,
                                                   const struct loader_envvar_id_filter* vendor_id_filter,
                                                   const struct loader_envvar_id_filter* driver_id_filter,
                                                   const uint32_t in_PhysicalDeviceCount,
                                                   const VkPhysicalDevice* in_pPhysicalDevices, uint32_t* out_pPhysicalDeviceCount,
                                                   VkPhysicalDevice* out_pPhysicalDevices);

VkResult loader_filter_enumerated_physical_device_groups(
    const struct loader_instance* inst, const struct loader_envvar_id_filter* device_id_filter,
    const struct loader_envvar_id_filter* vendor_id_filter, const struct loader_envvar_id_filter* driver_id_filter,
    const uint32_t in_PhysicalDeviceGroupCount, const VkPhysicalDeviceGroupProperties* in_pPhysicalDeviceGroupProperties,
    uint32_t* out_PhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties* out_pPhysicalDeviceGroupProperties);
