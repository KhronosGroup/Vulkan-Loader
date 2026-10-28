/*

    Implementation of POSIX directory browsing functions and types for Win32.

    Author:  Kevlin Henney (kevlin@acm.org, kevlin@curbralan.com)
    History: Created March 1997. Updated June 2003 and July 2012.
    Rights:  See end of file.

*/
#include "dirent_on_windows.h"

#include <errno.h>
#include <io.h> /* _findfirst and _findnext set errno iff they return -1 */
#include <stdlib.h>
#include <string.h>

#include "allocation.h"
#include "loader_common.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef ptrdiff_t handle_type; /* C99's intptr_t not sufficiently portable */

struct DIR {
    handle_type handle;        /* -1 for failed rewind */
    struct _wfinddata_t info;  // NOLINT(bugprone-reserved-identifier) - matching the Windows CRT type name
    struct dirent result;      /* d_name memset to zero after opendir & rewinddir */
    wchar_t* name;             /* null-terminated wchar_t string */
    bool readdir_called;       /*indicates if readdir has been called on this DIR object - set to false when rewinddir is called*/
};

DIR* opendir(const struct loader_instance* inst, const char* name) {
    DIR* dir = 0;

    if (name && name[0]) {
        bool oom = false;
        wchar_t* name_wide = loader_convert_char_to_wchar(inst, name, &oom, VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
        if (oom) {
            errno = ENOMEM;
            return dir;
        }
        if (NULL == name_wide) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "opendir: Failed to convert char string %s to a wchar_t string", name);
            errno = EINVAL;
            return dir;
        }

        size_t base_length = wcslen(name_wide);
        const wchar_t* all = /* search pattern must end with suitable wildcard */
            wcschr(L"/\\", name_wide[base_length - 1]) ? L"*" : L"/*";
        size_t full_length = base_length + wcslen(all) + 1;

        if ((dir = (DIR*)loader_instance_heap_calloc(inst, sizeof *dir, VK_SYSTEM_ALLOCATION_SCOPE_COMMAND)) != 0 &&
            (dir->name = (wchar_t*)loader_instance_heap_calloc(inst, sizeof(wchar_t) * full_length,
                                                               VK_SYSTEM_ALLOCATION_SCOPE_COMMAND)) != 0) {
            loader_wcsncpy(dir->name, full_length, name_wide, base_length);
            loader_wcsncat(dir->name, full_length, all, wcslen(all));

            if ((dir->handle = (handle_type)_wfindfirst(dir->name, &dir->info)) != -1) {
                memset(dir->result.d_name, 0, MAX_PATH_DIRENT);
                dir->readdir_called = false;
            } else /* rollback */
            {
                loader_instance_heap_free(inst, dir->name);
                loader_instance_heap_free(inst, dir);
                dir = 0;
            }
        } else /* rollback */
        {
            loader_instance_heap_free(inst, dir);
            dir = 0;
            errno = ENOMEM;
        }
        loader_instance_heap_free(inst, name_wide);
    } else {
        errno = EINVAL;
    }

    return dir;
}

int closedir(const struct loader_instance* inst, DIR* dir) {
    int result = -1;

    if (dir) {
        if (dir->handle != -1) {
            result = _findclose(dir->handle);
        }

        loader_instance_heap_free(inst, dir->name);
        loader_instance_heap_free(inst, dir);
    }

    if (result == -1) /* map all errors to EBADF */
    {
        errno = EBADF;
    }

    return result;
}

struct dirent* readdir(const struct loader_instance* inst, DIR* dir) {
    struct dirent* result = 0;

    if (dir && dir->handle != -1) {
        if (dir->readdir_called) {
            int find_next_ret = _wfindnext(dir->handle, &dir->info);
            if (find_next_ret == -1) {
                // _findnext sets errno to ENOENT when no more matching files could be found, does not indicate an error
                if (errno == ENOENT) {
                    errno = 0;
                }
                return result;
            }
        } else {
            dir->readdir_called = true;
        }
        result = &dir->result;

        memset(dir->result.d_name, 0, MAX_PATH_DIRENT);
        size_t name_len = wcsnlen(dir->info.name, MAX_PATH);

        int convert_ret =
            WideCharToMultiByte(CP_UTF8, 0, dir->info.name, (int)name_len, result->d_name, (int)MAX_PATH_DIRENT, NULL, NULL);
        if (convert_ret <= 0) {
            errno = EOVERFLOW;
            DWORD err = GetLastError();
            loader_log_wide(inst, VULKAN_LOADER_ERROR_BIT, 0,
                            L"readdir: Failed to convert wchar_t string %ls to a char string with error code %lu", dir->info.name,
                            err);
            return result;
        }
    } else {
        errno = EBADF;
    }

    return result;
}

void rewinddir(DIR* dir) {
    if (dir && dir->handle != -1) {
        _findclose(dir->handle);
        dir->handle = (handle_type)_wfindfirst(dir->name, &dir->info);
        memset(dir->result.d_name, 0, MAX_PATH_DIRENT);
        dir->readdir_called = false;
    } else {
        errno = EBADF;
    }
}

#if defined(__cplusplus)
}
#endif

/*

    Copyright Kevlin Henney, 1997, 2003, 2012. All rights reserved.
    Copyright (c) 2015 The Khronos Group Inc.
    Copyright (c) 2015 Valve Corporation
    Copyright (c) 2015 LunarG, Inc.
    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose is hereby granted without fee, provided
    that this copyright and permissions notice appear in all copies and
    derivatives.

    This software is supplied "as is" without express or implied warranty.

    But that said, if there are any problems please get in touch.

*/
