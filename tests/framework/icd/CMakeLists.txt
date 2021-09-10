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

set(CMAKE_CXX_STANDARD 11)

add_library(test_icd_deps INTERFACE)

target_compile_options(test_icd_deps INTERFACE
    $<$<PLATFORM_ID:WIN32>:${MSVC_LOADER_COMPILE_OPTIONS},/permissive->)

target_compile_definitions(test_icd_deps INTERFACE
# Workaround for TR1 deprecation in Visual Studio 15.5 until Google Test is updated
    $<$<PLATFORM_ID:WIN32>:-DWIN32_LEAN_AND_MEAN,-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING>
    VK_NO_PROTOTYPES)

target_link_libraries(test_icd_deps INTERFACE testing_framework_util)

set(TEST_ICD_SOURCES test_icd.cpp test_icd.h)

# test icd without any config macros defined
add_library(test_icd_export_none SHARED ${TEST_ICD_SOURCES})
target_link_libraries(test_icd_export_none PRIVATE test_icd_deps)

add_library(test_icd_export_icd_gipa SHARED ${TEST_ICD_SOURCES})
target_link_libraries(test_icd_export_icd_gipa PRIVATE test_icd_deps)
target_compile_definitions(test_icd_export_icd_gipa PRIVATE TEST_ICD_EXPORT_ICD_GIPA=1)

add_library(test_icd_export_negotiate_interface_version SHARED ${TEST_ICD_SOURCES})
target_link_libraries(test_icd_export_negotiate_interface_version PRIVATE test_icd_deps)
target_compile_definitions(test_icd_export_negotiate_interface_version PRIVATE TEST_ICD_EXPORT_NEGOTIATE_INTERFACE_VERSION=1)

set(TEST_ICD_VERSION_2_DEFINES TEST_ICD_EXPORT_NEGOTIATE_INTERFACE_VERSION=1 TEST_ICD_EXPORT_ICD_GIPA=1)

add_library(test_icd_version_2 SHARED ${TEST_ICD_SOURCES})
target_link_libraries(test_icd_version_2 PRIVATE test_icd_deps)
target_compile_definitions(test_icd_version_2 PRIVATE ${TEST_ICD_VERSION_2_DEFINES})

add_library(test_icd_version_2_export_icd_enumerate_adapter_physical_devices SHARED ${TEST_ICD_SOURCES})
target_link_libraries(test_icd_version_2_export_icd_enumerate_adapter_physical_devices PRIVATE test_icd_deps)
target_compile_definitions(test_icd_version_2_export_icd_enumerate_adapter_physical_devices PRIVATE TEST_ICD_EXPORT_ICD_ENUMERATE_ADAPTER_PHYSICAL_DEVICES=1 ${TEST_ICD_VERSION_2_DEFINES})

add_library(test_icd_version_2_export_icd_gpdpa SHARED ${TEST_ICD_SOURCES})
target_link_libraries(test_icd_version_2_export_icd_gpdpa PRIVATE test_icd_deps)
target_compile_definitions(test_icd_version_2_export_icd_gpdpa PRIVATE TEST_ICD_EXPORT_ICD_GPDPA=1 ${TEST_ICD_VERSION_2_DEFINES})

add_library(test_icd_version_6 SHARED ${TEST_ICD_SOURCES})
target_link_libraries(test_icd_version_6 PRIVATE test_icd_deps)
target_compile_definitions(test_icd_version_6 PRIVATE TEST_ICD_EXPORT_ICD_GPDPA=1 TEST_ICD_EXPORT_ICD_ENUMERATE_ADAPTER_PHYSICAL_DEVICES=1 ${TEST_ICD_VERSION_2_DEFINES})

if (WIN32)
    target_sources(test_icd_export_none PRIVATE export_definitions/test_icd_0.def)
    target_sources(test_icd_export_icd_gipa PRIVATE export_definitions/test_icd_gipa.def)
    target_sources(test_icd_export_negotiate_interface_version PRIVATE export_definitions/test_icd_negotiate_version.def)
    target_sources(test_icd_version_2 PRIVATE export_definitions/test_icd_2.def)
    target_sources(test_icd_version_2_export_icd_enumerate_adapter_physical_devices PRIVATE export_definitions/test_icd_2_enum_adapter.def)
    target_sources(test_icd_version_2_export_icd_gpdpa PRIVATE export_definitions/test_icd_2_gpdpa.def)
    target_sources(test_icd_version_6 PRIVATE export_definitions/test_icd_6.def)
endif()
