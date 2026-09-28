#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "sort_display_test.h"
#include "mailimap_sort.h"

int imap_sort_display_test_run(void)
{
  struct mailimap_sort_key * key_df;
  struct mailimap_sort_key * key_dt;

  key_df = mailimap_sort_key_new_displayfrom(0);
  assert(key_df != NULL);
  assert(key_df->sortk_type == MAILIMAP_SORT_KEY_DISPLAYFROM);
  mailimap_sort_key_free(key_df);

  key_dt = mailimap_sort_key_new_displayto(1);
  assert(key_dt != NULL);
  assert(key_dt->sortk_type == MAILIMAP_SORT_KEY_DISPLAYTO);
  assert(key_dt->sortk_is_reverse == 1);
  mailimap_sort_key_free(key_dt);

  puts("sort_display_test: ok");
  return 0;
}
