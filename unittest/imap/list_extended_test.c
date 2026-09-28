#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "list_extended_test.h"
#include "list_extended.h"

int imap_list_extended_test_run(void)
{
  int select_opts;
  int return_opts;

  select_opts = MAILIMAP_LIST_EXTENDED_SELECT_SUBSCRIBED | MAILIMAP_LIST_EXTENDED_SELECT_SPECIAL_USE;
  return_opts = MAILIMAP_LIST_EXTENDED_RETURN_CHILDREN | MAILIMAP_LIST_EXTENDED_RETURN_SPECIAL_USE;

  assert(select_opts != 0);
  assert(return_opts != 0);

  puts("list_extended_test: ok");
  return 0;
}
