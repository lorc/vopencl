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

#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#include "clara.h"
#include "consumer.h"

#include <CL/cl.h>

struct clara_consumer *consumer = NULL;

void
consumer_deinit(void)
{
	if (consumer == NULL)
		return;

	close(consumer->sd);
	free((void *)consumer);
	consumer = NULL;
}

int
consumer_init(void)
{
	ssize_t ssz;
	char *agent_port;
	message_type_t type;
	struct sockaddr_in from;
	struct sctp_sndrcvinfo recvinfo;
	int flags;

	if (consumer != NULL)
		return 1;

	clara_debug_init();

	consumer = (struct clara_consumer *)calloc(1, sizeof(struct clara_consumer));

	if ((agent_port = getenv("AGENT_PORT")) == NULL)
		agent_port = CLARA_DEFAULT_PORT;

	if (!clara_sockaddr(getenv("AGENT_ADDR"), agent_port, &consumer->agent_addr))
	{
		debug_printff("clara_sockaddr failed");
		return 0;
	}
	
	if ((consumer->sd = clara_socket()) == -1)
	{
		debug_printff("clara_socket failed");
		return 0;
	}

	if ((ssz = clara_sendmsg(consumer->sd, CLARA_MSG_CONSUMER, NULL, 0, &consumer->agent_addr,
		0, 0, CLARA_CONTROL_STREAM, 0, 0)) == -1)
	{
		debug_printff("clara_sendmsg failed");
		consumer_deinit();
		return 0;
	}

	if ((ssz = clara_recvmsg(consumer->sd, &type, NULL, 0, &from, &recvinfo, &flags)) == -1)
	{
		debug_printff("clara_recvmsg failed");
		consumer_deinit();
		return 0;
	} 

	if (ssz != 0)
	{
		debug_printff("size mismatch");
		consumer_deinit();
		return 0;
	}

	if (type != CLARA_MSG_ACK)
	{
		debug_printff("invalid type: %08x", type);
		consumer_deinit();
		return 0;
	}

	return 1;
}

bool
consumer_begin_allocate(size_t nmemb, size_t size, void *data, global_id_t hint)
{
	bool retval = false;

	if (!consumer_init())
	{
		debug_printff("could not initialize consumer");
	}
	else
	{
		retval = clara_send_allocate_request(consumer->sd, nmemb, size, data, hint, &consumer->agent_addr);
	}

	return retval;
}

handle_t
consumer_end_allocate(void)
{
	handle_t retval = INVALID_HANDLE;

	if (!clara_recv_allocate_response(consumer->sd, &retval))
	{
		debug_printff("clara_recv_allocate_response failed\n");
	}

	return retval;
}

bool
consumer_execute(arg_t d, void *retbuf, size_t retbuflen, global_id_t hint, ...)
{
	bool retval = false;
	va_list ap;

	if (!consumer_init())
	{
		debug_printf("consumer_execute: could not initialize consumer\n");
		return false;
	}

	va_start(ap, hint);
	const struct cl_function_info *fni = opencl_get_clfuninfo(d);
	size_t argc = fni->argc;
	arg_t *argv = malloc(argc * sizeof(arg_t));
	assert(argv != NULL);
	assert(clara_arg2buf(argv, ap, argc));
	va_end(ap);
	
	if (!clara_send_execute_request(consumer->sd, d, argv, argc, hint, &consumer->agent_addr))
	{
		debug_printff("clara_send_execute_request failed");
	}
	else if (!clara_recv_execute_response(consumer->sd, retbuf, retbuflen))
	{
		debug_printff("clara_recv_execute_response failed");
	}
	else
	{
		retval = true;
	}
	
	return retval;
}

bool
consumer_begin_fetch(handle_t src, size_t n, global_id_t hint)
{
	bool retval = false;

	struct stream_options opts = {
		.fmt = STREAM_FORMAT(n),
		.blksz = STREAM_BLOCKSIZE,
	};

	retval = clara_send_fetch_request(consumer->sd, src, n, hint, &opts, &consumer->agent_addr);

	if (!retval)
	{
		debug_printff("clara_send_fetch_request failed");
	}

	return retval;
}

bool
consumer_end_fetch(void *dst, size_t n)
{
	struct fetch_response res_params;

	if (dst == NULL)
		return false;

	if (!clara_recv_fetch_response(consumer->sd, &res_params, dst, n))
	{
		debug_printf("consumer_end_fetch: clara_recv_fetch_response failed\n");
		return false;
	}

	return true;
}

/* synchronous frontends for allocate and fetch */

handle_t
consumer_allocate(size_t nmemb, size_t size, void *data, global_id_t hint)
{
//	debug_printf("consumer_allocate(nmemb=%u, size=%u, &data=%016lx, hint=%016lx\n", nmemb, size, data, hint);

	if (!consumer_begin_allocate(nmemb, size, data, hint))
		return INVALID_HANDLE;

	return consumer_end_allocate();
}

bool
consumer_fetch(handle_t src, void *dst, size_t n, global_id_t hint)
{
//	debug_printf("consumer_fetch(%016lx, %016lx, %lu, %016lx)\n", src, dst, n, hint);

	if (src == INVALID_HANDLE)
		return false;

	if (dst == NULL)
		n = 0;

	if (!consumer_begin_fetch(src, n, hint))
	{
		debug_printf("consumer_fetch: consumer_begin_fetch(src=%016lx, n=%u, hint=%016lx) failed\n", src, n, hint);
		return false;
	}

	if (n == 0)
		return true;

	if (!consumer_end_fetch(dst, n))
	{
		debug_printf("consumer_fetch: consumer_end_fetch(&dst=%016lx, n=%u) failed\n", dst, n);
		return false;
	}

	return true;
}

bool
consumer_free(handle_t src, global_id_t hint)
{
	return consumer_fetch(src, NULL, 0, hint);
}

bool
consumer_memcpy(handle_t dst, void *src, size_t size, global_id_t hint)
{
	struct memput_stream_params params;

	params.dst = dst;
	params.offset = 0;
	params.length = size;
	params.hint = hint;

	ssize_t ssz;

	ssz = clara_sendmsg(consumer->sd, CLARA_MSG_MEMPUT_STREAM, &params, sizeof(params), &consumer->agent_addr,
		0, 0, CLARA_CONTROL_STREAM, 0, 0);

	if (ssz == -1)
	{
		debug_printf("consumer_memcpy: clara_sendmsg failed\n");
		return false;
	}

	size_t bytes_sent;

	struct stream_options opts = {
		.fmt = STREAM_FORMAT(size),
		.blksz = STREAM_BLOCKSIZE,
	};

	bytes_sent = stream_send(consumer->sd, src, size, &opts, &consumer->agent_addr);
	if (bytes_sent != size)
	{
		debug_printff("stream_send failed: %u bytes sent", bytes_sent);
		return false;
	}
	return true;
}

