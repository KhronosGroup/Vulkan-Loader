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
 * VulkanFunctions - loads vulkan functions for tests to use
 * Instance & Device create info helpers
 * operator == overloads for many vulkan structs - more concise tests
 */
#pragma once

// Following items are needed for C++ to work with PRIxLEAST64
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

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
#include <stdio.h>
#include <stdint.h>

#if defined(WIN32)
#include <direct.h>
#include <windows.h>
#include <strsafe.h>
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
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

/*
 * Common Environment Variable operations
 * These operate on the actual environemnt, they are not shims.
 * set_env_var - sets the env-var with `name` to `value`.
 * remove_env_var - unsets the env-var `name`. Different than set_env_var(name, "");
 * get_env_var - returns a std::string of `name`. if report_failure is true, then it will log to stderr that it didn't find the
 *     env-var
 */

#if defined(WIN32)
bool set_env_var(std::string const& name, std::string const& value);
bool remove_env_var(std::string const& name);
#define ENV_VAR_BUFFER_SIZE 4096
std::string get_env_var(std::string const& name, bool report_failure = true);

#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
bool set_env_var(std::string const& name, std::string const& value);
bool remove_env_var(std::string const& name);
std::string get_env_var(std::string const& name, bool report_failure = true);
#endif

// Windows specific error handling logic
#if defined(WIN32)
const long ERROR_SETENV_FAILED = 10543;           // chosen at random, attempts to not conflict
const long ERROR_REMOVEDIRECTORY_FAILED = 10544;  // chosen at random, attempts to not conflict
const char* win_api_error_str(LSTATUS status);
void print_error_message(LSTATUS status, const char* function_name, std::string optional_message = "");
#endif

enum class DebugMode {
    none,
    log,       // log all folder and file creation & deletion
    no_delete  // Will not delete create folders & files, but will report 'deleting them' to show when something *should* of been
               // deleted
};

struct ManifestICD;    // forward declaration for FolderManager::write
struct ManifestLayer;  // forward declaration for FolderManager::write

#ifdef _WIN32
// Environment variable list separator - not for filesystem paths
const char OS_ENV_VAR_LIST_SEPARATOR = ';';
#else
// Environment variable list separator - not for filesystem paths
const char OS_ENV_VAR_LIST_SEPARATOR = ':';
#endif

namespace fs {
std::string make_native(std::string const&);

struct path {
   private:
#if defined(WIN32)
    static const char path_separator = '\\';
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
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

    // accesors
    path parent_path() const;
    bool has_parent_path() const;
    path filename() const;
    path extension() const;
    path stem() const;

    // modifiers
    path& replace_filename(path const& replacement);

    // get c style string
    const char* c_str() const { return contents.c_str(); }
    // get C++ style string
    std::string const& str() const { return contents; }
    std::string& str() { return contents; }
    size_t size() const { return contents.size(); };

   private:
    std::string contents;
};

std::string fixup_backslashes_in_path(std::string const& in_path);
fs::path fixup_backslashes_in_path(fs::path const& in_path);

int create_folder(path const& path);
int delete_folder(path const& folder);

class FolderManager {
   public:
    explicit FolderManager(path root_path, std::string name, DebugMode debug = DebugMode::none);
    ~FolderManager();
    path write(std::string const& name, ManifestICD const& icd_manifest);
    path write(std::string const& name, ManifestLayer const& layer_manifest);

    // close file handle, delete file, remove `name` from managed file list.
    void remove(std::string const& name);

    // copy file into this folder with name `new_name`. Returns the full path of the file that was copied
    path copy_file(path const& file, std::string const& new_name);

    // location of the managed folder
    path location() const { return folder; }

    FolderManager(FolderManager const&) = delete;
    FolderManager& operator=(FolderManager const&) = delete;

   private:
    DebugMode debug;
    path folder;
    std::vector<std::string> files;
};
}  // namespace fs

// copy the contents of a std::string into a char array and add a null terminator at the end
// src - std::string to read from
// dst - char array to write to
// size_dst - number of characters in the dst array
inline void copy_string_to_char_array(std::string const& src, char* dst, size_t size_dst) {
// Creates a spurious C4996 Warning in VS 2015 - ignore it
#if defined(WIN32)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
    dst[src.copy(dst, size_dst - 1)] = 0;
#if defined(WIN32)
#pragma warning(pop)
#endif
}

#if defined(WIN32)
typedef HMODULE loader_platform_dl_handle;
static loader_platform_dl_handle loader_platform_open_library(const char* lib_path) {
    // Try loading the library the original way first.
    loader_platform_dl_handle lib_handle = LoadLibrary(lib_path);
    if (lib_handle == NULL && GetLastError() == ERROR_MOD_NOT_FOUND) {
        // If that failed, then try loading it with broader search folders.
        lib_handle = LoadLibraryEx(lib_path, NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);
    }
    return lib_handle;
}
inline char* loader_platform_open_library_error(const char* libPath) {
    static char errorMsg[164];
    (void)snprintf(errorMsg, 163, "Failed to open dynamic library \"%s\" with error %lu", libPath, GetLastError());
    return errorMsg;
}
inline void loader_platform_close_library(loader_platform_dl_handle library) { FreeLibrary(library); }
inline void* loader_platform_get_proc_address(loader_platform_dl_handle library, const char* name) {
    assert(library);
    assert(name);
    return (void*)GetProcAddress(library, name);
}
inline char* loader_platform_get_proc_address_error(const char* name) {
    static char errorMsg[120];
    (void)snprintf(errorMsg, 119, "Failed to find function \"%s\" in dynamic library", name);
    return errorMsg;
}

#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)

typedef void* loader_platform_dl_handle;
inline loader_platform_dl_handle loader_platform_open_library(const char* libPath) {
    return dlopen(libPath, RTLD_LAZY | RTLD_LOCAL);
}
inline const char* loader_platform_open_library_error(const char* libPath) { return dlerror(); }
inline void loader_platform_close_library(loader_platform_dl_handle library) { dlclose(library); }
inline void* loader_platform_get_proc_address(loader_platform_dl_handle library, const char* name) {
    assert(library);
    assert(name);
    return dlsym(library, name);
}
inline const char* loader_platform_get_proc_address_error(const char* name) { return dlerror(); }
#endif

struct LibraryWrapper {
    explicit LibraryWrapper() noexcept {}
    explicit LibraryWrapper(fs::path const& lib_path) noexcept : lib_path(lib_path) {
        lib_handle = loader_platform_open_library(lib_path.c_str());
        if (lib_handle == NULL) {
            fprintf(stderr, "Unable to open library %s: %s\n", lib_path.c_str(),
                    loader_platform_open_library_error(lib_path.c_str()));
            assert(lib_handle != NULL && "Must be able to open library");
        }
    }
    ~LibraryWrapper() noexcept {
        if (lib_handle != NULL) {
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
    template <typename T>
    T get_symbol(const char* symbol_name) const {
        assert(lib_handle != NULL && "Cannot get symbol with null library handle");
        T symbol = reinterpret_cast<T>(loader_platform_get_proc_address(lib_handle, symbol_name));
        if (symbol == NULL) {
            fprintf(stderr, "Unable to open symbol %s: %s\n", symbol_name, loader_platform_get_proc_address_error(symbol_name));
            assert(symbol != NULL && "Must be able to get symbol");
        }
        return symbol;
    }

    explicit operator bool() const noexcept { return lib_handle != nullptr; }

    loader_platform_dl_handle lib_handle = nullptr;
    fs::path lib_path;
};

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
    }
    return os << static_cast<int32_t>(result);
}

bool string_eq(const char* a, const char* b) noexcept;
bool string_eq(const char* a, const char* b, size_t len) noexcept;

inline std::string version_to_string(uint32_t version) {
    return std::to_string(VK_VERSION_MAJOR(version)) + "." + std::to_string(VK_VERSION_MINOR(version)) + "." +
           std::to_string(VK_VERSION_PATCH(version));
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
// singular_name = used for the `add_singluar_name` member function
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
    ManifestVersion() noexcept {};
    ManifestVersion(uint32_t major, uint32_t minor, uint32_t patch) noexcept : major(major), minor(minor), patch(patch){};

    std::string get_version_str() const noexcept {
        return std::string("\"file_format_version\": \"") + std::to_string(major) + "." + std::to_string(minor) + "." +
               std::to_string(patch) + "\",";
    }
};

// ManifestICD builder
struct ManifestICD {
    BUILDER_VALUE(ManifestICD, ManifestVersion, file_format_version, ManifestVersion())
    BUILDER_VALUE(ManifestICD, uint32_t, api_version, 0)
    BUILDER_VALUE(ManifestICD, std::string, lib_path, {})
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

            std::string get_manifest_str() const { return std::string("{ \"") + vk_func + "\":\"" + override_name + "\" }"; }
        };
        struct Extension {
            Extension() noexcept {}
            Extension(std::string name, uint32_t spec_version = 0, std::vector<std::string> entrypoints = {}) noexcept
                : name(name), spec_version(spec_version), entrypoints(entrypoints) {}
            std::string name;
            uint32_t spec_version = 0;
            std::vector<std::string> entrypoints;
            std::string get_manifest_str() const;
        };
        BUILDER_VALUE(LayerDescription, std::string, name, {})
        BUILDER_VALUE(LayerDescription, Type, type, Type::INSTANCE)
        BUILDER_VALUE(LayerDescription, fs::path, lib_path, {})
        BUILDER_VALUE(LayerDescription, uint32_t, api_version, VK_MAKE_VERSION(1, 0, 0))
        BUILDER_VALUE(LayerDescription, uint32_t, implementation_version, 0)
        BUILDER_VALUE(LayerDescription, std::string, description, {})
        BUILDER_VECTOR(LayerDescription, FunctionOverride, functions, function)
        BUILDER_VECTOR(LayerDescription, Extension, instance_extensions, instance_extension)
        BUILDER_VECTOR(LayerDescription, Extension, device_extensions, device_extension)
        BUILDER_VALUE(LayerDescription, std::string, enable_environment, {})
        BUILDER_VALUE(LayerDescription, std::string, disable_environment, {})
        BUILDER_VECTOR(LayerDescription, std::string, component_layers, component_layer)
        BUILDER_VECTOR(LayerDescription, std::string, pre_instance_functions, pre_instance_function)

        std::string get_manifest_str() const;
        VkLayerProperties get_layer_properties() const;
    };
    BUILDER_VALUE(ManifestLayer, ManifestVersion, file_format_version, {})
    BUILDER_VECTOR(ManifestLayer, LayerDescription, layers, layer)

    std::string get_manifest_str() const;
};

struct Extension {
    BUILDER_VALUE(Extension, std::string, extensionName, {})
    BUILDER_VALUE(Extension, uint32_t, specVersion, VK_MAKE_VERSION(1, 0, 0))

    Extension(std::string extensionName, uint32_t specVersion = VK_MAKE_VERSION(1, 0, 0))
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

    MockQueueFamilyProperties(VkQueueFamilyProperties properties, bool support_present = false)
        : properties(properties), support_present(support_present) {}

    VkQueueFamilyProperties get() const noexcept { return properties; }
};

struct VulkanFunctions {
    LibraryWrapper loader;

    // Pre-Instance
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = nullptr;
    PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties = nullptr;
    PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties = nullptr;
    PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion = nullptr;
    PFN_vkCreateInstance vkCreateInstance = nullptr;

    // Instance
    PFN_vkDestroyInstance vkDestroyInstance = nullptr;
    PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices = nullptr;
    PFN_vkEnumeratePhysicalDeviceGroups vkEnumeratePhysicalDeviceGroups = nullptr;
    PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures = nullptr;
    PFN_vkGetPhysicalDeviceFeatures2 vkGetPhysicalDeviceFeatures2 = nullptr;
    PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties = nullptr;
    PFN_vkGetPhysicalDeviceFormatProperties2 vkGetPhysicalDeviceFormatProperties2 = nullptr;
    PFN_vkGetPhysicalDeviceImageFormatProperties vkGetPhysicalDeviceImageFormatProperties = nullptr;
    PFN_vkGetPhysicalDeviceImageFormatProperties2 vkGetPhysicalDeviceImageFormatProperties2 = nullptr;
    PFN_vkGetPhysicalDeviceSparseImageFormatProperties vkGetPhysicalDeviceSparseImageFormatProperties = nullptr;
    PFN_vkGetPhysicalDeviceSparseImageFormatProperties2 vkGetPhysicalDeviceSparseImageFormatProperties2 = nullptr;
    PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties = nullptr;
    PFN_vkGetPhysicalDeviceProperties2 vkGetPhysicalDeviceProperties2 = nullptr;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties2 vkGetPhysicalDeviceQueueFamilyProperties2 = nullptr;
    PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties = nullptr;
    PFN_vkGetPhysicalDeviceMemoryProperties2 vkGetPhysicalDeviceMemoryProperties2 = nullptr;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR = nullptr;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
    PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties = nullptr;
    PFN_vkEnumerateDeviceLayerProperties vkEnumerateDeviceLayerProperties = nullptr;
    PFN_vkGetPhysicalDeviceExternalBufferProperties vkGetPhysicalDeviceExternalBufferProperties = nullptr;
    PFN_vkGetPhysicalDeviceExternalFenceProperties vkGetPhysicalDeviceExternalFenceProperties = nullptr;
    PFN_vkGetPhysicalDeviceExternalSemaphoreProperties vkGetPhysicalDeviceExternalSemaphoreProperties = nullptr;

    PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr = nullptr;
    PFN_vkCreateDevice vkCreateDevice = nullptr;
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = nullptr;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = nullptr;

    // WSI
    PFN_vkCreateHeadlessSurfaceEXT vkCreateHeadlessSurfaceEXT = nullptr;
    PFN_vkCreateDisplayPlaneSurfaceKHR vkCreateDisplayPlaneSurfaceKHR = nullptr;
    PFN_vkGetPhysicalDeviceDisplayPropertiesKHR vkGetPhysicalDeviceDisplayPropertiesKHR = nullptr;
    PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR vkGetPhysicalDeviceDisplayPlanePropertiesKHR = nullptr;
    PFN_vkGetDisplayPlaneSupportedDisplaysKHR vkGetDisplayPlaneSupportedDisplaysKHR = nullptr;
    PFN_vkGetDisplayModePropertiesKHR vkGetDisplayModePropertiesKHR = nullptr;
    PFN_vkCreateDisplayModeKHR vkCreateDisplayModeKHR = nullptr;
    PFN_vkGetDisplayPlaneCapabilitiesKHR vkGetDisplayPlaneCapabilitiesKHR = nullptr;
    PFN_vkGetPhysicalDevicePresentRectanglesKHR vkGetPhysicalDevicePresentRectanglesKHR = nullptr;
    PFN_vkGetPhysicalDeviceDisplayProperties2KHR vkGetPhysicalDeviceDisplayProperties2KHR = nullptr;
    PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR vkGetPhysicalDeviceDisplayPlaneProperties2KHR = nullptr;
    PFN_vkGetDisplayModeProperties2KHR vkGetDisplayModeProperties2KHR = nullptr;
    PFN_vkGetDisplayPlaneCapabilities2KHR vkGetDisplayPlaneCapabilities2KHR = nullptr;
    PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR vkGetPhysicalDeviceSurfaceCapabilities2KHR = nullptr;
    PFN_vkGetPhysicalDeviceSurfaceFormats2KHR vkGetPhysicalDeviceSurfaceFormats2KHR = nullptr;

#ifdef VK_USE_PLATFORM_ANDROID_KHR
    PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR = nullptr;
#endif  // VK_USE_PLATFORM_ANDROID_KHR
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
    PFN_vkCreateDirectFBSurfaceEXT vkCreateDirectFBSurfaceEXT = nullptr;
    PFN_vkGetPhysicalDeviceDirectFBPresentationSupportEXT vkGetPhysicalDeviceDirectFBPresentationSupportEXT = nullptr;
#endif  // VK_USE_PLATFORM_DIRECTFB_EXT
#ifdef VK_USE_PLATFORM_FUCHSIA
    PFN_vkCreateImagePipeSurfaceFUCHSIA vkCreateImagePipeSurfaceFUCHSIA = nullptr;
#endif  // VK_USE_PLATFORM_FUCHSIA
#ifdef VK_USE_PLATFORM_GGP
    PFN_vkCreateStreamDescriptorSurfaceGGP vkCreateStreamDescriptorSurfaceGGP = nullptr;
#endif  // VK_USE_PLATFORM_GGP
#ifdef VK_USE_PLATFORM_IOS_MVK
    PFN_vkCreateIOSSurfaceMVK vkCreateIOSSurfaceMVK = nullptr;
#endif  // VK_USE_PLATFORM_IOS_MVK
#ifdef VK_USE_PLATFORM_MACOS_MVK
    PFN_vkCreateMacOSSurfaceMVK vkCreateMacOSSurfaceMVK = nullptr;
#endif  // VK_USE_PLATFORM_MACOS_MVK
#ifdef VK_USE_PLATFORM_METAL_EXT
    PFN_vkCreateMetalSurfaceEXT vkCreateMetalSurfaceEXT = nullptr;
#endif  // VK_USE_PLATFORM_METAL_EXT
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    PFN_vkCreateScreenSurfaceQNX vkCreateScreenSurfaceQNX = nullptr;
    PFN_vkGetPhysicalDeviceScreenPresentationSupportQNX vkGetPhysicalDeviceScreenPresentationSupportQNX = nullptr;
#endif  // VK_USE_PLATFORM_SCREEN_QNX
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR = nullptr;
    PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR vkGetPhysicalDeviceWaylandPresentationSupportKHR = nullptr;
#endif  // VK_USE_PLATFORM_WAYLAND_KHR
#ifdef VK_USE_PLATFORM_XCB_KHR
    PFN_vkCreateXcbSurfaceKHR vkCreateXcbSurfaceKHR = nullptr;
    PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR vkGetPhysicalDeviceXcbPresentationSupportKHR = nullptr;
#endif  // VK_USE_PLATFORM_XCB_KHR
#ifdef VK_USE_PLATFORM_XLIB_KHR
    PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR = nullptr;
    PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR vkGetPhysicalDeviceXlibPresentationSupportKHR = nullptr;
#endif  // VK_USE_PLATFORM_XLIB_KHR
#ifdef VK_USE_PLATFORM_WIN32_KHR
    PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR = nullptr;
    PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR vkGetPhysicalDeviceWin32PresentationSupportKHR = nullptr;
#endif  // VK_USE_PLATFORM_WIN32_KHR
    PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR = nullptr;

    // device functions
    PFN_vkDestroyDevice vkDestroyDevice = nullptr;
    PFN_vkGetDeviceQueue vkGetDeviceQueue = nullptr;

    VulkanFunctions();
};

struct InstanceCreateInfo {
    BUILDER_VALUE(InstanceCreateInfo, VkInstanceCreateInfo, instance_info, {})
    BUILDER_VALUE(InstanceCreateInfo, VkApplicationInfo, application_info, {})
    BUILDER_VALUE(InstanceCreateInfo, std::string, app_name, {})
    BUILDER_VALUE(InstanceCreateInfo, std::string, engine_name, {})
    BUILDER_VALUE(InstanceCreateInfo, uint32_t, app_version, 0)
    BUILDER_VALUE(InstanceCreateInfo, uint32_t, engine_version, 0)
    BUILDER_VALUE(InstanceCreateInfo, uint32_t, api_version, VK_MAKE_VERSION(1, 0, 0))
    BUILDER_VECTOR(InstanceCreateInfo, const char*, enabled_layers, layer)
    BUILDER_VECTOR(InstanceCreateInfo, const char*, enabled_extensions, extension)

    InstanceCreateInfo();

    VkInstanceCreateInfo* get() noexcept;
    InstanceCreateInfo& set_api_version(uint32_t major, uint32_t minor, uint32_t patch);
};

struct DeviceQueueCreateInfo {
    BUILDER_VALUE(DeviceQueueCreateInfo, VkDeviceQueueCreateInfo, queue_create_info, {})
    BUILDER_VECTOR(DeviceQueueCreateInfo, float, priorities, priority)

    DeviceQueueCreateInfo();
};

struct DeviceCreateInfo {
    BUILDER_VALUE(DeviceCreateInfo, VkDeviceCreateInfo, dev, {})
    BUILDER_VECTOR(DeviceCreateInfo, const char*, enabled_extensions, extension)
    BUILDER_VECTOR(DeviceCreateInfo, const char*, enabled_layers, layer)
    BUILDER_VECTOR(DeviceCreateInfo, DeviceQueueCreateInfo, queue_info_details, device_queue)
    BUILDER_VECTOR(DeviceCreateInfo, VkDeviceQueueCreateInfo, queue_infos, queue_info)

    VkDeviceCreateInfo* get() noexcept;
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
    const char* name;
    void* function;
};