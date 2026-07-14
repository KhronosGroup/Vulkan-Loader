# Only used by the mac-univeral CI job, to get a fat (arm64 + x86_64) gtest
# that can link into that job's universal libvulkan.dylib/test binaries.
# vcpkg forwards VCPKG_OSX_ARCHITECTURES straight to CMake's
# CMAKE_OSX_ARCHITECTURES, so this only works for CMake-based ports (like
# gtest) that already support multi-arch builds; it would not work for a
# port built with autotools or a hand-rolled makefile.
set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME Darwin)
set(VCPKG_OSX_ARCHITECTURES "arm64;x86_64")
