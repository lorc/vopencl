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
clara_send_allocate_request(int sd, size_t nmemb, size_t size, void *data, global_id_t hint, struct sockaddr_in *to)
{
	ssize_t ssz;
	size_t msgsz;
	void *msgbuf;
	allocate_request_t params;

	msgsz = sizeof(*params);
	if (data != NULL) msgsz += (nmemb * size);

	msgbuf = (void *)calloc(1, msgsz);
	assert(msgbuf != NULL);

	params = (allocate_request_t)msgbuf;

	params->handle = INVALID_HANDLE;
	params->nmemb = nmemb;
	params->size = size;
	params->hint = hint;

	if (data != NULL)
	{
		/* copy data behind params structure in memory */
		memcpy(params + 1, data, nmemb * size);
	}

	ssz = clara_sendmsg(sd, CLARA_MSG_ALLOCATE_REQUEST, msgbuf, msgsz, to,
		0, 0, CLARA_CONTROL_STREAM, 0, 0);

	free((void *)msgbuf);

	if (ssz == -1)
	{
		debug_printff("clara_sendmsg failed");
		return false;
	}

	return true;
}

bool
clara_recv_allocate_response(int sd, handle_t *phandle)
{
	bool retval = false;
	handle_t handle;
	ssize_t ssz;
	message_type_t type = CLARA_MSG_ALLOCATE_RESPONSE;

	ssz = clara_recvmsg(sd, &type, &handle, sizeof(handle), NULL, NULL, NULL);

	if (ssz == -1)
	{
		debug_printff("clara_recvmsg failed");
	}
	else if (ssz != sizeof(handle_t))
	{
		debug_printff("size mismatch");
	}
	else
	{
		*phandle = handle;
		retval = true;
	}

	return retval;
}

bool
clara_recv_execute_response(int sd, void *retbuf, size_t retbuflen)
{
	bool retval = false;
	size_t bufsz = sizeof(arg_t);
	void *buffer = malloc(bufsz);	
	assert(buffer != NULL);

	ssize_t ssz;
	message_type_t type = CLARA_MSG_EXECUTE_RESPONSE;
	
	ssz = clara_recvmsg(sd, &type, buffer, bufsz, NULL, NULL, NULL);
	
	/* size 'zero' means that we received a response but the execution was not successful */
	if (ssz == 0)
	{
		debug_printff("execution failed");
	}
	else if (ssz == -1)
	{
		debug_printff("clara_recvmsg failed");
	}
	else if (ssz != sizeof(arg_t))
	{
		debug_printff("size mismatch");
	}
	else
	{
		memcpy(retbuf, buffer, retbuflen);
		retval = true;
	}
	free(buffer);

	return retval;
}

bool
clara_send_fetch_request(int sd, handle_t src, size_t n, global_id_t hint, stream_options_t opts, struct sockaddr_in *to)
{
	ssize_t ssz;
	struct fetch_request fetreq_p;

	if (src == INVALID_HANDLE)
		return false;

	fetreq_p.handle = src;
	fetreq_p.size = n;
	fetreq_p.hint = hint;
	memcpy(&fetreq_p.stream_opts, opts, sizeof(fetreq_p.stream_opts));

	ssz = clara_sendmsg(sd, CLARA_MSG_FETCH_REQUEST, &fetreq_p, sizeof(fetreq_p), to,
		0, 0, CLARA_CONTROL_STREAM, 0, 0);

	if (ssz == -1)
	{
		debug_printff("clara_sendmsg failed");
		return false;
	}

	return true;
}

bool
clara_recv_fetch_response(int sd, fetch_response_t params, void *buf, size_t count)
{
	ssize_t ssz;
	message_type_t type;
	struct sockaddr_in from;
	struct sctp_sndrcvinfo sinfo;
	int msg_flags;

	if (params == NULL)
		return false;

	memset(&from, 0, sizeof(from));
	type = CLARA_MSG_FETCH_RESPONSE;
	ssz = clara_recvmsg(sd, &type, params, sizeof(*params), &from, &sinfo, &msg_flags);

	if (ssz == -1)
	{
		debug_printf("clara_recv_fetch_response: clara_recvmsg failed, errno = %d\n", errno);
		return false;
	}

	assert(ssz == sizeof(*params));

	size_t bytes_received;

	bytes_received = stream_recv(sd, buf, count);
	if (bytes_received != count)
	{
		debug_printff("stream_recv failed, %u bytes received, %u expected", bytes_received, count);
		return false;
	}
	return true;
}

bool
clara_send_execute_request(int sd, arg_t d, arg_t *argv, int argc, global_id_t hint, struct sockaddr_in *to)
{
	void *msgbuf;
	size_t msgsz;
	execute_request_t params;
	ssize_t ssz;

	msgsz = sizeof(*params) + argc * sizeof(arg_t);
	msgbuf = (void *)calloc(1, msgsz);
	assert(msgbuf != NULL);

	params = (execute_request_t)msgbuf;
	params->hint = hint;
	params->fun = d;

	/* copy the arguments behind the parameter structure */
	memcpy(params + 1, argv, argc * sizeof(arg_t));

	ssz = clara_sendmsg(sd, CLARA_MSG_EXECUTE_REQUEST, (char *)msgbuf, msgsz, to,
		0, 0, CLARA_CONTROL_STREAM, 0, 0);

	free((void *)msgbuf);
	
	if (ssz == -1)
	{
		debug_printf("clara_send_execute_request: clara_sendmsg failed\n");
		return false;
	}

	return true;
}

bool
clara_send_mapatptr_request(int sd, global_id_t virt, global_id_t ptr, struct sockaddr_in *to)
{
	ssize_t ssz;

	struct mapatptr_request mapreq_p = {
		.virt = virt,
		.ptr = ptr,
	};

	ssz = clara_sendmsg(sd, CLARA_MSG_MAPATPTR_REQUEST, (char *)&mapreq_p, sizeof(mapreq_p), to, 0, 0, CLARA_CONTROL_STREAM, 0, 0);

	if (ssz == -1)
	{
		debug_printff("clara_sendmsg failed\n");
		return false;
	}

	return true;
}

bool
clara_recv_mapatptr_response(int sd, void **real)
{
	ssize_t ssz;
	message_type_t type = CLARA_MSG_MAPATPTR_RESPONSE;
	struct mapatptr_response mapres_p;

	if (real == NULL)
		return false;

	ssz = clara_recvmsg(sd, &type, &mapres_p, sizeof(mapres_p), NULL, NULL, NULL);
	if (ssz == -1)
	{
		debug_printff("clara_recvmsg failed\n");
		return false;
	}

	*real = mapres_p.real;

	return true;
}

