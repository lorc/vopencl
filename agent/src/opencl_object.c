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

#include "agent.h"
#include "object.h"

global_id_t
clCreateObject(provider_t provider, consumer_t consumer, void *local_id, global_id_t wish)
{
	agent_t agent = provider->agent;

	/* if the provider's invokation of the function returns NULL then
	 * the clCreateOBJECT function failed and we don't need to create
	 * a virtual ID.
	 */
	if (local_id == NULL)
		return INVALID_ID;

	global_id_t id;
	if (wish == INVALID_ID)
		assert(id_create(agent->id_pool, &id));
	else
		id = wish;

	object_t obj;
	assert(object_create(&obj, provider, local_id, id));

	/* increase the reference counter */
	assert(object_retain(obj, consumer));

	/* inform the provider about the ID mapping */
	if (wish == INVALID_ID)
		agent_remote_map(provider, local_id, id);

	return id;
}

bool
clRetainObject(consumer_t consumer, global_id_t id)
{
	object_t obj;

	if (!set_find(consumer->agent->objects, id, (void **)&obj))
	{
		debug_printf("clRetainObject: Object doesn't exist: %lx\n", id);
		return false;
	}

	assert(object_retain(obj, consumer));

	return true;
}

bool
clReleaseObject(consumer_t consumer, global_id_t id)
{
	object_t obj;

	if (!set_find(consumer->agent->objects, id, (void **)&obj))
	{
		debug_printf("clReleaseObject: Object doesn't exist: %lx\n", id);
		return false;
	}

	lock_wait(obj->lock);
	object_release(obj);

	if (obj->refcnt == 0)
	{
		agent_t agent = obj->provider->agent;
		assert(id_release(agent->id_pool, id));
		assert(object_destroy(obj));
	}
	else
	{
		lock_post(obj->lock);
	}

	return true;
}

