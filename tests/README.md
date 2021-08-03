
# Loader Tests

This directory contains a test suite for the Vulkan loader.
These tests are not exhaustive &mdash; they are expected to be supplemented with other tests, such as CTS.


## (NEW) Running Tests

For most purposes `ctest` is the desired method of running tests.
This is because when a test fails, `ctest` will automatically printout the failing test case.

Tests are organized into various executables:
 * `test_regression` - Contains the vast majority of tests.
 * `test_wsi` - These test require presentation engine support.
   * Because the test suite is meant to be run in CI, these test require extra work to make happen and thus live in their own executable.
 * `test_threading` - Tests which need multiple threads to execute.
   * This allows targeted testing which uses tools like ThreadSanitizer

The loader test framework is designed to be easy to use, as simple as just running a single executable. To achieve that requires extensive build script
automation is required. More details are in the tests/framework/README.md.
The consequence of this automation: Do not relocate the build folder of the project without cleaning the CMakeCache. Most components are found by absolute
path and thus require the contents of the folder to not be relocated.

## (OLD) Running Tests

To run the tests, your environment needs to be configured so that the test layers will be found.
This can be done by setting the `VK_LAYER_PATH` environment variable to point at the built layers.
Depending on the platform build tool you use, this location will either be `${CMAKE_BINARY_DIR}/tests/layers` or `${CMAKE_BINARY_DIR}/tests/layers/${CONFIGURATION}`.
When using Visual Studio, a the generated project will already be set up to set the environment as needed.
Running the tests through the `run_loader_tests.sh` script on Linux will also set up the environment properly.
With any other toolchain, the user will have to set up the environment manually.

## Writing Tests

The `test_environment.h/cpp` are the primary tool used when creating new tests. Either use the existing child classes of FrameworkEnvironment or create a new one
to suite the tests need. Refer to the `tests/framework/README.md` for more details.

IMPORTANT NOTES:
 * NEVER FORGET TO DESTROY A VKINSTANCE THAT WAS SUCCESSFULLY CREATED.
   * The loader loads dynamic libraries in `vkCreateInstance` and unloads them in `vkDestroyInstance`. If these dynamic libraries aren't unloaded, they leak state
   into the next test that runs, causing spurious failures or even crashes.