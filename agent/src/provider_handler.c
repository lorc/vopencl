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

#include <time.h>

#include "agent.h"
#include "object.h"

provider_t 
agent_find_provider_with_addr(agent_t agent, in_addr_t addr)
{
	provider_t provider;

	set_start(agent->providers);
	
	while ((provider = set_next(agent->providers)) != NULL)
	{
		if (provider->addr.sin_addr.s_addr == addr)
			break;
	}

	set_finish(agent->providers);

	return provider;
}

void
agent_print_tree(provider_t provider)
{
	platform_t platform;
	device_t device;

	set_start(provider->platforms);

	while ((platform = set_next(provider->platforms)) != NULL)
	{
		clara_printf("\tplatform %08lX --> %08lX: %s\n",
			platform->id, platform->global_id, platform->name);

		set_start(platform->devices);
	
		while ((device = set_next(platform->devices)) != NULL)
		{
			clara_printf("\t  device %08lX --> %08lX: %s\n",
				device->id, device->global_id, device->name);
		}

		set_finish(platform->devices);
	}
	set_finish(provider->platforms);
}

platform_t 
agent_find_platform_by_local_id(provider_t provider, cl_platform_id id)
{
	platform_t platform;

	set_start(provider->platforms);

	while ((platform = set_next(provider->platforms)) != NULL)
	{
		if (platform->id == id)
			break;
	}

	set_finish(provider->platforms);

	return platform;
}

void *
agent_provider_handler(void *arg)
{
	int retval = 0;
	provider_t provider = (provider_t)arg;
	int sd = provider->sd;

	size_t sz;
	message_type_t type;
	void *buf;
	size_t bufsz = 10000;
	struct sockaddr_in from;
	struct sctp_sndrcvinfo rcvinfo;
	int flags;

	buf = (void *)malloc(bufsz);

	bool idle = false;
	bool exit = false;

	while (!exit)
	{
		if ((sz = clara_recvmsg(sd, &type, buf, bufsz, &from, &rcvinfo, &flags)) == -1)
		{
//			debug_printf("agent_provider_handler(%d): clara_recvmsg failed, errno = %d\n", sd, errno);
			break;
		}

		if (flags & MSG_NOTIFICATION)
		{
			union sctp_notification *notif;
			notif = (union sctp_notification *)buf;

			switch (notif->sn_header.sn_type)
			{
			case SCTP_SHUTDOWN_EVENT:
				exit = true;
				break;
			default:
				debug_printf("agent_provider_handler(%d): received %d bytes from %s:%d (flags=%08x)\n", sd, sz, inet_ntoa(from.sin_addr), from.sin_port, flags);
				debug_printf("agent_provider_handler(%d): unknown notification type: %d\n", sd, notif->sn_header.sn_type);
				break;
			}
		}
		else
		{
			platform_t platform;
			device_t device;
			ssize_t ssz;

			switch (type)
			{
			case CLARA_MSG_OPENCL_PLATFORM_INFO:
				assert(platform_create(&platform, provider));
				assert(platform_init(platform, buf));
				assert(set_insert(provider->platforms, platform->global_id, platform));
				
				assert(object_create(NULL, provider, platform->id, platform->global_id));

				/* send the global platform ID to the provider */
				ssz = clara_sendmsg(sd, CLARA_MSG_OPENCL_PLATFORM_ID, &platform->global_id, sizeof(global_id_t), &from,
					0, 0, CLARA_CONTROL_STREAM, 0, 0);

				if (ssz == -1)
				{
//					debug_printf("agent_provider_handler(%d): clara_sendmsg failed, %s:%d, errno = %d\n", sd, __FILE__, __LINE__, errno);
				}

				break;
			case CLARA_MSG_OPENCL_DEVICE_INFO:
				assert(device_create(&device, provider));
				assert(device_init(device, buf));
				device->provider = provider;
				platform = agent_find_platform_by_local_id(provider, device->platform_id);
				assert(platform != NULL);
				assert(set_insert(platform->devices, device->global_id, device));
		
				assert(object_create(NULL, provider, device->id, device->global_id));
	
				/* send the global device ID to the provider */
				ssz = clara_sendmsg(sd, CLARA_MSG_OPENCL_DEVICE_ID, &device->global_id, sizeof(global_id_t), &from,
					0, 0, CLARA_CONTROL_STREAM, 0, 0);

				if (ssz == -1)
				{
//					debug_printf("agent_provider_handler(%d): clara_sendmsg failed, %s:%d, errno = %d\n", sd, __FILE__, __LINE__, errno);
				}

				break;
			case CLARA_MSG_IDLE:

				agent_print_tree(provider);

				idle = true;
				exit = true;
				break;
			default:
				debug_printf("agent_provider_handler(%d): received %d bytes from %s:%d (flags=%08x)\n", sd, sz, inet_ntoa(from.sin_addr), from.sin_port, flags);
				break;
			}
		}
	}

	ssize_t ssz;
	int count = 0;
	struct sctp_sndrcvinfo recvinfo;
	type = 0;
	struct timespec ts;
	time_t sec;

	if (idle)
	{
		/* make the platforms of this provider available for the load-balancer */
		if (provider->agent->opts.load_balancing)
		{
			platform_t platform;
			set_start(provider->platforms);
			while ((platform = set_next(provider->platforms)) != NULL)
			{
				if (!lb_add_platform(provider->agent->lb, platform))
				{
					debug_printff("lb_add_platform failed");
				}
			}
			set_finish(provider->platforms);
		}

		exit = false;
		while (!exit)
		{
			usleep(AGENT_PING_INTERVAL * 1000000);

			clock_gettime(CLOCK_MONOTONIC, &ts);
			provider_lock(provider);
//			debug_printf("agent_provider_handler(%d): PING\n");
			ssz = clara_sendmsg(provider->sd, CLARA_MSG_PING, &ts.tv_sec, sizeof(time_t), &provider->addr, 0, 0, CLARA_CONTROL_STREAM, 0, 0);
			count++;

			if (ssz == -1)
			{
//				debug_printf("agent_provider_handler(%d): clara_sendmsg: ssz=%d count=%d\n", sd, ssz, count);
			}

			ssz = clara_recvmsg(provider->sd, &type, buf, bufsz, &from, &recvinfo, &flags);
			provider_unlock(provider);

			if (ssz == -1)
			{
//				debug_printf("agent_provider_handler(%d): clara_recvmsg: ssz=%d count=%d, flags=%08x\n", sd, ssz, count, flags);
			}
			else

			if (flags & MSG_NOTIFICATION)
			{
				union sctp_notification *notif = (union sctp_notification *)buf;

				switch (notif->sn_header.sn_type)
				{
				case SCTP_SHUTDOWN_EVENT:
					exit = true;
					break;
				default:
					debug_printf("agent_provider_handler(%d): received %d bytes from %s:%d (flags=%08x)\n", sd, sz, inet_ntoa(from.sin_addr), from.sin_port, flags);
					debug_printf("agent_provider_handler(%d): unknown notification type: %d\n", sd, notif->sn_header.sn_type);
					break;
				}
			}
			else
			{
				switch (type)
				{
				case CLARA_MSG_PONG:
					memcpy(&sec, buf, sizeof(time_t));
//					debug_printf("agent_provider_handler(%d): PONG\n");
//					debug_printf("agent_provider_handler(%d): PONG %llu (%s)\n", sd, sec, (sec == ts.tv_sec) ? "ok" : "failed");
					break;
				default:
//					debug_printf("agent_provider_handler(%d): clara_recvmsg: ssz=%d type=%d\n", sd, ssz, type);
					break;
				}
			}
		}
	}

	free((void *)buf);

	shutdown(sd, SHUT_RDWR);
	
	assert(provider_detach(provider));
	assert(provider_destroy(provider));

	//debug_printf("agent_provider_handler(%d): exit with code %d\n", sd, retval);
	pthread_exit(&retval);
}

