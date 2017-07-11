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

cl_kernel
clCreateKernel(cl_program program, const char *kernel_name, cl_int *errcode_ret)
{
	INIT_OBJECT(cl_kernel);
	DECLARE_HANDLE(kernel_name);
	DECLARE_HANDLE(errcode_ret);
	DECLARE_HINT(program);

	MALLOC(kernel_name, (strlen(kernel_name) + 1) * sizeof(*kernel_name));
	CALLOC(errcode_ret, 1, sizeof(*errcode_ret));

	EXECUTE(CL_CREATEKERNEL, program, HANDLE(kernel_name), HANDLE(errcode_ret));

	FREE(kernel_name);
	FETCH(errcode_ret, sizeof(*errcode_ret));

	PRINT_RETOBJ;
	RETURN;
}

cl_int
clCreateKernelsInProgram(cl_program program, cl_uint num_kernels, cl_kernel *kernels, cl_uint *num_kernels_ret)
{
	INIT_STATUS(CL_INVALID_PROGRAM);
	PRINT_NOTIMPL;
	RETURN;
}

cl_int
clRetainKernel(cl_kernel kernel)
{
	INIT_STATUS(CL_INVALID_KERNEL);
	DECLARE_HINT(kernel);
	EXECUTE(CL_RETAINKERNEL, kernel);
	PRINT_RETVAL;
	RETURN;
}

cl_int
clReleaseKernel(cl_kernel kernel)
{
	INIT_STATUS(CL_INVALID_KERNEL);
	DECLARE_HINT(kernel);
	EXECUTE(CL_RELEASEKERNEL, kernel);
	PRINT_RETVAL;
	RETURN;
}

cl_int
clSetKernelArg(cl_kernel kernel, cl_uint arg_index, size_t arg_size, const void *arg_value)
{
	INIT_STATUS(CL_INVALID_KERNEL);
	DECLARE_HANDLE(arg_value);
	DECLARE_HINT(kernel);
	MALLOC(arg_value, arg_size);
	EXECUTE(CL_SETKERNELARG, kernel, arg_index, arg_size, HANDLE(arg_value));
	FREE(arg_value);
	PRINT_RETVAL;
	RETURN;
}

DECLARE_GETOBJECTINFO(Kernel, kernel, CL_GETKERNELINFO);

cl_int
clGetKernelWorkGroupInfo(
	cl_kernel kernel,
	cl_device_id device,
	cl_kernel_work_group_info param_name,
	size_t param_value_size,
	void *param_value,
	size_t *param_value_size_ret)
{
	
	INIT_STATUS(CL_INVALID_KERNEL);
	PRINT_NOTIMPL;
	RETURN;
}

cl_int
clEnqueueNDRangeKernel(
	cl_command_queue command_queue,
	cl_kernel kernel,
	cl_uint work_dim,
	const size_t *global_work_offset,
	const size_t *global_work_size,
	const size_t *local_work_size,
	cl_uint num_events_in_wait_list,
	const cl_event *event_wait_list,
	cl_event *event)
{
	INIT_STATUS(CL_INVALID_COMMAND_QUEUE);

	DECLARE_HANDLE(global_work_offset);
	DECLARE_HANDLE(global_work_size);
	DECLARE_HANDLE(local_work_size);
	DECLARE_HANDLE(event_wait_list);
	DECLARE_HANDLE(event);
	DECLARE_HINT(command_queue);

	MALLOC(global_work_offset, work_dim * sizeof(*global_work_offset));
	MALLOC(global_work_size, work_dim * sizeof(*global_work_size));
	MALLOC(local_work_size, work_dim * sizeof(*local_work_size));
	MALLOC(event_wait_list, num_events_in_wait_list * sizeof(*event_wait_list));
	CALLOC(event, 1, sizeof(*event));

	EXECUTE(CL_ENQUEUENDRANGEKERNEL,
		command_queue, kernel, work_dim, HANDLE(global_work_offset), HANDLE(global_work_size),
		HANDLE(local_work_size), num_events_in_wait_list, HANDLE(event_wait_list), HANDLE(event));

	FREE(global_work_offset);
	FREE(global_work_size);
	FREE(local_work_size);
	FREE(event_wait_list);
	FETCH(event, sizeof(*event));

	PRINT_RETVAL;
	RETURN;
}

cl_int
clEnqueueTask(
	cl_command_queue command_queue,
	cl_kernel kernel,
	cl_uint num_events_in_wait_list,
	const cl_event *event_wait_list,
	cl_event *event)
{
	INIT_STATUS(CL_INVALID_COMMAND_QUEUE);
	DECLARE_HANDLE(event_wait_list);
	DECLARE_HANDLE(event);
	DECLARE_HINT(command_queue);

	MALLOC(event_wait_list, num_events_in_wait_list * sizeof(*event_wait_list));
	CALLOC(event, 1, sizeof(*event));

	EXECUTE(CL_ENQUEUETASK, command_queue, kernel,
		num_events_in_wait_list, HANDLE(event_wait_list), HANDLE(event));

	FREE(event_wait_list);
	FETCH(event, sizeof(*event));

	RETURN;
}

cl_int
clEnqueueNativeKernel(
	cl_command_queue command_queue,
	void (*user_func)(void *),
	void *args,
	size_t cb_args,
	cl_uint num_mem_objects,
	const cl_mem *mem_list,
	const void **args_mem_loc,
	cl_uint num_events_in_wait_list,
	const cl_event *event_wait_list,
	cl_event *event)
{
	INIT_STATUS(CL_INVALID_COMMAND_QUEUE);
	PRINT_NOTSUPP;
	RETURN;
}

