/*
 * Copyright (c) 2019-2021 The Khronos Group Inc.
 * Copyright (c) 2019-2021 Valve Corporation
 * Copyright (c) 2019-2021 LunarG, Inc.
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

#include "allocation.h"

#include <stdlib.h>

void *loader_instance_heap_alloc(const struct loader_instance *instance, size_t size, VkSystemAllocationScope alloc_scope) {
    void *pMemory = NULL;
#if (DEBUG_DISABLE_APP_ALLOCATORS == 1)
    {
#else
    if (instance && instance->alloc_callbacks.pfnAllocation) {
        // These are internal structures, so it's best to align everything to
        // the largest unit size which is the size of a uint64_t.
        pMemory = instance->alloc_callbacks.pfnAllocation(instance->alloc_callbacks.pUserData, size, sizeof(uint64_t), alloc_scope);
    } else {
#endif
        pMemory = malloc(size);
    }

    return pMemory;
}

void loader_instance_heap_free(const struct loader_instance *instance, void *pMemory) {
    if (pMemory != NULL) {
#if (DEBUG_DISABLE_APP_ALLOCATORS == 1)
        {
#else
        if (instance && instance->alloc_callbacks.pfnFree) {
            instance->alloc_callbacks.pfnFree(instance->alloc_callbacks.pUserData, pMemory);
        } else {
#endif
            free(pMemory);
        }
    }
}

void *loader_instance_heap_realloc(const struct loader_instance *instance, void *pMemory, size_t orig_size, size_t size,
                                   VkSystemAllocationScope alloc_scope) {
    void *pNewMem = NULL;
    if (pMemory == NULL || orig_size == 0) {
        pNewMem = loader_instance_heap_alloc(instance, size, alloc_scope);
    } else if (size == 0) {
        loader_instance_heap_free(instance, pMemory);
#if (DEBUG_DISABLE_APP_ALLOCATORS == 1)
#else
    } else if (instance && instance->alloc_callbacks.pfnReallocation) {
        // These are internal structures, so it's best to align everything to
        // the largest unit size which is the size of a uint64_t.
        pNewMem = instance->alloc_callbacks.pfnReallocation(instance->alloc_callbacks.pUserData, pMemory, size, sizeof(uint64_t),
                                                            alloc_scope);
#endif
    } else {
        pNewMem = realloc(pMemory, size);
    }
    return pNewMem;
}

void *loader_device_heap_alloc(const struct loader_device *device, size_t size, VkSystemAllocationScope alloc_scope) {
    void *pMemory = NULL;
#if (DEBUG_DISABLE_APP_ALLOCATORS == 1)
    {
#else
    if (device && device->alloc_callbacks.pfnAllocation) {
        // These are internal structures, so it's best to align everything to
        // the largest unit size which is the size of a uint64_t.
        pMemory = device->alloc_callbacks.pfnAllocation(device->alloc_callbacks.pUserData, size, sizeof(uint64_t), alloc_scope);
    } else {
#endif
        pMemory = malloc(size);
    }
    return pMemory;
}

void loader_device_heap_free(const struct loader_device *device, void *pMemory) {
    if (pMemory != NULL) {
#if (DEBUG_DISABLE_APP_ALLOCATORS == 1)
        {
#else
        if (device && device->alloc_callbacks.pfnFree) {
            device->alloc_callbacks.pfnFree(device->alloc_callbacks.pUserData, pMemory);
        } else {
#endif
            free(pMemory);
        }
    }
}

void *loader_device_heap_realloc(const struct loader_device *device, void *pMemory, size_t orig_size, size_t size,
                                 VkSystemAllocationScope alloc_scope) {
    void *pNewMem = NULL;
    if (pMemory == NULL || orig_size == 0) {
        pNewMem = loader_device_heap_alloc(device, size, alloc_scope);
    } else if (size == 0) {
        loader_device_heap_free(device, pMemory);
#if (DEBUG_DISABLE_APP_ALLOCATORS == 1)
#else
    } else if (device && device->alloc_callbacks.pfnReallocation) {
        // These are internal structures, so it's best to align everything to
        // the largest unit size which is the size of a uint64_t.
        pNewMem = device->alloc_callbacks.pfnReallocation(device->alloc_callbacks.pUserData, pMemory, size, sizeof(uint64_t),
                                                          alloc_scope);
#endif
    } else {
        pNewMem = realloc(pMemory, size);
    }
    return pNewMem;
}