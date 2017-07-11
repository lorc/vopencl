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

CLARA_CLFUN(clCreateProgramWithSource)
{
	cl_context context = ARG2PTR(args->argv[1]);
	cl_uint count = (cl_uint)args->argv[2];
	const char **strings = ARG2PTR(args->argv[3]);
	const size_t *lengths = ARG2PTR(args->argv[4]);

	DECLARE_ERRCODE(5);

	/* convert handles in strings to pointers */
	assert(mapping_translate_list_to_real(provider->id_map, count,
		(global_id_t *)strings, (void **)strings) == count);

	args->argv[0] = PTR2ARG(clCreateProgramWithSource(context,
		count, strings, lengths, &ERRCODE));

	debug_printf("clCreateProgramWithSource(ctx=%lx, count=%u, strings=%lx, lengths=%lx, err=%d) -> %lx\n",
		context, count, strings, lengths, ERRCODE, args->argv[0]);
	
	/* print program */
	int i;
	size_t len = 0;
	char *strbuf;
	for (i = 0; i < count; i++)
	{
		len = 0;
		if (lengths != NULL)
			len = lengths[i];
		if (len == 0)
			len = strlen(strings[i]);

		strbuf = malloc(len + 1);
		memcpy(strbuf, strings[i], len);
		strbuf[len] = '\0';

		debug_printf("%4u: %s", i+1, strbuf);
	}

	COPY_ERRCODE();
}

CLARA_CLFUN(clCreateProgramWithBinary)
{
	cl_context context = ARG2PTR(args->argv[1]);
	cl_uint num_devices = (cl_uint)args->argv[2];
	const cl_device_id *device_list = ARG2PTR(args->argv[3]);
	const size_t *lengths = ARG2PTR(args->argv[4]);
	const unsigned char **binaries = ARG2PTR(args->argv[5]);
	cl_int *binary_status = ARG2PTR(args->argv[6]);
	cl_int *errcode_ret = ARG2PTR(args->argv[7]);

	assert(0);
	/* convert device_list and binaries */

	args->argv[0] = PTR2ARG(clCreateProgramWithBinary(context, num_devices,
		device_list, lengths, binaries, binary_status, errcode_ret));
	debug_printf("provider_clCreateProgramWithSource(context=%016lx, num_devices=%u, device_list=%016lx, lengths=%016lx, "
		"binaries=%016lx, binary_status=%016lx, err=%016lx) -> %08x\n",
		context, num_devices, device_list, lengths, binaries, binary_status, errcode_ret, args->argv[0]);
}

CLARA_CLFUN(clRetainProgram)
{
	args->argv[0] = (arg_t)clRetainProgram(ARG2PTR(args->argv[1]));
	debug_printff("(%lx) -> %d", args->argv[1], args->argv[0]);
}

CLARA_CLFUN(clReleaseProgram)
{
	args->argv[0] = (arg_t)clReleaseProgram(ARG2PTR(args->argv[1]));
	debug_printff("(%lx) -> %d", args->argv[1], args->argv[0]);
}

CLARA_CLFUN(clBuildProgram)
{
	cl_program program = ARG2PTR(args->argv[1]);
	cl_uint num_devices = (cl_uint)args->argv[2];
	cl_device_id *device_list = ARG2PTR(args->argv[3]);
	const char *options = ARG2PTR(args->argv[4]);
	/* skip pfn_notify and user_data */
	assert(args->argv[5] == 0);
	assert(args->argv[6] == 0);

	/* convert device virtual IDs in devices to real IDs */
	assert(mapping_translate_list_to_real(provider->id_map, num_devices,
		(global_id_t *)device_list, (void **)device_list) == num_devices);

	args->argv[0] = clBuildProgram(program, num_devices, device_list, options, NULL, NULL);
	debug_printf("clBuildProgram(prog=%lx, n=%u, devlist=%lx, opts=%lx, NULL, NULL) -> %d\n",
		program, num_devices, device_list, options, args->argv[0]);
 }

CLARA_CLFUN(clUnloadCompiler)
{
	args->argv[0] = (arg_t)clUnloadCompiler();
	debug_printf("provider_clUnloadCompiler() -> %08x\n", args->argv[0]);
}

DECLARE_GETOBJECTINFO(Program, program);

CLARA_CLFUN(clGetProgramBuildInfo)
{
	cl_program program = ARG2PTR(args->argv[1]);
	cl_device_id device = ARG2PTR(args->argv[2]);
	cl_program_build_info param_name = (cl_program_build_info)args->argv[3];
	size_t param_value_size = (size_t)args->argv[4];
	void *param_value = ARG2PTR(args->argv[5]);
	size_t *param_value_size_ret = ARG2PTR(args->argv[6]);

	args->argv[0] = clGetProgramBuildInfo(program, device, param_name, param_value_size, param_value, param_value_size_ret);
	debug_printff("(program=%016lx, device=%016lx, param_name=%08x, param_value_size=%u, param_value=%016lx, param_value_size_ret=%016lx) -> %08x",
		PTR2ARG(program), PTR2ARG(device), param_name, param_value_size,
		PTR2ARG(param_value), PTR2ARG(param_value_size_ret), args->argv[0]);
}

