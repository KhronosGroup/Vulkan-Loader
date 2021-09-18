#!/usr/bin/python3 -i
#
# Copyright (c) 2021 The Khronos Group Inc.
# Copyright (c) 2021 Valve Corporation
# Copyright (c) 2021 LunarG, Inc.
# Copyright (c) 2021 Google Inc.

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

import os,re,sys
import xml.etree.ElementTree as etree
from generator import *
from collections import namedtuple
from common_codegen import *

#
# LoaderVersioningGeneratorOptions - subclass of GeneratorOptions.
class LoaderVersioningGeneratorOptions(GeneratorOptions):
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
                 alignFuncParam = 0,
                 library_name = '',
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
        self.prefixText       = prefixText
        self.genFuncPointers  = genFuncPointers
        self.protectFile      = protectFile
        self.protectFeature   = protectFeature
        self.apicall          = apicall
        self.apientry         = apientry
        self.apientryp        = apientryp
        self.alignFuncParam   = alignFuncParam
        self.library_name     = library_name
#
# LoaderVersioningGenerator - subclass of OutputGenerator. Outputs cmake file containing vulkan version used when generating files
class LoaderVersioningGenerator(OutputGenerator):
    """Generate helper file based on XML element attributes"""
    def __init__(self,
                 errFile = sys.stderr,
                 warnFile = sys.stderr,
                 diagFile = sys.stdout):
        OutputGenerator.__init__(self, errFile, warnFile, diagFile)

    #
    # Called once at the beginning of each run
    def beginFile(self, genOpts):
        OutputGenerator.beginFile(self, genOpts)
        # User-supplied prefix text, if any (list of strings)
        self.library_name = genOpts.library_name

        version_major = ''
        version_minor = ''
        version_patch = ''
        for elem in self.registry.reg.find('types').findall('type'):
            if elem.get('category') == 'define':
                if elem.get('name') == 'VK_HEADER_VERSION_COMPLETE':
                    # Parses the following string:
                    #define <name>VK_HEADER_VERSION_COMPLETE</name> <type>VK_MAKE_API_VERSION</type>(0, 1, 2, VK_HEADER_VERSION)</type>
                    # The 0th index is the VARIANT version, 1st & 2nd are the Major & Minor
                    version_major = re.findall("[0-9]+", ''.join(elem.itertext()))[1]
                    version_minor = re.findall("[0-9]+", ''.join(elem.itertext()))[2]
                if elem.get('name') == 'VK_HEADER_VERSION':
                    # Parses the following string:
                    #define <name>VK_HEADER_VERSION</name> 189</type>
                    version_patch = re.findall("[0-9]+", ''.join(elem.itertext()))[0]

        # File Comment
        file_comment = '# *** THIS FILE IS GENERATED - DO NOT EDIT ***\n'
        file_comment += '# See loader_versioning_generator.py for modifications\n'
        write(file_comment, file=self.outFile)
        # Copyright Notice
        cmake_version_copyright = '''############################################################################
#
# Copyright (c) 2021 The Khronos Group Inc.
# Copyright (c) 2021 Valve Corporation
# Copyright (c) 2021 LunarG, Inc.
# Copyright (c) 2021 Google Inc.
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
#
############################################################################
'''
        write(cmake_version_copyright, file=self.outFile)
        write(f'set(LOADER_GENERATED_HEADER_VERSION \"{version_major}.{version_minor}.{version_patch}\")', file=self.outFile)

    #
    # Write generated file content to output file
    def endFile(self):
        dest_file = ''
        # Remove blank lines at EOF
        if dest_file.endswith('\n'):
            dest_file = dest_file[:-1]
        write(dest_file, file=self.outFile)
        # Finish processing in superclass
        OutputGenerator.endFile(self)