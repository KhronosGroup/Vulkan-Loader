#if defined(VK_USE_PLATFORM_XCB_KHR) && defined(LOADER_ENABLE_DRM)
/*
 * Copyright © 2019 Red Hat
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/* connect to an X server and work out the default device. */

#include <xcb/xcb.h>
#include <xcb/dri3.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <xf86drm.h>

#include "loader_linux.h"
#include "log.h"

static int ds_dri3_open(xcb_connection_t *conn, xcb_window_t root, uint32_t provider) {
    xcb_dri3_open_cookie_t cookie;
    xcb_dri3_open_reply_t *reply;
    int fd;

    cookie = xcb_dri3_open(conn, root, provider);

    reply = xcb_dri3_open_reply(conn, cookie, NULL);
    if (!reply) return -1;

    if (reply->nfd != 1) {
        free(reply);
        return -1;
    }

    fd = xcb_dri3_open_reply_fds(conn, reply)[0];
    free(reply);
    fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC);

    return fd;
}

bool linux_find_xcb_default_device(struct loader_instance *inst, uint32_t device_count,
                                   struct LinuxSortedDeviceInfo *sorted_device_info) {
    const xcb_setup_t *setup;
    xcb_screen_iterator_t iter;
    int scrn;
    xcb_connection_t *conn;
    conn = xcb_connect(NULL, &scrn);
    bool found = false;

    if (!conn) {
        goto out;
    }

    loader_log(inst, VULKAN_LOADER_DEBUG_BIT | VULKAN_LOADER_DRIVER_BIT, 0, "linux_find_xcb_default_device: Checking XCB DRI info");

    xcb_query_extension_cookie_t dri3_cookie;
    xcb_query_extension_reply_t *dri3_reply;

    dri3_cookie = xcb_query_extension(conn, 4, "DRI3");
    dri3_reply = xcb_query_extension_reply(conn, dri3_cookie, NULL);

    if (!dri3_reply) {
        goto out;
    }

    if (dri3_reply->present == 0) {
        goto out;
    }
    setup = xcb_get_setup(conn);
    iter = xcb_setup_roots_iterator(setup);

    xcb_screen_t *screen = iter.data;

    int dri3_fd = ds_dri3_open(conn, screen->root, 0);
    if (dri3_fd == -1) {
        goto out;
    }

    drmDevicePtr xdev;
    int ret = drmGetDevice2(dri3_fd, 0, &xdev);
    close(dri3_fd);
    if (ret < 0) {
        goto out;
    }

    for (int32_t i = 0; i < (int32_t)device_count; i++) {
        if (sorted_device_info[i].has_pci_bus_info) {
            if (xdev->businfo.pci->domain == sorted_device_info[i].pci_domain &&
                xdev->businfo.pci->bus == sorted_device_info[i].pci_bus &&
                xdev->businfo.pci->dev == sorted_device_info[i].pci_device &&
                xdev->businfo.pci->func == sorted_device_info[i].pci_function) {
                loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                           "linux_find_xcb_default_device:  Found default at index %u \'%s\' using PCI info", i,
                           sorted_device_info[i].device_name);
                sorted_device_info[i].default_device = true;
                found = true;
                break;
            }
        } else {
            if (xdev->deviceinfo.pci->vendor_id == sorted_device_info[i].vendor_id &&
                xdev->deviceinfo.pci->device_id == sorted_device_info[i].device_id) {
                loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                           "linux_find_xcb_default_device:  Found default at index %u \'%s\' using Vendor/Device info", i,
                           sorted_device_info[i].device_name);
                sorted_device_info[i].default_device = true;
                found = true;
                break;
            }
        }
    }
out:
    if (conn) {
        xcb_disconnect(conn);
    }
    return found;
}
#endif  // defined(VK_USE_PLATFORM_XCB_KHR) && defined(LOADER_ENABLE_DRM)
