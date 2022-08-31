/*
 *
 * Copyright (c) 2014-2021 The Khronos Group Inc.
 * Copyright (c) 2014-2021 Valve Corporation
 * Copyright (c) 2014-2021 LunarG, Inc.
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
 * Author: Jon Ashburn <jon@lunarg.com>
 * Author: Courtney Goeltzenleuchter <courtney@LunarG.com>
 * Author: Chia-I Wu <olvaffe@gmail.com>
 * Author: Chia-I Wu <olv@lunarg.com>
 * Author: Mark Lobodzinski <mark@LunarG.com>
 * Author: Lenny Komow <lenny@lunarg.com>
 * Author: Charles Giessen <charles@lunarg.com>
 *
 */

#include "get_environment.h"

#include "allocation.h"
#include "log.h"

// Environment variables
#if defined(__linux__) || defined(__APPLE__) || defined(__Fuchsia__) || defined(__QNXNTO__) || defined(__FreeBSD__) || defined(__OpenBSD__)

bool is_high_integrity() { return geteuid() != getuid() || getegid() != getgid(); }

char *loader_getenv(const char *name, const struct loader_instance *inst) {
    // No allocation of memory necessary for Linux, but we should at least touch
    // the inst pointer to get rid of compiler warnings.
    (void)inst;
    return getenv(name);
}

char *loader_secure_getenv(const char *name, const struct loader_instance *inst) {
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__)
    // Apple does not appear to have a secure getenv implementation.
    // The main difference between secure getenv and getenv is that secure getenv
    // returns NULL if the process is being run with elevated privileges by a normal user.
    // The idea is to prevent the reading of malicious environment variables by a process
    // that can do damage.
    // This algorithm is derived from glibc code that sets an internal
    // variable (__libc_enable_secure) if the process is running under setuid or setgid.
    return is_high_integrity() ? NULL : loader_getenv(name, inst);
#elif defined(__Fuchsia__)
    return loader_getenv(name, inst);
#else
    // Linux
    char *out;
#if defined(HAVE_SECURE_GETENV) && !defined(USE_UNSAFE_FILE_SEARCH)
    (void)inst;
    out = secure_getenv(name);
#elif defined(HAVE___SECURE_GETENV) && !defined(USE_UNSAFE_FILE_SEARCH)
    (void)inst;
    out = __secure_getenv(name);
#else
    out = loader_getenv(name, inst);
#if !defined(USE_UNSAFE_FILE_SEARCH)
    loader_log(inst, VULKAN_LOADER_INFO_BIT, 0, "Loader is using non-secure environment variable lookup for %s", name);
#endif
#endif
    return out;
#endif
}

void loader_free_getenv(char *val, const struct loader_instance *inst) {
    // No freeing of memory necessary for Linux, but we should at least touch
    // the val and inst pointers to get rid of compiler warnings.
    (void)val;
    (void)inst;
}

#elif defined(WIN32)

bool is_high_integrity() {
    HANDLE process_token;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_QUERY_SOURCE, &process_token)) {
        // Maximum possible size of SID_AND_ATTRIBUTES is maximum size of a SID + size of attributes DWORD.
        uint8_t mandatory_label_buffer[SECURITY_MAX_SID_SIZE + sizeof(DWORD)];
        DWORD buffer_size;
        if (GetTokenInformation(process_token, TokenIntegrityLevel, mandatory_label_buffer, sizeof(mandatory_label_buffer),
                                &buffer_size) != 0) {
            const TOKEN_MANDATORY_LABEL *mandatory_label = (const TOKEN_MANDATORY_LABEL *)mandatory_label_buffer;
            const DWORD sub_authority_count = *GetSidSubAuthorityCount(mandatory_label->Label.Sid);
            const DWORD integrity_level = *GetSidSubAuthority(mandatory_label->Label.Sid, sub_authority_count - 1);

            CloseHandle(process_token);
            return integrity_level > SECURITY_MANDATORY_MEDIUM_RID;
        }

        CloseHandle(process_token);
    }

    return false;
}

char *loader_getenv(const char *name, const struct loader_instance *inst) {
    int name_utf16_size = MultiByteToWideChar(CP_UTF8, 0, name, -1, NULL, 0);
    if (name_utf16_size <= 0) {
        return NULL;
    }
    wchar_t *name_utf16 = (wchar_t *)loader_stack_alloc(name_utf16_size * sizeof(wchar_t));
    if (MultiByteToWideChar(CP_UTF8, 0, name, -1, name_utf16, name_utf16_size) != name_utf16_size) {
        return NULL;
    }

    DWORD val_size = GetEnvironmentVariableW(name_utf16, NULL, 0);
    // val_size DOES include the null terminator, so for any set variable
    // will always be at least 1. If it's 0, the variable wasn't set.
    if (val_size == 0) {
        return NULL;
    }

    wchar_t *val = (wchar_t *)loader_stack_alloc(val_size * sizeof(wchar_t));
    if (GetEnvironmentVariableW(name_utf16, val, val_size) != val_size - 1) {
        return NULL;
    }

    int val_utf8_size = WideCharToMultiByte(CP_UTF8, 0, val, -1, NULL, 0, NULL, NULL);
    if (val_utf8_size <= 0) {
        return NULL;
    }
    char *val_utf8 = (char *)loader_instance_heap_alloc(inst, val_utf8_size * sizeof(char), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
    if (val_utf8 == NULL) {
        return NULL;
    }
    if (WideCharToMultiByte(CP_UTF8, 0, val, -1, val_utf8, val_utf8_size, NULL, NULL) != val_utf8_size) {
        loader_instance_heap_free(inst, val_utf8);
        return NULL;
    }
    return val_utf8;
}

char *loader_secure_getenv(const char *name, const struct loader_instance *inst) {
#if !defined(USE_UNSAFE_FILE_SEARCH)
    if (is_high_integrity()) {
        loader_log(inst, VULKAN_LOADER_INFO_BIT, 0,
                   "Loader is running with elevated permissions. Environment variable %s will be ignored", name);
        return NULL;
    }
#endif

    return loader_getenv(name, inst);
}

void loader_free_getenv(char *val, const struct loader_instance *inst) { loader_instance_heap_free(inst, (void *)val); }

#else

char *loader_getenv(const char *name, const struct loader_instance *inst) {
    // stub func
    (void)inst;
    (void)name;
    return NULL;
}
void loader_free_getenv(char *val, const struct loader_instance *inst) {
    // stub func
    (void)val;
    (void)inst;
}

#endif
