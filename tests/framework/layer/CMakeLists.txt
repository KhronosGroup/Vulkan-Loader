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

add_library(test_layer_deps INTERFACE)

target_compile_options(test_layer_deps  INTERFACE
    $<$<PLATFORM_ID:WIN32>:${MSVC_LOADER_COMPILE_OPTIONS},/permissive->)

target_compile_definitions(test_layer_deps  INTERFACE
    # Workaround for TR1 deprecation in Visual Studio 15.5 until Google Test is updated
    $<$<PLATFORM_ID:WIN32>:-DWIN32_LEAN_AND_MEAN,-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING>
    VK_NO_PROTOTYPES)

target_link_libraries(test_layer_deps INTERFACE testing_framework_util)

set(TEST_LAYER_SOURCES test_layer.cpp test_layer.h)

set(TEST_LAYER_BASE_EXPORTS TEST_LAYER_EXPORT_ENUMERATE_FUNCTIONS=1)
set(TEST_LAYER_VERSION_0_EXPORTS TEST_LAYER_EXPORT_LAYER_NAMED_GIPA=1 TEST_LAYER_EXPORT_LAYER_NAMED_GDPA=1 ${TEST_LAYER_BASE_EXPORTS})
set(TEST_LAYER_VERSION_1_EXPORTS TEST_LAYER_EXPORT_NO_PREFIX_GIPA=1 TEST_LAYER_EXPORT_NO_PREFIX_GDPA=1 ${TEST_LAYER_BASE_EXPORTS})
set(TEST_LAYER_VERSION_2_EXPORTS TEST_LAYER_EXPORT_GET_PHYSICAL_DEVICE_PROC_ADDR=1
    LAYER_EXPORT_NEGOTIATE_LOADER_LAYER_INTERFACE_VERSION=1 ${TEST_LAYER_BASE_EXPORTS})

add_library(test_layer_export_base SHARED ${TEST_LAYER_SOURCES})
target_link_libraries(test_layer_export_base PRIVATE test_layer_deps)
target_compile_definitions(test_layer_export_base PRIVATE ${TEST_LAYER_BASE_EXPORTS})

add_library(test_layer_export_version_0 SHARED ${TEST_LAYER_SOURCES})
target_link_libraries(test_layer_export_version_0 PRIVATE test_layer_deps)
target_compile_definitions(test_layer_export_version_0 PRIVATE ${TEST_LAYER_VERSION_0_EXPORTS})

add_library(test_layer_export_version_1 SHARED ${TEST_LAYER_SOURCES})
target_link_libraries(test_layer_export_version_1 PRIVATE test_layer_deps)
target_compile_definitions(test_layer_export_version_1 PRIVATE
    ${TEST_LAYER_VERSION_0_EXPORTS} ${TEST_LAYER_VERSION_1_EXPORTS})

add_library(test_layer_export_version_2 SHARED ${TEST_LAYER_SOURCES})
target_link_libraries(test_layer_export_version_2 PRIVATE test_layer_deps)
target_compile_definitions(test_layer_export_version_2 PRIVATE
    ${TEST_LAYER_VERSION_0_EXPORTS} ${TEST_LAYER_VERSION_1_EXPORTS} ${TEST_LAYER_VERSION_2_EXPORTS} )

add_library(test_layer_wrap_objects SHARED wrap_objects.cpp)
target_link_libraries(test_layer_wrap_objects PRIVATE test_layer_deps)

if(WIN32)
    target_sources(test_layer_export_base PRIVATE export_definitions/test_layer_base.def)
    target_sources(test_layer_export_version_0 PRIVATE export_definitions/test_layer_0.def)
    target_sources(test_layer_export_version_1 PRIVATE export_definitions/test_layer_1.def)
    target_sources(test_layer_export_version_2 PRIVATE export_definitions/test_layer_2.def)
    target_sources(test_layer_wrap_objects PRIVATE export_definitions/test_layer_wrap_objects.def)
endif()