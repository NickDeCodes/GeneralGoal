/*************************************************************************
	> File Name: json.h
	> Author: 
	> Mail: 
	> Created Time: 五 10/ 2 19:48:30 2020
 ************************************************************************/

#ifndef _JSON_H
#define _JSON_H

#ifdef __cplusplus
extern "C"
{
#endif

/* project version */
#define JSON_VERSION_MAJOR 1
#define JSON_VERSION_MINOR 7
#define JSON_VERSION_PATCH 4

#include <stddef.h>

/* JSON Types: */
#define JSON_Invalid (0)
#define JSON_False  (1 << 0)
#define JSON_True   (1 << 1)
#define JSON_NULL   (1 << 2)
#define JSON_Number (1 << 3)
#define JSON_String (1 << 4)
#define JSON_Array  (1 << 5)
#define JSON_Object (1 << 6)
#define JSON_Raw    (1 << 7) /* raw json */

#define JSON_IsReference 256
#define JSON_StringIsConst 512

/* The cJSON structure: */
typedef struct JSON {
    /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
    struct JSON *next;
    struct JSON *prev;
    /* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */
    struct JSON *child;

    /* The type of the item, as above. */
    int type;

    /* The item's string, if type==cJSON_String  and type == cJSON_Raw */
    char *valuestring;
    /* writing to valueint is DEPRECATED, use cJSON_SetNumberValue instead */
    int valueint;
    /* The item's number, if type==cJSON_Number */
    double valuedouble;

    /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
    char *string;
} JSON;

typedef struct JSON_Hooks {
      void *(*malloc_fn)(size_t sz);
      void (*free_fn)(void *ptr);
} JSON_Hooks;

typedef int JSON_bool;

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif
#ifdef __WINDOWS__

/* When compiling for windows, we specify a specific calling convention to avoid issues where we are being called from a project with a different default calling convention.  For windows you have 2 define options:

JSON_HIDE_SYMBOLS - Define this in the case where you don't want to ever dllexport symbols
JSON_EXPORT_SYMBOLS - Define this on library build when you want to dllexport symbols (default)
JSON_IMPORT_SYMBOLS - Define this if you want to dllimport symbol

For *nix builds that support visibility attribute, you can define similar behavior by

setting default visibility to hidden by adding
-fvisibility=hidden (for gcc)
or
-xldscope=hidden (for sun cc)
to CFLAGS

then using the CJSON_API_VISIBILITY flag to "export" the same symbols the way CJSON_EXPORT_SYMBOLS does

*/

/* export symbols by default, this is necessary for copy pasting the C and header file */
#if !defined(JSON_HIDE_SYMBOLS) && !defined(JSON_IMPORT_SYMBOLS) && !defined(JSON_EXPORT_SYMBOLS)
#define JSON_EXPORT_SYMBOLS
#endif

#if defined(JSON_HIDE_SYMBOLS)
#define JSON_PUBLIC(type)   type __stdcall
#elif defined(JSON_EXPORT_SYMBOLS)
#define JSON_PUBLIC(type)   __declspec(dllexport) type __stdcall
#elif defined(JSON_IMPORT_SYMBOLS)
#define JSON_PUBLIC(type)   __declspec(dllimport) type __stdcall
#endif
#else /* !WIN32 */
#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined (__SUNPRO_C)) && defined(CJSON_API_VISIBILITY)
#define JSON_PUBLIC(type)   __attribute__((visibility("default"))) type
#else
#define JSON_PUBLIC(type) type
#endif
#endif

/* Limits how deeply nested arrays/objects can be before cJSON rejects to parse them.
 * This is to prevent stack overflows. */
#ifndef JSON_NESTING_LIMIT
#define JSON_NESTING_LIMIT 1000
#endif

/* returns the version of cJSON as a string */
JSON_PUBLIC(const char*) JSON_Version(void);

/* Supply malloc, realloc and free functions to cJSON */
JSON_PUBLIC(void) JSON_InitHooks(JSON_Hooks* hooks);

/* Memory Management: the caller is always responsible to free the results from all variants of cJSON_Parse (with cJSON_Delete) and cJSON_Print (with stdlib free, cJSON_Hooks.free_fn, or cJSON_free as appropriate). The exception is cJSON_PrintPreallocated, where the caller has full responsibility of the buffer. */
/* Supply a block of JSON, and this returns a cJSON object you can interrogate. */
JSON_PUBLIC(JSON *) JSON_Parse(const char *value);
/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed. */
/* If you supply a ptr in return_parse_end and parsing fails, then return_parse_end will contain a pointer to the error so will match cJSON_GetErrorPtr(). */
JSON_PUBLIC(JSON *) JSON_ParseWithOpts(const char *value, const char **return_parse_end, JSON_bool require_null_terminated);

/* Render a cJSON entity to text for transfer/storage. */
JSON_PUBLIC(char *) JSON_Print(const JSON *item);
/* Render a cJSON entity to text for transfer/storage without any formatting. */
JSON_PUBLIC(char *) JSON_PrintUnformatted(const JSON *item);
/* Render a cJSON entity to text using a buffered strategy. prebuffer is a guess at the final size. guessing well reduces reallocation. fmt=0 gives unformatted, =1 gives formatted */
JSON_PUBLIC(char *) JSON_PrintBuffered(const JSON *item, int prebuffer, JSON_bool fmt);
/* Render a cJSON entity to text using a buffer already allocated in memory with given length. Returns 1 on success and 0 on failure. */
/* NOTE: cJSON is not always 100% accurate in estimating how much memory it will use, so to be safe allocate 5 bytes more than you actually need */
JSON_PUBLIC(JSON_bool) JSON_PrintPreallocated(JSON *item, char *buffer, const int length, const JSON_bool format);
/* Delete a cJSON entity and all subentities. */
JSON_PUBLIC(void) JSON_Delete(JSON *c);

/* Returns the number of items in an array (or object). */
JSON_PUBLIC(int) JSON_GetArraySize(const JSON *array);
/* Retrieve item number "item" from array "array". Returns NULL if unsuccessful. */
JSON_PUBLIC(JSON *) JSON_GetArrayItem(const JSON *array, int index);
/* Get item "string" from object. Case insensitive. */
JSON_PUBLIC(JSON *) JSON_GetObjectItem(const JSON * const object, const char * const string);
JSON_PUBLIC(JSON *) JSON_GetObjectItemCaseSensitive(const JSON * const object, const char * const string);
JSON_PUBLIC(JSON_bool) JSON_HasObjectItem(const JSON *object, const char *string);
/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when cJSON_Parse() returns 0. 0 when cJSON_Parse() succeeds. */
JSON_PUBLIC(const char *) JSON_GetErrorPtr(void);

/* Check if the item is a string and return its valuestring */
JSON_PUBLIC(char *) JSON_GetStringValue(JSON *item);

/* These functions check the type of an item */
JSON_PUBLIC(JSON_bool) JSON_IsInvalid(const JSON * const item);
JSON_PUBLIC(JSON_bool) JSON_IsFalse(const JSON * const item);
JSON_PUBLIC(JSON_bool) JSON_IsTrue(const JSON * const item);
JSON_PUBLIC(JSON_bool) JSON_IsBool(const JSON * const item);
JSON_PUBLIC(JSON_bool) JSON_IsNull(const JSON * const item);
JSON_PUBLIC(JSON_bool) JSON_IsNumber(const JSON * const item);
JSON_PUBLIC(JSON_bool) JSON_IsString(const JSON * const item);
JSON_PUBLIC(JSON_bool) JSON_IsArray(const JSON * const item);
JSON_PUBLIC(JSON_bool) JSON_IsObject(const JSON * const item);
JSON_PUBLIC(JSON_bool) JSON_IsRaw(const JSON * const item);

/* These calls create a cJSON item of the appropriate type. */
JSON_PUBLIC(JSON *) JSON_CreateNull(void);
JSON_PUBLIC(JSON *) JSON_CreateTrue(void);
JSON_PUBLIC(JSON *) JSON_CreateFalse(void);
JSON_PUBLIC(JSON *) JSON_CreateBool(JSON_bool boolean);
JSON_PUBLIC(JSON *) JSON_CreateNumber(double num);
JSON_PUBLIC(JSON *) JSON_CreateString(const char *string);
/* raw json */
JSON_PUBLIC(JSON *) JSON_CreateRaw(const char *raw);
JSON_PUBLIC(JSON *) JSON_CreateArray(void);
JSON_PUBLIC(JSON *) JSON_CreateObject(void);

/* Create a string where valuestring references a string so
 * it will not be freed by cJSON_Delete */
JSON_PUBLIC(JSON *) JSON_CreateStringReference(const char *string);
/* Create an object/arrray that only references it's elements so
 * they will not be freed by cJSON_Delete */
JSON_PUBLIC(JSON *) JSON_CreateObjectReference(const JSON *child);
JSON_PUBLIC(JSON *) JSON_CreateArrayReference(const JSON *child);

/* These utilities create an Array of count items. */
JSON_PUBLIC(JSON *) JSON_CreateIntArray(const int *numbers, int count);
JSON_PUBLIC(JSON *) JSON_CreateFloatArray(const float *numbers, int count);
JSON_PUBLIC(JSON *) JSON_CreateDoubleArray(const double *numbers, int count);
JSON_PUBLIC(JSON *) JSON_CreateStringArray(const char **strings, int count);

/* Append item to the specified array/object. */
JSON_PUBLIC(void) JSON_AddItemToArray(JSON *array, JSON *item);
JSON_PUBLIC(void) JSON_AddItemToObject(JSON *object, const char *string, JSON *item);
/* Use this when string is definitely const (i.e. a literal, or as good as), and will definitely survive the cJSON object.
 * WARNING: When this function was used, make sure to always check that (item->type & cJSON_StringIsConst) is zero before
 * writing to `item->string` */
JSON_PUBLIC(void) JSON_AddItemToObjectCS(JSON *object, const char *string, JSON *item);
/* Append reference to item to the specified array/object. Use this when you want to add an existing cJSON to a new cJSON, but don't want to corrupt your existing cJSON. */
JSON_PUBLIC(void) JSON_AddItemReferenceToArray(JSON *array, JSON *item);
JSON_PUBLIC(void) JSON_AddItemReferenceToObject(JSON *object, const char *string, JSON *item);

/* Remove/Detatch items from Arrays/Objects. */
JSON_PUBLIC(JSON *) JSON_DetachItemViaPointer(JSON *parent, JSON * const item);
JSON_PUBLIC(JSON *) JSON_DetachItemFromArray(JSON *array, int which);
JSON_PUBLIC(void) JSON_DeleteItemFromArray(JSON *array, int which);
JSON_PUBLIC(JSON *) JSON_DetachItemFromObject(JSON *object, const char *string);
JSON_PUBLIC(JSON *) JSON_DetachItemFromObjectCaseSensitive(JSON *object, const char *string);
JSON_PUBLIC(void) JSON_DeleteItemFromObject(JSON *object, const char *string);
JSON_PUBLIC(void) JSON_DeleteItemFromObjectCaseSensitive(JSON *object, const char *string);

/* Update array items. */
JSON_PUBLIC(void) JSON_InsertItemInArray(JSON *array, int which, JSON *newitem); /* Shifts pre-existing items to the right. */
JSON_PUBLIC(JSON_bool) JSON_ReplaceItemViaPointer(JSON *const parent, JSON *const item, JSON *replacement);
JSON_PUBLIC(void) JSON_ReplaceItemInArray(JSON *array, int which, JSON *newitem);
JSON_PUBLIC(void) JSON_ReplaceItemInObject(JSON *object,const char *string, JSON *newitem);
JSON_PUBLIC(void) JSON_ReplaceItemInObjectCaseSensitive(JSON *object,const char *string, JSON *newitem);

/* Duplicate a cJSON item */
JSON_PUBLIC(JSON *) JSON_Duplicate(const JSON *item, JSON_bool recurse);
/* Duplicate will create a new, identical cJSON item to the one you pass, in new memory that will
need to be released. With recurse!=0, it will duplicate any children connected to the item.
The item->next and ->prev pointers are always zero on return from Duplicate. */
/* Recursively compare two cJSON items for equality. If either a or b is NULL or invalid, they will be considered unequal.
 * case_sensitive determines if object keys are treated case sensitive (1) or case insensitive (0) */
JSON_PUBLIC(JSON_bool) JSON_Compare(const JSON * const a, const JSON * const b, const JSON_bool case_sensitive);


JSON_PUBLIC(void) JSON_Minify(char *json);

/* Helper functions for creating and adding items to an object at the same time.
 * They return the added item or NULL on failure. */
JSON_PUBLIC(JSON*) JSON_AddNullToObject(JSON * const object, const char * const name);
JSON_PUBLIC(JSON*) JSON_AddTrueToObject(JSON * const object, const char * const name);
JSON_PUBLIC(JSON*) JSON_AddFalseToObject(JSON * const object, const char * const name);
JSON_PUBLIC(JSON*) JSON_AddBoolToObject(JSON * const object, const char * const name, const JSON_bool boolean);
JSON_PUBLIC(JSON*) JSON_AddNumberToObject(JSON * const object, const char * const name, const double number);
JSON_PUBLIC(JSON*) JSON_AddStringToObject(JSON * const object, const char * const name, const char * const string);
JSON_PUBLIC(JSON*) JSON_AddRawToObject(JSON * const object, const char * const name, const char * const raw);
JSON_PUBLIC(JSON*) JSON_AddObjectToObject(JSON * const object, const char * const name);
JSON_PUBLIC(JSON*) JSON_AddArrayToObject(JSON * const object, const char * const name);

/* When assigning an integer value, it needs to be propagated to valuedouble too. */
#define cJSON_SetIntValue(object, number) ((object) ? (object)->valueint = (object)->valuedouble = (number) : (number))
/* helper for the cJSON_SetNumberValue macro */
JSON_PUBLIC(double) JSON_SetNumberHelper(JSON *object, double number);
#define cJSON_SetNumberValue(object, number) ((object != NULL) ? JSON_SetNumberHelper(object, (double)number) : (number))

/* Macro for iterating over an array or object */
#define cJSON_ArrayForEach(element, array) for(element = (array != NULL) ? (array)->child : NULL; element != NULL; element = element->next)

/* malloc/free objects using the malloc/free functions that have been set with cJSON_InitHooks */
JSON_PUBLIC(void *) JSON_malloc(size_t size);
JSON_PUBLIC(void) JSON_free(void *object);

#ifdef __cplusplus
}
#endif

#endif
