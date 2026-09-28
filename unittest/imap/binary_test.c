#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "binary_test.h"
#include "binary.h"
#include "clist.h"

static clist * make_test_sec_id(void)
{
  clist * sec_id = clist_new();
  uint32_t * val = malloc(sizeof(* val));
  if (sec_id == NULL || val == NULL) {
    free(val);
    clist_free(sec_id);
    return NULL;
  }
  *val = 1;
  clist_append(sec_id, val);
  return sec_id;
}

int imap_binary_test_run(void)
{
  struct mailimap_fetch_att * att1;
  struct mailimap_fetch_att * att2;
  struct mailimap_fetch_att * att3;

  att1 = mailimap_fetch_att_binary_new(make_test_sec_id(), 0, 1024);
  assert(att1 != NULL);
  assert(att1->att_type == MAILIMAP_FETCH_ATT_BODY_SECTION);

  att2 = mailimap_fetch_att_binary_peek_new(make_test_sec_id(), 0, 1024);
  assert(att2 != NULL);
  assert(att2->att_type == MAILIMAP_FETCH_ATT_BODY_PEEK_SECTION);

  att3 = mailimap_fetch_att_binary_size_new(make_test_sec_id());
  assert(att3 != NULL);

  mailimap_fetch_att_free(att1);
  mailimap_fetch_att_free(att2);
  mailimap_fetch_att_free(att3);

  puts("binary_test: ok");
  return 0;
}
