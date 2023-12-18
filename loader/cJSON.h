/*
  Copyright (c) 2009 Dave Gamble
  Copyright (c) 2015-2021 The Khronos Group Inc.
  Copyright (c) 2015-2021 Valve Corporation
  Copyright (c) 2015-2021 LunarG, Inc.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#pragma once

#include <stdint.h>

#include <vulkan/vulkan_core.h>

/* cJSON Types: */
#define cJSON_False 0
#define cJSON_True 1
#define cJSON_NULL 2
#define cJSON_Number 3
#define cJSON_String 4
#define cJSON_Array 5
#define cJSON_Object 6

#define cJSON_IsReference 256
#define cJSON_StringIsConst 512

/* The cJSON structure: */
typedef struct cJSON {
    struct cJSON *next, *prev; /* next/prev allow you to walk array/object
                                  chains. Alternatively, use
                                  GetArraySize/GetArrayItem/GetObjectItem */
    struct cJSON *child;       /* An array or object item will have a child pointer
                                  pointing to a chain of the items in the
                                  array/object. */

    int type; /* The type of the item, as above. */

    char *valuestring;  /* The item's string, if type==cJSON_String */
    int valueint;       /* The item's number, if type==cJSON_Number */
    double valuedouble; /* The item's number, if type==cJSON_Number */

    char *string; /* The item's name string, if this item is the child of, or is
                     in the list of subitems of an object. */
    /* pointer to the allocation callbacks to use */
    VkAllocationCallbacks *pAllocator;
} cJSON;

/* Render a cJSON entity to text for transfer/storage. Free the char* when
 * finished. */
char *loader_cJSON_Print(cJSON *item);
/* Render a cJSON entity to text for transfer/storage without any formatting.
 * Free the char* when finished. */
char *loader_cJSON_PrintUnformatted(cJSON *item);
/* Delete a cJSON entity and all subentities. */
void loader_cJSON_Delete(cJSON *c);

/* Returns the number of items in an array (or object). */
int loader_cJSON_GetArraySize(cJSON *array);
/* Retrieve item number "item" from array "array". Returns NULL if unsuccessful.
 */
cJSON *loader_cJSON_GetArrayItem(cJSON *array, int item);
/* Get item "string" from object. Case insensitive. */
cJSON *loader_cJSON_GetObjectItem(cJSON *object, const char *string);

/* When assigning an integer value, it needs to be propagated to valuedouble
 * too. */
#define cJSON_SetIntValue(object, val) ((object) ? (object)->valueint = (object)->valuedouble = (val) : (val))
#define cJSON_SetNumberValue(object, val) ((object) ? (object)->valueint = (object)->valuedouble = (val) : (val))

// Helper functions to using JSON

struct loader_instance;
struct loader_string_list;

// Read a JSON file into a buffer.
//
// @return -  A pointer to a cJSON object representing the JSON parse tree.
//            This returned buffer should be freed by caller.
VkResult loader_get_json(const struct loader_instance *inst, const char *filename, cJSON **json);

// Given a cJSON object, find the string associated with the key and puts an pre-allocated string into out_string.
// Length is given by out_str_len, and this function truncates the string with a null terminator if it the provided space isn't
// large enough.
VkResult loader_parse_json_string_to_existing_str(const struct loader_instance *inst, cJSON *object, const char *key,
                                                  size_t out_str_len, char *out_string);

// Given a cJSON object, find the string associated with the key and puts an allocated string into out_string.
// It is the callers responsibility to free out_string.
VkResult loader_parse_json_string(cJSON *object, const char *key, char **out_string);

// Given a cJSON object, find the array of strings assocated with they key and writes the count into out_count and data into
// out_array_of_strings. It is the callers responsibility to free out_array_of_strings.
VkResult loader_parse_json_array_of_strings(const struct loader_instance *inst, cJSON *object, const char *key,
                                            struct loader_string_list *string_list);
