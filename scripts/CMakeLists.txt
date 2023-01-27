
# ~~~
# Copyright (c) 2023 LunarG, Inc.
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

option(UPDATE_DEPS "Run update_deps.py for user")
if (UPDATE_DEPS)
    find_package(PythonInterp 3 REQUIRED)

    if (CMAKE_GENERATOR_PLATFORM)
        set(_target_arch ${CMAKE_GENERATOR_PLATFORM})
    else()
        if (MSVC_IDE)
            message(WARNING "CMAKE_GENERATOR_PLATFORM not set. Using x64 as target architecture.")
        endif()
        set(_target_arch x64)
    endif()

    if (NOT CMAKE_BUILD_TYPE)
        message(WARNING "CMAKE_BUILD_TYPE not set. Using Debug for dependency build type")
        set(_build_type Debug)
    else()
        set(_build_type ${CMAKE_BUILD_TYPE})
    endif()

    set(_build_tests_arg "")
    if (NOT BUILD_TESTS)
        set(_build_tests_arg "--optional=tests")
    endif()

    message("********************************************************************************")
    message("* NOTE: Adding target vl_update_deps to run as needed for updating             *")
    message("*       dependencies.                                                          *")
    message("********************************************************************************")

    set(update_dep_py "${CMAKE_CURRENT_LIST_DIR}/update_deps.py")
    set(known_good_json "${CMAKE_CURRENT_LIST_DIR}/known_good.json")

    # Add a target so that update_deps.py will run when necessary
    # NOTE: This is triggered off of the timestamps of known_good.json and helper.cmake
    add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/external/helper.cmake
        COMMAND ${PYTHON_EXECUTABLE} ${update_dep_py} --dir ${PROJECT_SOURCE_DIR}/external --arch ${_target_arch} --config ${_build_type} --generator "${CMAKE_GENERATOR}" ${_build_tests_arg}
        DEPENDS ${known_good_json}
    )

    add_custom_target(vl_update_deps DEPENDS ${PROJECT_SOURCE_DIR}/external/helper.cmake)

    # Check if update_deps.py needs to be run on first cmake run
    if (${known_good_json} IS_NEWER_THAN ${PROJECT_SOURCE_DIR}/external/helper.cmake)
        execute_process(
            COMMAND ${PYTHON_EXECUTABLE} ${update_dep_py} --dir ${PROJECT_SOURCE_DIR}/external --arch ${_target_arch} --config ${_build_type} --generator "${CMAKE_GENERATOR}" ${_build_tests_arg}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            RESULT_VARIABLE _update_deps_result
        )
        if (NOT (${_update_deps_result} EQUAL 0))
            message(FATAL_ERROR "Could not run update_deps.py which is necessary to download dependencies.")
        endif()
    endif()
    include(${PROJECT_SOURCE_DIR}/external/helper.cmake)
else()
    message("********************************************************************************")
    message("* NOTE: Not adding target to run update_deps.py automatically.                 *")
    message("********************************************************************************")
    find_package(PythonInterp 3 QUIET)
endif()

if (VULKAN_HEADERS_INSTALL_DIR)
    list(APPEND CMAKE_PREFIX_PATH ${VULKAN_HEADERS_INSTALL_DIR})
endif()

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} PARENT_SCOPE)
