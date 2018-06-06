# Add all optional dependencies
# Currently, the only optional project is googletest

if(TARGET gtest_main)
    message(STATUS "Google Test (googletest) already configured - use it")
elseif(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/googletest")
    message(STATUS "Building gtest from ${CMAKE_CURRENT_SOURCE_DIR}/googletest")
    set(BUILD_GTEST ON CACHE BOOL "Builds the googletest subproject")
    set(BUILD_GMOCK OFF CACHE BOOL "Builds the googlemock subproject")
    set(gtest_force_shared_crt ON CACHE BOOL "Link gtest runtimes dynamically")
    set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries")
    # EXCLUDE_FROM_ALL keeps the install target from installing GTEST files.
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/googletest" EXCLUDE_FROM_ALL)
else()
    message(STATUS "Google Test was not found - tests based on that will not build")
endif()
