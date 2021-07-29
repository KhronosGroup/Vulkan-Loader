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

#include "test_util.h"

#if defined(WIN32)
#include <strsafe.h>
const char* win_api_error_str(LSTATUS status) {
    if (status == ERROR_INVALID_FUNCTION) return "ERROR_INVALID_FUNCTION";
    if (status == ERROR_FILE_NOT_FOUND) return "ERROR_FILE_NOT_FOUND";
    if (status == ERROR_PATH_NOT_FOUND) return "ERROR_PATH_NOT_FOUND";
    if (status == ERROR_TOO_MANY_OPEN_FILES) return "ERROR_TOO_MANY_OPEN_FILES";
    if (status == ERROR_ACCESS_DENIED) return "ERROR_ACCESS_DENIED";
    if (status == ERROR_INVALID_HANDLE) return "ERROR_INVALID_HANDLE";
    if (status == ERROR_ENVVAR_NOT_FOUND) return "ERROR_ENVVAR_NOT_FOUND";
    if (status == ERROR_SETENV_FAILED) return "ERROR_SETENV_FAILED";
    return "UNKNOWN ERROR";
}

void print_error_message(LSTATUS status, const char* function_name, std::string optional_message) {
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError();

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

    std::cerr << function_name << " failed with " << win_api_error_str(status) << ": "
              << std::string(static_cast<LPTSTR>(lpMsgBuf));
    if (optional_message != "") {
        std::cerr << " | " << optional_message;
    }
    std::cerr << "\n";
    LocalFree(lpMsgBuf);
}

bool set_env_var(std::string const& name, std::string const& value) {
    bool ret = SetEnvironmentVariableA(name.c_str(), value.c_str());
    if (ret == false) {
        print_error_message(ERROR_SETENV_FAILED, "SetEnvironmentVariableA");
        return true;
    }
    return false;
}
bool remove_env_var(std::string const& name) { return SetEnvironmentVariableA(name.c_str(), nullptr); }
#define ENV_VAR_BUFFER_SIZE 4096
std::string get_env_var(std::string const& name) {
    std::string value;
    value.resize(ENV_VAR_BUFFER_SIZE);
    DWORD ret = GetEnvironmentVariable(name.c_str(), (LPSTR)value.c_str(), ENV_VAR_BUFFER_SIZE);
    if (0 == ret) {
        print_error_message(ERROR_ENVVAR_NOT_FOUND, "GetEnvironmentVariable");
        return std::string();
    } else if (ENV_VAR_BUFFER_SIZE < ret) {
        std::cerr << "Not enough space to write environment variable" << name << "\n";
        return std::string();
    } else {
        value.resize(ret);
    }
    return value;
}
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)

bool set_env_var(std::string const& name, std::string const& value) { return setenv(name.c_str(), value.c_str(), 1); }
bool remove_env_var(std::string const& name) { return unsetenv(name.c_str()); }
std::string get_env_var(std::string const& name, bool report_failure) {
    char* ret = getenv(name.c_str());
    if (ret == nullptr) {
        if (report_failure) std::cerr << "Failed to get environment variable:" << name << "\n";
        return std::string();
    }
    return ret;
}
#endif

std::string ManifestICD::get_manifest_str() const {
    std::string out;
    out += "{\n";
    out += "    " + file_format_version.get_version_str() + "\n";
    out += "    \"ICD\": {\n";
    out += "        \"library_path\": \"" + fs::fixup_backslashes_in_path(lib_path) + "\",\n";
    out += "        \"api_version\": \"" + version_to_string(api_version) + "\"\n";
    out += "    }\n";
    out += "}\n";
    return out;
}

std::string ManifestLayer::LayerDescription::Extension::get_manifest_str() const {
    std::string out;
    out += "{ \"name\":\"" + name + "\",\n\t\t\t\"spec_version\":\"" + std::to_string(spec_version) + "\"";
    if (entrypoints.size() > 0) {
        out += ",\n\t\t\t\"entrypoints\": [";
        for (size_t i = 0; i < entrypoints.size(); i++) {
            if (i > 0) out += ", ";
            out += "\"" + entrypoints.at(i) + "\"";
        }
        out += "]";
    }
    out += "\t\t\t}";
    return out;
}

std::string ManifestLayer::LayerDescription::get_manifest_str() const {
    std::string out;
    out += "\t{\n";
    out += "\t\t\"name\":\"" + name + "\",\n";
    out += "\t\t\"type\":\"" + get_type_str(type) + "\",\n";
    if (lib_path.size() > 0) {
        out += "\t\t\"library_path\": \"" + fs::fixup_backslashes_in_path(lib_path.str()) + "\",\n";
    }
    out += "\t\t\"api_version\": \"" + version_to_string(api_version) + "\",\n";
    out += "\t\t\"implementation_version\":\"" + std::to_string(implementation_version) + "\",\n";
    out += "\t\t\"description\": \"" + description + "\"";
    if (functions.size() > 0) {
        out += ",\n\t\t\"functions\": {";
        for (size_t i = 0; i < functions.size(); i++) {
            if (i > 0) out += ",";
            out += "\n\t\t\t" + functions.at(i).get_manifest_str();
        }
        out += "\n\t\t}";
    }
    if (instance_extensions.size() > 0) {
        out += ",\n\t\t\"instance_extensions\": [";
        for (size_t i = 0; i < instance_extensions.size(); i++) {
            if (i > 0) out += ",";
            out += "\n\t\t\t" + instance_extensions.at(i).get_manifest_str();
        }
        out += "\n\t\t]";
    }
    if (device_extensions.size() > 0) {
        out += ",\n\t\t\"device_extensions\": [";
        for (size_t i = 0; i < device_extensions.size(); i++) {
            if (i > 0) out += ",";
            out += "\n\t\t\t" + device_extensions.at(i).get_manifest_str();
        }
        out += "\n\t\t]";
    }
    if (enable_environment.size() > 0) {
        out += ",\n\t\t\"enable_environment\": { \"" + enable_environment + "\": \"1\"";
        out += "\n\t\t}";
    }
    if (disable_environment.size() > 0) {
        out += ",\n\t\t\"disable_environment\": { \"" + disable_environment + "\": \"1\"";
        out += "\n\t\t}";
    }
    if (component_layers.size() > 0) {
        out += ",\n\t\t\"component_layers\": [";
        for (size_t i = 0; i < component_layers.size(); i++) {
            if (i > 0) out += ", ";
            out += "\"" + component_layers.at(i) + "\"";
        }
        out += "]\n";
    }
    if (pre_instance_functions.size() > 0) {
        out += ",\n\t\t\"pre_instance_functions\": [";
        for (size_t i = 0; i < pre_instance_functions.size(); i++) {
            if (i > 0) out += ", ";
            out += "\"" + pre_instance_functions.at(i) + "\"";
        }
        out += "]\n\t\t}";
    }
    out += "\n\t}";
    return out;
}

VkLayerProperties ManifestLayer::LayerDescription::get_layer_properties() const {
    VkLayerProperties properties{};
    copy_string_to_char_array(name, properties.layerName, VK_MAX_EXTENSION_NAME_SIZE);
    copy_string_to_char_array(description, properties.description, VK_MAX_EXTENSION_NAME_SIZE);
    properties.implementationVersion = implementation_version;
    properties.specVersion = api_version;
    return properties;
}

std::string ManifestLayer::get_manifest_str() const {
    std::string out;
    out += "{\n";
    out += "\t" + file_format_version.get_version_str() + "\n";
    if (layers.size() == 1) {
        out += "\t\"layer\": ";
        out += layers.at(0).get_manifest_str() + "\n";
    } else {
        out += "\"\tlayers\": [";
        for (size_t i = 0; i < layers.size(); i++) {
            if (i > 0) out += ",";
            out += "\n" + layers.at(0).get_manifest_str();
        }
        out += "\n]";
    }
    out += "}\n";
    return out;
}

namespace fs {
std::string make_native(std::string const& in_path) {
    std::string out;
#if defined(WIN32)
    for (auto& c : in_path) {
        if (c == '/')
            out += "\\";
        else
            out += c;
    }
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
    for (size_t i = 0; i < in_path.size(); i++) {
        if (i + 1 < in_path.size() && in_path[i] == '\\' && in_path[i + 1] == '\\') {
            out += '/';
            i++;
        } else
            out += in_path[i];
    }
#endif
    return out;
}

// Json doesn't allow `\` in strings, it must be escaped. Thus we have to convert '\\' to '\\\\' in strings
std::string fixup_backslashes_in_path(std::string const& in_path) {
    std::string out;
    for (auto& c : in_path) {
        if (c == '\\')
            out += "\\\\";
        else
            out += c;
    }
    return out;
}

path& path::operator+=(path const& in) {
    contents += in.contents;
    return *this;
}
path& path::operator+=(std::string const& in) {
    contents += in;
    return *this;
}
path& path::operator+=(const char* in) {
    contents += std::string{in};
    return *this;
}
path& path::operator/=(path const& in) {
    if (contents.back() != path_separator && in.contents.front() != path_separator) contents += path_separator;
    contents += in.contents;
    return *this;
}
path& path::operator/=(std::string const& in) {
    if (contents.back() != path_separator && in.front() != path_separator) contents += path_separator;
    contents += in;
    return *this;
}
path& path::operator/=(const char* in) {
    std::string in_str{in};
    if (contents.back() != path_separator && in_str.front() != path_separator) contents += path_separator;
    contents += in_str;
    return *this;
}
path path::operator+(path const& in) const {
    path new_path = contents;
    new_path += in;
    return new_path;
}
path path::operator+(std::string const& in) const {
    path new_path = contents;
    new_path += in;
    return new_path;
}
path path::operator+(const char* in) const {
    path new_path(contents);
    new_path += in;
    return new_path;
}

path path::operator/(path const& in) const {
    path new_path = contents;
    new_path /= in;
    return new_path;
}
path path::operator/(std::string const& in) const {
    path new_path = contents;
    new_path /= in;
    return new_path;
}
path path::operator/(const char* in) const {
    path new_path(contents);
    new_path /= in;
    return new_path;
}

path path::parent_path() const {
    auto last_div = contents.rfind(path_separator);
    if (last_div == std::string::npos) return "";
    return path(contents.substr(0, last_div));
}
bool path::has_parent_path() const {
    auto last_div = contents.rfind(path_separator);
    return last_div != std::string::npos;
}
path path::filename() const {
    auto last_div = contents.rfind(path_separator);
    return path(contents.substr(last_div + 1, contents.size() - last_div + 1));
}

path path::extension() const {
    auto last_div = contents.rfind(path_separator);
    auto ext_div = contents.rfind('.');
    // Make sure to not get the special `.` and `..`, as well as any filename that being with a dot, like .profile
    if (last_div + 1 == ext_div || (last_div + 2 == ext_div && contents[last_div + 1] == '.')) return path("");
    path temp = path(contents.substr(ext_div, contents.size() - ext_div + 1));

    return path(contents.substr(ext_div, contents.size() - ext_div + 1));
}

path path::stem() const {
    auto last_div = contents.rfind(path_separator);
    auto ext_div = contents.rfind('.');
    if (last_div + 1 == ext_div || (last_div + 2 == ext_div && contents[last_div + 1] == '.')) {
        return path(contents.substr(last_div + 1, contents.size() - last_div + 1));
    }
    return path(contents.substr(last_div + 1, ext_div - last_div - 1));
}

path& path::replace_filename(path const& replacement) {
    *this = parent_path() / replacement.str();
    return *this;
}

// internal implementation helper for per-platform creating & destroying folders
int create_folder(path const& path) {
#if defined(WIN32)
    return _mkdir(path.c_str());
#else
    mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    return 0;
#endif
}

int delete_folder(path const& folder) {
#if defined(WIN32)
    if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(folder.c_str()) && GetLastError() == ERROR_FILE_NOT_FOUND) {
        // nothing to delete
        return 0;
    }
    bool ret = RemoveDirectoryA(folder.c_str());
    if (ret == 0) {
        print_error_message(ERROR_REMOVEDIRECTORY_FAILED, "RemoveDirectoryA");
    }
    return ret;
#else
    DIR* dir = opendir(folder.c_str());
    if (!dir) {
        return 0;
    }
    int ret = 0;
    dirent* file;
    while (!ret && (file = readdir(dir))) {
        int ret2 = -1;

        /* Skip the names "." and ".." as we don't want to recurse on them. */
        if (string_eq(file->d_name, ".") || string_eq(file->d_name, "..")) continue;

        path file_path = folder / file->d_name;
        struct stat statbuf;
        if (!stat(file_path.c_str(), &statbuf)) {
            if (S_ISDIR(statbuf.st_mode))
                ret2 = delete_folder(file_path);
            else
                ret2 = unlink(file_path.c_str());
        }

        ret = ret2;
    }
    closedir(dir);

    if (!ret) ret = rmdir(folder.c_str());
    return ret;
#endif
}

FolderManager::FolderManager(path root_path, std::string name, DebugMode debug) : debug(debug), folder(root_path / name) {
    create_folder(folder);
}
FolderManager::~FolderManager() {
    for (auto& file : files) {
        if (debug >= DebugMode::log) std::cout << "Removing manifest " << file << " at " << (folder / file).str() << "\n";
        if (debug != DebugMode::no_delete) {
            std::remove((folder / file).c_str());
        }
    }
    if (debug != DebugMode::no_delete) {
        delete_folder(folder);
    }
    if (debug >= DebugMode::log) {
        std::cout << "Deleting folder " << folder.str() << "\n";
    }
}
path FolderManager::write(std::string const& name, ManifestICD const& icd_manifest) {
    path out_path = folder / name;
    auto found = std::find(files.begin(), files.end(), name);
    if (found != files.end()) {
        if (debug >= DebugMode::log) std::cout << "Writing icd manifest to " << name << "\n";
    } else {
        if (debug >= DebugMode::log) std::cout << "Creating icd manifest " << name << " at " << out_path.str() << "\n";
        files.emplace_back(name);
    }
    auto file = std::ofstream(out_path.str(), std::ios_base::trunc | std::ios_base::out);
    if (!file) {
        std::cerr << "Failed to create icd manifest " << name << " at " << out_path.str() << "\n";
        return out_path;
    }
    file << icd_manifest.get_manifest_str() << std::endl;
    return out_path;
}
path FolderManager::write(std::string const& name, ManifestLayer const& layer_manifest) {
    path out_path = folder / name;
    auto found = std::find(files.begin(), files.end(), name);
    if (found != files.end()) {
        if (debug >= DebugMode::log) std::cout << "Writing layer manifest to " << name << "\n";
    } else {
        if (debug >= DebugMode::log) std::cout << "Creating layer manifest " << name << " at " << out_path.str() << "\n";
        files.emplace_back(name);
    }
    auto file = std::ofstream(out_path.str(), std::ios_base::trunc | std::ios_base::out);
    file << layer_manifest.get_manifest_str() << std::endl;
    if (!file) {
        std::cerr << "Failed to create icd manifest " << name << " at " << out_path.str() << "\n";
        return out_path;
    }
    return out_path;
}
// close file handle, delete file, remove `name` from managed file list.
void FolderManager::remove(std::string const& name) {
    path out_path = folder / name;
    auto found = std::find(files.begin(), files.end(), name);
    if (found != files.end()) {
        if (debug >= DebugMode::log) std::cout << "Removing manifest " << name << " at " << out_path.str() << "\n";
        if (debug != DebugMode::no_delete) {
            std::remove(out_path.c_str());
            files.erase(found);
        }
    } else {
        if (debug >= DebugMode::log) std::cout << "Couldn't remove manifest " << name << " at " << out_path.str() << ".\n";
    }
}

// copy file into this folder
path FolderManager::copy_file(path const& file, std::string const& new_name) {
    auto new_filepath = folder / new_name;
    auto found = std::find(files.begin(), files.end(), new_name);
    if (found != files.end()) {
        if (debug >= DebugMode::log) std::cout << "File location already contains" << new_name << ". Is this a bug?\n";
    } else {
        if (debug >= DebugMode::log) std::cout << "Copying file" << file.str() << " to " << new_filepath.str() << "\n";
        files.emplace_back(new_name);
    }
    std::ifstream src(file.str(), std::ios::binary);
    if (!src) {
        std::cerr << "Failed to create file " << file.str() << " for copying from\n";
        return new_filepath;
    }
    std::ofstream dst(new_filepath.str(), std::ios::binary);
    if (!dst) {
        std::cerr << "Failed to create file " << file.str() << " for copying to\n";
        return new_filepath;
    }
    dst << src.rdbuf();
    return new_filepath;
}
}  // namespace fs

bool string_eq(const char* a, const char* b) noexcept { return strcmp(a, b) == 0; }
bool string_eq(const char* a, const char* b, size_t len) noexcept { return strncmp(a, b, len) == 0; }

VulkanFunctions::VulkanFunctions() : loader(FRAMEWORK_VULKAN_LIBRARY_PATH) {
    // clang-format off
    vkGetInstanceProcAddr = loader.get_symbol<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    vkEnumerateInstanceExtensionProperties = loader.get_symbol<PFN_vkEnumerateInstanceExtensionProperties>("vkEnumerateInstanceExtensionProperties");
    vkEnumerateInstanceLayerProperties = loader.get_symbol<PFN_vkEnumerateInstanceLayerProperties>("vkEnumerateInstanceLayerProperties");
    vkEnumerateInstanceVersion = loader.get_symbol<PFN_vkEnumerateInstanceVersion>("vkEnumerateInstanceVersion");
    vkCreateInstance = loader.get_symbol<PFN_vkCreateInstance>("vkCreateInstance");
    vkDestroyInstance = loader.get_symbol<PFN_vkDestroyInstance>("vkDestroyInstance");
    vkEnumeratePhysicalDevices = loader.get_symbol<PFN_vkEnumeratePhysicalDevices>("vkEnumeratePhysicalDevices");
    vkEnumeratePhysicalDeviceGroups = loader.get_symbol<PFN_vkEnumeratePhysicalDeviceGroups>("vkEnumeratePhysicalDeviceGroups");
    vkGetPhysicalDeviceFeatures = loader.get_symbol<PFN_vkGetPhysicalDeviceFeatures>("vkGetPhysicalDeviceFeatures");
    vkGetPhysicalDeviceFeatures2 = loader.get_symbol<PFN_vkGetPhysicalDeviceFeatures2>("vkGetPhysicalDeviceFeatures2");
    vkGetPhysicalDeviceFormatProperties = loader.get_symbol<PFN_vkGetPhysicalDeviceFormatProperties>("vkGetPhysicalDeviceFormatProperties");
    vkGetPhysicalDeviceFormatProperties2 = loader.get_symbol<PFN_vkGetPhysicalDeviceFormatProperties2>("vkGetPhysicalDeviceFormatProperties2");
    vkGetPhysicalDeviceImageFormatProperties = loader.get_symbol<PFN_vkGetPhysicalDeviceImageFormatProperties>("vkGetPhysicalDeviceImageFormatProperties");
    vkGetPhysicalDeviceImageFormatProperties2 = loader.get_symbol<PFN_vkGetPhysicalDeviceImageFormatProperties2>("vkGetPhysicalDeviceImageFormatProperties2");
    vkGetPhysicalDeviceSparseImageFormatProperties = loader.get_symbol<PFN_vkGetPhysicalDeviceSparseImageFormatProperties>("vkGetPhysicalDeviceSparseImageFormatProperties");
    vkGetPhysicalDeviceSparseImageFormatProperties2 = loader.get_symbol<PFN_vkGetPhysicalDeviceSparseImageFormatProperties2>("vkGetPhysicalDeviceSparseImageFormatProperties2");
    vkGetPhysicalDeviceProperties = loader.get_symbol<PFN_vkGetPhysicalDeviceProperties>("vkGetPhysicalDeviceProperties");
    vkGetPhysicalDeviceProperties2 = loader.get_symbol<PFN_vkGetPhysicalDeviceProperties2>("vkGetPhysicalDeviceProperties2");
    vkGetPhysicalDeviceQueueFamilyProperties = loader.get_symbol<PFN_vkGetPhysicalDeviceQueueFamilyProperties>("vkGetPhysicalDeviceQueueFamilyProperties");
    vkGetPhysicalDeviceQueueFamilyProperties2 = loader.get_symbol<PFN_vkGetPhysicalDeviceQueueFamilyProperties2>("vkGetPhysicalDeviceQueueFamilyProperties2");
    vkGetPhysicalDeviceMemoryProperties = loader.get_symbol<PFN_vkGetPhysicalDeviceMemoryProperties>("vkGetPhysicalDeviceMemoryProperties");
    vkGetPhysicalDeviceMemoryProperties2 = loader.get_symbol<PFN_vkGetPhysicalDeviceMemoryProperties2>("vkGetPhysicalDeviceMemoryProperties2");
    vkGetPhysicalDeviceSurfaceSupportKHR = loader.get_symbol<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>("vkGetPhysicalDeviceSurfaceSupportKHR");
    vkGetPhysicalDeviceSurfaceFormatsKHR = loader.get_symbol<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>("vkGetPhysicalDeviceSurfaceFormatsKHR");
    vkGetPhysicalDeviceSurfacePresentModesKHR = loader.get_symbol<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>("vkGetPhysicalDeviceSurfacePresentModesKHR");
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR = loader.get_symbol<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>("vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    vkEnumerateDeviceExtensionProperties = loader.get_symbol<PFN_vkEnumerateDeviceExtensionProperties>("vkEnumerateDeviceExtensionProperties");
    vkEnumerateDeviceLayerProperties = loader.get_symbol<PFN_vkEnumerateDeviceLayerProperties>("vkEnumerateDeviceLayerProperties");
    vkGetPhysicalDeviceExternalBufferProperties = loader.get_symbol<PFN_vkGetPhysicalDeviceExternalBufferProperties>("vkGetPhysicalDeviceExternalBufferProperties");
    vkGetPhysicalDeviceExternalFenceProperties = loader.get_symbol<PFN_vkGetPhysicalDeviceExternalFenceProperties>("vkGetPhysicalDeviceExternalFenceProperties");
    vkGetPhysicalDeviceExternalSemaphoreProperties = loader.get_symbol<PFN_vkGetPhysicalDeviceExternalSemaphoreProperties>("vkGetPhysicalDeviceExternalSemaphoreProperties");

    vkDestroySurfaceKHR = loader.get_symbol<PFN_vkDestroySurfaceKHR>("vkDestroySurfaceKHR");
    vkGetDeviceProcAddr = loader.get_symbol<PFN_vkGetDeviceProcAddr>("vkGetDeviceProcAddr");
    vkCreateDevice = loader.get_symbol<PFN_vkCreateDevice>("vkCreateDevice");

    vkCreateHeadlessSurfaceEXT = loader.get_symbol<PFN_vkCreateHeadlessSurfaceEXT>("vkCreateHeadlessSurfaceEXT");
    vkCreateDisplayPlaneSurfaceKHR = loader.get_symbol<PFN_vkCreateDisplayPlaneSurfaceKHR>("vkCreateDisplayPlaneSurfaceKHR");
    vkGetPhysicalDeviceDisplayPropertiesKHR = loader.get_symbol<PFN_vkGetPhysicalDeviceDisplayPropertiesKHR>("vkGetPhysicalDeviceDisplayPropertiesKHR");
    vkGetPhysicalDeviceDisplayPlanePropertiesKHR = loader.get_symbol<PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR>("vkGetPhysicalDeviceDisplayPlanePropertiesKHR");
    vkGetDisplayPlaneSupportedDisplaysKHR = loader.get_symbol<PFN_vkGetDisplayPlaneSupportedDisplaysKHR>("vkGetDisplayPlaneSupportedDisplaysKHR");
    vkGetDisplayModePropertiesKHR = loader.get_symbol<PFN_vkGetDisplayModePropertiesKHR>("vkGetDisplayModePropertiesKHR");
    vkCreateDisplayModeKHR = loader.get_symbol<PFN_vkCreateDisplayModeKHR>("vkCreateDisplayModeKHR");
    vkGetDisplayPlaneCapabilitiesKHR = loader.get_symbol<PFN_vkGetDisplayPlaneCapabilitiesKHR>("vkGetDisplayPlaneCapabilitiesKHR");
    vkGetPhysicalDevicePresentRectanglesKHR = loader.get_symbol<PFN_vkGetPhysicalDevicePresentRectanglesKHR>("vkGetPhysicalDevicePresentRectanglesKHR");
    vkGetPhysicalDeviceDisplayProperties2KHR = loader.get_symbol<PFN_vkGetPhysicalDeviceDisplayProperties2KHR>("vkGetPhysicalDeviceDisplayProperties2KHR");
    vkGetPhysicalDeviceDisplayPlaneProperties2KHR = loader.get_symbol<PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR>("vkGetPhysicalDeviceDisplayPlaneProperties2KHR");
    vkGetDisplayModeProperties2KHR = loader.get_symbol<PFN_vkGetDisplayModeProperties2KHR>("vkGetDisplayModeProperties2KHR");
    vkGetDisplayPlaneCapabilities2KHR = loader.get_symbol<PFN_vkGetDisplayPlaneCapabilities2KHR>("vkGetDisplayPlaneCapabilities2KHR");
    vkGetPhysicalDeviceSurfaceCapabilities2KHR = loader.get_symbol<PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR>("vkGetPhysicalDeviceSurfaceCapabilities2KHR");
    vkGetPhysicalDeviceSurfaceFormats2KHR = loader.get_symbol<PFN_vkGetPhysicalDeviceSurfaceFormats2KHR>("vkGetPhysicalDeviceSurfaceFormats2KHR");

#ifdef VK_USE_PLATFORM_ANDROID_KHR
    vkCreateAndroidSurfaceKHR = loader.get_symbol<PFN_vkCreateAndroidSurfaceKHR>("vkCreateAndroidSurfaceKHR");
#endif  // VK_USE_PLATFORM_ANDROID_KHR
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
    vkCreateDirectFBSurfaceEXT = loader.get_symbol<PFN_vkCreateDirectFBSurfaceEXT>("vkCreateDirectFBSurfaceEXT");
    vkGetPhysicalDeviceDirectFBPresentationSupportEXT = loader.get_symbol<PFN_vkGetPhysicalDeviceDirectFBPresentationSupportEXT>("vkGetPhysicalDeviceDirectFBPresentationSupportEXT");
#endif  // VK_USE_PLATFORM_DIRECTFB_EXT
#ifdef VK_USE_PLATFORM_FUCHSIA
    vkCreateImagePipeSurfaceFUCHSIA = loader.get_symbol<PFN_vkCreateImagePipeSurfaceFUCHSIA>("vkCreateImagePipeSurfaceFUCHSIA");
#endif  // VK_USE_PLATFORM_FUCHSIA
#ifdef VK_USE_PLATFORM_GGP
    vkCreateStreamDescriptorSurfaceGGP = loader.get_symbol<PFN_vkCreateStreamDescriptorSurfaceGGP>("vkCreateStreamDescriptorSurfaceGGP");
#endif  // VK_USE_PLATFORM_GGP
#ifdef VK_USE_PLATFORM_IOS_MVK
    vkCreateIOSSurfaceMVK = loader.get_symbol<PFN_vkCreateIOSSurfaceMVK>("vkCreateIOSSurfaceMVK");
#endif  // VK_USE_PLATFORM_IOS_MVK
#ifdef VK_USE_PLATFORM_MACOS_MVK
    vkCreateMacOSSurfaceMVK = loader.get_symbol<PFN_vkCreateMacOSSurfaceMVK>("vkCreateMacOSSurfaceMVK");
#endif  // VK_USE_PLATFORM_MACOS_MVK
#ifdef VK_USE_PLATFORM_METAL_EXT
    vkCreateMetalSurfaceEXT = loader.get_symbol<PFN_vkCreateMetalSurfaceEXT>("vkCreateMetalSurfaceEXT")
#endif  // VK_USE_PLATFORM_METAL_EXT
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    vkCreateScreenSurfaceQNX = loader.get_symbol<PFN_vkCreateScreenSurfaceQNX>("vkCreateScreenSurfaceQNX");
    vkGetPhysicalDeviceScreenPresentationSupportQNX = loader.get_symbol<PFN_vkGetPhysicalDeviceScreenPresentationSupportQNX>("vkGetPhysicalDeviceScreenPresentationSupportQNX");
#endif  // VK_USE_PLATFORM_SCREEN_QNX
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    vkCreateWaylandSurfaceKHR = loader.get_symbol<PFN_vkCreateWaylandSurfaceKHR>("vkCreateWaylandSurfaceKHR");
    vkGetPhysicalDeviceWaylandPresentationSupportKHR = loader.get_symbol<PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR>("vkGetPhysicalDeviceWaylandPresentationSupportKHR");
#endif  // VK_USE_PLATFORM_WAYLAND_KHR
#ifdef VK_USE_PLATFORM_XCB_KHR
    vkCreateXcbSurfaceKHR = loader.get_symbol<PFN_vkCreateXcbSurfaceKHR>("vkCreateXcbSurfaceKHR");
    vkGetPhysicalDeviceXcbPresentationSupportKHR = loader.get_symbol<PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR>("vkGetPhysicalDeviceXcbPresentationSupportKHR");
#endif  // VK_USE_PLATFORM_XCB_KHR
#ifdef VK_USE_PLATFORM_XLIB_KHR
    vkCreateXlibSurfaceKHR = loader.get_symbol<PFN_vkCreateXlibSurfaceKHR>("vkCreateXlibSurfaceKHR");
    vkGetPhysicalDeviceXlibPresentationSupportKHR = loader.get_symbol<PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR>("vkGetPhysicalDeviceXlibPresentationSupportKHR");
#endif  // VK_USE_PLATFORM_XLIB_KHR
#ifdef VK_USE_PLATFORM_WIN32_KHR
    vkCreateWin32SurfaceKHR = loader.get_symbol<PFN_vkCreateWin32SurfaceKHR>("vkCreateWin32SurfaceKHR");
    vkGetPhysicalDeviceWin32PresentationSupportKHR = loader.get_symbol<PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR>("vkGetPhysicalDeviceWin32PresentationSupportKHR");
#endif  // VK_USE_PLATFORM_WIN32_KHR

    vkDestroyDevice = loader.get_symbol<PFN_vkDestroyDevice>("vkDestroyDevice");
    vkGetDeviceQueue = loader.get_symbol<PFN_vkGetDeviceQueue>("vkGetDeviceQueue");

    // clang-format on
}

InstanceCreateInfo::InstanceCreateInfo() {
    inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
}

VkInstanceCreateInfo* InstanceCreateInfo::get() noexcept {
    app_info.pApplicationName = app_name.c_str();
    app_info.pEngineName = engine_name.c_str();
    app_info.applicationVersion = app_version;
    app_info.engineVersion = engine_version;
    app_info.apiVersion = api_version;
    inst_info.pApplicationInfo = &app_info;
    inst_info.enabledLayerCount = static_cast<uint32_t>(enabled_layers.size());
    inst_info.ppEnabledLayerNames = enabled_layers.data();
    inst_info.enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size());
    inst_info.ppEnabledExtensionNames = enabled_extensions.data();
    return &inst_info;
}
InstanceCreateInfo& InstanceCreateInfo::set_application_name(std::string app_name) {
    this->app_name = app_name;
    return *this;
}
InstanceCreateInfo& InstanceCreateInfo::set_engine_name(std::string engine_name) {
    this->engine_name = engine_name;
    return *this;
}
InstanceCreateInfo& InstanceCreateInfo::set_app_version(uint32_t app_version) {
    this->app_version = app_version;
    return *this;
}
InstanceCreateInfo& InstanceCreateInfo::set_engine_version(uint32_t engine_version) {
    this->engine_version = engine_version;
    return *this;
}
InstanceCreateInfo& InstanceCreateInfo::set_api_version(uint32_t api_version) {
    this->api_version = api_version;
    return *this;
}
InstanceCreateInfo& InstanceCreateInfo::set_api_version(uint32_t major, uint32_t minor, uint32_t patch) {
    this->api_version = VK_MAKE_API_VERSION(0, major, minor, patch);
    return *this;
}
InstanceCreateInfo& InstanceCreateInfo::add_layer(const char* layer_name) {
    enabled_layers.push_back(layer_name);
    return *this;
}
InstanceCreateInfo& InstanceCreateInfo::add_extension(const char* ext_name) {
    enabled_extensions.push_back(ext_name);
    return *this;
}

DeviceQueueCreateInfo::DeviceQueueCreateInfo() { queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO; }
DeviceQueueCreateInfo& DeviceQueueCreateInfo::add_priority(float priority) {
    priorities.push_back(priority);
    return *this;
}
DeviceQueueCreateInfo& DeviceQueueCreateInfo::set_props(VkQueueFamilyProperties props) {
    queue.queueCount = props.queueCount;
    return *this;
}

VkDeviceCreateInfo* DeviceCreateInfo::get() noexcept {
    dev.enabledLayerCount = static_cast<uint32_t>(enabled_layers.size());
    dev.ppEnabledLayerNames = enabled_layers.data();
    dev.enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size());
    dev.ppEnabledExtensionNames = enabled_extensions.data();
    uint32_t index = 0;
    for (auto& queue : queue_infos) queue.queueFamilyIndex = index++;

    dev.queueCreateInfoCount = static_cast<uint32_t>(queue_infos.size());
    dev.pQueueCreateInfos = queue_infos.data();
    return &dev;
}
DeviceCreateInfo& DeviceCreateInfo::add_layer(const char* layer_name) {
    enabled_layers.push_back(layer_name);

    return *this;
}
DeviceCreateInfo& DeviceCreateInfo::add_extension(const char* ext_name) {
    enabled_extensions.push_back(ext_name);

    return *this;
}
DeviceCreateInfo& DeviceCreateInfo::add_device_queue(DeviceQueueCreateInfo queue_info_detail) {
    queue_info_details.push_back(queue_info_detail);
    return *this;
}
