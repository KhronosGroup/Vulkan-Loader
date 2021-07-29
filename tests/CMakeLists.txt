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


# By default, tests and loader are built without sanitizers
# Use these options to force a specific sanitizer on all test executables
option(TEST_USE_ADDRESS_SANITIZER "Linux only: Advanced memory checking" OFF)
option(TEST_USE_THREAD_SANITIZER "Linux only: Advanced thread checking" OFF)

include(GoogleTest)
add_subdirectory(framework)

add_executable(
    test_regression
        loader_testing_main.cpp
        loader_alloc_callback_tests.cpp
        loader_get_proc_addr_tests.cpp
        loader_handle_validation_tests.cpp
        loader_layer_tests.cpp
        loader_regression_tests.cpp
        loader_version_tests.cpp
        loader_unknown_ext_tests.cpp)
target_link_libraries(test_regression PRIVATE testing_dependencies)

add_executable(
    test_wsi
        loader_testing_main.cpp
        loader_wsi_tests.cpp)
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

add_executable(
    test_threading
        loader_testing_main.cpp
        loader_threading_tests.cpp)
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
