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
target_compile_definitions(vk_loader_validation_tests PRIVATE "GTEST_LINKED_AS_SHARED_LIBRARY=1")

if(WIN32)
    target_compile_definitions(vk_loader_validation_tests PRIVATE "VK_USE_PLATFORM_WIN32_KHR" "WIN32_LEAN_AND_MEAN")
    target_compile_definitions(vk_loader_validation_tests PRIVATE "_CRT_SECURE_NO_WARNINGS" "_USE_MATH_DEFINES")
    # Workaround for TR1 deprecation in Visual Studio 15.5 until Google Test is updated
    target_compile_definitions(vk_loader_validation_tests PRIVATE "_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING")

    if(MSVC)
        # disable some signed/unsigned mismatch warnings.
        target_compile_options(vk_loader_validation_tests PRIVATE "/wd4267")
    endif()

elseif(ANDROID)
    target_compile_definitions(vk_loader_validation_tests PRIVATE "VK_USE_PLATFORM_ANDROID_KHR")
    target_compile_features(vk_loader_validation_tests PRIVATE "cxx_std_11")
elseif(APPLE)
    target_compile_options(vk_loader_validation_tests PRIVATE "-Wno-sign-compare")
    target_compile_definitions(vk_loader_validation_tests PRIVATE "VK_USE_PLATFORM_MACOS_MVK")
    target_compile_features(vk_loader_validation_tests PRIVATE "cxx_std_11")
elseif(UNIX AND NOT APPLE)      # i.e.: Linux
    target_compile_options(vk_loader_validation_tests PRIVATE "-Wno-sign-compare")
    target_compile_features(vk_loader_validation_tests PRIVATE "cxx_std_11")

    if(BUILD_WSI_XCB_SUPPORT)
        target_compile_definitions(vk_loader_validation_tests PRIVATE "VK_USE_PLATFORM_XCB_KHR")
    endif()

    if(BUILD_WSI_XLIB_SUPPORT)
        target_compile_definitions(vk_loader_validation_tests PRIVATE "VK_USE_PLATFORM_XLIB_KHR")
    endif()

    if(BUILD_WSI_WAYLAND_SUPPORT)
        target_compile_definitions(vk_loader_validation_tests PRIVATE "VK_USE_PLATFORM_WAYLAND_KHR")
    endif()

    if(BUILD_WSI_MIR_SUPPORT)
        target_compile_definitions(vk_loader_validation_tests PRIVATE "VK_USE_PLATFORM_MIR_KHR")
        target_include_directories(vk_loader_validation_tests PRIVATE ${MIR_INCLUDE_DIR})
    endif()
else()
    message(FATAL_ERROR "Unsupported Platform!")
endif()

set(LIBGLM_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/libs)

target_include_directories(vk_loader_validation_tests PRIVATE
                    ${PROJECT_SOURCE_DIR}/external
                    ${GTEST_SOURCE_DIR}/googletest/include
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_BINARY_DIR}
                    ${PROJECT_BINARY_DIR})

if(UNIX)
    # extra setup for out-of-tree builds
    if(NOT (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR))
        add_custom_target(binary-dir-symlinks ALL
                          COMMAND $(CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/run_wrap_objects_tests.sh run_wrap_objects_tests.sh
                          COMMAND $(CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/run_loader_tests.sh run_loader_tests.sh
                          COMMAND $(CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/run_extra_loader_tests.sh run_extra_loader_tests.sh
                          COMMAND $(CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/run_all_tests.sh run_all_tests.sh
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
if(BUILD_LOADER AND ENABLE_STATIC_LOADER)
    set_target_properties(vk_loader_validation_tests PROPERTIES LINK_FLAGS "/ignore:4098")
endif()

# Copy loader and googletest (gtest) libs to test dir so the test executable can find them.
if(WIN32)
    file(COPY vk_loader_validation_tests.vcxproj.user DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
    if(CMAKE_GENERATOR MATCHES "^Visual Studio.*")
        file(TO_NATIVE_PATH ${PROJECT_BINARY_DIR}/external/googletest/googletest/$<CONFIG>/gtest_main$<$<CONFIG:Debug>:d>.dll
                            GTEST_COPY_SRC1)
        file(TO_NATIVE_PATH ${PROJECT_BINARY_DIR}/external/googletest/googletest/$<CONFIG>/gtest$<$<CONFIG:Debug>:d>.dll
                            GTEST_COPY_SRC2)
        file(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG> GTEST_COPY_DEST)
    else()
        file(TO_NATIVE_PATH ${PROJECT_BINARY_DIR}/external/googletest/googeltest/gtest_main.dll GTEST_COPY_SRC1)
        file(TO_NATIVE_PATH ${PROJECT_BINARY_DIR}/external/googletest/googletest/gtest.dll GTEST_COPY_SRC2)
        file(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR} GTEST_COPY_DEST)
    endif()
    add_custom_command(TARGET vk_loader_validation_tests POST_BUILD
                       COMMAND $(CMAKE_COMMAND} -E copy_directory ${GTEST_COPY_SRC1} ${GTEST_COPY_DEST}
                       COMMAND $(CMAKE_COMMAND} -E copy_directory ${GTEST_COPY_SRC2} ${GTEST_COPY_DEST})
    # Copy the loader shared lib (if built) to the test application directory so the test app finds it.
    if((NOT ENABLE_STATIC_LOADER) AND TARGET vulkan)
        add_custom_command(TARGET vk_loader_validation_tests POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:vulkan> $<TARGET_FILE_DIR:vk_loader_validation_tests>)
    endif()
endif()

add_subdirectory(layers)
