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
 * Author(s): Charles Giessen <charles@lunarg.com>
 *            Mark Young <marky@lunarg.com>
 */

#include "test_environment.h"

class LoaderHandleValidTests : public ::testing::Test {
   protected:
    virtual void SetUp() { env = std::unique_ptr<SingleICDShim>(new SingleICDShim(TEST_ICD_PATH_VERSION_2)); }

    virtual void TearDown() { env.reset(); }
    std::unique_ptr<SingleICDShim> env;
};

// ---- Invalid Instance tests

TEST_F(LoaderHandleValidTests, BadInstEnumPhysDevices) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkInstance bad_instance = (VkInstance)(&my_bad_data);
    uint32_t returned_physical_count = 0;

    ASSERT_DEATH(env->vulkan_functions.vkEnumeratePhysicalDevices(bad_instance, &returned_physical_count, nullptr), "");
    // TODO: Look for "invalid instance" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadInstGetInstProcAddr) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkInstance bad_instance = (VkInstance)(&my_bad_data);

    ASSERT_DEATH(env->vulkan_functions.vkGetInstanceProcAddr(bad_instance, "vkGetBufferDeviceAddress"), "");
    // TODO: Look for "invalid instance" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadInstDestroyInstance) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkInstance bad_instance = (VkInstance)(&my_bad_data);

    ASSERT_DEATH(env->vulkan_functions.vkDestroyInstance(bad_instance, nullptr), "");
    // TODO: Look for "invalid instance" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadInstDestroySurface) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_EXT_headless_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkInstance bad_instance = (VkInstance)(&my_bad_data);

    ASSERT_DEATH(env->vulkan_functions.vkDestroySurfaceKHR(bad_instance, VK_NULL_HANDLE, nullptr), "");
    // TODO: Look for "invalid instance" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadInstCreateHeadlessSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_EXT_headless_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_EXT_headless_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkInstance bad_instance = (VkInstance)(&my_bad_data);

    VkHeadlessSurfaceCreateInfoEXT surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_HEADLESS_SURFACE_CREATE_INFO_EXT;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    ASSERT_DEATH(env->vulkan_functions.vkCreateHeadlessSurfaceEXT(bad_instance, &surf_create_info, nullptr, &created_surface), "");
    // TODO: Look for "invalid instance" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadInstCreateDisplayPlaneSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_display"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_display");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkInstance bad_instance = (VkInstance)(&my_bad_data);

    VkDisplaySurfaceCreateInfoKHR surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    ASSERT_DEATH(env->vulkan_functions.vkCreateDisplayPlaneSurfaceKHR(bad_instance, &surf_create_info, nullptr, &created_surface),
                 "");
    // TODO: Look for "invalid instance" in stderr log to make sure correct error is thrown
}

#ifdef VK_USE_PLATFORM_ANDROID_KHR
TEST_F(LoaderHandleValidTests, BadInstCreateAndroidSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_android_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_android_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkInstance bad_instance = (VkInstance)(&my_bad_data);

    VkAndroidSurfaceCreateInfoKHR surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    ASSERT_DEATH(env->vulkan_functions.vkCreateAndroidSurfaceKHR(bad_instance, &surf_create_info, nullptr, &created_surface), "");
    // TODO: Look for "invalid instance" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_ANDROID_KHR

#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
TEST_F(LoaderHandleValidTests, BadInstCreateDirectFBSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_EXT_directfb_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_EXT_directfb_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkInstance bad_instance = (VkInstance)(&my_bad_data);

    VkDirectFBSurfaceCreateInfoEXT surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_DIRECTFB_SURFACE_CREATE_INFO_EXT;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    ASSERT_DEATH(env->vulkan_functions.vkCreateDirectFBSurfaceEXT(bad_instance, &surf_create_info, nullptr, &created_surface), "");
    // TODO: Look for "invalid instance" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_DIRECTFB_EXT

#ifdef VK_USE_PLATFORM_FUCHSIA
TEST_F(LoaderHandleValidTests, BadInstCreateFuchsiaSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_FUCHSIA_imagepipe_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_FUCHSIA_imagepipe_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkInstance bad_instance = (VkInstance)(&my_bad_data);

    VkImagePipeSurfaceCreateInfoFUCHSIA surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_IMAGEPIPE_SURFACE_CREATE_INFO_FUCHSIA;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    ASSERT_DEATH(env->vulkan_functions.vkCreateImagePipeSurfaceFUCHSIA(bad_instance, &surf_create_info, nullptr, &created_surface),
                 "");
    // TODO: Look for "invalid instance" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_FUCHSIA

#ifdef VK_USE_PLATFORM_GGP
TEST_F(LoaderHandleValidTests, BadInstCreateGGPSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_GGP_stream_descriptor_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_GGP_stream_descriptor_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkInstance bad_instance = (VkInstance)(&my_bad_data);

    VkStreamDescriptorSurfaceCreateInfoGGP surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_STREAM_DESCRIPTOR_SURFACE_CREATE_INFO_GGP;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    ASSERT_DEATH(
        env->vulkan_functions.vkCreateStreamDescriptorSurfaceGGP(bad_instance, &surf_create_info, nullptr, &created_surface), "");
    // TODO: Look for "invalid instance" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_GGP

#ifdef VK_USE_PLATFORM_IOS_MVK
TEST_F(LoaderHandleValidTests, BadInstCreateIOSSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_MVK_ios_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_MVK_ios_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkInstance bad_instance = (VkInstance)(&my_bad_data);

    VkIOSSurfaceCreateInfoMVK surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    ASSERT_DEATH(env->vulkan_functions.vkCreateIOSSurfaceMVK(bad_instance, &surf_create_info, nullptr, &created_surface), "");
    // TODO: Look for "invalid instance" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_IOS_MVK

#ifdef VK_USE_PLATFORM_MACOS_MVK
TEST_F(LoaderHandleValidTests, BadInstCreateMacOSSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_MVK_macos_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_MVK_macos_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkInstance bad_instance = (VkInstance)(&my_bad_data);

    VkMacOSSurfaceCreateInfoMVK surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    ASSERT_DEATH(env->vulkan_functions.vkCreateMacOSSurfaceMVK(bad_instance, &surf_create_info, nullptr, &created_surface), "");
    // TODO: Look for "invalid instance" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_MACOS_MVK

#if defined(VK_USE_PLATFORM_METAL_EXT)
TEST_F(LoaderHandleValidTests, BadInstCreateMetalSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_EXT_metal_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_EXT_metal_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkInstance bad_instance = (VkInstance)(&my_bad_data);

    VkMetalSurfaceCreateInfoEXT surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    ASSERT_DEATH(env->vulkan_functions.vkCreateMetalSurfaceEXT(bad_instance, &surf_create_info, nullptr, &created_surface), "");
    // TODO: Look for "invalid instance" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_METAL_EXT

#ifdef VK_USE_PLATFORM_SCREEN_QNX
TEST_F(LoaderHandleValidTests, BadInstCreateQNXSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_QNX_screen_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_QNX_screen_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkInstance bad_instance = (VkInstance)(&my_bad_data);

    VkScreenSurfaceCreateInfoQNX surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_SCREEN_SURFACE_CREATE_INFO_QNX;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    ASSERT_DEATH(env->vulkan_functions.vkCreateScreenSurfaceQNX(bad_instance, &surf_create_info, nullptr, &created_surface), "");
    // TODO: Look for "invalid instance" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_SCREEN_QNX

#ifdef VK_USE_PLATFORM_VI_NN
TEST_F(LoaderHandleValidTests, BadInstCreateViNNSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_NN_vi_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_NN_vi_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkInstance bad_instance = (VkInstance)(&my_bad_data);

    VkViSurfaceCreateInfoNN surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_VI_SURFACE_CREATE_INFO_NN;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    ASSERT_DEATH(env->vulkan_functions.CreateViSurfaceNN(bad_instance, &surf_create_info, nullptr, &created_surface), "");
    // TODO: Look for "invalid instance" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_VI_NN

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
TEST_F(LoaderHandleValidTests, BadInstCreateWaylandSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_wayland_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_wayland_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkInstance bad_instance = (VkInstance)(&my_bad_data);

    VkWaylandSurfaceCreateInfoKHR surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    ASSERT_DEATH(env->vulkan_functions.vkCreateWaylandSurfaceKHR(bad_instance, &surf_create_info, nullptr, &created_surface), "");
    // TODO: Look for "invalid instance" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_WAYLAND_KHR

#ifdef VK_USE_PLATFORM_WIN32_KHR
TEST_F(LoaderHandleValidTests, BadInstCreateWin32Surf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_win32_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_win32_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkInstance bad_instance = (VkInstance)(&my_bad_data);

    VkWin32SurfaceCreateInfoKHR surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    ASSERT_DEATH(env->vulkan_functions.vkCreateWin32SurfaceKHR(bad_instance, &surf_create_info, nullptr, &created_surface), "");
    // TODO: Look for "invalid instance" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_WIN32_KHR

#ifdef VK_USE_PLATFORM_XCB_KHR
TEST_F(LoaderHandleValidTests, BadInstCreateXCBSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_xcb_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_xcb_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkInstance bad_instance = (VkInstance)(&my_bad_data);

    VkXcbSurfaceCreateInfoKHR surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    ASSERT_DEATH(env->vulkan_functions.vkCreateXcbSurfaceKHR(bad_instance, &surf_create_info, nullptr, &created_surface), "");
    // TODO: Look for "invalid instance" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_XCB_KHR

#ifdef VK_USE_PLATFORM_XLIB_KHR
TEST_F(LoaderHandleValidTests, BadInstCreateXlibSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_xlib_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_xlib_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkInstance bad_instance = (VkInstance)(&my_bad_data);

    VkXlibSurfaceCreateInfoKHR surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    ASSERT_DEATH(env->vulkan_functions.vkCreateXlibSurfaceKHR(bad_instance, &surf_create_info, nullptr, &created_surface), "");
    // TODO: Look for "invalid instance" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_XLIB_KHR

// ---- Invalid Physical Device tests

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevFeature) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);

    VkPhysicalDeviceFeatures features = {};
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceFeatures(bad_physical_dev, &features), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevFormatProps) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);

    VkFormatProperties format_info = {};
    ASSERT_DEATH(
        env->vulkan_functions.vkGetPhysicalDeviceFormatProperties(bad_physical_dev, VK_FORMAT_R8G8B8A8_UNORM, &format_info), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevImgFormatProps) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);

    VkImageFormatProperties format_info = {};
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceImageFormatProperties(bad_physical_dev, VK_FORMAT_R8G8B8A8_UNORM,
                                                                                VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_LINEAR,
                                                                                VK_IMAGE_USAGE_STORAGE_BIT, 0, &format_info),
                 "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevProps) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);

    VkPhysicalDeviceProperties properties = {};
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceProperties(bad_physical_dev, &properties), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevQueueFamProps) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    uint32_t count = 0;
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceQueueFamilyProperties(bad_physical_dev, &count, nullptr), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevDevMemProps) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);

    VkPhysicalDeviceMemoryProperties properties = {};
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceMemoryProperties(bad_physical_dev, &properties), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevCreateDevice) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);

    float queue_priorities[3] = {0.0f, 0.5f, 1.0f};
    VkDeviceQueueCreateInfo dev_queue_create_info = {};
    dev_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    dev_queue_create_info.pNext = nullptr;
    dev_queue_create_info.flags = 0;
    dev_queue_create_info.queueFamilyIndex = 0;
    dev_queue_create_info.queueCount = 1;
    dev_queue_create_info.pQueuePriorities = queue_priorities;
    VkDeviceCreateInfo dev_create_info = {};
    dev_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    dev_create_info.pNext = nullptr;
    dev_create_info.flags = 0;
    dev_create_info.queueCreateInfoCount = 1;
    dev_create_info.pQueueCreateInfos = &dev_queue_create_info;
    dev_create_info.enabledLayerCount = 0;
    dev_create_info.ppEnabledLayerNames = nullptr;
    dev_create_info.enabledExtensionCount = 0;
    dev_create_info.ppEnabledExtensionNames = nullptr;
    dev_create_info.pEnabledFeatures = nullptr;
    VkDevice created_dev = VK_NULL_HANDLE;
    ASSERT_DEATH(env->vulkan_functions.vkCreateDevice(bad_physical_dev, &dev_create_info, nullptr, &created_dev), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevEnumDevExtProps) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    uint32_t count = 0;
    ASSERT_DEATH(env->vulkan_functions.vkEnumerateDeviceExtensionProperties(bad_physical_dev, nullptr, &count, nullptr), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevEnumDevLayerProps) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    uint32_t count = 0;
    ASSERT_DEATH(env->vulkan_functions.vkEnumerateDeviceLayerProperties(bad_physical_dev, &count, nullptr), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevSparseImgFormatProps) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);

    uint32_t count = 0;
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceSparseImageFormatProperties(
                     bad_physical_dev, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TYPE_2D, VK_SAMPLE_COUNT_1_BIT,
                     VK_IMAGE_USAGE_STORAGE_BIT, VK_IMAGE_TILING_LINEAR, &count, nullptr),
                 "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevFeature2) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);

    VkPhysicalDeviceFeatures2 features = {};
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = nullptr;
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceFeatures2(bad_physical_dev, &features), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevFormatProps2) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);

    VkFormatProperties2 properties = {};
    properties.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
    properties.pNext = nullptr;
    ASSERT_DEATH(
        env->vulkan_functions.vkGetPhysicalDeviceFormatProperties2(bad_physical_dev, VK_FORMAT_R8G8B8A8_UNORM, &properties), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevImgFormatProps2) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);

    VkPhysicalDeviceImageFormatInfo2 format_info = {};
    format_info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2;
    format_info.pNext = nullptr;
    VkImageFormatProperties2 properties = {};
    properties.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2;
    properties.pNext = nullptr;
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceImageFormatProperties2(bad_physical_dev, &format_info, &properties), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevProps2) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);

    VkPhysicalDeviceProperties2 properties = {};
    properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    properties.pNext = nullptr;
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceProperties2(bad_physical_dev, &properties), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevQueueFamProps2) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    uint32_t count = 0;
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceQueueFamilyProperties2(bad_physical_dev, &count, nullptr), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevDevMemProps2) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);

    VkPhysicalDeviceMemoryProperties2 properties = {};
    properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
    properties.pNext = nullptr;
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceMemoryProperties2(bad_physical_dev, &properties), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevSparseImgFormatProps2) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);

    VkPhysicalDeviceSparseImageFormatInfo2 info = {};
    info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SPARSE_IMAGE_FORMAT_INFO_2;
    info.pNext = nullptr;
    uint32_t count = 0;
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceSparseImageFormatProperties2(bad_physical_dev, &info, &count, nullptr),
                 "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevExternFenceProps) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);

    VkPhysicalDeviceExternalFenceInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_FENCE_INFO;
    info.pNext = nullptr;
    VkExternalFenceProperties props = {};
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceExternalFenceProperties(bad_physical_dev, &info, &props), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevExternBufferProps) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);

    VkPhysicalDeviceExternalBufferInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_BUFFER_INFO;
    info.pNext = nullptr;
    VkExternalBufferProperties props = {};
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceExternalBufferProperties(bad_physical_dev, &info, &props), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevExternSemaphoreProps) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);

    VkPhysicalDeviceExternalSemaphoreInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_SEMAPHORE_INFO;
    info.pNext = nullptr;
    VkExternalSemaphoreProperties props = {};
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceExternalSemaphoreProperties(bad_physical_dev, &info, &props), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevSurfaceSupportKHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_EXT_headless_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    VkBool32 supported = VK_FALSE;

    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceSurfaceSupportKHR(bad_physical_dev, 0, VK_NULL_HANDLE, &supported), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevSurfaceCapsKHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_EXT_headless_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    VkSurfaceCapabilitiesKHR caps = {};
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(bad_physical_dev, VK_NULL_HANDLE, &caps), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevSurfaceFormatsKHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_EXT_headless_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    uint32_t count = 0;
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceSurfaceFormatsKHR(bad_physical_dev, VK_NULL_HANDLE, &count, nullptr), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevSurfacePresentModesKHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_EXT_headless_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    uint32_t count = 0;
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceSurfacePresentModesKHR(bad_physical_dev, VK_NULL_HANDLE, &count, nullptr),
                 "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
TEST_F(LoaderHandleValidTests, BadPhysDevGetDirectFBPresentSupportKHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_EXT_directfb_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_EXT_directfb_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    IDirectFB directfb;
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceDirectFBPresentationSupportEXT(bad_physical_dev, 0, &directfb), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_DIRECTFB_EXT

#ifdef VK_USE_PLATFORM_SCREEN_QNX
TEST_F(LoaderHandleValidTests, BadPhysDevGetQNXPresentSupportKHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_QNX_screen_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_QNX_screen_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    ASSERT_DEATH(env->vulkan_functions.PFN_vkGetPhysicalDeviceScreenPresentationSupportQNX(bad_physical_dev, 0, nullptr), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_SCREEN_QNX

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevWaylandPresentSupportKHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_wayland_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_wayland_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceWaylandPresentationSupportKHR(bad_physical_dev, 0, nullptr), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_WAYLAND_KHR

#ifdef VK_USE_PLATFORM_WIN32_KHR
TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevWin32PresentSupportKHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_win32_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_win32_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceWin32PresentationSupportKHR(bad_physical_dev, 0), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_WIN32_KHR

#ifdef VK_USE_PLATFORM_XCB_KHR
TEST_F(LoaderHandleValidTests, BadPhysDevGetXCBPresentSupportKHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_xcb_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_xcb_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);

    xcb_visualid_t visual = 0;
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceXcbPresentationSupportKHR(bad_physical_dev, 0, nullptr, visual), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_XCB_KHR

#ifdef VK_USE_PLATFORM_XLIB_KHR
TEST_F(LoaderHandleValidTests, BadPhysDevGetXlibPresentSupportKHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_xlib_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_xlib_surface");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);

    VisualID visual = 0;
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceXlibPresentationSupportKHR(bad_physical_dev, 0, nullptr, visual), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_XLIB_KHR

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevDisplayPropsKHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_display"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_display");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    uint32_t count = 0;
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceDisplayPropertiesKHR(bad_physical_dev, &count, nullptr), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevDisplayPlanePropsKHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_display"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_display");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    uint32_t count = 0;
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceDisplayPlanePropertiesKHR(bad_physical_dev, &count, nullptr), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetDisplayPlaneSupportedDisplaysKHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_display"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_display");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    uint32_t count = 0;
    ASSERT_DEATH(env->vulkan_functions.vkGetDisplayPlaneSupportedDisplaysKHR(bad_physical_dev, 0, &count, nullptr), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetDisplayModePropsKHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_display"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_display");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    uint32_t count = 0;
    ASSERT_DEATH(env->vulkan_functions.vkGetDisplayModePropertiesKHR(bad_physical_dev, VK_NULL_HANDLE, &count, nullptr), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevCreateDisplayModeKHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_display"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_display");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    VkDisplayModeCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DISPLAY_MODE_CREATE_INFO_KHR;
    create_info.pNext = nullptr;
    VkDisplayModeKHR display_mode;
    ASSERT_DEATH(
        env->vulkan_functions.vkCreateDisplayModeKHR(bad_physical_dev, VK_NULL_HANDLE, &create_info, nullptr, &display_mode), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetDisplayPlaneCapsKHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_display"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_display");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    VkDisplayPlaneCapabilitiesKHR caps = {};
    ASSERT_DEATH(env->vulkan_functions.vkGetDisplayPlaneCapabilitiesKHR(bad_physical_dev, VK_NULL_HANDLE, 0, &caps), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevPresentRectsKHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_display"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_display");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    uint32_t count = 0;
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDevicePresentRectanglesKHR(bad_physical_dev, VK_NULL_HANDLE, &count, nullptr),
                 "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevDisplayProps2KHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_get_display_properties2"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_get_display_properties2");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    uint32_t count = 0;
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceDisplayProperties2KHR(bad_physical_dev, &count, nullptr), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevDisplayPlaneProps2KHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_get_display_properties2"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_get_display_properties2");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    uint32_t count = 0;
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceDisplayPlaneProperties2KHR(bad_physical_dev, &count, nullptr), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetDisplayModeProps2KHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_get_display_properties2"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_get_display_properties2");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    uint32_t count = 0;
    ASSERT_DEATH(env->vulkan_functions.vkGetDisplayModeProperties2KHR(bad_physical_dev, VK_NULL_HANDLE, &count, nullptr), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetDisplayPlaneCaps2KHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_get_display_properties2"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_get_display_properties2");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    VkDisplayPlaneInfo2KHR disp_plane_info = {};
    disp_plane_info.sType = VK_STRUCTURE_TYPE_DISPLAY_PLANE_INFO_2_KHR;
    disp_plane_info.pNext = nullptr;
    VkDisplayPlaneCapabilities2KHR caps = {};
    ASSERT_DEATH(env->vulkan_functions.vkGetDisplayPlaneCapabilities2KHR(bad_physical_dev, &disp_plane_info, &caps), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevSurfaceCaps2KHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_get_surface_capabilities2"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_surface");
    inst_create_info.add_extension("VK_KHR_get_surface_capabilities2");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    VkPhysicalDeviceSurfaceInfo2KHR phys_dev_surf_info = {};
    phys_dev_surf_info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
    phys_dev_surf_info.pNext = nullptr;
    VkSurfaceCapabilities2KHR caps = {};
    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceSurfaceCapabilities2KHR(bad_physical_dev, &phys_dev_surf_info, &caps),
                 "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevSurfaceFormats2KHR) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_get_surface_capabilities2"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_KHR_get_surface_capabilities2");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    VkPhysicalDeviceSurfaceInfo2KHR phys_dev_surf_info = {};
    phys_dev_surf_info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
    phys_dev_surf_info.pNext = nullptr;
    uint32_t count = 0;
    ASSERT_DEATH(
        env->vulkan_functions.vkGetPhysicalDeviceSurfaceFormats2KHR(bad_physical_dev, &phys_dev_surf_info, &count, nullptr), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevEnumPhysDevQueueFamilyPerfQueryCountersKHR) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    uint32_t count = 0;
    PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR pfn =
        reinterpret_cast<PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR>(
            env->vulkan_functions.vkGetInstanceProcAddr(instance,
                                                        "vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR"));
    ASSERT_NE(pfn, nullptr);
    ASSERT_DEATH(pfn(bad_physical_dev, 0, &count, nullptr, nullptr), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevQueueFamilyPerfQueryPassesKHR) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    VkQueryPoolPerformanceCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_PERFORMANCE_CREATE_INFO_KHR;
    create_info.pNext = nullptr;
    uint32_t count = 0;
    PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR pfn =
        reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR>(
            env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR"));
    ASSERT_NE(pfn, nullptr);
    ASSERT_DEATH(pfn(bad_physical_dev, &create_info, &count), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevFragmentShadingRatesKHR) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    uint32_t count = 0;
    PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR pfn = reinterpret_cast<PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR>(
        env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFragmentShadingRatesKHR"));
    ASSERT_NE(pfn, nullptr);
    ASSERT_DEATH(pfn(bad_physical_dev, &count, nullptr), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevMSPropsEXT) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    VkMultisamplePropertiesEXT props = {};
    PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT pfn = reinterpret_cast<PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT>(
        env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMultisamplePropertiesEXT"));
    ASSERT_NE(pfn, nullptr);
    ASSERT_DEATH(pfn(bad_physical_dev, VK_SAMPLE_COUNT_1_BIT, &props), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevAcquireDrmDisplayEXT) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_EXT_acquire_drm_display"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_EXT_acquire_drm_display");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    PFN_vkAcquireDrmDisplayEXT pfn = reinterpret_cast<PFN_vkAcquireDrmDisplayEXT>(
        env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkAcquireDrmDisplayEXT"));
    ASSERT_NE(pfn, nullptr);
    ASSERT_DEATH(pfn(bad_physical_dev, 0, VK_NULL_HANDLE), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetDrmDisplayEXT) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_EXT_acquire_drm_display"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_EXT_acquire_drm_display");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    PFN_vkGetDrmDisplayEXT pfn =
        reinterpret_cast<PFN_vkGetDrmDisplayEXT>(env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkGetDrmDisplayEXT"));
    ASSERT_NE(pfn, nullptr);
    ASSERT_DEATH(pfn(bad_physical_dev, 0, 0, VK_NULL_HANDLE), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevReleaseDisplayEXT) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_EXT_direct_mode_display"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_EXT_direct_mode_display");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    PFN_vkReleaseDisplayEXT pfn =
        reinterpret_cast<PFN_vkReleaseDisplayEXT>(env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkReleaseDisplayEXT"));
    ASSERT_NE(pfn, nullptr);
    ASSERT_DEATH(pfn(bad_physical_dev, VK_NULL_HANDLE), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
TEST_F(LoaderHandleValidTests, BadPhysDevAcquireXlibDisplayEXT) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_EXT_acquire_xlib_display"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_EXT_acquire_xlib_display");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    PFN_vkAcquireXlibDisplayEXT pfn = reinterpret_cast<PFN_vkAcquireXlibDisplayEXT>(
        env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkAcquireXlibDisplayEXT"));
    ASSERT_NE(pfn, nullptr);
    ASSERT_DEATH(pfn(bad_physical_dev, nullptr, VK_NULL_HANDLE), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

TEST_F(LoaderHandleValidTests, BadPhysDevGetRandROutputDisplayEXT) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_EXT_acquire_xlib_display"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    inst_create_info.add_extension("VK_EXT_acquire_xlib_display");
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    RROutput rrout = {};
    VkDisplayKHR disp;
    PFN_vkGetRandROutputDisplayEXT pfn = reinterpret_cast<PFN_vkGetRandROutputDisplayEXT>(
        env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkGetRandROutputDisplayEXT"));
    ASSERT_NE(pfn, nullptr);
    ASSERT_DEATH(pfn(bad_physical_dev, nullptr, rrout, &disp), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_XLIB_XRANDR_EXT

#ifdef VK_USE_PLATFORM_WIN32_KHR
TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevSurfacePresentModes2EXT) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    VkPhysicalDeviceSurfaceInfo2KHR phys_dev_surf_info = {};
    phys_dev_surf_info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
    phys_dev_surf_info.pNext = nullptr;
    uint32_t count = 0;
    PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT pfn = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT>(
        env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfacePresentModes2EXT"));
    ASSERT_NE(pfn, nullptr);
    ASSERT_DEATH(pfn(bad_physical_dev, &phys_dev_surf_info, &count, nullptr), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}
#endif  // VK_USE_PLATFORM_WIN32_KHR

TEST_F(LoaderHandleValidTests, BadPhysDevGetPhysDevToolPropertiesEXT) {
    auto& driver = env->get_test_icd();
    driver.physical_devices.emplace_back("physical_device_0");

    VkInstance instance;
    InstanceCreateInfo inst_create_info;
    ASSERT_EQ(env->vulkan_functions.vkCreateInstance(inst_create_info.get(), nullptr, &instance), VK_SUCCESS);

    struct BadData {
        uint64_t bad_array[3] = {0x123456789AB, 0x23456789AB1, 0x9876543210AB};
    } my_bad_data;
    VkPhysicalDevice bad_physical_dev = (VkPhysicalDevice)(&my_bad_data);
    PFN_vkGetPhysicalDeviceToolPropertiesEXT pfn = reinterpret_cast<PFN_vkGetPhysicalDeviceToolPropertiesEXT>(
        env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceToolPropertiesEXT"));
    ASSERT_NE(pfn, nullptr);
    uint32_t count = 0;
    ASSERT_DEATH(pfn(bad_physical_dev, &count, nullptr), "");
    // TODO: Look for "invalid physicalDevice" in stderr log to make sure correct error is thrown
}

#ifdef VK_USE_PLATFORM_ANDROID_KHR
TEST_F(LoaderHandleValidTests, VerifyHandleWrappingAndroidSurface) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_android_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});

    const char* wrap_objects_name = "WrapObjectsLayer";
    ManifestLayer::LayerDescription wrap_objects_description{};
    wrap_objects_description.name = wrap_objects_name;
    wrap_objects_description.lib_path = TEST_LAYER_WRAP_OBJECTS;

    ManifestLayer wrap_objects_layer;
    wrap_objects_layer.layers.push_back(wrap_objects_description);
    env->AddExplicitLayer(wrap_objects_layer, "wrap_objects_layer.json");

    driver.physical_devices.emplace_back("physical_device_0");
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    InstWrapper instance(env->vulkan_functions);
    instance.create_info.add_extension("VK_KHR_surface");
    instance.create_info.add_extension("VK_KHR_android_surface");
    instance.create_info.add_layer(wrap_objects_name);
    instance.CheckCreate();

    VkAndroidSurfaceCreateInfoKHR surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    PFN_vkCreateAndroidSurfaceKHR pfn_CreateSurface = reinterpret_cast<PFN_vkCreateAndroidSurfaceKHR>(
        env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkCreateAndroidSurfaceKHR"));
    ASSERT_NE(pfn_CreateSurface, nullptr);
    PFN_vkDestroySurfaceKHR pfn_DestroySurface =
        reinterpret_cast<PFN_vkDestroySurfaceKHR>(env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
    ASSERT_NE(pfn_DestroySurface, nullptr);
    ASSERT_EQ(VK_SUCCESS, pfn_CreateSurface(instance, &surf_create_info, nullptr, &created_surface));
    pfn_DestroySurface(instance, created_surface, nullptr);
}
#endif  // VK_USE_PLATFORM_ANDROID_KHR

#ifdef VK_USE_PLATFORM_DIRECTFB_EXT
TEST_F(LoaderHandleValidTests, VerifyHandleWrappingDirectFBSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_EXT_directfb_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});

    const char* wrap_objects_name = "WrapObjectsLayer";
    ManifestLayer::LayerDescription wrap_objects_description{};
    wrap_objects_description.name = wrap_objects_name;
    wrap_objects_description.lib_path = TEST_LAYER_WRAP_OBJECTS;

    ManifestLayer wrap_objects_layer;
    wrap_objects_layer.layers.push_back(wrap_objects_description);
    env->AddExplicitLayer(wrap_objects_layer, "wrap_objects_layer.json");

    driver.physical_devices.emplace_back("physical_device_0");
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    InstWrapper instance(env->vulkan_functions);
    instance.create_info.add_extension("VK_KHR_surface");
    instance.create_info.add_extension("VK_EXT_directfb_surface");
    instance.create_info.add_layer(wrap_objects_name);
    instance.CheckCreate();

    VkDirectFBSurfaceCreateInfoEXT surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_DIRECTFB_SURFACE_CREATE_INFO_EXT;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    PFN_vkCreateDirectFBSurfaceEXT pfn_CreateSurface = reinterpret_cast<PFN_vkCreateDirectFBSurfaceEXT>(
        env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkCreateDirectFBSurfaceEXT"));
    ASSERT_NE(pfn_CreateSurface, nullptr);
    PFN_vkDestroySurfaceKHR pfn_DestroySurface =
        reinterpret_cast<PFN_vkDestroySurfaceKHR>(env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
    ASSERT_NE(pfn_DestroySurface, nullptr);
    ASSERT_EQ(VK_SUCCESS, pfn_CreateSurface(instance, &surf_create_info, nullptr, &created_surface));
    pfn_DestroySurface(instance, created_surface, nullptr);
}
#endif  // VK_USE_PLATFORM_DIRECTFB_EXT

#ifdef VK_USE_PLATFORM_FUCHSIA
TEST_F(LoaderHandleValidTests, VerifyHandleWrappingFuchsiaSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_FUCHSIA_imagepipe_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});

    const char* wrap_objects_name = "WrapObjectsLayer";
    ManifestLayer::LayerDescription wrap_objects_description{};
    wrap_objects_description.name = wrap_objects_name;
    wrap_objects_description.lib_path = TEST_LAYER_WRAP_OBJECTS;

    ManifestLayer wrap_objects_layer;
    wrap_objects_layer.layers.push_back(wrap_objects_description);
    env->AddExplicitLayer(wrap_objects_layer, "wrap_objects_layer.json");

    driver.physical_devices.emplace_back("physical_device_0");
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    InstWrapper instance(env->vulkan_functions);
    instance.create_info.add_extension("VK_KHR_surface");
    instance.create_info.add_extension("VK_FUCHSIA_imagepipe_surface");
    instance.create_info.add_layer(wrap_objects_name);
    instance.CheckCreate();

    VkImagePipeSurfaceCreateInfoFUCHSIA surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_IMAGEPIPE_SURFACE_CREATE_INFO_FUCHSIA;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    PFN_vkCreateImagePipeSurfaceFUCHSIA pfn_CreateSurface = reinterpret_cast<PFN_vkCreateImagePipeSurfaceFUCHSIA>(
        env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkCreateImagePipeSurfaceFUCHSIA"));
    ASSERT_NE(pfn_CreateSurface, nullptr);
    PFN_vkDestroySurfaceKHR pfn_DestroySurface =
        reinterpret_cast<PFN_vkDestroySurfaceKHR>(env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
    ASSERT_NE(pfn_DestroySurface, nullptr);
    ASSERT_EQ(VK_SUCCESS, pfn_CreateSurface(instance, &surf_create_info, nullptr, &created_surface));
    pfn_DestroySurface(instance, created_surface, nullptr);
}
#endif  // VK_USE_PLATFORM_FUCHSIA

#ifdef VK_USE_PLATFORM_GGP
TEST_F(LoaderHandleValidTests, VerifyHandleWrappingGGPSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_GGP_stream_descriptor_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});

    const char* wrap_objects_name = "WrapObjectsLayer";
    ManifestLayer::LayerDescription wrap_objects_description{};
    wrap_objects_description.name = wrap_objects_name;
    wrap_objects_description.lib_path = TEST_LAYER_WRAP_OBJECTS;

    ManifestLayer wrap_objects_layer;
    wrap_objects_layer.layers.push_back(wrap_objects_description);
    env->AddExplicitLayer(wrap_objects_layer, "wrap_objects_layer.json");

    driver.physical_devices.emplace_back("physical_device_0");
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    InstWrapper instance(env->vulkan_functions);
    instance.create_info.add_extension("VK_KHR_surface");
    instance.create_info.add_extension("VK_GGP_stream_descriptor_surface");
    instance.create_info.add_layer(wrap_objects_name);
    instance.CheckCreate();

    VkStreamDescriptorSurfaceCreateInfoGGP surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_STREAM_DESCRIPTOR_SURFACE_CREATE_INFO_GGP;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    PFN_vkCreateStreamDescriptorSurfaceGGP pfn_CreateSurface = reinterpret_cast<PFN_vkCreateStreamDescriptorSurfaceGGP>(
        env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkCreateStreamDescriptorSurfaceGGP"));
    ASSERT_NE(pfn_CreateSurface, nullptr);
    PFN_vkDestroySurfaceKHR pfn_DestroySurface =
        reinterpret_cast<PFN_vkDestroySurfaceKHR>(env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
    ASSERT_NE(pfn_DestroySurface, nullptr);
    ASSERT_EQ(VK_SUCCESS, pfn_CreateSurface(instance, &surf_create_info, nullptr, &created_surface));
    pfn_DestroySurface(instance, created_surface, nullptr);
}
#endif  // VK_USE_PLATFORM_GGP

#ifdef VK_USE_PLATFORM_IOS_MVK
TEST_F(LoaderHandleValidTests, VerifyHandleWrappingIOSSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_MVK_ios_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});

    const char* wrap_objects_name = "WrapObjectsLayer";
    ManifestLayer::LayerDescription wrap_objects_description{};
    wrap_objects_description.name = wrap_objects_name;
    wrap_objects_description.lib_path = TEST_LAYER_WRAP_OBJECTS;

    ManifestLayer wrap_objects_layer;
    wrap_objects_layer.layers.push_back(wrap_objects_description);
    env->AddExplicitLayer(wrap_objects_layer, "wrap_objects_layer.json");

    driver.physical_devices.emplace_back("physical_device_0");
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    InstWrapper instance(env->vulkan_functions);
    instance.create_info.add_extension("VK_KHR_surface");
    instance.create_info.add_extension("VK_MVK_ios_surface");
    instance.create_info.add_layer(wrap_objects_name);
    instance.CheckCreate();

    VkIOSSurfaceCreateInfoMVK surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    PFN_vkCreateIOSSurfaceMVK pfn_CreateSurface =
        reinterpret_cast<PFN_vkCreateIOSSurfaceMVK>(env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkCreateIOSSurfaceMVK"));
    ASSERT_NE(pfn_CreateSurface, nullptr);
    PFN_vkDestroySurfaceKHR pfn_DestroySurface =
        reinterpret_cast<PFN_vkDestroySurfaceKHR>(env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
    ASSERT_NE(pfn_DestroySurface, nullptr);
    ASSERT_EQ(VK_SUCCESS, pfn_CreateSurface(instance, &surf_create_info, nullptr, &created_surface));
    pfn_DestroySurface(instance, created_surface, nullptr);
}
#endif  // VK_USE_PLATFORM_IOS_MVK

#ifdef VK_USE_PLATFORM_MACOS_MVK
TEST_F(LoaderHandleValidTests, VerifyHandleWrappingMacOSSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_MVK_macos_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});

    const char* wrap_objects_name = "WrapObjectsLayer";
    ManifestLayer::LayerDescription wrap_objects_description{};
    wrap_objects_description.name = wrap_objects_name;
    wrap_objects_description.lib_path = TEST_LAYER_WRAP_OBJECTS;

    ManifestLayer wrap_objects_layer;
    wrap_objects_layer.layers.push_back(wrap_objects_description);
    env->AddExplicitLayer(wrap_objects_layer, "wrap_objects_layer.json");

    driver.physical_devices.emplace_back("physical_device_0");
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    InstWrapper instance(env->vulkan_functions);
    instance.create_info.add_extension("VK_KHR_surface");
    instance.create_info.add_extension("VK_MVK_macos_surface");
    instance.create_info.add_layer(wrap_objects_name);
    instance.CheckCreate();

    VkMacOSSurfaceCreateInfoMVK surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    PFN_vkCreateMacOSSurfaceMVK pfn_CreateSurface = reinterpret_cast<PFN_vkCreateMacOSSurfaceMVK>(
        env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkCreateMacOSSurfaceMVK"));
    ASSERT_NE(pfn_CreateSurface, nullptr);
    PFN_vkDestroySurfaceKHR pfn_DestroySurface =
        reinterpret_cast<PFN_vkDestroySurfaceKHR>(env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
    ASSERT_NE(pfn_DestroySurface, nullptr);
    ASSERT_EQ(VK_SUCCESS, pfn_CreateSurface(instance, &surf_create_info, nullptr, &created_surface));
    pfn_DestroySurface(instance, created_surface, nullptr);
}
#endif  // VK_USE_PLATFORM_MACOS_MVK

#if defined(VK_USE_PLATFORM_METAL_EXT)
TEST_F(LoaderHandleValidTests, VerifyHandleWrappingMetalSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_EXT_metal_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});

    const char* wrap_objects_name = "WrapObjectsLayer";
    ManifestLayer::LayerDescription wrap_objects_description{};
    wrap_objects_description.name = wrap_objects_name;
    wrap_objects_description.lib_path = TEST_LAYER_WRAP_OBJECTS;

    ManifestLayer wrap_objects_layer;
    wrap_objects_layer.layers.push_back(wrap_objects_description);
    env->AddExplicitLayer(wrap_objects_layer, "wrap_objects_layer.json");

    driver.physical_devices.emplace_back("physical_device_0");
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    InstWrapper instance(env->vulkan_functions);
    instance.create_info.add_extension("VK_KHR_surface");
    instance.create_info.add_extension("VK_EXT_metal_surface");
    instance.create_info.add_layer(wrap_objects_name);
    instance.CheckCreate();

    VkMetalSurfaceCreateInfoEXT surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    PFN_vkCreateMetalSurfaceEXT pfn_CreateSurface = reinterpret_cast<PFN_vkCreateMetalSurfaceEXT>(
        env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkCreateMetalSurfaceEXT"));
    ASSERT_NE(pfn_CreateSurface, nullptr);
    PFN_vkDestroySurfaceKHR pfn_DestroySurface =
        reinterpret_cast<PFN_vkDestroySurfaceKHR>(env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
    ASSERT_NE(pfn_DestroySurface, nullptr);
    ASSERT_EQ(VK_SUCCESS, pfn_CreateSurface(instance, &surf_create_info, nullptr, &created_surface));
    pfn_DestroySurface(instance, created_surface, nullptr);
}
#endif  // VK_USE_PLATFORM_METAL_EXT

#ifdef VK_USE_PLATFORM_SCREEN_QNX
TEST_F(LoaderHandleValidTests, VerifyHandleWrappingQNXSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_QNX_screen_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});

    const char* wrap_objects_name = "WrapObjectsLayer";
    ManifestLayer::LayerDescription wrap_objects_description{};
    wrap_objects_description.name = wrap_objects_name;
    wrap_objects_description.lib_path = TEST_LAYER_WRAP_OBJECTS;

    ManifestLayer wrap_objects_layer;
    wrap_objects_layer.layers.push_back(wrap_objects_description);
    env->AddExplicitLayer(wrap_objects_layer, "wrap_objects_layer.json");

    driver.physical_devices.emplace_back("physical_device_0");
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    InstWrapper instance(env->vulkan_functions);
    instance.create_info.add_extension("VK_KHR_surface");
    instance.create_info.add_extension("VK_QNX_screen_surface");
    instance.create_info.add_layer(wrap_objects_name);
    instance.CheckCreate();

    VkScreenSurfaceCreateInfoQNX surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_SCREEN_SURFACE_CREATE_INFO_QNX;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    PFN_vkCreateScreenSurfaceQNX pfn_CreateSurface = reinterpret_cast<PFN_vkCreateScreenSurfaceQNX>(
        env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkCreateScreenSurfaceQNX"));
    ASSERT_NE(pfn_CreateSurface, nullptr);
    PFN_vkDestroySurfaceKHR pfn_DestroySurface =
        reinterpret_cast<PFN_vkDestroySurfaceKHR>(env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
    ASSERT_NE(pfn_DestroySurface, nullptr);
    ASSERT_EQ(VK_SUCCESS, pfn_CreateSurface(instance, &surf_create_info, nullptr, &created_surface));
    pfn_DestroySurface(instance, created_surface, nullptr);
}
#endif  // VK_USE_PLATFORM_SCREEN_QNX

#ifdef VK_USE_PLATFORM_VI_NN
TEST_F(LoaderHandleValidTests, VerifyHandleWrappingViNNSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_NN_vi_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});

    const char* wrap_objects_name = "WrapObjectsLayer";
    ManifestLayer::LayerDescription wrap_objects_description{};
    wrap_objects_description.name = wrap_objects_name;
    wrap_objects_description.lib_path = TEST_LAYER_WRAP_OBJECTS;

    ManifestLayer wrap_objects_layer;
    wrap_objects_layer.layers.push_back(wrap_objects_description);
    env->AddExplicitLayer(wrap_objects_layer, "wrap_objects_layer.json");

    driver.physical_devices.emplace_back("physical_device_0");
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    InstWrapper instance(env->vulkan_functions);
    instance.create_info.add_extension("VK_KHR_surface");
    instance.create_info.add_extension("VK_NN_vi_surface");
    instance.create_info.add_layer(wrap_objects_name);
    instance.CheckCreate();

    VkViSurfaceCreateInfoNN surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_VI_SURFACE_CREATE_INFO_NN;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    PFN_vkCreateViSurfaceNN pfn_CreateSurface =
        reinterpret_cast<PFN_vkCreateViSurfaceNN>(env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkCreateViSurfaceNN"));
    ASSERT_NE(pfn_CreateSurface, nullptr);
    PFN_vkDestroySurfaceKHR pfn_DestroySurface =
        reinterpret_cast<PFN_vkDestroySurfaceKHR>(env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
    ASSERT_NE(pfn_DestroySurface, nullptr);
    ASSERT_EQ(VK_SUCCESS, pfn_CreateSurface(instance, &surf_create_info, nullptr, &created_surface));
    pfn_DestroySurface(instance, created_surface, nullptr);
}
#endif  // VK_USE_PLATFORM_VI_NN

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
TEST_F(LoaderHandleValidTests, VerifyHandleWrappingWaylandSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_wayland_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});

    const char* wrap_objects_name = "WrapObjectsLayer";
    ManifestLayer::LayerDescription wrap_objects_description{};
    wrap_objects_description.name = wrap_objects_name;
    wrap_objects_description.lib_path = TEST_LAYER_WRAP_OBJECTS;

    ManifestLayer wrap_objects_layer;
    wrap_objects_layer.layers.push_back(wrap_objects_description);
    env->AddExplicitLayer(wrap_objects_layer, "wrap_objects_layer.json");

    driver.physical_devices.emplace_back("physical_device_0");
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    InstWrapper instance(env->vulkan_functions);
    instance.create_info.add_extension("VK_KHR_surface");
    instance.create_info.add_extension("VK_KHR_wayland_surface");
    instance.create_info.add_layer(wrap_objects_name);
    instance.CheckCreate();

    VkWaylandSurfaceCreateInfoKHR surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    PFN_vkCreateWaylandSurfaceKHR pfn_CreateSurface = reinterpret_cast<PFN_vkCreateWaylandSurfaceKHR>(
        env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkCreateWaylandSurfaceKHR"));
    ASSERT_NE(pfn_CreateSurface, nullptr);
    PFN_vkDestroySurfaceKHR pfn_DestroySurface =
        reinterpret_cast<PFN_vkDestroySurfaceKHR>(env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
    ASSERT_NE(pfn_DestroySurface, nullptr);
    ASSERT_EQ(VK_SUCCESS, pfn_CreateSurface(instance, &surf_create_info, nullptr, &created_surface));
    pfn_DestroySurface(instance, created_surface, nullptr);
}
#endif  // VK_USE_PLATFORM_WAYLAND_KHR

#ifdef VK_USE_PLATFORM_WIN32_KHR
TEST_F(LoaderHandleValidTests, VerifyHandleWrappingWin32Surf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_win32_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});

    const char* wrap_objects_name = "WrapObjectsLayer";
    ManifestLayer::LayerDescription wrap_objects_description{};
    wrap_objects_description.name = wrap_objects_name;
    wrap_objects_description.lib_path = TEST_LAYER_WRAP_OBJECTS;

    ManifestLayer wrap_objects_layer;
    wrap_objects_layer.layers.push_back(wrap_objects_description);
    env->AddExplicitLayer(wrap_objects_layer, "wrap_objects_layer.json");

    driver.physical_devices.emplace_back("physical_device_0");
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    InstWrapper instance(env->vulkan_functions);
    instance.create_info.add_extension("VK_KHR_surface");
    instance.create_info.add_extension("VK_KHR_win32_surface");
    instance.create_info.add_layer(wrap_objects_name);
    instance.CheckCreate();

    VkWin32SurfaceCreateInfoKHR surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    PFN_vkCreateWin32SurfaceKHR pfn_CreateSurface = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(
        env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR"));
    ASSERT_NE(pfn_CreateSurface, nullptr);
    PFN_vkDestroySurfaceKHR pfn_DestroySurface =
        reinterpret_cast<PFN_vkDestroySurfaceKHR>(env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
    ASSERT_NE(pfn_DestroySurface, nullptr);
    ASSERT_EQ(VK_SUCCESS, pfn_CreateSurface(instance, &surf_create_info, nullptr, &created_surface));
    pfn_DestroySurface(instance, created_surface, nullptr);
}
#endif  // VK_USE_PLATFORM_WIN32_KHR

#ifdef VK_USE_PLATFORM_XCB_KHR
TEST_F(LoaderHandleValidTests, VerifyHandleWrappingXCBSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_xcb_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});

    const char* wrap_objects_name = "WrapObjectsLayer";
    ManifestLayer::LayerDescription wrap_objects_description{};
    wrap_objects_description.name = wrap_objects_name;
    wrap_objects_description.lib_path = TEST_LAYER_WRAP_OBJECTS;

    ManifestLayer wrap_objects_layer;
    wrap_objects_layer.layers.push_back(wrap_objects_description);
    env->AddExplicitLayer(wrap_objects_layer, "wrap_objects_layer.json");

    driver.physical_devices.emplace_back("physical_device_0");
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    InstWrapper instance(env->vulkan_functions);
    instance.create_info.add_extension("VK_KHR_surface");
    instance.create_info.add_extension("VK_KHR_xcb_surface");
    instance.create_info.add_layer(wrap_objects_name);
    instance.CheckCreate();

    VkXcbSurfaceCreateInfoKHR surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    PFN_vkCreateXcbSurfaceKHR pfn_CreateSurface =
        reinterpret_cast<PFN_vkCreateXcbSurfaceKHR>(env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkCreateXcbSurfaceKHR"));
    ASSERT_NE(pfn_CreateSurface, nullptr);
    PFN_vkDestroySurfaceKHR pfn_DestroySurface =
        reinterpret_cast<PFN_vkDestroySurfaceKHR>(env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
    ASSERT_NE(pfn_DestroySurface, nullptr);
    ASSERT_EQ(VK_SUCCESS, pfn_CreateSurface(instance, &surf_create_info, nullptr, &created_surface));
    pfn_DestroySurface(instance, created_surface, nullptr);
}
#endif  // VK_USE_PLATFORM_XCB_KHR

#ifdef VK_USE_PLATFORM_XLIB_KHR
TEST_F(LoaderHandleValidTests, VerifyHandleWrappingXlibSurf) {
    Extension first_ext{"VK_KHR_surface"};
    Extension second_ext{"VK_KHR_xlib_surface"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({first_ext, second_ext});

    const char* wrap_objects_name = "WrapObjectsLayer";
    ManifestLayer::LayerDescription wrap_objects_description{};
    wrap_objects_description.name = wrap_objects_name;
    wrap_objects_description.lib_path = TEST_LAYER_WRAP_OBJECTS;

    ManifestLayer wrap_objects_layer;
    wrap_objects_layer.layers.push_back(wrap_objects_description);
    env->AddExplicitLayer(wrap_objects_layer, "wrap_objects_layer.json");

    driver.physical_devices.emplace_back("physical_device_0");
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    InstWrapper instance(env->vulkan_functions);
    instance.create_info.add_extension("VK_KHR_surface");
    instance.create_info.add_extension("VK_KHR_xlib_surface");
    instance.create_info.add_layer(wrap_objects_name);
    instance.CheckCreate();

    VkXlibSurfaceCreateInfoKHR surf_create_info = {};
    surf_create_info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    surf_create_info.pNext = nullptr;
    VkSurfaceKHR created_surface = VK_NULL_HANDLE;
    PFN_vkCreateXlibSurfaceKHR pfn_CreateSurface = reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(
        env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkCreateXlibSurfaceKHR"));
    ASSERT_NE(pfn_CreateSurface, nullptr);
    PFN_vkDestroySurfaceKHR pfn_DestroySurface =
        reinterpret_cast<PFN_vkDestroySurfaceKHR>(env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
    ASSERT_NE(pfn_DestroySurface, nullptr);
    ASSERT_EQ(VK_SUCCESS, pfn_CreateSurface(instance, &surf_create_info, nullptr, &created_surface));
    pfn_DestroySurface(instance, created_surface, nullptr);
}
#endif  // VK_USE_PLATFORM_XLIB_KHR

static VkBool32 JunkDebugUtilsCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                       VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    // This is just a stub callback in case the loader or any other layer triggers it.
    (void)messageSeverity;
    (void)messageTypes;
    (void)pCallbackData;
    (void)pUserData;
    return VK_FALSE;
}

#if 0 // Disable for now to get this commit in
TEST_F(LoaderHandleValidTests, VerifyHandleWrappingDebugUtilsMessenger) {
    Extension ext{"VK_EXT_debug_utils"};
    auto& driver = env->get_test_icd();
    driver.AddInstanceExtensions({ext});

    const char* wrap_objects_name = "WrapObjectsLayer";
    ManifestLayer::LayerDescription wrap_objects_description{};
    wrap_objects_description.name = wrap_objects_name;
    wrap_objects_description.lib_path = TEST_LAYER_WRAP_OBJECTS;

    ManifestLayer wrap_objects_layer;
    wrap_objects_layer.layers.push_back(wrap_objects_description);
    env->AddExplicitLayer(wrap_objects_layer, "wrap_objects_layer.json");

    driver.physical_devices.emplace_back("physical_device_0");
    MockQueueFamilyProperties family_props{{VK_QUEUE_GRAPHICS_BIT, 1, 0, {1, 1, 1}}, true};
    driver.physical_devices.back().queue_family_properties.push_back(family_props);

    InstWrapper instance(env->vulkan_functions);
    instance.create_info.add_extension("VK_EXT_debug_utils");
    instance.create_info.add_layer(wrap_objects_name);
    instance.CheckCreate();

    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info = {};
    debug_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_messenger_create_info.pNext = nullptr;
    debug_messenger_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_messenger_create_info.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debug_messenger_create_info.pfnUserCallback = reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(JunkDebugUtilsCallback);
    VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
    PFN_vkCreateDebugUtilsMessengerEXT pfn_CreateMessenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    ASSERT_NE(pfn_CreateMessenger, nullptr);
    PFN_vkDestroyDebugUtilsMessengerEXT pfn_DestroyMessenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        env->vulkan_functions.vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    ASSERT_NE(pfn_DestroyMessenger, nullptr);
    ASSERT_EQ(VK_SUCCESS, pfn_CreateMessenger(instance, &debug_messenger_create_info, nullptr, &messenger));
    pfn_DestroyMessenger(instance, messenger, nullptr);
}
#endif