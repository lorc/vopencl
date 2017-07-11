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

#include <stdio.h>
#include <CL/cl.h>

#include "consumer.h"

cl_context
clCreateContext(
	const cl_context_properties *properties,
	cl_uint num_devices,
	const cl_device_id *devices,
	void (*pfn_notify)(const char *errinfo,
	const void *private_info, size_t cb,
	void *user_data),
	void *user_data,
	cl_int *errcode_ret)
{
	INIT_OBJECT(cl_context);

	handle_t h_properties, h_devices, h_errcode_ret;

	if (properties == NULL)
	{
		ERRCODE_SET(CL_INVALID_PLATFORM);
		goto clCreateContext_return;
	}

	if ((devices == NULL) || (num_devices == 0))
	{
		ERRCODE_SET(CL_INVALID_VALUE);
		goto clCreateContext_return;
	}

	if ((pfn_notify == NULL) && (user_data != NULL))
	{
		ERRCODE_SET(CL_INVALID_VALUE);
		goto clCreateContext_return;
	}

	cl_platform_id platform_id = 0;

	size_t ctx_prop_size = 0;
	cl_context_properties *ctx_prop;
	for (ctx_prop = (cl_context_properties *)properties; *ctx_prop != 0; ctx_prop += 2)
	{
		switch (*ctx_prop)
		{
		case CL_CONTEXT_PLATFORM:
			platform_id = (cl_platform_id)*(ctx_prop + 1);
			break;
		default:
			ERRCODE_SET(CL_INVALID_VALUE);
			goto clCreateContext_return;
		}

		ctx_prop_size += 2 * sizeof(cl_context_properties);
	}
	ctx_prop_size += sizeof(cl_context_properties);

	if (platform_id == 0)
	{
		ERRCODE_SET(CL_INVALID_PLATFORM);
		goto clCreateContext_return;
	}

	DECLARE_HINT(platform_id);

	h_properties = consumer_allocate(1, ctx_prop_size, (void *)properties, HINT);
	if (h_properties == INVALID_HANDLE)
	{
		ERRCODE_SET(CL_OUT_OF_HOST_MEMORY);
		goto clCreateContext_return;
	}

	h_devices = consumer_allocate(num_devices, sizeof(cl_device_id), (void *)devices, HINT);
	if (h_devices == INVALID_HANDLE)
	{
		ERRCODE_SET(CL_OUT_OF_HOST_MEMORY);
		goto clCreateContext_free_h_properties;
	}

	h_errcode_ret = ERRCODE_INIT();
	if (h_errcode_ret == INVALID_HANDLE)
	{
		ERRCODE_SET(CL_OUT_OF_HOST_MEMORY);
		goto clCreateContext_free_h_devices;
	}

	EXECUTE(CL_CREATECONTEXT, h_properties, num_devices, h_devices, NULL, NULL, h_errcode_ret);

	ERRCODE_FETCH();
	goto clCreateContext_free_h_devices;

	FREE(errcode_ret);
clCreateContext_free_h_devices:
	FREE(devices);
clCreateContext_free_h_properties:
	FREE(properties);
clCreateContext_return:

	PRINT_RETOBJ;
	RETURN;
}

cl_context
clCreateContextFromType(
	const cl_context_properties *properties,
	cl_device_type device_type,
	void (*pfn_notify)(const char *errinfo,
	const void *private_info, size_t cb,
	void *user_data),
	void *user_data,
	cl_int *errcode_ret)
{
	INIT_OBJECT(cl_context);

	DECLARE_HANDLE(properties);

	handle_t h_errcode_ret;

	if (properties == NULL)
	{
		ERRCODE_SET(CL_INVALID_PLATFORM);
		goto clCreateContextFromType_return;
	}

	if ((pfn_notify == NULL) && (user_data != NULL))
	{
		ERRCODE_SET(CL_INVALID_VALUE);
		goto clCreateContextFromType_return;
	}

	cl_platform_id platform_id = 0;

	size_t ctx_prop_size = 0;
	cl_context_properties *ctx_prop;
	for (ctx_prop = (cl_context_properties *)properties; *ctx_prop != 0; ctx_prop += 2)
	{
		switch (*ctx_prop)
		{
		case CL_CONTEXT_PLATFORM:
			platform_id = (cl_platform_id)*(ctx_prop + 1);
			break;
		default:
			ERRCODE_SET(CL_INVALID_VALUE);
			goto clCreateContextFromType_return;
		}

		ctx_prop_size += 2 * sizeof(cl_context_properties);
	}
	ctx_prop_size += sizeof(cl_context_properties);

	if (platform_id == 0)
	{
		ERRCODE_SET(CL_INVALID_PLATFORM);
		goto clCreateContextFromType_return;
	}

	DECLARE_HINT(platform_id);

	h_properties = consumer_allocate(1, ctx_prop_size, (void *)properties, HINT);
	if (h_properties == INVALID_HANDLE)
	{
		ERRCODE_SET(CL_OUT_OF_HOST_MEMORY);
		goto clCreateContextFromType_return;
	}

	h_errcode_ret = ERRCODE_INIT();
	if (h_errcode_ret == INVALID_HANDLE)
	{
		ERRCODE_SET(CL_OUT_OF_HOST_MEMORY);
		goto clCreateContextFromType_free_h_properties;
	}

	EXECUTE(CL_CREATECONTEXTFROMTYPE, HANDLE(properties), device_type, NULL, NULL, HANDLE(errcode_ret));

	ERRCODE_FETCH();
	goto clCreateContextFromType_free_h_properties;

	FREE(errcode_ret);
clCreateContextFromType_free_h_properties:
	FREE(properties);
clCreateContextFromType_return:
	PRINT_RETOBJ;
	RETURN;
}

cl_int
clRetainContext(cl_context context)
{
	INIT_STATUS(CL_INVALID_CONTEXT);
	DECLARE_HINT(context);
	EXECUTE(CL_RETAINCONTEXT, context);
	PRINT_RETVAL;
	RETURN;
}

cl_int
clReleaseContext (cl_context context)
{
	INIT_STATUS(CL_INVALID_CONTEXT);
	DECLARE_HINT(context);
	EXECUTE(CL_RELEASECONTEXT, context);
	PRINT_RETVAL;
	RETURN;
}

cl_int
clGetContextInfo(
	cl_context context,
	cl_context_info param_name,
	size_t param_value_size,
	void *param_value,
	size_t *param_value_size_ret)
{
	INIT_STATUS(CL_INVALID_CONTEXT);
	PRINT_NOTIMPL;
	RETURN;
}

