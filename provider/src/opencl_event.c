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

CLARA_CLFUN(clWaitForEvents)
{
	args->argv[0] = (arg_t)clWaitForEvents(
		(cl_uint)args->argv[1],
		(const cl_event *)ARG2PTR(args->argv[2]));
	debug_printff("(%u, %lx) -> %d", args->argv[1],
		args->argv[2], args->argv[0]);
}

CLARA_CLFUN(clGetEventInfo)
{
	cl_event object = ARG2PTR(args->argv[1]);
	cl_event_info param_name = (cl_event_info)args->argv[2];
	size_t param_value_size = (size_t)args->argv[3];
	void *param_value = ARG2PTR(args->argv[4]);
	size_t *param_value_size_ret = ARG2PTR(args->argv[5]);

	args->argv[0] = clGetEventInfo(object, param_name,
		param_value_size, param_value, param_value_size_ret);

	if ((param_name == CL_EVENT_COMMAND_QUEUE)
		&& (args->argv[0] == CL_SUCCESS)
		&& (param_value != NULL))
	{
		mapping_t mapping;
		assert(mapping_find_by_real(provider->id_map, *(cl_command_queue *)param_value, &mapping));
		assert(sizeof(cl_command_queue) == sizeof(mapping->virt));
		memcpy(param_value, &mapping->virt, sizeof(cl_command_queue));
	}

	debug_printff("(object=%lx, param_name=%lx, %u, %lx, %lx) -> %d", object, param_name,
		param_value_size, param_value, param_value_size_ret, args->argv[0]);
}

CLARA_CLFUN(clRetainEvent)
{
	args->argv[0] = (arg_t)clRetainEvent(
		(cl_event)ARG2PTR(args->argv[1]));
	debug_printff("(%lx) -> %d", args->argv[1], args->argv[0]);
}

CLARA_CLFUN(clReleaseEvent)
{
	args->argv[0] = (arg_t)clReleaseEvent(
		(cl_event)ARG2PTR(args->argv[1]));
	debug_printff("(%lx) -> %d", args->argv[1], args->argv[0]);
}

CLARA_CLFUN(clEnqueueMarker)
{
	args->argv[0] = (arg_t)clEnqueueMarker(
		(cl_command_queue)ARG2PTR(args->argv[1]),
		(cl_event *)ARG2PTR(args->argv[2]));
	debug_printff("(%lx, %lx) -> %d", args->argv[1],
		args->argv[2], args->argv[0]);
}

CLARA_CLFUN(clEnqueueWaitForEvents)
{
	args->argv[0] = (arg_t)clEnqueueWaitForEvents(
		(cl_command_queue)ARG2PTR(args->argv[1]),
		(cl_uint)args->argv[2],
		(const cl_event *)ARG2PTR(args->argv[3]));
	debug_printff("(%lx, %u, %lx) -> %d", args->argv[1],
		args->argv[2], args->argv[3], args->argv[0]);
}

CLARA_CLFUN(clEnqueueBarrier)
{
	args->argv[0] = (arg_t)clEnqueueBarrier(
		(cl_command_queue)ARG2PTR(args->argv[1]));
	debug_printff("(%lx) -> %d", args->argv[1], args->argv[0]);
}

DECLARE_GETOBJECTINFO_EX(EventProfiling, event, profiling);

