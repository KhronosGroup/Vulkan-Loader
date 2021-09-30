#!/bin/bash

pushd $(dirname "$0") > /dev/null

export VK_LAYER_PATH="$PWD/layers"
./vk_loader_validation_tests

# Test the wrap objects layer.
./run_wrap_objects_tests.sh || exit 1

popd > /dev/null
