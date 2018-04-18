/*
 * Copyright (c) 2015-2016 The Khronos Group Inc.
 * Copyright (c) 2015-2016 Valve Corporation
 * Copyright (c) 2015-2016 LunarG, Inc.
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
 * Author: Courtney Goeltzenleuchter <courtney@LunarG.com>
 * Author: David Pinedo <david@lunarg.com>
 * Author: Mark Lobodzinski <mark@lunarg.com>
 * Author: Rene Lindsay <rene@lunarg.com>
 * Author: Jeremy Kniager <jeremyk@lunarg.com>
 * Author: Shannon McPherson <shannon@lunarg.com>
 */

#ifdef __GNUC__
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#else
#define strndup(p, n) strdup(p)
#endif

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif  // _WIN32

#if defined(VK_USE_PLATFORM_XLIB_KHR) || defined(VK_USE_PLATFORM_XCB_KHR)
#include <X11/Xutil.h>
#endif

#if defined(VK_USE_PLATFORM_MIR_KHR)
#warning "Vulkaninfo does not have code for Mir at this time"
#endif

#include <vulkan/vulkan.h>

#define ERR(err) fprintf(stderr, "%s:%d: failed with %s\n", __FILE__, __LINE__, VkResultString(err));

#ifdef _WIN32

#define snprintf _snprintf
#define strdup _strdup

// Returns nonzero if the console is used only for this process. Will return
// zero if another process (such as cmd.exe) is also attached.
static int ConsoleIsExclusive(void) {
    DWORD pids[2];
    DWORD num_pids = GetConsoleProcessList(pids, ARRAYSIZE(pids));
    return num_pids <= 1;
}

#define WAIT_FOR_CONSOLE_DESTROY                   \
    do {                                           \
        if (ConsoleIsExclusive()) Sleep(INFINITE); \
    } while (0)
#else
#define WAIT_FOR_CONSOLE_DESTROY
#endif

#define ERR_EXIT(err)             \
    do {                          \
        ERR(err);                 \
        fflush(stdout);           \
        fflush(stderr);           \
        WAIT_FOR_CONSOLE_DESTROY; \
        exit(-1);                 \
    } while (0)

#if defined(NDEBUG) && defined(__GNUC__)
#define U_ASSERT_ONLY __attribute__((unused))
#else
#define U_ASSERT_ONLY
#endif

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define MAX_QUEUE_TYPES 5
#define APP_SHORT_NAME "vulkaninfo"

static bool html_output = false;
static bool human_readable_output = true;
static bool json_output = false;
static uint32_t selected_gpu = 0;

struct VkStructureHeader {
    VkStructureType sType;
    void *pNext;
};

struct pNextChainBuildingBlockInfo {
    VkStructureType sType;
    uint32_t mem_size;
};

struct LayerExtensionList {
    VkLayerProperties layer_properties;
    uint32_t extension_count;
    VkExtensionProperties *extension_properties;
};

struct AppInstance {
    VkInstance instance;
    uint32_t global_layer_count;
    struct LayerExtensionList *global_layers;
    uint32_t global_extension_count;
    VkExtensionProperties *global_extensions;  // Instance Extensions

    const char **inst_extensions;
    uint32_t inst_extensions_count;

    PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
    PFN_vkGetPhysicalDeviceProperties2KHR vkGetPhysicalDeviceProperties2KHR;
    PFN_vkGetPhysicalDeviceFormatProperties2KHR vkGetPhysicalDeviceFormatProperties2KHR;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR vkGetPhysicalDeviceQueueFamilyProperties2KHR;
    PFN_vkGetPhysicalDeviceFeatures2KHR vkGetPhysicalDeviceFeatures2KHR;
    PFN_vkGetPhysicalDeviceMemoryProperties2KHR vkGetPhysicalDeviceMemoryProperties2KHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR vkGetPhysicalDeviceSurfaceCapabilities2KHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT vkGetPhysicalDeviceSurfaceCapabilities2EXT;

    VkSurfaceCapabilitiesKHR surface_capabilities;
    VkSurfaceCapabilities2KHR surface_capabilities2;
    VkSharedPresentSurfaceCapabilitiesKHR shared_surface_capabilities;
    VkSurfaceCapabilities2EXT surface_capabilities2_ext;

    VkSurfaceKHR surface;
    int width, height;

#ifdef VK_USE_PLATFORM_WIN32_KHR
    HINSTANCE h_instance;  // Windows Instance
    HWND h_wnd;            // window handle
#elif VK_USE_PLATFORM_XCB_KHR
    xcb_connection_t *xcb_connection;
    xcb_screen_t *xcb_screen;
    xcb_window_t xcb_window;
#elif VK_USE_PLATFORM_XLIB_KHR
    Display *xlib_display;
    Window xlib_window;
#elif VK_USE_PLATFORM_ANDROID_KHR  // TODO
    ANativeWindow *window;
#endif
};

struct AppGpu {
    uint32_t id;
    VkPhysicalDevice obj;

    VkPhysicalDeviceProperties props;
    VkPhysicalDeviceProperties2KHR props2;

    uint32_t queue_count;
    VkQueueFamilyProperties *queue_props;
    VkQueueFamilyProperties2KHR *queue_props2;
    VkDeviceQueueCreateInfo *queue_reqs;

    struct AppInstance *inst;

    VkPhysicalDeviceMemoryProperties memory_props;
    VkPhysicalDeviceMemoryProperties2KHR memory_props2;

    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceFeatures2KHR features2;
    VkPhysicalDevice limits;

    uint32_t device_extension_count;
    VkExtensionProperties *device_extensions;
};

static VKAPI_ATTR VkBool32 VKAPI_CALL DbgCallback(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject,
                                                  size_t location, int32_t msgCode, const char *pLayerPrefix, const char *pMsg,
                                                  void *pUserData) {
    char *message = (char *)malloc(strlen(pMsg) + 100);

    assert(message);

    if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        sprintf(message, "ERROR: [%s] Code %d : %s", pLayerPrefix, msgCode, pMsg);
    } else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        sprintf(message, "WARNING: [%s] Code %d : %s", pLayerPrefix, msgCode, pMsg);
    } else if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
        sprintf(message, "INFO: [%s] Code %d : %s", pLayerPrefix, msgCode, pMsg);
    } else if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
        sprintf(message, "DEBUG: [%s] Code %d : %s", pLayerPrefix, msgCode, pMsg);
    }

    printf("%s\n", message);
    fflush(stdout);
    free(message);

    /*
     * false indicates that layer should not bail-out of an
     * API call that had validation failures. This may mean that the
     * app dies inside the driver due to invalid parameter(s).
     * That's what would happen without validation layers, so we'll
     * keep that behavior here.
     */
    return false;
}

static const char *VkResultString(VkResult err) {
    switch (err) {
#define STR(r) \
    case r:    \
        return #r
        STR(VK_SUCCESS);
        STR(VK_NOT_READY);
        STR(VK_TIMEOUT);
        STR(VK_EVENT_SET);
        STR(VK_EVENT_RESET);
        STR(VK_ERROR_INITIALIZATION_FAILED);
        STR(VK_ERROR_OUT_OF_HOST_MEMORY);
        STR(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        STR(VK_ERROR_DEVICE_LOST);
        STR(VK_ERROR_LAYER_NOT_PRESENT);
        STR(VK_ERROR_EXTENSION_NOT_PRESENT);
        STR(VK_ERROR_MEMORY_MAP_FAILED);
        STR(VK_ERROR_INCOMPATIBLE_DRIVER);
#undef STR
        default:
            return "UNKNOWN_RESULT";
    }
}

static const char *VkPhysicalDeviceTypeString(VkPhysicalDeviceType type) {
    switch (type) {
#define STR(r)                        \
    case VK_PHYSICAL_DEVICE_TYPE_##r: \
        return #r
        STR(OTHER);
        STR(INTEGRATED_GPU);
        STR(DISCRETE_GPU);
        STR(VIRTUAL_GPU);
        STR(CPU);
#undef STR
        default:
            return "UNKNOWN_DEVICE";
    }
}

static const char *VkFormatString(VkFormat fmt) {
    switch (fmt) {
#define STR(r)          \
    case VK_FORMAT_##r: \
        return #r
        STR(UNDEFINED);
        STR(R4G4_UNORM_PACK8);
        STR(R4G4B4A4_UNORM_PACK16);
        STR(B4G4R4A4_UNORM_PACK16);
        STR(R5G6B5_UNORM_PACK16);
        STR(B5G6R5_UNORM_PACK16);
        STR(R5G5B5A1_UNORM_PACK16);
        STR(B5G5R5A1_UNORM_PACK16);
        STR(A1R5G5B5_UNORM_PACK16);
        STR(R8_UNORM);
        STR(R8_SNORM);
        STR(R8_USCALED);
        STR(R8_SSCALED);
        STR(R8_UINT);
        STR(R8_SINT);
        STR(R8_SRGB);
        STR(R8G8_UNORM);
        STR(R8G8_SNORM);
        STR(R8G8_USCALED);
        STR(R8G8_SSCALED);
        STR(R8G8_UINT);
        STR(R8G8_SINT);
        STR(R8G8_SRGB);
        STR(R8G8B8_UNORM);
        STR(R8G8B8_SNORM);
        STR(R8G8B8_USCALED);
        STR(R8G8B8_SSCALED);
        STR(R8G8B8_UINT);
        STR(R8G8B8_SINT);
        STR(R8G8B8_SRGB);
        STR(B8G8R8_UNORM);
        STR(B8G8R8_SNORM);
        STR(B8G8R8_USCALED);
        STR(B8G8R8_SSCALED);
        STR(B8G8R8_UINT);
        STR(B8G8R8_SINT);
        STR(B8G8R8_SRGB);
        STR(R8G8B8A8_UNORM);
        STR(R8G8B8A8_SNORM);
        STR(R8G8B8A8_USCALED);
        STR(R8G8B8A8_SSCALED);
        STR(R8G8B8A8_UINT);
        STR(R8G8B8A8_SINT);
        STR(R8G8B8A8_SRGB);
        STR(B8G8R8A8_UNORM);
        STR(B8G8R8A8_SNORM);
        STR(B8G8R8A8_USCALED);
        STR(B8G8R8A8_SSCALED);
        STR(B8G8R8A8_UINT);
        STR(B8G8R8A8_SINT);
        STR(B8G8R8A8_SRGB);
        STR(A8B8G8R8_UNORM_PACK32);
        STR(A8B8G8R8_SNORM_PACK32);
        STR(A8B8G8R8_USCALED_PACK32);
        STR(A8B8G8R8_SSCALED_PACK32);
        STR(A8B8G8R8_UINT_PACK32);
        STR(A8B8G8R8_SINT_PACK32);
        STR(A8B8G8R8_SRGB_PACK32);
        STR(A2R10G10B10_UNORM_PACK32);
        STR(A2R10G10B10_SNORM_PACK32);
        STR(A2R10G10B10_USCALED_PACK32);
        STR(A2R10G10B10_SSCALED_PACK32);
        STR(A2R10G10B10_UINT_PACK32);
        STR(A2R10G10B10_SINT_PACK32);
        STR(A2B10G10R10_UNORM_PACK32);
        STR(A2B10G10R10_SNORM_PACK32);
        STR(A2B10G10R10_USCALED_PACK32);
        STR(A2B10G10R10_SSCALED_PACK32);
        STR(A2B10G10R10_UINT_PACK32);
        STR(A2B10G10R10_SINT_PACK32);
        STR(R16_UNORM);
        STR(R16_SNORM);
        STR(R16_USCALED);
        STR(R16_SSCALED);
        STR(R16_UINT);
        STR(R16_SINT);
        STR(R16_SFLOAT);
        STR(R16G16_UNORM);
        STR(R16G16_SNORM);
        STR(R16G16_USCALED);
        STR(R16G16_SSCALED);
        STR(R16G16_UINT);
        STR(R16G16_SINT);
        STR(R16G16_SFLOAT);
        STR(R16G16B16_UNORM);
        STR(R16G16B16_SNORM);
        STR(R16G16B16_USCALED);
        STR(R16G16B16_SSCALED);
        STR(R16G16B16_UINT);
        STR(R16G16B16_SINT);
        STR(R16G16B16_SFLOAT);
        STR(R16G16B16A16_UNORM);
        STR(R16G16B16A16_SNORM);
        STR(R16G16B16A16_USCALED);
        STR(R16G16B16A16_SSCALED);
        STR(R16G16B16A16_UINT);
        STR(R16G16B16A16_SINT);
        STR(R16G16B16A16_SFLOAT);
        STR(R32_UINT);
        STR(R32_SINT);
        STR(R32_SFLOAT);
        STR(R32G32_UINT);
        STR(R32G32_SINT);
        STR(R32G32_SFLOAT);
        STR(R32G32B32_UINT);
        STR(R32G32B32_SINT);
        STR(R32G32B32_SFLOAT);
        STR(R32G32B32A32_UINT);
        STR(R32G32B32A32_SINT);
        STR(R32G32B32A32_SFLOAT);
        STR(R64_UINT);
        STR(R64_SINT);
        STR(R64_SFLOAT);
        STR(R64G64_UINT);
        STR(R64G64_SINT);
        STR(R64G64_SFLOAT);
        STR(R64G64B64_UINT);
        STR(R64G64B64_SINT);
        STR(R64G64B64_SFLOAT);
        STR(R64G64B64A64_UINT);
        STR(R64G64B64A64_SINT);
        STR(R64G64B64A64_SFLOAT);
        STR(B10G11R11_UFLOAT_PACK32);
        STR(E5B9G9R9_UFLOAT_PACK32);
        STR(D16_UNORM);
        STR(X8_D24_UNORM_PACK32);
        STR(D32_SFLOAT);
        STR(S8_UINT);
        STR(D16_UNORM_S8_UINT);
        STR(D24_UNORM_S8_UINT);
        STR(D32_SFLOAT_S8_UINT);
        STR(BC1_RGB_UNORM_BLOCK);
        STR(BC1_RGB_SRGB_BLOCK);
        STR(BC1_RGBA_UNORM_BLOCK);
        STR(BC1_RGBA_SRGB_BLOCK);
        STR(BC2_UNORM_BLOCK);
        STR(BC2_SRGB_BLOCK);
        STR(BC3_UNORM_BLOCK);
        STR(BC3_SRGB_BLOCK);
        STR(BC4_UNORM_BLOCK);
        STR(BC4_SNORM_BLOCK);
        STR(BC5_UNORM_BLOCK);
        STR(BC5_SNORM_BLOCK);
        STR(BC6H_UFLOAT_BLOCK);
        STR(BC6H_SFLOAT_BLOCK);
        STR(BC7_UNORM_BLOCK);
        STR(BC7_SRGB_BLOCK);
        STR(ETC2_R8G8B8_UNORM_BLOCK);
        STR(ETC2_R8G8B8_SRGB_BLOCK);
        STR(ETC2_R8G8B8A1_UNORM_BLOCK);
        STR(ETC2_R8G8B8A1_SRGB_BLOCK);
        STR(ETC2_R8G8B8A8_UNORM_BLOCK);
        STR(ETC2_R8G8B8A8_SRGB_BLOCK);
        STR(EAC_R11_UNORM_BLOCK);
        STR(EAC_R11_SNORM_BLOCK);
        STR(EAC_R11G11_UNORM_BLOCK);
        STR(EAC_R11G11_SNORM_BLOCK);
        STR(ASTC_4x4_UNORM_BLOCK);
        STR(ASTC_4x4_SRGB_BLOCK);
        STR(ASTC_5x4_UNORM_BLOCK);
        STR(ASTC_5x4_SRGB_BLOCK);
        STR(ASTC_5x5_UNORM_BLOCK);
        STR(ASTC_5x5_SRGB_BLOCK);
        STR(ASTC_6x5_UNORM_BLOCK);
        STR(ASTC_6x5_SRGB_BLOCK);
        STR(ASTC_6x6_UNORM_BLOCK);
        STR(ASTC_6x6_SRGB_BLOCK);
        STR(ASTC_8x5_UNORM_BLOCK);
        STR(ASTC_8x5_SRGB_BLOCK);
        STR(ASTC_8x6_UNORM_BLOCK);
        STR(ASTC_8x6_SRGB_BLOCK);
        STR(ASTC_8x8_UNORM_BLOCK);
        STR(ASTC_8x8_SRGB_BLOCK);
        STR(ASTC_10x5_UNORM_BLOCK);
        STR(ASTC_10x5_SRGB_BLOCK);
        STR(ASTC_10x6_UNORM_BLOCK);
        STR(ASTC_10x6_SRGB_BLOCK);
        STR(ASTC_10x8_UNORM_BLOCK);
        STR(ASTC_10x8_SRGB_BLOCK);
        STR(ASTC_10x10_UNORM_BLOCK);
        STR(ASTC_10x10_SRGB_BLOCK);
        STR(ASTC_12x10_UNORM_BLOCK);
        STR(ASTC_12x10_SRGB_BLOCK);
        STR(ASTC_12x12_UNORM_BLOCK);
        STR(ASTC_12x12_SRGB_BLOCK);
#undef STR
        default:
            return "UNKNOWN_FORMAT";
    }
}
#if defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR) || defined(VK_USE_PLATFORM_WIN32_KHR)
static const char *VkPresentModeString(VkPresentModeKHR mode) {
    switch (mode) {
#define STR(r)                \
    case VK_PRESENT_MODE_##r: \
        return #r
        STR(IMMEDIATE_KHR);
        STR(MAILBOX_KHR);
        STR(FIFO_KHR);
        STR(FIFO_RELAXED_KHR);
#undef STR
        default:
            return "UNKNOWN_FORMAT";
    }
}
#endif

static bool CheckExtensionEnabled(const char *extension_to_check, const char **extension_list, uint32_t extension_count) {
    for (uint32_t i = 0; i < extension_count; ++i) {
        if (!strcmp(extension_to_check, extension_list[i])) {
            return true;
        }
    }
    return false;
}

static bool CheckPhysicalDeviceExtensionIncluded(const char *extension_to_check, VkExtensionProperties *extension_list,
                                                 uint32_t extension_count) {
    for (uint32_t i = 0; i < extension_count; ++i) {
        if (!strcmp(extension_to_check, extension_list[i].extensionName)) {
            return true;
        }
    }
    return false;
}

static void buildpNextChain(struct VkStructureHeader *first, const struct pNextChainBuildingBlockInfo *chain_info,
                            uint32_t chain_info_len) {
    struct VkStructureHeader *place = first;

    for (uint32_t i = 0; i < chain_info_len; i++) {
        place->pNext = malloc(chain_info[i].mem_size);
        if (!place->pNext) {
            ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
        }
        place = place->pNext;
        place->sType = chain_info[i].sType;
    }

    place->pNext = NULL;
}

static void freepNextChain(struct VkStructureHeader *first) {
    struct VkStructureHeader *place = first;
    struct VkStructureHeader *next = NULL;

    while (place) {
        next = place->pNext;
        free(place);
        place = next;
    }
}

static void ExtractVersion(uint32_t version, uint32_t *major, uint32_t *minor, uint32_t *patch) {
    *major = version >> 22;
    *minor = (version >> 12) & 0x3ff;
    *patch = version & 0xfff;
}

static void AppGetPhysicalDeviceLayerExtensions(struct AppGpu *gpu, char *layer_name, uint32_t *extension_count,
                                                VkExtensionProperties **extension_properties) {
    VkResult err;
    uint32_t ext_count = 0;
    VkExtensionProperties *ext_ptr = NULL;

    /* repeat get until VK_INCOMPLETE goes away */
    do {
        err = vkEnumerateDeviceExtensionProperties(gpu->obj, layer_name, &ext_count, NULL);
        assert(!err);

        if (ext_ptr) {
            free(ext_ptr);
        }
        ext_ptr = malloc(ext_count * sizeof(VkExtensionProperties));
        err = vkEnumerateDeviceExtensionProperties(gpu->obj, layer_name, &ext_count, ext_ptr);
    } while (err == VK_INCOMPLETE);
    assert(!err);

    *extension_count = ext_count;
    *extension_properties = ext_ptr;
}

static void AppGetGlobalLayerExtensions(char *layer_name, uint32_t *extension_count, VkExtensionProperties **extension_properties) {
    VkResult err;
    uint32_t ext_count = 0;
    VkExtensionProperties *ext_ptr = NULL;

    /* repeat get until VK_INCOMPLETE goes away */
    do {
        // gets the extension count if the last parameter is NULL
        err = vkEnumerateInstanceExtensionProperties(layer_name, &ext_count, NULL);
        assert(!err);

        if (ext_ptr) {
            free(ext_ptr);
        }
        ext_ptr = malloc(ext_count * sizeof(VkExtensionProperties));
        // gets the extension properties if the last parameter is not NULL
        err = vkEnumerateInstanceExtensionProperties(layer_name, &ext_count, ext_ptr);
    } while (err == VK_INCOMPLETE);
    assert(!err);
    *extension_count = ext_count;
    *extension_properties = ext_ptr;
}

/* Gets a list of layer and instance extensions */
static void AppGetInstanceExtensions(struct AppInstance *inst) {
    VkResult U_ASSERT_ONLY err;

    uint32_t count = 0;

    /* Scan layers */
    VkLayerProperties *global_layer_properties = NULL;
    struct LayerExtensionList *global_layers = NULL;

    do {
        err = vkEnumerateInstanceLayerProperties(&count, NULL);
        assert(!err);

        if (global_layer_properties) {
            free(global_layer_properties);
        }
        global_layer_properties = malloc(sizeof(VkLayerProperties) * count);
        assert(global_layer_properties);

        if (global_layers) {
            free(global_layers);
        }
        global_layers = malloc(sizeof(struct LayerExtensionList) * count);
        assert(global_layers);

        err = vkEnumerateInstanceLayerProperties(&count, global_layer_properties);
    } while (err == VK_INCOMPLETE);
    assert(!err);

    inst->global_layer_count = count;
    inst->global_layers = global_layers;

    for (uint32_t i = 0; i < inst->global_layer_count; ++i) {
        VkLayerProperties *src_info = &global_layer_properties[i];
        struct LayerExtensionList *dst_info = &inst->global_layers[i];
        memcpy(&dst_info->layer_properties, src_info, sizeof(VkLayerProperties));

        // Save away layer extension info for report
        // Gets layer extensions, if first parameter is not NULL
        AppGetGlobalLayerExtensions(src_info->layerName, &dst_info->extension_count, &dst_info->extension_properties);
    }
    free(global_layer_properties);

    // Collect global extensions
    inst->global_extension_count = 0;
    // Gets instance extensions, if no layer was specified in the first
    // paramteter
    AppGetGlobalLayerExtensions(NULL, &inst->global_extension_count, &inst->global_extensions);
}

// Prints opening code for html output file
void PrintHtmlHeader(FILE *out) {
    fprintf(out, "<!doctype html>\n");
    fprintf(out, "<html>\n");
    fprintf(out, "\t<head>\n");
    fprintf(out, "\t\t<title>Vulkan Info</title>\n");
    fprintf(out, "\t\t<style type='text/css'>\n");
    fprintf(out, "\t\thtml {\n");
    fprintf(out, "\t\t\tbackground-color: #0b1e48;\n");
    fprintf(out, "\t\t\tbackground-image: url(\"https://vulkan.lunarg.com/img/bg-starfield.jpg\");\n");
    fprintf(out, "\t\t\tbackground-position: center;\n");
    fprintf(out, "\t\t\t-webkit-background-size: cover;\n");
    fprintf(out, "\t\t\t-moz-background-size: cover;\n");
    fprintf(out, "\t\t\t-o-background-size: cover;\n");
    fprintf(out, "\t\t\tbackground-size: cover;\n");
    fprintf(out, "\t\t\tbackground-attachment: fixed;\n");
    fprintf(out, "\t\t\tbackground-repeat: no-repeat;\n");
    fprintf(out, "\t\t\theight: 100%%;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\t#header {\n");
    fprintf(out, "\t\t\tz-index: -1;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\t#header>img {\n");
    fprintf(out, "\t\t\tposition: absolute;\n");
    fprintf(out, "\t\t\twidth: 160px;\n");
    fprintf(out, "\t\t\tmargin-left: -280px;\n");
    fprintf(out, "\t\t\ttop: -10px;\n");
    fprintf(out, "\t\t\tleft: 50%%;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\t#header>h1 {\n");
    fprintf(out, "\t\t\tfont-family: Arial, \"Helvetica Neue\", Helvetica, sans-serif;\n");
    fprintf(out, "\t\t\tfont-size: 44px;\n");
    fprintf(out, "\t\t\tfont-weight: 200;\n");
    fprintf(out, "\t\t\ttext-shadow: 4px 4px 5px #000;\n");
    fprintf(out, "\t\t\tcolor: #eee;\n");
    fprintf(out, "\t\t\tposition: absolute;\n");
    fprintf(out, "\t\t\twidth: 400px;\n");
    fprintf(out, "\t\t\tmargin-left: -80px;\n");
    fprintf(out, "\t\t\ttop: 8px;\n");
    fprintf(out, "\t\t\tleft: 50%%;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\tbody {\n");
    fprintf(out, "\t\t\tfont-family: Consolas, monaco, monospace;\n");
    fprintf(out, "\t\t\tfont-size: 14px;\n");
    fprintf(out, "\t\t\tline-height: 20px;\n");
    fprintf(out, "\t\t\tcolor: #eee;\n");
    fprintf(out, "\t\t\theight: 100%%;\n");
    fprintf(out, "\t\t\tmargin: 0;\n");
    fprintf(out, "\t\t\toverflow: hidden;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\t#wrapper {\n");
    fprintf(out, "\t\t\tbackground-color: rgba(0, 0, 0, 0.7);\n");
    fprintf(out, "\t\t\tborder: 1px solid #446;\n");
    fprintf(out, "\t\t\tbox-shadow: 0px 0px 10px #000;\n");
    fprintf(out, "\t\t\tpadding: 8px 12px;\n\n");
    fprintf(out, "\t\t\tdisplay: inline-block;\n");
    fprintf(out, "\t\t\tposition: absolute;\n");
    fprintf(out, "\t\t\ttop: 80px;\n");
    fprintf(out, "\t\t\tbottom: 25px;\n");
    fprintf(out, "\t\t\tleft: 50px;\n");
    fprintf(out, "\t\t\tright: 50px;\n");
    fprintf(out, "\t\t\toverflow: auto;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\tdetails>details {\n");
    fprintf(out, "\t\t\tmargin-left: 22px;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\tdetails>summary:only-child::-webkit-details-marker {\n");
    fprintf(out, "\t\t\tdisplay: none;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\t.var, .type, .val {\n");
    fprintf(out, "\t\t\tdisplay: inline;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\t.var {\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\t.type {\n");
    fprintf(out, "\t\t\tcolor: #acf;\n");
    fprintf(out, "\t\t\tmargin: 0 12px;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\t.val {\n");
    fprintf(out, "\t\t\tcolor: #afa;\n");
    fprintf(out, "\t\t\tbackground: #222;\n");
    fprintf(out, "\t\t\ttext-align: right;\n");
    fprintf(out, "\t\t}\n");
    fprintf(out, "\t\t</style>\n");
    fprintf(out, "\t</head>\n");
    fprintf(out, "\t<body>\n");
    fprintf(out, "\t\t<div id='header'>\n");
    fprintf(out, "\t\t\t<h1>Vulkan Info</h1>\n");
    fprintf(out, "\t\t</div>\n");
    fprintf(out, "\t\t<div id='wrapper'>\n");
}

// Prints closing code for html output file
void PrintHtmlFooter(FILE *out) {
    fprintf(out, "\t\t</div>\n");
    fprintf(out, "\t</body>\n");
    fprintf(out, "</html>");
}

// Prints opening code for json output file
void PrintJsonHeader(const int vulkan_major, const int vulkan_minor, const int vulkan_patch) {
    printf("{\n");
    printf("\t\"$schema\": \"https://schema.khronos.org/vulkan/devsim_1_0_0.json#\",\n");
    printf("\t\"comments\": {\n");
    printf("\t\t\"desc\": \"JSON configuration file describing GPU %u. Generated using the VulkanInfo program.\",\n", selected_gpu);
    printf("\t\t\"vulkanApiVersion\": \"%d.%d.%d\"\n", vulkan_major, vulkan_minor, vulkan_patch);
    printf("\t}");
}

// Checks if current argument specifies json output, interprets/updates gpu selection
void CheckForJsonOption(const char *arg) {
    if (strncmp("--json", arg, 6) == 0 || strcmp(arg, "-j") == 0) {
        if (strlen(arg) > 7 && strncmp("--json=", arg, 7) == 0) {
            selected_gpu = strtol(arg + 7, NULL, 10);
        }
        human_readable_output = false;
        json_output = true;
    }
}

// static void AppCreateInstance(struct AppInstance *inst, int argc, ...) {
static void AppCreateInstance(struct AppInstance *inst) {
    AppGetInstanceExtensions(inst);

    //---Build a list of extensions to load---

    const char *info_instance_extensions[] = {VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME,
                                              VK_KHR_DISPLAY_EXTENSION_NAME,
                                              VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
                                              VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME,
                                              VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME,
                                              VK_KHR_SURFACE_EXTENSION_NAME,
                                              VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME,
#ifdef VK_USE_PLATFORM_WIN32_KHR
                                              VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif VK_USE_PLATFORM_XCB_KHR
                                              VK_KHR_XCB_SURFACE_EXTENSION_NAME
#elif VK_USE_PLATFORM_XLIB_KHR
                                              VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#elif VK_USE_PLATFORM_WAYLAND_KHR
                                              VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME
#elif VK_USE_PLATFORM_ANDROID_KHR
                                              VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
#endif
    };
    const uint32_t info_instance_extensions_count = ARRAY_SIZE(info_instance_extensions);
    inst->inst_extensions = malloc(sizeof(char *) * ARRAY_SIZE(info_instance_extensions));
    inst->inst_extensions_count = 0;

    for (uint32_t k = 0; (k < info_instance_extensions_count); ++k) {
        for (uint32_t j = 0; (j < inst->global_extension_count); ++j) {
            const char *found_name = inst->global_extensions[j].extensionName;
            if (!strcmp(info_instance_extensions[k], found_name)) {
                inst->inst_extensions[inst->inst_extensions_count++] = info_instance_extensions[k];
                break;
            }
        }
    }

    //----------------------------------------

    const VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = APP_SHORT_NAME,
        .applicationVersion = 1,
        .pEngineName = APP_SHORT_NAME,
        .engineVersion = 1,
        .apiVersion = VK_API_VERSION_1_0,
    };

    VkInstanceCreateInfo inst_info = {.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                      .pNext = NULL,
                                      .pApplicationInfo = &app_info,
                                      .enabledLayerCount = 0,
                                      .ppEnabledLayerNames = NULL,
                                      .enabledExtensionCount = inst->inst_extensions_count,
                                      .ppEnabledExtensionNames = inst->inst_extensions};

    VkDebugReportCallbackCreateInfoEXT dbg_info;
    memset(&dbg_info, 0, sizeof(dbg_info));
    dbg_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    dbg_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    dbg_info.pfnCallback = DbgCallback;
    inst_info.pNext = &dbg_info;

    VkResult U_ASSERT_ONLY err;
    err = vkCreateInstance(&inst_info, NULL, &inst->instance);
    if (err == VK_ERROR_INCOMPATIBLE_DRIVER) {
        fprintf(stderr, "Cannot create Vulkan instance.\n");
        ERR_EXIT(err);
    } else if (err) {
        ERR_EXIT(err);
    }

    inst->vkGetPhysicalDeviceSurfaceSupportKHR =
        (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)vkGetInstanceProcAddr(inst->instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
    inst->vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)vkGetInstanceProcAddr(
        inst->instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    inst->vkGetPhysicalDeviceSurfaceFormatsKHR =
        (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)vkGetInstanceProcAddr(inst->instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
    inst->vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)vkGetInstanceProcAddr(
        inst->instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
    inst->vkGetPhysicalDeviceProperties2KHR =
        (PFN_vkGetPhysicalDeviceProperties2KHR)vkGetInstanceProcAddr(inst->instance, "vkGetPhysicalDeviceProperties2KHR");
    inst->vkGetPhysicalDeviceFormatProperties2KHR = (PFN_vkGetPhysicalDeviceFormatProperties2KHR)vkGetInstanceProcAddr(
        inst->instance, "vkGetPhysicalDeviceFormatProperties2KHR");
    inst->vkGetPhysicalDeviceQueueFamilyProperties2KHR = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR)vkGetInstanceProcAddr(
        inst->instance, "vkGetPhysicalDeviceQueueFamilyProperties2KHR");
    inst->vkGetPhysicalDeviceFeatures2KHR =
        (PFN_vkGetPhysicalDeviceFeatures2KHR)vkGetInstanceProcAddr(inst->instance, "vkGetPhysicalDeviceFeatures2KHR");
    inst->vkGetPhysicalDeviceMemoryProperties2KHR = (PFN_vkGetPhysicalDeviceMemoryProperties2KHR)vkGetInstanceProcAddr(
        inst->instance, "vkGetPhysicalDeviceMemoryProperties2KHR");
    inst->vkGetPhysicalDeviceSurfaceCapabilities2KHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR)vkGetInstanceProcAddr(
        inst->instance, "vkGetPhysicalDeviceSurfaceCapabilities2KHR");
    inst->vkGetPhysicalDeviceSurfaceCapabilities2EXT = (PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT)vkGetInstanceProcAddr(
        inst->instance, "vkGetPhysicalDeviceSurfaceCapabilities2EXT");
}

//-----------------------------------------------------------

static void AppDestroyInstance(struct AppInstance *inst) {
    free(inst->global_extensions);
    for (uint32_t i = 0; i < inst->global_layer_count; ++i) {
        free(inst->global_layers[i].extension_properties);
    }
    free(inst->global_layers);
    free((char **)inst->inst_extensions);
    vkDestroyInstance(inst->instance, NULL);
}

static void AppGpuInit(struct AppGpu *gpu, struct AppInstance *inst, uint32_t id, VkPhysicalDevice obj) {
    uint32_t i;

    memset(gpu, 0, sizeof(*gpu));

    gpu->id = id;
    gpu->obj = obj;
    gpu->inst = inst;

    vkGetPhysicalDeviceProperties(gpu->obj, &gpu->props);

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                              gpu->inst->inst_extensions_count)) {
        struct pNextChainBuildingBlockInfo chain_info[] = {
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES_KHR,
             .mem_size = sizeof(VkPhysicalDevicePointClippingPropertiesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR,
             .mem_size = sizeof(VkPhysicalDevicePushDescriptorPropertiesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISCARD_RECTANGLE_PROPERTIES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceDiscardRectanglePropertiesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR,
             .mem_size = sizeof(VkPhysicalDeviceMultiviewPropertiesKHR)}};

        uint32_t chain_info_len = ARRAY_SIZE(chain_info);

        gpu->props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR;
        buildpNextChain((struct VkStructureHeader *)&gpu->props2, chain_info, chain_info_len);

        inst->vkGetPhysicalDeviceProperties2KHR(gpu->obj, &gpu->props2);
    }

    /* get queue count */
    vkGetPhysicalDeviceQueueFamilyProperties(gpu->obj, &gpu->queue_count, NULL);

    gpu->queue_props = malloc(sizeof(gpu->queue_props[0]) * gpu->queue_count);

    if (!gpu->queue_props) {
        ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
    }
    vkGetPhysicalDeviceQueueFamilyProperties(gpu->obj, &gpu->queue_count, gpu->queue_props);

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                              gpu->inst->inst_extensions_count)) {
        gpu->queue_props2 = malloc(sizeof(gpu->queue_props2[0]) * gpu->queue_count);

        if (!gpu->queue_props2) {
            ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
        }

        for (i = 0; i < gpu->queue_count; ++i) {
            gpu->queue_props2[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2_KHR;
            gpu->queue_props2[i].pNext = NULL;
        }

        inst->vkGetPhysicalDeviceQueueFamilyProperties2KHR(gpu->obj, &gpu->queue_count, gpu->queue_props2);
    }

    /* set up queue requests */
    gpu->queue_reqs = malloc(sizeof(*gpu->queue_reqs) * gpu->queue_count);
    if (!gpu->queue_reqs) {
        ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
    }
    for (i = 0; i < gpu->queue_count; ++i) {
        float *queue_priorities = NULL;
        if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                                  gpu->inst->inst_extensions_count)) {
            queue_priorities = malloc(gpu->queue_props2[i].queueFamilyProperties.queueCount * sizeof(float));
        } else {
            queue_priorities = malloc(gpu->queue_props[i].queueCount * sizeof(float));
        }
        if (!queue_priorities) {
            ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
        }

        if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                                  gpu->inst->inst_extensions_count)) {
            memset(queue_priorities, 0, gpu->queue_props2[i].queueFamilyProperties.queueCount * sizeof(float));
        } else {
            memset(queue_priorities, 0, gpu->queue_props[i].queueCount * sizeof(float));
        }

        gpu->queue_reqs[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        gpu->queue_reqs[i].pNext = NULL;
        gpu->queue_reqs[i].flags = 0;
        gpu->queue_reqs[i].queueFamilyIndex = i;

        if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                                  gpu->inst->inst_extensions_count)) {
            gpu->queue_reqs[i].queueCount = gpu->queue_props2[i].queueFamilyProperties.queueCount;
        } else {
            gpu->queue_reqs[i].queueCount = gpu->queue_props[i].queueCount;
        }

        gpu->queue_reqs[i].pQueuePriorities = queue_priorities;
    }

    vkGetPhysicalDeviceMemoryProperties(gpu->obj, &gpu->memory_props);

    vkGetPhysicalDeviceFeatures(gpu->obj, &gpu->features);

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                              gpu->inst->inst_extensions_count)) {
        gpu->memory_props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2_KHR;
        gpu->memory_props2.pNext = NULL;

        inst->vkGetPhysicalDeviceMemoryProperties2KHR(gpu->obj, &gpu->memory_props2);

        struct pNextChainBuildingBlockInfo chain_info[] = {
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR,
             .mem_size = sizeof(VkPhysicalDevice16BitStorageFeaturesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR,
             .mem_size = sizeof(VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES_KHR,
             .mem_size = sizeof(VkPhysicalDeviceVariablePointerFeaturesKHR)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT,
             .mem_size = sizeof(VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT)},
            {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR,
             .mem_size = sizeof(VkPhysicalDeviceMultiviewFeaturesKHR)}};

        uint32_t chain_info_len = ARRAY_SIZE(chain_info);

        gpu->features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;
        buildpNextChain((struct VkStructureHeader *)&gpu->features2, chain_info, chain_info_len);

        inst->vkGetPhysicalDeviceFeatures2KHR(gpu->obj, &gpu->features2);
    }

    AppGetPhysicalDeviceLayerExtensions(gpu, NULL, &gpu->device_extension_count, &gpu->device_extensions);
}

static void AppGpuDestroy(struct AppGpu *gpu) {
    free(gpu->device_extensions);

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                              gpu->inst->inst_extensions_count)) {
        freepNextChain(gpu->features2.pNext);
    }

    for (uint32_t i = 0; i < gpu->queue_count; ++i) {
        free((void *)gpu->queue_reqs[i].pQueuePriorities);
    }
    free(gpu->queue_reqs);

    free(gpu->queue_props);
    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
                              gpu->inst->inst_extensions_count)) {
        free(gpu->queue_props2);

        freepNextChain(gpu->props2.pNext);
    }
}

// clang-format off

//-----------------------------------------------------------

//---------------------------Win32---------------------------
#ifdef VK_USE_PLATFORM_WIN32_KHR

// MS-Windows event handling function:
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

static void AppCreateWin32Window(struct AppInstance *inst) {
    inst->h_instance = GetModuleHandle(NULL);

    WNDCLASSEX win_class;

    // Initialize the window class structure:
    win_class.cbSize = sizeof(WNDCLASSEX);
    win_class.style = CS_HREDRAW | CS_VREDRAW;
    win_class.lpfnWndProc = WndProc;
    win_class.cbClsExtra = 0;
    win_class.cbWndExtra = 0;
    win_class.hInstance = inst->h_instance;
    win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    win_class.lpszMenuName = NULL;
    win_class.lpszClassName = APP_SHORT_NAME;
    win_class.hInstance = inst->h_instance;
    win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    // Register window class:
    if (!RegisterClassEx(&win_class)) {
        // It didn't work, so try to give a useful error:
        fprintf(stderr, "Failed to register the window class!\n");
        exit(1);
    }
    // Create window with the registered class:
    RECT wr = { 0, 0, inst->width, inst->height };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    inst->h_wnd = CreateWindowEx(0,
        APP_SHORT_NAME,       // class name
        APP_SHORT_NAME,       // app name
        //WS_VISIBLE | WS_SYSMENU |
        WS_OVERLAPPEDWINDOW,  // window style
        100, 100,             // x/y coords
        wr.right - wr.left,   // width
        wr.bottom - wr.top,   // height
        NULL,                 // handle to parent
        NULL,                 // handle to menu
        inst->h_instance,      // hInstance
        NULL);                // no extra parameters
    if (!inst->h_wnd) {
        // It didn't work, so try to give a useful error:
        fprintf(stderr, "Failed to create a window!\n");
        exit(1);
    }
}

static void AppCreateWin32Surface(struct AppInstance *inst) {
    VkResult U_ASSERT_ONLY err;
    VkWin32SurfaceCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.hinstance = inst->h_instance;
    createInfo.hwnd = inst->h_wnd;
    err = vkCreateWin32SurfaceKHR(inst->instance, &createInfo, NULL, &inst->surface);
    assert(!err);
}

static void AppDestroyWin32Window(struct AppInstance *inst) {
    DestroyWindow(inst->h_wnd);
}
#endif //VK_USE_PLATFORM_WIN32_KHR
//-----------------------------------------------------------

#if defined(VK_USE_PLATFORM_XCB_KHR)     || \
    defined(VK_USE_PLATFORM_XLIB_KHR)    || \
    defined(VK_USE_PLATFORM_WIN32_KHR)
static void AppDestroySurface(struct AppInstance *inst) { //same for all platforms
    vkDestroySurfaceKHR(inst->instance, inst->surface, NULL);
}
#endif

//----------------------------XCB----------------------------

#ifdef VK_USE_PLATFORM_XCB_KHR
static void AppCreateXcbWindow(struct AppInstance *inst) {
    //--Init Connection--
    const xcb_setup_t *setup;
    xcb_screen_iterator_t iter;
    int scr;

    // API guarantees non-null xcb_connection
    inst->xcb_connection = xcb_connect(NULL, &scr);
    int conn_error = xcb_connection_has_error(inst->xcb_connection);
    if (conn_error) {
        fprintf(stderr, "XCB failed to connect to the X server due to error:%d.\n", conn_error);
        fflush(stderr);
        inst->xcb_connection = NULL;
    }

    setup = xcb_get_setup(inst->xcb_connection);
    iter = xcb_setup_roots_iterator(setup);
    while (scr-- > 0) {
        xcb_screen_next(&iter);
    }

    inst->xcb_screen = iter.data;
    //-------------------

    inst->xcb_window = xcb_generate_id(inst->xcb_connection);
    xcb_create_window(inst->xcb_connection, XCB_COPY_FROM_PARENT, inst->xcb_window,
                      inst->xcb_screen->root, 0, 0, inst->width, inst->height, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, inst->xcb_screen->root_visual,
                      0, NULL);

    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(inst->xcb_connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *reply =  xcb_intern_atom_reply(inst->xcb_connection, cookie, 0);
    free(reply);
}

static void AppCreateXcbSurface(struct AppInstance *inst) {
    if (!inst->xcb_connection) {
        return;
    }

    VkResult U_ASSERT_ONLY err;
    VkXcbSurfaceCreateInfoKHR xcb_createInfo;
    xcb_createInfo.sType      = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    xcb_createInfo.pNext      = NULL;
    xcb_createInfo.flags      = 0;
    xcb_createInfo.connection = inst->xcb_connection;
    xcb_createInfo.window     = inst->xcb_window;
    err = vkCreateXcbSurfaceKHR(inst->instance, &xcb_createInfo, NULL, &inst->surface);
    assert(!err);
}

static void AppDestroyXcbWindow(struct AppInstance *inst) {
    if (!inst->xcb_connection) {
        return; // Nothing to destroy
    }

    xcb_destroy_window(inst->xcb_connection, inst->xcb_window);
    xcb_disconnect(inst->xcb_connection);
}
//VK_USE_PLATFORM_XCB_KHR
//-----------------------------------------------------------

//----------------------------XLib---------------------------
#elif VK_USE_PLATFORM_XLIB_KHR
static void AppCreateXlibWindow(struct AppInstance *inst) {
    long visualMask = VisualScreenMask;
    int numberOfVisuals;

    inst->xlib_display = XOpenDisplay(NULL);
    if (inst->xlib_display == NULL) {
        fprintf(stderr, "XLib failed to connect to the X server.\nExiting ...\n");
        exit(1);
    }

    XVisualInfo vInfoTemplate={};
    vInfoTemplate.screen = DefaultScreen(inst->xlib_display);
    XVisualInfo *visualInfo = XGetVisualInfo(inst->xlib_display, visualMask,
                                             &vInfoTemplate, &numberOfVisuals);
    inst->xlib_window = XCreateWindow(
                inst->xlib_display, RootWindow(inst->xlib_display, vInfoTemplate.screen), 0, 0,
                inst->width, inst->height, 0, visualInfo->depth, InputOutput,
                visualInfo->visual, 0, NULL);

    XSync(inst->xlib_display,false);
}

static void AppCreateXlibSurface(struct AppInstance *inst) {
    VkResult U_ASSERT_ONLY err;
    VkXlibSurfaceCreateInfoKHR createInfo;
    createInfo.sType  = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext  = NULL;
    createInfo.flags  = 0;
    createInfo.dpy    = inst->xlib_display;
    createInfo.window = inst->xlib_window;
    err = vkCreateXlibSurfaceKHR(inst->instance, &createInfo, NULL, &inst->surface);
    assert(!err);
}

static void AppDestroyXlibWindow(struct AppInstance *inst) {
    XDestroyWindow(inst->xlib_display, inst->xlib_window);
    XCloseDisplay(inst->xlib_display);
}
#endif //VK_USE_PLATFORM_XLIB_KHR
//-----------------------------------------------------------

#if defined(VK_USE_PLATFORM_XCB_KHR)     || \
    defined(VK_USE_PLATFORM_XLIB_KHR)    || \
    defined(VK_USE_PLATFORM_WIN32_KHR)
static int AppDumpSurfaceFormats(struct AppInstance *inst, struct AppGpu *gpu, FILE *out) {
    // Get the list of VkFormat's that are supported
    VkResult U_ASSERT_ONLY err;
    uint32_t format_count = 0;
    err = inst->vkGetPhysicalDeviceSurfaceFormatsKHR(gpu->obj, inst->surface, &format_count, NULL);
    assert(!err);

    VkSurfaceFormatKHR *surf_formats = (VkSurfaceFormatKHR *)malloc(format_count * sizeof(VkSurfaceFormatKHR));
    if (!surf_formats)
        ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
    err = inst->vkGetPhysicalDeviceSurfaceFormatsKHR(gpu->obj, inst->surface, &format_count, surf_formats);
    assert(!err);

    if (html_output) {
        fprintf(out, "\t\t\t\t<details><summary>Formats: count = <div class='val'>%d</div></summary>", format_count);
        if (format_count > 0) {
            fprintf(out, "\n");
        } else {
            fprintf(out, "</details>\n");
        }
    } else if (human_readable_output) {
        printf("Formats:\t\tcount = %d\n", format_count);
    }
    for (uint32_t i = 0; i < format_count; ++i) {
        if (html_output) {
            fprintf(out, "\t\t\t\t\t<details><summary><div class='type'>%s</div></summary></details>\n",
                    VkFormatString(surf_formats[i].format));
        } else if (human_readable_output) {
            printf("\t%s\n", VkFormatString(surf_formats[i].format));
        }
    }
    if (format_count > 0 && html_output) {
        fprintf(out, "\t\t\t\t</details>\n");
    }

    fflush(out);
    fflush(stdout);
    free(surf_formats);

    return format_count;
}

static int AppDumpSurfacePresentModes(struct AppInstance *inst, struct AppGpu *gpu, FILE *out) {
    // Get the list of VkPresentMode's that are supported:
    VkResult U_ASSERT_ONLY err;
    uint32_t present_mode_count = 0;
    err = inst->vkGetPhysicalDeviceSurfacePresentModesKHR(gpu->obj, inst->surface, &present_mode_count, NULL);
    assert(!err);

    VkPresentModeKHR *surf_present_modes = (VkPresentModeKHR *)malloc(present_mode_count * sizeof(VkPresentInfoKHR));
    if (!surf_present_modes)
        ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
    err = inst->vkGetPhysicalDeviceSurfacePresentModesKHR(gpu->obj, inst->surface, &present_mode_count, surf_present_modes);
    assert(!err);

    if (html_output) {
        fprintf(out, "\t\t\t\t<details><summary>Present Modes: count = <div class='val'>%d</div></summary>", present_mode_count);
        if (present_mode_count > 0) {
            fprintf(out, "\n");
        } else {
            fprintf(out, "</details>");
        }
    } else if (human_readable_output) {
        printf("Present Modes:\t\tcount = %d\n", present_mode_count);
    }
    for (uint32_t i = 0; i < present_mode_count; ++i) {
        if (html_output) {
            fprintf(out, "\t\t\t\t\t<details><summary><div class='type'>%s</div></summary></details>\n",
                    VkPresentModeString(surf_present_modes[i]));
        } else if (human_readable_output) {
            printf("\t%s\n", VkPresentModeString(surf_present_modes[i]));
        }
    }
    if (present_mode_count > 0 && html_output) {
        fprintf(out, "\t\t\t\t</details>\n");
    }

    fflush(out);
    fflush(stdout);
    free(surf_present_modes);

    return present_mode_count;
}

static void AppDumpSurfaceCapabilities(struct AppInstance *inst, struct AppGpu *gpu, FILE *out) {
    if (CheckExtensionEnabled(VK_KHR_SURFACE_EXTENSION_NAME, gpu->inst->inst_extensions, gpu->inst->inst_extensions_count)) {
        inst->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu->obj, inst->surface, &inst->surface_capabilities);

        if (html_output) {
            fprintf(out, "\t\t\t\t<details><summary>VkSurfaceCapabilitiesKHR</summary>\n");
            fprintf(out, "\t\t\t\t\t<details><summary>minImageCount = <div class='val'>%u</div></summary></details>\n", inst->surface_capabilities.minImageCount);
            fprintf(out, "\t\t\t\t\t<details><summary>maxImageCount = <div class='val'>%u</div></summary></details>\n", inst->surface_capabilities.maxImageCount);
            fprintf(out, "\t\t\t\t\t<details><summary>currentExtent</summary>\n");
            fprintf(out, "\t\t\t\t\t\t<details><summary>width = <div class='val'>%u</div></summary></details>\n", inst->surface_capabilities.currentExtent.width);
            fprintf(out, "\t\t\t\t\t\t<details><summary>height = <div class='val'>%u</div></summary></details>\n", inst->surface_capabilities.currentExtent.height);
            fprintf(out, "\t\t\t\t\t</details>\n");
            fprintf(out, "\t\t\t\t\t<details><summary>minImageExtent</summary>\n");
            fprintf(out, "\t\t\t\t\t\t<details><summary>width = <div class='val'>%u</div></summary></details>\n", inst->surface_capabilities.minImageExtent.width);
            fprintf(out, "\t\t\t\t\t\t<details><summary>height = <div class='val'>%u</div></summary></details>\n", inst->surface_capabilities.minImageExtent.height);
            fprintf(out, "\t\t\t\t\t</details>\n");
            fprintf(out, "\t\t\t\t\t<details><summary>maxImageExtent</summary>\n");
            fprintf(out, "\t\t\t\t\t\t<details><summary>width = <div class='val'>%u</div></summary></details>\n", inst->surface_capabilities.maxImageExtent.width);
            fprintf(out, "\t\t\t\t\t\t<details><summary>height = <div class='val'>%u</div></summary></details>\n", inst->surface_capabilities.maxImageExtent.height);
            fprintf(out, "\t\t\t\t\t</details>\n");
            fprintf(out, "\t\t\t\t\t<details><summary>maxImageArrayLayers = <div class='val'>%u</div></summary></details>\n", inst->surface_capabilities.maxImageArrayLayers);
            fprintf(out, "\t\t\t\t\t<details><summary>supportedTransform</summary>\n");
            if (inst->surface_capabilities.supportedTransforms == 0) {
                fprintf(out, "\t\t\t\t\t\t<details><summary>None</summary></details>\n");
            }
            if (inst->surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR</div></summary></details>\n");
            }
            if (inst->surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t\t<details><summary><div class='type'>VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR</div></summary></details>\n");
            }
            if (inst->surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t\t<details><summary><div class='type'>VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR</div></summary></details>\n");
            }
            if (inst->surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t\t<details><summary><div class='type'>VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR</div></summary></details>\n");
            }
            if (inst->surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t\t<details><summary><div class='type'>VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR</div></summary></details>\n");
            }
            if (inst->surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t\t<details><summary><div class='type'>VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR</div></summary></details>\n");
            }
            if (inst->surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t\t<details><summary><div class='type'>VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR</div></summary></details>\n");
            }
            if (inst->surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t\t<details><summary><div class='type'>VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR</div></summary></details>\n");
            }
            if (inst->surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t\t<details><summary><div class='type'>VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR</div></summary></details>\n");
            }
            fprintf(out, "\t\t\t\t\t</details>\n");
            fprintf(out, "\t\t\t\t\t<details><summary>currentTransform</summary>\n");
            if (inst->surface_capabilities.currentTransform == 0) {
                fprintf(out, "\t\t\t\t\t\t<details><summary>None</summary></details>\n");
            }
            if (inst->surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR</div></summary></details>\n");
            } else if (inst->surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR</div></summary></details>\n");
            } else if (inst->surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR</div></summary></details>\n");
            } else if (inst->surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR</div></summary></details>\n");
            } else if (inst->surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR</div></summary></details>\n");
            } else if (inst->surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t<details><summary><div class='type'>VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR</div></summary></details>\n");
            } else if (inst->surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR</div></summary></details>\n");
            } else if (inst->surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR</div></summary></details>\n");
            } else if (inst->surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR</div></summary></details>\n");
            }
            fprintf(out, "\t\t\t\t\t</details>\n");
            fprintf(out, "\t\t\t\t\t<details><summary>supportedCompositeAlpha</summary>\n");
            if (inst->surface_capabilities.supportedCompositeAlpha == 0) {
                fprintf(out, "\t\t\t\t\t\t<details><summary>None</summary></details>\n");
            }
            if (inst->surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR</div></summary></details>\n");
            }
            if (inst->surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR</div></summary></details>\n");
            }
            if (inst->surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR</div></summary></details>\n");
            }
            if (inst->surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR</div></summary></details>\n");
            }
            fprintf(out, "\t\t\t\t\t</details>\n");
            fprintf(out, "\t\t\t\t\t<details><summary>supportedUsageFlags</summary>\n");
            if (inst->surface_capabilities.supportedUsageFlags == 0) {
                fprintf(out, "\t\t\t\t\t\t<details><summary>None</summary></details>\n");
            }
            if (inst->surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_IMAGE_USAGE_TRANSFER_SRC_BIT</div></summary></details>\n");
            }
            if (inst->surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_IMAGE_USAGE_TRANSFER_DST_BIT</div></summary></details>\n");
            }
            if (inst->surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_SAMPLED_BIT) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_IMAGE_USAGE_SAMPLED_BIT</div></summary></details>\n");
            }
            if (inst->surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_STORAGE_BIT) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_IMAGE_USAGE_STORAGE_BIT</div></summary></details>\n");
            }
            if (inst->surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT</div></summary></details>\n");
            }
            if (inst->surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT</div></summary></details>\n");
            }
            if (inst->surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT</div></summary></details>\n");
            }
            if (inst->surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT) {
                fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT</div></summary></details>\n");
            }
            fprintf(out, "\t\t\t\t\t</details>\n");
        } else if (human_readable_output) {
            printf("\nVkSurfaceCapabilitiesKHR:\n");
            printf("=========================\n");
            printf("\tminImageCount       = %u\n", inst->surface_capabilities.minImageCount);
            printf("\tmaxImageCount       = %u\n", inst->surface_capabilities.maxImageCount);
            printf("\tcurrentExtent:\n");
            printf("\t\twidth       = %u\n", inst->surface_capabilities.currentExtent.width);
            printf("\t\theight      = %u\n", inst->surface_capabilities.currentExtent.height);
            printf("\tminImageExtent:\n");
            printf("\t\twidth       = %u\n", inst->surface_capabilities.minImageExtent.width);
            printf("\t\theight      = %u\n", inst->surface_capabilities.minImageExtent.height);
            printf("\tmaxImageExtent:\n");
            printf("\t\twidth       = %u\n", inst->surface_capabilities.maxImageExtent.width);
            printf("\t\theight      = %u\n", inst->surface_capabilities.maxImageExtent.height);
            printf("\tmaxImageArrayLayers = %u\n", inst->surface_capabilities.maxImageArrayLayers);
            printf("\tsupportedTransform:\n");
            if (inst->surface_capabilities.supportedTransforms == 0) { printf("\t\tNone\n"); }
            if (inst->surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) { printf("\t\tVK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR\n"); }
            if (inst->surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR) { printf("\t\tVK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR\n"); }
            if (inst->surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR) { printf("\t\tVK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR\n"); }
            if (inst->surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) { printf("\t\tVK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR\n"); }
            if (inst->surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR) { printf("\t\tVK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR\n"); }
            if (inst->surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR) { printf("\t\tVK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR\n"); }
            if (inst->surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR) { printf("\t\tVK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR\n"); }
            if (inst->surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR) { printf("\t\tVK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR\n"); }
            if (inst->surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR) { printf("\t\tVK_SURFACE_TRANSFORM_INHERIT_BIT_KHR\n"); }
            printf("\tcurrentTransform:\n");
            if (inst->surface_capabilities.currentTransform == 0) { printf("\t\tNone\n"); }
            if (inst->surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) { printf("\t\tVK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR\n"); }
            else if (inst->surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR) { printf("\t\tVK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR\n"); }
            else if (inst->surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR) { printf("\t\tVK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR\n"); }
            else if (inst->surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) { printf("\t\tVK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR\n"); }
            else if (inst->surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR) { printf("\t\tVK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR\n"); }
            else if (inst->surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR) { printf("\t\tVK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR\n"); }
            else if (inst->surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR) { printf("\t\tVK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR\n"); }
            else if (inst->surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR) { printf("\t\tVK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR\n"); }
            else if (inst->surface_capabilities.currentTransform & VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR) { printf("\t\tVK_SURFACE_TRANSFORM_INHERIT_BIT_KHR\n"); }
            printf("\tsupportedCompositeAlpha:\n");
            if (inst->surface_capabilities.supportedCompositeAlpha == 0) { printf("\t\tNone\n"); }
            if (inst->surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) { printf("\t\tVK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR\n"); }
            if (inst->surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) { printf("\t\tVK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR\n"); }
            if (inst->surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) { printf("\t\tVK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR\n"); }
            if (inst->surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) { printf("\t\tVK_COMPOSITE_ALPHA_INHERIT_BIT_KHR\n"); }
            printf("\tsupportedUsageFlags:\n");
            if (inst->surface_capabilities.supportedUsageFlags == 0) { printf("\t\tNone\n"); }
            if (inst->surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) { printf("\t\tVK_IMAGE_USAGE_TRANSFER_SRC_BIT\n"); }
            if (inst->surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) { printf("\t\tVK_IMAGE_USAGE_TRANSFER_DST_BIT\n"); }
            if (inst->surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_SAMPLED_BIT) { printf("\t\tVK_IMAGE_USAGE_SAMPLED_BIT\n"); }
            if (inst->surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_STORAGE_BIT) { printf("\t\tVK_IMAGE_USAGE_STORAGE_BIT\n"); }
            if (inst->surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) { printf("\t\tVK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT\n"); }
            if (inst->surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) { printf("\t\tVK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT\n"); }
            if (inst->surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT) { printf("\t\tVK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT\n"); }
            if (inst->surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT) { printf("\t\tVK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT\n"); }
        }

        // Get additional surface capability information from vkGetPhysicalDeviceSurfaceCapabilities2EXT
        if (CheckExtensionEnabled(VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME, gpu->inst->inst_extensions, gpu->inst->inst_extensions_count)) {
            memset(&inst->surface_capabilities2_ext, 0, sizeof(VkSurfaceCapabilities2EXT));
            inst->surface_capabilities2_ext.sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_EXT;
            inst->surface_capabilities2_ext.pNext = NULL;

            inst->vkGetPhysicalDeviceSurfaceCapabilities2EXT(gpu->obj, inst->surface, &inst->surface_capabilities2_ext);

            if (html_output) {
                fprintf(out, "\t\t\t\t\t<details><summary>VkSurfaceCapabilities2EXT</summary>\n");
                fprintf(out, "\t\t\t\t\t\t<details><summary>supportedSurfaceCounters</summary>\n");
                if (inst->surface_capabilities2_ext.supportedSurfaceCounters == 0) {
                    fprintf(out, "\t\t\t\t\t\t\t<details><summary>None</summary></details>\n");
                }
                if (inst->surface_capabilities2_ext.supportedSurfaceCounters & VK_SURFACE_COUNTER_VBLANK_EXT) {
                    fprintf(out, "\t\t\t\t\t\t\t<details><summary><div class='type'>VK_SURFACE_COUNTER_VBLANK_EXT</div></summary></details>\n");
                }
                fprintf(out, "\t\t\t\t\t\t</details>\n");
                fprintf(out, "\t\t\t\t\t</details>\n");
            } else if (human_readable_output) {
                printf("\nVkSurfaceCapabilities2EXT:\n");
                printf("==========================\n");
                printf("\tsupportedSurfaceCounters:\n");
                if (inst->surface_capabilities2_ext.supportedSurfaceCounters == 0) {
                    printf("\t\tNone\n");
                }
                if (inst->surface_capabilities2_ext.supportedSurfaceCounters & VK_SURFACE_COUNTER_VBLANK_EXT) {
                    printf("\t\tVK_SURFACE_COUNTER_VBLANK_EXT\n");
                }
            }
        }

        // Get additional surface capability information from vkGetPhysicalDeviceSurfaceCapabilities2KHR
        if (CheckExtensionEnabled(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME, gpu->inst->inst_extensions, gpu->inst->inst_extensions_count)) {
            if (CheckExtensionEnabled(VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME, gpu->inst->inst_extensions, gpu->inst->inst_extensions_count)) {
                inst->shared_surface_capabilities.sType = VK_STRUCTURE_TYPE_SHARED_PRESENT_SURFACE_CAPABILITIES_KHR;
                inst->shared_surface_capabilities.pNext = NULL;
                inst->surface_capabilities2.pNext = &inst->shared_surface_capabilities;
            } else {
                inst->surface_capabilities2.pNext = NULL;
            }

            inst->surface_capabilities2.sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR;

            VkPhysicalDeviceSurfaceInfo2KHR surface_info;
            surface_info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
            surface_info.pNext = NULL;
            surface_info.surface = inst->surface;

            inst->vkGetPhysicalDeviceSurfaceCapabilities2KHR(gpu->obj, &surface_info, &inst->surface_capabilities2);

            void *place = inst->surface_capabilities2.pNext;
            while (place) {
                struct VkStructureHeader* work = (struct VkStructureHeader*) place;
                if (work->sType == VK_STRUCTURE_TYPE_SHARED_PRESENT_SURFACE_CAPABILITIES_KHR) {
                    VkSharedPresentSurfaceCapabilitiesKHR* shared_surface_capabilities = (VkSharedPresentSurfaceCapabilitiesKHR*)place;
                    if (html_output) {
                        fprintf(out, "\t\t\t\t\t<details><summary>VkSharedPresentSurfaceCapabilitiesKHR</summary>\n");
                        fprintf(out, "\t\t\t\t\t\t<details><summary>sharedPresentSupportedUsageFlags</summary>\n");
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags == 0) {
                            fprintf(out, "\t\t\t\t\t\t\t<details><summary>None</summary></details>\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
                            fprintf(out, "\t\t\t\t\t\t\t<details><summary><div class='type'>VK_IMAGE_USAGE_TRANSFER_SRC_BIT</div></summary></details>\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
                            fprintf(out, "\t\t\t\t\t\t\t<details><summary><div class='type'>VK_IMAGE_USAGE_TRANSFER_DST_BIT</div></summary></details>\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_SAMPLED_BIT) {
                            fprintf(out, "\t\t\t\t\t\t\t<details><summary><div class='type'>VK_IMAGE_USAGE_SAMPLED_BIT</div></summary></details>\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_STORAGE_BIT) {
                            fprintf(out, "\t\t\t\t\t\t\t<details><summary><div class='type'>VK_IMAGE_USAGE_STORAGE_BIT</div></summary></details>\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
                            fprintf(out, "\t\t\t\t\t\t\t<details><summary><div class='type'>VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT</div></summary></details>\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
                            fprintf(out, "\t\t\t\t\t\t\t<details><summary><div class='type'>VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT</div></summary></details>\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT) {
                            fprintf(out, "\t\t\t\t\t\t\t<details><summary><div class='type'>VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT</div></summary></details>\n");
                        }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT) {
                            fprintf(out, "\t\t\t\t\t\t\t<details><summary><div class='type'>VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT</div></summary></details>\n");
                        }
                        fprintf(out, "\t\t\t\t\t\t</details>\n");
                        fprintf(out, "\t\t\t\t\t</details>\n");
                    } else if (human_readable_output) {
                        printf("\nVkSharedPresentSurfaceCapabilitiesKHR:\n");
                        printf("========================================\n");
                        printf("\tsharedPresentSupportedUsageFlags:\n");
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags == 0) { printf("\t\tNone\n"); }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) { printf("\t\tVK_IMAGE_USAGE_TRANSFER_SRC_BIT\n"); }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) { printf("\t\tVK_IMAGE_USAGE_TRANSFER_DST_BIT\n"); }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_SAMPLED_BIT) { printf("\t\tVK_IMAGE_USAGE_SAMPLED_BIT\n"); }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_STORAGE_BIT) { printf("\t\tVK_IMAGE_USAGE_STORAGE_BIT\n"); }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) { printf("\t\tVK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT\n"); }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) { printf("\t\tVK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT\n"); }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT) { printf("\t\tVK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT\n"); }
                        if (shared_surface_capabilities->sharedPresentSupportedUsageFlags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT) { printf("\t\tVK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT\n"); }
                    }
                }
                place = work->pNext;
            }
        }
        if (html_output) {
            fprintf(out, "\t\t\t\t</details>\n");
        }
    }
}

#endif

static void AppDevDumpFormatProps(const struct AppGpu *gpu, VkFormat fmt, bool *first_in_list, FILE *out) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(gpu->obj, fmt, &props);
    struct {
        const char *name;
        VkFlags flags;
    } features[3];

    features[0].name  = "linearTiling   FormatFeatureFlags";
    features[0].flags = props.linearTilingFeatures;
    features[1].name  = "optimalTiling  FormatFeatureFlags";
    features[1].flags = props.optimalTilingFeatures;
    features[2].name  = "bufferFeatures FormatFeatureFlags";
    features[2].flags = props.bufferFeatures;

    if (html_output) {
        fprintf(out, "\t\t\t\t\t\t<details><summary><div class='type'>FORMAT_%s</div></summary>\n", VkFormatString(fmt));
    } else if (human_readable_output) {
        printf("\nFORMAT_%s:", VkFormatString(fmt));
    }
    for (uint32_t i = 0; i < ARRAY_SIZE(features); ++i) {
        if (html_output) {
            fprintf(out, "\t\t\t\t\t\t\t<details open><summary>%s</summary>\n", features[i].name);
            if (features[i].flags == 0) {
                fprintf(out, "\t\t\t\t\t\t\t\t<details><summary>None</summary></details>\n");
            } else {
                fprintf(out, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
                        ((features[i].flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)                  ? "\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT</div></summary></details>\n"                  : ""),  //0x0001
                        ((features[i].flags & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT)                  ? "\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT</div></summary></details>\n"                  : ""),  //0x0002
                        ((features[i].flags & VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT)           ? "\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT</div></summary></details>\n"           : ""),  //0x0004
                        ((features[i].flags & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT)           ? "\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT</div></summary></details>\n"           : ""),  //0x0008
                        ((features[i].flags & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT)           ? "\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT</div></summary></details>\n"           : ""),  //0x0010
                        ((features[i].flags & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT)    ? "\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT</div></summary></details>\n"    : ""),  //0x0020
                        ((features[i].flags & VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT)                  ? "\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT</div></summary></details>\n"                  : ""),  //0x0040
                        ((features[i].flags & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT)               ? "\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT</div></summary></details>\n"               : ""),  //0x0080
                        ((features[i].flags & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT)         ? "\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT</div></summary></details>\n"         : ""),  //0x0100
                        ((features[i].flags & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)       ? "\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT</div></summary></details>\n"       : ""),  //0x0200
                        ((features[i].flags & VK_FORMAT_FEATURE_BLIT_SRC_BIT)                       ? "\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_FORMAT_FEATURE_BLIT_SRC_BIT</div></summary></details>\n"                       : ""),  //0x0400
                        ((features[i].flags & VK_FORMAT_FEATURE_BLIT_DST_BIT)                       ? "\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_FORMAT_FEATURE_BLIT_DST_BIT</div></summary></details>\n"                       : ""),  //0x0800
                        ((features[i].flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)    ? "\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT</div></summary></details>\n"    : ""),  //0x1000
                        ((features[i].flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG) ? "\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG</div></summary></details>\n" : ""),  //0x2000
                        ((features[i].flags & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR)               ? "\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR</div></summary></details>\n"               : ""),  //0x4000
                        ((features[i].flags & VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR)               ? "\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR</div></summary></details>\n"               : "")); //0x8000
            }
            fprintf(out, "\t\t\t\t\t\t\t</details>\n");
        } else if (human_readable_output) {
            printf("\n\t%s:", features[i].name);
            if (features[i].flags == 0) {
                printf("\n\t\tNone");
            } else {
                printf("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
                       ((features[i].flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)                  ? "\n\t\tVK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT"                  : ""),  //0x0001
                       ((features[i].flags & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT)                  ? "\n\t\tVK_FORMAT_FEATURE_STORAGE_IMAGE_BIT"                  : ""),  //0x0002
                       ((features[i].flags & VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT)           ? "\n\t\tVK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT"           : ""),  //0x0004
                       ((features[i].flags & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT)           ? "\n\t\tVK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT"           : ""),  //0x0008
                       ((features[i].flags & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT)           ? "\n\t\tVK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT"           : ""),  //0x0010
                       ((features[i].flags & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT)    ? "\n\t\tVK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT"    : ""),  //0x0020
                       ((features[i].flags & VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT)                  ? "\n\t\tVK_FORMAT_FEATURE_VERTEX_BUFFER_BIT"                  : ""),  //0x0040
                       ((features[i].flags & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT)               ? "\n\t\tVK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT"               : ""),  //0x0080
                       ((features[i].flags & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT)         ? "\n\t\tVK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT"         : ""),  //0x0100
                       ((features[i].flags & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)       ? "\n\t\tVK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT"       : ""),  //0x0200
                       ((features[i].flags & VK_FORMAT_FEATURE_BLIT_SRC_BIT)                       ? "\n\t\tVK_FORMAT_FEATURE_BLIT_SRC_BIT"                       : ""),  //0x0400
                       ((features[i].flags & VK_FORMAT_FEATURE_BLIT_DST_BIT)                       ? "\n\t\tVK_FORMAT_FEATURE_BLIT_DST_BIT"                       : ""),  //0x0800
                       ((features[i].flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)    ? "\n\t\tVK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT"    : ""),  //0x1000
                       ((features[i].flags & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG) ? "\n\t\tVK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_CUBIC_BIT_IMG" : ""),  //0x2000
                       ((features[i].flags & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR)               ? "\n\t\tVK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR"               : ""),  //0x4000
                       ((features[i].flags & VK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR)               ? "\n\t\tVK_FORMAT_FEATURE_TRANSFER_DST_BIT_KHR"               : "")); //0x8000
            }
        }
    }
    if (html_output) {
        fprintf(out, "\t\t\t\t\t\t</details>\n");
    } else if (human_readable_output) {
        printf("\n");
    }
    if (json_output && (props.linearTilingFeatures || props.optimalTilingFeatures || props.bufferFeatures)) {
        if (!(*first_in_list)) {
            printf(",");
        } else {
            *first_in_list = false;
        }
        printf("\n");
        printf("\t\t{\n");
        printf("\t\t\t\"formatID\": %d,\n", fmt);
        printf("\t\t\t\"linearTilingFeatures\": %u,\n", props.linearTilingFeatures);
        printf("\t\t\t\"optimalTilingFeatures\": %u,\n", props.optimalTilingFeatures);
        printf("\t\t\t\"bufferFeatures\": %u\n", props.bufferFeatures);
        printf("\t\t}");
    }
}

static void AppDevDump(const struct AppGpu *gpu, FILE *out) {
    if (html_output) {
        fprintf(out, "\t\t\t\t\t<details><summary>Format Properties</summary>\n");
    } else if (human_readable_output) {
        printf("Format Properties:\n");
        printf("==================");
    }
    if (json_output) {
        printf(",\n");
        printf("\t\"ArrayOfVkFormatProperties\": [");
    }

    bool first_in_list = true;   // Used for commas in json output
    for (VkFormat fmt = 0; fmt < VK_FORMAT_RANGE_SIZE; ++fmt) {
        AppDevDumpFormatProps(gpu, fmt, &first_in_list, out);
    }
    if (html_output) {
        fprintf(out, "\t\t\t\t\t</details>\n");
    }
    if (json_output) {
        printf("\n\t]");
    }
}

#ifdef _WIN32
#define PRINTF_SIZE_T_SPECIFIER    "%Iu"
#else
#define PRINTF_SIZE_T_SPECIFIER    "%zu"
#endif

static void AppGpuDumpFeatures(const struct AppGpu *gpu, FILE *out) {
    VkPhysicalDeviceFeatures features;

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
        gpu->inst->inst_extensions_count)) {
        const VkPhysicalDeviceFeatures *features2_const = &gpu->features2.features;
        features = *features2_const;
    } else {
        const VkPhysicalDeviceFeatures *features_const = &gpu->features;
        features = *features_const;
    }

    if (html_output) {
        fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceFeatures</summary>\n");
        fprintf(out, "\t\t\t\t\t\t<details><summary>alphaToOne                              = <div class='val'>%u</div></summary></details>\n", features.alphaToOne                             );
        fprintf(out, "\t\t\t\t\t\t<details><summary>depthBiasClamp                          = <div class='val'>%u</div></summary></details>\n", features.depthBiasClamp                         );
        fprintf(out, "\t\t\t\t\t\t<details><summary>depthBounds                             = <div class='val'>%u</div></summary></details>\n", features.depthBounds                            );
        fprintf(out, "\t\t\t\t\t\t<details><summary>depthClamp                              = <div class='val'>%u</div></summary></details>\n", features.depthClamp                             );
        fprintf(out, "\t\t\t\t\t\t<details><summary>drawIndirectFirstInstance               = <div class='val'>%u</div></summary></details>\n", features.drawIndirectFirstInstance              );
        fprintf(out, "\t\t\t\t\t\t<details><summary>dualSrcBlend                            = <div class='val'>%u</div></summary></details>\n", features.dualSrcBlend                           );
        fprintf(out, "\t\t\t\t\t\t<details><summary>fillModeNonSolid                        = <div class='val'>%u</div></summary></details>\n", features.fillModeNonSolid                       );
        fprintf(out, "\t\t\t\t\t\t<details><summary>fragmentStoresAndAtomics                = <div class='val'>%u</div></summary></details>\n", features.fragmentStoresAndAtomics               );
        fprintf(out, "\t\t\t\t\t\t<details><summary>fullDrawIndexUint32                     = <div class='val'>%u</div></summary></details>\n", features.fullDrawIndexUint32                    );
        fprintf(out, "\t\t\t\t\t\t<details><summary>geometryShader                          = <div class='val'>%u</div></summary></details>\n", features.geometryShader                         );
        fprintf(out, "\t\t\t\t\t\t<details><summary>imageCubeArray                          = <div class='val'>%u</div></summary></details>\n", features.imageCubeArray                         );
        fprintf(out, "\t\t\t\t\t\t<details><summary>independentBlend                        = <div class='val'>%u</div></summary></details>\n", features.independentBlend                       );
        fprintf(out, "\t\t\t\t\t\t<details><summary>inheritedQueries                        = <div class='val'>%u</div></summary></details>\n", features.inheritedQueries                       );
        fprintf(out, "\t\t\t\t\t\t<details><summary>largePoints                             = <div class='val'>%u</div></summary></details>\n", features.largePoints                            );
        fprintf(out, "\t\t\t\t\t\t<details><summary>logicOp                                 = <div class='val'>%u</div></summary></details>\n", features.logicOp                                );
        fprintf(out, "\t\t\t\t\t\t<details><summary>multiDrawIndirect                       = <div class='val'>%u</div></summary></details>\n", features.multiDrawIndirect                      );
        fprintf(out, "\t\t\t\t\t\t<details><summary>multiViewport                           = <div class='val'>%u</div></summary></details>\n", features.multiViewport                          );
        fprintf(out, "\t\t\t\t\t\t<details><summary>occlusionQueryPrecise                   = <div class='val'>%u</div></summary></details>\n", features.occlusionQueryPrecise                  );
        fprintf(out, "\t\t\t\t\t\t<details><summary>pipelineStatisticsQuery                 = <div class='val'>%u</div></summary></details>\n", features.pipelineStatisticsQuery                );
        fprintf(out, "\t\t\t\t\t\t<details><summary>robustBufferAccess                      = <div class='val'>%u</div></summary></details>\n", features.robustBufferAccess                     );
        fprintf(out, "\t\t\t\t\t\t<details><summary>samplerAnisotropy                       = <div class='val'>%u</div></summary></details>\n", features.samplerAnisotropy                      );
        fprintf(out, "\t\t\t\t\t\t<details><summary>sampleRateShading                       = <div class='val'>%u</div></summary></details>\n", features.sampleRateShading                      );
        fprintf(out, "\t\t\t\t\t\t<details><summary>shaderClipDistance                      = <div class='val'>%u</div></summary></details>\n", features.shaderClipDistance                     );
        fprintf(out, "\t\t\t\t\t\t<details><summary>shaderCullDistance                      = <div class='val'>%u</div></summary></details>\n", features.shaderCullDistance                     );
        fprintf(out, "\t\t\t\t\t\t<details><summary>shaderFloat64                           = <div class='val'>%u</div></summary></details>\n", features.shaderFloat64                          );
        fprintf(out, "\t\t\t\t\t\t<details><summary>shaderImageGatherExtended               = <div class='val'>%u</div></summary></details>\n", features.shaderImageGatherExtended              );
        fprintf(out, "\t\t\t\t\t\t<details><summary>shaderInt16                             = <div class='val'>%u</div></summary></details>\n", features.shaderInt16                            );
        fprintf(out, "\t\t\t\t\t\t<details><summary>shaderInt64                             = <div class='val'>%u</div></summary></details>\n", features.shaderInt64                            );
        fprintf(out, "\t\t\t\t\t\t<details><summary>shaderResourceMinLod                    = <div class='val'>%u</div></summary></details>\n", features.shaderResourceMinLod                   );
        fprintf(out, "\t\t\t\t\t\t<details><summary>shaderResourceResidency                 = <div class='val'>%u</div></summary></details>\n", features.shaderResourceResidency                );
        fprintf(out, "\t\t\t\t\t\t<details><summary>shaderSampledImageArrayDynamicIndexing  = <div class='val'>%u</div></summary></details>\n", features.shaderSampledImageArrayDynamicIndexing );
        fprintf(out, "\t\t\t\t\t\t<details><summary>shaderStorageBufferArrayDynamicIndexing = <div class='val'>%u</div></summary></details>\n", features.shaderStorageBufferArrayDynamicIndexing);
        fprintf(out, "\t\t\t\t\t\t<details><summary>shaderStorageImageArrayDynamicIndexing  = <div class='val'>%u</div></summary></details>\n", features.shaderStorageImageArrayDynamicIndexing );
        fprintf(out, "\t\t\t\t\t\t<details><summary>shaderStorageImageExtendedFormats       = <div class='val'>%u</div></summary></details>\n", features.shaderStorageImageExtendedFormats      );
        fprintf(out, "\t\t\t\t\t\t<details><summary>shaderStorageImageMultisample           = <div class='val'>%u</div></summary></details>\n", features.shaderStorageImageMultisample          );
        fprintf(out, "\t\t\t\t\t\t<details><summary>shaderStorageImageReadWithoutFormat     = <div class='val'>%u</div></summary></details>\n", features.shaderStorageImageReadWithoutFormat    );
        fprintf(out, "\t\t\t\t\t\t<details><summary>shaderStorageImageWriteWithoutFormat    = <div class='val'>%u</div></summary></details>\n", features.shaderStorageImageWriteWithoutFormat   );
        fprintf(out, "\t\t\t\t\t\t<details><summary>shaderTessellationAndGeometryPointSize  = <div class='val'>%u</div></summary></details>\n", features.shaderTessellationAndGeometryPointSize );
        fprintf(out, "\t\t\t\t\t\t<details><summary>shaderUniformBufferArrayDynamicIndexing = <div class='val'>%u</div></summary></details>\n", features.shaderUniformBufferArrayDynamicIndexing);
        fprintf(out, "\t\t\t\t\t\t<details><summary>sparseBinding                           = <div class='val'>%u</div></summary></details>\n", features.sparseBinding                          );
        fprintf(out, "\t\t\t\t\t\t<details><summary>sparseResidency2Samples                 = <div class='val'>%u</div></summary></details>\n", features.sparseResidency2Samples                );
        fprintf(out, "\t\t\t\t\t\t<details><summary>sparseResidency4Samples                 = <div class='val'>%u</div></summary></details>\n", features.sparseResidency4Samples                );
        fprintf(out, "\t\t\t\t\t\t<details><summary>sparseResidency8Samples                 = <div class='val'>%u</div></summary></details>\n", features.sparseResidency8Samples                );
        fprintf(out, "\t\t\t\t\t\t<details><summary>sparseResidency16Samples                = <div class='val'>%u</div></summary></details>\n", features.sparseResidency16Samples               );
        fprintf(out, "\t\t\t\t\t\t<details><summary>sparseResidencyAliased                  = <div class='val'>%u</div></summary></details>\n", features.sparseResidencyAliased                 );
        fprintf(out, "\t\t\t\t\t\t<details><summary>sparseResidencyBuffer                   = <div class='val'>%u</div></summary></details>\n", features.sparseResidencyBuffer                  );
        fprintf(out, "\t\t\t\t\t\t<details><summary>sparseResidencyImage2D                  = <div class='val'>%u</div></summary></details>\n", features.sparseResidencyImage2D                 );
        fprintf(out, "\t\t\t\t\t\t<details><summary>sparseResidencyImage3D                  = <div class='val'>%u</div></summary></details>\n", features.sparseResidencyImage3D                 );
        fprintf(out, "\t\t\t\t\t\t<details><summary>tessellationShader                      = <div class='val'>%u</div></summary></details>\n", features.tessellationShader                     );
        fprintf(out, "\t\t\t\t\t\t<details><summary>textureCompressionASTC_LDR              = <div class='val'>%u</div></summary></details>\n", features.textureCompressionASTC_LDR             );
        fprintf(out, "\t\t\t\t\t\t<details><summary>textureCompressionBC                    = <div class='val'>%u</div></summary></details>\n", features.textureCompressionBC                   );
        fprintf(out, "\t\t\t\t\t\t<details><summary>textureCompressionETC2                  = <div class='val'>%u</div></summary></details>\n", features.textureCompressionETC2                 );
        fprintf(out, "\t\t\t\t\t\t<details><summary>variableMultisampleRate                 = <div class='val'>%u</div></summary></details>\n", features.variableMultisampleRate                );
        fprintf(out, "\t\t\t\t\t\t<details><summary>vertexPipelineStoresAndAtomics          = <div class='val'>%u</div></summary></details>\n", features.vertexPipelineStoresAndAtomics         );
        fprintf(out, "\t\t\t\t\t\t<details><summary>wideLines                               = <div class='val'>%u</div></summary></details>\n", features.wideLines                              );
        fprintf(out, "\t\t\t\t\t</details>\n");
    } else if (human_readable_output) {
        printf("VkPhysicalDeviceFeatures:\n");
        printf("=========================\n");
        printf("\talphaToOne                              = %u\n", features.alphaToOne                             );
        printf("\tdepthBiasClamp                          = %u\n", features.depthBiasClamp                         );
        printf("\tdepthBounds                             = %u\n", features.depthBounds                            );
        printf("\tdepthClamp                              = %u\n", features.depthClamp                             );
        printf("\tdrawIndirectFirstInstance               = %u\n", features.drawIndirectFirstInstance              );
        printf("\tdualSrcBlend                            = %u\n", features.dualSrcBlend                           );
        printf("\tfillModeNonSolid                        = %u\n", features.fillModeNonSolid                       );
        printf("\tfragmentStoresAndAtomics                = %u\n", features.fragmentStoresAndAtomics               );
        printf("\tfullDrawIndexUint32                     = %u\n", features.fullDrawIndexUint32                    );
        printf("\tgeometryShader                          = %u\n", features.geometryShader                         );
        printf("\timageCubeArray                          = %u\n", features.imageCubeArray                         );
        printf("\tindependentBlend                        = %u\n", features.independentBlend                       );
        printf("\tinheritedQueries                        = %u\n", features.inheritedQueries                       );
        printf("\tlargePoints                             = %u\n", features.largePoints                            );
        printf("\tlogicOp                                 = %u\n", features.logicOp                                );
        printf("\tmultiDrawIndirect                       = %u\n", features.multiDrawIndirect                      );
        printf("\tmultiViewport                           = %u\n", features.multiViewport                          );
        printf("\tocclusionQueryPrecise                   = %u\n", features.occlusionQueryPrecise                  );
        printf("\tpipelineStatisticsQuery                 = %u\n", features.pipelineStatisticsQuery                );
        printf("\trobustBufferAccess                      = %u\n", features.robustBufferAccess                     );
        printf("\tsamplerAnisotropy                       = %u\n", features.samplerAnisotropy                      );
        printf("\tsampleRateShading                       = %u\n", features.sampleRateShading                      );
        printf("\tshaderClipDistance                      = %u\n", features.shaderClipDistance                     );
        printf("\tshaderCullDistance                      = %u\n", features.shaderCullDistance                     );
        printf("\tshaderFloat64                           = %u\n", features.shaderFloat64                          );
        printf("\tshaderImageGatherExtended               = %u\n", features.shaderImageGatherExtended              );
        printf("\tshaderInt16                             = %u\n", features.shaderInt16                            );
        printf("\tshaderInt64                             = %u\n", features.shaderInt64                            );
        printf("\tshaderSampledImageArrayDynamicIndexing  = %u\n", features.shaderSampledImageArrayDynamicIndexing );
        printf("\tshaderStorageBufferArrayDynamicIndexing = %u\n", features.shaderStorageBufferArrayDynamicIndexing);
        printf("\tshaderStorageImageArrayDynamicIndexing  = %u\n", features.shaderStorageImageArrayDynamicIndexing );
        printf("\tshaderStorageImageExtendedFormats       = %u\n", features.shaderStorageImageExtendedFormats      );
        printf("\tshaderStorageImageMultisample           = %u\n", features.shaderStorageImageMultisample          );
        printf("\tshaderStorageImageReadWithoutFormat     = %u\n", features.shaderStorageImageReadWithoutFormat    );
        printf("\tshaderStorageImageWriteWithoutFormat    = %u\n", features.shaderStorageImageWriteWithoutFormat   );
        printf("\tshaderTessellationAndGeometryPointSize  = %u\n", features.shaderTessellationAndGeometryPointSize );
        printf("\tshaderUniformBufferArrayDynamicIndexing = %u\n", features.shaderUniformBufferArrayDynamicIndexing);
        printf("\tsparseBinding                           = %u\n", features.sparseBinding                          );
        printf("\tsparseResidency2Samples                 = %u\n", features.sparseResidency2Samples                );
        printf("\tsparseResidency4Samples                 = %u\n", features.sparseResidency4Samples                );
        printf("\tsparseResidency8Samples                 = %u\n", features.sparseResidency8Samples                );
        printf("\tsparseResidency16Samples                = %u\n", features.sparseResidency16Samples               );
        printf("\tsparseResidencyAliased                  = %u\n", features.sparseResidencyAliased                 );
        printf("\tsparseResidencyBuffer                   = %u\n", features.sparseResidencyBuffer                  );
        printf("\tsparseResidencyImage2D                  = %u\n", features.sparseResidencyImage2D                 );
        printf("\tsparseResidencyImage3D                  = %u\n", features.sparseResidencyImage3D                 );
        printf("\tshaderResourceMinLod                    = %u\n", features.shaderResourceMinLod                   );
        printf("\tshaderResourceResidency                 = %u\n", features.shaderResourceResidency                );
        printf("\ttessellationShader                      = %u\n", features.tessellationShader                     );
        printf("\ttextureCompressionASTC_LDR              = %u\n", features.textureCompressionASTC_LDR             );
        printf("\ttextureCompressionBC                    = %u\n", features.textureCompressionBC                   );
        printf("\ttextureCompressionETC2                  = %u\n", features.textureCompressionETC2                 );
        printf("\tvariableMultisampleRate                 = %u\n", features.variableMultisampleRate                );
        printf("\tvertexPipelineStoresAndAtomics          = %u\n", features.vertexPipelineStoresAndAtomics         );
        printf("\twideLines                               = %u\n", features.wideLines                              );
    }
    if (json_output) {
        printf(",\n");
        printf("\t\"VkPhysicalDeviceFeatures\": {\n");
        printf("\t\t\"alphaToOne\": %u,\n",                              features.alphaToOne);
        printf("\t\t\"depthBiasClamp\": %u,\n",                          features.depthBiasClamp);
        printf("\t\t\"depthBounds\": %u,\n",                             features.depthBounds);
        printf("\t\t\"depthClamp\": %u,\n",                              features.depthClamp);
        printf("\t\t\"drawIndirectFirstInstance\": %u,\n",               features.drawIndirectFirstInstance);
        printf("\t\t\"dualSrcBlend\": %u,\n",                            features.dualSrcBlend);
        printf("\t\t\"fillModeNonSolid\": %u,\n",                        features.fillModeNonSolid);
        printf("\t\t\"fragmentStoresAndAtomics\": %u,\n",                features.fragmentStoresAndAtomics);
        printf("\t\t\"fullDrawIndexUint32\": %u,\n",                     features.fullDrawIndexUint32);
        printf("\t\t\"geometryShader\": %u,\n",                          features.geometryShader);
        printf("\t\t\"imageCubeArray\": %u,\n",                          features.imageCubeArray);
        printf("\t\t\"independentBlend\": %u,\n",                        features.independentBlend);
        printf("\t\t\"inheritedQueries\": %u,\n",                        features.inheritedQueries);
        printf("\t\t\"largePoints\": %u,\n",                             features.largePoints);
        printf("\t\t\"logicOp\": %u,\n",                                 features.logicOp);
        printf("\t\t\"multiDrawIndirect\": %u,\n",                       features.multiDrawIndirect);
        printf("\t\t\"multiViewport\": %u,\n",                           features.multiViewport);
        printf("\t\t\"occlusionQueryPrecise\": %u,\n",                   features.occlusionQueryPrecise);
        printf("\t\t\"pipelineStatisticsQuery\": %u,\n",                 features.pipelineStatisticsQuery);
        printf("\t\t\"robustBufferAccess\": %u,\n",                      features.robustBufferAccess);
        printf("\t\t\"samplerAnisotropy\": %u,\n",                       features.samplerAnisotropy);
        printf("\t\t\"sampleRateShading\": %u,\n",                       features.sampleRateShading);
        printf("\t\t\"shaderClipDistance\": %u,\n",                      features.shaderClipDistance);
        printf("\t\t\"shaderCullDistance\": %u,\n",                      features.shaderCullDistance);
        printf("\t\t\"shaderFloat64\": %u,\n",                           features.shaderFloat64);
        printf("\t\t\"shaderImageGatherExtended\": %u,\n",               features.shaderImageGatherExtended);
        printf("\t\t\"shaderInt16\": %u,\n",                             features.shaderInt16);
        printf("\t\t\"shaderInt64\": %u,\n",                             features.shaderInt64);
        printf("\t\t\"shaderResourceMinLod\": %u,\n",                    features.shaderResourceMinLod);
        printf("\t\t\"shaderResourceResidency\": %u,\n",                 features.shaderResourceResidency);
        printf("\t\t\"shaderSampledImageArrayDynamicIndexing\": %u,\n",  features.shaderSampledImageArrayDynamicIndexing);
        printf("\t\t\"shaderStorageBufferArrayDynamicIndexing\": %u,\n", features.shaderStorageBufferArrayDynamicIndexing);
        printf("\t\t\"shaderStorageImageArrayDynamicIndexing\": %u,\n",  features.shaderStorageImageArrayDynamicIndexing);
        printf("\t\t\"shaderStorageImageExtendedFormats\": %u,\n",       features.shaderStorageImageExtendedFormats);
        printf("\t\t\"shaderStorageImageMultisample\": %u,\n",           features.shaderStorageImageMultisample);
        printf("\t\t\"shaderStorageImageReadWithoutFormat\": %u,\n",     features.shaderStorageImageReadWithoutFormat);
        printf("\t\t\"shaderStorageImageWriteWithoutFormat\": %u,\n",    features.shaderStorageImageWriteWithoutFormat);
        printf("\t\t\"shaderTessellationAndGeometryPointSize\": %u,\n",  features.shaderTessellationAndGeometryPointSize);
        printf("\t\t\"shaderUniformBufferArrayDynamicIndexing\": %u,\n", features.shaderUniformBufferArrayDynamicIndexing);
        printf("\t\t\"sparseBinding\": %u,\n",                           features.sparseBinding);
        printf("\t\t\"sparseResidency2Samples\": %u,\n",                 features.sparseResidency2Samples);
        printf("\t\t\"sparseResidency4Samples\": %u,\n",                 features.sparseResidency4Samples);
        printf("\t\t\"sparseResidency8Samples\": %u,\n",                 features.sparseResidency8Samples);
        printf("\t\t\"sparseResidency16Samples\": %u,\n",                features.sparseResidency16Samples);
        printf("\t\t\"sparseResidencyAliased\": %u,\n",                  features.sparseResidencyAliased);
        printf("\t\t\"sparseResidencyBuffer\": %u,\n",                   features.sparseResidencyBuffer);
        printf("\t\t\"sparseResidencyImage2D\": %u,\n",                  features.sparseResidencyImage2D);
        printf("\t\t\"sparseResidencyImage3D\": %u,\n",                  features.sparseResidencyImage3D);
        printf("\t\t\"tessellationShader\": %u,\n",                      features.tessellationShader);
        printf("\t\t\"textureCompressionASTC_LDR\": %u,\n",              features.textureCompressionASTC_LDR);
        printf("\t\t\"textureCompressionBC\": %u,\n",                    features.textureCompressionBC);
        printf("\t\t\"textureCompressionETC2\": %u,\n",                  features.textureCompressionETC2);
        printf("\t\t\"variableMultisampleRate\": %u,\n",                 features.variableMultisampleRate);
        printf("\t\t\"vertexPipelineStoresAndAtomics\": %u,\n",          features.vertexPipelineStoresAndAtomics);
        printf("\t\t\"wideLines\": %u\n",                                features.wideLines);
        printf("\t}");
    }

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
        gpu->inst->inst_extensions_count)) {
        void *place = gpu->features2.pNext;
        while (place) {
            struct VkStructureHeader *structure = (struct VkStructureHeader*) place;
            if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES_KHR && CheckPhysicalDeviceExtensionIncluded(VK_KHR_16BIT_STORAGE_EXTENSION_NAME, gpu->device_extensions, gpu->device_extension_count)) {
                VkPhysicalDevice16BitStorageFeaturesKHR *b16_store_features = (VkPhysicalDevice16BitStorageFeaturesKHR*)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDevice16BitStorageFeatures</summary>\n");
                    fprintf(out, "\t\t\t\t\t\t<details><summary>storageBuffer16BitAccess           = <div class='val'>%u</div></summary></details>\n", b16_store_features->storageBuffer16BitAccess          );
                    fprintf(out, "\t\t\t\t\t\t<details><summary>uniformAndStorageBuffer16BitAccess = <div class='val'>%u</div></summary></details>\n", b16_store_features->uniformAndStorageBuffer16BitAccess);
                    fprintf(out, "\t\t\t\t\t\t<details><summary>storagePushConstant16              = <div class='val'>%u</div></summary></details>\n", b16_store_features->storagePushConstant16             );
                    fprintf(out, "\t\t\t\t\t\t<details><summary>storageInputOutput16               = <div class='val'>%u</div></summary></details>\n", b16_store_features->storageInputOutput16              );
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDevice16BitStorageFeatures:\n");
                    printf("=====================================\n");
                    printf("\tstorageBuffer16BitAccess           = %u\n", b16_store_features->storageBuffer16BitAccess          );
                    printf("\tuniformAndStorageBuffer16BitAccess = %u\n", b16_store_features->uniformAndStorageBuffer16BitAccess);
                    printf("\tstoragePushConstant16              = %u\n", b16_store_features->storagePushConstant16             );
                    printf("\tstorageInputOutput16               = %u\n", b16_store_features->storageInputOutput16              );
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES_KHR && CheckPhysicalDeviceExtensionIncluded(VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME, gpu->device_extensions, gpu->device_extension_count)) {
                VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR *sampler_ycbcr_features = (VkPhysicalDeviceSamplerYcbcrConversionFeaturesKHR*)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceSamplerYcbcrConversionFeatures</summary>\n");
                    fprintf(out, "\t\t\t\t\t\t<details><summary>samplerYcbcrConversion = <div class='val'>%u</div></summary></details>\n", sampler_ycbcr_features->samplerYcbcrConversion);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceSamplerYcbcrConversionFeatures:\n");
                    printf("===============================================\n");
                    printf("\tsamplerYcbcrConversion = %u\n", sampler_ycbcr_features->samplerYcbcrConversion);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES_KHR && CheckPhysicalDeviceExtensionIncluded(VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME, gpu->device_extensions, gpu->device_extension_count)) {
                VkPhysicalDeviceVariablePointerFeaturesKHR *var_pointer_features = (VkPhysicalDeviceVariablePointerFeaturesKHR*)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceVariablePointerFeatures</summary>\n");
                    fprintf(out, "\t\t\t\t\t\t<details><summary>variablePointersStorageBuffer = <div class='val'>%u</div></summary></details>\n", var_pointer_features->variablePointersStorageBuffer);
                    fprintf(out, "\t\t\t\t\t\t<details><summary>variablePointers              = <div class='val'>%u</div></summary></details>\n", var_pointer_features->variablePointers             );
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceVariablePointerFeatures:\n");
                    printf("========================================\n");
                    printf("\tvariablePointersStorageBuffer = %u\n", var_pointer_features->variablePointersStorageBuffer);
                    printf("\tvariablePointers              = %u\n", var_pointer_features->variablePointers             );
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_FEATURES_EXT && CheckPhysicalDeviceExtensionIncluded(VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME, gpu->device_extensions, gpu->device_extension_count)) {
                VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT *blend_op_adv_features = (VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT*)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceBlendOperationAdvancedFeatures</summary>\n");
                    fprintf(out, "\t\t\t\t\t\t<details><summary>advancedBlendCoherentOperations = <div class='val'>%u</div></summary></details>\n", blend_op_adv_features->advancedBlendCoherentOperations);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceBlendOperationAdvancedFeatures:\n");
                    printf("===============================================\n");
                    printf("\tadvancedBlendCoherentOperations = %u\n", blend_op_adv_features->advancedBlendCoherentOperations);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR && CheckPhysicalDeviceExtensionIncluded(VK_KHR_MULTIVIEW_EXTENSION_NAME, gpu->device_extensions, gpu->device_extension_count)) {
                VkPhysicalDeviceMultiviewFeaturesKHR *multiview_features = (VkPhysicalDeviceMultiviewFeaturesKHR*)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceMultiviewFeatures</summary>\n");
                    fprintf(out, "\t\t\t\t\t\t<details><summary>multiview                   = <div class='val'>%u</div></summary></details>\n", multiview_features->multiview                  );
                    fprintf(out, "\t\t\t\t\t\t<details><summary>multiviewGeometryShader     = <div class='val'>%u</div></summary></details>\n", multiview_features->multiviewGeometryShader    );
                    fprintf(out, "\t\t\t\t\t\t<details><summary>multiviewTessellationShader = <div class='val'>%u</div></summary></details>\n", multiview_features->multiviewTessellationShader);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                }
                else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceMultiviewFeatures:\n");
                    printf("==================================\n");
                    printf("\tmultiview                   = %u\n", multiview_features->multiview                  );
                    printf("\tmultiviewGeometryShader     = %u\n", multiview_features->multiviewGeometryShader    );
                    printf("\tmultiviewTessellationShader = %u\n", multiview_features->multiviewTessellationShader);
                }
            }
            place = structure->pNext;
        }
    }
}

static void AppDumpSparseProps(const VkPhysicalDeviceSparseProperties *sparse_props, FILE *out) {
    if (html_output) {
        fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceSparseProperties</summary>\n");
        fprintf(out, "\t\t\t\t\t\t<details><summary>residencyStandard2DBlockShape            = <div class='val'>%u</div></summary></details>\n", sparse_props->residencyStandard2DBlockShape           );
        fprintf(out, "\t\t\t\t\t\t<details><summary>residencyStandard2DMultisampleBlockShape = <div class='val'>%u</div></summary></details>\n", sparse_props->residencyStandard2DMultisampleBlockShape);
        fprintf(out, "\t\t\t\t\t\t<details><summary>residencyStandard3DBlockShape            = <div class='val'>%u</div></summary></details>\n", sparse_props->residencyStandard3DBlockShape           );
        fprintf(out, "\t\t\t\t\t\t<details><summary>residencyAlignedMipSize                  = <div class='val'>%u</div></summary></details>\n", sparse_props->residencyAlignedMipSize                 );
        fprintf(out, "\t\t\t\t\t\t<details><summary>residencyNonResidentStrict               = <div class='val'>%u</div></summary></details>\n", sparse_props->residencyNonResidentStrict              );
        fprintf(out, "\t\t\t\t\t</details>\n");
    } else if (human_readable_output) {
        printf("\tVkPhysicalDeviceSparseProperties:\n");
        printf("\t---------------------------------\n");
        printf("\t\tresidencyStandard2DBlockShape            = %u\n", sparse_props->residencyStandard2DBlockShape           );
        printf("\t\tresidencyStandard2DMultisampleBlockShape = %u\n", sparse_props->residencyStandard2DMultisampleBlockShape);
        printf("\t\tresidencyStandard3DBlockShape            = %u\n", sparse_props->residencyStandard3DBlockShape           );
        printf("\t\tresidencyAlignedMipSize                  = %u\n", sparse_props->residencyAlignedMipSize                 );
        printf("\t\tresidencyNonResidentStrict               = %u\n", sparse_props->residencyNonResidentStrict              );
    }
    if (json_output) {
        printf(",\n");
        printf("\t\t\"sparseProperties\": {\n");
        printf("\t\t\t\"residencyAlignedMipSize\": %u,\n",                  sparse_props->residencyAlignedMipSize);
        printf("\t\t\t\"residencyNonResidentStrict\": %u,\n",               sparse_props->residencyNonResidentStrict);
        printf("\t\t\t\"residencyStandard2DBlockShape\": %u,\n",            sparse_props->residencyStandard2DBlockShape);
        printf("\t\t\t\"residencyStandard2DMultisampleBlockShape\": %u,\n", sparse_props->residencyStandard2DMultisampleBlockShape);
        printf("\t\t\t\"residencyStandard3DBlockShape\": %u\n",             sparse_props->residencyStandard3DBlockShape);
        printf("\t\t}");
    }
}

static void AppDumpLimits(const VkPhysicalDeviceLimits *limits, FILE *out) {
    if (html_output) {
        fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceLimits</summary>\n");
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxImageDimension1D                     = <div class='val'>%u</div></summary></details>\n",                 limits->maxImageDimension1D                    );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxImageDimension2D                     = <div class='val'>%u</div></summary></details>\n",                 limits->maxImageDimension2D                    );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxImageDimension3D                     = <div class='val'>%u</div></summary></details>\n",                 limits->maxImageDimension3D                    );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxImageDimensionCube                   = <div class='val'>%u</div></summary></details>\n",                 limits->maxImageDimensionCube                  );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxImageArrayLayers                     = <div class='val'>%u</div></summary></details>\n",                 limits->maxImageArrayLayers                    );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxTexelBufferElements                  = <div class='val'>0x%" PRIxLEAST32 "</div></summary></details>\n", limits->maxTexelBufferElements                 );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxUniformBufferRange                   = <div class='val'>0x%" PRIxLEAST32 "</div></summary></details>\n", limits->maxUniformBufferRange                  );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxStorageBufferRange                   = <div class='val'>0x%" PRIxLEAST32 "</div></summary></details>\n", limits->maxStorageBufferRange                  );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxPushConstantsSize                    = <div class='val'>%u</div></summary></details>\n",                 limits->maxPushConstantsSize                   );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxMemoryAllocationCount                = <div class='val'>%u</div></summary></details>\n",                 limits->maxMemoryAllocationCount               );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxSamplerAllocationCount               = <div class='val'>%u</div></summary></details>\n",                 limits->maxSamplerAllocationCount              );
        fprintf(out, "\t\t\t\t\t\t<details><summary>bufferImageGranularity                  = <div class='val'>0x%" PRIxLEAST64 "</div></summary></details>\n", limits->bufferImageGranularity                 );
        fprintf(out, "\t\t\t\t\t\t<details><summary>sparseAddressSpaceSize                  = <div class='val'>0x%" PRIxLEAST64 "</div></summary></details>\n", limits->sparseAddressSpaceSize                 );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxBoundDescriptorSets                  = <div class='val'>%u</div></summary></details>\n",                 limits->maxBoundDescriptorSets                 );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxPerStageDescriptorSamplers           = <div class='val'>%u</div></summary></details>\n",                 limits->maxPerStageDescriptorSamplers          );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxPerStageDescriptorUniformBuffers     = <div class='val'>%u</div></summary></details>\n",                 limits->maxPerStageDescriptorUniformBuffers    );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxPerStageDescriptorStorageBuffers     = <div class='val'>%u</div></summary></details>\n",                 limits->maxPerStageDescriptorStorageBuffers    );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxPerStageDescriptorSampledImages      = <div class='val'>%u</div></summary></details>\n",                 limits->maxPerStageDescriptorSampledImages     );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxPerStageDescriptorStorageImages      = <div class='val'>%u</div></summary></details>\n",                 limits->maxPerStageDescriptorStorageImages     );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxPerStageDescriptorInputAttachments   = <div class='val'>%u</div></summary></details>\n",                 limits->maxPerStageDescriptorInputAttachments  );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxPerStageResources                    = <div class='val'>%u</div></summary></details>\n",                 limits->maxPerStageResources                   );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxDescriptorSetSamplers                = <div class='val'>%u</div></summary></details>\n",                 limits->maxDescriptorSetSamplers               );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxDescriptorSetUniformBuffers          = <div class='val'>%u</div></summary></details>\n",                 limits->maxDescriptorSetUniformBuffers         );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxDescriptorSetUniformBuffersDynamic   = <div class='val'>%u</div></summary></details>\n",                 limits->maxDescriptorSetUniformBuffersDynamic  );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxDescriptorSetStorageBuffers          = <div class='val'>%u</div></summary></details>\n",                 limits->maxDescriptorSetStorageBuffers         );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxDescriptorSetStorageBuffersDynamic   = <div class='val'>%u</div></summary></details>\n",                 limits->maxDescriptorSetStorageBuffersDynamic  );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxDescriptorSetSampledImages           = <div class='val'>%u</div></summary></details>\n",                 limits->maxDescriptorSetSampledImages          );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxDescriptorSetStorageImages           = <div class='val'>%u</div></summary></details>\n",                 limits->maxDescriptorSetStorageImages          );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxDescriptorSetInputAttachments        = <div class='val'>%u</div></summary></details>\n",                 limits->maxDescriptorSetInputAttachments       );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxVertexInputAttributes                = <div class='val'>%u</div></summary></details>\n",                 limits->maxVertexInputAttributes               );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxVertexInputBindings                  = <div class='val'>%u</div></summary></details>\n",                 limits->maxVertexInputBindings                 );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxVertexInputAttributeOffset           = <div class='val'>0x%" PRIxLEAST32 "</div></summary></details>\n", limits->maxVertexInputAttributeOffset          );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxVertexInputBindingStride             = <div class='val'>0x%" PRIxLEAST32 "</div></summary></details>\n", limits->maxVertexInputBindingStride            );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxVertexOutputComponents               = <div class='val'>%u</div></summary></details>\n",                 limits->maxVertexOutputComponents              );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxTessellationGenerationLevel          = <div class='val'>%u</div></summary></details>\n",                 limits->maxTessellationGenerationLevel         );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxTessellationPatchSize                        = <div class='val'>%u</div></summary></details>\n",                 limits->maxTessellationPatchSize                       );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxTessellationControlPerVertexInputComponents  = <div class='val'>%u</div></summary></details>\n",                 limits->maxTessellationControlPerVertexInputComponents );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxTessellationControlPerVertexOutputComponents = <div class='val'>%u</div></summary></details>\n",                 limits->maxTessellationControlPerVertexOutputComponents);
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxTessellationControlPerPatchOutputComponents  = <div class='val'>%u</div></summary></details>\n",                 limits->maxTessellationControlPerPatchOutputComponents );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxTessellationControlTotalOutputComponents     = <div class='val'>%u</div></summary></details>\n",                 limits->maxTessellationControlTotalOutputComponents    );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxTessellationEvaluationInputComponents        = <div class='val'>%u</div></summary></details>\n",                 limits->maxTessellationEvaluationInputComponents       );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxTessellationEvaluationOutputComponents       = <div class='val'>%u</div></summary></details>\n",                 limits->maxTessellationEvaluationOutputComponents      );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxGeometryShaderInvocations            = <div class='val'>%u</div></summary></details>\n",                 limits->maxGeometryShaderInvocations           );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxGeometryInputComponents              = <div class='val'>%u</div></summary></details>\n",                 limits->maxGeometryInputComponents             );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxGeometryOutputComponents             = <div class='val'>%u</div></summary></details>\n",                 limits->maxGeometryOutputComponents            );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxGeometryOutputVertices               = <div class='val'>%u</div></summary></details>\n",                 limits->maxGeometryOutputVertices              );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxGeometryTotalOutputComponents        = <div class='val'>%u</div></summary></details>\n",                 limits->maxGeometryTotalOutputComponents       );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxFragmentInputComponents              = <div class='val'>%u</div></summary></details>\n",                 limits->maxFragmentInputComponents             );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxFragmentOutputAttachments            = <div class='val'>%u</div></summary></details>\n",                 limits->maxFragmentOutputAttachments           );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxFragmentDualSrcAttachments           = <div class='val'>%u</div></summary></details>\n",                 limits->maxFragmentDualSrcAttachments          );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxFragmentCombinedOutputResources      = <div class='val'>%u</div></summary></details>\n",                 limits->maxFragmentCombinedOutputResources     );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxComputeSharedMemorySize              = <div class='val'>0x%" PRIxLEAST32 "</div></summary></details>\n", limits->maxComputeSharedMemorySize             );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxComputeWorkGroupCount[0]             = <div class='val'>%u</div></summary></details>\n",                 limits->maxComputeWorkGroupCount[0]            );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxComputeWorkGroupCount[1]             = <div class='val'>%u</div></summary></details>\n",                 limits->maxComputeWorkGroupCount[1]            );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxComputeWorkGroupCount[2]             = <div class='val'>%u</div></summary></details>\n",                 limits->maxComputeWorkGroupCount[2]            );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxComputeWorkGroupInvocations          = <div class='val'>%u</div></summary></details>\n",                 limits->maxComputeWorkGroupInvocations         );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxComputeWorkGroupSize[0]              = <div class='val'>%u</div></summary></details>\n",                 limits->maxComputeWorkGroupSize[0]             );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxComputeWorkGroupSize[1]              = <div class='val'>%u</div></summary></details>\n",                 limits->maxComputeWorkGroupSize[1]             );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxComputeWorkGroupSize[2]              = <div class='val'>%u</div></summary></details>\n",                 limits->maxComputeWorkGroupSize[2]             );
        fprintf(out, "\t\t\t\t\t\t<details><summary>subPixelPrecisionBits                   = <div class='val'>%u</div></summary></details>\n",                 limits->subPixelPrecisionBits                  );
        fprintf(out, "\t\t\t\t\t\t<details><summary>subTexelPrecisionBits                   = <div class='val'>%u</div></summary></details>\n",                 limits->subTexelPrecisionBits                  );
        fprintf(out, "\t\t\t\t\t\t<details><summary>mipmapPrecisionBits                     = <div class='val'>%u</div></summary></details>\n",                 limits->mipmapPrecisionBits                    );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxDrawIndexedIndexValue                = <div class='val'>%u</div></summary></details>\n",                 limits->maxDrawIndexedIndexValue               );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxDrawIndirectCount                    = <div class='val'>%u</div></summary></details>\n",                 limits->maxDrawIndirectCount                   );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxSamplerLodBias                       = <div class='val'>%f</div></summary></details>\n",                 limits->maxSamplerLodBias                      );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxSamplerAnisotropy                    = <div class='val'>%f</div></summary></details>\n",                 limits->maxSamplerAnisotropy                   );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxViewports                            = <div class='val'>%u</div></summary></details>\n",                 limits->maxViewports                           );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxViewportDimensions[0]                = <div class='val'>%u</div></summary></details>\n",                 limits->maxViewportDimensions[0]               );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxViewportDimensions[1]                = <div class='val'>%u</div></summary></details>\n",                 limits->maxViewportDimensions[1]               );
        fprintf(out, "\t\t\t\t\t\t<details><summary>viewportBoundsRange[0]                  =<div class='val'>%13f</div></summary></details>\n",                limits->viewportBoundsRange[0]                 );
        fprintf(out, "\t\t\t\t\t\t<details><summary>viewportBoundsRange[1]                  =<div class='val'>%13f</div></summary></details>\n",                limits->viewportBoundsRange[1]                 );
        fprintf(out, "\t\t\t\t\t\t<details><summary>viewportSubPixelBits                    = <div class='val'>%u</div></summary></details>\n",                 limits->viewportSubPixelBits                   );
        fprintf(out, "\t\t\t\t\t\t<details><summary>minMemoryMapAlignment                   = <div class='val'>" PRINTF_SIZE_T_SPECIFIER "</div></summary></details>\n", limits->minMemoryMapAlignment         );
        fprintf(out, "\t\t\t\t\t\t<details><summary>minTexelBufferOffsetAlignment           = <div class='val'>0x%" PRIxLEAST64 "</div></summary></details>\n", limits->minTexelBufferOffsetAlignment          );
        fprintf(out, "\t\t\t\t\t\t<details><summary>minUniformBufferOffsetAlignment         = <div class='val'>0x%" PRIxLEAST64 "</div></summary></details>\n", limits->minUniformBufferOffsetAlignment        );
        fprintf(out, "\t\t\t\t\t\t<details><summary>minStorageBufferOffsetAlignment         = <div class='val'>0x%" PRIxLEAST64 "</div></summary></details>\n", limits->minStorageBufferOffsetAlignment        );
        fprintf(out, "\t\t\t\t\t\t<details><summary>minTexelOffset                          =<div class='val'>%3d</div></summary></details>\n",                 limits->minTexelOffset                         );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxTexelOffset                          =<div class='val'>%3d</div></summary></details>\n",                 limits->maxTexelOffset                         );
        fprintf(out, "\t\t\t\t\t\t<details><summary>minTexelGatherOffset                    =<div class='val'>%3d</div></summary></details>\n",                 limits->minTexelGatherOffset                   );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxTexelGatherOffset                    =<div class='val'>%3d</div></summary></details>\n",                 limits->maxTexelGatherOffset                   );
        fprintf(out, "\t\t\t\t\t\t<details><summary>minInterpolationOffset                  =<div class='val'>%9f</div></summary></details>\n",                 limits->minInterpolationOffset                 );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxInterpolationOffset                  =<div class='val'>%9f</div></summary></details>\n",                 limits->maxInterpolationOffset                 );
        fprintf(out, "\t\t\t\t\t\t<details><summary>subPixelInterpolationOffsetBits         = <div class='val'>%u</div></summary></details>\n",                 limits->subPixelInterpolationOffsetBits        );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxFramebufferWidth                     = <div class='val'>%u</div></summary></details>\n",                 limits->maxFramebufferWidth                    );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxFramebufferHeight                    = <div class='val'>%u</div></summary></details>\n",                 limits->maxFramebufferHeight                   );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxFramebufferLayers                    = <div class='val'>%u</div></summary></details>\n",                 limits->maxFramebufferLayers                   );
        fprintf(out, "\t\t\t\t\t\t<details><summary>framebufferColorSampleCounts            = <div class='val'>%u</div></summary></details>\n",                 limits->framebufferColorSampleCounts           );
        fprintf(out, "\t\t\t\t\t\t<details><summary>framebufferDepthSampleCounts            = <div class='val'>%u</div></summary></details>\n",                 limits->framebufferDepthSampleCounts           );
        fprintf(out, "\t\t\t\t\t\t<details><summary>framebufferStencilSampleCounts          = <div class='val'>%u</div></summary></details>\n",                 limits->framebufferStencilSampleCounts         );
        fprintf(out, "\t\t\t\t\t\t<details><summary>framebufferNoAttachmentsSampleCounts    = <div class='val'>%u</div></summary></details>\n",                 limits->framebufferNoAttachmentsSampleCounts   );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxColorAttachments                     = <div class='val'>%u</div></summary></details>\n",                 limits->maxColorAttachments                    );
        fprintf(out, "\t\t\t\t\t\t<details><summary>sampledImageColorSampleCounts           = <div class='val'>%u</div></summary></details>\n",                 limits->sampledImageColorSampleCounts          );
        fprintf(out, "\t\t\t\t\t\t<details><summary>sampledImageDepthSampleCounts           = <div class='val'>%u</div></summary></details>\n",                 limits->sampledImageDepthSampleCounts          );
        fprintf(out, "\t\t\t\t\t\t<details><summary>sampledImageStencilSampleCounts         = <div class='val'>%u</div></summary></details>\n",                 limits->sampledImageStencilSampleCounts        );
        fprintf(out, "\t\t\t\t\t\t<details><summary>sampledImageIntegerSampleCounts         = <div class='val'>%u</div></summary></details>\n",                 limits->sampledImageIntegerSampleCounts        );
        fprintf(out, "\t\t\t\t\t\t<details><summary>storageImageSampleCounts                = <div class='val'>%u</div></summary></details>\n",                 limits->storageImageSampleCounts               );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxSampleMaskWords                      = <div class='val'>%u</div></summary></details>\n",                 limits->maxSampleMaskWords                     );
        fprintf(out, "\t\t\t\t\t\t<details><summary>timestampComputeAndGraphics             = <div class='val'>%u</div></summary></details>\n",                 limits->timestampComputeAndGraphics            );
        fprintf(out, "\t\t\t\t\t\t<details><summary>timestampPeriod                         = <div class='val'>%f</div></summary></details>\n",                 limits->timestampPeriod                        );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxClipDistances                        = <div class='val'>%u</div></summary></details>\n",                 limits->maxClipDistances                       );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxCullDistances                        = <div class='val'>%u</div></summary></details>\n",                 limits->maxCullDistances                       );
        fprintf(out, "\t\t\t\t\t\t<details><summary>maxCombinedClipAndCullDistances         = <div class='val'>%u</div></summary></details>\n",                 limits->maxCombinedClipAndCullDistances        );
        fprintf(out, "\t\t\t\t\t\t<details><summary>discreteQueuePriorities                 = <div class='val'>%u</div></summary></details>\n",                 limits->discreteQueuePriorities                );
        fprintf(out, "\t\t\t\t\t\t<details><summary>pointSizeRange[0]                       = <div class='val'>%f</div></summary></details>\n",                 limits->pointSizeRange[0]                      );
        fprintf(out, "\t\t\t\t\t\t<details><summary>pointSizeRange[1]                       = <div class='val'>%f</div></summary></details>\n",                 limits->pointSizeRange[1]                      );
        fprintf(out, "\t\t\t\t\t\t<details><summary>lineWidthRange[0]                       = <div class='val'>%f</div></summary></details>\n",                 limits->lineWidthRange[0]                      );
        fprintf(out, "\t\t\t\t\t\t<details><summary>lineWidthRange[1]                       = <div class='val'>%f</div></summary></details>\n",                 limits->lineWidthRange[1]                      );
        fprintf(out, "\t\t\t\t\t\t<details><summary>pointSizeGranularity                    = <div class='val'>%f</div></summary></details>\n",                 limits->pointSizeGranularity                   );
        fprintf(out, "\t\t\t\t\t\t<details><summary>lineWidthGranularity                    = <div class='val'>%f</div></summary></details>\n",                 limits->lineWidthGranularity                   );
        fprintf(out, "\t\t\t\t\t\t<details><summary>strictLines                             = <div class='val'>%u</div></summary></details>\n",                 limits->strictLines                            );
        fprintf(out, "\t\t\t\t\t\t<details><summary>standardSampleLocations                 = <div class='val'>%u</div></summary></details>\n",                 limits->standardSampleLocations                );
        fprintf(out, "\t\t\t\t\t\t<details><summary>optimalBufferCopyOffsetAlignment        = <div class='val'>0x%" PRIxLEAST64 "</div></summary></details>\n", limits->optimalBufferCopyOffsetAlignment       );
        fprintf(out, "\t\t\t\t\t\t<details><summary>optimalBufferCopyRowPitchAlignment      = <div class='val'>0x%" PRIxLEAST64 "</div></summary></details>\n", limits->optimalBufferCopyRowPitchAlignment     );
        fprintf(out, "\t\t\t\t\t\t<details><summary>nonCoherentAtomSize                     = <div class='val'>0x%" PRIxLEAST64 "</div></summary></details>\n", limits->nonCoherentAtomSize                    );
        fprintf(out, "\t\t\t\t\t</details>\n");
    } else if (human_readable_output) {
        printf("\tVkPhysicalDeviceLimits:\n");
        printf("\t-----------------------\n");
        printf("\t\tmaxImageDimension1D                     = %u\n",                 limits->maxImageDimension1D                    );
        printf("\t\tmaxImageDimension2D                     = %u\n",                 limits->maxImageDimension2D                    );
        printf("\t\tmaxImageDimension3D                     = %u\n",                 limits->maxImageDimension3D                    );
        printf("\t\tmaxImageDimensionCube                   = %u\n",                 limits->maxImageDimensionCube                  );
        printf("\t\tmaxImageArrayLayers                     = %u\n",                 limits->maxImageArrayLayers                    );
        printf("\t\tmaxTexelBufferElements                  = 0x%" PRIxLEAST32 "\n", limits->maxTexelBufferElements                 );
        printf("\t\tmaxUniformBufferRange                   = 0x%" PRIxLEAST32 "\n", limits->maxUniformBufferRange                  );
        printf("\t\tmaxStorageBufferRange                   = 0x%" PRIxLEAST32 "\n", limits->maxStorageBufferRange                  );
        printf("\t\tmaxPushConstantsSize                    = %u\n",                 limits->maxPushConstantsSize                   );
        printf("\t\tmaxMemoryAllocationCount                = %u\n",                 limits->maxMemoryAllocationCount               );
        printf("\t\tmaxSamplerAllocationCount               = %u\n",                 limits->maxSamplerAllocationCount              );
        printf("\t\tbufferImageGranularity                  = 0x%" PRIxLEAST64 "\n", limits->bufferImageGranularity                 );
        printf("\t\tsparseAddressSpaceSize                  = 0x%" PRIxLEAST64 "\n", limits->sparseAddressSpaceSize                 );
        printf("\t\tmaxBoundDescriptorSets                  = %u\n",                 limits->maxBoundDescriptorSets                 );
        printf("\t\tmaxPerStageDescriptorSamplers           = %u\n",                 limits->maxPerStageDescriptorSamplers          );
        printf("\t\tmaxPerStageDescriptorUniformBuffers     = %u\n",                 limits->maxPerStageDescriptorUniformBuffers    );
        printf("\t\tmaxPerStageDescriptorStorageBuffers     = %u\n",                 limits->maxPerStageDescriptorStorageBuffers    );
        printf("\t\tmaxPerStageDescriptorSampledImages      = %u\n",                 limits->maxPerStageDescriptorSampledImages     );
        printf("\t\tmaxPerStageDescriptorStorageImages      = %u\n",                 limits->maxPerStageDescriptorStorageImages     );
        printf("\t\tmaxPerStageDescriptorInputAttachments   = %u\n",                 limits->maxPerStageDescriptorInputAttachments  );
        printf("\t\tmaxPerStageResources                    = %u\n",                 limits->maxPerStageResources                   );
        printf("\t\tmaxDescriptorSetSamplers                = %u\n",                 limits->maxDescriptorSetSamplers               );
        printf("\t\tmaxDescriptorSetUniformBuffers          = %u\n",                 limits->maxDescriptorSetUniformBuffers         );
        printf("\t\tmaxDescriptorSetUniformBuffersDynamic   = %u\n",                 limits->maxDescriptorSetUniformBuffersDynamic  );
        printf("\t\tmaxDescriptorSetStorageBuffers          = %u\n",                 limits->maxDescriptorSetStorageBuffers         );
        printf("\t\tmaxDescriptorSetStorageBuffersDynamic   = %u\n",                 limits->maxDescriptorSetStorageBuffersDynamic  );
        printf("\t\tmaxDescriptorSetSampledImages           = %u\n",                 limits->maxDescriptorSetSampledImages          );
        printf("\t\tmaxDescriptorSetStorageImages           = %u\n",                 limits->maxDescriptorSetStorageImages          );
        printf("\t\tmaxDescriptorSetInputAttachments        = %u\n",                 limits->maxDescriptorSetInputAttachments       );
        printf("\t\tmaxVertexInputAttributes                = %u\n",                 limits->maxVertexInputAttributes               );
        printf("\t\tmaxVertexInputBindings                  = %u\n",                 limits->maxVertexInputBindings                 );
        printf("\t\tmaxVertexInputAttributeOffset           = 0x%" PRIxLEAST32 "\n", limits->maxVertexInputAttributeOffset          );
        printf("\t\tmaxVertexInputBindingStride             = 0x%" PRIxLEAST32 "\n", limits->maxVertexInputBindingStride            );
        printf("\t\tmaxVertexOutputComponents               = %u\n",                 limits->maxVertexOutputComponents              );
        printf("\t\tmaxTessellationGenerationLevel          = %u\n",                 limits->maxTessellationGenerationLevel         );
        printf("\t\tmaxTessellationPatchSize                        = %u\n",                 limits->maxTessellationPatchSize                       );
        printf("\t\tmaxTessellationControlPerVertexInputComponents  = %u\n",                 limits->maxTessellationControlPerVertexInputComponents );
        printf("\t\tmaxTessellationControlPerVertexOutputComponents = %u\n",                 limits->maxTessellationControlPerVertexOutputComponents);
        printf("\t\tmaxTessellationControlPerPatchOutputComponents  = %u\n",                 limits->maxTessellationControlPerPatchOutputComponents );
        printf("\t\tmaxTessellationControlTotalOutputComponents     = %u\n",                 limits->maxTessellationControlTotalOutputComponents    );
        printf("\t\tmaxTessellationEvaluationInputComponents        = %u\n",                 limits->maxTessellationEvaluationInputComponents       );
        printf("\t\tmaxTessellationEvaluationOutputComponents       = %u\n",                 limits->maxTessellationEvaluationOutputComponents      );
        printf("\t\tmaxGeometryShaderInvocations            = %u\n",                 limits->maxGeometryShaderInvocations           );
        printf("\t\tmaxGeometryInputComponents              = %u\n",                 limits->maxGeometryInputComponents             );
        printf("\t\tmaxGeometryOutputComponents             = %u\n",                 limits->maxGeometryOutputComponents            );
        printf("\t\tmaxGeometryOutputVertices               = %u\n",                 limits->maxGeometryOutputVertices              );
        printf("\t\tmaxGeometryTotalOutputComponents        = %u\n",                 limits->maxGeometryTotalOutputComponents       );
        printf("\t\tmaxFragmentInputComponents              = %u\n",                 limits->maxFragmentInputComponents             );
        printf("\t\tmaxFragmentOutputAttachments            = %u\n",                 limits->maxFragmentOutputAttachments           );
        printf("\t\tmaxFragmentDualSrcAttachments           = %u\n",                 limits->maxFragmentDualSrcAttachments          );
        printf("\t\tmaxFragmentCombinedOutputResources      = %u\n",                 limits->maxFragmentCombinedOutputResources     );
        printf("\t\tmaxComputeSharedMemorySize              = 0x%" PRIxLEAST32 "\n", limits->maxComputeSharedMemorySize             );
        printf("\t\tmaxComputeWorkGroupCount[0]             = %u\n",                 limits->maxComputeWorkGroupCount[0]            );
        printf("\t\tmaxComputeWorkGroupCount[1]             = %u\n",                 limits->maxComputeWorkGroupCount[1]            );
        printf("\t\tmaxComputeWorkGroupCount[2]             = %u\n",                 limits->maxComputeWorkGroupCount[2]            );
        printf("\t\tmaxComputeWorkGroupInvocations          = %u\n",                 limits->maxComputeWorkGroupInvocations         );
        printf("\t\tmaxComputeWorkGroupSize[0]              = %u\n",                 limits->maxComputeWorkGroupSize[0]             );
        printf("\t\tmaxComputeWorkGroupSize[1]              = %u\n",                 limits->maxComputeWorkGroupSize[1]             );
        printf("\t\tmaxComputeWorkGroupSize[2]              = %u\n",                 limits->maxComputeWorkGroupSize[2]             );
        printf("\t\tsubPixelPrecisionBits                   = %u\n",                 limits->subPixelPrecisionBits                  );
        printf("\t\tsubTexelPrecisionBits                   = %u\n",                 limits->subTexelPrecisionBits                  );
        printf("\t\tmipmapPrecisionBits                     = %u\n",                 limits->mipmapPrecisionBits                    );
        printf("\t\tmaxDrawIndexedIndexValue                = %u\n",                 limits->maxDrawIndexedIndexValue               );
        printf("\t\tmaxDrawIndirectCount                    = %u\n",                 limits->maxDrawIndirectCount                   );
        printf("\t\tmaxSamplerLodBias                       = %f\n",                 limits->maxSamplerLodBias                      );
        printf("\t\tmaxSamplerAnisotropy                    = %f\n",                 limits->maxSamplerAnisotropy                   );
        printf("\t\tmaxViewports                            = %u\n",                 limits->maxViewports                           );
        printf("\t\tmaxViewportDimensions[0]                = %u\n",                 limits->maxViewportDimensions[0]               );
        printf("\t\tmaxViewportDimensions[1]                = %u\n",                 limits->maxViewportDimensions[1]               );
        printf("\t\tviewportBoundsRange[0]                  =%13f\n",                limits->viewportBoundsRange[0]                 );
        printf("\t\tviewportBoundsRange[1]                  =%13f\n",                limits->viewportBoundsRange[1]                 );
        printf("\t\tviewportSubPixelBits                    = %u\n",                 limits->viewportSubPixelBits                   );
        printf("\t\tminMemoryMapAlignment                   = " PRINTF_SIZE_T_SPECIFIER "\n", limits->minMemoryMapAlignment         );
        printf("\t\tminTexelBufferOffsetAlignment           = 0x%" PRIxLEAST64 "\n", limits->minTexelBufferOffsetAlignment          );
        printf("\t\tminUniformBufferOffsetAlignment         = 0x%" PRIxLEAST64 "\n", limits->minUniformBufferOffsetAlignment        );
        printf("\t\tminStorageBufferOffsetAlignment         = 0x%" PRIxLEAST64 "\n", limits->minStorageBufferOffsetAlignment        );
        printf("\t\tminTexelOffset                          =%3d\n",                 limits->minTexelOffset                         );
        printf("\t\tmaxTexelOffset                          =%3d\n",                 limits->maxTexelOffset                         );
        printf("\t\tminTexelGatherOffset                    =%3d\n",                 limits->minTexelGatherOffset                   );
        printf("\t\tmaxTexelGatherOffset                    =%3d\n",                 limits->maxTexelGatherOffset                   );
        printf("\t\tminInterpolationOffset                  =%9f\n",                 limits->minInterpolationOffset                 );
        printf("\t\tmaxInterpolationOffset                  =%9f\n",                 limits->maxInterpolationOffset                 );
        printf("\t\tsubPixelInterpolationOffsetBits         = %u\n",                 limits->subPixelInterpolationOffsetBits        );
        printf("\t\tmaxFramebufferWidth                     = %u\n",                 limits->maxFramebufferWidth                    );
        printf("\t\tmaxFramebufferHeight                    = %u\n",                 limits->maxFramebufferHeight                   );
        printf("\t\tmaxFramebufferLayers                    = %u\n",                 limits->maxFramebufferLayers                   );
        printf("\t\tframebufferColorSampleCounts            = %u\n",                 limits->framebufferColorSampleCounts           );
        printf("\t\tframebufferDepthSampleCounts            = %u\n",                 limits->framebufferDepthSampleCounts           );
        printf("\t\tframebufferStencilSampleCounts          = %u\n",                 limits->framebufferStencilSampleCounts         );
        printf("\t\tframebufferNoAttachmentsSampleCounts    = %u\n",                 limits->framebufferNoAttachmentsSampleCounts   );
        printf("\t\tmaxColorAttachments                     = %u\n",                 limits->maxColorAttachments                    );
        printf("\t\tsampledImageColorSampleCounts           = %u\n",                 limits->sampledImageColorSampleCounts          );
        printf("\t\tsampledImageDepthSampleCounts           = %u\n",                 limits->sampledImageDepthSampleCounts          );
        printf("\t\tsampledImageStencilSampleCounts         = %u\n",                 limits->sampledImageStencilSampleCounts        );
        printf("\t\tsampledImageIntegerSampleCounts         = %u\n",                 limits->sampledImageIntegerSampleCounts        );
        printf("\t\tstorageImageSampleCounts                = %u\n",                 limits->storageImageSampleCounts               );
        printf("\t\tmaxSampleMaskWords                      = %u\n",                 limits->maxSampleMaskWords                     );
        printf("\t\ttimestampComputeAndGraphics             = %u\n",                 limits->timestampComputeAndGraphics            );
        printf("\t\ttimestampPeriod                         = %f\n",                 limits->timestampPeriod                        );
        printf("\t\tmaxClipDistances                        = %u\n",                 limits->maxClipDistances                       );
        printf("\t\tmaxCullDistances                        = %u\n",                 limits->maxCullDistances                       );
        printf("\t\tmaxCombinedClipAndCullDistances         = %u\n",                 limits->maxCombinedClipAndCullDistances        );
        printf("\t\tdiscreteQueuePriorities                 = %u\n",                 limits->discreteQueuePriorities                );
        printf("\t\tpointSizeRange[0]                       = %f\n",                 limits->pointSizeRange[0]                      );
        printf("\t\tpointSizeRange[1]                       = %f\n",                 limits->pointSizeRange[1]                      );
        printf("\t\tlineWidthRange[0]                       = %f\n",                 limits->lineWidthRange[0]                      );
        printf("\t\tlineWidthRange[1]                       = %f\n",                 limits->lineWidthRange[1]                      );
        printf("\t\tpointSizeGranularity                    = %f\n",                 limits->pointSizeGranularity                   );
        printf("\t\tlineWidthGranularity                    = %f\n",                 limits->lineWidthGranularity                   );
        printf("\t\tstrictLines                             = %u\n",                 limits->strictLines                            );
        printf("\t\tstandardSampleLocations                 = %u\n",                 limits->standardSampleLocations                );
        printf("\t\toptimalBufferCopyOffsetAlignment        = 0x%" PRIxLEAST64 "\n", limits->optimalBufferCopyOffsetAlignment       );
        printf("\t\toptimalBufferCopyRowPitchAlignment      = 0x%" PRIxLEAST64 "\n", limits->optimalBufferCopyRowPitchAlignment     );
        printf("\t\tnonCoherentAtomSize                     = 0x%" PRIxLEAST64 "\n", limits->nonCoherentAtomSize                    );
    }
    if (json_output) {
        printf(",\n");
        printf("\t\t\"limits\": {\n");
        printf("\t\t\t\"bufferImageGranularity\": %llu,\n",                               (unsigned long long)limits->bufferImageGranularity);
        printf("\t\t\t\"discreteQueuePriorities\": %u,\n",                                limits->discreteQueuePriorities);
        printf("\t\t\t\"framebufferColorSampleCounts\": %u,\n",                           limits->framebufferColorSampleCounts);
        printf("\t\t\t\"framebufferDepthSampleCounts\": %u,\n",                           limits->framebufferDepthSampleCounts);
        printf("\t\t\t\"framebufferNoAttachmentsSampleCounts\": %u,\n",                   limits->framebufferNoAttachmentsSampleCounts);
        printf("\t\t\t\"framebufferStencilSampleCounts\": %u,\n",                         limits->framebufferStencilSampleCounts);
        printf("\t\t\t\"lineWidthGranularity\": %g,\n",                                   limits->lineWidthGranularity);
        printf("\t\t\t\"lineWidthRange\": [\n");
        printf("\t\t\t\t%g,\n",                                                           limits->lineWidthRange[0]);
        printf("\t\t\t\t%g\n",                                                            limits->lineWidthRange[1]);
        printf("\t\t\t],\n");
        printf("\t\t\t\"maxBoundDescriptorSets\": %u,\n",                                 limits->maxBoundDescriptorSets);
        printf("\t\t\t\"maxClipDistances\": %u,\n",                                       limits->maxClipDistances);
        printf("\t\t\t\"maxColorAttachments\": %u,\n",                                    limits->maxColorAttachments);
        printf("\t\t\t\"maxCombinedClipAndCullDistances\": %u,\n",                        limits->maxCombinedClipAndCullDistances);
        printf("\t\t\t\"maxComputeSharedMemorySize\": %u,\n",                             limits->maxComputeSharedMemorySize);
        printf("\t\t\t\"maxComputeWorkGroupCount\": [\n");
        printf("\t\t\t\t%u,\n",                                                           limits->maxComputeWorkGroupCount[0]);
        printf("\t\t\t\t%u,\n",                                                           limits->maxComputeWorkGroupCount[1]);
        printf("\t\t\t\t%u\n",                                                            limits->maxComputeWorkGroupCount[2]);
        printf("\t\t\t],\n");
        printf("\t\t\t\"maxComputeWorkGroupInvocations\": %u,\n",                         limits->maxComputeWorkGroupInvocations);
        printf("\t\t\t\"maxComputeWorkGroupSize\": [\n");
        printf("\t\t\t\t%u,\n",                                                           limits->maxComputeWorkGroupSize[0]);
        printf("\t\t\t\t%u,\n",                                                           limits->maxComputeWorkGroupSize[1]);
        printf("\t\t\t\t%u\n",                                                            limits->maxComputeWorkGroupSize[2]);
        printf("\t\t\t],\n");
        printf("\t\t\t\"maxCullDistances\": %u,\n",                                       limits->maxCullDistances);
        printf("\t\t\t\"maxDescriptorSetInputAttachments\": %u,\n",                       limits->maxDescriptorSetInputAttachments);
        printf("\t\t\t\"maxDescriptorSetSampledImages\": %u,\n",                          limits->maxDescriptorSetSampledImages);
        printf("\t\t\t\"maxDescriptorSetSamplers\": %u,\n",                               limits->maxDescriptorSetSamplers);
        printf("\t\t\t\"maxDescriptorSetStorageBuffers\": %u,\n",                         limits->maxDescriptorSetStorageBuffers);
        printf("\t\t\t\"maxDescriptorSetStorageBuffersDynamic\": %u,\n",                  limits->maxDescriptorSetStorageBuffersDynamic);
        printf("\t\t\t\"maxDescriptorSetStorageImages\": %u,\n",                          limits->maxDescriptorSetStorageImages);
        printf("\t\t\t\"maxDescriptorSetUniformBuffers\": %u,\n",                         limits->maxDescriptorSetUniformBuffers);
        printf("\t\t\t\"maxDescriptorSetUniformBuffersDynamic\": %u,\n",                  limits->maxDescriptorSetUniformBuffersDynamic);
        printf("\t\t\t\"maxDrawIndexedIndexValue\": %u,\n",                               limits->maxDrawIndexedIndexValue);
        printf("\t\t\t\"maxDrawIndirectCount\": %u,\n",                                   limits->maxDrawIndirectCount);
        printf("\t\t\t\"maxFragmentCombinedOutputResources\": %u,\n",                     limits->maxFragmentCombinedOutputResources);
        printf("\t\t\t\"maxFragmentDualSrcAttachments\": %u,\n",                          limits->maxFragmentDualSrcAttachments);
        printf("\t\t\t\"maxFragmentInputComponents\": %u,\n",                             limits->maxFragmentInputComponents);
        printf("\t\t\t\"maxFragmentOutputAttachments\": %u,\n",                           limits->maxFragmentOutputAttachments);
        printf("\t\t\t\"maxFramebufferHeight\": %u,\n",                                   limits->maxFramebufferHeight);
        printf("\t\t\t\"maxFramebufferLayers\": %u,\n",                                   limits->maxFramebufferLayers);
        printf("\t\t\t\"maxFramebufferWidth\": %u,\n",                                    limits->maxFramebufferWidth);
        printf("\t\t\t\"maxGeometryInputComponents\": %u,\n",                             limits->maxGeometryInputComponents);
        printf("\t\t\t\"maxGeometryOutputComponents\": %u,\n",                            limits->maxGeometryOutputComponents);
        printf("\t\t\t\"maxGeometryOutputVertices\": %u,\n",                              limits->maxGeometryOutputVertices);
        printf("\t\t\t\"maxGeometryShaderInvocations\": %u,\n",                           limits->maxGeometryShaderInvocations);
        printf("\t\t\t\"maxGeometryTotalOutputComponents\": %u,\n",                       limits->maxGeometryTotalOutputComponents);
        printf("\t\t\t\"maxImageArrayLayers\": %u,\n",                                    limits->maxImageArrayLayers);
        printf("\t\t\t\"maxImageDimension1D\": %u,\n",                                    limits->maxImageDimension1D);
        printf("\t\t\t\"maxImageDimension2D\": %u,\n",                                    limits->maxImageDimension2D);
        printf("\t\t\t\"maxImageDimension3D\": %u,\n",                                    limits->maxImageDimension3D);
        printf("\t\t\t\"maxImageDimensionCube\": %u,\n",                                  limits->maxImageDimensionCube);
        printf("\t\t\t\"maxInterpolationOffset\": %g,\n",                                 limits->maxInterpolationOffset);
        printf("\t\t\t\"maxMemoryAllocationCount\": %u,\n",                               limits->maxMemoryAllocationCount);
        printf("\t\t\t\"maxPerStageDescriptorInputAttachments\": %u,\n",                  limits->maxPerStageDescriptorInputAttachments);
        printf("\t\t\t\"maxPerStageDescriptorSampledImages\": %u,\n",                     limits->maxPerStageDescriptorSampledImages);
        printf("\t\t\t\"maxPerStageDescriptorSamplers\": %u,\n",                          limits->maxPerStageDescriptorSamplers);
        printf("\t\t\t\"maxPerStageDescriptorStorageBuffers\": %u,\n",                    limits->maxPerStageDescriptorStorageBuffers);
        printf("\t\t\t\"maxPerStageDescriptorStorageImages\": %u,\n",                     limits->maxPerStageDescriptorStorageImages);
        printf("\t\t\t\"maxPerStageDescriptorUniformBuffers\": %u,\n",                    limits->maxPerStageDescriptorUniformBuffers);
        printf("\t\t\t\"maxPerStageResources\": %u,\n",                                   limits->maxPerStageResources);
        printf("\t\t\t\"maxPushConstantsSize\": %u,\n",                                   limits->maxPushConstantsSize);
        printf("\t\t\t\"maxSampleMaskWords\": %u,\n",                                     limits->maxSampleMaskWords);
        printf("\t\t\t\"maxSamplerAllocationCount\": %u,\n",                              limits->maxSamplerAllocationCount);
        printf("\t\t\t\"maxSamplerAnisotropy\": %g,\n",                                   limits->maxSamplerAnisotropy);
        printf("\t\t\t\"maxSamplerLodBias\": %g,\n",                                      limits->maxSamplerLodBias);
        printf("\t\t\t\"maxStorageBufferRange\": %u,\n",                                  limits->maxStorageBufferRange);
        printf("\t\t\t\"maxTessellationControlPerPatchOutputComponents\": %u,\n",         limits->maxTessellationControlPerPatchOutputComponents);
        printf("\t\t\t\"maxTessellationControlPerVertexInputComponents\": %u,\n",         limits->maxTessellationControlPerVertexInputComponents);
        printf("\t\t\t\"maxTessellationControlPerVertexOutputComponents\": %u,\n",        limits->maxTessellationControlPerVertexOutputComponents);
        printf("\t\t\t\"maxTessellationControlTotalOutputComponents\": %u,\n",            limits->maxTessellationControlTotalOutputComponents);
        printf("\t\t\t\"maxTessellationEvaluationInputComponents\": %u,\n",               limits->maxTessellationEvaluationInputComponents);
        printf("\t\t\t\"maxTessellationEvaluationOutputComponents\": %u,\n",              limits->maxTessellationEvaluationOutputComponents);
        printf("\t\t\t\"maxTessellationGenerationLevel\": %u,\n",                         limits->maxTessellationGenerationLevel);
        printf("\t\t\t\"maxTessellationPatchSize\": %u,\n",                               limits->maxTessellationPatchSize);
        printf("\t\t\t\"maxTexelBufferElements\": %u,\n",                                 limits->maxTexelBufferElements);
        printf("\t\t\t\"maxTexelGatherOffset\": %u,\n",                                   limits->maxTexelGatherOffset);
        printf("\t\t\t\"maxTexelOffset\": %u,\n",                                         limits->maxTexelOffset);
        printf("\t\t\t\"maxUniformBufferRange\": %u,\n",                                  limits->maxUniformBufferRange);
        printf("\t\t\t\"maxVertexInputAttributeOffset\": %u,\n",                          limits->maxVertexInputAttributeOffset);
        printf("\t\t\t\"maxVertexInputAttributes\": %u,\n",                               limits->maxVertexInputAttributes);
        printf("\t\t\t\"maxVertexInputBindings\": %u,\n",                                 limits->maxVertexInputBindings);
        printf("\t\t\t\"maxVertexInputBindingStride\": %u,\n",                            limits->maxVertexInputBindingStride);
        printf("\t\t\t\"maxVertexOutputComponents\": %u,\n",                              limits->maxVertexOutputComponents);
        printf("\t\t\t\"maxViewportDimensions\": [\n");
        printf("\t\t\t\t%u,\n",                                                           limits->maxViewportDimensions[0]);
        printf("\t\t\t\t%u\n",                                                            limits->maxViewportDimensions[1]);
        printf("\t\t\t],\n");
        printf("\t\t\t\"maxViewports\": %u,\n",                                           limits->maxViewports);
        printf("\t\t\t\"minInterpolationOffset\": %g,\n",                                 limits->minInterpolationOffset);
        printf("\t\t\t\"minMemoryMapAlignment\": " PRINTF_SIZE_T_SPECIFIER ",\n",         limits->minMemoryMapAlignment);
        printf("\t\t\t\"minStorageBufferOffsetAlignment\": %llu,\n",                      (unsigned long long)limits->minStorageBufferOffsetAlignment);
        printf("\t\t\t\"minTexelBufferOffsetAlignment\": %llu,\n",                        (unsigned long long)limits->minTexelBufferOffsetAlignment);
        printf("\t\t\t\"minTexelGatherOffset\": %d,\n",                                   limits->minTexelGatherOffset);
        printf("\t\t\t\"minTexelOffset\": %d,\n",                                         limits->minTexelOffset);
        printf("\t\t\t\"minUniformBufferOffsetAlignment\": %llu,\n",                      (unsigned long long)limits->minUniformBufferOffsetAlignment);
        printf("\t\t\t\"mipmapPrecisionBits\": %u,\n",                                    limits->mipmapPrecisionBits);
        printf("\t\t\t\"nonCoherentAtomSize\": %llu,\n",                                  (unsigned long long)limits->nonCoherentAtomSize);
        printf("\t\t\t\"optimalBufferCopyOffsetAlignment\": %llu,\n",                     (unsigned long long)limits->optimalBufferCopyOffsetAlignment);
        printf("\t\t\t\"optimalBufferCopyRowPitchAlignment\": %llu,\n",                   (unsigned long long)limits->optimalBufferCopyRowPitchAlignment);
        printf("\t\t\t\"pointSizeGranularity\": %g,\n",                                   limits->pointSizeGranularity);
        printf("\t\t\t\"pointSizeRange\": [\n");
        printf("\t\t\t\t%g,\n",                                                           limits->pointSizeRange[0]);
        printf("\t\t\t\t%g\n",                                                            limits->pointSizeRange[1]);
        printf("\t\t\t],\n");
        printf("\t\t\t\"sampledImageColorSampleCounts\": %u,\n",                          limits->sampledImageColorSampleCounts);
        printf("\t\t\t\"sampledImageDepthSampleCounts\": %u,\n",                          limits->sampledImageDepthSampleCounts);
        printf("\t\t\t\"sampledImageIntegerSampleCounts\": %u,\n",                        limits->sampledImageIntegerSampleCounts);
        printf("\t\t\t\"sampledImageStencilSampleCounts\": %u,\n",                        limits->sampledImageStencilSampleCounts);
        printf("\t\t\t\"sparseAddressSpaceSize\": %llu,\n",                               (unsigned long long)limits->sparseAddressSpaceSize);
        printf("\t\t\t\"standardSampleLocations\": %u,\n",                                limits->standardSampleLocations);
        printf("\t\t\t\"storageImageSampleCounts\": %u,\n",                               limits->storageImageSampleCounts);
        printf("\t\t\t\"strictLines\": %u,\n",                                            limits->strictLines);
        printf("\t\t\t\"subPixelInterpolationOffsetBits\": %u,\n",                        limits->subPixelInterpolationOffsetBits);
        printf("\t\t\t\"subPixelPrecisionBits\": %u,\n",                                  limits->subPixelPrecisionBits);
        printf("\t\t\t\"subTexelPrecisionBits\": %u,\n",                                  limits->subTexelPrecisionBits);
        printf("\t\t\t\"timestampComputeAndGraphics\": %u,\n",                            limits->timestampComputeAndGraphics);
        printf("\t\t\t\"timestampPeriod\": %g,\n",                                        limits->timestampPeriod);
        printf("\t\t\t\"viewportBoundsRange\": [\n");
        printf("\t\t\t\t%g,\n",                                                           limits->viewportBoundsRange[0]);
        printf("\t\t\t\t%g\n",                                                            limits->viewportBoundsRange[1]);
        printf("\t\t\t],\n");
        printf("\t\t\t\"viewportSubPixelBits\": %u\n",                                    limits->viewportSubPixelBits);
        printf("\t\t}");
    }
}

static void AppGpuDumpProps(const struct AppGpu *gpu, FILE *out) {
     VkPhysicalDeviceProperties props;

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
        gpu->inst->inst_extensions_count)) {
        const VkPhysicalDeviceProperties *props2_const = &gpu->props2.properties;
        props = *props2_const;
    } else {
        const VkPhysicalDeviceProperties *props_const = &gpu->props;
        props = *props_const;
    }
    const uint32_t apiVersion = props.apiVersion;
    const uint32_t major = VK_VERSION_MAJOR(apiVersion);
    const uint32_t minor = VK_VERSION_MINOR(apiVersion);
    const uint32_t patch = VK_VERSION_PATCH(apiVersion);

    if (html_output) {
        fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceProperties</summary>\n");
        fprintf(out, "\t\t\t\t\t\t<details><summary>apiVersion = <div class='val'>0x%" PRIxLEAST32 "</div>  (<div class='val'>%d.%d.%d</div>)</summary></details>\n", apiVersion, major, minor, patch);
        fprintf(out, "\t\t\t\t\t\t<details><summary>driverVersion = <div class='val'>%u</div> (<div class='val'>0x%" PRIxLEAST32 "</div>)</summary></details>\n", props.driverVersion, props.driverVersion);
        fprintf(out, "\t\t\t\t\t\t<details><summary>vendorID = <div class='val'>0x%04x</div></summary></details>\n", props.vendorID);
        fprintf(out, "\t\t\t\t\t\t<details><summary>deviceID = <div class='val'>0x%04x</div></summary></details>\n", props.deviceID);
        fprintf(out, "\t\t\t\t\t\t<details><summary>deviceType = %s</summary></details>\n", VkPhysicalDeviceTypeString(props.deviceType));
        fprintf(out, "\t\t\t\t\t\t<details><summary>deviceName = %s</summary></details>\n", props.deviceName);
        fprintf(out, "\t\t\t\t\t</details>\n");
    } else if (human_readable_output) {
        printf("VkPhysicalDeviceProperties:\n");
        printf("===========================\n");
        printf("\tapiVersion     = 0x%" PRIxLEAST32 "  (%d.%d.%d)\n", apiVersion, major, minor, patch);
        printf("\tdriverVersion  = %u (0x%" PRIxLEAST32 ")\n", props.driverVersion, props.driverVersion);
        printf("\tvendorID       = 0x%04x\n", props.vendorID);
        printf("\tdeviceID       = 0x%04x\n", props.deviceID);
        printf("\tdeviceType     = %s\n", VkPhysicalDeviceTypeString(props.deviceType));
        printf("\tdeviceName     = %s\n", props.deviceName);
    }
    if (json_output) {
        printf(",\n");
        printf("\t\"VkPhysicalDeviceProperties\": {\n");
        printf("\t\t\"apiVersion\": %u,\n", apiVersion);
        printf("\t\t\"deviceID\": %u,\n", props.deviceID);
        printf("\t\t\"deviceName\": \"%s\",\n", props.deviceName);
        printf("\t\t\"deviceType\": %u,\n", props.deviceType);
        printf("\t\t\"driverVersion\": %u", props.driverVersion);
    }

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions, gpu->inst->inst_extensions_count)) {
        AppDumpLimits(&gpu->props2.properties.limits, out);
    } else {
        AppDumpLimits(&gpu->props.limits, out);
    }

    // Dump pipeline cache UUIDs to json
    if (json_output) {
        printf(",\n");
        printf("\t\t\"pipelineCacheUUID\": [");
        for (uint32_t i = 0; i < VK_UUID_SIZE; ++i) {
            if (i > 0) {
                printf(",");
            }
            printf("\n");
            printf("\t\t\t%u", props.pipelineCacheUUID[i]);
        }
        printf("\n");
        printf("\t\t]");
    }

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions, gpu->inst->inst_extensions_count)) {
        AppDumpSparseProps(&gpu->props2.properties.sparseProperties, out);
    } else {
        AppDumpSparseProps(&gpu->props.sparseProperties, out);
    }

    if (json_output) {
        printf(",\n");
        printf("\t\t\"vendorID\": %u\n", props.vendorID);
        printf("\t}");
    }

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
        gpu->inst->inst_extensions_count)) {
        void *place = gpu->props2.pNext;
        while (place) {
            struct VkStructureHeader *structure = (struct VkStructureHeader*) place;
            if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BLEND_OPERATION_ADVANCED_PROPERTIES_EXT && CheckPhysicalDeviceExtensionIncluded(VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME, gpu->device_extensions, gpu->device_extension_count)) {
                VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT *blend_op_adv_props = (VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT*)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceBlendOperationAdvancedProperties</summary>\n");
                    fprintf(out, "\t\t\t\t\t\t<details><summary>advancedBlendMaxColorAttachments                = <div class='val'>%u</div></summary></details>\n", blend_op_adv_props->advancedBlendMaxColorAttachments     );
                    fprintf(out, "\t\t\t\t\t\t<details><summary>advancedBlendIndependentBlend                   = <div class='val'>%u</div></summary></details>\n", blend_op_adv_props->advancedBlendIndependentBlend        );
                    fprintf(out, "\t\t\t\t\t\t<details><summary>advancedBlendNonPremultipliedSrcColor           = <div class='val'>%u</div></summary></details>\n", blend_op_adv_props->advancedBlendNonPremultipliedSrcColor);
                    fprintf(out, "\t\t\t\t\t\t<details><summary>advancedBlendNonPremultipliedDstColor           = <div class='val'>%u</div></summary></details>\n", blend_op_adv_props->advancedBlendNonPremultipliedDstColor);
                    fprintf(out, "\t\t\t\t\t\t<details><summary>advancedBlendCorrelatedOverlap                  = <div class='val'>%u</div></summary></details>\n", blend_op_adv_props->advancedBlendCorrelatedOverlap       );
                    fprintf(out, "\t\t\t\t\t\t<details><summary>advancedBlendAllOperations                      = <div class='val'>%u</div></summary></details>\n", blend_op_adv_props->advancedBlendAllOperations           );
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceBlendOperationAdvancedProperties:\n");
                    printf("=================================================\n");
                    printf("\tadvancedBlendMaxColorAttachments               = %u\n", blend_op_adv_props->advancedBlendMaxColorAttachments     );
                    printf("\tadvancedBlendIndependentBlend                  = %u\n", blend_op_adv_props->advancedBlendIndependentBlend        );
                    printf("\tadvancedBlendNonPremultipliedSrcColor          = %u\n", blend_op_adv_props->advancedBlendNonPremultipliedSrcColor);
                    printf("\tadvancedBlendNonPremultipliedDstColor          = %u\n", blend_op_adv_props->advancedBlendNonPremultipliedDstColor);
                    printf("\tadvancedBlendCorrelatedOverlap                 = %u\n", blend_op_adv_props->advancedBlendCorrelatedOverlap       );
                    printf("\tadvancedBlendAllOperations                     = %u\n", blend_op_adv_props->advancedBlendAllOperations           );
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES_KHR && CheckPhysicalDeviceExtensionIncluded(VK_KHR_MAINTENANCE2_EXTENSION_NAME, gpu->device_extensions, gpu->device_extension_count)) {
                VkPhysicalDevicePointClippingPropertiesKHR *pt_clip_props = (VkPhysicalDevicePointClippingPropertiesKHR*)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDevicePointClippingProperties</summary>\n");
                    fprintf(out, "\t\t\t\t\t\t<details><summary>pointClippingBehavior               = <div class='val'>%u</div></summary></details>\n", pt_clip_props->pointClippingBehavior);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDevicePointClippingProperties:\n");
                    printf("========================================\n");
                    printf("\tpointClippingBehavior               = %u\n", pt_clip_props->pointClippingBehavior);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR && CheckPhysicalDeviceExtensionIncluded(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME, gpu->device_extensions, gpu->device_extension_count)) {
                VkPhysicalDevicePushDescriptorPropertiesKHR *push_desc_props = (VkPhysicalDevicePushDescriptorPropertiesKHR*)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDevicePushDescriptorProperties</summary>\n");
                    fprintf(out, "\t\t\t\t\t\t<details><summary>maxPushDescriptors                = <div class='val'>%u</div></summary></details>\n", push_desc_props->maxPushDescriptors);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDevicePushDescriptorProperties:\n");
                    printf("=========================================\n");
                    printf("\tmaxPushDescriptors               = %u\n", push_desc_props->maxPushDescriptors);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DISCARD_RECTANGLE_PROPERTIES_EXT && CheckPhysicalDeviceExtensionIncluded(VK_EXT_DISCARD_RECTANGLES_EXTENSION_NAME, gpu->device_extensions, gpu->device_extension_count)) {
                VkPhysicalDeviceDiscardRectanglePropertiesEXT *discard_rect_props = (VkPhysicalDeviceDiscardRectanglePropertiesEXT*)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceDiscardRectangleProperties</summary>\n");
                    fprintf(out, "\t\t\t\t\t\t<details><summary>maxDiscardRectangles               = <div class='val'>%u</div></summary></details>\n", discard_rect_props->maxDiscardRectangles);
                    fprintf(out, "\t\t\t\t\t</details>\n");
                } else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceDiscardRectangleProperties:\n");
                    printf("=========================================\n");
                    printf("\tmaxDiscardRectangles               = %u\n", discard_rect_props->maxDiscardRectangles);
                }
            } else if (structure->sType == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR && CheckPhysicalDeviceExtensionIncluded(VK_KHR_MULTIVIEW_EXTENSION_NAME, gpu->device_extensions, gpu->device_extension_count)) {
                VkPhysicalDeviceMultiviewPropertiesKHR *multiview_props = (VkPhysicalDeviceMultiviewPropertiesKHR*)structure;
                if (html_output) {
                    fprintf(out, "\n\t\t\t\t\t<details><summary>VkPhysicalDeviceMultiviewProperties</summary>\n");
                    fprintf(out, "\t\t\t\t\t\t<details><summary>maxMultiviewInstanceIndex = <div class='val'>%u</div></summary></details>\n", multiview_props->maxMultiviewInstanceIndex);
                    fprintf(out, "\t\t\t\t\t\t<details><summary>maxMultiviewViewCount     = <div class='val'>%u</div></summary></details>\n", multiview_props->maxMultiviewViewCount    );
                    fprintf(out, "\t\t\t\t\t</details>\n");
                }
                else if (human_readable_output) {
                    printf("\nVkPhysicalDeviceMultiviewProperties:\n");
                    printf("=========================================\n");
                    printf("\tmaxMultiviewInstanceIndex = %u\n", multiview_props->maxMultiviewInstanceIndex);
                    printf("\tmaxMultiviewViewCount     = %u\n", multiview_props->maxMultiviewViewCount    );
                }
            }
            place = structure->pNext;
        }
    }

    fflush(out);
    fflush(stdout);
}

static void AppDumpExtensions(const char *indent, const char *layer_name, const uint32_t extension_count,
                              const VkExtensionProperties *extension_properties, FILE *out) {
    if (html_output) {
        fprintf(out, "\t\t\t%s<details><summary>", indent);
    }
    if (layer_name && (strlen(layer_name) > 0)) {
        if (html_output) {
            fprintf(out, "%s Extensions", layer_name);
        } else if (human_readable_output) {
            printf("%s%s Extensions", indent, layer_name);
        }
    } else {
        if (html_output) {
            fprintf(out, "Extensions");
        } else if (human_readable_output) {
            printf("%sExtensions", indent);
        }
    }
    if (html_output) {
        fprintf(out, "\tcount = <div class='val'>%d</div></summary>", extension_count);
        if (extension_count > 0) {
            fprintf(out, "\n");
        }
    } else if (human_readable_output) {
        printf("\tcount = %d\n", extension_count);
    }

    const bool is_device_type = strcmp(layer_name, "Device") == 0;
    if (is_device_type && json_output) {
        printf(",\n");
        printf("\t\"ArrayOfVkExtensionProperties\": [");
    }

    for (uint32_t i = 0; i < extension_count; ++i) {
        VkExtensionProperties const *ext_prop = &extension_properties[i];
        if (html_output) {
            fprintf(out, "\t\t\t\t%s<details><summary>", indent);
            fprintf(out, "<div class='type'>%s</div>: extension revision <div class='val'>%d</div>", ext_prop->extensionName,
                    ext_prop->specVersion);
            fprintf(out, "</summary></details>\n");
        } else if (human_readable_output) {
            printf("%s\t", indent);
            printf("%-36s: extension revision %2d\n", ext_prop->extensionName, ext_prop->specVersion);
        }
        if (is_device_type && json_output) {
            if (i > 0) {
                printf(",");
            }
            printf("\n");
            printf("\t\t{\n");
            printf("\t\t\t\"extensionName\": \"%s\",\n", ext_prop->extensionName);
            printf("\t\t\t\"specVersion\": %u\n", ext_prop->specVersion);
            printf("\t\t}");
        }
    }
    if (html_output) {
        if (extension_count > 0) {
            fprintf(out, "\t\t\t%s</details>\n", indent);
        } else {
            fprintf(out, "</details>\n");
        }
    }
    if (is_device_type && json_output) {
        if (extension_count > 0) {
            printf("\n\t");
        }
        printf("]");
    }

    fflush(out);
    fflush(stdout);
}

static void AppGpuDumpQueueProps(const struct AppGpu *gpu, uint32_t id, FILE *out) {
    VkQueueFamilyProperties props;

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
        gpu->inst->inst_extensions_count)) {
        const VkQueueFamilyProperties *props2_const = &gpu->queue_props2[id].queueFamilyProperties;
        props = *props2_const;
    } else {
        const VkQueueFamilyProperties *props_const = &gpu->queue_props[id];
        props = *props_const;
    }

    if (html_output) {
        fprintf(out, "\t\t\t\t\t<details><summary>VkQueueFamilyProperties[<div class='val'>%d</div>]</summary>\n", id);
        fprintf(out, "\t\t\t\t\t\t<details><summary>queueFlags = ");
    } else if (human_readable_output) {
        printf("VkQueueFamilyProperties[%d]:\n", id);
        printf("===========================\n");
        printf("\tqueueFlags         = ");
    }
    if (html_output || human_readable_output) {
        char *sep = "";  // separator character
        if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            fprintf(out, "GRAPHICS");
            sep = " | ";
        }
        if (props.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            fprintf(out, "%sCOMPUTE", sep);
            sep = " | ";
        }
        if (props.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            fprintf(out, "%sTRANSFER", sep);
            sep = " | ";
        }
        if (props.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
            fprintf(out, "%sSPARSE", sep);
        }
    }

    if (html_output) {
        fprintf(out, "</summary></details>\n");
        fprintf(out, "\t\t\t\t\t\t<details><summary>queueCount         = <div class='val'>%u</div></summary></details>\n", props.queueCount);
        fprintf(out, "\t\t\t\t\t\t<details><summary>timestampValidBits = <div class='val'>%u</div></summary></details>\n", props.timestampValidBits);
        fprintf(out, "\t\t\t\t\t\t<details><summary>minImageTransferGranularity = (<div class='val'>%d</div>, <div class='val'>%d</div>, <div class='val'>%d</div>)</summary></details>\n",
                props.minImageTransferGranularity.width, props.minImageTransferGranularity.height, props.minImageTransferGranularity.depth);
        fprintf(out, "\t\t\t\t\t</details>\n");
    } else if (human_readable_output) {
        printf("\n");
        printf("\tqueueCount         = %u\n", props.queueCount);
        printf("\ttimestampValidBits = %u\n", props.timestampValidBits);
        printf("\tminImageTransferGranularity = (%d, %d, %d)\n", props.minImageTransferGranularity.width,
               props.minImageTransferGranularity.height, props.minImageTransferGranularity.depth);
    }
    if (json_output) {
        printf("\t\t{\n");
        printf("\t\t\t\"minImageTransferGranularity\": {\n");
        printf("\t\t\t\t\"depth\": %u,\n", props.minImageTransferGranularity.depth);
        printf("\t\t\t\t\"height\": %u,\n", props.minImageTransferGranularity.height);
        printf("\t\t\t\t\"width\": %u\n", props.minImageTransferGranularity.width);
        printf("\t\t\t},\n");
        printf("\t\t\t\"queueCount\": %u,\n", props.queueCount);
        printf("\t\t\t\"queueFlags\": %u,\n", props.queueFlags);
        printf("\t\t\t\"timestampValidBits\": %u\n", props.timestampValidBits);
        printf("\t\t}");
    }

    fflush(out);
    fflush(stdout);
}

// This prints a number of bytes in a human-readable format according to prefixes of the International System of Quantities (ISQ),
// defined in ISO/IEC 80000. The prefixes used here are not SI prefixes, but rather the binary prefixes based on powers of 1024
// (kibi-, mebi-, gibi- etc.).
#define kBufferSize 32

static char *HumanReadable(const size_t sz) {
    const char prefixes[] = "KMGTPEZY";
    char buf[kBufferSize];
    int which = -1;
    double result = (double)sz;
    while (result > 1024 && which < 7) {
        result /= 1024;
        ++which;
    }

    char unit[] = "\0i";
    if (which >= 0) {
        unit[0] = prefixes[which];
    }
    snprintf(buf, kBufferSize, "%.2f %sB", result, unit);
    return strndup(buf, kBufferSize);
}

static void AppGpuDumpMemoryProps(const struct AppGpu *gpu, FILE *out) {
    VkPhysicalDeviceMemoryProperties props;

    if (CheckExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, gpu->inst->inst_extensions,
        gpu->inst->inst_extensions_count)) {
        const VkPhysicalDeviceMemoryProperties *props2_const = &gpu->memory_props2.memoryProperties;
        props = *props2_const;
    } else {
        const VkPhysicalDeviceMemoryProperties *props_const = &gpu->memory_props;
        props = *props_const;
    }

    if (html_output) {
        fprintf(out, "\t\t\t\t\t<details><summary>VkPhysicalDeviceMemoryProperties</summary>\n");
        fprintf(out, "\t\t\t\t\t\t<details><summary>memoryHeapCount = <div class='val'>%u</div></summary>", props.memoryHeapCount);
        if (props.memoryHeapCount > 0) {
            fprintf(out, "\n");
        }
    } else if (human_readable_output) {
        printf("VkPhysicalDeviceMemoryProperties:\n");
        printf("=================================\n");
        printf("\tmemoryHeapCount       = %u\n", props.memoryHeapCount);
    }
    if (json_output) {
        printf(",\n");
        printf("\t\"VkPhysicalDeviceMemoryProperties\": {\n");
        printf("\t\t\"memoryHeaps\": [");
    }
    for (uint32_t i = 0; i < props.memoryHeapCount; ++i) {
        const VkDeviceSize memSize = props.memoryHeaps[i].size;
        char *mem_size_human_readable = HumanReadable((const size_t)memSize);

        if (html_output) {
            fprintf(out, "\t\t\t\t\t\t\t<details><summary>memoryHeaps[<div class='val'>%u</div>]</summary>\n", i);
            fprintf(out, "\t\t\t\t\t\t\t\t<details><summary>size = <div class='val'>" PRINTF_SIZE_T_SPECIFIER "</div> (<div class='val'>0x%" PRIxLEAST64 "</div>) (<div class='val'>%s</div>)</summary></details>\n",
                    (size_t)memSize, memSize, mem_size_human_readable);
        } else if (human_readable_output) {
            printf("\tmemoryHeaps[%u] :\n", i);
            printf("\t\tsize          = " PRINTF_SIZE_T_SPECIFIER " (0x%" PRIxLEAST64 ") (%s)\n", (size_t)memSize, memSize,
                   mem_size_human_readable);
        }
        free(mem_size_human_readable);

        const VkMemoryHeapFlags heap_flags = props.memoryHeaps[i].flags;
        if (html_output) {
            fprintf(out, "\t\t\t\t\t\t\t\t<details open><summary>flags</summary>\n");
            fprintf(out, "\t\t\t\t\t\t\t\t\t<details><summary>");
            fprintf(out, (heap_flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) ? "<div class='type'>VK_MEMORY_HEAP_DEVICE_LOCAL_BIT</div>" : "None");
            fprintf(out, "</summary></details>\n");
            fprintf(out, "\t\t\t\t\t\t\t\t</details>\n");
            fprintf(out, "\t\t\t\t\t\t\t</details>\n");
        } else if (human_readable_output) {
            printf("\t\tflags:\n\t\t\t");
            printf((heap_flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) ? "VK_MEMORY_HEAP_DEVICE_LOCAL_BIT\n" : "None\n");
        }
        if (json_output) {
            if (i > 0) {
                printf(",");
            }
            printf("\n");
            printf("\t\t\t{\n");
            printf("\t\t\t\t\"flags\": %u,\n", heap_flags);
            printf("\t\t\t\t\"size\": " PRINTF_SIZE_T_SPECIFIER "\n", (size_t)memSize);
            printf("\t\t\t}");
        }
    }
    if (html_output) {
        if (props.memoryHeapCount > 0) {
            fprintf(out, "\t\t\t\t\t\t");
        }
        fprintf(out, "</details>\n");
    }
    if (json_output) {
        if (props.memoryHeapCount > 0) {
            printf("\n\t\t");
        }
        printf("]");
    }

    if (html_output) {
        fprintf(out, "\t\t\t\t\t\t<details><summary>memoryTypeCount = <div class='val'>%u</div></summary>", props.memoryTypeCount);
        if (props.memoryTypeCount > 0) {
            fprintf(out, "\n");
        }
    } else if (human_readable_output) {
        printf("\tmemoryTypeCount       = %u\n", props.memoryTypeCount);
    }
    if (json_output) {
        printf(",\n");
        printf("\t\t\"memoryTypes\": [");
    }
    for (uint32_t i = 0; i < props.memoryTypeCount; ++i) {
        if (html_output) {
            fprintf(out, "\t\t\t\t\t\t\t<details><summary>memoryTypes[<div class='val'>%u</div>]</summary>\n", i);
            fprintf(out, "\t\t\t\t\t\t\t\t<details><summary>heapIndex = <div class='val'>%u</div></summary></details>\n", props.memoryTypes[i].heapIndex);
            fprintf(out, "\t\t\t\t\t\t\t\t<details open><summary>propertyFlags = <div class='val'>0x%" PRIxLEAST32 "</div></summary>", props.memoryTypes[i].propertyFlags);
            if (props.memoryTypes[i].propertyFlags == 0) {
                fprintf(out, "</details>\n");
            } else {
                fprintf(out, "\n");
            }
        } else if (human_readable_output) {
            printf("\tmemoryTypes[%u] :\n", i);
            printf("\t\theapIndex     = %u\n", props.memoryTypes[i].heapIndex);
            printf("\t\tpropertyFlags = 0x%" PRIxLEAST32 ":\n", props.memoryTypes[i].propertyFlags);
        }
        if (json_output) {
            if (i > 0) {
                printf(",");
            }
            printf("\n");
            printf("\t\t\t{\n");
            printf("\t\t\t\t\"heapIndex\": %u,\n", props.memoryTypes[i].heapIndex);
            printf("\t\t\t\t\"propertyFlags\": %u\n", props.memoryTypes[i].propertyFlags);
            printf("\t\t\t}");
        }

        // Print each named flag to html or std output if it is set
        const VkFlags flags = props.memoryTypes[i].propertyFlags;
        if (html_output) {
            if (flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)     fprintf(out, "\t\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT</div></summary></details>\n");
            if (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)     fprintf(out, "\t\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT</div></summary></details>\n");
            if (flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)    fprintf(out, "\t\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_MEMORY_PROPERTY_HOST_COHERENT_BIT</div></summary></details>\n");
            if (flags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)      fprintf(out, "\t\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_MEMORY_PROPERTY_HOST_CACHED_BIT</div></summary></details>\n");
            if (flags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) fprintf(out, "\t\t\t\t\t\t\t\t\t<details><summary><div class='type'>VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT</div></summary></details>\n");
            if (props.memoryTypes[i].propertyFlags > 0)         fprintf(out, "\t\t\t\t\t\t\t\t</details>\n");
            fprintf(out, "\t\t\t\t\t\t\t</details>\n");
        } else if (human_readable_output) {
            if (flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)     printf("\t\t\tVK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT\n");
            if (flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)     printf("\t\t\tVK_MEMORY_PROPERTY_HOST_VISIBLE_BIT\n");
            if (flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)    printf("\t\t\tVK_MEMORY_PROPERTY_HOST_COHERENT_BIT\n");
            if (flags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)      printf("\t\t\tVK_MEMORY_PROPERTY_HOST_CACHED_BIT\n");
            if (flags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) printf("\t\t\tVK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT\n");
        }
    }
    if (html_output) {
        if (props.memoryTypeCount > 0) {
            fprintf(out, "\t\t\t\t\t\t");
        }
        fprintf(out, "</details>\n");
        fprintf(out, "\t\t\t\t\t</details>\n");
    }
    if (json_output) {
        if (props.memoryTypeCount > 0) {
            printf("\n\t\t");
        }
        printf("]\n");
        printf("\t}");
    }

    fflush(out);
    fflush(stdout);
}
// clang-format on

static void AppGpuDump(const struct AppGpu *gpu, FILE *out) {
    if (html_output) {
        fprintf(out, "\t\t\t<details><summary>Device Properties and Extensions</summary>\n");
        fprintf(out, "\t\t\t\t<details><summary>GPU%u</summary>\n", gpu->id);
    } else if (human_readable_output) {
        printf("\nDevice Properties and Extensions :\n");
        printf("==================================\n");
        printf("GPU%u\n", gpu->id);
    }

    AppGpuDumpProps(gpu, out);
    if (html_output) {
        AppDumpExtensions("\t\t", "Device", gpu->device_extension_count, gpu->device_extensions, out);
    } else if (human_readable_output) {
        printf("\n");
        AppDumpExtensions("", "Device", gpu->device_extension_count, gpu->device_extensions, out);
        printf("\n");
    }

    if (json_output) {
        printf(",\n");
        printf("\t\"ArrayOfVkQueueFamilyProperties\": [");
    }
    for (uint32_t i = 0; i < gpu->queue_count; ++i) {
        if (json_output) {
            if (i > 0) {
                printf(",");
            }
            printf("\n");
        }
        AppGpuDumpQueueProps(gpu, i, out);
        if (human_readable_output) {
            printf("\n");
        }
    }
    if (json_output) {
        if (gpu->queue_count > 0) {
            printf("\n\t");
        }
        printf("]");
    }

    AppGpuDumpMemoryProps(gpu, out);
    if (human_readable_output) {
        printf("\n");
    }

    AppGpuDumpFeatures(gpu, out);
    if (human_readable_output) {
        printf("\n");
    }

    AppDevDump(gpu, out);
    if (html_output) {
        fprintf(out, "\t\t\t\t</details>\n");
        fprintf(out, "\t\t\t</details>\n");
    }
}

static void AppGroupDump(const VkPhysicalDeviceGroupProperties *group, const uint32_t id, const struct AppInstance *inst,
                         FILE *out) {
    if (html_output) {
        fprintf(out, "\t\t\t\t<details><summary>Device Group Properties (Group %u)</summary>\n", id);
        fprintf(out, "\t\t\t\t\t<details><summary>physicalDeviceCount = <div class='val'>%u</div></summary>\n",
                group->physicalDeviceCount);
    } else if (human_readable_output) {
        printf("\tDevice Group Properties (Group %u) :\n", id);
        printf("\t\tphysicalDeviceCount = %u\n", group->physicalDeviceCount);
    }

    // Keep a record of all physical device properties to give the user clearer information as output.
    VkPhysicalDeviceProperties *props = malloc(sizeof(props[0]) * group->physicalDeviceCount);
    if (!props) {
        ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
    }
    for (uint32_t i = 0; i < group->physicalDeviceCount; ++i) {
        vkGetPhysicalDeviceProperties(group->physicalDevices[i], &props[i]);
    }

    // Output information to the user.
    for (uint32_t i = 0; i < group->physicalDeviceCount; ++i) {
        if (html_output) {
            fprintf(out, "\t\t\t\t\t\t<details><summary>%s (ID: <div class='val'>%d</div>)</summary></details>\n",
                    props[i].deviceName, i);
        } else if (human_readable_output) {
            printf("\n\t\t\t%s (ID: %d)\n", props[i].deviceName, i);
        }
    }

    if (html_output) {
        fprintf(out, "\t\t\t\t\t</details>\n");
    }

    if (html_output) {
        fprintf(out, "\t\t\t\t\t<details><summary>subsetAllocation = <div class='val'>%u</div></summary></details>\n",
                group->subsetAllocation);
    } else if (human_readable_output) {
        printf("\n\t\tsubsetAllocation = %u\n", group->subsetAllocation);
    }

    if (html_output) {
        fprintf(out, "\t\t\t\t</details>\n");
    }

    // Build create info for logical device made from all physical devices in this group.
    const char *extensions_list[] = {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                     VK_KHR_DEVICE_GROUP_EXTENSION_NAME};

    VkDeviceGroupDeviceCreateInfoKHR dg_ci = {.sType = VK_STRUCTURE_TYPE_DEVICE_GROUP_DEVICE_CREATE_INFO_KHR,
                                              .pNext = NULL,
                                              .physicalDeviceCount = group->physicalDeviceCount,
                                              .pPhysicalDevices = group->physicalDevices};

    VkDeviceQueueCreateInfo q_ci = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, .pNext = NULL, .queueFamilyIndex = 0, .queueCount = 1};

    VkDeviceCreateInfo device_ci = {.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                    .pNext = &dg_ci,
                                    .queueCreateInfoCount = 1,
                                    .pQueueCreateInfos = &q_ci,
                                    .enabledExtensionCount = ARRAY_SIZE(extensions_list),
                                    .ppEnabledExtensionNames = extensions_list};

    VkDevice logical_device = VK_NULL_HANDLE;

    VkResult err = vkCreateDevice(group->physicalDevices[0], &device_ci, NULL, &logical_device);

    if (!err) {
        if (html_output) {
            fprintf(out, "\t\t\t\t<details><summary>Device Group Present Capabilities (Group %d)</summary>\n", id);
        } else if (human_readable_output) {
            printf("\n\tDevice Group Present Capabilities (Group %d) :\n", id);
        }

        VkDeviceGroupPresentCapabilitiesKHR group_capabilities = {.sType = VK_STRUCTURE_TYPE_DEVICE_GROUP_PRESENT_CAPABILITIES_KHR,
                                                                  .pNext = NULL};

        // If the KHR_device_group extension is present, write the capabilities of the logical device into a struct for later output
        // to user.
        PFN_vkGetDeviceGroupPresentCapabilitiesKHR vkGetDeviceGroupPresentCapabilitiesKHR =
            (PFN_vkGetDeviceGroupPresentCapabilitiesKHR)vkGetInstanceProcAddr(inst->instance,
                                                                              "vkGetDeviceGroupPresentCapabilitiesKHR");
        vkGetDeviceGroupPresentCapabilitiesKHR(logical_device, &group_capabilities);

        for (uint32_t i = 0; i < group->physicalDeviceCount; i++) {
            if (html_output) {
                fprintf(out, "\t\t\t\t\t<details><summary>%s (ID: <div class='val'>%d</div>)</summary></details>\n",
                        props[i].deviceName, i);
                fprintf(out, "\t\t\t\t\t<details><summary>Can present images from the following devices:</summary>\n");
                if (group_capabilities.presentMask[i] != 0) {
                    for (uint32_t j = 0; j < group->physicalDeviceCount; ++j) {
                        uint32_t mask = 1 << j;
                        if (group_capabilities.presentMask[i] & mask) {
                            fprintf(out, "\t\t\t\t\t\t<details><summary>%s (ID: <div class='val'>%d</div>)</summary></details>\n",
                                    props[j].deviceName, j);
                        }
                    }
                } else {
                    fprintf(out, "\t\t\t\t\t\t<details><summary>None</summary></details>\n");
                }
                fprintf(out, "\t\t\t\t\t</details>\n");
            } else if (human_readable_output) {
                printf("\n\t\t%s (ID: %d)\n", props[i].deviceName, i);
                printf("\t\tCan present images from the following devices:\n");
                if (group_capabilities.presentMask[i] != 0) {
                    for (uint32_t j = 0; j < group->physicalDeviceCount; ++j) {
                        uint32_t mask = 1 << j;
                        if (group_capabilities.presentMask[i] & mask) {
                            printf("\t\t\t%s (ID: %d)\n", props[j].deviceName, j);
                        }
                    }
                } else {
                    printf("\t\t\tNone\n");
                }
                printf("\n");
            }
        }

        if (html_output) {
            fprintf(out, "\t\t\t\t\t<details><summary>Present modes</summary>\n");
            if (group_capabilities.modes & VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR)
                fprintf(out, "\t\t\t\t\t\t<details><summary>VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR</summary></details>\n");
            if (group_capabilities.modes & VK_DEVICE_GROUP_PRESENT_MODE_REMOTE_BIT_KHR)
                fprintf(out, "\t\t\t\t\t\t<details><summary>VK_DEVICE_GROUP_PRESENT_MODE_REMOTE_BIT_KHR</summary></details>\n");
            if (group_capabilities.modes & VK_DEVICE_GROUP_PRESENT_MODE_SUM_BIT_KHR)
                fprintf(out, "\t\t\t\t\t\t<details><summary>VK_DEVICE_GROUP_PRESENT_MODE_SUM_BIT_KHR</summary></details>\n");
            if (group_capabilities.modes & VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_MULTI_DEVICE_BIT_KHR)
                fprintf(
                    out,
                    "\t\t\t\t\t\t<details><summary>VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_MULTI_DEVICE_BIT_KHR</summary></details>\n");
            fprintf(out, "\t\t\t\t\t</details>\n");
        } else if (human_readable_output) {
            printf("\t\tPresent modes:\n");
            if (group_capabilities.modes & VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR)
                printf("\t\t\tVK_DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR\n");
            if (group_capabilities.modes & VK_DEVICE_GROUP_PRESENT_MODE_REMOTE_BIT_KHR)
                printf("\t\t\tVK_DEVICE_GROUP_PRESENT_MODE_REMOTE_BIT_KHR\n");
            if (group_capabilities.modes & VK_DEVICE_GROUP_PRESENT_MODE_SUM_BIT_KHR)
                printf("\t\t\tVK_DEVICE_GROUP_PRESENT_MODE_SUM_BIT_KHR\n");
            if (group_capabilities.modes & VK_DEVICE_GROUP_PRESENT_MODE_LOCAL_MULTI_DEVICE_BIT_KHR)
                printf("\t\t\tVK_DEVICE_GROUP_PRESENT_MODE_LOCAL_MULTI_DEVICE_BIT_KHR\n");
        }

        if (html_output) {
            fprintf(out, "\t\t\t\t</details>\n");
        }
    }

    // Clean up after ourselves.
    free(props);
    vkDestroyDevice(logical_device, NULL);
}

#ifdef _WIN32
// Enlarges the console window to have a large scrollback size.
static void ConsoleEnlarge() {
    const HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    // make the console window bigger
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD buffer_size;
    if (GetConsoleScreenBufferInfo(console_handle, &csbi)) {
        buffer_size.X = csbi.dwSize.X + 30;
        buffer_size.Y = 20000;
        SetConsoleScreenBufferSize(console_handle, buffer_size);
    }

    SMALL_RECT r;
    r.Left = r.Top = 0;
    r.Right = csbi.dwSize.X - 1 + 30;
    r.Bottom = 50;
    SetConsoleWindowInfo(console_handle, true, &r);

    // change the console window title
    SetConsoleTitle(TEXT(APP_SHORT_NAME));
}
#endif

int main(int argc, char **argv) {
    uint32_t gpu_count;
    VkResult err;
    struct AppInstance inst;
    FILE *out = stdout;

#ifdef _WIN32
    if (ConsoleIsExclusive()) ConsoleEnlarge();
#endif
    PFN_vkEnumerateInstanceVersion enumerate_instance_version =
        (PFN_vkEnumerateInstanceVersion)vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceVersion");

    uint32_t instance_version = VK_API_VERSION_1_0;

    if (enumerate_instance_version != NULL) {
        enumerate_instance_version(&instance_version);
    }

    const uint32_t vulkan_major = VK_VERSION_MAJOR(instance_version);
    const uint32_t vulkan_minor = VK_VERSION_MINOR(instance_version);
    const uint32_t vulkan_patch = VK_VERSION_PATCH(VK_HEADER_VERSION);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--html") == 0) {
            out = fopen("vulkaninfo.html", "w");
            human_readable_output = false;
            html_output = true;
            continue;
        }
        CheckForJsonOption(argv[i]);
    }

    if (html_output) {
        PrintHtmlHeader(out);
        fprintf(out, "\t\t\t<details><summary>");
    } else if (human_readable_output) {
        printf("===========\n");
        printf("VULKAN INFO\n");
        printf("===========\n\n");
    }
    if (html_output || human_readable_output) {
        fprintf(out, "Vulkan Instance Version: ");
    }
    if (html_output) {
        fprintf(out, "<div class='val'>%d.%d.%d</div></summary></details>\n", vulkan_major, vulkan_minor, vulkan_patch);
        fprintf(out, "\t\t\t<br />\n");
    } else if (human_readable_output) {
        printf("%d.%d.%d\n\n", vulkan_major, vulkan_minor, vulkan_patch);
    }

    AppCreateInstance(&inst);

    err = vkEnumeratePhysicalDevices(inst.instance, &gpu_count, NULL);
    if (err) {
        ERR_EXIT(err);
    }

    VkPhysicalDevice *objs = malloc(sizeof(objs[0]) * gpu_count);
    if (!objs) {
        ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
    }

    err = vkEnumeratePhysicalDevices(inst.instance, &gpu_count, objs);
    if (err) {
        ERR_EXIT(err);
    }

    struct AppGpu *gpus = malloc(sizeof(gpus[0]) * gpu_count);
    if (!gpus) {
        ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
    }

    for (uint32_t i = 0; i < gpu_count; ++i) {
        AppGpuInit(&gpus[i], &inst, i, objs[i]);
        if (human_readable_output) {
            printf("\n\n");
        }
    }

    // If json output, confirm the desired gpu exists
    if (json_output) {
        if (selected_gpu >= gpu_count) {
            selected_gpu = 0;
        }
        PrintJsonHeader(vulkan_major, vulkan_minor, vulkan_patch);
    }

    if (human_readable_output) {
        printf("Instance Extensions:\n");
        printf("====================\n");
    }
    AppDumpExtensions("", "Instance", inst.global_extension_count, inst.global_extensions, out);

    //---Layer-Device-Extensions---
    if (html_output) {
        fprintf(out, "\t\t\t<details><summary>Layers: count = <div class='val'>%d</div></summary>", inst.global_layer_count);
        if (inst.global_layer_count > 0) {
            fprintf(out, "\n");
        }
    } else if (human_readable_output) {
        printf("Layers: count = %d\n", inst.global_layer_count);
        printf("=======\n");
    }
    if (json_output && (inst.global_layer_count > 0)) {
        printf(",\n");
        printf("\t\"ArrayOfVkLayerProperties\": [");
    }
    for (uint32_t i = 0; i < inst.global_layer_count; ++i) {
        uint32_t layer_major, layer_minor, layer_patch;
        char spec_version[64], layer_version[64];
        VkLayerProperties const *layer_prop = &inst.global_layers[i].layer_properties;

        ExtractVersion(layer_prop->specVersion, &layer_major, &layer_minor, &layer_patch);
        snprintf(spec_version, sizeof(spec_version), "%d.%d.%d", layer_major, layer_minor, layer_patch);
        snprintf(layer_version, sizeof(layer_version), "%d", layer_prop->implementationVersion);

        if (html_output) {
            fprintf(out, "\t\t\t\t<details><summary>");
            fprintf(out, "<div class='type'>%s</div> (%s) Vulkan version <div class='val'>%s</div>, ", layer_prop->layerName,
                    (char *)layer_prop->description, spec_version);
            fprintf(out, "layer version <div class='val'>%s</div></summary>\n", layer_version);
            AppDumpExtensions("\t\t", "Layer", inst.global_layers[i].extension_count, inst.global_layers[i].extension_properties,
                              out);
        } else if (human_readable_output) {
            printf("%s (%s) Vulkan version %s, layer version %s\n", layer_prop->layerName, (char *)layer_prop->description,
                   spec_version, layer_version);
            AppDumpExtensions("\t", "Layer", inst.global_layers[i].extension_count, inst.global_layers[i].extension_properties,
                              out);
        }
        if (json_output) {
            if (i > 0) {
                printf(",");
            }
            printf("\n");
            printf("\t\t{\n");
            printf("\t\t\t\"layerName\": \"%s\",\n", layer_prop->layerName);
            printf("\t\t\t\"specVersion\": %u,\n", layer_prop->specVersion);
            printf("\t\t\t\"implementationVersion\": %u,\n", layer_prop->implementationVersion);
            printf("\t\t\t\"description\": \"%s\"\n", layer_prop->description);
            printf("\t\t}");
        }

        if (html_output) {
            fprintf(out, "\t\t\t\t\t<details><summary>Devices count = <div class='val'>%d</div></summary>\n", gpu_count);
        } else if (human_readable_output) {
            printf("\tDevices \tcount = %d\n", gpu_count);
        }

        char *layer_name = inst.global_layers[i].layer_properties.layerName;

        for (uint32_t j = 0; j < gpu_count; ++j) {
            if (html_output) {
                fprintf(out, "\t\t\t\t\t\t<details><summary>");
                fprintf(out, "GPU id: <div class='val'>%u</div> (%s)</summary></details>\n", j, gpus[j].props.deviceName);
            } else if (human_readable_output) {
                printf("\t\tGPU id       : %u (%s)\n", j, gpus[j].props.deviceName);
            }
            uint32_t count = 0;
            VkExtensionProperties *props;
            AppGetPhysicalDeviceLayerExtensions(&gpus[j], layer_name, &count, &props);
            if (html_output) {
                AppDumpExtensions("\t\t\t", "Layer-Device", count, props, out);
                fprintf(out, "\t\t\t\t\t</details>\n");
            } else if (human_readable_output) {
                AppDumpExtensions("\t\t", "Layer-Device", count, props, out);
            }
            free(props);
        }

        if (html_output) {
            fprintf(out, "\t\t\t\t</details>\n");
        } else if (human_readable_output) {
            printf("\n");
        }
    }

    if (html_output) {
        fprintf(out, "\t\t\t</details>\n");
    }
    if (json_output && (inst.global_layer_count > 0)) {
        printf("\n\t]");
    }

    fflush(out);
    fflush(stdout);
    //-----------------------------

    if (html_output) {
        fprintf(out, "\t\t\t<details><summary>Presentable Surfaces</summary>");
        if (gpu_count > 0) {
            fprintf(out, "\n");
        } else {
            fprintf(out, "</details>\n");
        }
    } else if (human_readable_output) {
        printf("Presentable Surfaces:\n");
        printf("=====================\n");
    }
    inst.width = 256;
    inst.height = 256;
    int format_count = 0;
    int present_mode_count = 0;

#if defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR)
    bool has_display = true;
    const char *display_var = getenv("DISPLAY");
    if (display_var == NULL || strlen(display_var) == 0) {
        fprintf(stderr, "'DISPLAY' environment variable not set... skipping surface info\n");
        fflush(stderr);
        has_display = false;
    }
#endif

//--WIN32--
#ifdef VK_USE_PLATFORM_WIN32_KHR
    if (CheckExtensionEnabled(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, inst.inst_extensions, inst.inst_extensions_count)) {
        AppCreateWin32Window(&inst);
        for (uint32_t i = 0; i < gpu_count; ++i) {
            AppCreateWin32Surface(&inst);
            if (html_output) {
                fprintf(out, "\t\t\t\t<details><summary>GPU id : <div class='val'>%u</div> (%s)</summary></details>\n", i,
                        gpus[i].props.deviceName);
                fprintf(out, "\t\t\t\t<details><summary>Surface type : <div class='type'>%s</div></summary></details>\n",
                        VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
            } else if (human_readable_output) {
                printf("GPU id       : %u (%s)\n", i, gpus[i].props.deviceName);
                printf("Surface type : %s\n", VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
            }
            format_count += AppDumpSurfaceFormats(&inst, &gpus[i], out);
            present_mode_count += AppDumpSurfacePresentModes(&inst, &gpus[i], out);
            AppDumpSurfaceCapabilities(&inst, &gpus[i], out);
            AppDestroySurface(&inst);
        }
        AppDestroyWin32Window(&inst);
    }
//--XCB--
#elif VK_USE_PLATFORM_XCB_KHR
    if (has_display && CheckExtensionEnabled(VK_KHR_XCB_SURFACE_EXTENSION_NAME, inst.inst_extensions, inst.inst_extensions_count)) {
        AppCreateXcbWindow(&inst);
        for (uint32_t i = 0; i < gpu_count; ++i) {
            AppCreateXcbSurface(&inst);
            if (html_output) {
                fprintf(out, "\t\t\t\t<details><summary>GPU id : <div class='val'>%u</div> (%s)</summary></details>\n", i,
                        gpus[i].props.deviceName);
                fprintf(out, "\t\t\t\t<details><summary>Surface type : <div class='type'>%s</div></summary></details>\n",
                        VK_KHR_XCB_SURFACE_EXTENSION_NAME);
            } else if (human_readable_output) {
                printf("GPU id       : %u (%s)\n", i, gpus[i].props.deviceName);
                printf("Surface type : %s\n", VK_KHR_XCB_SURFACE_EXTENSION_NAME);
            }
            format_count += AppDumpSurfaceFormats(&inst, &gpus[i], out);
            present_mode_count += AppDumpSurfacePresentModes(&inst, &gpus[i], out);
            AppDumpSurfaceCapabilities(&inst, &gpus[i], out);
            AppDestroySurface(&inst);
        }
        AppDestroyXcbWindow(&inst);
    }
//--XLIB--
#elif VK_USE_PLATFORM_XLIB_KHR
    if (has_display &&
        CheckExtensionEnabled(VK_KHR_XLIB_SURFACE_EXTENSION_NAME, inst.inst_extensions, inst.inst_extensions_count)) {
        AppCreateXlibWindow(&inst);
        for (uint32_t i = 0; i < gpu_count; ++i) {
            AppCreateXlibSurface(&inst);
            if (html_output) {
                fprintf(out, "\t\t\t\t<details><summary>GPU id : <div class='val'>%u</div> (%s)</summary></details>\n", i,
                        gpus[i].props.deviceName);
                fprintf(out, "\t\t\t\t<details><summary>Surface type : <div class='type'>%s</div></summary></details>\n",
                        VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
            } else if (human_readable_output) {
                printf("GPU id       : %u (%s)\n", i, gpus[i].props.deviceName);
                printf("Surface type : %s\n", VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
            }
            format_count += AppDumpSurfaceFormats(&inst, &gpus[i], out);
            present_mode_count += AppDumpSurfacePresentModes(&inst, &gpus[i], out);
            AppDumpSurfaceCapabilities(&inst, &gpus[i], out);
            AppDestroySurface(&inst);
        }
        AppDestroyXlibWindow(&inst);
    }
#endif

    // TODO: Android / Wayland / MIR
    if (!format_count && !present_mode_count) {
        if (html_output) {
            fprintf(out, "\t\t\t\t<details><summary>None found</summary></details>\n");
        } else if (human_readable_output) {
            printf("None found\n");
        }
    }

    if (html_output) {
        fprintf(out, "\t\t\t</details>\n");
    } else if (human_readable_output) {
        printf("\n");
    }
    //---------

    if (CheckExtensionEnabled(VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME, inst.inst_extensions, inst.inst_extensions_count)) {
        PFN_vkEnumeratePhysicalDeviceGroupsKHR vkEnumeratePhysicalDeviceGroupsKHR =
            (PFN_vkEnumeratePhysicalDeviceGroupsKHR)vkGetInstanceProcAddr(inst.instance, "vkEnumeratePhysicalDeviceGroupsKHR");

        uint32_t group_count;
        err = vkEnumeratePhysicalDeviceGroupsKHR(inst.instance, &group_count, NULL);
        if (err) {
            ERR_EXIT(err);
        }

        VkPhysicalDeviceGroupProperties *groups = malloc(sizeof(groups[0]) * group_count);
        if (!groups) {
            ERR_EXIT(VK_ERROR_OUT_OF_HOST_MEMORY);
        }

        err = vkEnumeratePhysicalDeviceGroupsKHR(inst.instance, &group_count, groups);
        if (err) {
            ERR_EXIT(err);
        }

        if (html_output) {
            fprintf(out, "\t\t\t<details><summary>Groups</summary>\n");
        } else if (human_readable_output) {
            printf("\nGroups :\n");
            printf("========\n");
        }

        for (uint32_t i = 0; i < group_count; ++i) {
            AppGroupDump(&groups[i], i, &inst, out);
            if (human_readable_output) {
                printf("\n\n");
            }
        }

        if (html_output) {
            fprintf(out, "\t\t\t</details>\n");
        }

        free(groups);
    }

    for (uint32_t i = 0; i < gpu_count; ++i) {
        if (json_output && selected_gpu != i) {
            // Toggle json_output to allow html output without json output
            json_output = false;
            AppGpuDump(&gpus[i], out);
            json_output = true;
        } else {
            AppGpuDump(&gpus[i], out);
        }
        if (human_readable_output) {
            printf("\n\n");
        }
    }

    for (uint32_t i = 0; i < gpu_count; ++i) {
        AppGpuDestroy(&gpus[i]);
    }
    free(gpus);
    free(objs);

    AppDestroyInstance(&inst);

    if (html_output) {
        PrintHtmlFooter(out);
        fflush(out);
        fclose(out);
    }
    if (json_output) {
        printf("\n}\n");
    }

    fflush(stdout);

#ifdef _WIN32
    if (ConsoleIsExclusive() && human_readable_output) {
        Sleep(INFINITE);
    }
#endif

    return 0;
}
