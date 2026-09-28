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

#include "esearch.h"

#include <stdio.h>
#include <stdlib.h>

#include "mailimap_sender.h"
#include "mailimap.h"

LIBETPAN_EXPORT
struct mailimap_esearch_result * mailimap_esearch_result_new(void)
{
  struct mailimap_esearch_result * res;

  res = calloc(1, sizeof(* res));
  if (res == NULL)
    return NULL;

  return res;
}

LIBETPAN_EXPORT
void mailimap_esearch_result_free(struct mailimap_esearch_result * result)
{
  if (result == NULL)
    return;

  if (result->msg_list != NULL) {
    clist_foreach(result->msg_list, (clist_func) free, NULL);
    clist_free(result->msg_list);
  }
  free(result);
}

LIBETPAN_EXPORT
int mailimap_has_esearch(mailimap * session)
{
  return mailimap_has_extension(session, "ESEARCH");
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

  if (return_options & MAILIMAP_ESEARCH_RETURN_MIN) {
    if (!first) {
      r = mailimap_space_send(fd);
      if (r != MAILIMAP_NO_ERROR) return r;
    }
    r = mailimap_token_send(fd, "MIN");
    if (r != MAILIMAP_NO_ERROR) return r;
    first = 0;
  }
  if (return_options & MAILIMAP_ESEARCH_RETURN_MAX) {
    if (!first) {
      r = mailimap_space_send(fd);
      if (r != MAILIMAP_NO_ERROR) return r;
    }
    r = mailimap_token_send(fd, "MAX");
    if (r != MAILIMAP_NO_ERROR) return r;
    first = 0;
  }
  if (return_options & MAILIMAP_ESEARCH_RETURN_ALL) {
    if (!first) {
      r = mailimap_space_send(fd);
      if (r != MAILIMAP_NO_ERROR) return r;
    }
    r = mailimap_token_send(fd, "ALL");
    if (r != MAILIMAP_NO_ERROR) return r;
    first = 0;
  }
  if (return_options & MAILIMAP_ESEARCH_RETURN_COUNT) {
    if (!first) {
      r = mailimap_space_send(fd);
      if (r != MAILIMAP_NO_ERROR) return r;
    }
    r = mailimap_token_send(fd, "COUNT");
    if (r != MAILIMAP_NO_ERROR) return r;
    first = 0;
  }

  r = mailimap_token_send(fd, ")");
  if (r != MAILIMAP_NO_ERROR) return r;

  return MAILIMAP_NO_ERROR;
}

static int mailimap_esearch_send(mailstream * fd, const char * charset,
    struct mailimap_search_key * key, int return_options)
{
  int r;
  int needToSendCharset;

  r = mailimap_token_send(fd, "SEARCH");
  if (r != MAILIMAP_NO_ERROR) return r;

  r = send_return_options(fd, return_options);
  if (r != MAILIMAP_NO_ERROR) return r;

  if (charset != NULL) {
    r = mailimap_space_send(fd);
    if (r != MAILIMAP_NO_ERROR) return r;
    r = mailimap_token_send(fd, "CHARSET");
    if (r != MAILIMAP_NO_ERROR) return r;
    r = mailimap_space_send(fd);
    if (r != MAILIMAP_NO_ERROR) return r;
    r = mailimap_astring_send(fd, charset);
    if (r != MAILIMAP_NO_ERROR) return r;
  }

  r = mailimap_space_send(fd);
  if (r != MAILIMAP_NO_ERROR) return r;

  return mailimap_search_key_send(fd, key);
}

static int mailimap_uid_esearch_send(mailstream * fd, const char * charset,
    struct mailimap_search_key * key, int return_options)
{
  int r;

  r = mailimap_token_send(fd, "UID");
  if (r != MAILIMAP_NO_ERROR) return r;

  r = mailimap_space_send(fd);
  if (r != MAILIMAP_NO_ERROR) return r;

  return mailimap_esearch_send(fd, charset, key, return_options);
}

LIBETPAN_EXPORT
int mailimap_esearch(mailimap * session, const char * charset,
    struct mailimap_search_key * key, int return_options,
    struct mailimap_esearch_result ** result)
{
  struct mailimap_response * response;
  int r;
  int error_code;

  if (session->imap_state != MAILIMAP_STATE_SELECTED)
    return MAILIMAP_ERROR_BAD_STATE;

  r = mailimap_send_current_tag(session);
  if (r != MAILIMAP_NO_ERROR)
    return r;

  r = mailimap_esearch_send(session->imap_stream, charset, key, return_options);
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
    return MAILIMAP_ERROR_EXTENSION;

  if (result != NULL) {
    *result = mailimap_esearch_result_new();
    if (*result == NULL)
      return MAILIMAP_ERROR_MEMORY;
  }

  return MAILIMAP_NO_ERROR;
}

LIBETPAN_EXPORT
int mailimap_uid_esearch(mailimap * session, const char * charset,
    struct mailimap_search_key * key, int return_options,
    struct mailimap_esearch_result ** result)
{
  struct mailimap_response * response;
  int r;
  int error_code;

  if (session->imap_state != MAILIMAP_STATE_SELECTED)
    return MAILIMAP_ERROR_BAD_STATE;

  r = mailimap_send_current_tag(session);
  if (r != MAILIMAP_NO_ERROR)
    return r;

  r = mailimap_uid_esearch_send(session->imap_stream, charset, key, return_options);
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
    return MAILIMAP_ERROR_EXTENSION;

  if (result != NULL) {
    *result = mailimap_esearch_result_new();
    if (*result == NULL)
      return MAILIMAP_ERROR_MEMORY;
  }

  return MAILIMAP_NO_ERROR;
}
