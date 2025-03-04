/*
 *
 * Copyright (c) 2018 Google Inc.
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

#include <fidl/fuchsia.io/cpp/wire.h>
#include <lib/fdio/directory.h>
#include <string_view>
#include <limits.h>
#include <stdint.h>

// Include this file as it ensures that `__Fuchsia_API_level__`, which is used below, is correctly
// defined.
#include <zircon/availability.h>

#include "loader_extensions_fuchsia.h"
#include "loader_fuchsia_service.h"
#include "log.h"

namespace {

// Extended ICD API to allow I/O on ICDs. See Fuchsia RFC-205 for details:
// https://fuchsia.dev/fuchsia-src/contribute/governance/rfcs/0205_vulkan_loader
using PFN_vk_icdOpenInNamespaceAddr = VkResult(VKAPI_PTR*)(const char*, uint32_t);
using PFN_vk_icdInitializeOpenInNamespaceCallbackAddr = void(VKAPI_PTR*)(PFN_vk_icdOpenInNamespaceAddr);

// Extended ICD API to notify ICDs of supported Fuchsia API level:
// https://fuchsia.dev/fuchsia-src/development/api/evolution
using PFN_vk_icdSetFuchsiaApiLevel = void(VKAPI_PTR*)(uint32_t);

VKAPI_ATTR VkResult VKAPI_CALL loader_fdio_open_in_namespace(const char* const pName, uint32_t handle) {
    constexpr std::string_view kGpuDevicePrefix = "/loader-gpu-devices";
    zx_status_t status;
    if (std::string_view(pName).starts_with(kGpuDevicePrefix)) {
        // Accesses to subpaths of /loader-gpu-devices should be redirected through the devicefs
        // exposed by the loader service. See README.md in this directory for details.
        zx_handle_t device_fs = get_device_fs();
        if (!device_fs) {
            return VK_ERROR_INITIALIZATION_FAILED;
        }
        const char* new_name = pName + kGpuDevicePrefix.length();
        if (std::string_view(new_name).empty()) {
            new_name = ".";
        } else {
            if (new_name[0] != '/') {
                // Name must start with "/loader-gpu-devices/", or that's a bug.
                return VK_ERROR_INITIALIZATION_FAILED;
            }
        }
        status = fdio_service_connect_at(device_fs, new_name, handle);
    } else {
        status = fdio_service_connect(pName, handle);
    }

    if (status == ZX_OK) {
        return VK_SUCCESS;
    }
    loader_log(nullptr, VK_DEBUG_REPORT_WARNING_BIT_EXT, 0, "Failed to open in namespace %s - error %d", pName, status);
    return VK_ERROR_INITIALIZATION_FAILED;
}

}  // namespace

void loader_initialize_icd_services_fuchsia(loader_platform_dl_handle handle) {
    PFN_vk_icdInitializeOpenInNamespaceCallbackAddr fp_initialize_open_in_namespace_callback_addr;
    PFN_vk_icdSetFuchsiaApiLevel fp_vk_set_fuchsia_api_level;

    fp_vk_set_fuchsia_api_level =
        reinterpret_cast<PFN_vk_icdSetFuchsiaApiLevel>(loader_platform_get_proc_address(handle, "vk_icdSetFuchsiaApiLevel"));
    if (fp_vk_set_fuchsia_api_level) {
        fp_vk_set_fuchsia_api_level(__Fuchsia_API_level__);
    }

    fp_initialize_open_in_namespace_callback_addr = reinterpret_cast<PFN_vk_icdInitializeOpenInNamespaceCallbackAddr>(
        loader_platform_get_proc_address(handle, "vk_icdInitializeOpenInNamespaceCallback"));
    if (fp_initialize_open_in_namespace_callback_addr) {
        fp_initialize_open_in_namespace_callback_addr(&loader_fdio_open_in_namespace);
        // ICD isn't required to expose this entrypoint.
    }
}
