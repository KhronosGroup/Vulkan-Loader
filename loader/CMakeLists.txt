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

# Get version of the API the generated code used and put it into a cmake variable LOADER_GENERATED_HEADER_VERSION
include(generated/loader_generated_header_version.cmake)

add_library(loader_specific_options INTERFACE)
target_link_libraries(loader_specific_options INTERFACE loader_common_options Vulkan::Headers)
target_include_directories(loader_specific_options INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/generated ${CMAKE_CURRENT_BINARY_DIR})

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

    target_compile_options(loader_specific_options INTERFACE -D_CRT_SECURE_NO_WARNINGS)
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
else()
    # Used to make alloca() and secure_getenv() available
    target_compile_definitions(loader_specific_options INTERFACE _GNU_SOURCE)
    if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
        target_compile_definitions(loader_specific_options INTERFACE __BSD_VISIBLE=1)
    endif()
    check_include_file("alloca.h" HAVE_ALLOCA_H)
    if(HAVE_ALLOCA_H)
        target_compile_definitions(loader_specific_options INTERFACE HAVE_ALLOCA_H)
    endif()
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
    terminator.c
    trampoline.c
    unknown_function_handling.c
    wsi.c
    )

if(WIN32)
    list(APPEND NORMAL_LOADER_SRCS loader_windows.c dirent_on_windows.c)
elseif(UNIX AND NOT APPLE) # i.e.: Linux
    list(APPEND NORMAL_LOADER_SRCS loader_linux.c)
    target_compile_definitions(loader_specific_options INTERFACE LOADER_ENABLE_LINUX_SORT)
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
        target_link_libraries(asm_offset loader_specific_options)
        add_custom_command(OUTPUT gen_defines.asm DEPENDS asm_offset COMMAND asm_offset MASM)
        add_custom_target(loader_asm_gen_files DEPENDS gen_defines.asm)
        set_target_properties(loader_asm_gen_files PROPERTIES FOLDER ${LOADER_HELPER_FOLDER})
        add_library(loader-unknown-chain OBJECT unknown_ext_chain_masm.asm)
        target_link_libraries(loader-unknown-chain Vulkan::Headers)
        target_include_directories(loader-unknown-chain PUBLIC $<TARGET_PROPERTY:loader_asm_gen_files,BINARY_DIR>)
        add_dependencies(loader-unknown-chain loader_asm_gen_files)
    else()
        message(WARNING "Could not find working MASM assembler\n${ASM_FAILURE_MSG}")
        add_custom_target(loader_asm_gen_files)
        add_library(loader-unknown-chain OBJECT unknown_ext_chain.c)
        target_link_libraries(loader-unknown-chain loader_specific_options)
        set_target_properties(loader-unknown-chain PROPERTIES CMAKE_C_FLAGS_DEBUG "${MODIFIED_C_FLAGS_DEBUG}")
    endif()
elseif(APPLE)
    # For MacOS, use the C code and force the compiler's tail-call optimization instead of using assembly code.
    set(OPT_LOADER_SRCS ${OPT_LOADER_SRCS} unknown_ext_chain.c)
    set_source_files_properties(${OPT_LOADER_SRCS} PROPERTIES COMPILE_FLAGS -O)
    add_custom_target(loader_asm_gen_files) # This causes no assembly files to be generated.
else() # i.e.: Linux
    option(USE_GAS "Use GAS" ON)
    if(USE_GAS)
        enable_language(ASM)

        set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS}")
        set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

        if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
            try_compile(ASSEMBLER_WORKS ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/asm_test_aarch64.S)
            if(ASSEMBLER_WORKS)
                set(OPT_LOADER_SRCS ${OPT_LOADER_SRCS} unknown_ext_chain_gas_aarch64.S)
            endif()
        elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "amd64" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "^i.86$")
            check_include_file("cet.h" HAVE_CET_H)
            if(HAVE_CET_H)
                target_compile_definitions(loader_specific_options INTERFACE HAVE_CET_H)
            endif()

            try_compile(ASSEMBLER_WORKS ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/asm_test_x86.S)
            if(ASSEMBLER_WORKS)
                set(OPT_LOADER_SRCS ${OPT_LOADER_SRCS} unknown_ext_chain_gas_x86.S)
            endif()
        endif()
    endif()

    if(ASSEMBLER_WORKS)
        add_executable(asm_offset asm_offset.c)
        target_link_libraries(asm_offset loader_specific_options)
        add_custom_command(OUTPUT gen_defines.asm DEPENDS asm_offset COMMAND asm_offset GAS)
        add_custom_target(loader_asm_gen_files DEPENDS gen_defines.asm)
    else()
        if(USE_GAS)
            message(WARNING "Could not find working ${CMAKE_SYSTEM_PROCESSOR} GAS assembler\n${ASM_FAILURE_MSG}")
        else()
            message(WARNING "Assembly sources have been disabled\n${ASM_FAILURE_MSG}")
        endif()
        set(OPT_LOADER_SRCS ${OPT_LOADER_SRCS} unknown_ext_chain.c)
        add_custom_target(loader_asm_gen_files)
    endif()
endif()

if(WIN32)
    add_library(loader-opt STATIC ${OPT_LOADER_SRCS})
    target_link_libraries(loader-opt PUBLIC loader_specific_options)
    add_dependencies(loader-opt loader_asm_gen_files)
    set_target_properties(loader-opt PROPERTIES CMAKE_C_FLAGS_DEBUG "${MODIFIED_C_FLAGS_DEBUG}")

    add_library(vulkan
                SHARED
                ${NORMAL_LOADER_SRCS}
                $<TARGET_OBJECTS:loader-unknown-chain>
                ${CMAKE_CURRENT_SOURCE_DIR}/vulkan-1.def
                ${CMAKE_CURRENT_LIST_DIR}/loader.rc)

    target_link_libraries(vulkan PRIVATE loader_specific_options loader-opt)

    if (UPDATE_DEPS)
        add_dependencies(vulkan vl_update_deps)
    endif()

    # when adding the suffix the import and runtime library names must be consistent
    # mingw: libvulkan-1.dll.a / vulkan-1.dll
    # msvc: vulkan-1.lib / vulkan-1.dll
    set_target_properties(vulkan
                          PROPERTIES
                          OUTPUT_NAME vulkan-1)
    if(MINGW)
        # generate the same DLL with mingw
        set_target_properties(vulkan
                              PROPERTIES
                              PREFIX "")
    endif()

    if(MSVC AND ENABLE_WIN10_ONECORE)
        target_link_libraries(vulkan PRIVATE OneCoreUAP.lib LIBCMT.LIB LIBCMTD.LIB LIBVCRUNTIME.LIB LIBUCRT.LIB)
        set_target_properties(vulkan PROPERTIES LINK_FLAGS "/NODEFAULTLIB")
    else()
       target_link_libraries(vulkan PRIVATE cfgmgr32)
    endif()

    add_dependencies(vulkan loader_asm_gen_files)

else()
    if(APPLE AND BUILD_STATIC_LOADER)
        add_library(vulkan STATIC ${NORMAL_LOADER_SRCS} ${OPT_LOADER_SRCS})
        target_compile_definitions(vulkan PRIVATE BUILD_STATIC_LOADER)
    else()
        add_library(vulkan SHARED ${NORMAL_LOADER_SRCS} ${OPT_LOADER_SRCS})
    endif()
    add_dependencies(vulkan loader_asm_gen_files)
    # set version based on LOADER_GENERATED_HEADER_VERSION used to generate the code
    set_target_properties(vulkan
                          PROPERTIES SOVERSION "1"
                          VERSION ${LOADER_GENERATED_HEADER_VERSION})
    target_link_libraries(vulkan PRIVATE ${CMAKE_DL_LIBS} m)
    if (NOT ANDROID)
        target_link_libraries(vulkan PRIVATE Threads::Threads)
    endif()

    if(APPLE)
        find_library(COREFOUNDATION_LIBRARY NAMES CoreFoundation)
        target_link_libraries(vulkan PRIVATE "-framework CoreFoundation")

        # Build vulkan.framework
        # Use GLOB_RECURSE to find all the header files and populate the vulkan.framework headers with them
        # Use CONFIGURE_DEPENDS to ensure that if the header files are updated, this list is also updated
        # Note: CONFIGURE_DEPENDS is a 3.12 feature - gate it for now and remove when CMake minimum version is higher
        if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.12.0")
            file(GLOB_RECURSE CONFIGURE_DEPENDS FRAMEWORK_HEADERS ${VulkanHeaders_INCLUDE_DIRS})
        else()
            file(GLOB_RECURSE FRAMEWORK_HEADERS ${VulkanHeaders_INCLUDE_DIRS})
        endif()
        if(BUILD_STATIC_LOADER)
            add_library(vulkan-framework STATIC ${NORMAL_LOADER_SRCS} ${OPT_LOADER_SRCS} ${FRAMEWORK_HEADERS})
        else()
            add_library(vulkan-framework SHARED ${NORMAL_LOADER_SRCS} ${OPT_LOADER_SRCS} ${FRAMEWORK_HEADERS})
        endif()
        add_dependencies(vulkan-framework loader_asm_gen_files)
        target_link_libraries(vulkan-framework ${CMAKE_DL_LIBS} Threads::Threads -lm "-framework CoreFoundation")
        target_link_libraries(vulkan-framework loader_specific_options)

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
endif()

# common attributes of the vulkan library
target_link_libraries(vulkan PRIVATE loader_specific_options)

set_target_properties(vulkan ${LOADER_STANDARD_C_PROPERTIES})
if (TARGET asm_offset)
    set_target_properties(asm_offset ${LOADER_STANDARD_C_PROPERTIES})
endif()

# Generate pkg-config file.
include(FindPkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    set(VK_API_VERSION "${LOADER_GENERATED_HEADER_VERSION}")
    foreach(LIB ${CMAKE_CXX_IMPLICIT_LINK_LIBRARIES} ${PLATFORM_LIBS})
        set(PRIVATE_LIBS "${PRIVATE_LIBS} -l${LIB}")
    endforeach()
    if(WIN32)
        if(MINGW)
            set(VULKAN_LIB_SUFFIX "-1.dll")
        else()
            set(VULKAN_LIB_SUFFIX "-1")
        endif()
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

target_link_libraries(vulkan PRIVATE Vulkan::Headers)
add_library(Vulkan::Vulkan ALIAS vulkan)

install(TARGETS vulkan
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
