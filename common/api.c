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

#include "api.h"

const struct cl_function_info clfuninfo[] = {

	/* 4.1 */
	[CL_GETPLATFORMIDS]				= { CL_GETPLATFORMIDS,				"clGetPlatformIDs",				3 },
	[CL_GETPLATFORMINFO]			= { CL_GETPLATFORMINFO,				"clGetPlatformInfo",			5 },

	/* 4.2 */
	[CL_GETDEVICEIDS]				= { CL_GETDEVICEIDS,				"clGetDeviceIDs",				5 },
	[CL_GETDEVICEINFO]				= { CL_GETDEVICEINFO,				"clGetDeviceInfo",				5 },

	/* 4.3 */
	[CL_CREATECONTEXT]				= { CL_CREATECONTEXT,				"clCreateContext",				6 },
	[CL_CREATECONTEXTFROMTYPE]		= { CL_CREATECONTEXTFROMTYPE,		"clCreateContextFromType",		5 },
	[CL_RETAINCONTEXT]				= { CL_RETAINCONTEXT,				"clRetainContext",				1 },
	[CL_RELEASECONTEXT]				= { CL_RELEASECONTEXT,				"clReleaseContext",				1 },
	[CL_GETCONTEXTINFO]				= { CL_GETCONTEXTINFO,				"clGetContextInfo",				5 },

	/* 5.1 */
	[CL_CREATECOMMANDQUEUE]			= { CL_CREATECOMMANDQUEUE,			"clCreateCommandQueue",			4 },
	[CL_RETAINCOMMANDQUEUE]			= { CL_RETAINCOMMANDQUEUE,			"clRetainCommandQueue",			1 },
	[CL_RELEASECOMMANDQUEUE]		= { CL_RELEASECOMMANDQUEUE,			"clReleaseCommandQueue",		1 },
	[CL_GETCOMMANDQUEUEINFO]		= { CL_GETCOMMANDQUEUEINFO,			"clGetCommandQueueInfo",		5 },
	[CL_SETCOMMANDQUEUEPROPERTY]	= { CL_SETCOMMANDQUEUEPROPERTY,		"clSetCommandQueueProperty",	4 },

	/* 5.2.1 */
	[CL_CREATEBUFFER]				= { CL_CREATEBUFFER,				"clCreateBuffer",				5 },

	/* 5.2.2 */
	[CL_ENQUEUEREADBUFFER]			= { CL_ENQUEUEREADBUFFER,			"clEnqueueReadBuffer",			9 },
	[CL_ENQUEUEWRITEBUFFER]			= { CL_ENQUEUEWRITEBUFFER,			"clEnqueueWriteBuffer",			9 },
	[CL_ENQUEUECOPYBUFFER]			= { CL_ENQUEUECOPYBUFFER,			"clEnqueueCopyBuffer",			9 },
	
	/* 5.2.3 */
	[CL_RETAINMEMOBJECT]			= { CL_RETAINMEMOBJECT,				"clRetainMemObject",			1 },
	[CL_RELEASEMEMOBJECT]			= { CL_RELEASEMEMOBJECT,			"clReleaseMemObject",			1 },

	/* 5.2.4 */
	[CL_CREATEIMAGE2D]				= { CL_CREATEIMAGE2D,				"clCreateImage2D",				8 },
	[CL_CREATEIMAGE3D]				= { CL_CREATEIMAGE3D,				"clCreateImage3D",				10 },
	
	/* 5.2.5 */
	[CL_GETSUPPORTEDIMAGEFORMATS]	= { CL_GETSUPPORTEDIMAGEFORMATS,	"clGetSupportedImageFormats",	6 },
	
	/* 5.2.6 */
	[CL_ENQUEUEREADIMAGE]			= { CL_ENQUEUEREADIMAGE,			"clEnqueueReadImage",			11 },
	[CL_ENQUEUEWRITEIMAGE]			= { CL_ENQUEUEWRITEIMAGE,			"clEnqueueWriteImage",			11 },
	[CL_ENQUEUECOPYIMAGE]			= { CL_ENQUEUECOPYIMAGE,			"clEnqueueCopyImage",			9 },

	/* 5.2.7 */
	[CL_ENQUEUECOPYIMAGETOBUFFER]	= { CL_ENQUEUECOPYIMAGETOBUFFER,	"clEnqueueCopyImageToBuffer",	9 },
	[CL_ENQUEUECOPYBUFFERTOIMAGE]	= { CL_ENQUEUECOPYBUFFERTOIMAGE,	"clEnqueueCopyBufferToImage",	9 },

	/* 5.2.8 */
	[CL_ENQUEUEMAPBUFFER]			= { CL_ENQUEUEMAPBUFFER,			"clEnqueueMapBuffer",			10 },
	[CL_ENQUEUEMAPIMAGE]			= { CL_ENQUEUEMAPIMAGE,				"clEnqueueMapImage",			12 },
	[CL_ENQUEUEUNMAPMEMOBJECT]		= { CL_ENQUEUEUNMAPMEMOBJECT,		"clEnqueueUnmapMemObject",		6 },

	/* 5.2.9 */
	[CL_GETMEMOBJECTINFO]			= { CL_GETMEMOBJECTINFO,			"clGetMemObjectInfo",			5 },
	[CL_GETIMAGEINFO]				= { CL_GETIMAGEINFO,				"clGetImageInfo",				5 },

	/* 5.3 */
	[CL_CREATESAMPLER]				= { CL_CREATESAMPLER,				"clCreateSampler",				5 },
	[CL_RETAINSAMPLER]				= { CL_RETAINSAMPLER,				"clRetainSampler",				1 },
	[CL_RELEASESAMPLER]				= { CL_RELEASESAMPLER,				"clReleaseSampler",				1 },
	[CL_GETSAMPLERINFO]				= { CL_GETSAMPLERINFO,				"clGetSamplerInfo",				5 },

	/* 5.4.1 */
	[CL_CREATEPROGRAMWITHSOURCE]	= { CL_CREATEPROGRAMWITHSOURCE,		"clCreateProgramWithSource",	5 },
	[CL_CREATEPROGRAMWITHBINARY]	= { CL_CREATEPROGRAMWITHBINARY,		"clCreateProgramWithBinary",	7 },
	[CL_RETAINPROGRAM]				= { CL_RETAINPROGRAM,				"clRetainProgram",				1 },
	[CL_RELEASEPROGRAM]				= { CL_RELEASEPROGRAM,				"clReleaseProgram",				1 },

	/* 5.4.2 */
	[CL_BUILDPROGRAM]				= { CL_BUILDPROGRAM,				"clBuildProgram",				6 },
	
	/* 5.4.4 */
	[CL_UNLOADCOMPILER]				= { CL_UNLOADCOMPILER,				"clUnloadCompiler",				0 },

	/* 5.4.5 */
	[CL_GETPROGRAMINFO]				= { CL_GETPROGRAMINFO,				"clGetProgramInfo",				5 },
	[CL_GETPROGRAMBUILDINFO]		= { CL_GETPROGRAMBUILDINFO,			"clGetProgramBuildInfo",		6 },

	/* 5.5.1 */
	[CL_CREATEKERNEL]				= { CL_CREATEKERNEL,				"clCreateKernel",				3 },
	[CL_CREATEKERNELSINPROGRAM]		= { CL_CREATEKERNELSINPROGRAM,		"clCreateKernelsInProgram",		4 },
	[CL_RETAINKERNEL]				= { CL_RETAINKERNEL,				"clRetainKernel",				1 },
	[CL_RELEASEKERNEL]				= { CL_RELEASEKERNEL,				"clReleaseKernel",				1 },

	/* 5.5.2 */
	[CL_SETKERNELARG]				= { CL_SETKERNELARG,				"clSetKernelArg",				4 },

	/* 5.5.3 */
	[CL_GETKERNELINFO]				= { CL_GETKERNELINFO,				"clGetKernelInfo",				5 },
	[CL_GETKERNELWORKGROUPINFO]		= { CL_GETKERNELWORKGROUPINFO,		"clGetKernelWorkGroupInfo",		6 },

	/* 5.6 */
	[CL_ENQUEUENDRANGEKERNEL]		= { CL_ENQUEUENDRANGEKERNEL,		"clEnqueueNDRangeKernel",		9 },
	[CL_ENQUEUETASK]				= { CL_ENQUEUETASK,					"clEnqueueTask",				5 },
	[CL_ENQUEUENATIVEKERNEL]		= { CL_ENQUEUENATIVEKERNEL,			"clEnqueueNativeKernel",		10 },

	/* 5.7 */
	[CL_WAITFOREVENTS]				= { CL_WAITFOREVENTS,				"clWaitForEvents",				2 },
	[CL_GETEVENTINFO]				= { CL_GETEVENTINFO,				"clGetEventInfo",				5 },
	[CL_RETAINEVENT]				= { CL_RETAINEVENT,					"clRetainEvent",				1 },
	[CL_RELEASEEVENT]				= { CL_RELEASEEVENT,				"clReleaseEvent",				1 },

	/* 5.8 */
	[CL_ENQUEUEMARKER]				= { CL_ENQUEUEMARKER,				"clEnqueueMarker",				2 },
	[CL_ENQUEUEWAITFOREVENTS]		= { CL_ENQUEUEWAITFOREVENTS,		"clEnqueueWaitForEvents",		3 },
	[CL_ENQUEUEBARRIER]				= { CL_ENQUEUEBARRIER,				"clEnqueueBarrier",				1 },

	/* 5.9 */
	[CL_GETEVENTPROFILINGINFO]		= { CL_GETEVENTPROFILINGINFO,		"clGetEventProfilingInfo",		5 },
	
	/* 5.10 */
	[CL_FLUSH]						= { CL_FLUSH,						"clFlush",						1 },
	[CL_FINISH]						= { CL_FINISH,						"clFinish",						1 },
};

const struct cl_function_info *
opencl_get_clfuninfo(enum cl_function_t id)
{
	const struct cl_function_info *retval;

	retval = clfuninfo + id;

	assert(retval->id == id);

	return retval;
}


