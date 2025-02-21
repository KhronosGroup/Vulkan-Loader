/*
 *
 * Copyright (c) 2021 Google Inc.
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
 */

#include "loader_fuchsia_service.h"

#include <fcntl.h>
#include <fidl/fuchsia.io/cpp/wire.h>
#include <fidl/fuchsia.vulkan.loader/cpp/wire.h>
#include <inttypes.h>
#include <lib/component/incoming/cpp/protocol.h>
#include <lib/fdio/directory.h>
#include <lib/fdio/fd.h>
#include <lib/fdio/io.h>
#include <lib/fidl/cpp/wire/connect_service.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>
#include <zircon/status.h>
#include <zircon/syscalls.h>

#include "loader.h"
#include "log.h"

static fidl::WireSyncClient<fuchsia_vulkan_loader::Loader> vulkan_loader_svc;

static zx_handle_t device_fs = ZX_HANDLE_INVALID;

static int manifest_fs_fd = -1;

void connect_to_vulkan_loader_svc() {
    auto client_end = component::Connect<fuchsia_vulkan_loader::Loader>();
    if (!client_end.is_ok()) {
        loader_log(nullptr, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                   "connect_to_vulkan_loader_svc: Failed to connect to loader service: %s", client_end.status_string());
        return;
    }
    auto client = fidl::WireSyncClient(std::move(*client_end));
    auto feature_result = client->GetSupportedFeatures();
    fuchsia_vulkan_loader::wire::Features features;
    if (!feature_result.ok()) {
        loader_log(nullptr, VK_DEBUG_REPORT_INFORMATION_BIT_EXT, 0,
                   "connect_to_vulkan_loader_svc: Failed to get supported features, error \"%s\".",
                   feature_result.error().lossy_description());
        return;
    }
    features = feature_result.value().features;
    constexpr fuchsia_vulkan_loader::wire::Features kMandatoryFeatures =
        fuchsia_vulkan_loader::wire::Features::kConnectToDeviceFs | fuchsia_vulkan_loader::wire::Features::kConnectToManifestFs |
        fuchsia_vulkan_loader::wire::Features::kGet;
    if ((features & kMandatoryFeatures) != kMandatoryFeatures) {
        loader_log(nullptr, VK_DEBUG_REPORT_INFORMATION_BIT_EXT, 0,
                   "connect_to_vulkan_loader_svc: Missing mandatory feature 0x%" PRIx32,
                   static_cast<uint32_t>(kMandatoryFeatures & ~features));
        return;
    }
    zx::channel device_fs_client;
    {
        zx::channel device_fs_server;
        zx_status_t status = zx::channel::create(0, &device_fs_server, &device_fs_client);
        if (status != ZX_OK) {
            loader_log(nullptr, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0, "connect_to_vulkan_loader_svc: Failed to create channel: %s",
                       zx_status_get_string(status));
            return;
        }

        auto result = client->ConnectToDeviceFs(std::move(device_fs_server));
        if (!result.ok()) {
            loader_log(nullptr, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                       "connect_to_vulkan_loader_svc: Failed to connect to device fs: %s", result.status_string());
            return;
        }
    }
    {
        zx::channel manifest_fs_client;
        zx::channel manifest_fs_server;
        zx_status_t status = zx::channel::create(0, &manifest_fs_server, &manifest_fs_client);
        if (status != ZX_OK) {
            loader_log(nullptr, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0, "connect_to_vulkan_loader_svc: Failed to create channel: %s",
                       zx_status_get_string(status));
            return;
        }

        // Wait for idle so clients will be sure that any existing ICDs will be completely available.
        auto result = client->ConnectToManifestFs(fuchsia_vulkan_loader::wire::ConnectToManifestOptions::kWaitForIdle,
                                                  std::move(manifest_fs_server));
        if (!result.ok()) {
            loader_log(nullptr, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                       "connect_to_vulkan_loader_svc: Failed to connect to manifest fs: %s", result.status_string());
            return;
        }
        status = fdio_fd_create(manifest_fs_client.release(), &manifest_fs_fd);
        if (status != ZX_OK) {
            loader_log(nullptr, VK_DEBUG_REPORT_ERROR_BIT_EXT, 0,
                       "connect_to_vulkan_loader_svc: Failed to create manifest fs fd: %s", zx_status_get_string(status));
            return;
        }
    }

    vulkan_loader_svc = std::move(client);
    device_fs = device_fs_client.release();
}

static once_flag svc_connect_once_flag = ONCE_FLAG_INIT;

fidl::WireSyncClient<fuchsia_vulkan_loader::Loader>& get_vulkan_loader_service() {
    call_once(&svc_connect_once_flag, connect_to_vulkan_loader_svc);

    return vulkan_loader_svc;
}

zx_handle_t get_device_fs() {
    call_once(&svc_connect_once_flag, connect_to_vulkan_loader_svc);

    return device_fs;
}

int get_manifest_fs_fd(void) {
    call_once(&svc_connect_once_flag, connect_to_vulkan_loader_svc);

    return manifest_fs_fd;
}
