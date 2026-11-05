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
from generators.common_code import ADD_INST_CMDS, APISpecific

MANUALLY_GENERATED_ENTRYPOINTS = [
    'vkGetDeviceProcAddr',
    'vkEnumerateDeviceExtensionProperties',
    'vkEnumerateDeviceLayerProperties',
    'vkCreateDevice',
    'vkDestroyDevice',
    'vkAllocateCommandBuffers',
    'vkGetDeviceQueue',
    'vkEnumeratePhysicalDeviceGroups',
    'vkGetDeviceQueue2',
]

PROMOTED_GET_PHYS_DEVS_2_EXTS = [
    'VK_KHR_get_physical_device_properties2',
    'VK_KHR_external_memory_capabilities',
    'VK_KHR_external_fence_capabilities',
    'VK_KHR_external_semaphore_capabilities',
]

class LoaderCoreGenerator(BaseGenerator):
    def __init__(self):
        BaseGenerator.__init__(self)

        self.core_commands = []
        self.extension_commands = []
        self.instance_extensions = []

    def generate(self):

        self.core_commands = [x for x in self.vk.commands.values() if len(x.extensions) == 0]
        self.extension_commands = [x for x in self.vk.commands.values() if len(x.extensions) > 0]

        self.instance_extensions = [x for x in self.vk.extensions.values() if x.instance]

        out = []
        self.add_preamble(out)

        if self.filename == 'vk_loader_core_trampolines.c':
            self.print_vk_loader_core_trampoline_c(out)

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
 * Author: Charles Giessen <charles@lunarg.com>
 */

''')

    def DescribeBlock(self, command, current_block, out, custom_commands_string = ' commands', indent = '    '):
        effective_version_name = APISpecific.getEffectiveVersionName(self.targetApiName, command.version)
        if command.extensions != current_block and effective_version_name != current_block:
            if effective_version_name and len(command.extensions) == 0:
                # Entry points that have an effective version in the target API
                api_version_display_name = APISpecific.getApiVersionDisplayName(self.targetApiName, effective_version_name)
                out.append(f"\n{indent}// ---- Core {api_version_display_name}{custom_commands_string}\n")
                return effective_version_name
            else:
                # don't repeat unless the first extension is different (while rest can vary)
                if not isinstance(current_block, list) or current_block[0] != command.extensions[0]:
                    out.append(f"\n{indent}// ---- {command.extensions[0] if len(command.extensions) > 0 else ''} extension{custom_commands_string}\n")
                return command.extensions
        else:
            return current_block

    def print_vk_loader_core_trampoline_c(self, out):

        out.append('\n')
        out.append('#include <vulkan/vulkan.h>\n')
        out.append('#include "vk_loader_platform.h"\n')
        out.append('#include "loader.h"\n')
        out.append('\n\n')

        current_block = ''
        for command in [x for x in self.vk.commands.values() if not x.extensions]:

            # Can't generate these functions - hand written implementations are used instead
            if command.params[0].type == 'VkInstance' or command.name in ADD_INST_CMDS or command.name in MANUALLY_GENERATED_ENTRYPOINTS:
                continue

            current_block = self.DescribeBlock(command, current_block, out)

            param_decl = []
            for param in command.params:
                param_decl.append(param.cDeclaration)
            out.append(f'LOADER_EXPORT VKAPI_ATTR {command.returnType} VKAPI_CALL {command.name}({", ".join(param_decl)}) {{\n')

            if command.params[0].type == 'VkPhysicalDevice':
                out.append('VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(physicalDevice, __FUNCTION__);\n')
                out.append('const VkLayerInstanceDispatchTable* disp = loader_get_instance_layer_dispatch(physicalDevice);\n')
            elif command.params[0].type == 'VkDevice':
                out.append('const VkLayerDispatchTable* disp = loader_validate_and_get_device_dispatch(device, __FUNCTION__);\n')
            elif command.params[0].type == 'VkCommandBuffer':
                out.append('const VkLayerDispatchTable* disp = loader_validate_and_get_command_buffer_dispatch(commandBuffer, __FUNCTION__);\n')
            elif command.params[0].type == 'VkQueue':
                out.append('const VkLayerDispatchTable* disp = loader_validate_and_get_queue_dispatch(queue, __FUNCTION__);\n')
            else:
                if command.device:
                    out.append(f'const VkLayerDispatchTable* disp = loader_get_dispatch({command.params[0].name});\n')
                else:
                    out.append(f'#error /*need special handling*\n')

            params = []
            # Need to replace the first parameter of VkPhysicalDevice with our unwrapped handle
            for param in command.params:
                if command.params[0].type == 'VkPhysicalDevice' and param.name == command.params[0].name:
                    params.append('unwrapped_phys_dev')
                else:
                    params.append(param.name)


            ext_command = self.vk.commands.get(command.name + 'KHR') if command.params[0].type == 'VkPhysicalDevice' else None
            is_promoted_ext = ext_command is not None and ext_command.extensions[0] in PROMOTED_GET_PHYS_DEVS_2_EXTS

            # A few physical device commands from promoted extensions call the ext version if the extension is enabled for compatibility
            if is_promoted_ext:
                ext = '_'.join(ext_command.extensions[0].lower().split('_')[1:])

                out.append('const struct loader_instance* inst = ((struct loader_physical_device_tramp*)physicalDevice)->this_instance;\n')
                out.append(f'if (inst != NULL && inst->enabled_extensions.{ext}) {{\n')
                if command.returnType != 'void':
                    out.append('return ')
                out.append(f'disp->{command.name[2:]}KHR({", ".join(params)});\n')
                # only declare a block if there is no return type
                out.append(f'}} {" else {" if command.returnType == 'void' else ""}\n')

            if command.returnType != 'void':
                out.append('return ')
            out.append(f'disp->{command.name[2:]}({", ".join(params)});\n')
            if is_promoted_ext and command.returnType == 'void':
                out.append('}\n')

            out.append('}\n')
