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
agent_create(agent_t *result, agent_options_t opts)
{
	bool retval = false;
	agent_t agent;

	if (opts->load_balancing && opts->reverse_proxy)
	{
		clara_printf("Cannot enable load-balancing and reverse proxy at the same time.\n");
		errno = EINVAL;
		return false;
	}

	agent = calloc(1, sizeof(*agent));
	if (agent == NULL)
	{
		errno = ENOMEM;
		return false;
	}

	/* copy options */
	memcpy(&agent->opts, opts, sizeof(agent->opts));

	assert(set_create(&agent->providers));
	assert(set_create(&agent->consumers));
	assert(set_create(&agent->id_pool));
	assert(set_create(&agent->objects));

	if (agent->opts.load_balancing)
	{
		assert(lb_create(&agent->lb));
	}

	if (agent->opts.reverse_proxy)
	{
		assert(rp_create(&agent->rp, agent));
	}

	*result = agent;
	retval = true;

	return retval;
}

bool
agent_startup(agent_t agent)
{
	if (!clara_sockaddr(agent->opts.addr, agent->opts.port, &agent->addr))
	{
		debug_printff("clara_sockaddr failed, errno = %d", errno);
		return false;
	}

	if ((agent->sd = clara_socket()) == -1)
	{
		debug_printff("clara_socket failed, errno = %d", errno);
		return false;
	}

	if (bind(agent->sd, (struct sockaddr *)&agent->addr, sizeof(agent->addr)) == -1)
	{
		/* 98 */
		clara_printf("Cannot bind socket to %s:%u", inet_ntoa(agent->addr.sin_addr), agent->addr.sin_port);
		close(agent->sd);
		return false;
	}

	if (listen(agent->sd, AGENT_BACKLOG) == -1)
	{
		debug_printff("listen failed, errno = %d", errno);
		close(agent->sd);
		return false;
	}
	return true;
}

bool
agent_shutdown(agent_t agent)
{
	bool retval = false;

	if (agent == NULL)
		return false;

	if (agent->sd == -1)
		return false;

	if (close(agent->sd) != 0)
	{
		debug_printff("close failed, errno = %d", errno);
	}
	else
	{
		agent->sd = -1;
		retval = true;
	}

	return retval;
}

bool
agent_destroy(agent_t agent)
{
	if (agent->opts.load_balancing)
	{
		assert(lb_destroy(agent->lb));
	}

	if (agent->opts.reverse_proxy)
	{
		assert(rp_destroy(agent->rp));
	}

	set_destroy(agent->providers);
	set_destroy(agent->consumers);
	set_destroy(agent->id_pool);
	set_destroy(agent->objects);

	return true;
}

int
agent_main(agent_t agent)
{
	int rc = 0;

	size_t bufsz = AGENT_RECVBUFSZ;
	char buf[bufsz];
	struct sockaddr_in from;
	ssize_t sz;

	clara_printf("Listening at %s:%s.\n", agent->opts.addr, agent->opts.port);
	clara_printf("Load-balancing: %sabled\n", agent->opts.load_balancing ? "en" : "dis");
	clara_printf("Reverse proxy: %sabled\n", agent->opts.reverse_proxy ? "en" : "dis");

	/* regular operation starts here */
	clara_printf("Waiting for providers and consumers.\n");

	while (1)
	{
		message_type_t type;
		struct sctp_sndrcvinfo rcvinfo;
		int flags;

		type = 0;
		memset(&from, 0, sizeof(from));
		memset(&rcvinfo, 0, sizeof(rcvinfo));
		flags = 0;


		if ((sz = clara_recvmsg(agent->sd, &type, buf, bufsz, &from, &rcvinfo, &flags)) == -1)
		{
//			debug_printf("agent: clara_recvmsg failed (%s:%d), errno = %d\n", __FILE__, __LINE__, errno);
			rc = 5;
			goto clara_recvmsg_failed;
		}

//		debug_printf("agent: got a message from %s:%u (%d bytes) (flags=%08x)\n", inet_ntoa(from.sin_addr), from.sin_port, sz, flags);

		if (rcvinfo.sinfo_stream != CLARA_CONTROL_STREAM)
		{
			debug_printf("agent: invalid stream: %u\n", rcvinfo.sinfo_stream);
			continue;
		}

		if (flags & MSG_NOTIFICATION)
		{
			union sctp_notification *notif;
			notif = (union sctp_notification *)buf;

			switch (notif->sn_header.sn_type)
			{
			case SCTP_SHUTDOWN_EVENT:
//				debug_printf("agent: shutdown event\n");
				break;
			default:
				debug_printf("agent: notification: %u (%u)\n", notif->sn_header.sn_type, SCTP_ASSOC_CHANGE);
				break;
			}
		}
		else
		{
			int sd = -1;
			provider_t provider = NULL;
			consumer_t consumer = NULL;

			switch (type)
			{
			case CLARA_MSG_PROVIDER:
				/* check whether a provider already exists at the host */
				type = agent_find_provider_with_addr(agent, from.sin_addr.s_addr) == NULL ? CLARA_MSG_ACK : CLARA_MSG_NACK;
	
				if (type == CLARA_MSG_ACK)
				{
					type = CLARA_MSG_NACK;

					clara_printf("New provider: %s\n", inet_ntoa(from.sin_addr));

					if ((sd = sctp_peeloff(agent->sd, rcvinfo.sinfo_assoc_id)) == -1)
					{
						debug_printff("sctp_peeloff failed, errno = %d", errno);
					}
					else if (!provider_create(&provider, sd, &from))
					{
						debug_printff("provider_create failed");
					}
					else if (!provider_attach(provider, agent))
					{
						debug_printff("provider_attach failed");
					}
					else if (pthread_create(&provider->thread, NULL, &agent_provider_handler, (void *)provider) != 0)
					{
						debug_printff("pthread_create failed, errno = %d", errno);
					}
					else
					{
						/* finally accept the new provider */
						type = CLARA_MSG_ACK;
					}
				}
				else if (type == CLARA_MSG_NACK)
				{
					sd = agent->sd;
					clara_printf("Reject provider: %s\n", inet_ntoa(from.sin_addr));
				}

				if (clara_sendmsg(sd, type, NULL, 0, &from, 0, 0, CLARA_CONTROL_STREAM, 0, 0) == -1)
				{
					debug_printff("message_send (%s:%d) failed, errno = %d", __FILE__, __LINE__, errno);
					rc = 6;
					goto clara_sendmsg_failed;
				}

				break;

			case CLARA_MSG_CONSUMER:
				clara_printf("New consumer: %s\n", inet_ntoa(from.sin_addr));
				type = CLARA_MSG_NACK;

				if ((sd = sctp_peeloff(agent->sd, rcvinfo.sinfo_assoc_id)) == -1)
				{
					debug_printff("sctp_peeloff failed, errno = %d", errno);
				}
				else if (!consumer_create(&consumer, sd, &from))
				{
					debug_printff("consumer_create failed");
				}
				else if (!consumer_attach(consumer, agent))
				{
					debug_printff("consumer_attach failed");
				}
				else if (pthread_create(&consumer->thread, NULL, &agent_consumer_handler, (void *)consumer) != 0)
				{
					debug_printff("pthread_create failed, errno = %d", errno);
				}
				else
				{
					/* finally accept the new consumer */
					type = CLARA_MSG_ACK;
				}

				if (clara_sendmsg(sd, type, NULL, 0, &from, 0, 0, CLARA_CONTROL_STREAM, 0, 0) == -1)
				{
					debug_printff("message_send (%s:%d) failed, errno = %d", __FILE__, __LINE__, errno);
					rc = 6;
					goto clara_sendmsg_failed;
				}

				break;

			default:
				debug_printff("received message of unknown type: %u", type);
				int i;
				for (i = 0; i < sz; i++)
					debug_printf_raw("%02x ", (unsigned char)buf[i]);
				debug_printf_raw("\n");
				break;
			}
		}
	}
	
clara_sendmsg_failed:
clara_recvmsg_failed:

	agent_shutdown(agent);

	return (rc);
}

