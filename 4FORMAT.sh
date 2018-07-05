#! /bin/bash -x

VERSION="0.4.1"
#export PYTHONPATH="$HOME/gh/mikew-lunarg/cmake_stuff/cmake_format-$VERSION"
#export PYTHONPATH="$HOME/gh/cheshirekow/cmake_format"

[ "$(python -Bm cmake_format --version 2>&1)" != "$VERSION" ] && exit 1

for i in CMakeLists.txt external/CMakeLists.txt loader/CMakeLists.txt tests/CMakeLists.txt tests/layers/CMakeLists.txt
do
    #python -Bm cmake_format --in-place $i
    cmake-format --in-place $i
done
