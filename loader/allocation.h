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
 */

#pragma once

#include "loader_common.h"

void *loader_instance_heap_alloc(const struct loader_instance *instance, size_t size, VkSystemAllocationScope allocationScope);
void loader_instance_heap_free(const struct loader_instance *instance, void *pMemory);
void *loader_instance_heap_realloc(const struct loader_instance *instance, void *pMemory, size_t orig_size, size_t size,
                                   VkSystemAllocationScope alloc_scope);
void *loader_device_heap_alloc(const struct loader_device *device, size_t size, VkSystemAllocationScope allocationScope);
void loader_device_heap_free(const struct loader_device *device, void *pMemory);
void *loader_device_heap_realloc(const struct loader_device *device, void *pMemory, size_t orig_size, size_t size,
                                 VkSystemAllocationScope alloc_scope);

#if defined(__linux__) || defined(__APPLE__) || defined(__Fuchsia__) || defined(__QNXNTO__) || defined(__FreeBSD__)
#define loader_stack_alloc(size) alloca(size)
#elif defined(_WIN32)
#define loader_stack_alloc(size) _alloca(size)
#endif  // defined(_WIN32)