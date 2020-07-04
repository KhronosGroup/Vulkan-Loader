# ~~~
# Copyright (c) 2014-2019 Valve Corporation
# Copyright (c) 2014-2019 LunarG, Inc.
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

cmake_minimum_required(VERSION 3.10.2)

# Apple: Must be set before enable_language() or project() as it may influence configuration of the toolchain and flags.
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment version")

project(Vulkan-Loader)

enable_testing()

set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS API_NAME="Vulkan")

# Enable beta Vulkan extensions
add_definitions(-DVK_ENABLE_BETA_EXTENSIONS)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(PythonInterp 3 QUIET)

if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/googletest)
    option(BUILD_TESTS "Build Tests" ON)
else()
    option(BUILD_TESTS "Build Tests" OFF)
endif()

if(APPLE)
    option(BUILD_STATIC_LOADER "Build a loader that can be statically linked" OFF)
endif()

if(BUILD_STATIC_LOADER)
    message(WARNING "The ENABLE_STATIC_LOADER option has been set. Note that this will only work on MacOS and is not supported "
        "or tested as part of the loader. Use it at your own risk.")
endif()

if (TARGET Vulkan::Headers)
    message(STATUS "Using Vulkan headers from Vulkan::Headers target")
    get_target_property(VulkanHeaders_INCLUDE_DIRS Vulkan::Headers INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(VulkanRegistry_DIR Vulkan::Registry INTERFACE_INCLUDE_DIRECTORIES)
else()
    find_package(VulkanHeaders)
    if(NOT ${VulkanHeaders_FOUND})
        message(FATAL_ERROR "Could not find Vulkan headers path. This can be fixed by setting VULKAN_HEADERS_INSTALL_DIR to an "
                            "installation of the Vulkan-Headers repository.")
    endif()
    if(NOT ${VulkanRegistry_FOUND})
        message(FATAL_ERROR "Could not find Vulkan registry path. This can be fixed by setting VULKAN_HEADERS_INSTALL_DIR to an "
                            "installation of the Vulkan-Headers repository.")
    endif()

    # set up the Vulkan::Headers target for consistency
    add_library(vulkan-headers INTERFACE)
    target_include_directories(vulkan-headers SYSTEM INTERFACE "${VulkanHeaders_INCLUDE_DIRS}")
    add_library(Vulkan::Headers ALIAS vulkan-headers)
endif()

option(USE_CCACHE "Use ccache" OFF)
if(USE_CCACHE)
    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    endif()
endif()

include(GNUInstallDirs)

if(UNIX AND NOT APPLE) # i.e.: Linux
    include(FindPkgConfig)
endif()

if(APPLE)
    # CMake versions 3 or later need CMAKE_MACOSX_RPATH defined. This avoids the CMP0042 policy message.
    set(CMAKE_MACOSX_RPATH 1)
endif()

if(WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    # Windows: if install locations not set by user, set install prefix to "<build_dir>\install".
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE)
endif()

# Enable IDE GUI folders.  "Helper targets" that don't have interesting source code should set their FOLDER property to this
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(LOADER_HELPER_FOLDER "Helper Targets")

if(UNIX)
    set(
        FALLBACK_CONFIG_DIRS "/etc/xdg"
        CACHE
            STRING
            "Search path to use when XDG_CONFIG_DIRS is unset or empty or the current process is SUID/SGID. Default is freedesktop compliant."
        )
    set(
        FALLBACK_DATA_DIRS "/usr/local/share:/usr/share"
        CACHE
            STRING
            "Search path to use when XDG_DATA_DIRS is unset or empty or the current process is SUID/SGID. Default is freedesktop compliant."
        )
    set(
        SYSCONFDIR ""
        CACHE
            STRING
            "System-wide search directory. If not set or empty, CMAKE_INSTALL_FULL_SYSCONFDIR and /etc are used."
        )
endif()

if(UNIX AND NOT APPLE) # i.e.: Linux
    option(BUILD_WSI_XCB_SUPPORT "Build XCB WSI support" ON)
    option(BUILD_WSI_XLIB_SUPPORT "Build Xlib WSI support" ON)
    option(BUILD_WSI_WAYLAND_SUPPORT "Build Wayland WSI support" ON)
    option(BUILD_WSI_DIRECTFB_SUPPORT "Build DirectFB WSI support" OFF)

    if(BUILD_WSI_XCB_SUPPORT)
        find_package(XCB REQUIRED)
    endif()

    if(BUILD_WSI_XLIB_SUPPORT)
        find_package(X11 REQUIRED)
    endif()

    if(BUILD_WSI_WAYLAND_SUPPORT)
        find_package(Wayland REQUIRED)
        include_directories(SYSTEM ${WAYLAND_CLIENT_INCLUDE_DIR})
    endif()

    if(BUILD_WSI_DIRECTFB_SUPPORT)
        find_package(DirectFB REQUIRED)
        include_directories(SYSTEM ${DIRECTFB_INCLUDE_DIR})
    endif()
endif()

if(WIN32)
    option(ENABLE_WIN10_ONECORE "Link the loader with OneCore umbrella libraries" OFF)
endif()

option(BUILD_LOADER "Build loader" ON)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(COMMON_COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers")
    set(COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -fno-strict-aliasing -fno-builtin-memcmp")

    # For GCC version 7.1 or greater, we need to disable the implicit fallthrough warning since there's no consistent way to satisfy
    # all compilers until they all accept the C++17 standard
    if(CMAKE_COMPILER_IS_GNUCC)
        set(COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -Wno-stringop-truncation -Wno-stringop-overflow")
        if(CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 7.1)
            set(COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -Wimplicit-fallthrough=0")
        endif()
    endif()

    if(APPLE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_COMPILE_FLAGS}")
    #clang-cl on Windows
    elseif((CMAKE_C_COMPILER_ID MATCHES "Clang") AND (CMAKE_CXX_SIMULATE_ID MATCHES "MSVC"))
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Xclang -std=c99 ${COMMON_COMPILE_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xclang -std=c++11 -fno-rtti")
    # clang (not clang-cl) or gcc
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 ${COMMON_COMPILE_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_FLAGS}")

    if(UNIX)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
    endif()
endif()

if(MSVC)
    # /WX: Treat warnings as errors
    # /GR-: Disable RTTI
    # /w34456: Warn about nested declarations
    # /w34701, /w34703: Warn about potentially uninitialized variables
    # /w34057: Warn about different indirection types.
    # /w34245: Warn about signed/unsigned mismatch.
    set(MSVC_LOADER_COMPILE_OPTIONS /WX /GR- /w34456 /w34701 /w34703 /w34057 /w34245)
endif()

# Optional codegen target
if(PYTHONINTERP_FOUND)
    add_custom_target(VulkanLoader_generated_source
                      COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/scripts/generate_source.py
                              ${VulkanRegistry_DIR} --incremental
                      )
else()
    message("WARNING: VulkanLoader_generated_source target requires python 3")
endif()


if(UNIX)
    set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS FALLBACK_CONFIG_DIRS="${FALLBACK_CONFIG_DIRS}" FALLBACK_DATA_DIRS="${FALLBACK_DATA_DIRS}")

    if(NOT (SYSCONFDIR STREQUAL ""))
        # SYSCONFDIR is specified, use it and do not force /etc.
        set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS SYSCONFDIR="${SYSCONFDIR}")
    else()
        set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS SYSCONFDIR="${CMAKE_INSTALL_FULL_SYSCONFDIR}")

        # Make sure /etc is searched by the loader
        if(NOT (CMAKE_INSTALL_FULL_SYSCONFDIR STREQUAL "/etc"))
            set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS EXTRASYSCONFDIR="/etc")
        endif()
    endif()
endif()

# uninstall target
if(NOT TARGET uninstall)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
                   IMMEDIATE
                   @ONLY)
    add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
    set_target_properties(uninstall PROPERTIES FOLDER ${LOADER_HELPER_FOLDER})
endif()

if(BUILD_LOADER)
    add_subdirectory(loader)
endif()

add_subdirectory(external)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
