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

CLARA_CLFUN(clCreateBuffer)
{
	cl_context context = ARG2PTR(args->argv[1]);
	cl_mem_flags flags = (cl_mem_flags)args->argv[2];
	size_t size = (size_t)args->argv[3];
	void *host_ptr = ARG2PTR(args->argv[4]);
	cl_int *errcode_ret = ARG2PTR(args->argv[5]);

	args->argv[0] = PTR2ARG(clCreateBuffer(context, flags,
		size, host_ptr, errcode_ret));
	debug_printff("(ctx=%lx, flags=%lx, size=%ld, host_ptr=%lx, err=%d) -> %lx",
		context, flags, size, host_ptr, errcode_ret, args->argv[0]);
}

CLARA_CLFUN(clRetainMemObject)
{
	args->argv[0] = (arg_t)clRetainMemObject(ARG2PTR(args->argv[1]));
	debug_printff("(%lx) -> %d", args->argv[1], args->argv[0]);
}

CLARA_CLFUN(clReleaseMemObject)
{
	args->argv[0] = (arg_t)clReleaseMemObject(ARG2PTR(args->argv[1]));
	debug_printff("(%lx) -> %d", args->argv[1], args->argv[0]);
}

DECLARE_GETOBJECTINFO(MemObject, mem);

CLARA_CLFUN(clEnqueueReadBuffer)
{
	cl_command_queue command_queue = ARG2PTR(args->argv[1]);
	cl_mem buffer = ARG2PTR(args->argv[2]);
	cl_bool blocking_read = (cl_bool)args->argv[3];
	size_t offset = (size_t)args->argv[4];
	size_t cb = (size_t)args->argv[5];
	void *ptr = ARG2PTR(args->argv[6]);
	cl_uint num_events_in_wait_list = (cl_uint)args->argv[7];
	const cl_event *event_wait_list = ARG2PTR(args->argv[8]);
	cl_event *event = ARG2PTR(args->argv[9]);

	assert(mapping_translate_list_to_real(provider->id_map, num_events_in_wait_list,
		(global_id_t *)event_wait_list, (void **)event_wait_list) == num_events_in_wait_list);

	args->argv[0] = (arg_t)clEnqueueReadBuffer(command_queue, buffer, blocking_read,
		offset, cb, ptr, num_events_in_wait_list, event_wait_list, event);

	debug_printff("(cq=%lx, buffer=%lx, blocking=%u, offset=%u, cb=%u, ptr=%lx, %u, %lx, %lx) -> %d",
		command_queue, buffer, blocking_read, offset, cb, ptr,
		num_events_in_wait_list, event_wait_list, event, args->argv[0]);
}

CLARA_CLFUN(clEnqueueWriteBuffer)
{
	cl_command_queue command_queue = ARG2PTR(args->argv[1]);
	cl_mem buffer = ARG2PTR(args->argv[2]);
	cl_bool blocking_write = (cl_bool)args->argv[3];
	size_t offset = (size_t)args->argv[4];
	size_t cb = (size_t)args->argv[5];
	void *ptr = ARG2PTR(args->argv[6]);
	cl_uint num_events_in_wait_list = (cl_uint)args->argv[7];
	const cl_event *event_wait_list = ARG2PTR(args->argv[8]);
	cl_event *event = ARG2PTR(args->argv[9]);

	assert(mapping_translate_list_to_real(provider->id_map, num_events_in_wait_list,
		(global_id_t *)event_wait_list, (void **)event_wait_list) == num_events_in_wait_list);

	args->argv[0] = (arg_t)clEnqueueWriteBuffer(command_queue, buffer, blocking_write,
		offset, cb, ptr, num_events_in_wait_list, event_wait_list, event);

	debug_printff("(cq=%lx, buffer=%lx, blocking=%u, offset=%u, cb=%u, ptr=%lx, %u, %lx, %lx) -> %d",
		command_queue, buffer, blocking_write, offset, cb, ptr,
		num_events_in_wait_list, event_wait_list, event, args->argv[0]);
}

CLARA_CLFUN(clEnqueueCopyBuffer)
{
	cl_command_queue command_queue = ARG2PTR(args->argv[1]);
	cl_mem src_buffer = ARG2PTR(args->argv[2]);
	cl_mem dst_buffer = ARG2PTR(args->argv[3]);
	size_t src_offset = (size_t)args->argv[4];
	size_t dst_offset = (size_t)args->argv[5];
	size_t cb = (size_t)args->argv[6];
	cl_uint num_events_in_wait_list = (cl_uint)args->argv[7];
	const cl_event *event_wait_list = ARG2PTR(args->argv[8]);
	cl_event *event = ARG2PTR(args->argv[9]);

	assert(mapping_translate_list_to_real(provider->id_map, num_events_in_wait_list,
		(global_id_t *)event_wait_list, (void **)event_wait_list) == num_events_in_wait_list);

	args->argv[0] = (arg_t)clEnqueueCopyBuffer(command_queue, src_buffer, dst_buffer,
		src_offset, dst_offset, cb, num_events_in_wait_list, event_wait_list, event);

	debug_printff("(cq=%lx, src=%lx, dst=%lx, src_offset=%lx, dst_offset=%lx, cb=%u, %u, %lx, %lx) -> %d",
		command_queue, src_buffer, dst_buffer, src_offset, dst_offset, cb,
		num_events_in_wait_list, event_wait_list, event, args->argv[0]);
}

