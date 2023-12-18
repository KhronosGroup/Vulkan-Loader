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

/* cJSON */
/* JSON parser in C. */

#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"

#include "allocation.h"
#include "loader.h"
#include "log.h"

static void *cJSON_malloc(const VkAllocationCallbacks *pAllocator, size_t size) {
    return loader_calloc(pAllocator, size, VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
}

static void *cJSON_malloc_instance_scope(const VkAllocationCallbacks *pAllocator, size_t size) {
    return loader_calloc(pAllocator, size, VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
}

static void cJSON_Free(const VkAllocationCallbacks *pAllocator, void *pMemory) { loader_free(pAllocator, pMemory); }

/*
// commented out as it is unused - static error code channel requires external locks to be used.
static const char *ep;

const char *cJSON_GetErrorPtr(void) { return ep; }
*/

static char *cJSON_strdup(const VkAllocationCallbacks *pAllocator, const char *str) {
    size_t len;
    char *copy;

    len = strlen(str) + 1;
    copy = (char *)cJSON_malloc(pAllocator, len);
    if (!copy) return 0;
    memcpy(copy, str, len);
    return copy;
}

/* Internal constructor. */
static cJSON *cJSON_New_Item(const VkAllocationCallbacks *pAllocator) {
    cJSON *node = (cJSON *)cJSON_malloc(pAllocator, sizeof(cJSON));
    if (node) {
        memset(node, 0, sizeof(cJSON));
        node->pAllocator = (VkAllocationCallbacks *)pAllocator;
    }
    return node;
}

/* Delete a cJSON structure. */
void loader_cJSON_Delete(cJSON *c) {
    cJSON *next;
    while (c) {
        next = c->next;
        if (!(c->type & cJSON_IsReference) && c->child) loader_cJSON_Delete(c->child);
        if (!(c->type & cJSON_IsReference) && c->valuestring) cJSON_Free(c->pAllocator, c->valuestring);
        if (!(c->type & cJSON_StringIsConst) && c->string) cJSON_Free(c->pAllocator, c->string);
        cJSON_Free(c->pAllocator, c);
        c = next;
    }
}

/* Parse the input text to generate a number, and populate the result into item.
 */
static const char *parse_number(cJSON *item, const char *num) {
    double n = 0, sign = 1, scale = 0;
    int subscale = 0, signsubscale = 1;

    if (*num == '-') sign = -1, num++; /* Has sign? */
    if (*num == '0') num++;            /* is zero */
    if (*num >= '1' && *num <= '9') do
            n = (n * 10.0) + (*num++ - '0');
        while (*num >= '0' && *num <= '9'); /* Number? */
    if (*num == '.' && num[1] >= '0' && num[1] <= '9') {
        num++;
        do n = (n * 10.0) + (*num++ - '0'), scale--;
        while (*num >= '0' && *num <= '9');
    }                               /* Fractional part? */
    if (*num == 'e' || *num == 'E') /* Exponent? */
    {
        num++;
        if (*num == '+')
            num++;
        else if (*num == '-')
            signsubscale = -1, num++;                                                   /* With sign? */
        while (*num >= '0' && *num <= '9') subscale = (subscale * 10) + (*num++ - '0'); /* Number? */
    }

    n = sign * n * pow(10.0, (scale + subscale * signsubscale)); /* number = +/-
                                                                    number.fraction *
                                                                    10^+/- exponent */

    item->valuedouble = n;
    item->valueint = (int)n;
    item->type = cJSON_Number;
    return num;
}

static size_t pow2gt(size_t x) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

typedef struct {
    char *buffer;
    size_t length;
    size_t offset;
} printbuffer;

static char *ensure(const VkAllocationCallbacks *pAllocator, printbuffer *p, size_t needed) {
    char *newbuffer;
    size_t newsize;
    if (!p || !p->buffer) return 0;
    needed += p->offset;
    if (needed <= p->length) return p->buffer + p->offset;

    newsize = pow2gt(needed);
    newbuffer = (char *)cJSON_malloc(pAllocator, newsize);
    if (!newbuffer) {
        cJSON_Free(pAllocator, p->buffer);
        p->length = 0, p->buffer = 0;
        return 0;
    }
    if (newbuffer) memcpy(newbuffer, p->buffer, p->length);
    cJSON_Free(pAllocator, p->buffer);
    p->length = newsize;
    p->buffer = newbuffer;
    return newbuffer + p->offset;
}

static size_t cJSON_update(printbuffer *p) {
    char *str;
    if (!p || !p->buffer) return 0;
    str = p->buffer + p->offset;
    return p->offset + strlen(str);
}

/* Render the number nicely from the given item into a string. */
static char *print_number(cJSON *item, printbuffer *p) {
    char *str = 0;
    size_t str_buf_size;
    double d = item->valuedouble;
    if (d == 0) {
        str_buf_size = 2; /* special case for 0. */
        if (p)
            str = ensure(item->pAllocator, p, str_buf_size);
        else
            str = (char *)cJSON_malloc(item->pAllocator, str_buf_size);
        if (str) loader_strncpy(str, str_buf_size, "0", 2);
    } else if (fabs(((double)item->valueint) - d) <= DBL_EPSILON && d <= INT_MAX && d >= INT_MIN) {
        str_buf_size = 21; /* 2^64+1 can be represented in 21 chars. */
        if (p)
            str = ensure(item->pAllocator, p, str_buf_size);
        else
            str = (char *)cJSON_malloc(item->pAllocator, str_buf_size);
        if (str) snprintf(str, str_buf_size, "%d", item->valueint);
    } else {
        str_buf_size = 64; /* This is a nice tradeoff. */
        if (p)
            str = ensure(item->pAllocator, p, str_buf_size);
        else
            str = (char *)cJSON_malloc(item->pAllocator, str_buf_size);
        if (str) {
            if (fabs(floor(d) - d) <= DBL_EPSILON && fabs(d) < 1.0e60)
                snprintf(str, str_buf_size, "%.0f", d);
            else if (fabs(d) < 1.0e-6 || fabs(d) > 1.0e9)
                snprintf(str, str_buf_size, "%e", d);
            else
                snprintf(str, str_buf_size, "%f", d);
        }
    }
    return str;
}

static unsigned parse_hex4(const char *str) {
    unsigned h = 0;
    if (*str >= '0' && *str <= '9')
        h += (*str) - '0';
    else if (*str >= 'A' && *str <= 'F')
        h += 10 + (*str) - 'A';
    else if (*str >= 'a' && *str <= 'f')
        h += 10 + (*str) - 'a';
    else
        return 0;
    h = h << 4;
    str++;
    if (*str >= '0' && *str <= '9')
        h += (*str) - '0';
    else if (*str >= 'A' && *str <= 'F')
        h += 10 + (*str) - 'A';
    else if (*str >= 'a' && *str <= 'f')
        h += 10 + (*str) - 'a';
    else
        return 0;
    h = h << 4;
    str++;
    if (*str >= '0' && *str <= '9')
        h += (*str) - '0';
    else if (*str >= 'A' && *str <= 'F')
        h += 10 + (*str) - 'A';
    else if (*str >= 'a' && *str <= 'f')
        h += 10 + (*str) - 'a';
    else
        return 0;
    h = h << 4;
    str++;
    if (*str >= '0' && *str <= '9')
        h += (*str) - '0';
    else if (*str >= 'A' && *str <= 'F')
        h += 10 + (*str) - 'A';
    else if (*str >= 'a' && *str <= 'f')
        h += 10 + (*str) - 'a';
    else
        return 0;
    return h;
}

/* Parse the input text into an unescaped cstring, and populate item. */
static const unsigned char firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};
static const char *parse_string(cJSON *item, const char *str, bool *out_of_memory) {
    const char *ptr = str + 1;
    char *ptr2;
    char *out;
    int len = 0;
    unsigned uc, uc2;
    if (*str != '\"') {
        // ep = str; // commented out as it is unused
        return 0;
    } /* not a string! */

    while (*ptr != '\"' && *ptr && ++len)
        if (*ptr++ == '\\') ptr++; /* Skip escaped quotes. */

    out = (char *)cJSON_malloc(item->pAllocator, len + 1); /* This is how long we need for the string, roughly. */
    if (!out) {
        *out_of_memory = true;
        return 0;
    }

    ptr = str + 1;
    ptr2 = out;
    while (*ptr != '\"' && *ptr) {
        if (*ptr != '\\')
            *ptr2++ = *ptr++;
        else {
            ptr++;
            switch (*ptr) {
                case 'b':
                    *ptr2++ = '\b';
                    break;
                case 'f':
                    *ptr2++ = '\f';
                    break;
                case 'n':
                    *ptr2++ = '\n';
                    break;
                case 'r':
                    *ptr2++ = '\r';
                    break;
                case 't':
                    *ptr2++ = '\t';
                    break;
                case 'u': /* transcode utf16 to utf8. */
                    uc = parse_hex4(ptr + 1);
                    ptr += 4; /* get the unicode char. */

                    if ((uc >= 0xDC00 && uc <= 0xDFFF) || uc == 0) break; /* check for invalid.	*/

                    if (uc >= 0xD800 && uc <= 0xDBFF) /* UTF16 surrogate pairs.	*/
                    {
                        if (ptr[1] != '\\' || ptr[2] != 'u') break; /* missing second-half of surrogate.	*/
                        uc2 = parse_hex4(ptr + 3);
                        ptr += 6;
                        if (uc2 < 0xDC00 || uc2 > 0xDFFF) break; /* invalid second-half of surrogate.	*/
                        uc = 0x10000 + (((uc & 0x3FF) << 10) | (uc2 & 0x3FF));
                    }

                    len = 4;
                    if (uc < 0x80)
                        len = 1;
                    else if (uc < 0x800)
                        len = 2;
                    else if (uc < 0x10000)
                        len = 3;
                    ptr2 += len;

                    for (size_t i = len; i > 0; i--) {
                        if (i == 1) {
                            *--ptr2 = ((unsigned char)uc | firstByteMark[len]);
                        } else if (i >= 2) {
                            *--ptr2 = ((uc | 0x80) & 0xBF);
                            uc >>= 6;
                        }
                    }
                    ptr2 += len;
                    break;
                default:
                    *ptr2++ = *ptr;
                    break;
            }
            ptr++;
        }
    }
    *ptr2 = 0;
    if (*ptr == '\"') ptr++;
    item->valuestring = out;
    item->type = cJSON_String;
    return ptr;
}

/* Render the cstring provided to an escaped version that can be printed. */
static char *print_string_ptr(const VkAllocationCallbacks *pAllocator, const char *str, printbuffer *p) {
    const char *ptr;
    char *ptr2;
    char *out;
    size_t out_buf_size, len = 0, flag = 0;
    unsigned char token;

    for (ptr = str; *ptr; ptr++) flag |= ((*ptr > 0 && *ptr < 32) || (*ptr == '\"') || (*ptr == '\\')) ? 1 : 0;
    if (!flag) {
        len = ptr - str;
        out_buf_size = len + 1;
        // out_buf_size = len + 3; // Modified to not put quotes around the string
        if (p)
            out = ensure(pAllocator, p, out_buf_size);
        else
            out = (char *)cJSON_malloc_instance_scope(pAllocator, out_buf_size);
        if (!out) return 0;
        ptr2 = out;
        // *ptr2++ = '\"'; // Modified to not put quotes around the string
        loader_strncpy(ptr2, out_buf_size, str, out_buf_size);
        // ptr2[len] = '\"'; // Modified to not put quotes around the string
        ptr2[len] = 0;  // ptr2[len + 1] = 0; // Modified to not put quotes around the string
        return out;
    }

    if (!str) {
        out_buf_size = 3;
        if (p)
            out = ensure(pAllocator, p, out_buf_size);
        else
            out = (char *)cJSON_malloc_instance_scope(pAllocator, out_buf_size);
        if (!out) return 0;
        loader_strncpy(out, out_buf_size, "\"\"", 3);
        return out;
    }
    ptr = str;
    token = *ptr;
    while (token && ++len) {
        if (strchr("\"\\\b\f\n\r\t", token))
            len++;
        else if (token < 32)
            len += 5;
        ptr++;
        token = *ptr;
    }

    out_buf_size = len + 1;
    // out_buf_size = len + 3; // Modified to not put quotes around the string
    if (p)
        out = ensure(pAllocator, p, out_buf_size);
    else
        out = (char *)cJSON_malloc_instance_scope(pAllocator, out_buf_size);
    if (!out) return 0;

    ptr2 = out;
    ptr = str;
    // *ptr2++ = '\"'; // Modified to not put quotes around the string
    while (*ptr) {
        if ((unsigned char)*ptr > 31 && *ptr != '\"' && *ptr != '\\')
            *ptr2++ = *ptr++;
        else {
            switch (token = *ptr++) {
                case '\\':
                    *ptr2++ = '\\';
                    break;
                case '\"':
                    *ptr2++ = '\"';
                    break;
                case '\b':
                    *ptr2++ = '\b';
                    break;
                case '\f':
                    *ptr2++ = '\f';
                    break;
                case '\n':
                    *ptr2++ = '\n';
                    break;
                case '\r':
                    *ptr2++ = '\r';
                    break;
                case '\t':
                    *ptr2++ = '\t';
                    break;
                default:
                    snprintf(ptr2, out_buf_size - (ptr2 - out), "u%04x", token);
                    ptr2 += 5;
                    break; /* escape and print */
            }
        }
    }
    // *ptr2++ = '\"'; // Modified to not put quotes around the string
    *ptr2++ = 0;
    return out;
}
/* Invoke print_string_ptr (which is useful) on an item. */
static char *print_string(cJSON *item, printbuffer *p) { return print_string_ptr(item->pAllocator, item->valuestring, p); }

/* Predeclare these prototypes. */
static const char *parse_value(cJSON *item, const char *value, bool *out_of_memory);
static char *print_value(cJSON *item, int depth, int fmt, printbuffer *p);
static const char *parse_array(cJSON *item, const char *value, bool *out_of_memory);
static char *print_array(cJSON *item, int depth, int fmt, printbuffer *p);
static const char *parse_object(cJSON *item, const char *value, bool *out_of_memory);
static char *print_object(cJSON *item, int depth, int fmt, printbuffer *p);

/* Utility to jump whitespace and cr/lf */
static const char *skip(const char *in) {
    while (in && *in && (unsigned char)*in <= 32) in++;
    return in;
}

/* Parse an object - create a new root, and populate. */
static cJSON *cJSON_ParseWithOpts(const VkAllocationCallbacks *pAllocator, const char *value, const char **return_parse_end,
                                  int require_null_terminated, bool *out_of_memory) {
    const char *end = 0;
    cJSON *c = cJSON_New_Item(pAllocator);
    // ep = 0; // commented out as it is unused
    if (!c) {
        *out_of_memory = true;
        return 0; /* memory fail */
    }

    end = parse_value(c, skip(value), out_of_memory);
    if (!end) {
        loader_cJSON_Delete(c);
        return 0;
    } /* parse failure. ep is set. */

    /* if we require null-terminated JSON without appended garbage, skip and
     * then check for a null terminator */
    if (require_null_terminated) {
        end = skip(end);
        if (*end) {
            loader_cJSON_Delete(c);
            // ep = end; // commented out as it is unused
            return 0;
        }
    }
    if (return_parse_end) *return_parse_end = end;
    return c;
}
/* Default options for cJSON_Parse */
static cJSON *cJSON_Parse(const VkAllocationCallbacks *pAllocator, const char *value, bool *out_of_memory) {
    return cJSON_ParseWithOpts(pAllocator, value, 0, 0, out_of_memory);
}

/* Render a cJSON item/entity/structure to text. */
char *loader_cJSON_Print(cJSON *item) { return print_value(item, 0, 1, 0); }
char *loader_cJSON_PrintUnformatted(cJSON *item) { return print_value(item, 0, 0, 0); }

/* Parser core - when encountering text, process appropriately. */
static const char *parse_value(cJSON *item, const char *value, bool *out_of_memory) {
    if (!value) return 0; /* Fail on null. */
    if (!strncmp(value, "null", 4)) {
        item->type = cJSON_NULL;
        return value + 4;
    }
    if (!strncmp(value, "false", 5)) {
        item->type = cJSON_False;
        return value + 5;
    }
    if (!strncmp(value, "true", 4)) {
        item->type = cJSON_True;
        item->valueint = 1;
        return value + 4;
    }
    if (*value == '\"') {
        return parse_string(item, value, out_of_memory);
    }
    if (*value == '-' || (*value >= '0' && *value <= '9')) {
        return parse_number(item, value);
    }
    if (*value == '[') {
        return parse_array(item, value, out_of_memory);
    }
    if (*value == '{') {
        return parse_object(item, value, out_of_memory);
    }

    // ep = value; // commented out as it is unused
    return 0; /* failure. */
}

/* Render a value to text. */
static char *print_value(cJSON *item, int depth, int fmt, printbuffer *p) {
    char *out = 0;
    if (!item) return 0;
    if (p) {
        switch ((item->type) & 255) {
            case cJSON_NULL: {
                out = ensure(item->pAllocator, p, 5);
                if (out) loader_strncpy(out, 5, "null", 5);
                break;
            }
            case cJSON_False: {
                out = ensure(item->pAllocator, p, 6);
                if (out) loader_strncpy(out, 6, "false", 6);
                break;
            }
            case cJSON_True: {
                out = ensure(item->pAllocator, p, 5);
                if (out) loader_strncpy(out, 5, "true", 5);
                break;
            }
            case cJSON_Number:
                out = print_number(item, p);
                break;
            case cJSON_String:
                out = print_string(item, p);
                break;
            case cJSON_Array:
                out = print_array(item, depth, fmt, p);
                break;
            case cJSON_Object:
                out = print_object(item, depth, fmt, p);
                break;
        }
    } else {
        switch ((item->type) & 255) {
            case cJSON_NULL:
                out = cJSON_strdup(item->pAllocator, "null");
                break;
            case cJSON_False:
                out = cJSON_strdup(item->pAllocator, "false");
                break;
            case cJSON_True:
                out = cJSON_strdup(item->pAllocator, "true");
                break;
            case cJSON_Number:
                out = print_number(item, 0);
                break;
            case cJSON_String:
                out = print_string(item, 0);
                break;
            case cJSON_Array:
                out = print_array(item, depth, fmt, 0);
                break;
            case cJSON_Object:
                out = print_object(item, depth, fmt, 0);
                break;
        }
    }
    return out;
}

/* Build an array from input text. */
static const char *parse_array(cJSON *item, const char *value, bool *out_of_memory) {
    cJSON *child;
    if (*value != '[') {
        // ep = value; // commented out as it is unused
        return 0;
    } /* not an array! */

    item->type = cJSON_Array;
    value = skip(value + 1);
    if (*value == ']') return value + 1; /* empty array. */

    item->child = child = cJSON_New_Item(item->pAllocator);
    if (!item->child) {
        *out_of_memory = true;
        return 0; /* memory fail */
    }
    value = skip(parse_value(child, skip(value), out_of_memory)); /* skip any spacing, get the value. */
    if (!value) return 0;

    while (*value == ',') {
        cJSON *new_item;
        new_item = cJSON_New_Item(item->pAllocator);
        if (!new_item) {
            *out_of_memory = true;
            return 0; /* memory fail */
        }
        child->next = new_item;
        new_item->prev = child;
        child = new_item;
        value = skip(parse_value(child, skip(value + 1), out_of_memory));
        if (!value) return 0; /* memory fail */
    }

    if (*value == ']') return value + 1; /* end of array */
    // ep = value; // commented out as it is unused
    return 0; /* malformed. */
}

/* Render an array to text */
static char *print_array(cJSON *item, int depth, int fmt, printbuffer *p) {
    char **entries;
    char *out = 0, *ptr, *ret;
    size_t len = 5;
    cJSON *child = item->child;
    int numentries = 0, fail = 0, j = 0;
    size_t tmplen = 0, i = 0;

    /* How many entries in the array? */
    while (child) numentries++, child = child->next;
    /* Explicitly handle numentries==0 */
    if (!numentries) {
        if (p)
            out = ensure(item->pAllocator, p, 3);
        else
            out = (char *)cJSON_malloc(item->pAllocator, 3);
        if (out) loader_strncpy(out, 3, "[]", 3);
        return out;
    }

    if (p) {
        /* Compose the output array. */
        i = p->offset;
        ptr = ensure(item->pAllocator, p, 1);
        if (!ptr) return 0;
        *ptr = '[';
        p->offset++;
        child = item->child;
        while (child && !fail) {
            print_value(child, depth + 1, fmt, p);
            p->offset = cJSON_update(p);
            if (child->next) {
                len = fmt ? 2 : 1;
                ptr = ensure(item->pAllocator, p, len + 1);
                if (!ptr) return 0;
                *ptr++ = ',';
                if (fmt) *ptr++ = ' ';
                *ptr = 0;
                p->offset += len;
            }
            child = child->next;
        }
        ptr = ensure(item->pAllocator, p, 2);
        if (!ptr) return 0;
        *ptr++ = ']';
        *ptr = 0;
        out = (p->buffer) + i;
    } else {
        /* Allocate an array to hold the values for each */
        entries = (char **)cJSON_malloc(item->pAllocator, numentries * sizeof(char *));
        if (!entries) return 0;
        memset(entries, 0, numentries * sizeof(char *));
        /* Retrieve all the results: */
        child = item->child;
        while (child && !fail) {
            ret = print_value(child, depth + 1, fmt, 0);
            entries[i++] = ret;
            if (ret)
                len += strlen(ret) + 2 + (fmt ? 1 : 0);
            else
                fail = 1;
            child = child->next;
        }

        /* If we didn't fail, try to malloc the output string */
        if (!fail) out = (char *)cJSON_malloc(item->pAllocator, len);
        /* If that fails, we fail. */
        if (!out) fail = 1;

        /* Handle failure. */
        if (fail) {
            for (j = 0; j < numentries; j++)
                if (entries[j]) cJSON_Free(item->pAllocator, entries[j]);
            cJSON_Free(item->pAllocator, entries);
            return 0;
        }

        /* Compose the output array. */
        *out = '[';
        ptr = out + 1;
        *ptr = 0;
        for (j = 0; j < numentries; j++) {
            tmplen = strlen(entries[j]);
            memcpy(ptr, entries[j], tmplen);
            ptr += tmplen;
            if (j != numentries - 1) {
                *ptr++ = ',';
                if (fmt) *ptr++ = ' ';
                *ptr = 0;
            }
            cJSON_Free(item->pAllocator, entries[j]);
        }
        cJSON_Free(item->pAllocator, entries);
        *ptr++ = ']';
        *ptr++ = 0;
    }
    return out;
}

/* Build an object from the text. */
static const char *parse_object(cJSON *item, const char *value, bool *out_of_memory) {
    cJSON *child;
    if (*value != '{') {
        // ep = value; // commented out as it is unused
        return 0;
    } /* not an object! */

    item->type = cJSON_Object;
    value = skip(value + 1);
    if (*value == '}') return value + 1; /* empty array. */

    item->child = child = cJSON_New_Item(item->pAllocator);
    if (!item->child) {
        *out_of_memory = true;
        return 0;
    }
    value = skip(parse_string(child, skip(value), out_of_memory));
    if (!value) return 0;
    child->string = child->valuestring;
    child->valuestring = 0;
    if (*value != ':') {
        // ep = value; // commented out as it is unused
        return 0;
    }                                                                 /* fail! */
    value = skip(parse_value(child, skip(value + 1), out_of_memory)); /* skip any spacing, get the value. */
    if (!value) return 0;

    while (*value == ',') {
        cJSON *new_item;
        new_item = cJSON_New_Item(item->pAllocator);
        if (!new_item) {
            *out_of_memory = true;
            return 0; /* memory fail */
        }
        child->next = new_item;
        new_item->prev = child;
        child = new_item;
        value = skip(parse_string(child, skip(value + 1), out_of_memory));
        if (!value) return 0;
        child->string = child->valuestring;
        child->valuestring = 0;
        if (*value != ':') {
            // ep = value; // commented out as it is unused
            return 0;
        }                                                                 /* fail! */
        value = skip(parse_value(child, skip(value + 1), out_of_memory)); /* skip any spacing, get the value. */
        if (!value) return 0;
    }

    if (*value == '}') return value + 1; /* end of array */
    // ep = value; // commented out as it is unused
    return 0; /* malformed. */
}

/* Render an object to text. */
static char *print_object(cJSON *item, int depth, int fmt, printbuffer *p) {
    char **entries = 0, **names = 0;
    char *out = 0, *ptr, *ret, *str;
    int j;
    cJSON *child = item->child;
    int numentries = 0, fail = 0, k;
    size_t tmplen = 0, i = 0, len = 7;
    /* Count the number of entries. */
    while (child) numentries++, child = child->next;
    /* Explicitly handle empty object case */
    if (!numentries) {
        if (p)
            out = ensure(item->pAllocator, p, fmt ? depth + 4 : 3);
        else
            out = (char *)cJSON_malloc(item->pAllocator, fmt ? depth + 4 : 3);
        if (!out) return 0;
        ptr = out;
        *ptr++ = '{';
        if (fmt) {
            *ptr++ = '\n';
            for (j = 0; j < depth - 1; j++) *ptr++ = '\t';
        }
        *ptr++ = '}';
        *ptr++ = 0;
        return out;
    }
    if (p) {
        /* Compose the output: */
        i = p->offset;
        len = fmt ? 2 : 1;
        ptr = ensure(item->pAllocator, p, len + 1);
        if (!ptr) return 0;
        *ptr++ = '{';
        if (fmt) *ptr++ = '\n';
        *ptr = 0;
        p->offset += len;
        child = item->child;
        depth++;
        while (child) {
            if (fmt) {
                ptr = ensure(item->pAllocator, p, depth);
                if (!ptr) return 0;
                for (j = 0; j < depth; j++) *ptr++ = '\t';
                p->offset += depth;
            }
            print_string_ptr(item->pAllocator, child->string, p);
            p->offset = cJSON_update(p);

            len = fmt ? 2 : 1;
            ptr = ensure(item->pAllocator, p, len);
            if (!ptr) return 0;
            *ptr++ = ':';
            if (fmt) *ptr++ = '\t';
            p->offset += len;

            print_value(child, depth, fmt, p);
            p->offset = cJSON_update(p);

            len = (fmt ? 1 : 0) + (child->next ? 1 : 0);
            ptr = ensure(item->pAllocator, p, len + 1);
            if (!ptr) return 0;
            if (child->next) *ptr++ = ',';
            if (fmt) *ptr++ = '\n';
            *ptr = 0;
            p->offset += len;
            child = child->next;
        }
        ptr = ensure(item->pAllocator, p, fmt ? (depth + 1) : 2);
        if (!ptr) return 0;
        if (fmt)
            for (j = 0; j < depth - 1; j++) *ptr++ = '\t';
        *ptr++ = '}';
        *ptr = 0;
        out = (p->buffer) + i;
    } else {
        /* Allocate space for the names and the objects */
        entries = (char **)cJSON_malloc(item->pAllocator, numentries * sizeof(char *));
        if (!entries) return 0;
        names = (char **)cJSON_malloc(item->pAllocator, numentries * sizeof(char *));
        if (!names) {
            cJSON_Free(item->pAllocator, entries);
            return 0;
        }
        memset(entries, 0, sizeof(char *) * numentries);
        memset(names, 0, sizeof(char *) * numentries);

        /* Collect all the results into our arrays: */
        child = item->child;
        depth++;
        if (fmt) len += depth;
        while (child) {
            names[i] = str = print_string_ptr(item->pAllocator, child->string, 0);
            entries[i++] = ret = print_value(child, depth, fmt, 0);
            if (str && ret)
                len += strlen(ret) + strlen(str) + 2 + (fmt ? 2 + depth : 0);
            else
                fail = 1;
            child = child->next;
        }

        /* Try to allocate the output string */
        if (!fail) out = (char *)cJSON_malloc(item->pAllocator, len);
        if (!out) fail = 1;

        /* Handle failure */
        if (fail) {
            for (j = 0; j < numentries; j++) {
                if (names[i]) cJSON_Free(item->pAllocator, names[j]);
                if (entries[j]) cJSON_Free(item->pAllocator, entries[j]);
            }
            cJSON_Free(item->pAllocator, names);
            cJSON_Free(item->pAllocator, entries);
            return 0;
        }

        /* Compose the output: */
        *out = '{';
        ptr = out + 1;
        if (fmt) *ptr++ = '\n';
        *ptr = 0;
        for (j = 0; j < numentries; j++) {
            if (fmt)
                for (k = 0; k < depth; k++) *ptr++ = '\t';
            tmplen = strlen(names[j]);
            memcpy(ptr, names[j], tmplen);
            ptr += tmplen;
            *ptr++ = ':';
            if (fmt) *ptr++ = '\t';
            size_t entries_size = strlen(entries[j]);
            loader_strncpy(ptr, len - (ptr - out), entries[j], entries_size);
            ptr += entries_size;
            if (j != numentries - 1) *ptr++ = ',';
            if (fmt) *ptr++ = '\n';
            *ptr = 0;
            cJSON_Free(item->pAllocator, names[j]);
            cJSON_Free(item->pAllocator, entries[j]);
        }

        cJSON_Free(item->pAllocator, names);
        cJSON_Free(item->pAllocator, entries);
        if (fmt)
            for (j = 0; j < depth - 1; j++) *ptr++ = '\t';
        *ptr++ = '}';
        *ptr++ = 0;
    }
    return out;
}

/* Get Array size/item / object item. */
int loader_cJSON_GetArraySize(cJSON *array) {
    cJSON *c = array->child;
    int i = 0;
    while (c) i++, c = c->next;
    return i;
}
cJSON *loader_cJSON_GetArrayItem(cJSON *array, int item) {
    cJSON *c = array->child;
    while (c && item > 0) item--, c = c->next;
    return c;
}
cJSON *loader_cJSON_GetObjectItem(cJSON *object, const char *string) {
    cJSON *c = object->child;
    while (c && strcmp(c->string, string)) c = c->next;
    return c;
}

VkResult loader_get_json(const struct loader_instance *inst, const char *filename, cJSON **json) {
    FILE *file = NULL;
    char *json_buf = NULL;
    size_t len;
    VkResult res = VK_SUCCESS;

    assert(json != NULL);

    *json = NULL;

#if defined(_WIN32)
    int filename_utf16_size = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
    if (filename_utf16_size > 0) {
        wchar_t *filename_utf16 = (wchar_t *)loader_stack_alloc(filename_utf16_size * sizeof(wchar_t));
        if (MultiByteToWideChar(CP_UTF8, 0, filename, -1, filename_utf16, filename_utf16_size) == filename_utf16_size) {
            errno_t wfopen_error = _wfopen_s(&file, filename_utf16, L"rb");
            if (0 != wfopen_error) {
                loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_get_json: Failed to open JSON file %s", filename);
            }
        }
    }
#elif COMMON_UNIX_PLATFORMS
    file = fopen(filename, "rb");
#else
#warning fopen not available on this platform
#endif

    if (!file) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_get_json: Failed to open JSON file %s", filename);
        res = VK_ERROR_INITIALIZATION_FAILED;
        goto out;
    }
    // NOTE: We can't just use fseek(file, 0, SEEK_END) because that isn't guaranteed to be supported on all systems
    size_t fread_ret_count = 0;
    do {
        char buffer[256];
        fread_ret_count = fread(buffer, 1, 256, file);
    } while (fread_ret_count == 256 && !feof(file));
    len = ftell(file);
    fseek(file, 0, SEEK_SET);
    json_buf = (char *)loader_instance_heap_calloc(inst, len + 1, VK_SYSTEM_ALLOCATION_SCOPE_COMMAND);
    if (json_buf == NULL) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0,
                   "loader_get_json: Failed to allocate space for JSON file %s buffer of length %lu", filename, len);
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    }
    if (fread(json_buf, sizeof(char), len, file) != len) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_get_json: Failed to read JSON file %s.", filename);
        res = VK_ERROR_INITIALIZATION_FAILED;
        goto out;
    }
    json_buf[len] = '\0';

    // Can't be a valid json if the string is of length zero
    if (len == 0) {
        res = VK_ERROR_INITIALIZATION_FAILED;
        goto out;
    }
    // Parse text from file
    bool out_of_memory = false;
    *json = cJSON_Parse(inst ? &inst->alloc_callbacks : NULL, json_buf, &out_of_memory);
    if (out_of_memory) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_get_json: Out of Memory error occured while parsing JSON file %s.",
                   filename);
        res = VK_ERROR_OUT_OF_HOST_MEMORY;
        goto out;
    } else if (*json == NULL) {
        loader_log(inst, VULKAN_LOADER_ERROR_BIT, 0, "loader_get_json: Invalid JSON file %s.", filename);
        goto out;
    }

out:
    loader_instance_heap_free(inst, json_buf);
    if (NULL != file) {
        fclose(file);
    }
    if (res != VK_SUCCESS && *json != NULL) {
        loader_cJSON_Delete(*json);
        *json = NULL;
    }

    return res;
}

VkResult loader_parse_json_string_to_existing_str(const struct loader_instance *inst, cJSON *object, const char *key,
                                                  size_t out_str_len, char *out_string) {
    cJSON *item = loader_cJSON_GetObjectItem(object, key);
    if (NULL == item) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    char *str = loader_cJSON_Print(item);
    if (str == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    if (NULL != out_string) {
        loader_strncpy(out_string, out_str_len, str, out_str_len);
        if (out_str_len > 0) {
            out_string[out_str_len - 1] = '\0';
        }
    }
    loader_instance_heap_free(inst, str);
    return VK_SUCCESS;
}

VkResult loader_parse_json_string(cJSON *object, const char *key, char **out_string) {
    cJSON *item = loader_cJSON_GetObjectItem(object, key);
    if (NULL == item) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    char *str = loader_cJSON_Print(item);
    if (str == NULL) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    if (NULL != out_string) {
        *out_string = str;
    }
    return VK_SUCCESS;
}
VkResult loader_parse_json_array_of_strings(const struct loader_instance *inst, cJSON *object, const char *key,
                                            struct loader_string_list *string_list) {
    VkResult res = VK_SUCCESS;
    cJSON *item = loader_cJSON_GetObjectItem(object, key);
    if (NULL == item) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    uint32_t count = loader_cJSON_GetArraySize(item);
    if (count == 0) {
        return VK_SUCCESS;
    }

    res = create_string_list(inst, count, string_list);
    if (VK_ERROR_OUT_OF_HOST_MEMORY == res) {
        goto out;
    }
    for (uint32_t i = 0; i < count; i++) {
        cJSON *element = loader_cJSON_GetArrayItem(item, i);
        if (element == NULL) {
            return VK_ERROR_INITIALIZATION_FAILED;
        }
        char *out_data = loader_cJSON_Print(element);
        if (out_data == NULL) {
            res = VK_ERROR_OUT_OF_HOST_MEMORY;
            goto out;
        }
        res = append_str_to_string_list(inst, string_list, out_data);
        if (VK_ERROR_OUT_OF_HOST_MEMORY == res) {
            goto out;
        }
    }
out:
    if (res == VK_ERROR_OUT_OF_HOST_MEMORY && NULL != string_list->list) {
        free_string_list(inst, string_list);
    }

    return res;
}
