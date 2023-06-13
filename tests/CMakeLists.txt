# ~~~
# Copyright (c) 2014-2022 Valve Corporation
# Copyright (c) 2014-2022 LunarG, Inc.
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


option(ENABLE_LIVE_VERIFICATION_TESTS "Enable tests which expect to run on live drivers. Meant for manual verification only" OFF)

include(GoogleTest)
add_subdirectory(framework)

add_executable(
    test_regression
        loader_testing_main.cpp
        loader_alloc_callback_tests.cpp
        loader_envvar_tests.cpp
        loader_get_proc_addr_tests.cpp
        loader_debug_ext_tests.cpp
        loader_handle_validation_tests.cpp
        loader_layer_tests.cpp
        loader_regression_tests.cpp
        loader_phys_dev_inst_ext_tests.cpp
        loader_settings_tests.cpp
        loader_version_tests.cpp
        loader_unknown_ext_tests.cpp
        loader_wsi_tests.cpp)
target_link_libraries(test_regression PUBLIC testing_dependencies)
set_target_properties(test_regression PROPERTIES ${TESTS_STANDARD_CXX_PROPERTIES})
target_compile_definitions(test_regression PUBLIC VK_NO_PROTOTYPES)

# Threading tests live in separate executabe just for threading tests as it'll need support
# in the test harness to enable in CI, as thread sanitizer doesn't work with address sanitizer enabled.
add_executable(
    test_threading
        loader_testing_main.cpp
        loader_threading_tests.cpp)
target_link_libraries(test_threading PUBLIC testing_dependencies)
set_target_properties(test_threading PROPERTIES ${TESTS_STANDARD_CXX_PROPERTIES})
target_compile_definitions(test_threading PUBLIC VK_NO_PROTOTYPES)

# executables that are meant for testing against real drivers rather than the mocks
if (ENABLE_LIVE_VERIFICATION_TESTS)
    add_subdirectory(live_verification)
endif()

if(WIN32)
    # Copy loader and googletest (gtest) libs to test dir so the test executable can find them.
    add_custom_command(TARGET test_regression POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:gtest> $<TARGET_FILE_DIR:test_regression>)
    # Copy the loader shared lib (if built) to the test application directory so the test app finds it.
    if(TARGET vulkan)
        add_custom_command(TARGET test_regression POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:vulkan> $<TARGET_FILE_DIR:test_regression>)
    endif()

    # Copy the gtest shared lib (if built) to the live verification tests directory so the tests finds it.
    if(ENABLE_LIVE_VERIFICATION_TESTS)
        add_custom_command(TARGET test_regression POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:gtest> $<TARGET_FILE_DIR:dynamic_rendering_get_proc_addr>)
    endif()
endif()

# must happen after the dll's get copied over
if(NOT CMAKE_CROSSCOMPILING)
    gtest_discover_tests(test_regression PROPERTIES DISCOVERY_TIMEOUT 100)
else()
    gtest_add_tests(TARGET test_regression)
endif()
