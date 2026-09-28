#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "list_status_test.h"
#include "list_status.h"
#include "mailimap_types_helper.h"

int imap_list_status_test_run(void)
{
  struct mailimap_status_att_list * sa_list;

  sa_list = mailimap_status_att_list_new_empty();
  assert(sa_list != NULL);

  mailimap_status_att_list_add(sa_list, MAILIMAP_STATUS_ATT_MESSAGES);
  mailimap_status_att_list_add(sa_list, MAILIMAP_STATUS_ATT_UNSEEN);
  assert(sa_list->att_list != NULL);

  mailimap_status_att_list_free(sa_list);

  puts("list_status_test: ok");
  return 0;
}
