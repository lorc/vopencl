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

bool
agent_remote_allocate(provider_t provider, allocate_request_t params, size_t extra, handle_t *phandle)
{
	ssize_t ssz;
	message_type_t type;
	handle_t handle;
	struct sockaddr_in from;
	struct sctp_sndrcvinfo sinfo;
	int msg_flags;

	provider_lock(provider);

	ssz = clara_sendmsg(provider->sd, CLARA_MSG_ALLOCATE_REQUEST, (void *)params, sizeof(*params) + extra,
		&provider->addr, 0, 0, CLARA_CONTROL_STREAM, 0, 0);

	if (ssz == -1)
	{
		debug_printf("clara_remote_allocate: clara_sendmsg failed, errno = %d\n", errno);
		return false;
	}

	memset(&from, 0, sizeof(from));
	type = CLARA_MSG_ALLOCATE_RESPONSE;
	ssz = clara_recvmsg(provider->sd, &type, phandle, sizeof(*phandle), &from, &sinfo, &msg_flags);

	provider_unlock(provider);

	if (ssz == -1)
	{
		debug_printf("clara_remote_allocate: clara_recvmsg failed, errno = %d\n", errno);
		return false;
	}

	assert(ssz == sizeof(handle));

	return true;
}

bool
agent_remote_execute(provider_t provider, void *msgbuf, size_t msgsz, arg_t *pretval)
{
	ssize_t ssz;
	message_type_t type;
	struct sockaddr_in from;
	struct sctp_sndrcvinfo sinfo;
	int msg_flags;

	if (provider == NULL)
		return false;
	
	provider_lock(provider);

	ssz = clara_sendmsg(provider->sd, CLARA_MSG_EXECUTE_REQUEST, msgbuf, msgsz, &provider->addr,
		0, 0, CLARA_CONTROL_STREAM, 0, 0);

	if (ssz == -1)
	{
		debug_printf("clara_remote_execute: clara_sendmsg failed, errno = %d\n", errno);
		return false;
	}

	memset(&from, 0, sizeof(from));
	type = CLARA_MSG_EXECUTE_RESPONSE;
	ssz = clara_recvmsg(provider->sd, &type, pretval, sizeof(*pretval), &from, &sinfo, &msg_flags);

	provider_unlock(provider);

	if (ssz == -1)
	{
		debug_printf("clara_remote_execute: clara_recvmsg failed, errno = %d\n", errno);
		return false;
	}

	assert(ssz == sizeof(*pretval));

	return true;
}

bool
agent_remote_map(provider_t provider, void *real, global_id_t virt)
{
	ssize_t ssz;
	struct clara_mapping_params params;
	params.virt = virt;
	params.real = real;

	ssz = clara_sendmsg(provider->sd, CLARA_MSG_MAPPING, (void *)&params, sizeof(params),
		&provider->addr, 0, 0, CLARA_CONTROL_STREAM, 0, 0);

	if (ssz == -1)
	{
		debug_printf("clara_remote_map: clara_sendmsg failed, errno = %d\n", errno);
		return false;
	}

	assert(ssz == sizeof(params));

	return true;
}

