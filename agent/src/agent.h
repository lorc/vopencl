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

#if !defined(__AGENT_H__)
#define __AGENT_H__

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <CL/cl.h>

typedef struct clara_agent *		agent_t;

#include "clara.h"
#include "api.h"
#include "message.h"
#include "set.h"

#include "platform.h"
#include "device.h"
#include "provider.h"
#include "consumer.h"
#include "object.h"

#define AGENT_DEFAULT_ADDR		"0.0.0.0"
#define AGENT_DEFAULT_PORT		CLARA_DEFAULT_PORT

#define AGENT_PING_INTERVAL		3		/* seconds */
#define AGENT_BACKLOG			10
#define AGENT_RECVBUFSZ			10240

typedef struct _loadbalancer *loadbalancer_t;
typedef struct _revproxy *revproxy_t;

typedef struct agent_options {

	const char *addr;
	const char *port;

	bool load_balancing;
	bool reverse_proxy;
} *agent_options_t;

struct clara_agent {
	struct sockaddr_in addr;
	int sd;
	struct agent_options opts;

	set_t providers;					/* provider_t */
	set_t consumers;					/* consumer_t */

	set_t id_pool;
	set_t objects;						/* cl_object */

	loadbalancer_t lb;
	revproxy_t rp;
};

bool agent_create(agent_t *result, agent_options_t opts);
bool agent_startup(agent_t agent);
bool agent_shutdown(agent_t agent);
bool agent_destroy(agent_t agent);

int agent_main(agent_t);

/* consumer_handler.c */
void *agent_consumer_handler(void *arg);
struct clara_consumer *agent_create_consumer(int sd, struct sockaddr_in *paddr);
bool agent_add_consumer(agent_t agent, struct clara_consumer *consumer);

/* provider_handler.c */
provider_t agent_find_provider_with_addr(agent_t agent, in_addr_t addr);
void *agent_provider_handler(void *arg);

/* remote.c */
bool agent_remote_allocate(provider_t provider, allocate_request_t params, size_t extra, handle_t *phandle);
bool agent_remote_execute(provider_t provider, void *msgbuf, size_t msgsz, arg_t *pretval);
bool agent_remote_map(provider_t provider, void *real, global_id_t virt);

/* forward.c */
void agent_forward_fetch(consumer_t consumer, provider_t provider, fetch_request_t params);
void agent_forward_memput(consumer_t consumer, provider_t provider, struct memput_stream_params *params);

/* id.c */
bool id_create(set_t id_pool, global_id_t *result);
bool id_release(set_t id_pool, global_id_t id);

/* loadbalancer.c */
struct _loadbalancer {
	set_t platforms;
};

bool lb_create(loadbalancer_t *plb);
bool lb_destroy(loadbalancer_t lb);

bool lb_add_platform(loadbalancer_t lb, platform_t platform);
bool lb_remove_platform(loadbalancer_t lb, platform_t platform);

bool lb_allocate_platform(set_t platforms, consumer_t consumer);
bool lb_release_platform(consumer_t consumer);

/* reverseproxy.c */
struct _revproxy {
	platform_t platform;		/* the platform and the devices presented to the consumer */
	sem_t *lock;
};

bool rp_create(revproxy_t *prp, agent_t agent);
bool rp_destroy(revproxy_t rp);

#endif /* __AGENT_H__ */

