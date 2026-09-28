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

#include "metadata.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mailimap_sender.h"
#include "mailimap.h"

LIBETPAN_EXPORT
struct mailimap_metadata_entry * mailimap_metadata_entry_new(char * name, char * value)
{
  struct mailimap_metadata_entry * entry;

  entry = malloc(sizeof(* entry));
  if (entry == NULL)
    return NULL;

  entry->me_name = name;
  entry->me_value = value;
  return entry;
}

LIBETPAN_EXPORT
void mailimap_metadata_entry_free(struct mailimap_metadata_entry * entry)
{
  if (entry == NULL)
    return;

  free(entry->me_name);
  free(entry->me_value);
  free(entry);
}

LIBETPAN_EXPORT
int mailimap_has_metadata(mailimap * session)
{
  return mailimap_has_extension(session, "METADATA");
}

static int mailimap_getmetadata_send(mailstream * fd, const char * mb, clist * entry_names)
{
  int r;
  clistiter * cur;
  int first = 1;

  r = mailimap_token_send(fd, "GETMETADATA");
  if (r != MAILIMAP_NO_ERROR) return r;

  r = mailimap_space_send(fd);
  if (r != MAILIMAP_NO_ERROR) return r;

  r = mailimap_astring_send(fd, mb);
  if (r != MAILIMAP_NO_ERROR) return r;

  if (entry_names != NULL) {
    r = mailimap_space_send(fd);
    if (r != MAILIMAP_NO_ERROR) return r;

    r = mailimap_token_send(fd, "(");
    if (r != MAILIMAP_NO_ERROR) return r;

    for (cur = clist_begin(entry_names); cur != NULL; cur = clist_next(cur)) {
      char * name = clist_content(cur);
      if (!first) {
        r = mailimap_space_send(fd);
        if (r != MAILIMAP_NO_ERROR) return r;
      }
      r = mailimap_astring_send(fd, name);
      if (r != MAILIMAP_NO_ERROR) return r;
      first = 0;
    }

    r = mailimap_token_send(fd, ")");
    if (r != MAILIMAP_NO_ERROR) return r;
  }

  return MAILIMAP_NO_ERROR;
}

static int mailimap_setmetadata_send(mailstream * fd, const char * mb, clist * entries)
{
  int r;
  clistiter * cur;
  int first = 1;

  r = mailimap_token_send(fd, "SETMETADATA");
  if (r != MAILIMAP_NO_ERROR) return r;

  r = mailimap_space_send(fd);
  if (r != MAILIMAP_NO_ERROR) return r;

  r = mailimap_astring_send(fd, mb);
  if (r != MAILIMAP_NO_ERROR) return r;

  r = mailimap_space_send(fd);
  if (r != MAILIMAP_NO_ERROR) return r;

  r = mailimap_token_send(fd, "(");
  if (r != MAILIMAP_NO_ERROR) return r;

  if (entries != NULL) {
    for (cur = clist_begin(entries); cur != NULL; cur = clist_next(cur)) {
      struct mailimap_metadata_entry * entry = clist_content(cur);
      if (entry == NULL || entry->me_name == NULL)
        continue;

      if (!first) {
        r = mailimap_space_send(fd);
        if (r != MAILIMAP_NO_ERROR) return r;
      }

      r = mailimap_astring_send(fd, entry->me_name);
      if (r != MAILIMAP_NO_ERROR) return r;

      r = mailimap_space_send(fd);
      if (r != MAILIMAP_NO_ERROR) return r;

      if (entry->me_value != NULL)
        r = mailimap_astring_send(fd, entry->me_value);
      else
        r = mailimap_token_send(fd, "NIL");

      if (r != MAILIMAP_NO_ERROR) return r;
      first = 0;
    }
  }

  r = mailimap_token_send(fd, ")");
  if (r != MAILIMAP_NO_ERROR) return r;

  return MAILIMAP_NO_ERROR;
}

LIBETPAN_EXPORT
int mailimap_getmetadata(mailimap * session, const char * mb, clist * entry_names, clist ** result)
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

  r = mailimap_getmetadata_send(session->imap_stream, mb, entry_names);
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

  if (result != NULL)
    *result = NULL;

  error_code = response->rsp_resp_done->rsp_data.rsp_tagged->rsp_cond_state->rsp_type;
  mailimap_response_free(response);

  switch (error_code) {
  case MAILIMAP_RESP_COND_STATE_OK:
    return MAILIMAP_NO_ERROR;

  default:
    return MAILIMAP_ERROR_EXTENSION;
  }
}

LIBETPAN_EXPORT
int mailimap_setmetadata(mailimap * session, const char * mb, clist * entries)
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

  r = mailimap_setmetadata_send(session->imap_stream, mb, entries);
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

  switch (error_code) {
  case MAILIMAP_RESP_COND_STATE_OK:
    return MAILIMAP_NO_ERROR;

  default:
    return MAILIMAP_ERROR_EXTENSION;
  }
}
