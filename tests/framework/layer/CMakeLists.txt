# ~~~
# Copyright (c) 2021 Valve Corporation
# Copyright (c) 2021 LunarG, Inc.
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
# ~~~

add_library(test_layer_util INTERFACE)
target_include_directories(test_layer_util INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})
target_link_libraries(test_layer_util INTERFACE testing_framework_util)

add_library(test_layer_deps INTERFACE)

target_compile_options(test_layer_deps  INTERFACE
    $<$<PLATFORM_ID:WIN32>:${MSVC_LOADER_COMPILE_OPTIONS},/permissive->)

target_compile_definitions(test_layer_deps  INTERFACE
# Workaround for TR1 deprecation in Visual Studio 15.5 until Google Test is updated
    $<$<PLATFORM_ID:WIN32>:-DWIN32_LEAN_AND_MEAN,-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING>
    VK_NO_PROTOTYPES)

target_link_libraries(test_layer_deps INTERFACE Vulkan::Headers testing_framework_util test_layer_util)