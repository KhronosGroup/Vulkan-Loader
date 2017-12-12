set (GLMINC_PREFIX ${PROJECT_SOURCE_DIR}/libs)

macro(generate_dispatch_table out)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${out}
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/generate-dispatch-table.py ${CMAKE_CURRENT_SOURCE_DIR}/${out}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generate-dispatch-table.py
        )
endmacro()

macro(glsl_to_spirv src)
    add_custom_command(OUTPUT ${src}.h
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/glsl-to-spirv ${CMAKE_CURRENT_SOURCE_DIR}/${src} ${src}.h ${GLSLANG_VALIDATOR}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/glsl-to-spirv ${CMAKE_CURRENT_SOURCE_DIR}/${src} ${GLSLANG_VALIDATOR}
        )
endmacro()

generate_dispatch_table(HelpersDispatchTable.h)
generate_dispatch_table(HelpersDispatchTable.cpp)
glsl_to_spirv(Smoke.frag)
glsl_to_spirv(Smoke.vert)
glsl_to_spirv(Smoke.push_constant.vert)

set(sources
    Game.cpp
    Game.h
    Helpers.h
    HelpersDispatchTable.cpp
    HelpersDispatchTable.h
    Smoke.cpp
    Smoke.h
    Smoke.frag.h
    Smoke.vert.h
    Smoke.push_constant.vert.h
    Main.cpp
    Meshes.cpp
    Meshes.h
    Meshes.teapot.h
    Simulation.cpp
    Simulation.h
    Shell.cpp
    Shell.h
    )

set(definitions
    PRIVATE -DVK_NO_PROTOTYPES
    PRIVATE -DGLM_FORCE_RADIANS)

set(includes
    PRIVATE ${GLMINC_PREFIX}
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

set(libraries PRIVATE ${CMAKE_THREAD_LIBS_INIT})

if(TARGET vulkan)
    if(APPLE)
        # For macOS, the "uninstalled" location is in the bundle
        list(APPEND definitions PRIVATE -DUNINSTALLED_LOADER="./libvulkan.1.dylib")
    else()
        list(APPEND definitions PRIVATE -DUNINSTALLED_LOADER="$<TARGET_FILE:vulkan>")
    endif()
endif()

if(WIN32)
    list(APPEND definitions PRIVATE -DVK_USE_PLATFORM_WIN32_KHR)
    list(APPEND definitions PRIVATE -DWIN32_LEAN_AND_MEAN)

    list(APPEND sources ShellWin32.cpp ShellWin32.h)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    list(APPEND libraries PRIVATE -ldl -lrt)

    if(BUILD_WSI_XCB_SUPPORT AND DEMOS_WSI_SELECTION STREQUAL "XCB")
        find_package(XCB REQUIRED)

        list(APPEND sources ShellXcb.cpp ShellXcb.h)
        list(APPEND definitions PRIVATE -DVK_USE_PLATFORM_XCB_KHR)
        list(APPEND includes PRIVATE ${XCB_INCLUDES})
        list(APPEND libraries PRIVATE ${XCB_LIBRARIES})
    elseif(BUILD_WSI_WAYLAND_SUPPORT AND DEMOS_WSI_SELECTION STREQUAL "WAYLAND")
        find_package(Wayland REQUIRED)

        list(APPEND sources ShellWayland.cpp ShellWayland.h)
        list(APPEND definitions PRIVATE -DVK_USE_PLATFORM_WAYLAND_KHR)
        list(APPEND includes PRIVATE ${WAYLAND_CLIENT_INCLUDE_DIR})
        list(APPEND libraries PRIVATE ${WAYLAND_CLIENT_LIBRARIES})
    endif()
elseif(APPLE)
    set(LIBRARIES)
    list(APPEND sources
        macOS/AppDelegate.h macOS/AppDelegate.m
        macOS/DemoViewController.h macOS/DemoViewController.mm
        macOS/main.m macOS/ShellMVK.cpp macOS/ShellMVK.h
    )
    list(APPEND includes
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${MOLTENVK_DIR}/MoltenVK/include)
    list(APPEND libraries
        "-framework Cocoa -framework QuartzCore")
    set(smoketest_RESOURCES
        ${CMAKE_BINARY_DIR}/demos/staging-json/MoltenVK_icd.json
        ${CMAKE_CURRENT_SOURCE_DIR}/macOS/Resources/LunarGIcon.icns
        ${CMAKE_CURRENT_SOURCE_DIR}/macOS/Resources/Main.storyboard
    )
    set_source_files_properties(${CMAKE_BINARY_DIR}/demos/staging-json/MoltenVK_icd.json PROPERTIES
        GENERATED TRUE
    )
    # Have Xcode handle the Storyboard
    if(${CMAKE_GENERATOR} MATCHES "^Xcode.*")
        list(APPEND sources
            macOS/Resources/Main.storyboard
        )
    endif()
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/..)

add_executable(smoketest MACOSX_BUNDLE ${sources} ${smoketest_RESOURCES})
target_compile_definitions(smoketest ${definitions})
target_include_directories(smoketest ${includes})
target_link_libraries(smoketest ${libraries})

if(APPLE)
    # Handle the Storyboard ourselves
    if(NOT ${CMAKE_GENERATOR} MATCHES "^Xcode.*")
        # Compile the storyboard file with the ibtool.
        add_custom_command(TARGET smoketest POST_BUILD
            COMMAND ${IBTOOL} --errors --warnings --notices --output-format human-readable-text
                --compile ${CMAKE_CURRENT_BINARY_DIR}/../smoketest.app/Contents/Resources/Main.storyboardc
                ${CMAKE_CURRENT_SOURCE_DIR}/macOS/Resources/Main.storyboard
                COMMENT "Compiling storyboard"
        )
    endif()

    add_dependencies(smoketest MoltenVK_icd-staging-json)

    set_target_properties(smoketest PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/macOS/Info.plist
    )
    set_source_files_properties(${smoketest_RESOURCES} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
    set_source_files_properties("${CMAKE_BINARY_DIR}/demos/staging-json/MoltenVK_icd.json" PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources/vulkan/icd.d"
    )

    # Direct the MoltenVK library to the right place.
    install(FILES "${MOLTENVK_DIR}/MoltenVK/MacOS/libMoltenVK.dylib"
        DESTINATION "demos/smoketest.app/Contents/Frameworks"
        COMPONENT Runtime
    )

    # Xcode projects need some extra help with what would be install steps.
    # Vulkan lib needs to be copied manually since smoke does not link it.
    if(${CMAKE_GENERATOR} MATCHES "^Xcode.*")
        add_custom_command(TARGET smoketest POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:vulkan>"
                ${CMAKE_CURRENT_BINARY_DIR}/../$<CONFIG>/smoketest.app/Contents/MacOS/libvulkan.1.dylib
            COMMAND ${CMAKE_COMMAND} -E copy "${MOLTENVK_DIR}/MoltenVK/MacOS/libMoltenVK.dylib"
                ${CMAKE_CURRENT_BINARY_DIR}/../$<CONFIG>/smoketest.app/Contents/Frameworks/libMoltenVK.dylib
            DEPENDS vulkan
        )
    else()
        add_custom_command(TARGET smoketest POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:vulkan>"
                ${CMAKE_CURRENT_BINARY_DIR}/../smoketest.app/Contents/MacOS/libvulkan.1.dylib
            COMMAND ${CMAKE_COMMAND} -E copy "${MOLTENVK_DIR}/MoltenVK/MacOS/libMoltenVK.dylib"
                ${CMAKE_CURRENT_BINARY_DIR}/../smoketest.app/Contents/Frameworks/libMoltenVK.dylib
            DEPENDS vulkan
        )
    endif()

    # Fix up the library search path in the executable to find (loader) libraries in the bundle.
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(${CMAKE_INSTALL_PREFIX}/demos/smoketest.app \"\" \"\")
        " COMPONENT Runtime
    )
endif()

if(UNIX)
    if(INSTALL_LVL_FILES)
        install(TARGETS smoketest DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif()
endif()
