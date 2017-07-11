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

#include "agent.h"
#include "agent_opencl.h"

CLARA_POST_CLFUN_HANDLER(clCreateProgramWithSource)
{
	AGENT_GENERIC_CREATE(CL_PROGRAM);
}

CLARA_POST_CLFUN_HANDLER(clCreateProgramWithBinary)
{
	AGENT_GENERIC_CREATE(CL_PROGRAM);
}

CLARA_PRE_CLFUN_HANDLER(clRetainProgram)
{
	AGENT_GENERIC_RETAIN(CL_PROGRAM);
}

CLARA_PRE_CLFUN_HANDLER(clReleaseProgram)
{
	AGENT_GENERIC_RELEASE(CL_PROGRAM);
}

CLARA_POST_CLFUN_HANDLER(clUnloadCompiler)
{
	provider_t provider;
	arg_t retval;

	set_start(args->source->agent->providers);

	while ((provider = set_next(args->source->agent->providers)) != NULL)
	{
		if (!agent_remote_execute(provider, args->msgbuf, args->msgsz, &retval))
		{
			debug_printff("agent_remote_execute failed");
		}
		else
		if (retval != CL_SUCCESS)
		{
			debug_printff("Warning: According to the OpenCL 1.0 specification "
				"clUnloadCompiler should always return CL_SUCCESS. The returned "
				"value is %d.", retval);
		}
	}

	set_finish(args->source->agent->providers);

	/* clUnloadCompiler always returns success */
	args->argv[0] = CL_SUCCESS;
}

