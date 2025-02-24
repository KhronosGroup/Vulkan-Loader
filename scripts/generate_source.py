#!/usr/bin/env python3
# Copyright (c) 2019 The Khronos Group Inc.
# Copyright (c) 2019 Valve Corporation
# Copyright (c) 2019 LunarG, Inc.
# Copyright (c) 2019 Google Inc.
# Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
# Author: Mike Schuchardt <mikes@lunarg.com>

import argparse
import common_codegen
import filecmp
import os
import shutil
import sys
import tempfile
import datetime
import re

from xml.etree import ElementTree

def RunGenerators(api: str, registry: str, directory: str, targetFilter: str, caching: bool):

    # These live in the Vulkan-Docs repo, but are pulled in via the
    # Vulkan-Headers/registry folder
    # At runtime we inject python path to find these helper scripts
    scripts = os.path.dirname(registry)
    scripts_directory_path = os.path.dirname(os.path.abspath(__file__))
    registry_headers_path = os.path.join(scripts_directory_path, scripts)
    sys.path.insert(0, registry_headers_path)
    try:
        from reg import Registry
    except:
        print("ModuleNotFoundError: No module named 'reg'") # normal python error message
        print(f'{registry_headers_path} is not pointing to the Vulkan-Headers registry directory.')
        print("Inside Vulkan-Headers there is a registry/reg.py file that is used.")
        sys.exit(1) # Return without call stack so easy to spot error

    from base_generator import BaseGeneratorOptions
    from dispatch_table_helper_generator import DispatchTableHelperGenerator
    from helper_file_generator import HelperFileGenerator
    from loader_extension_generator import LoaderExtensionGenerator

    # These set fields that are needed by both OutputGenerator and BaseGenerator,
    # but are uniform and don't need to be set at a per-generated file level
    from base_generator import (SetTargetApiName, SetMergedApiNames)
    SetTargetApiName(api)

    generators = {
        'vk_layer_dispatch_table.h': {
            'generator' : LoaderExtensionGenerator,
            'genCombined': True,
            'directory' : 'loader/generated',
        },
        'vk_loader_extensions.c': {
            'generator' : LoaderExtensionGenerator,
            'genCombined': True,
            'directory' : 'loader/generated',
        },
        'vk_loader_extensions.h': {
            'generator' : LoaderExtensionGenerator,
            'genCombined': True,
            'directory' : 'loader/generated',
        },
        'vk_object_types.h': {
            'generator' : HelperFileGenerator,
            'genCombined': True,
            'directory' : 'loader/generated',
        },
        'vk_dispatch_table_helper.h': {
            'generator' : DispatchTableHelperGenerator,
            'genCombined': True,
            'directory' : 'tests/framework/layer',
        }
    }

    unknownTargets = [x for x in (targetFilter if targetFilter else []) if x not in generators.keys()]
    if unknownTargets:
        print(f'ERROR: No generator options for unknown target(s): {", ".join(unknownTargets)}', file=sys.stderr)
        return 1

    # Filter if --target is passed in
    targets = [x for x in generators.keys() if not targetFilter or x in targetFilter]

    for index, target in enumerate(targets, start=1):
        print(f'[{index}|{len(targets)}] Generating {target}')

        # First grab a class contructor object and create an instance
        generator = generators[target]['generator']
        gen = generator()

        # This code and the 'genCombined' generator metadata is used by downstream
        # users to generate code with all Vulkan APIs merged into the target API variant
        # (e.g. Vulkan SC) when needed. The constructed apiList is also used to filter
        # out non-applicable extensions later below.
        apiList = [api]
        if api != 'vulkan' and generators[target]['genCombined']:
            SetMergedApiNames('vulkan')
            apiList.append('vulkan')
        else:
            SetMergedApiNames(None)

        outDirectory = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', generators[target]['directory']))
        options = BaseGeneratorOptions(
            customFileName  = target,
            customDirectory = outDirectory)

        # Create the registry object with the specified generator and generator
        # options. The options are set before XML loading as they may affect it.
        reg = Registry(gen, options)

        # Parse the specified registry XML into an ElementTree object
        tree = ElementTree.parse(registry)

        # Filter out extensions that are not on the API list
        [exts.remove(e) for exts in tree.findall('extensions') for e in exts.findall('extension') if (sup := e.get('supported')) is not None and all(api not in sup.split(',') for api in apiList)]

        # Load the XML tree into the registry object
        reg.loadElementTree(tree)

        # Finally, use the output generator to create the requested target
        reg.apiGen()


def main(argv):
    parser = argparse.ArgumentParser(description='Generate source code for this repository')
    parser.add_argument('registry', metavar='REGISTRY_PATH', help='path to the Vulkan-Headers registry directory')
    parser.add_argument('--api',
                        default='vulkan',
                        choices=['vulkan'],
                        help='Specify API name to generate')
    parser.add_argument('--generated-version', help='sets the header version used to generate the repo')
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--target', nargs='+', help='only generate file names passed in')
    group.add_argument('-i', '--incremental', action='store_true', help='only update repo files that change')
    group.add_argument('-v', '--verify', action='store_true', help='verify repo files match generator output')
    group.add_argument('--no-caching', action='store_true', help='Do not try to cache generator objects')
    args = parser.parse_args(argv)

    repo_dir = common_codegen.repo_relative(f'loader/{args.api}/generated')

    registry = os.path.abspath(os.path.join(args.registry,  'vk.xml'))
    if not os.path.isfile(registry):
        registry = os.path.abspath(os.path.join(args.registry, 'Vulkan-Headers/registry/vk.xml'))
        if not os.path.isfile(registry):
            print(f'cannot find vk.xml in {args.registry}')
            return -1


    repo_dir = common_codegen.repo_relative('loader/generated')

    # get directory where generators will run
    if args.verify or args.incremental:
        # generate in temp directory so we can compare or copy later
        temp_obj = tempfile.TemporaryDirectory(prefix='loader_codegen_')
        temp_dir = temp_obj.name
        gen_dir = temp_dir
    else:
        # generate directly in the repo
        gen_dir = repo_dir

    caching = not args.no_caching
    RunGenerators(args.api, registry, gen_dir, args.target, caching)

    # optional post-generation steps
    if args.verify:
        # compare contents of temp dir and repo
        temp_files = set(os.listdir(temp_dir))
        repo_files = set(os.listdir(repo_dir))
        files_match = True
        for filename in sorted((temp_files | repo_files)):
            if filename not in repo_files:
                print('ERROR: Missing repo file', filename)
                files_match = False
            elif filename not in temp_files:
                print('ERROR: Missing generator for', filename)
                files_match = False
            elif not filecmp.cmp(os.path.join(temp_dir, filename),
                               os.path.join(repo_dir, filename),
                               shallow=False):
                print('ERROR: Repo files do not match generator output for', filename)
                files_match = False

        # return code for test scripts
        if files_match:
            print('SUCCESS: Repo files match generator output')
            return 0
        return 1

    elif args.incremental:
        # copy missing or differing files from temp directory to repo
        for filename in os.listdir(temp_dir):
            temp_filename = os.path.join(temp_dir, filename)
            repo_filename = os.path.join(repo_dir, filename)
            if not os.path.exists(repo_filename) or \
               not filecmp.cmp(temp_filename, repo_filename, shallow=False):
                print('update', repo_filename)
                shutil.copyfile(temp_filename, repo_filename)

    # write out the header version used to generate the code to a checked in CMake file
    if args.generated_version:
        # Update the CMake project version
        with open(common_codegen.repo_relative('CMakeLists.txt'), "r+") as f:
            data = f.read()
            f.seek(0)
            f.write(re.sub("project.*VERSION.*", f"project(VULKAN_LOADER VERSION {args.generated_version} LANGUAGES C)", data))
            f.truncate()

        with open(common_codegen.repo_relative('loader/loader.rc.in'), "r") as rc_file:
            rc_file_contents = rc_file.read()
        rc_ver = ', '.join(args.generated_version.split('.') + ['0'])
        rc_file_contents = rc_file_contents.replace('${LOADER_VER_FILE_VERSION}', f'{rc_ver}')
        rc_file_contents = rc_file_contents.replace('${LOADER_VER_FILE_DESCRIPTION_STR}', f'"{args.generated_version}.Dev Build"')
        rc_file_contents = rc_file_contents.replace('${LOADER_VER_FILE_VERSION_STR}', f'"Vulkan Loader - Dev Build"')
        rc_file_contents = rc_file_contents.replace('${LOADER_CUR_COPYRIGHT_YEAR}', f'{datetime.date.today().year}')
        with open(common_codegen.repo_relative('loader/loader.rc'), "w") as rc_file_out:
            rc_file_out.write(rc_file_contents)
            rc_file_out.close()

    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))

