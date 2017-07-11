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
#include <string.h>
#include <CL/cl.h>

#include "consumer.h"

cl_int
clGetPlatformIDs(cl_uint num_entries, cl_platform_id *platforms, cl_uint *num_platforms)
{
	INIT_STATUS(CL_SUCCESS);
	DECLARE_HANDLE(platforms);
	DECLARE_HANDLE(num_platforms);
	DECLARE_NO_HINT;

	if ((num_entries == 0) && (platforms != NULL))
		return CL_INVALID_VALUE;

	if ((platforms == NULL) && (num_platforms == NULL))
		return CL_INVALID_VALUE;

	if (num_platforms != NULL)
		*num_platforms = 0;

	CALLOC(platforms, num_entries, sizeof(cl_platform_id));
	CALLOC(num_platforms, 1, sizeof(cl_uint));

	EXECUTE(CL_GETPLATFORMIDS, num_entries, HANDLE(platforms), HANDLE(num_platforms));

	FETCH(platforms, num_entries * sizeof(cl_platform_id));
	FETCH(num_platforms, sizeof(cl_uint));

	PRINT_RETVAL;
	RETURN;
}

cl_int clGetPlatformInfo (
	cl_platform_id platform,
	cl_platform_info param_name,
	size_t param_value_size,
	void *param_value,
	size_t *param_value_size_ret)
{
	INIT_STATUS(CL_SUCCESS);
	DECLARE_HANDLE(param_value);
	DECLARE_HANDLE(param_value_size_ret);
	DECLARE_NO_HINT;
	CALLOC(param_value, 1, param_value_size);
	CALLOC(param_value_size_ret, 1, sizeof(size_t));
	EXECUTE(CL_GETPLATFORMINFO, platform, param_name, param_value_size,
		HANDLE(param_value), HANDLE(param_value_size_ret));
	FETCH(param_value, param_value_size);
	FETCH(param_value_size_ret, sizeof(size_t));
	RETURN;
}

