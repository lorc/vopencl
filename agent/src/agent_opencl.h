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

#if !defined(__AGENT_OPENCL_H__)
#define __AGENT_OPENCL_H__

#include "clara.h"
#include <CL/cl.h>

#include "object.h"

#define CLARA_PRE_CLFUN_HANDLER(name)	void clara_pre_ ## name(CLARA_CLFUN_ARGS)
#define CLARA_PRE_CLFUN_NAME(name)		clara_pre_ ## name
#define CLARA_POST_CLFUN_HANDLER(name)	void clara_post_ ## name(CLARA_CLFUN_ARGS)
#define CLARA_POST_CLFUN_NAME(name)		clara_post_ ## name

#define CLARA_DEFINE_CLFUN_HANDLER(name)	\
			void clara_pre_ ## name(CLARA_CLFUN_ARGS); \
			void clara_post_ ## name(CLARA_CLFUN_ARGS);

/* opencl_object.c */
global_id_t clCreateObject(provider_t, consumer_t, void *local_id, global_id_t wish);
bool clRetainObject(consumer_t, global_id_t id);
bool clReleaseObject(consumer_t, global_id_t id);

static inline cl_int
invalid_object_status(object_type_t type)
{
	switch (type)
	{
	case CL_CONTEXT:		return CL_INVALID_CONTEXT;
	case CL_COMMANDQUEUE:	return CL_INVALID_COMMAND_QUEUE;
	case CL_MEMOBJECT:		return CL_INVALID_MEM_OBJECT;
	case CL_SAMPLER:		return CL_INVALID_SAMPLER;
	case CL_PROGRAM:		return CL_INVALID_PROGRAM;
	case CL_KERNEL:			return CL_INVALID_KERNEL;
	case CL_EVENT:			return CL_INVALID_EVENT;
	default:
		assert(0);
	}
	return CL_SUCCESS;
}

#define AGENT_GENERIC_CREATE(type)	args->argv[0] = (arg_t)clCreateObject(args->target, args->source, ARG2PTR(args->argv[0]), INVALID_ID)
#define AGENT_GENERIC_RETAIN(type)	args->argv[0] = clRetainObject(args->source, (global_id_t)args->argv[1]) ? CL_SUCCESS : invalid_object_status(type)
#define AGENT_GENERIC_RELEASE(type)	args->argv[0] = clReleaseObject(args->source, (global_id_t)args->argv[1]) ? CL_SUCCESS : invalid_object_status(type)

#define AGENT_GENERIC_MAP_EVENT(n)	\
{ \
	global_id_t handle = (global_id_t)args->argv[n]; \
	if (handle != INVALID_ID) \
	{ \
		provider_t provider = args->target; \
		void *real; \
		global_id_t virt; \
		assert(id_create(provider->agent->id_pool, &virt)); \
		provider_lock(provider); \
		/* request the real ID of the object at *handle */ \
		assert(clara_send_mapatptr_request(provider->sd, virt, handle, &provider->addr)); \
		assert(clara_recv_mapatptr_response(provider->sd, &real)); \
		/* debug_printff("%lx <--> %lx", real, virt); */ \
		assert(clCreateObject(provider, args->source, real, virt) == virt); \
		provider_unlock(provider); \
	} \
}

/* platform and device */
CLARA_DEFINE_CLFUN_HANDLER(clGetPlatformIDs);
CLARA_DEFINE_CLFUN_HANDLER(clGetPlatformInfo);
CLARA_DEFINE_CLFUN_HANDLER(clGetDeviceIDs);
CLARA_DEFINE_CLFUN_HANDLER(clGetDeviceInfo);

/* context */
CLARA_DEFINE_CLFUN_HANDLER(clCreateContext);
CLARA_DEFINE_CLFUN_HANDLER(clCreateContextFromType);
CLARA_DEFINE_CLFUN_HANDLER(clRetainContext);
CLARA_DEFINE_CLFUN_HANDLER(clReleaseContext);

/* command queue */
CLARA_DEFINE_CLFUN_HANDLER(clCreateCommandQueue);
CLARA_DEFINE_CLFUN_HANDLER(clRetainCommandQueue);
CLARA_DEFINE_CLFUN_HANDLER(clReleaseCommandQueue);

/* memory object */
CLARA_DEFINE_CLFUN_HANDLER(clCreateBuffer);
CLARA_DEFINE_CLFUN_HANDLER(clRetainMemObject);
CLARA_DEFINE_CLFUN_HANDLER(clReleaseMemObject);
CLARA_DEFINE_CLFUN_HANDLER(clEnqueueReadBuffer);
CLARA_DEFINE_CLFUN_HANDLER(clEnqueueWriteBuffer);
CLARA_DEFINE_CLFUN_HANDLER(clEnqueueCopyBuffer);

/* opencl_program.c */
CLARA_DEFINE_CLFUN_HANDLER(clCreateProgramWithSource);
CLARA_DEFINE_CLFUN_HANDLER(clCreateProgramWithBinary);
CLARA_DEFINE_CLFUN_HANDLER(clRetainProgram);
CLARA_DEFINE_CLFUN_HANDLER(clReleaseProgram);
CLARA_DEFINE_CLFUN_HANDLER(clBuildProgram);
CLARA_DEFINE_CLFUN_HANDLER(clUnloadCompiler);

/* opencl_kernel.c */
CLARA_DEFINE_CLFUN_HANDLER(clCreateKernel);
CLARA_DEFINE_CLFUN_HANDLER(clRetainKernel);
CLARA_DEFINE_CLFUN_HANDLER(clReleaseKernel);

CLARA_DEFINE_CLFUN_HANDLER(clEnqueueNDRangeKernel);
CLARA_DEFINE_CLFUN_HANDLER(clEnqueueTask);

/* opencl_event.c */
CLARA_DEFINE_CLFUN_HANDLER(clRetainEvent);
CLARA_DEFINE_CLFUN_HANDLER(clReleaseEvent);
CLARA_DEFINE_CLFUN_HANDLER(clEnqueueMarker);
CLARA_DEFINE_CLFUN_HANDLER(clEnqueueBarrier);
CLARA_DEFINE_CLFUN_HANDLER(clFlush);
CLARA_DEFINE_CLFUN_HANDLER(clFinish);

#endif /* __AGENT_OPENCL_H__ */

