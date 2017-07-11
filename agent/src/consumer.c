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

#include "consumer.h"
#include "object.h"

bool
consumer_create(consumer_t *result, int sd, struct sockaddr_in *addr)
{
	consumer_t consumer;

	if ((result == NULL) || (sd == -1) || (addr == NULL))
	{
		errno = EINVAL;
		return false;
	}

	consumer = calloc(1, sizeof(*consumer));
	if (consumer == NULL)
	{
		errno = ENOMEM;
		return false;
	}

	consumer->sd = sd;
	memcpy(&consumer->addr, addr, sizeof(consumer->addr));

	assert(set_create(&consumer->objects));
	assert(set_create(&consumer->allocations));

	*result = consumer;
	return true;
}

bool
consumer_attach(consumer_t consumer, agent_t agent)
{
	if ((consumer == NULL) || (agent == NULL))
	{
		errno = EINVAL;
		return false;
	}

	if (!set_insert(agent->consumers, (global_id_t)PTR2ARG(consumer), consumer))
	{
		debug_printff("set_insert failed");
		return false;
	}

	consumer->agent = agent;

	return true;
}

bool
consumer_detach(consumer_t consumer)
{
	if ((consumer == NULL))
	{
		errno = EINVAL;
		return false;
	}

	if (consumer->agent == NULL)
	{
		debug_printff("consumer not attached");
		return false;
	}

	if (consumer->agent->opts.load_balancing)
	{
		if (!lb_release_platform(consumer))
		{
			debug_printff("Couldn't release platform");
		}
	}

	if (!set_remove(consumer->agent->consumers, (global_id_t)PTR2ARG(consumer)))
	{
		debug_printff("set_remove failed");
		return false;
	}

	consumer->agent = NULL;

	return true;
}

bool
consumer_destroy(consumer_t consumer)
{
	struct clara_allocation *allocation;
	uint32_t cnt;

	if (consumer == NULL)
	{
		errno = EINVAL;
		return false;
	}

	if (consumer->agent != NULL)
	{
		debug_printff("consumer is still attached to agent");
		return false;
	}

	clara_printf("Unregister consumer: %s\n", inet_ntoa(consumer->addr.sin_addr));

	cnt = 0;
	set_start(consumer->allocations);
	while ((allocation = set_next(consumer->allocations)) != NULL)
	{
		handle_t old_handle;
		assert(clara_destroy_allocation(allocation, &old_handle));
		assert(id_release(consumer->agent->id_pool, old_handle));
		cnt++;
	}
	set_finish(consumer->allocations);

	if (cnt > 0)
		debug_printff("consumer did not remove %u memory allocation(s)", cnt);

	assert(set_destroy(consumer->allocations));

	object_t object;
	cnt = 0;
	set_start(consumer->objects);
	while ((object = set_next(consumer->objects)) != NULL)
	{
		cnt++;
	}
	set_finish(consumer->objects);

	if (cnt > 0)
		debug_printff("consumer did not release %u OpenCL object(s)", cnt);

	set_destroy(consumer->objects);
	free(consumer);

	return true;
}

