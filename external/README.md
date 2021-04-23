# External dependencies

This directory provides a location where external projects can be cloned that are used by the loader.

Tests require GoogleTest on all platforms and Detours on Windows only.

The dependencies for testing need to be manually downloaded into this folder. CMake configuring will fail if they are not present
If the repositories are already in the `external` folder, then CMake won't download them.

To download googletest

```
git clone https://github.com/google/googletest.git
```

To download Detours (only necessary for Windows)

```
git clone https://github.com/microsoft/Detours.git
```

If you don't have a local install of the Vulkan Headers they can be placed in
the externals folder with:

```
git clone https://github.com/KhronosGroup/Vulkan-Headers.git
```
