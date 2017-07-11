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
#include "provider_opencl.h"

CLARA_CLFUN(clCreateContext)
{
	cl_context_properties *properties = ARG2PTR(args->argv[1]);
	cl_uint num_devices = args->argv[2];
	cl_device_id *devices = ARG2PTR(args->argv[3]);
	/* skip pfn_notify and user_data */

	DECLARE_ERRCODE(6);

	cl_platform_id platform_id = NULL;
	cl_context_properties *ctx_prop;

	ctx_prop = (cl_context_properties *)properties;
	while (*ctx_prop != 0)
	{
		switch (*ctx_prop)
		{
		case CL_CONTEXT_PLATFORM:
			platform_id = (cl_platform_id)*(ctx_prop + 1);
			break;
		default:
			break;
		}
		ctx_prop += 2;
	}

	/* lookup local platform id */
	platform_t platform = provider_find_platform(PTR2ARG(platform_id));
	assert(platform != NULL);

	ctx_prop = (cl_context_properties *)properties;
	while (*ctx_prop != 0)
	{
		switch (*ctx_prop)
		{
		case CL_CONTEXT_PLATFORM:
			*(ctx_prop + 1) = (cl_context_properties)platform->id;
			break;
		default:
			break;
		}
		ctx_prop += 2;
	}

	int i;
	device_t device;
	for (i = 0; i < num_devices; i++)
	{
		device = provider_find_device(platform, (global_id_t)PTR2ARG(devices[i]));
		devices[i] = device->id;
	}

	args->argv[0] = PTR2ARG(clCreateContext(properties,
		num_devices, devices, NULL, NULL, &ERRCODE));

	debug_printff("(prop=%lx, n=%u, devs=%lx, 0, 0, err=%d) -> %lx",
		properties, num_devices, devices, ERRCODE, args->argv[0]);

	COPY_ERRCODE();
}
 
CLARA_CLFUN(clCreateContextFromType)
{
	cl_context_properties *properties = ARG2PTR(args->argv[1]);
	cl_device_type device_type = (cl_device_type)args->argv[2];
	/* skip pfn_notify and user_data (argv[3], argv[4]) */
	DECLARE_ERRCODE(5);

	cl_platform_id platform_id = NULL;
	cl_context_properties *ctx_prop;

	ctx_prop = (cl_context_properties *)properties;
	while (*ctx_prop != 0)
	{
		switch (*ctx_prop)
		{
		case CL_CONTEXT_PLATFORM:
			platform_id = (cl_platform_id)*(ctx_prop + 1);
			break;
		default:
			break;
		}
		ctx_prop += 2;
	}

	/* lookup local platform id */
	platform_t platform = provider_find_platform(PTR2ARG(platform_id));
	assert(platform != NULL);

	ctx_prop = (cl_context_properties *)properties;
	while (*ctx_prop != 0)
	{
		switch (*ctx_prop)
		{
		case CL_CONTEXT_PLATFORM:
			*(ctx_prop + 1) = (cl_context_properties)platform->id;
			break;
		default:
			break;
		}
		ctx_prop += 2;
	}

	args->argv[0] = PTR2ARG(clCreateContextFromType(properties,
		device_type, NULL, NULL, &ERRCODE));

	debug_printff("(%016lx, %u, ..., ..., err=%d) -> %lx",
		properties, device_type, ERRCODE, args->argv[0]);

	COPY_ERRCODE();
}

CLARA_CLFUN(clRetainContext)
{
	args->argv[0] = (arg_t)clRetainContext(ARG2PTR(args->argv[1]));
	debug_printff("(%lx) -> %d", args->argv[1], args->argv[0]);
}

CLARA_CLFUN(clReleaseContext)
{
	args->argv[0] = (arg_t)clReleaseContext(ARG2PTR(args->argv[1]));
	debug_printff("(%lx) -> %d", args->argv[1], args->argv[0]);
}

