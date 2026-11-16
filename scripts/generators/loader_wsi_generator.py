#!/usr/bin/python3 -i
#
# Copyright (c) 2015-2022 The Khronos Group Inc.
# Copyright (c) 2015-2022 Valve Corporation
# Copyright (c) 2015-2022 LunarG, Inc.
# Copyright (c) 2015-2017 Google Inc.
# Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
# Copyright (c) 2023-2023 RasterGrid Kft.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Author: Mark Young <marky@lunarg.com>
# Author: Mark Lobodzinski <mark@lunarg.com>
# Author: Charles Giessen <charles@lunarg.com>

import os
from base_generator import BaseGenerator
from generators.common_code import get_surface_ext_names, DescribeBlock, PlatformGuardHelper

class LoaderWsiGenerator(BaseGenerator):
    def __init__(self):
        BaseGenerator.__init__(self)

        self.surface_extensions = []


    def generate(self):

        # extensions which we cannot reasonably autogenerate code for
        skip_wsi_extensions = ['VK_KHR_surface',
                               'VK_KHR_get_surface_capabilities2',
                               'VK_KHR_surface_maintenance1',
                               'VK_EXT_surface_maintenance1',
                               'VK_EXT_swapchain_colorspace',
                               'VK_GOOGLE_surfaceless_query']

        self.surface_extensions = get_surface_ext_names(self.vk)
        for wsi_ext_to_skip in skip_wsi_extensions:
            self.surface_extensions.remove(wsi_ext_to_skip)

        out = []
        self.add_preamble(out)

        if self.filename == 'vk_loader_wsi.c':
            self.print_vk_loader_wsi_c(out)
        elif self.filename == 'vk_loader_wsi.h':
            self.print_vk_loader_wsi_h(out)
        self.write(''.join(out))

    def add_preamble(self, out):
        out.append(f'''// *** THIS FILE IS GENERATED - DO NOT EDIT ***
// See {os.path.basename(__file__)} for modifications

/*
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
 * Author: Ian Elliott <ian@lunarg.com>
 * Author: Jon Ashburn <jon@lunarg.com>
 * Author: Ian Elliott <ianelliott@google.com>
 * Author: Mark Lobodzinski <mark@lunarg.com>
 * Author: Lenny Komow <lenny@lunarg.com>
 * Author: Charles Giessen <charles@lunarg.com>
 */

''')

    def get_wsi_enum_name(self, ext):
        ext_enum = ext.lower().split('_')[2].upper()
        ext_enum_fixup_dict = {'STREAM':'GGP', 'IMAGEPIPE': 'FUCHSIA'}
        if ext_enum in ext_enum_fixup_dict:
            ext_enum = ext_enum_fixup_dict[ext_enum]
        return 'VK_ICD_WSI_PLATFORM_' + ext_enum

    def print_vk_loader_wsi_c(self, out):
        out.append('''
            #include "vk_loader_wsi.h"

            #include <vulkan/vk_icd.h>

            #include "loader.h"
            #include "log.h"
            #include "vk_loader_platform.h"
            #include "wsi.h"

            // This function unwraps the application-provided surface handle into the ICD-specific surface handle
            // corresponding to the specified physical device. If the ICD-specific surface handle does not exist
            // yet, then this function also creates of the ICD-specific surface object. This enables lazy creation
            // of ICD-specific surface objects and therefore avoids creating surfaces for ICDs that may not be able
            // to or should not create the specific type of surface (one example is the VK_KHR_display extension
            // where the VkDisplayModeKHR handles the surfaces are created from are physical-device-specific
            // non-dispatchable handles and therefore they should not be passed down to a foreign ICD).
            VkResult wsi_unwrap_icd_surface(struct loader_icd_term* icd_term, VkSurfaceKHR* surface) {
            // NOLINTNEXTLINE(performance-no-int-to-ptr) - VkSurfaceKHR handle decode requires round-tripping through uintptr_t
            VkIcdSurface* icd_surface = (VkIcdSurface*)(uintptr_t)(*surface);

            // The surface creation terminators grow each driver's surface_list under loader_lock, which can reallocate the
            // array this function reads, and terminator_DestroySurfaceKHR clears slots in it under the same lock. The surface
            // query commands that land here don't require external synchronization of the surface, so take the lock before
            // touching the list. loader_lock is recursive, so callers already holding it are unaffected.
            loader_platform_thread_lock_mutex(&loader_lock);

            if (NULL == icd_term->surface_list.list ||
                icd_term->surface_list.capacity <= icd_surface->surface_index * sizeof(VkSurfaceKHR)) {
                // This surface handle is not one that was created by the loader, therefore we simply return
                // the input surface handle unmodified. This matches legacy behavior.
                loader_platform_thread_unlock_mutex(&loader_lock);
                return VK_SUCCESS;
            }

            VkResult result = VK_SUCCESS;
            if (icd_term->scanned_icd->interface_version >= ICD_VER_SUPPORTS_ICD_SURFACE_KHR) {
                VkAllocationCallbacks* pAllocator = icd_surface->callbacks_valid ? &icd_surface->callbacks : NULL;
                if (VK_NULL_HANDLE == icd_term->surface_list.list[icd_surface->surface_index]) {
                    // If the surface does not exist yet for the target ICD, then create it lazily
                    switch (icd_surface->base.platform) {
            ''')

        current_block = ''

        for ext in self.surface_extensions:
            for command in self.vk.extensions[ext].commands:
                if command.params[0].type != 'VkInstance':
                    continue

                surface_create_info_struct = self.vk.structs[command.params[1].type]

                ext_enum = self.get_wsi_enum_name(ext)

                ext_enabled_bool_name = ext[3:].lower()

                if command.protect is not None:
                    out.append(f'#if defined({command.protect})\n')
                out.append(f'''case {ext_enum}:
                    if (NULL != icd_term->dispatch.{command.name[2:]} &&
                        icd_term->enabled_instance_extensions.{ext_enabled_bool_name}) {{
                        result = icd_term->dispatch.{command.name[2:]}(
                            icd_term->instance, (const {surface_create_info_struct.name}*)icd_surface->create_info, pAllocator,
                            &icd_term->surface_list.list[icd_surface->surface_index]);
                    }} else {{
                        result = VK_ERROR_EXTENSION_NOT_PRESENT;
                    }}
                    break;
                    ''')
                if command.protect is not None:
                    out.append(f'#endif  // {command.protect}\n')
                out.append('\n')

        out.append('''default:
                        // Not supported
                        result = VK_ERROR_EXTENSION_NOT_PRESENT;
                        break;
                        }
                    }

                    *surface = icd_term->surface_list.list[icd_surface->surface_index];
                }

                loader_platform_thread_unlock_mutex(&loader_lock);
                return result;
            }
            ''')

        current_block = ''
        protect = PlatformGuardHelper()

        for ext in self.surface_extensions:
            ext_enum = self.get_wsi_enum_name(ext)

            ext_union_name = ext.lower().split('_')[2]
            ext_union_name_fixup_dict = {'win32':'win', 'stream': 'ggp'}
            if ext_union_name in ext_union_name_fixup_dict:
                ext_union_name = ext_union_name_fixup_dict[ext_union_name]
            # they all end with _surf, so add it after we fixed up the name
            ext_union_name = ext_union_name + '_surf'

            ext_enabled_bool_name = ext[3:].lower()

            for command in self.vk.extensions[ext].commands:
                protect.add_guard(out, command.protect)
                current_block = DescribeBlock(self.targetApiName, command, current_block, out)

                param_decl = []
                for param in command.params:
                    param_decl.append(param.cDeclaration)

                params = []
                for param in command.params:
                    params.append(param.name)

                if command.params[0].type == 'VkInstance':

                    surface_create_info_struct = self.vk.structs[command.params[1].type]
                    surface_create_info_structs = [surface_create_info_struct]
                    for struct in surface_create_info_struct.extendedBy:
                        surface_create_info_structs.append(self.vk.structs[struct])

                    # Trampoline
                    out.append(f'''LOADER_EXPORT VKAPI_ATTR {command.returnType} VKAPI_CALL {command.name}({", ".join(param_decl)}) {{
                        struct loader_instance* loader_inst = loader_validate_and_get_instance_dispatch(instance, "{command.name}");
                        return loader_inst->disp->layer_inst_disp.{command.name[2:]}(loader_inst->instance, {", ".join(params[1:])});
                    }}

                    VKAPI_ATTR {command.returnType} VKAPI_CALL terminator_{command.name[2:]}({", ".join(param_decl)}) {{
                        VkResult result = VK_SUCCESS;
                        VkIcdSurface* icd_surface = NULL;
                        loader_platform_thread_lock_mutex(&loader_lock);

                        // Initialize pSurface to NULL just to be safe.
                        *pSurface = VK_NULL_HANDLE;

                        // First, check to ensure the appropriate extension was enabled:
                        struct loader_instance* loader_inst = loader_get_instance(instance);
                        if (!loader_inst->enabled_extensions.{ext_enabled_bool_name}) {{
                            loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                                    "{ext} extension not enabled. {command.name} not executed!");
                            result = VK_ERROR_EXTENSION_NOT_PRESENT;
                            goto out;
                        }}

                        // Next, if so, proceed with the implementation of this function:
                        result = allocate_icd_surface_struct(loader_inst, sizeof(icd_surface->{ext_union_name}.base), sizeof(icd_surface->{ext_union_name}),
                                                            pAllocator, &icd_surface);
                        if (VK_SUCCESS != result) {{
                            goto out;
                        }}

                        icd_surface->{ext_union_name}.base.platform = {ext_enum};''')

                    # Skip copying the sType, pNext, and flags
                    # Fuchsia doesn't have any members to copy
                    if command.name != 'vkCreateImagePipeSurfaceFUCHSIA':
                        for param in surface_create_info_struct.members[3:]:
                            out.append(f'icd_surface->{ext_union_name}.{param.name} = pCreateInfo->{param.name};\n')
                    out.append('\n')
                    out.append('const struct loader_struct_type_info ci_types[] = {\n')
                    for struct in surface_create_info_structs:
                        out.append(f'{{{struct.sType}, sizeof({struct.name})}},\n')
                    out.append(f'''}};
                            result = copy_surface_create_info(loader_inst, icd_surface, pCreateInfo, sizeof(ci_types) / sizeof(ci_types[0]), ci_types,
                                                            "{surface_create_info_struct.name}", pAllocator);
                            if (VK_SUCCESS != result) {{
                                goto out;
                            }}

                            *pSurface = wrap_surface_handle(icd_surface);

                        out:
                            cleanup_surface_creation(loader_inst, result, icd_surface, pAllocator);
                            loader_platform_thread_unlock_mutex(&loader_lock);

                            return result;
                        }}
                        ''')
                    out.append('\n')
                elif command.params[0].type == 'VkPhysicalDevice':

                    ext_not_enabled_return = 'VK_SUCCESS'
                    if command.returnType == 'VkResult' and ext == 'VK_KHR_display':
                        ext_not_enabled_return = 'VK_ERROR_EXTENSION_NOT_PRESENT'
                    elif command.returnType == 'VkBool32':
                        ext_not_enabled_return = 'VK_FALSE'

                    icd_unsupported_ext_return = 'VK_SUCCESS'
                    if command.returnType == 'VkBool32':
                        icd_unsupported_ext_return = 'VK_FALSE'
                    elif command.name == 'vkCreateDisplayModeKHR':
                        icd_unsupported_ext_return = 'VK_ERROR_INITIALIZATION_FAILED' # Can't emulate this function, so return an appropriate error

                    icd_unsupported_log_level = "VULKAN_LOADER_ERROR_BIT"
                    if icd_unsupported_ext_return == "VK_SUCCESS":
                        icd_unsupported_log_level = "VULKAN_LOADER_WARN_BIT"

                    out.append(f'''
                    LOADER_EXPORT VKAPI_ATTR {command.returnType} VKAPI_CALL {command.name}({", ".join(param_decl)}) {{
                        VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, "{command.name}");
                        const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);
                    ''')
                    if command.returnType != 'void':
                        out.append('return ')
                    out.append(f'''
                        disp->{command.name[2:]}(unwrapped_phys_dev, {", ".join(params[1:])});
                    }}

                    VKAPI_ATTR {command.returnType} VKAPI_CALL terminator_{command.name[2:]}({", ".join(param_decl)}) {{
                        // First, check to ensure the appropriate extension was enabled:
                        struct loader_physical_device_term* phys_dev_term = (struct loader_physical_device_term*)physicalDevice;
                        struct loader_icd_term* icd_term = phys_dev_term->this_icd_term;
                        struct loader_instance* loader_inst = (struct loader_instance*)icd_term->this_instance;
                        if (!loader_inst->enabled_extensions.{ext_enabled_bool_name}) {{
                            loader_log(loader_inst, VULKAN_LOADER_ERROR_BIT, 0,
                                    "{ext} extension not enabled. {command.name} not executed!");
                            return {ext_not_enabled_return};
                        }}

                        if (NULL == icd_term->dispatch.{command.name[2:]}) {{
                            loader_log(loader_inst, {icd_unsupported_log_level}, 0,
                                    "ICD for selected physical device does not export {command.name}!");
                        ''')
                    # VK_KHR_display functions have params which need zeroing out in the error path
                    if command.name in ['vkGetPhysicalDeviceDisplayPlanePropertiesKHR', 'vkGetPhysicalDeviceDisplayPropertiesKHR', 'vkGetDisplayModePropertiesKHR']:
                        out.append('''// return 0 for property count as this driver doesn't support WSI functionality
                            if (pPropertyCount) {
                                *pPropertyCount = 0;
                            }''')
                    elif command.name == 'vkGetDisplayPlaneSupportedDisplaysKHR':
                        out.append('''// return 0 for property count as this driver doesn't support WSI functionality
                            if (pDisplayCount) {
                                *pDisplayCount = 0;
                            }''')
                    elif command.name == 'vkGetDisplayPlaneCapabilitiesKHR':
                        out.append('''if (pCapabilities) {
                                memset(pCapabilities, 0, sizeof(VkDisplayPlaneCapabilitiesKHR));
                            }''')
                    out.append(f'''return {icd_unsupported_ext_return};
                        }}

                        return icd_term->dispatch.{command.name[2:]}(phys_dev_term->phys_dev, {", ".join(params[1:])});
                    }}

                    ''')
                else:
                    out.append(f'''LOADER_EXPORT VKAPI_ATTR {command.returnType} VKAPI_CALL {command.name}({", ".join(param_decl)}) {{
                        #error("Not implemented. Codegen needs to be updated to handle this dispatchable handle function type!");\n')
                    }}''')
        protect.add_guard(out, None)
        current_block = ''

        out.append('\nbool generated_wsi_swapchain_instance_gpa(struct loader_instance* loader_inst, const char* name, void** addr) {\n')
        out.append('    *addr = NULL;\n')
        for ext in self.surface_extensions:
            for command in self.vk.extensions[ext].commands:
                protect.add_guard(out, command.protect)
                current_block = DescribeBlock(self.targetApiName, command, current_block, out)
                out.append(f'''if (!strcmp("{command.name}", name)) {{
                    *addr = loader_inst->enabled_extensions.{ext[3:].lower()} ? (void*){command.name} : NULL;
                    return true;
                }}''')
            out.append('\n')

        protect.add_guard(out, None)
        out.append('    return false;\n')
        out.append('}\n')


    def print_vk_loader_wsi_h(self, out):
        out.append('#pragma once\n\n')

        out.append('#include <vulkan/vulkan.h>\n\n')
        out.append('#include "vk_loader_platform.h"\n\n')

        current_block = ''
        protect = PlatformGuardHelper()

        for ext in self.surface_extensions:
            for command in self.vk.extensions[ext].commands:
                protect.add_guard(out, command.protect)
                current_block = DescribeBlock(self.targetApiName, command, current_block, out)

                param_decl = []
                for param in command.params:
                    param_decl.append(param.cDeclaration)
                out.append(f'LOADER_EXPORT VKAPI_ATTR {command.returnType} VKAPI_CALL {command.name}({", ".join(param_decl)});\n\n')
                out.append(f'VKAPI_ATTR {command.returnType} VKAPI_CALL terminator_{command.name[2:]}({", ".join(param_decl)});\n\n')

        protect.add_guard(out, None)

        out.append('\nbool generated_wsi_swapchain_instance_gpa(struct loader_instance* loader_inst, const char* name, void** addr);\n')


