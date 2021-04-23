## Loader Testing Framework

The loader testing framework is a mocking environment which fakes many of the global systems that vulkan requires to run. This allows the writing of tests against a known driver, layer, and system configuration.

The framework consists of:
* Test ICD and Test Layer - Programmable components which can have specific behavior, capabilities, features, extensions, and allow querying of various actions the loader takes
* Shim - Platform specific functionality that isolates the loader from the rest of the system
  * Linux - Redirects of filesystem access
  * Windows - Overrides registry with framework component and has dll injection for
* Manifest Writer - Programmatically create layer & ICD manifests and put them in a folder the framework will use
* Tests - Written with GoogleTest, these are the specific test cases written using the various components
* Utility - Glue code like environment variable, paths, boilerplate helpers

### Test ICD and Layer
The layer and ICD interface have several combinations of functionality that is determined by the presence or lack of exported functions in a shared library. To accomplish the testing of these combinations, multiple binaries are created using the same C++ file. These exports are controlled by macro defines which are set through CMake.

To make it easy to run tests from the command line, extensive use CMake is made to put the paths of all the binaries into header files that tests can directly reference. These are the various `config.h.in` files scattered throughout the framework.
Note: Due to hard coding of paths, moving the project to another folder without rebuilding will cause the tests to break.

Because the test ICDs and layers are loaded by the loader, the framework loads them directly and pulls the exported backdoor function which acts as the communication channel between the framework and the test ICDs & layers.

### Shim

Because the loader makes many calls to various OS functionality, the framework intercepts certain calls and makes a few of its own calls to OS functionality to allow proper isolation of the loader from the system it is running on.
This allows multiple tests to be run in isolation.

### Running

CMake Build options: set `BUILD_TESTS` to enable tests, and `TEST_USE_ADDRESS_SANITIZER` to enable Address Sanitizer inside the testing framework

Run the executables as normal
