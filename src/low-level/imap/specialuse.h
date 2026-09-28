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

#ifndef SPECIALUSE_H
#define SPECIALUSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libetpan/libetpan-config.h>
#include <libetpan/mailimap_types.h>

enum {
  MAILIMAP_SPECIALUSE_FLAG_ALL = 1 << 0,
  MAILIMAP_SPECIALUSE_FLAG_ARCHIVE = 1 << 1,
  MAILIMAP_SPECIALUSE_FLAG_DRAFTS = 1 << 2,
  MAILIMAP_SPECIALUSE_FLAG_FLAGGED = 1 << 3,
  MAILIMAP_SPECIALUSE_FLAG_JUNK = 1 << 4,
  MAILIMAP_SPECIALUSE_FLAG_SENT = 1 << 5,
  MAILIMAP_SPECIALUSE_FLAG_TRASH = 1 << 6
};

LIBETPAN_EXPORT
int mailimap_has_special_use(mailimap * session);

LIBETPAN_EXPORT
int mailimap_create_special_use(mailimap * session, const char * mb, int use_flags);

LIBETPAN_EXPORT
int mailimap_mailbox_list_has_special_use_flag(struct mailimap_mailbox_list * mb_list, int use_flag);

#ifdef __cplusplus
}
#endif

#endif
