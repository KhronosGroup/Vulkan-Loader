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

#pragma once

#include "test_util.h"

#include <stdlib.h>

#if defined(WIN32)
#include <strsafe.h>
#include <cfgmgr32.h>
#include <initguid.h>
#include <devpkey.h>
#include <winternl.h>

#define CINTERFACE
#include <dxgi1_6.h>
#include <adapters.h>
#endif

enum class ManifestCategory { implicit_layer, explicit_layer, icd };
enum class GpuType { unspecified, integrated, discrete, external };

#if defined(WIN32)

struct KeyWrapper {
    explicit KeyWrapper(HKEY key) noexcept;
    explicit KeyWrapper(HKEY key_root, const char* key_path) noexcept;
    ~KeyWrapper() noexcept;
    explicit KeyWrapper(KeyWrapper const&) = delete;
    KeyWrapper& operator=(KeyWrapper const&) = delete;
    explicit KeyWrapper(KeyWrapper&& other) noexcept;
    KeyWrapper& operator=(KeyWrapper&& other) noexcept;

    HKEY get() const noexcept { return key; }
    operator HKEY() { return key; }
    operator HKEY() const { return key; }
    operator HKEY&() { return key; }
    operator HKEY const &() const { return key; }

    HKEY key{};
};

static const char* pnp_registry_path = "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}";

// Needed for DXGI mocking
struct KnownDriverData {
    const char* filename;
    int vendor_id;
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
    DXGIAdapter(fs::path const& manifest_path, GpuType gpu_preference, uint32_t known_driver_index, DXGI_ADAPTER_DESC1 desc1,
                uint32_t adapter_index)
        : manifest_path(manifest_path),
          gpu_preference(gpu_preference),
          known_driver_index(known_driver_index),
          desc1(desc1),
          adapter_index(adapter_index) {}
    fs::path manifest_path;
    GpuType gpu_preference = GpuType::unspecified;
    uint32_t known_driver_index = UINT_MAX;  // index into the known_driver_list, UINT_MAX if it shouldn't index at all.
    DXGI_ADAPTER_DESC1 desc1{};
    uint32_t adapter_index = 0;
};

struct SHIM_D3DKMT_ADAPTERINFO {
    UINT hAdapter;
    LUID AdapterLuid;
    ULONG NumOfSources;
    BOOL bPresentMoveRegionsPreferred;
};

struct D3DKMT_Adapter {
    SHIM_D3DKMT_ADAPTERINFO info;
    fs::path path;
};

#endif
// Necessary to have inline definitions as shim is a dll and thus functions
// defined in the .cpp wont be found by the rest of the application
struct PlatformShim {
    // Test Framework interface
    void setup_override(DebugMode debug_mode = DebugMode::none);
    void clear_override(DebugMode debug_mode = DebugMode::none);

    void reset(DebugMode debug_mode = DebugMode::none);

    void redirect_all_paths(fs::path const& path);
    void redirect_category(fs::path const& new_path, ManifestCategory category);

    void set_path(ManifestCategory category, fs::path const& path);

    void add_manifest(ManifestCategory category, fs::path const& path);

// platform specific shim interface
#if defined(WIN32)
    // Control Platform Elevation Level
    unsigned long elevation_level = SECURITY_MANDATORY_LOW_RID;
    void set_elevation_level(unsigned long new_elevation_level) { elevation_level = new_elevation_level; }

    void add_dxgi_adapter(fs::path const& manifest_path, GpuType gpu_preference, uint32_t known_driver_index,
                          DXGI_ADAPTER_DESC1 desc1);
    void add_d3dkmt_adapter(SHIM_D3DKMT_ADAPTERINFO adapter, fs::path const& path);
    void add_CM_Device_ID(std::wstring const& id, fs::path const& icd_path, fs::path const& layer_path);

    uint32_t next_adapter_handle = 1;  // increment everytime add_dxgi_adapter is called
    std::vector<DXGIAdapter> dxgi_adapters;
    std::unordered_map<IDXGIAdapter1*, uint32_t> dxgi_adapter_map;
    // next two are a pair
    std::vector<D3DKMT_Adapter> d3dkmt_adapters;

    std::wstring CM_device_ID_list = {L'\0'};
    std::vector<KeyWrapper> CM_device_ID_registry_keys;

    uint32_t random_base_path = 0;

    std::vector<fs::path> icd_paths;

#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
    bool is_fake_path(fs::path const& path);
    fs::path const& get_fake_path(fs::path const& path);

    void redirect_path(fs::path const& path, fs::path const& new_path);
    void remove_redirect(fs::path const& path);

    std::unordered_map<std::string, fs::path> redirection_map;

#endif
};

std::vector<std::string> parse_env_var_list(std::string const& var);
std::string category_path_name(ManifestCategory category);

extern "C" {
// dynamically link on windows and macos
#if defined(WIN32) || defined(__APPLE__)
using PFN_get_platform_shim = PlatformShim* (*)();
#define GET_PLATFORM_SHIM_STR "get_platform_shim"

#elif defined(__linux__) || defined(__FreeBSD__)
// statically link on linux
PlatformShim* get_platform_shim();
#endif
}
