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
device_create(device_t *pdv, provider_t provider)
{
	device_t dv;

	if ((dv = malloc(sizeof(*dv))) == NULL)
		return false;

	memset(dv, 0, sizeof(*dv));

	assert(id_create(provider->agent->id_pool, &dv->global_id));
	dv->provider = provider;

	*pdv = dv;
	return true;
}

bool
device_destroy(device_t dv)
{
	if (dv == NULL)
		return false;

	if (dv->max_work_item_sizes != NULL)
		free(dv->max_work_item_sizes);

	if (dv->name != NULL)
		free(dv->name);

	if (dv->vendor != NULL)
		free(dv->vendor);

	if (dv->driver_version != NULL)
		free(dv->driver_version);

	if (dv->profile != NULL)
		free(dv->profile);

	if (dv->version != NULL)
		free(dv->version);

	if (dv->extensions != NULL)
		free(dv->extensions);

	assert(id_release(dv->provider->agent->id_pool, (global_id_t)dv->global_id));

	memset(dv, 0, sizeof(*dv));
	free(dv);

	return true;
}

bool
device_init(device_t device, const char *ptr)
{
	if ((device == NULL) || (ptr == NULL))
		return false;

	size_t nel = sizeof(cl_device_type) + sizeof(cl_uint) + sizeof(cl_uint) + sizeof(cl_uint);
	memcpy(&device->type, ptr, nel);
	ptr += nel;

	device->max_work_item_sizes = (size_t *)malloc(sizeof(size_t) * device->max_work_item_dimensions);
	assert(device->max_work_item_sizes != NULL);
	memcpy(device->max_work_item_sizes, ptr, sizeof(size_t) * device->max_work_item_dimensions);
	ptr += sizeof(size_t) * device->max_work_item_dimensions;

	nel = 8 * sizeof(size_t) + 15 * sizeof(cl_uint) + 5 * sizeof(cl_ulong) + 5 * sizeof(cl_bool)
		+ sizeof(cl_device_fp_config) + sizeof(cl_device_mem_cache_type) + sizeof(cl_device_local_mem_type)
		+ sizeof(cl_device_exec_capabilities) + sizeof(cl_command_queue_properties) + sizeof(cl_platform_id);

	memcpy(&device->max_work_group_size, ptr, nel);
	ptr += nel;

	
	ptr += clara_malloc_and_strcpy(&device->name, ptr);
	ptr += clara_malloc_and_strcpy(&device->vendor, ptr);
	ptr += clara_malloc_and_strcpy(&device->driver_version, ptr);
	ptr += clara_malloc_and_strcpy(&device->profile, ptr);
	ptr += clara_malloc_and_strcpy(&device->version, ptr);
	ptr += clara_malloc_and_strcpy(&device->extensions, ptr);

	/* save the device id */
	memcpy(&device->id, ptr, sizeof(cl_device_id));

	return true;
}

device_t
agent_find_device_by_global_id(agent_t agent, global_id_t id)
{
	provider_t provider;
	platform_t platform;
	device_t device;

	if (agent->opts.reverse_proxy)
	{
		platform = agent->rp->platform;
		set_start(platform->devices);
		while ((device = set_next(platform->devices)) != NULL)
		{
			if (device->global_id == id)
				break;
		}
		set_finish(platform->devices);
		return device;
	}

	set_start(agent->providers);

	while ((provider = set_next(agent->providers)) != NULL)
	{
		set_start(provider->platforms);

		while ((platform = set_next(provider->platforms)) != NULL)
		{
			set_start(platform->devices);

			while ((device = set_next(platform->devices)) != NULL)
			{
				if (device->global_id == id)
				{
					set_finish(platform->devices);
					set_finish(provider->platforms);
					set_finish(agent->providers);
					return device;
				}
			}
			set_finish(platform->devices);
		}
		set_finish(provider->platforms);
	}
	set_finish(agent->providers);

	return NULL;
}

cl_bool
agent_device_is_available(device_t device)
{
	cl_bool result = false;
	int sd;
	struct sockaddr_in *to;

	if (!device->available)
		return result;

	/* ask the provider */
	sd = device->provider->sd;
	to = &device->provider->addr;

	arg_t argv[5];

	cl_int retval;
	argv[0] = (arg_t)device->global_id;
	argv[1] = CL_DEVICE_AVAILABLE;
	argv[2] = sizeof(result);
	argv[4] = 0;

	struct fetch_response fetres_p;

	provider_lock(device->provider);

	struct allocate_request allreq_p = {
		.nmemb = 1,
		.size = sizeof(result),
		.hint = INVALID_HANDLE,
	};

	assert(id_create(device->provider->agent->id_pool, &allreq_p.handle));

	if (!agent_remote_allocate(device->provider, &allreq_p, 0, &argv[3]))
	{
		debug_printff("agent_remote_allocate failed");
	}
	else if (!clara_send_execute_request(sd, CL_GETDEVICEINFO, argv, 5, INVALID_HANDLE, to))
	{
		debug_printff("clara_send_execute_request failed");
	}
	else if (!clara_recv_execute_response(sd, &retval, sizeof(retval)))
	{
		debug_printff("clara_recv_execute_response failed");
	}
	else if (!clara_send_fetch_request(sd, argv[3], argv[2], 0, 0, to))
	{
		debug_printff("clara_send_fetch_request failed");
	}
	else if (!clara_recv_fetch_response(sd, &fetres_p, &result, sizeof(result)))
	{
		debug_printff("clara_recv_fetch_response failed");
	}
	else
	{
		if (retval != CL_SUCCESS)
			result = false;
//		debug_printff("device is%s available", result ? "" : " not");
	}

	provider_unlock(device->provider);

	return result;
}

