#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "multiappend_test.h"
#include "multiappend.h"
#include "clist.h"

int imap_multiappend_test_run(void)
{
  struct mailimap_append_message * msg1;
  struct mailimap_append_message * msg2;
  clist * list;

  msg1 = mailimap_append_message_new(NULL, NULL, "Test message 1", 14);
  assert(msg1 != NULL);
  assert(msg1->am_size == 14);

  msg2 = mailimap_append_message_new(NULL, NULL, "Test message 2", 14);
  assert(msg2 != NULL);

  list = clist_new();
  assert(list != NULL);
  assert(clist_append(list, msg1) == 0);
  assert(clist_append(list, msg2) == 0);

  mailimap_append_message_free(msg1);
  mailimap_append_message_free(msg2);
  clist_free(list);

  puts("multiappend_test: ok");
  return 0;
}
