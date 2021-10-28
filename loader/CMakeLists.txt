# ~~~
# Copyright (c) 2014-2021 The Khronos Group Inc.
# Copyright (c) 2014-2021 Valve Corporation
# Copyright (c) 2014-2021 LunarG, Inc.
# Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/generated ${CMAKE_CURRENT_BINARY_DIR})

# Get version of the API the generated code used and put it into a cmake variable LOADER_GENERATED_HEADER_VERSION
include(generated/loader_generated_header_version.cmake)

# Check for the existance of the secure_getenv or __secure_getenv commands
include(CheckFunctionExists)
include(CheckIncludeFile)

check_function_exists(secure_getenv HAVE_SECURE_GETENV)
check_function_exists(__secure_getenv HAVE___SECURE_GETENV)
if(NOT (HAVE_SECURE_GETENV OR HAVE__SECURE_GETENV))
    message(WARNING "Using non-secure environmental lookups. This loader will not properly disable environent variables when run with elevated permissions.")
endif()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/loader_cmake_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/loader_cmake_config.h)

if(WIN32)
    set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS VK_USE_PLATFORM_WIN32_KHR WIN32_LEAN_AND_MEAN)
    if(MSVC AND NOT MSVC_VERSION LESS 1900)
        # Enable control flow guard
        message(STATUS "Building loader with control flow guard")
        set(MSVC_LOADER_COMPILE_OPTIONS ${MSVC_LOADER_COMPILE_OPTIONS} /guard:cf)
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /guard:cf")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /guard:cf")
    endif()
elseif(ANDROID)
    set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS VK_USE_PLATFORM_ANDROID_KHR)
elseif(APPLE)
    set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS VK_USE_PLATFORM_MACOS_MVK VK_USE_PLATFORM_METAL_EXT)
elseif(UNIX AND NOT APPLE) # i.e.: Linux
    if(BUILD_WSI_XCB_SUPPORT)
    set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS VK_USE_PLATFORM_XCB_KHR)
    endif()

    if(BUILD_WSI_XLIB_SUPPORT)
        set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS VK_USE_PLATFORM_XLIB_KHR VK_USE_PLATFORM_XLIB_XRANDR_EXT)
    endif()

    if(BUILD_WSI_WAYLAND_SUPPORT)
        set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS VK_USE_PLATFORM_WAYLAND_KHR)
    endif()

    if(BUILD_WSI_DIRECTFB_SUPPORT)
        set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS VK_USE_PLATFORM_DIRECTFB_EXT)
    endif()

    if(BUILD_WSI_SCREEN_QNX_SUPPORT)
        set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS VK_USE_PLATFORM_SCREEN_QNX)
    endif()
else()
    message(FATAL_ERROR "Unsupported Platform!")
endif()

# DEBUG enables runtime loader ICD verification
set(CMAKE_C_FLAGS_DEBUG
        "${CMAKE_C_FLAGS_DEBUG} -DDEBUG -DGIT_BRANCH_NAME=${GIT_BRANCH_NAME} -DGIT_TAG_INFO=${GIT_TAG_INFO}")
set(CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -DGIT_BRANCH_NAME=${GIT_BRANCH_NAME} -DGIT_TAG_INFO=${GIT_TAG_INFO}")

if(WIN32)
    if(MSVC)
    # Use static MSVCRT libraries
    foreach(configuration
            in
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_MINSIZEREL
            CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_MINSIZEREL
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if(${configuration} MATCHES "/MD")
            string(REGEX
                   REPLACE "/MD"
                           "/MT"
                           ${configuration}
                           "${${configuration}}")
        endif()
    endforeach()
    endif()

    if(ENABLE_WIN10_ONECORE)
        # Note: When linking your app or driver to OneCore.lib, be sure to remove any links to non-umbrella libs (such as
        # kernel32.lib).
        set(CMAKE_CXX_STANDARD_LIBRARIES " ") # space is intentional
        set(CMAKE_C_STANDARD_LIBRARIES ${CMAKE_CXX_STANDARD_LIBRARIES})
    endif()

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
    # ~~~
    # Build dev_ext_trampoline.c and unknown_ext_chain.c with /O2 to allow tail-call optimization.
    # Setup two CMake targets (loader-norm and loader-opt) for the different compilation flags.
    # ~~~
    set(MODIFIED_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})

    string(REPLACE "/Od" "/O2" MODIFIED_C_FLAGS_DEBUG ${MODIFIED_C_FLAGS_DEBUG})
    string(REPLACE "/Ob0" "/Ob2" MODIFIED_C_FLAGS_DEBUG ${MODIFIED_C_FLAGS_DEBUG})
    string(REGEX REPLACE "/RTC." "" MODIFIED_C_FLAGS_DEBUG ${MODIFIED_C_FLAGS_DEBUG})  #remove run-time error checks

    separate_arguments(MODIFIED_C_FLAGS_DEBUG WINDOWS_COMMAND ${MODIFIED_C_FLAGS_DEBUG})

    # ~~~
    # Setup the loader.rc flie to contain the correct info
    # Optionally uses the BUILD_DLL_VERSIONINFO build option to allow setting the exact build version
    # Adds "Dev Build" to any build without the BUILD_DLL_VERSIONINFO option set
    # ~~~
    if ("$CACHE{BUILD_DLL_VERSIONINFO}" STREQUAL "")
        # default case - use 0 as the BUILDNO
        set(LOADER_RC_VERSION "1.0.1111.2222")
        set(LOADER_VER_FILE_VERSION_STR "\"${LOADER_RC_VERSION}.Dev Build\"")
        set(LOADER_VER_FILE_DESCRIPTION_STR "\"Vulkan Loader - Dev Build\"")
    else()
        set(LOADER_RC_VERSION "$CACHE{BUILD_DLL_VERSIONINFO}")
        set(LOADER_VER_FILE_VERSION_STR "\"${LOADER_RC_VERSION}\"")
        set(LOADER_VER_FILE_DESCRIPTION_STR "\"Vulkan Loader\"")
    endif()

    # RC file wants the value of FILEVERSION to separated by commas
    string(REPLACE "." ", " LOADER_VER_FILE_VERSION "${LOADER_RC_VERSION}")

    # Configure the file to include the versioning info
    # Place it in the current directory for check-in - so the GN build has up to date info
    configure_file(loader.rc.in ${CMAKE_CURRENT_LIST_DIR}/loader.rc)
endif()

set(NORMAL_LOADER_SRCS
    allocation.c
    cJSON.c
    debug_utils.c
    extension_manual.c
    get_environment.c
    gpa_helper.c
    loader.c
    log.c
    murmurhash.c
    terminator.c
    trampoline.c
    wsi.c
    )

if(WIN32)
    set(NORMAL_LOADER_SRCS ${NORMAL_LOADER_SRCS} loader_windows.c)
endif()

set(OPT_LOADER_SRCS dev_ext_trampoline.c phys_dev_ext.c)

# Check for assembler support
set(ASM_FAILURE_MSG "The build will fall back on building with C code\n")
set(ASM_FAILURE_MSG "${ASM_FAILURE_MSG}Note that this may be unsafe, as the C code requires tail-call optimizations to remove")
set(ASM_FAILURE_MSG "${ASM_FAILURE_MSG} the stack frame for certain calls. If the compiler does not do this, then unknown device")
set(ASM_FAILURE_MSG "${ASM_FAILURE_MSG} extensions will suffer from a corrupted stack.")
if(WIN32)
    if(MINGW)
        find_program(JWASM_FOUND jwasm)
        if (JWASM_FOUND)
            set(CMAKE_ASM_MASM_COMPILER ${JWASM_FOUND})
            execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpmachine OUTPUT_VARIABLE COMPILER_VERSION_OUTPUT)
            if (COMPILER_VERSION_OUTPUT)
                if (COMPILER_VERSION_OUTPUT MATCHES "x86_64")
                    set(JWASM_FLAGS -win64)
                else()
                    set(JWASM_FLAGS -coff)
                endif()
            endif()
        endif()
    endif()
    option(USE_MASM "Use MASM" ON)
    if (USE_MASM)
      enable_language(ASM_MASM)
    endif ()
    if(CMAKE_ASM_MASM_COMPILER_WORKS OR JWASM_FOUND)
        if(MINGW)
            set(CMAKE_ASM_MASM_FLAGS ${CMAKE_ASM_MASM_FLAGS} ${JWASM_FLAGS})
        elseif(NOT CMAKE_CL_64 AND NOT JWASM_FOUND)
            set(CMAKE_ASM_MASM_FLAGS ${CMAKE_ASM_MASM_FLAGS} /safeseh)
        endif()

        add_executable(asm_offset asm_offset.c)
        target_link_libraries(asm_offset Vulkan::Headers)
        add_custom_command(OUTPUT gen_defines.asm DEPENDS asm_offset COMMAND asm_offset MASM)
        add_custom_target(loader_asm_gen_files DEPENDS gen_defines.asm)
        set_target_properties(loader_asm_gen_files PROPERTIES FOLDER ${LOADER_HELPER_FOLDER})
        add_library(loader-unknown-chain OBJECT unknown_ext_chain_masm.asm)
        target_link_libraries(loader-unknown-chain Vulkan::Headers)
        add_dependencies(loader-unknown-chain loader_asm_gen_files)
    else()
        message(WARNING "Could not find working MASM assebler\n${ASM_FAILURE_MSG}")
        add_custom_target(loader_asm_gen_files)
        add_library(loader-unknown-chain OBJECT unknown_ext_chain.c)
        target_link_libraries(loader-unknown-chain Vulkan::Headers)
        set_target_properties(loader-unknown-chain PROPERTIES CMAKE_C_FLAGS_DEBUG "${MODIFIED_C_FLAGS_DEBUG}")
        target_compile_options(loader-unknown-chain PUBLIC ${MSVC_LOADER_COMPILE_OPTIONS})
    endif()
elseif(APPLE)
    # For MacOS, use the C code and force the compiler's tail-call optimization instead of using assembly code.
    set(OPT_LOADER_SRCS ${OPT_LOADER_SRCS} unknown_ext_chain.c)
    set_source_files_properties(${OPT_LOADER_SRCS} PROPERTIES COMPILE_FLAGS -O)
    add_custom_target(loader_asm_gen_files) # This causes no assembly files to be generated.
else(UNIX AND NOT APPLE) # i.e.: Linux
    option(USE_GAS "Use GAS" ON)
    if(USE_GAS)
        enable_language(ASM)

        set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS}")
        set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

        if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "aarch64")
            try_compile(ASSEMBLER_WORKS ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/asm_test_aarch64.S)
            if(ASSEMBLER_WORKS)
                set(OPT_LOADER_SRCS ${OPT_LOADER_SRCS} unknown_ext_chain_gas_aarch64.S)
            endif()
        elseif(${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86_64" OR ${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86")
            check_include_file("cet.h" HAVE_CET_H)
            if(HAVE_CET_H)
                set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS HAVE_CET_H)
            endif()

            try_compile(ASSEMBLER_WORKS ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/asm_test_x86.S)
            if(ASSEMBLER_WORKS)
                set(OPT_LOADER_SRCS ${OPT_LOADER_SRCS} unknown_ext_chain_gas_x86.S)
            endif()
        endif()
    endif()

    if(ASSEMBLER_WORKS)
        add_executable(asm_offset asm_offset.c)
        target_link_libraries(asm_offset Vulkan::Headers)
        add_custom_command(OUTPUT gen_defines.asm DEPENDS asm_offset COMMAND asm_offset GAS)
        add_custom_target(loader_asm_gen_files DEPENDS gen_defines.asm)
    else()
        if(USE_GAS)
            message(WARNING "Could not find working ${CMAKE_HOST_SYSTEM_PROCESSOR} GAS assembler\n${ASM_FAILURE_MSG}")
        else()
            message(WARNING "Assembly sources have been disabled\n${ASM_FAILURE_MSG}")
        endif()
        set(OPT_LOADER_SRCS ${OPT_LOADER_SRCS} unknown_ext_chain.c)
        add_custom_target(loader_asm_gen_files)
    endif()
endif()

if(WIN32)
    add_library(loader-norm OBJECT ${NORMAL_LOADER_SRCS} dirent_on_windows.c)
    # target_compile_options(loader-norm PUBLIC "$<$<CONFIG:DEBUG>:${LOCAL_C_FLAGS_DBG}>")
    target_compile_options(loader-norm PUBLIC ${MSVC_LOADER_COMPILE_OPTIONS})
    target_include_directories(loader-norm PRIVATE "$<TARGET_PROPERTY:Vulkan::Headers,INTERFACE_INCLUDE_DIRECTORIES>")

    add_library(loader-opt OBJECT ${OPT_LOADER_SRCS})
    add_dependencies(loader-opt loader_asm_gen_files)
    set_target_properties(loader-opt PROPERTIES CMAKE_C_FLAGS_DEBUG "${MODIFIED_C_FLAGS_DEBUG}")
    target_compile_options(loader-opt PUBLIC ${MSVC_LOADER_COMPILE_OPTIONS})
    target_include_directories(loader-opt PRIVATE "$<TARGET_PROPERTY:Vulkan::Headers,INTERFACE_INCLUDE_DIRECTORIES>")

    add_library(vulkan
                SHARED
                $<TARGET_OBJECTS:loader-opt>
                $<TARGET_OBJECTS:loader-norm>
                $<TARGET_OBJECTS:loader-unknown-chain>
                ${CMAKE_CURRENT_SOURCE_DIR}/vulkan-1.def
                ${CMAKE_CURRENT_LIST_DIR}/loader.rc)

    if (UPDATE_DEPS)
        add_dependencies(vulkan vl_update_deps)
    endif()

    # when adding the suffix the import and runtime library names must be consistent
    # mingw: libvulkan-1.dll.a / libvulkan-1.dll
    # msvc: vulkan-1.lib / vulkan-1.dll
    set_target_properties(vulkan
                          PROPERTIES
                          OUTPUT_NAME vulkan-1)

    target_link_libraries(vulkan Vulkan::Headers)

    if(MSVC AND ENABLE_WIN10_ONECORE)
        target_link_libraries(vulkan OneCoreUAP.lib LIBCMT.LIB LIBCMTD.LIB LIBVCRUNTIME.LIB LIBUCRT.LIB)
        set_target_properties(vulkan PROPERTIES LINK_FLAGS "/NODEFAULTLIB")
    else()
        target_link_libraries(vulkan cfgmgr32)
    endif()

    add_dependencies(vulkan loader_asm_gen_files)

else()
    # Linux and MacOS
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wpointer-arith")

    # Clang (and not gcc) warns about redefining a typedef with the same types, so disable that warning. Note that it will still
    # throw an error if a typedef is redefined with a different type.
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-typedef-redefinition")
    endif()

    if(APPLE AND BUILD_STATIC_LOADER)
        add_library(vulkan STATIC ${NORMAL_LOADER_SRCS} ${OPT_LOADER_SRCS})
    else()
        add_library(vulkan SHARED ${NORMAL_LOADER_SRCS} ${OPT_LOADER_SRCS})
    endif()
    add_dependencies(vulkan loader_asm_gen_files)
    # set version based on LOADER_GENERATED_HEADER_VERSION used to generate the code
    set_target_properties(vulkan
                          PROPERTIES SOVERSION "1"
                          VERSION ${LOADER_GENERATED_HEADER_VERSION})
    target_link_libraries(vulkan ${CMAKE_DL_LIBS} m)
    if (NOT ANDROID)
        target_link_libraries(vulkan pthread)
    endif()
    target_link_libraries(vulkan Vulkan::Headers)
    # Used to make alloca() and secure_getenv() available
    target_compile_definitions(vulkan PRIVATE _GNU_SOURCE)
    if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
        target_compile_definitions(vulkan PRIVATE __BSD_VISIBLE=1)
        target_compile_definitions(asm_offset PRIVATE __BSD_VISIBLE=1)
    endif()
    if(APPLE)
        find_library(COREFOUNDATION_LIBRARY NAMES CoreFoundation)
        target_link_libraries(vulkan "-framework CoreFoundation")

        # Build vulkan.framework
        set(FRAMEWORK_HEADERS
            ${VulkanHeaders_INCLUDE_DIRS}/vulkan/vk_icd.h
            ${VulkanHeaders_INCLUDE_DIRS}/vulkan/vk_layer.h
            ${VulkanHeaders_INCLUDE_DIRS}/vulkan/vk_platform.h
            ${VulkanHeaders_INCLUDE_DIRS}/vulkan/vk_sdk_platform.h
            ${VulkanHeaders_INCLUDE_DIRS}/vulkan/vulkan_android.h
            ${VulkanHeaders_INCLUDE_DIRS}/vulkan/vulkan_core.h
            ${VulkanHeaders_INCLUDE_DIRS}/vulkan/vulkan_ios.h
            ${VulkanHeaders_INCLUDE_DIRS}/vulkan/vulkan_macos.h
            ${VulkanHeaders_INCLUDE_DIRS}/vulkan/vulkan_vi.h
            ${VulkanHeaders_INCLUDE_DIRS}/vulkan/vulkan_wayland.h
            ${VulkanHeaders_INCLUDE_DIRS}/vulkan/vulkan_win32.h
            ${VulkanHeaders_INCLUDE_DIRS}/vulkan/vulkan_xcb.h
            ${VulkanHeaders_INCLUDE_DIRS}/vulkan/vulkan_xlib.h
            ${VulkanHeaders_INCLUDE_DIRS}/vulkan/vulkan_xlib_xrandr.h
            ${VulkanHeaders_INCLUDE_DIRS}/vulkan/vulkan_screen.h
            ${VulkanHeaders_INCLUDE_DIRS}/vulkan/vulkan.h
            ${VulkanHeaders_INCLUDE_DIRS}/vulkan/vulkan.hpp)
        if(BUILD_STATIC_LOADER)
            add_library(vulkan-framework STATIC ${NORMAL_LOADER_SRCS} ${OPT_LOADER_SRCS} ${FRAMEWORK_HEADERS})
        else()
            add_library(vulkan-framework SHARED ${NORMAL_LOADER_SRCS} ${OPT_LOADER_SRCS} ${FRAMEWORK_HEADERS})
        endif()
        add_dependencies(vulkan-framework loader_asm_gen_files)
        target_link_libraries(vulkan-framework -ldl -lpthread -lm "-framework CoreFoundation")
        target_link_libraries(vulkan-framework Vulkan::Headers)

        # The FRAMEWORK_VERSION needs to be "A" here so that Xcode code-signing works when a user adds their framework to an Xcode
        # project and does "Sign on Copy". It would have been nicer to use "1" to denote Vulkan 1. Although Apple docs say that a
        # framework version does not have to be "A", this part of the Apple toolchain expects it.
        # https://forums.developer.apple.com/thread/65963

# cmake-format: off
        set_target_properties(vulkan-framework PROPERTIES
            OUTPUT_NAME vulkan
            FRAMEWORK TRUE
            FRAMEWORK_VERSION A
            VERSION "${LOADER_GENERATED_HEADER_VERSION}" # "current version"
            SOVERSION "1.0.0"                        # "compatibility version"
            MACOSX_FRAMEWORK_IDENTIFIER com.lunarg.vulkanFramework
            PUBLIC_HEADER "${FRAMEWORK_HEADERS}"
        )
        install(TARGETS vulkan-framework
            PUBLIC_HEADER DESTINATION vulkan
            FRAMEWORK DESTINATION loader
        )
# cmake-format: on
    endif()

    if(NOT APPLE)
        target_compile_definitions(vulkan PRIVATE _XOPEN_SOURCE=500) # hush compiler warnings for readlink
    endif()
endif()

# Generate pkg-config file.
include(FindPkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    set(VK_API_VERSION "${LOADER_GENERATED_HEADER_VERSION}")
    foreach(LIB ${CMAKE_CXX_IMPLICIT_LINK_LIBRARIES} ${PLATFORM_LIBS})
        set(PRIVATE_LIBS "${PRIVATE_LIBS} -l${LIB}")
    endforeach()
    if(WIN32)
        set(VULKAN_LIB_SUFFIX "-1")
        # Set libdir path as in cmake's FindVulkan.cmake
        # https://github.com/KhronosGroup/Vulkan-Loader/issues/668
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            string(REPLACE "lib" "Lib" CMAKE_INSTALL_FULL_LIBDIR_PC ${CMAKE_INSTALL_FULL_LIBDIR})
        else()
            string(REPLACE "lib" "Lib32" CMAKE_INSTALL_FULL_LIBDIR_PC ${CMAKE_INSTALL_FULL_LIBDIR})
        endif()
    else()
        set(CMAKE_INSTALL_FULL_LIBDIR_PC ${CMAKE_INSTALL_FULL_LIBDIR})
    endif ()
    configure_file("vulkan.pc.in" "vulkan.pc" @ONLY)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/vulkan.pc" DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
endif()

target_link_libraries(vulkan Vulkan::Headers)
add_library(Vulkan::Vulkan ALIAS vulkan)

install(TARGETS vulkan
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
