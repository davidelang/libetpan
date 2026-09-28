/*
 * libEtPan! -- a mail stuff library
 *
 * Copyright (C) 2001, 2026 - DINH Viet Hoa
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the libEtPan! project nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "specialuse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "mailimap_sender.h"
#include "mailimap.h"

LIBETPAN_EXPORT
int mailimap_has_special_use(mailimap * session)
{
  return mailimap_has_extension(session, "SPECIAL-USE");
}

static int send_special_use_flags(mailstream * fd, int use_flags)
{
  int r;
  int first = 1;

  if (use_flags & MAILIMAP_SPECIALUSE_FLAG_ALL) {
    if (!first) {
      r = mailimap_space_send(fd);
      if (r != MAILIMAP_NO_ERROR) return r;
    }
    r = mailimap_token_send(fd, "\\All");
    if (r != MAILIMAP_NO_ERROR) return r;
    first = 0;
  }
  if (use_flags & MAILIMAP_SPECIALUSE_FLAG_ARCHIVE) {
    if (!first) {
      r = mailimap_space_send(fd);
      if (r != MAILIMAP_NO_ERROR) return r;
    }
    r = mailimap_token_send(fd, "\\Archive");
    if (r != MAILIMAP_NO_ERROR) return r;
    first = 0;
  }
  if (use_flags & MAILIMAP_SPECIALUSE_FLAG_DRAFTS) {
    if (!first) {
      r = mailimap_space_send(fd);
      if (r != MAILIMAP_NO_ERROR) return r;
    }
    r = mailimap_token_send(fd, "\\Drafts");
    if (r != MAILIMAP_NO_ERROR) return r;
    first = 0;
  }
  if (use_flags & MAILIMAP_SPECIALUSE_FLAG_FLAGGED) {
    if (!first) {
      r = mailimap_space_send(fd);
      if (r != MAILIMAP_NO_ERROR) return r;
    }
    r = mailimap_token_send(fd, "\\Flagged");
    if (r != MAILIMAP_NO_ERROR) return r;
    first = 0;
  }
  if (use_flags & MAILIMAP_SPECIALUSE_FLAG_JUNK) {
    if (!first) {
      r = mailimap_space_send(fd);
      if (r != MAILIMAP_NO_ERROR) return r;
    }
    r = mailimap_token_send(fd, "\\Junk");
    if (r != MAILIMAP_NO_ERROR) return r;
    first = 0;
  }
  if (use_flags & MAILIMAP_SPECIALUSE_FLAG_SENT) {
    if (!first) {
      r = mailimap_space_send(fd);
      if (r != MAILIMAP_NO_ERROR) return r;
    }
    r = mailimap_token_send(fd, "\\Sent");
    if (r != MAILIMAP_NO_ERROR) return r;
    first = 0;
  }
  if (use_flags & MAILIMAP_SPECIALUSE_FLAG_TRASH) {
    if (!first) {
      r = mailimap_space_send(fd);
      if (r != MAILIMAP_NO_ERROR) return r;
    }
    r = mailimap_token_send(fd, "\\Trash");
    if (r != MAILIMAP_NO_ERROR) return r;
    first = 0;
  }

  return MAILIMAP_NO_ERROR;
}

static int mailimap_create_special_use_send(mailstream * fd, const char * mb, int use_flags)
{
  int r;

  r = mailimap_token_send(fd, "CREATE");
  if (r != MAILIMAP_NO_ERROR) return r;

  r = mailimap_space_send(fd);
  if (r != MAILIMAP_NO_ERROR) return r;

  r = mailimap_astring_send(fd, mb);
  if (r != MAILIMAP_NO_ERROR) return r;

  if (use_flags != 0) {
    r = mailimap_space_send(fd);
    if (r != MAILIMAP_NO_ERROR) return r;
    r = mailimap_token_send(fd, "(");
    if (r != MAILIMAP_NO_ERROR) return r;
    r = mailimap_token_send(fd, "USE");
    if (r != MAILIMAP_NO_ERROR) return r;
    r = mailimap_space_send(fd);
    if (r != MAILIMAP_NO_ERROR) return r;
    r = mailimap_token_send(fd, "(");
    if (r != MAILIMAP_NO_ERROR) return r;

    r = send_special_use_flags(fd, use_flags);
    if (r != MAILIMAP_NO_ERROR) return r;

    r = mailimap_token_send(fd, ")");
    if (r != MAILIMAP_NO_ERROR) return r;
    r = mailimap_token_send(fd, ")");
    if (r != MAILIMAP_NO_ERROR) return r;
  }

  return MAILIMAP_NO_ERROR;
}

LIBETPAN_EXPORT
int mailimap_create_special_use(mailimap * session, const char * mb, int use_flags)
{
  struct mailimap_response * response;
  int r;
  int error_code;

  if (session->imap_state != MAILIMAP_STATE_AUTHENTICATED &&
      session->imap_state != MAILIMAP_STATE_SELECTED)
    return MAILIMAP_ERROR_BAD_STATE;

  r = mailimap_send_current_tag(session);
  if (r != MAILIMAP_NO_ERROR)
    return r;

  r = mailimap_create_special_use_send(session->imap_stream, mb, use_flags);
  if (r != MAILIMAP_NO_ERROR)
    return r;

  r = mailimap_crlf_send(session->imap_stream);
  if (r != MAILIMAP_NO_ERROR)
    return r;

  if (mailstream_flush(session->imap_stream) == -1)
    return MAILIMAP_ERROR_STREAM;

  if (mailimap_read_line(session) == NULL)
    return MAILIMAP_ERROR_STREAM;

  r = mailimap_parse_response(session, &response);
  if (r != MAILIMAP_NO_ERROR)
    return r;

  error_code = response->rsp_resp_done->rsp_data.rsp_tagged->rsp_cond_state->rsp_type;
  mailimap_response_free(response);

  if (error_code != MAILIMAP_RESP_COND_STATE_OK)
    return MAILIMAP_ERROR_CREATE;

  return MAILIMAP_NO_ERROR;
}

LIBETPAN_EXPORT
int mailimap_mailbox_list_has_special_use_flag(struct mailimap_mailbox_list * mb_list, int use_flag)
{
  clistiter * cur;
  const char * expected = NULL;

  if (mb_list == NULL || mb_list->mb_flag == NULL || mb_list->mb_flag->mbf_oflags == NULL)
    return 0;

  switch (use_flag) {
  case MAILIMAP_SPECIALUSE_FLAG_ALL: expected = "\\All"; break;
  case MAILIMAP_SPECIALUSE_FLAG_ARCHIVE: expected = "\\Archive"; break;
  case MAILIMAP_SPECIALUSE_FLAG_DRAFTS: expected = "\\Drafts"; break;
  case MAILIMAP_SPECIALUSE_FLAG_FLAGGED: expected = "\\Flagged"; break;
  case MAILIMAP_SPECIALUSE_FLAG_JUNK: expected = "\\Junk"; break;
  case MAILIMAP_SPECIALUSE_FLAG_SENT: expected = "\\Sent"; break;
  case MAILIMAP_SPECIALUSE_FLAG_TRASH: expected = "\\Trash"; break;
  default: return 0;
  }

  for (cur = clist_begin(mb_list->mb_flag->mbf_oflags); cur != NULL; cur = clist_next(cur)) {
    struct mailimap_mbx_list_oflag * oflag = clist_content(cur);
    if (oflag != NULL && oflag->of_type == MAILIMAP_MBX_LIST_OFLAG_FLAG_EXT && oflag->of_flag_ext != NULL) {
      if (strcasecmp(oflag->of_flag_ext, expected) == 0)
        return 1;
      if (oflag->of_flag_ext[0] == '\\' && strcasecmp(oflag->of_flag_ext, expected) == 0)
        return 1;
      if (expected[0] == '\\' && strcasecmp(oflag->of_flag_ext, expected + 1) == 0)
        return 1;
    }
  }

  return 0;
}
