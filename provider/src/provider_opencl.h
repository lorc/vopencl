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

#if !defined(__PROVIDER_OPENCL_H__)
#define __PROVIDER_OPENCL_H__

#define CLARA_CLFUN(name)			void clara_ ## name(CLARA_CLFUN_ARGS)
#define CLARA_CLFUN_HANDLER(name)	void clara_ ## name(CLARA_CLFUN_ARGS)
#define CLARA_CLFUN_NAME(name)		clara_ ## name

/* opencl_device.c */
CLARA_CLFUN(clGetDeviceInfo);

/* opencl_context.c */
CLARA_CLFUN(clCreateContext);
CLARA_CLFUN(clCreateContextFromType);
CLARA_CLFUN(clRetainContext);
CLARA_CLFUN(clReleaseContext);

/* opencl_cmdqueue.c */
CLARA_CLFUN(clCreateCommandQueue);
CLARA_CLFUN(clRetainCommandQueue);
CLARA_CLFUN(clReleaseCommandQueue);
CLARA_CLFUN(clGetCommandQueueInfo);

/* opencl_memobj.c */
CLARA_CLFUN(clCreateBuffer);
CLARA_CLFUN(clRetainMemObject);
CLARA_CLFUN(clReleaseMemObject);
CLARA_CLFUN(clGetMemObjectInfo);
CLARA_CLFUN(clEnqueueReadBuffer);
CLARA_CLFUN(clEnqueueWriteBuffer);
CLARA_CLFUN(clEnqueueCopyBuffer);

/* opencl_program.c */
CLARA_CLFUN(clCreateProgramWithSource);
CLARA_CLFUN(clCreateProgramWithBinary);
CLARA_CLFUN(clRetainProgram);
CLARA_CLFUN(clReleaseProgram);
CLARA_CLFUN(clBuildProgram);
CLARA_CLFUN(clUnloadCompiler);
CLARA_CLFUN(clGetProgramInfo);
CLARA_CLFUN(clGetProgramBuildInfo);

/* opencl_kernel.c */
CLARA_CLFUN(clCreateKernel);
CLARA_CLFUN(clSetKernelArg);
CLARA_CLFUN(clRetainKernel);
CLARA_CLFUN(clReleaseKernel);
CLARA_CLFUN(clGetKernelInfo);
CLARA_CLFUN(clEnqueueNDRangeKernel);
CLARA_CLFUN(clEnqueueTask);

/* opencl_event.c */
CLARA_CLFUN(clWaitForEvents);
CLARA_CLFUN(clGetEventInfo);
CLARA_CLFUN(clRetainEvent);
CLARA_CLFUN(clReleaseEvent);
CLARA_CLFUN(clEnqueueMarker);
CLARA_CLFUN(clEnqueueWaitForEvents);
CLARA_CLFUN(clEnqueueBarrier);
CLARA_CLFUN(clGetEventProfilingInfo);

CLARA_CLFUN(clFlush);
CLARA_CLFUN(clFinish);

#define DECLARE_GETOBJECTINFO(name, typ) \
CLARA_CLFUN(clGet ## name ## Info) \
{ \
	cl_ ## typ object = ARG2PTR(args->argv[1]); \
	cl_ ## typ ## _info param_name = (cl_ ## typ ## _info)args->argv[2]; \
	size_t param_value_size = (size_t)args->argv[3]; \
	void *param_value = ARG2PTR(args->argv[4]); \
	size_t *param_value_size_ret = ARG2PTR(args->argv[5]); \
	args->argv[0] = clGet ## name ## Info(object, param_name, \
		param_value_size, param_value, param_value_size_ret); \
	debug_printff("(object=%lx, param_name=%lx, %u, %lx, %lx) -> %d", \
		object, param_name, param_value_size, PTR2ARG(param_value), \
		PTR2ARG(param_value_size_ret), args->argv[0]); \
}

#define DECLARE_GETOBJECTINFO_EX(name, typ1, typ2) \
CLARA_CLFUN(clGet ## name ## Info) \
{ \
	cl_ ## typ1 object = ARG2PTR(args->argv[1]); \
	cl_ ## typ2 ## _info param_name = (cl_ ## typ2 ## _info)args->argv[2]; \
	size_t param_value_size = (size_t)args->argv[3]; \
	void *param_value = ARG2PTR(args->argv[4]); \
	size_t *param_value_size_ret = ARG2PTR(args->argv[5]); \
	args->argv[0] = clGet ## name ## Info(object, param_name, \
		param_value_size, param_value, param_value_size_ret); \
	debug_printff("(object=%lx, param_name=%lx, %u, %lx, %lx) -> %d", \
		object, param_name, param_value_size, PTR2ARG(param_value), \
		PTR2ARG(param_value_size_ret), args->argv[0]); \
}

#define DECLARE_ERRCODE(n) \
	cl_int *__errcode_ret = ARG2PTR(args->argv[n]); \
	cl_int __errcode;

#define COPY_ERRCODE() \
	if (__errcode_ret != NULL) *__errcode_ret = __errcode;

#define ERRCODE			__errcode

#endif /* __PROVIDER_OPENCL_H__ */

