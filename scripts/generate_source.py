#!/usr/bin/env python3
# Copyright (c) 2019 The Khronos Group Inc.
# Copyright (c) 2019 Valve Corporation
# Copyright (c) 2019 LunarG, Inc.
# Copyright (c) 2019 Google Inc.
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
import subprocess
import sys
import tempfile

# files to exclude from --verify check
verify_exclude = ['.clang-format']

def main(argv):
    parser = argparse.ArgumentParser(description='Generate source code for this repository')
    parser.add_argument('registry', metavar='REGISTRY_PATH', help='path to the Vulkan-Headers registry directory')
    group = parser.add_mutually_exclusive_group()
    group.add_argument('-i', '--incremental', action='store_true', help='only update repo files that change')
    group.add_argument('-v', '--verify', action='store_true', help='verify repo files match generator output')
    args = parser.parse_args(argv)

    gen_cmds = [[common_codegen.repo_relative('scripts/loader_genvk.py'),
                 '-registry', os.path.abspath(os.path.join(args.registry,  'vk.xml')),
                 '-quiet',
                 filename] for filename in ['vk_dispatch_table_helper.h',
                                            'vk_loader_extension_utils.h',
                                            'vk_loader_extension_utils.c',
                                            'vk_loader_trampolines.h',
                                            'vk_loader_trampolines.c',
                                            'vk_loader_terminators.h',
                                            'vk_loader_terminators.c',
                                            'vk_layer_dispatch_table.h',
                                            'vk_object_types.h',
                                            'vk_test_entrypoint_layer.h',
                                            'vk_test_entrypoint_layer.cpp',
                                            'vk_test_entrypoint_driver.h',
                                            'vk_test_entrypoint_driver.cpp',
                                            'vk_test_entrypoint_core_tests.cpp',
                                            'vk_test_entrypoint_extension_tests.cpp',
                                            'loader_generated_header_version.cmake']]

    loader_repo_dir = common_codegen.repo_relative('loader/generated')
    test_repo_dir = common_codegen.repo_relative('tests/generated')
    gen_dir = loader_repo_dir
    test_gen_dir = test_repo_dir

    # get directory where generators will run
    if args.verify or args.incremental:
        # generate in temp directory so we can compare or copy later
        temp_obj = tempfile.TemporaryDirectory(prefix='VulkanLoader_generated_source_')
        temp_dir = temp_obj.name
        gen_dir = temp_dir
        test_gen_dir = temp_dir

    # run each code generator
    for cmd in gen_cmds:
        try:
            out_dir = gen_dir
            if 'vk_test_entrypoint' in cmd[4]:
                out_dir = test_gen_dir

            print(' '.join(cmd), ' -> ', out_dir)

            subprocess.check_call([sys.executable] + cmd,
                                  # ignore generator output, vk_validation_stats.py is especially noisy
                                  stdout=subprocess.DEVNULL,
                                  cwd=gen_dir)
        except Exception as e:
            print('ERROR:', str(e))
            return 1

    # optional post-generation steps
    if args.verify:
        # compare contents of temp dir and repo
        temp_files = set(os.listdir(temp_dir))
        loader_repo_files = set(os.listdir(loader_repo_dir))
        test_repo_files = set(os.listdir(test_repo_dir))

        search_folders = loader_repo_files | test_repo_files
        if args.verify or args.incremental:
            search_folders = temp_files | search_folders
        search_folders = search_folders - set(verify_exclude)

        files_match = True
        for filename in sorted(search_folders):
            is_test_file = ('vk_test_entrypoint' in filename)
            compare_repo_path = loader_repo_dir
            compare_repo_files = loader_repo_files
            if is_test_file:
                compare_repo_path = test_repo_dir
                compare_repo_files = test_repo_files
            if filename not in compare_repo_files:
                print('ERROR: Missing repo file', filename)
                files_match = False
            elif filename not in temp_files:
                print('ERROR: Missing generator for', filename)
                files_match = False
            else:
                orig_file = os.path.join(compare_repo_path, filename)
                orig_size = os.stat(orig_file)
                temp_file = os.path.join(temp_dir, filename)
                temp_size = os.stat(temp_file)
                print('Comparing %s to %s' % (temp_file, orig_file))
                if not filecmp.cmp(temp_file, orig_file, shallow=False):
                    print('ERROR: Repo files do not match generator output for', filename)
                    print('          orig %s (size %d)'%(orig_file, orig_size.st_size))
                    print('          temp %s (size %d)' %(temp_file, temp_size.st_size))
                    files_match = False

        # return code for test scripts
        if files_match:
            print('SUCCESS: Repo files match generator output')
            return 0
        return 1

    elif args.incremental:
        # copy missing or differing files from temp directory to repo
        for filename in os.listdir(temp_dir):
            target_dir = loader_repo_dir
            if 'vk_test_entrypoint' in filename:
                target_dir = test_repo_dir
            temp_filename = os.path.join(temp_dir, filename)
            repo_filename = os.path.join(target_dir, filename)
            if not os.path.exists(repo_filename) or \
               not filecmp.cmp(temp_filename, repo_filename, shallow=False):
                print('---Updating ', repo_filename)
                shutil.copyfile(temp_filename, repo_filename)

    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))

