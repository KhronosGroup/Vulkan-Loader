# Build Instructions

Instructions for building this repository on Linux, Windows, and MacOS.

## Index

1. [Contributing](#contributing-to-the-repository)
2. [Repository Set-Up](#repository-set-up)
3. [Windows Build](#building-on-windows)
4. [Linux Build](#building-on-linux)
5. [MacOS build](#building-on-macos)

## Contributing to the Repository

If you intend to contribute, the preferred work flow is for you to develop
your contribution in a fork of this repository in your GitHub account and
then submit a pull request.
Please see the [CONTRIBUTING.md](CONTRIBUTING.md) file in this repository for more details.

## Repository Set-Up

### Display Drivers

This repository does not contain a Vulkan-capable driver.
Before proceeding, it is strongly recommended that you obtain a Vulkan driver from your
graphics hardware vendor and install it properly.

### Download the Repository

To create your local git repository:

    git clone --recurse-submodules https://github.com/KhronosGroup/Vulkan-Loader

The `--recurse-submodules` option is required because this repository contains
submodules.
If you forget to specify this option, you can later run:

    git submodule update --init --recursive

## Building On Windows

### Windows Build Requirements

Windows 7+ with the following software packages:

- Microsoft Visual Studio 2013 Update 4 Professional, VS2015 (any version), or VS2017 (any version).
- [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers)
  - Vulkan headers should be built with the "install" target. Generally, you will want to use the
    `CMAKE_INSTALL_PREFIX` and remember where you set this to.
- [CMake](http://www.cmake.org/download/)
  - Tell the installer to "Add CMake to the system PATH" environment variable.
- [Python 3](https://www.python.org/downloads)
  - Select to install the optional sub-package to add Python to the system PATH
    environment variable.
  - Ensure the `pip` module is installed (it should be by default)
  - Python3.3 or later is necessary for the Windows py.exe launcher that is used to select python3
    rather than python2 if both are installed
- [Git](http://git-scm.com/download/win)
  - Tell the installer to allow it to be used for "Developer Prompt" as well as "Git Bash".
  - Tell the installer to treat line endings "as is" (i.e. both DOS and Unix-style line endings).
  - Install both the 32-bit and 64-bit versions, as the 64-bit installer does not install the
    32-bit libraries and tools.

### Windows Build - Microsoft Visual Studio

The general approach is to run `cmake` to generate the VS project files.
Then either run `cmake` again to build from the command line or use the
Visual Studio IDE to open the generated solution and work with the solution
interactively.

It should be possible to perform these `cmake` invocations from any one of the Windows
"terminal programs", including the standard Windows Command Prompt, MSBuild Command Prompt,
PowerShell, MINGW, CygWin, etc.

#### Use `cmake` to create the VS project files

Switch to the top of the cloned repository directory,
create a build directory and generate the VS project files.
When generating the project files, the location to a Vulkan-Headers install
directory must be provided. This can be done by setting the environment variable
`VULKAN_HEADERS_INSTALL_DIR` or by setting it as a CMake flag.
In either case, the variable should point to the installation directory of the
Vulkan-Headers repo:

    cd Vulkan-Loader
    mkdir build
    cd build
    cmake -DVULKAN_HEADERS_INSTALL_DIR=C:\SampleDir ..

As it starts generating the project files, `cmake` responds with something like:

    -- Building for: Visual Studio 14 2015

which is a 32-bit generator.

If this is not what you want, you can supply one of the
[specific generators](#cmake-visual-studio-generators).
For example:

    cmake -DVULKAN_HEADERS_INSTALL_DIR=C:\SampleDir -G "Visual Studio 14 2015 Win64" ..

This creates a Windows 64-bit solution file named `Vulkan-Loader.sln`
in the build directory for Visual Studio 2015.

At this point, you can build the solution from the command line or open the
generated solution with Visual Studio.

#### Build the solution from the command line

While still in the build directory:

    cmake --build .

to build the Debug configuration (the default), or:

    cmake --build . --config Release

to make a Release build.

#### Build the solution with Visual Studio

Launch Visual Studio and open the "Vulkan-Loader.sln" solution file in the build folder.
You may select "Debug" or "Release" from the Solution Configurations drop-down list.
Start a build by selecting the Build->Build Solution menu item.

#### Windows Install Target

The CMake project also generates an "install" target that you can use to
copy the primary build artifacts to a specific location using a
"bin, include, lib" style directory structure.
This may be useful for collecting the artifacts and providing them to
another project that is dependent on them.

The default location is `$CMAKE_BINARY_DIR\install`, but can be changed
with the `CMAKE_INSTALL_PREFIX` variable.
You can build the install target with:

    cmake --build . --config Release --target install

or build the `INSTALL` target from the Visual Studio solution explorer.

### Windows Tests

The Vulkan-Loader repository contains some simple unit tests for the loader
but no other test clients.

To run the loader test script, open a Powershell Console,
change to the `build\tests` directory, and run:

For Release builds:

    .\run_all_tests.ps1

For Debug builds:

    .\run_all_tests.ps1 -Debug

This script will run the following tests:

- `vk_loader_validation_tests`:
  Vulkan loader handle wrapping, allocation callback, and loader/layer interface tests

You can also change to either `build\tests\Debug` or `build\tests\Release`
(depending on which one you built) and run the executable tests (`*.exe`) files
from there.

### Windows Notes

#### CMake Visual Studio Generators

The chosen generator should match one of the Visual Studio versions that you have installed.
Appropriate Visual Studio generators include:

| Build Platform               | 64-bit Generator              | 32-bit Generator        |
|------------------------------|-------------------------------|-------------------------|
| Microsoft Visual Studio 2013 | "Visual Studio 12 2013 Win64" | "Visual Studio 12 2013" |
| Microsoft Visual Studio 2015 | "Visual Studio 14 2015 Win64" | "Visual Studio 14 2015" |
| Microsoft Visual Studio 2017 | "Visual Studio 15 2017 Win64" | "Visual Studio 15 2017" |

#### The Vulkan Loader Library

Vulkan programs must be able to find and use the Vulkan loader (`vulkan-1.dll`)
library as well as any other libraries the program requires.
One convenient way to do this is to copy the required libraries into the same
directory as the program.
The projects in this solution copy the Vulkan loader library and the "googletest"
libraries to the `build\tests\Debug` or the `build\tests\Release` directory,
which is where the `vk_loader_validation_test.exe` executable is found, depending
on what configuration you built.
(The loader validation tests use the "googletest" testing framework.)

Other techniques include placing the library in a system folder (C:\Windows\System32) or in a
directory that appears in the `PATH` environment variable.

See the `LoaderAndLayerInterface` document in the `loader` folder in this repository
for more information on how the loader finds driver libraries and layer libraries.
The document also describes both how ICDs and layers should be packaged,
and how developers can point to ICDs and layers within their builds.

## Building On Linux

### Linux Build Requirements

This repository has been built and tested on the two most recent Ubuntu LTS versions.
Currently, the oldest supported version is Ubuntu 14.04, meaning that the minimum supported compiler versions are GCC 4.8.2 and Clang 3.4, although earlier versions may work.
It should be straightforward to adapt this repository to other Linux distributions.

**Required Package List:**

    sudo apt-get install git cmake build-essential libx11-xcb-dev libxkbcommon-dev libmirclient-dev libwayland-dev libxrandr-dev

In addition to this, the [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers) repo should be
built and installed to the directory of your choosing. This can be done by setting `CMAKE_INSTALL_PREFIX`
and running the "install" target from within the Vulkan-Headers repository.

### Linux Build

Example debug build (Note that the update\_external\_sources script used below builds external tools into predefined locations.
See **Loader and Validation Layer Dependencies** for more information and other options):

Switch to the top of the cloned repository directory,
create a build directory and generate the make files.
When generating the project files, the location to a Vulkan-Headers install
directory must be provided. This can be done by setting the environment variable
`VULKAN_HEADERS_INSTALL_DIR` or by setting it as a CMake flag.
In either case, the variable should point to the installation directory of the
Vulkan-Headers repo:

    cd Vulkan-Loader
    mkdir build
    cd build
    cmake -DVULKAN_HEADERS_INSTALL_DIR=/sample/dir -DCMAKE_BUILD_TYPE=Debug ..

Run `make -j8` to begin the build.

If your build system supports ccache, you can enable that via CMake option `-DUSE_CCACHE=On`

#### Using the new loader

If you want to test a Vulkan application with the loader you just built, you can
direct the application to load it from your build directory:

    export LD_LIBRARY_PATH=<path to your repository root>/build/loader

The `LoaderAndLayerInterface` document in the `loader` folder in this repository
is a specification that describes both how ICDs and layers should be properly packaged,
and how developers can point to ICDs and layers within their builds.

### WSI Support Build Options

By default, the Vulkan Loader is built with support for all 4 Vulkan-defined WSI display servers: Xcb, Xlib, Wayland, and Mir.
It is recommended to build the repository components with support for these display servers to maximize their usability across Linux platforms.
If it is necessary to build these modules without support for one of the display servers, the appropriate CMake option of the form `BUILD_WSI_xxx_SUPPORT` can be set to `OFF`.
See the top-level CMakeLists.txt file for more info.

### Linux Install to System Directories

Installing the files resulting from your build to the systems directories is optional since environment variables can usually be used instead to locate the binaries.
There are also risks with interfering with binaries installed by packages.
If you are certain that you would like to install your binaries to system directories, you can proceed with these instructions.

Assuming that you've built the code as described above and the current directory is still `build`, you can execute:

    sudo make install

This command installs files to:

- `/usr/local/include/vulkan`:  Vulkan include files
- `/usr/local/lib`:  Vulkan loader library and package config files

You may need to run `ldconfig` in order to refresh the system loader search cache on some Linux systems.

You can further customize the installation location by setting additional CMake variables to override their defaults.
For example, if you would like to install to `/tmp/build` instead of `/usr/local`, on your CMake command line specify:

    -DCMAKE_INSTALL_PREFIX=/tmp/build
    -DDEST_DIR=/tmp/build

Then run `make install` as before. The install step places the files in `/tmp/build`.
This may be useful for collecting the artifacts and providing them to
another project that is dependent on them.

Using the `CMAKE_INSTALL_PREFIX` to customize the install location also modifies
the loader search paths to include searching for layers in the specified install location.
In this example, setting `CMAKE_INSTALL_PREFIX` to `/tmp/build` causes the loader to search
`/tmp/build/etc/vulkan/explicit_layer.d` and `/tmp/build/share/vulkan/explicit_layer.d`
for the layer JSON files.
The loader also searches the "standard" system locations of `/etc/vulkan/explicit_layer.d` and
`/usr/share/vulkan/explicit_layer.d` after searching the two locations under `/tmp/build`.

You can further customize the installation directories by using the CMake variables
`CMAKE_INSTALL_SYSCONFDIR` to rename the `etc` directory and `CMAKE_INSTALL_DATADIR`
to rename the `share` directory.

See the CMake documentation for more details on using these variables
to further customize your installation.

Also see the `LoaderAndLayerInterface` document in the `loader` folder in this
repository for more information about loader operation.

Note that some executables in this repository (e.g., `vk_loader_validation_tests`)
use the "rpath" linker directive to
load the Vulkan loader from the build directory, `build` in this example.
This means that even after installing the loader to the system directories, these executables still
use the loader from the build directory.

### Linux Uninstall

To uninstall the files from the system directories, you can execute:

    sudo make uninstall

### Linux Tests

The Vulkan-Loader repository contains some simple unit tests for the loader
but no other test clients.

To run the loader test script,
change to the `build/tests` directory, and run:

    ./run_all_tests.sh

This script will run the following tests:

- `vk_loader_validation_tests`:
  Vulkan loader handle wrapping, allocation callback, and loader/layer interface tests

### Linux Notes

#### RPATH

The `vk_loader_validation_tests` executable is linked with an `RPATH` setting to allow
it to find the Vulkan loader library in the repository's build directory.
This allows the test executable to run and find this Vulkan loader library
without installing the loader library to a directory searched by the system
loader or in the `LD_LIBRARY_PATH`.

#### Linux 32-bit support

Usage of this repository's contents in 32-bit Linux environments is not officially supported.
However, since this repository is supported on 32-bit Windows,
these modules should generally work on 32-bit Linux.

Here are some notes for building 32-bit targets on a 64-bit Ubuntu "reference" platform:

If not already installed, install the following 32-bit development libraries:

`gcc-multilib g++-multilib libx11-dev:i386`

This list may vary depending on your distribution and which windowing systems you are building for.

Set up your environment for building 32-bit targets:

    export ASFLAGS=--32
    export CFLAGS=-m32
    export CXXFLAGS=-m32
    export PKG_CONFIG_LIBDIR=/usr/lib/i386-linux-gnu

Again, your PKG_CONFIG configuration may be different, depending on your distribution.

Finally, rebuild the repository using `cmake` and `make`, as explained above.

## Building on MacOS

### MacOS Build Requirements

Tested on OSX version 10.12.6

Setup Homebrew and components

- Follow instructions on [brew.sh](http://brew.sh) to get Homebrew installed.

      /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

- Ensure Homebrew is at the beginning of your PATH:

      export PATH=/usr/local/bin:$PATH

- Add packages with the following (may need refinement)

      brew install cmake python python3 git

      In addition to this, the [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers) repo should be
      built and installed to the directory of your choosing. This can be done by setting `CMAKE_INSTALL_PREFIX`
      and running the "install" target from within the Vulkan-Headers repository.

### MacOS build

#### CMake Generators

This repository uses CMake to generate build or project files that are
then used to build the repository.
The CMake generators explicitly supported in this repository are:

- Unix Makefiles
- Xcode

#### Building with the Unix Makefiles Generator

This generator is the default generator.
However, when generating the project files, the location to a Vulkan-Headers install
directory must be provided. This can be done by setting the environment variable
`VULKAN_HEADERS_INSTALL_DIR` or by setting it as a CMake flag.
In either case, the variable should point to the installation directory of the
Vulkan-Headers repo:

        mkdir build
        cd build
        cmake -DVULKAN_HEADERS_INSTALL_DIR=/sample/dir -DCMAKE_BUILD_TYPE=Debug ..
        make

To speed up the build on a multi-core machine, use the `-j` option for `make`
to specify the number of cores to use for the build.
For example:

    make -j4

#### Building with the Xcode Generator

To create and open an Xcode project:

        mkdir build-xcode
        cd build-xcode
        cmake -GXcode ..
        open Vulkan-Loader.xcodeproj

Within Xcode, you can select Debug or Release builds in the project's Build Settings.

### Using the new macOS loader

If you want to test a Vulkan application with the loader you just built, you can direct the application to load it from your build directory:

        export DYLD_LIBRARY_PATH=<path to your repository>/build/loader

### MacOS Tests

The Vulkan-Loader repository contains some simple unit tests for the loader but no other test clients.

Before you run these tests, you will need to clone and build the [MoltenVK](https://github.com/KhronosGroup/MoltenVK) repository.

You will also need to direct your new loader to the MoltenVK ICD:

        export VK_ICD_FILENAMES=<path to MoltenVK repository>/Package/Latest/MoltenVK/macOS/MoltenVK_icd.json

To run the loader test script, change to the `build/tests` directory in your Vulkan-Loader repository, and run:

        ./vk_loader_validation_tests

## Ninja Builds - All Platforms

The [Qt Creator IDE](https://qt.io/download-open-source/#section-2) can open a root CMakeList.txt
as a project directly, and it provides tools within Creator to configure and generate Vulkan SDK
build files for one to many targets concurrently.
Alternatively, when invoking CMake, use the `-G "Codeblocks - Ninja"` option to generate Ninja build
files to be used as project files for QtCreator

- Follow the steps defined elsewhere for the OS using the update\_external\_sources script or as
  shown in **Loader and Validation Layer Dependencies** below
- Open, configure, and build the glslang CMakeList.txt files. Note that building the glslang
  project will provide access to spirv-tools and spirv-headers
- Then do the same with the Vulkan-LoaderAndValidationLayers CMakeList.txt file
- In order to debug with QtCreator, a
  [Microsoft WDK: eg WDK 10](http://go.microsoft.com/fwlink/p/?LinkId=526733) is required.

Note that installing the WDK breaks the MSVC vcvarsall.bat build scripts provided by MSVC,
requiring that the LIB, INCLUDE, and PATHenv variables be set to the WDK paths by some other means

## Optional software packages

- [Cygwin for windows](https://www.cygwin.com/)
  - Cygwin provides some Linux-like tools, which can be valuable for working with the repository,
    such as the BASH shell and git packages
  - With appropriate adjustments, it is possible to use other shells and environments as well

- [Ninja on all platforms](https://github.com/ninja-build/ninja/releases)
- [The Ninja-build project](https://ninja-build.org)
- [Ninja Users Manual](https://ninja-build.org/manual.html)

- [QtCreator as IDE for CMake builds on all platforms](https://qt.io/download-open-source/#section-2)

## Build Options

When generating native platform build files through CMake, several options can be specified to customize the build.
Some of the options are binary on/off options, while others take a string as input.
The following is a table of all on/off options currently supported by this repository:

| Option | Platform | Default | Description |
| ------ | -------- | ------- | ----------- |
| BUILD_LOADER | All | `ON` | Controls whether or not the loader is built. Setting this to `OFF` will allow building the tests against a loader that is installed to the system. |
| BUILD_TESTS | All | `ON` | Controls whether or not the loader tests are built. |
| BUILD_WSI_XCB_SUPPORT | Linux | `ON` | Build the loader with the XCB entry points enabled. Without this, the XCB headers should not be needed, but the extension `VK_KHR_xcb_surface` won't be available. |
| BUILD_WSI_XLIB_SUPPORT | Linux | `ON` | Build the loader with the Xlib entry points enabled. Without this, the X11 headers should not be needed, but the extension `VK_KHR_xlib_surface` won't be available. |
| BUILD_WSI_WAYLAND_SUPPORT | Linux | `ON` | Build the loader with the Wayland entry points enabled. Without this, the Wayland headers should not be needed, but the extension `VK_KHR_wayland_surface` won't be available. |
| BUILD_WSI_MIR_SUPPORT | Linux | `OFF` | Build the loader with the Mir entry points enabled. Without this, the Mir headers should not be needed, but the extension `VK_KHR_mir_surface` won't be available. |
| ENABLE_STATIC_LOADER | Windows | `OFF` | By default, the loader is built as a dynamic library. This allows it to be built as a static library, instead. |
| ENABLE_WIN10_ONECORE | Windows | `OFF` | Link the loader to the [OneCore](https://msdn.microsoft.com/en-us/library/windows/desktop/mt654039.aspx) umbrella library, instead of the standard Win32 ones. |
| USE_CCACHE | Linux | `OFF` | Enable caching with the CCache program. |

The following is a table of all string options currently supported by this repsitory:

| Option | Platform | Default | Description |
| ------ | -------- | ------- | ----------- |
| CMAKE_OSX_DEPLOYMENT_TARGET | MacOS | `10.12` | The minimum version of MacOS for loader deployment. |
| FALLBACK_CONFIG_DIRS | Linux/MacOS | `/etc/xdg` | Configuration path(s) to use instead of `XDG_CONFIG_DIRS` if that environment variable is unavailable. The default setting is freedesktop compliant. |
| FALLBACK_DATA_DIRS | Linux/MacOS | `/usr/local/share:/usr/share` | Configuration path(s) to use instead of `XDG_DATA_DIRS` if that environment variable is unavailable. The default setting is freedesktop compliant. |

These environment variables should be set using the `-D` flag when invoking CMake to generate the native platform files.
For example, to generate a Linux makefile with the tests disabled and a custom configuration directory, one would run:

```shell
cmake .. -DBUILD_TESTS=OFF -DFALLBACK_CONFIG_DIRS=/my/custom/location
```

The directory can then be built as usual, which on Linux would simply be the `make` command.
