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

#include "binary.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mailimap.h"

LIBETPAN_EXPORT
int mailimap_has_binary(mailimap * session)
{
  return mailimap_has_extension(session, "BINARY");
}

static struct mailimap_section * make_section_part(clist * section_id)
{
  struct mailimap_section_part * sec_part;
  struct mailimap_section_spec * sec_spec;

  sec_part = mailimap_section_part_new(section_id);
  if (sec_part == NULL)
    return NULL;

  sec_spec = mailimap_section_spec_new(MAILIMAP_SECTION_SPEC_SECTION_PART, NULL, sec_part, NULL);
  if (sec_spec == NULL) {
    mailimap_section_part_free(sec_part);
    return NULL;
  }

  return mailimap_section_new(sec_spec);
}

LIBETPAN_EXPORT
struct mailimap_fetch_att *
mailimap_fetch_att_binary_new(clist * section_id, uint32_t offset, uint32_t size)
{
  struct mailimap_section * section;

  section = make_section_part(section_id);
  if (section == NULL)
    return NULL;

  return mailimap_fetch_att_new(MAILIMAP_FETCH_ATT_BODY_SECTION, section, offset, size, strdup("BINARY"));
}

LIBETPAN_EXPORT
struct mailimap_fetch_att *
mailimap_fetch_att_binary_peek_new(clist * section_id, uint32_t offset, uint32_t size)
{
  struct mailimap_section * section;

  section = make_section_part(section_id);
  if (section == NULL)
    return NULL;

  return mailimap_fetch_att_new(MAILIMAP_FETCH_ATT_BODY_PEEK_SECTION, section, offset, size, strdup("BINARY"));
}

LIBETPAN_EXPORT
struct mailimap_fetch_att *
mailimap_fetch_att_binary_size_new(clist * section_id)
{
  struct mailimap_section * section;

  section = make_section_part(section_id);
  if (section == NULL)
    return NULL;

  return mailimap_fetch_att_new(MAILIMAP_FETCH_ATT_EXTENSION, section, 0, 0, strdup("BINARY.SIZE"));
}
