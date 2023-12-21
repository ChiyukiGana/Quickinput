/*
 Copyright (c) 2009 Dave Gamble

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

 /*******************************************************************************
  * Project:  neb
  * @file     CJsonObject.hpp
   * @brief    Json
   * @author   bwarliao
   * @date:    2014-7-16
   * @note
   * Modify history:
   ******************************************************************************/

#pragma warning (disable: 4996)

#ifndef CJSONOBJECT_H_
#define CJSONOBJECT_H_

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <malloc.h>
#include <math.h>
#include <string>
#include <list>

#if __cplusplus < 201101L
#include <map>
#else
#include <unordered_map>
#endif

#ifdef _WIN32
#define snprintf _snprintf_s
#endif

#ifndef INT_MAX
#define INT_MAX 2147483647
#define INT_MIN (-INT_MAX - 1)
#define UINT_MAX   4294967295U
#endif

   /* cJSON Types: */
#define cJSON_False 0
#define cJSON_True 1
#define cJSON_NULL 2
#define cJSON_Int 3
#define cJSON_Double 4
#define cJSON_String 5
#define cJSON_Array 6
#define cJSON_Object 7
#define cJSON_IsReference 256

typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;

/* The cJSON structure: */
typedef struct cJSON
{
	struct cJSON* next, * prev; /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
	struct cJSON* child; /* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */

	int type; /* The type of the item, as above. */

	char* valuestring; /* The item's string, if type==cJSON_String */
	int64 valueint; /* The item's number, if type==cJSON_Number */
	double valuedouble; /* The item's number, if type==cJSON_Number */
	int sign;   /* sign of valueint, 1(unsigned), -1(signed) */

	char* string; /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
} cJSON;

typedef struct cJSON_Hooks
{
	void* (*malloc_fn)(size_t sz);
	void (*free_fn)(void* ptr);
} cJSON_Hooks;

static void* (*cJSON_malloc)(size_t sz) = malloc;
static void (*cJSON_free)(void* ptr) = free;

static int cJSON_strcasecmp(const char* s1, const char* s2)
{
	if (!s1)
		return (s1 == s2) ? 0 : 1;
	if (!s2)
		return 1;
	for (; tolower(*s1) == tolower(*s2); ++s1, ++s2)
		if (*s1 == 0)
			return 0;
	return tolower(*(const unsigned char*)s1)
		- tolower(*(const unsigned char*)s2);
}
static char* cJSON_strdup(const char* str)
{
	size_t len;
	char* copy;

	len = strlen(str) + 1;
	if (!(copy = (char*)cJSON_malloc(len)))
		return 0;
	memcpy(copy, str, len);
	return copy;
}
/* Internal constructor. */
static cJSON* cJSON_New_Item()
{
	cJSON* node = (cJSON*)cJSON_malloc(sizeof(cJSON));
	if (node)
		memset(node, 0, sizeof(cJSON));
	return node;
}


/* Utility to jump whitespace and cr/lf */
static const char* skip(const char* in)
{
	while (in && *in && (unsigned char)*in <= 32)
		in++;
	return in;
}


/* Parse the input text into an unescaped cstring, and populate item. */
static const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
static const char* parse_value(cJSON* item, const char* value, const char** ep);
/* Parse the input text to generate a number, and populate the result into item. */
static const char* parse_number(cJSON* item, const char* num)
{
	int64 n = 0;
	long double d = 0.0;
	double base = 0.0;
	double point = 0.1;
	int scale = 0.0;
	int subscale = 0;
	int signsubscale = 1;
	item->sign = 1;

	/* Could use sscanf for this? */
	if (*num == '-')
		item->sign = -1, num++; /* Has sign? */
	if (*num == '0')
		num++; /* is zero */
	while (*num >= '0' && *num <= '9')
	{
		n = (n * 10) + (*num++ - '0');
	}
	d = n;
	if (*num == '.' && num[1] >= '0' && num[1] <= '9')
	{
		num++;
		base = d;
		do
		{
			d += point * (*num - '0');
			point *= 0.1;
			base = (base * 10.0) + (*num - '0'), scale--;
			++num;
		} while (*num >= '0' && *num <= '9');
	} /* Fractional part? */
	if (*num == 'e' || *num == 'E') /* Exponent? */
	{
		num++;
		if (*num == '+')
			num++;
		else if (*num == '-')
			signsubscale = -1, num++; /* With sign? */
		while (*num >= '0' && *num <= '9')
			subscale = (subscale * 10) + (*num++ - '0'); /* Number? */
	}

	if (scale == 0 && subscale == 0)
	{
		item->valuedouble = item->sign * d;
		item->valueint = item->sign * n;
		item->type = cJSON_Int;
	}
	else
	{
		if (subscale != 0)
		{
			d = item->sign * base * pow(10.0, (scale + subscale * signsubscale)); /* number = +/- number.fraction * 10^+/- exponent */
		}
		else {
			d = item->sign * d;
		}
		item->valuedouble = d;
		item->valueint = n;
		item->type = cJSON_Double;
	}
	return num;
}
static const char* parse_string(cJSON* item, const char* str, const char** ep)
{
	const char* ptr = str + 1;
	char* ptr2;
	char* out;
	int len = 0;
	unsigned uc, uc2;
	if (*str != '\"')
	{
		*ep = str;
		return 0;
	} /* not a string! */

	while (*ptr != '\"' && *ptr && ++len)
		if (*ptr++ == '\\')
			ptr++; /* Skip escaped quotes. */

	out = (char*)cJSON_malloc(len + 1); /* This is how long we need for the string, roughly. */
	if (!out)
		return 0;

	ptr = str + 1;
	ptr2 = out;
	while (*ptr != '\"' && *ptr)
	{
		if (*ptr != '\\')
			*ptr2++ = *ptr++;
		else
		{
			ptr++;
			switch (*ptr)
			{
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
				sscanf(ptr + 1, "%4x", &uc);
				ptr += 4; /* get the unicode char. */

				if ((uc >= 0xDC00 && uc <= 0xDFFF) || uc == 0)
					break;	// check for invalid.

				if (uc >= 0xD800 && uc <= 0xDBFF)	// UTF16 surrogate pairs.
				{
					if (ptr[1] != '\\' || ptr[2] != 'u')
						break;	// missing second-half of surrogate.
					sscanf(ptr + 3, "%4x", &uc2);
					ptr += 6;
					if (uc2 < 0xDC00 || uc2 > 0xDFFF)
						break;	// invalid second-half of surrogate.
					uc = 0x10000 | ((uc & 0x3FF) << 10) | (uc2 & 0x3FF);
				}

				len = 4;
				if (uc < 0x80)
					len = 1;
				else if (uc < 0x800)
					len = 2;
				else if (uc < 0x10000)
					len = 3;
				ptr2 += len;

				switch (len)
				{
				case 4:
					*--ptr2 = ((uc | 0x80) & 0xBF);
					uc >>= 6;
				case 3:
					*--ptr2 = ((uc | 0x80) & 0xBF);
					uc >>= 6;
				case 2:
					*--ptr2 = ((uc | 0x80) & 0xBF);
					uc >>= 6;
				case 1:
					*--ptr2 = (uc | firstByteMark[len]);
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
	if (*ptr == '\"')
		ptr++;
	item->valuestring = out;
	item->type = cJSON_String;
	return ptr;
}
/* Build an array from input text. */
static const char* parse_array(cJSON* item, const char* value, const char** ep)
{
	cJSON* child;
	if (*value != '[')
	{
		*ep = value;
		return 0;
	} /* not an array! */

	item->type = cJSON_Array;
	value = skip(value + 1);
	if (*value == ']')
		return value + 1; /* empty array. */

	item->child = child = cJSON_New_Item();
	if (!item->child)
		return 0; /* memory fail */
	value = skip(parse_value(child, skip(value), ep)); /* skip any spacing, get the value. */
	if (!value)
		return 0;

	while (*value == ',')
	{
		cJSON* new_item;
		if (!(new_item = cJSON_New_Item()))
			return 0; /* memory fail */
		child->next = new_item;
		new_item->prev = child;
		child = new_item;
		value = skip(parse_value(child, skip(value + 1), ep));
		if (!value)
			return 0; /* memory fail */
	}

	if (*value == ']')
		return value + 1; /* end of array */
	*ep = value;
	return 0; /* malformed. */
}
/* Build an object from the text. */
static const char* parse_object(cJSON* item, const char* value, const char** ep)
{
	cJSON* child;
	if (*value != '{')
	{
		*ep = value;
		return 0;
	} /* not an object! */

	item->type = cJSON_Object;
	value = skip(value + 1);
	if (*value == '}')
		return value + 1; /* empty array. */

	item->child = child = cJSON_New_Item();
	if (!item->child)
		return 0;
	value = skip(parse_string(child, skip(value), ep));
	if (!value)
		return 0;
	child->string = child->valuestring;
	child->valuestring = 0;
	if (*value != ':')
	{
		*ep = value;
		return 0;
	} /* fail! */
	value = skip(parse_value(child, skip(value + 1), ep)); /* skip any spacing, get the value. */
	if (!value)
		return 0;

	while (*value == ',')
	{
		cJSON* new_item;
		if (!(new_item = cJSON_New_Item()))
			return 0; /* memory fail */
		child->next = new_item;
		new_item->prev = child;
		child = new_item;
		value = skip(parse_string(child, skip(value + 1), ep));
		if (!value)
			return 0;
		child->string = child->valuestring;
		child->valuestring = 0;
		if (*value != ':')
		{
			*ep = value;
			return 0;
		} /* fail! */
		value = skip(parse_value(child, skip(value + 1), ep)); /* skip any spacing, get the value. */
		if (!value)
			return 0;
	}

	if (*value == '}')
		return value + 1; /* end of array */
	*ep = value;
	return 0; /* malformed. */
}
/* Parser core - when encountering text, process appropriately. */
static const char* parse_value(cJSON* item, const char* value, const char** ep)
{
	if (!value)
		return 0; /* Fail on null. */
	if (!strncmp(value, "null", 4))
	{
		item->type = cJSON_NULL;
		return value + 4;
	}
	if (!strncmp(value, "false", 5))
	{
		item->type = cJSON_False;
		return value + 5;
	}
	if (!strncmp(value, "true", 4))
	{
		item->type = cJSON_True;
		item->valueint = 1;
		return value + 4;
	}
	if (*value == '\"')
	{
		return parse_string(item, value, ep);
	}
	if (*value == '-' || (*value >= '0' && *value <= '9'))
	{
		return parse_number(item, value);
	}
	if (*value == '[')
	{
		return parse_array(item, value, ep);
	}
	if (*value == '{')
	{
		return parse_object(item, value, ep);
	}

	*ep = value;
	return 0; /* failure. */
}


static char* print_value(cJSON* item, int depth, int fmt);
/* Render the number nicely from the given item into a string. */
static char* print_int(cJSON* item)
{
	char* str;
	str = (char*)cJSON_malloc(22); /* 2^64+1 can be represented in 21 chars. */
	if (str)
	{
		if (item->sign == -1)
		{
			if ((int64)item->valueint <= (int64)INT_MAX && (int64)item->valueint >= (int64)INT_MIN)
			{
				sprintf(str, "%d", (int32)item->valueint);
			}
			else
			{
#if LLONG_MAX==LONG_MAX
				sprintf(str, "%ld", (int64)item->valueint);
#else
				sprintf(str, "%lld", (int64)item->valueint);
#endif
			}
		}
		else
		{
			if (item->valueint <= (uint64)UINT_MAX)
			{
				sprintf(str, "%u", (uint32)item->valueint);
			}
			else
			{
#if LLONG_MAX==LONG_MAX
				sprintf(str, "%lu", (uint64)item->valueint);
#else
				sprintf(str, "%llu", (uint64)item->valueint);
#endif
			}
		}
	}
	return str;
}
static char* print_double(cJSON* item)
{
	char* str;
	double d = item->valuedouble;
	str = (char*)cJSON_malloc(64); /* This is a nice tradeoff. */
	if (str)
	{
		sprintf(str, "%.15f", d);
	}
	return str;
}
/* Render the cstring provided to an escaped version that can be printed. */
static char* print_string_ptr(const char* str)
{
	const char* ptr;
	char* ptr2, * out;
	int len = 0;
	unsigned char token;

	if (!str)
		return cJSON_strdup("");
	ptr = str;
	while ((token = *ptr) && ++len)
	{
		if (strchr("\"\\\b\f\n\r\t", token))
			len++;
		else if (token < 32)
			len += 5;
		ptr++;
	}

	out = (char*)cJSON_malloc(len + 3);
	if (!out)
		return 0;

	ptr2 = out;
	ptr = str;
	*ptr2++ = '\"';
	while (*ptr)
	{
		if ((unsigned char)*ptr > 31 && *ptr != '\"' && *ptr != '\\')
			*ptr2++ = *ptr++;
		else
		{
			*ptr2++ = '\\';
			switch (token = *ptr++)
			{
			case '\\':
				*ptr2++ = '\\';
				break;
			case '\"':
				*ptr2++ = '\"';
				break;
			case '\b':
				*ptr2++ = 'b';
				break;
			case '\f':
				*ptr2++ = 'f';
				break;
			case '\n':
				*ptr2++ = 'n';
				break;
			case '\r':
				*ptr2++ = 'r';
				break;
			case '\t':
				*ptr2++ = 't';
				break;
			default:
				sprintf(ptr2, "u%04x", token);
				ptr2 += 5;
				break; /* escape and print */
			}
		}
	}
	*ptr2++ = '\"';
	*ptr2++ = 0;
	return out;
}
/* Invote print_string_ptr (which is useful) on an item. */
static char* print_string(cJSON* item)
{
	return print_string_ptr(item->valuestring);
}
/* Render an array to text */
static char* print_array(cJSON* item, int depth, int fmt)
{
	char** entries;
	char* out = 0, * ptr, * ret;
	int len = 5;
	cJSON* child = item->child;
	int numentries = 0, i = 0, fail = 0;

	/* How many entries in the array? */
	while (child)
		numentries++, child = child->next;
	/* Allocate an array to hold the values for each */
	entries = (char**)cJSON_malloc(numentries * sizeof(char*));
	if (!entries)
		return 0;
	memset(entries, 0, numentries * sizeof(char*));
	/* Retrieve all the results: */
	child = item->child;
	while (child && !fail)
	{
		ret = print_value(child, depth + 1, fmt);
		entries[i++] = ret;
		if (ret)
			len += strlen(ret) + 2 + (fmt ? 1 : 0);
		else
			fail = 1;
		child = child->next;
	}

	/* If we didn't fail, try to malloc the output string */
	if (!fail)
		out = (char*)cJSON_malloc(len);
	/* If that fails, we fail. */
	if (!out)
		fail = 1;

	/* Handle failure. */
	if (fail)
	{
		for (i = 0; i < numentries; i++)
			if (entries[i])
				cJSON_free(entries[i]);
		cJSON_free(entries);
		return 0;
	}

	/* Compose the output array. */
	*out = '[';
	ptr = out + 1;
	*ptr = 0;
	for (i = 0; i < numentries; i++)
	{
		strcpy(ptr, entries[i]);
		ptr += strlen(entries[i]);
		if (i != numentries - 1)
		{
			*ptr++ = ',';
			if (fmt)
				*ptr++ = ' ';
			*ptr = 0;
		}
		cJSON_free(entries[i]);
	}
	cJSON_free(entries);
	*ptr++ = ']';
	*ptr++ = 0;
	return out;
}
/* Render an object to text. */
static char* print_object(cJSON* item, int depth, int fmt)
{
	char** entries = 0, ** names = 0;
	char* out = 0, * ptr, * ret, * str;
	int len = 7, i = 0, j;
	cJSON* child = item->child;
	int numentries = 0, fail = 0;
	/* Count the number of entries. */
	while (child)
		numentries++, child = child->next;
	/* Allocate space for the names and the objects */
	entries = (char**)cJSON_malloc(numentries * sizeof(char*));
	if (!entries)
		return 0;
	names = (char**)cJSON_malloc(numentries * sizeof(char*));
	if (!names)
	{
		cJSON_free(entries);
		return 0;
	}
	memset(entries, 0, sizeof(char*) * numentries);
	memset(names, 0, sizeof(char*) * numentries);

	/* Collect all the results into our arrays: */
	child = item->child;
	depth++;
	if (fmt)
		len += depth;
	while (child)
	{
		names[i] = str = print_string_ptr(child->string);
		entries[i++] = ret = print_value(child, depth, fmt);
		if (str && ret)
			len += strlen(ret) + strlen(str) + 2 + (fmt ? 2 + depth : 0);
		else
			fail = 1;
		child = child->next;
	}

	/* Try to allocate the output string */
	if (!fail)
		out = (char*)cJSON_malloc(len);
	if (!out)
		fail = 1;

	/* Handle failure */
	if (fail)
	{
		for (i = 0; i < numentries; i++)
		{
			if (names[i])
				cJSON_free(names[i]);
			if (entries[i])
				cJSON_free(entries[i]);
		}
		cJSON_free(names);
		cJSON_free(entries);
		return 0;
	}

	/* Compose the output: */
	*out = '{';
	ptr = out + 1;
	if (fmt)
		*ptr++ = '\n';
	*ptr = 0;
	for (i = 0; i < numentries; i++)
	{
		if (fmt)
			for (j = 0; j < depth; j++)
				*ptr++ = '\t';
		strcpy(ptr, names[i]);
		ptr += strlen(names[i]);
		*ptr++ = ':';
		if (fmt)
			*ptr++ = '\t';
		strcpy(ptr, entries[i]);
		ptr += strlen(entries[i]);
		if (i != numentries - 1)
			*ptr++ = ',';
		if (fmt)
			*ptr++ = '\n';
		*ptr = 0;
		cJSON_free(names[i]);
		cJSON_free(entries[i]);
	}

	cJSON_free(names);
	cJSON_free(entries);
	if (fmt)
		for (i = 0; i < depth - 1; i++)
			*ptr++ = '\t';
	*ptr++ = '}';
	*ptr++ = 0;
	return out;
}
/* Render a value to text. */
static char* print_value(cJSON* item, int depth, int fmt)
{
	char* out = 0;
	if (!item)
		return 0;
	switch ((item->type) & 255)
	{
	case cJSON_NULL:
		out = cJSON_strdup("null");
		break;
	case cJSON_False:
		out = cJSON_strdup("false");
		break;
	case cJSON_True:
		out = cJSON_strdup("true");
		break;
	case cJSON_Int:
		out = print_int(item);
		break;
	case cJSON_Double:
		out = print_double(item);
		break;
	case cJSON_String:
		out = print_string(item);
		break;
	case cJSON_Array:
		out = print_array(item, depth, fmt);
		break;
	case cJSON_Object:
		out = print_object(item, depth, fmt);
		break;
	}
	return out;
}


/* Utility for array list handling. */
static void suffix_object(cJSON* prev, cJSON* item)
{
	prev->next = item;
	item->prev = prev;
}
/* Utility for handling references. */
static cJSON* create_reference(cJSON* item)
{
	cJSON* ref = cJSON_New_Item();
	if (!ref)
		return 0;
	memcpy(ref, item, sizeof(cJSON));
	ref->string = 0;
	ref->type |= cJSON_IsReference;
	ref->next = ref->prev = 0;
	return ref;
}


/* Render a cJSON entity to text for transfer/storage. Free the char* when finished. */
static char* cJSON_Print(cJSON* item)
{
	return print_value(item, 0, 1);
}
/* Render a cJSON entity to text for transfer/storage without any formatting. Free the char* when finished. */
static char* cJSON_PrintUnformatted(cJSON* item)
{
	return print_value(item, 0, 0);
}
/* Delete a cJSON entity and all subentities. */
static void cJSON_Delete(cJSON* c)
{
	cJSON* next;
	while (c)
	{
		next = c->next;
		if (!(c->type & cJSON_IsReference) && c->child)
			cJSON_Delete(c->child);
		if (!(c->type & cJSON_IsReference) && c->valuestring)
			cJSON_free(c->valuestring);
		if (c->string)
			cJSON_free(c->string);
		cJSON_free(c);
		c = next;
	}
}
/* Supply malloc, realloc and free functions to cJSON */
static void cJSON_InitHooks(cJSON_Hooks* hooks)
{
	if (!hooks)
	{ /* Reset hooks */
		cJSON_malloc = malloc;
		cJSON_free = free;
		return;
	}

	cJSON_malloc = (hooks->malloc_fn) ? hooks->malloc_fn : malloc;
	cJSON_free = (hooks->free_fn) ? hooks->free_fn : free;
}
/* Supply a block of JSON, and this returns a cJSON object you can interrogate. Call cJSON_Delete when finished. */
static cJSON* cJSON_Parse(const char* value, const char** ep)
{
	cJSON* c = cJSON_New_Item();
	*ep = 0;
	if (!c)
		return 0; /* memory fail */

	if (!parse_value(c, skip(value), ep))
	{
		cJSON_Delete(c);
		return 0;
	}
	return c;
}
/* Returns the number of items in an array (or object). */
static int cJSON_GetArraySize(cJSON* array)
{
	cJSON* c = array->child;
	int i = 0;
	while (c)
		i++, c = c->next;
	return i;
}
/* Retrieve item number "item" from array "array". Returns NULL if unsuccessful. */
static cJSON* cJSON_GetArrayItem(cJSON* array, int item)
{
	cJSON* c = array->child;
	while (c && item > 0)
		item--, c = c->next;
	return c;
}
/* Get item "string" from object. Case insensitive. */
static cJSON* cJSON_GetObjectItem(cJSON* object, const char* string)
{
	cJSON* c = object->child;
	while (c && cJSON_strcasecmp(c->string, string))
		c = c->next;
	return c;
}

/* remove gloal variable for thread safe. --by Bwar on 2020-11-15 */
/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when cJSON_Parse() returns 0. 0 when cJSON_Parse() succeeds. */
/* extern const char *cJSON_GetErrorPtr(); */

/* These calls create a cJSON item of the appropriate type. */
static cJSON* cJSON_CreateNull()
{
	cJSON* item = cJSON_New_Item();
	if (item)
		item->type = cJSON_NULL;
	return item;
}
static cJSON* cJSON_CreateTrue()
{
	cJSON* item = cJSON_New_Item();
	if (item)
		item->type = cJSON_True;
	return item;
}
static cJSON* cJSON_CreateFalse()
{
	cJSON* item = cJSON_New_Item();
	if (item)
		item->type = cJSON_False;
	return item;
}
static cJSON* cJSON_CreateBool(int b)
{
	cJSON* item = cJSON_New_Item();
	if (item)
		item->type = b ? cJSON_True : cJSON_False;
	return item;
}
static cJSON* cJSON_CreateDouble(double num, int sign)
{
	cJSON* item = cJSON_New_Item();
	if (item)
	{
		item->type = cJSON_Double;
		item->valuedouble = num;
		item->valueint = (int64)num;
		item->sign = sign;
	}
	return item;
}
static cJSON* cJSON_CreateInt(uint64 num, int sign)
{
	cJSON* item = cJSON_New_Item();
	if (item)
	{
		item->type = cJSON_Int;
		item->valuedouble = (double)num;
		item->valueint = (int64)num;
		item->sign = sign;
	}
	return item;
}
static cJSON* cJSON_CreateString(const char* string)
{
	cJSON* item = cJSON_New_Item();
	if (item)
	{
		item->type = cJSON_String;
		item->valuestring = cJSON_strdup(string);
	}
	return item;
}
static cJSON* cJSON_CreateArray()
{
	cJSON* item = cJSON_New_Item();
	if (item)
		item->type = cJSON_Array;
	return item;
}
static cJSON* cJSON_CreateObject()
{
	cJSON* item = cJSON_New_Item();
	if (item)
		item->type = cJSON_Object;
	return item;
}


/* These utilities create an Array of count items. */
static cJSON* cJSON_CreateIntArray(int* numbers, int sign, int count)
{
	int i;
	cJSON* n = 0, * p = 0, * a = cJSON_CreateArray();
	for (i = 0; a && i < count; i++)
	{
		n = cJSON_CreateDouble((long double)((unsigned int)numbers[i]), sign);
		if (!i)
			a->child = n;
		else
			suffix_object(p, n);
		p = n;
	}
	return a;
}
static cJSON* cJSON_CreateFloatArray(float* numbers, int count)
{
	int i;
	cJSON* n = 0, * p = 0, * a = cJSON_CreateArray();
	for (i = 0; a && i < count; i++)
	{
		n = cJSON_CreateDouble((long double)numbers[i], -1);
		if (!i)
			a->child = n;
		else
			suffix_object(p, n);
		p = n;
	}
	return a;
}
static cJSON* cJSON_CreateDoubleArray(double* numbers, int count)
{
	int i;
	cJSON* n = 0, * p = 0, * a = cJSON_CreateArray();
	for (i = 0; a && i < count; i++)
	{
		n = cJSON_CreateDouble((long double)numbers[i], -1);
		if (!i)
			a->child = n;
		else
			suffix_object(p, n);
		p = n;
	}
	return a;
}
static cJSON* cJSON_CreateStringArray(const char** strings, int count)
{
	int i;
	cJSON* n = 0, * p = 0, * a = cJSON_CreateArray();
	for (i = 0; a && i < count; i++)
	{
		n = cJSON_CreateString(strings[i]);
		if (!i)
			a->child = n;
		else
			suffix_object(p, n);
		p = n;
	}
	return a;
}


/* Append item to the specified array/object. */
static void cJSON_AddItemToArray(cJSON* array, cJSON* item)
{
	cJSON* c = array->child;
	if (!item)
		return;
	if (!c)
	{
		array->child = item;
	}
	else
	{
		while (c && c->next)
			c = c->next;
		suffix_object(c, item);
	}
}
static void cJSON_AddItemToArrayHead(cJSON* array, cJSON* item)
{
	cJSON* c = array->child;
	if (!item)
		return;
	if (!c)
	{
		array->child = item;
	}
	else
	{
		item->prev = c->prev;
		item->next = c;
		c->prev = item;
		array->child = item;
	}
}
static void cJSON_AddItemToObject(cJSON* object, const char* string, cJSON* item)
{
	if (!item)
		return;
	if (item->string)
		cJSON_free(item->string);
	item->string = cJSON_strdup(string);
	cJSON_AddItemToArray(object, item);
}
/* Append reference to item to the specified array/object. Use this when you want to add an existing cJSON to a new cJSON, but don't want to corrupt your existing cJSON. */
static void cJSON_AddItemReferenceToArray(cJSON* array, cJSON* item)
{
	cJSON_AddItemToArray(array, create_reference(item));
}
static void cJSON_AddItemReferenceToObject(cJSON* object, const char* string, cJSON* item)
{
	cJSON_AddItemToObject(object, string, create_reference(item));
}


/* Remove/Detatch items from Arrays/Objects. */
static cJSON* cJSON_DetachItemFromArray(cJSON* array, int which)
{
	cJSON* c = array->child;
	while (c && which > 0)
		c = c->next, which--;
	if (!c)
		return 0;
	if (c->prev)
		c->prev->next = c->next;
	if (c->next)
		c->next->prev = c->prev;
	if (c == array->child)
		array->child = c->next;
	c->prev = c->next = 0;
	return c;
}
static void cJSON_DeleteItemFromArray(cJSON* array, int which)
{
	cJSON_Delete(cJSON_DetachItemFromArray(array, which));
}
static cJSON* cJSON_DetachItemFromObject(cJSON* object, const char* string)
{
	int i = 0;
	cJSON* c = object->child;
	while (c && cJSON_strcasecmp(c->string, string))
		i++, c = c->next;
	if (c)
		return cJSON_DetachItemFromArray(object, i);
	return 0;
}
static void cJSON_DeleteItemFromObject(cJSON* object, const char* string)
{
	cJSON_Delete(cJSON_DetachItemFromObject(object, string));
}


/* Update array items. */
static void cJSON_ReplaceItemInArray(cJSON* array, int which, cJSON* newitem)
{
	cJSON* c = array->child;
	while (c && which > 0)
		c = c->next, which--;
	if (!c)
		return;
	newitem->next = c->next;
	newitem->prev = c->prev;
	if (newitem->next)
		newitem->next->prev = newitem;
	if (c == array->child)
		array->child = newitem;
	else
		newitem->prev->next = newitem;
	c->next = c->prev = 0;
	cJSON_Delete(c);
}
static void cJSON_ReplaceItemInObject(cJSON* object, const char* string, cJSON* newitem)
{
	int i = 0;
	cJSON* c = object->child;
	while (c && cJSON_strcasecmp(c->string, string))
		i++, c = c->next;
	if (c)
	{
		newitem->string = cJSON_strdup(string);
		cJSON_ReplaceItemInArray(object, i, newitem);
	}
}


#define cJSON_AddNullToObject(object,name)	cJSON_AddItemToObject(object, name, cJSON_CreateNull())
#define cJSON_AddTrueToObject(object,name)	cJSON_AddItemToObject(object, name, cJSON_CreateTrue())
#define cJSON_AddFalseToObject(object,name)		cJSON_AddItemToObject(object, name, cJSON_CreateFalse())
#define cJSON_AddNumberToObject(object,name,n)	cJSON_AddItemToObject(object, name, cJSON_CreateNumber(n))
#define cJSON_AddStringToObject(object,name,s)	cJSON_AddItemToObject(object, name, cJSON_CreateString(s))

namespace neb
{
	class CJsonObject
	{
	public:     // method of ordinary json object or json array
		CJsonObject() : m_pJsonData(NULL), m_pExternJsonDataRef(NULL), m_pKeyTravers(NULL)
		{
			// m_pJsonData = cJSON_CreateObject();  
			m_array_iter = m_mapJsonArrayRef.end();
			m_object_iter = m_mapJsonObjectRef.end();
		}
		CJsonObject(const std::string& strJson) : m_pJsonData(NULL), m_pExternJsonDataRef(NULL), m_pKeyTravers(NULL)
		{
			m_array_iter = m_mapJsonArrayRef.end();
			m_object_iter = m_mapJsonObjectRef.end();
			Parse(strJson);
		}
		CJsonObject(const CJsonObject* pJsonObject) : m_pJsonData(NULL), m_pExternJsonDataRef(NULL), m_pKeyTravers(NULL)
		{
			if (pJsonObject)
			{
				m_array_iter = m_mapJsonArrayRef.end();
				m_object_iter = m_mapJsonObjectRef.end();
				Parse(pJsonObject->ToString());
			}
		}
		CJsonObject(const CJsonObject& oJsonObject) : m_pJsonData(NULL), m_pExternJsonDataRef(NULL), m_pKeyTravers(NULL)
		{
			m_array_iter = m_mapJsonArrayRef.end();
			m_object_iter = m_mapJsonObjectRef.end();
			Parse(oJsonObject.ToString());
		}
#if __cplusplus >= 201101L
		CJsonObject(CJsonObject&& oJsonObject)
			: m_pJsonData(oJsonObject.m_pJsonData),
			m_pExternJsonDataRef(oJsonObject.m_pExternJsonDataRef),
			m_pKeyTravers(oJsonObject.m_pKeyTravers),
			mc_pError(oJsonObject.mc_pError)
		{
			oJsonObject.m_pJsonData = NULL;
			oJsonObject.m_pExternJsonDataRef = NULL;
			oJsonObject.m_pKeyTravers = NULL;
			oJsonObject.mc_pError = NULL;
			m_strErrMsg = std::move(oJsonObject.m_strErrMsg);
			m_mapJsonArrayRef = std::move(oJsonObject.m_mapJsonArrayRef);
			m_mapJsonObjectRef = std::move(oJsonObject.m_mapJsonObjectRef);
			m_array_iter = m_mapJsonArrayRef.end();
			m_object_iter = m_mapJsonObjectRef.end();
		}
#endif

		virtual ~CJsonObject() { Clear(); }


		CJsonObject& operator=(const CJsonObject& oJsonObject)
		{
			Parse(oJsonObject.ToString().c_str());
			return(*this);
		}
#if __cplusplus >= 201101L
		CJsonObject& operator=(CJsonObject&& oJsonObject)
		{
			Clear();

			m_pJsonData = oJsonObject.m_pJsonData;
			oJsonObject.m_pJsonData = NULL;
			m_pExternJsonDataRef = oJsonObject.m_pExternJsonDataRef;
			oJsonObject.m_pExternJsonDataRef = NULL;
			m_pKeyTravers = oJsonObject.m_pKeyTravers;
			oJsonObject.m_pKeyTravers = NULL;
			mc_pError = oJsonObject.mc_pError;
			oJsonObject.mc_pError = NULL;
			m_strErrMsg = std::move(oJsonObject.m_strErrMsg);
			m_mapJsonArrayRef = std::move(oJsonObject.m_mapJsonArrayRef);
			m_mapJsonObjectRef = std::move(oJsonObject.m_mapJsonObjectRef);
			m_array_iter = m_mapJsonArrayRef.end();
			m_object_iter = m_mapJsonObjectRef.end();
			return(*this);
		}
#endif
		bool operator==(const CJsonObject& oJsonObject) const
		{
			return(this->ToString() == oJsonObject.ToString());
		}


		bool Parse(const std::string& strJson)
		{
			Clear();
			m_pJsonData = cJSON_Parse(strJson.c_str(), &mc_pError);
			m_pKeyTravers = m_pJsonData;
			if (m_pJsonData == NULL)
			{
				m_strErrMsg = std::string("prase json string error at ") + mc_pError;
				return(false);
			}
			return(true);
		}
		void Clear()
		{
			m_pExternJsonDataRef = NULL;
			m_pKeyTravers = NULL;
			if (m_pJsonData != NULL)
			{
				cJSON_Delete(m_pJsonData);
				m_pJsonData = NULL;
			}
#if __cplusplus < 201101L
			for (std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.begin();
				iter != m_mapJsonArrayRef.end(); ++iter)
#else
			for (auto iter = m_mapJsonArrayRef.begin(); iter != m_mapJsonArrayRef.end(); ++iter)
#endif
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
			}
			m_mapJsonArrayRef.clear();
			m_array_iter = m_mapJsonArrayRef.end();
#if __cplusplus < 201101L
			for (std::map<std::string, CJsonObject*>::iterator iter = m_mapJsonObjectRef.begin();
				iter != m_mapJsonObjectRef.end(); ++iter)
#else
			for (auto iter = m_mapJsonObjectRef.begin(); iter != m_mapJsonObjectRef.end(); ++iter)
#endif
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
			}
			m_mapJsonObjectRef.clear();
			m_object_iter = m_mapJsonObjectRef.end();
		}
		bool IsEmpty() const
		{
			if (m_pJsonData != NULL)
			{
				return(false);
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				return(false);
			}
			return(true);
		}
		bool IsArray() const
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}

			if (pFocusData == NULL)
			{
				return(false);
			}

			if (pFocusData->type == cJSON_Array)
			{
				return(true);
			}
			else
			{
				return(false);
			}
		}
		std::string ToString() const
		{
			char* pJsonString = NULL;
			std::string strJsonData = "";
			if (m_pJsonData != NULL)
			{
				pJsonString = cJSON_PrintUnformatted(m_pJsonData);
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pJsonString = cJSON_PrintUnformatted(m_pExternJsonDataRef);
			}
			if (pJsonString != NULL)
			{
				strJsonData = pJsonString;
				free(pJsonString);
			}
			return(strJsonData);
		}
		std::string ToFormattedString() const
		{
			char* pJsonString = NULL;
			std::string strJsonData = "";
			if (m_pJsonData != NULL)
			{
				pJsonString = cJSON_Print(m_pJsonData);
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pJsonString = cJSON_Print(m_pExternJsonDataRef);
			}
			if (pJsonString != NULL)
			{
				strJsonData = pJsonString;
				free(pJsonString);
			}
			return(strJsonData);
		}
		const std::string& GetErrMsg() const { return(m_strErrMsg); }



	public:     // method of ordinary json object
		bool AddEmptySubObject(const std::string& strKey)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateObject();
				m_pKeyTravers = m_pJsonData;
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
			{
				m_strErrMsg = "key exists!";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateObject();
			if (pJsonStruct == NULL)
			{
				m_strErrMsg = std::string("create sub empty object error!");
				return(false);
			}
			cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
			m_pKeyTravers = pFocusData;
			return(true);
		}
		bool AddEmptySubArray(const std::string& strKey)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateObject();
				m_pKeyTravers = m_pJsonData;
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
			{
				m_strErrMsg = "key exists!";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateArray();
			if (pJsonStruct == NULL)
			{
				m_strErrMsg = std::string("create sub empty array error!");
				return(false);
			}
			cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
			m_pKeyTravers = pFocusData;
			return(true);
		}
		bool GetKey(std::string& strKey)
		{
			if (IsArray())
			{
				return(false);
			}
			if (m_pKeyTravers == NULL)
			{
				if (m_pJsonData != NULL)
				{
					m_pKeyTravers = m_pJsonData;
				}
				else if (m_pExternJsonDataRef != NULL)
				{
					m_pKeyTravers = m_pExternJsonDataRef;
				}
				return(false);
			}
			else if (m_pKeyTravers == m_pJsonData || m_pKeyTravers == m_pExternJsonDataRef)
			{
				cJSON* c = m_pKeyTravers->child;
				if (c)
				{
					strKey = c->string;
					m_pKeyTravers = c->next;
					return(true);
				}
				else
				{
					return(false);
				}
			}
			else
			{
				strKey = m_pKeyTravers->string;
				m_pKeyTravers = m_pKeyTravers->next;
				return(true);
			}
		}
		void ResetTraversing()
		{
			if (m_pJsonData != NULL)
			{
				m_pKeyTravers = m_pJsonData;
			}
			else
			{
				m_pKeyTravers = m_pExternJsonDataRef;
			}
		}


		CJsonObject& operator[](const std::string& strKey)
		{
#if __cplusplus < 201101L
			std::map<std::string, CJsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
#else
			auto iter = m_mapJsonObjectRef.find(strKey);
#endif
			if (iter == m_mapJsonObjectRef.end())
			{
				cJSON* pJsonStruct = NULL;
				if (m_pJsonData != NULL)
				{
					if (m_pJsonData->type == cJSON_Object)
					{
						pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
					}
				}
				else if (m_pExternJsonDataRef != NULL)
				{
					if (m_pExternJsonDataRef->type == cJSON_Object)
					{
						pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
					}
				}
				if (pJsonStruct == NULL)
				{
					CJsonObject* pJsonObject = new CJsonObject();
					m_mapJsonObjectRef.insert(std::pair<std::string, CJsonObject*>(strKey, pJsonObject));
					return(*pJsonObject);
				}
				else
				{
					CJsonObject* pJsonObject = new CJsonObject(pJsonStruct);
					m_mapJsonObjectRef.insert(std::pair<std::string, CJsonObject*>(strKey, pJsonObject));
					return(*pJsonObject);
				}
			}
			else
			{
				return(*(iter->second));
			}
		}
		std::string operator()(const std::string& strKey) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
				}
			}
			if (pJsonStruct == NULL)
			{
				return(std::string(""));
			}
			if (pJsonStruct->type == cJSON_String)
			{
				return(pJsonStruct->valuestring);
			}
			else if (pJsonStruct->type == cJSON_Int)
			{
				char szNumber[128] = { 0 };
				if (pJsonStruct->sign == -1)
				{
					if (pJsonStruct->valueint <= (int64)INT_MAX && (int64)pJsonStruct->valueint >= (int64)INT_MIN)
					{
						snprintf(szNumber, sizeof(szNumber), "%d", (int32)pJsonStruct->valueint);
					}
					else
					{
#if LLONG_MAX==LONG_MAX
						snprintf(szNumber, sizeof(szNumber), "%ld", (int64)pJsonStruct->valueint);
#else
						snprintf(szNumber, sizeof(szNumber), "%lld", (int64)pJsonStruct->valueint);
#endif
					}
				}
				else
				{
					if ((uint64)pJsonStruct->valueint <= (uint64)UINT_MAX)
					{
						snprintf(szNumber, sizeof(szNumber), "%u", (uint32)pJsonStruct->valueint);
					}
					else
					{
#if LLONG_MAX==LONG_MAX
						snprintf(szNumber, sizeof(szNumber), "%lu", pJsonStruct->valueint);
#else
						snprintf(szNumber, sizeof(szNumber), "%llu", pJsonStruct->valueint);
#endif
					}
				}
				return(std::string(szNumber));
			}
			else if (pJsonStruct->type == cJSON_Double)
			{
				char szNumber[128] = { 0 };
				if (fabs(pJsonStruct->valuedouble) < 1.0e-6 || fabs(pJsonStruct->valuedouble) > 1.0e9)
				{
					snprintf(szNumber, sizeof(szNumber), "%e", pJsonStruct->valuedouble);
				}
				else
				{
					snprintf(szNumber, sizeof(szNumber), "%f", pJsonStruct->valuedouble);
				}
				return(std::string(szNumber));
			}
			else if (pJsonStruct->type == cJSON_False)
			{
				return(std::string("false"));
			}
			else if (pJsonStruct->type == cJSON_True)
			{
				return(std::string("true"));
			}
			return(std::string(""));
		}


		bool KeyExist(const std::string& strKey) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			return(true);
		}
		bool Get(const std::string& strKey, CJsonObject& oJsonObject) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			char* pJsonString = cJSON_Print(pJsonStruct);
			std::string strJsonData = pJsonString;
			free(pJsonString);
			if (oJsonObject.Parse(strJsonData))
			{
				return(true);
			}
			else
			{
				return(false);
			}
		}
		bool Get(const std::string& strKey, std::string& strValue) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			if (pJsonStruct->type != cJSON_String)
			{
				return(false);
			}
			strValue = pJsonStruct->valuestring;
			return(true);
		}
		bool Get(const std::string& strKey, int32& iValue) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			if (pJsonStruct->type == cJSON_Int)
			{
				iValue = (int32)(pJsonStruct->valueint);
				return(true);
			}
			else if (pJsonStruct->type == cJSON_Double)
			{
				iValue = (int32)(pJsonStruct->valuedouble);
				return(true);
			}
			return(false);
		}
		bool Get(const std::string& strKey, uint32& uiValue) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			if (pJsonStruct->type == cJSON_Int)
			{
				uiValue = (uint32)(pJsonStruct->valueint);
				return(true);
			}
			else if (pJsonStruct->type == cJSON_Double)
			{
				uiValue = (uint32)(pJsonStruct->valuedouble);
				return(true);
			}
			return(false);
		}
		bool Get(const std::string& strKey, int64& llValue) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			if (pJsonStruct->type == cJSON_Int)
			{
				llValue = (int64)(pJsonStruct->valueint);
				return(true);
			}
			else if (pJsonStruct->type == cJSON_Double)
			{
				llValue = (int64)(pJsonStruct->valuedouble);
				return(true);
			}
			return(false);
		}
		bool Get(const std::string& strKey, uint64& ullValue) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			if (pJsonStruct->type == cJSON_Int)
			{
				ullValue = (uint64)(pJsonStruct->valueint);
				return(true);
			}
			else if (pJsonStruct->type == cJSON_Double)
			{
				ullValue = (uint64)(pJsonStruct->valuedouble);
				return(true);
			}
			return(false);
		}
		bool Get(const std::string& strKey, bool& bValue) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			if (pJsonStruct->type > cJSON_True)
			{
				return(false);
			}
			bValue = pJsonStruct->type;
			return(true);
		}
		bool Get(const std::string& strKey, float& fValue) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			if (pJsonStruct->type == cJSON_Double || pJsonStruct->type == cJSON_Int)
			{
				fValue = (float)(pJsonStruct->valuedouble);
				return(true);
			}
			return(false);
		}
		bool Get(const std::string& strKey, double& dValue) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			if (pJsonStruct->type == cJSON_Double || pJsonStruct->type == cJSON_Int)
			{
				dValue = pJsonStruct->valuedouble;
				return(true);
			}
			return(false);
		}
		int GetValueType(const std::string& strKey) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			return(pJsonStruct->type);
		}
		bool IsNull(const std::string& strKey) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pJsonData, strKey.c_str());
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Object)
				{
					pJsonStruct = cJSON_GetObjectItem(m_pExternJsonDataRef, strKey.c_str());
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			if (pJsonStruct->type != cJSON_NULL)
			{
				return(false);
			}
			return(true);
		}
		bool Add(const std::string& strKey, const CJsonObject& oJsonObject)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateObject();
				m_pKeyTravers = m_pJsonData;
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
			{
				m_strErrMsg = "key exists!";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_Parse(oJsonObject.ToString().c_str(), &mc_pError);
			if (pJsonStruct == NULL)
			{
				m_strErrMsg = std::string("prase json string error at ") + mc_pError;
				return(false);
			}
			cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<std::string, CJsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
#else
			auto iter = m_mapJsonObjectRef.find(strKey);
#endif
			if (iter != m_mapJsonObjectRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonObjectRef.erase(iter);
			}
			m_pKeyTravers = pFocusData;
			return(true);
		}
#if __cplusplus < 201101L
		bool AddWithMove(const std::string& strKey, CJsonObject& oJsonObject)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateObject();
				m_pKeyTravers = m_pJsonData;
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
			{
				m_strErrMsg = "key exists!";
				return(false);
			}
			cJSON* pJsonStruct = oJsonObject.m_pJsonData;
			oJsonObject.m_pJsonData = NULL;
			if (pJsonStruct == NULL)
			{
				m_strErrMsg = "can not move a non-independent(internal) CJsonObject from one to another.";
				return(false);
			}
			cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			std::map<std::string, CJsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
			if (iter != m_mapJsonObjectRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonObjectRef.erase(iter);
			}
			m_pKeyTravers = pFocusData;
			return(true);
		}
#else
		bool Add(const std::string& strKey, CJsonObject&& oJsonObject)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateObject();
				m_pKeyTravers = m_pJsonData;
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
			{
				m_strErrMsg = "key exists!";
				return(false);
			}
			cJSON* pJsonStruct = oJsonObject.m_pJsonData;
			oJsonObject.m_pJsonData = NULL;
			if (pJsonStruct == NULL)
			{
				m_strErrMsg = "can not move a non-independent(internal) CJsonObject from one to another.";
				return(false);
			}
			cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			auto iter = m_mapJsonObjectRef.find(strKey);
			if (iter != m_mapJsonObjectRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonObjectRef.erase(iter);
			}
			m_pKeyTravers = pFocusData;
			return(true);
		}
#endif
		bool Add(const std::string& strKey, const std::string& strValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateObject();
				m_pKeyTravers = m_pJsonData;
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
			{
				m_strErrMsg = "key exists!";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateString(strValue.c_str());
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			m_pKeyTravers = pFocusData;
			return(true);
		}
		bool Add(const std::string& strKey, int32 iValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateObject();
				m_pKeyTravers = m_pJsonData;
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
			{
				m_strErrMsg = "key exists!";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateInt((uint64)iValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			m_pKeyTravers = pFocusData;
			return(true);
		}
		bool Add(const std::string& strKey, uint32 uiValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateObject();
				m_pKeyTravers = m_pJsonData;
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
			{
				m_strErrMsg = "key exists!";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateInt((uint64)uiValue, 1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			m_pKeyTravers = pFocusData;
			return(true);
		}
		bool Add(const std::string& strKey, int64 llValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateObject();
				m_pKeyTravers = m_pJsonData;
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
			{
				m_strErrMsg = "key exists!";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateInt((uint64)llValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			m_pKeyTravers = pFocusData;
			return(true);
		}
		bool Add(const std::string& strKey, uint64 ullValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateObject();
				m_pKeyTravers = m_pJsonData;
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
			{
				m_strErrMsg = "key exists!";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateInt(ullValue, 1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			m_pKeyTravers = pFocusData;
			return(true);
		}
		bool Add(const std::string& strKey, bool bValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateObject();
				m_pKeyTravers = m_pJsonData;
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
			{
				m_strErrMsg = "key exists!";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateBool(bValue);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			m_pKeyTravers = pFocusData;
			return(true);
		}
		bool Add(const std::string& strKey, float fValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateObject();
				m_pKeyTravers = m_pJsonData;
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
			{
				m_strErrMsg = "key exists!";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateDouble((double)fValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			m_pKeyTravers = pFocusData;
			return(true);
		}
		bool Add(const std::string& strKey, double dValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateObject();
				m_pKeyTravers = m_pJsonData;
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
			{
				m_strErrMsg = "key exists!";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateDouble((double)dValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			m_pKeyTravers = pFocusData;
			return(true);
		}
		bool AddNull(const std::string& strKey)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateObject();
				m_pKeyTravers = m_pJsonData;
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) != NULL)
			{
				m_strErrMsg = "key exists!";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateNull();
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			cJSON_AddItemToObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			m_pKeyTravers = pFocusData;
			return(true);
		}
		bool Delete(const std::string& strKey)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			cJSON_DeleteItemFromObject(pFocusData, strKey.c_str());
#if __cplusplus < 201101L
			std::map<std::string, CJsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
#else
			auto iter = m_mapJsonObjectRef.find(strKey);
#endif
			if (iter != m_mapJsonObjectRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonObjectRef.erase(iter);
			}
			m_pKeyTravers = pFocusData;
			return(true);
		}
		bool Replace(const std::string& strKey, const CJsonObject& oJsonObject)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_Parse(oJsonObject.ToString().c_str(), &mc_pError);
			if (pJsonStruct == NULL)
			{
				m_strErrMsg = std::string("prase json string error at ") + mc_pError;
				return(false);
			}
			cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<std::string, CJsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
#else
			auto iter = m_mapJsonObjectRef.find(strKey);
#endif
			if (iter != m_mapJsonObjectRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonObjectRef.erase(iter);
			}
			return(true);
		}
#if __cplusplus < 201101L
		bool ReplaceWithMove(const std::string& strKey, CJsonObject& oJsonObject)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			cJSON* pJsonStruct = oJsonObject.m_pJsonData;
			oJsonObject.m_pJsonData = NULL;
			if (pJsonStruct == NULL)
			{
				m_strErrMsg = "can not move a non-independent(internal) CJsonObject from one to another.";
				return(false);
			}
			cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			std::map<std::string, CJsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
			if (iter != m_mapJsonObjectRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonObjectRef.erase(iter);
			}
			return(true);
		}
#else
		bool Replace(const std::string& strKey, CJsonObject&& oJsonObject)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			cJSON* pJsonStruct = oJsonObject.m_pJsonData;
			oJsonObject.m_pJsonData = NULL;
			if (pJsonStruct == NULL)
			{
				m_strErrMsg = "can not move a non-independent(internal) CJsonObject from one to another.";
				return(false);
			}
			cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			auto iter = m_mapJsonObjectRef.find(strKey);
			if (iter != m_mapJsonObjectRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonObjectRef.erase(iter);
			}
			return(true);
		}
#endif
		bool Replace(const std::string& strKey, const std::string& strValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateString(strValue.c_str());
			if (pJsonStruct == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<std::string, CJsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
#else
			auto iter = m_mapJsonObjectRef.find(strKey);
#endif
			if (iter != m_mapJsonObjectRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonObjectRef.erase(iter);
			}
			cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			return(true);
		}
		bool Replace(const std::string& strKey, int32 iValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateInt((uint64)iValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<std::string, CJsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
#else
			auto iter = m_mapJsonObjectRef.find(strKey);
#endif
			if (iter != m_mapJsonObjectRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonObjectRef.erase(iter);
			}
			cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			return(true);
		}
		bool Replace(const std::string& strKey, uint32 uiValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateInt((uint64)uiValue, 1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<std::string, CJsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
#else
			auto iter = m_mapJsonObjectRef.find(strKey);
#endif
			if (iter != m_mapJsonObjectRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonObjectRef.erase(iter);
			}
			cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			return(true);
		}
		bool Replace(const std::string& strKey, int64 llValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateInt((uint64)llValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<std::string, CJsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
#else
			auto iter = m_mapJsonObjectRef.find(strKey);
#endif
			if (iter != m_mapJsonObjectRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonObjectRef.erase(iter);
			}
			cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			return(true);
		}
		bool Replace(const std::string& strKey, uint64 ullValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateInt((uint64)ullValue, 1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<std::string, CJsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
#else
			auto iter = m_mapJsonObjectRef.find(strKey);
#endif
			if (iter != m_mapJsonObjectRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonObjectRef.erase(iter);
			}
			cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			return(true);
		}
		bool Replace(const std::string& strKey, bool bValue, bool bValueAgain)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateBool(bValue);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<std::string, CJsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
#else
			auto iter = m_mapJsonObjectRef.find(strKey);
#endif
			if (iter != m_mapJsonObjectRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonObjectRef.erase(iter);
			}
			cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			return(true);
		}
		bool Replace(const std::string& strKey, float fValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateDouble((double)fValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<std::string, CJsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
#else
			auto iter = m_mapJsonObjectRef.find(strKey);
#endif
			if (iter != m_mapJsonObjectRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonObjectRef.erase(iter);
			}
			cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			return(true);
		}
		bool Replace(const std::string& strKey, double dValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateDouble((double)dValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<std::string, CJsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
#else
			auto iter = m_mapJsonObjectRef.find(strKey);
#endif
			if (iter != m_mapJsonObjectRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonObjectRef.erase(iter);
			}
			cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			return(true);
		}
		bool ReplaceWithNull(const std::string& strKey)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Object)
			{
				m_strErrMsg = "not a json object! json array?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateNull();
			if (pJsonStruct == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<std::string, CJsonObject*>::iterator iter = m_mapJsonObjectRef.find(strKey);
#else
			auto iter = m_mapJsonObjectRef.find(strKey);
#endif
			if (iter != m_mapJsonObjectRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonObjectRef.erase(iter);
			}
			cJSON_ReplaceItemInObject(pFocusData, strKey.c_str(), pJsonStruct);
			if (cJSON_GetObjectItem(pFocusData, strKey.c_str()) == NULL)
			{
				return(false);
			}
			return(true);
		}
#if __cplusplus < 201101L
		bool ReplaceAdd(const std::string& strKey, const CJsonObject& oJsonObject)
		{
			if (KeyExist(strKey))
			{
				return(Replace(strKey, oJsonObject));
			}
			return(Add(strKey, oJsonObject));
		}
		bool ReplaceAdd(const std::string& strKey, const std::string& strValue)
		{
			if (KeyExist(strKey))
			{
				return(Replace(strKey, strValue));
			}
			return(Add(strKey, strValue));
		}
		template <typename T>
		bool ReplaceAdd(const std::string& strKey, T value)
		{
			if (KeyExist(strKey))
			{
				return(Replace(strKey, value));
			}
			return(Add(strKey, value));
		}
#else
		template <typename T>
		bool ReplaceAdd(const std::string& strKey, T&& value)
		{
			if (KeyExist(strKey))
			{
				return(Replace(strKey, std::forward<T>(value)));
			}
			return(Add(strKey, std::forward<T>(value)));
		}
#endif



	public:     // method of json array
		CJsonObject& operator[](unsigned int uiWhich)
		{
#if __cplusplus < 201101L
			std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.find(uiWhich);
#else
			auto iter = m_mapJsonArrayRef.find(uiWhich);
#endif
			if (iter == m_mapJsonArrayRef.end())
			{
				cJSON* pJsonStruct = NULL;
				if (m_pJsonData != NULL)
				{
					if (m_pJsonData->type == cJSON_Array)
					{
						pJsonStruct = cJSON_GetArrayItem(m_pJsonData, uiWhich);
					}
				}
				else if (m_pExternJsonDataRef != NULL)
				{
					if (m_pExternJsonDataRef->type == cJSON_Array)
					{
						pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, uiWhich);
					}
				}
				if (pJsonStruct == NULL)
				{
					CJsonObject* pJsonObject = new CJsonObject();
					m_mapJsonArrayRef.insert(std::pair<unsigned int, CJsonObject*>(uiWhich, pJsonObject));
					return(*pJsonObject);
				}
				else
				{
					CJsonObject* pJsonObject = new CJsonObject(pJsonStruct);
					m_mapJsonArrayRef.insert(std::pair<unsigned int, CJsonObject*>(uiWhich, pJsonObject));
					return(*pJsonObject);
				}
			}
			else
			{
				return(*(iter->second));
			}
		}
		std::string operator()(unsigned int uiWhich) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pJsonData, uiWhich);
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, uiWhich);
				}
			}
			if (pJsonStruct == NULL)
			{
				return(std::string(""));
			}
			if (pJsonStruct->type == cJSON_String)
			{
				return(pJsonStruct->valuestring);
			}
			else if (pJsonStruct->type == cJSON_Int)
			{
				char szNumber[128] = { 0 };
				if (pJsonStruct->sign == -1)
				{
					if (pJsonStruct->valueint <= (int64)INT_MAX && (int64)pJsonStruct->valueint >= (int64)INT_MIN)
					{
						snprintf(szNumber, sizeof(szNumber), "%d", (int32)pJsonStruct->valueint);
					}
					else
					{
#if LLONG_MAX==LONG_MAX
						snprintf(szNumber, sizeof(szNumber), "%ld", (int64)pJsonStruct->valueint);
#else
						snprintf(szNumber, sizeof(szNumber), "%lld", (int64)pJsonStruct->valueint);
#endif
					}
				}
				else
				{
					if ((uint64)pJsonStruct->valueint <= (uint64)UINT_MAX)
					{
						snprintf(szNumber, sizeof(szNumber), "%u", (uint32)pJsonStruct->valueint);
					}
					else
					{
#if LLONG_MAX==LONG_MAX
						snprintf(szNumber, sizeof(szNumber), "%lu", pJsonStruct->valueint);
#else
						snprintf(szNumber, sizeof(szNumber), "%llu", pJsonStruct->valueint);
#endif
					}
				}
				return(std::string(szNumber));
			}
			else if (pJsonStruct->type == cJSON_Double)
			{
				char szNumber[128] = { 0 };
				if (fabs(pJsonStruct->valuedouble) < 1.0e-6 || fabs(pJsonStruct->valuedouble) > 1.0e9)
				{
					snprintf(szNumber, sizeof(szNumber), "%e", pJsonStruct->valuedouble);
				}
				else
				{
					snprintf(szNumber, sizeof(szNumber), "%f", pJsonStruct->valuedouble);
				}
				return(std::string(szNumber));
			}
			else if (pJsonStruct->type == cJSON_False)
			{
				return(std::string("false"));
			}
			else if (pJsonStruct->type == cJSON_True)
			{
				return(std::string("true"));
			}
			return(std::string(""));
		}


		int GetArraySize() const
		{
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Array)
				{
					return(cJSON_GetArraySize(m_pJsonData));
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Array)
				{
					return(cJSON_GetArraySize(m_pExternJsonDataRef));
				}
			}
			return(0);
		}
		bool Get(int iWhich, CJsonObject& oJsonObject) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			char* pJsonString = cJSON_Print(pJsonStruct);
			std::string strJsonData = pJsonString;
			free(pJsonString);
			if (oJsonObject.Parse(strJsonData))
			{
				return(true);
			}
			else
			{
				return(false);
			}
		}
		bool Get(int iWhich, std::string& strValue) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			if (pJsonStruct->type != cJSON_String)
			{
				return(false);
			}
			strValue = pJsonStruct->valuestring;
			return(true);
		}
		bool Get(int iWhich, int32& iValue) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			if (pJsonStruct->type == cJSON_Int)
			{
				iValue = (int32)(pJsonStruct->valueint);
				return(true);
			}
			else if (pJsonStruct->type == cJSON_Double)
			{
				iValue = (int32)(pJsonStruct->valuedouble);
				return(true);
			}
			return(false);
		}
		bool Get(int iWhich, uint32& uiValue) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			if (pJsonStruct->type == cJSON_Int)
			{
				uiValue = (uint32)(pJsonStruct->valueint);
				return(true);
			}
			else if (pJsonStruct->type == cJSON_Double)
			{
				uiValue = (uint32)(pJsonStruct->valuedouble);
				return(true);
			}
			return(false);
		}
		bool Get(int iWhich, int64& llValue) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			if (pJsonStruct->type == cJSON_Int)
			{
				llValue = (int64)(pJsonStruct->valueint);
				return(true);
			}
			else if (pJsonStruct->type == cJSON_Double)
			{
				llValue = (int64)(pJsonStruct->valuedouble);
				return(true);
			}
			return(false);
		}
		bool Get(int iWhich, uint64& ullValue) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			if (pJsonStruct->type == cJSON_Int)
			{
				ullValue = (uint64)(pJsonStruct->valueint);
				return(true);
			}
			else if (pJsonStruct->type == cJSON_Double)
			{
				ullValue = (uint64)(pJsonStruct->valuedouble);
				return(true);
			}
			return(false);
		}
		bool Get(int iWhich, bool& bValue) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			if (pJsonStruct->type > cJSON_True)
			{
				return(false);
			}
			bValue = pJsonStruct->type;
			return(true);
		}
		bool Get(int iWhich, float& fValue) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			if (pJsonStruct->type == cJSON_Double || pJsonStruct->type == cJSON_Int)
			{
				fValue = (float)(pJsonStruct->valuedouble);
				return(true);
			}
			return(false);
		}
		bool Get(int iWhich, double& dValue) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			if (pJsonStruct->type == cJSON_Double || pJsonStruct->type == cJSON_Int)
			{
				dValue = pJsonStruct->valuedouble;
				return(true);
			}
			return(false);
		}
		int GetValueType(int iWhich) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			return(pJsonStruct->type);
		}
		bool IsNull(int iWhich) const
		{
			cJSON* pJsonStruct = NULL;
			if (m_pJsonData != NULL)
			{
				if (m_pJsonData->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pJsonData, iWhich);
				}
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				if (m_pExternJsonDataRef->type == cJSON_Array)
				{
					pJsonStruct = cJSON_GetArrayItem(m_pExternJsonDataRef, iWhich);
				}
			}
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			if (pJsonStruct->type != cJSON_NULL)
			{
				return(false);
			}
			return(true);
		}
		bool Add(const CJsonObject& oJsonObject)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_Parse(oJsonObject.ToString().c_str(), &mc_pError);
			if (pJsonStruct == NULL)
			{
				m_strErrMsg = std::string("prase json string error at ") + mc_pError;
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArray(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			unsigned int uiLastIndex = (unsigned int)cJSON_GetArraySize(pFocusData) - 1;
#if __cplusplus < 201101L
			for (std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.begin();
				iter != m_mapJsonArrayRef.end(); )
#else
			for (auto iter = m_mapJsonArrayRef.begin(); iter != m_mapJsonArrayRef.end(); )
#endif
			{
				if (iter->first >= uiLastIndex)
				{
					if (iter->second != NULL)
					{
						delete (iter->second);
						iter->second = NULL;
					}
					m_mapJsonArrayRef.erase(iter++);
				}
				else
				{
					iter++;
				}
			}
			return(true);
		}
#if __cplusplus < 201101L
		bool AddWithMove(CJsonObject& oJsonObject)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = oJsonObject.m_pJsonData;
			oJsonObject.m_pJsonData = NULL;
			if (pJsonStruct == NULL)
			{
				m_strErrMsg = "can not move a non-independent(internal) CJsonObject from one to another.";
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArray(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			unsigned int uiLastIndex = (unsigned int)cJSON_GetArraySize(pFocusData) - 1;
			for (std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.begin(); iter != m_mapJsonArrayRef.end(); )
			{
				if (iter->first >= uiLastIndex)
				{
					if (iter->second != NULL)
					{
						delete (iter->second);
						iter->second = NULL;
					}
					m_mapJsonArrayRef.erase(iter++);
				}
				else
				{
					iter++;
				}
			}
			return(true);
		}
#else
		bool Add(CJsonObject&& oJsonObject)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = oJsonObject.m_pJsonData;
			oJsonObject.m_pJsonData = NULL;
			if (pJsonStruct == NULL)
			{
				m_strErrMsg = "can not move a non-independent(internal) CJsonObject from one to another.";
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArray(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			unsigned int uiLastIndex = (unsigned int)cJSON_GetArraySize(pFocusData) - 1;
			for (auto iter = m_mapJsonArrayRef.begin(); iter != m_mapJsonArrayRef.end(); )
			{
				if (iter->first >= uiLastIndex)
				{
					if (iter->second != NULL)
					{
						delete (iter->second);
						iter->second = NULL;
					}
					m_mapJsonArrayRef.erase(iter++);
				}
				else
				{
					iter++;
				}
			}
			return(true);
		}
#endif
		bool Add(const std::string& strValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateString(strValue.c_str());
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArray(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			return(true);
		}
		bool Add(int32 iValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateInt((uint64)iValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArray(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			return(true);
		}
		bool Add(uint32 uiValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateInt((uint64)uiValue, 1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArray(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			return(true);
		}
		bool Add(int64 llValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateInt((uint64)llValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArray(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			return(true);
		}
		bool Add(uint64 ullValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateInt((uint64)ullValue, 1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArray(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			return(true);
		}
		bool Add(bool bValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateBool(bValue);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArray(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			return(true);
		}
		bool Add(float fValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateDouble((double)fValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArray(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			return(true);
		}
		bool Add(double dValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateDouble((double)dValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArray(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			return(true);
		}
		bool AddNull()
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateNull();
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArray(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			return(true);
		}
		bool AddAsFirst(const CJsonObject& oJsonObject)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_Parse(oJsonObject.ToString().c_str(), &mc_pError);
			if (pJsonStruct == NULL)
			{
				m_strErrMsg = std::string("prase json string error at ") + mc_pError;
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
#if __cplusplus < 201101L
			for (std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.begin();
				iter != m_mapJsonArrayRef.end(); )
#else
			for (auto iter = m_mapJsonArrayRef.begin(); iter != m_mapJsonArrayRef.end(); )
#endif
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonArrayRef.erase(iter++);
			}
			return(true);
		}
#if __cplusplus < 201101L
		bool AddAsFirstWithMove(CJsonObject& oJsonObject)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = oJsonObject.m_pJsonData;
			oJsonObject.m_pJsonData = NULL;
			if (pJsonStruct == NULL)
			{
				m_strErrMsg = "can not move a non-independent(internal) CJsonObject from one to another.";
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			for (std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.begin(); iter != m_mapJsonArrayRef.end(); )
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonArrayRef.erase(iter++);
			}
			return(true);
		}
#else
		bool AddAsFirst(CJsonObject&& oJsonObject)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = oJsonObject.m_pJsonData;
			oJsonObject.m_pJsonData = NULL;
			if (pJsonStruct == NULL)
			{
				m_strErrMsg = "can not move a non-independent(internal) CJsonObject from one to another.";
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			for (auto iter = m_mapJsonArrayRef.begin(); iter != m_mapJsonArrayRef.end(); )
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonArrayRef.erase(iter++);
			}
			return(true);
		}
#endif
		bool AddAsFirst(const std::string& strValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateString(strValue.c_str());
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			return(true);
		}
		bool AddAsFirst(int32 iValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateInt((uint64)iValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			return(true);
		}
		bool AddAsFirst(uint32 uiValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateInt((uint64)uiValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			return(true);
		}
		bool AddAsFirst(int64 llValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateInt((uint64)llValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			return(true);
		}
		bool AddAsFirst(uint64 ullValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateInt((uint64)ullValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			return(true);
		}
		bool AddAsFirst(int iAnywhere, bool bValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateBool(bValue);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			return(true);
		}
		bool AddAsFirst(float fValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateDouble((double)fValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			return(true);
		}
		bool AddAsFirst(double dValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateDouble((double)dValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			return(true);
		}
		bool AddNullAsFirst()
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData != NULL)
			{
				pFocusData = m_pJsonData;
			}
			else if (m_pExternJsonDataRef != NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				m_pJsonData = cJSON_CreateArray();
				pFocusData = m_pJsonData;
			}

			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateNull();
			if (pJsonStruct == NULL)
			{
				return(false);
			}
			int iArraySizeBeforeAdd = cJSON_GetArraySize(pFocusData);
			cJSON_AddItemToArrayHead(pFocusData, pJsonStruct);
			int iArraySizeAfterAdd = cJSON_GetArraySize(pFocusData);
			if (iArraySizeAfterAdd == iArraySizeBeforeAdd)
			{
				return(false);
			}
			return(true);
		}
		bool Delete(int iWhich)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON_DeleteItemFromArray(pFocusData, iWhich);
#if __cplusplus < 201101L
			for (std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.begin();
				iter != m_mapJsonArrayRef.end(); )
#else
			for (auto iter = m_mapJsonArrayRef.begin(); iter != m_mapJsonArrayRef.end(); )
#endif
			{
				if (iter->first >= (unsigned int)iWhich)
				{
					if (iter->second != NULL)
					{
						delete (iter->second);
						iter->second = NULL;
					}
					m_mapJsonArrayRef.erase(iter++);
				}
				else
				{
					iter++;
				}
			}
			return(true);
		}
		bool Replace(int iWhich, const CJsonObject& oJsonObject)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_Parse(oJsonObject.ToString().c_str(), &mc_pError);
			if (pJsonStruct == NULL)
			{
				m_strErrMsg = std::string("prase json string error at ") + mc_pError;
				return(false);
			}
			cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
			if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
#else
			auto iter = m_mapJsonArrayRef.find(iWhich);
#endif
			if (iter != m_mapJsonArrayRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonArrayRef.erase(iter);
			}
			return(true);
		}
#if __cplusplus < 201101L
		bool ReplaceWithMove(int iWhich, CJsonObject& oJsonObject)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = oJsonObject.m_pJsonData;
			oJsonObject.m_pJsonData = NULL;
			if (pJsonStruct == NULL)
			{
				m_strErrMsg = "can not move a non-independent(internal) CJsonObject from one to another.";
				return(false);
			}
			cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
			if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
			{
				return(false);
			}
			std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
			if (iter != m_mapJsonArrayRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonArrayRef.erase(iter);
			}
			return(true);
		}
#else
		bool Replace(int iWhich, CJsonObject&& oJsonObject)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = oJsonObject.m_pJsonData;
			oJsonObject.m_pJsonData = NULL;
			if (pJsonStruct == NULL)
			{
				m_strErrMsg = "can not move a non-independent(internal) CJsonObject from one to another.";
				return(false);
			}
			cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
			if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
			{
				return(false);
			}
			auto iter = m_mapJsonArrayRef.find(iWhich);
			if (iter != m_mapJsonArrayRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonArrayRef.erase(iter);
			}
			return(true);
		}
#endif
		bool Replace(int iWhich, const std::string& strValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateString(strValue.c_str());
			if (pJsonStruct == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
#else
			auto iter = m_mapJsonArrayRef.find(iWhich);
#endif
			if (iter != m_mapJsonArrayRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonArrayRef.erase(iter);
			}
			cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
			if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
			{
				return(false);
			}
			return(true);
		}
		bool Replace(int iWhich, int32 iValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateInt((uint64)iValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
#else
			auto iter = m_mapJsonArrayRef.find(iWhich);
#endif
			if (iter != m_mapJsonArrayRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonArrayRef.erase(iter);
			}
			cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
			if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
			{
				return(false);
			}
			return(true);
		}
		bool Replace(int iWhich, uint32 uiValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateInt((uint64)uiValue, 1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
#else
			auto iter = m_mapJsonArrayRef.find(iWhich);
#endif
			if (iter != m_mapJsonArrayRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonArrayRef.erase(iter);
			}
			cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
			if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
			{
				return(false);
			}
			return(true);
		}
		bool Replace(int iWhich, int64 llValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateInt((uint64)((uint64)llValue), -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
#else
			auto iter = m_mapJsonArrayRef.find(iWhich);
#endif
			if (iter != m_mapJsonArrayRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonArrayRef.erase(iter);
			}
			cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
			if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
			{
				return(false);
			}
			return(true);
		}
		bool Replace(int iWhich, uint64 ullValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateInt((uint64)ullValue, 1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
#else
			auto iter = m_mapJsonArrayRef.find(iWhich);
#endif
			if (iter != m_mapJsonArrayRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonArrayRef.erase(iter);
			}
			cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
			if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
			{
				return(false);
			}
			return(true);
		}
		bool Replace(int iWhich, bool bValue, bool bValueAgain)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateBool(bValue);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
#else
			auto iter = m_mapJsonArrayRef.find(iWhich);
#endif
			if (iter != m_mapJsonArrayRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonArrayRef.erase(iter);
			}
			cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
			if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
			{
				return(false);
			}
			return(true);
		}
		bool Replace(int iWhich, float fValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateDouble((double)fValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
#else
			auto iter = m_mapJsonArrayRef.find(iWhich);
#endif
			if (iter != m_mapJsonArrayRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonArrayRef.erase(iter);
			}
			cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
			if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
			{
				return(false);
			}
			return(true);
		}
		bool Replace(int iWhich, double dValue)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateDouble((double)dValue, -1);
			if (pJsonStruct == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
#else
			auto iter = m_mapJsonArrayRef.find(iWhich);
#endif
			if (iter != m_mapJsonArrayRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonArrayRef.erase(iter);
			}
			cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
			if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
			{
				return(false);
			}
			return(true);
		}
		bool ReplaceWithNull(int iWhich)
		{
			cJSON* pFocusData = NULL;
			if (m_pJsonData == NULL)
			{
				pFocusData = m_pExternJsonDataRef;
			}
			else
			{
				pFocusData = m_pJsonData;
			}
			if (pFocusData == NULL)
			{
				m_strErrMsg = "json data is null!";
				return(false);
			}
			if (pFocusData->type != cJSON_Array)
			{
				m_strErrMsg = "not a json array! json object?";
				return(false);
			}
			cJSON* pJsonStruct = cJSON_CreateNull();
			if (pJsonStruct == NULL)
			{
				return(false);
			}
#if __cplusplus < 201101L
			std::map<unsigned int, CJsonObject*>::iterator iter = m_mapJsonArrayRef.find(iWhich);
#else
			auto iter = m_mapJsonArrayRef.find(iWhich);
#endif
			if (iter != m_mapJsonArrayRef.end())
			{
				if (iter->second != NULL)
				{
					delete (iter->second);
					iter->second = NULL;
				}
				m_mapJsonArrayRef.erase(iter);
			}
			cJSON_ReplaceItemInArray(pFocusData, iWhich, pJsonStruct);
			if (cJSON_GetArrayItem(pFocusData, iWhich) == NULL)
			{
				return(false);
			}
			return(true);
		}



	private:
		CJsonObject(cJSON* pJsonData) : m_pJsonData(NULL), m_pExternJsonDataRef(pJsonData), m_pKeyTravers(pJsonData) {}



	private:
		cJSON* m_pJsonData;
		cJSON* m_pExternJsonDataRef;
		cJSON* m_pKeyTravers;
		const char* mc_pError;
		std::string m_strErrMsg;
#if __cplusplus < 201101L
		std::map<unsigned int, CJsonObject*> m_mapJsonArrayRef;
		std::map<unsigned int, CJsonObject*>::iterator m_array_iter;
		std::map<std::string, CJsonObject*> m_mapJsonObjectRef;
		std::map<std::string, CJsonObject*>::iterator m_object_iter;
#else
		std::unordered_map<unsigned int, CJsonObject*> m_mapJsonArrayRef;
		std::unordered_map<std::string, CJsonObject*>::iterator m_object_iter;
		std::unordered_map<std::string, CJsonObject*> m_mapJsonObjectRef;
		std::unordered_map<unsigned int, CJsonObject*>::iterator m_array_iter;
#endif
	};

}

#endif // CJSONHELPER_H_