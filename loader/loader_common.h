/*
 *
 * Copyright (c) 2014-2022 The Khronos Group Inc.
 * Copyright (c) 2014-2022 Valve Corporation
 * Copyright (c) 2014-2022 LunarG, Inc.
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

#pragma once

#include "vulkan/vk_platform.h"
#include "vulkan/vk_sdk_platform.h"
#include <vulkan/vulkan.h>
#include <vulkan/vk_layer.h>
#include <vulkan/vk_icd.h>

#include "vk_loader_platform.h"
#include "vk_loader_layer.h"
#include "vk_layer_dispatch_table.h"
#include "vk_loader_extensions.h"

typedef enum VkStringErrorFlagBits {
    VK_STRING_ERROR_NONE = 0x00000000,
    VK_STRING_ERROR_LENGTH = 0x00000001,
    VK_STRING_ERROR_BAD_DATA = 0x00000002,
    VK_STRING_ERROR_NULL_PTR = 0x00000004,
} VkStringErrorFlagBits;
typedef VkFlags VkStringErrorFlags;

static const int MaxLoaderStringLength = 256;           // 0xFF;
static const unsigned char UTF8_ONE_BYTE_CODE = 192;    // 0xC0;
static const unsigned char UTF8_ONE_BYTE_MASK = 224;    // 0xE0;
static const unsigned char UTF8_TWO_BYTE_CODE = 224;    // 0xE0;
static const unsigned char UTF8_TWO_BYTE_MASK = 240;    // 0xF0;
static const unsigned char UTF8_THREE_BYTE_CODE = 240;  // 0xF0;
static const unsigned char UTF8_THREE_BYTE_MASK = 248;  // 0xF8;
static const unsigned char UTF8_DATA_BYTE_CODE = 128;   // 0x80;
static const unsigned char UTF8_DATA_BYTE_MASK = 192;   // 0xC0;

// form of all dynamic lists/arrays
// only the list element should be changed
struct loader_generic_list {
    size_t capacity;
    uint32_t count;
    void *list;
};

struct loader_extension_list {
    size_t capacity;
    uint32_t count;
    VkExtensionProperties *list;
};

struct loader_dev_ext_props {
    VkExtensionProperties props;
    uint32_t entrypoint_count;
    char **entrypoints;
};

struct loader_device_extension_list {
    size_t capacity;
    uint32_t count;
    struct loader_dev_ext_props *list;
};

struct loader_name_value {
    char name[MAX_STRING_SIZE];
    char value[MAX_STRING_SIZE];
};

struct loader_layer_functions {
    char str_gipa[MAX_STRING_SIZE];
    char str_gdpa[MAX_STRING_SIZE];
    char str_negotiate_interface[MAX_STRING_SIZE];
    PFN_vkNegotiateLoaderLayerInterfaceVersion negotiate_layer_interface;
    PFN_vkGetInstanceProcAddr get_instance_proc_addr;
    PFN_vkGetDeviceProcAddr get_device_proc_addr;
    PFN_GetPhysicalDeviceProcAddr get_physical_device_proc_addr;
};

struct loader_override_expiration {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
};

// This structure is used to store the json file version in a more manageable way.
typedef struct {
    uint16_t major;
    uint16_t minor;
    uint16_t patch;
} loader_api_version;

// Enumeration used to clearly identify reason for library load failure.
enum loader_layer_library_status {
    LOADER_LAYER_LIB_NOT_LOADED = 0,

    LOADER_LAYER_LIB_SUCCESS_LOADED = 1,

    LOADER_LAYER_LIB_ERROR_WRONG_BIT_TYPE = 20,
    LOADER_LAYER_LIB_ERROR_FAILED_TO_LOAD = 21,
};

enum layer_type_flags {
    VK_LAYER_TYPE_FLAG_INSTANCE_LAYER = 0x1,  // If not set, indicates Device layer
    VK_LAYER_TYPE_FLAG_EXPLICIT_LAYER = 0x2,  // If not set, indicates Implicit layer
    VK_LAYER_TYPE_FLAG_META_LAYER = 0x4,      // If not set, indicates standard layer
};

struct loader_layer_properties {
    VkLayerProperties info;
    enum layer_type_flags type_flags;
    uint32_t interface_version;  // PFN_vkNegotiateLoaderLayerInterfaceVersion
    char manifest_file_name[MAX_STRING_SIZE];
    char lib_name[MAX_STRING_SIZE];
    enum loader_layer_library_status lib_status;
    loader_platform_dl_handle lib_handle;
    struct loader_layer_functions functions;
    struct loader_extension_list instance_extension_list;
    struct loader_device_extension_list device_extension_list;
    struct loader_name_value disable_env_var;
    struct loader_name_value enable_env_var;
    uint32_t num_component_layers;
    char (*component_layer_names)[MAX_STRING_SIZE];
    struct {
        char enumerate_instance_extension_properties[MAX_STRING_SIZE];
        char enumerate_instance_layer_properties[MAX_STRING_SIZE];
        char enumerate_instance_version[MAX_STRING_SIZE];
    } pre_instance_functions;
    uint32_t num_override_paths;
    char (*override_paths)[MAX_STRING_SIZE];
    bool is_override;
    bool has_expiration;
    struct loader_override_expiration expiration;
    bool keep;
    uint32_t num_blacklist_layers;
    char (*blacklist_layer_names)[MAX_STRING_SIZE];
    uint32_t num_app_key_paths;
    char (*app_key_paths)[MAX_STRING_SIZE];
};

struct loader_layer_list {
    size_t capacity;
    uint32_t count;
    struct loader_layer_properties *list;
};

typedef VkResult(VKAPI_PTR *PFN_vkDevExt)(VkDevice device);

struct loader_dev_dispatch_table {
    VkLayerDispatchTable core_dispatch;
    PFN_vkDevExt ext_dispatch[MAX_NUM_UNKNOWN_EXTS];
};

// per CreateDevice structure
struct loader_device {
    struct loader_dev_dispatch_table loader_dispatch;
    VkDevice chain_device;  // device object from the dispatch chain
    VkDevice icd_device;    // device object from the icd
    struct loader_physical_device_term *phys_dev_term;

    // List of activated layers.
    //  app_      is the version based on exactly what the application asked for.
    //            This is what must be returned to the application on Enumerate calls.
    //  expanded_ is the version based on expanding meta-layers into their
    //            individual component layers.  This is what is used internally.
    struct loader_layer_list app_activated_layer_list;
    struct loader_layer_list expanded_activated_layer_list;

    VkAllocationCallbacks alloc_callbacks;

    // List of activated device extensions that have terminators implemented in the loader
    struct {
        bool khr_swapchain_enabled;
        bool khr_display_swapchain_enabled;
        bool khr_device_group_enabled;
        bool ext_debug_marker_enabled;
        bool ext_debug_utils_enabled;
        bool ext_full_screen_exclusive_enabled;
    } extensions;

    struct loader_device *next;
};

// Per ICD information

// Per ICD structure
struct loader_icd_term {
    // pointers to find other structs
    const struct loader_scanned_icd *scanned_icd;
    const struct loader_instance *this_instance;
    struct loader_device *logical_device_list;
    VkInstance instance;  // instance object from the icd
    struct loader_icd_term_dispatch dispatch;

    struct loader_icd_term *next;

    PFN_PhysDevExt phys_dev_ext[MAX_NUM_UNKNOWN_EXTS];
    bool supports_get_dev_prop_2;
};

// Per ICD library structure
struct loader_icd_tramp_list {
    size_t capacity;
    uint32_t count;
    struct loader_scanned_icd *scanned_list;
};

struct loader_instance_dispatch_table {
    VkLayerInstanceDispatchTable layer_inst_disp;  // must be first entry in structure

    // Physical device functions unknown to the loader
    PFN_PhysDevExt phys_dev_ext[MAX_NUM_UNKNOWN_EXTS];
};

// Unique magic number identifier for the loader.
#define LOADER_MAGIC_NUMBER 0x10ADED010110ADEDUL

// Per instance structure
struct loader_instance {
    struct loader_instance_dispatch_table *disp;  // must be first entry in structure
    uint64_t magic;                               // Should be LOADER_MAGIC_NUMBER

    // Vulkan API version the app is intending to use.
    loader_api_version app_api_version;

    // We need to manually track physical devices over time.  If the user
    // re-queries the information, we don't want to delete old data or
    // create new data unless necessary.
    uint32_t total_gpu_count;
    uint32_t phys_dev_count_term;
    struct loader_physical_device_term **phys_devs_term;
    uint32_t phys_dev_count_tramp;
    struct loader_physical_device_tramp **phys_devs_tramp;

    // We also need to manually track physical device groups, but we don't need
    // loader specific structures since we have that content in the physical
    // device stored internal to the public structures.
    uint32_t phys_dev_group_count_term;
    struct VkPhysicalDeviceGroupProperties **phys_dev_groups_term;

    struct loader_instance *next;

    uint32_t total_icd_count;
    struct loader_icd_term *icd_terms;
    struct loader_icd_tramp_list icd_tramp_list;

    uint32_t dev_ext_disp_function_count;
    char *dev_ext_disp_functions[MAX_NUM_UNKNOWN_EXTS];
    uint32_t phys_dev_ext_disp_function_count;
    char *phys_dev_ext_disp_functions[MAX_NUM_UNKNOWN_EXTS];

    struct loader_msg_callback_map_entry *icd_msg_callback_map;

    struct loader_layer_list instance_layer_list;
    bool override_layer_present;

    // List of activated layers.
    //  app_      is the version based on exactly what the application asked for.
    //            This is what must be returned to the application on Enumerate calls.
    //  expanded_ is the version based on expanding meta-layers into their
    //            individual component layers.  This is what is used internally.
    struct loader_layer_list app_activated_layer_list;
    struct loader_layer_list expanded_activated_layer_list;

    VkInstance instance;  // layers/ICD instance returned to trampoline

    struct loader_extension_list ext_list;  // icds and loaders extensions
    struct loader_instance_extension_enables enabled_known_extensions;

    // Stores debug callbacks - used in the log
    VkLayerDbgFunctionNode *DbgFunctionHead;

    // Stores the debug callbacks set during instance creation
    // These are kept separate because they aren't to be used outside of instance creation and destruction
    // So they are swapped out at the end of instance creation and swapped in at instance destruction
    VkLayerDbgFunctionNode *InstanceCreationDeletionDebugFunctionHead;

    VkAllocationCallbacks alloc_callbacks;

    bool portability_enumeration_enabled;

    bool wsi_surface_enabled;
#ifdef VK_USE_PLATFORM_WIN32_KHR
    bool wsi_win32_surface_enabled;
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    bool wsi_wayland_surface_enabled;
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
    bool wsi_xcb_surface_enabled;
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
    bool wsi_xlib_surface_enabled;
#endif
#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
    bool wsi_directfb_surface_enabled;
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    bool wsi_android_surface_enabled;
#endif
#ifdef VK_USE_PLATFORM_MACOS_MVK
    bool wsi_macos_surface_enabled;
#endif
#ifdef VK_USE_PLATFORM_IOS_MVK
    bool wsi_ios_surface_enabled;
#endif
#ifdef VK_USE_PLATFORM_GGP
    bool wsi_ggp_surface_enabled;
#endif
    bool wsi_headless_surface_enabled;
#if defined(VK_USE_PLATFORM_METAL_EXT)
    bool wsi_metal_surface_enabled;
#endif
#ifdef VK_USE_PLATFORM_FUCHSIA
    bool wsi_imagepipe_surface_enabled;
#endif
#ifdef VK_USE_PLATFORM_SCREEN_QNX
    bool wsi_screen_surface_enabled;
#endif
#ifdef VK_USE_PLATFORM_VI_NN
    bool wsi_vi_surface_enabled;
#endif
    bool wsi_display_enabled;
    bool wsi_display_props2_enabled;
    bool create_terminator_invalid_extension;
    bool supports_get_dev_prop_2;
};

// VkPhysicalDevice requires special treatment by loader.  Firstly, terminator
// code must be able to get the struct loader_icd_term to call into the proper
// driver  (multiple ICD/gpu case). This can be accomplished by wrapping the
// created VkPhysicalDevice in loader terminate_EnumeratePhysicalDevices().
// Secondly, the loader must be able to handle wrapped by layer VkPhysicalDevice
// in trampoline code.  This implies, that the loader trampoline code must also
// wrap the VkPhysicalDevice object in trampoline code.  Thus, loader has to
// wrap the VkPhysicalDevice created object twice. In trampoline code it can't
// rely on the terminator object wrapping since a layer may also wrap. Since
// trampoline code wraps the VkPhysicalDevice this means all loader trampoline
// code that passes a VkPhysicalDevice should unwrap it.

// Unique identifier for physical devices
#define PHYS_TRAMP_MAGIC_NUMBER 0x10ADED020210ADEDUL

// Per enumerated PhysicalDevice structure, used to wrap in trampoline code and
// also same structure used to wrap in terminator code
struct loader_physical_device_tramp {
    struct loader_instance_dispatch_table *disp;  // must be first entry in structure
    struct loader_instance *this_instance;
    uint64_t magic;             // Should be PHYS_TRAMP_MAGIC_NUMBER
    VkPhysicalDevice phys_dev;  // object from layers/loader terminator
};

// Per enumerated PhysicalDevice structure, used to wrap in terminator code
struct loader_physical_device_term {
    struct loader_instance_dispatch_table *disp;  // must be first entry in structure
    struct loader_icd_term *this_icd_term;
    uint8_t icd_index;
    VkPhysicalDevice phys_dev;  // object from ICD
};

#ifdef LOADER_ENABLE_LINUX_SORT
// Structure for storing the relevent device information for selecting a device.
// NOTE: Needs to be defined here so we can store this content in the term structrue
//       for quicker sorting.
struct LinuxSortedDeviceInfo {
    // Associated Vulkan Physical Device
    VkPhysicalDevice physical_device;
    bool default_device;

    // Loader specific items about the driver providing support for this physical device
    uint32_t icd_index;
    struct loader_icd_term *icd_term;

    // Some generic device properties
    VkPhysicalDeviceType device_type;
    char device_name[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
    uint32_t vendor_id;
    uint32_t device_id;

    // PCI information on this device
    bool has_pci_bus_info;
    uint32_t pci_domain;
    uint32_t pci_bus;
    uint32_t pci_device;
    uint32_t pci_function;
};
#endif  // LOADER_ENABLE_LINUX_SORT

// Per enumerated PhysicalDeviceGroup structure, used to wrap in terminator code
struct loader_physical_device_group_term {
    struct loader_icd_term *this_icd_term;
    uint8_t icd_index;
    VkPhysicalDeviceGroupProperties group_props;
#ifdef LOADER_ENABLE_LINUX_SORT
    struct LinuxSortedDeviceInfo internal_device_info[VK_MAX_DEVICE_GROUP_SIZE];
#endif  // LOADER_ENABLE_LINUX_SORT
};

struct loader_struct {
    struct loader_instance *instances;
};

struct loader_scanned_icd {
    char *lib_name;
    loader_platform_dl_handle handle;
    uint32_t api_version;
    uint32_t interface_version;
    PFN_vkGetInstanceProcAddr GetInstanceProcAddr;
    PFN_GetPhysicalDeviceProcAddr GetPhysicalDeviceProcAddr;
    PFN_vkCreateInstance CreateInstance;
    PFN_vkEnumerateInstanceExtensionProperties EnumerateInstanceExtensionProperties;
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    PFN_vk_icdEnumerateAdapterPhysicalDevices EnumerateAdapterPhysicalDevices;
#endif
};

enum loader_data_files_type {
    LOADER_DATA_FILE_MANIFEST_DRIVER = 0,
    LOADER_DATA_FILE_MANIFEST_EXPLICIT_LAYER,
    LOADER_DATA_FILE_MANIFEST_IMPLICIT_LAYER,
    LOADER_DATA_FILE_NUM_TYPES  // Not a real field, used for possible loop terminator
};

struct loader_data_files {
    uint32_t count;
    uint32_t alloc_count;
    char **filename_list;
};

struct loader_phys_dev_per_icd {
    uint32_t device_count;
    VkPhysicalDevice *physical_devices;
    uint32_t icd_index;
    struct loader_icd_term *icd_term;
};

struct loader_msg_callback_map_entry {
    VkDebugReportCallbackEXT icd_obj;
    VkDebugReportCallbackEXT loader_obj;
};
