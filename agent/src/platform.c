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
platform_create(platform_t *ppf, provider_t provider)
{
	platform_t pf;

	if ((pf = malloc(sizeof(*pf))) == NULL)
		return false;

	memset(pf, 0, sizeof(*pf));

	if (!set_create(&pf->devices))
	{
		free(pf);
		return false;
	}

	if (!id_create(provider->agent->id_pool, &pf->global_id))
	{
		if (!set_destroy(pf->devices))
			debug_printff("set_destroy failed");
		free(pf);
		return false;
	}

	pf->provider = provider;

	*ppf = pf;
	return true;
}

bool
platform_destroy(platform_t pf)
{
	device_t device;

	if (pf == NULL)
		return false;

	if (pf->profile != NULL)
		free(pf->profile);

	if (pf->version != NULL)
		free(pf->version);

	if (pf->name != NULL)
		free(pf->name);

	if (pf->vendor != NULL)
		free(pf->vendor);

	if (pf->extensions != NULL)
		free(pf->extensions);

	/* provider_detach keeps the reference */
	if (!id_release(pf->provider->agent->id_pool, (global_id_t)pf->global_id))
	{
		debug_printff("id_release failed");
		return false;
	}

	set_start(pf->devices);
	while ((device = set_next(pf->devices)) != NULL)
	{
		if (!device_destroy(device))
			debug_printff("device_destroy failed");
	}
	set_finish(pf->devices);
	set_destroy(pf->devices);

	memset(pf, 0, sizeof(*pf));
	free(pf);

	return true;
}

bool
platform_init(platform_t platform, const char *ptr)
{
	if ((platform == NULL) || (ptr == NULL))
		return false;

	ptr += clara_malloc_and_strcpy(&platform->profile, ptr);
	ptr += clara_malloc_and_strcpy(&platform->version, ptr);
	ptr += clara_malloc_and_strcpy(&platform->name, ptr);
	ptr += clara_malloc_and_strcpy(&platform->vendor, ptr);
	ptr += clara_malloc_and_strcpy(&platform->extensions, ptr);

	/* save the platform id */
	memcpy(&platform->id, ptr, sizeof(platform->id));

	return platform;
}

platform_t 
agent_find_platform_by_global_id(agent_t agent, global_id_t id)
{
	provider_t provider;
	platform_t platform;

	if (agent->opts.reverse_proxy)
	{
		if (id == agent->rp->platform->global_id)
			return agent->rp->platform;
		else
			return NULL;
	}

	set_start(agent->providers);
	
	while ((provider = set_next(agent->providers)) != NULL)
	{
		set_start(provider->platforms);

		while ((platform = set_next(provider->platforms)) != NULL)
		{
			if (platform->global_id == id)
				break;
		}

		set_finish(provider->platforms);

		if (platform != NULL)
			break;
	}
	set_finish(agent->providers);

	return platform;
}

