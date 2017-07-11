/*-
 * Copyright 2009, 2010 Bjoern Koenig
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
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "clara.h"
#include "set.h"

bool
mapping_create(mapping_t *result, void *real, global_id_t virt)
{
	mapping_t mapping;

	mapping = (mapping_t )calloc(1, sizeof(*mapping));

	if (mapping == NULL)
		return false;

	mapping->virt = virt;
	mapping->real = real;

	*result = mapping;
	return true;
}

bool
mapping_destroy(mapping_t mapping)
{
	if (mapping == NULL)
		return false;
	free((void *) mapping);
	return true;
}

bool
mapping_find_by_virt(set_t map, global_id_t virt, mapping_t *result)
{
	bool retval = false;
	mapping_t mapping;

	if (set_find(map, virt, (void **)&mapping))
	{
		*result = mapping;
		retval = true;
	}
	return retval;
}

bool
mapping_find_by_real(set_t map, void *real, mapping_t *result)
{
	bool retval = false;
	mapping_t mapping;

	set_start(map);
	while ((mapping = set_next(map)) != NULL)
	{
		if (mapping->real == real)
		{
			*result = mapping;
			retval = true;
			break;
		}
	}
	set_finish(map);

	return retval;
}

unsigned int
mapping_translate_list_to_real(set_t map, unsigned int num_items, global_id_t *virt_list, void **real_list)
{
	unsigned int i;
	mapping_t mapping;

	if ((num_items == 0) || (virt_list == NULL))
		return 0;

	global_id_t *virt_ptr = virt_list;
	void **real_ptr = real_list;

	for (i = 0; i < num_items; i++)
	{
		if (!mapping_find_by_virt(map, *virt_ptr++, &mapping))
		{
			debug_printff("Can't translate item list completely. Failed item: #%d", i);
			break;
		}
#if 0
		else
		{
			debug_printff("%lx <--> %lx", mapping->real, mapping->virt);
		}
#endif
		*real_ptr++ = mapping->real;
	}

	return i;
}

