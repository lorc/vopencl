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

cl_int
clGetDeviceIDs(
	cl_platform_id platform,
	cl_device_type device_type,
	cl_uint num_entries,
	cl_device_id *devices,
	cl_uint *num_devices)
{
	INIT_STATUS(CL_SUCCESS);
	DECLARE_HANDLE(devices);
	DECLARE_HANDLE(num_devices);
	DECLARE_NO_HINT;

	if ((num_entries == 0) && (devices != NULL))
		return CL_INVALID_VALUE;

	if ((devices == NULL) && (num_devices == NULL))
		return CL_INVALID_VALUE;

	if (num_devices != NULL)
		*num_devices = 0;

	CALLOC(devices, num_entries, sizeof(cl_device_id));
	CALLOC(num_devices, 1, sizeof(cl_uint));

	EXECUTE(CL_GETDEVICEIDS, platform, device_type, num_entries, HANDLE(devices), HANDLE(num_devices));

	FETCH(devices, num_entries * sizeof(cl_device_id));
	FETCH(num_devices, sizeof(cl_uint));

	PRINT_RETVAL;
	RETURN;
}

cl_int clGetDeviceInfo (
	cl_device_id device,
	cl_device_info param_name,
	size_t param_value_size,
	void *param_value,
	size_t *param_value_size_ret)
{
	INIT_STATUS(CL_SUCCESS);
	DECLARE_HANDLE(param_value);
	DECLARE_HANDLE(param_value_size_ret);
	DECLARE_NO_HINT;
	CALLOC(param_value, 1, param_value_size);
	CALLOC(param_value_size_ret, 1, sizeof(size_t));
	EXECUTE(CL_GETDEVICEINFO, device, param_name, param_value_size,
		HANDLE(param_value), HANDLE(param_value_size_ret));
	FETCH(param_value, param_value_size);
	FETCH(param_value_size_ret, sizeof(size_t));
	RETURN;
}

