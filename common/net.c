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
#include "message.h"

/*
 * name:			clara_socket
 * description:		creates a socket suitable for providers, agents and consumers
 * arguments:		none
 * return value:	a socket descriptor, or -1 if not successful
 */
int
clara_socket(void)
{
	int sd;
	struct sctp_event_subscribe es;

	sd = socket(CLARA_DOMAIN, SOCK_SEQPACKET, IPPROTO_SCTP);

	if (sd == -1)
	{
		debug_printf("clara_socket: socket failed, errno = %d\n", errno);
	}
	else
	{	
		memset(&es, 0, sizeof(es));
		es.sctp_data_io_event = 1;
/*		es.sctp_send_failure_event = 1;
		es.sctp_peer_error_event = 1;
*/		es.sctp_shutdown_event = 1;
		if ((setsockopt(sd, IPPROTO_SCTP, SCTP_EVENTS, &es, sizeof(es)) == -1))
		{
			debug_printf("clara_socket: setsockopt(SCTP_EVENTS) failed, errno = %d\n", errno);
			close(sd);
			sd = -1;
		}

#if !defined(__FreeBSD__)
		struct sctp_assoc_value av;
		memset(&av, 0, sizeof(av));
		av.assoc_value = 1;
		if ((setsockopt(sd, IPPROTO_SCTP, SCTP_DELAYED_ACK_TIME, &av, sizeof(av)) == -1))
		{
			debug_printf("clara_socket: setsockopt(SCTP_DELAYED_ACK_TIME) failed, errno = %d\n", errno);
			close(sd);
			sd = -1;
		}
#endif /* __FreeBSD__ */
	}

	return (sd);
}

/*
 * name:			clara_sockaddr
 * decription:		creates a sockaddr_in structure from IP address and port
 * arguments:		addr - a null-terminated string that represents an IP address
 * 					port - a null-terminated string that represents an SCTP port
 * 					sa - the pointer to a sockaddr_in structure that will be populated
 * return value:	1 if successful, otherwise 0
 */
int
clara_sockaddr(const char *addr, const char *port, struct sockaddr_in *sa)
{
	long int conv;
	char *endptr;

	assert(sa != NULL);

	if (addr == NULL)
	{
		debug_printf("clara_sockaddr: addr is NULL\n");
		return 0;
	}

	if (port == NULL)
	{
		debug_printf("clara_sockaddr: port is NULL\n");
		return 0;
	}

	if (inet_pton(CLARA_DOMAIN, addr, &sa->sin_addr) != 1)
	{
		debug_printf("clara_sockaddr: inet_pton failed, errno = %d\n", errno);
		return 0;
	}

	conv = strtol(port, &endptr, 10);
	if (*endptr != '\0')
	{
		debug_printf("clara_sockaddr: strtol failed, can't convert \"%s\"\n", port);
		return 0;
	}

	if ((conv > 65535) || (conv < 1))
	{
		debug_printf("clara_sockaddr: invalid port number: %lu\n", conv);
		return 0;
	}

	sa->sin_port = (in_port_t)conv;
	sa->sin_family = CLARA_DOMAIN;

	return 1;
}

/*
 * name:			clara_send
 * description:		sends data
 * arguments:		many
 * return value:	see sctp_sendmsg
 */
int
clara_send(int sd, const void* msg, size_t len, struct sockaddr_in *to,
	uint32_t ppid, uint32_t flags, uint16_t stream_no, uint32_t timetolive, uint32_t context)
{
	int retval;
	retval = sctp_sendmsg(sd, msg, len, (struct sockaddr *)to, sizeof(*to), ppid, flags, stream_no, timetolive, context);
	return retval;
}

/*
 * name:			clara_sendmsg
 * description:		sends a message
 * arguments:		many
 * return value:	see sctp_sendmsg
 */
int
clara_sendmsg(int sd, message_type_t type, const void* msg, size_t len, struct sockaddr_in *to,
	uint32_t ppid, uint32_t flags, uint16_t stream_no, uint32_t timetolive, uint32_t context)
{
	int retval;
	char *buf;

	buf = (char *)malloc(sizeof(type) + len);
	memcpy(buf, &type, sizeof(type));
	memcpy(buf + sizeof(type), msg, len);

	retval = sctp_sendmsg(sd, buf, sizeof(type) + len, (struct sockaddr *)to, sizeof(*to), ppid, flags, stream_no, timetolive, context);
	if (retval == -1)
	{
//		debug_printf("clara_sendmsg: sctp_sendmsg failed\n");
	}
	else
	{
		assert(retval >= sizeof(type));
		retval -= sizeof(type);
	}

	free((void *)buf);

	return (retval);
}

/*
 * name:			clara_recv
 * description:		receives data
 * arguments:		many
 * return value:	see sctp_recvmsg
 */
int
clara_recv(int sd, void *msg, size_t len,
	struct sockaddr_in* from, struct sctp_sndrcvinfo *sinfo, int *msg_flags)
{
	int retval;
	struct sockaddr_in __from;
	socklen_t fromlen = sizeof(*from);
	struct sctp_sndrcvinfo __sinfo;
	int __msg_flags = 0;

	if (from == NULL)
		from = &__from;

	if (sinfo == NULL)
		sinfo = &__sinfo;

	if (msg_flags == NULL)
		msg_flags = &__msg_flags;

	retval = sctp_recvmsg(sd, msg, len, (struct sockaddr *)from, &fromlen, sinfo, msg_flags);
	if (retval == -1)
	{
//		debug_printf("clara_recv: sctp_recvmsg failed\n");
	}
	else if ((*msg_flags & MSG_EOR) == 0)
	{
		debug_printf("clara_recv: buffer is too small: need more than %d bytes\n", len);
		retval = -1;
	}

	return retval;
}

/*
 * name:			clara_recvmsg
 * description:		receives a message
 * arguments:		many
 * return value:	see sctp_recvmsg
 */
int
clara_recvmsg(int sd, message_type_t *ptype, void *msg, size_t len, struct sockaddr_in* from, struct sctp_sndrcvinfo *sinfo, int *msg_flags)
{
	int retval;
	char *buf;
	socklen_t fromlen;
	message_type_t expected = *ptype;

	struct sockaddr_in __from;
	struct sctp_sndrcvinfo __sinfo;
	int __msg_flags = 0;

	if (from == NULL)
		from = &__from;

	if (sinfo == NULL)
		sinfo = &__sinfo;

	if (msg_flags == NULL)
		msg_flags = &__msg_flags;

	buf = (char *)malloc(sizeof(*ptype) + len);
	fromlen = sizeof(*from);

	retval = sctp_recvmsg(sd, buf, sizeof(*ptype) + len, (struct sockaddr *)from, &fromlen, sinfo, msg_flags);
	if (retval == -1)
	{
//		debug_printf("clara_recvmsg: sctp_recvmsg failed\n");
	}
	else if ((*msg_flags & MSG_EOR) == 0)
	{
		debug_printf("clara_recvmsg: buffer too small: %d bytes\n", len);
		retval = -1;
	}
	else if (*msg_flags & MSG_NOTIFICATION)
	{
		assert(retval <= len);	/* fix this */
		memcpy(msg, buf, retval);
	}
	else
	{
		assert(retval >= sizeof(*ptype));
		memcpy(ptype, buf, sizeof(*ptype));
		if (expected != CLARA_MSG_INVALID)
		{
			if (*ptype != expected)
			{
				debug_printf("clara_recvmsg: unexpected message: %x, expected %x\n", *ptype, expected);
				retval = -1;
				goto out;
			}
		}

		retval -= sizeof(*ptype);
		memcpy(msg, buf + sizeof(*ptype), retval);
	}
out:
	free((void *)buf);

	return (retval);
}

