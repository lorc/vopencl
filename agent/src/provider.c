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

#include "provider.h"

bool
provider_create(provider_t *result, int sd, struct sockaddr_in *addr)
{
	provider_t provider;

	if ((result == NULL) || (sd == -1) || (addr == NULL))
	{
		errno = EINVAL;
		return false;
	}
	
	provider = malloc(sizeof(*provider));
	if (provider == NULL)
	{
		errno = ENOMEM;
		return false;
	}

	provider->sd = sd;
	memcpy(&provider->addr, addr, sizeof(provider->addr));

	if (!set_create(&provider->platforms))
	{
		free((void *)provider);
		return false;
	}

	assert(lock_create(&provider->lock));

	*result = provider;
	return true;
}

bool
provider_attach(provider_t provider, agent_t agent)
{
	if ((agent == NULL) || (provider == NULL))
	{
		errno = EINVAL;
		return false;
	}

	if (!set_insert(agent->providers, (global_id_t)PTR2ARG(provider), provider))
	{
		debug_printff("set_insert failed");
		return false;
	}

	provider->agent = agent;

	return true;
}

bool
provider_detach(provider_t provider)
{
	if (provider == NULL)
	{
		errno = EINVAL;
		return false;
	}

	if (provider->agent == NULL)
	{
		debug_printff("provider not attached");
		return false;
	}

	agent_t agent = provider->agent;

	if (agent->opts.load_balancing)
	{
		platform_t platform;

		set_start(provider->platforms);
		while ((platform = set_next(provider->platforms)) != NULL)
		{
			if (!lb_remove_platform(agent->lb, platform))
			{
				debug_printff("lb_remove_platform failed");
			}
		}
		set_finish(provider->platforms);
	}

	if (!set_remove(agent->providers, (global_id_t)PTR2ARG(provider)))
	{
		debug_printff("set_remove failed");
		return false;
	}

	/* keep this reference because it is needed by platform_destroy */
	//provider->agent = NULL;

	return true;
}

bool
provider_destroy(provider_t provider)
{
	platform_t platform;

	if (provider == NULL)
	{
		errno = EINVAL;
		return false;
	}

	clara_printf("Lost provider: %s\n", inet_ntoa(provider->addr.sin_addr));

	assert(lock_destroy(provider->lock));

	/* destroy platforms */
	set_start(provider->platforms);
	while ((platform = set_next(provider->platforms)) != NULL)
		platform_destroy(platform);
	set_finish(provider->platforms);
	assert(set_destroy(provider->platforms));

	memset(provider, 0, sizeof(*provider));
	free((void *)provider);

	return true;
}

