# ~~~
# Copyright (c) 2018 Valve Corporation
# Copyright (c) 2018 LunarG, Inc.
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

# Add your optional dependencies in this "external" directory.

# googletest is an optional external dependency for this repo.
if(BUILD_TESTS)
    # Set gtest build configuration
    set(BUILD_GTEST ON CACHE BOOL "Builds the googletest subproject")
    set(BUILD_GMOCK OFF CACHE BOOL "Builds the googlemock subproject")
    set(gtest_force_shared_crt ON CACHE BOOL "Link gtest runtimes dynamically")
    set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries")
    # Attempt to enable if it is available.
    if(TARGET gtest)
        # Already enabled as a target (perhaps by a project enclosing this one)
        message(STATUS "Vulkan-Loader/external: " "googletest already configured - using it")
    elseif(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/googletest")
        # The googletest directory exists, so enable it as a target.
        message(STATUS "Vulkan-Loader/external: " "googletest found - configuring it for tests")
        # EXCLUDE_FROM_ALL keeps the install target from installing GTEST files.
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/googletest" EXCLUDE_FROM_ALL)
    else()
        message(FATAL_ERROR "Vulkan-Loader/external: Google Test was not found. Please download it and place it in this folder")
    endif()
    if (WIN32)
        if(TARGET detours)
            # Already enabled as a target (perhaps by a project enclosing this one)
            message(STATUS "Vulkan-Loader/external: " "detours already configured - using it")
        else()
            if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/detours")
                # The detours directory exists, so enable it as a target.
                message(STATUS "Vulkan-Loader/external: " "detours found - configuring it for tests")
            else()
                message(FATAL_ERROR "Vulkan-Loader/external: Microsoft Detours was not found. Please download it and place it in this folder")
            endif()
            add_library(detours STATIC
                ${CMAKE_CURRENT_SOURCE_DIR}/detours/src/creatwth.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/detours/src/detours.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/detours/src/detours.h
                ${CMAKE_CURRENT_SOURCE_DIR}/detours/src/detver.h
                ${CMAKE_CURRENT_SOURCE_DIR}/detours/src/disasm.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/detours/src/disolarm.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/detours/src/disolarm64.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/detours/src/disolia64.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/detours/src/disolx64.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/detours/src/disolx86.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/detours/src/image.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/detours/src/modules.cpp
                )
            target_include_directories(detours PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/detours/src)

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

            include_directories("${CMAKE_CURRENT_SOURCE_DIR}/detours/src")

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
        endif()
    endif()
endif()
