# ~~~
# Copyright (c) 2016-2018 Valve Corporation
# Copyright (c) 2016-2018 LunarG, Inc.
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

cmake_minimum_required(VERSION 2.8.11)

set(LAYER_JSON_FILES VkLayer_wrap_objects VkLayer_test VkLayer_meta)

set(VK_LAYER_RPATH /usr/lib/x86_64-linux-gnu/vulkan/layer:/usr/lib/i386-linux-gnu/vulkan/layer)
set(CMAKE_INSTALL_RPATH ${VK_LAYER_RPATH})

if(WIN32)
    if(NOT (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR))
        add_custom_target(mk_test_layer_config_dir ALL
                          COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
        set_target_properties(mk_test_layer_config_dir PROPERTIES FOLDER ${LOADER_HELPER_FOLDER})
        foreach(config_file ${LAYER_JSON_FILES})
            file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/windows/${config_file}.json src_json)
            if(CMAKE_GENERATOR MATCHES "^Visual Studio.*")
                file(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIGURATION>/${config_file}.json dst_json)
            else()
                file(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR}/${config_file}.json dst_json)
            endif()
            add_custom_target(${config_file}-json ALL COMMAND copy ${src_json} ${dst_json} VERBATIM)
            add_dependencies(${config_file}-json mk_test_layer_config_dir)
            set_target_properties(${config_file}-json PROPERTIES FOLDER ${LOADER_HELPER_FOLDER})
        endforeach(config_file)
    endif()
elseif(APPLE)
    # extra setup for out-of-tree builds
    if(NOT (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR))
        if(CMAKE_GENERATOR MATCHES "^Xcode.*")
            add_custom_target(mk_test_layer_config_dir ALL
                              COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
            foreach(config_file ${LAYER_JSON_FILES})
                add_custom_target(${config_file}-json ALL
                                  DEPENDS mk_test_layer_config_dir
                                  COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/macos/${config_file}.json $<CONFIG>
                                  VERBATIM)
            endforeach(config_file)
        else()
            foreach(config_file ${LAYER_JSON_FILES})
                add_custom_target(${config_file}-json ALL
                                  COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/macos/${config_file}.json
                                  VERBATIM)
            endforeach(config_file)
        endif()
    endif()
else()
    # extra setup for out-of-tree builds
    if(NOT (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR))
        foreach(config_file ${LAYER_JSON_FILES})
            add_custom_target(${config_file}-json ALL COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/linux/${config_file}.json VERBATIM)
        endforeach(config_file)
    endif()
endif()

if(WIN32)
    macro(add_vk_layer target)
        file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/VkLayer_${target}.def DEF_FILE)
        add_custom_target(copy-${target}-def-file ALL
                          COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DEF_FILE} VkLayer_${target}.def
                          VERBATIM)
        set_target_properties(copy-${target}-def-file PROPERTIES FOLDER ${LOADER_HELPER_FOLDER})
        add_library(VkLayer_${target} SHARED ${ARGN} VkLayer_${target}.def)
        add_dependencies(VkLayer_${target} generate_helper_files)
    endmacro()
elseif(APPLE)
    macro(add_vk_layer target)
        add_library(VkLayer_${target} SHARED ${ARGN})
        add_dependencies(VkLayer_${target} generate_helper_files)
        set_target_properties(VkLayer_${target} PROPERTIES LINK_FLAGS "-Wl")
    endmacro()
else()
    macro(add_vk_layer target)
        add_library(VkLayer_${target} SHARED ${ARGN})
        add_dependencies(VkLayer_${target} generate_helper_files)
        set_target_properties(VkLayer_${target} PROPERTIES LINK_FLAGS "-Wl,-Bsymbolic")
    endmacro()
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../loader
                    ${PROJECT_SOURCE_DIR}/Vulkan-Headers/include
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${PROJECT_BINARY_DIR}
                    ${CMAKE_BINARY_DIR})

if(WIN32)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_CRT_SECURE_NO_WARNINGS /bigobj")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_CRT_SECURE_NO_WARNINGS /bigobj")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpointer-arith -Wno-unused-function")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpointer-arith -Wno-unused-function")
endif()

set(WRAP_SRCS wrap_objects.cpp vk_layer_table.cpp vk_layer_extension_utils.cpp)
add_vk_layer(wrap_objects ${WRAP_SRCS})

set(TEST_SRCS test.cpp vk_layer_table.cpp vk_layer_extension_utils.cpp)
add_vk_layer(test ${TEST_SRCS})
