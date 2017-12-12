# Cube Application Bundle

set(cube_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/macOS/cube/main.m
    ${CMAKE_CURRENT_SOURCE_DIR}/macOS/cube/AppDelegate.m
    ${CMAKE_CURRENT_SOURCE_DIR}/macOS/cube/DemoViewController.m
)
set(cube_HDRS
    ${CMAKE_CURRENT_SOURCE_DIR}/macOS/cube/AppDelegate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/macOS/cube/DemoViewController.h
)
set(cube_RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/lunarg.ppm
    ${CMAKE_CURRENT_BINARY_DIR}/staging-json/MoltenVK_icd.json
    ${CMAKE_CURRENT_SOURCE_DIR}/macOS/cube/Resources/LunarGIcon.icns
)

# Have Xcode handle the Storyboard
if(${CMAKE_GENERATOR} MATCHES "^Xcode.*")
    set(cube_RESOURCES ${cube_RESOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/macOS/cube/Resources/Main.storyboard
    )
endif()

add_executable(cube MACOSX_BUNDLE
    ${cube_SRCS}
    ${cube_HDRS}
    ${cube_RESOURCES}
    cube.vert.inc cube.frag.inc
)

# Handle the Storyboard ourselves
if(NOT ${CMAKE_GENERATOR} MATCHES "^Xcode.*")
    # Compile the storyboard file with the ibtool.
    add_custom_command(TARGET cube POST_BUILD
        COMMAND ${IBTOOL} --errors --warnings --notices --output-format human-readable-text
            --compile ${CMAKE_CURRENT_BINARY_DIR}/cube.app/Contents/Resources/Main.storyboardc
            ${CMAKE_CURRENT_SOURCE_DIR}/macOS/cube/Resources/Main.storyboard
            COMMENT "Compiling storyboard"
    )
endif()

add_dependencies(cube MoltenVK_icd-staging-json)

# Include demo source code dir because the MacOS cube's Objective-C source includes
# the "original" cube application C source code.
# Also include the MoltenVK helper files.
target_include_directories(cube PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${MOLTENVK_DIR}/MoltenVK/include
)

target_link_libraries(cube ${LIBRARIES} "-framework Cocoa -framework QuartzCore")

set_target_properties(cube PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/macOS/cube/Info.plist
)

# The RESOURCE target property cannot be used in conjunction with the MACOSX_PACKAGE_LOCATION
# property.  We need fine-grained control over the Resource directory, so we have to specify
# the destination of all the resource files on a per-destination-directory basis.
# If all the files went into the top-level Resource directory, then we could simply set
# the RESOURCE property to a list of all the resource files.
set_source_files_properties(${cube_RESOURCES} PROPERTIES
    MACOSX_PACKAGE_LOCATION "Resources"
)
set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/staging-json/MoltenVK_icd.json" PROPERTIES
    MACOSX_PACKAGE_LOCATION "Resources/vulkan/icd.d"
)

# Copy the MoltenVK lib into the bundle.
if(${CMAKE_GENERATOR} MATCHES "^Xcode.*")
    add_custom_command(TARGET cube POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${MOLTENVK_DIR}/MoltenVK/MacOS/libMoltenVK.dylib"
            ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/cube.app/Contents/Frameworks/libMoltenVK.dylib
        DEPENDS vulkan
    )
else()
    add_custom_command(TARGET cube POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${MOLTENVK_DIR}/MoltenVK/MacOS/libMoltenVK.dylib"
            ${CMAKE_CURRENT_BINARY_DIR}/cube.app/Contents/Frameworks/libMoltenVK.dylib
        DEPENDS vulkan
    )
endif()

# Fix up the library search path in the executable to find (loader) libraries in the bundle.
install(CODE "
    include(BundleUtilities)
    fixup_bundle(${CMAKE_INSTALL_PREFIX}/demos/cube.app \"\" \"\")
    " COMPONENT Runtime
)

# Not sure this is needed.  When activated, it makes a symlink from
# libvulkan.dylib to libvulkan.1.dylib (which in turn symlinks to libvulkan.1.0.xx.dylib.)
#        install(FILES
#            "${CMAKE_BINARY_DIR}/loader/libvulkan.dylib"
#            DESTINATION "demos/cube.app/Contents/MacOS"
#            COMPONENT Runtime)
