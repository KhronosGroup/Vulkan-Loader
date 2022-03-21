#!/usr/bin/python3 -i
#
# Copyright (c) 2015-2022 The Khronos Group Inc.
# Copyright (c) 2015-2022 Valve Corporation
# Copyright (c) 2015-2022 LunarG, Inc.
# Copyright (c) 2015-2017 Google Inc.
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

import os,re,sys
import xml.etree.ElementTree as etree
from generator import *
from collections import namedtuple
from common_codegen import *

# WSI extensions that should be ignored
WSI_EXT_NAMES = [
    'VK_KHR_android_surface',
    'VK_KHR_display',
    'VK_KHR_display_swapchain',
    'VK_KHR_get_display_properties2',
    'VK_KHR_get_surface_capabilities2',
    'VK_KHR_surface',
    'VK_KHR_swapchain',
    'VK_KHR_wayland_surface',
    'VK_KHR_win32_surface',
    'VK_KHR_xcb_surface',
    'VK_KHR_xlib_surface',
    'VK_EXT_directfb_surface',
    'VK_EXT_headless_surface',
    'VK_EXT_metal_surface',
    'VK_FUCHSIA_imagepipe_surface',
    'VK_GGP_stream_descriptor_surface',
    'VK_MVK_macos_surface',
    'VK_MVK_ios_surface',
    'VK_NN_vi_surface',
    'VK_QNX_screen_surface',
]

# These are commands that should be ignored because we manually implement the
# trampolines and terminators
MANUALLY_IMPLEMENTED_COMMANDS = [
    # 1.0 commands
    'vkGetInstanceProcAddr',
    'vkGetDeviceProcAddr',
    'vkEnumerateInstanceExtensionProperties',
    'vkEnumerateInstanceLayerProperties',
    'vkEnumerateInstanceVersion',
    'vkCreateInstance',
    'vkDestroyInstance',
    'vkEnumerateDeviceLayerProperties',
    'vkCreateDevice',
    'vkDestroyDevice',
    'vkEnumeratePhysicalDevices',
    # 1.1 commands
    'vkEnumeratePhysicalDeviceGroups',
    # Extension commands
    # -- VK_EXT_debug_utils
    'vkCreateDebugUtilsMessengerEXT',
    'vkDestroyDebugUtilsMessengerEXT',
    'vkSubmitDebugUtilsMessageEXT',
    # -- VK_EXT_full_screen_exclusive
    'vkGetPhysicalDeviceSurfacePresentModes2EXT'
    'vkGetDeviceGroupSurfacePresentModes2EXT'
]

# Instance Extensions that need to be handled
INST_EXTENSION_OVERRIDES = [
    'VK_EXT_acquire_drm_display',
    'VK_EXT_acquire_xlib_display',
    'VK_EXT_debug_utils',
    'VK_EXT_direct_mode_display',
    'VK_EXT_display_surface_counter',
    'VK_EXT_full_screen_exclusive',
    'VK_NV_external_memory_capabilities',
]

MANUALLY_IMPLEMENTED_TERMINATORS = [
    # 1.0 commands
    'vkEnumerateInstanceExtensionProperties',
    'vkEnumerateInstanceLayerProperties',
    'vkEnumerateDeviceExtensionProperties',
    # 1.1 commands
    'vkEnumerateInstanceVersion',
    'vkGetPhysicalDeviceFeatures2',
    'vkGetPhysicalDeviceProperties2',
    'vkGetPhysicalDeviceExternalBufferProperties',
    'vkGetPhysicalDeviceExternalSemaphoreProperties',
    'vkGetPhysicalDeviceExternalFenceProperties',
    'vkGetPhysicalDeviceFormatProperties2',
    'vkGetPhysicalDeviceImageFormatProperties2',
    'vkGetPhysicalDeviceQueueFamilyProperties2',
    'vkGetPhysicalDeviceMemoryProperties2',
    'vkGetPhysicalDeviceSparseImageFormatProperties2',
    # 1.3 commands
    'vkGetPhysicalDeviceToolProperties',
]

INST_EXTENSION_HAVING_MANUAL_TERMINATORS = [
    # VK_EXT_acquire_xlib_display
    'vkAcquireXlibDisplayEXT',
    'vkGetRandROutputDisplayEXT',
    # VK_EXT_display_surface_counter
    'vkGetPhysicalDeviceSurfaceCapabilities2EXT',
    # VK_EXT_full_screen_exclusive
    'vkGetPhysicalDeviceSurfacePresentModes2EXT',
    'vkGetDeviceGroupSurfacePresentModes2EXT',
    # VK_NV_external_memory_capabilities
    'vkGetPhysicalDeviceExternalImageFormatPropertiesNV',
]

# Device commands that require a terminator
DEVICE_CMDS_NEEDING_TERMINATOR = [
    'vkGetDeviceProcAddr',
    # VK_KHR_swapchain
    'vkCreateSwapchainKHR',
    'vkCreateSharedSwapchainsKHR',
    'vkGetDeviceGroupSurfacePresentModesKHR',
    'vkGetDeviceGroupSurfacePresentModes2EXT',
    # VK_EXT_debug_marker
    'vkDebugMarkerSetObjectTagEXT',
    'vkDebugMarkerSetObjectNameEXT',
    # VK_EXT_debug_utils - Really instance extension, but they are device commands
    'vkSetDebugUtilsObjectNameEXT',
    'vkSetDebugUtilsObjectTagEXT',
    'vkQueueBeginDebugUtilsLabelEXT',
    'vkQueueEndDebugUtilsLabelEXT',
    'vkQueueInsertDebugUtilsLabelEXT',
    'vkCmdBeginDebugUtilsLabelEXT',
    'vkCmdEndDebugUtilsLabelEXT',
    'vkCmdInsertDebugUtilsLabelEXT',
]

# Additional device extensions also tracked by loader
ADD_DEVICE_EXTS_TRACKED_BY_LOADER = [
    "VK_KHR_device_group",
]

TRAMP_EXT_GIPA_AVOID_EXTS = [
    'VK_EXT_debug_report'
]

TRAMP_EXT_GIPA_AVOID_CMD_NAMES = [
    'vkCreateDebugUtilsMessengerEXT',
    'vkDestroyDebugUtilsMessengerEXT',
    'vkSubmitDebugUtilsMessageEXT'
]

NULL_CHECK_EXT_NAMES= ['VK_EXT_debug_utils']

#
# LoaderTrampTermGeneratorOptions - subclass of GeneratorOptions.
class LoaderTrampTermGeneratorOptions(GeneratorOptions):
    def __init__(self,
                 conventions = None,
                 filename = None,
                 directory = '.',
                 genpath = None,
                 apiname = None,
                 profile = None,
                 versions = '.*',
                 emitversions = '.*',
                 defaultExtensions = None,
                 addExtensions = None,
                 removeExtensions = None,
                 emitExtensions = None,
                 sortProcedure = regSortFeatures,
                 prefixText = "",
                 genFuncPointers = True,
                 protectFile = True,
                 protectFeature = True,
                 apicall = '',
                 apientry = '',
                 apientryp = '',
                 indentFuncProto = True,
                 indentFuncPointer = False,
                 alignFuncParam = 0,
                 expandEnumerants = True):
        GeneratorOptions.__init__(self,
                conventions = conventions,
                filename = filename,
                directory = directory,
                genpath = genpath,
                apiname = apiname,
                profile = profile,
                versions = versions,
                emitversions = emitversions,
                defaultExtensions = defaultExtensions,
                addExtensions = addExtensions,
                removeExtensions = removeExtensions,
                emitExtensions = emitExtensions,
                sortProcedure = sortProcedure)
        self.prefixText      = prefixText
        self.prefixText      = None
        self.apicall         = apicall
        self.apientry        = apientry
        self.apientryp       = apientryp
        self.alignFuncParam  = alignFuncParam
        self.expandEnumerants = expandEnumerants

#
# LoaderTrampTermOutputGenerator - subclass of OutputGenerator.
class LoaderTrampTermOutputGenerator(OutputGenerator):
    """Generate common trampoline and terminator functions based on incoming registry file data"""
    def __init__(self,
                 errFile = sys.stderr,
                 warnFile = sys.stderr,
                 diagFile = sys.stdout):
        OutputGenerator.__init__(self, errFile, warnFile, diagFile)

        self.basic_commands = []
        self.BasicCommandData = namedtuple('BasicCommandData', ['name', 'protect', 'return_type', 'handle', 'handle_type', 'params', 'cdecl', 'alias'])
        self.basic_extensions = []
        self.BasicExtensionCommandData = namedtuple('BasicExtensionCommandData', ['name', 'requires'])
        self.BasicExtensionData = namedtuple('BasicExtensionData', ['name', 'type', 'define', 'protect', 'required_exts', 'command_data'])
        self.required_ext_list = []
        self.dispatchable_handles = []
        self.core_command_groups = []
        self.CoreCommandGroup = namedtuple('CoreCommandGroup', ['major', 'minor', 'inst_cmds', 'dev_cmds', 'needs_dev_term_override', 'needs_tramp', 'needs_term'])
        self.ext_command_groups = []
        self.ExtCommandGroup = namedtuple('ExtCommandGroup', ['ext_name', 'ext_type', 'define', 'protect', 'required_exts', 'cmds', 'has_inst', 'needs_dev_term_override', 'needs_tramp', 'needs_term', 'requires_term_disp'])
        self.CommandParam = namedtuple('CommandParam', ['type', 'name', 'is_const', 'is_pointer', 'cdecl'])
        self.CommandData = namedtuple('CommandData', ['name', 'require', 'protect', 'return_type', 'handle', 'handle_type', 'params', 'cdecl', 'alias', 'alias_ext', 'needs_tramp', 'needs_term', 'requires_term_disp'])
        self.dev_extensions_tracked_by_loader = []

    #
    # Called once at the beginning of each run
    def beginFile(self, genOpts):
        OutputGenerator.beginFile(self, genOpts)

        # User-supplied prefix text, if any (list of strings)
        if (genOpts.prefixText):
            for s in genOpts.prefixText:
                write(s, file=self.outFile)

        # File Comment
        file_comment = '// *** THIS FILE IS GENERATED - DO NOT EDIT ***\n'
        file_comment += '// See loader_tramp_term_generator.py for modifications\n'
        write(file_comment, file=self.outFile)

        # Copyright Notice
        copyright =  '/*\n'
        copyright += ' * Copyright (c) 2015-2022 The Khronos Group Inc.\n'
        copyright += ' * Copyright (c) 2015-2022 Valve Corporation\n'
        copyright += ' * Copyright (c) 2015-2022 LunarG, Inc.\n'
        copyright += ' *\n'
        copyright += ' * Licensed under the Apache License, Version 2.0 (the "License");\n'
        copyright += ' * you may not use this file except in compliance with the License.\n'
        copyright += ' * You may obtain a copy of the License at\n'
        copyright += ' *\n'
        copyright += ' *     http://www.apache.org/licenses/LICENSE-2.0\n'
        copyright += ' *\n'
        copyright += ' * Unless required by applicable law or agreed to in writing, software\n'
        copyright += ' * distributed under the License is distributed on an "AS IS" BASIS,\n'
        copyright += ' * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n'
        copyright += ' * See the License for the specific language governing permissions and\n'
        copyright += ' * limitations under the License.\n'
        copyright += ' *\n'
        copyright += ' * Author: Mark Lobodzinski <mark@lunarg.com>\n'
        copyright += ' * Author: Mark Young <marky@lunarg.com>\n'
        copyright += ' */\n'

        preamble = '// clang-format off\n'

        if self.genOpts.filename[-2:] == '.h':
            preamble += '#pragma once\n'
        if self.genOpts.filename == 'vk_loader_extension_utils.c':
            preamble += '#include <stdlib.h>\n'
            preamble += '#include <string.h>\n'
            preamble += '\n'
            preamble += '#include "allocation.h"\n'
            preamble += '#include "debug_utils.h"\n'
            preamble += '#include "gpa_helper.h"\n'
            preamble += '#include "loader.h"\n'
            preamble += '#include "log.h"\n'
            preamble += '#include "vk_loader_platform.h"\n'
        elif self.genOpts.filename == 'vk_loader_trampolines.c':
            preamble += '#include <stdlib.h>\n'
            preamble += '#include <string.h>\n'
            preamble += '\n'
            preamble += '#include "allocation.h"\n'
            preamble += '#include "debug_utils.h"\n'
            preamble += '#include "gpa_helper.h"\n'
            preamble += '#include "loader.h"\n'
            preamble += '#include "log.h"\n'
            preamble += '#include "vk_loader_platform.h"\n'
            preamble += '#include "vk_loader_extension_utils.h"\n'
            preamble += '#include "wsi.h"\n'
            preamble += '#include <vulkan/vk_icd.h>\n'
        elif self.genOpts.filename == 'vk_loader_terminators.c':
            preamble += '#include "allocation.h"\n'
            preamble += '#include "loader.h"\n'
            preamble += '#include "log.h"\n'
            preamble += '#include "debug_utils.h"\n'
            preamble += '#include "extension_manual.h"\n'
            preamble += '#include "vk_loader_platform.h"\n'
            preamble += '#include "wsi.h"\n'
            preamble += '#include <vulkan/vk_icd.h>\n'

        write(copyright, file=self.outFile)
        write(preamble, file=self.outFile)

    #
    # Write generate and write dispatch tables to output file
    def endFile(self):
        for cur_ext in self.basic_extensions:
            if len(cur_ext.command_data) > 0:
                for command in cur_ext.command_data:
                    self.AddCommandToDispatchList(cur_ext, command)
            else:
                self.FindExtCommandGroup(cur_ext, False, False, False, False, False)

        file_data = ''

        # Now generate a list of device commands that we need to track
        for ext_group in self.ext_command_groups:
            if ext_group.needs_dev_term_override or ext_group.ext_name in ADD_DEVICE_EXTS_TRACKED_BY_LOADER:
                self.dev_extensions_tracked_by_loader.append(ext_group.ext_name)

        if self.genOpts.filename == 'vk_layer_dispatch_table.h':
            file_data += self.OutputLayerInstanceTermDispatchTable(True)
            file_data += self.OutputLayerDeviceTermDispatchTable()

        elif self.genOpts.filename == 'vk_loader_extension_utils.h':
            file_data += self.OutputInstaceExtensionPrototypes()
            file_data += self.OutputExtensionEnableStructs()

        elif self.genOpts.filename == 'vk_loader_extension_utils.c':
            file_data += self.OutputInstanceExtensionEnableCheck()
            file_data += self.OutputInstantExtensionWhitelistArray()
            file_data += self.OutputLoaderDispatchTables()

        elif self.genOpts.filename == 'vk_loader_trampolines.h':
            file_data += self.OutputLoaderTrampolinePrototypes()

        elif self.genOpts.filename == 'vk_loader_trampolines.c':
            file_data += self.OutputLoaderTrampolineDefinitions()
            file_data += self.OutputLoaderTrampolineUtils()

        elif self.genOpts.filename == 'vk_loader_terminators.h':
            file_data += self.OutputAdditionalTerminatorPrototypes()
            file_data += self.OutputLoaderTerminatorPrototypes()
            file_data += self.OutputLoaderTerminatorDispatchStruct()

        elif self.genOpts.filename == 'vk_loader_terminators.c':
            file_data += self.OutputLoaderTerminatorDefinitions()
            file_data += self.OutputLoaderTerminatorFunctions()
            file_data += self.OutputLayerInstanceTermDispatchTable(False)

        file_data += '// clang-format on\n'

        write(file_data, file=self.outFile);

        # Finish processing in superclass
        OutputGenerator.endFile(self)


    def beginFeature(self, interface, emit):
        # Start processing in superclass
        OutputGenerator.beginFeature(self, interface, emit)
        self.featureExtraProtect = GetFeatureProtect(interface)

        enums = interface[0].findall('enum')
        self.currentExtension = ''
        self.name_definition = ''

        for item in enums:
            name_definition = item.get('name')
            if 'EXTENSION_NAME' in name_definition:
                self.name_definition = name_definition

        self.type = interface.get('type')
        self.num_commands = 0
        name = interface.get('name')
        self.currentExtension = name

        extension_index = -1
        for i in range(0, len(self.basic_extensions)):
            if self.basic_extensions[i].name == name:
                extension_index = i
                break

        commands = []
        for require_element in interface.findall('require'):
            req_ext = require_element.get('extension')
            xml_commands = require_element.findall('command')
            if xml_commands:
                for xml_command in xml_commands:
                    command_name = xml_command.get('name')
                    skip = False
                    for ext in self.basic_extensions:
                        for cmd in ext.command_data:
                            if cmd.name == command_name:
                                skip = True
                                break
                    for cmd in commands:
                        if cmd.name == command_name:
                            skip = True
                            break
                    if not skip:
                        commands.append(
                            self.BasicExtensionCommandData(
                                name = command_name,
                                requires = req_ext))

        if extension_index >= 0:
            if len(commands) > 0:
                new_commands = self.basic_extensions[extension_index].command_data
                new_commands.append(commands)
                new_ext = self.basic_extensions[extension_index]._replace(command_data = new_commands)
                self.basic_extensions[extension_index] = new_ext
        else:
            requires = interface.get('requires')
            if requires is not None:
                self.required_ext_list = requires.split(',')
            else:
                self.required_ext_list = list()

            self.basic_extensions.append(
                self.BasicExtensionData(
                    name = name,
                    type = self.type,
                    define = self.name_definition,
                    protect = self.featureExtraProtect,
                    required_exts = self.required_ext_list,
                    command_data = commands
                )
            )


    #
    # Called for each type -- if the type is a struct/union, grab the metadata
    def genType(self, typeinfo, name, alias):
        OutputGenerator.genType(self, typeinfo, name, alias)
        typeElem = typeinfo.elem
        # If the type is a struct type, traverse the imbedded <member> tags generating a structure.
        # Otherwise, emit the tag text.
        category = typeElem.get('category')

        if (category == 'handle' and not (typeElem.find('type') is None) and
            not (typeElem.find('type').text is None) and typeElem.find('type').text == 'VK_DEFINE_HANDLE'):

            self.dispatchable_handles.append(name)

    # Process commands, adding to appropriate dispatch tables
    def genCmd(self, cmdinfo, name, alias):
        OutputGenerator.genCmd(self, cmdinfo, name, alias)

        # Get first param type
        params = cmdinfo.elem.findall('param')
        info = self.getTypeNameTuple(params[0])

        self.num_commands += 1

        if 'android' not in name.lower():
            # Record all command data to a temporary basic extension and command data structs
            # which will be used to properly parse the data after we've gone through the whole XML.

            handle = self.registry.tree.find("types/type/[name='" + info[0] + "'][@category='handle']")
            return_type =  cmdinfo.elem.find('proto/type')

            if (return_type is not None and return_type.text == 'void'):
                return_type = None

            cmd_params = []

            params = cmdinfo.elem.findall('param')
            lens = set()
            for param in params:
                len = self.getLen(param)
                if len:
                    lens.add(len)
            paramsInfo = []
            for param in params:
                paramInfo = self.getTypeNameTuple(param)
                param_type = paramInfo[0]
                param_name = paramInfo[1]
                param_cdecl = self.makeCParamDecl(param, 0)

                is_pointer = False
                for elem in param:
                    if elem.tag == 'type' and elem.tail is not None and '*' in elem.tail:
                        is_pointer = True

                cmd_params.append(self.CommandParam(type = param_type,
                                                    name = param_name,
                                                    cdecl = param_cdecl,
                                                    is_const = True if 'const' in param_cdecl else False,
                                                    is_pointer = is_pointer))

            self.basic_commands.append(
                self.BasicCommandData(
                    name = name,
                    protect = self.featureExtraProtect,
                    return_type = return_type,
                    handle_type = info[0],
                    handle = handle,
                    params = cmd_params,
                    cdecl = self.makeCDecls(cmdinfo.elem)[0],
                    alias = alias
                )
            )

    # Retrieve the value of the len tag
    def getLen(self, param):
        result = None
        len = param.attrib.get('len')
        if len and len != 'null-terminated':
            # For string arrays, 'len' can look like 'count,null-terminated',
            # indicating that we have a null terminated array of strings.  We
            # strip the null-terminated from the 'len' field and only return
            # the parameter specifying the string count
            if 'null-terminated' in len:
                result = len.split(',')[0]
            else:
                result = len
            result = str(result).replace('::', '->')
        return result

    def endFeature(self):
        # Finish processing in superclass
        OutputGenerator.endFeature(self)

    def RequiresTrampoline(self, extension_type, extension_name, cmd_name, alias):
        if (extension_name in WSI_EXT_NAMES or cmd_name in MANUALLY_IMPLEMENTED_COMMANDS):
            return False
        if alias is not None:
            print("Extension %s Command %s has alias %s" % (extension_name, cmd_name, alias))
            return False
        requires = 'VK_VERSION_' in extension_name
        if not requires and ('instance' != extension_type or extension_name in INST_EXTENSION_OVERRIDES):
            requires = True
        return requires

    def RequiresTerminator(self, extension_type, extension_name, cmd_name, handle_type, params, alias):
        if extension_name in WSI_EXT_NAMES or ('instance' == extension_type and extension_name not in INST_EXTENSION_OVERRIDES):
            return False
        if cmd_name in MANUALLY_IMPLEMENTED_COMMANDS or cmd_name in MANUALLY_IMPLEMENTED_TERMINATORS or cmd_name in INST_EXTENSION_HAVING_MANUAL_TERMINATORS:
            return False
        if (handle_type == 'VkInstance') or (handle_type == 'VkPhysicalDevice') or (cmd_name in DEVICE_CMDS_NEEDING_TERMINATOR):
            return True
        else:
            for param in params:
                if (param.type == 'VkInstance' or param.type == 'VkPhysicalDevice' or
                    param.type == 'VkSurfaceKHR' or param.type == 'VkPhysicalDeviceSurfaceInfo2KHR'):
                    return True
        return False

    def RequiresTerminatorDispatch(self, extension_type, extension_name, cmd_name, handle_type, params, alias):
        if self.RequiresTerminator(extension_type, extension_name, cmd_name, handle_type, params, alias):
            return True
        if (cmd_name in MANUALLY_IMPLEMENTED_COMMANDS or cmd_name in MANUALLY_IMPLEMENTED_TERMINATORS or extension_name in INST_EXTENSION_OVERRIDES or
            extension_name in WSI_EXT_NAMES or self.CommandIsInstanceType(handle_type, extension_name)):
            return True
        return False

    # Find the existing core  group for the version of Vulkan, or create it.
    def FindCoreCommandGroup(self, major_version, minor_version, needs_dev_term_override, needs_tramp, needs_term):
        cmd_group = None
        num_cur_cmd_groups = 0

        if self.core_command_groups != None:
            num_cur_cmd_groups = len(self.core_command_groups)
        for i in range(0, num_cur_cmd_groups):
            if (self.core_command_groups[i].major == major_version and
                self.core_command_groups[i].minor == minor_version):

                # If there and needs replacing, replace it and restart (we have to restart for python)
                if ((needs_dev_term_override and (self.core_command_groups[i].needs_dev_term_override != needs_dev_term_override)) or
                    (needs_tramp and (self.core_command_groups[i].needs_tramp != needs_tramp)) or
                    (needs_term and (self.core_command_groups[i].needs_term != needs_term))):
                    new_version = self.core_command_groups[i]._replace(
                                        needs_dev_term_override = (needs_dev_term_override or self.core_command_groups[i].needs_dev_term_override),
                                        needs_tramp = (needs_tramp or self.core_command_groups[i].needs_tramp),
                                        needs_term = (needs_term or self.core_command_groups[i].needs_term))
                    self.core_command_groups[i] = new_version

                cmd_group = self.core_command_groups[i]
                break

        # Not found
        if cmd_group is None:
            cmd_group = self.CoreCommandGroup(major = major_version,
                                                minor = minor_version,
                                                inst_cmds = [],
                                                dev_cmds = [],
                                                needs_dev_term_override = needs_dev_term_override,
                                                needs_tramp = needs_tramp,
                                                needs_term = needs_term)
            self.core_command_groups.append(cmd_group)

        return cmd_group

    # Find the existing extension group for the extesnsion, or create it.
    def FindExtCommandGroup(self, extension_data, has_inst, needs_dev_term_override, needs_tramp, needs_term, requires_term_disp):
        cmd_group = None
        num_cur_cmd_groups = 0
        if self.ext_command_groups != None:
            num_cur_cmd_groups = len(self.ext_command_groups)
        for i in range(0, num_cur_cmd_groups):
            if (self.ext_command_groups[i].ext_name == extension_data.name and
                self.ext_command_groups[i].ext_type == extension_data.type):

                # If there and needs replacing, replace it and restart (we have to restart for python)
                if ((has_inst and (self.ext_command_groups[i].has_inst != has_inst)) or
                    (needs_dev_term_override and (self.ext_command_groups[i].needs_dev_term_override != needs_dev_term_override)) or
                    (needs_tramp and (self.ext_command_groups[i].needs_tramp != needs_tramp)) or
                    (needs_term and (self.ext_command_groups[i].needs_term != needs_term)) or
                    (requires_term_disp and (self.ext_command_groups[i].requires_term_disp != requires_term_disp))):
                    new_version = self.ext_command_groups[i]._replace(
                                        has_inst = (has_inst or self.ext_command_groups[i].has_inst),
                                        needs_dev_term_override = (needs_dev_term_override or self.ext_command_groups[i].needs_dev_term_override),
                                        needs_tramp = (needs_tramp or self.ext_command_groups[i].needs_tramp),
                                        needs_term = (needs_term or self.ext_command_groups[i].needs_term),
                                        requires_term_disp = (requires_term_disp or self.ext_command_groups[i].requires_term_disp))
                    self.ext_command_groups[i] = new_version

                cmd_group = self.ext_command_groups[i]
                break

        # Not found
        if cmd_group is None:
            cmd_group = self.ExtCommandGroup(ext_name = extension_data.name,
                                             ext_type = extension_data.type,
                                             protect = extension_data.protect,
                                             define = extension_data.define,
                                             required_exts = extension_data.required_exts,
                                             cmds = [],
                                             has_inst = has_inst,
                                             needs_dev_term_override = needs_dev_term_override,
                                             needs_tramp = needs_tramp,
                                             needs_term = needs_term,
                                             requires_term_disp = requires_term_disp)
            self.ext_command_groups.append(cmd_group)

        return cmd_group

    def UpdateAliasCommandGroup(self, alias, extension_name, command_name):
        found = False
        alias_ext = ''

        # First look at other extensions.  We have to do this here because the extension that
        # aliases this one may not be defined in the other internal structs yet.
        for ext in self.basic_extensions:
            for cmd in ext.command_data:
                if cmd.name == alias:
                    alias_ext = ext.name
                    found = True
                    break
            if found:
                break

        # If it's a core extension, we want to update it so that we know it is also used in an
        # extension so our trampolines can fall back to the extension option
        if 'VK_VERSION_' in alias_ext:
            found = False
            for cur_group in self.core_command_groups:
                for i in range (0, len(cur_group.inst_cmds)):
                    if cur_group.inst_cmds[i].name == alias:
                        new_command = cur_group.inst_cmds[i]._replace(alias = command_name,
                                                                    alias_ext = extension_name)
                        cur_group.inst_cmds[i] = new_command
                        found = True
                        break

                if not found:
                    for i in range (0, len(cur_group.dev_cmds)):
                        if cur_group.dev_cmds[i].name == alias:
                            new_command = cur_group.dev_cmds[i]._replace(alias = command_name,
                                                                        alias_ext = extension_name)
                            cur_group.dev_cmds[i] = new_command
                            found = True
                            break
            if found:
                alias_ext = ''

        return alias_ext

    def CommandIsInstanceType(self, handle_type, extension_name):
        return (('VkInstance' == handle_type or 'VkPhysicalDevice' == handle_type or 'VkSurfaceKHR' == handle_type) and 'android' not in extension_name.lower())

    # Determine if this API should be ignored or added to the instance or device dispatch table
    def AddCommandToDispatchList(self, basic_extension_data, command_data):
        command = None
        for i in range(0, len(self.basic_commands)):
            if self.basic_commands[i].name == command_data.name:
                command = self.basic_commands[i]
                break

        if command is None:
            return

        command_requires = None
        require_node = self.registry.tree.find("./extensions/extension[@name='{}']/require/command[@name='{}']/..".format(basic_extension_data.name, command.name))
        if require_node is not None:
            if 'extension' in require_node.attrib:
                command_requires = require_node.attrib['extension']

        requires_dev_term_override = command.name in DEVICE_CMDS_NEEDING_TERMINATOR 
        requires_tramp = self.RequiresTrampoline(basic_extension_data.type, basic_extension_data.name, command.name, command.alias)
        requires_term = self.RequiresTerminator(basic_extension_data.type, basic_extension_data.name, command.name, command.handle_type, command.params, command.alias)
        requires_term_disp = self.RequiresTerminatorDispatch(basic_extension_data.type, basic_extension_data.name, command.name, command.handle_type, command.params, command.alias)

        # The Core Vulkan code will be wrapped in a feature called VK_VERSION_#_#
        # For example: VK_VERSION_1_0 wraps the core 1.0 Vulkan functionality
        if 'VK_VERSION_' in basic_extension_data.name:
            version_numbers = list(map(int, re.findall(r'\d+', basic_extension_data.name)))

            cur_cmd_group = self.FindCoreCommandGroup(version_numbers[0],
                                                      version_numbers[1],
                                                      requires_dev_term_override,
                                                      requires_tramp,
                                                      requires_term)

            cmd_data = self.CommandData(name = command.name,
                                        require = command_requires,
                                        protect = command.protect,
                                        return_type = command.return_type,
                                        handle = command.handle,
                                        handle_type = command.handle_type,
                                        params = command.params,
                                        cdecl = command.cdecl,
                                        alias = command.alias,
                                        alias_ext = '',
                                        needs_tramp = requires_tramp,
                                        needs_term = requires_term,
                                        requires_term_disp = requires_term_disp)

            if command.handle is not None and not (command.handle_type == 'VkInstance' or command.handle_type == 'VkPhysicalDevice'):
                cur_cmd_group.dev_cmds.append(cmd_data)
            else:
                cur_cmd_group.inst_cmds.append(cmd_data)
        else:
            has_inst = self.CommandIsInstanceType(command.handle_type, basic_extension_data.name)
            cur_cmd_group = self.FindExtCommandGroup(basic_extension_data,
                                                     has_inst,
                                                     requires_dev_term_override,
                                                     requires_tramp,
                                                     requires_term,
                                                     requires_term_disp)

            alias_ext = ''
            if command.alias is not None:
                alias_ext = self.UpdateAliasCommandGroup(command.alias, basic_extension_data.name, command.name)

            cur_cmd_group.cmds.append(
                self.CommandData(name=command.name,
                                 require=command_requires,
                                 protect=command.protect,
                                 return_type = command.return_type,
                                 handle = command.handle,
                                 handle_type = command.handle_type,
                                 params = command.params,
                                 cdecl = command.cdecl,
                                 alias = command.alias,
                                 alias_ext = alias_ext,
                                 needs_tramp = requires_tramp,
                                 needs_term = requires_term,
                                 requires_term_disp = requires_term_disp))

    #
    # Retrieve the type and name for a parameter
    def getTypeNameTuple(self, param):
        type = ''
        name = ''
        for elem in param:
            if elem.tag == 'type':
                type = noneStr(elem.text)
            elif elem.tag == 'name':
                name = noneStr(elem.text)
        return (type, name)

    # Output the trampoline defined prototypes
    def OutputLoaderTrampolinePrototypes(self):
        protos = ''
        protos += '// Extension interception for vkGetInstanceProcAddr function, so we can return\n'
        protos += '// the appropriate information for any instance extensions we know about.\n'
        protos += 'bool extension_instance_gpa(struct loader_instance *ptr_instance, const char *name, void **addr);\n\n'
        return protos


    # Create a comment header for the core function section to follow
    def OutputCoreSectionHeader(self, small_header, indent, major, minor, identifier):
        header_string = '---- Vulkan API version %s.%s %s ----' % (major, minor, identifier)
        header_dashes = ''
        for x in range(0, len(header_string)):
            header_dashes += '-'

        header = '\n'
        if not small_header:
            if indent:
                header += '    '
            header += '// %s\n' % header_dashes

        if indent:
            header += '    '
        header += '// %s\n' % header_string

        if not small_header:
            if indent:
                header += '    '
            header += '// %s\n\n' % header_dashes
        return header

    # Create a comment header for the extension function section to follow
    def OutputExtensionSectionHeader(self, small_header, indent, ext_type, ext_name, identifier):
        header_string = '---- Vulkan %s extension %s %s ----' % (ext_type, ext_name, identifier)
        header_dashes = ''
        for x in range(0, len(header_string)):
            header_dashes += '-'

        header = '\n'
        if not small_header:
            if indent:
                header += '    '
            header += '// %s\n' % header_dashes

        if indent:
            header += '    '
        header += '// %s\n' % header_string

        if not small_header:
            if indent:
                header += '    '
            header += '// %s\n\n' % header_dashes
        return header

    # Output physical device unwrapping and validation functionality for trampolines
    def OutputTrampPhysDevValidFunc(self, func_name, var_name):
        valid = '    const VkLayerInstanceDispatchTable *disp;\n'
        valid += '    VkPhysicalDevice unwrapped_phys_dev = loader_unwrap_physical_device(%s);\n' % var_name
        valid += '    if (VK_NULL_HANDLE == unwrapped_phys_dev) {\n'
        valid += '        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,\n'
        valid += '                   "%s: Invalid VkPhysicalDevice [VUID-%s-%s-parameter]");\n' % (func_name, func_name, var_name)
        valid += '        abort(); /* Intentionally fail so user can correct issue. */\n'
        valid += '    }\n'
        valid += '    disp = loader_get_instance_layer_dispatch(%s);\n' % var_name
        return valid

    # Output instance unwrapping and validation functionality for trampolines
    def OutputTrampInstanceValidFunc(self, func_name, var_name):
        valid = '    struct loader_instance *inst = loader_get_instance(%s);\n' % var_name
        valid += '    if (NULL == inst) {\n'
        valid += '        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,\n'
        valid += '                   "%s: Invalid VkInstance [VUID-%s-%s-parameter]");\n' % (func_name, func_name, var_name)
        valid += '        abort(); /* Intentionally fail so user can correct issue. */\n'
        valid += '    }\n'
        valid += '#error("Not implemented. Likely needs to be manually generated!");\n'
        return valid

    # Output generic unwrapping and validation functionality for trampolines
    def OutputTrampGenericValidFunc(self, is_extension, func_name, var_type, var_name):
        valid = '    const VkLayerDispatchTable *disp = loader_get_dispatch(%s);\n' % var_name
        if is_extension:
            valid += '    if (NULL == disp) {\n'
            valid += '        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,\n'
            valid += '                   "%s: Invalid %s [VUID-%s-%s-parameter]");\n' % (func_name, var_type, func_name, var_name)
            valid += '        abort(); /* Intentionally fail so user can correct issue. */\n'
            valid += '    }\n'
        return valid

    # Output some code to fix debug marker contents for trampolines
    def OutputTrampDebugMarkerDataTypePatches(self, func_name):
        fixes = ''
        if 'DebugMarkerSetObjectName' in func_name:
            fixes += '    VkDebugMarkerObjectNameInfoEXT local_name_info;\n'
            fixes += '    memcpy(&local_name_info, pNameInfo, sizeof(VkDebugMarkerObjectNameInfoEXT));\n'
            fixes += '    // If this is a physical device, we have to replace it with the proper one for the next call.\n'
            fixes += '    if (pNameInfo->objectType == VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT) {\n'
            fixes += '        struct loader_physical_device_tramp *phys_dev_tramp = (struct loader_physical_device_tramp *)(uintptr_t)pNameInfo->object;\n'
            fixes += '        local_name_info.object = (uint64_t)(uintptr_t)phys_dev_tramp->phys_dev;\n'
            fixes += '    }\n'
        elif 'DebugMarkerSetObjectTag' in func_name:
            fixes += '    VkDebugMarkerObjectTagInfoEXT local_tag_info;\n'
            fixes += '    memcpy(&local_tag_info, pTagInfo, sizeof(VkDebugMarkerObjectTagInfoEXT));\n'
            fixes += '    // If this is a physical device, we have to replace it with the proper one for the next call.\n'
            fixes += '    if (pTagInfo->objectType == VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT) {\n'
            fixes += '        struct loader_physical_device_tramp *phys_dev_tramp = (struct loader_physical_device_tramp *)(uintptr_t)pTagInfo->object;\n'
            fixes += '        local_tag_info.object = (uint64_t)(uintptr_t)phys_dev_tramp->phys_dev;\n'
            fixes += '    }\n'
        return fixes

    # Output some code to fix debug marker contents for trampolines
    def OutputTrampDebugUtilDataTypePatches(self, func_name):
        fixes = ''
        if 'SetDebugUtilsObjectName' in func_name:
            fixes += '    VkDebugUtilsObjectNameInfoEXT local_name_info;\n'
            fixes += '    memcpy(&local_name_info, pNameInfo, sizeof(VkDebugUtilsObjectNameInfoEXT));\n'
            fixes += '    // If this is a physical device, we have to replace it with the proper one for the next call.\n'
            fixes += '    if (pNameInfo->objectType == VK_OBJECT_TYPE_PHYSICAL_DEVICE) {\n'
            fixes += '        struct loader_physical_device_tramp *phys_dev_tramp = (struct loader_physical_device_tramp *)(uintptr_t)pNameInfo->objectHandle;\n'
            fixes += '        local_name_info.objectHandle = (uint64_t)(uintptr_t)phys_dev_tramp->phys_dev;\n'
            fixes += '    }\n'
        elif 'SetDebugUtilsObjectTag' in func_name:
            fixes += '    VkDebugUtilsObjectTagInfoEXT local_tag_info;\n'
            fixes += '    memcpy(&local_tag_info, pTagInfo, sizeof(VkDebugUtilsObjectTagInfoEXT));\n'
            fixes += '    // If this is a physical device, we have to replace it with the proper one for the next call.\n'
            fixes += '    if (pTagInfo->objectType == VK_OBJECT_TYPE_PHYSICAL_DEVICE) {\n'
            fixes += '        struct loader_physical_device_tramp *phys_dev_tramp = (struct loader_physical_device_tramp *)(uintptr_t)pTagInfo->objectHandle;\n'
            fixes += '        local_tag_info.objectHandle = (uint64_t)(uintptr_t)phys_dev_tramp->phys_dev;\n'
            fixes += '    }\n'
        return fixes

    # Print out a call to the next step past the trampoline
    def PrintOutInternalTrampolineCommandCall(self, command_name, return_prefix, handle_type, command_params):
        cmd_call = return_prefix
        if handle_type == 'VkInstance':
            cmd_call  += 'inst->'
        cmd_call  += 'disp->'
        cmd_call  += command_name
        cmd_call  += '('
        past_first = False
        for param in command_params:
            if past_first:
                cmd_call  += ', '
            past_first = True

            if param.type == 'VkPhysicalDevice':
                cmd_call  += 'unwrapped_phys_dev'
            elif ('DebugMarkerSetObject' in command_name or 'SetDebugUtilsObject' in command_name) and param.name == 'pNameInfo':
                    cmd_call  += '&local_name_info'
            elif ('DebugMarkerSetObject' in command_name or 'SetDebugUtilsObject' in command_name) and param.name == 'pTagInfo':
                    cmd_call  += '&local_tag_info'
            else:
                cmd_call  += param.name

        cmd_call  += ');\n'
        return cmd_call

    # Determine if we need to set the dispatch on a returned handle
    def NeedSetDispatch(self, command):
        last_param = command.params[-1]
        if last_param.type in self.dispatchable_handles and last_param.is_pointer and not last_param.is_const:
            return True
        return False

    # Print out a call to set the dispatch on a returned handle
    def PrintOutSetDispatch(self, command):
        set_dispatch = ''
        last_param = command.params[-1]
        if last_param.type in self.dispatchable_handles and last_param.is_pointer and not last_param.is_const:
            is_array = (last_param.type[-1] != 's' and last_param.name[-1] == 's')
            has_temp_wrapper = False
            pre_space = '    '
            if command.return_type is not None and 'VkResult' in command.return_type.text:
                set_dispatch += pre_space
                set_dispatch += 'if (VK_SUCCESS == temp) {\n'
                pre_space += '    '
                has_temp_wrapper = True

            if is_array:
                set_dispatch += pre_space
                array_count = 'UNKNOWN_COUNT_ITEM_PLEASE_EDIT_SCRIPT'
                if (command.name == 'vkAllocateCommandBuffers'):
                    array_count = 'pAllocateInfo->commandBufferCount'
                set_dispatch += 'for (uint32_t i = 0; i < %s; i++) {\n' % array_count
                pre_space += '    '
                set_dispatch += pre_space
                set_dispatch += 'if (%s != NULL && %s[i] != NULL) {\n' % (last_param.name, last_param.name)
            else:
                set_dispatch += pre_space
                set_dispatch += 'if (%s != NULL && *%s != NULL) {\n' % (last_param.name, last_param.name)

            pre_space += '    '

            if is_array:
                set_dispatch += pre_space
                set_dispatch += 'loader_set_dispatch(%s[i], disp);\n' % last_param.name
                pre_space = pre_space[0:-4]
                set_dispatch += pre_space
                set_dispatch += '}\n'
            else:
                set_dispatch += pre_space
                set_dispatch += 'loader_set_dispatch(*%s, disp);\n' % last_param.name

            if command.return_type == 'VkResult':
                pre_space = pre_space[0:-4]
                set_dispatch += pre_space
                set_dispatch += '}\n'

            pre_space = pre_space[0:-4]
            set_dispatch += pre_space
            set_dispatch += '}\n'

            if has_temp_wrapper:
                pre_space = pre_space[0:-4]
                set_dispatch += pre_space
                set_dispatch += '}\n'

                set_dispatch += pre_space
                set_dispatch += 'return temp;\n'

        return set_dispatch

    # Output a function trampoline definition
    def OutputTrampolineDefinition(self, is_extension, ext_name, cmd):
        has_return_type = False
        needs_tmp_var = False
        return_prefix = '    '
        if cmd.return_type is not None:
            # If it's filling in set dispatch, we can't just return
            if self.NeedSetDispatch(cmd):
                needs_tmp_var = True
                return_prefix += 'temp = '
            else:
                return_prefix += 'return '
            has_return_type = True

        cmd_base_name = cmd.name[2:]

        tramp = '\n'
        if is_extension:
            command = cmd.cdecl.replace(';', ' {\n').replace('VKAPI_CALL vk', 'VKAPI_CALL ')
        else:
            command = cmd.cdecl.replace(';', ' {\n').replace('VKAPI_ATTR', 'LOADER_EXPORT VKAPI_ATTR')

        tramp += command

        if needs_tmp_var:
            tramp += '    %s temp;\n' % cmd.return_type.text

        if cmd.handle_type == 'VkPhysicalDevice':
            tramp += self.OutputTrampPhysDevValidFunc(cmd.name, cmd.params[0].name)
        elif cmd.handle_type == 'VkInstance':
            tramp += self.OutputTrampInstanceValidFunc(cmd.name, cmd.params[0].name)
        else:
            tramp += self.OutputTrampGenericValidFunc(is_extension, cmd.name, cmd.params[0].type, cmd.params[0].name)

        # Patch any data types necessary
        tramp += self.OutputTrampDebugMarkerDataTypePatches(cmd.name)
        tramp += self.OutputTrampDebugUtilDataTypePatches(cmd.name)

        printed_inst_ext_check = False
        if ext_name in NULL_CHECK_EXT_NAMES:
            tramp += '    if (disp->' + cmd_base_name + ' != NULL) {\n'
            tramp += '    '
        elif cmd.alias is not None and len(cmd.alias_ext) > 0 and cmd.handle_type == 'VkPhysicalDevice':
            for ext in self.basic_extensions:
                if ext.name == cmd.alias_ext:
                    if ext.type == 'instance':
                        extension_var_name = cmd.alias_ext[3:].lower()
                        tramp += '    const struct loader_instance *inst = ((struct loader_physical_device_tramp *)%s)->this_instance;\n' % cmd.params[0].name
                        tramp += '    if (inst != NULL && inst->inst_ext_enables.%s) {\n' % extension_var_name
                        tramp += '    '
                        tramp += self.PrintOutInternalTrampolineCommandCall(cmd.alias[2:], return_prefix, cmd.handle_type, cmd.params)
                        tramp += '    } else {\n'
                        tramp += '    '
                        printed_inst_ext_check = True
                        break

        tramp += self.PrintOutInternalTrampolineCommandCall(cmd_base_name, return_prefix, cmd.handle_type, cmd.params)

        if ext_name in NULL_CHECK_EXT_NAMES:
            if cmd.return_type is not None:
                tramp += '    } else {\n'
                tramp += '        return VK_SUCCESS;\n'
            tramp += '    }\n'
        elif printed_inst_ext_check:
            tramp += '    }\n'
        else:
            tramp += self.PrintOutSetDispatch(cmd)

        tramp += '}\n'

        return tramp

    # Creates the definitions for the loader's trampolines
    def OutputLoaderTrampolineDefinitions(self):
        tramps = ''

        for cur_core_cmd_group in self.core_command_groups:
            if cur_core_cmd_group.needs_tramp:
                tramps += self.OutputCoreSectionHeader(False, False, cur_core_cmd_group.major, cur_core_cmd_group.minor, 'trampolines')

                for cur_cmd in cur_core_cmd_group.inst_cmds:
                    if cur_cmd.needs_tramp:
                        tramps += self.OutputTrampolineDefinition(False, '', cur_cmd)

                for cur_cmd in cur_core_cmd_group.dev_cmds:
                    if cur_cmd.needs_tramp:
                        tramps += self.OutputTrampolineDefinition(False, '', cur_cmd)

        for cur_ext_cmd_group in self.ext_command_groups:
            if cur_ext_cmd_group.needs_tramp:
                tramps += self.OutputExtensionSectionHeader(False, False, cur_ext_cmd_group.ext_type, cur_ext_cmd_group.ext_name, 'trampolines')
                if (cur_ext_cmd_group.protect is not None):
                    tramps += '#ifdef %s\n' % cur_ext_cmd_group.protect

                for cur_cmd in cur_ext_cmd_group.cmds:
                    if cur_cmd.needs_tramp:
                        tramps += self.OutputTrampolineDefinition(True, cur_ext_cmd_group.ext_name, cur_cmd)

                if (cur_ext_cmd_group.protect is not None):
                    tramps += '#endif // %s\n' % cur_ext_cmd_group.protect

        tramps += '\n'
        return tramps

    def OutputLoaderTrampGIPA(self, extension_name, extension_type, command):
        gipa = ''
        base_name = command.name[2:]
        if command.alias is not None:
            if len(command.alias_ext) > 0:
                base_name = command.alias[2:]
            else:
                base_name = command.alias

        if (extension_type == 'instance'):
            gipa += '    if (!strcmp("%s", name)) {\n' % (command.name)
            gipa += '        *addr = (ptr_instance->inst_ext_enables.'
            gipa += extension_name[3:].lower()
            gipa += ' == 1)\n'
            gipa += '                     ? (void *)%s\n' % (base_name)
            gipa += '                     : NULL;\n'
            gipa += '        return true;\n'
            gipa += '    }\n'
        else:
            gipa += '    if (!strcmp("%s", name)) {\n' % (command.name)
            gipa += '        *addr = (void *)%s;\n' % (base_name)
            gipa += '        return true;\n'
            gipa += '    }\n'

        return gipa


    def OutputLoaderTrampolineUtils(self):
        utils = ''
        utils += '// GPA helpers for extensions\n'
        utils += 'bool extension_instance_gpa(struct loader_instance *ptr_instance, const char *name, void **addr) {\n'
        utils += '    *addr = NULL;\n\n'

        for cur_ext_cmd_group in self.ext_command_groups:
            if (cur_ext_cmd_group.ext_name in WSI_EXT_NAMES or cur_ext_cmd_group.ext_name in TRAMP_EXT_GIPA_AVOID_EXTS or
                'android' in cur_ext_cmd_group.ext_name.lower() or 0 == len(cur_ext_cmd_group.cmds)):
                continue
            utils += '\n    // ---- %s extension commands\n' % cur_ext_cmd_group.ext_name
            if cur_ext_cmd_group.protect is not None:
                utils += '#ifdef %s\n' % cur_ext_cmd_group.protect

            for cur_cmd in cur_ext_cmd_group.cmds:
                if cur_cmd.name in TRAMP_EXT_GIPA_AVOID_CMD_NAMES:
                    continue
                utils += self.OutputLoaderTrampGIPA(cur_ext_cmd_group.ext_name, cur_ext_cmd_group.ext_type, cur_cmd)

            if cur_ext_cmd_group.protect is not None:
                utils += '#endif // %s\n' % cur_ext_cmd_group.protect

        utils += '    return false;\n'
        utils += '}\n\n'
        return utils

    # Output Additional terminator prototypes
    def OutputAdditionalTerminatorPrototypes(self):
        prototypes = ''
        prototypes += 'struct loader_device;\n'
        prototypes += 'struct loader_icd_term;\n'
        prototypes += '\n'
        prototypes += '\n'
        prototypes += '// Dispatch table properly filled in with appropriate terminators for the\n'
        prototypes += '// supported extensions.\n'
        prototypes += 'extern const VkLayerInstanceDispatchTable instance_term_disp;\n'
        prototypes += '\n'
        prototypes += '\n'
        prototypes += 'VKAPI_ATTR bool VKAPI_CALL loader_icd_init_entries(struct loader_icd_term *icd_term, VkInstance inst,\n'
        prototypes += '                                                   const PFN_vkGetInstanceProcAddr fp_gipa);\n'
        prototypes += '\n'
        prototypes += '// Extension interception for vkGetDeviceProcAddr function, so we can return\n'
        prototypes += '// an appropriate terminator if this is one of those few device commands requiring\n'
        prototypes += '// a terminator.\n'
        prototypes += 'PFN_vkVoidFunction get_extension_device_proc_terminator(struct loader_device *dev, const char *pName);\n'
        prototypes += '\n'

        skip_commands = ['vkEnumerateInstanceExtensionProperties',
                         'vkEnumerateInstanceLayerProperties',
                         'vkEnumerateInstanceVersion',
                        ]

        prototypes += '// Manually implemented terminators\n'
        prototypes += '// --------------------------------\n'
        for term in MANUALLY_IMPLEMENTED_TERMINATORS:
            if term in skip_commands:
                continue
            for cmd in self.basic_commands:
                if cmd.name == term:
                    cmd_proto = cmd.cdecl.replace('VKAPI_CALL vk', 'VKAPI_CALL terminator_')
                    cmd_proto = re.sub('\n', ' ', cmd_proto)
                    cmd_proto = re.sub(' +', ' ', cmd_proto)
                    prototypes += cmd_proto
                    prototypes += '\n'
                    break
        return prototypes

    # Output a function terminator prototype
    def OutputTerminatorPrototype(self, cmd):
        prototype = ''
        mod_string = cmd.cdecl.replace("VKAPI_CALL vk", "VKAPI_CALL terminator_")

        prototype += mod_string
        prototype += '\n'

        return prototype

    # Creates the prototypes for the loader's terminators (we only need ones from certain extensions right now)
    def OutputLoaderTerminatorPrototypes(self):
        prototypes = ''

        for cur_ext_cmd_group in self.ext_command_groups:
            if cur_ext_cmd_group.ext_name != 'VK_EXT_debug_utils':
                continue

            if (cur_ext_cmd_group.protect is not None):
                prototypes += '#ifdef %s\n' % cur_ext_cmd_group.protect

            prototypes += self.OutputExtensionSectionHeader(False, False, cur_ext_cmd_group.ext_type, cur_ext_cmd_group.ext_name, 'terminators')

            for cur_cmd in cur_ext_cmd_group.cmds:
                if not cur_cmd.needs_term or cur_cmd.alias is not None:
                    continue
                prototypes += self.OutputTerminatorPrototype(cur_cmd)

            if (cur_ext_cmd_group.protect is not None):
                prototypes += '#endif // %s\n' % cur_ext_cmd_group.protect

        prototypes += '\n'
        return prototypes

    # Output physical device unwrapping and validation functionality for terminators
    def OutputTermPhysDevValidFunc(self, ext_type, base_name, var_name, has_return):
        valid = '    struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)%s;\n' % var_name
        valid += '    struct loader_icd_term *icd_term = phys_dev_term->this_icd_term;\n'
        valid += '    if (NULL == icd_term->dispatch.'
        valid += base_name
        valid += ') {\n'
        valid += '        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_DRIVER_BIT, 0,\n'
        valid += '                   "Driver %s with VkPhysicalDevice \\"%s\\" does not support '
        valid += base_name
        valid += '",\n'
        valid += '                    icd_term->scanned_icd->lib_name, phys_dev_term->properties.deviceName);\n'
        # If this is an instance function taking a physical device (i.e. pre Vulkan 1.1), we need to behave and not crash so return an
        # error here.
        if ext_type =='instance' and has_return:
            valid += '        return VK_ERROR_INITIALIZATION_FAILED;\n'
        valid += '    }\n'
        return valid

    # Output instance unwrapping and validation functionality for terminators
    def OutputTermInstanceValidFunc(self, func_name, var_name):
        valid = '    struct loader_instance *inst = loader_get_instance(%s);\n' % var_name
        valid += '    if (NULL == inst) {\n'
        valid += '        loader_log(NULL, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_VALIDATION_BIT, 0,\n'
        valid += '                   "%s: Invalid VkInstance [VUID-%s-%s-parameter]");\n' % (func_name, func_name, var_name)
        valid += '        abort(); /* Intentionally fail so user can correct issue. */\n'
        valid += '    }\n'
        valid += '#error("Not implemented. Likely needs to be manually generated!");\n'
        return valid

    # Output surface unwrapping and validation functionality for terminators
    def OutputTermSurfaceValidFunc(self, cmd, base_name, var_name, return_prefix):
        valid = '    uint32_t icd_index = 0;\n'
        valid += '    struct loader_device *dev;\n'
        valid += '    struct loader_icd_term *icd_term = loader_get_icd_and_device(device, &dev, &icd_index);\n'
        valid += '    if (NULL != icd_term && NULL != icd_term->dispatch.%s) {\n' % base_name
        valid += '        VkIcdSurface *icd_surface = (VkIcdSurface *)(uintptr_t)(%s);\n' % var_name
        valid += '        if (NULL != icd_surface->real_icd_surfaces && (VkSurfaceKHR)NULL != icd_surface->real_icd_surfaces[icd_index]) {\n'
        valid += '        %sicd_term->dispatch.%s(' % (return_prefix, base_name)
        out_list = []
        for param in cmd.params:
            if param.type == 'VkSurfaceKHR':
                out_list.append('icd_surface->real_icd_surfaces[icd_index]')
            else:
                out_list.append(param.name)
        valid += ', '.join(out_list)
        valid += ');\n'
        if len(return_prefix) == 0:
            valid += '                return;\n'
        valid += '        }\n'
        return valid

    # Output Debug Marker unwrapping and validation functionality for terminators
    def OutputTermDebugMarkerValidFunc(self, base_name, var_name):
        valid = '    uint32_t icd_index = 0;\n'
        valid += '    struct loader_device *dev;\n'
        valid += '    struct loader_icd_term *icd_term = loader_get_icd_and_device(%s, &dev, &icd_index);\n' % var_name
        valid += '    if (NULL != icd_term && NULL != icd_term->dispatch.'
        valid += base_name
        valid += ') {\n'

        if 'DebugMarkerSetObjectName' in base_name:
            valid += '        VkDebugMarkerObjectNameInfoEXT local_name_info;\n'
            valid += '        memcpy(&local_name_info, pNameInfo, sizeof(VkDebugMarkerObjectNameInfoEXT));\n'
            valid += '        // If this is a physical device, we have to replace it with the proper one for the next call.\n'
            valid += '        if (pNameInfo->objectType == VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT) {\n'
            valid += '            struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)(uintptr_t)pNameInfo->object;\n'
            valid += '            local_name_info.object = (uint64_t)(uintptr_t)phys_dev_term->phys_dev;\n'
            valid += '        // If this is a KHR_surface, and the ICD has created its own, we have to replace it with the proper one for the next call.\n'
            valid += '        } else if (pNameInfo->objectType == VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT) {\n'
            valid += '            if (NULL != icd_term && NULL != icd_term->dispatch.CreateSwapchainKHR) {\n'
            valid += '                VkIcdSurface *icd_surface = (VkIcdSurface *)(uintptr_t)pNameInfo->object;\n'
            valid += '                if (NULL != icd_surface->real_icd_surfaces) {\n'
            valid += '                    local_name_info.object = (uint64_t)icd_surface->real_icd_surfaces[icd_index];\n'
            valid += '                }\n'
            valid += '            }\n'
            valid += '        }\n'

        elif 'DebugMarkerSetObjectTag' in base_name:
            valid += '        VkDebugMarkerObjectTagInfoEXT local_tag_info;\n'
            valid += '        memcpy(&local_tag_info, pTagInfo, sizeof(VkDebugMarkerObjectTagInfoEXT));\n'
            valid += '        // If this is a physical device, we have to replace it with the proper one for the next call.\n'
            valid += '        if (pTagInfo->objectType == VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT) {\n'
            valid += '            struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)(uintptr_t)pTagInfo->object;\n'
            valid += '            local_tag_info.object = (uint64_t)(uintptr_t)phys_dev_term->phys_dev;\n'
            valid += '        // If this is a KHR_surface, and the ICD has created its own, we have to replace it with the proper one for the next call.\n'
            valid += '        } else if (pTagInfo->objectType == VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT) {\n'
            valid += '            if (NULL != icd_term && NULL != icd_term->dispatch.CreateSwapchainKHR) {\n'
            valid += '                VkIcdSurface *icd_surface = (VkIcdSurface *)(uintptr_t)pTagInfo->object;\n'
            valid += '                if (NULL != icd_surface->real_icd_surfaces) {\n'
            valid += '                    local_tag_info.object = (uint64_t)icd_surface->real_icd_surfaces[icd_index];\n'
            valid += '                }\n'
            valid += '            }\n'
            valid += '        }\n'
        valid += '    '
        return valid

    # Output Debug Utils unwrapping and validation functionality for terminators
    def OutputTermDebugUtilsValidFunc(self, base_name, var_name):
        valid = '    uint32_t icd_index = 0;\n'
        valid += '    struct loader_device *dev;\n'
        valid += '    struct loader_icd_term *icd_term = loader_get_icd_and_device(%s, &dev, &icd_index);\n' % var_name
        valid += '    if (NULL != icd_term && NULL != icd_term->dispatch.'
        valid += base_name
        valid += ') {\n'

        if 'SetDebugUtilsObjectName' in base_name:
            valid += '        VkDebugUtilsObjectNameInfoEXT local_name_info;\n'
            valid += '        memcpy(&local_name_info, pNameInfo, sizeof(VkDebugUtilsObjectNameInfoEXT));\n'
            valid += '        // If this is a physical device, we have to replace it with the proper one for the next call.\n'
            valid += '        if (pNameInfo->objectType == VK_OBJECT_TYPE_PHYSICAL_DEVICE) {\n'
            valid += '            struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)(uintptr_t)pNameInfo->objectHandle;\n'
            valid += '            local_name_info.objectHandle = (uint64_t)(uintptr_t)phys_dev_term->phys_dev;\n'
            valid += '        // If this is a KHR_surface, and the ICD has created its own, we have to replace it with the proper one for the next call.\n'
            valid += '        } else if (pNameInfo->objectType == VK_OBJECT_TYPE_SURFACE_KHR) {\n'
            valid += '            if (NULL != icd_term && NULL != icd_term->dispatch.CreateSwapchainKHR) {\n'
            valid += '                VkIcdSurface *icd_surface = (VkIcdSurface *)(uintptr_t)pNameInfo->objectHandle;\n'
            valid += '                if (NULL != icd_surface->real_icd_surfaces) {\n'
            valid += '                    local_name_info.objectHandle = (uint64_t)icd_surface->real_icd_surfaces[icd_index];\n'
            valid += '                }\n'
            valid += '            }\n'
            valid += '        }\n'
        elif 'SetDebugUtilsObjectTag' in base_name:
            valid += '        VkDebugUtilsObjectTagInfoEXT local_tag_info;\n'
            valid += '        memcpy(&local_tag_info, pTagInfo, sizeof(VkDebugUtilsObjectTagInfoEXT));\n'
            valid += '        // If this is a physical device, we have to replace it with the proper one for the next call.\n'
            valid += '        if (pTagInfo->objectType == VK_OBJECT_TYPE_PHYSICAL_DEVICE) {\n'
            valid += '            struct loader_physical_device_term *phys_dev_term = (struct loader_physical_device_term *)(uintptr_t)pTagInfo->objectHandle;\n'
            valid += '            local_tag_info.objectHandle = (uint64_t)(uintptr_t)phys_dev_term->phys_dev;\n'
            valid += '        // If this is a KHR_surface, and the ICD has created its own, we have to replace it with the proper one for the next call.\n'
            valid += '        } else if (pTagInfo->objectType == VK_OBJECT_TYPE_SURFACE_KHR) {\n'
            valid += '            if (NULL != icd_term && NULL != icd_term->dispatch.CreateSwapchainKHR) {\n'
            valid += '                VkIcdSurface *icd_surface = (VkIcdSurface *)(uintptr_t)pTagInfo->objectHandle;\n'
            valid += '                if (NULL != icd_surface->real_icd_surfaces) {\n'
            valid += '                    local_tag_info.objectHandle = (uint64_t)icd_surface->real_icd_surfaces[icd_index];\n'
            valid += '                }\n'
            valid += '            }\n'
            valid += '        }\n'

        valid += '    '
        return valid

    # Output a function terminator definition
    def OutputTerminatorDefinition(self, ext_type, ext_name, cmd):
        has_return_type = False
        return_prefix = '    '
        if (cmd.return_type is not None):
            return_prefix += 'return '
            has_return_type = True

        has_surface = 0
        surface_var_name = ''
        phys_dev_var_name = ''
        instance_var_name = ''
        always_use_param_name = True
        surface_type_to_replace = ''
        surface_name_replacement = ''
        physdev_type_to_replace = ''
        physdev_name_replacement = ''

        for param in cmd.params:
            if param.type == 'VkSurfaceKHR':
                has_surface = 1
                surface_var_name = param.name
                always_use_param_name = False
                surface_type_to_replace = 'VkSurfaceKHR'
                surface_name_replacement = 'icd_surface->real_icd_surfaces[icd_index]'
            if param.type == 'VkPhysicalDeviceSurfaceInfo2KHR':
                has_surface = 1
                surface_var_name = param.name + '->surface'
                update_structure_surface = 1
                update_structure_string = '        VkPhysicalDeviceSurfaceInfo2KHR info_copy = *pSurfaceInfo;\n'
                update_structure_string += '        info_copy.surface = icd_surface->real_icd_surfaces[icd_index];\n'
                always_use_param_name = False
                surface_type_to_replace = 'VkPhysicalDeviceSurfaceInfo2KHR'
                surface_name_replacement = '&info_copy'
            if param.type == 'VkPhysicalDevice':
                phys_dev_var_name = param.name
                always_use_param_name = False
                physdev_type_to_replace = 'VkPhysicalDevice'
                physdev_name_replacement = 'phys_dev_term->phys_dev'
            if param.type == 'VkInstance':
                instance_var_name = param.name

        cmd_base_name = cmd.name[2:]

        term = '\n'

        command = cmd.cdecl.replace(';', ' {\n')
        command = command.replace("VKAPI_CALL vk", "VKAPI_CALL terminator_")

        needs_final_return = False
        needs_final_brace = False
        term += command
        if cmd.handle_type == 'VkPhysicalDevice':
            term += self.OutputTermPhysDevValidFunc(ext_type, cmd_base_name, cmd.params[0].name, has_return_type)
        elif cmd.handle_type == 'VkInstance':
            term += self.OutputTermInstanceValidFunc(cmd.name, cmd.params[0].name)
        elif has_surface:
            term += self.OutputTermSurfaceValidFunc(cmd, cmd_base_name, surface_var_name, return_prefix)
        elif 'DebugMarkerSetObject' in cmd_base_name:
            term += self.OutputTermDebugMarkerValidFunc(cmd_base_name, cmd.params[0].name)
            needs_final_return = has_return_type
            needs_final_brace = not needs_final_return
        elif 'SetDebugUtilsObject' in cmd_base_name or 'DebugUtilsLabel' in cmd_base_name:
            term += self.OutputTermDebugUtilsValidFunc(cmd_base_name, cmd.params[0].name)
            needs_final_return = has_return_type
            needs_final_brace = not needs_final_return

        term += return_prefix
        if cmd.handle_type == 'VkInstance':
            term += 'inst->'
        term += 'icd_term->dispatch.'
        term += cmd_base_name
        term += '('
        count = 0
        for param in cmd.params:
            if count != 0:
                term += ', '

            if param.type == 'VkPhysicalDevice':
                term += 'phys_dev_term->phys_dev'
            elif param.type == 'VkSurfaceKHR':
                term += 'icd_surface->real_icd_surfaces[icd_index]'
            elif ('DebugMarkerSetObject' in cmd.name or 'SetDebugUtilsObject' in cmd.name) and param.name == 'pNameInfo':
                term += '&local_name_info'
            elif ('DebugMarkerSetObject' in cmd.name or 'SetDebugUtilsObject' in cmd.name) and param.name == 'pTagInfo':
                term += '&local_tag_info'
            else:
                term += param.name

            count += 1
        term += ');\n'
        if needs_final_return:
            term += '    } else {\n'
            term += '        return VK_SUCCESS;\n'
            term += '    }\n'
        elif needs_final_brace:
            term += '    }\n'
        term += '}\n'
        return term

    # Creates the definitions for the loader's terminators
    def OutputLoaderTerminatorDefinitions(self):
        terms = ''

        for cur_core_cmd_group in self.core_command_groups:
            if not cur_core_cmd_group.needs_term:
                continue
            terms += self.OutputCoreSectionHeader(False, False, cur_core_cmd_group.major, cur_core_cmd_group.minor, 'terminators')

            for cur_cmd in cur_core_cmd_group.inst_cmds:
                if not cur_cmd.needs_term:
                    continue
                terms += self.OutputTerminatorDefinition('', '', cur_cmd)

            for cur_cmd in cur_core_cmd_group.dev_cmds:
                if not cur_cmd.needs_term:
                    continue
                terms += self.OutputTerminatorDefinition('', '', cur_cmd)

        for cur_ext_cmd_group in self.ext_command_groups:
            print_header = True
            if not cur_ext_cmd_group.needs_term:
                continue

            for cur_cmd in cur_ext_cmd_group.cmds:
                if not cur_cmd.needs_term or cur_cmd.alias is not None:
                    continue
                if print_header:
                    terms += self.OutputExtensionSectionHeader(False, False, cur_ext_cmd_group.ext_type, cur_ext_cmd_group.ext_name, 'terminators')
                    if (cur_ext_cmd_group.protect is not None):
                        terms += '#ifdef %s\n' % cur_ext_cmd_group.protect
                    print_header = False
                terms += self.OutputTerminatorDefinition(cur_ext_cmd_group.ext_type, cur_ext_cmd_group.ext_name, cur_cmd)

            if not print_header and cur_ext_cmd_group.protect is not None:
                terms += '#endif // %s\n' % cur_ext_cmd_group.protect

        terms += '\n'
        return terms

    def PrintTerminatorDispatchTableCommand(self, command_protect, command_name):
        cmd_entry = ''

        # Remove 'vk' from proto name
        base_name = command_name[2:]

        cmd_entry += '    PFN_%s %s;\n' % (command_name, base_name)
        return cmd_entry

    # Create a dispatch table from the appropriate terminator list and return it as a string
    def OutputLoaderTerminatorDispatchStruct(self):
        table = ''
        table += '// ICD function pointer dispatch table\n'
        table += 'struct loader_icd_term_dispatch {\n'

        for cur_core_cmd_group in self.core_command_groups:
            print_header = True

            for cur_cmd in cur_core_cmd_group.inst_cmds:
                if "GetInstanceProcAddr" in cur_cmd.name or "EnumerateDeviceLayerProperties" in cur_cmd.name:
                    continue

                if print_header:
                    table += self.OutputCoreSectionHeader(True, True, cur_core_cmd_group.major, cur_core_cmd_group.minor, '')
                    print_header = False

                table += self.PrintTerminatorDispatchTableCommand(cur_cmd.protect, cur_cmd.name)
    
            for cur_cmd in cur_core_cmd_group.dev_cmds:
                if (cur_cmd.requires_term_disp or 'GetDeviceProcAddr' in cur_cmd.name):
                    if print_header:
                        table += self.OutputCoreSectionHeader(True, True, cur_core_cmd_group.major, cur_core_cmd_group.minor, '')
                        print_header = False

                    table += self.PrintTerminatorDispatchTableCommand(cur_cmd.protect, cur_cmd.name)

        for cur_ext_cmd_group in self.ext_command_groups:
            print_header = True
            if cur_ext_cmd_group.requires_term_disp:

                for cur_cmd in cur_ext_cmd_group.cmds:
                    if cur_cmd.requires_term_disp or self.CommandIsInstanceType(cur_cmd.handle_type, cur_ext_cmd_group.ext_name):
                        if print_header:
                            table += self.OutputExtensionSectionHeader(True, True, cur_ext_cmd_group.ext_type, cur_ext_cmd_group.ext_name, '')
                            if cur_ext_cmd_group.protect is not None:
                                table += '#ifdef %s\n' % cur_ext_cmd_group.protect

                            print_header = False
                        table += self.PrintTerminatorDispatchTableCommand(cur_cmd.protect, cur_cmd.name)

                if not print_header and cur_ext_cmd_group.protect is not None:
                    table += '#endif // %s\n' % cur_ext_cmd_group.protect

        table += '};\n\n'
        return table

    def GenerateInstanceExtensionList(self):
        ext_used = []
        # First add instance extensions we need to verify are enabled
        for cur_ext_cmd_group in self.ext_command_groups:
            if cur_ext_cmd_group.ext_name not in ext_used:
                if 'instance' == cur_ext_cmd_group.ext_type and 'android' not in cur_ext_cmd_group.ext_name.lower():
                    ext_used.append(cur_ext_cmd_group)
                else:
                    for cur_cmd in cur_ext_cmd_group.cmds:
                        if cur_cmd.handle_type == 'VkPhysicalDevice':
                            ext_used.append(cur_ext_cmd_group)
                            break

        # Hack, make sure the first one is not protected or else it messes up the if else blocks
        if ext_used[0].protect is not None:
            for i in range(0, len(ext_used)):
                if ext_used[i].protect is None:
                    ext_used[0], ext_used[i] = ext_used[i], ext_used[0]
                    break

        return ext_used

    # Add externs and prototype
    def OutputInstaceExtensionPrototypes(self):
        protos = '\n'
        protos += '// Define types as externally available structs where necessary\n'
        protos += 'struct loader_instance;\n'
        protos += 'struct loader_dev_dispatch_table;\n'
        protos += 'struct loader_physical_device_term;\n'
        protos += '\n'
        protos += '\n'
        protos += '// Extension interception for vkCreateInstance function, so we can properly detect and\n'
        protos += '// enable any instance extension information for extensions we know about.\n'
        protos += 'void extensions_create_instance(struct loader_instance *ptr_instance, const VkInstanceCreateInfo *pCreateInfo);\n'
        protos += '\n'
        protos += '// Extension interception for vkCreateDevice function, so we can properly detect and\n'
        protos += '// enable any device extension information for extensions we know about.\n'
        protos += 'void extensions_create_device(struct loader_device *dev, const struct loader_physical_device_term *phys_dev_term,\n'
        protos += '                              const VkDeviceCreateInfo *pCreateInfo);\n'
        protos += '\n'
        protos += '// Array of extension strings for instance extensions we support.\n'
        protos += 'extern const char *const LOADER_INSTANCE_EXTENSIONS[];\n'
        protos += '\n'
        protos += '// Init Device function pointer dispatch table with core commands\n'
        protos += 'VKAPI_ATTR void VKAPI_CALL loader_init_device_dispatch_table(struct loader_dev_dispatch_table *dev_table, PFN_vkGetDeviceProcAddr gdpa,\n'
        protos += '                                                             VkDevice dev);\n'
        protos += '\n'
        protos += '// Init Device function pointer dispatch table with extension commands\n'
        protos += 'VKAPI_ATTR void VKAPI_CALL loader_init_device_extension_dispatch_table(struct loader_dev_dispatch_table *dev_table,\n'
        protos += '                                                                       PFN_vkGetInstanceProcAddr gipa,\n'
        protos += '                                                                       PFN_vkGetDeviceProcAddr gdpa,\n'
        protos += '                                                                       VkInstance inst,\n'
        protos += '                                                                       VkDevice dev);\n'
        protos += '\n'
        protos += '// Init Instance function pointer dispatch table with core commands\n'
        protos += 'VKAPI_ATTR void VKAPI_CALL loader_init_instance_core_dispatch_table(VkLayerInstanceDispatchTable *table, PFN_vkGetInstanceProcAddr gipa,\n'
        protos += '                                                                    VkInstance inst);\n'
        protos += '\n'
        protos += '// Init Instance function pointer dispatch table with core commands\n'
        protos += 'VKAPI_ATTR void VKAPI_CALL loader_init_instance_extension_dispatch_table(VkLayerInstanceDispatchTable *table, PFN_vkGetInstanceProcAddr gipa,\n'
        protos += '                                                                         VkInstance inst);\n'
        protos += '\n'
        protos += '// Instance command lookup function\n'
        protos += 'VKAPI_ATTR void* VKAPI_CALL loader_lookup_instance_dispatch_table(const VkLayerInstanceDispatchTable *table, const char *name,\n'
        protos += '                                                                  bool *found_name);\n'
        protos += '\n'
        protos += '// Device command lookup function\n'
        protos += 'VKAPI_ATTR void* VKAPI_CALL loader_lookup_device_dispatch_table(const VkLayerDispatchTable *table, const char *name);\n'
        protos += '\n'
        return protos

    # Create the extension enable structs
    def OutputExtensionEnableStructs(self):
        structs = ''
        structs += '// Struct of all known instance extensions that the loader may need to refer to when\n'
        structs += '// making function pointer decisions.\n'
        structs += 'struct loader_instance_extension_enables {\n'

        ext_used = self.GenerateInstanceExtensionList()

        # Print out the function
        for ext in ext_used:
            structs += '    uint8_t %s : 1;\n' % ext.ext_name[3:].lower()

        structs += '};\n\n'
        structs += ''
        structs += '// Struct of all device extensions that the loader has to override at least one command for\n'
        structs += 'struct loader_device_extension_enables {\n'

        # Print out the function
        for ext in self.dev_extensions_tracked_by_loader:
            structs += '    uint8_t %s : 1;\n' % ext[3:].lower()

        structs += '};\n\n'
        return structs

    # Create the extension enable check during vkCreateInstance
    def OutputInstanceExtensionEnableCheck(self):
        print_else = False
        cur_extension_name = ''

        create_funcs = ''
        create_funcs += '// A function that can be used to query enabled extensions during a vkCreateInstance call\n'
        create_funcs += 'void extensions_create_instance(struct loader_instance *ptr_instance, const VkInstanceCreateInfo *pCreateInfo) {\n'
        create_funcs += '    for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; i++) {\n'

        ext_used = self.GenerateInstanceExtensionList()

        # Print out the function
        for ext in ext_used:
            create_funcs += '\n        // ---- %s extension commands\n' % ext.ext_name

            if ext.protect is not None:
                create_funcs += '#ifdef %s\n' % ext.protect
            if print_else:
                create_funcs += '        } else if (0 == strcmp(pCreateInfo->ppEnabledExtensionNames[i], '
            else:
                create_funcs += '        if (0 == strcmp(pCreateInfo->ppEnabledExtensionNames[i], '
                print_else = True

            create_funcs += ext.define + ')) {\n'
            create_funcs += '            ptr_instance->inst_ext_enables.'
            create_funcs += ext.ext_name[3:].lower()
            create_funcs += ' = 1;\n'

            if ext.protect is not None:
                create_funcs += '#endif // %s\n' % ext.protect

        create_funcs += '        }\n'
        create_funcs += '    }\n'
        create_funcs += '}\n\n'

        create_funcs += '// A function that can be used to query enabled extensions during a vkCreateDevice call\n'
        create_funcs += 'void extensions_create_device(struct loader_device *dev, const struct loader_physical_device_term *phys_dev_term,\n'
        create_funcs += '                              const VkDeviceCreateInfo *pCreateInfo) {\n'
        create_funcs += '    for (uint32_t i = 0; i < pCreateInfo->enabledExtensionCount; i++) {\n'

        first = True
        for cur_ext_cmd_group in self.ext_command_groups:
            if cur_ext_cmd_group.needs_dev_term_override:
                if cur_ext_cmd_group.protect is not None:
                    create_funcs += '\n'
                    create_funcs += '#ifdef %s\n' % cur_ext_cmd_group.protect
                    create_funcs += '       '
                if first:
                    create_funcs += '        if ('
                    first = False
                else:
                    create_funcs += ' else if ('
                create_funcs += '!strcmp(pCreateInfo->ppEnabledExtensionNames[i], %s_EXTENSION_NAME)) {\n' % cur_ext_cmd_group.ext_name.upper()
                create_funcs += '            dev->dev_ext_enables.%s = 1;\n' % cur_ext_cmd_group.ext_name[3:].lower()

                create_funcs += '        }'

                if cur_ext_cmd_group.protect is not None:
                    create_funcs += '\n'
                    create_funcs += '#endif // %s\n' % cur_ext_cmd_group.protect
        create_funcs += '\n'
        create_funcs += '    }\n'

        # Hard code to determine if we can use VK_EXT_debug_utils
        create_funcs += '    dev->dev_ext_enables.ext_debug_utils = phys_dev_term->this_icd_term->this_instance->inst_ext_enables.ext_debug_utils;\n\n'

        # Hard code to determine if we can use VK_KHR_device_group
        create_funcs += '    if (!dev->dev_ext_enables.khr_device_group && phys_dev_term->properties.apiVersion >= VK_API_VERSION_1_1) {\n'
        create_funcs += '        dev->dev_ext_enables.khr_device_group = 1;\n'
        create_funcs += '    }\n\n'
        create_funcs += '    loader_log(phys_dev_term->this_icd_term->this_instance, VULKAN_LOADER_LAYER_BIT | VULKAN_LOADER_DRIVER_BIT, 0,\n'
        create_funcs += '               "       Using \\"%s\\" using driver \\"%s\\"\\n",\n'
        create_funcs += '               phys_dev_term->properties.deviceName, phys_dev_term->this_icd_term->scanned_icd->lib_name);\n'
        create_funcs += '}\n\n'
        return create_funcs

    # Create the extension name whitelist array
    def OutputInstantExtensionWhitelistArray(self):
        table = ''
        table += '// A null-terminated list of all of the instance extensions supported by the loader.\n'
        table += '// If an instance extension name is not in this list, but it is exported by one or more of the\n'
        table += '// ICDs detected by the loader, then the extension name not in the list will be filtered out\n'
        table += '// before passing the list of extensions to the application.\n'
        table += 'const char *const LOADER_INSTANCE_EXTENSIONS[] = {\n'

        ext_used = self.GenerateInstanceExtensionList()

        for ext in ext_used:
            if ext.protect is not None:
                table += '#ifdef %s\n' % ext.protect
            table += '                                                  '
            table += ext.define + ',\n'

            if ext.protect is not None:
                table += '#endif // %s\n' % ext.protect
        table += '                                                  NULL };\n'
        return table

    def OutputCommandToDispatchTable(self, command_name, inst_dev, proc_addr, is_lookup):
        cmd_disp_entry = ''

        # Remove 'vk' from proto name
        base_name = command_name[2:]

        # Names to skip
        if (base_name == 'CreateInstance' or base_name == 'CreateDevice' or
            base_name == 'EnumerateInstanceExtensionProperties' or
            base_name == 'EnumerateInstanceLayerProperties' or
            base_name == 'EnumerateInstanceVersion'):
            return ''

        # If we're looking for the proc we are passing in, just point the table to it.  This fixes the issue where
        # a layer overrides the function name for the loader.
        if is_lookup:
            cmd_disp_entry += '    if (!strcmp(name, "%s")) return (void *)table->%s;\n' % (base_name, base_name)
        else:
            if base_name == 'GetDeviceProcAddr':
                cmd_disp_entry += '    table->GetDeviceProcAddr = gdpa;\n'
            elif (base_name == 'GetInstanceProcAddr'):
                cmd_disp_entry += '    table->GetInstanceProcAddr = gipa;\n'
            else:
                cmd_disp_entry += '    table->%s = (PFN_%s)%s(%s, "%s");\n' % (base_name, command_name, proc_addr, inst_dev, command_name)

        return cmd_disp_entry

    # Creates code to initialize the various dispatch tables
    def OutputLoaderDispatchTables(self):
        tables = ''

        tables += '// Device extension error function\n'
        tables += 'VKAPI_ATTR VkResult VKAPI_CALL vkDevExtError(VkDevice dev) {\n'
        tables += '    struct loader_device *found_dev;\n'
        tables += '    // The device going in is a trampoline device\n'
        tables += '    struct loader_icd_term *icd_term = loader_get_icd_and_device(dev, &found_dev, NULL);\n'
        tables += '\n'
        tables += '    if (icd_term)\n'
        tables += '        loader_log(icd_term->this_instance, VULKAN_LOADER_ERROR_BIT, 0,\n'
        tables += '                   "Bad destination in loader trampoline dispatch,"\n'
        tables += '                   "Are layers and extensions that you are calling enabled?");\n'
        tables += '    return VK_ERROR_EXTENSION_NOT_PRESENT;\n'
        tables += '}\n\n'

        inst_core_disp = ''
        inst_core_disp += '// Init Instance function pointer dispatch table with core commands\n'
        inst_core_disp += 'VKAPI_ATTR void VKAPI_CALL loader_init_instance_core_dispatch_table(VkLayerInstanceDispatchTable *table, PFN_vkGetInstanceProcAddr gipa,\n'
        inst_core_disp += '                                                                    VkInstance inst) {\n'

        inst_ext_disp = ''
        inst_ext_disp += '// Init Instance function pointer dispatch table with core commands\n'
        inst_ext_disp += 'VKAPI_ATTR void VKAPI_CALL loader_init_instance_extension_dispatch_table(VkLayerInstanceDispatchTable *table, PFN_vkGetInstanceProcAddr gipa,\n'
        inst_ext_disp += '                                                                        VkInstance inst) {\n'

        dev_core_disp = ''
        dev_core_disp += '// Init Device function pointer dispatch table with core commands\n'
        dev_core_disp += 'VKAPI_ATTR void VKAPI_CALL loader_init_device_dispatch_table(struct loader_dev_dispatch_table *dev_table, PFN_vkGetDeviceProcAddr gdpa,\n'
        dev_core_disp += '                                                             VkDevice dev) {\n'
        dev_core_disp += '    VkLayerDispatchTable *table = &dev_table->core_dispatch;\n'
        dev_core_disp += '    table->magic = DEVICE_DISP_TABLE_MAGIC_NUMBER;\n'
        dev_core_disp += '    for (uint32_t i = 0; i < MAX_NUM_UNKNOWN_EXTS; i++) dev_table->ext_dispatch[i] = (PFN_vkDevExt)vkDevExtError;\n'

        dev_ext_disp = ''
        dev_ext_disp += '// Init Device function pointer dispatch table with extension commands\n'
        dev_ext_disp += 'VKAPI_ATTR void VKAPI_CALL loader_init_device_extension_dispatch_table(struct loader_dev_dispatch_table *dev_table,\n'
        dev_ext_disp += '                                                                       PFN_vkGetInstanceProcAddr gipa,\n'
        dev_ext_disp += '                                                                       PFN_vkGetDeviceProcAddr gdpa,\n'
        dev_ext_disp += '                                                                       VkInstance inst,\n'
        dev_ext_disp += '                                                                       VkDevice dev) {\n'
        dev_ext_disp += '    VkLayerDispatchTable *table = &dev_table->core_dispatch;\n'
        dev_ext_disp += '    table->magic = DEVICE_DISP_TABLE_MAGIC_NUMBER;\n'

        lookup_inst_disp = ''
        lookup_inst_disp += '// Instance command lookup function\n'
        lookup_inst_disp += 'VKAPI_ATTR void* VKAPI_CALL loader_lookup_instance_dispatch_table(const VkLayerInstanceDispatchTable *table, const char *name,\n'
        lookup_inst_disp += '                                                                  bool *found_name) {\n'
        lookup_inst_disp += '    if (!name || name[0] != \'v\' || name[1] != \'k\') {\n'
        lookup_inst_disp += '        *found_name = false;\n'
        lookup_inst_disp += '        return NULL;\n'
        lookup_inst_disp += '    }\n'
        lookup_inst_disp += '\n'
        lookup_inst_disp += '    *found_name = true;\n'
        lookup_inst_disp += '    name += 2;\n'

        lookup_dev_disp = ''
        lookup_dev_disp += '// Device command lookup function\n'
        lookup_dev_disp += 'VKAPI_ATTR void* VKAPI_CALL loader_lookup_device_dispatch_table(const VkLayerDispatchTable *table, const char *name) {\n'
        lookup_dev_disp += '    if (!name || name[0] != \'v\' || name[1] != \'k\') return NULL;\n'
        lookup_dev_disp += '\n'
        lookup_dev_disp += '    name += 2;\n'

        for cur_core_cmd_group in self.core_command_groups:
            if len(cur_core_cmd_group.inst_cmds) > 0:
                inst_head = '\n    // ---- Vulkan %d.%d commands\n' % (cur_core_cmd_group.major, cur_core_cmd_group.minor)
                inst_core_disp += inst_head
                lookup_inst_disp += inst_head
            for cur_cmd in cur_core_cmd_group.inst_cmds:
                inst_core_disp += self.OutputCommandToDispatchTable(cur_cmd.name, 'inst', 'gipa', False)
                lookup_inst_disp += self.OutputCommandToDispatchTable(cur_cmd.name, 'inst', 'gipa', True)

            if len(cur_core_cmd_group.dev_cmds) > 0:
                dev_head = '\n    // ---- Vulkan %d.%d commands\n' % (cur_core_cmd_group.major, cur_core_cmd_group.minor)
                dev_core_disp += dev_head
                lookup_dev_disp += dev_head
            for cur_cmd in cur_core_cmd_group.dev_cmds:
                dev_core_disp += self.OutputCommandToDispatchTable(cur_cmd.name, 'dev', 'gdpa', False)
                lookup_dev_disp += self.OutputCommandToDispatchTable(cur_cmd.name, 'dev', 'gdpa', True)

        for cur_ext_cmd_group in self.ext_command_groups:
            print_inst_header = True
            print_dev_header = True
            for cur_cmd in cur_ext_cmd_group.cmds:
                if self.CommandIsInstanceType(cur_cmd.handle_type, cur_ext_cmd_group.ext_name):
                    if print_inst_header:
                        inst_head = '\n    // ---- %s extension commands\n' % cur_ext_cmd_group.ext_name

                        if cur_ext_cmd_group.protect is not None:
                            inst_head += '#ifdef %s\n' % cur_ext_cmd_group.protect

                        inst_ext_disp += inst_head
                        lookup_inst_disp += inst_head
                        print_inst_header = False

                    inst_ext_disp += self.OutputCommandToDispatchTable(cur_cmd.name, 'inst', 'gipa', False)
                    lookup_inst_disp += self.OutputCommandToDispatchTable(cur_cmd.name, 'inst', 'gipa', True)
                else:
                    if print_dev_header:
                        dev_head = '\n    // ---- %s extension commands\n' % cur_ext_cmd_group.ext_name

                        if cur_ext_cmd_group.protect is not None:
                            dev_head += '#ifdef %s\n' % cur_ext_cmd_group.protect

                        dev_ext_disp += dev_head
                        lookup_dev_disp += dev_head
                        print_dev_header = False

                    if 'instance' == cur_ext_cmd_group.ext_type:
                        dev_ext_disp += self.OutputCommandToDispatchTable(cur_cmd.name, 'inst', 'gipa', False)
                    else:
                        dev_ext_disp += self.OutputCommandToDispatchTable(cur_cmd.name, 'dev', 'gdpa', False)
                    lookup_dev_disp += self.OutputCommandToDispatchTable(cur_cmd.name, 'inst', 'gipa', True)
            if cur_ext_cmd_group.protect is not None:
                protect_end_msg = '#endif // %s\n' % cur_ext_cmd_group.protect
                if not print_inst_header:
                    inst_ext_disp += protect_end_msg
                    lookup_inst_disp += protect_end_msg
                if not print_dev_header:
                    dev_ext_disp += protect_end_msg
                    lookup_dev_disp += protect_end_msg

        inst_core_disp += '}\n\n'
        inst_ext_disp += '}\n\n'
        dev_core_disp += '}\n\n'
        dev_ext_disp += '}\n\n'

        lookup_inst_disp += '\n'
        lookup_inst_disp += '    *found_name = false;\n'
        lookup_inst_disp += '    return NULL;\n'
        lookup_inst_disp += '}\n\n'

        lookup_dev_disp += '\n'
        lookup_dev_disp += '    return NULL;\n'
        lookup_dev_disp += '}\n\n'

        tables += inst_core_disp
        tables += inst_ext_disp
        tables += dev_core_disp
        tables += dev_ext_disp
        tables += lookup_inst_disp
        tables += lookup_dev_disp
        return tables

    # Output an entry for the current command to the LoaderDriverDispatchTable
    def OutputLoaderDriverEntry(self, command, required):
        entry = ''

        skip_gipa_commands = ['vkGetInstanceProcAddr',
                              'vkEnumerateDeviceLayerProperties',
                              'vkCreateInstance',
                              'vkEnumerateInstanceExtensionProperties',
                              'vkEnumerateInstanceLayerProperties',
                              'vkEnumerateInstanceVersion',
                             ]
        if command.name in skip_gipa_commands:
            return ''

        # Remove 'vk' from proto name
        base_name = command.name[2:]

        # The Core Vulkan code will be wrapped in a feature called VK_VERSION_#_#
        # For example: VK_VERSION_1_0 wraps the core 1.0 Vulkan functionality
        entry += '    LOOKUP_GIPA(%s, %s);\n' % (base_name, 'true' if required else 'false')

        return entry

    # Output an entry for the current command to the LoaderDriverDispatchTable
    def OutputLoaderDeviceProcEntry(self, extension_name, command, first):
        entry = ''

        # Remove 'vk' from proto name
        base_name = command.name[2:]

        if first:
            entry += '        if'
        else:
            entry += ' else if'

        entry += '(!strcmp(pName, "%s")) {\n' % (command.name)
        entry += '            addr = (PFN_vkVoidFunction)terminator_%s;\n' % (command.name[2:])

        entry += '        }'
        return entry

    # Creates code to output additional terminator functions
    def OutputLoaderTerminatorFunctions(self):
        add_funcs = ''

        icd_dis_init = '\n'
        icd_dis_init += 'VKAPI_ATTR bool VKAPI_CALL loader_icd_init_entries(struct loader_icd_term *icd_term, VkInstance inst,\n'
        icd_dis_init += '                                                   const PFN_vkGetInstanceProcAddr fp_gipa) {\n'
        icd_dis_init += '\n'
        icd_dis_init += '#define LOOKUP_GIPA(func, required)                                                        \\\n'
        icd_dis_init += '    do {                                                                                   \\\n'
        icd_dis_init += '        icd_term->dispatch.func = (PFN_vk##func)fp_gipa(inst, "vk" #func);                 \\\n'
        icd_dis_init += '        if (!icd_term->dispatch.func && required) {                                        \\\n'
        icd_dis_init += '            loader_log((struct loader_instance *)inst, VULKAN_LOADER_WARN_BIT, 0, \\\n'
        icd_dis_init += '                       loader_platform_get_proc_address_error("vk" #func));                \\\n'
        icd_dis_init += '            return false;                                                                  \\\n'
        icd_dis_init += '        }                                                                                  \\\n'
        icd_dis_init += '    } while (0)\n'

        dev_proc_term = ''
        dev_proc_term += '// Some device commands still need a terminator because the loader needs to unwrap something about them.\n'
        dev_proc_term += '// In many cases, the item needing unwrapping is a VkPhysicalDevice or VkSurfaceKHR object.  But there may be other items\n'
        dev_proc_term += '// in the future.\n'
        dev_proc_term += 'PFN_vkVoidFunction get_extension_device_proc_terminator(struct loader_device *dev, const char *pName) {\n'
        dev_proc_term += '    PFN_vkVoidFunction addr = NULL;\n'

        for cur_core_cmd_group in self.core_command_groups:
            print_icd_header = True
            print_dev_header = True
            dev_core_header = '\n    // ---- Vulkan %d.%d commands\n' % (cur_core_cmd_group.major, cur_core_cmd_group.minor)
            required = cur_core_cmd_group.major == 1 and cur_core_cmd_group.minor == 0
            for cur_cmd in cur_core_cmd_group.inst_cmds:

                if print_icd_header:
                    icd_dis_init += dev_core_header
                    print_icd_header = False

                icd_dis_init += self.OutputLoaderDriverEntry(cur_cmd, required)

            for cur_cmd in cur_core_cmd_group.dev_cmds:
                if cur_cmd.name in DEVICE_CMDS_NEEDING_TERMINATOR:
                    if print_dev_header:
                        dev_proc_term += dev_core_header
                        print_dev_header = False

                if (self.CommandIsInstanceType(cur_cmd.handle_type, '') or
                    cur_cmd.name in DEVICE_CMDS_NEEDING_TERMINATOR):

                    if print_icd_header:
                        dev_proc_term += dev_core_header
                        print_icd_header = False

                    icd_dis_init += self.OutputLoaderDriverEntry(cur_cmd, required)

        for cur_ext_cmd_group in self.ext_command_groups:
            if 'android' in cur_ext_cmd_group.ext_name.lower():
                continue

            print_icd_header = True
            print_dev_header = True
            first = True
            dev_ext_header = '\n    // ---- %s extension commands\n' % cur_ext_cmd_group.ext_name
            if cur_ext_cmd_group.needs_dev_term_override:
                dev_proc_term += dev_ext_header
                if cur_ext_cmd_group.protect is not None:
                    dev_proc_term += '#ifdef %s\n' % cur_ext_cmd_group.protect

                dev_proc_term += '    if (dev->dev_ext_enables.%s' % cur_ext_cmd_group.ext_name[3:].lower()

                # If this requires additional extensions, check the other extensions are also enabled, but only
                # if we also track those.
                if cur_ext_cmd_group.required_exts is not None:
                    for req_ext in cur_ext_cmd_group.required_exts:
                        if req_ext in self.dev_extensions_tracked_by_loader:
                            dev_proc_term += ' && dev->dev_ext_enables.%s' % req_ext[3:].lower()
                dev_proc_term += ') {\n'
            for cur_cmd in cur_ext_cmd_group.cmds:
                if (self.CommandIsInstanceType(cur_cmd.handle_type, cur_ext_cmd_group.ext_name) or
                    cur_cmd.name in DEVICE_CMDS_NEEDING_TERMINATOR):

                    if print_icd_header:
                        icd_dis_init += dev_ext_header
                        if cur_ext_cmd_group.protect is not None:
                            icd_dis_init += '#ifdef %s\n' % cur_ext_cmd_group.protect
                        print_icd_header = False

                    icd_dis_init += self.OutputLoaderDriverEntry(cur_cmd, False)

                    if cur_cmd.name in DEVICE_CMDS_NEEDING_TERMINATOR:
                        dev_proc_term += self.OutputLoaderDeviceProcEntry(cur_ext_cmd_group.ext_name, cur_cmd, first)
                        first = False

            if not print_icd_header and cur_ext_cmd_group.protect is not None:
                icd_dis_init += '#endif // %s\n' % cur_ext_cmd_group.protect

            if cur_ext_cmd_group.needs_dev_term_override:
                dev_proc_term += '\n    } // %s \n' % cur_ext_cmd_group.ext_name
                if cur_ext_cmd_group.protect is not None:
                    dev_proc_term += '#endif // %s\n' % cur_ext_cmd_group.protect

        icd_dis_init += '\n'
        icd_dis_init += '#undef LOOKUP_GIPA\n'
        icd_dis_init += '\n'
        icd_dis_init += '    return true;\n'
        icd_dis_init += '};\n\n'

        dev_proc_term += '\n    return addr;\n'
        dev_proc_term += '}\n\n'

        add_funcs += icd_dis_init
        add_funcs += dev_proc_term
        return add_funcs

    # Output an entry for the current command to the LoaderDriverDispatchTable
    def OutputLoaderInstanceTermDispatchEntry(self, command, core, required, is_define):
        entry = ''

        skip_gipa_commands = ['vkEnumerateInstanceExtensionProperties',
                              'vkEnumerateInstanceLayerProperties',
                              'vkEnumerateInstanceVersion',
                             ]
        if not is_define and command.name in skip_gipa_commands:
            return ''

        # Remove 'vk' from proto name
        base_name = command.name[2:]
        aliased_name = base_name
        if not core and command.alias is not None:
            aliased_name = command.alias[2:]

        if is_define:
            entry += '    PFN_%s %s;\n' % (command.name, base_name)
        else:
            if base_name == 'GetInstanceProcAddr':
                entry += '    .%s = %s,\n' % (base_name, command.name)
            else:
                entry += '    .%s = terminator_%s,\n' % (base_name, aliased_name)
        return entry

    # Create a layer instance dispatch table from the appropriate list and return it as a string
    def OutputLayerInstanceTermDispatchTable(self, is_definition):
        table = ''
        table += '\n'

        if is_definition:
            table += 'typedef PFN_vkVoidFunction (VKAPI_PTR *PFN_GetPhysicalDeviceProcAddr)(VkInstance instance, const char* pName);\n'
            table += '\n'
            table += '// Instance function pointer dispatch table\n'
            table += 'typedef struct VkLayerInstanceDispatchTable_ {\n'

            # First add in an entry for GetPhysicalDeviceProcAddr.  This will not
            # ever show up in the XML or header, so we have to manually add it.
            table += '    // Manually add in GetPhysicalDeviceProcAddr entry\n'
            table += '    PFN_GetPhysicalDeviceProcAddr GetPhysicalDeviceProcAddr;\n'
        else:
            table += '// This table contains the loader\'s instance dispatch table, which contains\n'
            table += '// default functions if no instance layers are activated.  This contains\n'
            table += '// pointers to "terminator functions".\n'
            table += 'const VkLayerInstanceDispatchTable instance_term_disp = {\n'

        for cur_core_cmd_group in self.core_command_groups:
            print_header = True
            required = cur_core_cmd_group.major == 1 and cur_core_cmd_group.minor == 0
            for cur_cmd in cur_core_cmd_group.inst_cmds:
                if print_header:
                    table += '\n    // ---- Vulkan %d.%d commands\n' % (cur_core_cmd_group.major, cur_core_cmd_group.minor)
                    print_header = False
                table += self.OutputLoaderInstanceTermDispatchEntry(cur_cmd, True, required, is_definition)

        for cur_ext_cmd_group in self.ext_command_groups:
            print_inst_header = True
            if not cur_ext_cmd_group.has_inst:
                continue

            for cur_cmd in cur_ext_cmd_group.cmds:
                if self.CommandIsInstanceType(cur_cmd.handle_type, cur_ext_cmd_group.ext_name):
                    if print_inst_header:
                        table += '\n'
                        if cur_ext_cmd_group.protect is not None:
                            table += '    // ---- %s extension commands\n' % cur_ext_cmd_group.ext_name
                            table += '#ifdef %s\n' % cur_ext_cmd_group.protect
                        else:
                            table += '    // ---- %s extension commands\n' % cur_ext_cmd_group.ext_name
                        print_inst_header = False

                    table += self.OutputLoaderInstanceTermDispatchEntry(cur_cmd, False, False, is_definition)
                else:
                    if 'instance' == cur_ext_cmd_group.ext_type and self.CommandIsInstanceType(cur_cmd.handle_type, cur_ext_cmd_group.ext_name):
                        if print_inst_header:
                            table += '\n'
                            if cur_ext_cmd_group.protect is not None:
                                table += '    // ---- %s extension commands\n' % cur_ext_cmd_group.ext_name
                                table += '#ifdef %s\n' % cur_ext_cmd_group.protect
                            else:
                                table += '    // ---- %s extension commands\n' % cur_ext_cmd_group.ext_name
                            print_inst_header = False

                        table += self.OutputLoaderInstanceTermDispatchEntry(cur_cmd, False, False, is_definition)
            if cur_ext_cmd_group.protect is not None:
                table += '#endif // %s\n' % cur_ext_cmd_group.protect

        if is_definition:
            table += '} VkLayerInstanceDispatchTable;\n\n'
        else:
            table += '};\n\n'
        return table

    # Create a layer device dispatch table from the appropriate list and return it as a string
    def OutputLayerDeviceTermDispatchTable(self):
        table = ''
        table += '// Device function pointer dispatch table\n'
        table += '#define DEVICE_DISP_TABLE_MAGIC_NUMBER 0x10ADED040410ADEDUL\n'
        table += 'typedef struct VkLayerDispatchTable_ {\n'
        table += '    uint64_t magic; // Should be DEVICE_DISP_TABLE_MAGIC_NUMBER\n'

        for cur_core_cmd_group in self.core_command_groups:
            table += '\n    // ---- Vulkan %d.%d commands\n' % (cur_core_cmd_group.major, cur_core_cmd_group.minor)
            required = cur_core_cmd_group.major == 1 and cur_core_cmd_group.minor == 0
            for cur_cmd in cur_core_cmd_group.dev_cmds:
                table += self.OutputLoaderInstanceTermDispatchEntry(cur_cmd, True, required, True)

        for cur_ext_cmd_group in self.ext_command_groups:
            print_header = True

            if ('instance' == cur_ext_cmd_group.ext_type and 'VK_EXT_debug_utils' != cur_ext_cmd_group.ext_name) or len(cur_ext_cmd_group.cmds) == 0:
                continue

            for cur_cmd in cur_ext_cmd_group.cmds:
                if cur_cmd.handle_type == 'VkInstance' or cur_cmd.handle_type == 'VkPhysicalDevice':
                    continue

                if print_header:
                    table += '\n'
                    table += '    // ---- %s extension commands\n' % cur_ext_cmd_group.ext_name
                    if cur_ext_cmd_group.protect is not None:
                        table += '#ifdef %s\n' % cur_ext_cmd_group.protect
                    print_header = False

                table += self.OutputLoaderInstanceTermDispatchEntry(cur_cmd, False, False, True)

            if not print_header and cur_ext_cmd_group.protect is not None:
                table += '#endif // %s\n' % cur_ext_cmd_group.protect

        table += '} VkLayerDispatchTable;\n\n'
        return table
