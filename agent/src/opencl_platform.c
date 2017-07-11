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
#include "agent_opencl.h"

CLARA_POST_CLFUN_HANDLER(clGetPlatformIDs)
{
	args->argv[0] = CL_SUCCESS;
	cl_uint num_entries = args->argv[1];
	cl_platform_id *platforms = ARG2PTR(args->argv[2]);
	cl_uint *num_platforms = ARG2PTR(args->argv[3]);

	provider_t provider;
	platform_t platform;
	int c;

//	debug_printf("agent_clGetPlatformIDs(%lu, %lx, %lx)\n", (arg_t)num_entries, (arg_t)platforms, (arg_t) num_platforms);

	/* If load-balancing is enabled then reveal only one platform to the consumer.
	 * At first we need to allocate a platform to this consumer. */
	if (args->source->agent->opts.load_balancing)
	{
		if (args->source->platform == NULL)
		{
			if (!lb_allocate_platform(args->source->agent->lb->platforms, args->source))
			{
//				debug_printff("Couldn't allocate platform");
	
				if (num_platforms != NULL)
					*num_platforms = 0;

				return;
			}
		}
		
//		debug_printff("platform: %lx", args->source->platform->global_id);

		if (num_platforms != NULL)	
			*num_platforms = 1;

		if (platforms != NULL)
			platforms[0] = ARG2PTR(args->source->platform->global_id);

		return;
	}

	/* If the reverse proxy is enabled then present the virtual platform. */
	if (args->source->agent->opts.reverse_proxy)
	{
		/* always have one platform, even if there are no providers */
		if (num_platforms != NULL)
			*num_platforms = 1;

		if (platforms != NULL)
			platforms[0] = ARG2PTR(args->source->agent->rp->platform->global_id);

		return;
	}

	if (num_platforms != NULL)
	{
		*num_platforms = 0;

		set_start(args->source->agent->providers);

		while ((provider = set_next(args->source->agent->providers)) != NULL)
		{
			*num_platforms += set_count(provider->platforms);
		}
		set_finish(args->source->agent->providers);
	}

	if (platforms != NULL)
	{
		c = 0;

		set_start(args->source->agent->providers);

		while ((provider = set_next(args->source->agent->providers)) != NULL)
		{
			if (c == num_entries)
				break;

			set_start(provider->platforms);
			while ((platform = set_next(provider->platforms)) != NULL)
			{
				if (c == num_entries)
					break;

				platforms[c++] = ARG2PTR(platform->global_id);
			}
			set_finish(provider->platforms);
		}
		set_finish(args->source->agent->providers);
	}
}

CLARA_POST_CLFUN_HANDLER(clGetPlatformInfo)
{
	args->argv[0] = CL_SUCCESS;
	global_id_t platform_id = args->argv[1];
	cl_platform_info param_name = args->argv[2];
	size_t param_value_size = args->argv[3];
	void *param_value = ARG2PTR(args->argv[4]);
	size_t *param_value_size_ret = ARG2PTR(args->argv[5]);

	platform_t platform = agent_find_platform_by_global_id(args->source->agent, platform_id);
	char *str;

	if (platform == NULL)
	{
		args->argv[0] = CL_INVALID_PLATFORM;
		return;
	}

	switch (param_name)
	{
	case CL_PLATFORM_PROFILE: str = platform->profile; break;
	case CL_PLATFORM_VERSION: str = platform->version; break;
	case CL_PLATFORM_NAME: str = platform->name; break;
	case CL_PLATFORM_VENDOR: str = platform->vendor; break;
	case CL_PLATFORM_EXTENSIONS: str = platform->extensions; break;
	default:
		args->argv[0] = CL_INVALID_VALUE;
		return;
	}

	if (param_value_size_ret != NULL)
		*param_value_size_ret = strlen(str) + 1;

	if (param_value != NULL)
	{
		if (param_value_size < strlen(str) + 1)
		{
			args->argv[0] = CL_INVALID_VALUE;
			return;
		}
		strcpy(param_value, str);
	}
}

