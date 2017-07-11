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

bool
lb_create(loadbalancer_t *plb)
{
	loadbalancer_t lb;

	if (plb == NULL)
	{
		errno = EINVAL;
		return false;
	}

	if ((lb = malloc(sizeof(*lb))) == NULL)
	{
		errno = ENOMEM;
		return false;
	}

	if (!set_create(&lb->platforms))
	{
		free(lb);
		return false;
	}

	*plb = lb;

	return true;
}

bool
lb_destroy(loadbalancer_t lb)
{
	if (lb == NULL)
	{
		errno = EINVAL;
		return false;
	}

	if (!set_destroy(lb->platforms))
	{
		debug_printff("set_destroy failed");
		return false;
	}

	free(lb);

	return true;
}

bool
lb_add_platform(loadbalancer_t lb, platform_t platform)
{
	debug_printff("adding platform %lx", platform->global_id);
	return set_insert(lb->platforms, platform->global_id, platform);
}

bool
lb_remove_platform(loadbalancer_t lb, platform_t platform)
{
	debug_printff("removing platform %lx", platform->global_id);
	return set_remove(lb->platforms, platform->global_id);
}

bool
lb_allocate_platform(set_t platforms, consumer_t consumer)
{
	bool retval = false;

	platform_t platform;
	set_start(platforms);

	while ((platform = set_next(platforms)) != NULL)
	{
		if (platform->consumer == NULL)
			break;
	}

	if (platform != NULL)
	{
		platform->consumer = consumer;
		consumer->platform = platform;
		retval = true;
//		debug_printff("allocate platform %lx to consumer %s", platform->global_id, inet_ntoa(consumer->addr.sin_addr));
	}

	set_finish(platforms);

	return retval;
}

bool
lb_release_platform(consumer_t consumer)
{
	platform_t platform;

	if (consumer == NULL)
	{
		errno = EINVAL;
		return false;
	}

	platform = consumer->platform;

	if (platform == NULL)
	{
		debug_printff("No platform allocated");
		return false;
	}

	if (platform->consumer != consumer)
	{
		debug_printff("consistency check failed: platform->consumer=%lx != consumer=%lx", platform->consumer, consumer);
		return false;
	}

	consumer->platform = NULL;
	platform->consumer = NULL;
//	debug_printff("release platform %lx from consumer %s", platform->global_id, inet_ntoa(consumer->addr.sin_addr));

	return true;
}

