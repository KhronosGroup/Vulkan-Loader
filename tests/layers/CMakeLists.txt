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

set(TARGET_NAMES VkLayer_wrap_objects VkLayer_test VkLayer_meta)

set(VK_LAYER_RPATH /usr/lib/x86_64-linux-gnu/vulkan/layer:/usr/lib/i386-linux-gnu/vulkan/layer)
set(CMAKE_INSTALL_RPATH ${VK_LAYER_RPATH})

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

# The output file needs Unix "/" separators or Windows "\" separators
# On top of that, Windows separators actually need to be doubled because the json format uses backslash escapes
file(TO_NATIVE_PATH "./" RELATIVE_PATH_PREFIX)
string(REPLACE "\\" "\\\\" RELATIVE_PATH_PREFIX "${RELATIVE_PATH_PREFIX}")

# Run each .json.in file through the generator
# We need to create the generator.cmake script so that the generator can be run at compile time, instead of configure time
# Running at compile time lets us use cmake generator expressions (TARGET_FILE_NAME and TARGET_FILE_DIR, specifically)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/generator.cmake" "configure_file(\"\${INPUT_FILE}\" \"\${OUTPUT_FILE}\")")
foreach(TARGET_NAME ${TARGET_NAMES})
    set(CONFIG_DEFINES
        -DINPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/json/${TARGET_NAME}.json.in"
        -DVK_VERSION="${VulkanHeaders_VERSION_MAJOR}.${VulkanHeaders_VERSION_MINOR}.${VulkanHeaders_VERSION_PATCH}"
    )
    # If this json file is not a metalayer, get the needed properties from that target
    if(TARGET ${TARGET_NAME})
        set(CONFIG_DEFINES ${CONFIG_DEFINES}
            -DOUTPUT_FILE="$<TARGET_FILE_DIR:${TARGET_NAME}>/${TARGET_NAME}.json"
            -DRELATIVE_LAYER_BINARY="${RELATIVE_PATH_PREFIX}$<TARGET_FILE_NAME:${TARGET_NAME}>"
        )
    # If this json file is a metalayer, make the output path match the test layer, and there is no layer binary file
    else()
        set(CONFIG_DEFINES ${CONFIG_DEFINES}
            -DOUTPUT_FILE="$<TARGET_FILE_DIR:VkLayer_test>/${TARGET_NAME}.json"
        )
    endif()
    add_custom_target(${TARGET_NAME}-json ALL COMMAND ${CMAKE_COMMAND} ${CONFIG_DEFINES} -P "${CMAKE_CURRENT_BINARY_DIR}/generator.cmake")
endforeach()
