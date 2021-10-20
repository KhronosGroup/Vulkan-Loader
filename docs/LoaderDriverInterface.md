<!-- markdownlint-disable MD041 -->
[![Khronos Vulkan][1]][2]

[1]: https://vulkan.lunarg.com/img/Vulkan_100px_Dec16.png "https://www.khronos.org/vulkan/"
[2]: https://www.khronos.org/vulkan/

# Driver interface to the Vulkan Loader
[![Creative Commons][3]][4]

<!-- Copyright &copy; 2015-2021 LunarG, Inc. -->

[3]: https://i.creativecommons.org/l/by-nd/4.0/88x31.png "Creative Commons License"
[4]: https://creativecommons.org/licenses/by-nd/4.0/


## Table of Contents

* [Overview](#overview)
* [Driver Discovery](#driver-discovery)
  * [Overriding the Default Driver Discovery](#overriding-the-default-driver-discovery)
    * [Exception for Elevated Privileges](#exception-for-elevated-privileges)
  * [Driver Manifest File Usage](#driver-manifest-file-usage)
  * [Driver Discovery on Windows](#driver-discovery-on-windows)
  * [Driver Discovery on Linux](#driver-discovery-on-linux)
    * [Example Linux Driver Search Path](#example-linux-driver-search-path)
  * [Driver Discovery on Fuchsia](#driver-discovery-on-fuchsia)
  * [Driver Discovery on macOS](#driver-discovery-on-macos)
    * [Example macOS Driver Search Path](#example-macos-driver-search-path)
  * [Using Pre-Production ICDs or Software Drivers](#using-pre-production-icds-or-software-drivers)
  * [Driver Discovery on Android](#driver-discovery-on-android)
* [Driver Manifest File Format](#driver-manifest-file-format)
  * [Driver Manifest File Versions](#driver-manifest-file-versions)
    * [Driver Manifest File Version 1.0.0](#driver-manifest-file-version-100)
* [Driver Vulkan Entry Point Discovery](#driver-vulkan-entry-point-discovery)
* [Driver API Version](#driver-api-version)
* [Mixed Driver Instance Extension Support](#mixed-driver-instance-extension-support)
  * [Filtering Out Instance Extension Names](#filtering-out-instance-extension-names)
  * [Loader Instance Extension Emulation Support](#loader-instance-extension-emulation-support)
* [Driver Unknown Physical Device Extensions](#driver-unknown-physical-device-extensions)
* [Driver Dispatchable Object Creation](#driver-dispatchable-object-creation)
* [Handling KHR Surface Objects in WSI Extensions](#handling-khr-surface-objects-in-wsi-extensions)
* [Loader and Driver Interface Negotiation](#loader-and-driver-interface-negotiation)
  * [Windows, Linux, and macOS Driver Negotiation](#windows-linux-and-macos-driver-negotiation)
    * [Version Negotiation Between Loader and Drivers](#version-negotiation-between-loader-and-drivers)
    * [Interfacing With Legacy Drivers or Loaders](#interfacing-with-legacy-drivers-or-loaders)
    * [Loader Version 6 Interface Requirements](#loader-version-6-interface-requirements)
    * [Loader Version 5 Interface Requirements](#loader-version-5-interface-requirements)
    * [Loader Version 4 Interface Requirements](#loader-version-4-interface-requirements)
    * [Loader Version 3 Interface Requirements](#loader-version-3-interface-requirements)
    * [Loader Version 2 Interface Requirements](#loader-version-2-interface-requirements)
    * [Loader Version 1 Interface Requirements](#loader-version-1-interface-requirements)
    * [Loader Version 0 Interface Requirements](#loader-version-0-interface-requirements)
    * [Additional Interface Notes](#additional-interface-notes)
  * [Android Driver Negotiation](#android-driver-negotiation)


## Overview

This is the Driver-centric view of working with the Vulkan loader.
For the complete overview of all sections of the loader, please refer to the
[LoaderInterfaceArchitecture.md](LoaderInterfaceArchitecture.md) file.

**NOTE:** While many of the interfaces still use the "icd" sub-string to
identify various behavior associated with drivers, this is purely
historical and should not indicate that the implementing code do so through
the traditional ICD interface.
Granted, the majority of drivers to this date are ICD drivers
targeting specific GPU hardware.

## Driver Discovery

Vulkan allows multiple drivers each with one or more devices
(represented by a Vulkan `VkPhysicalDevice` object) to be used collectively.
The loader is responsible for discovering available Vulkan drivers on
the system.
Given a list of available drivers, the loader can enumerate all the
physical devices available for an application and return this information to the
application.
The process in which the loader discovers the available drivers on a
system is platform-dependent.
Windows, Linux, Android, and macOS Driver Discovery details are listed
below.

### Overriding the Default Driver Discovery

There may be times that a developer wishes to force the loader to use a specific
Driver.
This could be for many reasons including using a beta driver, or forcing the
loader to skip a problematic driver.
In order to support this, the loader can be forced to look at specific
drivers with the `VK_ICD_FILENAMES` environment variable.

The `VK_ICD_FILENAMES` environment variable is a list of Driver Manifest
files, containing the full path to the driver JSON Manifest file.
This list is colon-separated on Linux and macOS, and semicolon-separated on
Windows.
Typically, `VK_ICD_FILENAMES` will only contain a full pathname to one info
file for a single driver.
A separator (colon or semicolon) is only used if more than one driver is needed.

#### Exception for Elevated Privileges

For security reasons, `VK_ICD_FILENAMES` is ignored if running the Vulkan
application with elevated privileges.
Because of this, `VK_ICD_FILENAMES` can only be used for applications that do not
use elevated privileges.

For more information see
[Elevated Privilege Caveats](LoaderInterfaceArchitecture.md#elevated-privilege-caveats)
in the top-level
[LoaderInterfaceArchitecture.md][LoaderInterfaceArchitecture.md] document.

#### Examples

In order to use the setting, simply set it to a properly delimited list of
Driver Manifest files.
In this case, please provide the global path to these files to reduce issues.

For example:

##### On Windows

```
set VK_ICD_FILENAMES=\windows\system32\nv-vk64.json
```

This is an example which is using the `VK_ICD_FILENAMES` override on Windows to
point to the Nvidia Vulkan Driver's Manifest file.

##### On Linux

```
export VK_ICD_FILENAMES=/home/user/dev/mesa/share/vulkan/icd.d/intel_icd.x86_64.json
```

This is an example which is using the `VK_ICD_FILENAMES` override on Linux to
point to the Intel Mesa Driver's Manifest file.

##### On macOS

```
export VK_ICD_FILENAMES=/home/user/MoltenVK/Package/Latest/MoltenVK/macOS/MoltenVK_icd.json
```

This is an example which is using the `VK_ICD_FILENAMES` override on macOS to
point to an installation and build of the MoltenVK GitHub repository that
contains the MoltenVK driver.

See the
[Table of Debug Environment Variables](LoaderInterfaceArchitecture.md#table-of-debug-environment-variables)
in the [LoaderInterfaceArchitecture.md document](LoaderInterfaceArchitecture.md)
for more details


### Driver Manifest File Usage

As with layers, on Windows, Linux and macOS systems, JSON-formatted manifest
files are used to store driver information.
In order to find system-installed drivers, the Vulkan loader will read the JSON
files to identify the names and attributes of each driver.
Notice that Driver Manifest files are much simpler than the corresponding
layer Manifest files.

See the
[Current Driver Manifest File Format](#driver-manifest-file-format)
section for more details.


### Driver Discovery on Windows

In order to find available drivers (including installed ICDs), the
loader scans through registry keys specific to Display Adapters and all Software
Components associated with these adapters for the locations of JSON manifest
files.
These keys are located in device keys created during driver installation and
contain configuration information for base settings, including OpenGL and
Direct3D locations.

The Device Adapter and Software Component key paths will be obtained by first
enumerating DXGI adapters.
Should that fail it will use the PnP Configuration Manager API.
The `000X` key will be a numbered key, where each device is assigned a different
number.

```
HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\Class\{Adapter GUID}\000X\VulkanDriverName
HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\Class\{SoftwareComponent GUID}\000X\VulkanDriverName
```

In addition, on 64-bit systems there may be another set of registry values,
listed below.
These values record the locations of 32-bit layers on 64-bit operating systems,
in the same way as the Windows-on-Windows functionality.

```
HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\Class\{Adapter GUID}\000X\VulkanDriverNameWow
HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\Class\{SoftwareComponent GUID}\000X\VulkanDriverNameWow
```

If any of the above values exist and is of type `REG_SZ`, the loader will open
the JSON manifest file specified by the key value.
Each value must be a full absolute path to a JSON manifest file.
The values may also be of type `REG_MULTI_SZ`, in which case the value will be
interpreted as a list of paths to JSON manifest files.

Additionally, the Vulkan loader will scan the values in the following Windows
registry key:

```
HKEY_LOCAL_MACHINE\SOFTWARE\Khronos\Vulkan\Drivers
```

For 32-bit applications on 64-bit Windows, the loader scan's the 32-bit
registry location:

```
HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Khronos\Vulkan\Drivers
```

Every driver in these locations should be given as a DWORD, with value 0, where
the name of the value is the full path to a JSON manifest file.
The Vulkan loader will attempt to open each manifest file to obtain the
information about a driver's shared library (".dll") file.

For example, let us assume the registry contains the following data:

```
[HKEY_LOCAL_MACHINE\SOFTWARE\Khronos\Vulkan\Drivers\]

"C:\vendor a\vk_vendora.json"=dword:00000000
"C:\windows\system32\vendorb_vk.json"=dword:00000001
"C:\windows\system32\vendorc_icd.json"=dword:00000000
```

In this case, the loader will step through each entry, and check the value.
If the value is 0, then the loader will attempt to load the file.
In this case, the loader will open the first and last listings, but not the
middle.
This is because the value of 1 for vendorb_vk.json disables the driver.

The Vulkan loader will open each enabled manifest file found to obtain the name
or pathname of a driver's shared library (".DLL") file.

Drivers should use the registry locations from the PnP Configuration
Manager wherever practical.
Typically, this is most important for drivers, and the location clearly
ties the driver to a given device.
The `SOFTWARE\Khronos\Vulkan\Drivers` location is the older method for locating
drivers, but is the primary location for software based drivers.

See the
[Driver Manifest File Format](#driver-manifest-file-format)
section for more details.


### Driver Discovery on Linux

On Linux, the Vulkan loader will scan for Driver Manifest files using
environment variables or corresponding fallback values if the corresponding
environment variable is not defined:

<table style="width:100%">
  <tr>
    <th>Search Order</th>
    <th>Directory/Environment Variable</th>
    <th>Fallback</th>
    <th>Additional Notes</th>
  </tr>
  <tr>
    <td>1</td>
    <td>$XDG_CONFIG_HOME</td>
    <td>$HOME/.config</td>
    <td><b>This path is ignored when running with elevated privileges such as
           setuid, setgid, or filesystem capabilities</b>.<br/>
        This is done because under these scenarios it is not safe to trust
        that the environment variables are non-malicious.<br/>
        See <a href="LoaderInterfaceArchitecture.md#elevated-privilege-caveats">
        Elevated Privilege Caveats</a> for more information.
    </td>
  </tr>
  <tr>
    <td>1</td>
    <td>$XDG_CONFIG_DIRS</td>
    <td>/etc/xdg</td>
    <td></td>
  </tr>
  <tr>
    <td>2</td>
    <td>SYSCONFDIR</td>
    <td>/etc</td>
    <td>Compile-time option set to possible location of drivers
        installed from non-Linux-distribution-provided packages.
    </td>
  </tr>
  <tr>
    <td>3</td>
    <td>EXTRASYSCONFDIR</td>
    <td>/etc</td>
    <td>Compile-time option set to possible location of drivers
        installed from non-Linux-distribution-provided packages.
        Typically only set if SYSCONFDIR is set to something other than /etc
    </td>
  </tr>
  <tr>
    <td>4</td>
    <td>$XDG_DATA_HOME</td>
    <td>$HOME/.local/share</td>
    <td><b>This path is ignored when running with elevated privileges such as
           setuid, setgid, or filesystem capabilities</b>.<br/>
        This is done because under these scenarios it is not safe to trust
        that the environment variables are non-malicious.<br/>
        See <a href="LoaderInterfaceArchitecture.md#elevated-privilege-caveats">
        Elevated Privilege Caveats</a> for more information.
    </td>
  </tr>
  <tr>
    <td>5</td>
    <td>$XDG_DATA_DIRS</td>
    <td>/usr/local/share/:/usr/share/</td>
    <td></td>
  </tr>
</table>

The directory lists are concatenated together using the standard platform path
separator (:).
The loader then selects each path, and applies the "/vulkan/icd.d" suffix onto
each and looks in that specific folder for manifest files.

The Vulkan loader will open each manifest file found to obtain the name or
pathname of a driver's shared library (".dylib") file.

**NOTE** While the order of folders searched for manifest files is well
defined, the order contents are read by the loader in each directory is
[random due to the behavior of readdir](https://www.ibm.com/support/pages/order-directory-contents-returned-calls-readdir).

See the
[Driver Manifest File Format](#driver-manifest-file-format)
section for more details.

It is also important to note that while `VK_LAYER_PATH` will point the loader
to finding the manifest files, it does not guarantee the library files mentioned
by the manifest will immediately be found.
Often, the Driver Manifest file will point to the library file using a
relative or absolute path.
When a relative or absolute path is used, the loader can typically find the
library file without querying the operating system.
However, if a library is listed only by name, the loader may not find it,
unless the driver is installed placing the library in an operating system
searchable default location.
If problems occur finding a library file associated with a driver, try updating
the `LD_LIBRARY_PATH` environment variable to point at the location of the
corresponding `.so` file.


#### Example Linux Driver Search Path

For a fictional user "me" the Driver Manifest search path might look
like the following:

```
  /home/me/.config/vulkan/icd.d
  /etc/xdg/vulkan/icd.d
  /usr/local/etc/vulkan/icd.d
  /etc/vulkan/icd.d
  /home/me/.local/share/vulkan/icd.d
  /usr/local/share/vulkan/icd.d
  /usr/share/vulkan/icd.d
```


### Driver Discovery on Fuchsia

On Fuchsia, the Vulkan loader will scan for manifest files using environment
variables or corresponding fallback values if the corresponding environment
variable is not defined in the same way as
[Linux](#linux-driver-discovery).
The **only** difference is that Fuchsia does not allow fallback values for
*$XDG_DATA_DIRS* or *$XDG_HOME_DIRS*.


### Driver Discovery on macOS

On macOS, the Vulkan loader will scan for Driver Manifest files using
the application resource folder as well as environment variables or
corresponding fallback values if the corresponding environment variable is not
defined.
The order is similar to the search path on Linux with the exception that
the application's bundle resources are searched first:
`(bundle)/Contents/Resources/`.

#### Example macOS Driver Search Path

For a fictional user "Me" the Driver Manifest search path might look
like the following:

```
  <bundle>/Contents/Resources/vulkan/icd.d
  /Users/Me/.config/vulkan/icd.d
  /etc/xdg/vulkan/icd.d
  /usr/local/etc/vulkan/icd.d
  /etc/vulkan/icd.d
  /Users/Me/.local/share/vulkan/icd.d
  /usr/local/share/vulkan/icd.d
  /usr/share/vulkan/icd.d
```


#### Additional Settings For Driver Debugging

Sometimes, the driver may encounter issues when loading.
A useful option may be to enable the `LD_BIND_NOW` environment variable
to debug the issue.
This forces every dynamic library's symbols to be fully resolved on load.
If there is a problem with a driver missing symbols on the current system, this
will expose it and cause the Vulkan loader to fail on loading the driver.
It is recommended that `LD_BIND_NOW` along with `VK_LOADER_DEBUG=error,warn`
to expose any issues.


### Using Pre-Production ICDs or Software Drivers

Both software and pre-production ICDs can use an alternative mechanism to
detect their drivers.
Independent Hardware Vendor (IHV) may not want to fully install a pre-production
ICD and so it can't be found in the standard location.
For example, a pre-production ICD may simply be a shared library in the
developer's build tree.
In this case, there should be a way to allow developers to point to such an
ICD without modifying the system-installed ICD(s) on their system.

This need is met with the use of the `VK_ICD_FILENAMES` environment variable,
which will override the mechanism used for finding system-installed
drivers.

In other words, only the drivers listed in `VK_ICD_FILENAMES` will be
used.

See
[Overriding the Default Driver Discovery](#overriding-the-default-driver-discovery)
for more information on this.


### Driver Discovery on Android

The Android loader lives in the system library folder.
The location cannot be changed.
The loader will load the driver via `hw_get_module` with the ID of "vulkan".
**Due to security policies in Android, none of this can be modified under**
**normal use.**


## Driver Manifest File Format

The following section discusses the details of the Driver Manifest JSON
file format.
The JSON file itself does not have any requirements for naming.
The only requirement is that the extension suffix of the file is ".json".

Here is an example driver JSON Manifest file:

```
{
   "file_format_version": "1.0.0",
   "ICD": {
      "library_path": "path to driver library",
      "api_version": "1.0.5"
   }
}
```

<table style="width:100%">
  <tr>
    <th>Field Name</th>
    <th>Field Value</th>
  </tr>
  <tr>
    <td>"file_format_version"</td>
    <td>The JSON format major.minor.patch version number of this file.<br/>
        Currently supported version is 1.0.0.</td>
  </tr>
  <tr>
    <td>"ICD"</td>
    <td>The identifier used to group all driver information together.
        <br/>
        <b>NOTE:</b> Even though this is labelled <i>ICD</i> it is historical
        and just as accurate to use for other drivers.</td>
  </tr>
  <tr>
    <td>"library_path"</td>
    <td>The "library_path" specifies either a filename, a relative pathname, or
        a full pathname to a driver shared library file. <br />
        If "library_path" specifies a relative pathname, it is relative to the
        path of the JSON manifest file. <br />
        If "library_path" specifies a filename, the library must live in the
        system's shared object search path. <br />
        There are no rules about the name of the driver's shared library file
        other than it should end with the appropriate suffix (".DLL" on
        Windows, ".so" on Linux and ".dylib" on macOS).</td>
  </tr>
  <tr>
    <td>"api_version" </td>
    <td>The major.minor.patch version number of the Vulkan API that the shared
        library files for the driver was built against.<br/>
        For example: 1.0.33.</td>
  </tr>
</table>

**NOTE:** If the same driver shared library supports multiple, incompatible
versions of text manifest file format versions, it must have separate JSON files
for each (all of which may point to the same shared library).

#### Driver Manifest File Versions

There has only been one version of the Driver Manifest files supported.
This is version 1.0.0.

#### Driver Manifest File Version 1.0.0

The initial version of the Driver Manifest file specified the basic
format and fields of a layer JSON file.
The fields supported in version 1.0.0 of the file format include:
 * "file\_format\_version"
 * "ICD"
 * "library\_path"
 * "api\_version"


##  Driver Vulkan Entry Point Discovery

The Vulkan symbols exported by a driver must not clash with the loader's
exported Vulkan symbols.
Because of this, all drivers must export the following function that is
used for discovery of driver Vulkan entry-points.
This entry-point is not a part of the Vulkan API itself, only a private
interface between the loader and drivers for version 1 and higher
interfaces.

```cpp
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
   vk_icdGetInstanceProcAddr(
      VkInstance instance,
      const char* pName);
```

This function has very similar semantics to `vkGetInstanceProcAddr`.
`vk_icdGetInstanceProcAddr` returns valid function pointers for all the
global-level and instance-level Vulkan functions, and also for
`vkGetDeviceProcAddr`.
Global-level functions are those which contain no dispatchable object as the
first parameter, such as `vkCreateInstance` and
`vkEnumerateInstanceExtensionProperties`.
The driver must support querying global-level entry points by calling
`vk_icdGetInstanceProcAddr` with a NULL `VkInstance` parameter.
Instance-level functions are those that have either `VkInstance`, or
`VkPhysicalDevice` as the first parameter dispatchable object.
Both core entry points and any instance extension entry points the
driver supports should be available via `vk_icdGetInstanceProcAddr`.
Future Vulkan instance extensions may define and use new instance-level
dispatchable objects other than `VkInstance` and `VkPhysicalDevice`, in which
case extension entry points using these newly defined dispatchable objects must
be queryable via `vk_icdGetInstanceProcAddr`.

All other Vulkan entry points must either:
 * NOT be exported directly from the driver library
 * or NOT use the official Vulkan function names if they are exported

This requirement is for driver libraries that include other functionality (such
as OpenGL) and thus could be loaded by the application prior to when the Vulkan
loader library is loaded by the application.

Beware of interposing by dynamic OS library loaders if the official Vulkan
names are used.
On Linux, if official names are used, the driver library must be linked with
`-Bsymbolic`.


## Driver API Version

When an application calls `vkCreateInstance`, it can optionally include a
`VkApplicationInfo` struct, which includes an `apiVersion` field.
A Vulkan 1.0 driver was required to return `VK_ERROR_INCOMPATIBLE_DRIVER` if it
did not support the API version that the user passed.
Beginning with Vulkan 1.1, drivers are not allowed to return this error
for any value of `apiVersion`.
This creates a problem when working with multiple drivers, where one is
a 1.0 driver and another is newer.

A loader that is newer than 1.0 will always give the version it supports when
the application calls `vkEnumerateInstanceVersion`, regardless of the API
version supported by the drivers on the system.
This means that when the application calls `vkCreateInstance`, the loader will
be forced to pass a copy of the `VkApplicationInfo` struct where `apiVersion` is
1.0 to any 1.0 drivers in order to prevent an error.
To determine if this must be done, the loader will perform the following steps:

1. Check the driver's JSON manifest file for the "api_version" field.
2. If the JSON version is greater than or equal to 1.1, Load the driver's
dynamic library
3. Call the driver's `vkGetInstanceProcAddr` command to get a pointer to
`vkEnumerateInstanceVersion`
4. If the pointer to `vkEnumerateInstanceVersion` is not `NULL`, it will be
called to get the driver's supported API version

The driver will be treated as a 1.0 driver if any of the following conditions
are met:

- The JSON manifest's "api_version" field is less that version 1.1
- The function pointer to `vkEnumerateInstanceVersion` is `NULL`
- The version returned by `vkEnumerateInstanceVersion` is less than 1.1
- `vkEnumerateInstanceVersion` returns anything other than `VK_SUCCESS`

If the driver only supports Vulkan 1.0, the loader will ensure that any
`VkApplicationInfo` struct that is passed to the driver will have an
`apiVersion` field set to Vulkan 1.0.
Otherwise, the loader will pass the struct to the driver without any
changes.


## Mixed Driver Instance Extension Support

On a system with more than one driver, a special case can arise.
Some drivers may expose an instance extension that the loader is already
aware of.
Other drivers on that same system may not support the same instance
extension.

In that scenario, the loader has some additional responsibilities:


### Filtering Out Instance Extension Names

During a call to `vkCreateInstance`, the list of requested instance extensions
is passed down to each driver.
Since the driver may not support one or more of these instance extensions, the
loader will filter out any instance extensions that are not supported by the
driver.
This is done per driver since different drivers may support different instance
extensions.


### Loader Instance Extension Emulation Support

In the same scenario, the loader must emulate the instance extension
entry-points, to the best of its ability, for each driver that does not support
an instance extension directly.
This must work correctly when combined with calling into the other
drivers which do support the extension natively.
In this fashion, the application will be unaware of what drivers are
missing support for this extension.


## Driver Unknown Physical Device Extensions

Originally, when the loader's `vkGetInstanceProcAddr` was called, it would
result in the following behavior:
 1. The loader would check if it was a core function:
    - If so, it would return the function pointer
 2. The loader would check if it was a known extension function:
    - If so, it would return the function pointer
 3. If the loader knew nothing about it, it would call down using
`GetInstanceProcAddr`
    - If it returned `non-NULL`, treat it as an unknown logical device command.
    - This meant setting up a generic trampoline function that takes in a
VkDevice as the first parameter and adjusting the dispatch table to call the
driver/layer's function after getting the dispatch table from the
`VkDevice`.
 4. If all the above failed, the loader would return `NULL` to the application.

This caused problems when a driver attempted to expose new physical device
extensions the loader knew nothing about, but an application was aware of.
Because the loader knew nothing about it, the loader would get to step 3 in the
above process and would treat the function as an unknown logical device command.
The problem is, this would create a generic `VkDevice` trampoline function
which, on the first call, would attempt to dereference the VkPhysicalDevice as a
`VkDevice`.
This would lead to a crash or corruption.

In order to identify the extension entry points specific to physical device
extensions, the following function can be added to a driver:

```cpp
PFN_vkVoidFunction
   vk_icdGetPhysicalDeviceProcAddr(
      VkInstance instance,
      const char* pName);
```

This function behaves similar to `vkGetInstanceProcAddr` and
`vkGetDeviceProcAddr` except it should only return values for physical device
extension entry points.
In this way, it compares "pName" to every physical device function supported in
the driver.

The following rules apply:
* If it is the name of a physical device function supported by the driver, the
pointer to the driver's corresponding function should be returned.
* If it is the name of a valid function which is **not** a physical device
function (i.e. an instance, device, or other function implemented by the
driver), then the value of `NULL` should be returned.
* If the driver has no idea what this function is, it should return `NULL`.

This support is optional and should not be considered a requirement.
This is only required if a driver intends to support some functionality not
directly supported by a significant population of loaders in the public.
If a driver does implement this support, it must export the function from the
driver library using the name `vk_icdGetPhysicalDeviceProcAddr` so that the
symbol can be located through the platform's dynamic linking utilities.

The new behavior of the loader's vkGetInstanceProcAddr with support for the
`vk_icdGetPhysicalDeviceProcAddr` function is as follows:
 1. Check if core function:
    - If it is, return the function pointer
 2. Check if known instance or device extension function:
    - If it is, return the function pointer
 3. Call the layer/driver `GetPhysicalDeviceProcAddr`
    - If it returns `non-NULL`, return a trampoline to a generic physical device
function, and set up a generic terminator which will pass it to the proper
driver.
 4. Call down using `GetInstanceProcAddr`
    - If it returns non-NULL, treat it as an unknown logical device command.
This means setting up a generic trampoline function that takes in a `VkDevice`
as the first parameter and adjusting the dispatch table to call the
driver/layer's function after getting the dispatch table from the
`VkDevice`.
Then, return the pointer to the corresponding trampoline function.
 5. Return `NULL`

The result is that if the command gets promoted to Vulkan core later, it will no
longer be set up using `vk_icdGetPhysicalDeviceProcAddr`.
Additionally, if the loader adds direct support for the extension, it will no
longer get to step 3, because step 2 will return a valid function pointer.
However, the driver should continue to support the command query via
`vk_icdGetPhysicalDeviceProcAddr`, until at least a Vulkan version bump, because
an older loader may still be attempting to use the commands.


## Physical Device Sorting

When an application selects a GPU to use, it must enumerate physical devices or
physical device groups.
These API functions do not specify which order the physical devices or physical
device groups will be presented in.
On Windows, the loader will attempt to sort these objects so that the system
preference will be listed first.
This mechanism does not force an application to use any particular GPU &mdash;
it merely changes the order in which they are presented.

This mechanism requires that a driver provide version 6 of the loader/driver
interface.
Version 6 of this interface defines a new exported function that the driver may
provide on Windows:

```c
VKAPI_ATTR VkResult VKAPI_CALL
   vk_icdEnumerateAdapterPhysicalDevices(
      VkInstance instance,
      LUID adapterLUID,
      uint32_t* pPhysicalDeviceCount,
      VkPhysicalDevice* pPhysicalDevices);
```

This function takes an adapter LUID as input, and enumerates all Vulkan physical
devices that are associated with that LUID.
This works in the same way as other Vulkan enumerations &mdash; if
`pPhysicalDevices` is `NULL`, then the count will be provided.
Otherwise, the physical devices associated with the queried adapter will be
provided.
The function must provide multiple physical devices when the LUID refers to a
linked adapter.
This allows the loader to translate the adapter into Vulkan physical device
groups.

While the loader attempts to match the system's preference for GPU ordering,
there are some limitations.
Because this feature requires a new driver interface, only physical devices from
drivers that support this function will be sorted.
All unsorted physical devices will be listed at the end of the list, in an
indeterminate order.
Furthermore, only physical devices that correspond to an adapter may be sorted.
This means that a software driver would likely not be sorted.
Finally, this API only applies to Windows systems and will only work on versions
of Windows 10 that support GPU selection through the OS.
Other platforms may be included in the future, but they will require separate
platform-specific interfaces.


## Driver Dispatchable Object Creation

As previously covered, the loader requires dispatch tables to be accessible
within Vulkan dispatchable objects, such as: `VkInstance`, `VkPhysicalDevice`,
`VkDevice`, `VkQueue`, and `VkCommandBuffer`.
The specific requirements on all dispatchable objects created by drivers
are as follows:

- All dispatchable objects created by a driver can be cast to void \*\*
- The loader will replace the first entry with a pointer to the dispatch table
which is owned by the loader.
This implies three things for drivers:
  1. The driver must return a pointer for the opaque dispatchable object handle
  2. This pointer points to a regular C structure with the first entry being a
   pointer.
   * **NOTE:** For any C\++ drivers that implement VK objects directly
as C\++ classes:
     * The C\++ compiler may put a vtable at offset zero if the class is
non-POD due to the use of a virtual function.
     * In this case use a regular C structure (see below).
  3. The loader checks for a magic value (ICD\_LOADER\_MAGIC) in all the created
   dispatchable objects, as follows (see `include/vulkan/vk_icd.h`):

```cpp
#include "vk_icd.h"

union _VK_LOADER_DATA {
  uintptr loadermagic;
  void *  loaderData;
} VK_LOADER_DATA;

vkObj
   alloc_icd_obj()
{
  vkObj *newObj = alloc_obj();
  ...
  // Initialize pointer to loader's dispatch table with ICD_LOADER_MAGIC

  set_loader_magic_value(newObj);
  ...
  return newObj;
}
```


## Handling KHR Surface Objects in WSI Extensions

Normally, drivers handle object creation and destruction for various Vulkan
objects.
The WSI surface extensions for Linux, Windows, macOS, and QNX
("VK\_KHR\_win32\_surface", "VK\_KHR\_xcb\_surface", "VK\_KHR\_xlib\_surface",
"VK\_KHR\_wayland\_surface", "VK\_MVK\_macos\_surface",
"VK\_QNX\_screen\_surface" and "VK\_KHR\_surface") are handled differently.
For these extensions, the `VkSurfaceKHR` object creation and destruction may be
handled by either the loader or a driver.

If the loader handles the management of the `VkSurfaceKHR` objects:
 1. The loader will handle the calls to `vkCreateXXXSurfaceKHR` and
`vkDestroySurfaceKHR`
    functions without involving the drivers.
    * Where XXX stands for the Windowing System name:
      * Wayland
      * XCB
      * Xlib
      * Windows
      * Android
      * MacOS (`vkCreateMacOSSurfaceMVK`)
      * QNX (`vkCreateScreenSurfaceQNX`)
 2. The loader creates a `VkIcdSurfaceXXX` object for the corresponding
`vkCreateXXXSurfaceKHR` call.
    * The `VkIcdSurfaceXXX` structures are defined in `include/vulkan/vk_icd.h`.
 3. Drivers can cast any `VkSurfaceKHR` object to a pointer to the
appropriate `VkIcdSurfaceXXX` structure.
 4. The first field of all the `VkIcdSurfaceXXX` structures is a
`VkIcdSurfaceBase` enumerant that indicates whether the
    surface object is Win32, XCB, Xlib, Wayland, or Screen.

The driver may choose to handle `VkSurfaceKHR` object creation instead.
If a driver desires to handle creating and destroying it must do the following:
 1. Support version 3 or newer of the loader/driver interface.
 2. Export and handle all functions that take in a `VkSurfaceKHR` object,
including:
     * `vkCreateXXXSurfaceKHR`
     * `vkGetPhysicalDeviceSurfaceSupportKHR`
     * `vkGetPhysicalDeviceSurfaceCapabilitiesKHR`
     * `vkGetPhysicalDeviceSurfaceFormatsKHR`
     * `vkGetPhysicalDeviceSurfacePresentModesKHR`
     * `vkCreateSwapchainKHR`
     * `vkDestroySurfaceKHR`

Because the `VkSurfaceKHR` object is an instance-level object, one object can be
associated with multiple drivers.
Therefore, when the loader receives the `vkCreateXXXSurfaceKHR` call, it still
creates an internal `VkSurfaceIcdXXX` object.
This object acts as a container for each driver's version of the
`VkSurfaceKHR` object.
If a driver does not support the creation of its own `VkSurfaceKHR` object, the
loader's container stores a NULL for that driver.
On the other hand, if the driver does support `VkSurfaceKHR` creation, the
loader will make the appropriate `vkCreateXXXSurfaceKHR` call to the
driver, and store the returned pointer in its container object.
The loader then returns the `VkSurfaceIcdXXX` as a `VkSurfaceKHR` object back up
the call chain.
Finally, when the loader receives the `vkDestroySurfaceKHR` call, it
subsequently calls `vkDestroySurfaceKHR` for each driver whose internal
`VkSurfaceKHR` object is not NULL.
Then the loader destroys the container object before returning.


## Loader and Driver Interface Negotiation

Generally, for functions issued by an application, the loader can be viewed as a
pass through.
That is, the loader generally doesn't modify the functions or their parameters,
but simply calls the driver's entry point for that function.
There are specific additional interface requirements a driver needs to comply
with that are not part of any requirements from the Vulkan specification.
These additional requirements are versioned to allow flexibility in the future.


### Windows, Linux and macOS Driver Negotiation


#### Version Negotiation Between Loader and Drivers

All drivers (supporting interface version 2 or higher) must export the
following function that is used for determination of the interface version that
will be used.
This entry point is not a part of the Vulkan API itself, only a private
interface between the loader and drivers.

```cpp
VKAPI_ATTR VkResult VKAPI_CALL
   vk_icdNegotiateLoaderICDInterfaceVersion(
      uint32_t* pSupportedVersion);
```

This function allows the loader and driver to agree on an interface version to
use.
The "pSupportedVersion" parameter is both an input and output parameter.
"pSupportedVersion" is filled in by the loader with the desired latest interface
version supported by the loader (typically the latest).
The driver receives this and returns back the version it desires in the same
field.
Because it is setting up the interface version between the loader and
driver, this should be the first call made by a loader to the driver (even prior
to any calls to `vk_icdGetInstanceProcAddr`).

If the driver receiving the call no longer supports the interface version
provided by the loader (due to deprecation), then it should report a
`VK_ERROR_INCOMPATIBLE_DRIVER` error.
Otherwise it sets the value pointed by "pSupportedVersion" to the latest
interface version supported by both the driver and the loader and returns
`VK_SUCCESS`.

The driver should report `VK_SUCCESS` in case the loader-provided interface
version is newer than that supported by the driver, as it's the loader's
responsibility to determine whether it can support the older interface version
supported by the driver.
The driver should also report `VK_SUCCESS` in the case its interface version is
greater than the loader's, but return the loader's version.
Thus, upon return of `VK_SUCCESS` the "pSupportedVersion" will contain the
desired interface version to be used by the driver.

If the loader receives an interface version from the driver that the loader no
longer supports (due to deprecation), or it receives a
`VK_ERROR_INCOMPATIBLE_DRIVER` error instead of `VK_SUCCESS`, then the loader
will treat the driver as incompatible and will not load it for use.
In this case, the application will not see the driver's `vkPhysicalDevice`
during enumeration.

#### Interfacing With Legacy Drivers or Loaders

If a loader sees that a driver does not export the
`vk_icdNegotiateLoaderICDInterfaceVersion` function, then the loader assumes the
corresponding driver only supports either interface version 0 or 1.

From the other side of the interface, if a driver sees a call to
`vk_icdGetInstanceProcAddr` before a call to
`vk_icdNegotiateLoaderICDInterfaceVersion`, then it knows that loader making the
calls is a legacy loader supporting version 0 or 1.
If the loader calls `vk_icdGetInstanceProcAddr` first, it supports at least
version 1.
Otherwise, the loader only supports version 0.

#### Loader Version 6 Interface Requirements

Version 6 provides a mechanism to allow the loader to sort physical devices.
The loader will only attempt to sort physical devices on a driver if version 6
of the interface is supported.
This version provides the `vk_icdEnumerateAdapterPhysicalDevices` function
defined earlier in this document.

#### Loader Version 5 Interface Requirements

Version 5 of the loader/driver interface has no changes to the actual interface.
If the loader requests interface version 5 or greater, it is simply
an indication to drivers that the loader is now evaluating whether the API
Version info passed into vkCreateInstance is a valid version for the loader.
If it is not, the loader will catch this during vkCreateInstance and fail with a
`VK_ERROR_INCOMPATIBLE_DRIVER` error.

On the other hand, if version 5 or newer is not requested by the loader, then it
indicates to the driver that the loader is ignorant of the API version being
requested.
Because of this, it falls on the driver to validate that the API Version is not
greater than major = 1 and minor = 0.
If it is, then the driver should automatically fail with a
`VK_ERROR_INCOMPATIBLE_DRIVER` error since the loader is a 1.0 loader, and is
unaware of the version.

Here is a table of the expected behaviors:

<table style="width:100%">
  <tr>
    <th>Loader Supports I/f Version</th>
    <th>Driver Supports I/f Version</th>
    <th>Result</th>
  </tr>
  <tr>
    <td>4 or Earlier</td>
    <td>Any Version</td>
    <td>Driver <b>must fail</b> with <b>VK_ERROR_INCOMPATIBLE_DRIVER</b>
        for all vkCreateInstance calls with apiVersion set to > Vulkan 1.0
        because the loader is still at interface version <= 4.<br/>
        Otherwise, the driver should behave as normal.
    </td>
  </tr>
  <tr>
    <td>5 or Newer</td>
    <td>4 or Earlier</td>
    <td>Loader <b>must fail</b> with <b>VK_ERROR_INCOMPATIBLE_DRIVER</b> if it
        can't handle the apiVersion.
        Driver may pass for all apiVersions, but since its interface is
        <= 4, it is best if it assumes it needs to do the work of rejecting
        anything > Vulkan 1.0 and fail with <b>VK_ERROR_INCOMPATIBLE_DRIVER</b>.
        <br/>
        Otherwise, the driver should behave as normal.
    </td>
  </tr>
  <tr>
    <td>5 or Newer</td>
    <td>5 or Newer</td>
    <td>Loader <b>must fail</b> with <b>VK_ERROR_INCOMPATIBLE_DRIVER</b> if it
        can't handle the apiVersion, and drivers should fail with
        <b>VK_ERROR_INCOMPATIBLE_DRIVER</b> <i>only if</i> they can not support
        the specified apiVersion. <br/>
        Otherwise, the driver should behave as normal.
    </td>
  </tr>
</table>

#### Loader Version 4 Interface Requirements

The major change to version 4 of the loader/driver interface is the
support of
[Unknown Physical Device Extensions](#driver-unknown-physical-device-extensions)
using the `vk_icdGetPhysicalDeviceProcAddr` function.
This function is purely optional.
However, if a driver supports a physical device extension, it must provide a
`vk_icdGetPhysicalDeviceProcAddr` function.
Otherwise, the loader will continue to treat any unknown functions as VkDevice
functions and cause invalid behavior.


#### Loader Version 3 Interface Requirements

The primary change that occurred in version 3 of the loader/driver interface was
to allow a driver to handle creation/destruction of their own KHR_surfaces.
Up until this point, the loader created a surface object that was used by all
drivers.
However, some drivers may want to provide their own surface handles.
If a driver chooses to enable this support, it must export support for version 3
of the loader/driver interface, as well as any Vulkan function that uses a
KHR_surface handle, such as:
- `vkCreateXXXSurfaceKHR` (where XXX is the platform-specific identifier [i.e.
`vkCreateWin32SurfaceKHR` for Windows])
- `vkDestroySurfaceKHR`
- `vkCreateSwapchainKHR`
- `vkGetPhysicalDeviceSurfaceSupportKHR`
- `vkGetPhysicalDeviceSurfaceCapabilitiesKHR`
- `vkGetPhysicalDeviceSurfaceFormatsKHR`
- `vkGetPhysicalDeviceSurfacePresentModesKHR`

A driver can still choose to not take advantage of this functionality
by simply not exposing the above `vkCreateXXXSurfaceKHR` and
`vkDestroySurfaceKHR` functions.


#### Loader Version 2 Interface Requirements

Version 2 interface is the first to implement the new
`vk_icdNegotiateLoaderICDInterfaceVersion` functionality, see
[Version Negotiation Between Loader and Drivers](#version-negotiation-between-loader-and-drivers) for more details
on that function.

Additional, version 2 was the first to define that Vulkan dispatchable objects
created by drivers must now be created in accordance to the
[Driver Dispatchable Object Creation](#driver-dispatchable-object-creation)
section.


#### Loader Version 1 Interface Requirements

Version 1 of the interface added the driver-specific entry-point
`vk_icdGetInstanceProcAddr`.
Since this is before the creation of the
`vk_icdNegotiateLoaderICDInterfaceVersion` entry-point, the loader has no
negotiation process for determine what interface version the driver
supports.
Because of this, the loader detects support for version 1 of the interface
by the absence of the negotiate function, but the presence of the
`vk_icdGetInstanceProcAddr`.
No other entry-points need to be exported by the driver as the loader will query
the appropriate function pointers using that.


#### Loader Version 0 Interface Requirements

Version 0 interface does not support either `vk_icdGetInstanceProcAddr` or
`vk_icdNegotiateLoaderICDInterfaceVersion`.
Because of this, the loader will assume the driver supports only version 0 of
the interface unless one of those functions exists.

Additionally, for version 0, the driver must expose at least the following core
Vulkan entry-points so the loader may build up the interface to the driver:

- The function `vkGetInstanceProcAddr` **must be exported** in the driver
library and returns valid function pointers for all the Vulkan API entry points.
- `vkCreateInstance` **must be exported** by the driver library.
- `vkEnumerateInstanceExtensionProperties` **must be exported** by the driver
library.


#### Additional Interface Notes:

- The loader will filter out extensions requested in `vkCreateInstance` and
`vkCreateDevice` before calling into the driver; filtering will be of extensions
advertised by entities (e.g. layers) different from the driver in question.
- The loader will not call the driver for `vkEnumerate*LayerProperties`
as layer properties are obtained from the layer libraries and layer JSON files.
- If a driver library author wants to implement a layer, it can do so by having
the appropriate layer JSON manifest file refer to the driver library file.
- The loader will not call the driver for `vkEnumerate*ExtensionProperties` if
"pLayerName" is not equal to `NULL`.
- Drivers creating new dispatchable objects via device extensions need
to initialize the created dispatchable object.
The loader has generic *trampoline* code for unknown device extensions.
This generic *trampoline* code doesn't initialize the dispatch table within the
newly created object.
See the
[Driver Dispatchable Object Creation](#driver-dispatchable-object-creation)
section for more information on how to initialize created dispatchable objects
for extensions non known by the loader.


### Android Driver Negotiation

The Android loader uses the same protocol for initializing the dispatch table as
described above.
The only difference is that the Android loader queries layer and extension
information directly from the respective libraries and does not use the JSON
manifest files used by the Windows, Linux and macOS loaders.

<br/>

[Return to the top-level LoaderInterfaceArchitecture.md file.](LoaderInterfaceArchitecture.md)
