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

add_library(testing_framework_util STATIC test_util.cpp test_util.h)
target_link_libraries(testing_framework_util PUBLIC Vulkan::Headers)
if(UNIX OR APPLE)
    target_link_libraries(testing_framework_util PUBLIC ${CMAKE_DL_LIBS})
endif()

if(WIN32)
    target_compile_definitions(testing_framework_util PUBLIC VK_USE_PLATFORM_WIN32_KHR)
elseif(ANDROID)
    target_compile_definitions(testing_framework_util PUBLIC VK_USE_PLATFORM_ANDROID_KHR)
elseif(APPLE)
    target_compile_definitions(testing_framework_util PUBLIC VK_USE_PLATFORM_MACOS_MVK VK_USE_PLATFORM_METAL_EXT)
elseif(UNIX AND NOT APPLE) # i.e.: Linux
    if(BUILD_WSI_XCB_SUPPORT)
        target_compile_definitions(testing_framework_util PUBLIC VK_USE_PLATFORM_XCB_KHR)
    endif()
    if(BUILD_WSI_XLIB_SUPPORT)
        target_compile_definitions(testing_framework_util PUBLIC VK_USE_PLATFORM_XLIB_KHR VK_USE_PLATFORM_XLIB_XRANDR_EXT)
    endif()
    if(BUILD_WSI_WAYLAND_SUPPORT)
        target_compile_definitions(testing_framework_util PUBLIC VK_USE_PLATFORM_WAYLAND_KHR)
    endif()
    if(BUILD_WSI_DIRECTFB_SUPPORT)
        target_compile_definitions(testing_framework_util PUBLIC VK_USE_PLATFORM_DIRECTFB_EXT)
    endif()
    if(BUILD_WSI_SCREEN_QNX_SUPPORT)
        target_compile_definitions(testing_framework_util PUBLIC VK_USE_PLATFORM_SCREEN_QNX)
    endif()
else()
    message(FATAL_ERROR "Unsupported Platform!")
endif()

if(UNIX)
    target_compile_options(testing_framework_util PUBLIC -fPIC)
endif()
# Gives access to all headers in the framework folder, in the framework binary, and in the whole project (mainly for loader/generated)
target_include_directories(testing_framework_util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR})

if (UNIX)
    if (TEST_USE_ADDRESS_SANITIZER)
        target_compile_options(testing_framework_util PUBLIC -fsanitize=address)
        target_link_options(testing_framework_util PUBLIC -fsanitize=address)
        target_compile_options(vulkan PUBLIC -fsanitize=address)
        target_link_options(vulkan PUBLIC -fsanitize=address)
    endif()
    if (TEST_USE_THREAD_SANITIZER)
        target_compile_options(testing_framework_util PUBLIC -fsanitize=thread)
        target_link_options(testing_framework_util PUBLIC -fsanitize=thread)
        target_compile_options(vulkan PUBLIC -fsanitize=thread)
        target_link_options(vulkan PUBLIC -fsanitize=thread)
    endif()
endif()

add_subdirectory(data)
add_subdirectory(shim)
add_subdirectory(icd)
add_subdirectory(layer)

#configure the framework_config.h.in file - used to locate all the binaries generated so that it can be used in the tests
#setup framework_config_temp.h.in in the current binary directory
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/framework_config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/framework_config_temp.h.in")

# setup framework_config.h.in using framework_config_temp.h.in as a source
file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/framework_config_$<CONFIG>.h" INPUT "${CMAKE_CURRENT_BINARY_DIR}/framework_config_temp.h.in")

# copy framework_config_$<CONFIG> to the loader build directory
add_custom_command(
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} "-E" "copy_if_different" "${CMAKE_CURRENT_BINARY_DIR}/framework_config_$<CONFIG>.h" "${CMAKE_CURRENT_BINARY_DIR}/framework_config.h"
    VERBATIM
    PRE_BUILD
    DEPENDS  "${CMAKE_CURRENT_BINARY_DIR}/framework_config_$<CONFIG>.h"
    OUTPUT   "${CMAKE_CURRENT_BINARY_DIR}/framework_config.h"
    COMMENT  "creating framework_config.h file ({event: PRE_BUILD}, {filename: framework_config.h })"
    )
add_custom_target (generate_framework_config DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/framework_config.h")
add_dependencies (testing_framework_util generate_framework_config)

add_library(test-environment STATIC test_environment.cpp test_environment.h)
target_link_libraries(test-environment
    PUBLIC gtest
    testing_framework_util shim-library)
target_include_directories(test-environment PUBLIC ${CMAKE_BINARY_DIR}/tests/framework)
target_compile_definitions(test-environment PUBLIC "GTEST_LINKED_AS_SHARED_LIBRARY=1")

add_library(testing_dependencies INTERFACE)

target_compile_options(testing_dependencies INTERFACE $<$<PLATFORM_ID:WIN32>:${MSVC_LOADER_COMPILE_OPTIONS},/permissive->)
target_compile_definitions(testing_dependencies INTERFACE
# Workaround for TR1 deprecation in Visual Studio 15.5 until Google Test is updated
    $<$<PLATFORM_ID:WIN32>:-DWIN32_LEAN_AND_MEAN,-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING>)

target_link_libraries(testing_dependencies INTERFACE Vulkan::Headers testing_framework_util test-environment)
