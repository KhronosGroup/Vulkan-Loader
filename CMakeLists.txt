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

cmake_minimum_required(VERSION 3.10.2)

# Apple: Must be set before enable_language() or project() as it may influence configuration of the toolchain and flags.
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment version")

# If we are building in Visual Studio 2015 and with a CMake version 3.19 or greater, we need to set this variable
# so that CMake will choose a Windows SDK version higher than 10.0.14393.0, as dxgi1_6.h is only found in Windows SDK
# 10.0.17763 and higher.
set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION_MAXIMUM OFF)

project(Vulkan-Loader)

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
    message("* NOTE: Adding target vl_update_deps to run as needed for updating            *")
    message("*       dependencies.                                                          *")
    message("********************************************************************************")

    # Add a target so that update_deps.py will run when necessary
    # NOTE: This is triggered off of the timestamps of known_good.json and helper.cmake
    add_custom_command(OUTPUT ${CMAKE_CURRENT_LIST_DIR}/external/helper.cmake
                       COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/scripts/update_deps.py --dir ${CMAKE_CURRENT_LIST_DIR}/external --arch ${_target_arch} --config ${_build_type} --generator "${CMAKE_GENERATOR}" ${_build_tests_arg}
                       DEPENDS ${CMAKE_CURRENT_LIST_DIR}/scripts/known_good.json)

    add_custom_target(vl_update_deps DEPENDS ${CMAKE_CURRENT_LIST_DIR}/external/helper.cmake)

    # Check if update_deps.py needs to be run on first cmake run
    if (${CMAKE_CURRENT_LIST_DIR}/scripts/known_good.json IS_NEWER_THAN ${CMAKE_CURRENT_LIST_DIR}/external/helper.cmake)
        execute_process(
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/scripts/update_deps.py --dir ${CMAKE_CURRENT_LIST_DIR}/external --arch ${_target_arch} --config ${_build_type} --generator "${CMAKE_GENERATOR}" ${_build_tests_arg}
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            RESULT_VARIABLE _update_deps_result
        )
        if (NOT (${_update_deps_result} EQUAL 0))
            message(FATAL_ERROR "Could not run update_deps.py which is necessary to download dependencies.")
        endif()
    endif()
    include(${CMAKE_CURRENT_LIST_DIR}/external/helper.cmake)
else()
    message("********************************************************************************")
    message("* NOTE: Not adding target to run update_deps.py automatically.                 *")
    message("********************************************************************************")
    find_package(PythonInterp 3 QUIET)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(PythonInterp 3 QUIET)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

option(BUILD_TESTS "Build Tests" OFF)

if(BUILD_TESTS)
    enable_testing()
endif()

if(APPLE)
    option(BUILD_STATIC_LOADER "Build a loader that can be statically linked" OFF)
endif()

if(WIN32)
    # Optional: Allow specify the exact version used in the loader dll
    # Format is major.minor.patch.build
    set(BUILD_DLL_VERSIONINFO "" CACHE STRING "Set the version to be used in the loader.rc file. Default value is the currently generated header version")
endif()

if(BUILD_STATIC_LOADER)
    message(WARNING "The BUILD_STATIC_LOADER option has been set. Note that this will only work on MacOS and is not supported "
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

set(GIT_BRANCH_NAME "--unknown--")
set(GIT_TAG_INFO "--unknown--")
find_package (Git)
if (GIT_FOUND AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/.git/HEAD")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --always
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        OUTPUT_VARIABLE GIT_TAG_INFO)
    string(REGEX REPLACE "\n$" "" GIT_TAG_INFO "${GIT_TAG_INFO}")

    file(READ "${CMAKE_CURRENT_LIST_DIR}/.git/HEAD" GIT_HEAD_REF_INFO)
    if (GIT_HEAD_REF_INFO)
        string(REGEX MATCH "ref: refs/heads/(.*)" _ ${GIT_HEAD_REF_INFO})
        if (CMAKE_MATCH_1)
            set(GIT_BRANCH_NAME ${CMAKE_MATCH_1})
        else()
            set(GIT_BRANCH_NAME ${GIT_HEAD_REF_INFO})
        endif()
        string(REGEX REPLACE "\n$" "" GIT_BRANCH_NAME "${GIT_BRANCH_NAME}")
    endif()
endif()

if(WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    # Windows: if install locations not set by user, set install prefix to "<build_dir>\install".
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE)
endif()

# Enable IDE GUI folders.  "Helper targets" that don't have interesting source code should set their FOLDER property to this
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(LOADER_HELPER_FOLDER "Helper Targets")

if(UNIX)
    set(FALLBACK_CONFIG_DIRS "/etc/xdg" CACHE STRING
            "Search path to use when XDG_CONFIG_DIRS is unset or empty or the current process is SUID/SGID. Default is freedesktop compliant.")
    set(FALLBACK_DATA_DIRS "/usr/local/share:/usr/share" CACHE STRING
            "Search path to use when XDG_DATA_DIRS is unset or empty or the current process is SUID/SGID. Default is freedesktop compliant.")
    set(SYSCONFDIR "" CACHE STRING
            "System-wide search directory. If not set or empty, CMAKE_INSTALL_FULL_SYSCONFDIR and /etc are used.")
endif()

# Because we use CMake 3.10.2, we can't use the policy which would disable adding /W3 by default. In the interim, replace the flags
# When this project is updated to 3.15 and above, use the following line.
#     cmake_policy(SET CMP0092 NEW)
string(REGEX REPLACE "/W3" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REGEX REPLACE "/W3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# For MSVC/Windows, replace /GR with an empty string, this prevents warnings of /GR being overriden by /GR-
# Newer CMake versions (3.20) have better solutions for this through policy - using the old
# way while waiting for when updating can occur
string(REPLACE "/GR" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

if(UNIX AND NOT APPLE) # i.e.: Linux
    option(BUILD_WSI_XCB_SUPPORT "Build XCB WSI support" ON)
    option(BUILD_WSI_XLIB_SUPPORT "Build Xlib WSI support" ON)
    option(BUILD_WSI_WAYLAND_SUPPORT "Build Wayland WSI support" ON)
    option(BUILD_WSI_DIRECTFB_SUPPORT "Build DirectFB WSI support" OFF)
    option(BUILD_WSI_SCREEN_QNX_SUPPORT "Build QNX Screen WSI support" OFF)

    if(BUILD_WSI_XCB_SUPPORT)
        find_package(XCB REQUIRED)
        include_directories(SYSTEM ${XCB_INCLUDE_DIR})
    endif()

    if(BUILD_WSI_XLIB_SUPPORT)
        find_package(X11 REQUIRED)
    endif()

    if(BUILD_WSI_DIRECTFB_SUPPORT)
        find_package(DirectFB REQUIRED)
        include_directories(SYSTEM ${DIRECTFB_INCLUDE_DIR})
    endif()

    if(BUILD_WSI_SCREEN_QNX_SUPPORT)
        # Part of OS, no additional include directories are required
    endif()
endif()

if(WIN32)
    option(ENABLE_WIN10_ONECORE "Link the loader with OneCore umbrella libraries" OFF)
endif()

add_library(platform_wsi_defines INTERFACE)
if(WIN32)
    target_compile_definitions(platform_wsi_defines INTERFACE VK_USE_PLATFORM_WIN32_KHR)
elseif(ANDROID)
    target_compile_definitions(platform_wsi_defines INTERFACE VK_USE_PLATFORM_ANDROID_KHR)
elseif(APPLE)
    target_compile_definitions(platform_wsi_defines INTERFACE VK_USE_PLATFORM_MACOS_MVK VK_USE_PLATFORM_METAL_EXT)
elseif(UNIX AND NOT APPLE) # i.e.: Linux
    if(BUILD_WSI_XCB_SUPPORT)
        target_compile_definitions(platform_wsi_defines INTERFACE VK_USE_PLATFORM_XCB_KHR)
    endif()
    if(BUILD_WSI_XLIB_SUPPORT)
        target_compile_definitions(platform_wsi_defines INTERFACE VK_USE_PLATFORM_XLIB_KHR VK_USE_PLATFORM_XLIB_XRANDR_EXT)
    endif()
    if(BUILD_WSI_WAYLAND_SUPPORT)
        target_compile_definitions(platform_wsi_defines INTERFACE VK_USE_PLATFORM_WAYLAND_KHR)
    endif()
    if(BUILD_WSI_DIRECTFB_SUPPORT)
        target_compile_definitions(platform_wsi_defines INTERFACE VK_USE_PLATFORM_DIRECTFB_EXT)
    endif()
    if(BUILD_WSI_SCREEN_QNX_SUPPORT)
        target_compile_definitions(platform_wsi_defines INTERFACE VK_USE_PLATFORM_SCREEN_QNX)
    endif()
else()
    message(FATAL_ERROR "Unsupported Platform!")
endif()

add_library(loader_common_options INTERFACE)
target_compile_definitions(loader_common_options INTERFACE API_NAME="Vulkan")
target_link_libraries(loader_common_options INTERFACE platform_wsi_defines)

# Enable beta Vulkan extensions
target_compile_definitions(loader_common_options INTERFACE VK_ENABLE_BETA_EXTENSIONS)

target_compile_features(loader_common_options INTERFACE c_std_99)
target_compile_features(loader_common_options INTERFACE cxx_std_11)
set(LOADER_STANDARD_C_PROPERTIES PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED YES C_EXTENSIONS OFF)
set(LOADER_STANDARD_CXX_PROPERTIES PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS OFF)

option(ENABLE_WERROR "Enable warnings as errors" ON)

# Set warnings as errors and the main diagnostic flags
# Must be set first so the warning silencing later on works properly
# Note that clang-cl.exe should use MSVC flavor flags, not GNU
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND "${CMAKE_CXX_SIMULATE_ID}" MATCHES "MSVC"))
    if (ENABLE_WERROR)
        target_compile_options(loader_common_options INTERFACE /WX)
    endif()
    target_compile_options(loader_common_options INTERFACE /W4)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using GCC or Clang with the regular front end
    if (ENABLE_WERROR)
        target_compile_options(loader_common_options INTERFACE -Werror)
    endif()
    target_compile_options(loader_common_options INTERFACE -Wall -Wextra)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(loader_common_options INTERFACE -Wno-unused-parameter -Wno-unused-function -Wno-missing-field-initializers)

    # need to prepend /clang: to compiler arguments when using clang-cl
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND "${CMAKE_CXX_SIMULATE_ID}" MATCHES "MSVC")
        target_compile_options(loader_common_options INTERFACE /clang:-fno-strict-aliasing /clang:-fno-builtin-memcmp)
    else()
        target_compile_options(loader_common_options INTERFACE -fno-strict-aliasing -fno-builtin-memcmp)
    endif()

    # For GCC version 7.1 or greater, we need to disable the implicit fallthrough warning since there's no consistent way to satisfy
    # all compilers until they all accept the C++17 standard
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(loader_common_options INTERFACE -Wno-stringop-truncation -Wno-stringop-overflow)
        if(CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 7.1)
            target_compile_options(loader_common_options INTERFACE -Wimplicit-fallthrough=0)
        endif()
    endif()

    if(UNIX)
        target_compile_options(loader_common_options INTERFACE -fvisibility=hidden)
    endif()

    target_compile_options(loader_common_options INTERFACE -Wpointer-arith)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND "${CMAKE_CXX_SIMULATE_ID}" MATCHES "MSVC"))
    # /GR-: Disable RTTI
    # /guard:cf: Enable control flow guard
    # /wd4100: Disable warning on unreferenced formal parameter
    # /wd4152: Disable warning on conversion of a function pointer to a data pointer
    # /wd4201: Disable warning on anonymous struct/unions
    target_compile_options(loader_common_options INTERFACE /GR- /guard:cf /wd4100 /wd4152 /wd4201)

    # Enable control flow guard
    if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.13.0")
        target_link_options(loader_common_options INTERFACE "LINKER:/guard:cf")
    else()
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /guard:cf")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /guard:cf")
    endif()

    # Prevent <windows.h> from polluting the code. guards against things like MIN and MAX
    target_compile_definitions(loader_common_options INTERFACE WIN32_LEAN_AND_MEAN)
endif()

# DEBUG enables runtime loader ICD verification
# Add git branch and tag info in debug mode
target_compile_definitions(loader_common_options INTERFACE $<$<CONFIG:DEBUG>:DEBUG;GIT_BRANCH_NAME="${GIT_BRANCH_NAME}";GIT_TAG_INFO="${GIT_TAG_INFO}">)

# Check for the existance of the secure_getenv or __secure_getenv commands
include(CheckFunctionExists)
include(CheckIncludeFile)

check_function_exists(secure_getenv HAVE_SECURE_GETENV)
check_function_exists(__secure_getenv HAVE___SECURE_GETENV)

if (HAVE_SECURE_GETENV)
    target_compile_definitions(loader_common_options INTERFACE HAVE_SECURE_GETENV)
endif()
if (HAVE___SECURE_GETENV)
    target_compile_definitions(loader_common_options INTERFACE HAVE___SECURE_GETENV)
endif()
if(NOT MSVC AND NOT (HAVE_SECURE_GETENV OR HAVE___SECURE_GETENV))
    message(WARNING "Using non-secure environmental lookups. This loader will not properly disable environent variables when run with elevated permissions.")
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
    target_compile_definitions(loader_common_options INTERFACE FALLBACK_CONFIG_DIRS="${FALLBACK_CONFIG_DIRS}" FALLBACK_DATA_DIRS="${FALLBACK_DATA_DIRS}")

    if(NOT (SYSCONFDIR STREQUAL ""))
        # SYSCONFDIR is specified, use it and do not force /etc.
        target_compile_definitions(loader_common_options INTERFACE SYSCONFDIR="${SYSCONFDIR}")
    else()
        target_compile_definitions(loader_common_options INTERFACE SYSCONFDIR="${CMAKE_INSTALL_FULL_SYSCONFDIR}")

        # Make sure /etc is searched by the loader
        if(NOT (CMAKE_INSTALL_FULL_SYSCONFDIR STREQUAL "/etc"))
            target_compile_definitions(loader_common_options INTERFACE EXTRASYSCONFDIR="/etc")
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

add_subdirectory(loader)

if(BUILD_TESTS)
    # Set gtest build configuration
    # Attempt to enable if it is available.
    if(TARGET gtest)
        # Already enabled as a target (perhaps by a project enclosing this one)
        message(STATUS "Vulkan-Loader/external: " "googletest already configured - using it")
    elseif(IS_DIRECTORY "${GOOGLETEST_INSTALL_DIR}/googletest")
        set(BUILD_GTEST ON CACHE BOOL "Builds the googletest subproject")
        set(BUILD_GMOCK OFF CACHE BOOL "Builds the googlemock subproject")
        set(gtest_force_shared_crt ON CACHE BOOL "Link gtest runtimes dynamically" FORCE)
        set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries")
        # The googletest directory exists, so enable it as a target.
        message(STATUS "Vulkan-Loader/external: " "googletest found - configuring it for tests")
        add_subdirectory("${GOOGLETEST_INSTALL_DIR}")
    else()
        message(SEND_ERROR "Could not find googletest directory. Be sure to run update_deps.py with the --tests option to download the appropriate version of googletest")
        set(BUILD_TESTS OFF)
    endif()
    if (WIN32)
        if(TARGET detours)
            # Already enabled as a target (perhaps by a project enclosing this one)
            message(STATUS "Vulkan-Loader/external: " "detours already configured - using it")
        else()
            if(IS_DIRECTORY ${DETOURS_INSTALL_DIR})
                # The detours directory exists, so enable it as a target.
                message(STATUS "Vulkan-Loader/external: " "detours found - configuring it for tests")
            else()
                message(SEND_ERROR "Could not find detours directory. Be sure to run update_deps.py with the --tests option to download the appropriate version of detours")
                set(BUILD_TESTS OFF)
            endif()
            add_library(detours STATIC
                ${DETOURS_INSTALL_DIR}/src/creatwth.cpp
                ${DETOURS_INSTALL_DIR}/src/detours.cpp
                ${DETOURS_INSTALL_DIR}/src/detours.h
                ${DETOURS_INSTALL_DIR}/src/detver.h
                ${DETOURS_INSTALL_DIR}/src/disasm.cpp
                ${DETOURS_INSTALL_DIR}/src/disolarm.cpp
                ${DETOURS_INSTALL_DIR}/src/disolarm64.cpp
                ${DETOURS_INSTALL_DIR}/src/disolia64.cpp
                ${DETOURS_INSTALL_DIR}/src/disolx64.cpp
                ${DETOURS_INSTALL_DIR}/src/disolx86.cpp
                ${DETOURS_INSTALL_DIR}/src/image.cpp
                ${DETOURS_INSTALL_DIR}/src/modules.cpp
                )
            target_include_directories(detours PUBLIC ${DETOURS_INSTALL_DIR}/src)

            macro(GET_WIN32_WINNT version)
                if(WIN32 AND CMAKE_SYSTEM_VERSION)
            		set(ver ${CMAKE_SYSTEM_VERSION})
            		string(REGEX MATCH "^([0-9]+).([0-9])" ver ${ver})
            		string(REGEX MATCH "^([0-9]+)" verMajor ${ver})
            		# Check for Windows 10, b/c we'll need to convert to hex 'A'.
            		if("${verMajor}" MATCHES "10")
            			set(verMajor "A")
            			string(REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver})
            		endif("${verMajor}" MATCHES "10")
            		# Remove all remaining '.' characters.
            		string(REPLACE "." "" ver ${ver})
            		# Prepend each digit with a zero.
            		string(REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver})
            		set(${version} "0x${ver}")
                endif()
            endmacro()

            set(DETOURS_MAJOR_VERSION "4")
            set(DETOURS_MINOR_VERSION "0")
            set(DETOURS_PATCH_VERSION "1")
            set(DETOURS_VERSION "${DETOURS_MAJOR_VERSION}.${DETOURS_MINOR_VERSION}.${DETOURS_PATCH_VERSION}")

            target_include_directories(detours PUBLIC ${DETOURS_INSTALL_DIR}/src)

            if(MSVC_VERSION GREATER_EQUAL 1700)
                target_compile_definitions(detours PUBLIC DETOURS_CL_17_OR_NEWER)
            endif(MSVC_VERSION GREATER_EQUAL 1700)
            GET_WIN32_WINNT(ver)
            if(ver EQUAL 0x0700)
                target_compile_definitions(detours PUBLIC _USING_V110_SDK71_ DETOURS_WIN_7)
            endif(ver EQUAL 0x0700)
            target_compile_definitions(detours PUBLIC "_WIN32_WINNT=${ver}")

            if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
                target_compile_definitions(detours PUBLIC "DETOURS_TARGET_PROCESSOR=X64" DETOURS_X64 DETOURS_64BIT _AMD64_)
            else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
                target_compile_definitions(detours PUBLIC "DETOURS_TARGET_PROCESSOR=X86" DETOURS_X86 _X86_)
            endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")

            target_compile_definitions(detours PUBLIC  "DETOURS_VERSION=0x4c0c1" WIN32_LEAN_AND_MEAN)

            if(MSVC)
                target_compile_definitions(detours PUBLIC  "_CRT_SECURE_NO_WARNINGS=1")
                set_target_properties(detours PROPERTIES COMPILE_FLAGS /EHsc)
            endif()

            # Silence errors found in clang-cl
            if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND "${CMAKE_CXX_SIMULATE_ID}" MATCHES "MSVC")
                target_compile_options(detours PRIVATE -Wno-sizeof-pointer-memaccess -Wno-microsoft-goto -Wno-microsoft-cast)
            endif()
        endif()
    endif()

    if (BUILD_TESTS)
        add_subdirectory(tests ${CMAKE_BINARY_DIR}/tests)
    endif()

endif()
