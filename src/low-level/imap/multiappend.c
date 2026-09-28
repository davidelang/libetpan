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

#include "multiappend.h"

#include <stdlib.h>

#include "mailimap_sender.h"
#include "mailimap_parser.h"
#include "mailimap.h"

LIBETPAN_EXPORT
struct mailimap_append_message *
mailimap_append_message_new(struct mailimap_flag_list * flags,
    struct mailimap_date_time * date_time,
    const char * message, size_t size)
{
  struct mailimap_append_message * msg;

  msg = malloc(sizeof(* msg));
  if (msg == NULL)
    return NULL;

  msg->am_flags = flags;
  msg->am_date_time = date_time;
  msg->am_message = message;
  msg->am_size = size;

  return msg;
}

LIBETPAN_EXPORT
void mailimap_append_message_free(struct mailimap_append_message * msg)
{
  if (msg == NULL)
    return;
  if (msg->am_flags != NULL)
    mailimap_flag_list_free(msg->am_flags);
  if (msg->am_date_time != NULL)
    mailimap_date_time_free(msg->am_date_time);
  free(msg);
}

LIBETPAN_EXPORT
int mailimap_multiappend(mailimap * session, const char * mailbox,
    clist * message_list)
{
  struct mailimap_response * response;
  int r;
  int error_code;
  clistiter * iter;

  if ((session->imap_state != MAILIMAP_STATE_AUTHENTICATED) &&
      (session->imap_state != MAILIMAP_STATE_SELECTED))
    return MAILIMAP_ERROR_BAD_STATE;

  if (message_list == NULL || clist_begin(message_list) == NULL)
    return MAILIMAP_ERROR_INVAL;

  r = mailimap_send_current_tag(session);
  if (r != MAILIMAP_NO_ERROR)
    return r;

  r = mailimap_token_send(session->imap_stream, "APPEND");
  if (r != MAILIMAP_NO_ERROR)
    return r;

  r = mailimap_space_send(session->imap_stream);
  if (r != MAILIMAP_NO_ERROR)
    return r;

  r = mailimap_mailbox_send(session->imap_stream, mailbox);
  if (r != MAILIMAP_NO_ERROR)
    return r;

  for (iter = clist_begin(message_list); iter != NULL; iter = clist_next(iter)) {
    struct mailimap_append_message * msg;
    struct mailimap_continue_req * cont_req;
    size_t fixed_literal_size;
    size_t indx;

    msg = clist_content(iter);

    r = mailimap_space_send(session->imap_stream);
    if (r != MAILIMAP_NO_ERROR)
      return r;

    if (msg->am_flags != NULL) {
      r = mailimap_flag_list_send(session->imap_stream, msg->am_flags);
      if (r != MAILIMAP_NO_ERROR)
        return r;
      r = mailimap_space_send(session->imap_stream);
      if (r != MAILIMAP_NO_ERROR)
        return r;
    }

    if (msg->am_date_time != NULL) {
      r = mailimap_date_time_send(session->imap_stream, msg->am_date_time);
      if (r != MAILIMAP_NO_ERROR)
        return r;
      r = mailimap_space_send(session->imap_stream);
      if (r != MAILIMAP_NO_ERROR)
        return r;
    }

    fixed_literal_size = mailstream_get_data_crlf_size(msg->am_message, msg->am_size);
    r = mailimap_literal_count_send(session->imap_stream, fixed_literal_size);
    if (r != MAILIMAP_NO_ERROR)
      return r;

    if (mailstream_flush(session->imap_stream) == -1)
      return MAILIMAP_ERROR_STREAM;

    if (mailimap_read_line(session) == NULL)
      return MAILIMAP_ERROR_STREAM;

    indx = 0;
    r = mailimap_continue_req_parse(session->imap_stream,
        session->imap_stream_buffer, NULL,
        &indx, &cont_req,
        session->imap_progr_rate, session->imap_progr_fun);
    if (r == MAILIMAP_NO_ERROR)
      mailimap_continue_req_free(cont_req);

    if (r == MAILIMAP_ERROR_PARSE) {
      r = mailimap_parse_response(session, &response);
      if (r != MAILIMAP_NO_ERROR)
        return r;
      mailimap_response_free(response);
      return MAILIMAP_ERROR_APPEND;
    }

    if (session->imap_body_progress_fun != NULL) {
      r = mailimap_literal_data_send_with_context(session->imap_stream,
          msg->am_message, msg->am_size,
          session->imap_body_progress_fun,
          session->imap_progress_context);
    }
    else {
      r = mailimap_literal_data_send(session->imap_stream,
          msg->am_message, msg->am_size,
          session->imap_progr_rate, session->imap_progr_fun);
    }
    if (r != MAILIMAP_NO_ERROR)
      return r;
  }

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

  switch (error_code) {
  case MAILIMAP_RESP_COND_STATE_OK:
    return MAILIMAP_NO_ERROR;

  default:
    return MAILIMAP_ERROR_APPEND;
  }
}

LIBETPAN_EXPORT
int mailimap_has_multiappend(mailimap * session)
{
  return mailimap_has_extension(session, "MULTIAPPEND");
}
