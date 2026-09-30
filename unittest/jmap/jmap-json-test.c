/*
 * libEtPan! -- a mail stuff library
 */

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "../../src/data-types/mailjson.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int check(int condition, const char * message)
{
  if (!condition) {
    fprintf(stderr, "%s\n", message);
    return 0;
  }

  return 1;
}

static int str_equal(const char * left, const char * right)
{
  return (left != NULL) && (right != NULL) && (strcmp(left, right) == 0);
}

static int expect_text(const char * actual, const char * expected,
    const char * message)
{
  if (str_equal(actual, expected))
    return 1;
  fprintf(stderr, "%s\n", message);
  fprintf(stderr, "  expected: %s\n", expected);
  fprintf(stderr, "  actual:   %s\n", (actual != NULL) ? actual : "(null)");
  return 0;
}

static int expect_parse_error(const char * data, size_t len,
    const char * message)
{
  mailjson_value * sentinel;
  mailjson_value * value;
  int r;

  sentinel = (mailjson_value *) 1;
  value = sentinel;
  r = mailjson_parse(data, len, &value);
  if (value != sentinel)
    mailjson_free(value);
  return check(r == MAILJSON_ERROR_PARSE, message) &&
      check(value == NULL, "parse error did not clear result");
}

static int serialize_is(mailjson_value * value, int flags,
    const char * expected, const char * message)
{
  char * data;
  size_t data_len;
  int r;
  int ok;

  data = NULL;
  data_len = 0;
  r = mailjson_serialize(value, flags, &data, &data_len);
  if (!check(r == MAILJSON_NO_ERROR, "serialization failed")) {
    free(data);
    return 0;
  }
  ok = expect_text(data, expected, message) &&
      check(data_len == strlen(data), "serialized length mismatch");
  free(data);
  return ok;
}

static int test_parse_invalid_json(void)
{
  mailjson_value * sentinel;
  mailjson_value * value;
  int r;

  sentinel = (mailjson_value *) 1;
  value = sentinel;
  r = mailjson_parse("{\"bad\"", 6, &value);

  if (value != sentinel)
    mailjson_free(value);
  return check(r == MAILJSON_ERROR_PARSE,
      "invalid JSON was not rejected") &&
      check(value == NULL, "invalid JSON did not clear result");
}

static int test_parse_duplicate_keys(void)
{
  mailjson_value * value;
  int r;
  int ok;

  value = NULL;
  r = mailjson_parse("{\"id\":\"a\",\"id\":\"b\"}", 19, &value);

  mailjson_free(value);
  ok = check(r == MAILJSON_ERROR_PARSE,
      "duplicate JSON object keys were not rejected");

  value = NULL;
  r = mailjson_parse("{\"outer\":{\"id\":\"a\",\"id\":\"b\"}}",
      strlen("{\"outer\":{\"id\":\"a\",\"id\":\"b\"}}"), &value);

  mailjson_free(value);
  ok = check(r == MAILJSON_ERROR_PARSE,
      "nested duplicate JSON object keys were not rejected") && ok;

  value = NULL;
  r = mailjson_parse("{\"id\":1,\"\\u0069d\":2}",
      strlen("{\"id\":1,\"\\u0069d\":2}"), &value);
  mailjson_free(value);
  return check(r == MAILJSON_ERROR_PARSE,
      "escaped duplicate JSON object keys were not rejected") && ok;
}

static int test_parse_trailing_data(void)
{
  mailjson_value * sentinel;
  mailjson_value * value;
  int r;

  sentinel = (mailjson_value *) 1;
  value = sentinel;
  r = mailjson_parse("{}{}", 4, &value);

  if (value != sentinel)
    mailjson_free(value);
  return check(r == MAILJSON_ERROR_PARSE,
      "JSON with trailing data was not rejected") &&
      check(value == NULL, "trailing JSON did not clear result");
}

static int test_parse_top_level_scalars(void)
{
  static const char * inputs[] = { "null", "true", "7", "\"value\"" };
  size_t index;

  for (index = 0; index < sizeof(inputs) / sizeof(inputs[0]); index ++) {
    mailjson_value * value;
    int r;

    value = NULL;
    r = mailjson_parse(inputs[index], strlen(inputs[index]), &value);
    if (!check(r == MAILJSON_NO_ERROR,
        "top-level JSON scalar was not accepted")) {
      mailjson_free(value);
      return 0;
    }
    mailjson_free(value);
  }
  return 1;
}

static int test_parse_and_lookup(void)
{
  static const char json[] =
      "{"
      "\"name\":\"Inbox\","
      "\"count\":9223372036854775807,"
      "\"enabled\":true,"
      "\"label\":\"caf\\u00e9\","
      "\"ids\":[\"a\",\"b\"],"
      "\"none\":null"
      "}";
  mailjson_value * root;
  mailjson_value * ids;
  mailjson_value * first;
  mailjson_value * none;
  char * name;
  char * label;
  char * id;
  int64_t count;
  int enabled;
  int r;
  int ok;

  root = NULL;
  ids = NULL;
  first = NULL;
  none = NULL;
  name = NULL;
  label = NULL;
  id = NULL;
  count = 0;
  enabled = 0;
  ok = 0;

  r = mailjson_parse(json, strlen(json), &root);
  if (!check(r == MAILJSON_NO_ERROR, "JSON parse failed"))
    goto cleanup;

  r = mailjson_object_get_string_dup(root, "name", &name);
  if (!check(r == MAILJSON_NO_ERROR, "string lookup failed"))
    goto cleanup;

  r = mailjson_object_get_integer(root, "count", &count);
  if (!check(r == MAILJSON_NO_ERROR, "integer lookup failed"))
    goto cleanup;

  r = mailjson_object_get_string_dup(root, "label", &label);
  if (!check(r == MAILJSON_NO_ERROR, "UTF-8 string lookup failed"))
    goto cleanup;

  r = mailjson_object_get_boolean(root, "enabled", &enabled);
  if (!check(r == MAILJSON_NO_ERROR, "boolean lookup failed"))
    goto cleanup;

  r = mailjson_object_get(root, "ids", &ids);
  if (!check(r == MAILJSON_NO_ERROR, "array lookup failed"))
    goto cleanup;

  r = mailjson_array_get(ids, 0, &first);
  if (!check(r == MAILJSON_NO_ERROR, "array item lookup failed"))
    goto cleanup;

  r = mailjson_string_dup(first, &id);
  if (!check(r == MAILJSON_NO_ERROR, "array string dup failed"))
    goto cleanup;

  r = mailjson_object_get(root, "none", &none);
  if (!check(r == MAILJSON_NO_ERROR, "null lookup failed"))
    goto cleanup;

  ok = check(str_equal(name, "Inbox"), "name mismatch") &&
      check(count == INT64_C(9223372036854775807), "count mismatch") &&
      check(str_equal(label, "caf" "\xC3" "\xA9"), "UTF-8 label mismatch") &&
      check(enabled == 1, "enabled mismatch") &&
      check(mailjson_array_size(ids) == 2, "array size mismatch") &&
      check(str_equal(id, "a"), "array value mismatch") &&
      check(mailjson_is_null(none), "null type mismatch");

 cleanup:
  free(name);
  free(label);
  free(id);
  mailjson_free(none);
  mailjson_free(first);
  mailjson_free(ids);
  mailjson_free(root);
  return ok;
}

static int test_build_and_serialize(void)
{
  mailjson_value * root;
  mailjson_value * using_array;
  mailjson_value * value;
  char * data;
  size_t data_len;
  int r;
  int ok;

  root = NULL;
  using_array = NULL;
  value = NULL;
  data = NULL;
  data_len = 0;
  ok = 0;

  r = mailjson_new_object(&root);
  if (!check(r == MAILJSON_NO_ERROR, "object allocation failed"))
    goto cleanup;

  r = mailjson_new_array(&using_array);
  if (!check(r == MAILJSON_NO_ERROR, "array allocation failed"))
    goto cleanup;

  r = mailjson_new_string("urn:ietf:params:jmap:core", &value);
  if (!check(r == MAILJSON_NO_ERROR, "string allocation failed"))
    goto cleanup;
  r = mailjson_array_append_new(using_array, value);
  value = NULL;
  if (!check(r == MAILJSON_NO_ERROR, "array append failed"))
    goto cleanup;

  r = mailjson_object_set_new(root, "using", using_array);
  using_array = NULL;
  if (!check(r == MAILJSON_NO_ERROR, "object set failed"))
    goto cleanup;

  r = mailjson_new_boolean(1, &value);
  if (!check(r == MAILJSON_NO_ERROR, "boolean allocation failed"))
    goto cleanup;
  r = mailjson_object_set_new(root, "ok", value);
  value = NULL;
  if (!check(r == MAILJSON_NO_ERROR, "boolean object set failed"))
    goto cleanup;

  r = mailjson_serialize(root,
      MAILJSON_SERIALIZE_COMPACT |
      MAILJSON_SERIALIZE_SORT_KEYS,
      &data, &data_len);
  if (!check(r == MAILJSON_NO_ERROR, "serialization failed"))
    goto cleanup;

  ok = check(str_equal(data,
      "{\"ok\":true,\"using\":[\"urn:ietf:params:jmap:core\"]}"),
      "serialized JSON mismatch") &&
      check(data_len == strlen(data), "serialized length mismatch");

 cleanup:
  free(data);
  mailjson_free(value);
  mailjson_free(using_array);
  mailjson_free(root);
  return ok;
}

static int test_build_scalars_and_sorted_serialize(void)
{
  mailjson_value * root;
  mailjson_value * value;
  char * data;
  size_t data_len;
  int r;
  int ok;

  root = NULL;
  value = NULL;
  data = NULL;
  data_len = 0;
  ok = 0;

  r = mailjson_new_object(&root);
  if (!check(r == MAILJSON_NO_ERROR, "object allocation failed"))
    goto cleanup;

  r = mailjson_new_string("z", &value);
  if (!check(r == MAILJSON_NO_ERROR, "string allocation failed"))
    goto cleanup;
  r = mailjson_object_set_new(root, "zeta", value);
  value = NULL;
  if (!check(r == MAILJSON_NO_ERROR, "zeta set failed"))
    goto cleanup;

  r = mailjson_new_null(&value);
  if (!check(r == MAILJSON_NO_ERROR, "null allocation failed"))
    goto cleanup;
  r = mailjson_object_set_new(root, "middle", value);
  value = NULL;
  if (!check(r == MAILJSON_NO_ERROR, "null set failed"))
    goto cleanup;

  r = mailjson_new_integer(7, &value);
  if (!check(r == MAILJSON_NO_ERROR, "integer allocation failed"))
    goto cleanup;
  r = mailjson_object_set_new(root, "alpha", value);
  value = NULL;
  if (!check(r == MAILJSON_NO_ERROR, "integer set failed"))
    goto cleanup;

  r = mailjson_serialize(root,
      MAILJSON_SERIALIZE_COMPACT |
      MAILJSON_SERIALIZE_SORT_KEYS,
      &data, &data_len);
  if (!check(r == MAILJSON_NO_ERROR, "serialization failed"))
    goto cleanup;

  ok = check(str_equal(data, "{\"alpha\":7,\"middle\":null,\"zeta\":\"z\"}"),
      "sorted scalar serialization mismatch") &&
      check(data_len == strlen(data), "serialized length mismatch");

 cleanup:
  free(data);
  mailjson_free(value);
  mailjson_free(root);
  return ok;
}

static int test_integer_creation_bounds(void)
{
  mailjson_value * value;
  int64_t parsed;
  int r;
  int ok;

  value = NULL;
  parsed = 0;
  ok = 0;

  r = mailjson_new_integer(INT64_C(9223372036854775807), &value);
  if (!check(r == MAILJSON_NO_ERROR, "INT64_MAX allocation failed"))
    goto cleanup;
  r = mailjson_integer_value(value, &parsed);
  if (!check(r == MAILJSON_NO_ERROR, "INT64_MAX lookup failed"))
    goto cleanup;
  ok = check(parsed == INT64_C(9223372036854775807),
      "INT64_MAX value mismatch");

 cleanup:
  mailjson_free(value);
  return ok;
}

static int test_solidus_round_trip(void)
{
  mailjson_value * value;
  int r;
  int ok;

  value = NULL;
  r = mailjson_parse("{\"s\":\"/\",\"a/b\":1}",
      strlen("{\"s\":\"/\",\"a/b\":1}"), &value);
  if (!check(r == MAILJSON_NO_ERROR, "solidus JSON parse failed")) {
    mailjson_free(value);
    return 0;
  }
  ok = serialize_is(value,
      MAILJSON_SERIALIZE_COMPACT | MAILJSON_SERIALIZE_SORT_KEYS,
      "{\"a/b\":1,\"s\":\"/\"}",
      "solidus serialization mismatch");
  mailjson_free(value);

  value = NULL;
  r = mailjson_parse("{\"s\":\"a/b\\\\/c\"}",
      strlen("{\"s\":\"a/b\\\\/c\"}"), &value);
  if (!check(r == MAILJSON_NO_ERROR, "backslash solidus parse failed")) {
    mailjson_free(value);
    return 0;
  }
  ok = serialize_is(value, MAILJSON_SERIALIZE_COMPACT,
      "{\"s\":\"a/b\\\\/c\"}",
      "backslash solidus serialization mismatch") && ok;
  mailjson_free(value);

  value = NULL;
  r = mailjson_parse("{\"b\":1,\"a\":\"x/y\"}",
      strlen("{\"b\":1,\"a\":\"x/y\"}"), &value);
  if (!check(r == MAILJSON_NO_ERROR, "spaced solidus parse failed")) {
    mailjson_free(value);
    return 0;
  }
  ok = serialize_is(value, 0, "{ \"b\": 1, \"a\": \"x/y\" }",
      "spaced solidus serialization mismatch") && ok;
  mailjson_free(value);
  return ok;
}

static int test_number_round_trip(void)
{
  static const char * inputs[] = { "1.5", "0.1" };
  size_t index;

  for (index = 0; index < sizeof(inputs) / sizeof(inputs[0]); index ++) {
    mailjson_value * value;
    mailjson_value * copy;
    int r;

    value = NULL;
    copy = NULL;
    r = mailjson_parse(inputs[index], strlen(inputs[index]), &value);
    if (!check(r == MAILJSON_NO_ERROR, "number parse failed")) {
      mailjson_free(value);
      return 0;
    }
    if (!serialize_is(value, MAILJSON_SERIALIZE_COMPACT, inputs[index],
        "number serialization mismatch")) {
      mailjson_free(value);
      return 0;
    }
    r = mailjson_deep_copy(value, &copy);
    if (!check(r == MAILJSON_NO_ERROR, "number deep copy failed")) {
      mailjson_free(copy);
      mailjson_free(value);
      return 0;
    }
    if (!serialize_is(copy, MAILJSON_SERIALIZE_COMPACT, inputs[index],
        "copied number serialization mismatch")) {
      mailjson_free(copy);
      mailjson_free(value);
      return 0;
    }
    mailjson_free(copy);
    mailjson_free(value);
  }
  return 1;
}

static int test_top_level_null_serialize(void)
{
  mailjson_value * value;
  int r;
  int ok;

  value = NULL;
  r = mailjson_parse("null", 4, &value);
  if (!check(r == MAILJSON_NO_ERROR, "top-level null parse failed")) {
    mailjson_free(value);
    return 0;
  }
  ok = check(mailjson_is_null(value), "top-level null type mismatch") &&
      serialize_is(value, MAILJSON_SERIALIZE_COMPACT, "null",
          "top-level null serialization mismatch");
  mailjson_free(value);

  value = NULL;
  r = mailjson_new_null(&value);
  if (!check(r == MAILJSON_NO_ERROR, "null allocation failed")) {
    mailjson_free(value);
    return 0;
  }
  ok = serialize_is(value, 0, "null",
      "constructed null serialization mismatch") && ok;
  mailjson_free(value);
  return ok;
}

static int test_strict_rejections(void)
{
  static const char trailing_object[] = "{\"a\":1,}";
  static const char trailing_array[] = "[1,]";
  static const char block_comment[] = "{\"a\":1}/*c*/";
  static const char inside_comment[] = "{/*c*/\"a\":1}";
  int ok;

  ok = expect_parse_error(trailing_object, strlen(trailing_object),
      "object trailing comma was not rejected");
  ok = expect_parse_error(trailing_array, strlen(trailing_array),
      "array trailing comma was not rejected") && ok;
  ok = expect_parse_error(block_comment, strlen(block_comment),
      "trailing comment was not rejected") && ok;
  return expect_parse_error(inside_comment, strlen(inside_comment),
      "comment inside an object was not rejected") && ok;
}

static int test_invalid_utf8(void)
{
  static const char ff_text[] = { '"', '\xFF', '"' };
  static const char overlong[] = { '"', '\xC0', '\x80', '"' };
  static const char surrogate[] = { '"', '\xED', '\xA0', '\x80', '"' };
  int ok;

  ok = expect_parse_error(ff_text, sizeof(ff_text),
      "invalid UTF-8 byte was not rejected");
  ok = expect_parse_error(overlong, sizeof(overlong),
      "overlong UTF-8 sequence was not rejected") && ok;
  return expect_parse_error(surrogate, sizeof(surrogate),
      "UTF-8 encoded surrogate was not rejected") && ok;
}

struct visit_keys {
  int count;
  int saw_a;
  int saw_b;
};

static int collect_keys(const char * key, mailjson_value * value,
    void * context)
{
  struct visit_keys * keys;

  (void) value;
  keys = context;
  keys->count ++;
  if (str_equal(key, "a"))
    keys->saw_a = 1;
  if (str_equal(key, "b"))
    keys->saw_b = 1;
  return MAILJSON_NO_ERROR;
}

static int stop_on_first(const char * key, mailjson_value * value,
    void * context)
{
  int * count;

  (void) key;
  (void) value;
  count = context;
  (* count) ++;
  return MAILJSON_ERROR_PARSE;
}

static int test_object_foreach(void)
{
  mailjson_value * root;
  struct visit_keys keys;
  int count;
  int r;
  int ok;

  root = NULL;
  memset(&keys, 0, sizeof(keys));
  r = mailjson_parse("{\"b\":1,\"a\":true}",
      strlen("{\"b\":1,\"a\":true}"), &root);
  if (!check(r == MAILJSON_NO_ERROR, "foreach object parse failed")) {
    mailjson_free(root);
    return 0;
  }
  r = mailjson_object_foreach(root, collect_keys, &keys);
  ok = check(r == MAILJSON_NO_ERROR, "foreach failed") &&
      check(keys.count == 2, "foreach visit count mismatch") &&
      check(keys.saw_a && keys.saw_b, "foreach missed a key");

  count = 0;
  r = mailjson_object_foreach(root, stop_on_first, &count);
  ok = check(r == MAILJSON_ERROR_PARSE, "foreach did not stop on error") &&
      check(count == 1, "foreach kept going after an error") && ok;
  mailjson_free(root);
  return ok;
}

static int test_deep_copy_independence(void)
{
  static const char input[] =
      "{\"b\":{\"d\":1,\"c\":2},\"a\":[],\"n\":null,\"s\":\"/\"}";
  static const char expected[] =
      "{\"a\":[],\"b\":{\"c\":2,\"d\":1},\"n\":null,\"s\":\"/\"}";
  static const char mutated[] =
      "{\"a\":[\"z\"],\"b\":{\"c\":2,\"d\":1,\"e\":3},\"n\":null,\"s\":\"/\"}";
  mailjson_value * root;
  mailjson_value * copy;
  mailjson_value * array;
  mailjson_value * nested;
  mailjson_value * extra;
  int r;
  int ok;

  root = NULL;
  copy = NULL;
  array = NULL;
  nested = NULL;
  extra = NULL;
  ok = 0;

  r = mailjson_parse(input, strlen(input), &root);
  if (!check(r == MAILJSON_NO_ERROR, "deep copy source parse failed"))
    goto cleanup;
  if (!serialize_is(root,
      MAILJSON_SERIALIZE_COMPACT | MAILJSON_SERIALIZE_SORT_KEYS,
      expected, "sorted nested serialization mismatch"))
    goto cleanup;

  r = mailjson_deep_copy(root, &copy);
  if (!check(r == MAILJSON_NO_ERROR, "deep copy failed"))
    goto cleanup;
  if (!serialize_is(copy,
      MAILJSON_SERIALIZE_COMPACT | MAILJSON_SERIALIZE_SORT_KEYS,
      expected, "deep copy serialization mismatch"))
    goto cleanup;

  r = mailjson_object_get(copy, "a", &array);
  if (!check(r == MAILJSON_NO_ERROR, "copied array lookup failed"))
    goto cleanup;
  r = mailjson_new_string("z", &extra);
  if (!check(r == MAILJSON_NO_ERROR, "copied array item allocation failed"))
    goto cleanup;
  r = mailjson_array_append_new(array, extra);
  extra = NULL;
  if (!check(r == MAILJSON_NO_ERROR, "copied array append failed"))
    goto cleanup;

  r = mailjson_object_get(copy, "b", &nested);
  if (!check(r == MAILJSON_NO_ERROR, "copied object lookup failed"))
    goto cleanup;
  r = mailjson_new_integer(3, &extra);
  if (!check(r == MAILJSON_NO_ERROR, "copied integer allocation failed"))
    goto cleanup;
  r = mailjson_object_set_new(nested, "e", extra);
  extra = NULL;
  if (!check(r == MAILJSON_NO_ERROR, "copied object set failed"))
    goto cleanup;

  ok = serialize_is(root,
      MAILJSON_SERIALIZE_COMPACT | MAILJSON_SERIALIZE_SORT_KEYS,
      expected, "original changed after mutating the copy") &&
      serialize_is(copy,
          MAILJSON_SERIALIZE_COMPACT | MAILJSON_SERIALIZE_SORT_KEYS,
          mutated, "mutated copy serialization mismatch");

 cleanup:
  mailjson_free(extra);
  mailjson_free(nested);
  mailjson_free(array);
  mailjson_free(copy);
  mailjson_free(root);
  return ok;
}

int main(void)
{
  int ok;

  ok = 1;
  ok = test_parse_invalid_json() && ok;
  ok = test_parse_duplicate_keys() && ok;
  ok = test_parse_trailing_data() && ok;
  ok = test_parse_top_level_scalars() && ok;
  ok = test_parse_and_lookup() && ok;
  ok = test_build_and_serialize() && ok;
  ok = test_build_scalars_and_sorted_serialize() && ok;
  ok = test_integer_creation_bounds() && ok;
  ok = test_solidus_round_trip() && ok;
  ok = test_number_round_trip() && ok;
  ok = test_top_level_null_serialize() && ok;
  ok = test_strict_rejections() && ok;
  ok = test_invalid_utf8() && ok;
  ok = test_object_foreach() && ok;
  ok = test_deep_copy_independence() && ok;

  if (!ok)
    return 1;

  puts("jmap-json-test: ok");
  return 0;
}
