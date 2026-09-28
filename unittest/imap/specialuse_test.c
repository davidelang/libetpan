#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "specialuse_test.h"
#include "specialuse.h"
#include "clist.h"

int imap_specialuse_test_run(void)
{
  struct mailimap_mailbox_list * mb_list;
  struct mailimap_mbx_list_flags * flags;
  struct mailimap_mbx_list_oflag * oflag;
  clist * oflags;

  oflags = clist_new();
  assert(oflags != NULL);

  oflag = malloc(sizeof(* oflag));
  assert(oflag != NULL);
  oflag->of_type = MAILIMAP_MBX_LIST_OFLAG_FLAG_EXT;
  oflag->of_flag_ext = strdup("\\Sent");
  clist_append(oflags, oflag);

  flags = mailimap_mbx_list_flags_new(0, oflags, 0);
  assert(flags != NULL);

  mb_list = mailimap_mailbox_list_new(flags, '/', strdup("Sent"));
  assert(mb_list != NULL);

  assert(mailimap_mailbox_list_has_special_use_flag(mb_list, MAILIMAP_SPECIALUSE_FLAG_SENT) == 1);
  assert(mailimap_mailbox_list_has_special_use_flag(mb_list, MAILIMAP_SPECIALUSE_FLAG_DRAFTS) == 0);

  mailimap_mailbox_list_free(mb_list);

  puts("specialuse_test: ok");
  return 0;
}
