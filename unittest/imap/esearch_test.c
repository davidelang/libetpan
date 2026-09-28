#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "esearch_test.h"
#include "esearch.h"

int imap_esearch_test_run(void)
{
  struct mailimap_esearch_result * res;

  res = mailimap_esearch_result_new();
  assert(res != NULL);
  assert(res->min == 0);
  assert(res->max == 0);
  assert(res->count == 0);
  assert(res->msg_list == NULL);

  res->min = 5;
  res->has_min = 1;
  res->max = 42;
  res->has_max = 1;
  res->count = 10;
  res->has_count = 1;

  mailimap_esearch_result_free(res);

  puts("esearch_test: ok");
  return 0;
}
