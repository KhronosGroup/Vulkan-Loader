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

#if defined(VK_USE_PLATFORM_XLIB_KHR) || defined(VK_USE_PLATFORM_XCB_KHR)
#include <X11/Xutil.h>
#endif

class RegressionTests : public ::testing::Test {
   protected:
    virtual void SetUp() { env = std::unique_ptr<SingleICDShim>(new SingleICDShim(TestICDDetails(TEST_ICD_PATH_VERSION_2))); }

    virtual void TearDown() { env.reset(); }
    std::unique_ptr<SingleICDShim> env;

    int width = 100;
    int height = 100;
};

#if defined(VK_USE_PLATFORM_WIN32_KHR)

// MS-Windows event handling function:
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return DefWindowProcA(hWnd, uMsg, wParam, lParam); }

TEST_F(RegressionTests, CreateSurfaceWin32) {
    auto& driver = env->get_test_icd();
    driver.SetICDAPIVersion(VK_MAKE_VERSION(1, 0, 0));
    driver.SetMinICDInterfaceVersion(5);
    driver.AddInstanceExtension(Extension(VK_KHR_SURFACE_EXTENSION_NAME));
    driver.AddInstanceExtension(Extension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME));
    driver.enable_icd_wsi = true;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info = driver.GetVkInstanceCreateInfo();
    inst.CheckCreate();

    WNDCLASSEX win_class;
    const char* app_short_name = "loader_surface_test";
    HINSTANCE h_instance = GetModuleHandle(nullptr);  // Windows Instance
    HWND h_wnd = nullptr;                             // window handle

    // Initialize the window class structure:
    win_class.cbSize = sizeof(WNDCLASSEX);
    win_class.style = CS_HREDRAW | CS_VREDRAW;
    win_class.lpfnWndProc = WndProc;
    win_class.cbClsExtra = 0;
    win_class.cbWndExtra = 0;
    win_class.hInstance = h_instance;
    win_class.hIcon = LoadIconA(nullptr, IDI_APPLICATION);
    win_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
    win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    win_class.lpszMenuName = nullptr;
    win_class.lpszClassName = app_short_name;
    win_class.hIconSm = LoadIconA(nullptr, IDI_WINLOGO);

    // Register window class:
    EXPECT_TRUE(RegisterClassExA(&win_class) != NULL);

    // Create window with the registered class:
    RECT wr = {0, 0, width, height};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    h_wnd = CreateWindowExA(0,
                            app_short_name,  // class name
                            app_short_name,  // app name
                            // WS_VISIBLE | WS_SYSMENU |
                            WS_OVERLAPPEDWINDOW,  // window style
                            width, height,        // x/y coords
                            wr.right - wr.left,   // width
                            wr.bottom - wr.top,   // height
                            nullptr,              // handle to parent
                            nullptr,              // handle to menu
                            h_instance,           // hInstance
                            nullptr);             // no extra parameters
    EXPECT_TRUE(h_wnd != nullptr);

    VkSurfaceKHR surface{};
    VkWin32SurfaceCreateInfoKHR surf_create_info{};
    surf_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surf_create_info.hwnd = h_wnd;
    surf_create_info.hinstance = h_instance;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkCreateWin32SurfaceKHR(inst, &surf_create_info, nullptr, &surface));
    ASSERT_TRUE(surface != VK_NULL_HANDLE);
    //    ASSERT_EQ(driver.is_using_icd_wsi, UsingICDProvidedWSI::not_using);

    env->vulkan_functions.vkDestroySurfaceKHR(inst, surface, nullptr);
    DestroyWindow(h_wnd);
}

#endif

#if defined(VK_USE_PLATFORM_XCB_KHR)
TEST_F(RegressionTests, CreateSurfaceXCB) {
    auto& driver = env->get_test_icd();
    driver.SetICDAPIVersion(VK_MAKE_VERSION(1, 0, 0));
    driver.SetMinICDInterfaceVersion(5);
    driver.AddInstanceExtension(Extension(VK_KHR_SURFACE_EXTENSION_NAME));
    driver.AddInstanceExtension(Extension(VK_KHR_XCB_SURFACE_EXTENSION_NAME));
    driver.enable_icd_wsi = true;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info = driver.GetVkInstanceCreateInfo();
    inst.CheckCreate();

    xcb_connection_t* xcb_connection;
    xcb_screen_t* xcb_screen;
    xcb_window_t xcb_window;

    //--Init Connection--
    const xcb_setup_t* setup;
    xcb_screen_iterator_t iter;
    int scr;

    // API guarantees non-null xcb_connection
    xcb_connection = xcb_connect(nullptr, &scr);
    int conn_error = xcb_connection_has_error(xcb_connection);
    ASSERT_EQ(conn_error, 0);

    setup = xcb_get_setup(xcb_connection);
    iter = xcb_setup_roots_iterator(setup);
    while (scr-- > 0) {
        xcb_screen_next(&iter);
    }

    xcb_screen = iter.data;

    xcb_window = xcb_generate_id(xcb_connection);
    xcb_create_window(xcb_connection, XCB_COPY_FROM_PARENT, xcb_window, xcb_screen->root, 0, 0, width, height, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, xcb_screen->root_visual, 0, nullptr);

    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(xcb_connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(xcb_connection, cookie, 0);
    free(reply);

    VkXcbSurfaceCreateInfoKHR xcb_createInfo;
    xcb_createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    xcb_createInfo.pNext = nullptr;
    xcb_createInfo.flags = 0;
    xcb_createInfo.connection = xcb_connection;
    xcb_createInfo.window = xcb_window;

    VkSurfaceKHR surface{};
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkCreateXcbSurfaceKHR(inst, &xcb_createInfo, nullptr, &surface));
    ASSERT_TRUE(surface != VK_NULL_HANDLE);

    env->vulkan_functions.vkDestroySurfaceKHR(inst, surface, nullptr);

    xcb_destroy_window(xcb_connection, xcb_window);
    xcb_disconnect(xcb_connection);
}
#endif

#if defined(VK_USE_PLATFORM_XLIB_KHR)
TEST_F(RegressionTests, CreateSurfaceXLIB) {
    auto& driver = env->get_test_icd();
    driver.SetICDAPIVersion(VK_MAKE_VERSION(1, 0, 0));
    driver.SetMinICDInterfaceVersion(5);
    driver.AddInstanceExtension(Extension(VK_KHR_SURFACE_EXTENSION_NAME));
    driver.AddInstanceExtension(Extension(VK_KHR_XLIB_SURFACE_EXTENSION_NAME));
    driver.enable_icd_wsi = true;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info = driver.GetVkInstanceCreateInfo();
    inst.CheckCreate();

    Display* xlib_display;
    Window xlib_window;

    long visualMask = VisualScreenMask;
    int numberOfVisuals;

    xlib_display = XOpenDisplay(nullptr);
    ASSERT_NE(xlib_display, nullptr);

    XVisualInfo vInfoTemplate = {};
    vInfoTemplate.screen = DefaultScreen(xlib_display);
    XVisualInfo* visualInfo = XGetVisualInfo(xlib_display, visualMask, &vInfoTemplate, &numberOfVisuals);
    xlib_window = XCreateWindow(xlib_display, RootWindow(xlib_display, vInfoTemplate.screen), 0, 0, width, height, 0,
                                visualInfo->depth, InputOutput, visualInfo->visual, 0, nullptr);

    XSync(xlib_display, false);
    XFree(visualInfo);

    VkXlibSurfaceCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.dpy = xlib_display;
    createInfo.window = xlib_window;

    VkSurfaceKHR surface;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkCreateXlibSurfaceKHR(inst, &createInfo, nullptr, &surface));
    ASSERT_TRUE(surface != VK_NULL_HANDLE);

    env->vulkan_functions.vkDestroySurfaceKHR(inst, surface, nullptr);

    XDestroyWindow(xlib_display, xlib_window);
    XCloseDisplay(xlib_display);
}
#endif

#if defined(VK_USE_PLATFORM_WAYLAND_KHR)

struct WaylandState {
    wl_display* display{};
    wl_registry* registry{};
    wl_compositor* compositor{};
    wl_surface* surface{};
};

static void wayland_registry_global(void* data, struct wl_registry* registry, uint32_t id, const char* interface,
                                    uint32_t version) {
    WaylandState* wayland = static_cast<WaylandState*>(data);
    if (string_eq(interface, "wl_compositor")) {
        wayland->compositor = (struct wl_compositor*)wl_registry_bind(registry, id, &wl_compositor_interface, 1);
        wayland->surface = wl_compositor_create_surface(wayland->compositor);
    }
}
static void wayland_registry_global_remove(void* data, struct wl_registry* registry, uint32_t id) {}
static const struct wl_registry_listener wayland_registry_listener = {wayland_registry_global, wayland_registry_global_remove};

TEST_F(RegressionTests, CreateSurfaceWayland) {
    auto& driver = env->get_test_icd();
    driver.SetICDAPIVersion(VK_MAKE_VERSION(1, 0, 0));
    driver.SetMinICDInterfaceVersion(5);
    driver.AddInstanceExtension(Extension(VK_KHR_SURFACE_EXTENSION_NAME));
    driver.AddInstanceExtension(Extension(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME));
    driver.enable_icd_wsi = true;

    InstWrapper inst{env->vulkan_functions};
    inst.create_info = driver.GetVkInstanceCreateInfo();
    inst.CheckCreate();

    WaylandState wayland;

    wayland.display = wl_display_connect(nullptr);
    ASSERT_NE(wayland.display, nullptr);
    wayland.registry = wl_display_get_registry(wayland.display);
    wl_registry_add_listener(wl_display_get_registry(wayland.display), &wayland_registry_listener, static_cast<void*>(&wayland));
    wl_display_roundtrip(wayland.display);
    wl_registry_destroy(wayland.registry);

    VkWaylandSurfaceCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.display = wayland.display;
    createInfo.surface = wayland.surface;

    VkSurfaceKHR surface;
    ASSERT_EQ(VK_SUCCESS, env->vulkan_functions.vkCreateWaylandSurfaceKHR(inst, &createInfo, nullptr, &surface));
    ASSERT_TRUE(surface != VK_NULL_HANDLE);

    env->vulkan_functions.vkDestroySurfaceKHR(inst, surface, nullptr);

    wl_display_disconnect(wayland.display);
}
#endif