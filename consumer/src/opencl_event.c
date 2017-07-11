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
clWaitForEvents(cl_uint num_events, const cl_event *event_list)
{
	INIT_STATUS(CL_INVALID_VALUE);
	if (num_events == 0) RETURN;
	DECLARE_HANDLE(event_list);
	DECLARE_HINT(event_list[0]);
	MALLOC(event_list, num_events * sizeof(*event_list));
	EXECUTE(CL_WAITFOREVENTS, num_events, HANDLE(event_list));
	FREE(event_list);
	RETURN;
}

DECLARE_GETOBJECTINFO(Event, event, CL_GETEVENTINFO);

cl_int
clRetainEvent(cl_event event)
{
	INIT_STATUS(CL_INVALID_EVENT);
	DECLARE_HINT(event);
	EXECUTE(CL_RETAINEVENT, event);
	PRINT_RETVAL;
	RETURN;
}

cl_int
clReleaseEvent(cl_event event)
{
	INIT_STATUS(CL_INVALID_EVENT);
	DECLARE_HINT(event);
	EXECUTE(CL_RELEASEEVENT, event);
	PRINT_RETVAL;
	RETURN;
}

cl_int
clEnqueueMarker(cl_command_queue command_queue, cl_event *event)
{
	INIT_STATUS(CL_INVALID_COMMAND_QUEUE);
	DECLARE_HANDLE(event);
	DECLARE_HINT(command_queue);
	CALLOC(event, 1, sizeof(cl_event));
	EXECUTE(CL_ENQUEUEMARKER, command_queue, HANDLE(event));
	FETCH(event, sizeof(cl_event));
	RETURN;
}

cl_int
clEnqueueWaitForEvents(cl_command_queue command_queue, cl_uint num_events, const cl_event *event_list)
{
	INIT_STATUS(CL_INVALID_COMMAND_QUEUE);
	DECLARE_HANDLE(event_list);
	DECLARE_HINT(command_queue);
	MALLOC(event_list, num_events * sizeof(*event_list));
	EXECUTE(CL_ENQUEUEWAITFOREVENTS, command_queue, num_events, HANDLE(event_list));
	FREE(event_list);
	RETURN;
}

cl_int
clEnqueueBarrier(cl_command_queue command_queue)
{
	INIT_STATUS(CL_INVALID_COMMAND_QUEUE);
	DECLARE_HINT(command_queue);
	EXECUTE(CL_ENQUEUEBARRIER, command_queue);
	RETURN;
}

cl_int
clGetEventProfilingInfo(
	cl_event event,
	cl_profiling_info param_name,
	size_t param_value_size,
	void *param_value,
	size_t *param_value_size_ret)
{
	INIT_STATUS(CL_INVALID_EVENT);
	DECLARE_HANDLE(param_value);
	DECLARE_HANDLE(param_value_size_ret);
	DECLARE_HINT(event);
	CALLOC(param_value, 1, param_value_size);
	CALLOC(param_value_size_ret, 1, sizeof(size_t));
	EXECUTE(CL_GETEVENTPROFILINGINFO, event, param_name, param_value_size,
		HANDLE(param_value), HANDLE(param_value_size_ret));
	FETCH(param_value, param_value_size);
	FETCH(param_value_size_ret, sizeof(size_t));
	PRINT_RETVAL;
	RETURN;
}

cl_int
clFlush(cl_command_queue command_queue)
{
	INIT_STATUS(CL_INVALID_COMMAND_QUEUE);
	DECLARE_HINT(command_queue);
	EXECUTE(CL_FLUSH, command_queue);
	RETURN;
}

cl_int
clFinish(cl_command_queue command_queue)
{
	INIT_STATUS(CL_INVALID_COMMAND_QUEUE);
	DECLARE_HINT(command_queue);
	EXECUTE(CL_FINISH, command_queue);
	RETURN;
}

