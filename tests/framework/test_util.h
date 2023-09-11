/*
 * Copyright (c) 2021-2023 The Khronos Group Inc.
 * Copyright (c) 2021-2023 Valve Corporation
 * Copyright (c) 2021-2023 LunarG, Inc.
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

/*
 * Contains all the utilities needed to make the framework and tests work.
 * Contains:
 * All the standard library includes and main platform specific includes
 * Dll export macro
 * Manifest ICD & Layer structs
 * path abstraction class - modelled after C++17's filesystem::path
 * FolderManager - manages the contents of a folder, cleaning up when needed
 * per-platform library loading - mirrors the vk_loader_platform
 * LibraryWrapper - RAII wrapper for a library
 * DispatchableHandle - RAII wrapper for vulkan dispatchable handle objects
 * ostream overload for VkResult - prettifies googletest output
 * Instance & Device create info helpers
 * operator == overloads for many vulkan structs - more concise tests
 */
#pragma once

#include <algorithm>
#include <array>
#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include <memory>
#include <functional>

#include <cassert>
#include <cstring>
#include <ctime>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>

// Set of platforms with a common set of functionality which is queried throughout the program
#if defined(__linux__) || defined(__APPLE__) || defined(__Fuchsia__) || defined(__QNX__) || defined(__FreeBSD__) || \
    defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__) || defined(__GNU__)
#define COMMON_UNIX_PLATFORMS 1
#else
#define COMMON_UNIX_PLATFORMS 0
#endif

#if defined(WIN32)
#include <direct.h>
#include <windows.h>
#include <strsafe.h>
#elif COMMON_UNIX_PLATFORMS
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dlfcn.h>

// Prevent macro collisions from <sys/types.h>
#undef major
#undef minor

#endif

#include <vulkan/vulkan.h>
#include <vulkan/vk_icd.h>
#include <vulkan/vk_layer.h>

#include "framework_config.h"

#if defined(__GNUC__) && __GNUC__ >= 4
#define FRAMEWORK_EXPORT __attribute__((visibility("default")))
#elif defined(__SUNPRO_C) && (__SUNPRO_C >= 0x590)
#define FRAMEWORK_EXPORT __attribute__((visibility("default")))
#elif defined(WIN32)
#define FRAMEWORK_EXPORT __declspec(dllexport)
#else
#define FRAMEWORK_EXPORT
#endif

#include "json_writer.h"

// get_env_var() - returns a std::string of `name`. if report_failure is true, then it will log to stderr that it didn't find the
//     env-var
// NOTE: This is only intended for test framework code, all test code MUST use EnvVarWrapper
std::string get_env_var(std::string const& name, bool report_failure = true);

/*
 * Wrapper around Environment Variables with common operations
 * Since Environment Variables leak between tests, there needs to be RAII code to remove them during test cleanup

 */

// Wrapper to set & remove env-vars automatically
struct EnvVarWrapper {
    // Constructor which unsets the env-var
    EnvVarWrapper(std::string const& name) noexcept : name(name) {
        initial_value = get_env_var(name, false);
        remove_env_var();
    }
    // Constructor which set the env-var to the specified value
    EnvVarWrapper(std::string const& name, std::string const& value) noexcept : name(name), cur_value(value) {
        initial_value = get_env_var(name, false);
        set_env_var();
    }
    ~EnvVarWrapper() noexcept {
        remove_env_var();
        if (!initial_value.empty()) {
            set_new_value(initial_value);
        }
    }

    // delete copy operators
    EnvVarWrapper(const EnvVarWrapper&) = delete;
    EnvVarWrapper& operator=(const EnvVarWrapper&) = delete;

    void set_new_value(std::string const& value) {
        cur_value = value;
        set_env_var();
    }
    void add_to_list(std::string const& list_item) {
        if (!cur_value.empty()) {
            cur_value += OS_ENV_VAR_LIST_SEPARATOR;
        }
        cur_value += list_item;
        set_env_var();
    }
    void remove_value() const { remove_env_var(); }
    const char* get() const { return name.c_str(); }
    const char* value() const { return cur_value.c_str(); }

   private:
    std::string name;
    std::string cur_value;
    std::string initial_value;

    void set_env_var();
    void remove_env_var() const;
#if defined(WIN32)
    // Environment variable list separator - not for filesystem paths
    const char OS_ENV_VAR_LIST_SEPARATOR = ';';
#elif COMMON_UNIX_PLATFORMS
    // Environment variable list separator - not for filesystem paths
    const char OS_ENV_VAR_LIST_SEPARATOR = ':';
#endif
};

// Windows specific error handling logic
#if defined(WIN32)
const long ERROR_SETENV_FAILED = 10543;           // chosen at random, attempts to not conflict
const long ERROR_REMOVEDIRECTORY_FAILED = 10544;  // chosen at random, attempts to not conflict
const char* win_api_error_str(LSTATUS status);
void print_error_message(LSTATUS status, const char* function_name, std::string optional_message = "");
#endif

struct ManifestICD;    // forward declaration for FolderManager::write
struct ManifestLayer;  // forward declaration for FolderManager::write

namespace fs {
std::string make_native(std::string const&);

struct path {
#if defined(WIN32)
    static const char path_separator = '\\';
#elif COMMON_UNIX_PLATFORMS
    static const char path_separator = '/';
#endif

   public:
    path() {}
    path(std::string const& in) : contents(make_native(in)) {}
    path(const char* in) : contents(make_native(std::string(in))) {}

    // concat paths without directoryseperator
    path& operator+=(path const& in);
    path& operator+=(std::string const& in);
    path& operator+=(const char* in);

    // append paths with directoryseperator
    path& operator/=(path const& in);
    path& operator/=(std::string const& in);
    path& operator/=(const char* in);

    // concat paths without directory seperator
    path operator+(path const& in) const;
    path operator+(std::string const& in) const;
    path operator+(const char* in) const;

    // append paths with directory seperator
    path operator/(path const& in) const;
    path operator/(std::string const& in) const;
    path operator/(const char* in) const;

    // accessors
    path parent_path() const;      // Everything before the last path separator, if there is one.
    bool has_parent_path() const;  // True if the path contains more than just a filename.
    path filename() const;         // Everything after the last path separator.
    path extension() const;        // The file extension, if it has one.
    path stem() const;             // The filename minus the extension.

    // modifiers
    path& replace_filename(path const& replacement);

    // get c style string
    const char* c_str() const { return contents.c_str(); }
    // get C++ style string
    std::string const& str() const { return contents; }
    std::string& str() { return contents; }
    size_t size() const { return contents.size(); }

    // equality
    bool operator==(path const& other) const noexcept { return contents == other.contents; }
    bool operator!=(path const& other) const noexcept { return !(*this == other); }

   private:
    std::string contents;
};

std::string fixup_backslashes_in_path(std::string const& in_path);
fs::path fixup_backslashes_in_path(fs::path const& in_path);

int create_folder(path const& path);
int delete_folder(path const& folder);

class FolderManager {
   public:
    explicit FolderManager(path root_path, std::string name) noexcept;
    ~FolderManager() noexcept;
    FolderManager(FolderManager const&) = delete;
    FolderManager& operator=(FolderManager const&) = delete;
    FolderManager(FolderManager&& other) noexcept;
    FolderManager& operator=(FolderManager&& other) noexcept;

    // Add a manifest to the folder
    path write_manifest(std::string const& name, std::string const& contents);

    // Add an already existing file to the manager, so it will be cleaned up automatically
    void add_existing_file(std::string const& file_name);

    // close file handle, delete file, remove `name` from managed file list.
    void remove(std::string const& name);

    // copy file into this folder with name `new_name`. Returns the full path of the file that was copied
    path copy_file(path const& file, std::string const& new_name);

    // location of the managed folder
    path location() const { return folder; }

    std::vector<std::string> get_files() const { return files; }

   private:
    path folder;
    std::vector<std::string> files;
};
}  // namespace fs

// copy the contents of a std::string into a char array and add a null terminator at the end
// src - std::string to read from
// dst - char array to write to
// size_dst - number of characters in the dst array
inline void copy_string_to_char_array(std::string const& src, char* dst, size_t size_dst) { dst[src.copy(dst, size_dst - 1)] = 0; }

#if defined(WIN32)
// Convert an UTF-16 wstring to an UTF-8 string
std::string narrow(const std::wstring& utf16);
// Convert an UTF-8 string to an UTF-16 wstring
std::wstring widen(const std::string& utf8);
#endif

#if defined(WIN32)
typedef HMODULE loader_platform_dl_handle;
inline loader_platform_dl_handle loader_platform_open_library(const char* lib_path) {
    std::wstring lib_path_utf16 = widen(lib_path);
    // Try loading the library the original way first.
    loader_platform_dl_handle lib_handle = LoadLibraryW(lib_path_utf16.c_str());
    if (lib_handle == nullptr && GetLastError() == ERROR_MOD_NOT_FOUND) {
        // If that failed, then try loading it with broader search folders.
        lib_handle =
            LoadLibraryExW(lib_path_utf16.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);
    }
    return lib_handle;
}
inline char* loader_platform_open_library_error(const char* libPath) {
    static char errorMsg[164];
    snprintf(errorMsg, 163, "Failed to open dynamic library \"%s\" with error %lu", libPath, GetLastError());
    return errorMsg;
}
inline void loader_platform_close_library(loader_platform_dl_handle library) { FreeLibrary(library); }
inline void* loader_platform_get_proc_address(loader_platform_dl_handle library, const char* name) {
    assert(library);
    assert(name);
    return reinterpret_cast<void*>(GetProcAddress(library, name));
}
inline char* loader_platform_get_proc_address_error(const char* name) {
    static char errorMsg[120];
    snprintf(errorMsg, 119, "Failed to find function \"%s\" in dynamic library", name);
    return errorMsg;
}

#elif COMMON_UNIX_PLATFORMS

typedef void* loader_platform_dl_handle;
inline loader_platform_dl_handle loader_platform_open_library(const char* libPath) {
    return dlopen(libPath, RTLD_LAZY | RTLD_LOCAL);
}
inline const char* loader_platform_open_library_error([[maybe_unused]] const char* libPath) { return dlerror(); }
inline void loader_platform_close_library(loader_platform_dl_handle library) { dlclose(library); }
inline void* loader_platform_get_proc_address(loader_platform_dl_handle library, const char* name) {
    assert(library);
    assert(name);
    return dlsym(library, name);
}
inline const char* loader_platform_get_proc_address_error([[maybe_unused]] const char* name) { return dlerror(); }
#endif

class FromVoidStarFunc {
   private:
    void* function;

   public:
    FromVoidStarFunc(void* function) : function(function) {}
    FromVoidStarFunc(PFN_vkVoidFunction function) : function(reinterpret_cast<void*>(function)) {}

    template <typename T>
    operator T() {
        return reinterpret_cast<T>(function);
    }
};

struct LibraryWrapper {
    explicit LibraryWrapper() noexcept {}
    explicit LibraryWrapper(fs::path const& lib_path) noexcept : lib_path(lib_path) {
        lib_handle = loader_platform_open_library(lib_path.c_str());
        if (lib_handle == nullptr) {
            fprintf(stderr, "Unable to open library %s: %s\n", lib_path.c_str(),
                    loader_platform_open_library_error(lib_path.c_str()));
            assert(lib_handle != nullptr && "Must be able to open library");
        }
    }
    ~LibraryWrapper() noexcept {
        if (lib_handle != nullptr) {
            loader_platform_close_library(lib_handle);
            lib_handle = nullptr;
        }
    }
    LibraryWrapper(LibraryWrapper const& wrapper) = delete;
    LibraryWrapper& operator=(LibraryWrapper const& wrapper) = delete;
    LibraryWrapper(LibraryWrapper&& wrapper) noexcept : lib_handle(wrapper.lib_handle), lib_path(wrapper.lib_path) {
        wrapper.lib_handle = nullptr;
    }
    LibraryWrapper& operator=(LibraryWrapper&& wrapper) noexcept {
        if (this != &wrapper) {
            if (lib_handle != nullptr) {
                loader_platform_close_library(lib_handle);
            }
            lib_handle = wrapper.lib_handle;
            lib_path = wrapper.lib_path;
            wrapper.lib_handle = nullptr;
        }
        return *this;
    }
    FromVoidStarFunc get_symbol(const char* symbol_name) const {
        assert(lib_handle != nullptr && "Cannot get symbol with null library handle");
        void* symbol = loader_platform_get_proc_address(lib_handle, symbol_name);
        if (symbol == nullptr) {
            fprintf(stderr, "Unable to open symbol %s: %s\n", symbol_name, loader_platform_get_proc_address_error(symbol_name));
            assert(symbol != nullptr && "Must be able to get symbol");
        }
        return FromVoidStarFunc(symbol);
    }

    explicit operator bool() const noexcept { return lib_handle != nullptr; }

    loader_platform_dl_handle lib_handle = nullptr;
    fs::path lib_path;
};

template <typename T>
PFN_vkVoidFunction to_vkVoidFunction(T func) {
    return reinterpret_cast<PFN_vkVoidFunction>(func);
}
template <typename T>
struct FRAMEWORK_EXPORT DispatchableHandle {
    DispatchableHandle() {
        auto ptr_handle = new VK_LOADER_DATA;
        set_loader_magic_value(ptr_handle);
        handle = reinterpret_cast<T>(ptr_handle);
    }
    ~DispatchableHandle() {
        delete reinterpret_cast<VK_LOADER_DATA*>(handle);
        handle = nullptr;
    }
    DispatchableHandle(DispatchableHandle const&) = delete;
    DispatchableHandle& operator=(DispatchableHandle const&) = delete;
    DispatchableHandle(DispatchableHandle&& other) noexcept : handle(other.handle) { other.handle = nullptr; }
    DispatchableHandle& operator=(DispatchableHandle&& other) noexcept {
        if (this != &other) {
            delete reinterpret_cast<VK_LOADER_DATA*>(handle);
            handle = other.handle;
            other.handle = nullptr;
        }
        return *this;
    }
    bool operator==(T base_handle) { return base_handle == handle; }
    bool operator!=(T base_handle) { return base_handle != handle; }

    T handle = nullptr;
};

// Stream operator for VkResult so GTEST will print out error codes as strings (automatically)
inline std::ostream& operator<<(std::ostream& os, const VkResult& result) {
    switch (result) {
        case (VK_SUCCESS):
            return os << "VK_SUCCESS";
        case (VK_NOT_READY):
            return os << "VK_NOT_READY";
        case (VK_TIMEOUT):
            return os << "VK_TIMEOUT";
        case (VK_EVENT_SET):
            return os << "VK_EVENT_SET";
        case (VK_EVENT_RESET):
            return os << "VK_EVENT_RESET";
        case (VK_INCOMPLETE):
            return os << "VK_INCOMPLETE";
        case (VK_ERROR_OUT_OF_HOST_MEMORY):
            return os << "VK_ERROR_OUT_OF_HOST_MEMORY";
        case (VK_ERROR_OUT_OF_DEVICE_MEMORY):
            return os << "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case (VK_ERROR_INITIALIZATION_FAILED):
            return os << "VK_ERROR_INITIALIZATION_FAILED";
        case (VK_ERROR_DEVICE_LOST):
            return os << "VK_ERROR_DEVICE_LOST";
        case (VK_ERROR_MEMORY_MAP_FAILED):
            return os << "VK_ERROR_MEMORY_MAP_FAILED";
        case (VK_ERROR_LAYER_NOT_PRESENT):
            return os << "VK_ERROR_LAYER_NOT_PRESENT";
        case (VK_ERROR_EXTENSION_NOT_PRESENT):
            return os << "VK_ERROR_EXTENSION_NOT_PRESENT";
        case (VK_ERROR_FEATURE_NOT_PRESENT):
            return os << "VK_ERROR_FEATURE_NOT_PRESENT";
        case (VK_ERROR_INCOMPATIBLE_DRIVER):
            return os << "VK_ERROR_INCOMPATIBLE_DRIVER";
        case (VK_ERROR_TOO_MANY_OBJECTS):
            return os << "VK_ERROR_TOO_MANY_OBJECTS";
        case (VK_ERROR_FORMAT_NOT_SUPPORTED):
            return os << "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case (VK_ERROR_FRAGMENTED_POOL):
            return os << "VK_ERROR_FRAGMENTED_POOL";
        case (VK_ERROR_UNKNOWN):
            return os << "VK_ERROR_UNKNOWN";
        case (VK_ERROR_OUT_OF_POOL_MEMORY):
            return os << "VK_ERROR_OUT_OF_POOL_MEMORY";
        case (VK_ERROR_INVALID_EXTERNAL_HANDLE):
            return os << "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        case (VK_ERROR_FRAGMENTATION):
            return os << "VK_ERROR_FRAGMENTATION";
        case (VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS):
            return os << "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
        case (VK_ERROR_SURFACE_LOST_KHR):
            return os << "VK_ERROR_SURFACE_LOST_KHR";
        case (VK_ERROR_NATIVE_WINDOW_IN_USE_KHR):
            return os << "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case (VK_SUBOPTIMAL_KHR):
            return os << "VK_SUBOPTIMAL_KHR";
        case (VK_ERROR_OUT_OF_DATE_KHR):
            return os << "VK_ERROR_OUT_OF_DATE_KHR";
        case (VK_ERROR_INCOMPATIBLE_DISPLAY_KHR):
            return os << "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case (VK_ERROR_VALIDATION_FAILED_EXT):
            return os << "VK_ERROR_VALIDATION_FAILED_EXT";
        case (VK_ERROR_INVALID_SHADER_NV):
            return os << "VK_ERROR_INVALID_SHADER_NV";
        case (VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT):
            return os << "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        case (VK_ERROR_NOT_PERMITTED_EXT):
            return os << "VK_ERROR_NOT_PERMITTED_EXT";
        case (VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT):
            return os << "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
        case (VK_THREAD_IDLE_KHR):
            return os << "VK_THREAD_IDLE_KHR";
        case (VK_THREAD_DONE_KHR):
            return os << "VK_THREAD_DONE_KHR";
        case (VK_OPERATION_DEFERRED_KHR):
            return os << "VK_OPERATION_DEFERRED_KHR";
        case (VK_OPERATION_NOT_DEFERRED_KHR):
            return os << "VK_OPERATION_NOT_DEFERRED_KHR";
        case (VK_PIPELINE_COMPILE_REQUIRED_EXT):
            return os << "VK_PIPELINE_COMPILE_REQUIRED_EXT";
        case (VK_RESULT_MAX_ENUM):
            return os << "VK_RESULT_MAX_ENUM";
        case (VK_ERROR_COMPRESSION_EXHAUSTED_EXT):
            return os << "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
        case (VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR):
            return os << "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
        case (VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR):
            return os << "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
        case (VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR):
            return os << "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
        case (VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR):
            return os << "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
        case (VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR):
            return os << "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
        case (VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR):
            return os << "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
        case (VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR):
            return os << "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
        case (VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT):
            return os << "VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT";
    }
    return os << static_cast<int32_t>(result);
}

const char* get_platform_wsi_extension([[maybe_unused]] const char* api_selection);

bool string_eq(const char* a, const char* b) noexcept;
bool string_eq(const char* a, const char* b, size_t len) noexcept;

inline std::string version_to_string(uint32_t version) {
    std::string out = std::to_string(VK_API_VERSION_MAJOR(version)) + "." + std::to_string(VK_API_VERSION_MINOR(version)) + "." +
                      std::to_string(VK_API_VERSION_PATCH(version));
    if (VK_API_VERSION_VARIANT(version) != 0) out += std::to_string(VK_API_VERSION_VARIANT(version)) + "." + out;
    return out;
}

// Macro to ease the definition of variables with builder member functions
// class_name = class the member variable is apart of
// type = type of the variable
// name = name of the variable
// default_value = value to default initialize, use {} if nothing else makes sense
#define BUILDER_VALUE(class_name, type, name, default_value) \
    type name = default_value;                               \
    class_name& set_##name(type const& name) {               \
        this->name = name;                                   \
        return *this;                                        \
    }

// Macro to ease the definition of vectors with builder member functions
// class_name = class the member variable is apart of
// type = type of the variable
// name = name of the variable
// singular_name = used for the `add_singular_name` member function
#define BUILDER_VECTOR(class_name, type, name, singular_name)                    \
    std::vector<type> name;                                                      \
    class_name& add_##singular_name(type const& singular_name) {                 \
        this->name.push_back(singular_name);                                     \
        return *this;                                                            \
    }                                                                            \
    class_name& add_##singular_name##s(std::vector<type> const& singular_name) { \
        for (auto& elem : singular_name) this->name.push_back(elem);             \
        return *this;                                                            \
    }
// Like BUILDER_VECTOR but for move only types - where passing in means giving up ownership
#define BUILDER_VECTOR_MOVE_ONLY(class_name, type, name, singular_name) \
    std::vector<type> name;                                             \
    class_name& add_##singular_name(type&& singular_name) {             \
        this->name.push_back(std::move(singular_name));                 \
        return *this;                                                   \
    }

struct ManifestVersion {
    BUILDER_VALUE(ManifestVersion, uint32_t, major, 1)
    BUILDER_VALUE(ManifestVersion, uint32_t, minor, 0)
    BUILDER_VALUE(ManifestVersion, uint32_t, patch, 0)

    std::string get_version_str() const noexcept {
        return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
    }
};

// ManifestICD builder
struct ManifestICD {
    BUILDER_VALUE(ManifestICD, ManifestVersion, file_format_version, {})
    BUILDER_VALUE(ManifestICD, uint32_t, api_version, 0)
    BUILDER_VALUE(ManifestICD, fs::path, lib_path, {})
    BUILDER_VALUE(ManifestICD, bool, is_portability_driver, false)
    BUILDER_VALUE(ManifestICD, std::string, library_arch, "")
    std::string get_manifest_str() const;
};

// ManifestLayer builder
struct ManifestLayer {
    struct LayerDescription {
        enum class Type { INSTANCE, GLOBAL, DEVICE };
        std::string get_type_str(Type layer_type) const {
            if (layer_type == Type::GLOBAL)
                return "GLOBAL";
            else if (layer_type == Type::DEVICE)
                return "DEVICE";
            else  // default
                return "INSTANCE";
        }
        struct FunctionOverride {
            BUILDER_VALUE(FunctionOverride, std::string, vk_func, {})
            BUILDER_VALUE(FunctionOverride, std::string, override_name, {})

            void get_manifest_str(JsonWriter& writer) const { writer.AddKeyedString(vk_func, override_name); }
        };
        struct Extension {
            Extension() noexcept {}
            Extension(std::string name, uint32_t spec_version = 0, std::vector<std::string> entrypoints = {}) noexcept
                : name(name), spec_version(spec_version), entrypoints(entrypoints) {}
            std::string name;
            uint32_t spec_version = 0;
            std::vector<std::string> entrypoints;
            void get_manifest_str(JsonWriter& writer) const;
        };
        BUILDER_VALUE(LayerDescription, std::string, name, {})
        BUILDER_VALUE(LayerDescription, Type, type, Type::INSTANCE)
        BUILDER_VALUE(LayerDescription, fs::path, lib_path, {})
        BUILDER_VALUE(LayerDescription, uint32_t, api_version, VK_API_VERSION_1_0)
        BUILDER_VALUE(LayerDescription, uint32_t, implementation_version, 0)
        BUILDER_VALUE(LayerDescription, std::string, description, {})
        BUILDER_VECTOR(LayerDescription, FunctionOverride, functions, function)
        BUILDER_VECTOR(LayerDescription, Extension, instance_extensions, instance_extension)
        BUILDER_VECTOR(LayerDescription, Extension, device_extensions, device_extension)
        BUILDER_VALUE(LayerDescription, std::string, enable_environment, {})
        BUILDER_VALUE(LayerDescription, std::string, disable_environment, {})
        BUILDER_VECTOR(LayerDescription, std::string, component_layers, component_layer)
        BUILDER_VECTOR(LayerDescription, std::string, blacklisted_layers, blacklisted_layer)
        BUILDER_VECTOR(LayerDescription, std::string, override_paths, override_path)
        BUILDER_VECTOR(LayerDescription, FunctionOverride, pre_instance_functions, pre_instance_function)
        BUILDER_VECTOR(LayerDescription, std::string, app_keys, app_key)
        BUILDER_VALUE(LayerDescription, std::string, library_arch, "")

        void get_manifest_str(JsonWriter& writer) const;
        VkLayerProperties get_layer_properties() const;
    };
    BUILDER_VALUE(ManifestLayer, ManifestVersion, file_format_version, {})
    BUILDER_VECTOR(ManifestLayer, LayerDescription, layers, layer)

    std::string get_manifest_str() const;
};

struct Extension {
    BUILDER_VALUE(Extension, std::string, extensionName, {})
    BUILDER_VALUE(Extension, uint32_t, specVersion, VK_API_VERSION_1_0)

    Extension(const char* name, uint32_t specVersion = VK_API_VERSION_1_0) noexcept
        : extensionName(name), specVersion(specVersion) {}
    Extension(std::string extensionName, uint32_t specVersion = VK_API_VERSION_1_0) noexcept
        : extensionName(extensionName), specVersion(specVersion) {}

    VkExtensionProperties get() const noexcept {
        VkExtensionProperties props{};
        copy_string_to_char_array(extensionName, &props.extensionName[0], VK_MAX_EXTENSION_NAME_SIZE);
        props.specVersion = specVersion;
        return props;
    }
};

struct MockQueueFamilyProperties {
    BUILDER_VALUE(MockQueueFamilyProperties, VkQueueFamilyProperties, properties, {})
    BUILDER_VALUE(MockQueueFamilyProperties, bool, support_present, false)

    VkQueueFamilyProperties get() const noexcept { return properties; }
};

struct InstanceCreateInfo {
    BUILDER_VALUE(InstanceCreateInfo, VkInstanceCreateInfo, instance_info, {})
    BUILDER_VALUE(InstanceCreateInfo, VkApplicationInfo, application_info, {})
    BUILDER_VALUE(InstanceCreateInfo, std::string, app_name, {})
    BUILDER_VALUE(InstanceCreateInfo, std::string, engine_name, {})
    BUILDER_VALUE(InstanceCreateInfo, uint32_t, flags, 0)
    BUILDER_VALUE(InstanceCreateInfo, uint32_t, app_version, 0)
    BUILDER_VALUE(InstanceCreateInfo, uint32_t, engine_version, 0)
    BUILDER_VALUE(InstanceCreateInfo, uint32_t, api_version, VK_API_VERSION_1_0)
    BUILDER_VECTOR(InstanceCreateInfo, const char*, enabled_layers, layer)
    BUILDER_VECTOR(InstanceCreateInfo, const char*, enabled_extensions, extension)
    // tell the get() function to not provide `application_info`
    BUILDER_VALUE(InstanceCreateInfo, bool, fill_in_application_info, true)

    InstanceCreateInfo();

    VkInstanceCreateInfo* get() noexcept;

    InstanceCreateInfo& set_api_version(uint32_t major, uint32_t minor, uint32_t patch);

    InstanceCreateInfo& setup_WSI(const char* api_selection = nullptr);
};

struct DeviceQueueCreateInfo {
    DeviceQueueCreateInfo();
    DeviceQueueCreateInfo(const VkDeviceQueueCreateInfo* create_info);

    BUILDER_VALUE(DeviceQueueCreateInfo, VkDeviceQueueCreateInfo, queue_create_info, {})
    BUILDER_VECTOR(DeviceQueueCreateInfo, float, priorities, priority)

    VkDeviceQueueCreateInfo get() noexcept;
};

struct DeviceCreateInfo {
    DeviceCreateInfo() = default;
    DeviceCreateInfo(const VkDeviceCreateInfo* create_info);

    BUILDER_VALUE(DeviceCreateInfo, VkDeviceCreateInfo, dev, {})
    BUILDER_VECTOR(DeviceCreateInfo, const char*, enabled_extensions, extension)
    BUILDER_VECTOR(DeviceCreateInfo, const char*, enabled_layers, layer)
    BUILDER_VECTOR(DeviceCreateInfo, DeviceQueueCreateInfo, queue_info_details, device_queue)

    VkDeviceCreateInfo* get() noexcept;

   private:
    std::vector<VkDeviceQueueCreateInfo> device_queue_infos;
};

inline bool operator==(const VkExtent3D& a, const VkExtent3D& b) {
    return a.width == b.width && a.height == b.height && a.depth == b.depth;
}
inline bool operator!=(const VkExtent3D& a, const VkExtent3D& b) { return !(a == b); }

inline bool operator==(const VkQueueFamilyProperties& a, const VkQueueFamilyProperties& b) {
    return a.minImageTransferGranularity == b.minImageTransferGranularity && a.queueCount == b.queueCount &&
           a.queueFlags == b.queueFlags && a.timestampValidBits == b.timestampValidBits;
}
inline bool operator!=(const VkQueueFamilyProperties& a, const VkQueueFamilyProperties& b) { return !(a == b); }

inline bool operator==(const VkLayerProperties& a, const VkLayerProperties& b) {
    return string_eq(a.layerName, b.layerName, 256) && string_eq(a.description, b.description, 256) &&
           a.implementationVersion == b.implementationVersion && a.specVersion == b.specVersion;
}
inline bool operator!=(const VkLayerProperties& a, const VkLayerProperties& b) { return !(a == b); }

inline bool operator==(const VkExtensionProperties& a, const VkExtensionProperties& b) {
    return string_eq(a.extensionName, b.extensionName, 256) && a.specVersion == b.specVersion;
}
inline bool operator!=(const VkExtensionProperties& a, const VkExtensionProperties& b) { return !(a == b); }

struct VulkanFunction {
    std::string name;
    PFN_vkVoidFunction function = nullptr;
};

template <typename T, size_t U>
bool check_permutation(std::initializer_list<const char*> expected, std::array<T, U> const& returned) {
    if (expected.size() != returned.size()) return false;
    for (uint32_t i = 0; i < expected.size(); i++) {
        auto found = std::find_if(std::begin(returned), std::end(returned),
                                  [&](T elem) { return string_eq(*(expected.begin() + i), elem.layerName); });
        if (found == std::end(returned)) return false;
    }
    return true;
}
template <typename T>
bool check_permutation(std::initializer_list<const char*> expected, std::vector<T> const& returned) {
    if (expected.size() != returned.size()) return false;
    for (uint32_t i = 0; i < expected.size(); i++) {
        auto found = std::find_if(std::begin(returned), std::end(returned),
                                  [&](T elem) { return string_eq(*(expected.begin() + i), elem.layerName); });
        if (found == std::end(returned)) return false;
    }
    return true;
}

inline bool contains(std::vector<VkExtensionProperties> const& vec, const char* name) {
    return std::any_of(std::begin(vec), std::end(vec),
                       [name](VkExtensionProperties const& elem) { return string_eq(name, elem.extensionName); });
}
inline bool contains(std::vector<VkLayerProperties> const& vec, const char* name) {
    return std::any_of(std::begin(vec), std::end(vec),
                       [name](VkLayerProperties const& elem) { return string_eq(name, elem.layerName); });
}

#if defined(__linux__) || defined(__GNU__)

// find application path + name. Path cannot be longer than 1024, returns NULL if it is greater than that.
inline std::string test_platform_executable_path() {
    std::string buffer;
    buffer.resize(1024);
    ssize_t count = readlink("/proc/self/exe", &buffer[0], buffer.size());
    if (count == -1) return NULL;
    if (count == 0) return NULL;
    buffer[count] = '\0';
    buffer.resize(count);
    return buffer;
}
#elif defined(__APPLE__)
#include <libproc.h>
inline std::string test_platform_executable_path() {
    std::string buffer;
    buffer.resize(1024);
    pid_t pid = getpid();
    int ret = proc_pidpath(pid, &buffer[0], buffer.size());
    if (ret <= 0) return NULL;
    buffer[ret] = '\0';
    buffer.resize(ret);
    return buffer;
}
#elif defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__)
#include <sys/sysctl.h>
inline std::string test_platform_executable_path() {
    int mib[] = {
        CTL_KERN,
#if defined(__NetBSD__)
        KERN_PROC_ARGS,
        -1,
        KERN_PROC_PATHNAME,
#else
        KERN_PROC,
        KERN_PROC_PATHNAME,
        -1,
#endif
    };
    std::string buffer;
    buffer.resize(1024);
    size_t size = buffer.size();
    if (sysctl(mib, sizeof(mib) / sizeof(mib[0]), &buffer[0], &size, NULL, 0) < 0) {
        return NULL;
    }
    buffer.resize(size);

    return buffer;
}
#elif defined(__Fuchsia__) || defined(__OpenBSD__)
inline std::string test_platform_executable_path() { return {}; }
#elif defined(__QNX__)

#define SYSCONFDIR "/etc"

#include <fcntl.h>
#include <sys/stat.h>

inline std::string test_platform_executable_path() {
    std::string buffer;
    buffer.resize(1024);
    int fd = open("/proc/self/exefile", O_RDONLY);
    size_t rdsize;

    if (fd == -1) {
        return NULL;
    }

    rdsize = read(fd, &buffer[0], buffer.size());
    if (rdsize == size) {
        return NULL;
    }
    buffer[rdsize] = 0x00;
    close(fd);
    buffer.resize(rdsize);

    return buffer;
}
#endif  // defined (__QNX__)
#if defined(WIN32)
inline std::string test_platform_executable_path() {
    std::string buffer;
    buffer.resize(1024);
    DWORD ret = GetModuleFileName(NULL, static_cast<LPSTR>(&buffer[0]), (DWORD)buffer.size());
    if (ret == 0) return NULL;
    if (ret > buffer.size()) return NULL;
    buffer.resize(ret);
    buffer[ret] = '\0';
    return buffer;
}

inline std::wstring conver_str_to_wstr(std::string const& input) {
    std::wstring output{};
    output.resize(input.size());
    size_t characters_converted = 0;
    mbstowcs_s(&characters_converted, &output[0], output.size() + 1, input.c_str(), input.size());
    return output;
}

#endif
