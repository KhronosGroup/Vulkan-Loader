# Cube Application Bundle

set(cubepp_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/macOS/cubepp/main.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/macOS/cubepp/AppDelegate.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/macOS/cubepp/DemoViewController.mm
)
set(cubepp_HDRS
    ${CMAKE_CURRENT_SOURCE_DIR}/macOS/cubepp/AppDelegate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/macOS/cubepp/DemoViewController.h
)
set(cubepp_RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/lunarg.ppm
    ${CMAKE_CURRENT_BINARY_DIR}/staging-json/MoltenVK_icd.json
    ${CMAKE_CURRENT_SOURCE_DIR}/macOS/cubepp/Resources/LunarGIcon.icns
)

# Have Xcode handle the Storyboard
if(${CMAKE_GENERATOR} MATCHES "^Xcode.*")
    set(cubepp_RESOURCES ${cubepp_RESOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/macOS/cubepp/Resources/Main.storyboard
    )
endif()

add_executable(cubepp MACOSX_BUNDLE
    ${cubepp_SRCS}
    ${cubepp_HDRS}
    ${cubepp_RESOURCES}
    cube.vert.inc cube.frag.inc
)

# Handle the Storyboard ourselves
if(NOT ${CMAKE_GENERATOR} MATCHES "^Xcode.*")
    # Compile the storyboard file with the ibtool.
    add_custom_command(TARGET cubepp POST_BUILD
        COMMAND ${IBTOOL} --errors --warnings --notices --output-format human-readable-text
            --compile ${CMAKE_CURRENT_BINARY_DIR}/cubepp.app/Contents/Resources/Main.storyboardc
            ${CMAKE_CURRENT_SOURCE_DIR}/macOS/cubepp/Resources/Main.storyboard
            COMMENT "Compiling storyboard"
    )
endif()

add_dependencies(cubepp MoltenVK_icd-staging-json)

# Include demo source code dir because the MacOS cubepp's Objective-C source includes
# the "original" cubepp application C++ source code.
# Also include the MoltenVK helper files.
target_include_directories(cubepp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${MOLTENVK_DIR}/MoltenVK/include
)

target_link_libraries(cubepp ${LIBRARIES} "-framework Cocoa -framework QuartzCore")

set_target_properties(cubepp PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/macOS/cubepp/Info.plist
)

# The RESOURCE target property cannot be used in conjunction with the MACOSX_PACKAGE_LOCATION
# property.  We need fine-grained control over the Resource directory, so we have to specify
# the destination of all the resource files on a per-destination-directory basis.
# If all the files went into the top-level Resource directory, then we could simply set
# the RESOURCE property to a list of all the resource files.
set_source_files_properties(${cubepp_RESOURCES} PROPERTIES
    MACOSX_PACKAGE_LOCATION "Resources"
)
set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/staging-json/MoltenVK_icd.json" PROPERTIES
    MACOSX_PACKAGE_LOCATION "Resources/vulkan/icd.d"
)

# Direct the MoltenVK library to the right place.
install(FILES "${MOLTENVK_DIR}/MoltenVK/MacOS/libMoltenVK.dylib"
        DESTINATION "demos/cubepp.app/Contents/Frameworks"
        COMPONENT Runtime
)

# Copy the MoltenVK lib into the bundle.
if(${CMAKE_GENERATOR} MATCHES "^Xcode.*")
    add_custom_command(TARGET cubepp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${MOLTENVK_DIR}/MoltenVK/MacOS/libMoltenVK.dylib"
            ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/cubepp.app/Contents/Frameworks/libMoltenVK.dylib
        DEPENDS vulkan
    )
else()
    add_custom_command(TARGET cubepp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${MOLTENVK_DIR}/MoltenVK/MacOS/libMoltenVK.dylib"
            ${CMAKE_CURRENT_BINARY_DIR}/cubepp.app/Contents/Frameworks/libMoltenVK.dylib
        DEPENDS vulkan
    )
endif()

# Fix up the library search path in the executable to find (loader) libraries in the bundle.
install(CODE "
    include(BundleUtilities)
    fixup_bundle(${CMAKE_INSTALL_PREFIX}/demos/cubepp.app \"\" \"\")
    " COMPONENT Runtime
)

# Not sure this is needed.  When activated, it makes a symlink from
# libvulkan.dylib to libvulkan.1.dylib (which in turn symlinks to libvulkan.1.0.xx.dylib.)
#        install(FILES
#            "${CMAKE_BINARY_DIR}/loader/libvulkan.dylib"
#            DESTINATION "demos/cubepp.app/Contents/MacOS"
#            COMPONENT Runtime)
