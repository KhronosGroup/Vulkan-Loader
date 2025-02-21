# Fuchsia extensions to the loader

## Shared library opening

The loader doesn't have direct access to the filesystem containing an ICD. Instead, it must connect
to the [Vulkan loader service][loader-service] at `/svc/fuchsia.vulkan.loader.Loader`. That service
can provide an executable VMO for an ICD, which can be loaded using `dlopen_vmo`.

## Open in namespace callback

According to the [Fuchsia system ABI][abi], Vulkan ICDs cannot directly use the filesystem to read
files or connect to services. Instead loader will call the `vk_icdInitializeOpenInNamespaceCallback`
function in the ICD immediately after the ICD is dlopened. The callback may be used by the ICD to
connect to services, or the driver device node or to read from files.

## Exporting device nodes

If an ICD tries to open a path under `/loader-gpu-devices` using
`vk_icdInitializeOpenInNamespaceCallback`, the loader will redirect that request to a directory
returned by [fuchsia.vulkan.loader/Loader.ConnectToDeviceFs][connect-device-fs]. This allows the
loader service to ensure all devnodes the ICD needs are accessible to it, without the application or
application runtime needing to special-case device nodes in the application's component manifest.

## Getting ICD manifests

[ICD manifests][manifests] are retrieved using [ConnectToManifestFs][connect-manifest-fs] on the
loader service instead of being read directly from the filesystem. This allows ICD manifests to be
packaged alongside the ICD shared library. Layer manifests are read from the filesystem like normal.

[loader-service]: https://fuchsia.googlesource.com/fuchsia/+/refs/heads/main/src/graphics/bin/vulkan_loader/README.md
[abi]: https://fuchsia.dev/fuchsia-src/concepts/system/abi/system?hl=en
[connect-device-fs]: https://fuchsia.dev/reference/fidl/fuchsia.vulkan.loader?hl=en#fuchsia.vulkan.loader/Loader.ConnectToDeviceFs
[connect-manifest-fs]: https://fuchsia.dev/reference/fidl/fuchsia.vulkan.loader?hl=en#fuchsia.vulkan.loader/Loader.ConnectToManifestFs
[manifests]: LoaderAndLayerInterface.md
