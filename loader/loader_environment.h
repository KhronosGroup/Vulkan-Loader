/*
 *
 * Copyright (c) 2014-2023 The Khronos Group Inc.
 * Copyright (c) 2014-2023 Valve Corporation
 * Copyright (c) 2014-2023 LunarG, Inc.
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

// Common functionality for accessing environemnt variables and whether the process is running in priviledged mode

char *loader_getenv(const char *name, const struct loader_instance *inst);
void loader_free_getenv(char *val, const struct loader_instance *inst);

#if defined(WIN32) || defined(__linux__) || defined(__APPLE__) || defined(__Fuchsia__) || defined(__QNXNTO__) || \
    defined(__FreeBSD__) || defined(__OpenBSD__)

bool is_high_integrity();

char *loader_secure_getenv(const char *name, const struct loader_instance *inst);

#endif

bool check_name_matches_filter_environment_var(const struct loader_instance *inst, const char *name,
                                               const struct loader_envvar_filter *filter_struct);
VkResult loader_add_environment_layers(struct loader_instance *inst, const enum layer_type_flags type_flags,
                                       struct loader_layer_list *target_list, struct loader_layer_list *expanded_target_list,
                                       const struct loader_layer_list *source_list);

// Functionality used for the new loader settings structure and file

VkResult generateCompleteSearchPath(const struct loader_instance *inst, enum loader_data_files_type search_type,
                                    const char *default_path, const char *override_path, const char *add_path, uint32_t *count,
                                    char ***value);
void freeList(const struct loader_instance *inst, uint32_t *count, char ***list);
void freeSearchPath(const struct loader_instance *inst, char **search_path);
VkResult generateSettingsStruct(struct loader_instance *inst, struct loader_settings **set_struc);
void freeSettingsStruct(struct loader_instance *inst, struct loader_settings **set_struct);
