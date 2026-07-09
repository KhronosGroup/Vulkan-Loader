# Ideally in the future we should be using the vcpkg port of vulkan-headers instead of this overlay.
# This requires updating the vcpkg port of vulkan-headers first in the vcpkg repo, and then updating
# the builtin-baseline in vcpkg.json.
#
# IMHO this is less work moving forward than trying to maintain a separate overlay for vulkan-headers,
# and it will also make it easier to use the vcpkg port of vulkan-headers in other projects.
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO KhronosGroup/Vulkan-Headers
    REF "v${VERSION}"
    SHA512 2c336c163cc184f1aad147f8829dedabdffe0a801e1636c1ae8445feca76a64fc71815d63a69a85db6f5c485ecb6ff3197d94aef63b06f7f9daac0260f7853a0
    HEAD_REF main
)

set(VCPKG_BUILD_TYPE release) # header-only port

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DVULKAN_HEADERS_ENABLE_MODULE=OFF
        -DVULKAN_HEADERS_ENABLE_TESTS=OFF
)
vcpkg_cmake_install()

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE.md")
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
