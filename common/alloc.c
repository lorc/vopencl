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

struct clara_allocation *
clara_create_allocation(allocate_request_t params)
{
	struct clara_allocation *allocation;

	allocation = (struct clara_allocation *)calloc(1, sizeof(*allocation));
	assert(allocation != NULL);

	allocation->handle = params->handle;
	allocation->nmemb = params->nmemb;
	allocation->size = params->size;
	allocation->ptr = (void *)calloc(allocation->nmemb, allocation->size);

	return allocation;
}

struct clara_allocation *
clara_find_allocation(set_t allocations, handle_t handle)
{
	struct clara_allocation *allocation;

	set_start(allocations);

	while ((allocation = set_next(allocations)) != NULL)
	{
		if (allocation->handle == handle)
			break;
	}

	set_finish(allocations);

	return allocation;
}

bool
clara_destroy_allocation(struct clara_allocation *allocation, handle_t *old_handle)
{
	handle_t handle;

	if (allocation == NULL)
		return false;

	handle = allocation->handle;
	free((void *)allocation->ptr);
	free((void *)allocation);

	if (old_handle != NULL)
		*old_handle = handle;
	return true;
}

