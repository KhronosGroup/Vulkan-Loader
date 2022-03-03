// *** THIS FILE IS GENERATED - DO NOT EDIT ***
// See loader_test_generator.py for modifications

/*
 * Copyright (c) 2022 The Khronos Group Inc.
 * Copyright (c) 2022 Valve Corporation
 * Copyright (c) 2022 LunarG, Inc.
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
 * Author: Mark Young <marky@lunarg.com>
 */

// clang-format off

#include "test_environment.h"
#include "loader/generated/vk_dispatch_table_helper.h"

// Test for VK_KHR_surface
TEST(BasicEntrypointTest, KHR_surface) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_SURFACE_EXTENSION_NAME}, {VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    VkHeadlessSurfaceCreateInfoEXT var_vkheadlesssurfacecreateinfoext{VK_STRUCTURE_TYPE_HEADLESS_SURFACE_CREATE_INFO_EXT};
    VkSurfaceKHR var_vksurfacekhr{};
    inst_disp_table.CreateHeadlessSurfaceEXT(instance, &var_vkheadlesssurfacecreateinfoext, nullptr, &var_vksurfacekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateHeadlessSurfaceEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateHeadlessSurfaceEXT"));
    log.logger.clear();

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkBool32 var_vkbool32{};
    inst_disp_table.GetPhysicalDeviceSurfaceSupportKHR(var_vkphysicaldevice, var_uint32_t, var_vksurfacekhr, &var_vkbool32);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceSurfaceSupportKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceSurfaceSupportKHR"));
    log.logger.clear();

    VkSurfaceCapabilitiesKHR var_vksurfacecapabilitieskhr{};
    inst_disp_table.GetPhysicalDeviceSurfaceCapabilitiesKHR(var_vkphysicaldevice, var_vksurfacekhr, &var_vksurfacecapabilitieskhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
    log.logger.clear();

    VkSurfaceFormatKHR var_vksurfaceformatkhr{};
    inst_disp_table.GetPhysicalDeviceSurfaceFormatsKHR(var_vkphysicaldevice, var_vksurfacekhr, &var_uint32_t, &var_vksurfaceformatkhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceSurfaceFormatsKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceSurfaceFormatsKHR"));
    log.logger.clear();

    VkPresentModeKHR var_vkpresentmodekhr{};
    inst_disp_table.GetPhysicalDeviceSurfacePresentModesKHR(var_vkphysicaldevice, var_vksurfacekhr, &var_uint32_t, &var_vkpresentmodekhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceSurfacePresentModesKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceSurfacePresentModesKHR"));
    log.logger.clear();

    inst_disp_table.DestroySurfaceKHR(instance, var_vksurfacekhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySurfaceKHR"));
    log.logger.clear();

} // Test for VK_KHR_surface

// Test for VK_KHR_swapchain
TEST(BasicEntrypointTest, KHR_swapchain) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_SURFACE_EXTENSION_NAME}, {VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    VkHeadlessSurfaceCreateInfoEXT var_vkheadlesssurfacecreateinfoext{VK_STRUCTURE_TYPE_HEADLESS_SURFACE_CREATE_INFO_EXT};
    VkSurfaceKHR var_vksurfacekhr{};
    inst_disp_table.CreateHeadlessSurfaceEXT(instance, &var_vkheadlesssurfacecreateinfoext, nullptr, &var_vksurfacekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateHeadlessSurfaceEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateHeadlessSurfaceEXT"));
    log.logger.clear();

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkRect2D var_vkrect2d{};
    inst_disp_table.GetPhysicalDevicePresentRectanglesKHR(var_vkphysicaldevice, var_vksurfacekhr, &var_uint32_t, &var_vkrect2d);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDevicePresentRectanglesKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDevicePresentRectanglesKHR"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_KHR_SWAPCHAIN_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkSwapchainKHR var_vkswapchainkhr{};
    uint64_t var_uint64_t = 1;
    VkSemaphore var_vksemaphore{};
    VkFence var_vkfence{};
    device_disp_table.AcquireNextImageKHR(dev.dev, var_vkswapchainkhr, var_uint64_t, var_vksemaphore, var_vkfence, &var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkAcquireNextImageKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkAcquireNextImageKHR"));
    log.logger.clear();

    VkAcquireNextImageInfoKHR var_vkacquirenextimageinfokhr{VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR};
    device_disp_table.AcquireNextImage2KHR(dev.dev, &var_vkacquirenextimageinfokhr, &var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkAcquireNextImage2KHR"));
    ASSERT_TRUE(log.find("Generated Driver vkAcquireNextImage2KHR"));
    log.logger.clear();

    VkDeviceQueueInfo2 var_vkdevicequeueinfo2{VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2};
    VkQueue var_vkqueue{};
    device_disp_table.GetDeviceQueue2(dev.dev, &var_vkdevicequeueinfo2, &var_vkqueue);
    ASSERT_TRUE(log.find("Generated Layer vkGetDeviceQueue2"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDeviceQueue2"));
    log.logger.clear();

    VkPresentInfoKHR var_vkpresentinfokhr{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    device_disp_table.QueuePresentKHR(var_vkqueue, &var_vkpresentinfokhr);
    ASSERT_TRUE(log.find("Generated Layer vkQueuePresentKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkQueuePresentKHR"));
    log.logger.clear();

    VkFenceCreateInfo var_vkfencecreateinfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    device_disp_table.CreateFence(dev.dev, &var_vkfencecreateinfo, nullptr, &var_vkfence);
    ASSERT_TRUE(log.find("Generated Layer vkCreateFence"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateFence"));
    log.logger.clear();

    device_disp_table.DestroyFence(dev.dev, var_vkfence, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyFence"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyFence"));
    log.logger.clear();

    VkSemaphoreCreateInfo var_vksemaphorecreateinfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    device_disp_table.CreateSemaphore(dev.dev, &var_vksemaphorecreateinfo, nullptr, &var_vksemaphore);
    ASSERT_TRUE(log.find("Generated Layer vkCreateSemaphore"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateSemaphore"));
    log.logger.clear();

    device_disp_table.DestroySemaphore(dev.dev, var_vksemaphore, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySemaphore"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySemaphore"));
    log.logger.clear();

    VkSwapchainCreateInfoKHR var_vkswapchaincreateinfokhr{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    var_vkswapchaincreateinfokhr.surface = var_vksurfacekhr;
    device_disp_table.CreateSwapchainKHR(dev.dev, &var_vkswapchaincreateinfokhr, nullptr, &var_vkswapchainkhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateSwapchainKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateSwapchainKHR"));
    log.logger.clear();

    device_disp_table.DestroySwapchainKHR(dev.dev, var_vkswapchainkhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySwapchainKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySwapchainKHR"));
    log.logger.clear();

    inst_disp_table.DestroySurfaceKHR(instance, var_vksurfacekhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySurfaceKHR"));
    log.logger.clear();

} // Test for VK_KHR_swapchain

// Test for VK_KHR_display
TEST(BasicEntrypointTest, KHR_display) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_DISPLAY_EXTENSION_NAME}, {VK_KHR_SURFACE_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    VkDisplaySurfaceCreateInfoKHR var_vkdisplaysurfacecreateinfokhr{VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR};
    VkSurfaceKHR var_vksurfacekhr{};
    inst_disp_table.CreateDisplayPlaneSurfaceKHR(instance, &var_vkdisplaysurfacecreateinfokhr, nullptr, &var_vksurfacekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateDisplayPlaneSurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateDisplayPlaneSurfaceKHR"));
    log.logger.clear();

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkDisplayPropertiesKHR var_vkdisplaypropertieskhr{};
    inst_disp_table.GetPhysicalDeviceDisplayPropertiesKHR(var_vkphysicaldevice, &var_uint32_t, &var_vkdisplaypropertieskhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceDisplayPropertiesKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceDisplayPropertiesKHR"));
    log.logger.clear();

    VkDisplayPlanePropertiesKHR var_vkdisplayplanepropertieskhr{};
    inst_disp_table.GetPhysicalDeviceDisplayPlanePropertiesKHR(var_vkphysicaldevice, &var_uint32_t, &var_vkdisplayplanepropertieskhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceDisplayPlanePropertiesKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceDisplayPlanePropertiesKHR"));
    log.logger.clear();

    VkDisplayKHR var_vkdisplaykhr{};
    VkDisplayModePropertiesKHR var_vkdisplaymodepropertieskhr{};
    inst_disp_table.GetDisplayModePropertiesKHR(var_vkphysicaldevice, var_vkdisplaykhr, &var_uint32_t, &var_vkdisplaymodepropertieskhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetDisplayModePropertiesKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDisplayModePropertiesKHR"));
    log.logger.clear();

    VkDisplayModeKHR var_vkdisplaymodekhr{};
    VkDisplayPlaneCapabilitiesKHR var_vkdisplayplanecapabilitieskhr{};
    inst_disp_table.GetDisplayPlaneCapabilitiesKHR(var_vkphysicaldevice, var_vkdisplaymodekhr, var_uint32_t, &var_vkdisplayplanecapabilitieskhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetDisplayPlaneCapabilitiesKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDisplayPlaneCapabilitiesKHR"));
    log.logger.clear();

    inst_disp_table.GetDisplayPlaneSupportedDisplaysKHR(var_vkphysicaldevice, var_uint32_t, &var_uint32_t, &var_vkdisplaykhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetDisplayPlaneSupportedDisplaysKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDisplayPlaneSupportedDisplaysKHR"));
    log.logger.clear();

    VkDisplayModeCreateInfoKHR var_vkdisplaymodecreateinfokhr{VK_STRUCTURE_TYPE_DISPLAY_MODE_CREATE_INFO_KHR};
    inst_disp_table.CreateDisplayModeKHR(var_vkphysicaldevice, var_vkdisplaykhr, &var_vkdisplaymodecreateinfokhr, nullptr, &var_vkdisplaymodekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateDisplayModeKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateDisplayModeKHR"));
    log.logger.clear();

    inst_disp_table.DestroySurfaceKHR(instance, var_vksurfacekhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySurfaceKHR"));
    log.logger.clear();

} // Test for VK_KHR_display

// Test for VK_KHR_display_swapchain
TEST(BasicEntrypointTest, KHR_display_swapchain) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_DISPLAY_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_KHR_DISPLAY_SWAPCHAIN_EXTENSION_NAME}, {VK_KHR_SWAPCHAIN_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

} // Test for VK_KHR_display_swapchain

#ifdef VK_USE_PLATFORM_XLIB_KHR
// Test for VK_KHR_xlib_surface
TEST(BasicEntrypointTest, KHR_xlib_surface) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_XLIB_SURFACE_EXTENSION_NAME}, {VK_KHR_SURFACE_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    VkXlibSurfaceCreateInfoKHR var_vkxlibsurfacecreateinfokhr{VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR};
    VkSurfaceKHR var_vksurfacekhr{};
    inst_disp_table.CreateXlibSurfaceKHR(instance, &var_vkxlibsurfacecreateinfokhr, nullptr, &var_vksurfacekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateXlibSurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateXlibSurfaceKHR"));
    log.logger.clear();

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    uint64_t big_chunk_of_mem[512][512];

    VisualID var_visualid{};
    inst_disp_table.GetPhysicalDeviceXlibPresentationSupportKHR(var_vkphysicaldevice, var_uint32_t, reinterpret_cast<Display*>(&big_chunk_of_mem), var_visualid);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceXlibPresentationSupportKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceXlibPresentationSupportKHR"));
    log.logger.clear();

    inst_disp_table.DestroySurfaceKHR(instance, var_vksurfacekhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySurfaceKHR"));
    log.logger.clear();

} // Test for VK_KHR_xlib_surface
#endif // VK_USE_PLATFORM_XLIB_KHR

#ifdef VK_USE_PLATFORM_XCB_KHR
// Test for VK_KHR_xcb_surface
TEST(BasicEntrypointTest, KHR_xcb_surface) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_XCB_SURFACE_EXTENSION_NAME}, {VK_KHR_SURFACE_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    VkXcbSurfaceCreateInfoKHR var_vkxcbsurfacecreateinfokhr{VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR};
    VkSurfaceKHR var_vksurfacekhr{};
    inst_disp_table.CreateXcbSurfaceKHR(instance, &var_vkxcbsurfacecreateinfokhr, nullptr, &var_vksurfacekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateXcbSurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateXcbSurfaceKHR"));
    log.logger.clear();

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    uint64_t big_chunk_of_mem[512][512];

    xcb_visualid_t var_xcb_visualid_t{};
    inst_disp_table.GetPhysicalDeviceXcbPresentationSupportKHR(var_vkphysicaldevice, var_uint32_t, reinterpret_cast<xcb_connection_t*>(&big_chunk_of_mem), var_xcb_visualid_t);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceXcbPresentationSupportKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceXcbPresentationSupportKHR"));
    log.logger.clear();

    inst_disp_table.DestroySurfaceKHR(instance, var_vksurfacekhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySurfaceKHR"));
    log.logger.clear();

} // Test for VK_KHR_xcb_surface
#endif // VK_USE_PLATFORM_XCB_KHR

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
// Test for VK_KHR_wayland_surface
TEST(BasicEntrypointTest, KHR_wayland_surface) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME}, {VK_KHR_SURFACE_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    VkWaylandSurfaceCreateInfoKHR var_vkwaylandsurfacecreateinfokhr{VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR};
    VkSurfaceKHR var_vksurfacekhr{};
    inst_disp_table.CreateWaylandSurfaceKHR(instance, &var_vkwaylandsurfacecreateinfokhr, nullptr, &var_vksurfacekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateWaylandSurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateWaylandSurfaceKHR"));
    log.logger.clear();

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    uint64_t big_chunk_of_mem[512][512];

    inst_disp_table.GetPhysicalDeviceWaylandPresentationSupportKHR(var_vkphysicaldevice, var_uint32_t, reinterpret_cast<struct wl_display*>(&big_chunk_of_mem));
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceWaylandPresentationSupportKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceWaylandPresentationSupportKHR"));
    log.logger.clear();

    inst_disp_table.DestroySurfaceKHR(instance, var_vksurfacekhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySurfaceKHR"));
    log.logger.clear();

} // Test for VK_KHR_wayland_surface
#endif // VK_USE_PLATFORM_WAYLAND_KHR

#ifdef VK_USE_PLATFORM_WIN32_KHR
// Test for VK_KHR_win32_surface
TEST(BasicEntrypointTest, KHR_win32_surface) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_WIN32_SURFACE_EXTENSION_NAME}, {VK_KHR_SURFACE_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    VkWin32SurfaceCreateInfoKHR var_vkwin32surfacecreateinfokhr{VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
    VkSurfaceKHR var_vksurfacekhr{};
    inst_disp_table.CreateWin32SurfaceKHR(instance, &var_vkwin32surfacecreateinfokhr, nullptr, &var_vksurfacekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateWin32SurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateWin32SurfaceKHR"));
    log.logger.clear();

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    inst_disp_table.GetPhysicalDeviceWin32PresentationSupportKHR(var_vkphysicaldevice, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceWin32PresentationSupportKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceWin32PresentationSupportKHR"));
    log.logger.clear();

    inst_disp_table.DestroySurfaceKHR(instance, var_vksurfacekhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySurfaceKHR"));
    log.logger.clear();

} // Test for VK_KHR_win32_surface
#endif // VK_USE_PLATFORM_WIN32_KHR

#ifdef VK_ENABLE_BETA_EXTENSIONS
// Test for VK_KHR_video_queue
TEST(BasicEntrypointTest, KHR_video_queue) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkVideoProfileKHR var_vkvideoprofilekhr{VK_STRUCTURE_TYPE_VIDEO_PROFILE_KHR};
    VkVideoCapabilitiesKHR var_vkvideocapabilitieskhr{VK_STRUCTURE_TYPE_VIDEO_CAPABILITIES_KHR};
    inst_disp_table.GetPhysicalDeviceVideoCapabilitiesKHR(var_vkphysicaldevice, &var_vkvideoprofilekhr, &var_vkvideocapabilitieskhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceVideoCapabilitiesKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceVideoCapabilitiesKHR"));
    log.logger.clear();

    VkPhysicalDeviceVideoFormatInfoKHR var_vkphysicaldevicevideoformatinfokhr{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_FORMAT_INFO_KHR};
    VkVideoFormatPropertiesKHR var_vkvideoformatpropertieskhr{VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR};
    inst_disp_table.GetPhysicalDeviceVideoFormatPropertiesKHR(var_vkphysicaldevice, &var_vkphysicaldevicevideoformatinfokhr, &var_uint32_t, &var_vkvideoformatpropertieskhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceVideoFormatPropertiesKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceVideoFormatPropertiesKHR"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_KHR_VIDEO_QUEUE_EXTENSION_NAME}, {VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkVideoSessionKHR var_vkvideosessionkhr{};
    VkVideoGetMemoryPropertiesKHR var_vkvideogetmemorypropertieskhr{VK_STRUCTURE_TYPE_VIDEO_GET_MEMORY_PROPERTIES_KHR};
    device_disp_table.GetVideoSessionMemoryRequirementsKHR(dev.dev, var_vkvideosessionkhr, &var_uint32_t, &var_vkvideogetmemorypropertieskhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetVideoSessionMemoryRequirementsKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetVideoSessionMemoryRequirementsKHR"));
    log.logger.clear();

    VkVideoBindMemoryKHR var_vkvideobindmemorykhr{VK_STRUCTURE_TYPE_VIDEO_BIND_MEMORY_KHR};
    device_disp_table.BindVideoSessionMemoryKHR(dev.dev, var_vkvideosessionkhr, var_uint32_t, &var_vkvideobindmemorykhr);
    ASSERT_TRUE(log.find("Generated Layer vkBindVideoSessionMemoryKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkBindVideoSessionMemoryKHR"));
    log.logger.clear();

    VkVideoSessionParametersKHR var_vkvideosessionparameterskhr{};
    VkVideoSessionParametersUpdateInfoKHR var_vkvideosessionparametersupdateinfokhr{VK_STRUCTURE_TYPE_VIDEO_SESSION_PARAMETERS_UPDATE_INFO_KHR};
    device_disp_table.UpdateVideoSessionParametersKHR(dev.dev, var_vkvideosessionparameterskhr, &var_vkvideosessionparametersupdateinfokhr);
    ASSERT_TRUE(log.find("Generated Layer vkUpdateVideoSessionParametersKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkUpdateVideoSessionParametersKHR"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkVideoBeginCodingInfoKHR var_vkvideobegincodinginfokhr{VK_STRUCTURE_TYPE_VIDEO_BEGIN_CODING_INFO_KHR};
    device_disp_table.CmdBeginVideoCodingKHR(var_vkcommandbuffer, &var_vkvideobegincodinginfokhr);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBeginVideoCodingKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBeginVideoCodingKHR"));
    log.logger.clear();

    VkVideoEndCodingInfoKHR var_vkvideoendcodinginfokhr{VK_STRUCTURE_TYPE_VIDEO_END_CODING_INFO_KHR};
    device_disp_table.CmdEndVideoCodingKHR(var_vkcommandbuffer, &var_vkvideoendcodinginfokhr);
    ASSERT_TRUE(log.find("Generated Layer vkCmdEndVideoCodingKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdEndVideoCodingKHR"));
    log.logger.clear();

    VkVideoCodingControlInfoKHR var_vkvideocodingcontrolinfokhr{VK_STRUCTURE_TYPE_VIDEO_CODING_CONTROL_INFO_KHR};
    device_disp_table.CmdControlVideoCodingKHR(var_vkcommandbuffer, &var_vkvideocodingcontrolinfokhr);
    ASSERT_TRUE(log.find("Generated Layer vkCmdControlVideoCodingKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdControlVideoCodingKHR"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

    VkVideoSessionCreateInfoKHR var_vkvideosessioncreateinfokhr{VK_STRUCTURE_TYPE_VIDEO_SESSION_CREATE_INFO_KHR};
    device_disp_table.CreateVideoSessionKHR(dev.dev, &var_vkvideosessioncreateinfokhr, nullptr, &var_vkvideosessionkhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateVideoSessionKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateVideoSessionKHR"));
    log.logger.clear();

    device_disp_table.DestroyVideoSessionKHR(dev.dev, var_vkvideosessionkhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyVideoSessionKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyVideoSessionKHR"));
    log.logger.clear();

    VkVideoSessionParametersCreateInfoKHR var_vkvideosessionparameterscreateinfokhr{VK_STRUCTURE_TYPE_VIDEO_SESSION_PARAMETERS_CREATE_INFO_KHR};
    device_disp_table.CreateVideoSessionParametersKHR(dev.dev, &var_vkvideosessionparameterscreateinfokhr, nullptr, &var_vkvideosessionparameterskhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateVideoSessionParametersKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateVideoSessionParametersKHR"));
    log.logger.clear();

    device_disp_table.DestroyVideoSessionParametersKHR(dev.dev, var_vkvideosessionparameterskhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyVideoSessionParametersKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyVideoSessionParametersKHR"));
    log.logger.clear();

} // Test for VK_KHR_video_queue
#endif // VK_ENABLE_BETA_EXTENSIONS

#ifdef VK_ENABLE_BETA_EXTENSIONS
// Test for VK_KHR_video_decode_queue
TEST(BasicEntrypointTest, KHR_video_decode_queue) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_KHR_VIDEO_DECODE_QUEUE_EXTENSION_NAME}, {VK_KHR_VIDEO_QUEUE_EXTENSION_NAME}, {VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkVideoDecodeInfoKHR var_vkvideodecodeinfokhr{VK_STRUCTURE_TYPE_VIDEO_DECODE_INFO_KHR};
    device_disp_table.CmdDecodeVideoKHR(var_vkcommandbuffer, &var_vkvideodecodeinfokhr);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDecodeVideoKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDecodeVideoKHR"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_KHR_video_decode_queue
#endif // VK_ENABLE_BETA_EXTENSIONS

// Test for VK_KHR_dynamic_rendering
TEST(BasicEntrypointTest, KHR_dynamic_rendering) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkRenderingInfo var_vkrenderinginfo{VK_STRUCTURE_TYPE_RENDERING_INFO};
    device_disp_table.CmdBeginRenderingKHR(var_vkcommandbuffer, &var_vkrenderinginfo);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBeginRenderingKHR") ||
                log.find("Generated Layer vkCmdBeginRendering"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBeginRenderingKHR") ||
                log.find("Generated Driver vkCmdBeginRendering"));
    log.logger.clear();

    device_disp_table.CmdEndRenderingKHR(var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkCmdEndRenderingKHR") ||
                log.find("Generated Layer vkCmdEndRendering"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdEndRenderingKHR") ||
                log.find("Generated Driver vkCmdEndRendering"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_KHR_dynamic_rendering

// Test for VK_KHR_get_physical_device_properties2
TEST(BasicEntrypointTest, KHR_get_physical_device_properties2) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkFormat var_vkformat{};
    VkFormatProperties2 var_vkformatproperties2{VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2};
    inst_disp_table.GetPhysicalDeviceFormatProperties2KHR(var_vkphysicaldevice, var_vkformat, &var_vkformatproperties2);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceFormatProperties2KHR") ||
                log.find("Generated Layer vkGetPhysicalDeviceFormatProperties2"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceFormatProperties2KHR") ||
                log.find("Generated Driver vkGetPhysicalDeviceFormatProperties2"));
    log.logger.clear();

    VkPhysicalDeviceSparseImageFormatInfo2 var_vkphysicaldevicesparseimageformatinfo2{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SPARSE_IMAGE_FORMAT_INFO_2};
    VkSparseImageFormatProperties2 var_vksparseimageformatproperties2{VK_STRUCTURE_TYPE_SPARSE_IMAGE_FORMAT_PROPERTIES_2};
    inst_disp_table.GetPhysicalDeviceSparseImageFormatProperties2KHR(var_vkphysicaldevice, &var_vkphysicaldevicesparseimageformatinfo2, &var_uint32_t, &var_vksparseimageformatproperties2);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceSparseImageFormatProperties2KHR") ||
                log.find("Generated Layer vkGetPhysicalDeviceSparseImageFormatProperties2"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceSparseImageFormatProperties2KHR") ||
                log.find("Generated Driver vkGetPhysicalDeviceSparseImageFormatProperties2"));
    log.logger.clear();

} // Test for VK_KHR_get_physical_device_properties2

// Test for VK_KHR_device_group
TEST(BasicEntrypointTest, KHR_device_group) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_KHR_DEVICE_GROUP_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkPeerMemoryFeatureFlags var_vkpeermemoryfeatureflags{};
    device_disp_table.GetDeviceGroupPeerMemoryFeaturesKHR(dev.dev, var_uint32_t, var_uint32_t, var_uint32_t, &var_vkpeermemoryfeatureflags);
    ASSERT_TRUE(log.find("Generated Layer vkGetDeviceGroupPeerMemoryFeaturesKHR") ||
                log.find("Generated Layer vkGetDeviceGroupPeerMemoryFeatures"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDeviceGroupPeerMemoryFeaturesKHR") ||
                log.find("Generated Driver vkGetDeviceGroupPeerMemoryFeatures"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    device_disp_table.CmdSetDeviceMaskKHR(var_vkcommandbuffer, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetDeviceMaskKHR") ||
                log.find("Generated Layer vkCmdSetDeviceMask"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetDeviceMaskKHR") ||
                log.find("Generated Driver vkCmdSetDeviceMask"));
    log.logger.clear();

    device_disp_table.CmdDispatchBaseKHR(var_vkcommandbuffer, var_uint32_t, var_uint32_t, var_uint32_t, var_uint32_t, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDispatchBaseKHR") ||
                log.find("Generated Layer vkCmdDispatchBase"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDispatchBaseKHR") ||
                log.find("Generated Driver vkCmdDispatchBase"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_KHR_device_group

// Test for VK_KHR_maintenance1
TEST(BasicEntrypointTest, KHR_maintenance1) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_KHR_MAINTENANCE1_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkCommandPool var_vkcommandpool{};
    VkCommandPoolTrimFlags var_vkcommandpooltrimflags{};
    device_disp_table.TrimCommandPoolKHR(dev.dev, var_vkcommandpool, var_vkcommandpooltrimflags);
    ASSERT_TRUE(log.find("Generated Layer vkTrimCommandPoolKHR") ||
                log.find("Generated Layer vkTrimCommandPool"));
    ASSERT_TRUE(log.find("Generated Driver vkTrimCommandPoolKHR") ||
                log.find("Generated Driver vkTrimCommandPool"));
    log.logger.clear();

    VkCommandPoolCreateInfo var_vkcommandpoolcreateinfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    device_disp_table.CreateCommandPool(dev.dev, &var_vkcommandpoolcreateinfo, nullptr, &var_vkcommandpool);
    ASSERT_TRUE(log.find("Generated Layer vkCreateCommandPool"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateCommandPool"));
    log.logger.clear();

    device_disp_table.DestroyCommandPool(dev.dev, var_vkcommandpool, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyCommandPool"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyCommandPool"));
    log.logger.clear();

} // Test for VK_KHR_maintenance1

// Test for VK_KHR_device_group_creation
TEST(BasicEntrypointTest, KHR_device_group_creation) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

} // Test for VK_KHR_device_group_creation

// Test for VK_KHR_external_memory_capabilities
TEST(BasicEntrypointTest, KHR_external_memory_capabilities) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkPhysicalDeviceExternalBufferInfo var_vkphysicaldeviceexternalbufferinfo{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_BUFFER_INFO};
    VkExternalBufferProperties var_vkexternalbufferproperties{VK_STRUCTURE_TYPE_EXTERNAL_BUFFER_PROPERTIES};
    inst_disp_table.GetPhysicalDeviceExternalBufferPropertiesKHR(var_vkphysicaldevice, &var_vkphysicaldeviceexternalbufferinfo, &var_vkexternalbufferproperties);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceExternalBufferPropertiesKHR") ||
                log.find("Generated Layer vkGetPhysicalDeviceExternalBufferProperties"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceExternalBufferPropertiesKHR") ||
                log.find("Generated Driver vkGetPhysicalDeviceExternalBufferProperties"));
    log.logger.clear();

} // Test for VK_KHR_external_memory_capabilities

#ifdef VK_USE_PLATFORM_WIN32_KHR
// Test for VK_KHR_external_memory_win32
TEST(BasicEntrypointTest, KHR_external_memory_win32) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME}, {VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkMemoryGetWin32HandleInfoKHR var_vkmemorygetwin32handleinfokhr{VK_STRUCTURE_TYPE_MEMORY_GET_WIN32_HANDLE_INFO_KHR};
    HANDLE var_handle{};
    device_disp_table.GetMemoryWin32HandleKHR(dev.dev, &var_vkmemorygetwin32handleinfokhr, &var_handle);
    ASSERT_TRUE(log.find("Generated Layer vkGetMemoryWin32HandleKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetMemoryWin32HandleKHR"));
    log.logger.clear();

    VkExternalMemoryHandleTypeFlagBits var_vkexternalmemoryhandletypeflagbits{};
    VkMemoryWin32HandlePropertiesKHR var_vkmemorywin32handlepropertieskhr{VK_STRUCTURE_TYPE_MEMORY_WIN32_HANDLE_PROPERTIES_KHR};
    device_disp_table.GetMemoryWin32HandlePropertiesKHR(dev.dev, var_vkexternalmemoryhandletypeflagbits, var_handle, &var_vkmemorywin32handlepropertieskhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetMemoryWin32HandlePropertiesKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetMemoryWin32HandlePropertiesKHR"));
    log.logger.clear();

} // Test for VK_KHR_external_memory_win32
#endif // VK_USE_PLATFORM_WIN32_KHR

// Test for VK_KHR_external_memory_fd
TEST(BasicEntrypointTest, KHR_external_memory_fd) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME}, {VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkMemoryGetFdInfoKHR var_vkmemorygetfdinfokhr{VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR};
    int var_int{};
    device_disp_table.GetMemoryFdKHR(dev.dev, &var_vkmemorygetfdinfokhr, &var_int);
    ASSERT_TRUE(log.find("Generated Layer vkGetMemoryFdKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetMemoryFdKHR"));
    log.logger.clear();

    VkExternalMemoryHandleTypeFlagBits var_vkexternalmemoryhandletypeflagbits{};
    VkMemoryFdPropertiesKHR var_vkmemoryfdpropertieskhr{VK_STRUCTURE_TYPE_MEMORY_FD_PROPERTIES_KHR};
    device_disp_table.GetMemoryFdPropertiesKHR(dev.dev, var_vkexternalmemoryhandletypeflagbits, var_int, &var_vkmemoryfdpropertieskhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetMemoryFdPropertiesKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetMemoryFdPropertiesKHR"));
    log.logger.clear();

} // Test for VK_KHR_external_memory_fd

// Test for VK_KHR_external_semaphore_capabilities
TEST(BasicEntrypointTest, KHR_external_semaphore_capabilities) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkPhysicalDeviceExternalSemaphoreInfo var_vkphysicaldeviceexternalsemaphoreinfo{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_SEMAPHORE_INFO};
    VkExternalSemaphoreProperties var_vkexternalsemaphoreproperties{VK_STRUCTURE_TYPE_EXTERNAL_SEMAPHORE_PROPERTIES};
    inst_disp_table.GetPhysicalDeviceExternalSemaphorePropertiesKHR(var_vkphysicaldevice, &var_vkphysicaldeviceexternalsemaphoreinfo, &var_vkexternalsemaphoreproperties);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceExternalSemaphorePropertiesKHR") ||
                log.find("Generated Layer vkGetPhysicalDeviceExternalSemaphoreProperties"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceExternalSemaphorePropertiesKHR") ||
                log.find("Generated Driver vkGetPhysicalDeviceExternalSemaphoreProperties"));
    log.logger.clear();

} // Test for VK_KHR_external_semaphore_capabilities

#ifdef VK_USE_PLATFORM_WIN32_KHR
// Test for VK_KHR_external_semaphore_win32
TEST(BasicEntrypointTest, KHR_external_semaphore_win32) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME}, {VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkImportSemaphoreWin32HandleInfoKHR var_vkimportsemaphorewin32handleinfokhr{VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_WIN32_HANDLE_INFO_KHR};
    device_disp_table.ImportSemaphoreWin32HandleKHR(dev.dev, &var_vkimportsemaphorewin32handleinfokhr);
    ASSERT_TRUE(log.find("Generated Layer vkImportSemaphoreWin32HandleKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkImportSemaphoreWin32HandleKHR"));
    log.logger.clear();

    VkSemaphoreGetWin32HandleInfoKHR var_vksemaphoregetwin32handleinfokhr{VK_STRUCTURE_TYPE_SEMAPHORE_GET_WIN32_HANDLE_INFO_KHR};
    HANDLE var_handle{};
    device_disp_table.GetSemaphoreWin32HandleKHR(dev.dev, &var_vksemaphoregetwin32handleinfokhr, &var_handle);
    ASSERT_TRUE(log.find("Generated Layer vkGetSemaphoreWin32HandleKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetSemaphoreWin32HandleKHR"));
    log.logger.clear();

} // Test for VK_KHR_external_semaphore_win32
#endif // VK_USE_PLATFORM_WIN32_KHR

// Test for VK_KHR_external_semaphore_fd
TEST(BasicEntrypointTest, KHR_external_semaphore_fd) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME}, {VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkImportSemaphoreFdInfoKHR var_vkimportsemaphorefdinfokhr{VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_FD_INFO_KHR};
    device_disp_table.ImportSemaphoreFdKHR(dev.dev, &var_vkimportsemaphorefdinfokhr);
    ASSERT_TRUE(log.find("Generated Layer vkImportSemaphoreFdKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkImportSemaphoreFdKHR"));
    log.logger.clear();

    VkSemaphoreGetFdInfoKHR var_vksemaphoregetfdinfokhr{VK_STRUCTURE_TYPE_SEMAPHORE_GET_FD_INFO_KHR};
    int var_int{};
    device_disp_table.GetSemaphoreFdKHR(dev.dev, &var_vksemaphoregetfdinfokhr, &var_int);
    ASSERT_TRUE(log.find("Generated Layer vkGetSemaphoreFdKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetSemaphoreFdKHR"));
    log.logger.clear();

} // Test for VK_KHR_external_semaphore_fd

// Test for VK_KHR_push_descriptor
TEST(BasicEntrypointTest, KHR_push_descriptor) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkPipelineLayoutCreateInfo var_vkpipelinelayoutcreateinfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    VkPipelineLayout var_vkpipelinelayout{};
    device_disp_table.CreatePipelineLayout(dev.dev, &var_vkpipelinelayoutcreateinfo, nullptr, &var_vkpipelinelayout);
    ASSERT_TRUE(log.find("Generated Layer vkCreatePipelineLayout"));
    ASSERT_TRUE(log.find("Generated Driver vkCreatePipelineLayout"));
    log.logger.clear();

    device_disp_table.DestroyPipelineLayout(dev.dev, var_vkpipelinelayout, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyPipelineLayout"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyPipelineLayout"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkPipelineBindPoint var_vkpipelinebindpoint{};
    VkWriteDescriptorSet var_vkwritedescriptorset{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
    device_disp_table.CmdPushDescriptorSetKHR(var_vkcommandbuffer, var_vkpipelinebindpoint, var_vkpipelinelayout, var_uint32_t, var_uint32_t, &var_vkwritedescriptorset);
    ASSERT_TRUE(log.find("Generated Layer vkCmdPushDescriptorSetKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdPushDescriptorSetKHR"));
    log.logger.clear();

    VkDescriptorUpdateTemplate var_vkdescriptorupdatetemplate{};
    uint64_t big_chunk_of_mem[512][512];

    device_disp_table.CmdPushDescriptorSetWithTemplateKHR(var_vkcommandbuffer, var_vkdescriptorupdatetemplate, var_vkpipelinelayout, var_uint32_t, reinterpret_cast<void**>(big_chunk_of_mem));
    ASSERT_TRUE(log.find("Generated Layer vkCmdPushDescriptorSetWithTemplateKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdPushDescriptorSetWithTemplateKHR"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

    VkDescriptorUpdateTemplateCreateInfo var_vkdescriptorupdatetemplatecreateinfo{VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO};
    device_disp_table.CreateDescriptorUpdateTemplate(dev.dev, &var_vkdescriptorupdatetemplatecreateinfo, nullptr, &var_vkdescriptorupdatetemplate);
    ASSERT_TRUE(log.find("Generated Layer vkCreateDescriptorUpdateTemplate"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateDescriptorUpdateTemplate"));
    log.logger.clear();

    device_disp_table.DestroyDescriptorUpdateTemplate(dev.dev, var_vkdescriptorupdatetemplate, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyDescriptorUpdateTemplate"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyDescriptorUpdateTemplate"));
    log.logger.clear();

} // Test for VK_KHR_push_descriptor

// Test for VK_KHR_descriptor_update_template
TEST(BasicEntrypointTest, KHR_descriptor_update_template) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkDescriptorSet var_vkdescriptorset{};
    VkDescriptorUpdateTemplate var_vkdescriptorupdatetemplate{};
    uint64_t big_chunk_of_mem[512][512];

    device_disp_table.UpdateDescriptorSetWithTemplateKHR(dev.dev, var_vkdescriptorset, var_vkdescriptorupdatetemplate, reinterpret_cast<void**>(big_chunk_of_mem));
    ASSERT_TRUE(log.find("Generated Layer vkUpdateDescriptorSetWithTemplateKHR") ||
                log.find("Generated Layer vkUpdateDescriptorSetWithTemplate"));
    ASSERT_TRUE(log.find("Generated Driver vkUpdateDescriptorSetWithTemplateKHR") ||
                log.find("Generated Driver vkUpdateDescriptorSetWithTemplate"));
    log.logger.clear();

    VkDescriptorSetAllocateInfo var_vkdescriptorsetallocateinfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    device_disp_table.AllocateDescriptorSets(dev.dev, &var_vkdescriptorsetallocateinfo, &var_vkdescriptorset);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateDescriptorSets"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateDescriptorSets"));
    log.logger.clear();

    VkDescriptorPool var_vkdescriptorpool{};
    device_disp_table.FreeDescriptorSets(dev.dev, var_vkdescriptorpool, var_uint32_t, &var_vkdescriptorset);
    ASSERT_TRUE(log.find("Generated Layer vkFreeDescriptorSets"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeDescriptorSets"));
    log.logger.clear();

    VkDescriptorUpdateTemplateCreateInfo var_vkdescriptorupdatetemplatecreateinfo{VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO};
    device_disp_table.CreateDescriptorUpdateTemplate(dev.dev, &var_vkdescriptorupdatetemplatecreateinfo, nullptr, &var_vkdescriptorupdatetemplate);
    ASSERT_TRUE(log.find("Generated Layer vkCreateDescriptorUpdateTemplate"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateDescriptorUpdateTemplate"));
    log.logger.clear();

    device_disp_table.DestroyDescriptorUpdateTemplate(dev.dev, var_vkdescriptorupdatetemplate, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyDescriptorUpdateTemplate"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyDescriptorUpdateTemplate"));
    log.logger.clear();

} // Test for VK_KHR_descriptor_update_template

// Test for VK_KHR_create_renderpass2
TEST(BasicEntrypointTest, KHR_create_renderpass2) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME}, {VK_KHR_MULTIVIEW_EXTENSION_NAME}, {VK_KHR_MAINTENANCE2_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkRenderPassBeginInfo var_vkrenderpassbegininfo{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    VkSubpassBeginInfo var_vksubpassbegininfo{VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO};
    device_disp_table.CmdBeginRenderPass2KHR(var_vkcommandbuffer, &var_vkrenderpassbegininfo, &var_vksubpassbegininfo);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBeginRenderPass2KHR") ||
                log.find("Generated Layer vkCmdBeginRenderPass2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBeginRenderPass2KHR") ||
                log.find("Generated Driver vkCmdBeginRenderPass2"));
    log.logger.clear();

    VkSubpassEndInfo var_vksubpassendinfo{VK_STRUCTURE_TYPE_SUBPASS_END_INFO};
    device_disp_table.CmdNextSubpass2KHR(var_vkcommandbuffer, &var_vksubpassbegininfo, &var_vksubpassendinfo);
    ASSERT_TRUE(log.find("Generated Layer vkCmdNextSubpass2KHR") ||
                log.find("Generated Layer vkCmdNextSubpass2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdNextSubpass2KHR") ||
                log.find("Generated Driver vkCmdNextSubpass2"));
    log.logger.clear();

    device_disp_table.CmdEndRenderPass2KHR(var_vkcommandbuffer, &var_vksubpassendinfo);
    ASSERT_TRUE(log.find("Generated Layer vkCmdEndRenderPass2KHR") ||
                log.find("Generated Layer vkCmdEndRenderPass2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdEndRenderPass2KHR") ||
                log.find("Generated Driver vkCmdEndRenderPass2"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_KHR_create_renderpass2

// Test for VK_KHR_shared_presentable_image
TEST(BasicEntrypointTest, KHR_shared_presentable_image) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}, {VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_KHR_SHARED_PRESENTABLE_IMAGE_EXTENSION_NAME}, {VK_KHR_SWAPCHAIN_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkSwapchainKHR var_vkswapchainkhr{};
    device_disp_table.GetSwapchainStatusKHR(dev.dev, var_vkswapchainkhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetSwapchainStatusKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetSwapchainStatusKHR"));
    log.logger.clear();

} // Test for VK_KHR_shared_presentable_image

// Test for VK_KHR_external_fence_capabilities
TEST(BasicEntrypointTest, KHR_external_fence_capabilities) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkPhysicalDeviceExternalFenceInfo var_vkphysicaldeviceexternalfenceinfo{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_FENCE_INFO};
    VkExternalFenceProperties var_vkexternalfenceproperties{VK_STRUCTURE_TYPE_EXTERNAL_FENCE_PROPERTIES};
    inst_disp_table.GetPhysicalDeviceExternalFencePropertiesKHR(var_vkphysicaldevice, &var_vkphysicaldeviceexternalfenceinfo, &var_vkexternalfenceproperties);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceExternalFencePropertiesKHR") ||
                log.find("Generated Layer vkGetPhysicalDeviceExternalFenceProperties"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceExternalFencePropertiesKHR") ||
                log.find("Generated Driver vkGetPhysicalDeviceExternalFenceProperties"));
    log.logger.clear();

} // Test for VK_KHR_external_fence_capabilities

#ifdef VK_USE_PLATFORM_WIN32_KHR
// Test for VK_KHR_external_fence_win32
TEST(BasicEntrypointTest, KHR_external_fence_win32) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_KHR_EXTERNAL_FENCE_WIN32_EXTENSION_NAME}, {VK_KHR_EXTERNAL_FENCE_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkImportFenceWin32HandleInfoKHR var_vkimportfencewin32handleinfokhr{VK_STRUCTURE_TYPE_IMPORT_FENCE_WIN32_HANDLE_INFO_KHR};
    device_disp_table.ImportFenceWin32HandleKHR(dev.dev, &var_vkimportfencewin32handleinfokhr);
    ASSERT_TRUE(log.find("Generated Layer vkImportFenceWin32HandleKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkImportFenceWin32HandleKHR"));
    log.logger.clear();

    VkFenceGetWin32HandleInfoKHR var_vkfencegetwin32handleinfokhr{VK_STRUCTURE_TYPE_FENCE_GET_WIN32_HANDLE_INFO_KHR};
    HANDLE var_handle{};
    device_disp_table.GetFenceWin32HandleKHR(dev.dev, &var_vkfencegetwin32handleinfokhr, &var_handle);
    ASSERT_TRUE(log.find("Generated Layer vkGetFenceWin32HandleKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetFenceWin32HandleKHR"));
    log.logger.clear();

} // Test for VK_KHR_external_fence_win32
#endif // VK_USE_PLATFORM_WIN32_KHR

// Test for VK_KHR_external_fence_fd
TEST(BasicEntrypointTest, KHR_external_fence_fd) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_KHR_EXTERNAL_FENCE_FD_EXTENSION_NAME}, {VK_KHR_EXTERNAL_FENCE_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkImportFenceFdInfoKHR var_vkimportfencefdinfokhr{VK_STRUCTURE_TYPE_IMPORT_FENCE_FD_INFO_KHR};
    device_disp_table.ImportFenceFdKHR(dev.dev, &var_vkimportfencefdinfokhr);
    ASSERT_TRUE(log.find("Generated Layer vkImportFenceFdKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkImportFenceFdKHR"));
    log.logger.clear();

    VkFenceGetFdInfoKHR var_vkfencegetfdinfokhr{VK_STRUCTURE_TYPE_FENCE_GET_FD_INFO_KHR};
    int var_int{};
    device_disp_table.GetFenceFdKHR(dev.dev, &var_vkfencegetfdinfokhr, &var_int);
    ASSERT_TRUE(log.find("Generated Layer vkGetFenceFdKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetFenceFdKHR"));
    log.logger.clear();

} // Test for VK_KHR_external_fence_fd

// Test for VK_KHR_performance_query
TEST(BasicEntrypointTest, KHR_performance_query) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkQueryPoolPerformanceCreateInfoKHR var_vkquerypoolperformancecreateinfokhr{VK_STRUCTURE_TYPE_QUERY_POOL_PERFORMANCE_CREATE_INFO_KHR};
    inst_disp_table.GetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR(var_vkphysicaldevice, &var_vkquerypoolperformancecreateinfokhr, &var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkAcquireProfilingLockInfoKHR var_vkacquireprofilinglockinfokhr{VK_STRUCTURE_TYPE_ACQUIRE_PROFILING_LOCK_INFO_KHR};
    device_disp_table.AcquireProfilingLockKHR(dev.dev, &var_vkacquireprofilinglockinfokhr);
    ASSERT_TRUE(log.find("Generated Layer vkAcquireProfilingLockKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkAcquireProfilingLockKHR"));
    log.logger.clear();

    device_disp_table.ReleaseProfilingLockKHR(dev.dev);
    ASSERT_TRUE(log.find("Generated Layer vkReleaseProfilingLockKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkReleaseProfilingLockKHR"));
    log.logger.clear();

} // Test for VK_KHR_performance_query

// Test for VK_KHR_get_surface_capabilities2
TEST(BasicEntrypointTest, KHR_get_surface_capabilities2) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME}, {VK_KHR_SURFACE_EXTENSION_NAME}, {VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    VkHeadlessSurfaceCreateInfoEXT var_vkheadlesssurfacecreateinfoext{VK_STRUCTURE_TYPE_HEADLESS_SURFACE_CREATE_INFO_EXT};
    VkSurfaceKHR var_vksurfacekhr{};
    inst_disp_table.CreateHeadlessSurfaceEXT(instance, &var_vkheadlesssurfacecreateinfoext, nullptr, &var_vksurfacekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateHeadlessSurfaceEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateHeadlessSurfaceEXT"));
    log.logger.clear();

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkPhysicalDeviceSurfaceInfo2KHR var_vkphysicaldevicesurfaceinfo2khr{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR};
    var_vkphysicaldevicesurfaceinfo2khr.surface = var_vksurfacekhr;
    VkSurfaceCapabilities2KHR var_vksurfacecapabilities2khr{VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR};
    inst_disp_table.GetPhysicalDeviceSurfaceCapabilities2KHR(var_vkphysicaldevice, &var_vkphysicaldevicesurfaceinfo2khr, &var_vksurfacecapabilities2khr);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceSurfaceCapabilities2KHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceSurfaceCapabilities2KHR"));
    log.logger.clear();

    VkSurfaceFormat2KHR var_vksurfaceformat2khr{VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR};
    inst_disp_table.GetPhysicalDeviceSurfaceFormats2KHR(var_vkphysicaldevice, &var_vkphysicaldevicesurfaceinfo2khr, &var_uint32_t, &var_vksurfaceformat2khr);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceSurfaceFormats2KHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceSurfaceFormats2KHR"));
    log.logger.clear();

    inst_disp_table.DestroySurfaceKHR(instance, var_vksurfacekhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySurfaceKHR"));
    log.logger.clear();

} // Test for VK_KHR_get_surface_capabilities2

// Test for VK_KHR_get_display_properties2
TEST(BasicEntrypointTest, KHR_get_display_properties2) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME}, {VK_KHR_DISPLAY_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkDisplayProperties2KHR var_vkdisplayproperties2khr{VK_STRUCTURE_TYPE_DISPLAY_PROPERTIES_2_KHR};
    inst_disp_table.GetPhysicalDeviceDisplayProperties2KHR(var_vkphysicaldevice, &var_uint32_t, &var_vkdisplayproperties2khr);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceDisplayProperties2KHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceDisplayProperties2KHR"));
    log.logger.clear();

    VkDisplayPlaneProperties2KHR var_vkdisplayplaneproperties2khr{VK_STRUCTURE_TYPE_DISPLAY_PLANE_PROPERTIES_2_KHR};
    inst_disp_table.GetPhysicalDeviceDisplayPlaneProperties2KHR(var_vkphysicaldevice, &var_uint32_t, &var_vkdisplayplaneproperties2khr);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceDisplayPlaneProperties2KHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceDisplayPlaneProperties2KHR"));
    log.logger.clear();

    VkDisplayKHR var_vkdisplaykhr{};
    VkDisplayModeProperties2KHR var_vkdisplaymodeproperties2khr{VK_STRUCTURE_TYPE_DISPLAY_MODE_PROPERTIES_2_KHR};
    inst_disp_table.GetDisplayModeProperties2KHR(var_vkphysicaldevice, var_vkdisplaykhr, &var_uint32_t, &var_vkdisplaymodeproperties2khr);
    ASSERT_TRUE(log.find("Generated Layer vkGetDisplayModeProperties2KHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDisplayModeProperties2KHR"));
    log.logger.clear();

    VkDisplayPlaneInfo2KHR var_vkdisplayplaneinfo2khr{VK_STRUCTURE_TYPE_DISPLAY_PLANE_INFO_2_KHR};
    VkDisplayPlaneCapabilities2KHR var_vkdisplayplanecapabilities2khr{VK_STRUCTURE_TYPE_DISPLAY_PLANE_CAPABILITIES_2_KHR};
    inst_disp_table.GetDisplayPlaneCapabilities2KHR(var_vkphysicaldevice, &var_vkdisplayplaneinfo2khr, &var_vkdisplayplanecapabilities2khr);
    ASSERT_TRUE(log.find("Generated Layer vkGetDisplayPlaneCapabilities2KHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDisplayPlaneCapabilities2KHR"));
    log.logger.clear();

} // Test for VK_KHR_get_display_properties2

// Test for VK_KHR_get_memory_requirements2
TEST(BasicEntrypointTest, KHR_get_memory_requirements2) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkImageMemoryRequirementsInfo2 var_vkimagememoryrequirementsinfo2{VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2};
    VkMemoryRequirements2 var_vkmemoryrequirements2{VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2};
    device_disp_table.GetImageMemoryRequirements2KHR(dev.dev, &var_vkimagememoryrequirementsinfo2, &var_vkmemoryrequirements2);
    ASSERT_TRUE(log.find("Generated Layer vkGetImageMemoryRequirements2KHR") ||
                log.find("Generated Layer vkGetImageMemoryRequirements2"));
    ASSERT_TRUE(log.find("Generated Driver vkGetImageMemoryRequirements2KHR") ||
                log.find("Generated Driver vkGetImageMemoryRequirements2"));
    log.logger.clear();

    VkBufferMemoryRequirementsInfo2 var_vkbuffermemoryrequirementsinfo2{VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2};
    device_disp_table.GetBufferMemoryRequirements2KHR(dev.dev, &var_vkbuffermemoryrequirementsinfo2, &var_vkmemoryrequirements2);
    ASSERT_TRUE(log.find("Generated Layer vkGetBufferMemoryRequirements2KHR") ||
                log.find("Generated Layer vkGetBufferMemoryRequirements2"));
    ASSERT_TRUE(log.find("Generated Driver vkGetBufferMemoryRequirements2KHR") ||
                log.find("Generated Driver vkGetBufferMemoryRequirements2"));
    log.logger.clear();

    VkImageSparseMemoryRequirementsInfo2 var_vkimagesparsememoryrequirementsinfo2{VK_STRUCTURE_TYPE_IMAGE_SPARSE_MEMORY_REQUIREMENTS_INFO_2};
    VkSparseImageMemoryRequirements2 var_vksparseimagememoryrequirements2{VK_STRUCTURE_TYPE_SPARSE_IMAGE_MEMORY_REQUIREMENTS_2};
    device_disp_table.GetImageSparseMemoryRequirements2KHR(dev.dev, &var_vkimagesparsememoryrequirementsinfo2, &var_uint32_t, &var_vksparseimagememoryrequirements2);
    ASSERT_TRUE(log.find("Generated Layer vkGetImageSparseMemoryRequirements2KHR") ||
                log.find("Generated Layer vkGetImageSparseMemoryRequirements2"));
    ASSERT_TRUE(log.find("Generated Driver vkGetImageSparseMemoryRequirements2KHR") ||
                log.find("Generated Driver vkGetImageSparseMemoryRequirements2"));
    log.logger.clear();

} // Test for VK_KHR_get_memory_requirements2

// Test for VK_KHR_sampler_ycbcr_conversion
TEST(BasicEntrypointTest, KHR_sampler_ycbcr_conversion) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME}, {VK_KHR_MAINTENANCE1_EXTENSION_NAME}, {VK_KHR_BIND_MEMORY_2_EXTENSION_NAME}, {VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

} // Test for VK_KHR_sampler_ycbcr_conversion

// Test for VK_KHR_bind_memory2
TEST(BasicEntrypointTest, KHR_bind_memory2) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_KHR_BIND_MEMORY_2_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkBindBufferMemoryInfo var_vkbindbuffermemoryinfo{VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO};
    device_disp_table.BindBufferMemory2KHR(dev.dev, var_uint32_t, &var_vkbindbuffermemoryinfo);
    ASSERT_TRUE(log.find("Generated Layer vkBindBufferMemory2KHR") ||
                log.find("Generated Layer vkBindBufferMemory2"));
    ASSERT_TRUE(log.find("Generated Driver vkBindBufferMemory2KHR") ||
                log.find("Generated Driver vkBindBufferMemory2"));
    log.logger.clear();

    VkBindImageMemoryInfo var_vkbindimagememoryinfo{VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO};
    device_disp_table.BindImageMemory2KHR(dev.dev, var_uint32_t, &var_vkbindimagememoryinfo);
    ASSERT_TRUE(log.find("Generated Layer vkBindImageMemory2KHR") ||
                log.find("Generated Layer vkBindImageMemory2"));
    ASSERT_TRUE(log.find("Generated Driver vkBindImageMemory2KHR") ||
                log.find("Generated Driver vkBindImageMemory2"));
    log.logger.clear();

} // Test for VK_KHR_bind_memory2

// Test for VK_KHR_maintenance3
TEST(BasicEntrypointTest, KHR_maintenance3) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_KHR_MAINTENANCE3_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

} // Test for VK_KHR_maintenance3

// Test for VK_KHR_draw_indirect_count
TEST(BasicEntrypointTest, KHR_draw_indirect_count) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkBufferCreateInfo var_vkbuffercreateinfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    VkBuffer var_vkbuffer{};
    device_disp_table.CreateBuffer(dev.dev, &var_vkbuffercreateinfo, nullptr, &var_vkbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkCreateBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateBuffer"));
    log.logger.clear();

    device_disp_table.DestroyBuffer(dev.dev, var_vkbuffer, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyBuffer"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkDeviceSize var_vkdevicesize{};
    device_disp_table.CmdDrawIndirectCountKHR(var_vkcommandbuffer, var_vkbuffer, var_vkdevicesize, var_vkbuffer, var_vkdevicesize, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDrawIndirectCountKHR") ||
                log.find("Generated Layer vkCmdDrawIndirectCount"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDrawIndirectCountKHR") ||
                log.find("Generated Driver vkCmdDrawIndirectCount"));
    log.logger.clear();

    device_disp_table.CmdDrawIndexedIndirectCountKHR(var_vkcommandbuffer, var_vkbuffer, var_vkdevicesize, var_vkbuffer, var_vkdevicesize, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDrawIndexedIndirectCountKHR") ||
                log.find("Generated Layer vkCmdDrawIndexedIndirectCount"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDrawIndexedIndirectCountKHR") ||
                log.find("Generated Driver vkCmdDrawIndexedIndirectCount"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_KHR_draw_indirect_count

// Test for VK_KHR_timeline_semaphore
TEST(BasicEntrypointTest, KHR_timeline_semaphore) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkSemaphore var_vksemaphore{};
    uint64_t var_uint64_t = 1;
    device_disp_table.GetSemaphoreCounterValueKHR(dev.dev, var_vksemaphore, &var_uint64_t);
    ASSERT_TRUE(log.find("Generated Layer vkGetSemaphoreCounterValueKHR") ||
                log.find("Generated Layer vkGetSemaphoreCounterValue"));
    ASSERT_TRUE(log.find("Generated Driver vkGetSemaphoreCounterValueKHR") ||
                log.find("Generated Driver vkGetSemaphoreCounterValue"));
    log.logger.clear();

    VkSemaphoreWaitInfo var_vksemaphorewaitinfo{VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO};
    device_disp_table.WaitSemaphoresKHR(dev.dev, &var_vksemaphorewaitinfo, var_uint64_t);
    ASSERT_TRUE(log.find("Generated Layer vkWaitSemaphoresKHR") ||
                log.find("Generated Layer vkWaitSemaphores"));
    ASSERT_TRUE(log.find("Generated Driver vkWaitSemaphoresKHR") ||
                log.find("Generated Driver vkWaitSemaphores"));
    log.logger.clear();

    VkSemaphoreSignalInfo var_vksemaphoresignalinfo{VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO};
    device_disp_table.SignalSemaphoreKHR(dev.dev, &var_vksemaphoresignalinfo);
    ASSERT_TRUE(log.find("Generated Layer vkSignalSemaphoreKHR") ||
                log.find("Generated Layer vkSignalSemaphore"));
    ASSERT_TRUE(log.find("Generated Driver vkSignalSemaphoreKHR") ||
                log.find("Generated Driver vkSignalSemaphore"));
    log.logger.clear();

    VkSemaphoreCreateInfo var_vksemaphorecreateinfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    device_disp_table.CreateSemaphore(dev.dev, &var_vksemaphorecreateinfo, nullptr, &var_vksemaphore);
    ASSERT_TRUE(log.find("Generated Layer vkCreateSemaphore"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateSemaphore"));
    log.logger.clear();

    device_disp_table.DestroySemaphore(dev.dev, var_vksemaphore, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySemaphore"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySemaphore"));
    log.logger.clear();

} // Test for VK_KHR_timeline_semaphore

// Test for VK_KHR_fragment_shading_rate
TEST(BasicEntrypointTest, KHR_fragment_shading_rate) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME}, {VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkExtent2D var_vkextent2d{};
    VkFragmentShadingRateCombinerOpKHR var_vkfragmentshadingratecombineropkhr_1d_2[2]{};
    device_disp_table.CmdSetFragmentShadingRateKHR(var_vkcommandbuffer, &var_vkextent2d, var_vkfragmentshadingratecombineropkhr_1d_2);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetFragmentShadingRateKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetFragmentShadingRateKHR"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_KHR_fragment_shading_rate

// Test for VK_KHR_present_wait
TEST(BasicEntrypointTest, KHR_present_wait) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_KHR_PRESENT_WAIT_EXTENSION_NAME}, {VK_KHR_SWAPCHAIN_EXTENSION_NAME}, {VK_KHR_PRESENT_ID_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkSwapchainKHR var_vkswapchainkhr{};
    uint64_t var_uint64_t = 1;
    device_disp_table.WaitForPresentKHR(dev.dev, var_vkswapchainkhr, var_uint64_t, var_uint64_t);
    ASSERT_TRUE(log.find("Generated Layer vkWaitForPresentKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkWaitForPresentKHR"));
    log.logger.clear();

} // Test for VK_KHR_present_wait

// Test for VK_KHR_buffer_device_address
TEST(BasicEntrypointTest, KHR_buffer_device_address) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkBufferDeviceAddressInfo var_vkbufferdeviceaddressinfo{VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO};
    device_disp_table.GetBufferDeviceAddressKHR(dev.dev, &var_vkbufferdeviceaddressinfo);
    ASSERT_TRUE(log.find("Generated Layer vkGetBufferDeviceAddressKHR") ||
                log.find("Generated Layer vkGetBufferDeviceAddress"));
    ASSERT_TRUE(log.find("Generated Driver vkGetBufferDeviceAddressKHR") ||
                log.find("Generated Driver vkGetBufferDeviceAddress"));
    log.logger.clear();

    device_disp_table.GetBufferOpaqueCaptureAddressKHR(dev.dev, &var_vkbufferdeviceaddressinfo);
    ASSERT_TRUE(log.find("Generated Layer vkGetBufferOpaqueCaptureAddressKHR") ||
                log.find("Generated Layer vkGetBufferOpaqueCaptureAddress"));
    ASSERT_TRUE(log.find("Generated Driver vkGetBufferOpaqueCaptureAddressKHR") ||
                log.find("Generated Driver vkGetBufferOpaqueCaptureAddress"));
    log.logger.clear();

    VkDeviceMemoryOpaqueCaptureAddressInfo var_vkdevicememoryopaquecaptureaddressinfo{VK_STRUCTURE_TYPE_DEVICE_MEMORY_OPAQUE_CAPTURE_ADDRESS_INFO};
    device_disp_table.GetDeviceMemoryOpaqueCaptureAddressKHR(dev.dev, &var_vkdevicememoryopaquecaptureaddressinfo);
    ASSERT_TRUE(log.find("Generated Layer vkGetDeviceMemoryOpaqueCaptureAddressKHR") ||
                log.find("Generated Layer vkGetDeviceMemoryOpaqueCaptureAddress"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDeviceMemoryOpaqueCaptureAddressKHR") ||
                log.find("Generated Driver vkGetDeviceMemoryOpaqueCaptureAddress"));
    log.logger.clear();

} // Test for VK_KHR_buffer_device_address

// Test for VK_KHR_deferred_host_operations
TEST(BasicEntrypointTest, KHR_deferred_host_operations) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkDeferredOperationKHR var_vkdeferredoperationkhr{};
    device_disp_table.GetDeferredOperationMaxConcurrencyKHR(dev.dev, var_vkdeferredoperationkhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetDeferredOperationMaxConcurrencyKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDeferredOperationMaxConcurrencyKHR"));
    log.logger.clear();

    device_disp_table.GetDeferredOperationResultKHR(dev.dev, var_vkdeferredoperationkhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetDeferredOperationResultKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDeferredOperationResultKHR"));
    log.logger.clear();

    device_disp_table.DeferredOperationJoinKHR(dev.dev, var_vkdeferredoperationkhr);
    ASSERT_TRUE(log.find("Generated Layer vkDeferredOperationJoinKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDeferredOperationJoinKHR"));
    log.logger.clear();

    device_disp_table.CreateDeferredOperationKHR(dev.dev, nullptr, &var_vkdeferredoperationkhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateDeferredOperationKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateDeferredOperationKHR"));
    log.logger.clear();

    device_disp_table.DestroyDeferredOperationKHR(dev.dev, var_vkdeferredoperationkhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyDeferredOperationKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyDeferredOperationKHR"));
    log.logger.clear();

} // Test for VK_KHR_deferred_host_operations

// Test for VK_KHR_pipeline_executable_properties
TEST(BasicEntrypointTest, KHR_pipeline_executable_properties) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_KHR_PIPELINE_EXECUTABLE_PROPERTIES_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

} // Test for VK_KHR_pipeline_executable_properties

#ifdef VK_ENABLE_BETA_EXTENSIONS
// Test for VK_KHR_video_encode_queue
TEST(BasicEntrypointTest, KHR_video_encode_queue) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_KHR_VIDEO_ENCODE_QUEUE_EXTENSION_NAME}, {VK_KHR_VIDEO_QUEUE_EXTENSION_NAME}, {VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkVideoEncodeInfoKHR var_vkvideoencodeinfokhr{VK_STRUCTURE_TYPE_VIDEO_ENCODE_INFO_KHR};
    device_disp_table.CmdEncodeVideoKHR(var_vkcommandbuffer, &var_vkvideoencodeinfokhr);
    ASSERT_TRUE(log.find("Generated Layer vkCmdEncodeVideoKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdEncodeVideoKHR"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_KHR_video_encode_queue
#endif // VK_ENABLE_BETA_EXTENSIONS

// Test for VK_KHR_synchronization2
TEST(BasicEntrypointTest, KHR_synchronization2) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME}, {VK_AMD_BUFFER_MARKER_EXTENSION_NAME}, {VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkDeviceQueueInfo2 var_vkdevicequeueinfo2{VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2};
    VkQueue var_vkqueue{};
    device_disp_table.GetDeviceQueue2(dev.dev, &var_vkdevicequeueinfo2, &var_vkqueue);
    ASSERT_TRUE(log.find("Generated Layer vkGetDeviceQueue2"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDeviceQueue2"));
    log.logger.clear();

    VkSubmitInfo2 var_vksubmitinfo2{VK_STRUCTURE_TYPE_SUBMIT_INFO_2};
    VkFence var_vkfence{};
    device_disp_table.QueueSubmit2KHR(var_vkqueue, var_uint32_t, &var_vksubmitinfo2, var_vkfence);
    ASSERT_TRUE(log.find("Generated Layer vkQueueSubmit2KHR") ||
                log.find("Generated Layer vkQueueSubmit2"));
    ASSERT_TRUE(log.find("Generated Driver vkQueueSubmit2KHR") ||
                log.find("Generated Driver vkQueueSubmit2"));
    log.logger.clear();

    VkCheckpointData2NV var_vkcheckpointdata2nv{VK_STRUCTURE_TYPE_CHECKPOINT_DATA_2_NV};
    device_disp_table.GetQueueCheckpointData2NV(var_vkqueue, &var_uint32_t, &var_vkcheckpointdata2nv);
    ASSERT_TRUE(log.find("Generated Layer vkGetQueueCheckpointData2NV"));
    ASSERT_TRUE(log.find("Generated Driver vkGetQueueCheckpointData2NV"));
    log.logger.clear();

    VkFenceCreateInfo var_vkfencecreateinfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    device_disp_table.CreateFence(dev.dev, &var_vkfencecreateinfo, nullptr, &var_vkfence);
    ASSERT_TRUE(log.find("Generated Layer vkCreateFence"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateFence"));
    log.logger.clear();

    device_disp_table.DestroyFence(dev.dev, var_vkfence, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyFence"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyFence"));
    log.logger.clear();

    VkEventCreateInfo var_vkeventcreateinfo{VK_STRUCTURE_TYPE_EVENT_CREATE_INFO};
    VkEvent var_vkevent{};
    device_disp_table.CreateEvent(dev.dev, &var_vkeventcreateinfo, nullptr, &var_vkevent);
    ASSERT_TRUE(log.find("Generated Layer vkCreateEvent"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateEvent"));
    log.logger.clear();

    device_disp_table.DestroyEvent(dev.dev, var_vkevent, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyEvent"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyEvent"));
    log.logger.clear();

    VkQueryPoolCreateInfo var_vkquerypoolcreateinfo{VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO};
    VkQueryPool var_vkquerypool{};
    device_disp_table.CreateQueryPool(dev.dev, &var_vkquerypoolcreateinfo, nullptr, &var_vkquerypool);
    ASSERT_TRUE(log.find("Generated Layer vkCreateQueryPool"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateQueryPool"));
    log.logger.clear();

    device_disp_table.DestroyQueryPool(dev.dev, var_vkquerypool, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyQueryPool"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyQueryPool"));
    log.logger.clear();

    VkBufferCreateInfo var_vkbuffercreateinfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    VkBuffer var_vkbuffer{};
    device_disp_table.CreateBuffer(dev.dev, &var_vkbuffercreateinfo, nullptr, &var_vkbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkCreateBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateBuffer"));
    log.logger.clear();

    device_disp_table.DestroyBuffer(dev.dev, var_vkbuffer, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyBuffer"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkDependencyInfo var_vkdependencyinfo{VK_STRUCTURE_TYPE_DEPENDENCY_INFO};
    device_disp_table.CmdSetEvent2KHR(var_vkcommandbuffer, var_vkevent, &var_vkdependencyinfo);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetEvent2KHR") ||
                log.find("Generated Layer vkCmdSetEvent2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetEvent2KHR") ||
                log.find("Generated Driver vkCmdSetEvent2"));
    log.logger.clear();

    VkPipelineStageFlags2 var_vkpipelinestageflags2{};
    device_disp_table.CmdResetEvent2KHR(var_vkcommandbuffer, var_vkevent, var_vkpipelinestageflags2);
    ASSERT_TRUE(log.find("Generated Layer vkCmdResetEvent2KHR") ||
                log.find("Generated Layer vkCmdResetEvent2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdResetEvent2KHR") ||
                log.find("Generated Driver vkCmdResetEvent2"));
    log.logger.clear();

    device_disp_table.CmdWaitEvents2KHR(var_vkcommandbuffer, var_uint32_t, &var_vkevent, &var_vkdependencyinfo);
    ASSERT_TRUE(log.find("Generated Layer vkCmdWaitEvents2KHR") ||
                log.find("Generated Layer vkCmdWaitEvents2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdWaitEvents2KHR") ||
                log.find("Generated Driver vkCmdWaitEvents2"));
    log.logger.clear();

    device_disp_table.CmdPipelineBarrier2KHR(var_vkcommandbuffer, &var_vkdependencyinfo);
    ASSERT_TRUE(log.find("Generated Layer vkCmdPipelineBarrier2KHR") ||
                log.find("Generated Layer vkCmdPipelineBarrier2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdPipelineBarrier2KHR") ||
                log.find("Generated Driver vkCmdPipelineBarrier2"));
    log.logger.clear();

    device_disp_table.CmdWriteTimestamp2KHR(var_vkcommandbuffer, var_vkpipelinestageflags2, var_vkquerypool, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdWriteTimestamp2KHR") ||
                log.find("Generated Layer vkCmdWriteTimestamp2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdWriteTimestamp2KHR") ||
                log.find("Generated Driver vkCmdWriteTimestamp2"));
    log.logger.clear();

    VkDeviceSize var_vkdevicesize{};
    device_disp_table.CmdWriteBufferMarker2AMD(var_vkcommandbuffer, var_vkpipelinestageflags2, var_vkbuffer, var_vkdevicesize, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdWriteBufferMarker2AMD"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdWriteBufferMarker2AMD"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_KHR_synchronization2

// Test for VK_KHR_copy_commands2
TEST(BasicEntrypointTest, KHR_copy_commands2) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_KHR_COPY_COMMANDS_2_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkCopyBufferInfo2 var_vkcopybufferinfo2{VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2};
    device_disp_table.CmdCopyBuffer2KHR(var_vkcommandbuffer, &var_vkcopybufferinfo2);
    ASSERT_TRUE(log.find("Generated Layer vkCmdCopyBuffer2KHR") ||
                log.find("Generated Layer vkCmdCopyBuffer2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdCopyBuffer2KHR") ||
                log.find("Generated Driver vkCmdCopyBuffer2"));
    log.logger.clear();

    VkCopyImageInfo2 var_vkcopyimageinfo2{VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2};
    device_disp_table.CmdCopyImage2KHR(var_vkcommandbuffer, &var_vkcopyimageinfo2);
    ASSERT_TRUE(log.find("Generated Layer vkCmdCopyImage2KHR") ||
                log.find("Generated Layer vkCmdCopyImage2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdCopyImage2KHR") ||
                log.find("Generated Driver vkCmdCopyImage2"));
    log.logger.clear();

    VkCopyBufferToImageInfo2 var_vkcopybuffertoimageinfo2{VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2};
    device_disp_table.CmdCopyBufferToImage2KHR(var_vkcommandbuffer, &var_vkcopybuffertoimageinfo2);
    ASSERT_TRUE(log.find("Generated Layer vkCmdCopyBufferToImage2KHR") ||
                log.find("Generated Layer vkCmdCopyBufferToImage2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdCopyBufferToImage2KHR") ||
                log.find("Generated Driver vkCmdCopyBufferToImage2"));
    log.logger.clear();

    VkCopyImageToBufferInfo2 var_vkcopyimagetobufferinfo2{VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2};
    device_disp_table.CmdCopyImageToBuffer2KHR(var_vkcommandbuffer, &var_vkcopyimagetobufferinfo2);
    ASSERT_TRUE(log.find("Generated Layer vkCmdCopyImageToBuffer2KHR") ||
                log.find("Generated Layer vkCmdCopyImageToBuffer2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdCopyImageToBuffer2KHR") ||
                log.find("Generated Driver vkCmdCopyImageToBuffer2"));
    log.logger.clear();

    VkBlitImageInfo2 var_vkblitimageinfo2{VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2};
    device_disp_table.CmdBlitImage2KHR(var_vkcommandbuffer, &var_vkblitimageinfo2);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBlitImage2KHR") ||
                log.find("Generated Layer vkCmdBlitImage2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBlitImage2KHR") ||
                log.find("Generated Driver vkCmdBlitImage2"));
    log.logger.clear();

    VkResolveImageInfo2 var_vkresolveimageinfo2{VK_STRUCTURE_TYPE_RESOLVE_IMAGE_INFO_2};
    device_disp_table.CmdResolveImage2KHR(var_vkcommandbuffer, &var_vkresolveimageinfo2);
    ASSERT_TRUE(log.find("Generated Layer vkCmdResolveImage2KHR") ||
                log.find("Generated Layer vkCmdResolveImage2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdResolveImage2KHR") ||
                log.find("Generated Driver vkCmdResolveImage2"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_KHR_copy_commands2

// Test for VK_KHR_maintenance4
TEST(BasicEntrypointTest, KHR_maintenance4) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_KHR_MAINTENANCE_4_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkDeviceBufferMemoryRequirements var_vkdevicebuffermemoryrequirements{VK_STRUCTURE_TYPE_DEVICE_BUFFER_MEMORY_REQUIREMENTS};
    VkMemoryRequirements2 var_vkmemoryrequirements2{VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2};
    device_disp_table.GetDeviceBufferMemoryRequirementsKHR(dev.dev, &var_vkdevicebuffermemoryrequirements, &var_vkmemoryrequirements2);
    ASSERT_TRUE(log.find("Generated Layer vkGetDeviceBufferMemoryRequirementsKHR") ||
                log.find("Generated Layer vkGetDeviceBufferMemoryRequirements"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDeviceBufferMemoryRequirementsKHR") ||
                log.find("Generated Driver vkGetDeviceBufferMemoryRequirements"));
    log.logger.clear();

    VkDeviceImageMemoryRequirements var_vkdeviceimagememoryrequirements{VK_STRUCTURE_TYPE_DEVICE_IMAGE_MEMORY_REQUIREMENTS};
    device_disp_table.GetDeviceImageMemoryRequirementsKHR(dev.dev, &var_vkdeviceimagememoryrequirements, &var_vkmemoryrequirements2);
    ASSERT_TRUE(log.find("Generated Layer vkGetDeviceImageMemoryRequirementsKHR") ||
                log.find("Generated Layer vkGetDeviceImageMemoryRequirements"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDeviceImageMemoryRequirementsKHR") ||
                log.find("Generated Driver vkGetDeviceImageMemoryRequirements"));
    log.logger.clear();

    VkSparseImageMemoryRequirements2 var_vksparseimagememoryrequirements2{VK_STRUCTURE_TYPE_SPARSE_IMAGE_MEMORY_REQUIREMENTS_2};
    device_disp_table.GetDeviceImageSparseMemoryRequirementsKHR(dev.dev, &var_vkdeviceimagememoryrequirements, &var_uint32_t, &var_vksparseimagememoryrequirements2);
    ASSERT_TRUE(log.find("Generated Layer vkGetDeviceImageSparseMemoryRequirementsKHR") ||
                log.find("Generated Layer vkGetDeviceImageSparseMemoryRequirements"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDeviceImageSparseMemoryRequirementsKHR") ||
                log.find("Generated Driver vkGetDeviceImageSparseMemoryRequirements"));
    log.logger.clear();

} // Test for VK_KHR_maintenance4

// Test for VK_EXT_debug_report
TEST(BasicEntrypointTest, EXT_debug_report) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_EXT_DEBUG_REPORT_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    VkDebugReportFlagsEXT var_vkdebugreportflagsext{};
    VkDebugReportObjectTypeEXT var_vkdebugreportobjecttypeext{};
    uint64_t var_uint64_t = 1;
    size_t var_size_t{};
    int32_t var_int32_t{};
    char var_char{};
    inst_disp_table.DebugReportMessageEXT(instance, var_vkdebugreportflagsext, var_vkdebugreportobjecttypeext, var_uint64_t, var_size_t, var_int32_t, &var_char, &var_char);
    ASSERT_TRUE(log.find("Generated Layer vkDebugReportMessageEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkDebugReportMessageEXT"));
    log.logger.clear();

} // Test for VK_EXT_debug_report

// Test for VK_EXT_debug_marker
TEST(BasicEntrypointTest, EXT_debug_marker) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_EXT_DEBUG_REPORT_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_EXT_DEBUG_MARKER_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkDebugMarkerObjectTagInfoEXT var_vkdebugmarkerobjecttaginfoext{VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_TAG_INFO_EXT};
    device_disp_table.DebugMarkerSetObjectTagEXT(dev.dev, &var_vkdebugmarkerobjecttaginfoext);
    ASSERT_TRUE(log.find("Generated Layer vkDebugMarkerSetObjectTagEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkDebugMarkerSetObjectTagEXT"));
    log.logger.clear();

    VkDebugMarkerObjectNameInfoEXT var_vkdebugmarkerobjectnameinfoext{VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT};
    device_disp_table.DebugMarkerSetObjectNameEXT(dev.dev, &var_vkdebugmarkerobjectnameinfoext);
    ASSERT_TRUE(log.find("Generated Layer vkDebugMarkerSetObjectNameEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkDebugMarkerSetObjectNameEXT"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkDebugMarkerMarkerInfoEXT var_vkdebugmarkermarkerinfoext{VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT};
    device_disp_table.CmdDebugMarkerBeginEXT(var_vkcommandbuffer, &var_vkdebugmarkermarkerinfoext);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDebugMarkerBeginEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDebugMarkerBeginEXT"));
    log.logger.clear();

    device_disp_table.CmdDebugMarkerEndEXT(var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDebugMarkerEndEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDebugMarkerEndEXT"));
    log.logger.clear();

    device_disp_table.CmdDebugMarkerInsertEXT(var_vkcommandbuffer, &var_vkdebugmarkermarkerinfoext);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDebugMarkerInsertEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDebugMarkerInsertEXT"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_EXT_debug_marker

// Test for VK_EXT_transform_feedback
TEST(BasicEntrypointTest, EXT_transform_feedback) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkQueryPoolCreateInfo var_vkquerypoolcreateinfo{VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO};
    VkQueryPool var_vkquerypool{};
    device_disp_table.CreateQueryPool(dev.dev, &var_vkquerypoolcreateinfo, nullptr, &var_vkquerypool);
    ASSERT_TRUE(log.find("Generated Layer vkCreateQueryPool"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateQueryPool"));
    log.logger.clear();

    device_disp_table.DestroyQueryPool(dev.dev, var_vkquerypool, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyQueryPool"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyQueryPool"));
    log.logger.clear();

    VkBufferCreateInfo var_vkbuffercreateinfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    VkBuffer var_vkbuffer{};
    device_disp_table.CreateBuffer(dev.dev, &var_vkbuffercreateinfo, nullptr, &var_vkbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkCreateBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateBuffer"));
    log.logger.clear();

    device_disp_table.DestroyBuffer(dev.dev, var_vkbuffer, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyBuffer"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkDeviceSize var_vkdevicesize{};
    device_disp_table.CmdBindTransformFeedbackBuffersEXT(var_vkcommandbuffer, var_uint32_t, var_uint32_t, &var_vkbuffer, &var_vkdevicesize, &var_vkdevicesize);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBindTransformFeedbackBuffersEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBindTransformFeedbackBuffersEXT"));
    log.logger.clear();

    device_disp_table.CmdBeginTransformFeedbackEXT(var_vkcommandbuffer, var_uint32_t, var_uint32_t, &var_vkbuffer, &var_vkdevicesize);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBeginTransformFeedbackEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBeginTransformFeedbackEXT"));
    log.logger.clear();

    device_disp_table.CmdEndTransformFeedbackEXT(var_vkcommandbuffer, var_uint32_t, var_uint32_t, &var_vkbuffer, &var_vkdevicesize);
    ASSERT_TRUE(log.find("Generated Layer vkCmdEndTransformFeedbackEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdEndTransformFeedbackEXT"));
    log.logger.clear();

    VkQueryControlFlags var_vkquerycontrolflags{};
    device_disp_table.CmdBeginQueryIndexedEXT(var_vkcommandbuffer, var_vkquerypool, var_uint32_t, var_vkquerycontrolflags, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBeginQueryIndexedEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBeginQueryIndexedEXT"));
    log.logger.clear();

    device_disp_table.CmdEndQueryIndexedEXT(var_vkcommandbuffer, var_vkquerypool, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdEndQueryIndexedEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdEndQueryIndexedEXT"));
    log.logger.clear();

    device_disp_table.CmdDrawIndirectByteCountEXT(var_vkcommandbuffer, var_uint32_t, var_uint32_t, var_vkbuffer, var_vkdevicesize, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDrawIndirectByteCountEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDrawIndirectByteCountEXT"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_EXT_transform_feedback

// Test for VK_NVX_binary_import
TEST(BasicEntrypointTest, NVX_binary_import) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_NVX_BINARY_IMPORT_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkCuLaunchInfoNVX var_vkculaunchinfonvx{VK_STRUCTURE_TYPE_CU_LAUNCH_INFO_NVX};
    device_disp_table.CmdCuLaunchKernelNVX(var_vkcommandbuffer, &var_vkculaunchinfonvx);
    ASSERT_TRUE(log.find("Generated Layer vkCmdCuLaunchKernelNVX"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdCuLaunchKernelNVX"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_NVX_binary_import

// Test for VK_NVX_image_view_handle
TEST(BasicEntrypointTest, NVX_image_view_handle) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_NVX_IMAGE_VIEW_HANDLE_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkImageViewHandleInfoNVX var_vkimageviewhandleinfonvx{VK_STRUCTURE_TYPE_IMAGE_VIEW_HANDLE_INFO_NVX};
    device_disp_table.GetImageViewHandleNVX(dev.dev, &var_vkimageviewhandleinfonvx);
    ASSERT_TRUE(log.find("Generated Layer vkGetImageViewHandleNVX"));
    ASSERT_TRUE(log.find("Generated Driver vkGetImageViewHandleNVX"));
    log.logger.clear();

    VkImageViewCreateInfo var_vkimageviewcreateinfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    VkImageView var_vkimageview{};
    device_disp_table.CreateImageView(dev.dev, &var_vkimageviewcreateinfo, nullptr, &var_vkimageview);
    ASSERT_TRUE(log.find("Generated Layer vkCreateImageView"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateImageView"));
    log.logger.clear();

    device_disp_table.DestroyImageView(dev.dev, var_vkimageview, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyImageView"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyImageView"));
    log.logger.clear();

} // Test for VK_NVX_image_view_handle

// Test for VK_AMD_draw_indirect_count
TEST(BasicEntrypointTest, AMD_draw_indirect_count) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_AMD_DRAW_INDIRECT_COUNT_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkBufferCreateInfo var_vkbuffercreateinfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    VkBuffer var_vkbuffer{};
    device_disp_table.CreateBuffer(dev.dev, &var_vkbuffercreateinfo, nullptr, &var_vkbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkCreateBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateBuffer"));
    log.logger.clear();

    device_disp_table.DestroyBuffer(dev.dev, var_vkbuffer, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyBuffer"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkDeviceSize var_vkdevicesize{};
    device_disp_table.CmdDrawIndirectCountAMD(var_vkcommandbuffer, var_vkbuffer, var_vkdevicesize, var_vkbuffer, var_vkdevicesize, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDrawIndirectCountAMD") ||
                log.find("Generated Layer vkCmdDrawIndirectCount"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDrawIndirectCountAMD") ||
                log.find("Generated Driver vkCmdDrawIndirectCount"));
    log.logger.clear();

    device_disp_table.CmdDrawIndexedIndirectCountAMD(var_vkcommandbuffer, var_vkbuffer, var_vkdevicesize, var_vkbuffer, var_vkdevicesize, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDrawIndexedIndirectCountAMD") ||
                log.find("Generated Layer vkCmdDrawIndexedIndirectCount"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDrawIndexedIndirectCountAMD") ||
                log.find("Generated Driver vkCmdDrawIndexedIndirectCount"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_AMD_draw_indirect_count

// Test for VK_AMD_shader_info
TEST(BasicEntrypointTest, AMD_shader_info) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_AMD_SHADER_INFO_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkPipeline var_vkpipeline{};
    VkShaderStageFlagBits var_vkshaderstageflagbits{};
    VkShaderInfoTypeAMD var_vkshaderinfotypeamd{};
    size_t var_size_t{};
    uint64_t big_chunk_of_mem[512][512];

    device_disp_table.GetShaderInfoAMD(dev.dev, var_vkpipeline, var_vkshaderstageflagbits, var_vkshaderinfotypeamd, &var_size_t, reinterpret_cast<void**>(big_chunk_of_mem));
    ASSERT_TRUE(log.find("Generated Layer vkGetShaderInfoAMD"));
    ASSERT_TRUE(log.find("Generated Driver vkGetShaderInfoAMD"));
    log.logger.clear();

    VkPipelineCache var_vkpipelinecache{};
    VkGraphicsPipelineCreateInfo var_vkgraphicspipelinecreateinfo{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    device_disp_table.CreateGraphicsPipelines(dev.dev, var_vkpipelinecache, var_uint32_t, &var_vkgraphicspipelinecreateinfo, nullptr, &var_vkpipeline);
    ASSERT_TRUE(log.find("Generated Layer vkCreateGraphicsPipelines"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateGraphicsPipelines"));
    log.logger.clear();

    device_disp_table.DestroyPipeline(dev.dev, var_vkpipeline, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyPipeline"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyPipeline"));
    log.logger.clear();

} // Test for VK_AMD_shader_info

#ifdef VK_USE_PLATFORM_GGP
// Test for VK_GGP_stream_descriptor_surface
TEST(BasicEntrypointTest, GGP_stream_descriptor_surface) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_GGP_STREAM_DESCRIPTOR_SURFACE_EXTENSION_NAME}, {VK_KHR_SURFACE_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    VkStreamDescriptorSurfaceCreateInfoGGP var_vkstreamdescriptorsurfacecreateinfoggp{VK_STRUCTURE_TYPE_STREAM_DESCRIPTOR_SURFACE_CREATE_INFO_GGP};
    VkSurfaceKHR var_vksurfacekhr{};
    inst_disp_table.CreateStreamDescriptorSurfaceGGP(instance, &var_vkstreamdescriptorsurfacecreateinfoggp, nullptr, &var_vksurfacekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateStreamDescriptorSurfaceGGP"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateStreamDescriptorSurfaceGGP"));
    log.logger.clear();

    inst_disp_table.DestroySurfaceKHR(instance, var_vksurfacekhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySurfaceKHR"));
    log.logger.clear();

} // Test for VK_GGP_stream_descriptor_surface
#endif // VK_USE_PLATFORM_GGP

// Test for VK_NV_external_memory_capabilities
TEST(BasicEntrypointTest, NV_external_memory_capabilities) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_NV_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkFormat var_vkformat{};
    VkImageType var_vkimagetype{};
    VkImageTiling var_vkimagetiling{};
    VkImageUsageFlags var_vkimageusageflags{};
    VkImageCreateFlags var_vkimagecreateflags{};
    VkExternalMemoryHandleTypeFlagsNV var_vkexternalmemoryhandletypeflagsnv{};
    VkExternalImageFormatPropertiesNV var_vkexternalimageformatpropertiesnv{};
    inst_disp_table.GetPhysicalDeviceExternalImageFormatPropertiesNV(var_vkphysicaldevice, var_vkformat, var_vkimagetype, var_vkimagetiling, var_vkimageusageflags, var_vkimagecreateflags, var_vkexternalmemoryhandletypeflagsnv, &var_vkexternalimageformatpropertiesnv);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceExternalImageFormatPropertiesNV"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceExternalImageFormatPropertiesNV"));
    log.logger.clear();

} // Test for VK_NV_external_memory_capabilities

#ifdef VK_USE_PLATFORM_WIN32_KHR
// Test for VK_NV_external_memory_win32
TEST(BasicEntrypointTest, NV_external_memory_win32) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_NV_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME}, {VK_NV_EXTERNAL_MEMORY_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkDeviceMemory var_vkdevicememory{};
    VkExternalMemoryHandleTypeFlagsNV var_vkexternalmemoryhandletypeflagsnv{};
    HANDLE var_handle{};
    device_disp_table.GetMemoryWin32HandleNV(dev.dev, var_vkdevicememory, var_vkexternalmemoryhandletypeflagsnv, &var_handle);
    ASSERT_TRUE(log.find("Generated Layer vkGetMemoryWin32HandleNV"));
    ASSERT_TRUE(log.find("Generated Driver vkGetMemoryWin32HandleNV"));
    log.logger.clear();

    VkMemoryAllocateInfo var_vkmemoryallocateinfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    device_disp_table.AllocateMemory(dev.dev, &var_vkmemoryallocateinfo, nullptr, &var_vkdevicememory);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateMemory"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateMemory"));
    log.logger.clear();

    device_disp_table.FreeMemory(dev.dev, var_vkdevicememory, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkFreeMemory"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeMemory"));
    log.logger.clear();

} // Test for VK_NV_external_memory_win32
#endif // VK_USE_PLATFORM_WIN32_KHR

#ifdef VK_USE_PLATFORM_VI_NN
// Test for VK_NN_vi_surface
TEST(BasicEntrypointTest, NN_vi_surface) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_NN_VI_SURFACE_EXTENSION_NAME}, {VK_KHR_SURFACE_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    VkViSurfaceCreateInfoNN var_vkvisurfacecreateinfonn{VK_STRUCTURE_TYPE_VI_SURFACE_CREATE_INFO_NN};
    VkSurfaceKHR var_vksurfacekhr{};
    inst_disp_table.CreateViSurfaceNN(instance, &var_vkvisurfacecreateinfonn, nullptr, &var_vksurfacekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateViSurfaceNN"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateViSurfaceNN"));
    log.logger.clear();

    inst_disp_table.DestroySurfaceKHR(instance, var_vksurfacekhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySurfaceKHR"));
    log.logger.clear();

} // Test for VK_NN_vi_surface
#endif // VK_USE_PLATFORM_VI_NN

// Test for VK_EXT_conditional_rendering
TEST(BasicEntrypointTest, EXT_conditional_rendering) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_EXT_CONDITIONAL_RENDERING_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkConditionalRenderingBeginInfoEXT var_vkconditionalrenderingbegininfoext{VK_STRUCTURE_TYPE_CONDITIONAL_RENDERING_BEGIN_INFO_EXT};
    device_disp_table.CmdBeginConditionalRenderingEXT(var_vkcommandbuffer, &var_vkconditionalrenderingbegininfoext);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBeginConditionalRenderingEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBeginConditionalRenderingEXT"));
    log.logger.clear();

    device_disp_table.CmdEndConditionalRenderingEXT(var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkCmdEndConditionalRenderingEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdEndConditionalRenderingEXT"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_EXT_conditional_rendering

// Test for VK_NV_clip_space_w_scaling
TEST(BasicEntrypointTest, NV_clip_space_w_scaling) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_NV_CLIP_SPACE_W_SCALING_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkViewportWScalingNV var_vkviewportwscalingnv{};
    device_disp_table.CmdSetViewportWScalingNV(var_vkcommandbuffer, var_uint32_t, var_uint32_t, &var_vkviewportwscalingnv);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetViewportWScalingNV"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetViewportWScalingNV"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_NV_clip_space_w_scaling

// Test for VK_EXT_direct_mode_display
TEST(BasicEntrypointTest, EXT_direct_mode_display) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME}, {VK_KHR_DISPLAY_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkDisplayKHR var_vkdisplaykhr{};
    inst_disp_table.ReleaseDisplayEXT(var_vkphysicaldevice, var_vkdisplaykhr);
    ASSERT_TRUE(log.find("Generated Layer vkReleaseDisplayEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkReleaseDisplayEXT"));
    log.logger.clear();

} // Test for VK_EXT_direct_mode_display

#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
// Test for VK_EXT_acquire_xlib_display
TEST(BasicEntrypointTest, EXT_acquire_xlib_display) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_EXT_ACQUIRE_XLIB_DISPLAY_EXTENSION_NAME}, {VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    uint64_t big_chunk_of_mem[512][512];

    VkDisplayKHR var_vkdisplaykhr{};
    inst_disp_table.AcquireXlibDisplayEXT(var_vkphysicaldevice, reinterpret_cast<Display*>(&big_chunk_of_mem), var_vkdisplaykhr);
    ASSERT_TRUE(log.find("Generated Layer vkAcquireXlibDisplayEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkAcquireXlibDisplayEXT"));
    log.logger.clear();

    RROutput var_rroutput{};
    inst_disp_table.GetRandROutputDisplayEXT(var_vkphysicaldevice, reinterpret_cast<Display*>(&big_chunk_of_mem), var_rroutput, &var_vkdisplaykhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetRandROutputDisplayEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkGetRandROutputDisplayEXT"));
    log.logger.clear();

} // Test for VK_EXT_acquire_xlib_display
#endif // VK_USE_PLATFORM_XLIB_XRANDR_EXT

// Test for VK_EXT_display_surface_counter
TEST(BasicEntrypointTest, EXT_display_surface_counter) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME}, {VK_KHR_DISPLAY_EXTENSION_NAME}, {VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME}, {VK_KHR_SURFACE_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    VkHeadlessSurfaceCreateInfoEXT var_vkheadlesssurfacecreateinfoext{VK_STRUCTURE_TYPE_HEADLESS_SURFACE_CREATE_INFO_EXT};
    VkSurfaceKHR var_vksurfacekhr{};
    inst_disp_table.CreateHeadlessSurfaceEXT(instance, &var_vkheadlesssurfacecreateinfoext, nullptr, &var_vksurfacekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateHeadlessSurfaceEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateHeadlessSurfaceEXT"));
    log.logger.clear();

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkSurfaceCapabilities2EXT var_vksurfacecapabilities2ext{VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_EXT};
    inst_disp_table.GetPhysicalDeviceSurfaceCapabilities2EXT(var_vkphysicaldevice, var_vksurfacekhr, &var_vksurfacecapabilities2ext);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceSurfaceCapabilities2EXT"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceSurfaceCapabilities2EXT"));
    log.logger.clear();

    inst_disp_table.DestroySurfaceKHR(instance, var_vksurfacekhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySurfaceKHR"));
    log.logger.clear();

} // Test for VK_EXT_display_surface_counter

// Test for VK_EXT_display_control
TEST(BasicEntrypointTest, EXT_display_control) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_EXT_DISPLAY_CONTROL_EXTENSION_NAME}, {VK_KHR_SWAPCHAIN_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkDisplayKHR var_vkdisplaykhr{};
    VkDisplayPowerInfoEXT var_vkdisplaypowerinfoext{VK_STRUCTURE_TYPE_DISPLAY_POWER_INFO_EXT};
    device_disp_table.DisplayPowerControlEXT(dev.dev, var_vkdisplaykhr, &var_vkdisplaypowerinfoext);
    ASSERT_TRUE(log.find("Generated Layer vkDisplayPowerControlEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkDisplayPowerControlEXT"));
    log.logger.clear();

    VkSwapchainKHR var_vkswapchainkhr{};
    VkSurfaceCounterFlagBitsEXT var_vksurfacecounterflagbitsext{};
    uint64_t var_uint64_t = 1;
    device_disp_table.GetSwapchainCounterEXT(dev.dev, var_vkswapchainkhr, var_vksurfacecounterflagbitsext, &var_uint64_t);
    ASSERT_TRUE(log.find("Generated Layer vkGetSwapchainCounterEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkGetSwapchainCounterEXT"));
    log.logger.clear();

} // Test for VK_EXT_display_control

// Test for VK_GOOGLE_display_timing
TEST(BasicEntrypointTest, GOOGLE_display_timing) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_GOOGLE_DISPLAY_TIMING_EXTENSION_NAME}, {VK_KHR_SWAPCHAIN_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkSwapchainKHR var_vkswapchainkhr{};
    VkRefreshCycleDurationGOOGLE var_vkrefreshcycledurationgoogle{};
    device_disp_table.GetRefreshCycleDurationGOOGLE(dev.dev, var_vkswapchainkhr, &var_vkrefreshcycledurationgoogle);
    ASSERT_TRUE(log.find("Generated Layer vkGetRefreshCycleDurationGOOGLE"));
    ASSERT_TRUE(log.find("Generated Driver vkGetRefreshCycleDurationGOOGLE"));
    log.logger.clear();

    VkPastPresentationTimingGOOGLE var_vkpastpresentationtiminggoogle{};
    device_disp_table.GetPastPresentationTimingGOOGLE(dev.dev, var_vkswapchainkhr, &var_uint32_t, &var_vkpastpresentationtiminggoogle);
    ASSERT_TRUE(log.find("Generated Layer vkGetPastPresentationTimingGOOGLE"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPastPresentationTimingGOOGLE"));
    log.logger.clear();

} // Test for VK_GOOGLE_display_timing

// Test for VK_EXT_discard_rectangles
TEST(BasicEntrypointTest, EXT_discard_rectangles) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_EXT_DISCARD_RECTANGLES_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkRect2D var_vkrect2d{};
    device_disp_table.CmdSetDiscardRectangleEXT(var_vkcommandbuffer, var_uint32_t, var_uint32_t, &var_vkrect2d);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetDiscardRectangleEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetDiscardRectangleEXT"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_EXT_discard_rectangles

// Test for VK_EXT_hdr_metadata
TEST(BasicEntrypointTest, EXT_hdr_metadata) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_EXT_HDR_METADATA_EXTENSION_NAME}, {VK_KHR_SWAPCHAIN_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkSwapchainKHR var_vkswapchainkhr{};
    VkHdrMetadataEXT var_vkhdrmetadataext{VK_STRUCTURE_TYPE_HDR_METADATA_EXT};
    device_disp_table.SetHdrMetadataEXT(dev.dev, var_uint32_t, &var_vkswapchainkhr, &var_vkhdrmetadataext);
    ASSERT_TRUE(log.find("Generated Layer vkSetHdrMetadataEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkSetHdrMetadataEXT"));
    log.logger.clear();

} // Test for VK_EXT_hdr_metadata

#ifdef VK_USE_PLATFORM_IOS_MVK
// Test for VK_MVK_ios_surface
TEST(BasicEntrypointTest, MVK_ios_surface) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_MVK_IOS_SURFACE_EXTENSION_NAME}, {VK_KHR_SURFACE_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    VkIOSSurfaceCreateInfoMVK var_vkiossurfacecreateinfomvk{VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK};
    VkSurfaceKHR var_vksurfacekhr{};
    inst_disp_table.CreateIOSSurfaceMVK(instance, &var_vkiossurfacecreateinfomvk, nullptr, &var_vksurfacekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateIOSSurfaceMVK"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateIOSSurfaceMVK"));
    log.logger.clear();

    inst_disp_table.DestroySurfaceKHR(instance, var_vksurfacekhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySurfaceKHR"));
    log.logger.clear();

} // Test for VK_MVK_ios_surface
#endif // VK_USE_PLATFORM_IOS_MVK

#ifdef VK_USE_PLATFORM_MACOS_MVK
// Test for VK_MVK_macos_surface
TEST(BasicEntrypointTest, MVK_macos_surface) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_MVK_MACOS_SURFACE_EXTENSION_NAME}, {VK_KHR_SURFACE_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    VkMacOSSurfaceCreateInfoMVK var_vkmacossurfacecreateinfomvk{VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK};
    VkSurfaceKHR var_vksurfacekhr{};
    inst_disp_table.CreateMacOSSurfaceMVK(instance, &var_vkmacossurfacecreateinfomvk, nullptr, &var_vksurfacekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateMacOSSurfaceMVK"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateMacOSSurfaceMVK"));
    log.logger.clear();

    inst_disp_table.DestroySurfaceKHR(instance, var_vksurfacekhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySurfaceKHR"));
    log.logger.clear();

} // Test for VK_MVK_macos_surface
#endif // VK_USE_PLATFORM_MACOS_MVK

// Test for VK_EXT_sample_locations
TEST(BasicEntrypointTest, EXT_sample_locations) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkSampleCountFlagBits var_vksamplecountflagbits{};
    VkMultisamplePropertiesEXT var_vkmultisamplepropertiesext{VK_STRUCTURE_TYPE_MULTISAMPLE_PROPERTIES_EXT};
    inst_disp_table.GetPhysicalDeviceMultisamplePropertiesEXT(var_vkphysicaldevice, var_vksamplecountflagbits, &var_vkmultisamplepropertiesext);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceMultisamplePropertiesEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceMultisamplePropertiesEXT"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkSampleLocationsInfoEXT var_vksamplelocationsinfoext{VK_STRUCTURE_TYPE_SAMPLE_LOCATIONS_INFO_EXT};
    device_disp_table.CmdSetSampleLocationsEXT(var_vkcommandbuffer, &var_vksamplelocationsinfoext);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetSampleLocationsEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetSampleLocationsEXT"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_EXT_sample_locations

// Test for VK_EXT_image_drm_format_modifier
TEST(BasicEntrypointTest, EXT_image_drm_format_modifier) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_EXT_IMAGE_DRM_FORMAT_MODIFIER_EXTENSION_NAME}, {VK_KHR_BIND_MEMORY_2_EXTENSION_NAME}, {VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME}, {VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkImageCreateInfo var_vkimagecreateinfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    VkImage var_vkimage{};
    device_disp_table.CreateImage(dev.dev, &var_vkimagecreateinfo, nullptr, &var_vkimage);
    ASSERT_TRUE(log.find("Generated Layer vkCreateImage"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateImage"));
    log.logger.clear();

    device_disp_table.DestroyImage(dev.dev, var_vkimage, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyImage"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyImage"));
    log.logger.clear();

} // Test for VK_EXT_image_drm_format_modifier

// Test for VK_EXT_validation_cache
TEST(BasicEntrypointTest, EXT_validation_cache) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_EXT_VALIDATION_CACHE_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkValidationCacheEXT var_vkvalidationcacheext{};
    device_disp_table.MergeValidationCachesEXT(dev.dev, var_vkvalidationcacheext, var_uint32_t, &var_vkvalidationcacheext);
    ASSERT_TRUE(log.find("Generated Layer vkMergeValidationCachesEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkMergeValidationCachesEXT"));
    log.logger.clear();

    size_t var_size_t{};
    uint64_t big_chunk_of_mem[512][512];

    device_disp_table.GetValidationCacheDataEXT(dev.dev, var_vkvalidationcacheext, &var_size_t, reinterpret_cast<void**>(big_chunk_of_mem));
    ASSERT_TRUE(log.find("Generated Layer vkGetValidationCacheDataEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkGetValidationCacheDataEXT"));
    log.logger.clear();

    VkValidationCacheCreateInfoEXT var_vkvalidationcachecreateinfoext{VK_STRUCTURE_TYPE_VALIDATION_CACHE_CREATE_INFO_EXT};
    device_disp_table.CreateValidationCacheEXT(dev.dev, &var_vkvalidationcachecreateinfoext, nullptr, &var_vkvalidationcacheext);
    ASSERT_TRUE(log.find("Generated Layer vkCreateValidationCacheEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateValidationCacheEXT"));
    log.logger.clear();

    device_disp_table.DestroyValidationCacheEXT(dev.dev, var_vkvalidationcacheext, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyValidationCacheEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyValidationCacheEXT"));
    log.logger.clear();

} // Test for VK_EXT_validation_cache

// Test for VK_NV_shading_rate_image
TEST(BasicEntrypointTest, NV_shading_rate_image) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_NV_SHADING_RATE_IMAGE_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkImageViewCreateInfo var_vkimageviewcreateinfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    VkImageView var_vkimageview{};
    device_disp_table.CreateImageView(dev.dev, &var_vkimageviewcreateinfo, nullptr, &var_vkimageview);
    ASSERT_TRUE(log.find("Generated Layer vkCreateImageView"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateImageView"));
    log.logger.clear();

    device_disp_table.DestroyImageView(dev.dev, var_vkimageview, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyImageView"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyImageView"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkImageLayout var_vkimagelayout{};
    device_disp_table.CmdBindShadingRateImageNV(var_vkcommandbuffer, var_vkimageview, var_vkimagelayout);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBindShadingRateImageNV"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBindShadingRateImageNV"));
    log.logger.clear();

    VkShadingRatePaletteNV var_vkshadingratepalettenv{};
    device_disp_table.CmdSetViewportShadingRatePaletteNV(var_vkcommandbuffer, var_uint32_t, var_uint32_t, &var_vkshadingratepalettenv);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetViewportShadingRatePaletteNV"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetViewportShadingRatePaletteNV"));
    log.logger.clear();

    VkCoarseSampleOrderTypeNV var_vkcoarsesampleordertypenv{};
    VkCoarseSampleOrderCustomNV var_vkcoarsesampleordercustomnv{};
    device_disp_table.CmdSetCoarseSampleOrderNV(var_vkcommandbuffer, var_vkcoarsesampleordertypenv, var_uint32_t, &var_vkcoarsesampleordercustomnv);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetCoarseSampleOrderNV"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetCoarseSampleOrderNV"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_NV_shading_rate_image

// Test for VK_NV_ray_tracing
TEST(BasicEntrypointTest, NV_ray_tracing) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_NV_RAY_TRACING_EXTENSION_NAME}, {VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkAccelerationStructureMemoryRequirementsInfoNV var_vkaccelerationstructurememoryrequirementsinfonv{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_NV};
    VkMemoryRequirements2KHR var_vkmemoryrequirements2khr{};
    device_disp_table.GetAccelerationStructureMemoryRequirementsNV(dev.dev, &var_vkaccelerationstructurememoryrequirementsinfonv, &var_vkmemoryrequirements2khr);
    ASSERT_TRUE(log.find("Generated Layer vkGetAccelerationStructureMemoryRequirementsNV"));
    ASSERT_TRUE(log.find("Generated Driver vkGetAccelerationStructureMemoryRequirementsNV"));
    log.logger.clear();

    VkBindAccelerationStructureMemoryInfoNV var_vkbindaccelerationstructurememoryinfonv{VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_NV};
    device_disp_table.BindAccelerationStructureMemoryNV(dev.dev, var_uint32_t, &var_vkbindaccelerationstructurememoryinfonv);
    ASSERT_TRUE(log.find("Generated Layer vkBindAccelerationStructureMemoryNV"));
    ASSERT_TRUE(log.find("Generated Driver vkBindAccelerationStructureMemoryNV"));
    log.logger.clear();

    VkPipeline var_vkpipeline{};
    size_t var_size_t{};
    uint64_t big_chunk_of_mem[512][512];

    device_disp_table.GetRayTracingShaderGroupHandlesNV(dev.dev, var_vkpipeline, var_uint32_t, var_uint32_t, var_size_t, reinterpret_cast<void**>(big_chunk_of_mem));
    ASSERT_TRUE(log.find("Generated Layer vkGetRayTracingShaderGroupHandlesNV") ||
                log.find("Generated Layer vkGetRayTracingShaderGroupHandlesKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetRayTracingShaderGroupHandlesNV") ||
                log.find("Generated Driver vkGetRayTracingShaderGroupHandlesKHR"));
    log.logger.clear();

    VkAccelerationStructureNV var_vkaccelerationstructurenv{};
    device_disp_table.GetAccelerationStructureHandleNV(dev.dev, var_vkaccelerationstructurenv, var_size_t, reinterpret_cast<void**>(big_chunk_of_mem));
    ASSERT_TRUE(log.find("Generated Layer vkGetAccelerationStructureHandleNV"));
    ASSERT_TRUE(log.find("Generated Driver vkGetAccelerationStructureHandleNV"));
    log.logger.clear();

    device_disp_table.CompileDeferredNV(dev.dev, var_vkpipeline, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCompileDeferredNV"));
    ASSERT_TRUE(log.find("Generated Driver vkCompileDeferredNV"));
    log.logger.clear();

    VkQueryPoolCreateInfo var_vkquerypoolcreateinfo{VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO};
    VkQueryPool var_vkquerypool{};
    device_disp_table.CreateQueryPool(dev.dev, &var_vkquerypoolcreateinfo, nullptr, &var_vkquerypool);
    ASSERT_TRUE(log.find("Generated Layer vkCreateQueryPool"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateQueryPool"));
    log.logger.clear();

    device_disp_table.DestroyQueryPool(dev.dev, var_vkquerypool, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyQueryPool"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyQueryPool"));
    log.logger.clear();

    VkBufferCreateInfo var_vkbuffercreateinfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    VkBuffer var_vkbuffer{};
    device_disp_table.CreateBuffer(dev.dev, &var_vkbuffercreateinfo, nullptr, &var_vkbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkCreateBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateBuffer"));
    log.logger.clear();

    device_disp_table.DestroyBuffer(dev.dev, var_vkbuffer, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyBuffer"));
    log.logger.clear();

    VkPipelineCacheCreateInfo var_vkpipelinecachecreateinfo{VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO};
    VkPipelineCache var_vkpipelinecache{};
    device_disp_table.CreatePipelineCache(dev.dev, &var_vkpipelinecachecreateinfo, nullptr, &var_vkpipelinecache);
    ASSERT_TRUE(log.find("Generated Layer vkCreatePipelineCache"));
    ASSERT_TRUE(log.find("Generated Driver vkCreatePipelineCache"));
    log.logger.clear();

    device_disp_table.DestroyPipelineCache(dev.dev, var_vkpipelinecache, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyPipelineCache"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyPipelineCache"));
    log.logger.clear();

    VkGraphicsPipelineCreateInfo var_vkgraphicspipelinecreateinfo{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    device_disp_table.CreateGraphicsPipelines(dev.dev, var_vkpipelinecache, var_uint32_t, &var_vkgraphicspipelinecreateinfo, nullptr, &var_vkpipeline);
    ASSERT_TRUE(log.find("Generated Layer vkCreateGraphicsPipelines"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateGraphicsPipelines"));
    log.logger.clear();

    device_disp_table.DestroyPipeline(dev.dev, var_vkpipeline, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyPipeline"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyPipeline"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkAccelerationStructureInfoNV var_vkaccelerationstructureinfonv{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_INFO_NV};
    VkDeviceSize var_vkdevicesize{};
    VkBool32 var_vkbool32{};
    device_disp_table.CmdBuildAccelerationStructureNV(var_vkcommandbuffer, &var_vkaccelerationstructureinfonv, var_vkbuffer, var_vkdevicesize, var_vkbool32, var_vkaccelerationstructurenv, var_vkaccelerationstructurenv, var_vkbuffer, var_vkdevicesize);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBuildAccelerationStructureNV"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBuildAccelerationStructureNV"));
    log.logger.clear();

    VkCopyAccelerationStructureModeKHR var_vkcopyaccelerationstructuremodekhr{};
    device_disp_table.CmdCopyAccelerationStructureNV(var_vkcommandbuffer, var_vkaccelerationstructurenv, var_vkaccelerationstructurenv, var_vkcopyaccelerationstructuremodekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCmdCopyAccelerationStructureNV"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdCopyAccelerationStructureNV"));
    log.logger.clear();

    device_disp_table.CmdTraceRaysNV(var_vkcommandbuffer, var_vkbuffer, var_vkdevicesize, var_vkbuffer, var_vkdevicesize, var_vkdevicesize, var_vkbuffer, var_vkdevicesize, var_vkdevicesize, var_vkbuffer, var_vkdevicesize, var_vkdevicesize, var_uint32_t, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdTraceRaysNV"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdTraceRaysNV"));
    log.logger.clear();

    VkQueryType var_vkquerytype{};
    device_disp_table.CmdWriteAccelerationStructuresPropertiesNV(var_vkcommandbuffer, var_uint32_t, &var_vkaccelerationstructurenv, var_vkquerytype, var_vkquerypool, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdWriteAccelerationStructuresPropertiesNV"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdWriteAccelerationStructuresPropertiesNV"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

    VkAccelerationStructureCreateInfoNV var_vkaccelerationstructurecreateinfonv{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV};
    device_disp_table.CreateAccelerationStructureNV(dev.dev, &var_vkaccelerationstructurecreateinfonv, nullptr, &var_vkaccelerationstructurenv);
    ASSERT_TRUE(log.find("Generated Layer vkCreateAccelerationStructureNV"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateAccelerationStructureNV"));
    log.logger.clear();

    device_disp_table.DestroyAccelerationStructureNV(dev.dev, var_vkaccelerationstructurenv, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyAccelerationStructureNV"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyAccelerationStructureNV"));
    log.logger.clear();

} // Test for VK_NV_ray_tracing

// Test for VK_EXT_external_memory_host
TEST(BasicEntrypointTest, EXT_external_memory_host) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_EXT_EXTERNAL_MEMORY_HOST_EXTENSION_NAME}, {VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkExternalMemoryHandleTypeFlagBits var_vkexternalmemoryhandletypeflagbits{};
    uint64_t big_chunk_of_mem[512][512];

    VkMemoryHostPointerPropertiesEXT var_vkmemoryhostpointerpropertiesext{VK_STRUCTURE_TYPE_MEMORY_HOST_POINTER_PROPERTIES_EXT};
    device_disp_table.GetMemoryHostPointerPropertiesEXT(dev.dev, var_vkexternalmemoryhandletypeflagbits, reinterpret_cast<void**>(big_chunk_of_mem), &var_vkmemoryhostpointerpropertiesext);
    ASSERT_TRUE(log.find("Generated Layer vkGetMemoryHostPointerPropertiesEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkGetMemoryHostPointerPropertiesEXT"));
    log.logger.clear();

} // Test for VK_EXT_external_memory_host

// Test for VK_AMD_buffer_marker
TEST(BasicEntrypointTest, AMD_buffer_marker) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_AMD_BUFFER_MARKER_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkBufferCreateInfo var_vkbuffercreateinfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    VkBuffer var_vkbuffer{};
    device_disp_table.CreateBuffer(dev.dev, &var_vkbuffercreateinfo, nullptr, &var_vkbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkCreateBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateBuffer"));
    log.logger.clear();

    device_disp_table.DestroyBuffer(dev.dev, var_vkbuffer, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyBuffer"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkPipelineStageFlagBits var_vkpipelinestageflagbits{};
    VkDeviceSize var_vkdevicesize{};
    device_disp_table.CmdWriteBufferMarkerAMD(var_vkcommandbuffer, var_vkpipelinestageflagbits, var_vkbuffer, var_vkdevicesize, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdWriteBufferMarkerAMD"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdWriteBufferMarkerAMD"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_AMD_buffer_marker

// Test for VK_EXT_calibrated_timestamps
TEST(BasicEntrypointTest, EXT_calibrated_timestamps) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkTimeDomainEXT var_vktimedomainext{};
    inst_disp_table.GetPhysicalDeviceCalibrateableTimeDomainsEXT(var_vkphysicaldevice, &var_uint32_t, &var_vktimedomainext);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceCalibrateableTimeDomainsEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceCalibrateableTimeDomainsEXT"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkCalibratedTimestampInfoEXT var_vkcalibratedtimestampinfoext{VK_STRUCTURE_TYPE_CALIBRATED_TIMESTAMP_INFO_EXT};
    uint64_t var_uint64_t = 1;
    device_disp_table.GetCalibratedTimestampsEXT(dev.dev, var_uint32_t, &var_vkcalibratedtimestampinfoext, &var_uint64_t, &var_uint64_t);
    ASSERT_TRUE(log.find("Generated Layer vkGetCalibratedTimestampsEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkGetCalibratedTimestampsEXT"));
    log.logger.clear();

} // Test for VK_EXT_calibrated_timestamps

// Test for VK_NV_mesh_shader
TEST(BasicEntrypointTest, NV_mesh_shader) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_NV_MESH_SHADER_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkBufferCreateInfo var_vkbuffercreateinfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    VkBuffer var_vkbuffer{};
    device_disp_table.CreateBuffer(dev.dev, &var_vkbuffercreateinfo, nullptr, &var_vkbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkCreateBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateBuffer"));
    log.logger.clear();

    device_disp_table.DestroyBuffer(dev.dev, var_vkbuffer, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyBuffer"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    device_disp_table.CmdDrawMeshTasksNV(var_vkcommandbuffer, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDrawMeshTasksNV"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDrawMeshTasksNV"));
    log.logger.clear();

    VkDeviceSize var_vkdevicesize{};
    device_disp_table.CmdDrawMeshTasksIndirectNV(var_vkcommandbuffer, var_vkbuffer, var_vkdevicesize, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDrawMeshTasksIndirectNV"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDrawMeshTasksIndirectNV"));
    log.logger.clear();

    device_disp_table.CmdDrawMeshTasksIndirectCountNV(var_vkcommandbuffer, var_vkbuffer, var_vkdevicesize, var_vkbuffer, var_vkdevicesize, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDrawMeshTasksIndirectCountNV"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDrawMeshTasksIndirectCountNV"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_NV_mesh_shader

// Test for VK_NV_scissor_exclusive
TEST(BasicEntrypointTest, NV_scissor_exclusive) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_NV_SCISSOR_EXCLUSIVE_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkRect2D var_vkrect2d{};
    device_disp_table.CmdSetExclusiveScissorNV(var_vkcommandbuffer, var_uint32_t, var_uint32_t, &var_vkrect2d);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetExclusiveScissorNV"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetExclusiveScissorNV"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_NV_scissor_exclusive

// Test for VK_NV_device_diagnostic_checkpoints
TEST(BasicEntrypointTest, NV_device_diagnostic_checkpoints) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkDeviceQueueInfo2 var_vkdevicequeueinfo2{VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2};
    VkQueue var_vkqueue{};
    device_disp_table.GetDeviceQueue2(dev.dev, &var_vkdevicequeueinfo2, &var_vkqueue);
    ASSERT_TRUE(log.find("Generated Layer vkGetDeviceQueue2"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDeviceQueue2"));
    log.logger.clear();

    VkCheckpointDataNV var_vkcheckpointdatanv{VK_STRUCTURE_TYPE_CHECKPOINT_DATA_NV};
    device_disp_table.GetQueueCheckpointDataNV(var_vkqueue, &var_uint32_t, &var_vkcheckpointdatanv);
    ASSERT_TRUE(log.find("Generated Layer vkGetQueueCheckpointDataNV"));
    ASSERT_TRUE(log.find("Generated Driver vkGetQueueCheckpointDataNV"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    uint64_t big_chunk_of_mem[512][512];

    device_disp_table.CmdSetCheckpointNV(var_vkcommandbuffer, reinterpret_cast<void**>(big_chunk_of_mem));
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetCheckpointNV"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetCheckpointNV"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_NV_device_diagnostic_checkpoints

// Test for VK_INTEL_performance_query
TEST(BasicEntrypointTest, INTEL_performance_query) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_INTEL_PERFORMANCE_QUERY_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkInitializePerformanceApiInfoINTEL var_vkinitializeperformanceapiinfointel{VK_STRUCTURE_TYPE_INITIALIZE_PERFORMANCE_API_INFO_INTEL};
    device_disp_table.InitializePerformanceApiINTEL(dev.dev, &var_vkinitializeperformanceapiinfointel);
    ASSERT_TRUE(log.find("Generated Layer vkInitializePerformanceApiINTEL"));
    ASSERT_TRUE(log.find("Generated Driver vkInitializePerformanceApiINTEL"));
    log.logger.clear();

    device_disp_table.UninitializePerformanceApiINTEL(dev.dev);
    ASSERT_TRUE(log.find("Generated Layer vkUninitializePerformanceApiINTEL"));
    ASSERT_TRUE(log.find("Generated Driver vkUninitializePerformanceApiINTEL"));
    log.logger.clear();

    VkPerformanceParameterTypeINTEL var_vkperformanceparametertypeintel{};
    VkPerformanceValueINTEL var_vkperformancevalueintel{};
    device_disp_table.GetPerformanceParameterINTEL(dev.dev, var_vkperformanceparametertypeintel, &var_vkperformancevalueintel);
    ASSERT_TRUE(log.find("Generated Layer vkGetPerformanceParameterINTEL"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPerformanceParameterINTEL"));
    log.logger.clear();

    VkDeviceQueueInfo2 var_vkdevicequeueinfo2{VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2};
    VkQueue var_vkqueue{};
    device_disp_table.GetDeviceQueue2(dev.dev, &var_vkdevicequeueinfo2, &var_vkqueue);
    ASSERT_TRUE(log.find("Generated Layer vkGetDeviceQueue2"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDeviceQueue2"));
    log.logger.clear();

    VkPerformanceConfigurationINTEL var_vkperformanceconfigurationintel{};
    device_disp_table.QueueSetPerformanceConfigurationINTEL(var_vkqueue, var_vkperformanceconfigurationintel);
    ASSERT_TRUE(log.find("Generated Layer vkQueueSetPerformanceConfigurationINTEL"));
    ASSERT_TRUE(log.find("Generated Driver vkQueueSetPerformanceConfigurationINTEL"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkPerformanceMarkerInfoINTEL var_vkperformancemarkerinfointel{VK_STRUCTURE_TYPE_PERFORMANCE_MARKER_INFO_INTEL};
    device_disp_table.CmdSetPerformanceMarkerINTEL(var_vkcommandbuffer, &var_vkperformancemarkerinfointel);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetPerformanceMarkerINTEL"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetPerformanceMarkerINTEL"));
    log.logger.clear();

    VkPerformanceStreamMarkerInfoINTEL var_vkperformancestreammarkerinfointel{VK_STRUCTURE_TYPE_PERFORMANCE_STREAM_MARKER_INFO_INTEL};
    device_disp_table.CmdSetPerformanceStreamMarkerINTEL(var_vkcommandbuffer, &var_vkperformancestreammarkerinfointel);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetPerformanceStreamMarkerINTEL"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetPerformanceStreamMarkerINTEL"));
    log.logger.clear();

    VkPerformanceOverrideInfoINTEL var_vkperformanceoverrideinfointel{VK_STRUCTURE_TYPE_PERFORMANCE_OVERRIDE_INFO_INTEL};
    device_disp_table.CmdSetPerformanceOverrideINTEL(var_vkcommandbuffer, &var_vkperformanceoverrideinfointel);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetPerformanceOverrideINTEL"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetPerformanceOverrideINTEL"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

    VkPerformanceConfigurationAcquireInfoINTEL var_vkperformanceconfigurationacquireinfointel{VK_STRUCTURE_TYPE_PERFORMANCE_CONFIGURATION_ACQUIRE_INFO_INTEL};
    device_disp_table.AcquirePerformanceConfigurationINTEL(dev.dev, &var_vkperformanceconfigurationacquireinfointel, &var_vkperformanceconfigurationintel);
    ASSERT_TRUE(log.find("Generated Layer vkAcquirePerformanceConfigurationINTEL"));
    ASSERT_TRUE(log.find("Generated Driver vkAcquirePerformanceConfigurationINTEL"));
    log.logger.clear();

    device_disp_table.ReleasePerformanceConfigurationINTEL(dev.dev, var_vkperformanceconfigurationintel);
    ASSERT_TRUE(log.find("Generated Layer vkReleasePerformanceConfigurationINTEL"));
    ASSERT_TRUE(log.find("Generated Driver vkReleasePerformanceConfigurationINTEL"));
    log.logger.clear();

} // Test for VK_INTEL_performance_query

// Test for VK_AMD_display_native_hdr
TEST(BasicEntrypointTest, AMD_display_native_hdr) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}, {VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_AMD_DISPLAY_NATIVE_HDR_EXTENSION_NAME}, {VK_KHR_SWAPCHAIN_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkSwapchainKHR var_vkswapchainkhr{};
    VkBool32 var_vkbool32{};
    device_disp_table.SetLocalDimmingAMD(dev.dev, var_vkswapchainkhr, var_vkbool32);
    ASSERT_TRUE(log.find("Generated Layer vkSetLocalDimmingAMD"));
    ASSERT_TRUE(log.find("Generated Driver vkSetLocalDimmingAMD"));
    log.logger.clear();

} // Test for VK_AMD_display_native_hdr

#ifdef VK_USE_PLATFORM_FUCHSIA
// Test for VK_FUCHSIA_imagepipe_surface
TEST(BasicEntrypointTest, FUCHSIA_imagepipe_surface) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_FUCHSIA_IMAGEPIPE_SURFACE_EXTENSION_NAME}, {VK_KHR_SURFACE_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    VkImagePipeSurfaceCreateInfoFUCHSIA var_vkimagepipesurfacecreateinfofuchsia{VK_STRUCTURE_TYPE_IMAGEPIPE_SURFACE_CREATE_INFO_FUCHSIA};
    VkSurfaceKHR var_vksurfacekhr{};
    inst_disp_table.CreateImagePipeSurfaceFUCHSIA(instance, &var_vkimagepipesurfacecreateinfofuchsia, nullptr, &var_vksurfacekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateImagePipeSurfaceFUCHSIA"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateImagePipeSurfaceFUCHSIA"));
    log.logger.clear();

    inst_disp_table.DestroySurfaceKHR(instance, var_vksurfacekhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySurfaceKHR"));
    log.logger.clear();

} // Test for VK_FUCHSIA_imagepipe_surface
#endif // VK_USE_PLATFORM_FUCHSIA

#ifdef VK_USE_PLATFORM_METAL_EXT
// Test for VK_EXT_metal_surface
TEST(BasicEntrypointTest, EXT_metal_surface) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_EXT_METAL_SURFACE_EXTENSION_NAME}, {VK_KHR_SURFACE_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    VkMetalSurfaceCreateInfoEXT var_vkmetalsurfacecreateinfoext{VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT};
    VkSurfaceKHR var_vksurfacekhr{};
    inst_disp_table.CreateMetalSurfaceEXT(instance, &var_vkmetalsurfacecreateinfoext, nullptr, &var_vksurfacekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateMetalSurfaceEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateMetalSurfaceEXT"));
    log.logger.clear();

    inst_disp_table.DestroySurfaceKHR(instance, var_vksurfacekhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySurfaceKHR"));
    log.logger.clear();

} // Test for VK_EXT_metal_surface
#endif // VK_USE_PLATFORM_METAL_EXT

// Test for VK_EXT_buffer_device_address
TEST(BasicEntrypointTest, EXT_buffer_device_address) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkBufferDeviceAddressInfo var_vkbufferdeviceaddressinfo{VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO};
    device_disp_table.GetBufferDeviceAddressEXT(dev.dev, &var_vkbufferdeviceaddressinfo);
    ASSERT_TRUE(log.find("Generated Layer vkGetBufferDeviceAddressEXT") ||
                log.find("Generated Layer vkGetBufferDeviceAddress"));
    ASSERT_TRUE(log.find("Generated Driver vkGetBufferDeviceAddressEXT") ||
                log.find("Generated Driver vkGetBufferDeviceAddress"));
    log.logger.clear();

} // Test for VK_EXT_buffer_device_address

// Test for VK_EXT_tooling_info
TEST(BasicEntrypointTest, EXT_tooling_info) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

} // Test for VK_EXT_tooling_info

// Test for VK_NV_cooperative_matrix
TEST(BasicEntrypointTest, NV_cooperative_matrix) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkCooperativeMatrixPropertiesNV var_vkcooperativematrixpropertiesnv{VK_STRUCTURE_TYPE_COOPERATIVE_MATRIX_PROPERTIES_NV};
    inst_disp_table.GetPhysicalDeviceCooperativeMatrixPropertiesNV(var_vkphysicaldevice, &var_uint32_t, &var_vkcooperativematrixpropertiesnv);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceCooperativeMatrixPropertiesNV"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceCooperativeMatrixPropertiesNV"));
    log.logger.clear();

} // Test for VK_NV_cooperative_matrix

// Test for VK_NV_coverage_reduction_mode
TEST(BasicEntrypointTest, NV_coverage_reduction_mode) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

} // Test for VK_NV_coverage_reduction_mode

#ifdef VK_USE_PLATFORM_WIN32_KHR
// Test for VK_EXT_full_screen_exclusive
TEST(BasicEntrypointTest, EXT_full_screen_exclusive) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}, {VK_KHR_SURFACE_EXTENSION_NAME}, {VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME}, {VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    VkHeadlessSurfaceCreateInfoEXT var_vkheadlesssurfacecreateinfoext{VK_STRUCTURE_TYPE_HEADLESS_SURFACE_CREATE_INFO_EXT};
    VkSurfaceKHR var_vksurfacekhr{};
    inst_disp_table.CreateHeadlessSurfaceEXT(instance, &var_vkheadlesssurfacecreateinfoext, nullptr, &var_vksurfacekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateHeadlessSurfaceEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateHeadlessSurfaceEXT"));
    log.logger.clear();

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkPhysicalDeviceSurfaceInfo2KHR var_vkphysicaldevicesurfaceinfo2khr{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR};
    var_vkphysicaldevicesurfaceinfo2khr.surface = var_vksurfacekhr;
    VkPresentModeKHR var_vkpresentmodekhr{};
    inst_disp_table.GetPhysicalDeviceSurfacePresentModes2EXT(var_vkphysicaldevice, &var_vkphysicaldevicesurfaceinfo2khr, &var_uint32_t, &var_vkpresentmodekhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceSurfacePresentModes2EXT"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceSurfacePresentModes2EXT"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME}, {VK_KHR_SWAPCHAIN_EXTENSION_NAME}, {VK_KHR_DEVICE_GROUP_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkSwapchainKHR var_vkswapchainkhr{};
    device_disp_table.AcquireFullScreenExclusiveModeEXT(dev.dev, var_vkswapchainkhr);
    ASSERT_TRUE(log.find("Generated Layer vkAcquireFullScreenExclusiveModeEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkAcquireFullScreenExclusiveModeEXT"));
    log.logger.clear();

    device_disp_table.ReleaseFullScreenExclusiveModeEXT(dev.dev, var_vkswapchainkhr);
    ASSERT_TRUE(log.find("Generated Layer vkReleaseFullScreenExclusiveModeEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkReleaseFullScreenExclusiveModeEXT"));
    log.logger.clear();

    inst_disp_table.DestroySurfaceKHR(instance, var_vksurfacekhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySurfaceKHR"));
    log.logger.clear();

} // Test for VK_EXT_full_screen_exclusive
#endif // VK_USE_PLATFORM_WIN32_KHR

// Test for VK_EXT_headless_surface
TEST(BasicEntrypointTest, EXT_headless_surface) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME}, {VK_KHR_SURFACE_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    VkHeadlessSurfaceCreateInfoEXT var_vkheadlesssurfacecreateinfoext{VK_STRUCTURE_TYPE_HEADLESS_SURFACE_CREATE_INFO_EXT};
    VkSurfaceKHR var_vksurfacekhr{};
    inst_disp_table.CreateHeadlessSurfaceEXT(instance, &var_vkheadlesssurfacecreateinfoext, nullptr, &var_vksurfacekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateHeadlessSurfaceEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateHeadlessSurfaceEXT"));
    log.logger.clear();

    inst_disp_table.DestroySurfaceKHR(instance, var_vksurfacekhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySurfaceKHR"));
    log.logger.clear();

} // Test for VK_EXT_headless_surface

// Test for VK_EXT_line_rasterization
TEST(BasicEntrypointTest, EXT_line_rasterization) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    uint16_t var_uint16_t = 1;
    device_disp_table.CmdSetLineStippleEXT(var_vkcommandbuffer, var_uint32_t, var_uint16_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetLineStippleEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetLineStippleEXT"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_EXT_line_rasterization

// Test for VK_EXT_host_query_reset
TEST(BasicEntrypointTest, EXT_host_query_reset) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkQueryPool var_vkquerypool{};
    device_disp_table.ResetQueryPoolEXT(dev.dev, var_vkquerypool, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkResetQueryPoolEXT") ||
                log.find("Generated Layer vkResetQueryPool"));
    ASSERT_TRUE(log.find("Generated Driver vkResetQueryPoolEXT") ||
                log.find("Generated Driver vkResetQueryPool"));
    log.logger.clear();

    VkQueryPoolCreateInfo var_vkquerypoolcreateinfo{VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO};
    device_disp_table.CreateQueryPool(dev.dev, &var_vkquerypoolcreateinfo, nullptr, &var_vkquerypool);
    ASSERT_TRUE(log.find("Generated Layer vkCreateQueryPool"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateQueryPool"));
    log.logger.clear();

    device_disp_table.DestroyQueryPool(dev.dev, var_vkquerypool, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyQueryPool"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyQueryPool"));
    log.logger.clear();

} // Test for VK_EXT_host_query_reset

// Test for VK_EXT_extended_dynamic_state
TEST(BasicEntrypointTest, EXT_extended_dynamic_state) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkBufferCreateInfo var_vkbuffercreateinfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    VkBuffer var_vkbuffer{};
    device_disp_table.CreateBuffer(dev.dev, &var_vkbuffercreateinfo, nullptr, &var_vkbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkCreateBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateBuffer"));
    log.logger.clear();

    device_disp_table.DestroyBuffer(dev.dev, var_vkbuffer, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyBuffer"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyBuffer"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkCullModeFlags var_vkcullmodeflags{};
    device_disp_table.CmdSetCullModeEXT(var_vkcommandbuffer, var_vkcullmodeflags);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetCullModeEXT") ||
                log.find("Generated Layer vkCmdSetCullMode"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetCullModeEXT") ||
                log.find("Generated Driver vkCmdSetCullMode"));
    log.logger.clear();

    VkFrontFace var_vkfrontface{};
    device_disp_table.CmdSetFrontFaceEXT(var_vkcommandbuffer, var_vkfrontface);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetFrontFaceEXT") ||
                log.find("Generated Layer vkCmdSetFrontFace"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetFrontFaceEXT") ||
                log.find("Generated Driver vkCmdSetFrontFace"));
    log.logger.clear();

    VkPrimitiveTopology var_vkprimitivetopology{};
    device_disp_table.CmdSetPrimitiveTopologyEXT(var_vkcommandbuffer, var_vkprimitivetopology);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetPrimitiveTopologyEXT") ||
                log.find("Generated Layer vkCmdSetPrimitiveTopology"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetPrimitiveTopologyEXT") ||
                log.find("Generated Driver vkCmdSetPrimitiveTopology"));
    log.logger.clear();

    VkViewport var_vkviewport{};
    device_disp_table.CmdSetViewportWithCountEXT(var_vkcommandbuffer, var_uint32_t, &var_vkviewport);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetViewportWithCountEXT") ||
                log.find("Generated Layer vkCmdSetViewportWithCount"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetViewportWithCountEXT") ||
                log.find("Generated Driver vkCmdSetViewportWithCount"));
    log.logger.clear();

    VkRect2D var_vkrect2d{};
    device_disp_table.CmdSetScissorWithCountEXT(var_vkcommandbuffer, var_uint32_t, &var_vkrect2d);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetScissorWithCountEXT") ||
                log.find("Generated Layer vkCmdSetScissorWithCount"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetScissorWithCountEXT") ||
                log.find("Generated Driver vkCmdSetScissorWithCount"));
    log.logger.clear();

    VkDeviceSize var_vkdevicesize{};
    device_disp_table.CmdBindVertexBuffers2EXT(var_vkcommandbuffer, var_uint32_t, var_uint32_t, &var_vkbuffer, &var_vkdevicesize, &var_vkdevicesize, &var_vkdevicesize);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBindVertexBuffers2EXT") ||
                log.find("Generated Layer vkCmdBindVertexBuffers2"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBindVertexBuffers2EXT") ||
                log.find("Generated Driver vkCmdBindVertexBuffers2"));
    log.logger.clear();

    VkBool32 var_vkbool32{};
    device_disp_table.CmdSetDepthTestEnableEXT(var_vkcommandbuffer, var_vkbool32);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetDepthTestEnableEXT") ||
                log.find("Generated Layer vkCmdSetDepthTestEnable"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetDepthTestEnableEXT") ||
                log.find("Generated Driver vkCmdSetDepthTestEnable"));
    log.logger.clear();

    device_disp_table.CmdSetDepthWriteEnableEXT(var_vkcommandbuffer, var_vkbool32);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetDepthWriteEnableEXT") ||
                log.find("Generated Layer vkCmdSetDepthWriteEnable"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetDepthWriteEnableEXT") ||
                log.find("Generated Driver vkCmdSetDepthWriteEnable"));
    log.logger.clear();

    VkCompareOp var_vkcompareop{};
    device_disp_table.CmdSetDepthCompareOpEXT(var_vkcommandbuffer, var_vkcompareop);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetDepthCompareOpEXT") ||
                log.find("Generated Layer vkCmdSetDepthCompareOp"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetDepthCompareOpEXT") ||
                log.find("Generated Driver vkCmdSetDepthCompareOp"));
    log.logger.clear();

    device_disp_table.CmdSetDepthBoundsTestEnableEXT(var_vkcommandbuffer, var_vkbool32);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetDepthBoundsTestEnableEXT") ||
                log.find("Generated Layer vkCmdSetDepthBoundsTestEnable"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetDepthBoundsTestEnableEXT") ||
                log.find("Generated Driver vkCmdSetDepthBoundsTestEnable"));
    log.logger.clear();

    device_disp_table.CmdSetStencilTestEnableEXT(var_vkcommandbuffer, var_vkbool32);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetStencilTestEnableEXT") ||
                log.find("Generated Layer vkCmdSetStencilTestEnable"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetStencilTestEnableEXT") ||
                log.find("Generated Driver vkCmdSetStencilTestEnable"));
    log.logger.clear();

    VkStencilFaceFlags var_vkstencilfaceflags{};
    VkStencilOp var_vkstencilop{};
    device_disp_table.CmdSetStencilOpEXT(var_vkcommandbuffer, var_vkstencilfaceflags, var_vkstencilop, var_vkstencilop, var_vkstencilop, var_vkcompareop);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetStencilOpEXT") ||
                log.find("Generated Layer vkCmdSetStencilOp"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetStencilOpEXT") ||
                log.find("Generated Driver vkCmdSetStencilOp"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_EXT_extended_dynamic_state

// Test for VK_NV_device_generated_commands
TEST(BasicEntrypointTest, NV_device_generated_commands) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_NV_DEVICE_GENERATED_COMMANDS_EXTENSION_NAME}, {VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkGeneratedCommandsMemoryRequirementsInfoNV var_vkgeneratedcommandsmemoryrequirementsinfonv{VK_STRUCTURE_TYPE_GENERATED_COMMANDS_MEMORY_REQUIREMENTS_INFO_NV};
    VkMemoryRequirements2 var_vkmemoryrequirements2{VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2};
    device_disp_table.GetGeneratedCommandsMemoryRequirementsNV(dev.dev, &var_vkgeneratedcommandsmemoryrequirementsinfonv, &var_vkmemoryrequirements2);
    ASSERT_TRUE(log.find("Generated Layer vkGetGeneratedCommandsMemoryRequirementsNV"));
    ASSERT_TRUE(log.find("Generated Driver vkGetGeneratedCommandsMemoryRequirementsNV"));
    log.logger.clear();

    VkPipelineCache var_vkpipelinecache{};
    VkGraphicsPipelineCreateInfo var_vkgraphicspipelinecreateinfo{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    VkPipeline var_vkpipeline{};
    device_disp_table.CreateGraphicsPipelines(dev.dev, var_vkpipelinecache, var_uint32_t, &var_vkgraphicspipelinecreateinfo, nullptr, &var_vkpipeline);
    ASSERT_TRUE(log.find("Generated Layer vkCreateGraphicsPipelines"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateGraphicsPipelines"));
    log.logger.clear();

    device_disp_table.DestroyPipeline(dev.dev, var_vkpipeline, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyPipeline"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyPipeline"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkGeneratedCommandsInfoNV var_vkgeneratedcommandsinfonv{VK_STRUCTURE_TYPE_GENERATED_COMMANDS_INFO_NV};
    device_disp_table.CmdPreprocessGeneratedCommandsNV(var_vkcommandbuffer, &var_vkgeneratedcommandsinfonv);
    ASSERT_TRUE(log.find("Generated Layer vkCmdPreprocessGeneratedCommandsNV"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdPreprocessGeneratedCommandsNV"));
    log.logger.clear();

    VkBool32 var_vkbool32{};
    device_disp_table.CmdExecuteGeneratedCommandsNV(var_vkcommandbuffer, var_vkbool32, &var_vkgeneratedcommandsinfonv);
    ASSERT_TRUE(log.find("Generated Layer vkCmdExecuteGeneratedCommandsNV"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdExecuteGeneratedCommandsNV"));
    log.logger.clear();

    VkPipelineBindPoint var_vkpipelinebindpoint{};
    device_disp_table.CmdBindPipelineShaderGroupNV(var_vkcommandbuffer, var_vkpipelinebindpoint, var_vkpipeline, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBindPipelineShaderGroupNV"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBindPipelineShaderGroupNV"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_NV_device_generated_commands

// Test for VK_EXT_acquire_drm_display
TEST(BasicEntrypointTest, EXT_acquire_drm_display) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_EXT_ACQUIRE_DRM_DISPLAY_EXTENSION_NAME}, {VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    int32_t var_int32_t{};
    VkDisplayKHR var_vkdisplaykhr{};
    inst_disp_table.AcquireDrmDisplayEXT(var_vkphysicaldevice, var_int32_t, var_vkdisplaykhr);
    ASSERT_TRUE(log.find("Generated Layer vkAcquireDrmDisplayEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkAcquireDrmDisplayEXT"));
    log.logger.clear();

    inst_disp_table.GetDrmDisplayEXT(var_vkphysicaldevice, var_int32_t, var_uint32_t, &var_vkdisplaykhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetDrmDisplayEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDrmDisplayEXT"));
    log.logger.clear();

} // Test for VK_EXT_acquire_drm_display

// Test for VK_EXT_private_data
TEST(BasicEntrypointTest, EXT_private_data) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_EXT_PRIVATE_DATA_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkObjectType var_vkobjecttype{};
    uint64_t var_uint64_t = 1;
    VkPrivateDataSlot var_vkprivatedataslot{};
    device_disp_table.SetPrivateDataEXT(dev.dev, var_vkobjecttype, var_uint64_t, var_vkprivatedataslot, var_uint64_t);
    ASSERT_TRUE(log.find("Generated Layer vkSetPrivateDataEXT") ||
                log.find("Generated Layer vkSetPrivateData"));
    ASSERT_TRUE(log.find("Generated Driver vkSetPrivateDataEXT") ||
                log.find("Generated Driver vkSetPrivateData"));
    log.logger.clear();

    device_disp_table.GetPrivateDataEXT(dev.dev, var_vkobjecttype, var_uint64_t, var_vkprivatedataslot, &var_uint64_t);
    ASSERT_TRUE(log.find("Generated Layer vkGetPrivateDataEXT") ||
                log.find("Generated Layer vkGetPrivateData"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPrivateDataEXT") ||
                log.find("Generated Driver vkGetPrivateData"));
    log.logger.clear();

    VkPrivateDataSlotCreateInfo var_vkprivatedataslotcreateinfo{VK_STRUCTURE_TYPE_PRIVATE_DATA_SLOT_CREATE_INFO};
    device_disp_table.CreatePrivateDataSlot(dev.dev, &var_vkprivatedataslotcreateinfo, nullptr, &var_vkprivatedataslot);
    ASSERT_TRUE(log.find("Generated Layer vkCreatePrivateDataSlot"));
    ASSERT_TRUE(log.find("Generated Driver vkCreatePrivateDataSlot"));
    log.logger.clear();

    device_disp_table.DestroyPrivateDataSlot(dev.dev, var_vkprivatedataslot, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyPrivateDataSlot"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyPrivateDataSlot"));
    log.logger.clear();

} // Test for VK_EXT_private_data

// Test for VK_NV_fragment_shading_rate_enums
TEST(BasicEntrypointTest, NV_fragment_shading_rate_enums) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_NV_FRAGMENT_SHADING_RATE_ENUMS_EXTENSION_NAME}, {VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkFragmentShadingRateNV var_vkfragmentshadingratenv{};
    VkFragmentShadingRateCombinerOpKHR var_vkfragmentshadingratecombineropkhr_1d_2[2]{};
    device_disp_table.CmdSetFragmentShadingRateEnumNV(var_vkcommandbuffer, var_vkfragmentshadingratenv, var_vkfragmentshadingratecombineropkhr_1d_2);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetFragmentShadingRateEnumNV"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetFragmentShadingRateEnumNV"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_NV_fragment_shading_rate_enums

#ifdef VK_USE_PLATFORM_WIN32_KHR
// Test for VK_NV_acquire_winrt_display
TEST(BasicEntrypointTest, NV_acquire_winrt_display) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    VkDisplayKHR var_vkdisplaykhr{};
    inst_disp_table.AcquireWinrtDisplayNV(var_vkphysicaldevice, var_vkdisplaykhr);
    ASSERT_TRUE(log.find("Generated Layer vkAcquireWinrtDisplayNV"));
    ASSERT_TRUE(log.find("Generated Driver vkAcquireWinrtDisplayNV"));
    log.logger.clear();

    inst_disp_table.GetWinrtDisplayNV(var_vkphysicaldevice, var_uint32_t, &var_vkdisplaykhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetWinrtDisplayNV"));
    ASSERT_TRUE(log.find("Generated Driver vkGetWinrtDisplayNV"));
    log.logger.clear();

} // Test for VK_NV_acquire_winrt_display
#endif // VK_USE_PLATFORM_WIN32_KHR

#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
// Test for VK_EXT_directfb_surface
TEST(BasicEntrypointTest, EXT_directfb_surface) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_EXT_DIRECTFB_SURFACE_EXTENSION_NAME}, {VK_KHR_SURFACE_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    VkDirectFBSurfaceCreateInfoEXT var_vkdirectfbsurfacecreateinfoext{VK_STRUCTURE_TYPE_DIRECTFB_SURFACE_CREATE_INFO_EXT};
    VkSurfaceKHR var_vksurfacekhr{};
    inst_disp_table.CreateDirectFBSurfaceEXT(instance, &var_vkdirectfbsurfacecreateinfoext, nullptr, &var_vksurfacekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateDirectFBSurfaceEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateDirectFBSurfaceEXT"));
    log.logger.clear();

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    IDirectFB var_idirectfb{};
    inst_disp_table.GetPhysicalDeviceDirectFBPresentationSupportEXT(var_vkphysicaldevice, var_uint32_t, &var_idirectfb);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceDirectFBPresentationSupportEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceDirectFBPresentationSupportEXT"));
    log.logger.clear();

    inst_disp_table.DestroySurfaceKHR(instance, var_vksurfacekhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySurfaceKHR"));
    log.logger.clear();

} // Test for VK_EXT_directfb_surface
#endif // VK_USE_PLATFORM_DIRECTFB_EXT

// Test for VK_EXT_vertex_input_dynamic_state
TEST(BasicEntrypointTest, EXT_vertex_input_dynamic_state) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkVertexInputBindingDescription2EXT var_vkvertexinputbindingdescription2ext{VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT};
    VkVertexInputAttributeDescription2EXT var_vkvertexinputattributedescription2ext{VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT};
    device_disp_table.CmdSetVertexInputEXT(var_vkcommandbuffer, var_uint32_t, &var_vkvertexinputbindingdescription2ext, var_uint32_t, &var_vkvertexinputattributedescription2ext);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetVertexInputEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetVertexInputEXT"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_EXT_vertex_input_dynamic_state

#ifdef VK_USE_PLATFORM_FUCHSIA
// Test for VK_FUCHSIA_external_memory
TEST(BasicEntrypointTest, FUCHSIA_external_memory) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_FUCHSIA_EXTERNAL_MEMORY_EXTENSION_NAME}, {VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkMemoryGetZirconHandleInfoFUCHSIA var_vkmemorygetzirconhandleinfofuchsia{VK_STRUCTURE_TYPE_MEMORY_GET_ZIRCON_HANDLE_INFO_FUCHSIA};
    zx_handle_t var_zx_handle_t{};
    device_disp_table.GetMemoryZirconHandleFUCHSIA(dev.dev, &var_vkmemorygetzirconhandleinfofuchsia, &var_zx_handle_t);
    ASSERT_TRUE(log.find("Generated Layer vkGetMemoryZirconHandleFUCHSIA"));
    ASSERT_TRUE(log.find("Generated Driver vkGetMemoryZirconHandleFUCHSIA"));
    log.logger.clear();

    VkExternalMemoryHandleTypeFlagBits var_vkexternalmemoryhandletypeflagbits{};
    VkMemoryZirconHandlePropertiesFUCHSIA var_vkmemoryzirconhandlepropertiesfuchsia{VK_STRUCTURE_TYPE_MEMORY_ZIRCON_HANDLE_PROPERTIES_FUCHSIA};
    device_disp_table.GetMemoryZirconHandlePropertiesFUCHSIA(dev.dev, var_vkexternalmemoryhandletypeflagbits, var_zx_handle_t, &var_vkmemoryzirconhandlepropertiesfuchsia);
    ASSERT_TRUE(log.find("Generated Layer vkGetMemoryZirconHandlePropertiesFUCHSIA"));
    ASSERT_TRUE(log.find("Generated Driver vkGetMemoryZirconHandlePropertiesFUCHSIA"));
    log.logger.clear();

} // Test for VK_FUCHSIA_external_memory
#endif // VK_USE_PLATFORM_FUCHSIA

#ifdef VK_USE_PLATFORM_FUCHSIA
// Test for VK_FUCHSIA_external_semaphore
TEST(BasicEntrypointTest, FUCHSIA_external_semaphore) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_FUCHSIA_EXTERNAL_SEMAPHORE_EXTENSION_NAME}, {VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkImportSemaphoreZirconHandleInfoFUCHSIA var_vkimportsemaphorezirconhandleinfofuchsia{VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_ZIRCON_HANDLE_INFO_FUCHSIA};
    device_disp_table.ImportSemaphoreZirconHandleFUCHSIA(dev.dev, &var_vkimportsemaphorezirconhandleinfofuchsia);
    ASSERT_TRUE(log.find("Generated Layer vkImportSemaphoreZirconHandleFUCHSIA"));
    ASSERT_TRUE(log.find("Generated Driver vkImportSemaphoreZirconHandleFUCHSIA"));
    log.logger.clear();

    VkSemaphoreGetZirconHandleInfoFUCHSIA var_vksemaphoregetzirconhandleinfofuchsia{VK_STRUCTURE_TYPE_SEMAPHORE_GET_ZIRCON_HANDLE_INFO_FUCHSIA};
    zx_handle_t var_zx_handle_t{};
    device_disp_table.GetSemaphoreZirconHandleFUCHSIA(dev.dev, &var_vksemaphoregetzirconhandleinfofuchsia, &var_zx_handle_t);
    ASSERT_TRUE(log.find("Generated Layer vkGetSemaphoreZirconHandleFUCHSIA"));
    ASSERT_TRUE(log.find("Generated Driver vkGetSemaphoreZirconHandleFUCHSIA"));
    log.logger.clear();

} // Test for VK_FUCHSIA_external_semaphore
#endif // VK_USE_PLATFORM_FUCHSIA

#ifdef VK_USE_PLATFORM_FUCHSIA
// Test for VK_FUCHSIA_buffer_collection
TEST(BasicEntrypointTest, FUCHSIA_buffer_collection) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_FUCHSIA_BUFFER_COLLECTION_EXTENSION_NAME}, {VK_FUCHSIA_EXTERNAL_MEMORY_EXTENSION_NAME}, {VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkBufferCollectionFUCHSIA var_vkbuffercollectionfuchsia{};
    VkImageConstraintsInfoFUCHSIA var_vkimageconstraintsinfofuchsia{VK_STRUCTURE_TYPE_IMAGE_CONSTRAINTS_INFO_FUCHSIA};
    device_disp_table.SetBufferCollectionImageConstraintsFUCHSIA(dev.dev, var_vkbuffercollectionfuchsia, &var_vkimageconstraintsinfofuchsia);
    ASSERT_TRUE(log.find("Generated Layer vkSetBufferCollectionImageConstraintsFUCHSIA"));
    ASSERT_TRUE(log.find("Generated Driver vkSetBufferCollectionImageConstraintsFUCHSIA"));
    log.logger.clear();

    VkBufferConstraintsInfoFUCHSIA var_vkbufferconstraintsinfofuchsia{VK_STRUCTURE_TYPE_BUFFER_CONSTRAINTS_INFO_FUCHSIA};
    device_disp_table.SetBufferCollectionBufferConstraintsFUCHSIA(dev.dev, var_vkbuffercollectionfuchsia, &var_vkbufferconstraintsinfofuchsia);
    ASSERT_TRUE(log.find("Generated Layer vkSetBufferCollectionBufferConstraintsFUCHSIA"));
    ASSERT_TRUE(log.find("Generated Driver vkSetBufferCollectionBufferConstraintsFUCHSIA"));
    log.logger.clear();

    VkBufferCollectionCreateInfoFUCHSIA var_vkbuffercollectioncreateinfofuchsia{VK_STRUCTURE_TYPE_BUFFER_COLLECTION_CREATE_INFO_FUCHSIA};
    device_disp_table.CreateBufferCollectionFUCHSIA(dev.dev, &var_vkbuffercollectioncreateinfofuchsia, nullptr, &var_vkbuffercollectionfuchsia);
    ASSERT_TRUE(log.find("Generated Layer vkCreateBufferCollectionFUCHSIA"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateBufferCollectionFUCHSIA"));
    log.logger.clear();

    device_disp_table.DestroyBufferCollectionFUCHSIA(dev.dev, var_vkbuffercollectionfuchsia, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyBufferCollectionFUCHSIA"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyBufferCollectionFUCHSIA"));
    log.logger.clear();

} // Test for VK_FUCHSIA_buffer_collection
#endif // VK_USE_PLATFORM_FUCHSIA

// Test for VK_HUAWEI_subpass_shading
TEST(BasicEntrypointTest, HUAWEI_subpass_shading) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_HUAWEI_SUBPASS_SHADING_EXTENSION_NAME}, {VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME}, {VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkRenderPass var_vkrenderpass{};
    VkExtent2D var_vkextent2d{};
    device_disp_table.GetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI(dev.dev, var_vkrenderpass, &var_vkextent2d);
    ASSERT_TRUE(log.find("Generated Layer vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI"));
    log.logger.clear();

    VkRenderPassCreateInfo2 var_vkrenderpasscreateinfo2{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2};
    device_disp_table.CreateRenderPass2(dev.dev, &var_vkrenderpasscreateinfo2, nullptr, &var_vkrenderpass);
    ASSERT_TRUE(log.find("Generated Layer vkCreateRenderPass2"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateRenderPass2"));
    log.logger.clear();

    device_disp_table.DestroyRenderPass(dev.dev, var_vkrenderpass, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyRenderPass"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyRenderPass"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    device_disp_table.CmdSubpassShadingHUAWEI(var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSubpassShadingHUAWEI"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSubpassShadingHUAWEI"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_HUAWEI_subpass_shading

// Test for VK_HUAWEI_invocation_mask
TEST(BasicEntrypointTest, HUAWEI_invocation_mask) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_HUAWEI_INVOCATION_MASK_EXTENSION_NAME}, {VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME}, {VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkImageViewCreateInfo var_vkimageviewcreateinfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    VkImageView var_vkimageview{};
    device_disp_table.CreateImageView(dev.dev, &var_vkimageviewcreateinfo, nullptr, &var_vkimageview);
    ASSERT_TRUE(log.find("Generated Layer vkCreateImageView"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateImageView"));
    log.logger.clear();

    device_disp_table.DestroyImageView(dev.dev, var_vkimageview, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyImageView"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyImageView"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkImageLayout var_vkimagelayout{};
    device_disp_table.CmdBindInvocationMaskHUAWEI(var_vkcommandbuffer, var_vkimageview, var_vkimagelayout);
    ASSERT_TRUE(log.find("Generated Layer vkCmdBindInvocationMaskHUAWEI"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBindInvocationMaskHUAWEI"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_HUAWEI_invocation_mask

// Test for VK_NV_external_memory_rdma
TEST(BasicEntrypointTest, NV_external_memory_rdma) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_NV_EXTERNAL_MEMORY_RDMA_EXTENSION_NAME}, {VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkMemoryGetRemoteAddressInfoNV var_vkmemorygetremoteaddressinfonv{VK_STRUCTURE_TYPE_MEMORY_GET_REMOTE_ADDRESS_INFO_NV};
    VkRemoteAddressNV var_vkremoteaddressnv{};
    device_disp_table.GetMemoryRemoteAddressNV(dev.dev, &var_vkmemorygetremoteaddressinfonv, &var_vkremoteaddressnv);
    ASSERT_TRUE(log.find("Generated Layer vkGetMemoryRemoteAddressNV"));
    ASSERT_TRUE(log.find("Generated Driver vkGetMemoryRemoteAddressNV"));
    log.logger.clear();

} // Test for VK_NV_external_memory_rdma

// Test for VK_EXT_extended_dynamic_state2
TEST(BasicEntrypointTest, EXT_extended_dynamic_state2) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_EXT_EXTENDED_DYNAMIC_STATE_2_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    device_disp_table.CmdSetPatchControlPointsEXT(var_vkcommandbuffer, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetPatchControlPointsEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetPatchControlPointsEXT"));
    log.logger.clear();

    VkBool32 var_vkbool32{};
    device_disp_table.CmdSetRasterizerDiscardEnableEXT(var_vkcommandbuffer, var_vkbool32);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetRasterizerDiscardEnableEXT") ||
                log.find("Generated Layer vkCmdSetRasterizerDiscardEnable"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetRasterizerDiscardEnableEXT") ||
                log.find("Generated Driver vkCmdSetRasterizerDiscardEnable"));
    log.logger.clear();

    device_disp_table.CmdSetDepthBiasEnableEXT(var_vkcommandbuffer, var_vkbool32);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetDepthBiasEnableEXT") ||
                log.find("Generated Layer vkCmdSetDepthBiasEnable"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetDepthBiasEnableEXT") ||
                log.find("Generated Driver vkCmdSetDepthBiasEnable"));
    log.logger.clear();

    VkLogicOp var_vklogicop{};
    device_disp_table.CmdSetLogicOpEXT(var_vkcommandbuffer, var_vklogicop);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetLogicOpEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetLogicOpEXT"));
    log.logger.clear();

    device_disp_table.CmdSetPrimitiveRestartEnableEXT(var_vkcommandbuffer, var_vkbool32);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetPrimitiveRestartEnableEXT") ||
                log.find("Generated Layer vkCmdSetPrimitiveRestartEnable"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetPrimitiveRestartEnableEXT") ||
                log.find("Generated Driver vkCmdSetPrimitiveRestartEnable"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_EXT_extended_dynamic_state2

#ifdef VK_USE_PLATFORM_SCREEN_QNX
// Test for VK_QNX_screen_surface
TEST(BasicEntrypointTest, QNX_screen_surface) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_QNX_SCREEN_SURFACE_EXTENSION_NAME}, {VK_KHR_SURFACE_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    VkScreenSurfaceCreateInfoQNX var_vkscreensurfacecreateinfoqnx{VK_STRUCTURE_TYPE_SCREEN_SURFACE_CREATE_INFO_QNX};
    VkSurfaceKHR var_vksurfacekhr{};
    inst_disp_table.CreateScreenSurfaceQNX(instance, &var_vkscreensurfacecreateinfoqnx, nullptr, &var_vksurfacekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateScreenSurfaceQNX"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateScreenSurfaceQNX"));
    log.logger.clear();

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    _screen_window var__screen_window{};
    inst_disp_table.GetPhysicalDeviceScreenPresentationSupportQNX(var_vkphysicaldevice, var_uint32_t, &var__screen_window);
    ASSERT_TRUE(log.find("Generated Layer vkGetPhysicalDeviceScreenPresentationSupportQNX"));
    ASSERT_TRUE(log.find("Generated Driver vkGetPhysicalDeviceScreenPresentationSupportQNX"));
    log.logger.clear();

    inst_disp_table.DestroySurfaceKHR(instance, var_vksurfacekhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroySurfaceKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroySurfaceKHR"));
    log.logger.clear();

} // Test for VK_QNX_screen_surface
#endif // VK_USE_PLATFORM_SCREEN_QNX

// Test for VK_EXT_color_write_enable
TEST(BasicEntrypointTest, EXT_color_write_enable) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extensions({{VK_EXT_DEBUG_UTILS_EXTENSION_NAME}, {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME}});
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_EXT_COLOR_WRITE_ENABLE_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkBool32 var_vkbool32{};
    device_disp_table.CmdSetColorWriteEnableEXT(var_vkcommandbuffer, var_uint32_t, &var_vkbool32);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetColorWriteEnableEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetColorWriteEnableEXT"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_EXT_color_write_enable

// Test for VK_EXT_multi_draw
TEST(BasicEntrypointTest, EXT_multi_draw) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_EXT_MULTI_DRAW_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkMultiDrawInfoEXT var_vkmultidrawinfoext{};
    device_disp_table.CmdDrawMultiEXT(var_vkcommandbuffer, var_uint32_t, &var_vkmultidrawinfoext, var_uint32_t, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDrawMultiEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDrawMultiEXT"));
    log.logger.clear();

    VkMultiDrawIndexedInfoEXT var_vkmultidrawindexedinfoext{};
    int32_t var_int32_t{};
    device_disp_table.CmdDrawMultiIndexedEXT(var_vkcommandbuffer, var_uint32_t, &var_vkmultidrawindexedinfoext, var_uint32_t, var_uint32_t, var_uint32_t, &var_int32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdDrawMultiIndexedEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdDrawMultiIndexedEXT"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_EXT_multi_draw

// Test for VK_EXT_pageable_device_local_memory
TEST(BasicEntrypointTest, EXT_pageable_device_local_memory) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_EXT_PAGEABLE_DEVICE_LOCAL_MEMORY_EXTENSION_NAME}, {VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkDeviceMemory var_vkdevicememory{};
    float var_float{};
    device_disp_table.SetDeviceMemoryPriorityEXT(dev.dev, var_vkdevicememory, var_float);
    ASSERT_TRUE(log.find("Generated Layer vkSetDeviceMemoryPriorityEXT"));
    ASSERT_TRUE(log.find("Generated Driver vkSetDeviceMemoryPriorityEXT"));
    log.logger.clear();

    VkMemoryAllocateInfo var_vkmemoryallocateinfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    device_disp_table.AllocateMemory(dev.dev, &var_vkmemoryallocateinfo, nullptr, &var_vkdevicememory);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateMemory"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateMemory"));
    log.logger.clear();

    device_disp_table.FreeMemory(dev.dev, var_vkdevicememory, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkFreeMemory"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeMemory"));
    log.logger.clear();

} // Test for VK_EXT_pageable_device_local_memory

// Test for VK_VALVE_descriptor_set_host_mapping
TEST(BasicEntrypointTest, VALVE_descriptor_set_host_mapping) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extension({VK_VALVE_DESCRIPTOR_SET_HOST_MAPPING_EXTENSION_NAME}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkDescriptorSet var_vkdescriptorset{};
    uint64_t big_chunk_of_mem[512][512];

    device_disp_table.GetDescriptorSetHostMappingVALVE(dev.dev, var_vkdescriptorset, reinterpret_cast<void**>(&big_chunk_of_mem));
    ASSERT_TRUE(log.find("Generated Layer vkGetDescriptorSetHostMappingVALVE"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDescriptorSetHostMappingVALVE"));
    log.logger.clear();

    VkDescriptorSetAllocateInfo var_vkdescriptorsetallocateinfo{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    device_disp_table.AllocateDescriptorSets(dev.dev, &var_vkdescriptorsetallocateinfo, &var_vkdescriptorset);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateDescriptorSets"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateDescriptorSets"));
    log.logger.clear();

    VkDescriptorPool var_vkdescriptorpool{};
    device_disp_table.FreeDescriptorSets(dev.dev, var_vkdescriptorpool, var_uint32_t, &var_vkdescriptorset);
    ASSERT_TRUE(log.find("Generated Layer vkFreeDescriptorSets"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeDescriptorSets"));
    log.logger.clear();

} // Test for VK_VALVE_descriptor_set_host_mapping

// Test for VK_KHR_acceleration_structure
TEST(BasicEntrypointTest, KHR_acceleration_structure) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME}, {VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME}, {VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME}, {VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkDeferredOperationKHR var_vkdeferredoperationkhr{};
    VkAccelerationStructureBuildGeometryInfoKHR var_vkaccelerationstructurebuildgeometryinfokhr{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR};
    VkAccelerationStructureBuildRangeInfoKHR var_vkaccelerationstructurebuildrangeinfokhr{};
    device_disp_table.BuildAccelerationStructuresKHR(dev.dev, var_vkdeferredoperationkhr, var_uint32_t, &var_vkaccelerationstructurebuildgeometryinfokhr, reinterpret_cast<const VkAccelerationStructureBuildRangeInfoKHR* const*>(&var_vkaccelerationstructurebuildrangeinfokhr));
    ASSERT_TRUE(log.find("Generated Layer vkBuildAccelerationStructuresKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkBuildAccelerationStructuresKHR"));
    log.logger.clear();

    VkCopyAccelerationStructureInfoKHR var_vkcopyaccelerationstructureinfokhr{VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR};
    device_disp_table.CopyAccelerationStructureKHR(dev.dev, var_vkdeferredoperationkhr, &var_vkcopyaccelerationstructureinfokhr);
    ASSERT_TRUE(log.find("Generated Layer vkCopyAccelerationStructureKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCopyAccelerationStructureKHR"));
    log.logger.clear();

    VkCopyAccelerationStructureToMemoryInfoKHR var_vkcopyaccelerationstructuretomemoryinfokhr{VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_TO_MEMORY_INFO_KHR};
    device_disp_table.CopyAccelerationStructureToMemoryKHR(dev.dev, var_vkdeferredoperationkhr, &var_vkcopyaccelerationstructuretomemoryinfokhr);
    ASSERT_TRUE(log.find("Generated Layer vkCopyAccelerationStructureToMemoryKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCopyAccelerationStructureToMemoryKHR"));
    log.logger.clear();

    VkCopyMemoryToAccelerationStructureInfoKHR var_vkcopymemorytoaccelerationstructureinfokhr{VK_STRUCTURE_TYPE_COPY_MEMORY_TO_ACCELERATION_STRUCTURE_INFO_KHR};
    device_disp_table.CopyMemoryToAccelerationStructureKHR(dev.dev, var_vkdeferredoperationkhr, &var_vkcopymemorytoaccelerationstructureinfokhr);
    ASSERT_TRUE(log.find("Generated Layer vkCopyMemoryToAccelerationStructureKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCopyMemoryToAccelerationStructureKHR"));
    log.logger.clear();

    VkAccelerationStructureKHR var_vkaccelerationstructurekhr{};
    VkQueryType var_vkquerytype{};
    size_t var_size_t{};
    uint64_t big_chunk_of_mem[512][512];

    device_disp_table.WriteAccelerationStructuresPropertiesKHR(dev.dev, var_uint32_t, &var_vkaccelerationstructurekhr, var_vkquerytype, var_size_t, reinterpret_cast<void**>(big_chunk_of_mem), var_size_t);
    ASSERT_TRUE(log.find("Generated Layer vkWriteAccelerationStructuresPropertiesKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkWriteAccelerationStructuresPropertiesKHR"));
    log.logger.clear();

    VkAccelerationStructureDeviceAddressInfoKHR var_vkaccelerationstructuredeviceaddressinfokhr{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR};
    device_disp_table.GetAccelerationStructureDeviceAddressKHR(dev.dev, &var_vkaccelerationstructuredeviceaddressinfokhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetAccelerationStructureDeviceAddressKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetAccelerationStructureDeviceAddressKHR"));
    log.logger.clear();

    VkAccelerationStructureVersionInfoKHR var_vkaccelerationstructureversioninfokhr{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_VERSION_INFO_KHR};
    VkAccelerationStructureCompatibilityKHR var_vkaccelerationstructurecompatibilitykhr{};
    device_disp_table.GetDeviceAccelerationStructureCompatibilityKHR(dev.dev, &var_vkaccelerationstructureversioninfokhr, &var_vkaccelerationstructurecompatibilitykhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetDeviceAccelerationStructureCompatibilityKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetDeviceAccelerationStructureCompatibilityKHR"));
    log.logger.clear();

    VkAccelerationStructureBuildTypeKHR var_vkaccelerationstructurebuildtypekhr{};
    VkAccelerationStructureBuildSizesInfoKHR var_vkaccelerationstructurebuildsizesinfokhr{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR};
    device_disp_table.GetAccelerationStructureBuildSizesKHR(dev.dev, var_vkaccelerationstructurebuildtypekhr, &var_vkaccelerationstructurebuildgeometryinfokhr, &var_uint32_t, &var_vkaccelerationstructurebuildsizesinfokhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetAccelerationStructureBuildSizesKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetAccelerationStructureBuildSizesKHR"));
    log.logger.clear();

    VkQueryPoolCreateInfo var_vkquerypoolcreateinfo{VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO};
    VkQueryPool var_vkquerypool{};
    device_disp_table.CreateQueryPool(dev.dev, &var_vkquerypoolcreateinfo, nullptr, &var_vkquerypool);
    ASSERT_TRUE(log.find("Generated Layer vkCreateQueryPool"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateQueryPool"));
    log.logger.clear();

    device_disp_table.DestroyQueryPool(dev.dev, var_vkquerypool, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyQueryPool"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyQueryPool"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    device_disp_table.CmdBuildAccelerationStructuresKHR(var_vkcommandbuffer, var_uint32_t, &var_vkaccelerationstructurebuildgeometryinfokhr, reinterpret_cast<const VkAccelerationStructureBuildRangeInfoKHR* const*>(&var_vkaccelerationstructurebuildrangeinfokhr));
    ASSERT_TRUE(log.find("Generated Layer vkCmdBuildAccelerationStructuresKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBuildAccelerationStructuresKHR"));
    log.logger.clear();

    VkDeviceAddress var_vkdeviceaddress{};
    device_disp_table.CmdBuildAccelerationStructuresIndirectKHR(var_vkcommandbuffer, var_uint32_t, &var_vkaccelerationstructurebuildgeometryinfokhr, &var_vkdeviceaddress, &var_uint32_t, reinterpret_cast<const uint32_t* const*>(&var_uint32_t));
    ASSERT_TRUE(log.find("Generated Layer vkCmdBuildAccelerationStructuresIndirectKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdBuildAccelerationStructuresIndirectKHR"));
    log.logger.clear();

    device_disp_table.CmdCopyAccelerationStructureKHR(var_vkcommandbuffer, &var_vkcopyaccelerationstructureinfokhr);
    ASSERT_TRUE(log.find("Generated Layer vkCmdCopyAccelerationStructureKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdCopyAccelerationStructureKHR"));
    log.logger.clear();

    device_disp_table.CmdCopyAccelerationStructureToMemoryKHR(var_vkcommandbuffer, &var_vkcopyaccelerationstructuretomemoryinfokhr);
    ASSERT_TRUE(log.find("Generated Layer vkCmdCopyAccelerationStructureToMemoryKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdCopyAccelerationStructureToMemoryKHR"));
    log.logger.clear();

    device_disp_table.CmdCopyMemoryToAccelerationStructureKHR(var_vkcommandbuffer, &var_vkcopymemorytoaccelerationstructureinfokhr);
    ASSERT_TRUE(log.find("Generated Layer vkCmdCopyMemoryToAccelerationStructureKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdCopyMemoryToAccelerationStructureKHR"));
    log.logger.clear();

    device_disp_table.CmdWriteAccelerationStructuresPropertiesKHR(var_vkcommandbuffer, var_uint32_t, &var_vkaccelerationstructurekhr, var_vkquerytype, var_vkquerypool, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdWriteAccelerationStructuresPropertiesKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdWriteAccelerationStructuresPropertiesKHR"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

    VkAccelerationStructureCreateInfoKHR var_vkaccelerationstructurecreateinfokhr{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR};
    device_disp_table.CreateAccelerationStructureKHR(dev.dev, &var_vkaccelerationstructurecreateinfokhr, nullptr, &var_vkaccelerationstructurekhr);
    ASSERT_TRUE(log.find("Generated Layer vkCreateAccelerationStructureKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateAccelerationStructureKHR"));
    log.logger.clear();

    device_disp_table.DestroyAccelerationStructureKHR(dev.dev, var_vkaccelerationstructurekhr, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyAccelerationStructureKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyAccelerationStructureKHR"));
    log.logger.clear();

} // Test for VK_KHR_acceleration_structure

// Test for VK_KHR_ray_tracing_pipeline
TEST(BasicEntrypointTest, KHR_ray_tracing_pipeline) {
    FrameworkEnvironment env{};
    uint32_t vulkan_version = VK_API_VERSION_1_3;
    env.add_icd(TestICDDetails(TEST_ENTRYPOINT_DRIVER, vulkan_version));

    const char* entrypoint_test_layer_name = "VkLayer_LunarG_entrypoint_layer";
    env.add_explicit_layer(
        ManifestLayer{}.add_layer(
            ManifestLayer::LayerDescription{}.set_name(entrypoint_test_layer_name).set_lib_path(TEST_ENTRYPOINT_LAYER)),
        "regular_test_layer.json");

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.set_api_version(vulkan_version);
    instance.create_info.add_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance.create_info.add_layer(entrypoint_test_layer_name);
    instance.CheckCreate();

    DebugUtilsWrapper log{instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};
    CreateDebugUtilsMessenger(log);

    VkLayerInstanceDispatchTable inst_disp_table;
    layer_init_instance_dispatch_table(instance, &inst_disp_table, instance.functions->vkGetInstanceProcAddr);

    uint32_t var_uint32_t = 1;
    VkPhysicalDevice var_vkphysicaldevice{};
    inst_disp_table.EnumeratePhysicalDevices(instance, &var_uint32_t, &var_vkphysicaldevice);
    ASSERT_TRUE(log.find("Generated Layer vkEnumeratePhysicalDevices"));
    ASSERT_TRUE(log.find("Generated Driver vkEnumeratePhysicalDevices"));
    log.logger.clear();

    DeviceWrapper dev{instance};
    dev.create_info.add_extensions({{VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME}, {VK_KHR_SPIRV_1_4_EXTENSION_NAME}, {VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME}}).add_device_queue(DeviceQueueCreateInfo{}.add_priority(0.0f));
    dev.CheckCreate(var_vkphysicaldevice);

    VkLayerDispatchTable device_disp_table;
    layer_init_device_dispatch_table(dev.dev, &device_disp_table, instance.functions->vkGetDeviceProcAddr);

    VkPipeline var_vkpipeline{};
    size_t var_size_t{};
    uint64_t big_chunk_of_mem[512][512];

    device_disp_table.GetRayTracingShaderGroupHandlesKHR(dev.dev, var_vkpipeline, var_uint32_t, var_uint32_t, var_size_t, reinterpret_cast<void**>(big_chunk_of_mem));
    ASSERT_TRUE(log.find("Generated Layer vkGetRayTracingShaderGroupHandlesKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetRayTracingShaderGroupHandlesKHR"));
    log.logger.clear();

    device_disp_table.GetRayTracingCaptureReplayShaderGroupHandlesKHR(dev.dev, var_vkpipeline, var_uint32_t, var_uint32_t, var_size_t, reinterpret_cast<void**>(big_chunk_of_mem));
    ASSERT_TRUE(log.find("Generated Layer vkGetRayTracingCaptureReplayShaderGroupHandlesKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetRayTracingCaptureReplayShaderGroupHandlesKHR"));
    log.logger.clear();

    VkShaderGroupShaderKHR var_vkshadergroupshaderkhr{};
    device_disp_table.GetRayTracingShaderGroupStackSizeKHR(dev.dev, var_vkpipeline, var_uint32_t, var_vkshadergroupshaderkhr);
    ASSERT_TRUE(log.find("Generated Layer vkGetRayTracingShaderGroupStackSizeKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkGetRayTracingShaderGroupStackSizeKHR"));
    log.logger.clear();

    VkPipelineCacheCreateInfo var_vkpipelinecachecreateinfo{VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO};
    VkPipelineCache var_vkpipelinecache{};
    device_disp_table.CreatePipelineCache(dev.dev, &var_vkpipelinecachecreateinfo, nullptr, &var_vkpipelinecache);
    ASSERT_TRUE(log.find("Generated Layer vkCreatePipelineCache"));
    ASSERT_TRUE(log.find("Generated Driver vkCreatePipelineCache"));
    log.logger.clear();

    device_disp_table.DestroyPipelineCache(dev.dev, var_vkpipelinecache, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyPipelineCache"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyPipelineCache"));
    log.logger.clear();

    VkGraphicsPipelineCreateInfo var_vkgraphicspipelinecreateinfo{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    device_disp_table.CreateGraphicsPipelines(dev.dev, var_vkpipelinecache, var_uint32_t, &var_vkgraphicspipelinecreateinfo, nullptr, &var_vkpipeline);
    ASSERT_TRUE(log.find("Generated Layer vkCreateGraphicsPipelines"));
    ASSERT_TRUE(log.find("Generated Driver vkCreateGraphicsPipelines"));
    log.logger.clear();

    device_disp_table.DestroyPipeline(dev.dev, var_vkpipeline, nullptr);
    ASSERT_TRUE(log.find("Generated Layer vkDestroyPipeline"));
    ASSERT_TRUE(log.find("Generated Driver vkDestroyPipeline"));
    log.logger.clear();

    VkCommandBufferAllocateInfo var_vkcommandbufferallocateinfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    VkCommandBuffer var_vkcommandbuffer{};
    device_disp_table.AllocateCommandBuffers(dev.dev, &var_vkcommandbufferallocateinfo, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkAllocateCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkAllocateCommandBuffers"));
    log.logger.clear();

    VkStridedDeviceAddressRegionKHR var_vkstrideddeviceaddressregionkhr{};
    device_disp_table.CmdTraceRaysKHR(var_vkcommandbuffer, &var_vkstrideddeviceaddressregionkhr, &var_vkstrideddeviceaddressregionkhr, &var_vkstrideddeviceaddressregionkhr, &var_vkstrideddeviceaddressregionkhr, var_uint32_t, var_uint32_t, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdTraceRaysKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdTraceRaysKHR"));
    log.logger.clear();

    VkDeviceAddress var_vkdeviceaddress{};
    device_disp_table.CmdTraceRaysIndirectKHR(var_vkcommandbuffer, &var_vkstrideddeviceaddressregionkhr, &var_vkstrideddeviceaddressregionkhr, &var_vkstrideddeviceaddressregionkhr, &var_vkstrideddeviceaddressregionkhr, var_vkdeviceaddress);
    ASSERT_TRUE(log.find("Generated Layer vkCmdTraceRaysIndirectKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdTraceRaysIndirectKHR"));
    log.logger.clear();

    device_disp_table.CmdSetRayTracingPipelineStackSizeKHR(var_vkcommandbuffer, var_uint32_t);
    ASSERT_TRUE(log.find("Generated Layer vkCmdSetRayTracingPipelineStackSizeKHR"));
    ASSERT_TRUE(log.find("Generated Driver vkCmdSetRayTracingPipelineStackSizeKHR"));
    log.logger.clear();

    VkCommandPool var_vkcommandpool{};
    device_disp_table.FreeCommandBuffers(dev.dev, var_vkcommandpool, var_uint32_t, &var_vkcommandbuffer);
    ASSERT_TRUE(log.find("Generated Layer vkFreeCommandBuffers"));
    ASSERT_TRUE(log.find("Generated Driver vkFreeCommandBuffers"));
    log.logger.clear();

} // Test for VK_KHR_ray_tracing_pipeline

// clang-format on

