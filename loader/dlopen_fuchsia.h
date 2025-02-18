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
#ifndef LOADER_DLOPEN_FUCHSIA_H_
#define LOADER_DLOPEN_FUCHSIA_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// If not |driver|, then the request is to load a layer.
void *dlopen_fuchsia(const char *name, int mode, bool driver);
const char *dlerror_fuchsia(void);
void dlclose_fuchsia(void *dso);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // LOADER_DLOPEN_FUCHSIA_H_