/*
 * Copyright (c) 2021-2022 The Khronos Group Inc.
 * Copyright (c) 2021-2022 Valve Corporation
 * Copyright (c) 2021-2022 LunarG, Inc.
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

#pragma once

#include "test_util.h"

#include <unordered_set>
#include <stdlib.h>

#if defined(WIN32)
#include <strsafe.h>
#include <cfgmgr32.h>
#include <initguid.h>
#include <devpkey.h>
#include <winternl.h>
#include <appmodel.h>

#define CINTERFACE
#include <dxgi1_6.h>
#include <adapters.h>
#endif

enum class ManifestCategory { implicit_layer, explicit_layer, icd, settings };
enum class GpuType { unspecified, integrated, discrete, external };

#if defined(WIN32)
struct RegistryEntry {
    RegistryEntry() = default;
    RegistryEntry(std::string const& name) noexcept : name(name) {}
    RegistryEntry(std::string const& name, DWORD value) noexcept : name(name), value(value) {}
    std::string name;
    DWORD value{};
};

struct HKeyHandle {
    explicit HKeyHandle(const size_t value, const std::string& key_path) noexcept : key(HKEY{}), path(key_path) {
        key = reinterpret_cast<HKEY>(value);
    }

    HKEY get() const noexcept { return key; }

    HKEY key{};
    std::string path;
};

static const char* pnp_registry_path = "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}";

// Needed for DXGI mocking
struct KnownDriverData {
    const char* filename = nullptr;
    int vendor_id = 0;
};
static std::array<KnownDriverData, 4> known_driver_list = {
#if defined(_WIN64)
    KnownDriverData{"igvk64.json", 0x8086}, KnownDriverData{"nv-vk64.json", 0x10de}, KnownDriverData{"amd-vulkan64.json", 0x1002},
    KnownDriverData{"amdvlk64.json", 0x1002}
#else
    KnownDriverData{"igvk32.json", 0x8086}, KnownDriverData{"nv-vk32.json", 0x10de}, KnownDriverData{"amd-vulkan32.json", 0x1002},
    KnownDriverData{"amdvlk32.json", 0x1002}
#endif
};

struct DXGIAdapter {
    GpuType gpu_preference = GpuType::unspecified;
    DXGI_ADAPTER_DESC1 desc1{};
    uint32_t adapter_index = 0;
};

struct D3DKMT_Adapter {
    D3DKMT_Adapter& add_driver_manifest_path(fs::path const& src);
    D3DKMT_Adapter& add_implicit_layer_manifest_path(fs::path const& src);
    D3DKMT_Adapter& add_explicit_layer_manifest_path(fs::path const& src);

    UINT hAdapter;
    LUID adapter_luid;
    std::vector<std::wstring> driver_paths;
    std::vector<std::wstring> implicit_layer_paths;
    std::vector<std::wstring> explicit_layer_paths;

   private:
    D3DKMT_Adapter& add_path(fs::path src, std::vector<std::wstring>& dest);
};

#elif COMMON_UNIX_PLATFORMS

struct DirEntry {
    DIR* directory = nullptr;
    std::string folder_path;
    std::vector<struct dirent*> contents;
    // the current item being read by an app (incremented by readdir, reset to zero by opendir & closedir)
    size_t current_index = 0;
    bool is_fake_path = false;  // true when this entry is for folder redirection
};

#endif

struct FrameworkEnvironment;  // forward declaration

// Necessary to have inline definitions as shim is a dll and thus functions
// defined in the .cpp wont be found by the rest of the application
struct PlatformShim {
    PlatformShim() = default;
    PlatformShim(std::vector<fs::FolderManager>* folders) : folders(folders) {}

    // Used to get info about which drivers & layers have been added to folders
    std::vector<fs::FolderManager>* folders;

    // Test Framework interface
    void reset();

    void redirect_all_paths(fs::path const& path);
    void redirect_category(fs::path const& new_path, ManifestCategory category);

    // fake paths are paths that the loader normally looks in but actually point to locations inside the test framework
    void set_fake_path(ManifestCategory category, fs::path const& path);

    // known paths are real paths but since the test framework guarantee's the order files are found in, files in these paths
    // need to be ordered correctly
    void add_known_path(fs::path const& path);

    void add_manifest(ManifestCategory category, fs::path const& path);
    void add_unsecured_manifest(ManifestCategory category, fs::path const& path);

// platform specific shim interface
#if defined(WIN32)
    // Control Platform Elevation Level
    void set_elevated_privilege(bool elev) { elevation_level = (elev) ? SECURITY_MANDATORY_HIGH_RID : SECURITY_MANDATORY_LOW_RID; }
    unsigned long elevation_level = SECURITY_MANDATORY_LOW_RID;

    void add_dxgi_adapter(GpuType gpu_preference, DXGI_ADAPTER_DESC1 desc1);
    void add_d3dkmt_adapter(D3DKMT_Adapter const& adapter);
    void set_app_package_path(fs::path const& path);

    uint32_t next_adapter_handle = 1;  // increment everytime add_dxgi_adapter is called
    std::vector<DXGIAdapter> dxgi_adapters;
    std::unordered_map<IDXGIAdapter1*, uint32_t> dxgi_adapter_map;
    // next two are a pair
    std::vector<D3DKMT_Adapter> d3dkmt_adapters;

    // TODO:
    void add_CM_Device_ID(std::wstring const& id, fs::path const& icd_path, fs::path const& layer_path);
    std::wstring CM_device_ID_list = {L'\0'};
    std::vector<RegistryEntry> CM_device_ID_registry_keys;

    uint32_t random_base_path = 0;

    std::vector<fs::path> icd_paths;

    std::vector<RegistryEntry> hkey_current_user_explicit_layers;
    std::vector<RegistryEntry> hkey_current_user_implicit_layers;
    std::vector<RegistryEntry> hkey_local_machine_explicit_layers;
    std::vector<RegistryEntry> hkey_local_machine_implicit_layers;
    std::vector<RegistryEntry> hkey_local_machine_drivers;
    std::vector<RegistryEntry> hkey_local_machine_settings;
    std::vector<RegistryEntry> hkey_current_user_settings;

    std::wstring app_package_path;

    // When a key is created, return the index of the
    size_t created_key_count = 0;
    std::vector<HKeyHandle> created_keys;

#elif COMMON_UNIX_PLATFORMS
    bool is_fake_path(fs::path const& path);
    fs::path const& get_real_path_from_fake_path(fs::path const& path);

    void redirect_path(fs::path const& path, fs::path const& new_path);
    void remove_redirect(fs::path const& path);

    bool is_known_path(fs::path const& path);
    void remove_known_path(fs::path const& path);

    void redirect_dlopen_name(fs::path const& filename, fs::path const& actual_path);
    bool is_dlopen_redirect_name(fs::path const& filename);

    fs::path query_default_redirect_path(ManifestCategory category);

    std::unordered_map<std::string, fs::path> redirection_map;
    std::unordered_map<std::string, fs::path> dlopen_redirection_map;
    std::unordered_set<std::string> known_path_set;

    void set_elevated_privilege(bool elev) { use_fake_elevation = elev; }
    bool use_fake_elevation = false;

    std::vector<DirEntry> dir_entries;

#if defined(__APPLE__)
    std::string bundle_contents;
#endif
#endif
    bool is_during_destruction = false;
};

std::vector<std::string> parse_env_var_list(std::string const& var);
std::string category_path_name(ManifestCategory category);

std::vector<std::string> get_folder_contents(std::vector<fs::FolderManager>* folders, std::string folder_name) noexcept;

extern "C" {
// dynamically link on windows and macos
#if defined(WIN32) || defined(__APPLE__)
using PFN_get_platform_shim = PlatformShim* (*)(std::vector<fs::FolderManager>* folders);
#define GET_PLATFORM_SHIM_STR "get_platform_shim"

#elif defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__GNU__)
// statically link on linux
PlatformShim* get_platform_shim(std::vector<fs::FolderManager>* folders);
#endif
}
