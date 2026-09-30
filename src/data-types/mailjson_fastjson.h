/*
 * libEtPan! -- a mail stuff library
 *
 * Internal mapping from the json-c calls in mailjson.c to libfastjson.
 * libfastjson's own compatibility macros are incomplete, so this file
 * owns the subset the adapter uses. Not installed.
 */

#ifndef MAILJSON_FASTJSON_H

#define MAILJSON_FASTJSON_H

#define FJSON_NATIVE_API_ONLY 1

#include <json.h>

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define json_object fjson_object
#define json_tokener fjson_tokener
#define json_tokener_error fjson_tokener_error
#define json_type fjson_type
#define json_type_null fjson_type_null
#define json_type_boolean fjson_type_boolean
#define json_type_double fjson_type_double
#define json_type_int fjson_type_int
#define json_type_object fjson_type_object
#define json_type_array fjson_type_array
#define json_type_string fjson_type_string

#define json_tokener_success fjson_tokener_success
#define JSON_TOKENER_STRICT FJSON_TOKENER_STRICT
/* libfastjson has no UTF-8 validation flag. mailjson_parse checks. */
#define JSON_TOKENER_VALIDATE_UTF8 0

#define JSON_C_TO_STRING_PLAIN FJSON_TO_STRING_PLAIN
#define JSON_C_TO_STRING_SPACED FJSON_TO_STRING_SPACED
/*
 * libfastjson always writes '/' as '\/' and ignores this bit.
 * mailjson_serialize turns those escapes back into '/'.
 */
#define JSON_C_TO_STRING_NOSLASHESCAPE (1 << 4)

#define json_object_get fjson_object_get
#define json_object_put fjson_object_put
#define json_object_is_type fjson_object_is_type
#define json_object_get_type fjson_object_get_type
#define json_object_new_object fjson_object_new_object
#define json_object_new_array fjson_object_new_array
#define json_object_new_string fjson_object_new_string
#define json_object_new_int64 fjson_object_new_int64
#define json_object_new_boolean fjson_object_new_boolean
#define json_object_object_length fjson_object_object_length
#define json_object_object_get_ex fjson_object_object_get_ex
#define json_object_array_add fjson_object_array_add
#define json_object_get_string fjson_object_get_string
#define json_object_get_int64 fjson_object_get_int64
#define json_object_get_boolean fjson_object_get_boolean
#define json_object_get_double fjson_object_get_double
#define json_object_to_json_string_ext fjson_object_to_json_string_ext
#define json_tokener_new fjson_tokener_new
#define json_tokener_free fjson_tokener_free
#define json_tokener_set_flags fjson_tokener_set_flags
#define json_tokener_parse_ex fjson_tokener_parse_ex
#define json_tokener_get_error fjson_tokener_get_error

/*
 * libfastjson documents char_offset as the consumed-byte count and does
 * not provide a getter. With the terminating NUL mailjson_parse appends,
 * that count matches json_tokener_get_parse_end().
 */
static inline size_t json_tokener_get_parse_end(struct fjson_tokener * tokener)
{
  if (tokener == NULL)
    return 0;
  if (tokener->char_offset < 0)
    return 0;
  return (size_t) tokener->char_offset;
}

/*
 * fjson_object_array_length and fjson_object_array_get_idx take and return
 * int. json-c 0.18 uses size_t for both, and mailjson.c shares that shape.
 */
static inline size_t json_object_array_length(struct fjson_object * object)
{
  int count;

  count = fjson_object_array_length(object);
  if (count < 0)
    return 0;
  return (size_t) count;
}

static inline struct fjson_object * json_object_array_get_idx(
    struct fjson_object * object, size_t index)
{
  if (index > (size_t) INT_MAX)
    return NULL;
  return fjson_object_array_get_idx(object, (int) index);
}

/*
 * fjson_object_object_add is void and does not report allocation failure.
 * A new key that does not increase the length was not stored, and the
 * value is still owned by the caller. Replacing an existing key keeps
 * the length and does take ownership.
 */
static inline int json_object_object_add(struct fjson_object * object,
    const char * key, struct fjson_object * value)
{
  int before;
  fjson_bool existed;

  if ((object == NULL) || (key == NULL))
    return -1;
  existed = fjson_object_object_get_ex(object, key, NULL);
  before = fjson_object_object_length(object);
  fjson_object_object_add(object, key, value);
  if (!existed && (fjson_object_object_length(object) == before))
    return -1;
  return 0;
}

static struct fjson_object * fastjson_copy_node(struct fjson_object * source);

static struct fjson_object * fastjson_copy_double(struct fjson_object * source)
{
  const char * printed;
  char * printed_copy;
  struct fjson_object * copy;

  printed = fjson_object_to_json_string_ext(source, FJSON_TO_STRING_PLAIN);
  if (printed == NULL)
    return NULL;
  printed_copy = strdup(printed);
  if (printed_copy == NULL)
    return NULL;
  copy = fjson_object_new_double_s(fjson_object_get_double(source),
      printed_copy);
  free(printed_copy);
  return copy;
}

static struct fjson_object * fastjson_copy_array(struct fjson_object * source)
{
  struct fjson_object * copy;
  int count;
  int index;

  copy = fjson_object_new_array();
  if (copy == NULL)
    return NULL;
  count = fjson_object_array_length(source);
  for (index = 0; index < count; index ++) {
    struct fjson_object * child;
    struct fjson_object * child_copy;

    child = fjson_object_array_get_idx(source, index);
    child_copy = fastjson_copy_node(child);
    if ((child != NULL) && (child_copy == NULL)) {
      fjson_object_put(copy);
      return NULL;
    }
    if (fjson_object_array_add(copy, child_copy) < 0) {
      fjson_object_put(child_copy);
      fjson_object_put(copy);
      return NULL;
    }
  }
  return copy;
}

static struct fjson_object * fastjson_copy_object(struct fjson_object * source)
{
  struct fjson_object * copy;
  struct fjson_object_iterator iter;
  struct fjson_object_iterator end;

  copy = fjson_object_new_object();
  if (copy == NULL)
    return NULL;
  iter = fjson_object_iter_begin(source);
  end = fjson_object_iter_end(source);
  while (!fjson_object_iter_equal(&iter, &end)) {
    const char * key;
    struct fjson_object * child;
    struct fjson_object * child_copy;
    int before;

    key = fjson_object_iter_peek_name(&iter);
    child = fjson_object_iter_peek_value(&iter);
    child_copy = fastjson_copy_node(child);
    if ((child != NULL) && (child_copy == NULL)) {
      fjson_object_put(copy);
      return NULL;
    }
    before = fjson_object_object_length(copy);
    fjson_object_object_add(copy, key, child_copy);
    if (fjson_object_object_length(copy) == before) {
      fjson_object_put(child_copy);
      fjson_object_put(copy);
      return NULL;
    }
    fjson_object_iter_next(&iter);
  }
  return copy;
}

static struct fjson_object * fastjson_copy_node(struct fjson_object * source)
{
  if (source == NULL)
    return NULL;
  switch (fjson_object_get_type(source)) {
  case fjson_type_null:
    return NULL;
  case fjson_type_boolean:
    return fjson_object_new_boolean(fjson_object_get_boolean(source));
  case fjson_type_double:
    return fastjson_copy_double(source);
  case fjson_type_int:
    return fjson_object_new_int64(fjson_object_get_int64(source));
  case fjson_type_string: {
    const char * text;

    text = fjson_object_get_string(source);
    if (text == NULL)
      return NULL;
    return fjson_object_new_string(text);
  }
  case fjson_type_array:
    return fastjson_copy_array(source);
  case fjson_type_object:
    return fastjson_copy_object(source);
  default:
    return NULL;
  }
}

static inline int json_object_deep_copy(struct fjson_object * source,
    struct fjson_object ** destination, void * shallow_copy)
{
  (void) shallow_copy;
  if (destination == NULL)
    return -1;
  * destination = fastjson_copy_node(source);
  if ((source != NULL) && (* destination == NULL) &&
      !fjson_object_is_type(source, fjson_type_null))
    return -1;
  return 0;
}

/*
 * Same shape as json-c's json_object_object_foreach. Key pointers stay
 * valid after the loop because they belong to the object, not the iterator.
 */
#define json_object_object_foreach(obj, key, val) \
  char * key = NULL; \
  struct fjson_object * val = NULL; \
  for (struct fjson_object_iterator key##_iter = \
           fjson_object_iter_begin(obj), \
           key##_end = fjson_object_iter_end(obj); \
       !fjson_object_iter_equal(&key##_iter, &key##_end) && \
           ((key = (char *) fjson_object_iter_peek_name(&key##_iter)), \
            (val = fjson_object_iter_peek_value(&key##_iter)), 1); \
       fjson_object_iter_next(&key##_iter))

#endif
