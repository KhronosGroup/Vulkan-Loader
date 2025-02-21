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

#ifndef LOADER_LOADER_FUCHSIA_SERVICE_H_
#define LOADER_LOADER_FUCHSIA_SERVICE_H_

#include <fidl/fuchsia.vulkan.loader/cpp/wire.h>
#include <zircon/types.h>

#include "loader_fuchsia.h"

// Returns a singleton handle to the fuchsia.vulkan.loader.Loader service. Does not transfer
// ownership.
fidl::WireSyncClient<fuchsia_vulkan_loader::Loader>& get_vulkan_loader_service();

// Returns a singleton handle to the device fs retrieved from
// fuchsia.vulkan.loader/Loader.ConnectToDeviceFs. Does not transfer ownership.
zx_handle_t get_device_fs();

#endif  // LOADER_LOADER_FUCHSIA_SERVICE_H_
