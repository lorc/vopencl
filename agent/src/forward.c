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

void
agent_forward_fetch(consumer_t consumer, provider_t provider, fetch_request_t params)
{
	ssize_t ssz;

	provider_lock(provider);

	ssz = clara_sendmsg(provider->sd, CLARA_MSG_FETCH_REQUEST, params, sizeof(*params),
		&provider->addr, 0, 0, CLARA_CONTROL_STREAM, 0, 0);
	
	if (params->size > 0)
	{
		struct fetch_response frp;

		message_type_t type;
		type = CLARA_MSG_FETCH_RESPONSE;
		ssz = clara_recvmsg(provider->sd, &type, &frp, sizeof(frp), NULL, NULL, NULL);
		ssz = clara_sendmsg(consumer->sd, type, &frp, sizeof(frp), &consumer->addr, 0, 0, CLARA_CONTROL_STREAM, 0, 0);

		stream_forward(provider->sd, consumer->sd, &consumer->addr);
	}
	provider_unlock(provider);
}

void
agent_forward_memput(consumer_t consumer, provider_t provider, struct memput_stream_params *params)
{
	ssize_t ssz;

	provider_lock(provider);

	ssz = clara_sendmsg(provider->sd, CLARA_MSG_MEMPUT_STREAM, params, sizeof(*params),
		&provider->addr, 0, 0, CLARA_CONTROL_STREAM, 0, 0);

	if (ssz == -1)
	{
		debug_printff("clara_sendmsg failed");
	}
	else
	{
		stream_forward(consumer->sd, provider->sd, &provider->addr);
	}

	provider_unlock(provider);
}

