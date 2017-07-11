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

cl_mem
clCreateBuffer(
	cl_context context,
	cl_mem_flags flags,
	size_t size,
	void *host_ptr,
	cl_int *errcode_ret)
{
	INIT_OBJECT(cl_mem);
	DECLARE_HANDLE(errcode_ret);
	DECLARE_HANDLE(host_ptr);
	DECLARE_HINT(context);

	if (errcode_ret != NULL)
		h_errcode_ret = consumer_allocate(1, sizeof(*errcode_ret), NULL, HINT);

	MALLOC(host_ptr, size);
	EXECUTE(CL_CREATEBUFFER, context, flags, size, HANDLE(host_ptr), HANDLE(errcode_ret));

	if (h_errcode_ret != INVALID_HANDLE)
		consumer_fetch(h_errcode_ret, errcode_ret, sizeof(*errcode_ret), HINT);

	FREE(host_ptr);

	PRINT_RETOBJ;
	RETURN;
}

cl_int
clEnqueueReadBuffer(
	cl_command_queue command_queue,
	cl_mem buffer,
	cl_bool blocking_read,
	size_t offset,
	size_t cb,
	void *ptr,
	cl_uint num_events_in_wait_list,
	const cl_event *event_wait_list,
	cl_event *event)
{
	INIT_STATUS(CL_INVALID_COMMAND_QUEUE);

	DECLARE_HANDLE(ptr);
	DECLARE_HANDLE(event_wait_list);
	DECLARE_HANDLE(event);
	DECLARE_HINT(command_queue);

	CALLOC(ptr, 1, cb);
	MALLOC(event_wait_list, num_events_in_wait_list * sizeof(*event_wait_list));
	CALLOC(event, 1, sizeof(*event));

	EXECUTE(CL_ENQUEUEREADBUFFER,
		command_queue, buffer, blocking_read, offset, cb,
		HANDLE(ptr), num_events_in_wait_list, HANDLE(event_wait_list), HANDLE(event));

	FETCH(ptr, cb);
	FREE(event_wait_list);
	FETCH(event, sizeof(*event));

	PRINT_RETVAL;
	RETURN;
}

cl_int
clEnqueueWriteBuffer(
	cl_command_queue command_queue,
	cl_mem buffer,
	cl_bool blocking_write,
	size_t offset,
	size_t cb,
	const void *ptr,
	cl_uint num_events_in_wait_list,
	const cl_event *event_wait_list,
	cl_event *event)
{
	INIT_STATUS(CL_INVALID_COMMAND_QUEUE);
	DECLARE_HANDLE(ptr);
	DECLARE_HANDLE(event_wait_list);
	DECLARE_HANDLE(event);
	DECLARE_HINT(command_queue);

	CALLOC(ptr, 1, cb);
	MEMPUT(ptr, cb);	
	MALLOC(event_wait_list, num_events_in_wait_list * sizeof(*event_wait_list));
	CALLOC(event, 1, sizeof(*event));

	EXECUTE(CL_ENQUEUEWRITEBUFFER,
		command_queue, buffer, blocking_write, offset, cb,
		HANDLE(ptr), num_events_in_wait_list, HANDLE(event_wait_list), HANDLE(event));

	FREE(ptr);
	FREE(event_wait_list);
	FETCH(event, sizeof(*event));

	PRINT_RETVAL;
	RETURN;
}

cl_int
clEnqueueCopyBuffer(
	cl_command_queue command_queue,
	cl_mem src_buffer,
	cl_mem dst_buffer,
	size_t src_offset,
	size_t dst_offset,
	size_t cb,
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

	EXECUTE(CL_ENQUEUECOPYBUFFER,
		command_queue, src_buffer, dst_buffer, src_offset, dst_offset, cb,
		num_events_in_wait_list, HANDLE(event_wait_list), HANDLE(event));

	FREE(event_wait_list);
	FETCH(event, sizeof(*event));

	PRINT_RETVAL;
	RETURN;
}

cl_int
clRetainMemObject(cl_mem memobj)
{
	INIT_STATUS(CL_INVALID_MEM_OBJECT);
	DECLARE_HINT(memobj);
	EXECUTE(CL_RETAINMEMOBJECT, memobj);
	PRINT_RETVAL;
	RETURN;
}

cl_int
clReleaseMemObject(cl_mem memobj)
{
	INIT_STATUS(CL_INVALID_MEM_OBJECT);
	DECLARE_HINT(memobj);
	EXECUTE(CL_RELEASEMEMOBJECT, memobj);
	PRINT_RETVAL;
	RETURN;
}

void *
clEnqueueMapBuffer(
	cl_command_queue command_queue,
	cl_mem buffer,
	cl_bool blocking_map,
	cl_map_flags map_flags,
	size_t offset,
	size_t cb,
	cl_uint num_events_in_wait_list,
	const cl_event *event_wait_list,
	cl_event *event,
	cl_int *errcode_ret)
{
	PRINT_NOTIMPL;
	if (errcode_ret != NULL)
		*errcode_ret = CL_INVALID_MEM_OBJECT;
	return (NULL);
}

cl_int
clEnqueueUnmapMemObject(
	cl_command_queue command_queue,
	cl_mem memobj,
	void *mapped_ptr,
	cl_uint num_events_in_wait_list,
	const cl_event *event_wait_list,
	cl_event *event)
{
	INIT_STATUS(CL_INVALID_MEM_OBJECT);
	PRINT_NOTIMPL;
	RETURN;
}

DECLARE_GETOBJECTINFO(MemObject, mem, CL_GETMEMOBJECTINFO);

