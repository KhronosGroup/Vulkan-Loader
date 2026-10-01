#!/usr/bin/python3 -i
#
# Copyright (c) 2026 The Khronos Group Inc.
# Copyright (c) 2026 Valve Corporation
# Copyright (c) 2026 LunarG, Inc.
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
# Author: Charles Giessen <charles@lunarg.com>

import os
from base_generator import BaseGenerator
from generators.common_code import DISPATCHABLE_TYPES, AVOID_EXT_NAMES, AVOID_CMD_NAMES, WSI_EXT_NAMES

class GetProcAddrValidatorGenerator(BaseGenerator):
    def __init__(self):
        BaseGenerator.__init__(self)

    def generate(self):
        out = []

        out.append(f'''#pragma once
// *** THIS FILE IS GENERATED - DO NOT EDIT ***
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

 */

#include <array>
''')
        self.OutputExportedFunctionValidator(out)
        out.append('\n')

        self.write(''.join(out))

    # Create a dispatch table from the corresponding table_type and append it to out
    def OutputExportedFunctionValidator(self, out: list):
        out.append('''// Every name gpa_helper.c's trampoline_get_proc_addr() hand-checks via
// `case 0x...u: if (!strcmp(...)) return ...; break;` (all core Vulkan 1.0-1.4 commands - see
// loader/gpa_helper.c). This list mirrors that function's checks
// exactly so a mistake made while mechanically threading the hash pre-filter through ~230 hand-written
// strcmp branches (wrong hash constant, dropped branch, mismatched name) shows up as a concrete failure
// here instead of silently returning NULL/wrong-pointer for some entry point.\n''')
        out.append('static constexpr std::array kGpaHelperCoreInstanceNames = {\n')
        for command_name, command in self.vk.commands.items():
            if len(command.extensions) == 0 and command.params[0].type in DISPATCHABLE_TYPES:
                out.append(f'"{command_name}",\n')
        out.append('};\n')
        out.append('\n')

        out.append('''// Every name loader_lookup_device_dispatch_table() (loader/generated/vk_loader_extensions.c) hand-checks
// for the core Vulkan 1.0-1.4 sections. Unlike the instance trampolines above, these entries only resolve
// to a non-null pointer if the underlying (mock) driver actually reports the function, so the test
// physical device must be told about every one of them via add_device_function() first.\n''')
        out.append('static constexpr std::array kDeviceDispatchCoreNames = {\n')
        for command_name, command in self.vk.commands.items():
            if len(command.extensions) == 0 and command.device:
                out.append(f'"{command_name}",\n')
        out.append('};\n')
        out.append('\n')

        out.append(f'''// The ~553 extension command names extension_instance_gpa() (loader/generated/vk_loader_extensions.c)
// hand-checks, minus ~72 that can't be exercised this way without extra per-name setup: ~48 compiled only
// under a platform-specific #if this Linux test build doesn't define (VK_USE_PLATFORM_WIN32_KHR,
// _ANDROID_KHR, _METAL_EXT, _FUCHSIA, etc.), and ~24 whose case only returns non-null when the backing
// extension is enabled at instance creation, so a bare resolve can't tell "recognized but disabled" apart
// from "a hash-mapping bug made this fall through to not-found". The remaining 481 names resolve
// unconditionally regardless of instance/ICD state, so they can be exhaustively checked like the arrays above.
//
// Explicit template arguments (not CTAD) are load-bearing: Clang caps the recursive fold expression in
// libstdc++'s std::array deduction guide at 256 instantiations, below this array's 481 entries. Naming the
// type and size directly skips that deduction guide.\n''')

        extension_instance_gpa_funcs = []
        for command in [x for x in self.vk.commands.values() if x.extensions]:
            if (command.protect or command.version or
                command.extensions[0] in WSI_EXT_NAMES or
                command.extensions[0] in AVOID_EXT_NAMES or
                command.name in AVOID_CMD_NAMES or
                self.vk.extensions[command.extensions[0]].instance):
                continue

            extension_instance_gpa_funcs.append(command)

        out.append(f'static constexpr std::array<const char*, {len(extension_instance_gpa_funcs)}> kExtensionInstanceGpaNames = {{\n')
        for func in extension_instance_gpa_funcs:
            out.append(f'"{func.name}",\n')
        out.append('};\n')
        out.append('\n')

        exported_functions_all_platforms = []
        out.append('''// List of all functions that should be exported by the loader binary\n''')
        for command in self.vk.commands.values():
            if command.protect:
                continue
            if ((command.version is None and len(command.extensions) == 0) or
                command.version or (len(command.extensions) > 0 and command.extensions[0] in WSI_EXT_NAMES)):
                exported_functions_all_platforms.append(command.name)
        out.append(f'static constexpr std::array<const char*, {len(exported_functions_all_platforms)}> kExportedFunctionNames = {{\n')
        for command_name in exported_functions_all_platforms:
            out.append(f'"{command_name}",\n')
        out.append('};\n')
        out.append('\n')

        out.append('''// List of functions that should be exported by the loader binary that are platform specific\n''')
        out.append(f'static constexpr std::array kPlatformExportedFunctionNames = {{\n')
        for command in self.vk.commands.values():
            if not command.protect:
                continue
            if ((command.version is None and len(command.extensions) == 0) or
                command.version or (len(command.extensions) > 0 and command.extensions[0] in WSI_EXT_NAMES)):
                if command.protect:
                    out.append(f'#if defined({command.protect})\n')
                out.append(f'"{command.name}",\n')
                if command.protect:
                    out.append(f'#endif // defined({command.protect})\n')
        out.append('};\n')
        out.append('\n')
