#include <stdio.h>

#include "imap_tests.h"

int main(void)
{
  imap_response_data_test_run();
  imap_response_done_test_run();
  imap_unsupported_response_test_run();
  imap_command_sender_test_run();
  imap_command_parameter_sender_test_run();
  imap_idle_test_run();
  imap_multiappend_test_run();
  imap_binary_test_run();
  imap_esearch_test_run();
  imap_specialuse_test_run();
  imap_list_status_test_run();
  imap_list_extended_test_run();
  imap_sort_display_test_run();
  imap_metadata_test_run();
  imap_utf8_test_run();

  puts("imap_test: ok");
  return 0;
}
