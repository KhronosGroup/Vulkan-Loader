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

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, dw,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&lpMsgBuf), 0, nullptr);

    std::cerr << function_name << " failed with " << win_api_error_str(status) << ": "
              << std::string(reinterpret_cast<LPTSTR>(lpMsgBuf));
    if (optional_message != "") {
        std::cerr << " | " << optional_message;
    }
    std::cerr << "\n";
    LocalFree(lpMsgBuf);
}

void set_env_var(std::string const& name, std::string const& value) {
    BOOL ret = SetEnvironmentVariableW(widen(name).c_str(), widen(value).c_str());
    if (ret == 0) {
        print_error_message(ERROR_SETENV_FAILED, "SetEnvironmentVariableW");
    }
}
void remove_env_var(std::string const& name) { SetEnvironmentVariableW(widen(name).c_str(), nullptr); }
std::string get_env_var(std::string const& name, bool report_failure) {
    std::wstring name_utf16 = widen(name);
    DWORD value_size = GetEnvironmentVariableW(name_utf16.c_str(), nullptr, 0);
    if (0 == value_size) {
        if (report_failure) print_error_message(ERROR_ENVVAR_NOT_FOUND, "GetEnvironmentVariableW");
        return {};
    }
    std::wstring value(value_size, L'\0');
    if (GetEnvironmentVariableW(name_utf16.c_str(), &value[0], value_size) != value_size - 1) {
        return {};
    }
    return narrow(value);
}
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__)

void set_env_var(std::string const& name, std::string const& value) { setenv(name.c_str(), value.c_str(), 1); }
void remove_env_var(std::string const& name) { unsetenv(name.c_str()); }
std::string get_env_var(std::string const& name, bool report_failure) {
    char* ret = getenv(name.c_str());
    if (ret == nullptr) {
        if (report_failure) std::cerr << "Failed to get environment variable:" << name << "\n";
        return std::string();
    }
    return ret;
}
#endif

template <typename T>
void print_list_of_t(std::string& out, const char* object_name, std::vector<T> const& vec) {
    if (vec.size() > 0) {
        out += std::string(",\n\t\t\"") + object_name + "\": {";
        for (size_t i = 0; i < vec.size(); i++) {
            if (i > 0) out += ",\t\t\t";
            out += "\n\t\t\t" + vec.at(i).get_manifest_str();
        }
        out += "\n\t\t}";
    }
}

template <typename T>
void print_vector_of_t(std::string& out, const char* object_name, std::vector<T> const& vec) {
    if (vec.size() > 0) {
        out += std::string(",\n\t\t\"") + object_name + "\": [";
        for (size_t i = 0; i < vec.size(); i++) {
            if (i > 0) out += ",\t\t\t";
            out += "\n\t\t\t" + vec.at(i).get_manifest_str();
        }
        out += "\n\t\t]";
    }
}
void print_vector_of_strings(std::string& out, const char* object_name, std::vector<std::string> const& strings) {
    if (strings.size() > 0) {
        out += std::string(",\n\t\t\"") + object_name + "\": [";
        for (size_t i = 0; i < strings.size(); i++) {
            if (i > 0) out += ",\t\t\t";
            out += "\"" + fs::fixup_backslashes_in_path(strings.at(i)) + "\"";
        }
        out += "]";
    }
}

std::string to_text(bool b) { return b ? std::string("true") : std::string("false"); }

std::string ManifestICD::get_manifest_str() const {
    std::string out;
    out += "{\n";
    out += "    " + file_format_version.get_version_str() + "\n";
    out += "    \"ICD\": {\n";
    out += "        \"library_path\": \"" + fs::fixup_backslashes_in_path(lib_path) + "\",\n";
    out += "        \"api_version\": \"" + version_to_string(api_version) + "\",\n";
    out += "        \"is_portability_driver\": " + to_text(is_portability_driver);
    if (!library_arch.empty()) {
        out += ",\n       \"library_arch\": \"" + library_arch + "\"\n";
    } else {
        out += "\n";
    }
    out += "    }\n";
    out += "}\n";
    return out;
}

std::string ManifestLayer::LayerDescription::Extension::get_manifest_str() const {
    std::string out;
    out += "{ \"name\":\"" + name + "\",\n\t\t\t\"spec_version\":\"" + std::to_string(spec_version) + "\"";
    print_vector_of_strings(out, "entrypoints", entrypoints);
    out += "\n\t\t\t}";
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
    print_list_of_t(out, "functions", functions);
    print_vector_of_t(out, "instance_extensions", instance_extensions);
    print_vector_of_t(out, "device_extensions", device_extensions);
    if (!enable_environment.empty()) {
        out += ",\n\t\t\"enable_environment\": { \"" + enable_environment + "\": \"1\" }";
    }
    if (!disable_environment.empty()) {
        out += ",\n\t\t\"disable_environment\": { \"" + disable_environment + "\": \"1\" }";
    }
    print_vector_of_strings(out, "component_layers", component_layers);
    print_vector_of_strings(out, "blacklisted_layers", blacklisted_layers);
    print_vector_of_strings(out, "override_paths", override_paths);
    print_vector_of_strings(out, "app_keys", app_keys);
    print_list_of_t(out, "pre_instance_functions", pre_instance_functions);
    if (!library_arch.empty()) {
        out += ",\n\t\t\"library_arch\": \"" + library_arch + "\"";
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
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__)
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
fs::path fixup_backslashes_in_path(fs::path const& in_path) { return fixup_backslashes_in_path(in_path.str()); }

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
    return _wmkdir(widen(path.str()).c_str());
#else
    mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    return 0;
#endif
}

int delete_folder_contents(path const& folder) {
#if defined(WIN32)
    std::wstring folder_utf16 = widen(folder.str());
    if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(folder_utf16.c_str()) && GetLastError() == ERROR_FILE_NOT_FOUND) {
        // nothing to delete
        return 0;
    }
    std::wstring search_path = folder_utf16 + L"/*.*";
    std::string s_p = folder.str() + "/";
    WIN32_FIND_DATAW fd;
    HANDLE hFind = ::FindFirstFileW(search_path.c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            std::string file_name_utf8 = narrow(fd.cFileName);
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (!string_eq(file_name_utf8.c_str(), ".") && !string_eq(file_name_utf8.c_str(), "..")) {
                    delete_folder(s_p + file_name_utf8);
                }
            } else {
                std::string child_name = s_p + file_name_utf8;
                DeleteFileW(widen(child_name).c_str());
            }
        } while (::FindNextFileW(hFind, &fd));
        ::FindClose(hFind);
    }
    return 0;
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
    return ret;
#endif
}

int delete_folder(path const& folder) {
    int ret = delete_folder_contents(folder);
    if (ret != 0) return ret;
#if defined(WIN32)
    _wrmdir(widen(folder.str()).c_str());
    return 0;
#else
    return rmdir(folder.c_str());
#endif
}

#if defined(WIN32)
std::wstring native_path(const std::string& utf8) { return widen(utf8); }
#else
const std::string& native_path(const std::string& utf8) { return utf8; }
#endif

FolderManager::FolderManager(path root_path, std::string name) noexcept : folder(root_path / name) {
    delete_folder_contents(folder);
    create_folder(folder);
}
FolderManager::~FolderManager() noexcept {
    if (folder.str().empty()) return;
    auto list_of_files_to_delete = files;
    // remove(file) modifies the files variable, copy the list before deleting it
    // Note: the allocation tests currently leak the loaded driver handles because in an OOM scenario the loader doesn't bother
    // removing those. Since this is in an OOM situation, it is a low priority to fix. It does have the effect that Windows will
    // be unable to delete the binaries that were leaked.
    for (auto& file : list_of_files_to_delete) {
        remove(file);
    }
    delete_folder(folder);
}
FolderManager::FolderManager(FolderManager&& other) noexcept : folder(other.folder), files(other.files) {
    other.folder.str().clear();
}
FolderManager& FolderManager::operator=(FolderManager&& other) noexcept {
    folder = other.folder;
    files = other.files;
    other.folder.str().clear();
    return *this;
}

path FolderManager::write_manifest(std::string const& name, std::string const& contents) {
    path out_path = folder / name;
    auto found = std::find(files.begin(), files.end(), name);
    if (found != files.end()) {
        std::cout << "Overwriting manifest " << name << ". Was this intended?\n";
    } else {
        files.emplace_back(name);
    }
    auto file = std::ofstream(native_path(out_path.str()), std::ios_base::trunc | std::ios_base::out);
    if (!file) {
        std::cerr << "Failed to create manifest " << name << " at " << out_path.str() << "\n";
        return out_path;
    }
    file << contents << std::endl;
    return out_path;
}
void FolderManager::add_existing_file(std::string const& file_name) { files.emplace_back(file_name); }

// close file handle, delete file, remove `name` from managed file list.
void FolderManager::remove(std::string const& name) {
    path out_path = folder / name;
    auto found = std::find(files.begin(), files.end(), name);
    if (found != files.end()) {
        int rc = std::remove(out_path.c_str());
        if (rc != 0) {
            std::cerr << "Failed to remove file " << name << " at " << out_path.str() << "\n";
        }

        files.erase(found);

    } else {
        std::cout << "Couldn't remove file " << name << " at " << out_path.str() << ".\n";
    }
}

// copy file into this folder
path FolderManager::copy_file(path const& file, std::string const& new_name) {
    auto new_filepath = folder / new_name;
    auto found = std::find(files.begin(), files.end(), new_name);
    if (found != files.end()) {
        std::cout << "File location already contains" << new_name << ". Is this a bug?\n";
    } else if (file.str() == new_filepath.str()) {
        std::cout << "Trying to copy " << new_name << " into itself. Is this a bug?\n";
    } else {
        files.emplace_back(new_name);
    }
    std::ifstream src(native_path(file.str()), std::ios::binary);
    if (!src) {
        std::cerr << "Failed to create file " << file.str() << " for copying from\n";
        return new_filepath;
    }
    std::ofstream dst(native_path(new_filepath.str()), std::ios::binary);
    if (!dst) {
        std::cerr << "Failed to create file " << new_filepath.str() << " for copying to\n";
        return new_filepath;
    }
    dst << src.rdbuf();
    return new_filepath;
}
}  // namespace fs

bool string_eq(const char* a, const char* b) noexcept { return a && b && strcmp(a, b) == 0; }
bool string_eq(const char* a, const char* b, size_t len) noexcept { return a && b && strncmp(a, b, len) == 0; }

InstanceCreateInfo::InstanceCreateInfo() {
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
}

VkInstanceCreateInfo* InstanceCreateInfo::get() noexcept {
    if (fill_in_application_info) {
        application_info.pApplicationName = app_name.c_str();
        application_info.pEngineName = engine_name.c_str();
        application_info.applicationVersion = app_version;
        application_info.engineVersion = engine_version;
        application_info.apiVersion = api_version;
        instance_info.pApplicationInfo = &application_info;
    }
    instance_info.flags = flags;
    instance_info.enabledLayerCount = static_cast<uint32_t>(enabled_layers.size());
    instance_info.ppEnabledLayerNames = enabled_layers.data();
    instance_info.enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size());
    instance_info.ppEnabledExtensionNames = enabled_extensions.data();
    return &instance_info;
}
InstanceCreateInfo& InstanceCreateInfo::set_api_version(uint32_t major, uint32_t minor, uint32_t patch) {
    this->api_version = VK_MAKE_API_VERSION(0, major, minor, patch);
    return *this;
}

DeviceQueueCreateInfo::DeviceQueueCreateInfo() { queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO; }
DeviceQueueCreateInfo::DeviceQueueCreateInfo(const VkDeviceQueueCreateInfo* create_info) {
    queue_create_info = *create_info;
    for (uint32_t i = 0; i < create_info->queueCount; i++) {
        priorities.push_back(create_info->pQueuePriorities[i]);
    }
}

VkDeviceQueueCreateInfo DeviceQueueCreateInfo::get() noexcept {
    queue_create_info.pQueuePriorities = priorities.data();
    queue_create_info.queueCount = 1;
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    return queue_create_info;
}

DeviceCreateInfo::DeviceCreateInfo(const VkDeviceCreateInfo* create_info) {
    dev = *create_info;
    for (uint32_t i = 0; i < create_info->enabledExtensionCount; i++) {
        enabled_extensions.push_back(create_info->ppEnabledExtensionNames[i]);
    }
    for (uint32_t i = 0; i < create_info->enabledLayerCount; i++) {
        enabled_layers.push_back(create_info->ppEnabledLayerNames[i]);
    }
    for (uint32_t i = 0; i < create_info->queueCreateInfoCount; i++) {
        device_queue_infos.push_back(create_info->pQueueCreateInfos[i]);
    }
}

VkDeviceCreateInfo* DeviceCreateInfo::get() noexcept {
    dev.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    dev.enabledLayerCount = static_cast<uint32_t>(enabled_layers.size());
    dev.ppEnabledLayerNames = enabled_layers.data();
    dev.enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size());
    dev.ppEnabledExtensionNames = enabled_extensions.data();
    uint32_t index = 0;
    for (auto& queue : queue_info_details) {
        queue.queue_create_info.queueFamilyIndex = index++;
        queue.queue_create_info.queueCount = 1;
        device_queue_infos.push_back(queue.get());
    }

    dev.queueCreateInfoCount = static_cast<uint32_t>(device_queue_infos.size());
    dev.pQueueCreateInfos = device_queue_infos.data();
    return &dev;
}

#if defined(WIN32)
std::string narrow(const std::wstring& utf16) {
    if (utf16.empty()) {
        return {};
    }
    int size = WideCharToMultiByte(CP_UTF8, 0, utf16.data(), static_cast<int>(utf16.size()), nullptr, 0, nullptr, nullptr);
    if (size <= 0) {
        return {};
    }
    std::string utf8(size, '\0');
    if (WideCharToMultiByte(CP_UTF8, 0, utf16.data(), static_cast<int>(utf16.size()), &utf8[0], size, nullptr, nullptr) != size) {
        return {};
    }
    return utf8;
}

std::wstring widen(const std::string& utf8) {
    if (utf8.empty()) {
        return {};
    }
    int size = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), nullptr, 0);
    if (size <= 0) {
        return {};
    }
    std::wstring utf16(size, L'\0');
    if (MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), &utf16[0], size) != size) {
        return {};
    }
    return utf16;
}
#endif
