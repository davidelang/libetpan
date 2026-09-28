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

#include "list_extended.h"

#include <stdio.h>
#include <stdlib.h>

#include "mailimap_sender.h"
#include "mailimap.h"

LIBETPAN_EXPORT
int mailimap_has_list_extended(mailimap * session)
{
  return mailimap_has_extension(session, "LIST-EXTENDED");
}

static int send_selection_options(mailstream * fd, int selection_options)
{
  int r;
  int first = 1;

  if (selection_options == 0)
    return MAILIMAP_NO_ERROR;

  r = mailimap_space_send(fd);
  if (r != MAILIMAP_NO_ERROR) return r;

  r = mailimap_token_send(fd, "(");
  if (r != MAILIMAP_NO_ERROR) return r;

  if (selection_options & MAILIMAP_LIST_EXTENDED_SELECT_SUBSCRIBED) {
    if (!first) {
      r = mailimap_space_send(fd);
      if (r != MAILIMAP_NO_ERROR) return r;
    }
    r = mailimap_token_send(fd, "SUBSCRIBED");
    if (r != MAILIMAP_NO_ERROR) return r;
    first = 0;
  }
  if (selection_options & MAILIMAP_LIST_EXTENDED_SELECT_REMOTE) {
    if (!first) {
      r = mailimap_space_send(fd);
      if (r != MAILIMAP_NO_ERROR) return r;
    }
    r = mailimap_token_send(fd, "REMOTE");
    if (r != MAILIMAP_NO_ERROR) return r;
    first = 0;
  }
  if (selection_options & MAILIMAP_LIST_EXTENDED_SELECT_RECURSIVEMATCH) {
    if (!first) {
      r = mailimap_space_send(fd);
      if (r != MAILIMAP_NO_ERROR) return r;
    }
    r = mailimap_token_send(fd, "RECURSIVEMATCH");
    if (r != MAILIMAP_NO_ERROR) return r;
    first = 0;
  }
  if (selection_options & MAILIMAP_LIST_EXTENDED_SELECT_SPECIAL_USE) {
    if (!first) {
      r = mailimap_space_send(fd);
      if (r != MAILIMAP_NO_ERROR) return r;
    }
    r = mailimap_token_send(fd, "SPECIAL-USE");
    if (r != MAILIMAP_NO_ERROR) return r;
    first = 0;
  }

  r = mailimap_token_send(fd, ")");
  if (r != MAILIMAP_NO_ERROR) return r;

  return MAILIMAP_NO_ERROR;
}

static int send_return_options(mailstream * fd, int return_options)
{
  int r;
  int first = 1;

  if (return_options == 0)
    return MAILIMAP_NO_ERROR;

  r = mailimap_space_send(fd);
  if (r != MAILIMAP_NO_ERROR) return r;

  r = mailimap_token_send(fd, "RETURN");
  if (r != MAILIMAP_NO_ERROR) return r;

  r = mailimap_space_send(fd);
  if (r != MAILIMAP_NO_ERROR) return r;

  r = mailimap_token_send(fd, "(");
  if (r != MAILIMAP_NO_ERROR) return r;

  if (return_options & MAILIMAP_LIST_EXTENDED_RETURN_SUBSCRIBED) {
    if (!first) {
      r = mailimap_space_send(fd);
      if (r != MAILIMAP_NO_ERROR) return r;
    }
    r = mailimap_token_send(fd, "SUBSCRIBED");
    if (r != MAILIMAP_NO_ERROR) return r;
    first = 0;
  }
  if (return_options & MAILIMAP_LIST_EXTENDED_RETURN_CHILDREN) {
    if (!first) {
      r = mailimap_space_send(fd);
      if (r != MAILIMAP_NO_ERROR) return r;
    }
    r = mailimap_token_send(fd, "CHILDREN");
    if (r != MAILIMAP_NO_ERROR) return r;
    first = 0;
  }
  if (return_options & MAILIMAP_LIST_EXTENDED_RETURN_SPECIAL_USE) {
    if (!first) {
      r = mailimap_space_send(fd);
      if (r != MAILIMAP_NO_ERROR) return r;
    }
    r = mailimap_token_send(fd, "SPECIAL-USE");
    if (r != MAILIMAP_NO_ERROR) return r;
    first = 0;
  }

  r = mailimap_token_send(fd, ")");
  if (r != MAILIMAP_NO_ERROR) return r;

  return MAILIMAP_NO_ERROR;
}

static int mailimap_list_extended_send(mailstream * fd, int selection_options,
    const char * mb, const char * list_mb, int return_options)
{
  int r;

  r = mailimap_token_send(fd, "LIST");
  if (r != MAILIMAP_NO_ERROR) return r;

  r = send_selection_options(fd, selection_options);
  if (r != MAILIMAP_NO_ERROR) return r;

  r = mailimap_space_send(fd);
  if (r != MAILIMAP_NO_ERROR) return r;

  r = mailimap_mailbox_send(fd, mb);
  if (r != MAILIMAP_NO_ERROR) return r;

  r = mailimap_space_send(fd);
  if (r != MAILIMAP_NO_ERROR) return r;

  r = mailimap_list_mailbox_send(fd, list_mb);
  if (r != MAILIMAP_NO_ERROR) return r;

  return send_return_options(fd, return_options);
}

LIBETPAN_EXPORT
int mailimap_list_extended(mailimap * session, int selection_options,
    const char * mb, const char * list_mb, int return_options,
    clist ** result)
{
  struct mailimap_response * response;
  int r;
  int error_code;

  if ((session->imap_state != MAILIMAP_STATE_AUTHENTICATED) &&
      (session->imap_state != MAILIMAP_STATE_SELECTED))
    return MAILIMAP_ERROR_BAD_STATE;

  r = mailimap_send_current_tag(session);
  if (r != MAILIMAP_NO_ERROR)
    return r;

  r = mailimap_list_extended_send(session->imap_stream, selection_options, mb, list_mb, return_options);
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

  if (result != NULL) {
    *result = session->imap_response_info->rsp_mailbox_list;
    session->imap_response_info->rsp_mailbox_list = NULL;
  }

  error_code = response->rsp_resp_done->rsp_data.rsp_tagged->rsp_cond_state->rsp_type;
  mailimap_response_free(response);

  switch (error_code) {
  case MAILIMAP_RESP_COND_STATE_OK:
    return MAILIMAP_NO_ERROR;

  default:
    return MAILIMAP_ERROR_LIST;
  }
}
