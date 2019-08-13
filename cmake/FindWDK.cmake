
#.rst:
# FindWDK
# -----------------
#
# Try to find headers in the Windows Driver Kit
#
# This module is intended to be used by the Vulkan loader to locate
# the required headers from the Windows Driver Kit. This should not
# be considered feature complete for other uses, as it is only
# intended for the Vulkan loader.
#
# By default, this module uses environment variables to try to find an
# installed WDK. These environment variables are present in a developer
# command prompt, or can be set by running vcvarsall.bat (which comes
# with a Visual Studio installation.
#
# If a user wants to override the default behavior, or not set the
# needed environment variables, they may also specify one or more of
# the following CMake variables:
#
#   WDK_FULL_PATH       - Attempts to find a WDK in the given path. No
#                         other environment or CMake variable will have
#                         any effect if this is set.
#   WDK_BASE            - Sets the base path for the Windows Kit to use.
#                         This should be the path one level up from the
#                         "Include" directory.
#   WDK_VERSION         - Sets the version of the Windows Kit to use.
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines no IMPORTED targets
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables::
#
#   WDK_FOUND                    - True if the WDK was found
#   WDK_INCLUDE_DIRS             - include directories for the WDK
#

if(DEFINED WDK_FULL_PATH)
    find_path(WDK_VERSION_INCLUDE_DIR
        NAMES km/d3dkmthk.h shared/d3dkmdt.h
        PATHS "${WDK_FULL_PATH}"
    )
else()
    if(NOT DEFINED WDK_BASE AND DEFINED "$ENV{UniversalCRTSdkDir}")
        set(WDK_BASE "$ENV{UniversalCRTSdkDir}")
    elseif(NOT DEFINED WDK_BASE)
        set(WDK_BASE "C:/Program Files (x86)/Windows Kits/10/")
    endif()

    if(DEFINED WDK_VERSION)
        set(SUBDIRS "Include/${WDK_VERSION}")
    else()
        file(GLOB DIR_LIST RELATIVE "C:/Program Files (x86)/Windows Kits/10/" "C:/Program Files (x86)/Windows Kits/10/Include/*")
        list(REVERSE DIR_LIST)
        foreach(DIR IN ITEMS "${DIR_LIST}")
            set(SUBDIRS "${SUBDIRS}" "${DIR}")
        endforeach()
    endif()
    find_path(WDK_VERSION_INCLUDE_DIR
        NAMES km/d3dkmthk.h shared/d3dkmdt.h
        PATHS "${WDK_BASE}"
        PATH_SUFFIXES ${SUBDIRS}
    )
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WDK DEFAULT_MSG WDK_VERSION_INCLUDE_DIR)

if(WDK_FOUND)
    set(WDK_INCLUDE_DIRS "${WDK_VERSION_INCLUDE_DIR}/km" "${WDK_VERSION_INCLUDE_DIR}/shared")
endif()
