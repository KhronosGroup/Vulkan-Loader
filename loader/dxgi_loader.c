/*
 * Copyright (c) 2019-2020 The Khronos Group Inc.
 * Copyright (c) 2019-2020 Valve Corporation
 * Copyright (c) 2019-2020 LunarG, Inc.
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
 */

#include "dxgi_loader.h"

#include <strsafe.h>

static HMODULE load_dxgi_module() {
    TCHAR systemPath[MAX_PATH] = "";
    GetSystemDirectory(systemPath, MAX_PATH);
    StringCchCat(systemPath, MAX_PATH, TEXT("\\dxgi.dll"));

    return LoadLibrary(systemPath);
}

typedef HRESULT(APIENTRY *PFN_CreateDXGIFactory1)(REFIID riid, void **ppFactory);

HRESULT dyn_CreateDXGIFactory1(REFIID riid, void **ppFactory) {
    PFN_CreateDXGIFactory1 fpCreateDXGIFactory1 = (PFN_CreateDXGIFactory1)GetProcAddress(load_dxgi_module(), "CreateDXGIFactory1");

    if (fpCreateDXGIFactory1 != NULL) return fpCreateDXGIFactory1(riid, ppFactory);

    return DXGI_ERROR_NOT_FOUND;
}
