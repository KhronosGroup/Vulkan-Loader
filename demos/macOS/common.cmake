# Set up common settings for building all demos on Apple platforms.

# Source for the MoltenVK ICD library and JSON file
set(MOLTENVK_DIR "${PROJECT_SOURCE_DIR}/external/MoltenVK")

# MoltenVK JSON File

# Modify the ICD JSON file to adjust the library path.
# The ICD JSON file goes in the Resources/vulkan/icd.d directory, so adjust the
# library_path to the relative path to the Frameworks directory in the bundle..
# The regex does: substitute ':<whitespace>"<whitespace><all occurences of . and />' with:
# ': "../../../Frameworks/'
add_custom_target(MoltenVK_icd-staging-json ALL
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/staging-json
    COMMAND sed -e "/\"library_path\":/s$:[[:space:]]*\"[[:space:]]*[\\.\\/]*$: \"..\\/..\\/..\\/Frameworks\\/$"
        ${MOLTENVK_DIR}/MoltenVK/icd/MoltenVK_icd.json >
        ${CMAKE_CURRENT_BINARY_DIR}/staging-json/MoltenVK_icd.json
    VERBATIM
    DEPENDS "${MOLTENVK_DIR}/MoltenVK/icd/MoltenVK_icd.json"
)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/staging-json/MoltenVK_icd.json PROPERTIES
    GENERATED TRUE
)

find_library(COCOA NAMES Cocoa)

# Locate Interface Builder Tool, needed to build things like Storyboards outside of Xcode.
if (NOT ${CMAKE_GENERATOR} MATCHES "^Xcode.*")
    # Make sure we can find the 'ibtool' program. If we can NOT find it we
    # skip generation of this project
    find_program(IBTOOL ibtool HINTS "/usr/bin" "${OSX_DEVELOPER_ROOT}/usr/bin")
    if (${IBTOOL} STREQUAL "IBTOOL-NOTFOUND")
        message(SEND_ERROR 
            "ibtool can not be found and is needed to compile the .xib files. "
            "It should have been installed with the Apple developer tools. "
            "The default system paths were searched in addition to ${OSX_DEVELOPER_ROOT}/usr/bin."
        )
    endif()
endif()
