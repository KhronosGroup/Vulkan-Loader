/*
 *
 * Copyright (c) 2014-2022 The Khronos Group Inc.
 * Copyright (c) 2014-2022 Valve Corporation
 * Copyright (c) 2014-2022 LunarG, Inc.
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

#if defined(_WIN32)

#include "loader_common.h"

#include <minwindef.h>
#include <cfgmgr32.h>
#include <sdkddkver.h>

// Windows specific initialization functionality
void windows_initialization(void);

// Look for data files in the registry.
VkResult windows_read_data_files_in_registry(const struct loader_instance* inst, enum loader_data_files_type data_file_type,
                                             bool warn_if_not_present, wchar_t* registry_location,
                                             struct loader_string_list* out_files);

// This function allocates an array in sorted_devices which must be freed by the caller if not null
VkResult windows_read_sorted_physical_devices(struct loader_instance* inst, uint32_t* icd_phys_devs_array_count,
                                              struct loader_icd_physical_devices** icd_phys_devs_array);

// This function sorts an array in physical device groups based on the sorted physical device information
VkResult windows_sort_physical_device_groups(struct loader_instance* inst, const uint32_t group_count,
                                             struct loader_physical_device_group_term* sorted_group_term,
                                             const uint32_t sorted_device_count,
                                             struct loader_icd_physical_devices* sorted_phys_dev_array);

// Creates a DXGI factory
// Returns VkLoaderFeatureFlags containing VK_LOADER_FEATURE_PHYSICAL_DEVICE_SORTING if successful, otherwise 0
VkLoaderFeatureFlags windows_initialize_dxgi(void);

// Retrieve a path to an installed app package that contains Vulkan manifests.
// When done using the returned string, the caller should free the pointer.
char* windows_get_app_package_manifest_path(const struct loader_instance* inst, bool* oom);

// Gets the path to the loader settings file, if it exists. If it doesn't exists, writes NULL to out_path.
// The path is located through the registry as an entry in HKEY_CURRENT_USER/SOFTWARE/Khronos/Vulkan/LoaderSettings
// and if nothing is there, will try to look in HKEY_LOCAL_MACHINE/SOFTWARE/Khronos/Vulkan/LoaderSettings.
// If running with elevated privileges, this function only looks in HKEY_LOCAL_MACHINE.
VkResult windows_get_loader_settings_file_path(const struct loader_instance* inst, char** out_path);
#endif  // WIN32
