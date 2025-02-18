/*
 *
 * Copyright (C) 2025 Fuchsia Authors.
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

#ifndef LOADER_LOADER_EXTENSIONS_FUCHSIA_H_
#define LOADER_LOADER_EXTENSIONS_FUCHSIA_H_

#include "vk_loader_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

void loader_initialize_icd_services_fuchsia(loader_platform_dl_handle handle);

#ifdef __cplusplus
}
#endif

#endif  // LOADER_LOADER_EXTENSIONS_FUCHSIA_H_
