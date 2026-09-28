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

#include "list_status.h"

#include <stdio.h>
#include <stdlib.h>

#include "mailimap_sender.h"
#include "mailimap.h"

LIBETPAN_EXPORT
int mailimap_has_list_status(mailimap * session)
{
  return mailimap_has_extension(session, "LIST-STATUS");
}

static int mailimap_list_status_send(mailstream * fd, const char * mb, const char * list_mb,
    struct mailimap_status_att_list * status_att_list)
{
  int r;

  r = mailimap_token_send(fd, "LIST");
  if (r != MAILIMAP_NO_ERROR) return r;

  if (status_att_list != NULL) {
    r = mailimap_space_send(fd);
    if (r != MAILIMAP_NO_ERROR) return r;
    r = mailimap_token_send(fd, "(");
    if (r != MAILIMAP_NO_ERROR) return r;
    r = mailimap_token_send(fd, "STATUS");
    if (r != MAILIMAP_NO_ERROR) return r;
    r = mailimap_space_send(fd);
    if (r != MAILIMAP_NO_ERROR) return r;
    r = mailimap_oparenth_send(fd);
    if (r != MAILIMAP_NO_ERROR) return r;
    r = mailimap_status_att_list_send(fd, status_att_list);
    if (r != MAILIMAP_NO_ERROR) return r;
    r = mailimap_cparenth_send(fd);
    if (r != MAILIMAP_NO_ERROR) return r;
    r = mailimap_token_send(fd, ")");
    if (r != MAILIMAP_NO_ERROR) return r;
  }

  r = mailimap_space_send(fd);
  if (r != MAILIMAP_NO_ERROR) return r;

  r = mailimap_mailbox_send(fd, mb);
  if (r != MAILIMAP_NO_ERROR) return r;

  r = mailimap_space_send(fd);
  if (r != MAILIMAP_NO_ERROR) return r;

  return mailimap_list_mailbox_send(fd, list_mb);
}

LIBETPAN_EXPORT
int mailimap_list_status(mailimap * session, const char * mb, const char * list_mb,
    struct mailimap_status_att_list * status_att_list,
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

  r = mailimap_list_status_send(session->imap_stream, mb, list_mb, status_att_list);
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
