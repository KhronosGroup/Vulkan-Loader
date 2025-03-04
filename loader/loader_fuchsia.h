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

#ifndef LOADER_LOADER_FUCHSIA_H_
#define LOADER_LOADER_FUCHSIA_H_

#ifdef __cplusplus
extern "C" {
#endif  // _cplusplus

// Returns a file descriptor with a directory FD for the loader manifest file
// system. May return -1 if the loader doesn't support ConnectToManifestFs.
int get_manifest_fs_fd(void);

// Called on loader shared library construction.
void fuchsia_initialize(void);

// Called on loader shared library destruction.
void fuchsia_teardown(void);

#ifdef __cplusplus
}
#endif  // _cplusplus

#endif  // LOADER_LOADER_FUCHSIA_H_
