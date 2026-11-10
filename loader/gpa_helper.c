/*
 *
 * Copyright (c) 2015-2024 The Khronos Group Inc.
 * Copyright (c) 2015-2024 Valve Corporation
 * Copyright (c) 2015-2024 LunarG, Inc.
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
 * Author: Charles Giessen <charles@lunarg.com>
 */

#include "gpa_helper.h"

#include <string.h>

#include "debug_utils.h"
#include "unknown_function_handling.h"
#include "wsi.h"

void* globalGetProcAddr(const char* name) {
    if (!name || name[0] != 'v' || name[1] != 'k') return NULL;

    name += 2;
    if (!strcmp(name, "CreateInstance")) return vkCreateInstance;
    if (!strcmp(name, "EnumerateInstanceExtensionProperties")) return vkEnumerateInstanceExtensionProperties;
    if (!strcmp(name, "EnumerateInstanceLayerProperties")) return vkEnumerateInstanceLayerProperties;
    if (!strcmp(name, "EnumerateInstanceVersion")) return vkEnumerateInstanceVersion;

    return NULL;
}
