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

#include <stdlib.h>
#include <CL/cl.h>

cl_platform_id *
clara_GetPlatformIDs(void)
{
	cl_int status;

	cl_platform_id *platforms = NULL;
	cl_uint num_platforms = 0;

	if ((status = clGetPlatformIDs(0, NULL, &num_platforms)) != CL_SUCCESS)
		return NULL;

	if (num_platforms == 0)
		return NULL;

	if ((platforms = (cl_platform_id *)calloc(num_platforms + 1, sizeof(cl_platform_id))) == NULL)
		return NULL;

	if ((status = clGetPlatformIDs(num_platforms, platforms, NULL)) != CL_SUCCESS)
	{
		free((void *)platforms);
		return NULL;
	}

	return platforms;
}

void *
clara_GetPlatformInfo(cl_platform_id platform, cl_platform_info param_name)
{
	cl_int status;

	void *param_value = NULL;
	size_t param_value_size = 0;

	if ((status = clGetPlatformInfo(platform, param_name, 0, NULL, &param_value_size)) != CL_SUCCESS)
		return NULL;

	if ((param_value = (void *)malloc(param_value_size)) == NULL)
		return NULL;

	if ((status = clGetPlatformInfo(platform, param_name, param_value_size, param_value, NULL)) != CL_SUCCESS)
	{
		free(param_value);
		return NULL;
	}

	return param_value;
}

cl_device_id *
clara_GetDeviceIDs(cl_platform_id platform, cl_device_type type)
{
	cl_int status;

	cl_device_id *devices = NULL;
	cl_uint num_devices = 0;

	if ((status = clGetDeviceIDs(platform, type, 0, NULL, &num_devices)) != CL_SUCCESS)
		return NULL;

	if (num_devices == 0)
		return NULL;

	if ((devices = (cl_device_id *)calloc(num_devices + 1, sizeof(cl_device_id))) == NULL)
		return NULL;

	if ((status = clGetDeviceIDs(platform, type, num_devices, devices, NULL)) != CL_SUCCESS)
	{
		free((void *)devices);
		return NULL;
	}

	return devices;
}

void *
clara_GetDeviceInfo(cl_device_id device, cl_device_info param_name)
{
	cl_int status;

	void *param_value = NULL;
	size_t param_value_size = 0;

	if ((status = clGetDeviceInfo(device, param_name, 0, NULL, &param_value_size)) != CL_SUCCESS)
		return NULL;

	if ((param_value = (void *)malloc(param_value_size)) == NULL)
		return NULL;

	if ((status = clGetDeviceInfo(device, param_name, param_value_size, param_value, NULL)) != CL_SUCCESS)
	{
		free(param_value);
		return NULL;
	}

	return param_value;
}

