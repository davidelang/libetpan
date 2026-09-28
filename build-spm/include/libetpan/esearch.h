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

#ifndef ESEARCH_H
#define ESEARCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libetpan/libetpan-config.h>
#include <libetpan/mailimap_types.h>

enum {
  MAILIMAP_ESEARCH_RETURN_MIN = 1 << 0,
  MAILIMAP_ESEARCH_RETURN_MAX = 1 << 1,
  MAILIMAP_ESEARCH_RETURN_ALL = 1 << 2,
  MAILIMAP_ESEARCH_RETURN_COUNT = 1 << 3
};

struct mailimap_esearch_result {
  uint32_t min;
  int has_min;
  uint32_t max;
  int has_max;
  uint32_t count;
  int has_count;
  clist * msg_list;
};

LIBETPAN_EXPORT
struct mailimap_esearch_result * mailimap_esearch_result_new(void);

LIBETPAN_EXPORT
void mailimap_esearch_result_free(struct mailimap_esearch_result * result);

LIBETPAN_EXPORT
int mailimap_has_esearch(mailimap * session);

LIBETPAN_EXPORT
int mailimap_esearch(mailimap * session, const char * charset,
    struct mailimap_search_key * key, int return_options,
    struct mailimap_esearch_result ** result);

LIBETPAN_EXPORT
int mailimap_uid_esearch(mailimap * session, const char * charset,
    struct mailimap_search_key * key, int return_options,
    struct mailimap_esearch_result ** result);

#ifdef __cplusplus
}
#endif

#endif
