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

char *rp_platform_profile		= "FULL_PROFILE";
char *rp_platform_version		= "OpenCL 1.0 clara";
char *rp_platform_name			= "CLara virtual platform";
char *rp_platform_vendor		= "Bjoern Koenig";
char *rp_platform_extensions	= "";

bool
rp_create(revproxy_t *prp, agent_t agent)
{
	revproxy_t rp;
	provider_t provider;

	if ((rp = malloc(sizeof(*rp))) == NULL)
		return false;

	debug_printff("Todo: create dummy provider");
	assert(0);

	if (!platform_create(&rp->platform, provider))
	{
		free(rp);
		return false;
	}

	if (sem_init(rp->lock, 0, 1) != 0)
	{
		platform_destroy(rp->platform);
		free(rp);
		return false;
	}

	clara_malloc_and_strcpy(&rp->platform->profile, rp_platform_profile);
	clara_malloc_and_strcpy(&rp->platform->version, rp_platform_version);
	clara_malloc_and_strcpy(&rp->platform->name, rp_platform_name);
	clara_malloc_and_strcpy(&rp->platform->vendor, rp_platform_vendor);
	clara_malloc_and_strcpy(&rp->platform->extensions, rp_platform_extensions);

	*prp = rp;

	return true;
}

bool
rp_destroy(revproxy_t rp)
{
	sem_t *lock;

	if (rp == NULL)
		return false;

	sem_wait(rp->lock);
	lock = rp->lock;

	platform_destroy(rp->platform);
	free(rp);

	sem_destroy(lock);

	return true;
}

