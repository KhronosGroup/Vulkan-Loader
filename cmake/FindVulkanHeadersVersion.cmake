#.rst:
# FindVulkanHeadersVersion
# -----------------
#
# Try to find Vulkan Headers Version strings.
#
# This module defines the following variables::
#
#   VulkanHeaders_VERSION_MAJOR  - The Major API version of the latest version
#                                  contained in the Vulkan header
#   VulkanHeaders_VERSION_MINOR  - The Minor API version of the latest version
#                                  contained in the Vulkan header
#   VulkanHeaders_VERSION_PATCH  - The Patch API version of the latest version
#                                  contained in the Vulkan header

if(TARGET Vulkan::Headers)
  get_target_property(VulkanHeaders_INCLUDE_DIR Vulkan::Headers INTERFACE_INCLUDE_DIRECTORIES)
endif()

# Determine the major/minor/patch version from the vulkan header
set(VulkanHeaders_VERSION_MAJOR "0")
set(VulkanHeaders_VERSION_MINOR "0")
set(VulkanHeaders_VERSION_PATCH "0")

# First, determine which header we need to grab the version information from.
# Starting with Vulkan 1.1, we should use vulkan_core.h, but prior to that,
# the information was in vulkan.h.
if (EXISTS "${VulkanHeaders_INCLUDE_DIR}/vulkan/vulkan_core.h")
    set(VulkanHeaders_main_header ${VulkanHeaders_INCLUDE_DIR}/vulkan/vulkan_core.h)
else()
    set(VulkanHeaders_main_header ${VulkanHeaders_INCLUDE_DIR}/vulkan/vulkan.h)
endif()

# Find all lines in the header file that contain any version we may be interested in
#  NOTE: They start with #define and then have other keywords
file(STRINGS
        ${VulkanHeaders_main_header}
        VulkanHeaders_lines
        REGEX "^#define (VK_API_VERSION.*VK_MAKE_VERSION|VK_HEADER_VERSION)")

foreach(VulkanHeaders_line ${VulkanHeaders_lines})

    # First, handle the case where we have a major/minor version
    #   Format is:
    #        #define VK_API_VERSION_X_Y VK_MAKE_VERSION(X, Y, 0)
    #   We grab the major version (X) and minor version (Y) out of the parentheses
    string(REGEX MATCH "VK_MAKE_VERSION\\(.*\\)" VulkanHeaders_out ${VulkanHeaders_line})
    string(REGEX MATCHALL "[0-9]+" VulkanHeaders_MAJOR_MINOR "${VulkanHeaders_out}")
    if (VulkanHeaders_MAJOR_MINOR)
        list (GET VulkanHeaders_MAJOR_MINOR 0 VulkanHeaders_cur_major)
        list (GET VulkanHeaders_MAJOR_MINOR 1 VulkanHeaders_cur_minor)
        if (${VulkanHeaders_cur_major} GREATER ${VulkanHeaders_VERSION_MAJOR})
            set(VulkanHeaders_VERSION_MAJOR ${VulkanHeaders_cur_major})
            set(VulkanHeaders_VERSION_MINOR ${VulkanHeaders_cur_minor})
        endif()
        if (${VulkanHeaders_cur_major} EQUAL ${VulkanHeaders_VERSION_MAJOR} AND
            ${VulkanHeaders_cur_minor} GREATER ${VulkanHeaders_VERSION_MINOR})
            set(VulkanHeaders_VERSION_MINOR ${VulkanHeaders_cur_minor})
        endif()
    endif()

    # Second, handle the case where we have the patch version
    #   Format is:
    #      #define VK_HEADER_VERSION Z
    #   Where Z is the patch version which we just grab off the end
    string(REGEX MATCH "define.*VK_HEADER_VERSION[^_].*[0-9]+" VulkanHeaders_out ${VulkanHeaders_line})
    list(LENGTH VulkanHeaders_out VulkanHeaders_len)
    if (VulkanHeaders_len)
        string(REGEX MATCH "[0-9]+" VulkanHeaders_VERSION_PATCH "${VulkanHeaders_out}")
        set(VulkanHeaders_VERSION_PATCH ${VulkanHeaders_VERSION_PATCH})
    endif()

endforeach()

MESSAGE(STATUS
        "Detected Vulkan Version ${VulkanHeaders_VERSION_MAJOR}."
        "${VulkanHeaders_VERSION_MINOR}."
        "${VulkanHeaders_VERSION_PATCH}")
