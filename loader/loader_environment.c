/*
 *
 * Copyright (c) 2014-2023 The Khronos Group Inc.
 * Copyright (c) 2014-2023 Valve Corporation
 * Copyright (c) 2014-2023 LunarG, Inc.
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
 * Author: Mark Young <marky@LunarG.com>
 * Author: Lenny Komow <lenny@lunarg.com>
 * Author: Charles Giessen <charles@lunarg.com>
 *
 */

#include "loader_environment.h"

#include "allocation.h"
#include "loader.h"
#include "log.h"

#include <vulkan/vulkan.h>

#include <ctype.h>

// Environment variables
#if defined(__linux__) || defined(__APPLE__) || defined(__Fuchsia__) || defined(__QNXNTO__) || defined(__FreeBSD__) || \
    defined(__OpenBSD__)

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
            return integrity_level >= SECURITY_MANDATORY_HIGH_RID;
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

// Determine the type of filter string based on the contents of it.
// This will properly check against:
//  - substrings "*string*"
//  - prefixes "string*"
//  - suffixes "*string"
//  - full string names "string"
// It will also return the correct start and finish to remove any star '*' characters for the actual string compare
void determine_filter_type(const char *filter_string, enum loader_filter_string_type *filter_type, const char **new_start,
                           size_t *new_length) {
    size_t filter_length = strlen(filter_string);
    bool star_begin = false;
    bool star_end = false;
    if ('~' == filter_string[0]) {
        // One of the special identifiers like: ~all~, ~implicit~, or ~explicit~
        *filter_type = FILTER_STRING_SPECIAL;
        *new_start = filter_string;
        *new_length = filter_length;
    } else {
        if ('*' == filter_string[0]) {
            // Only the * means everything
            if (filter_length == 1) {
                *filter_type = FILTER_STRING_SPECIAL;
                *new_start = filter_string;
                *new_length = filter_length;
            } else {
                star_begin = true;
            }
        }
        if ('*' == filter_string[filter_length - 1]) {
            // Not really valid, but just catch this case so if someone accidentally types "**" it will also mean everything
            if (filter_length == 2) {
                *filter_type = FILTER_STRING_SPECIAL;
                *new_start = filter_string;
                *new_length = filter_length;
            } else {
                star_end = true;
            }
        }
        if (star_begin && star_end) {
            *filter_type = FILTER_STRING_SUBSTRING;
            *new_start = &filter_string[1];
            *new_length = filter_length - 2;
        } else if (star_begin) {
            *new_start = &filter_string[1];
            *new_length = filter_length - 1;
            *filter_type = FILTER_STRING_SUFFIX;
        } else if (star_end) {
            *filter_type = FILTER_STRING_PREFIX;
            *new_start = filter_string;
            *new_length = filter_length - 1;
        } else {
            *filter_type = FILTER_STRING_FULLNAME;
            *new_start = filter_string;
            *new_length = filter_length;
        }
    }
}

// Parse the provided filter string provided by the envrionment variable into the appropriate filter
// struct variable.
VkResult parse_generic_filter_environment_var(const struct loader_instance *inst, const char *env_var_name,
                                              struct loader_envvar_filter *filter_struct) {
    VkResult result = VK_SUCCESS;
    memset(filter_struct, 0, sizeof(struct loader_envvar_filter));
    char *env_var_value = loader_secure_getenv(env_var_name, inst);
    if (NULL == env_var_value) {
        return result;
    }
    if (strlen(env_var_value) > 0) {
        const size_t env_var_len = strlen(env_var_value);
        // Allocate a separate string since strtok modifies the original string
        char *parsing_string = loader_instance_heap_calloc(inst, env_var_len + 1, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (NULL != parsing_string) {
            const char tokenizer[3] = ",";

            for (uint32_t iii = 0; iii < env_var_len; ++iii) {
                parsing_string[iii] = (char)tolower(env_var_value[iii]);
            }
            parsing_string[env_var_len] = '\0';

            char *token = strtok(parsing_string, tokenizer);
            while (NULL != token) {
                enum loader_filter_string_type cur_filter_type;
                const char *actual_start;
                size_t actual_len;
                determine_filter_type(token, &cur_filter_type, &actual_start, &actual_len);
                if (actual_len > VK_MAX_EXTENSION_NAME_SIZE) {
                    strncpy(filter_struct->filters[filter_struct->count].value, actual_start, VK_MAX_EXTENSION_NAME_SIZE);
                } else {
                    strncpy(filter_struct->filters[filter_struct->count].value, actual_start, actual_len);
                }
                filter_struct->filters[filter_struct->count].length = actual_len;
                filter_struct->filters[filter_struct->count++].type = cur_filter_type;
                if (filter_struct->count >= MAX_ADDITIONAL_FILTERS) {
                    break;
                }
                token = strtok(NULL, tokenizer);
            }
            loader_instance_heap_free(inst, parsing_string);
        } else {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "parse_generic_filter_environment_var: Failed to allocate space for parsing env var \'%s\'", env_var_name);
            result = VK_ERROR_OUT_OF_HOST_MEMORY;
        }
    }
    loader_free_getenv(env_var_value, inst);
    return result;
}

// Parse the disable layer string.  The layer disable has some special behavior because we allow it to disable
// all layers (either with "~all~", "*", or "**"), all implicit layers (with "~implicit~"), and all explicit layers
// (with "~explicit~"), in addition to the other layer filtering behavior.
VkResult parse_layers_disable_filter_environment_var(const struct loader_instance *inst,
                                                     struct loader_envvar_disable_layers_filter *disable_struct) {
    VkResult result = VK_SUCCESS;
    memset(disable_struct, 0, sizeof(struct loader_envvar_disable_layers_filter));
    char *env_var_value = loader_secure_getenv(VK_LAYERS_DISABLE_ENV_VAR, inst);
    if (NULL == env_var_value) {
        goto out;
    }
    if (strlen(env_var_value) > 0) {
        const size_t env_var_len = strlen(env_var_value);
        // Allocate a separate string since strtok modifies the original string
        char *parsing_string = loader_instance_heap_calloc(inst, env_var_len + 1, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (NULL != parsing_string) {
            const char tokenizer[3] = ",";

            for (uint32_t iii = 0; iii < env_var_len; ++iii) {
                parsing_string[iii] = (char)tolower(env_var_value[iii]);
            }
            parsing_string[env_var_len] = '\0';

            char *token = strtok(parsing_string, tokenizer);
            while (NULL != token) {
                uint32_t cur_count = disable_struct->additional_filters.count;
                enum loader_filter_string_type cur_filter_type;
                const char *actual_start;
                size_t actual_len;
                determine_filter_type(token, &cur_filter_type, &actual_start, &actual_len);
                if (cur_filter_type == FILTER_STRING_SPECIAL) {
                    if (!strcmp(VK_LOADER_DISABLE_ALL_LAYERS_VAR_1, token) || !strcmp(VK_LOADER_DISABLE_ALL_LAYERS_VAR_2, token) ||
                        !strcmp(VK_LOADER_DISABLE_ALL_LAYERS_VAR_3, token)) {
                        disable_struct->disable_all = true;
                    } else if (!strcmp(VK_LOADER_DISABLE_IMPLICIT_LAYERS_VAR, token)) {
                        disable_struct->disable_all_implicit = true;
                    } else if (!strcmp(VK_LOADER_DISABLE_EXPLICIT_LAYERS_VAR, token)) {
                        disable_struct->disable_all_explicit = true;
                    }
                } else {
                    if (actual_len > VK_MAX_EXTENSION_NAME_SIZE) {
                        strncpy(disable_struct->additional_filters.filters[cur_count].value, actual_start,
                                VK_MAX_EXTENSION_NAME_SIZE);
                    } else {
                        strncpy(disable_struct->additional_filters.filters[cur_count].value, actual_start, actual_len);
                    }
                    disable_struct->additional_filters.filters[cur_count].length = actual_len;
                    disable_struct->additional_filters.filters[cur_count].type = cur_filter_type;
                    disable_struct->additional_filters.count++;
                    if (disable_struct->additional_filters.count >= MAX_ADDITIONAL_FILTERS) {
                        break;
                    }
                }
                token = strtok(NULL, tokenizer);
            }
            loader_instance_heap_free(inst, parsing_string);
        } else {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "parse_layers_disable_filter_environment_var: Failed to allocate space for parsing env var "
                       "\'VK_LAYERS_DISABLE_ENV_VAR\'");
            result = VK_ERROR_OUT_OF_HOST_MEMORY;
        }
    }
    loader_free_getenv(env_var_value, inst);
out:
    return result;
}

// Check to see if the provided layer name matches any of the filter strings.
// This will properly check against:
//  - substrings "*string*"
//  - prefixes "string*"
//  - suffixes "*string"
//  - full string names "string"
bool check_name_matches_filter_environment_var(const struct loader_instance *inst, const char *name,
                                               const struct loader_envvar_filter *filter_struct) {
    bool ret_value = false;
    const size_t name_len = strlen(name);
    char lower_name[VK_MAX_EXTENSION_NAME_SIZE];
    for (uint32_t iii = 0; iii < name_len; ++iii) {
        lower_name[iii] = (char)tolower(name[iii]);
    }
    lower_name[name_len] = '\0';
    for (uint32_t filt = 0; filt < filter_struct->count; ++filt) {
        // Check if the filter name is longer (this is with all special characters removed), and if it is
        // continue since it can't match.
        if (filter_struct->filters[filt].length > name_len) {
            continue;
        }
        switch (filter_struct->filters[filt].type) {
            case FILTER_STRING_SPECIAL:
                if (!strcmp(VK_LOADER_DISABLE_ALL_LAYERS_VAR_1, filter_struct->filters[filt].value) ||
                    !strcmp(VK_LOADER_DISABLE_ALL_LAYERS_VAR_2, filter_struct->filters[filt].value) ||
                    !strcmp(VK_LOADER_DISABLE_ALL_LAYERS_VAR_3, filter_struct->filters[filt].value)) {
                    ret_value = true;
                }
                break;

            case FILTER_STRING_SUBSTRING:
                if (NULL != strstr(lower_name, filter_struct->filters[filt].value)) {
                    ret_value = true;
                }
                break;

            case FILTER_STRING_SUFFIX:
                if (0 == strncmp(lower_name + name_len - filter_struct->filters[filt].length, filter_struct->filters[filt].value,
                                 filter_struct->filters[filt].length)) {
                    ret_value = true;
                }
                break;

            case FILTER_STRING_PREFIX:
                if (0 == strncmp(lower_name, filter_struct->filters[filt].value, filter_struct->filters[filt].length)) {
                    ret_value = true;
                }
                break;

            case FILTER_STRING_FULLNAME:
                if (0 == strncmp(lower_name, filter_struct->filters[filt].value, name_len)) {
                    ret_value = true;
                }
                break;
        }
        if (ret_value) {
            break;
        }
    }
    return ret_value;
}

// Get the layer name(s) from the env_name environment variable. If layer is found in
// search_list then add it to layer_list.  But only add it to layer_list if type_flags matches.
VkResult loader_add_environment_layers(struct loader_instance *inst, const enum layer_type_flags type_flags, const char *env_name,
                                       const struct loader_envvar_filter *enable_filter,
                                       const struct loader_envvar_disable_layers_filter *disable_filter,
                                       struct loader_layer_list *target_list, struct loader_layer_list *expanded_target_list,
                                       const struct loader_layer_list *source_list) {
    VkResult res = VK_SUCCESS;
    char *next, *name;
    char *layer_env = loader_getenv(env_name, inst);
    char **vk_inst_layers = NULL;
    uint32_t vk_inst_layer_count = 0;
    uint32_t separator_count = 0;

    // If the layer environment variable is present (i.e. VK_INSTANCE_LAYERS), we will always add it to the layer list.
    if (layer_env != NULL) {
        name = loader_stack_alloc(strlen(layer_env) + 1);
        if (name != NULL) {
            separator_count = 1;
            for (uint32_t c = 0; c < strlen(layer_env); ++c) {
                if (layer_env[c] == PATH_SEPARATOR) {
                    separator_count++;
                }
            }

            vk_inst_layers =
                loader_instance_heap_calloc(inst, (separator_count * sizeof(char *)), VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
            if (vk_inst_layers == NULL) {
                res = VK_ERROR_OUT_OF_HOST_MEMORY;
                goto out;
            }
            for (uint32_t cur_layer = 0; cur_layer < separator_count; ++cur_layer) {
                vk_inst_layers[cur_layer] =
                    loader_instance_heap_calloc(inst, VK_MAX_EXTENSION_NAME_SIZE, VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
                if (vk_inst_layers[cur_layer] == NULL) {
                    res = VK_ERROR_OUT_OF_HOST_MEMORY;
                    goto out;
                }
            }

            strcpy(name, layer_env);

            loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_LAYER_BIT, 0, "env var \'%s\' defined and adding layers \"%s\"",
                       env_name, name);

            // First look for the old-fashion layers forced on with VK_INSTANCE_LAYERS
            while (name && *name) {
                next = loader_get_next_path(name);

                if (strlen(name) > 0) {
                    strncpy(vk_inst_layers[vk_inst_layer_count++], name, VK_MAX_EXTENSION_NAME_SIZE);
                }
                name = next;
            }
        }
    }

    // Loop through all the layers and check the enable/disable filters as well as the VK_INSTANCE_LAYERS value.
    for (uint32_t i = 0; i < source_list->count; i++) {
        struct loader_layer_properties *source_prop = &source_list->list[i];

        // If it doesn't match the type, or the name isn't what we're looking for, just continue
        if ((source_prop->type_flags & type_flags) != type_flags) {
            continue;
        }

        // We found a layer we're interested in, but has it been disabled...
        bool adding = true;
        bool is_implicit = (0 == (source_prop->type_flags & VK_LAYER_TYPE_FLAG_EXPLICIT_LAYER));
        bool disabled_by_type = (is_implicit) ? (NULL != disable_filter && disable_filter->disable_all_implicit)
                                              : (NULL != disable_filter && disable_filter->disable_all_explicit);
        if (NULL != disable_filter &&
            (disable_filter->disable_all || disabled_by_type ||
             check_name_matches_filter_environment_var(inst, source_prop->info.layerName, &disable_filter->additional_filters))) {
            loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                       "Layer \"%s\" ignored because it has been disabled by env var \'%s\'", source_prop->info.layerName,
                       VK_LAYERS_DISABLE_ENV_VAR);
            adding = false;
        }

        // If we are supposed to filter through all layers, we need to compare the layer name against the filter.
        // This can override the disable above, so we want to do it second.
        if (check_name_matches_filter_environment_var(inst, source_prop->info.layerName, enable_filter)) {
            adding = true;
            // Only way is_substring is true is if there are enable variables.  If that's the case, and we're past the
            // above, we should indicate that it was forced on in this way.
            loader_log(inst, VULKAN_LOADER_WARN_BIT | VULKAN_LOADER_LAYER_BIT, 0,
                       "Layer \"%s\" forced enabled due to env var \'%s\'", source_prop->info.layerName, VK_LAYERS_ENABLE_ENV_VAR);
        } else {
            adding = false;
            // If it's not in the enable filter, check the environment variable if it exists
            if (vk_inst_layer_count > 0) {
                for (uint32_t cur_layer = 0; cur_layer < vk_inst_layer_count; ++cur_layer) {
                    if (!strcmp(vk_inst_layers[cur_layer], source_prop->info.layerName)) {
                        adding = true;
                        break;
                    }
                }
            }
        }

        if (!adding) {
            continue;
        }

        // If not a meta-layer, simply add it.
        if (0 == (source_prop->type_flags & VK_LAYER_TYPE_FLAG_META_LAYER)) {
            res = loader_add_layer_properties_to_list(inst, target_list, 1, source_prop);
            if (res == VK_ERROR_OUT_OF_HOST_MEMORY) goto out;
            res = loader_add_layer_properties_to_list(inst, expanded_target_list, 1, source_prop);
            if (res == VK_ERROR_OUT_OF_HOST_MEMORY) goto out;
        } else {
            res = loader_add_meta_layer(inst, enable_filter, disable_filter, source_prop, target_list, expanded_target_list,
                                        source_list, NULL);
            if (res == VK_ERROR_OUT_OF_HOST_MEMORY) goto out;
        }
    }

out:

    if (NULL != vk_inst_layers) {
        for (uint32_t cur_layer = 0; cur_layer < separator_count; ++cur_layer) {
            loader_instance_heap_free(inst, vk_inst_layers[cur_layer]);
        }
        loader_instance_heap_free(inst, vk_inst_layers);
    }

    if (layer_env != NULL) {
        loader_free_getenv(layer_env, inst);
    }

    return res;
}

// Loader settings handling

void parseEnvLoaderDebugLogFlags(uint32_t *debug_flags) {
    char *env = NULL;
    char *orig = NULL;

    if (NULL == debug_flags) {
        return;
    }

    // Parse comma-separated debug options
    orig = env = loader_getenv("VK_LOADER_DEBUG", NULL);
    while (env) {
        char *p = strchr(env, ',');
        size_t len;

        if (p) {
            len = p - env;
        } else {
            len = strlen(env);
        }

        if (len > 0) {
            if (strncmp(env, "all", len) == 0) {
                *debug_flags = ~0u;
            } else if (strncmp(env, "warn", len) == 0) {
                *debug_flags |= VULKAN_LOADER_WARN_BIT;
            } else if (strncmp(env, "info", len) == 0) {
                *debug_flags |= VULKAN_LOADER_INFO_BIT;
            } else if (strncmp(env, "perf", len) == 0) {
                *debug_flags |= VULKAN_LOADER_PERF_BIT;
            } else if (strncmp(env, "error", len) == 0) {
                *debug_flags |= VULKAN_LOADER_ERROR_BIT;
            } else if (strncmp(env, "debug", len) == 0) {
                *debug_flags |= VULKAN_LOADER_DEBUG_BIT;
            } else if (strncmp(env, "layer", len) == 0) {
                *debug_flags |= VULKAN_LOADER_LAYER_BIT;
            } else if (strncmp(env, "driver", len) == 0 || strncmp(env, "implem", len) == 0 || strncmp(env, "icd", len) == 0) {
                *debug_flags |= VULKAN_LOADER_DRIVER_BIT;
            }
        }

        if (!p) {
            break;
        }

        env = p + 1;
    }

    loader_free_getenv(orig, NULL);
}

// Determine if the environment variables indicate that unknown instance extensions should be disabled.
void parseEnvDisableInstanceExtensions(struct loader_instance *inst, bool *disable_instance_ext) {
    char *env = NULL;
    if (NULL == disable_instance_ext) {
        return;
    }
    env = loader_getenv("VK_LOADER_DISABLE_INST_EXT_FILTER", NULL);
    if (NULL != env && atoi(env) != 0) {
        loader_log(inst, VULKAN_LOADER_DEBUG_BIT, 0, "Found environment var 'VK_LOADER_DISABLE_INST_EXT_FILTER' set to %s", env);
        *disable_instance_ext = true;
    } else {
        *disable_instance_ext = false;
    }
    loader_free_getenv(env, NULL);
}

// Determine if the environment variables indicate that driver should be modified.
VkResult parseEnvDeviceSettings(struct loader_instance *inst, struct loader_device_settings *device_settings) {
    if (NULL == device_settings) {
        return VK_SUCCESS;
    }

#if LOADER_ENABLE_LINUX_SORT
    char *env_value = loader_getenv("VK_LOADER_DISABLE_SELECT", inst);
    if (NULL != env_value && atoi(env_value) != 0) {
        // Device select disabled so bail out early
        loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                   "Found environment var `VK_LOADER_DISABLE_SELECT` set to non-zero.  Disabling device sorting.");
        loader_free_getenv(env_value, inst);
        device_settings->device_sorting_enabled = false;
    } else {
        char *selection = loader_getenv("VK_LOADER_DEVICE_SELECT", inst);
        size_t string_len = selection == NULL ? 0 : strlen(selection);
        if (NULL != selection && 1 < string_len) {
            loader_log(inst, VULKAN_LOADER_INFO_BIT | VULKAN_LOADER_DRIVER_BIT, 0,
                       "Found environment var `VK_LOADER_DEVICE_SELECT` set %s", selection);
            device_settings->device_select_string =
                loader_instance_heap_alloc(inst, string_len + 1, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
            if (NULL == device_settings->device_select_string) {
                loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "Failed allocating loader settings structure");
                return VK_ERROR_OUT_OF_HOST_MEMORY;
            }
            strncpy(device_settings->device_select_string, selection, string_len);
            device_settings->device_select_string[string_len] = '\0';
            device_settings->device_select_enabled = true;
            loader_free_getenv(selection, inst);
        }
    }
#endif  // LOADER_ENABLE_LINUX_SORT
    return VK_SUCCESS;
}

// Free the paths we used for setting up the search paths for layers and drivers.
void freeSearchPath(const struct loader_instance *inst, char **search_path) {
    if (search_path != NULL && *search_path != NULL) {
        loader_instance_heap_free(inst, *search_path);
        *search_path = NULL;
    }
}

// Free the paths we used for setting up the search paths for layers and drivers.
void freeSearchPaths(const struct loader_instance *inst, char **driver_path, char **implicit_path, char **explicit_path,
                     char **settings_path) {
    freeSearchPath(inst, driver_path);
    freeSearchPath(inst, implicit_path);
    freeSearchPath(inst, explicit_path);
    freeSearchPath(inst, settings_path);
}

// Figure out the default search paths for driver and layers based on the current operating
// system.
VkResult loaderDefaultSearchPaths(const struct loader_instance *inst, char **default_driver_path, char **default_implicit_path,
                                  char **default_explicit_path, char **default_settings_path) {
    VkResult result = VK_SUCCESS;
    const char curPassString[4][24] = {{"Driver"}, {"Implicit Layers"}, {"Explicit Layers"}, {"Settings"}};
    const char *relative_driver_folder = VK_DRIVERS_INFO_RELATIVE_DIR;
    const char *relative_implicit_layer_folder = VK_ILAYERS_INFO_RELATIVE_DIR;
    const char *relative_explicit_layer_folder = VK_ELAYERS_INFO_RELATIVE_DIR;
    const char *relative_settings_folder = VK_SETTINGS_INFO_RELATIVE_DIR;

#if defined(_WIN32)
    // Driver only
    char *package_path = windows_get_app_package_manifest_path(inst);
#else
    // Determine how much space is needed to generate the full search path
    // for the current manifest files.
    char *xdg_config_home = loader_secure_getenv("XDG_CONFIG_HOME", inst);
    char *xdg_config_dirs = loader_secure_getenv("XDG_CONFIG_DIRS", inst);
    bool env_xdg_config_dirs = true;
    bool env_xdg_data_dirs = true;

#if !defined(__Fuchsia__) && !defined(__QNXNTO__)
    if (NULL != xdg_config_dirs && '\0' == xdg_config_dirs[0]) {
        loader_free_getenv(xdg_config_dirs, inst);
        xdg_config_dirs = NULL;
    }
    if (NULL == xdg_config_dirs) {
        xdg_config_dirs = FALLBACK_CONFIG_DIRS;
        env_xdg_config_dirs = false;
    }
#endif

    char *xdg_data_home = loader_secure_getenv("XDG_DATA_HOME", inst);
    char *xdg_data_dirs = loader_secure_getenv("XDG_DATA_DIRS", inst);

#if !defined(__Fuchsia__) && !defined(__QNXNTO__)
    if (NULL != xdg_data_dirs && '\0' == xdg_data_dirs[0]) {
        loader_free_getenv(xdg_data_dirs, inst);
        xdg_data_dirs = NULL;
    }
    if (NULL == xdg_data_dirs) {
        xdg_data_dirs = FALLBACK_DATA_DIRS;
        env_xdg_data_dirs = false;
    }
#endif

    char *home = NULL;
    char *default_data_home = NULL;
    char *default_config_home = NULL;
    char *home_data_dir = NULL;
    char *home_config_dir = NULL;

    // Only use HOME if XDG_DATA_HOME is not present on the system
    home = loader_secure_getenv("HOME", inst);
    if (home != NULL) {
        if (NULL == xdg_config_home || '\0' == xdg_config_home[0]) {
            const char config_suffix[] = "/.config";
            default_config_home =
                loader_instance_heap_alloc(inst, strlen(home) + strlen(config_suffix) + 1, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
            if (default_config_home == NULL) {
                result = VK_ERROR_OUT_OF_HOST_MEMORY;
                goto out;
            }
            strcpy(default_config_home, home);
            strcat(default_config_home, config_suffix);
        }
        if (NULL == xdg_data_home || '\0' == xdg_data_home[0]) {
            const char data_suffix[] = "/.local/share";
            default_data_home =
                loader_instance_heap_alloc(inst, strlen(home) + strlen(data_suffix) + 1, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
            if (default_data_home == NULL) {
                result = VK_ERROR_OUT_OF_HOST_MEMORY;
                goto out;
            }
            strcpy(default_data_home, home);
            strcat(default_data_home, data_suffix);
        }
    }

    if (NULL != default_config_home) {
        home_config_dir = default_config_home;
    } else {
        home_config_dir = xdg_config_home;
    }
    if (NULL != default_data_home) {
        home_data_dir = default_data_home;
    } else {
        home_data_dir = xdg_data_home;
    }
#endif  // !_WIN32

    for (uint32_t pass = 0; pass < 4; ++pass) {
        const char *relative_folder = NULL;
        char **target_pointer = NULL;
        char *cur_path_ptr = NULL;
        size_t search_path_size = 2;  // Add two for NULL terminator and one path separator (just in case)
        size_t rel_size = 0;
        bool settings_file_search = false;
        switch (pass) {
            case 0:
            default:
                relative_folder = relative_driver_folder;
                target_pointer = default_driver_path;
                break;
            case 1:
                relative_folder = relative_implicit_layer_folder;
                target_pointer = default_implicit_path;
                break;
            case 2:
                relative_folder = relative_explicit_layer_folder;
                target_pointer = default_explicit_path;
                break;
            case 3:
                relative_folder = relative_settings_folder;
                target_pointer = default_settings_path;
                settings_file_search = true;
                break;
        }

#if defined(_WIN32)
        if (NULL != package_path) {
            search_path_size += determine_data_file_path_size(package_path, 0) + 2;
        }
        if (search_path_size == 2) {
            continue;
        }
#else  // !_WIN32

        // Add the general search folders (with the appropriate relative folder added)
        rel_size = strlen(relative_folder);
#if defined(__APPLE__)
        search_path_size += MAXPATHLEN;
#endif
        // Only add the home folders if defined
        if (NULL != home_config_dir) {
            search_path_size += determine_data_file_path_size(home_config_dir, rel_size);
        }
        search_path_size += determine_data_file_path_size(xdg_config_dirs, rel_size);
        // Don't add system folders for settings file search
        if (!settings_file_search) {
            search_path_size += determine_data_file_path_size(SYSCONFDIR, rel_size);
#if defined(EXTRASYSCONFDIR)
            search_path_size += determine_data_file_path_size(EXTRASYSCONFDIR, rel_size);
#endif
        }
        // Only add the home folders if defined
        if (NULL != home_data_dir) {
            search_path_size += determine_data_file_path_size(home_data_dir, rel_size);
        }
        search_path_size += determine_data_file_path_size(xdg_data_dirs, rel_size);
#endif  // !_WIN32

        // Allocate the required space
        *target_pointer = loader_instance_heap_calloc(inst, search_path_size, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (NULL == *target_pointer) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                       "Failed to allocate space for default search path of %s files of length %d", curPassString[pass],
                       (uint32_t)search_path_size);
            result = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }

        cur_path_ptr = *target_pointer;

#if defined(_WIN32)
        if (NULL != package_path) {
            copy_data_file_info(package_path, NULL, 0, &cur_path_ptr);
        }
#else
#if defined(__APPLE__)
        // Add the bundle's Resources dir to the beginning of the search path.
        // Looks for manifests in the bundle first, before any system directories.
        CFBundleRef main_bundle = CFBundleGetMainBundle();
        if (NULL != main_bundle) {
            CFURLRef ref = CFBundleCopyResourcesDirectoryURL(main_bundle);
            if (NULL != ref) {
                if (CFURLGetFileSystemRepresentation(ref, TRUE, (UInt8 *)cur_path_ptr, search_path_size)) {
                    cur_path_ptr += strlen(cur_path_ptr);
                    *cur_path_ptr++ = DIRECTORY_SYMBOL;
                    memcpy(cur_path_ptr, relative_folder, rel_size);
                    cur_path_ptr += rel_size;
                    *cur_path_ptr++ = PATH_SEPARATOR;
                    if (manifest_type == LOADER_DATA_FILE_MANIFEST_DRIVER) {
                        use_first_found_manifest = true;
                    }
                }
                CFRelease(ref);
            }
        }
#endif  // __APPLE__

        // Only add the home folders if not NULL
        if (NULL != home_config_dir) {
            copy_data_file_info(home_config_dir, relative_folder, rel_size, &cur_path_ptr);
        }
        copy_data_file_info(xdg_config_dirs, relative_folder, rel_size, &cur_path_ptr);

        // Don't add system folders for settings file search
        if (!settings_file_search) {
            copy_data_file_info(SYSCONFDIR, relative_folder, rel_size, &cur_path_ptr);
#if defined(EXTRASYSCONFDIR)
            copy_data_file_info(EXTRASYSCONFDIR, relative_folder, rel_size, &cur_path_ptr);
#endif
        }

        // Only add the home folders if not NULL
        if (NULL != home_data_dir) {
            copy_data_file_info(home_data_dir, relative_folder, rel_size, &cur_path_ptr);
        }
        copy_data_file_info(xdg_data_dirs, relative_folder, rel_size, &cur_path_ptr);

        // Remove the last path separator
        --cur_path_ptr;

        assert(cur_path_ptr - (*target_pointer) < (ptrdiff_t)search_path_size);
        *cur_path_ptr = '\0';
#endif  // !_WIN32
    }

out:
    if (VK_SUCCESS != result) {
        freeSearchPaths(inst, default_driver_path, default_implicit_path, default_explicit_path, default_settings_path);
    }

#if defined(_WIN32)
    loader_instance_heap_free(inst, package_path);
#else
    loader_free_getenv(xdg_config_home, inst);
    if (env_xdg_config_dirs) {
        loader_free_getenv(xdg_config_dirs, inst);
    }
    loader_free_getenv(xdg_data_home, inst);
    if (env_xdg_data_dirs) {
        loader_free_getenv(xdg_data_dirs, inst);
    }
    loader_free_getenv(xdg_data_home, inst);
    loader_free_getenv(home, inst);
    loader_instance_heap_free(inst, default_data_home);
    loader_instance_heap_free(inst, default_config_home);
#endif

    return result;
}

// Read a path that is stored in an environment variable.
VkResult readEnvVarPath(const struct loader_instance *inst, uint32_t log_msg_flag, char **output_var, char *env_var) {
    if (output_var == NULL || env_var == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    char *env_var_value = loader_secure_getenv(env_var, inst);
    size_t env_var_size = env_var_value == NULL ? 0 : strlen(env_var_value);
    if (NULL != env_var_value && env_var_size > 1) {
        *output_var = loader_instance_heap_calloc(inst, env_var_size + 1, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (NULL == *output_var) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT | log_msg_flag, 0, "Failed alloc space for env var `%s` value", env_var);
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        } else {
            loader_log(inst, VULKAN_LOADER_INFO_BIT | log_msg_flag, 0, "Found environment var `%s`, using value: `%s`", env_var,
                       env_var_value);
            strncpy(*output_var, env_var_value, env_var_size);
            (*output_var)[env_var_size] = '\0';
        }
        loader_free_getenv(env_var_value, inst);
    }
    return VK_SUCCESS;
}

// Determine how many actual paths are listed in a path_list string
uint32_t getPathCount(char *path_list) {
    uint32_t count = 0;
    if (path_list == NULL) {
        return 0;
    }
    // Now, parse the paths
    char *cur_path;
    char *next_path = path_list;
    while (NULL != next_path && *next_path != '\0') {
        cur_path = next_path;
        next_path = loader_get_next_path(cur_path);

        // Is this a JSON file, then try to open it.
        size_t len = strlen(cur_path);
        if (len > 1) {
            count++;
        }
    }
    return count;
}

// Extract each individual paths listed in a path_list string and place it in a path_array for
// easier use.
VkResult addPathsToArray(const struct loader_instance *inst, uint32_t log_msg_flag, char *path_list, char **path_array,
                         uint32_t *start_index) {
    // Now, parse the paths
    char *cur_path;
    char *next_path = path_list;
    while (NULL != next_path && *next_path != '\0') {
        cur_path = next_path;
        next_path = loader_get_next_path(cur_path);

        // Is this a JSON file, then try to open it.
        size_t len = strlen(cur_path);
        if (len > 1) {
            path_array[*start_index] = loader_instance_heap_alloc(inst, len + 1, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
            if (path_array[*start_index] == NULL) {
                loader_log(inst, VULKAN_LOADER_ERROR_BIT | log_msg_flag, 0, "Failed alloc space for path search element %d",
                           *start_index);
                for (uint32_t count = 0; count < *start_index; ++count) {
                    loader_instance_heap_free(inst, path_array[count]);
                    path_array[count] = NULL;
                }
                *start_index = 0;
                return VK_ERROR_OUT_OF_HOST_MEMORY;
            }
            strncpy(path_array[*start_index], cur_path, len);
            path_array[*start_index][len] = '\0';
            (*start_index)++;
        }
    }
    return VK_SUCCESS;
}

// Generate the complete list of search paths using the environment variables (or file variables) and
// the default paths for this platform.
// NOTE: This can be overridden by the "override" layer later on.
VkResult generateCompleteSearchPath(const struct loader_instance *inst, enum loader_data_files_type search_type,
                                    const char *default_path, const char *override_path, const char *add_path, uint32_t *count,
                                    char ***value) {
    uint32_t path_count = 0;
    uint32_t cur_path_index = 0;
    enum vulkan_loader_debug_flags log_msg_flag;
    uint8_t message_index = 0;
    const char message_str[4][20] = {{"driver"}, {"implicit layer"}, {"explicit layer"}, {"settings file"}};

    if (default_path == NULL || count == NULL || value == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // Set to 0, just in case we have a failure.
    *count = 0;

    switch (search_type) {
        case LOADER_DATA_FILE_MANIFEST_DRIVER:
        default:
            log_msg_flag = VULKAN_LOADER_DRIVER_BIT;
            message_index = 0;
            break;
        case LOADER_DATA_FILE_MANIFEST_EXPLICIT_LAYER:
            log_msg_flag = VULKAN_LOADER_LAYER_BIT;
            message_index = 2;
            break;
        case LOADER_DATA_FILE_MANIFEST_IMPLICIT_LAYER:
            log_msg_flag = VULKAN_LOADER_LAYER_BIT;
            message_index = 1;
            break;
        case LOADER_DATA_FILE_SETTINGS_FILE:
            log_msg_flag = VULKAN_LOADER_INFO_BIT;
            message_index = 3;
            break;
    }

    char **path_array = NULL;
    char *modifiable_path = NULL;
    char *cur_mod_ptr;
    bool failed = false;

    if (NULL != override_path) {
        size_t cur_len = strlen(override_path);
        modifiable_path = loader_instance_heap_calloc(inst, cur_len + 1, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (modifiable_path == NULL) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT | log_msg_flag, 0, "Failed alloc space for modifiable path string");
            failed = true;
        }
        if (!failed) {
            cur_mod_ptr = modifiable_path;
            strncpy(cur_mod_ptr, override_path, cur_len);
            cur_mod_ptr[cur_len] = '\0';
            path_count = getPathCount(cur_mod_ptr);
            path_array = loader_instance_heap_calloc(inst, sizeof(char *) * path_count, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
            if (path_array == NULL) {
                loader_log(inst, VULKAN_LOADER_ERROR_BIT | log_msg_flag, 0, "Failed alloc space for %s override path search array",
                           message_str[message_index]);
                failed = true;
            }
        }
        if (!failed) {
            cur_mod_ptr = modifiable_path;
            strncpy(cur_mod_ptr, override_path, cur_len);
            cur_mod_ptr[cur_len] = '\0';
            if (addPathsToArray(inst, log_msg_flag, cur_mod_ptr, path_array, &cur_path_index)) {
                loader_log(inst, VULKAN_LOADER_ERROR_BIT | log_msg_flag, 0,
                           "Failed copying paths from %s override path into search array", message_str[message_index]);
                failed = true;
            }
        }
    } else {
        size_t add_len = add_path == NULL ? 0 : strlen(add_path);
        size_t default_len = default_path == NULL ? 0 : strlen(default_path);
        size_t max_len = add_len > default_len ? add_len : default_len;
        assert(max_len > 0);
        modifiable_path = loader_instance_heap_calloc(inst, max_len + 1, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
        if (modifiable_path == NULL) {
            loader_log(inst, VULKAN_LOADER_ERROR_BIT | log_msg_flag, 0, "Failed alloc space for modifiable path string");
            failed = true;
        }
        if (!failed) {
            path_count = 0;
            if (add_path != NULL) {
                cur_mod_ptr = modifiable_path;
                strncpy(cur_mod_ptr, add_path, add_len);
                cur_mod_ptr[add_len] = '\0';
                path_count += getPathCount(cur_mod_ptr);
            }
            if (default_path != NULL) {
                cur_mod_ptr = modifiable_path;
                strncpy(cur_mod_ptr, default_path, default_len);
                cur_mod_ptr[default_len] = '\0';
                path_count += getPathCount(cur_mod_ptr);
            }
            path_array = loader_instance_heap_calloc(inst, sizeof(char *) * path_count, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
            if (path_array == NULL) {
                loader_log(inst, VULKAN_LOADER_ERROR_BIT | log_msg_flag, 0, "Failed alloc space for standard %s path search array",
                           message_str[message_index]);
                failed = true;
            }
        }
        if (!failed && add_path != NULL) {
            cur_mod_ptr = modifiable_path;
            strncpy(cur_mod_ptr, add_path, add_len);
            cur_mod_ptr[add_len] = '\0';
            if (addPathsToArray(inst, log_msg_flag, cur_mod_ptr, path_array, &cur_path_index)) {
                loader_log(inst, VULKAN_LOADER_ERROR_BIT | log_msg_flag, 0,
                           "Failed copying paths from additional %s path into search array", message_str[message_index]);
                failed = true;
            }
        }
        if (!failed && default_path != NULL) {
            cur_mod_ptr = modifiable_path;
            strncpy(cur_mod_ptr, default_path, default_len);
            cur_mod_ptr[default_len] = '\0';
            if (addPathsToArray(inst, log_msg_flag, cur_mod_ptr, path_array, &cur_path_index)) {
                loader_log(inst, VULKAN_LOADER_ERROR_BIT | log_msg_flag, 0,
                           "Failed copying paths from default %s path into search array", message_str[message_index]);
                failed = true;
            }
        }
    }
    if (modifiable_path != NULL) {
        loader_instance_heap_free(inst, modifiable_path);
    }
    if (failed) {
        if (path_array != NULL) {
            loader_instance_heap_free(inst, path_array);
        }
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    // Remove duplicate paths, or it would result in duplicate extensions, duplicate devices, etc.
    for (int32_t cur_path = 0; cur_path < (int32_t)path_count; ++cur_path) {
        for (int32_t check_path = cur_path + 1; check_path < (int32_t)path_count; ++check_path) {
            if (!strcmp(path_array[check_path], path_array[cur_path])) {
                loader_instance_heap_free(inst, path_array[check_path]);
                for (int32_t copy_path = check_path; copy_path < (int32_t)path_count - 1; ++copy_path) {
                    path_array[copy_path] = path_array[copy_path + 1];
                }
                path_array[--path_count] = NULL;
                check_path--;
            }
        }
    }

    // Print out the paths being searched if debugging is enabled
    if (path_count > 0) {
        loader_log(inst, log_msg_flag, 0, "Search paths for %s manifest files set to following order:", message_str[message_index]);
        for (uint32_t cur_path = 0; cur_path < path_count; ++cur_path) {
            loader_log(inst, log_msg_flag, 0, "   %s", path_array[cur_path]);
        }
    } else {
        if (path_array != NULL) {
            loader_instance_heap_free(inst, path_array);
            path_array = NULL;
        }
    }

    assert(path_array != NULL);
    assert(path_count != 0);
    *count = path_count;
    *value = path_array;
    return VK_SUCCESS;
}

// Free the items that were allocated in the settings struct, and then the settings struct itself.
void freeSettingsStruct(struct loader_instance *inst, struct loader_settings **set_struct) {
    if (set_struct == NULL || *set_struct == NULL) {
        return;
    }
    struct loader_settings *settings = *set_struct;

    // Free log file info
    if (settings->log_settings.log_to_file) {
        loader_instance_heap_free(inst, settings->log_settings.log_filename);
        if (NULL != settings->log_settings.log_file) {
            fclose(settings->log_settings.log_file);
            settings->log_settings.log_file = NULL;
        }
        settings->log_settings.log_to_file = false;
    }
    settings->log_settings.log_filename = NULL;

    // Free device items
    if (NULL != settings->device_settings.device_select_string) {
        loader_instance_heap_free(inst, settings->device_settings.device_select_string);
        settings->device_settings.device_select_string = NULL;
    }

    // Free search paths
    if (NULL != settings->driver_search_paths) {
        for (uint32_t count = 0; count < settings->driver_search_paths_count; ++count) {
            loader_instance_heap_free(inst, settings->driver_search_paths[count]);
        }
        loader_instance_heap_free(inst, settings->driver_search_paths);
        settings->driver_search_paths_count = 0;
        settings->driver_search_paths = NULL;
    }
    if (NULL != settings->implicit_layer_search_paths) {
        for (uint32_t count = 0; count < settings->implicit_layer_search_paths_count; ++count) {
            loader_instance_heap_free(inst, settings->implicit_layer_search_paths[count]);
        }
        loader_instance_heap_free(inst, settings->implicit_layer_search_paths);
        settings->implicit_layer_search_paths_count = 0;
        settings->implicit_layer_search_paths = NULL;
    }
    if (NULL != settings->explicit_layer_search_paths) {
        for (uint32_t count = 0; count < settings->explicit_layer_search_paths_count; ++count) {
            loader_instance_heap_free(inst, settings->explicit_layer_search_paths[count]);
        }
        loader_instance_heap_free(inst, settings->explicit_layer_search_paths);
        settings->explicit_layer_search_paths_count = 0;
        settings->explicit_layer_search_paths = NULL;
    }

    // Free the rest of the struct
    loader_instance_heap_free(inst, settings);
    *set_struct = NULL;
}

// Generate a settings structure containing all the settings necessary for the loader.
// This is generated from platform defaults, environment variables, and the loader settings
// file.
VkResult generateSettingsStruct(struct loader_instance *inst, struct loader_settings **set_struct) {
    VkResult result = VK_SUCCESS;
    if (set_struct == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    *set_struct = NULL;
    struct loader_settings *settings =
        loader_instance_heap_calloc(inst, sizeof(struct loader_settings), VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
    if (settings == NULL) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "Failed allocating loader settings structure");
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    *set_struct = settings;

    // Search paths, these are eventually used to fill in the final search paths
    // for each driver, implicit layers, and explicit layers.
    char *default_driver_search_path = NULL;
    char *override_driver_env_var_search_path = NULL;
    char *override_driver_file_search_path = NULL;
    char *additional_driver_env_var_search_path = NULL;
    char *additional_driver_file_search_path = NULL;
    char *default_implicit_layer_search_path = NULL;
    char *default_explicit_layer_search_path = NULL;
    char *override_explicit_layer_env_var_search_path = NULL;
    char *override_explicit_layer_file_search_path = NULL;
    char *additional_explicit_layer_env_var_search_path = NULL;
    char *additional_explicit_layer_file_search_path = NULL;
    char *default_settings_file_search_path = NULL;
    uint32_t settings_file_search_paths_count = 0;
    char **settings_file_search_paths = NULL;

    // Set defaults
    settings->log_settings.log_errors_to_stderr = true;
    settings->log_settings.log_nonerrors_to_stdout = true;
    settings->driver_search_paths_count = 0;
    settings->driver_search_paths = NULL;
    settings->implicit_layer_search_paths_count = 0;
    settings->implicit_layer_search_paths = NULL;
    settings->explicit_layer_search_paths_count = 0;
    settings->explicit_layer_search_paths = NULL;
#if LOADER_ENABLE_LINUX_SORT
    settings->device_settings.device_sorting_enabled = true;
#else
    settings->device_settings.device_sorting_enabled = false;
#endif
    settings->device_settings.device_select_enabled = false;
    settings->device_settings.device_select_string = NULL;
    result = loaderDefaultSearchPaths(inst, &default_driver_search_path, &default_implicit_layer_search_path,
                                      &default_explicit_layer_search_path, &default_settings_file_search_path);
    if (VK_SUCCESS != result) {
        goto out;
    }

    // Find a settings file and load the settings out of that if one exists in one of the
    // appropriate locations.
    result = generateCompleteSearchPath(inst, LOADER_DATA_FILE_SETTINGS_FILE, default_settings_file_search_path, NULL, NULL,
                                        &settings_file_search_paths_count, &settings_file_search_paths);
    if (VK_SUCCESS != result) {
        goto out;
    }

    //
    // TODO: Read loader settings file here.
    //

    // Override any settings with environment variable versions
    parseEnvLoaderDebugLogFlags(&settings->log_settings.enabled_log_flags);
    parseEnvDisableInstanceExtensions(inst, &settings->disable_instance_extension_filter);
    result = parseEnvDeviceSettings(inst, &settings->device_settings);
    if (VK_SUCCESS != result) {
        goto out;
    }
    result = readEnvVarPath(inst, VULKAN_LOADER_DRIVER_BIT, &override_driver_env_var_search_path, VK_DRIVER_FILES_ENV_VAR);
    if (VK_SUCCESS != result) {
        result = readEnvVarPath(inst, VULKAN_LOADER_DRIVER_BIT, &override_driver_env_var_search_path, VK_ICD_FILENAMES_ENV_VAR);
        if (VK_SUCCESS != result) {
            goto out;
        }
    }
    result =
        readEnvVarPath(inst, VULKAN_LOADER_DRIVER_BIT, &additional_driver_env_var_search_path, VK_ADDITIONAL_DRIVER_FILES_ENV_VAR);
    if (VK_SUCCESS != result) {
        goto out;
    }
    result = readEnvVarPath(inst, VULKAN_LOADER_LAYER_BIT, &override_explicit_layer_env_var_search_path, VK_LAYER_PATH_ENV_VAR);
    if (VK_SUCCESS != result) {
        goto out;
    }
    result = readEnvVarPath(inst, VULKAN_LOADER_LAYER_BIT, &additional_explicit_layer_env_var_search_path,
                            VK_ADDITIONAL_LAYER_PATH_ENV_VAR);
    if (VK_SUCCESS != result) {
        goto out;
    }
    result = generateCompleteSearchPath(
        inst, LOADER_DATA_FILE_MANIFEST_DRIVER, default_driver_search_path,
        (override_driver_file_search_path == NULL) ? override_driver_env_var_search_path : override_driver_file_search_path,
        (additional_driver_file_search_path == NULL) ? additional_driver_env_var_search_path : additional_driver_file_search_path,
        &settings->driver_search_paths_count, &settings->driver_search_paths);
    if (VK_SUCCESS != result) {
        goto out;
    }
    result = generateCompleteSearchPath(inst, LOADER_DATA_FILE_MANIFEST_IMPLICIT_LAYER, default_implicit_layer_search_path, NULL,
                                        NULL, &settings->implicit_layer_search_paths_count, &settings->implicit_layer_search_paths);
    if (VK_SUCCESS != result) {
        goto out;
    }
    result = generateCompleteSearchPath(
        inst, LOADER_DATA_FILE_MANIFEST_EXPLICIT_LAYER, default_explicit_layer_search_path,
        (override_explicit_layer_file_search_path == NULL) ? override_explicit_layer_env_var_search_path
                                                           : override_explicit_layer_file_search_path,
        (additional_explicit_layer_file_search_path == NULL) ? additional_explicit_layer_env_var_search_path
                                                             : additional_explicit_layer_file_search_path,
        &settings->explicit_layer_search_paths_count, &settings->explicit_layer_search_paths);
    if (VK_SUCCESS != result) {
        goto out;
    }

out:
    if (settings_file_search_paths != NULL) {
        for (uint32_t count = 0; count < settings_file_search_paths_count; ++count) {
            loader_instance_heap_free(inst, settings_file_search_paths[count]);
        }
        loader_instance_heap_free(inst, settings_file_search_paths);
    }
    freeSearchPaths(inst, &default_driver_search_path, &default_implicit_layer_search_path, &default_explicit_layer_search_path,
                    &default_settings_file_search_path);
    freeSearchPaths(inst, &override_driver_env_var_search_path, &override_explicit_layer_env_var_search_path,
                    &additional_driver_env_var_search_path, &additional_explicit_layer_env_var_search_path);
    if (result != VK_SUCCESS) {
        freeSettingsStruct(inst, &settings);
        *set_struct = NULL;
    }
    return result;
}
