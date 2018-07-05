#! /bin/bash -x

# Get/update sources:
# git clone https://github.com/KhronosGroup/Vulkan-Loader.git

set -o errexit
set -o nounset

cd $(dirname "$BASH_SOURCE[0]}")

rm -rf BUILD
mkdir BUILD
cd BUILD

cmake \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_INSTALL_PREFIX="$(pwd -P)/INSTALL" \
    -DVULKAN_HEADERS_INSTALL_DIR="/home/mikew/gits/github.com/mikew-lunarg/cmake_stuff/INSTALL" \
    .. \
    |& tee 000_CMAKE_LOG.txt

#make VERBOSE=1 -j install


# vim: set sw=4 ts=8 et ic ai:
