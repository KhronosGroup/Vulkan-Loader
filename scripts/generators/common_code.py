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
import re
from vulkan_object import Version

ADD_INST_CMDS = ['vkCreateInstance',
                 'vkEnumerateInstanceExtensionProperties',
                 'vkEnumerateInstanceLayerProperties',
                 'vkEnumerateInstanceVersion']

DISPATCHABLE_TYPES = ['VkInstance', 'VkPhysicalDevice', 'VkDevice', 'VkQueue', 'VkCommandBuffer']

AVOID_EXT_NAMES = ['VK_EXT_debug_report']

AVOID_CMD_NAMES = ['vkCreateDebugUtilsMessengerEXT',
                   'vkDestroyDebugUtilsMessengerEXT',
                   'vkSubmitDebugUtilsMessageEXT']

WSI_EXT_NAMES = ['VK_KHR_surface',
                 'VK_KHR_display',
                 'VK_KHR_xlib_surface',
                 'VK_KHR_xcb_surface',
                 'VK_KHR_wayland_surface',
                 'VK_EXT_directfb_surface',
                 'VK_KHR_win32_surface',
                 'VK_KHR_android_surface',
                 'VK_GGP_stream_descriptor_surface',
                 'VK_MVK_macos_surface',
                 'VK_MVK_ios_surface',
                 'VK_EXT_headless_surface',
                 'VK_EXT_metal_surface',
                 'VK_FUCHSIA_imagepipe_surface',
                 'VK_KHR_swapchain',
                 'VK_KHR_display_swapchain',
                 'VK_KHR_get_display_properties2',
                 'VK_KHR_get_surface_capabilities2',
                 'VK_QNX_screen_surface',
                 'VK_NN_vi_surface',
                 'VK_SEC_ubm_surface',
                 ]

def _read_max_loader_string_length_from_c_header() -> int:
    header_path = os.path.join(os.path.dirname(__file__), '..', '..', 'loader', 'loader_common.h')
    with open(header_path, 'r', encoding='utf-8') as f:
        header_text = f.read()
    match = re.search(r'^\s*static const int MaxLoaderStringLength\s*=\s*(\d+)\s*;', header_text, re.MULTILINE)
    assert match, (
        f'Could not find "static const int MaxLoaderStringLength = <N>;" in {header_path}. '
        'loader_hash_string() below needs this bound to stay in sync with loader/loader_common.h - '
        'did that declaration get renamed or reformatted?')
    return int(match.group(1))

# Must match MaxLoaderStringLength in loader/loader_common.h - loader_hash_string() there stops scanning
# after this many bytes (a name of exactly this length is scanned in full and hashes the same either way;
# only a name LONGER than this would hash differently in C than in Python below). Parsed directly out of
# the header (rather than hand-copied) so the two can never silently drift apart.
MAX_LOADER_STRING_LENGTH = _read_max_loader_string_length_from_c_header()

# 32-bit FNV-1a (https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function). Must stay
# byte-for-byte identical to loader_hash_string() in loader/loader_common.h - this precomputes the hash
# constants baked into the generated *_gpa()/dispatch-table-lookup functions below, which the C function
# then has to reproduce at runtime to hit the right branch. HashString.MatchesGeneratorEmbeddedConstants in
# tests/loader_hash_string_tests.cpp catches any divergence.
def loader_hash_string(s: str) -> int:
    assert len(s) <= MAX_LOADER_STRING_LENGTH, (
        f'"{s}" is {len(s)} bytes, over MaxLoaderStringLength ({MAX_LOADER_STRING_LENGTH}). '
        'loader_hash_string() in loader/loader_common.h truncates its scan at that length, so this name '
        "would hash differently at runtime than the constant this generator is about to embed for it - "
        'shorten the name or raise MaxLoaderStringLength in both the C and Python copies.')
    h = 0x811c9dc5
    for c in s.encode('ascii'):
        h ^= c
        h = (h * 0x01000193) & 0xFFFFFFFF
    return h

# Raises if any two names in `names` share a hash - used as a generation-time tripwire so that if the
# Vulkan command set ever grows into a collision, it's caught immediately with a clear message instead of
# failing to compile with an inscrutable "duplicate case value" error naming neither colliding string (each
# name in `names` becomes its own `case <hash>u:` label in the generated switch - see OutputLoaderLookupFunc
# and InstExtensionGPA below).
def check_no_hash_collisions(names, context: str):
    seen = {}
    for name in names:
        h = loader_hash_string(name)
        if h in seen and seen[h] != name:
            raise RuntimeError(
                f'FNV-1a hash collision in {context}: "{seen[h]}" and "{name}" both hash to {h:#010x}. '
                'Left unaddressed, this would make the generated switch emit two identical case labels and '
                'fail to compile with a "duplicate case value" error naming neither string - '
                'loader_hash_string() (C and Python copies) should be revisited (e.g. a different seed/prime).')
        seen[h] = name

# This class is a container for any source code, data, or other behavior that is necessary to
# customize the generator script for a specific target API variant (e.g. Vulkan SC). As such,
# all of these API-specific interfaces and their use in the generator script are part of the
# contract between this repository and its downstream users. Changing or removing any of these
# interfaces or their use in the generator script will have downstream effects and thus
# should be avoided unless absolutely necessary.
class APISpecific:
    @staticmethod
    def getApiVersionDisplayName(targetApiName: str, name: str) -> str:
        match targetApiName:
            case 'vulkan':
                api_name = 'Vulkan'

        version_number = name[name.find('_VERSION_') + len('_VERSION_'):].replace('_', '.')
        return f'{api_name} {version_number}'

    @staticmethod
    def getFirstVersionName(targetApiName: str) -> str:
        match targetApiName:
            case 'vulkan':
                return 'VK_VERSION_1_0'

    # This handles the dependency across versions of APIs, even across API variants
    # (e.g. maps VK_VERSION_1_2 to VKSC_VERSION_1_0 for Vulkan SC).
    # This handles the lack of proper support for the "depends" attribute of <feature> tags in the XML.
    @staticmethod
    def getEffectiveVersionName(targetApiName: str, version: Version) -> str:
        if version is None:
            return APISpecific.getFirstVersionName(targetApiName)

        match targetApiName:
            case 'vulkan':
                return version.name
