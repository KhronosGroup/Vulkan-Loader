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

struct KeyWrapper;

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

#elif defined(__linux__) || defined(__APPLE__)
    bool is_fake_path(fs::path const& path);
    fs::path const& get_fake_path(fs::path const& path);

    std::unordered_map<std::string, fs::path> redirection_map;
#endif
   private:
    void redirect_category(fs::path const& new_path, ManifestCategory category);
#if defined(WIN32)
#elif defined(__linux__) || defined(__APPLE__)
    void add(fs::path const& path, fs::path const& new_path);
    void remove(fs::path const& path);
#endif
};

std::vector<std::string> parse_env_var_list(std::string const& var);
extern "C" {
// dynamically link on windows and macos
#if defined(WIN32) || defined(__APPLE__)
using PFN_get_platform_shim = PlatformShim* (*)();
#define GET_PLATFORM_SHIM_STR "get_platform_shim"

#elif defined(__linux__)
// statically link on linux
PlatformShim* get_platform_shim();
#endif
}
// Functions which are called by the Test Framework need a definition, but since the shim is a DLL, this
// would require loading the functions before using. To get around this, most PlatformShim functions are member
// functions, so only the `get_platform_shim()` is needed to be loaded. As a consequence, all the member functions
// need to be defined inline, so that both framework code and shim dlls can use them.

inline void PlatformShim::redirect_all_paths(fs::path const& path) {
    redirect_category(path, ManifestCategory::implicit_layer);
    redirect_category(path, ManifestCategory::explicit_layer);
    redirect_category(path, ManifestCategory::icd);
}

inline std::vector<std::string> parse_env_var_list(std::string const& var) {
    std::vector<std::string> items;
    int start = 0;
    int len = 0;
    for (size_t i = 0; i < var.size(); i++) {
#if defined(WIN32)
        if (var[i] == ';') {
#elif defined(__linux__) || defined(__APPLE__)
        if (var[i] == ':') {
#endif
            items.push_back(var.substr(start, len));
            start = i + 1;
            len = 0;
        } else {
            len++;
        }
    }
    items.push_back(var.substr(start, len));

    return items;
}

#if defined(WIN32)

inline std::string category_path_name(ManifestCategory category) {
    if (category == ManifestCategory::implicit_layer) return "ImplicitLayers";
    if (category == ManifestCategory::explicit_layer)
        return "ExplicitLayers";
    else
        return "Drivers";
}

inline std::string override_base_path(uint32_t random_base_path) {
    return std::string("SOFTWARE\\LoaderRegressionTests_") + std::to_string(random_base_path);
}

inline std::string get_override_path(HKEY root_key, uint32_t random_base_path) {
    std::string override_path = override_base_path(random_base_path);

    if (root_key == HKEY_CURRENT_USER) {
        override_path += "\\HKCU";
    } else if (root_key == HKEY_LOCAL_MACHINE) {
        override_path += "\\HKLM";
    }
    return override_path;
}

inline HKEY create_key(HKEY key_root, const char* key_path) {
    DWORD dDisposition{};
    HKEY key{};
    LSTATUS out =
        RegCreateKeyExA(key_root, key_path, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &dDisposition);
    if (out != ERROR_SUCCESS) std::cerr << win_api_error_str(out) << " failed to create key " << key << " at " << key_path << "\n";
    return key;
}

inline void close_key(HKEY& key) {
    LSTATUS out = RegCloseKey(key);
    if (out != ERROR_SUCCESS) std::cerr << win_api_error_str(out) << " failed to close key " << key << "\n";
}

inline void setup_override_key(HKEY root_key, uint32_t random_base_path) {
    DWORD dDisposition{};
    LSTATUS out;

    auto override_path = get_override_path(root_key, random_base_path);
    HKEY override_key;
    out = RegCreateKeyExA(HKEY_CURRENT_USER, override_path.c_str(), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
                          &override_key, &dDisposition);
    if (out != ERROR_SUCCESS)
        std::cerr << win_api_error_str(out) << " failed to create key " << override_key << " with path " << override_path << "\n";

    out = RegOverridePredefKey(root_key, override_key);
    if (out != ERROR_SUCCESS) std::cerr << win_api_error_str(out) << " failed to override key " << override_key << "\n";

    close_key(override_key);
}
inline void revert_override(HKEY root_key, uint32_t random_base_path) {
    LSTATUS out = RegOverridePredefKey(root_key, NULL);
    if (out != ERROR_SUCCESS) std::cerr << win_api_error_str(out) << " failed to revert override key " << root_key << "\n";

    auto override_path = get_override_path(root_key, random_base_path);
    out = RegDeleteTreeA(HKEY_CURRENT_USER, override_path.c_str());
    if (out != ERROR_SUCCESS) print_error_message(out, "RegDeleteTreeA", std::string("Key") + override_path);
}

struct KeyWrapper {
    explicit KeyWrapper(HKEY key) noexcept : key(key) {}
    explicit KeyWrapper(HKEY key_root, const char* key_path) noexcept { key = create_key(key_root, key_path); }
    ~KeyWrapper() noexcept {
        if (key != NULL) close_key(key);
    }
    explicit KeyWrapper(KeyWrapper const&) = delete;
    KeyWrapper& operator=(KeyWrapper const&) = delete;
    explicit KeyWrapper(KeyWrapper&& other) : key(other.key) { other.key = NULL; };
    KeyWrapper& operator=(KeyWrapper&& other) {
        if (this != &other) {
            if (key != NULL) close_key(key);
            key = other.key;
            other.key = NULL;
        }
        return *this;
    };

    HKEY get() const noexcept { return key; }
    operator HKEY() { return key; }
    operator HKEY() const { return key; }
    operator HKEY&() { return key; }
    operator HKEY const &() const { return key; }

    HKEY key{};
};

inline void add_key_value(HKEY const& key, fs::path const& manifest_path, bool enabled = true) {
    DWORD value = enabled ? 0 : 1;
    LSTATUS out = RegSetValueEx(key, manifest_path.c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(value));
    if (out != ERROR_SUCCESS) std::cerr << win_api_error_str(out) << " failed to set key value for " << manifest_path.str() << "\n";
}

inline void add_key_value_string(HKEY const& key, const char* name, const char* str) {
    LSTATUS out = RegSetValueExA(key, name, 0, REG_SZ, reinterpret_cast<const BYTE*>(str), strlen(str));
    if (out != ERROR_SUCCESS)
        std::cerr << win_api_error_str(out) << " failed to set string value for " << name << ":" << str << "\n";
}

inline void remove_key_value(HKEY const& key, fs::path const& manifest_path) {
    LSTATUS out = RegDeleteValueA(key, manifest_path.c_str());
    if (out != ERROR_SUCCESS)
        std::cerr << win_api_error_str(out) << " failed to delete key value for " << manifest_path.str() << "\n";
}

inline void clear_key_values(HKEY const& key) {
    DWORD dwNumValues, dwValueNameLen;

    LSTATUS out = RegQueryInfoKey(key, 0, 0, 0, 0, 0, 0, &dwNumValues, &dwValueNameLen, 0, 0, 0);
    if (out != ERROR_SUCCESS) {
        std::cerr << win_api_error_str(out) << "couldn't query enum " << key << " values\n";
        return;
    }
    std::string tchValName(dwValueNameLen + 1, '\0');
    for (DWORD i = 0; i < dwNumValues; i++) {
        DWORD length = tchValName.size();
        LPSTR lpstr = &tchValName[0];
        out = RegEnumValue(key, i, lpstr, &length, 0, 0, 0, 0);
        if (out != ERROR_SUCCESS) {
            std::cerr << win_api_error_str(out) << "couldn't get enum value " << tchValName << "\n";
            return;
        }
    }
}

inline void PlatformShim::setup_override(DebugMode debug_mode) {
    std::random_device rd;
    std::ranlux48 gen(rd());
    std::uniform_int_distribution<uint32_t> dist(0, 2000000);
    while (random_base_path == 0) {
        uint32_t random_num = dist(gen);
        auto override_path = get_override_path(HKEY_CURRENT_USER, random_num);
        HKEY temp_key = NULL;
        auto result = RegOpenKeyEx(HKEY_CURRENT_USER, override_path.c_str(), 0, KEY_READ, &temp_key);
        if (result != ERROR_SUCCESS) {
            // Didn't find it, use the random number
            random_base_path = random_num;
        } else {
            // try a different random number that isn't being used
            std::cout << "INFO: Encountered existing registry key, is the registry full of old LoaderRegressionTest keys?\n";
        }
    }
    auto reg_base = override_base_path(random_base_path);
    HKEY timestamp_key = create_key(HKEY_CURRENT_USER, reg_base.c_str());

    std::time_t cur_time = std::time(nullptr);
    auto* cur_time_text = std::ctime(&cur_time);
    add_key_value_string(timestamp_key, "Timestamp", cur_time_text);

    setup_override_key(HKEY_LOCAL_MACHINE, random_base_path);
    setup_override_key(HKEY_CURRENT_USER, random_base_path);
}
inline void PlatformShim::clear_override(DebugMode debug_mode) {
    if (debug_mode != DebugMode::no_delete) {
        revert_override(HKEY_CURRENT_USER, random_base_path);
        revert_override(HKEY_LOCAL_MACHINE, random_base_path);

        LSTATUS out = RegDeleteKeyA(HKEY_CURRENT_USER, override_base_path(random_base_path).c_str());
        if (out != ERROR_SUCCESS)
            print_error_message(out, "RegDeleteKeyA", std::string("Key") + override_base_path(random_base_path).c_str());
    }
}
inline void PlatformShim::reset(DebugMode debug_mode) {
    KeyWrapper implicit_key{HKEY_LOCAL_MACHINE, "SOFTWARE\\Khronos\\Vulkan\\ImplicitLayers"};
    KeyWrapper explicit_key{HKEY_LOCAL_MACHINE, "SOFTWARE\\Khronos\\Vulkan\\ExplicitLayers"};
    KeyWrapper icd_key{HKEY_LOCAL_MACHINE, "SOFTWARE\\Khronos\\Vulkan\\Drivers"};
    if (debug_mode != DebugMode::no_delete) {
        clear_key_values(implicit_key);
        clear_key_values(explicit_key);
        clear_key_values(icd_key);
    }
}

inline void PlatformShim::set_path(ManifestCategory category, fs::path const& path) {}

inline void PlatformShim::add_manifest(ManifestCategory category, fs::path const& path) {
    std::string reg_path = std::string("SOFTWARE\\Khronos\\Vulkan\\") + category_path_name(category);
    KeyWrapper key{HKEY_LOCAL_MACHINE, reg_path.c_str()};
    add_key_value(key, path);
    if (category == ManifestCategory::icd) {
        icd_paths.push_back(path);
    }
}
inline void PlatformShim::add_dxgi_adapter(fs::path const& manifest_path, GpuType gpu_preference, uint32_t known_driver_index,
                                           DXGI_ADAPTER_DESC1 desc1) {
    dxgi_adapters.push_back(DXGIAdapter(manifest_path, gpu_preference, known_driver_index, desc1, next_adapter_handle++));
}

inline void PlatformShim::add_d3dkmt_adapter(SHIM_D3DKMT_ADAPTERINFO adapter, fs::path const& path) {
    d3dkmt_adapters.push_back({adapter, path});
}

inline void PlatformShim::add_CM_Device_ID(std::wstring const& id, fs::path const& icd_path, fs::path const& layer_path) {
    // append a null byte as separator if there is already id's in the list
    if (CM_device_ID_list.size() != 0) {
        CM_device_ID_list += L'\0';  // I'm sure this wont cause issues with std::string down the line... /s
    }
    CM_device_ID_list += id;
    std::string id_str(id.length(), '\0');
    std::wcstombs(&id_str[0], id.c_str(), id_str.length());

    std::string device_path = std::string(pnp_registry_path) + "\\" + id_str;
    CM_device_ID_registry_keys.emplace_back(HKEY_LOCAL_MACHINE, device_path.c_str());
    HKEY id_key = CM_device_ID_registry_keys.back().key;
    add_key_value_string(id_key, "VulkanDriverName", icd_path.c_str());
    add_key_value_string(id_key, "VulkanLayerName", layer_path.c_str());
    // TODO: decide how to handle 32 bit
    // add_key_value_string(id_key, "VulkanDriverNameWoW", icd_path.c_str());
    // add_key_value_string(id_key, "VulkanLayerName", layer_path.c_str());
}

inline HKEY GetRegistryKey() { return HKEY{}; }

inline void PlatformShim::redirect_category(fs::path const& new_path, ManifestCategory search_category) {
    // create_key(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\0001");
    // create_key(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\0002");
    switch (search_category) {
        case (ManifestCategory::implicit_layer):
            create_key(HKEY_CURRENT_USER, "SOFTWARE\\Khronos\\Vulkan\\ImplicitLayers");
            create_key(HKEY_LOCAL_MACHINE, "SOFTWARE\\Khronos\\Vulkan\\ImplicitLayers");
            create_key(HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\Khronos\\Vulkan\\ImplicitLayers");
            break;
        case (ManifestCategory::explicit_layer):
            create_key(HKEY_CURRENT_USER, "SOFTWARE\\Khronos\\Vulkan\\ExplicitLayers");
            create_key(HKEY_LOCAL_MACHINE, "SOFTWARE\\Khronos\\Vulkan\\ExplicitLayers");
            create_key(HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\Khronos\\Vulkan\\ExplicitLayers");
            break;
        case (ManifestCategory::icd):
            create_key(HKEY_LOCAL_MACHINE, "SOFTWARE\\Khronos\\Vulkan\\Drivers");
            create_key(HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\Khronos\\Vulkan\\Drivers");
            break;
    }
}

#elif defined(__linux__) || defined(__APPLE__)

#include <dirent.h>
#include <unistd.h>

inline std::string category_path_name(ManifestCategory category) {
    if (category == ManifestCategory::implicit_layer) return "implicit_layer.d";
    if (category == ManifestCategory::explicit_layer)
        return "explicit_layer.d";
    else
        return "icd.d";
}

inline void PlatformShim::setup_override(DebugMode debug_mode) {}
inline void PlatformShim::clear_override(DebugMode debug_mode) {}
inline void PlatformShim::reset(DebugMode debug_mode) { redirection_map.clear(); }

inline void PlatformShim::add(fs::path const& path, fs::path const& new_path) { redirection_map[path.str()] = new_path; }
inline void PlatformShim::remove(fs::path const& path) { redirection_map.erase(path.str()); }
inline bool PlatformShim::is_fake_path(fs::path const& path) { return redirection_map.count(path.str()) > 0; }
inline fs::path const& PlatformShim::get_fake_path(fs::path const& path) { return redirection_map.at(path.str()); }

inline void PlatformShim::add_manifest(ManifestCategory category, fs::path const& path) {}

inline void PlatformShim::redirect_category(fs::path const& new_path, ManifestCategory category) {
    std::vector<std::string> xdg_paths;
    std::string xdg_data_dirs_var = get_env_var("XDG_DATA_DIRS");
    if (xdg_data_dirs_var.size() == 0) {
        xdg_data_dirs_var = FALLBACK_CONFIG_DIRS;
    }
    auto data_dirs_paths = parse_env_var_list(xdg_data_dirs_var);
    xdg_paths.insert(xdg_paths.begin(), data_dirs_paths.begin(), data_dirs_paths.end());

    std::string xdg_config_dirs_var = get_env_var("XDG_CONFIG_DIRS");
    if (xdg_config_dirs_var.size() == 0) {
        xdg_config_dirs_var = FALLBACK_DATA_DIRS;
    }
    auto config_dirs_paths = parse_env_var_list(xdg_config_dirs_var);
    xdg_paths.insert(xdg_paths.begin(), config_dirs_paths.begin(), config_dirs_paths.end());

    add(fs::path(SYSCONFDIR) / "vulkan" / category_path_name(category), new_path);
#if defined(EXTRASYSCONFDIR)
    // EXTRASYSCONFDIR default is /etc, if SYSCONFDIR wasn't defined, it will have /etc put
    // as its default. Don't want to double add it
    if (!string_eq(SYSCONFDIR, EXTRASYSCONFDIR)) {
        add(fs::path(EXTRASYSCONFDIR) / "vulkan" / category_path_name(category), new_path);
    }
#endif

    for (auto& path : xdg_paths) {
        add(fs::path(path) / "vulkan" / category_path_name(category), new_path);
    }

    std::string home = get_env_var("HOME");
    if (home.size() != 0) {
        add(fs::path(home) / ".local/share/vulkan" / category_path_name(category), new_path);
    }
}

inline void PlatformShim::set_path(ManifestCategory category, fs::path const& path) {
    // use /etc as the 'redirection path' by default since its always searched

    if (category == ManifestCategory::implicit_layer) add(fs::path(SYSCONFDIR) / "vulkan/implicit_layer.d", path);
    if (category == ManifestCategory::explicit_layer) add(fs::path(SYSCONFDIR) / "vulkan/explicit_layer.d", path);
    if (category == ManifestCategory::icd) add(fs::path(SYSCONFDIR) / "vulkan/icd.d", path);
}

#endif