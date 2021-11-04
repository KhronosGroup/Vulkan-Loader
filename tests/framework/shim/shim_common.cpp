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

#include "shim.h"

#include <random>

void PlatformShim::redirect_all_paths(fs::path const& path) {
    redirect_category(path, ManifestCategory::implicit_layer);
    redirect_category(path, ManifestCategory::explicit_layer);
    redirect_category(path, ManifestCategory::icd);
}

std::vector<std::string> parse_env_var_list(std::string const& var) {
    std::vector<std::string> items;
    size_t start = 0;
    size_t len = 0;
    for (size_t i = 0; i < var.size(); i++) {
#if defined(WIN32)
        if (var[i] == ';') {
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
        if (var[i] == ':') {
#endif
            if (len != 0) {
                // only push back non empty strings
                items.push_back(var.substr(start, len));
            }
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

std::string category_path_name(ManifestCategory category) {
    if (category == ManifestCategory::implicit_layer) return "ImplicitLayers";
    if (category == ManifestCategory::explicit_layer)
        return "ExplicitLayers";
    else
        return "Drivers";
}

std::string override_base_path(uint32_t random_base_path) {
    return std::string("SOFTWARE\\LoaderRegressionTests_") + std::to_string(random_base_path);
}

std::string get_override_path(HKEY root_key, uint32_t random_base_path) {
    std::string override_path = override_base_path(random_base_path);

    if (root_key == HKEY_CURRENT_USER) {
        override_path += "\\HKCU";
    } else if (root_key == HKEY_LOCAL_MACHINE) {
        override_path += "\\HKLM";
    }
    return override_path;
}

HKEY create_key(HKEY key_root, const char* key_path) {
    DWORD dDisposition{};
    HKEY key{};
    LSTATUS out =
        RegCreateKeyExA(key_root, key_path, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &dDisposition);
    if (out != ERROR_SUCCESS) std::cerr << win_api_error_str(out) << " failed to create key " << key << " at " << key_path << "\n";
    return key;
}

void close_key(HKEY& key) {
    LSTATUS out = RegCloseKey(key);
    if (out != ERROR_SUCCESS) std::cerr << win_api_error_str(out) << " failed to close key " << key << "\n";
}

void setup_override_key(HKEY root_key, uint32_t random_base_path) {
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
void revert_override(HKEY root_key, uint32_t random_base_path) {
    LSTATUS out = RegOverridePredefKey(root_key, NULL);
    if (out != ERROR_SUCCESS) std::cerr << win_api_error_str(out) << " failed to revert override key " << root_key << "\n";

    auto override_path = get_override_path(root_key, random_base_path);
    out = RegDeleteTreeA(HKEY_CURRENT_USER, override_path.c_str());
    if (out != ERROR_SUCCESS) print_error_message(out, "RegDeleteTreeA", std::string("Key") + override_path);
}

KeyWrapper::KeyWrapper(HKEY key) noexcept : key(key) {}
KeyWrapper::KeyWrapper(HKEY key_root, const char* key_path) noexcept { key = create_key(key_root, key_path); }
KeyWrapper::~KeyWrapper() noexcept {
    if (key != NULL) close_key(key);
}
KeyWrapper::KeyWrapper(KeyWrapper&& other) noexcept : key(other.key) { other.key = NULL; };
KeyWrapper& KeyWrapper::operator=(KeyWrapper&& other) noexcept {
    if (this != &other) {
        if (key != NULL) close_key(key);
        key = other.key;
        other.key = NULL;
    }
    return *this;
};

void add_key_value(HKEY const& key, fs::path const& manifest_path, bool enabled = true) {
    DWORD value = enabled ? 0 : 1;
    LSTATUS out = RegSetValueEx(key, manifest_path.c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(value));
    if (out != ERROR_SUCCESS) std::cerr << win_api_error_str(out) << " failed to set key value for " << manifest_path.str() << "\n";
}

void add_key_value_string(HKEY const& key, const char* name, const char* str) {
    LSTATUS out = RegSetValueExA(key, name, 0, REG_SZ, reinterpret_cast<const BYTE*>(str), static_cast<DWORD>(strlen(str)));
    if (out != ERROR_SUCCESS)
        std::cerr << win_api_error_str(out) << " failed to set string value for " << name << ":" << str << "\n";
}

void remove_key_value(HKEY const& key, fs::path const& manifest_path) {
    LSTATUS out = RegDeleteValueA(key, manifest_path.c_str());
    if (out != ERROR_SUCCESS)
        std::cerr << win_api_error_str(out) << " failed to delete key value for " << manifest_path.str() << "\n";
}

void clear_key_values(HKEY const& key) {
    DWORD dwNumValues, dwValueNameLen;

    LSTATUS out = RegQueryInfoKey(key, 0, 0, 0, 0, 0, 0, &dwNumValues, &dwValueNameLen, 0, 0, 0);
    if (out != ERROR_SUCCESS) {
        std::cerr << win_api_error_str(out) << "couldn't query enum " << key << " values\n";
        return;
    }
    std::string tchValName(dwValueNameLen + 1, '\0');
    for (DWORD i = 0; i < dwNumValues; i++) {
        DWORD length = static_cast<DWORD>(tchValName.size());
        LPSTR lpstr = &tchValName[0];
        out = RegEnumValue(key, i, lpstr, &length, 0, 0, 0, 0);
        if (out != ERROR_SUCCESS) {
            std::cerr << win_api_error_str(out) << "couldn't get enum value " << tchValName << "\n";
            return;
        }
    }
}

void PlatformShim::setup_override(DebugMode debug_mode) {
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
    char mbstr[100];
    tm time_buf{};
    localtime_s(&time_buf, &cur_time);
    if (std::strftime(mbstr, sizeof(mbstr), "%A %c", &time_buf)) {
        add_key_value_string(timestamp_key, "Timestamp", mbstr);
    }

    setup_override_key(HKEY_LOCAL_MACHINE, random_base_path);
    setup_override_key(HKEY_CURRENT_USER, random_base_path);
}
void PlatformShim::clear_override(DebugMode debug_mode) {
    if (debug_mode != DebugMode::no_delete) {
        revert_override(HKEY_CURRENT_USER, random_base_path);
        revert_override(HKEY_LOCAL_MACHINE, random_base_path);

        LSTATUS out = RegDeleteKeyA(HKEY_CURRENT_USER, override_base_path(random_base_path).c_str());
        if (out != ERROR_SUCCESS)
            print_error_message(out, "RegDeleteKeyA", std::string("Key") + override_base_path(random_base_path).c_str());
    }
}
void PlatformShim::reset(DebugMode debug_mode) {
    KeyWrapper implicit_key{HKEY_LOCAL_MACHINE, "SOFTWARE\\Khronos\\Vulkan\\ImplicitLayers"};
    KeyWrapper explicit_key{HKEY_LOCAL_MACHINE, "SOFTWARE\\Khronos\\Vulkan\\ExplicitLayers"};
    KeyWrapper icd_key{HKEY_LOCAL_MACHINE, "SOFTWARE\\Khronos\\Vulkan\\Drivers"};
    if (debug_mode != DebugMode::no_delete) {
        clear_key_values(implicit_key);
        clear_key_values(explicit_key);
        clear_key_values(icd_key);
    }
}

void PlatformShim::set_path(ManifestCategory category, fs::path const& path) {}

void PlatformShim::add_manifest(ManifestCategory category, fs::path const& path) {
    std::string reg_path = std::string("SOFTWARE\\Khronos\\Vulkan\\") + category_path_name(category);
    KeyWrapper key{HKEY_LOCAL_MACHINE, reg_path.c_str()};
    add_key_value(key, path);
    if (category == ManifestCategory::icd) {
        icd_paths.push_back(path);
    }
}
void PlatformShim::add_dxgi_adapter(fs::path const& manifest_path, GpuType gpu_preference, uint32_t known_driver_index,
                                    DXGI_ADAPTER_DESC1 desc1) {
    dxgi_adapters.push_back(DXGIAdapter(manifest_path, gpu_preference, known_driver_index, desc1, next_adapter_handle++));
}

void PlatformShim::add_d3dkmt_adapter(SHIM_D3DKMT_ADAPTERINFO adapter, fs::path const& path) {
    d3dkmt_adapters.push_back({adapter, path});
}

void PlatformShim::add_CM_Device_ID(std::wstring const& id, fs::path const& icd_path, fs::path const& layer_path) {
    // append a null byte as separator if there is already id's in the list
    if (CM_device_ID_list.size() != 0) {
        CM_device_ID_list += L'\0';  // I'm sure this wont cause issues with std::string down the line... /s
    }
    CM_device_ID_list += id;
    std::string id_str(id.length(), '\0');
    size_t size_written{};
    wcstombs_s(&size_written, &id_str[0], id_str.length(), id.c_str(), id.length());

    std::string device_path = std::string(pnp_registry_path) + "\\" + id_str;
    CM_device_ID_registry_keys.emplace_back(HKEY_LOCAL_MACHINE, device_path.c_str());
    HKEY id_key = CM_device_ID_registry_keys.back().key;
    add_key_value_string(id_key, "VulkanDriverName", icd_path.c_str());
    add_key_value_string(id_key, "VulkanLayerName", layer_path.c_str());
    // TODO: decide how to handle 32 bit
    // add_key_value_string(id_key, "VulkanDriverNameWoW", icd_path.c_str());
    // add_key_value_string(id_key, "VulkanLayerName", layer_path.c_str());
}

HKEY GetRegistryKey() { return HKEY{}; }

void PlatformShim::redirect_category(fs::path const& new_path, ManifestCategory search_category) {
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

#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)

#include <dirent.h>
#include <unistd.h>

std::string category_path_name(ManifestCategory category) {
    if (category == ManifestCategory::implicit_layer) return "implicit_layer.d";
    if (category == ManifestCategory::explicit_layer)
        return "explicit_layer.d";
    else
        return "icd.d";
}

void PlatformShim::setup_override(DebugMode debug_mode) {}
void PlatformShim::clear_override(DebugMode debug_mode) {}
void PlatformShim::reset(DebugMode debug_mode) { redirection_map.clear(); }

void PlatformShim::redirect_path(fs::path const& path, fs::path const& new_path) { redirection_map[path.str()] = new_path; }
void PlatformShim::remove_redirect(fs::path const& path) { redirection_map.erase(path.str()); }
bool PlatformShim::is_fake_path(fs::path const& path) { return redirection_map.count(path.str()) > 0; }
fs::path const& PlatformShim::get_fake_path(fs::path const& path) { return redirection_map.at(path.str()); }

void PlatformShim::add_manifest(ManifestCategory category, fs::path const& path) {}

void parse_and_add_env_var_override(std::vector<std::string>& paths, std::string env_var_contents) {
    auto parsed_paths = parse_env_var_list(env_var_contents);
    paths.insert(paths.end(), parsed_paths.begin(), parsed_paths.end());
}

void PlatformShim::redirect_category(fs::path const& new_path, ManifestCategory category) {
    std::vector<std::string> paths;
    auto home = fs::path(get_env_var("HOME"));
    if (home.size() != 0) {
        paths.push_back((home / ".config").str());
        paths.push_back((home / ".local/share").str());
    }
    parse_and_add_env_var_override(paths, get_env_var("XDG_CONFIG_DIRS"));
    parse_and_add_env_var_override(paths, get_env_var("XDG_CONFIG_HOME"));
    parse_and_add_env_var_override(paths, get_env_var("XDG_DATA_DIRS"));
    parse_and_add_env_var_override(paths, get_env_var("XDG_DATA_HOME"));
    if (category == ManifestCategory::explicit_layer) {
        parse_and_add_env_var_override(paths, get_env_var("VK_LAYER_PATH", false));  // don't report failure
    }
    parse_and_add_env_var_override(paths, FALLBACK_DATA_DIRS);
    parse_and_add_env_var_override(paths, FALLBACK_CONFIG_DIRS);

    auto sys_conf_dir = std::string(SYSCONFDIR);
    if (!sys_conf_dir.empty()) {
        paths.push_back(sys_conf_dir);
    }
#if defined(EXTRASYSCONFDIR)
    // EXTRASYSCONFDIR default is /etc, if SYSCONFDIR wasn't defined, it will have /etc put
    // as its default. Don't want to double add it
    auto extra_sys_conf_dir = std::string(EXTRASYSCONFDIR);
    if (!extra_sys_conf_dir.empty() && sys_conf_dir != extra_sys_conf_dir) {
        paths.push_back(extra_sys_conf_dir);
    }
#endif

    for (auto& path : paths) {
        if (!path.empty()) {
            redirect_path(fs::path(path) / "vulkan" / category_path_name(category), new_path);
        }
    }
}

void PlatformShim::set_path(ManifestCategory category, fs::path const& path) {
    // use /etc as the 'redirection path' by default since its always searched
    redirect_path(fs::path(SYSCONFDIR) / "vulkan" / category_path_name(category), path);
}

#endif