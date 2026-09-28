#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "metadata_test.h"
#include "metadata.h"

int imap_metadata_test_run(void)
{
  struct mailimap_metadata_entry * entry;

  entry = mailimap_metadata_entry_new(strdup("/private/comment"), strdup("My note"));
  assert(entry != NULL);
  assert(strcmp(entry->me_name, "/private/comment") == 0);
  assert(strcmp(entry->me_value, "My note") == 0);

  mailimap_metadata_entry_free(entry);

  puts("metadata_test: ok");
  return 0;
}
