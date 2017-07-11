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

CLARA_CLFUN(clCreateKernel)
{
	cl_program program = ARG2PTR(args->argv[1]);
	const char *kernel_name = ARG2PTR(args->argv[2]);
	cl_int *errcode_ret = ARG2PTR(args->argv[3]);

	args->argv[0] = PTR2ARG(clCreateKernel(program, kernel_name, errcode_ret));
	debug_printf("clCreateKernel(prog=%lx, kname=%s, err=%d) -> %lx\n",
		program, kernel_name, errcode_ret, args->argv[0]);
}

CLARA_CLFUN(clSetKernelArg)
{
	cl_kernel kernel = ARG2PTR(args->argv[1]);
	cl_uint arg_index = (cl_uint)args->argv[2];
	size_t arg_size = (size_t)args->argv[3];
	const void *arg_value = ARG2PTR(args->argv[4]);

	mapping_t mapping;

	/* only if the size of the argument equals the size
	 * of an ID then this argument could be an ID */
	if (arg_size == sizeof(global_id_t))
	{
		/* assume that the argument is an ID */
		global_id_t id = *(global_id_t *)arg_value;
		/* try to find a mapping of the ID to a real pointer */
		if (mapping_find_by_virt(provider->id_map, id, &mapping))
		{
			assert(sizeof(global_id_t) == sizeof(void *));
			memcpy((void *)arg_value, &mapping->real, sizeof(global_id_t));
			debug_printff("argument is a global ID, %lx, local=%lx", id, *(uint64_t *)arg_value);
		}
		else
		{
			debug_printff("argument is not a global ID, %lx", id);
		}
	}
/*
	int i;
	debug_printf("provider_clSetKernelArg:");
	for (i = 0; i < arg_size; i++)
	{
		debug_printf_raw(" %02x", ((unsigned char *)arg_value)[i]);
	}
	debug_printf_raw("\n");
*/
	args->argv[0] = (arg_t)clSetKernelArg(kernel, arg_index, arg_size, arg_value);
	debug_printff("(kernel=%lx, arg_index=%u, arg_size=%u, arg_value=%lx) -> %d",
		kernel, arg_index, arg_size, arg_value, args->argv[0]);
}

CLARA_CLFUN(clRetainKernel)
{
	args->argv[0] = (arg_t)clRetainKernel(ARG2PTR(args->argv[1]));
	debug_printff("(%lx) -> %d", args->argv[1], args->argv[0]);
}

CLARA_CLFUN(clReleaseKernel)
{
	args->argv[0] = (arg_t)clReleaseKernel(ARG2PTR(args->argv[1]));
	debug_printff("(%lx) -> %d", args->argv[1], args->argv[0]);
}

DECLARE_GETOBJECTINFO(Kernel, kernel);

CLARA_CLFUN(clEnqueueNDRangeKernel)
{
	cl_command_queue command_queue = ARG2PTR(args->argv[1]);
	cl_kernel kernel = ARG2PTR(args->argv[2]);
	cl_uint work_dim = (cl_uint)args->argv[3];
	const size_t *global_work_offset = ARG2PTR(args->argv[4]);
	const size_t *global_work_size = ARG2PTR(args->argv[5]);
	const size_t *local_work_size = ARG2PTR(args->argv[6]);
	cl_uint num_events_in_wait_list = (cl_uint)args->argv[7];
	const cl_event *event_wait_list = ARG2PTR(args->argv[8]);
	cl_event *event = ARG2PTR(args->argv[9]);

	assert(mapping_translate_list_to_real(provider->id_map, num_events_in_wait_list,
		(global_id_t *)event_wait_list, (void **)event_wait_list) == num_events_in_wait_list);

	args->argv[0] = (arg_t)clEnqueueNDRangeKernel(command_queue, kernel, work_dim, global_work_offset,
		global_work_size, local_work_size, num_events_in_wait_list, event_wait_list, event);

	debug_printff("(cq=%lx, kernel=%lx, work_dim=%u, %lx, %lx, %lx, %u, %lx, *event=%lx) -> %d",
		command_queue, kernel, work_dim, global_work_offset, global_work_size, local_work_size,
		num_events_in_wait_list, event_wait_list, event == NULL ? 0 : *event, args->argv[0]);
}

CLARA_CLFUN(clEnqueueTask)
{
	cl_command_queue command_queue = ARG2PTR(args->argv[1]);
	cl_kernel kernel = ARG2PTR(args->argv[2]);
	cl_uint num_events_in_wait_list = (cl_uint)args->argv[3];
	const cl_event *event_wait_list = ARG2PTR(args->argv[4]);
	cl_event *event = ARG2PTR(args->argv[5]);

	assert(mapping_translate_list_to_real(provider->id_map, num_events_in_wait_list,
		(global_id_t *)event_wait_list, (void **)event_wait_list) == num_events_in_wait_list);

	args->argv[0] = (arg_t)clEnqueueTask(command_queue, kernel,
		num_events_in_wait_list, event_wait_list, event);
	
	debug_printff("(cq=%lx, kernel=%lx, %u, %lx, *event=%lx) -> %d",
		command_queue, kernel, num_events_in_wait_list,
		event_wait_list, event == NULL ? 0 : *event, args->argv[0]);
}

