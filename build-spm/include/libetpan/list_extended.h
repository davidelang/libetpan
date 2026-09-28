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

#ifndef LIST_EXTENDED_H
#define LIST_EXTENDED_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libetpan/libetpan-config.h>
#include <libetpan/mailimap_types.h>

enum {
  MAILIMAP_LIST_EXTENDED_SELECT_SUBSCRIBED = 1 << 0,
  MAILIMAP_LIST_EXTENDED_SELECT_REMOTE = 1 << 1,
  MAILIMAP_LIST_EXTENDED_SELECT_RECURSIVEMATCH = 1 << 2,
  MAILIMAP_LIST_EXTENDED_SELECT_SPECIAL_USE = 1 << 3
};

enum {
  MAILIMAP_LIST_EXTENDED_RETURN_SUBSCRIBED = 1 << 0,
  MAILIMAP_LIST_EXTENDED_RETURN_CHILDREN = 1 << 1,
  MAILIMAP_LIST_EXTENDED_RETURN_SPECIAL_USE = 1 << 2
};

LIBETPAN_EXPORT
int mailimap_has_list_extended(mailimap * session);

LIBETPAN_EXPORT
int mailimap_list_extended(mailimap * session, int selection_options,
    const char * mb, const char * list_mb, int return_options,
    clist ** result);

#ifdef __cplusplus
}
#endif

#endif
