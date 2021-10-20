/*
 *
 * Copyright (c) 2014-2021 The Khronos Group Inc.
 * Copyright (c) 2014-2021 Valve Corporation
 * Copyright (c) 2014-2021 LunarG, Inc.
 * Copyright (C) 2015 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Jon Ashburn <jon@lunarg.com>
 * Author: Courtney Goeltzenleuchter <courtney@LunarG.com>
 * Author: Chia-I Wu <olvaffe@gmail.com>
 * Author: Chia-I Wu <olv@lunarg.com>
 * Author: Mark Lobodzinski <mark@LunarG.com>
 * Author: Lenny Komow <lenny@lunarg.com>
 * Author: Charles Giessen <charles@lunarg.com>
 *
 */
// Windows only header file, guard it so that accidental inclusion doesn't cause unknown header include errors
#ifdef _WIN32

// This needs to be defined first, or else we'll get redefinitions on NTSTATUS values
#define UMDF_USING_NTSTATUS
#include <ntstatus.h>

#include "loader_windows.h"

#include "allocation.h"
#include "get_environment.h"
#include "loader.h"
#include "log.h"

#include <cfgmgr32.h>
#include <initguid.h>
#include <devpkey.h>
#include <winternl.h>
#include <strsafe.h>
#ifdef __MINGW32__
#undef strcpy  // fix error with redfined strcpy when building with MinGW-w64
#endif
#include <dxgi1_6.h>
#include "adapters.h"

#if !defined(NDEBUG)
#include <crtdbg.h>
#endif

typedef HRESULT(APIENTRY *PFN_CreateDXGIFactory1)(REFIID riid, void **ppFactory);
static PFN_CreateDXGIFactory1 fpCreateDXGIFactory1;

void windows_initialization(void) {
    char dll_location[MAX_PATH];
    HMODULE module_handle = NULL;

    // Get a module handle to a static function inside of this source
    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          (LPCSTR)&loader_debug_init, &module_handle) != 0 &&
        GetModuleFileName(module_handle, dll_location, sizeof(dll_location)) != 0) {
        loader_log(NULL, VULKAN_LOADER_INFO_BIT, 0, "Using Vulkan Loader %s", dll_location);
    }

    // This is needed to ensure that newer APIs are available right away
    // and not after the first call that has been statically linked
    LoadLibrary("gdi32.dll");

    TCHAR systemPath[MAX_PATH] = "";
    GetSystemDirectory(systemPath, MAX_PATH);
    StringCchCat(systemPath, MAX_PATH, TEXT("\\dxgi.dll"));
    HMODULE dxgi_module = LoadLibrary(systemPath);
    fpCreateDXGIFactory1 = dxgi_module == NULL ? NULL : (PFN_CreateDXGIFactory1)GetProcAddress(dxgi_module, "CreateDXGIFactory1");

#if !defined(NDEBUG)
    _set_error_mode(_OUT_TO_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            loader_initialize();
            break;
        case DLL_PROCESS_DETACH:
            if (NULL == reserved) {
                loader_release();
            }
            break;
        default:
            // Do nothing
            break;
    }
    return TRUE;
}

bool windows_add_json_entry(const struct loader_instance *inst,
                            char **reg_data,    // list of JSON files
                            PDWORD total_size,  // size of reg_data
                            LPCSTR key_name,    // key name - used for debug prints - i.e. VulkanDriverName
                            DWORD key_type,     // key data type
                            LPSTR json_path,    // JSON string to add to the list reg_data
                            DWORD json_size,    // size in bytes of json_path
                            VkResult *result) {
    // Check for and ignore duplicates.
    if (*reg_data && strstr(*reg_data, json_path)) {
        // Success. The json_path is already in the list.
        return true;
    }

    if (NULL == *reg_data) {
        *reg_data = loader_instance_heap_alloc(inst, *total_size, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (NULL == *reg_data) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "windows_add_json_entry: Failed to allocate space for registry data for key %s", json_path);
            *result = VK_ERROR_OUT_OF_HOST_MEMORY;
            return false;
        }
        *reg_data[0] = '\0';
    } else if (strlen(*reg_data) + json_size + 1 > *total_size) {
        void *new_ptr =
            loader_instance_heap_realloc(inst, *reg_data, *total_size, *total_size * 2, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (NULL == new_ptr) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "windows_add_json_entry: Failed to reallocate space for registry value of size %d for key %s",
                       *total_size * 2, json_path);
            *result = VK_ERROR_OUT_OF_HOST_MEMORY;
            return false;
        }
        *reg_data = new_ptr;
        *total_size *= 2;
    }

    for (char *curr_filename = json_path; curr_filename[0] != '\0'; curr_filename += strlen(curr_filename) + 1) {
        if (strlen(*reg_data) == 0) {
            (void)snprintf(*reg_data, json_size + 1, "%s", curr_filename);
        } else {
            (void)snprintf(*reg_data + strlen(*reg_data), json_size + 2, "%c%s", PATH_SEPARATOR, curr_filename);
        }
        loader_log(inst, VULKAN_LOADER_INFO_BIT, 0, "%s: Located json file \"%s\" from PnP registry: %s", __FUNCTION__,
                   curr_filename, key_name);

        if (key_type == REG_SZ) {
            break;
        }
    }
    return true;
}

bool windows_get_device_registry_entry(const struct loader_instance *inst, char **reg_data, PDWORD total_size, DEVINST dev_id,
                                       LPCSTR value_name, VkResult *result) {
    HKEY hkrKey = INVALID_HANDLE_VALUE;
    DWORD requiredSize, data_type;
    char *manifest_path = NULL;
    bool found = false;

    assert(reg_data != NULL && "windows_get_device_registry_entry: reg_data is a NULL pointer");
    assert(total_size != NULL && "windows_get_device_registry_entry: total_size is a NULL pointer");

    CONFIGRET status = CM_Open_DevNode_Key(dev_id, KEY_QUERY_VALUE, 0, RegDisposition_OpenExisting, &hkrKey, CM_REGISTRY_SOFTWARE);
    if (status != CR_SUCCESS) {
        loader_log(inst, VULKAN_LOADER_WARN_BIT, 0,
                   "windows_get_device_registry_entry: Failed to open registry key for DeviceID(%d)", dev_id);
        *result = VK_ERROR_INCOMPATIBLE_DRIVER;
        return false;
    }

    // query value
    LSTATUS ret = RegQueryValueEx(hkrKey, value_name, NULL, NULL, NULL, &requiredSize);

    if (ret != ERROR_SUCCESS) {
        if (ret == ERROR_FILE_NOT_FOUND) {
            loader_log(inst, VULKAN_LOADER_INFO_BIT, 0,
                       "windows_get_device_registry_entry: Device ID(%d) Does not contain a value for \"%s\"", dev_id, value_name);
        } else {
            loader_log(inst, VULKAN_LOADER_INFO_BIT, 0, "windows_get_device_registry_entry: DeviceID(%d) Failed to obtain %s size",
                       dev_id, value_name);
        }
        goto out;
    }

    manifest_path = loader_instance_heap_alloc(inst, requiredSize, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    if (manifest_path == NULL) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "windows_get_device_registry_entry: Failed to allocate space for DriverName.");
        *result = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    ret = RegQueryValueEx(hkrKey, value_name, NULL, &data_type, (BYTE *)manifest_path, &requiredSize);

    if (ret != ERROR_SUCCESS) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "windows_get_device_registry_entry: DeviceID(%d) Failed to obtain %s",
                   value_name);
        *result = VK_ERROR_INCOMPATIBLE_DRIVER;
        goto out;
    }

    if (data_type != REG_SZ && data_type != REG_MULTI_SZ) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "windows_get_device_registry_entry: Invalid %s data type. Expected REG_SZ or REG_MULTI_SZ.", value_name);
        *result = VK_ERROR_INCOMPATIBLE_DRIVER;
        goto out;
    }

    found = windows_add_json_entry(inst, reg_data, total_size, value_name, data_type, manifest_path, requiredSize, result);

out:
    if (manifest_path != NULL) {
        loader_instance_heap_free(inst, manifest_path);
    }
    RegCloseKey(hkrKey);
    return found;
}

VkResult windows_get_device_registry_files(const struct loader_instance *inst, char **reg_data, PDWORD reg_data_size,
                                           LPCSTR value_name) {
    static const wchar_t *softwareComponentGUID = L"{5c4c3332-344d-483c-8739-259e934c9cc8}";
    static const wchar_t *displayGUID = L"{4d36e968-e325-11ce-bfc1-08002be10318}";
#ifdef CM_GETIDLIST_FILTER_PRESENT
    const ULONG flags = CM_GETIDLIST_FILTER_CLASS | CM_GETIDLIST_FILTER_PRESENT;
#else
    const ULONG flags = 0x300;
#endif

    wchar_t childGuid[MAX_GUID_STRING_LEN + 2];  // +2 for brackets {}
    ULONG childGuidSize = sizeof(childGuid);

    DEVINST devID = 0, childID = 0;
    wchar_t *pDeviceNames = NULL;
    ULONG deviceNamesSize = 0;
    VkResult result = VK_SUCCESS;
    bool found = false;

    assert(reg_data != NULL && "windows_get_device_registry_files: reg_data is NULL");

    // if after obtaining the DeviceNameSize, new device is added start over
    do {
        CM_Get_Device_ID_List_SizeW(&deviceNamesSize, displayGUID, flags);

        if (pDeviceNames != NULL) {
            loader_instance_heap_free(inst, pDeviceNames);
        }

        pDeviceNames = loader_instance_heap_alloc(inst, deviceNamesSize * sizeof(wchar_t), VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (pDeviceNames == NULL) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "windows_get_device_registry_files: Failed to allocate space for display device names.");
            result = VK_ERROR_OUT_OF_HOST_MEMORY;
            return result;
        }
    } while (CM_Get_Device_ID_ListW(displayGUID, pDeviceNames, deviceNamesSize, flags) == CR_BUFFER_SMALL);

    if (pDeviceNames) {
        for (wchar_t *deviceName = pDeviceNames; *deviceName; deviceName += wcslen(deviceName) + 1) {
            CONFIGRET status = CM_Locate_DevNodeW(&devID, deviceName, CM_LOCATE_DEVNODE_NORMAL);
            if (CR_SUCCESS != status) {
                loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "windows_get_device_registry_files: failed to open DevNode %ls",
                           deviceName);
                continue;
            }
            ULONG ulStatus, ulProblem;
            status = CM_Get_DevNode_Status(&ulStatus, &ulProblem, devID, 0);

            if (CR_SUCCESS != status) {
                loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "windows_get_device_registry_files: failed to probe device status %ls",
                           deviceName);
                continue;
            }
            if ((ulStatus & DN_HAS_PROBLEM) && (ulProblem == CM_PROB_NEED_RESTART || ulProblem == DN_NEED_RESTART)) {
                loader_log(inst, VULKAN_LOADER_INFO_BIT, 0,
                           "windows_get_device_registry_files: device %ls is pending reboot, skipping ...", deviceName);
                continue;
            }

            loader_log(inst, VULKAN_LOADER_INFO_BIT, 0, "windows_get_device_registry_files: opening device %ls", deviceName);

            if (windows_get_device_registry_entry(inst, reg_data, reg_data_size, devID, value_name, &result)) {
                found = true;
                continue;
            } else if (result == VK_ERROR_OUT_OF_HOST_MEMORY) {
                break;
            }

            status = CM_Get_Child(&childID, devID, 0);
            if (status != CR_SUCCESS) {
                loader_log(inst, VULKAN_LOADER_INFO_BIT, 0,
                           "windows_get_device_registry_files: unable to open child-device error:%d", status);
                continue;
            }

            do {
                wchar_t buffer[MAX_DEVICE_ID_LEN];
                CM_Get_Device_IDW(childID, buffer, MAX_DEVICE_ID_LEN, 0);

                loader_log(inst, VULKAN_LOADER_INFO_BIT, 0, "windows_get_device_registry_files: Opening child device %d - %ls",
                           childID, buffer);

                status = CM_Get_DevNode_Registry_PropertyW(childID, CM_DRP_CLASSGUID, NULL, &childGuid, &childGuidSize, 0);
                if (status != CR_SUCCESS) {
                    loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                               "windows_get_device_registry_files: unable to obtain GUID for:%d error:%d", childID, status);

                    result = VK_ERROR_INCOMPATIBLE_DRIVER;
                    continue;
                }

                if (wcscmp(childGuid, softwareComponentGUID) != 0) {
                    loader_log(inst, VULKAN_LOADER_DEBUG_BIT, 0,
                               "windows_get_device_registry_files: GUID for %d is not SoftwareComponent skipping", childID);
                    continue;
                }

                if (windows_get_device_registry_entry(inst, reg_data, reg_data_size, childID, value_name, &result)) {
                    found = true;
                    break;  // check next-display-device
                }

            } while (CM_Get_Sibling(&childID, childID, 0) == CR_SUCCESS);
        }

        loader_instance_heap_free(inst, pDeviceNames);
    }

    if (!found && result != VK_ERROR_OUT_OF_HOST_MEMORY) {
        result = VK_ERROR_INCOMPATIBLE_DRIVER;
    }

    return result;
}

VkResult windows_get_registry_files(const struct loader_instance *inst, char *location, bool use_secondary_hive, char **reg_data,
                                    PDWORD reg_data_size) {
    // This list contains all of the allowed ICDs. This allows us to verify that a device is actually present from the vendor
    // specified. This does disallow other vendors, but any new driver should use the device-specific registries anyway.
    static const struct {
        const char *filename;
        int vendor_id;
    } known_drivers[] = {
#if defined(_WIN64)
        {
            .filename = "igvk64.json",
            .vendor_id = 0x8086,
        },
        {
            .filename = "nv-vk64.json",
            .vendor_id = 0x10de,
        },
        {
            .filename = "amd-vulkan64.json",
            .vendor_id = 0x1002,
        },
        {
            .filename = "amdvlk64.json",
            .vendor_id = 0x1002,
        },
#else
        {
            .filename = "igvk32.json",
            .vendor_id = 0x8086,
        },
        {
            .filename = "nv-vk32.json",
            .vendor_id = 0x10de,
        },
        {
            .filename = "amd-vulkan32.json",
            .vendor_id = 0x1002,
        },
        {
            .filename = "amdvlk32.json",
            .vendor_id = 0x1002,
        },
#endif
    };

    LONG rtn_value;
    HKEY hive = DEFAULT_VK_REGISTRY_HIVE, key;
    DWORD access_flags;
    char name[2048];
    char *loc = location;
    char *next;
    DWORD name_size = sizeof(name);
    DWORD value;
    DWORD value_size = sizeof(value);
    VkResult result = VK_SUCCESS;
    bool found = false;
    IDXGIFactory1 *dxgi_factory = NULL;
    bool is_driver = !strcmp(location, VK_DRIVERS_INFO_REGISTRY_LOC);

    assert(reg_data != NULL && "windows_get_registry_files: reg_data is a NULL pointer");

    if (is_driver) {
        HRESULT hres = fpCreateDXGIFactory1(&IID_IDXGIFactory1, (void **)&dxgi_factory);
        if (hres != S_OK) {
            loader_log(inst, VULKAN_LOADER_WARN_BIT, 0,
                       "windows_get_registry_files: Failed to create dxgi factory for ICD registry verification. No ICDs will be "
                       "added from "
                       "legacy registry locations");
            goto out;
        }
    }

    while (*loc) {
        next = loader_get_next_path(loc);
        access_flags = KEY_QUERY_VALUE;
        rtn_value = RegOpenKeyEx(hive, loc, 0, access_flags, &key);
        if (ERROR_SUCCESS == rtn_value) {
            for (DWORD idx = 0;
                 (rtn_value = RegEnumValue(key, idx++, name, &name_size, NULL, NULL, (LPBYTE)&value, &value_size)) == ERROR_SUCCESS;
                 name_size = sizeof(name), value_size = sizeof(value)) {
                if (value_size == sizeof(value) && value == 0) {
                    if (NULL == *reg_data) {
                        *reg_data = loader_instance_heap_alloc(inst, *reg_data_size, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
                        if (NULL == *reg_data) {
                            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                                       "windows_get_registry_files: Failed to allocate space for registry data for key %s", name);
                            RegCloseKey(key);
                            result = VK_ERROR_OUT_OF_HOST_MEMORY;
                            goto out;
                        }
                        *reg_data[0] = '\0';
                    } else if (strlen(*reg_data) + name_size + 1 > *reg_data_size) {
                        void *new_ptr = loader_instance_heap_realloc(inst, *reg_data, *reg_data_size, *reg_data_size * 2,
                                                                     VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
                        if (NULL == new_ptr) {
                            loader_log(
                                inst, VULKAN_LOADER_ERROR_BIT, 0,
                                "windows_get_registry_files: Failed to reallocate space for registry value of size %d for key %s",
                                *reg_data_size * 2, name);
                            RegCloseKey(key);
                            result = VK_ERROR_OUT_OF_HOST_MEMORY;
                            goto out;
                        }
                        *reg_data = new_ptr;
                        *reg_data_size *= 2;
                    }

                    // We've now found a json file. If this is an ICD, we still need to check if there is actually a device
                    // that matches this ICD
                    loader_log(inst, VULKAN_LOADER_INFO_BIT, 0, "Located json file \"%s\" from registry \"%s\\%s\"", name,
                               hive == DEFAULT_VK_REGISTRY_HIVE ? DEFAULT_VK_REGISTRY_HIVE_STR : SECONDARY_VK_REGISTRY_HIVE_STR,
                               location);
                    if (is_driver) {
                        int i;
                        for (i = 0; i < sizeof(known_drivers) / sizeof(known_drivers[0]); ++i) {
                            if (!strcmp(name + strlen(name) - strlen(known_drivers[i].filename), known_drivers[i].filename)) {
                                break;
                            }
                        }
                        if (i == sizeof(known_drivers) / sizeof(known_drivers[0])) {
                            loader_log(inst, VULKAN_LOADER_INFO_BIT, 0,
                                       "Driver %s is not recognized as a known driver. It will be assumed to be active", name);
                        } else {
                            bool found_gpu = false;
                            for (int j = 0;; ++j) {
                                IDXGIAdapter1 *adapter;
                                HRESULT hres = dxgi_factory->lpVtbl->EnumAdapters1(dxgi_factory, j, &adapter);
                                if (hres == DXGI_ERROR_NOT_FOUND) {
                                    break;
                                } else if (hres != S_OK) {
                                    loader_log(inst, VULKAN_LOADER_WARN_BIT, 0,
                                               "Failed to enumerate DXGI adapters at index %d. As a result, drivers may be skipped",
                                               j);
                                    continue;
                                }

                                DXGI_ADAPTER_DESC1 description;
                                hres = adapter->lpVtbl->GetDesc1(adapter, &description);
                                if (hres != S_OK) {
                                    loader_log(
                                        inst, VULKAN_LOADER_INFO_BIT, 0,
                                        "Failed to get DXGI adapter information at index %d. As a result, drivers may be skipped",
                                        j);
                                    continue;
                                }

                                if (description.VendorId == known_drivers[i].vendor_id) {
                                    found_gpu = true;
                                    break;
                                }
                            }

                            if (!found_gpu) {
                                loader_log(inst, VULKAN_LOADER_INFO_BIT, 0,
                                           "Dropping driver %s as no corresponding DXGI adapter was found", name);
                                continue;
                            }
                        }
                    }

                    if (strlen(*reg_data) == 0) {
                        // The list is emtpy. Add the first entry.
                        (void)snprintf(*reg_data, name_size + 1, "%s", name);
                        found = true;
                    } else {
                        // At this point the reg_data variable contains other JSON paths, likely from the PNP/device section
                        // of the registry that we want to have precedence over this non-device specific section of the registry.
                        // To make sure we avoid enumerating old JSON files/drivers that might be present in the non-device specific
                        // area of the registry when a newer device specific JSON file is present, do a check before adding.
                        // Find the file name, without path, of the JSON file found in the non-device specific registry location.
                        // If the same JSON file name is already found in the list, don't add it again.
                        bool foundDuplicate = false;
                        char *pLastSlashName = strrchr(name, '\\');
                        if (pLastSlashName != NULL) {
                            char *foundMatch = strstr(*reg_data, pLastSlashName + 1);
                            if (foundMatch != NULL) {
                                foundDuplicate = true;
                            }
                        }

                        if (foundDuplicate == false) {
                            // Add the new entry to the list.
                            (void)snprintf(*reg_data + strlen(*reg_data), name_size + 2, "%c%s", PATH_SEPARATOR, name);
                            found = true;
                        } else {
                            loader_log(
                                inst, VULKAN_LOADER_INFO_BIT, 0,
                                "Skipping adding of json file \"%s\" from registry \"%s\\%s\" to the list due to duplication", name,
                                hive == DEFAULT_VK_REGISTRY_HIVE ? DEFAULT_VK_REGISTRY_HIVE_STR : SECONDARY_VK_REGISTRY_HIVE_STR,
                                location);
                        }
                    }
                }
            }
            RegCloseKey(key);
        }

        // Advance the location - if the next location is in the secondary hive, then reset the locations and advance the hive
        if (use_secondary_hive && (hive == DEFAULT_VK_REGISTRY_HIVE) && (*next == '\0')) {
            loc = location;
            hive = SECONDARY_VK_REGISTRY_HIVE;
        } else {
            loc = next;
        }
    }

    if (!found && result != VK_ERROR_OUT_OF_HOST_MEMORY) {
        result = VK_ERROR_INCOMPATIBLE_DRIVER;
    }

out:
    if (is_driver && dxgi_factory != NULL) {
        dxgi_factory->lpVtbl->Release(dxgi_factory);
    }

    return result;
}

// Read manifest JSON files using the Windows driver interface
VkResult windows_read_manifest_from_d3d_adapters(const struct loader_instance *inst, char **reg_data, PDWORD reg_data_size,
                                                 const wchar_t *value_name) {
    VkResult result = VK_INCOMPLETE;
    LoaderEnumAdapters2 adapters = {.adapter_count = 0, .adapters = NULL};
    LoaderQueryRegistryInfo *full_info = NULL;
    size_t full_info_size = 0;
    char *json_path = NULL;
    size_t json_path_size = 0;

    PFN_LoaderEnumAdapters2 fpLoaderEnumAdapters2 =
        (PFN_LoaderEnumAdapters2)GetProcAddress(GetModuleHandle("gdi32.dll"), "D3DKMTEnumAdapters2");
    PFN_LoaderQueryAdapterInfo fpLoaderQueryAdapterInfo =
        (PFN_LoaderQueryAdapterInfo)GetProcAddress(GetModuleHandle("gdi32.dll"), "D3DKMTQueryAdapterInfo");
    if (fpLoaderEnumAdapters2 == NULL || fpLoaderQueryAdapterInfo == NULL) {
        result = VK_ERROR_INCOMPATIBLE_DRIVER;
        goto out;
    }

    // Get all of the adapters
    NTSTATUS status = fpLoaderEnumAdapters2(&adapters);
    if (status == STATUS_SUCCESS && adapters.adapter_count > 0) {
        adapters.adapters = loader_instance_heap_alloc(inst, sizeof(*adapters.adapters) * adapters.adapter_count,
                                                       VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (adapters.adapters == NULL) {
            goto out;
        }
        status = fpLoaderEnumAdapters2(&adapters);
    }
    if (status != STATUS_SUCCESS) {
        goto out;
    }

    // If that worked, we need to get the manifest file(s) for each adapter
    for (ULONG i = 0; i < adapters.adapter_count; ++i) {
        // The first query should just check if the field exists and how big it is
        LoaderQueryRegistryInfo filename_info = {
            .query_type = LOADER_QUERY_REGISTRY_ADAPTER_KEY,
            .query_flags =
                {
                    .translate_path = true,
                },
            .value_type = REG_MULTI_SZ,
            .physical_adapter_index = 0,
        };
        wcsncpy(filename_info.value_name, value_name, sizeof(filename_info.value_name) / sizeof(WCHAR));
        LoaderQueryAdapterInfo query_info = {
            .handle = adapters.adapters[i].handle,
            .type = LOADER_QUERY_TYPE_REGISTRY,
            .private_data = &filename_info,
            .private_data_size = sizeof(filename_info),
        };
        status = fpLoaderQueryAdapterInfo(&query_info);

        // This error indicates that the type didn't match, so we'll try a REG_SZ
        if (status != STATUS_SUCCESS) {
            filename_info.value_type = REG_SZ;
            status = fpLoaderQueryAdapterInfo(&query_info);
        }

        if (status != STATUS_SUCCESS || filename_info.status != LOADER_QUERY_REGISTRY_STATUS_BUFFER_OVERFLOW) {
            continue;
        }

        while (status == STATUS_SUCCESS &&
               ((LoaderQueryRegistryInfo *)query_info.private_data)->status == LOADER_QUERY_REGISTRY_STATUS_BUFFER_OVERFLOW) {
            bool needs_copy = (full_info == NULL);
            size_t full_size = sizeof(LoaderQueryRegistryInfo) + filename_info.output_value_size;
            void *buffer =
                loader_instance_heap_realloc(inst, full_info, full_info_size, full_size, VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
            if (buffer == NULL) {
                result = VK_ERROR_OUT_OF_HOST_MEMORY;
                goto out;
            }
            full_info = buffer;
            full_info_size = full_size;

            if (needs_copy) {
                memcpy(full_info, &filename_info, sizeof(LoaderQueryRegistryInfo));
            }
            query_info.private_data = full_info;
            query_info.private_data_size = (UINT)full_info_size;
            status = fpLoaderQueryAdapterInfo(&query_info);
        }

        if (status != STATUS_SUCCESS || full_info->status != LOADER_QUERY_REGISTRY_STATUS_SUCCESS) {
            goto out;
        }

        // Convert the wide string to a narrow string
        void *buffer = loader_instance_heap_realloc(inst, json_path, json_path_size, full_info->output_value_size,
                                                    VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (buffer == NULL) {
            result = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }
        json_path = buffer;
        json_path_size = full_info->output_value_size;

        // Iterate over each component string
        for (const wchar_t *curr_path = full_info->output_string; curr_path[0] != '\0'; curr_path += wcslen(curr_path) + 1) {
            WideCharToMultiByte(CP_UTF8, 0, curr_path, -1, json_path, (int)json_path_size, NULL, NULL);

            // Add the string to the output list
            result = VK_SUCCESS;
            windows_add_json_entry(inst, reg_data, reg_data_size, (LPCTSTR)L"EnumAdapters", REG_SZ, json_path,
                                   (DWORD)strlen(json_path) + 1, &result);
            if (result != VK_SUCCESS) {
                goto out;
            }

            // If this is a string and not a multi-string, we don't want to go throught the loop more than once
            if (full_info->value_type == REG_SZ) {
                break;
            }
        }
    }

out:
    if (json_path != NULL) {
        loader_instance_heap_free(inst, json_path);
    }
    if (full_info != NULL) {
        loader_instance_heap_free(inst, full_info);
    }
    if (adapters.adapters != NULL) {
        loader_instance_heap_free(inst, adapters.adapters);
    }

    return result;
}

// Look for data files in the registry.
VkResult windows_read_data_files_in_registry(const struct loader_instance *inst, enum loader_data_files_type data_file_type,
                                             bool warn_if_not_present, char *registry_location,
                                             struct loader_data_files *out_files) {
    VkResult vk_result = VK_SUCCESS;
    char *search_path = NULL;

    if (data_file_type == LOADER_DATA_FILE_MANIFEST_ICD) {
        loader_log(inst, VULKAN_LOADER_DRIVER_BIT, 0, "Checking for Driver Manifest files in Registry at %s",
                   registry_location);
    } else {
        loader_log(inst, VULKAN_LOADER_LAYER_BIT, 0, "Checking for Driver Manifest files in Registry at %s",
                   registry_location);
    }

    // These calls look at the PNP/Device section of the registry.
    VkResult regHKR_result = VK_SUCCESS;
    DWORD reg_size = 4096;
    if (!strncmp(registry_location, VK_DRIVERS_INFO_REGISTRY_LOC, sizeof(VK_DRIVERS_INFO_REGISTRY_LOC))) {
        // If we're looking for drivers we need to try enumerating adapters
        regHKR_result = windows_read_manifest_from_d3d_adapters(inst, &search_path, &reg_size, LoaderPnpDriverRegistryWide());
        if (regHKR_result == VK_INCOMPLETE) {
            regHKR_result = windows_get_device_registry_files(inst, &search_path, &reg_size, LoaderPnpDriverRegistry());
        }
    } else if (!strncmp(registry_location, VK_ELAYERS_INFO_REGISTRY_LOC, sizeof(VK_ELAYERS_INFO_REGISTRY_LOC))) {
        regHKR_result = windows_read_manifest_from_d3d_adapters(inst, &search_path, &reg_size, LoaderPnpELayerRegistryWide());
        if (regHKR_result == VK_INCOMPLETE) {
            regHKR_result = windows_get_device_registry_files(inst, &search_path, &reg_size, LoaderPnpELayerRegistry());
        }
    } else if (!strncmp(registry_location, VK_ILAYERS_INFO_REGISTRY_LOC, sizeof(VK_ILAYERS_INFO_REGISTRY_LOC))) {
        regHKR_result = windows_read_manifest_from_d3d_adapters(inst, &search_path, &reg_size, LoaderPnpILayerRegistryWide());
        if (regHKR_result == VK_INCOMPLETE) {
            regHKR_result = windows_get_device_registry_files(inst, &search_path, &reg_size, LoaderPnpILayerRegistry());
        }
    }

    if (regHKR_result == VK_ERROR_OUT_OF_HOST_MEMORY) {
        vk_result = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    // This call looks into the Khronos non-device specific section of the registry.
    bool use_secondary_hive = (data_file_type == LOADER_DATA_FILE_MANIFEST_LAYER) && (!is_high_integrity());
    VkResult reg_result = windows_get_registry_files(inst, registry_location, use_secondary_hive, &search_path, &reg_size);
    if (reg_result == VK_ERROR_OUT_OF_HOST_MEMORY) {
        vk_result = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }

    if ((VK_SUCCESS != reg_result && VK_SUCCESS != regHKR_result) || NULL == search_path) {
        if (data_file_type == LOADER_DATA_FILE_MANIFEST_ICD) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                       "windows_read_data_files_in_registry: Registry lookup failed to get ICD manifest files.  Possibly missing "
                       "Vulkan driver?");
            vk_result = VK_ERROR_INCOMPATIBLE_DRIVER;
        } else {
            if (warn_if_not_present) {
                if (data_file_type == LOADER_DATA_FILE_MANIFEST_LAYER) {
                    // This is only a warning for layers
                    loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                               "windows_read_data_files_in_registry: Registry lookup failed to get layer manifest files.");
                } else {
                    // This is only a warning for general data files
                    loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                               "windows_read_data_files_in_registry: Registry lookup failed to get data files.");
                }
            }
            // Return success for now since it's not critical for layers
            vk_result = VK_SUCCESS;
        }
        goto out;
    }

    // Now, parse the paths and add any manifest files found in them.
    vk_result = add_data_files_in_path(inst, search_path, false, out_files, false);

out:

    if (NULL != search_path) {
        loader_instance_heap_free(inst, search_path);
    }

    return vk_result;
}

// This function allocates an array in sorted_devices which must be freed by the caller if not null
VkResult windows_read_sorted_physical_devices(struct loader_instance *inst, struct LoaderSortedPhysicalDevice **sorted_devices,
                                              uint32_t *sorted_count) {
    VkResult res = VK_SUCCESS;

    uint32_t sorted_alloc = 0;
    struct loader_icd_term *icd_term = NULL;
    IDXGIFactory6 *dxgi_factory = NULL;
    HRESULT hres = fpCreateDXGIFactory1(&IID_IDXGIFactory6, (void **)&dxgi_factory);
    if (hres != S_OK) {
        loader_log(inst, VULKAN_LOADER_INFO_BIT, 0, "Failed to create DXGI factory 6. Physical devices will not be sorted");
    } else {
        sorted_alloc = 16;
        *sorted_devices = loader_instance_heap_alloc(inst, sorted_alloc * sizeof(struct LoaderSortedPhysicalDevice),
                                                     VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (*sorted_devices == NULL) {
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }

        memset(*sorted_devices, 0, sorted_alloc * sizeof(struct LoaderSortedPhysicalDevice));

        *sorted_count = 0;
        for (uint32_t i = 0;; ++i) {
            IDXGIAdapter1 *adapter;
            hres = dxgi_factory->lpVtbl->EnumAdapterByGpuPreference(dxgi_factory, i, DXGI_GPU_PREFERENCE_UNSPECIFIED,
                                                                    &IID_IDXGIAdapter1, (void **)&adapter);
            if (hres == DXGI_ERROR_NOT_FOUND) {
                break;  // No more adapters
            } else if (hres != S_OK) {
                loader_log(inst, VULKAN_LOADER_WARN_BIT, 0,
                           "Failed to enumerate adapters by GPU preference at index %u. This adapter will not be sorted", i);
                break;
            }

            DXGI_ADAPTER_DESC1 description;
            hres = adapter->lpVtbl->GetDesc1(adapter, &description);
            if (hres != S_OK) {
                loader_log(inst, VULKAN_LOADER_WARN_BIT, 0, "Failed to get adapter LUID index %u. This adapter will not be sorted",
                           i);
                continue;
            }

            if (sorted_alloc <= i) {
                uint32_t old_size = sorted_alloc * sizeof(struct LoaderSortedPhysicalDevice);
                *sorted_devices =
                    loader_instance_heap_realloc(inst, *sorted_devices, old_size, 2 * old_size, VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
                if (*sorted_devices == NULL) {
                    adapter->lpVtbl->Release(adapter);
                    res = VK_ERROR_OUT_OF_HOST_MEMORY;
                    goto out;
                }
                sorted_alloc *= 2;
            }
            struct LoaderSortedPhysicalDevice *sorted_array = *sorted_devices;
            sorted_array[*sorted_count].device_count = 0;
            sorted_array[*sorted_count].physical_devices = NULL;
            //*sorted_count = i;

            icd_term = inst->icd_terms;
            for (uint32_t icd_idx = 0; NULL != icd_term; icd_term = icd_term->next, icd_idx++) {
                // This is the new behavior, which cannot be run unless the ICD provides EnumerateAdapterPhysicalDevices
                if (icd_term->scanned_icd->EnumerateAdapterPhysicalDevices == NULL) {
                    continue;
                }

                uint32_t count;
                VkResult vkres = icd_term->scanned_icd->EnumerateAdapterPhysicalDevices(icd_term->instance, description.AdapterLuid,
                                                                                        &count, NULL);
                if (vkres == VK_ERROR_INCOMPATIBLE_DRIVER) {
                    continue;  // This driver doesn't support the adapter
                } else if (vkres == VK_ERROR_OUT_OF_HOST_MEMORY) {
                    res = VK_ERROR_OUT_OF_HOST_MEMORY;
                    goto out;
                } else if (vkres != VK_SUCCESS) {
                    loader_log(inst, VULKAN_LOADER_WARN_BIT, 0,
                               "Failed to convert DXGI adapter into Vulkan physical device with unexpected error code");
                    continue;
                }

                // Get the actual physical devices
                if (0 != count) {
                    do {
                        sorted_array[*sorted_count].physical_devices =
                            loader_instance_heap_realloc(inst, sorted_array[*sorted_count].physical_devices,
                                                         sorted_array[*sorted_count].device_count * sizeof(VkPhysicalDevice),
                                                         count * sizeof(VkPhysicalDevice), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
                        if (sorted_array[*sorted_count].physical_devices == NULL) {
                            res = VK_ERROR_OUT_OF_HOST_MEMORY;
                            break;
                        }
                        sorted_array[*sorted_count].device_count = count;
                    } while ((vkres = icd_term->scanned_icd->EnumerateAdapterPhysicalDevices(
                                  icd_term->instance, description.AdapterLuid, &count,
                                  sorted_array[*sorted_count].physical_devices)) == VK_INCOMPLETE);
                }

                if (vkres != VK_SUCCESS) {
                    loader_instance_heap_free(inst, sorted_array[*sorted_count].physical_devices);
                    sorted_array[*sorted_count].physical_devices = NULL;
                    if (vkres == VK_ERROR_OUT_OF_HOST_MEMORY) {
                        res = VK_ERROR_OUT_OF_HOST_MEMORY;
                        goto out;
                    } else {
                        loader_log(inst, VULKAN_LOADER_WARN_BIT, 0, "Failed to convert DXGI adapter into Vulkan physical device");
                        continue;
                    }
                }
                inst->total_gpu_count += (sorted_array[*sorted_count].device_count = count);
                sorted_array[*sorted_count].icd_index = icd_idx;
                sorted_array[*sorted_count].icd_term = icd_term;
                ++(*sorted_count);
            }

            adapter->lpVtbl->Release(adapter);
        }

        dxgi_factory->lpVtbl->Release(dxgi_factory);
    }

out:

    if (*sorted_count == 0 && *sorted_devices != NULL) {
        loader_instance_heap_free(inst, *sorted_devices);
        *sorted_devices = NULL;
    }
    return res;
}

VkLoaderFeatureFlags windows_initialize_dxgi(void) {
    VkLoaderFeatureFlags feature_flags = 0;
    IDXGIFactory6 *dxgi_factory = NULL;
    HRESULT hres = fpCreateDXGIFactory1(&IID_IDXGIFactory6, (void **)&dxgi_factory);
    if (hres == S_OK) {
        feature_flags |= VK_LOADER_FEATURE_PHYSICAL_DEVICE_SORTING;
        dxgi_factory->lpVtbl->Release(dxgi_factory);
    }
    return feature_flags;
}

#endif  // _WIN32