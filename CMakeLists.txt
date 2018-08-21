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

cmake_minimum_required(VERSION 3.4)

# Apple: Must be set before enable_language() or project() as it may influence configuration of the toolchain and flags.
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment version")

project(Vulkan-Loader)

#set(CMAKE_DEBUG_TARGET_PROPERTIES INCLUDE_DIRECTORIES COMPILE_DEFINITIONS COMPILE_OPTIONS AUTOUIC_OPTIONS SOURCES COMPILE_FEATURES POSITION_INDEPENDENT_CODE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(PythonInterp 3 REQUIRED)

find_package(VulkanHeaders)
if(NOT ${VulkanHeaders_FOUND})
    message(FATAL_ERROR "Could not find Vulkan headers path. This can be fixed by setting VULKAN_HEADERS_INSTALL_DIR to an "
                        "installation of the Vulkan-Headers repository.")
endif()
if(NOT ${VulkanRegistry_FOUND})
    message(FATAL_ERROR "Could not find Vulkan registry path. This can be fixed by setting VULKAN_HEADERS_INSTALL_DIR to an "
                        "installation of the Vulkan-Headers repository.")
endif()

option(USE_CCACHE "Use ccache" OFF)
if(USE_CCACHE)
    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    endif()
endif()

include(GNUInstallDirs)

if(UNIX AND NOT APPLE)  # i.e.: Linux
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
endif()

if(UNIX AND NOT APPLE)  # i.e.: Linux
    option(BUILD_WSI_XCB_SUPPORT "Build XCB WSI support" ON)
    option(BUILD_WSI_XLIB_SUPPORT "Build Xlib WSI support" ON)
    option(BUILD_WSI_WAYLAND_SUPPORT "Build Wayland WSI support" ON)
    option(BUILD_WSI_MIR_SUPPORT "Build Mir WSI support" OFF)

    if(BUILD_WSI_XCB_SUPPORT)
        find_package(XCB REQUIRED)
    endif()

    if(BUILD_WSI_XLIB_SUPPORT)
        find_package(X11 REQUIRED)
    endif()

    if(BUILD_WSI_WAYLAND_SUPPORT)
        find_package(Wayland REQUIRED)
        include_directories(${WAYLAND_CLIENT_INCLUDE_DIR})
    endif()

    if(BUILD_WSI_MIR_SUPPORT)
        find_package(Mir REQUIRED)
    endif()
endif()

if(WIN32)
    option(ENABLE_WIN10_ONECORE "Link the loader with OneCore umbrella libraries" OFF)
    option(ENABLE_STATIC_LOADER "Build the loader as a static library" OFF)
endif()

# Add location of Vulkan header files to include search path
include_directories(${VulkanHeaders_INCLUDE_DIRS})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
# TODO(mew) replace COMMON_COMPILE_FLAGS
    set(COMMON_COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers")
    set(COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -fno-strict-aliasing -fno-builtin-memcmp")

    # For GCC version 7.1 or greater, we need to disable the implicit fallthrough warning since there's no consistent way to satisfy
    # all compilers until they all accept the C++17 standard
    if(CMAKE_COMPILER_IS_GNUCC AND NOT (CMAKE_CXX_COMPILER_VERSION LESS 7.1))
        set(COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -Wimplicit-fallthrough=0")
    endif()

    if(APPLE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_COMPILE_FLAGS}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 ${COMMON_COMPILE_FLAGS}")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_FLAGS} -std=c++11 -fno-rtti")
    if(UNIX)
        # hide ELF symbols by default; see http://gcc.gnu.org/wiki/Visibility
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
    endif()
endif()

if(MSVC)
    # Treat warnings as errors
    add_compile_options("/WX")
    # Disable RTTI
    add_compile_options("/GR-")
    # Warn about nested declarations
    add_compile_options("/w34456")
    # Warn about potentially uninitialized variables
    add_compile_options("/w34701")
    add_compile_options("/w34703")
    # Warn about different indirection types.
    add_compile_options("/w34057")
    # Warn about signed/unsigned mismatch.
    add_compile_options("/w34245")
endif()

option(BUILD_LOADER "Build loader" ON)
if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/googletest)
    option(BUILD_TESTS "Build Tests" ON)
else()
    option(BUILD_TESTS "Build Tests" OFF)
endif()

# Define macro used for building vkxml generated files
set(SCRIPTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/scripts")
macro(GenerateFromVkXml output dependency)
    add_custom_command(OUTPUT ${output}
                       COMMAND ${PYTHON_EXECUTABLE} ${SCRIPTS_DIR}/loader_genvk.py -registry ${VulkanRegistry_DIR}/vk.xml -scripts
                               ${VulkanRegistry_DIR} ${output}
                       DEPENDS ${SCRIPTS_DIR}/${dependency}
                               ${SCRIPTS_DIR}/loader_genvk.py
                               ${VulkanRegistry_DIR}/vk.xml
                               ${VulkanRegistry_DIR}/generator.py
                               ${VulkanRegistry_DIR}/reg.py)
endmacro()

# Custom target for generated vulkan helper file dependencies
add_custom_target(generate_helper_files
                  DEPENDS vk_enum_string_helper.h
                          vk_safe_struct.h
                          vk_safe_struct.cpp
                          vk_object_types.h
                          vk_layer_dispatch_table.h
                          vk_dispatch_table_helper.h
                          vk_extension_helper.h
                          vk_typemap_helper.h)
set_target_properties(generate_helper_files PROPERTIES FOLDER ${LOADER_HELPER_FOLDER})

# Rules to build generated helper files
GenerateFromVkXml(vk_layer_dispatch_table.h loader_extension_generator.py)
GenerateFromVkXml(vk_dispatch_table_helper.h dispatch_table_helper_generator.py)
GenerateFromVkXml(vk_safe_struct.h helper_file_generator.py)
GenerateFromVkXml(vk_safe_struct.cpp helper_file_generator.py)
GenerateFromVkXml(vk_enum_string_helper.h helper_file_generator.py)
GenerateFromVkXml(vk_object_types.h helper_file_generator.py)
GenerateFromVkXml(vk_extension_helper.h helper_file_generator.py)
GenerateFromVkXml(vk_typemap_helper.h helper_file_generator.py)

if(UNIX)
    add_definitions(-DFALLBACK_CONFIG_DIRS="${FALLBACK_CONFIG_DIRS}")
    add_definitions(-DFALLBACK_DATA_DIRS="${FALLBACK_DATA_DIRS}")
    add_definitions(-DSYSCONFDIR="${CMAKE_INSTALL_FULL_SYSCONFDIR}")

    # Make sure /etc is searched by the loader
    if(NOT (CMAKE_INSTALL_FULL_SYSCONFDIR STREQUAL "/etc"))
        add_definitions(-DEXTRASYSCONFDIR="/etc")
    endif()
endif()

# uninstall target
if(NOT TARGET uninstall)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
                   IMMEDIATE
                   @ONLY)
    add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
    set_target_properties(uninstall PROPERTIES FOLDER ${LOADER_HELPER_FOLDER})
endif()

# TODO(mew) API_NAME is only referenced in loader/vk_loader_platform.h
add_definitions(-DAPI_NAME="Vulkan")

if(BUILD_LOADER)
    add_subdirectory(loader)
endif()

add_subdirectory(external)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Debug stuff ------------------------------------------------------------------------

message("CMAKE_SYSTEM : ${CMAKE_SYSTEM}")
message("CMAKE_SYSTEM_NAME : ${CMAKE_SYSTEM_NAME}") # Windows Darwin Linux Android FreeBSD
message("CMAKE_ROOT : ${CMAKE_ROOT}")
message("PROJECT_NAME : ${PROJECT_NAME}")
message("CMAKE_PROJECT_NAME : ${CMAKE_PROJECT_NAME}")
message("CMAKE_VERSION : ${CMAKE_VERSION}")
message("CMAKE_GENERATOR : ${CMAKE_GENERATOR}")

message("UNIX : ${UNIX}")         # includes OSX and CygWin
message("APPLE : ${APPLE}")
message("WIN32 : ${WIN32}")

message("MSVC : ${MSVC}")
message("MSYS : ${MSYS}")
message("MINGW : ${MINGW}")
message("CYGWIN : ${CYGWIN}")

message("CMAKE_BUILD_TYPE : ${CMAKE_BUILD_TYPE}")
message("CMAKE_CONFIGURATION_TYPES : ${CMAKE_CONFIGURATION_TYPES}") # When using a multi-configuration generator, such as the one for Visual Studio, this variable contains a list of the available configurations.

message("LIB_TYPE : ${LIB_TYPE}")
message("BUILD_SHARED_LIBS : ${BUILD_SHARED_LIBS}")
message("CMAKE_SHARED_LINKER_FLAGS : ${CMAKE_SHARED_LINKER_FLAGS}")

message("CMAKE_COMPILER_IS_GNUCC : ${CMAKE_COMPILER_IS_GNUCC}")
message("CMAKE_COMPILER_IS_GNUCXX : ${CMAKE_COMPILER_IS_GNUCXX}")

# CMAKE_MODULE_PATH # tell CMake to search first in directories listed in CMAKE_MODULE_PATH when you use FIND_PACKAGE() or INCLUDE()

message("CMAKE_C_COMPILER_ID : ${CMAKE_C_COMPILER_ID}") # Clang GNU Intel MSVC
message("CMAKE_C_COMPILER_VERSION : ${CMAKE_C_COMPILER_VERSION}")
message("CMAKE_C_COMPILE_FLAGS : ${CMAKE_C_COMPILE_FLAGS}")
message("CMAKE_C_FLAGS : ${CMAKE_C_FLAGS}")
message("CMAKE_C_FLAGS_DEBUG : ${CMAKE_C_FLAGS_DEBUG}")
message("CMAKE_C_FLAGS_RELEASE : ${CMAKE_C_FLAGS_RELEASE}")
message("CMAKE_C_FLAGS_MINSIZEREL : ${CMAKE_C_FLAGS_MINSIZEREL}")
message("CMAKE_C_FLAGS_RELWITHDEBINFO : ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
message("CMAKE_C_OUTPUT_EXTENSION : ${CMAKE_C_OUTPUT_EXTENSION}")

message("CMAKE_CXX_COMPILER_ID : ${CMAKE_CXX_COMPILER_ID}") # Clang GNU Intel MSVC
message("CMAKE_CXX_COMPILER_VERSION : ${CMAKE_CXX_COMPILER_VERSION}")
message("CMAKE_CXX_COMPILE_FLAGS : ${CMAKE_CXX_COMPILE_FLAGS}")
message("CMAKE_CXX_FLAGS : ${CMAKE_CXX_FLAGS}")
message("CMAKE_CXX_FLAGS_DEBUG : ${CMAKE_CXX_FLAGS_DEBUG}")
message("CMAKE_CXX_FLAGS_RELEASE : ${CMAKE_CXX_FLAGS_RELEASE}")
message("CMAKE_CXX_FLAGS_MINSIZEREL : ${CMAKE_CXX_FLAGS_MINSIZEREL}")
message("CMAKE_CXX_FLAGS_RELWITHDEBINFO : ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
message("CMAKE_CXX_OUTPUT_EXTENSION : ${CMAKE_C_OUTPUT_EXTENSION}")

message("CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}") # this is the directory which contains the top-level CMakeLists.txt, i.e. the top level source directory
MESSAGE("CMAKE_CURRENT_LIST_FILE : ${CMAKE_CURRENT_LIST_FILE}") # this is the filename including the complete path of the file where this variable is used.
MESSAGE("CMAKE_CURRENT_LIST_LINE : ${CMAKE_CURRENT_LIST_LINE}") # this is linenumber where the variable is used
MESSAGE("CMAKE_INCLUDE_PATH : ${CMAKE_INCLUDE_PATH}") # this is used when searching for include files e.g. using the FIND_PATH() command.
MESSAGE("CMAKE_LIBRARY_PATH : ${CMAKE_LIBRARY_PATH}") # this is used when searching for libraries e.g. using the FIND_LIBRARY() command.
MESSAGE("CMAKE_SKIP_RPATH : ${CMAKE_SKIP_RPATH}") # If set, runtime paths are not added when using shared libraries. Default it is set to OFF
MESSAGE("CMAKE_VERBOSE_MAKEFILE : ${CMAKE_VERBOSE_MAKEFILE}")
MESSAGE("CMAKE_SUPPRESS_REGENERATION : ${CMAKE_SUPPRESS_REGENERATION}")


get_property(mewmew GLOBAL PROPERTY CMAKE_CXX_KNOWN_FEATURES)
message("get_property(mewmew GLOBAL PROPERTY CMAKE_CXX_KNOWN_FEATURES) : ${mewmew}")
#MESSAGE("CMAKE_CXX_COMPILE_FEATURES : ${CMAKE_CXX_COMPILE_FEATURES}")

# Prefixes, Suffixes (Postfixes), and Extensions
# CMAKE_EXECUTABLE_SUFFIX : Suffix of executables on the target platform.
# CMAKE_FIND_LIBRARY_PREFIXES : List of possible library prefixes used by find_library(). "lib" on UNIX systems.
# CMAKE_FIND_LIBRARY_SUFFIXES : List of possible library suffixes used by find_library(). ".a;.so" on UNIX systems. Note that it's possible to use this to control whether find_package() modules find shared or static libraries.
# CMAKE_<SHARED|STATIC>_LIBRARY_PREFIX : Prefix for shared or static libraries on this platform. Read-only.
# CMAKE_<SHARED|STATIC>_LIBRARY_SUFFIX : Suffix for shared or static libraries on this platform. Read-only.
# CMAKE__POSTFIX : Adds a custom "postfix" to static and shared libraries when in a certain build type. Example: Setting CMAKE_BUILD_TYPE=DEBUG and CMAKE_DEBUG_POSTFIX="_d" would turn mylib.lib into mylib_d.lib.

# CMAKE_BINARY_DIR  if you are building in-source, this is the same as CMAKE_SOURCE_DIR, otherwise this is the top level directory of your build tree
# CMAKE_CURRENT_BINARY_DIR if you are building in-source, this is the same as CMAKE_CURRENT_SOURCE_DIR, otherwise this is the directory where the compiled or generated files from the current CMakeLists.txt will go to
# CMAKE_CURRENT_SOURCE_DIR # this is the directory where the currently processed CMakeLists.txt is located in
# CMAKE_CURRENT_LIST_FILE
# CMAKE_CURRENT_LIST_DIR
# CMAKE_CURRENT_LIST_LINE
# CMAKE_FILES_DIRECTORY
# PROJECT_BINARY_DIR : contains the full path to the top level directory of your build tree
# PROJECT_SOURCE_DIR : contains the full path to the root of your project source directory, i.e. to the nearest directory where CMakeLists.txt contains the PROJECT() command

# System & Compiler Information
# CMAKE_VERBOSE_MAKEFILE : set this to true if you are using makefiles and want to see the full compile and link commands instead of only the shortened ones
# CMAKE_SUPPRESS_REGENERATION : this will cause CMake to not put in the rules that re-run CMake. This might be useful if you want to use the generated build files on another machine.
# CMAKE_COLOR_MAKEFILE : create Makefiles with colored output (defaults to on)

# Compilers and Tools
# CMake will create by default the following variables when using a single-configuration generator:
# - None (CMAKE_C_FLAGS or CMAKE_CXX_FLAGS used)
# - Debug (CMAKE_C_FLAGS_DEBUG or CMAKE_CXX_FLAGS_DEBUG)
# - Release (CMAKE_C_FLAGS_RELEASE or CMAKE_CXX_FLAGS_RELEASE)
# - RelWithDebInfo (CMAKE_C_FLAGS_RELWITHDEBINFO or CMAKE_CXX_FLAGS_RELWITHDEBINFO)
# - MinSizeRel (CMAKE_C_FLAGS_MINSIZEREL or CMAKE_CXX_FLAGS_MINSIZEREL)
# You can use these default compilation flags (or modify them) by setting the CMAKE_BUILD_TYPE variable at configuration time on the command line, or from within the "ccmake" GUI. Note! The default values for
# these flags change with different compilers. If CMake does not know your compiler, the contents will be empty.
# If you are using the Makefile generator, you can create your own build type like this:
    # SET(CMAKE_BUILD_TYPE distribution)
    # SET(CMAKE_CXX_FLAGS_DISTRIBUTION "-O3")
    # SET(CMAKE_C_FLAGS_DISTRIBUTION "-O3")
# Note that CMAKE_BUILD_TYPE is not initialized with a readable value at configuration time. This is because the user is free to select a build
# type at build time. Use CMAKE_CFG_INTDIR if you need a variable that evaluates to the correct build time directory.

MESSAGE("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-")
