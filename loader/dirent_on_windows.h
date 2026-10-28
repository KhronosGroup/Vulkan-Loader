#pragma once

/*

    Declaration of POSIX directory browsing functions and types for Win32.

    Author:  Kevlin Henney (kevlin@acm.org, kevlin@curbralan.com)
    History: Created March 1997. Updated June 2003.
    Rights:  See end of file.

*/

#if defined(__cplusplus)
extern "C" {
#endif

struct loader_instance;
typedef struct DIR DIR;

// Need storage enough to hold a 260 wchar_t string converted to a multi-byte-character char string and a null terminator
#define MAX_PATH_DIRENT (260 * 4 + 1)

struct dirent {
    char d_name[MAX_PATH_DIRENT];
};

// pass in VkAllocationCallbacks to allow allocation callback usage
DIR* opendir(const struct loader_instance* inst, const char*);
int closedir(const struct loader_instance* inst, DIR*);
struct dirent* readdir(const struct loader_instance* inst, DIR*);
void rewinddir(DIR*);

/*

    Copyright Kevlin Henney, 1997, 2003. All rights reserved.
    Copyright (c) 2015-2021 The Khronos Group Inc.
    Copyright (c) 2015-2021 Valve Corporation
    Copyright (c) 2015-2021 LunarG, Inc.

    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose is hereby granted without fee, provided
    that this copyright and permissions notice appear in all copies and
    derivatives.

    This software is supplied "as is" without express or implied warranty.

    But that said, if there are any problems please get in touch.

*/

#if defined(__cplusplus)
}
#endif
