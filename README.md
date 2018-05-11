# Vulkan Ecosystem Components

This project provides the Khronos official Vulkan ICD desktop loader
for Windows, Linux, and MacOS.

## CI Build Status

| Platform | Build Status |
|:--------:|:------------:|
| Linux | [![Build Status](https://travis-ci.org/KhronosGroup/Vulkan-Loader.svg?branch=master)](https://travis-ci.org/KhronosGroup/Vulkan-Loader) |
| Windows |[![Build status](https://ci.appveyor.com/api/projects/status/l93pu0w90tui708m?svg=true)](https://ci.appveyor.com/project/Khronoswebmaster/vulkan-loader/branch/master) |

## Introduction

Vulkan is an Explicit API, enabling direct control over how GPUs actually work.
By design, minimal error checking is done inside a Vulkan driver.
Applications have full control and responsibility for correct operation.
Any errors in how Vulkan is used can result in a crash.
This project provides Vulkan validation layers that can be enabled
to assist development by enabling developers to verify their applications
correct use of the Vulkan API.

Vulkan supports multiple GPUs and multiple global contexts (VkInstance).
The ICD loader is necessary to support multiple GPUs and VkInstance-level Vulkan commands.
Additionally, the loader manages inserting Vulkan layer libraries such as
validation layers between the application and the ICD.

The following components are available in this repository:

- [Vulkan header files (Vulkan-Headers submodule)](https://github.com/KhronosGroup/Vulkan-Headers)
- [*ICD Loader*](loader/)
- [*Tests*](tests/)

## Contact Information

- [Lenny Komow](mailto:lenny@lunarg.com)

## Information for Developing or Contributing

Please see the [CONTRIBUTING.md](CONTRIBUTING.md) file in this repository for more details.
Please see the [GOVERNANCE.md](GOVERNANCE.md) file in this repository for repository
management details.

## How to Build and Run

[BUILD.md](BUILD.md)
Includes directions for building all components.

Architecture and interface information for the loader is in
[loader/LoaderAndLayerInterface.md](loader/LoaderAndLayerInterface.md).

## License

This work is released as open source under a Apache-style license from Khronos
including a Khronos copyright.

See COPYRIGHT.txt for a full list of licenses used in this repository.

## Acknowledgements

While this project has been developed primarily by LunarG, Inc., there are many other
companies and individuals making this possible: Valve Corporation, funding
project development; Google providing significant contributions to the validation layers;
Khronos providing oversight and hosting of the project.
