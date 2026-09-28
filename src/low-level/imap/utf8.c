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

#include "utf8.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "enable.h"
#include "mailimap.h"

LIBETPAN_EXPORT
int mailimap_has_utf8_accept(mailimap * session)
{
  return mailimap_has_extension(session, "UTF8=ACCEPT");
}

LIBETPAN_EXPORT
int mailimap_enable_utf8_accept(mailimap * session)
{
  struct mailimap_capability * cap;
  clist * list;
  struct mailimap_capability_data * caps;
  struct mailimap_capability_data * enabled_res = NULL;
  int r;

  cap = mailimap_capability_new(MAILIMAP_CAPABILITY_NAME, NULL, strdup("UTF8=ACCEPT"));
  if (cap == NULL)
    return MAILIMAP_ERROR_MEMORY;

  list = clist_new();
  if (list == NULL) {
    mailimap_capability_free(cap);
    return MAILIMAP_ERROR_MEMORY;
  }

  r = clist_append(list, cap);
  if (r < 0) {
    mailimap_capability_free(cap);
    clist_free(list);
    return MAILIMAP_ERROR_MEMORY;
  }

  caps = mailimap_capability_data_new(list);
  if (caps == NULL) {
    mailimap_capability_free(cap);
    clist_free(list);
    return MAILIMAP_ERROR_MEMORY;
  }

  r = mailimap_enable(session, caps, &enabled_res);
  mailimap_capability_data_free(caps);

  if (r == MAILIMAP_NO_ERROR && enabled_res != NULL)
    mailimap_capability_data_free(enabled_res);

  return r;
}
