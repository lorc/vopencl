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

#include "clara.h"
#include "api.h"

bool
clara_execute(const struct fun_table *fntbl, id_map_function_t id_map, void *data, execute_request_t params, arg_t *retval, struct cl_function_handler_args *args)
{
	int i;
	int d = params->fun;
	fntbl_entry_t fntblentry = (fntbl_entry_t)&fntbl->entries[d];

	arg_t *argv, *argp;

	/* some sanity checks */
	if ((data == NULL) || (params == NULL) || (retval == NULL))
	{
		errno = EINVAL;
		return false;
	}

	/* skip execution if there is no appropriate function in the table */
	if (fntbl->num_entries <= d)
		return true;
	if (fntblentry->pfn == NULL)
		return true;

	const struct cl_function_info *fni = opencl_get_clfuninfo(d);

	assert(fni->argc <= MAX_ARGS);

	/* argument list starts after regular parameters, but we have to consider the retval member */
	argv = (arg_t *)(params + 1) - 1;
	params->retval = *retval;

	/* this piece of code converts machine-independent handles to machine-dependend pointers */
	uint32_t argsel = 1;
	uint32_t argmsk = fntblentry->ptrargs;
	argp = argv + 1;
	for (i = 1; i <= fni->argc; i++, argsel <<= 1, argp++)
	{
		/* it's not necessary to translate NULL pointers or invalid handles, skip */
		if (*argp == 0)
			continue;

		if ((argmsk & argsel) != 0)
			*argp = id_map(*argp, data);
	}
	
	/* finally execute the function */
	args->argv = argv;
	fntblentry->pfn(args);
	*retval = argv[0];

	return true;
}

bool
clara_send_allocate_response(int sd, handle_t handle, struct sockaddr_in *to)
{
	ssize_t ssz;

	ssz = clara_sendmsg(sd, CLARA_MSG_ALLOCATE_RESPONSE, &handle, sizeof(handle), to,
		0, 0, CLARA_CONTROL_STREAM, 0, 0);

	if (ssz == -1)
	{
		debug_printf("clara_send_allocate_response: clara_sendmsg failed, errno = %d\n", errno);
		return false;
	}

	if (ssz != sizeof(handle))
	{
		debug_printf("clara_send_allocate_response: incomplete, errno = %d\n", errno);
		return false;
	}

	return true;
}

bool
clara_send_execute_response(int sd, arg_t retval, struct sockaddr_in *to, bool successful)
{
	ssize_t ssz;

	size_t msgsz = 0;
	void *msgbuf = NULL;

	if (successful)
	{
		msgsz = sizeof(retval);
		msgbuf = (void *)&retval;
	}

	ssz = clara_sendmsg(sd, CLARA_MSG_EXECUTE_RESPONSE, msgbuf, msgsz, to,
		0, 0, CLARA_CONTROL_STREAM, 0, 0);

	if (ssz == -1)
	{
		debug_printf("clara_send_execute_response: clara_sendmsg failed\n", errno);
		return false;
	}

	return true;
}

bool
clara_send_fetch_response(int sd, const void *buf, size_t count, stream_options_t opts, struct sockaddr_in *to)
{
	ssize_t ssz;

	struct fetch_response fetres_p;
	fetres_p.length = count;

	ssz = clara_sendmsg(sd, CLARA_MSG_FETCH_RESPONSE, &fetres_p, sizeof(fetres_p), to,
		0, 0, CLARA_CONTROL_STREAM, 0, 0);

	if (ssz == -1)
	{
		debug_printf("clara_send_fetch_response: clara_sendmsg failed, errno = %d\n", errno);
		return false;
	}

	size_t bytes_sent;

	bytes_sent = stream_send(sd, buf, count, opts, to);
	if (bytes_sent != count)
	{
		debug_printff("stream_send failed: %u bytes sent, %u expected", bytes_sent, count);
		return false;
	}
	return true;
}

bool
clara_send_mapatptr_response(int sd, void *real, struct sockaddr_in *to)
{
	ssize_t ssz;

	struct mapatptr_response mapres_p = {
		.real = real
	};

	ssz = clara_sendmsg(sd, CLARA_MSG_MAPATPTR_RESPONSE, (char *)&mapres_p, sizeof(mapres_p), to,
		0, 0, CLARA_CONTROL_STREAM, 0, 0);

	if (ssz == -1)
	{
		debug_printff("clara_sendmsg failed\n");
		return false;
	}

	return true;
}

