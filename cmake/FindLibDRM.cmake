# - Try to find libdrm
#
# LibDRM_FOUND - system has libdrm
# LibDRM_LIBRARIES - Link these to use libdrm
# LibDRM_INCLUDE_DIRS - the libdrm include dir

# Copyright (c) 2021 The Khronos Group Inc.
# Copyright (c) 2021 Valve Corporation
# Copyright (c) 2021 LunarG, Inc.

if (NOT WIN32)

  # use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  find_package(PkgConfig)
  pkg_check_modules(PKG_LIBDRM QUIET LibDRM)

  find_path(
    LibDRM_INCLUDE_DIRS
      NAMES
        drm.h
        drm_fourcc.h
        drm_mode.h
      HINTS
        ${PKG_LIBDRM_INCLUDE_DIRS}
        /usr/include/libdrm
  )
  find_library(
    LibDRM_LIBRARIES
      NAMES
        drm
      HINTS
        ${PKG_LIBDRM_LIBRARY_DIRS}
        /usr/lib64
        /usr/lib
  )

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(LibDRM
      FOUND_VAR
          LibDRM_FOUND
      REQUIRED_VARS
          LibDRM_LIBRARIES
          LibDRM_INCLUDE_DIRS
  )

else()
  set(LibDRM_FOUND FALSE)
endif()

