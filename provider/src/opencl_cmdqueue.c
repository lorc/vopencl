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

CLARA_CLFUN(clCreateCommandQueue)
{
	cl_context context = ARG2PTR(args->argv[1]);
	cl_device_id device = ARG2PTR(args->argv[2]);
	cl_command_queue_properties properties = (cl_command_queue_properties)args->argv[3];
	cl_int *errcode_ret = ARG2PTR(args->argv[4]);

	args->argv[0] = PTR2ARG(clCreateCommandQueue(context, device,
		properties, errcode_ret));
	debug_printff("(ctx=%lx, dev=%lx, prop=%lx, err=%d) -> %lx",
		PTR2ARG(context), PTR2ARG(device), properties,
		PTR2ARG(errcode_ret), args->argv[0]);
}

CLARA_CLFUN(clRetainCommandQueue)
{
	args->argv[0] = (arg_t)clRetainCommandQueue(ARG2PTR(args->argv[1]));
	debug_printff("(%lx) -> %d", args->argv[1], args->argv[0]);
}

CLARA_CLFUN(clReleaseCommandQueue)
{
	args->argv[0] = (arg_t)clReleaseCommandQueue(ARG2PTR(args->argv[1]));
	debug_printff("(%lx) -> %d", args->argv[1], args->argv[0]);
}

DECLARE_GETOBJECTINFO(CommandQueue, command_queue);

CLARA_CLFUN(clFlush)
{
	args->argv[0] = (arg_t)clFlush(ARG2PTR(args->argv[1]));
	debug_printff("(%lx) -> %d", args->argv[1], args->argv[0]);
}

CLARA_CLFUN(clFinish)
{
	args->argv[0] = (arg_t)clFinish(ARG2PTR(args->argv[1]));
	debug_printff("(%lx) -> %d", args->argv[1], args->argv[0]);
}

