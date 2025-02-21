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

#include "dlopen_fuchsia.h"

#include <dlfcn.h>
#include <fidl/fuchsia.io/cpp/wire.h>
#include <lib/fdio/directory.h>
#include <lib/fdio/io.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <zircon/dlfcn.h>
#include <zircon/errors.h>
#include <zircon/types.h>

#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include "loader_fuchsia_service.h"

namespace {

char g_error[256] = {};

}  // namespace

const char *dlerror_fuchsia(void) { return g_error; }

struct FuchsiaLoaderData {
    std::mutex table_lock;

    // Keep a cache of ICD shared libraries to avoid loading the same library
    // multiple times. dlclose doesn't currently do anything on Fuchsia, so we never
    // bother calling it.
    std::unordered_map<std::string, void *> icd_dso_map __attribute__((guarded_by(table_lock)));
    std::unordered_set<void *> icd_dso_set __attribute__((guarded_by(table_lock)));
};

namespace {

FuchsiaLoaderData *fuchsia_loader_data;

}  // namespace

void fuchsia_initialize() { fuchsia_loader_data = new FuchsiaLoaderData; }

void fuchsia_teardown() { delete fuchsia_loader_data; }

namespace {

__attribute__((format(printf, 1, 2))) void append_to_error(const char *format, ...) {
    // Size of an individual error message. Multiple can be concatenated.
    constexpr size_t kErrorSize = 128;
    char local_error[kErrorSize];

    va_list args;
    va_start(args, format);
    vsnprintf(local_error, sizeof(local_error), format, args);
    va_end(args);

    // Subtract 1 because the NUL-terminator isn't counted.
    strncat(g_error, local_error, sizeof(g_error) - strlen(g_error) - 1);
    // Set last byte to 0 in case there wasn't enough room in g_error to fit the entire string.
    g_error[sizeof(g_error) - 1] = '\0';
}

// Some VMOs may be loaded from directories outside the package which the shared library loader doesn't
// have access to. They can only be loaded by opening the file and using dlopen_vmo.
void *dlopen_using_vmo(const char *name, int mode) {
    int fd;
    zx_status_t status =
        fdio_open3_fd(name, /*flags=*/uint64_t{fuchsia_io::wire::kPermReadable | fuchsia_io::wire::kPermExecutable}, &fd);
    if (status != ZX_OK) {
        append_to_error("libvulkan.so:dlopen_using_vmo: couldn't open \"%s\" as fd, %s\n", name, zx_status_get_string(status));
        return nullptr;
    }
    zx::vmo vmo;
    status = fdio_get_vmo_exec(fd, vmo.reset_and_get_address());
    close(fd);
    if (status != ZX_OK) {
        append_to_error("libvulkan.so:dlopen_using_vmo: couldn't get vmo \"%s\" as exec: %s\n", name, zx_status_get_string(status));
        return nullptr;
    }
    void *result = dlopen_vmo(vmo.get(), mode);
    if (!result) {
        append_to_error("%s", dlerror());
    }
    return result;
}

void *dlopen_from_vulkan_loader(const char *name, int mode, bool *retry_out) {
    // Connect to the vulkan loader service to request this library.
    // If loading fails at these early points, the ICD may just not be available
    // through the loader service, so retry using plain dlopen.
    *retry_out = true;

    std::lock_guard<std::mutex> lock(fuchsia_loader_data->table_lock);
    auto it = fuchsia_loader_data->icd_dso_map.find(name);
    if (it != fuchsia_loader_data->icd_dso_map.end()) {
        return it->second;
    }

    auto &loader_svc = get_vulkan_loader_service();
    if (!loader_svc.is_valid()) {
        append_to_error("libvulkan.so:dlopen_fuchsia: no connection to loader svc\n");
        return nullptr;
    }

    auto get_result = loader_svc->Get(fidl::StringView::FromExternal(name));
    if (!get_result.ok()) {
        append_to_error("libvulkan.so:dlopen_fuchsia: Get() of \"%s\" failed: %s\n", name, get_result.status_string());
        return nullptr;
    }

    if (!get_result.value().lib) {
        append_to_error("libvulkan.so:dlopen_fuchsia: Get() returned invalid vmo\n");
        return nullptr;
    }
    // If we've got a valid VMO matching the name then that VMO is expected to work.
    *retry_out = false;

    void *result = dlopen_vmo(get_result.value().lib.get(), mode);
    if (!result) {
        append_to_error("%s", dlerror());
    } else {
        fuchsia_loader_data->icd_dso_map[name] = result;
        fuchsia_loader_data->icd_dso_set.insert(result);
    }
    return result;
}

}  // namespace

void *dlopen_fuchsia(const char *name, int mode, bool driver) {
    // All errors are appended to g_error, so reset it to empty.
    g_error[0] = '\0';
    void *result;
    if (driver) {
        // ICDs generally should by loaded from the loader service, so try that first.
        bool retry{};
        result = dlopen_from_vulkan_loader(name, mode, &retry);
        if (result || !retry) {
            return result;
        }
    }
    result = dlopen(name, mode);
    if (result != nullptr) {
        return result;
    }
    return dlopen_using_vmo(name, mode);
}

void dlclose_fuchsia(void *dso) {
    {
        std::lock_guard<std::mutex> lock(fuchsia_loader_data->table_lock);
        if (fuchsia_loader_data->icd_dso_set.count(dso)) {
            // dlclose doesn't actually do anything on Fuchsia, so it's better
            // to cache the shared object forever rather than leak it.
            return;
        }
    }
    // It's fine to dlclose normal shared objects with sonames because the
    // library loader caches them by soname.
    dlclose(dso);
}
