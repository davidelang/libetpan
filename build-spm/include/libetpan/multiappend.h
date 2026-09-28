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

#ifndef MAILIMAP_MULTIAPPEND_H

#define MAILIMAP_MULTIAPPEND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libetpan/mailimap_types.h>

struct mailimap_append_message {
  struct mailimap_flag_list * am_flags;      /* can be NULL */
  struct mailimap_date_time * am_date_time;  /* can be NULL */
  const char * am_message;
  size_t am_size;
};

LIBETPAN_EXPORT
struct mailimap_append_message *
mailimap_append_message_new(struct mailimap_flag_list * flags,
    struct mailimap_date_time * date_time,
    const char * message, size_t size);

LIBETPAN_EXPORT
void mailimap_append_message_free(struct mailimap_append_message * msg);

LIBETPAN_EXPORT
int mailimap_multiappend(mailimap * session, const char * mailbox,
    clist /* struct mailimap_append_message * */ * message_list);

LIBETPAN_EXPORT
int mailimap_has_multiappend(mailimap * session);

#ifdef __cplusplus
}
#endif

#endif
