# ~~~
# Copyright (c) 2014-2018 Valve Corporation
# Copyright (c) 2014-2018 LunarG, Inc.
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

add_executable(vk_loader_validation_tests loader_validation_tests.cpp)

set_target_properties(vk_loader_validation_tests PROPERTIES COMPILE_DEFINITIONS "GTEST_LINKED_AS_SHARED_LIBRARY=1")
if(UNIX)
    set_target_properties(vk_loader_validation_tests PROPERTIES COMPILE_FLAGS "-Wno-sign-compare")
endif()

if(WIN32)
    target_compile_options(vk_loader_validation_tests PUBLIC ${MSVC_LOADER_COMPILE_OPTIONS})

    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR -DWIN32_LEAN_AND_MEAN)
    # Workaround for TR1 deprecation in Visual Studio 15.5 until Google Test is updated
    add_definitions(-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
elseif(ANDROID)
    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)
elseif(APPLE)
    add_definitions(-DVK_USE_PLATFORM_MACOS_MVK)
elseif(UNIX AND NOT APPLE) # i.e.: Linux
    target_compile_options(vk_loader_validation_tests PUBLIC -fPIC)
    if(BUILD_WSI_XCB_SUPPORT)
        add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
    endif()

    if(BUILD_WSI_XLIB_SUPPORT)
        add_definitions(-DVK_USE_PLATFORM_XLIB_KHR)
    endif()

    if(BUILD_WSI_WAYLAND_SUPPORT)
        add_definitions(-DVK_USE_PLATFORM_WAYLAND_KHR)
    endif()

    if(BUILD_WSI_SCREEN_QNX_SUPPORT)
        add_definitions(-DVK_USE_PLATFORM_SCREEN_QNX)
    endif()
else()
    message(FATAL_ERROR "Unsupported Platform!")
endif()

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS -D_USE_MATH_DEFINES")

    # If MSVC, disable some signed/unsigned mismatch warnings.
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4267")
    endif()

else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(LIBGLM_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/libs)

include_directories(${PROJECT_SOURCE_DIR}/external
                    ${GTEST_SOURCE_DIR}/googletest/include
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_BINARY_DIR}
                    ${PROJECT_BINARY_DIR})

if(UNIX)
    # extra setup for out-of-tree builds
    if(NOT (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR))
        add_custom_target(binary-dir-symlinks ALL
                          COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/run_wrap_objects_tests.sh
                                  run_wrap_objects_tests.sh
                          COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/run_loader_tests.sh
                                  run_loader_tests.sh
                          COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/run_extra_loader_tests.sh
                                  run_extra_loader_tests.sh
                          COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/run_all_tests.sh run_all_tests.sh
                          VERBATIM)
    endif()
else()
    if(NOT (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR))
        file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/_run_all_tests.ps1 RUN_ALL)
        add_custom_target(binary-dir-symlinks ALL
                          COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RUN_ALL} run_all_tests.ps1
                          VERBATIM)
        set_target_properties(binary-dir-symlinks PROPERTIES FOLDER ${LOADER_HELPER_FOLDER})
    endif()
endif()

if(TARGET vulkan)
    set(LOADER_LIB vulkan)
elseif(WIN32 AND NOT $ENV{VULKAN_SDK} EQUAL "")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(LOADER_LIB "$ENV{VULKAN_SDK}/Lib/vulkan-1.lib")
    else()
        set(LOADER_LIB "$ENV{VULKAN_SDK}/Lib32/vulkan-1.lib")
    endif()
else()
    set(LOADER_LIB vulkan)
endif()

target_link_libraries(vk_loader_validation_tests "${LOADER_LIB}" gtest gtest_main)

add_subdirectory(layers)

# By default, tests and loader are built without sanitizers
# Use these options to force a specific sanitizer on all test executables
option(TEST_USE_ADDRESS_SANITIZER "Linux only: Advanced memory checking" OFF)
option(TEST_USE_THREAD_SANITIZER "Linux only: Advanced thread checking" OFF)

include(GoogleTest)
add_subdirectory(framework)

add_executable(test_regression loader_testing_main.cpp loader_regression_tests.cpp loader_layer_tests.cpp
    loader_version_tests.cpp loader_alloc_callback_tests.cpp)
target_link_libraries(test_regression PRIVATE testing_dependencies)

add_executable(test_wsi loader_testing_main.cpp loader_wsi_tests.cpp)
target_link_libraries(test_wsi PRIVATE testing_dependencies)

if(UNIX AND NOT APPLE) # i.e. Linux
    if(BUILD_WSI_XCB_SUPPORT)
        target_include_directories(test_wsi PRIVATE ${XCB_INCLUDE_DIRS})
        target_link_libraries(test_wsi PRIVATE ${XCB_LIBRARIES})
    endif()

    if(BUILD_WSI_XLIB_SUPPORT)
        target_include_directories(test_wsi PRIVATE ${X11_INCLUDE_DIR})
        target_link_libraries(test_wsi PRIVATE ${X11_LIBRARIES})
    endif()

    if(BUILD_WSI_WAYLAND_SUPPORT)
        target_include_directories(test_wsi PRIVATE ${WAYLAND_CLIENT_INCLUDE_DIR})
        target_link_libraries(test_wsi PRIVATE ${WAYLAND_CLIENT_LIBRARIES})
    endif()
endif()

add_executable(test_threading loader_testing_main.cpp loader_threading_tests.cpp)
target_link_libraries(test_threading PRIVATE testing_dependencies)

if(WIN32)
    # Copy loader and googletest (gtest) libs to test dir so the test executable can find them.
    add_custom_command(TARGET test_regression POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:gtest> $<TARGET_FILE_DIR:test_regression>)
    # Copy the loader shared lib (if built) to the test application directory so the test app finds it.
    if(TARGET vulkan)
        add_custom_command(TARGET test_regression POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:vulkan> $<TARGET_FILE_DIR:test_regression>)
    endif()
endif()

# must happen after the dll's get copied over
gtest_discover_tests(test_regression)
