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

#include "loader_common.h"

#ifdef __cplusplus
extern "C" {
#endif

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
} cJSON;

/* Supply a block of JSON, and this returns a cJSON object you can interrogate.
 * Call cJSON_Delete when finished. */
cJSON *cJSON_Parse(const struct loader_instance *instance, const char *value);
/* Render a cJSON entity to text for transfer/storage. Free the char* when
 * finished. */
char *cJSON_Print(const struct loader_instance *instance, cJSON *item);
/* Render a cJSON entity to text for transfer/storage without any formatting.
 * Free the char* when finished. */
char *cJSON_PrintUnformatted(const struct loader_instance *instance, cJSON *item);
/* Render a cJSON entity to text using a buffered strategy. prebuffer is a guess
 * at the final size. guessing well reduces reallocation. fmt=0 gives
 * unformatted, =1 gives formatted */
char *cJSON_PrintBuffered(const struct loader_instance *instance, cJSON *item, int prebuffer, int fmt);
/* Delete a cJSON entity and all subentities. */
void cJSON_Delete(const struct loader_instance *instance, cJSON *c);
/* Delete an item allocated inside the JSON parser*/
void cJSON_Free(const struct loader_instance *instance, void *p);

/* Returns the number of items in an array (or object). */
int cJSON_GetArraySize(cJSON *array);
/* Retrieve item number "item" from array "array". Returns NULL if unsuccessful.
 */
cJSON *cJSON_GetArrayItem(cJSON *array, int item);
/* Get item "string" from object. Case insensitive. */
cJSON *cJSON_GetObjectItem(cJSON *object, const char *string);

/* For analysing failed parses. This returns a pointer to the parse error.
 * You'll probably need to look a few chars back to make sense of it. Defined
 * when cJSON_Parse() returns 0. 0 when cJSON_Parse() succeeds. */
const char *cJSON_GetErrorPtr(void);

/* These calls create a cJSON item of the appropriate type. */
cJSON *cJSON_CreateNull(const struct loader_instance *instance);
cJSON *cJSON_CreateTrue(const struct loader_instance *instance);
cJSON *cJSON_CreateFalse(const struct loader_instance *instance);
cJSON *cJSON_CreateBool(const struct loader_instance *instance, int b);
cJSON *cJSON_CreateNumber(const struct loader_instance *instance, double num);
cJSON *cJSON_CreateString(const struct loader_instance *instance, const char *string);
cJSON *cJSON_CreateArray(const struct loader_instance *instance);
cJSON *cJSON_CreateObject(const struct loader_instance *instance);

/* These utilities create an Array of count items. */
cJSON *cJSON_CreateIntArray(const struct loader_instance *instance, const int *numbers, int count);
cJSON *cJSON_CreateFloatArray(const struct loader_instance *instance, const float *numbers, int count);
cJSON *cJSON_CreateDoubleArray(const struct loader_instance *instance, const double *numbers, int count);
cJSON *cJSON_CreateStringArray(const struct loader_instance *instance, const char **strings, int count);

/* Append item to the specified array/object. */
void cJSON_AddItemToArray(cJSON *array, cJSON *item);
void cJSON_AddItemToObject(const struct loader_instance *instance, cJSON *object, const char *string, cJSON *item);
/* Use this when string is definitely const  (i.e. a literal, or as good as), and
 * will definitely survive the cJSON object */
void cJSON_AddItemToObjectCS(const struct loader_instance *instance, cJSON *object, const char *string, cJSON *item);
/* Append reference to item to the specified array/object. Use this when you
 * want to add an existing cJSON to a new cJSON, but don't want to corrupt your
 * existing cJSON. */
void cJSON_AddItemReferenceToArray(const struct loader_instance *instance, cJSON *array, cJSON *item);
void cJSON_AddItemReferenceToObject(const struct loader_instance *instance, cJSON *object, const char *string, cJSON *item);

/* Remove/Detatch items from Arrays/Objects. */
cJSON *cJSON_DetachItemFromArray(cJSON *array, int which);
void cJSON_DeleteItemFromArray(const struct loader_instance *instance, cJSON *array, int which);
cJSON *cJSON_DetachItemFromObject(cJSON *object, const char *string);
void cJSON_DeleteItemFromObject(const struct loader_instance *instance, cJSON *object, const char *string);

/* Update array items. */
void cJSON_InsertItemInArray(cJSON *array, int which, cJSON *newitem); /* Shifts pre-existing items to the right. */
void cJSON_ReplaceItemInArray(const struct loader_instance *instance, cJSON *array, int which, cJSON *newitem);
void cJSON_ReplaceItemInObject(const struct loader_instance *instance, cJSON *object, const char *string, cJSON *newitem);

/* Duplicate a cJSON item */
cJSON *cJSON_Duplicate(const struct loader_instance *instance, cJSON *item, int recurse);
/* Duplicate will create a new, identical cJSON item to the one you pass, in new
memory that will
need to be released. With recurse!=0, it will duplicate any children connected
to the item.
The item->next and ->prev pointers are always zero on return from Duplicate. */

/* ParseWithOpts allows you to require (and check) that the JSON is null
 * terminated, and to retrieve the pointer to the final byte parsed. */
cJSON *cJSON_ParseWithOpts(const struct loader_instance *instance, const char *value, const char **return_parse_end,
                           int require_null_terminated);

void cJSON_Minify(char *json);

/* Macros for creating things quickly. */
#define cJSON_AddNullToObject(object, name) cJSON_AddItemToObject(object, name, cJSON_CreateNull())
#define cJSON_AddTrueToObject(object, name) cJSON_AddItemToObject(object, name, cJSON_CreateTrue())
#define cJSON_AddFalseToObject(object, name) cJSON_AddItemToObject(object, name, cJSON_CreateFalse())
#define cJSON_AddBoolToObject(object, name, b) cJSON_AddItemToObject(object, name, cJSON_CreateBool(b))
#define cJSON_AddNumberToObject(object, name, n) cJSON_AddItemToObject(object, name, cJSON_CreateNumber(n))
#define cJSON_AddStringToObject(object, name, s) cJSON_AddItemToObject(object, name, cJSON_CreateString(s))

/* When assigning an integer value, it needs to be propagated to valuedouble
 * too. */
#define cJSON_SetIntValue(object, val) ((object) ? (object)->valueint = (object)->valuedouble = (val) : (val))
#define cJSON_SetNumberValue(object, val) ((object) ? (object)->valueint = (object)->valuedouble = (val) : (val))

#ifdef __cplusplus
}
#endif
