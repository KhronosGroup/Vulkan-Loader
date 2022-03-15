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

#include "test_environment.h"

class WsiTests : public ::testing::Test {
   protected:
    virtual void SetUp() {
        env = std::unique_ptr<FrameworkEnvironment>(new FrameworkEnvironment());
        env->add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_6));
    }

    virtual void TearDown() { env.reset(); }
    std::unique_ptr<FrameworkEnvironment> env;

    int width = 100;
    int height = 100;
};

#if defined(VK_USE_PLATFORM_WIN32_KHR)

// When ICD doesn't support the extension, create instance should fail
TEST_F(WsiTests, CreateSurfaceWin32NoICDSupport) {
    auto& driver = env->get_test_icd();
    driver.set_icd_api_version(VK_API_VERSION_1_0);
    driver.set_min_icd_interface_version(5);
    driver.enable_icd_wsi = false;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extensions({VK_KHR_WIN32_SURFACE_EXTENSION_NAME});
    inst.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    InstWrapper inst2{env->vulkan_functions};
    inst2.CheckCreate();

    ASSERT_EQ(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst2.inst, "vkCreateWin32SurfaceKHR"));
}

// When ICD doesn't support the surface creation, the loader should handle it
TEST_F(WsiTests, CreateSurfaceWin32NoICDCreateSupport) {
    auto& driver = env->get_test_icd();
    driver.set_icd_api_version(VK_API_VERSION_1_0);
    driver.set_min_icd_interface_version(5);
    driver.add_instance_extension({VK_KHR_SURFACE_EXTENSION_NAME});
    driver.add_instance_extension({VK_KHR_WIN32_SURFACE_EXTENSION_NAME});
    driver.enable_icd_wsi = false;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME});
    inst.CheckCreate();

    VkSurfaceKHR surface{VK_NULL_HANDLE};
    VkWin32SurfaceCreateInfoKHR surf_create_info{VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkCreateWin32SurfaceKHR(inst, &surf_create_info, nullptr, &surface));
    ASSERT_TRUE(surface != VK_NULL_HANDLE);
    //    ASSERT_EQ(driver.is_using_icd_wsi, UsingICDProvidedWSI::not_using);

    env->vulkan_functions.vkDestroySurfaceKHR(inst, surface, nullptr);
}

// When ICD does support the surface creation, the loader should  delegat handle it to the ICD
TEST_F(WsiTests, CreateSurfaceWin32ICDSupport) {
    auto& driver = env->get_test_icd();
    driver.set_icd_api_version(VK_API_VERSION_1_0);
    driver.set_min_icd_interface_version(5);
    driver.add_instance_extension({VK_KHR_SURFACE_EXTENSION_NAME});
    driver.add_instance_extension({VK_KHR_WIN32_SURFACE_EXTENSION_NAME});
    driver.enable_icd_wsi = true;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME});
    inst.CheckCreate();

    VkSurfaceKHR surface{VK_NULL_HANDLE};
    VkWin32SurfaceCreateInfoKHR surf_create_info{VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkCreateWin32SurfaceKHR(inst, &surf_create_info, nullptr, &surface));
    ASSERT_TRUE(surface != VK_NULL_HANDLE);
    //    ASSERT_EQ(driver.is_using_icd_wsi, UsingICDProvidedWSI::not_using);

    env->vulkan_functions.vkDestroySurfaceKHR(inst, surface, nullptr);
}

// Some drivers supporting vkCreateWin32SurfaceKHR, and at least one that doesn't
TEST_F(WsiTests, CreateSurfaceWin32MixedICDSupport) {
    FrameworkEnvironment env{};
    for (uint32_t icd = 0; icd < 3; ++icd) {
        Extension first_ext{VK_KHR_SURFACE_EXTENSION_NAME};
        Extension second_ext{VK_KHR_WIN32_SURFACE_EXTENSION_NAME};
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_0;
        cur_icd.add_instance_extensions({first_ext, second_ext});
        if (icd < 2) {
            // Only enable ICD for first two
            cur_icd.enable_icd_wsi = true;
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME});
    instance.CheckCreate();

    VkSurfaceKHR surface{VK_NULL_HANDLE};
    VkWin32SurfaceCreateInfoKHR surf_create_info{VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkCreateWin32SurfaceKHR(instance.inst, &surf_create_info, nullptr, &surface));
    ASSERT_TRUE(surface != VK_NULL_HANDLE);
    //    ASSERT_EQ(driver.is_using_icd_wsi, UsingICDProvidedWSI::not_using);

    env.vulkan_functions.vkDestroySurfaceKHR(instance.inst, surface, nullptr);
}

TEST_F(WsiTests, GetPhysicalDeviceWin32PresentNoICDSupport) {
    auto& driver = env->get_test_icd();
    driver.set_icd_api_version(VK_API_VERSION_1_0);
    driver.set_min_icd_interface_version(5);
    driver.add_instance_extension({VK_KHR_SURFACE_EXTENSION_NAME});
    driver.add_instance_extension({VK_KHR_WIN32_SURFACE_EXTENSION_NAME});
    driver.physical_devices.emplace_back("physical_device_0");
    driver.enable_icd_wsi = false;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME});
    inst.CheckCreate();

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceWin32PresentationSupportKHR(physical_device, 0),
                 "ICD for selected physical device does not export vkGetPhysicalDeviceWin32PresentationSupportKHR!");
}

TEST_F(WsiTests, GetPhysicalDeviceWin32PresentICDSupport) {
    auto& driver = env->get_test_icd();
    driver.set_icd_api_version(VK_API_VERSION_1_0);
    driver.set_min_icd_interface_version(5);
    driver.add_instance_extension({VK_KHR_SURFACE_EXTENSION_NAME});
    driver.add_instance_extension({VK_KHR_WIN32_SURFACE_EXTENSION_NAME});
    driver.physical_devices.emplace_back("physical_device_0");
    driver.enable_icd_wsi = true;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME});
    inst.CheckCreate();

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    ASSERT_EQ(VK_TRUE, env->vulkan_functions.vkGetPhysicalDeviceWin32PresentationSupportKHR(physical_device, 0));
}
#endif

#if defined(VK_USE_PLATFORM_XCB_KHR)
// When ICD doesn't support the extension, create instance should fail
TEST_F(WsiTests, CreateSurfaceXCBNoICDSupport) {
    auto& driver = env->get_test_icd();
    driver.set_icd_api_version(VK_API_VERSION_1_0);
    driver.set_min_icd_interface_version(5);
    driver.enable_icd_wsi = false;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extensions({VK_KHR_XCB_SURFACE_EXTENSION_NAME});
    inst.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    InstWrapper inst2{env->vulkan_functions};
    inst2.CheckCreate();

    ASSERT_EQ(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst2.inst, "vkCreateXcbSurfaceKHR"));
}

// When ICD doesn't support the surface creation, the loader should handle it
TEST_F(WsiTests, CreateSurfaceXCBNoICDCreateSupport) {
    auto& driver = env->get_test_icd();
    driver.set_icd_api_version(VK_API_VERSION_1_0);
    driver.set_min_icd_interface_version(5);
    driver.add_instance_extension({VK_KHR_SURFACE_EXTENSION_NAME});
    driver.add_instance_extension({VK_KHR_XCB_SURFACE_EXTENSION_NAME});
    driver.enable_icd_wsi = false;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XCB_SURFACE_EXTENSION_NAME});
    inst.CheckCreate();

    VkXcbSurfaceCreateInfoKHR xcb_createInfo{VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR};

    VkSurfaceKHR surface{VK_NULL_HANDLE};
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkCreateXcbSurfaceKHR(inst, &xcb_createInfo, nullptr, &surface));
    ASSERT_TRUE(surface != VK_NULL_HANDLE);

    env->vulkan_functions.vkDestroySurfaceKHR(inst, surface, nullptr);
}

// When ICD does support the surface creation, the loader should  delegat handle it to the ICD
TEST_F(WsiTests, CreateSurfaceXCBICDSupport) {
    auto& driver = env->get_test_icd();
    driver.set_icd_api_version(VK_API_VERSION_1_0);
    driver.set_min_icd_interface_version(5);
    driver.add_instance_extension({VK_KHR_SURFACE_EXTENSION_NAME});
    driver.add_instance_extension({VK_KHR_XCB_SURFACE_EXTENSION_NAME});
    driver.enable_icd_wsi = true;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XCB_SURFACE_EXTENSION_NAME});
    inst.CheckCreate();

    VkXcbSurfaceCreateInfoKHR xcb_createInfo{VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR};

    VkSurfaceKHR surface{VK_NULL_HANDLE};
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkCreateXcbSurfaceKHR(inst, &xcb_createInfo, nullptr, &surface));
    ASSERT_TRUE(surface != VK_NULL_HANDLE);

    env->vulkan_functions.vkDestroySurfaceKHR(inst, surface, nullptr);
}

// Some drivers supporting vkCreateXcbSurfaceKHR, and at least one that doesn't
TEST_F(WsiTests, CreateSurfaceXCBMixedICDSupport) {
    FrameworkEnvironment env{};
    for (uint32_t icd = 0; icd < 3; ++icd) {
        Extension first_ext{VK_KHR_SURFACE_EXTENSION_NAME};
        Extension second_ext{VK_KHR_XCB_SURFACE_EXTENSION_NAME};
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_0;
        cur_icd.add_instance_extensions({first_ext, second_ext});
        if (icd < 2) {
            // Only enable ICD for first two
            cur_icd.enable_icd_wsi = true;
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XCB_SURFACE_EXTENSION_NAME});
    instance.CheckCreate();

    VkXcbSurfaceCreateInfoKHR xcb_createInfo{VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR};

    VkSurfaceKHR surface{VK_NULL_HANDLE};
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkCreateXcbSurfaceKHR(instance.inst, &xcb_createInfo, nullptr, &surface));
    ASSERT_TRUE(surface != VK_NULL_HANDLE);

    env.vulkan_functions.vkDestroySurfaceKHR(instance.inst, surface, nullptr);
}

TEST_F(WsiTests, GetPhysicalDeviceXcbPresentNoICDSupport) {
    auto& driver = env->get_test_icd();
    driver.set_icd_api_version(VK_API_VERSION_1_0);
    driver.set_min_icd_interface_version(5);
    driver.add_instance_extension({VK_KHR_SURFACE_EXTENSION_NAME});
    driver.add_instance_extension({VK_KHR_XCB_SURFACE_EXTENSION_NAME});
    driver.physical_devices.emplace_back("physical_device_0");
    driver.enable_icd_wsi = false;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XCB_SURFACE_EXTENSION_NAME});
    inst.CheckCreate();

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceXcbPresentationSupportKHR(physical_device, 0, nullptr, 0),
                 "ICD for selected physical device does not exportvkGetPhysicalDeviceXcbPresentationSupportKHR!");
}

TEST_F(WsiTests, GetPhysicalDeviceXcbPresentICDSupport) {
    auto& driver = env->get_test_icd();
    driver.set_icd_api_version(VK_API_VERSION_1_0);
    driver.set_min_icd_interface_version(5);
    driver.add_instance_extension({VK_KHR_SURFACE_EXTENSION_NAME});
    driver.add_instance_extension({VK_KHR_XCB_SURFACE_EXTENSION_NAME});
    driver.physical_devices.emplace_back("physical_device_0");
    driver.enable_icd_wsi = true;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XCB_SURFACE_EXTENSION_NAME});
    inst.CheckCreate();

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    ASSERT_EQ(VK_TRUE, env->vulkan_functions.vkGetPhysicalDeviceXcbPresentationSupportKHR(physical_device, 0, nullptr, 0));
}
#endif

#if defined(VK_USE_PLATFORM_XLIB_KHR)
// When ICD doesn't support the extension, create instance should fail
TEST_F(WsiTests, CreateSurfaceXLIBNoICDSupport) {
    auto& driver = env->get_test_icd();
    driver.set_icd_api_version(VK_API_VERSION_1_0);
    driver.set_min_icd_interface_version(5);
    driver.enable_icd_wsi = false;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extensions({VK_KHR_XLIB_SURFACE_EXTENSION_NAME});
    inst.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    InstWrapper inst2{env->vulkan_functions};
    inst2.CheckCreate();

    ASSERT_EQ(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst2.inst, "vkCreateXlibSurfaceKHR"));
}

// When ICD doesn't support the surface creation, the loader should handle it
TEST_F(WsiTests, CreateSurfaceXLIBNoICDCreateSupport) {
    auto& driver = env->get_test_icd();
    driver.set_icd_api_version(VK_API_VERSION_1_0);
    driver.set_min_icd_interface_version(5);
    driver.add_instance_extension({VK_KHR_SURFACE_EXTENSION_NAME});
    driver.add_instance_extension({VK_KHR_XLIB_SURFACE_EXTENSION_NAME});
    driver.enable_icd_wsi = false;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XLIB_SURFACE_EXTENSION_NAME});
    inst.CheckCreate();

    VkXlibSurfaceCreateInfoKHR createInfo{VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR};

    VkSurfaceKHR surface;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkCreateXlibSurfaceKHR(inst, &createInfo, nullptr, &surface));
    ASSERT_TRUE(surface != VK_NULL_HANDLE);

    env->vulkan_functions.vkDestroySurfaceKHR(inst, surface, nullptr);
}

// When ICD does support the surface creation, the loader should  delegat handle it to the ICD
TEST_F(WsiTests, CreateSurfaceXLIBICDSupport) {
    auto& driver = env->get_test_icd();
    driver.set_icd_api_version(VK_API_VERSION_1_0);
    driver.set_min_icd_interface_version(5);
    driver.add_instance_extension({VK_KHR_SURFACE_EXTENSION_NAME});
    driver.add_instance_extension({VK_KHR_XLIB_SURFACE_EXTENSION_NAME});
    driver.enable_icd_wsi = true;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XLIB_SURFACE_EXTENSION_NAME});
    inst.CheckCreate();

    VkXlibSurfaceCreateInfoKHR createInfo{VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR};

    VkSurfaceKHR surface;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkCreateXlibSurfaceKHR(inst, &createInfo, nullptr, &surface));
    ASSERT_TRUE(surface != VK_NULL_HANDLE);

    env->vulkan_functions.vkDestroySurfaceKHR(inst, surface, nullptr);
}

// Some drivers supporting vkCreateXlibSurfaceKHR, and at least one that doesn't
TEST_F(WsiTests, CreateSurfaceXLIBMixedICDSupport) {
    FrameworkEnvironment env{};
    for (uint32_t icd = 0; icd < 3; ++icd) {
        Extension first_ext{VK_KHR_SURFACE_EXTENSION_NAME};
        Extension second_ext{VK_KHR_XLIB_SURFACE_EXTENSION_NAME};
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_0;
        cur_icd.add_instance_extensions({first_ext, second_ext});
        if (icd < 2) {
            // Only enable ICD for first two
            cur_icd.enable_icd_wsi = true;
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XLIB_SURFACE_EXTENSION_NAME});
    instance.CheckCreate();

    VkXlibSurfaceCreateInfoKHR createInfo{VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR};

    VkSurfaceKHR surface;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkCreateXlibSurfaceKHR(instance.inst, &createInfo, nullptr, &surface));
    ASSERT_TRUE(surface != VK_NULL_HANDLE);

    env.vulkan_functions.vkDestroySurfaceKHR(instance.inst, surface, nullptr);
}

TEST_F(WsiTests, GetPhysicalDeviceXlibPresentNoICDSupport) {
    auto& driver = env->get_test_icd();
    driver.set_icd_api_version(VK_API_VERSION_1_0);
    driver.set_min_icd_interface_version(5);
    driver.add_instance_extension({VK_KHR_SURFACE_EXTENSION_NAME});
    driver.add_instance_extension({VK_KHR_XLIB_SURFACE_EXTENSION_NAME});
    driver.physical_devices.emplace_back("physical_device_0");
    driver.enable_icd_wsi = false;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XLIB_SURFACE_EXTENSION_NAME});
    inst.CheckCreate();

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceXlibPresentationSupportKHR(physical_device, 0, nullptr, 0),
                 "ICD for selected physical device does not exportvkGetPhysicalDeviceXlibPresentationSupportKHR!");
}

TEST_F(WsiTests, GetPhysicalDeviceXlibPresentICDSupport) {
    auto& driver = env->get_test_icd();
    driver.set_icd_api_version(VK_API_VERSION_1_0);
    driver.set_min_icd_interface_version(5);
    driver.add_instance_extension({VK_KHR_SURFACE_EXTENSION_NAME});
    driver.add_instance_extension({VK_KHR_XLIB_SURFACE_EXTENSION_NAME});
    driver.physical_devices.emplace_back("physical_device_0");
    driver.enable_icd_wsi = true;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XLIB_SURFACE_EXTENSION_NAME});
    inst.CheckCreate();

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    ASSERT_EQ(VK_TRUE, env->vulkan_functions.vkGetPhysicalDeviceXlibPresentationSupportKHR(physical_device, 0, nullptr, 0));
}
#endif

#if defined(VK_USE_PLATFORM_WAYLAND_KHR)
// When ICD doesn't support the extension, create instance should fail
TEST_F(WsiTests, CreateSurfaceWaylandNoICDSupport) {
    auto& driver = env->get_test_icd();
    driver.set_icd_api_version(VK_API_VERSION_1_0);
    driver.set_min_icd_interface_version(5);
    driver.enable_icd_wsi = false;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extensions({VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME});
    inst.CheckCreate(VK_ERROR_EXTENSION_NOT_PRESENT);

    InstWrapper inst2{env->vulkan_functions};
    inst2.CheckCreate();

    ASSERT_EQ(nullptr, env->vulkan_functions.vkGetInstanceProcAddr(inst2.inst, "vkCreateWaylandSurfaceKHR"));
}

// When ICD doesn't support the surface creation, the loader should handle it
TEST_F(WsiTests, CreateSurfaceWaylandNoICDCreateSupport) {
    auto& driver = env->get_test_icd();
    driver.set_icd_api_version(VK_API_VERSION_1_0);
    driver.set_min_icd_interface_version(5);
    driver.add_instance_extension({VK_KHR_SURFACE_EXTENSION_NAME});
    driver.add_instance_extension({VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME});
    driver.enable_icd_wsi = false;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME});
    inst.CheckCreate();

    VkWaylandSurfaceCreateInfoKHR createInfo{VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR};

    VkSurfaceKHR surface;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkCreateWaylandSurfaceKHR(inst, &createInfo, nullptr, &surface));
    ASSERT_TRUE(surface != VK_NULL_HANDLE);

    env->vulkan_functions.vkDestroySurfaceKHR(inst, surface, nullptr);
}

// When ICD does support the surface creation, the loader should  delegat handle it to the ICD
TEST_F(WsiTests, CreateSurfaceWaylandICDSupport) {
    auto& driver = env->get_test_icd();
    driver.set_icd_api_version(VK_API_VERSION_1_0);
    driver.set_min_icd_interface_version(5);
    driver.add_instance_extension({VK_KHR_SURFACE_EXTENSION_NAME});
    driver.add_instance_extension({VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME});
    driver.enable_icd_wsi = true;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME});
    inst.CheckCreate();

    VkWaylandSurfaceCreateInfoKHR createInfo{VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR};

    VkSurfaceKHR surface;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkCreateWaylandSurfaceKHR(inst, &createInfo, nullptr, &surface));
    ASSERT_TRUE(surface != VK_NULL_HANDLE);

    env->vulkan_functions.vkDestroySurfaceKHR(inst, surface, nullptr);
}

// Some drivers supporting vkCreateWaylandSurfaceKHR, and at least one that doesn't
TEST_F(WsiTests, CreateSurfaceWaylandMixedICDSupport) {
    FrameworkEnvironment env{};
    for (uint32_t icd = 0; icd < 3; ++icd) {
        Extension first_ext{VK_KHR_SURFACE_EXTENSION_NAME};
        Extension second_ext{VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME};
        env.add_icd(TestICDDetails(TEST_ICD_PATH_VERSION_2));
        auto& cur_icd = env.get_test_icd(icd);
        cur_icd.icd_api_version = VK_API_VERSION_1_0;
        cur_icd.add_instance_extensions({first_ext, second_ext});
        if (icd < 2) {
            // Only enable ICD for first two
            cur_icd.enable_icd_wsi = true;
        }
    }

    InstWrapper instance(env.vulkan_functions);
    instance.create_info.add_extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME});
    instance.CheckCreate();

    VkWaylandSurfaceCreateInfoKHR createInfo{VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR};

    VkSurfaceKHR surface;
    ASSERT_EQ(VK_SUCCESS, env.vulkan_functions.vkCreateWaylandSurfaceKHR(instance.inst, &createInfo, nullptr, &surface));
    ASSERT_TRUE(surface != VK_NULL_HANDLE);

    env.vulkan_functions.vkDestroySurfaceKHR(instance.inst, surface, nullptr);
}

TEST_F(WsiTests, GetPhysicalDeviceWaylandPresentNoICDSupport) {
    auto& driver = env->get_test_icd();
    driver.set_icd_api_version(VK_API_VERSION_1_0);
    driver.set_min_icd_interface_version(5);
    driver.add_instance_extension({VK_KHR_SURFACE_EXTENSION_NAME});
    driver.add_instance_extension({VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME});
    driver.physical_devices.emplace_back("physical_device_0");
    driver.enable_icd_wsi = false;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME});
    inst.CheckCreate();

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    ASSERT_DEATH(env->vulkan_functions.vkGetPhysicalDeviceWaylandPresentationSupportKHR(physical_device, 0, nullptr),
                 "ICD for selected physical device does not exportvkGetPhysicalDeviceWaylandPresentationSupportKHR!");
}

TEST_F(WsiTests, GetPhysicalDeviceWaylandPresentICDSupport) {
    auto& driver = env->get_test_icd();
    driver.set_icd_api_version(VK_API_VERSION_1_0);
    driver.set_min_icd_interface_version(5);
    driver.add_instance_extension({VK_KHR_SURFACE_EXTENSION_NAME});
    driver.add_instance_extension({VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME});
    driver.physical_devices.emplace_back("physical_device_0");
    driver.enable_icd_wsi = true;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info.add_extensions({VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME});
    inst.CheckCreate();

    uint32_t driver_count = 1;
    VkPhysicalDevice physical_device;
    ASSERT_EQ(VK_SUCCESS, inst->vkEnumeratePhysicalDevices(inst, &driver_count, &physical_device));
    ASSERT_EQ(driver_count, 1);

    ASSERT_EQ(VK_TRUE, env->vulkan_functions.vkGetPhysicalDeviceWaylandPresentationSupportKHR(physical_device, 0, nullptr));
}
#endif