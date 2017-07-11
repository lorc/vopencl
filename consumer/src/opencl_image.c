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
clCreateImage2D(
	cl_context context,
	cl_mem_flags flags,
	const cl_image_format *image_format,
	size_t image_width,
	size_t image_height,
	size_t image_row_pitch,
	void *host_ptr,
	cl_int *errcode_ret)
{
	INIT_OBJECT(cl_mem);
	PRINT_NOTSUPP;
	if (errcode_ret != NULL)
		*errcode_ret = CL_INVALID_OPERATION;
	RETURN;
}

cl_mem
clCreateImage3D(
	cl_context context,
	cl_mem_flags flags,
	const cl_image_format *image_format,
	size_t image_width,
	size_t image_height,
	size_t image_depth,
	size_t image_row_pitch,
	size_t image_slice_pitch,
	void *host_ptr,
	cl_int *errcode_ret)
{
	INIT_OBJECT(cl_mem);
	PRINT_NOTSUPP;
	if (errcode_ret != NULL)
		*errcode_ret = CL_INVALID_OPERATION;
	RETURN;
}

cl_int
clGetSupportedImageFormats(
	cl_context context,
	cl_mem_flags flags,
	cl_mem_object_type image_type,
	cl_uint num_entries,
	cl_image_format *image_formats,
	cl_uint *num_image_formats)
{
	INIT_STATUS(CL_INVALID_CONTEXT);
	PRINT_NOTSUPP;
	RETURN;
}

cl_int
clEnqueueReadImage(
	cl_command_queue command_queue,
	cl_mem image,
	cl_bool blocking_read,
	const size_t origin[3],
	const size_t region[3],
	size_t row_pitch,
	size_t slice_pitch,
	void *ptr,
	cl_uint num_events_in_wait_list,
	const cl_event *event_wait_list,
	cl_event *event)
{
	INIT_STATUS(CL_INVALID_OPERATION);
	PRINT_NOTSUPP;
	RETURN;
}

cl_int
clEnqueueWriteImage(
	cl_command_queue command_queue,
	cl_mem image,
	cl_bool blocking_write,
	const size_t origin[3],
	const size_t region[3],
	size_t input_row_pitch,
	size_t input_slice_pitch,
	const void * ptr,
	cl_uint num_events_in_wait_list,
	const cl_event *event_wait_list,
	cl_event *event)
{
	INIT_STATUS(CL_INVALID_OPERATION);
	PRINT_NOTSUPP;
	RETURN;
}

cl_int
clEnqueueCopyImage(
	cl_command_queue command_queue,
	cl_mem src_image,
	cl_mem dst_image,
	const size_t src_origin[3],
	const size_t dst_origin[3],
	const size_t region[3],
	cl_uint num_events_in_wait_list,
	const cl_event *event_wait_list,
	cl_event *event)
{
	INIT_STATUS(CL_INVALID_OPERATION);
	PRINT_NOTSUPP;
	RETURN;
}

cl_int
clEnqueueCopyImageToBuffer(
	cl_command_queue command_queue,
	cl_mem src_image,
	cl_mem dst_buffer,
	const size_t src_origin[3],
	const size_t region[3],
	size_t dst_offset,
	cl_uint num_events_in_wait_list,
	const cl_event *event_wait_list,
	cl_event *event)
{
	INIT_STATUS(CL_INVALID_OPERATION);
	PRINT_NOTSUPP;
	RETURN;
}

cl_int
clEnqueueCopyBufferToImage(
	cl_command_queue command_queue,
	cl_mem src_buffer,
	cl_mem dst_image,
	size_t src_offset,
	const size_t dst_origin[3],
	const size_t region[3],
	cl_uint num_events_in_wait_list,
	const cl_event *event_wait_list,
	cl_event *event)
{
	INIT_STATUS(CL_INVALID_OPERATION);
	PRINT_NOTSUPP;
	RETURN;
}

void *
clEnqueueMapImage(
	cl_command_queue command_queue,
	cl_mem image,
	cl_bool blocking_map,
	cl_map_flags map_flags,
	const size_t origin[3],
	const size_t region[3],
	size_t *image_row_pitch,
	size_t *image_slice_pitch,
	cl_uint num_events_in_wait_list,
	const cl_event *event_wait_list,
	cl_event *event,
	cl_int *errcode_ret)
{
	INIT_OBJECT(void *);
	PRINT_NOTSUPP;
	if (errcode_ret != NULL)
		*errcode_ret = CL_INVALID_OPERATION;
	RETURN;
}

cl_int
clGetImageInfo(
	cl_mem image,
	cl_image_info param_name,
	size_t param_value_size,
	void *param_value,
	size_t *param_value_size_ret)
{
	INIT_STATUS(CL_INVALID_MEM_OBJECT);
	PRINT_NOTSUPP;
	RETURN;
}

