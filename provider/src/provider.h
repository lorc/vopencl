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

#if !defined(__PROVIDER_H__)
#define __PROVIDER_H__

#include <assert.h>
#include <errno.h>
#include <sched.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#include <CL/cl.h>

#include "clara.h"
#include "api.h"
#include "message.h"
#include "set.h"

#include "device.h"
#include "platform.h"

#define PROVIDER_DEFAULT_PORT	"52428"

#define PROVIDER_RECVBUFSZ		10240

typedef struct provider_options {

	const char *addr;
	const char *port;

	bool list_devices;
	int device;

} *provider_options_t;

typedef struct _agent {

	struct sockaddr_in addr;

} *agent_t;

struct clara_provider {

	int sd;
	agent_t agent;

	set_t platforms;	
	struct provider_options opts;

	set_t id_map;
	set_t allocations;

};

extern provider_t provider;

extern const cl_device_info device_info_list[];

platform_t provider_find_platform(global_id_t id);
device_t provider_find_device(platform_t platform, global_id_t id);

/* provider.c */
bool provider_init(provider_options_t);
void provider_deinit(void);

bool provider_main(provider_options_t);
void provider_shutdown(void);

bool provider_add_allocation(provider_t provider, struct clara_allocation *allocation);
bool provider_remove_allocation(provider_t provider, struct clara_allocation *allocation);
 
/* query.c */
cl_platform_id *clara_GetPlatformIDs(void);
void *clara_GetPlatformInfo(cl_platform_id platform, cl_platform_info param_name);
cl_device_id *clara_GetDeviceIDs(cl_platform_id platform, cl_device_type type);
void *clara_GetDeviceInfo(cl_device_id device, cl_device_info param_name);

#endif /* __PROVIDER_H__ */

