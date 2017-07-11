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

cl_program
clCreateProgramWithSource(
	cl_context context,
	cl_uint count,
	const char **strings,
	const size_t *lengths,
	cl_int *errcode_ret)
{
	INIT_OBJECT(cl_program);

	DECLARE_HANDLE(strings);
	handle_t *h_string = NULL;
	DECLARE_HANDLE(lengths);
	DECLARE_HANDLE(errcode_ret);
	
	DECLARE_HINT(context);

	/* build h_strings with handles of strings */
	int i;
	size_t len;
	h_string = (handle_t *)calloc(count, sizeof(handle_t));
	assert(h_string != NULL);
	for (i = 0; i < count; i++)
	{
		len = 0;
		if (lengths != NULL)
			if (lengths[i] != 0)
				len = lengths[i];
		if (len == 0)
			len = strlen(strings[i]) + 1;

		h_string[i] = consumer_allocate(len, sizeof(**strings), (void *)strings[i], HINT);
		assert(h_string[i] != INVALID_HANDLE);
	}
	h_strings = consumer_allocate(count, sizeof(*h_string), h_string, HINT);
	assert(h_strings != INVALID_HANDLE);

	MALLOC(lengths, count * sizeof(*lengths));
	CALLOC(errcode_ret, 1, sizeof(*errcode_ret));

	EXECUTE(CL_CREATEPROGRAMWITHSOURCE, context, count, HANDLE(strings), HANDLE(lengths), HANDLE(errcode_ret));

	FETCH(errcode_ret, sizeof(cl_int));
	FREE(lengths);

	for (i = 0; i < count; i++)
	{
		FREE(string[i]);
	}
	FREE(strings);

	PRINT_RETOBJ;
	RETURN;
}

cl_program
clCreateProgramWithBinary(
	cl_context context,
	cl_uint num_devices,
	const cl_device_id *device_list,
	const size_t *lengths,
	const unsigned char **binaries,
	cl_int *binary_status,
	cl_int *errcode_ret)
{
	INIT_OBJECT(cl_program);
	PRINT_NOTIMPL;
	if (errcode_ret != NULL)
		*errcode_ret = CL_INVALID_CONTEXT;
	RETURN;
}

cl_int
clRetainProgram(cl_program program)
{
	INIT_STATUS(CL_INVALID_PROGRAM);
	DECLARE_HINT(program);
	EXECUTE(CL_RETAINPROGRAM, program);
	PRINT_RETVAL;
	RETURN;
}

cl_int
clReleaseProgram(cl_program program)
{
	INIT_STATUS(CL_INVALID_PROGRAM);
	DECLARE_HINT(program);
	EXECUTE(CL_RELEASEPROGRAM, program);
	PRINT_RETVAL;
	RETURN;
}

cl_int
clBuildProgram(
	cl_program program,
	cl_uint num_devices,
	const cl_device_id *device_list,
	const char *options,
	void (*pfn_notify)(cl_program, void *user_data),
	void *user_data)
{
	INIT_STATUS(0);

	DECLARE_HANDLE(device_list);
	DECLARE_HANDLE(options);
	DECLARE_HANDLE(pfn_notify);
	DECLARE_HANDLE(user_data);

	DECLARE_HINT(program);

	MALLOC(device_list, num_devices * sizeof(*device_list));
	MALLOC(options, (strlen(options) + 1) * sizeof(*options));

	EXECUTE(CL_BUILDPROGRAM, program, num_devices, HANDLE(device_list),
		HANDLE(options), HANDLE(pfn_notify), HANDLE(user_data));

	FREE(device_list);
	FREE(options);

	PRINT_RETVAL;
	RETURN;
}

cl_int
clUnloadCompiler(void)
{
	INIT_STATUS(CL_SUCCESS);
	DECLARE_NO_HINT;

	EXECUTE(CL_UNLOADCOMPILER);

	PRINT_RETVAL;
	RETURN;
}

DECLARE_GETOBJECTINFO(Program, program, CL_GETPROGRAMINFO);

cl_int
clGetProgramBuildInfo(
	cl_program program,
	cl_device_id device,
	cl_program_build_info param_name,
	size_t param_value_size,
	void *param_value,
	size_t *param_value_size_ret)
{
	INIT_STATUS(CL_SUCCESS);

	DECLARE_HANDLE(param_value);
	DECLARE_HANDLE(param_value_size_ret);

	DECLARE_HINT(program);

	CALLOC(param_value, 1, param_value_size);
	CALLOC(param_value_size_ret, 1, sizeof(size_t));

	EXECUTE(CL_GETPROGRAMBUILDINFO, program, device, param_name, param_value_size,
		HANDLE(param_value), HANDLE(param_value_size_ret));

	FETCH(param_value, param_value_size);
	FETCH(param_value_size_ret, sizeof(size_t));

	PRINT_RETVAL;
	RETURN;
}

