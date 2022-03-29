/*
 * Copyright (c) 2021 The Khronos Group Inc.
 * Copyright (c) 2021 Valve Corporation
 * Copyright (c) 2021 LunarG, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and/or associated documentation files (the "Materials"), to
 * deal in the Materials without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Materials, and to permit persons to whom the Materials are
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice(s) and this permission notice shall be included in
 * all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR THE
 * USE OR OTHER DEALINGS IN THE MATERIALS.
 *
 * Author: Charles Giessen <charles@lunarg.com>
 */

// This needs to be defined first, or else we'll get redefinitions on NTSTATUS values
#ifdef _WIN32
#define UMDF_USING_NTSTATUS
#include <ntstatus.h>
#endif

#include "shim.h"

#include "detours.h"

static PlatformShim platform_shim;

extern "C" {

static LibraryWrapper gdi32_dll;

using PFN_GetSidSubAuthority = PDWORD(__stdcall *)(PSID pSid, DWORD nSubAuthority);
static PFN_GetSidSubAuthority fpGetSidSubAuthority = GetSidSubAuthority;

PDWORD __stdcall ShimGetSidSubAuthority(PSID pSid, DWORD nSubAuthority) { return &platform_shim.elevation_level; }

static PFN_LoaderEnumAdapters2 fpEnumAdapters2 = nullptr;
static PFN_LoaderQueryAdapterInfo fpQueryAdapterInfo = nullptr;

NTSTATUS APIENTRY ShimEnumAdapters2(LoaderEnumAdapters2 *adapters) {
    if (adapters == nullptr) {
        return STATUS_INVALID_PARAMETER;
    }
    if (platform_shim.d3dkmt_adapters.size() == 0) {
        if (adapters->adapters != nullptr) adapters->adapter_count = 0;
        return STATUS_SUCCESS;
    }
    if (adapters->adapters != nullptr) {
        for (size_t i = 0; i < platform_shim.d3dkmt_adapters.size(); i++) {
            adapters->adapters[i].handle = platform_shim.d3dkmt_adapters[i].hAdapter;
            adapters->adapters[i].luid = platform_shim.d3dkmt_adapters[i].adapter_luid;
        }
        adapters->adapter_count = static_cast<ULONG>(platform_shim.d3dkmt_adapters.size());
    } else {
        adapters->adapter_count = static_cast<ULONG>(platform_shim.d3dkmt_adapters.size());
    }
    return STATUS_SUCCESS;
}
NTSTATUS APIENTRY ShimQueryAdapterInfo(const LoaderQueryAdapterInfo *query_info) {
    if (query_info == nullptr || query_info->private_data == nullptr) {
        return STATUS_INVALID_PARAMETER;
    }
    auto handle = query_info->handle;
    auto it = std::find_if(platform_shim.d3dkmt_adapters.begin(), platform_shim.d3dkmt_adapters.end(),
                           [handle](D3DKMT_Adapter const &adapter) { return handle == adapter.hAdapter; });
    if (it == platform_shim.d3dkmt_adapters.end()) {
        return STATUS_INVALID_PARAMETER;
    }
    auto &adapter = *it;
    auto *reg_info = reinterpret_cast<LoaderQueryRegistryInfo *>(query_info->private_data);

    std::vector<std::wstring> *paths = nullptr;
    if (reg_info->value_name[6] == L'D') {  // looking for drivers
        paths = &adapter.driver_paths;
    } else if (reg_info->value_name[6] == L'I') {  // looking for implicit layers
        paths = &adapter.implicit_layer_paths;
    } else if (reg_info->value_name[6] == L'E') {  // looking for explicit layers
        paths = &adapter.explicit_layer_paths;
    }

    reg_info->status = LOADER_QUERY_REGISTRY_STATUS_SUCCESS;
    if (reg_info->output_value_size == 0) {
        ULONG size = 2;  // final null terminator
        for (auto const &path : *paths) size = static_cast<ULONG>(path.length() * sizeof(wchar_t));
        // size in bytes, so multiply path size by two and add 2 for the null terminator
        reg_info->output_value_size = size;
        if (size != 2) {
            // only want to write data if there is path data to write
            reg_info->status = LOADER_QUERY_REGISTRY_STATUS_BUFFER_OVERFLOW;
        }
    } else if (reg_info->output_value_size > 2) {
        size_t index = 0;
        for (auto const &path : *paths) {
            for (auto w : path) {
                reg_info->output_string[index++] = w;
            }
            reg_info->output_string[index++] = L'\0';
        }
        // make sure there is a null terminator
        reg_info->output_string[index++] = L'\0';

        reg_info->status = LOADER_QUERY_REGISTRY_STATUS_SUCCESS;
    }

    return STATUS_SUCCESS;
}

// clang-format off
static CONFIGRET(WINAPI *REAL_CM_Get_Device_ID_List_SizeW)(PULONG pulLen, PCWSTR pszFilter, ULONG ulFlags) = CM_Get_Device_ID_List_SizeW;
static CONFIGRET(WINAPI *REAL_CM_Get_Device_ID_ListW)(PCWSTR pszFilter, PZZWSTR Buffer, ULONG BufferLen, ULONG ulFlags) = CM_Get_Device_ID_ListW;
static CONFIGRET(WINAPI *REAL_CM_Locate_DevNodeW)(PDEVINST pdnDevInst, DEVINSTID_W pDeviceID, ULONG ulFlags) =  CM_Locate_DevNodeW;
static CONFIGRET(WINAPI *REAL_CM_Get_DevNode_Status)(PULONG pulStatus, PULONG pulProblemNumber, DEVINST dnDevInst, ULONG ulFlags) =  CM_Get_DevNode_Status;
static CONFIGRET(WINAPI *REAL_CM_Get_Device_IDW)(DEVINST dnDevInst, PWSTR Buffer, ULONG BufferLen, ULONG ulFlags) =  CM_Get_Device_IDW;
static CONFIGRET(WINAPI *REAL_CM_Get_Child)(PDEVINST pdnDevInst, DEVINST dnDevInst, ULONG ulFlags) =  CM_Get_Child;
static CONFIGRET(WINAPI *REAL_CM_Get_DevNode_Registry_PropertyW)(DEVINST dnDevInst, ULONG ulProperty, PULONG pulRegDataType, PVOID Buffer, PULONG pulLength, ULONG ulFlags) =  CM_Get_DevNode_Registry_PropertyW;
static CONFIGRET(WINAPI *REAL_CM_Get_Sibling)(PDEVINST pdnDevInst, DEVINST dnDevInst, ULONG ulFlags) = CM_Get_Sibling;
// clang-format on

CONFIGRET WINAPI SHIM_CM_Get_Device_ID_List_SizeW(PULONG pulLen, PCWSTR pszFilter, ULONG ulFlags) {
    if (pulLen == nullptr) {
        return CR_INVALID_POINTER;
    }
    *pulLen = static_cast<ULONG>(platform_shim.CM_device_ID_list.size());
    return CR_SUCCESS;
}
CONFIGRET WINAPI SHIM_CM_Get_Device_ID_ListW(PCWSTR pszFilter, PZZWSTR Buffer, ULONG BufferLen, ULONG ulFlags) {
    if (Buffer != NULL) {
        if (BufferLen < platform_shim.CM_device_ID_list.size()) return CR_BUFFER_SMALL;
        for (size_t i = 0; i < BufferLen; i++) {
            Buffer[i] = platform_shim.CM_device_ID_list[i];
        }
    }
    return CR_SUCCESS;
}
// TODO
CONFIGRET WINAPI SHIM_CM_Locate_DevNodeW(PDEVINST pdnDevInst, DEVINSTID_W pDeviceID, ULONG ulFlags) { return CR_FAILURE; }
// TODO
CONFIGRET WINAPI SHIM_CM_Get_DevNode_Status(PULONG pulStatus, PULONG pulProblemNumber, DEVINST dnDevInst, ULONG ulFlags) {
    return CR_FAILURE;
}
// TODO
CONFIGRET WINAPI SHIM_CM_Get_Device_IDW(DEVINST dnDevInst, PWSTR Buffer, ULONG BufferLen, ULONG ulFlags) { return CR_FAILURE; }
// TODO
CONFIGRET WINAPI SHIM_CM_Get_Child(PDEVINST pdnDevInst, DEVINST dnDevInst, ULONG ulFlags) { return CR_FAILURE; }
// TODO
CONFIGRET WINAPI SHIM_CM_Get_DevNode_Registry_PropertyW(DEVINST dnDevInst, ULONG ulProperty, PULONG pulRegDataType, PVOID Buffer,
                                                        PULONG pulLength, ULONG ulFlags) {
    return CR_FAILURE;
}
// TODO
CONFIGRET WINAPI SHIM_CM_Get_Sibling(PDEVINST pdnDevInst, DEVINST dnDevInst, ULONG ulFlags) { return CR_FAILURE; }

static LibraryWrapper dxgi_module;
typedef HRESULT(APIENTRY *PFN_CreateDXGIFactory1)(REFIID riid, void **ppFactory);

PFN_CreateDXGIFactory1 RealCreateDXGIFactory1;

HRESULT __stdcall ShimGetDesc1(IDXGIAdapter1 *pAdapter,
                               /* [annotation][out] */
                               _Out_ DXGI_ADAPTER_DESC1 *pDesc) {
    if (pAdapter == nullptr || pDesc == nullptr) return DXGI_ERROR_INVALID_CALL;
    auto it = platform_shim.dxgi_adapter_map.find(pAdapter);
    if (it == platform_shim.dxgi_adapter_map.end()) {
        return DXGI_ERROR_INVALID_CALL;
    }
    *pDesc = platform_shim.dxgi_adapters[it->second].desc1;
    return S_OK;
}
ULONG __stdcall ShimIDXGIFactory1Release(IDXGIFactory1 *factory) {
    if (factory != nullptr) {
        if (factory->lpVtbl != nullptr) {
            delete factory->lpVtbl;
        }
        delete factory;
    }
    return S_OK;
}
ULONG __stdcall ShimIDXGIFactory6Release(IDXGIFactory6 *factory) {
    if (factory != nullptr) {
        if (factory->lpVtbl != nullptr) {
            delete factory->lpVtbl;
        }
        delete factory;
    }
    return S_OK;
}

ULONG __stdcall ShimRelease(IDXGIAdapter1 *pAdapter) {
    if (pAdapter != nullptr) {
        if (pAdapter->lpVtbl != nullptr) {
            delete pAdapter->lpVtbl;
        }
        delete pAdapter;
    }
    return S_OK;
}

IDXGIAdapter1 *create_IDXGIAdapter1() {
    IDXGIAdapter1Vtbl *vtbl = new IDXGIAdapter1Vtbl();
    vtbl->GetDesc1 = ShimGetDesc1;
    vtbl->Release = ShimRelease;
    IDXGIAdapter1 *adapter = new IDXGIAdapter1();
    adapter->lpVtbl = vtbl;
    return adapter;
}

HRESULT __stdcall ShimEnumAdapters1_1(IDXGIFactory1 *This,
                                      /* [in] */ UINT Adapter,
                                      /* [annotation][out] */
                                      _COM_Outptr_ IDXGIAdapter1 **ppAdapter) {
    if (Adapter >= platform_shim.dxgi_adapters.size()) {
        return DXGI_ERROR_INVALID_CALL;
    }
    if (ppAdapter != nullptr) {
        auto* pAdapter = create_IDXGIAdapter1();
        *ppAdapter = pAdapter;
        platform_shim.dxgi_adapter_map[pAdapter] = Adapter;
    }
    return S_OK;
}

HRESULT __stdcall ShimEnumAdapters1_6(IDXGIFactory6 *This,
                                      /* [in] */ UINT Adapter,
                                      /* [annotation][out] */
                                      _COM_Outptr_ IDXGIAdapter1 **ppAdapter) {
    if (Adapter >= platform_shim.dxgi_adapters.size()) {
        return DXGI_ERROR_INVALID_CALL;
    }
    if (ppAdapter != nullptr) {
        auto* pAdapter = create_IDXGIAdapter1();
        *ppAdapter = pAdapter;
        platform_shim.dxgi_adapter_map[pAdapter] = Adapter;
    }
    return S_OK;
}

HRESULT __stdcall ShimEnumAdapterByGpuPreference(IDXGIFactory6 *This, _In_ UINT Adapter, _In_ DXGI_GPU_PREFERENCE GpuPreference,
                                                 _In_ REFIID riid, _COM_Outptr_ void **ppvAdapter) {
    if (Adapter >= platform_shim.dxgi_adapters.size()) {
        return DXGI_ERROR_NOT_FOUND;
    }
    // loader always uses DXGI_GPU_PREFERENCE_UNSPECIFIED
    // Update the shim if this isn't the case
    assert(GpuPreference == DXGI_GPU_PREFERENCE::DXGI_GPU_PREFERENCE_UNSPECIFIED &&
           "Test shim assumes the GpuPreference is unspecified.");
    if (ppvAdapter != nullptr) {
        auto* pAdapter = create_IDXGIAdapter1();
        *ppvAdapter = pAdapter;
        platform_shim.dxgi_adapter_map[pAdapter] = Adapter;
    }
    return S_OK;
}

IDXGIFactory1 *create_IDXGIFactory1() {
    IDXGIFactory1Vtbl *vtbl = new IDXGIFactory1Vtbl();
    vtbl->EnumAdapters1 = ShimEnumAdapters1_1;
    vtbl->Release = ShimIDXGIFactory1Release;
    IDXGIFactory1 *factory = new IDXGIFactory1();
    factory->lpVtbl = vtbl;
    return factory;
}

IDXGIFactory6 *create_IDXGIFactory6() {
    IDXGIFactory6Vtbl *vtbl = new IDXGIFactory6Vtbl();
    vtbl->EnumAdapters1 = ShimEnumAdapters1_6;
    vtbl->EnumAdapterByGpuPreference = ShimEnumAdapterByGpuPreference;
    vtbl->Release = ShimIDXGIFactory6Release;
    IDXGIFactory6 *factory = new IDXGIFactory6();
    factory->lpVtbl = vtbl;
    return factory;
}

HRESULT __stdcall ShimCreateDXGIFactory1(REFIID riid, void **ppFactory) {
    if (riid == IID_IDXGIFactory1) {
        auto *factory = create_IDXGIFactory1();
        *ppFactory = factory;
        return S_OK;
    }
    if (riid == IID_IDXGIFactory6) {
        auto *factory = create_IDXGIFactory6();
        *ppFactory = factory;
        return S_OK;
    }
    assert(false && "new riid, update shim code to handle");
    return S_FALSE;
}

// Windows Registry shims
using PFN_RegOpenKeyExA = LSTATUS(__stdcall *)(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);
static PFN_RegOpenKeyExA fpRegOpenKeyExA = RegOpenKeyExA;
using PFN_RegQueryValueExA = LSTATUS(__stdcall *)(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData,
                                                  LPDWORD lpcbData);
static PFN_RegQueryValueExA fpRegQueryValueExA = RegQueryValueExA;
using PFN_RegEnumValueA = LSTATUS(__stdcall *)(HKEY hKey, DWORD dwIndex, LPSTR lpValueName, LPDWORD lpcchValueName,
                                               LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
static PFN_RegEnumValueA fpRegEnumValueA = RegEnumValueA;

using PFN_RegCloseKey = LSTATUS(__stdcall *)(HKEY hKey);
static PFN_RegCloseKey fpRegCloseKey = RegCloseKey;

LSTATUS __stdcall ShimRegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult) {
    if (HKEY_LOCAL_MACHINE != hKey && HKEY_CURRENT_USER != hKey) return ERROR_BADKEY;
    std::string hive = "";
    if (HKEY_LOCAL_MACHINE == hKey)
        hive = "HKEY_LOCAL_MACHINE";
    else if (HKEY_CURRENT_USER == hKey)
        hive = "HKEY_CURRENT_USER";

    platform_shim.created_keys.emplace_back(platform_shim.created_key_count++, hive + "\\" + lpSubKey);
    *phkResult = platform_shim.created_keys.back().get();
    return 0;
}
const std::string *get_path_of_created_key(HKEY hKey) {
    for (const auto &key : platform_shim.created_keys) {
        if (key.key == hKey) {
            return &key.path;
        }
    }
    return nullptr;
}
std::vector<RegistryEntry> *get_registry_vector(std::string const &path) {
    if (path == "HKEY_LOCAL_MACHINE\\SOFTWARE\\Khronos\\Vulkan\\Drivers") return &platform_shim.hkey_local_machine_drivers;
    if (path == "HKEY_LOCAL_MACHINE\\SOFTWARE\\Khronos\\Vulkan\\ExplicitLayers")
        return &platform_shim.hkey_local_machine_explicit_layers;
    if (path == "HKEY_LOCAL_MACHINE\\SOFTWARE\\Khronos\\Vulkan\\ImplicitLayers")
        return &platform_shim.hkey_local_machine_implicit_layers;
    if (path == "HKEY_CURRENT_USER\\SOFTWARE\\Khronos\\Vulkan\\ExplicitLayers")
        return &platform_shim.hkey_current_user_explicit_layers;
    if (path == "HKEY_CURRENT_USER\\SOFTWARE\\Khronos\\Vulkan\\ImplicitLayers")
        return &platform_shim.hkey_current_user_implicit_layers;
    return nullptr;
}
LSTATUS __stdcall ShimRegQueryValueExA(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData,
                                       LPDWORD lpcbData) {
    // TODO:
    return ERROR_SUCCESS;
}
LSTATUS __stdcall ShimRegEnumValueA(HKEY hKey, DWORD dwIndex, LPSTR lpValueName, LPDWORD lpcchValueName, LPDWORD lpReserved,
                                    LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    const std::string *path = get_path_of_created_key(hKey);
    if (path == nullptr) return ERROR_NO_MORE_ITEMS;
    const auto *location_ptr = get_registry_vector(*path);
    if (location_ptr == nullptr) return ERROR_NO_MORE_ITEMS;
    const auto &location = *location_ptr;
    if (dwIndex >= location.size()) return ERROR_NO_MORE_ITEMS;
    if (*lpcchValueName < location[dwIndex].name.size()) return ERROR_NO_MORE_ITEMS;
    for (size_t i = 0; i < location[dwIndex].name.size(); i++) {
        lpValueName[i] = location[dwIndex].name[i];
    }
    lpValueName[location[dwIndex].name.size()] = '\0';
    *lpcchValueName = static_cast<DWORD>(location[dwIndex].name.size() + 1);
    if (*lpcbData < sizeof(DWORD)) return ERROR_NO_MORE_ITEMS;
    DWORD *lpcbData_dword = reinterpret_cast<DWORD *>(lpData);
    *lpcbData_dword = location[dwIndex].value;
    *lpcbData = sizeof(DWORD);
    return ERROR_SUCCESS;
}
LSTATUS __stdcall ShimRegCloseKey(HKEY hKey) {
    for (size_t i = 0; i < platform_shim.created_keys.size(); i++) {
        if (platform_shim.created_keys[i].get() == hKey) {
            platform_shim.created_keys.erase(platform_shim.created_keys.begin() + i);
            return ERROR_SUCCESS;
        }
    }
    return ERROR_SUCCESS;
}

// Initialization
void WINAPI DetourFunctions() {
    if (!gdi32_dll) {
        gdi32_dll = LibraryWrapper("gdi32.dll");
        fpEnumAdapters2 = gdi32_dll.get_symbol("D3DKMTEnumAdapters2");
        if (fpEnumAdapters2 == nullptr) {
            std::cerr << "Failed to load D3DKMTEnumAdapters2\n";
            return;
        }
        fpQueryAdapterInfo = gdi32_dll.get_symbol("D3DKMTQueryAdapterInfo");
        if (fpQueryAdapterInfo == nullptr) {
            std::cerr << "Failed to load D3DKMTQueryAdapterInfo\n";
            return;
        }
    }
    if (!dxgi_module) {
        TCHAR systemPath[MAX_PATH] = "";
        GetSystemDirectory(systemPath, MAX_PATH);
        StringCchCat(systemPath, MAX_PATH, TEXT("\\dxgi.dll"));
        dxgi_module = LibraryWrapper(systemPath);
        RealCreateDXGIFactory1 = dxgi_module.get_symbol("CreateDXGIFactory1");
        if (RealCreateDXGIFactory1 == nullptr) {
            std::cerr << "Failed to load CreateDXGIFactory1\n";
        }
    }

    DetourRestoreAfterWith();

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID &)fpGetSidSubAuthority, (PVOID)ShimGetSidSubAuthority);
    DetourAttach(&(PVOID &)fpEnumAdapters2, (PVOID)ShimEnumAdapters2);
    DetourAttach(&(PVOID &)fpQueryAdapterInfo, (PVOID)ShimQueryAdapterInfo);
    DetourAttach(&(PVOID &)REAL_CM_Get_Device_ID_List_SizeW, (PVOID)SHIM_CM_Get_Device_ID_List_SizeW);
    DetourAttach(&(PVOID &)REAL_CM_Get_Device_ID_ListW, (PVOID)SHIM_CM_Get_Device_ID_ListW);
    DetourAttach(&(PVOID &)REAL_CM_Get_Device_ID_ListW, (PVOID)SHIM_CM_Get_Device_ID_ListW);
    DetourAttach(&(PVOID &)REAL_CM_Locate_DevNodeW, (PVOID)SHIM_CM_Locate_DevNodeW);
    DetourAttach(&(PVOID &)REAL_CM_Get_DevNode_Status, (PVOID)SHIM_CM_Get_DevNode_Status);
    DetourAttach(&(PVOID &)REAL_CM_Get_Device_IDW, (PVOID)SHIM_CM_Get_Device_IDW);
    DetourAttach(&(PVOID &)REAL_CM_Get_Child, (PVOID)SHIM_CM_Get_Child);
    DetourAttach(&(PVOID &)REAL_CM_Get_DevNode_Registry_PropertyW, (PVOID)SHIM_CM_Get_DevNode_Registry_PropertyW);
    DetourAttach(&(PVOID &)REAL_CM_Get_Sibling, (PVOID)SHIM_CM_Get_Sibling);
    DetourAttach(&(PVOID &)RealCreateDXGIFactory1, (PVOID)ShimCreateDXGIFactory1);
    DetourAttach(&(PVOID &)fpRegOpenKeyExA, (PVOID)ShimRegOpenKeyExA);
    DetourAttach(&(PVOID &)fpRegQueryValueExA, (PVOID)ShimRegQueryValueExA);
    DetourAttach(&(PVOID &)fpRegEnumValueA, (PVOID)ShimRegEnumValueA);
    DetourAttach(&(PVOID &)fpRegCloseKey, (PVOID)ShimRegCloseKey);
    LONG error = DetourTransactionCommit();

    if (error != NO_ERROR) {
        std::cerr << "simple" << DETOURS_STRINGIFY(DETOURS_BITS) << ".dll:"
                  << " Error detouring function(): " << error << "\n";
    }
}

void DetachFunctions() {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID &)fpGetSidSubAuthority, (PVOID)ShimGetSidSubAuthority);
    DetourDetach(&(PVOID &)fpEnumAdapters2, (PVOID)ShimEnumAdapters2);
    DetourDetach(&(PVOID &)fpQueryAdapterInfo, (PVOID)ShimQueryAdapterInfo);
    DetourDetach(&(PVOID &)REAL_CM_Get_Device_ID_List_SizeW, (PVOID)SHIM_CM_Get_Device_ID_List_SizeW);
    DetourDetach(&(PVOID &)REAL_CM_Get_Device_ID_ListW, (PVOID)SHIM_CM_Get_Device_ID_ListW);
    DetourDetach(&(PVOID &)REAL_CM_Locate_DevNodeW, (PVOID)SHIM_CM_Locate_DevNodeW);
    DetourDetach(&(PVOID &)REAL_CM_Get_DevNode_Status, (PVOID)SHIM_CM_Get_DevNode_Status);
    DetourDetach(&(PVOID &)REAL_CM_Get_Device_IDW, (PVOID)SHIM_CM_Get_Device_IDW);
    DetourDetach(&(PVOID &)REAL_CM_Get_Child, (PVOID)SHIM_CM_Get_Child);
    DetourDetach(&(PVOID &)REAL_CM_Get_DevNode_Registry_PropertyW, (PVOID)SHIM_CM_Get_DevNode_Registry_PropertyW);
    DetourDetach(&(PVOID &)REAL_CM_Get_Sibling, (PVOID)SHIM_CM_Get_Sibling);
    DetourDetach(&(PVOID &)RealCreateDXGIFactory1, (PVOID)ShimCreateDXGIFactory1);
    DetourDetach(&(PVOID &)fpRegOpenKeyExA, (PVOID)ShimRegOpenKeyExA);
    DetourDetach(&(PVOID &)fpRegQueryValueExA, (PVOID)ShimRegQueryValueExA);
    DetourDetach(&(PVOID &)fpRegEnumValueA, (PVOID)ShimRegEnumValueA);
    DetourDetach(&(PVOID &)fpRegCloseKey, (PVOID)ShimRegCloseKey);
    DetourTransactionCommit();
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
    if (DetourIsHelperProcess()) {
        return TRUE;
    }

    if (dwReason == DLL_PROCESS_ATTACH) {
        DetourFunctions();
    } else if (dwReason == DLL_PROCESS_DETACH) {
        DetachFunctions();
    }
    return TRUE;
}
FRAMEWORK_EXPORT PlatformShim *get_platform_shim() {
    platform_shim = PlatformShim();
    return &platform_shim;
}
}