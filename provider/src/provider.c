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


#include "provider.h"
#include "provider_opencl.h"
#include "api.h"

provider_t provider;

const struct fun_table_entry exec_fun_table_entries[] = {

	/* device */
	[CL_GETDEVICEINFO]				= { &CLARA_CLFUN_NAME(clGetDeviceInfo), ARG1 | ARG4 | ARG5 },

	/* context */
	[CL_CREATECONTEXT]				= { &CLARA_CLFUN_NAME(clCreateContext), ARG1 | ARG3 | ARG4 | ARG5 | ARG6 },
	[CL_CREATECONTEXTFROMTYPE]		= { &CLARA_CLFUN_NAME(clCreateContextFromType), ARG1 | ARG3 | ARG4 | ARG5 },
	[CL_RETAINCONTEXT]				= { &CLARA_CLFUN_NAME(clRetainContext), ARG1 },
	[CL_RELEASECONTEXT]				= { &CLARA_CLFUN_NAME(clReleaseContext), ARG1 },

	/* command queue */
	[CL_CREATECOMMANDQUEUE]			= { &CLARA_CLFUN_NAME(clCreateCommandQueue), ARG1 | ARG2 | ARG4 },
	[CL_RETAINCOMMANDQUEUE]			= { &CLARA_CLFUN_NAME(clRetainCommandQueue), ARG1 },
	[CL_RELEASECOMMANDQUEUE]		= { &CLARA_CLFUN_NAME(clReleaseCommandQueue), ARG1 },
	[CL_GETCOMMANDQUEUEINFO]		= { &CLARA_CLFUN_NAME(clGetCommandQueueInfo), ARG1 | ARG4 | ARG5 },

	/* memory object */
	[CL_CREATEBUFFER]				= { &CLARA_CLFUN_NAME(clCreateBuffer), ARG1 | ARG4 | ARG5 },
	[CL_RETAINMEMOBJECT]			= { &CLARA_CLFUN_NAME(clRetainMemObject), ARG1 },
	[CL_RELEASEMEMOBJECT]			= { &CLARA_CLFUN_NAME(clReleaseMemObject), ARG1 },
	[CL_ENQUEUEREADBUFFER]			= { &CLARA_CLFUN_NAME(clEnqueueReadBuffer), ARG1 | ARG2 | ARG6 | ARG8 | ARG9 },
	[CL_ENQUEUEWRITEBUFFER]			= { &CLARA_CLFUN_NAME(clEnqueueWriteBuffer), ARG1 | ARG2 | ARG6 | ARG8 | ARG9 },
	[CL_ENQUEUECOPYBUFFER]			= { &CLARA_CLFUN_NAME(clEnqueueCopyBuffer), ARG1 | ARG2 | ARG3 | ARG8 | ARG9 },
	[CL_GETMEMOBJECTINFO]			= { &CLARA_CLFUN_NAME(clGetMemObjectInfo), ARG1 | ARG4 | ARG5 },

	/* program */
	[CL_CREATEPROGRAMWITHSOURCE]	= { &CLARA_CLFUN_NAME(clCreateProgramWithSource), ARG1 | ARG3 | ARG4 | ARG5 },
	[CL_CREATEPROGRAMWITHBINARY]	= { &CLARA_CLFUN_NAME(clCreateProgramWithBinary), ARG1 | ARG3 | ARG4 | ARG5 | ARG6 | ARG7 },
	[CL_RETAINPROGRAM]				= { &CLARA_CLFUN_NAME(clRetainProgram), ARG1 },
	[CL_RELEASEPROGRAM]				= { &CLARA_CLFUN_NAME(clReleaseProgram), ARG1 },
	[CL_BUILDPROGRAM]				= { &CLARA_CLFUN_NAME(clBuildProgram), ARG1 | ARG3 | ARG4 },
	[CL_UNLOADCOMPILER]				= { &CLARA_CLFUN_NAME(clUnloadCompiler), NONE },
	[CL_GETPROGRAMINFO]				= { &CLARA_CLFUN_NAME(clGetProgramInfo), ARG1 | ARG4 | ARG5 },
	[CL_GETPROGRAMBUILDINFO]		= { &CLARA_CLFUN_NAME(clGetProgramBuildInfo), ARG1 | ARG2 | ARG5 | ARG6 },
	
	/* kernel */
	[CL_CREATEKERNEL]				= { &CLARA_CLFUN_NAME(clCreateKernel), ARG1 | ARG2 | ARG3 },
	[CL_RETAINKERNEL]				= { &CLARA_CLFUN_NAME(clRetainKernel), ARG1 },
	[CL_RELEASEKERNEL]				= { &CLARA_CLFUN_NAME(clReleaseKernel), ARG1 },
	[CL_SETKERNELARG]				= { &CLARA_CLFUN_NAME(clSetKernelArg), ARG1 | ARG4 },
	[CL_ENQUEUENDRANGEKERNEL]		= { &CLARA_CLFUN_NAME(clEnqueueNDRangeKernel), ARG1 | ARG2 | ARG4 | ARG5 | ARG6 | ARG8 | ARG9 },
	[CL_ENQUEUETASK]				= { &CLARA_CLFUN_NAME(clEnqueueTask), ARG1 | ARG2 | ARG4 | ARG5 },
	[CL_GETKERNELINFO]				= { &CLARA_CLFUN_NAME(clGetKernelInfo), ARG1 | ARG4 | ARG5 },

	/* event */
	[CL_WAITFOREVENTS]				= { &CLARA_CLFUN_NAME(clWaitForEvents), ARG2 },
	[CL_GETEVENTINFO]				= { &CLARA_CLFUN_NAME(clGetEventInfo), ARG1 | ARG4 | ARG5 },
	[CL_RETAINEVENT]				= { &CLARA_CLFUN_NAME(clRetainEvent), ARG1 },
	[CL_RELEASEEVENT]				= { &CLARA_CLFUN_NAME(clReleaseEvent), ARG1 },
	[CL_ENQUEUEMARKER]				= { &CLARA_CLFUN_NAME(clEnqueueMarker), ARG1 | ARG2 },
	[CL_ENQUEUEWAITFOREVENTS]		= { &CLARA_CLFUN_NAME(clEnqueueWaitForEvents), ARG1 | ARG3 },
	[CL_ENQUEUEBARRIER]				= { &CLARA_CLFUN_NAME(clEnqueueBarrier), ARG1 },
	[CL_GETEVENTPROFILINGINFO]		= { &CLARA_CLFUN_NAME(clGetEventProfilingInfo), ARG1 | ARG4 | ARG5 },

	[CL_FLUSH]						= { &CLARA_CLFUN_NAME(clFlush), ARG1 },
	[CL_FINISH]						= { &CLARA_CLFUN_NAME(clFinish), ARG1 },
};

const struct fun_table exec_fun_table = {
	.num_entries = sizeof(exec_fun_table_entries) / sizeof(*exec_fun_table_entries),
	.entries = exec_fun_table_entries
};

platform_t 
provider_find_platform(global_id_t id)
{
	platform_t platform;
	set_start(provider->platforms);
	while ((platform = set_next(provider->platforms)) != NULL)
	{
		if (platform->global_id == id)
			break;
	}
	set_finish(provider->platforms);

	return platform;
}

device_t
provider_find_device(platform_t platform, global_id_t id)
{
	device_t device;

	set_start(platform->devices);
	while ((device = set_next(platform->devices)) != NULL)
	{
		if (device->global_id == id)
			break;
	}
	set_finish(platform->devices);

	return device;
}

void
provider_signal_handler(int signum)
{
	switch (signum)
	{
	case SIGINT:
	case SIGTERM:
		provider_shutdown();
		exit(0);
		break;
	default:
		debug_printf("provider_signal_handler: don't know how to handle signal %d\n", signum);
		break;
	}
}

arg_t
provider_id_map(global_id_t id, void *data)
{
	provider_t provider = (provider_t)data;
	mapping_t mapping;
	assert(mapping_find_by_virt(provider->id_map, id, &mapping));
	return PTR2ARG(mapping->real);
}

void
usage(char *argv[])
{
	clara_printf("usage: %s [-l] [-a ip [-p port] [-d n]]\n", argv[0]);
	clara_printf("\t-a ip\t\tconnect to the specified IP address\n");
	clara_printf("\t-d n\t\tprovide the nth device only (see -l)\n");
	clara_printf("\t-l\t\tlist available devices\n");
	clara_printf("\t-p port\t\tuse the specified port instead of the default port (%s)\n", PROVIDER_DEFAULT_PORT);
	clara_printf("\n");
}

int
main(int argc, char *argv[])
{
	clara_debug_init();

	struct provider_options opts = {
		.addr = NULL,
		.port = PROVIDER_DEFAULT_PORT,
		.list_devices = true,
		.device = -1
	};

	int c;

	bool f_help = true;

	while ((c = getopt(argc, argv, "hla:p:d:")) != -1)
	{
		switch (c)
		{
		case 'a':
			f_help = false;
			opts.addr = optarg;
			opts.list_devices = false;
			break;
		case 'h':
			f_help = true;
			/* does not return */
			break;
		case 'p':
			opts.port = optarg;
			break;
		case 'd':
			opts.device = atoi(optarg);
			break;
		case 'l':
			f_help = false;
			opts.addr = NULL;
			opts.list_devices = true;
			break;
		default:
			return 1;
			break;
		}
	}

	if (f_help)
	{
		usage(argv);
		exit(0);
	}

	struct sigaction act;
	sigemptyset(&act.sa_mask);
	act.sa_handler = &provider_signal_handler;
	act.sa_flags = 0;
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGTERM, &act, NULL);

	/* init pseudo-random number generator */
	clara_srand();

	unsigned long sec;		/* sleep time before retry */
	bool end = false;

	while (!end)
	{	
		if (!provider_main(&opts))
		{
			switch (errno)
			{
			case EAGAIN:
				sec = ((rand() % 11) + 5);		/* 5 .. 15 */
				clara_printf("Reconnect in %u seconds\n", sec);
				usleep(sec * 1000000);
				break;
			case EPERM:
				clara_printf("Permission denied.\n");
				end = true;
				break;
			case EINTR:
				clara_printf("Interrupted.\n");
				end = true;
				break;
			default:
				debug_printf("main: unknown error, errno = %d\n", errno);
				end = true;
				break;
			}
		}
		else
		{
			end = true;
		}
	}
 
	return (0);
}

int
provider_agent_sendmsg(message_type_t type, const char *buf, size_t len)
{
	int retval;

	retval = clara_sendmsg(provider->sd, type, buf, len, &provider->agent->addr, 0, 0, CLARA_CONTROL_STREAM, 0, 0);

	return (retval);
}

int
provider_agent_recvmsg(message_type_t *ptype, char *buf, size_t len)
{
	struct sockaddr_in from;
	struct sctp_sndrcvinfo rcvinfo;
	int flags;

	return clara_recvmsg(provider->sd, ptype, buf, len, &from, &rcvinfo, &flags);
}

void
provider_shutdown(void)
{
	platform_t platform;
	device_t device;

	if (provider == NULL)
		return;

//	debug_printf("provider_shutdown\n");

	/* release memory */

	if (provider->platforms != NULL)
	{
		set_start(provider->platforms);
		while ((platform = set_next(provider->platforms)) != NULL)
		{
			if (platform->devices != NULL)
			{
				set_start(platform->devices);
				while ((device = set_next(platform->devices)) != NULL)
				{
					set_remove(platform->devices, (global_id_t)device);
					free((void *)device);
				}
				set_finish(platform->devices);
	
				set_destroy(platform->devices);
				platform->devices = NULL;
			}
			set_remove(provider->platforms, (global_id_t)platform);
			free((void *)platform);
		}
		set_finish(provider->platforms);
		set_destroy(provider->platforms);
		provider->platforms = NULL;
	}

	/* close sockets */
	close(provider->sd);
}

bool
provider_startup(void)
{
	int i;
	message_type_t type;
	mapping_t mapping;

	/* send platform information */
	assert(set_create(&provider->platforms));
	
	platform_t platform;
	platform = calloc(1, sizeof(*platform));
	set_insert(provider->platforms, (global_id_t)platform, platform);

	if (clGetPlatformIDs(1, &platform->id, NULL) != CL_SUCCESS)
	{
		debug_printf("provider_startup: clGetPlatformIDs failed\n");
		return false;
	}
	cl_platform_id platform_id = platform->id;

	cl_platform_info platform_info_list[] = {
		CL_PLATFORM_PROFILE,
		CL_PLATFORM_VERSION,
		CL_PLATFORM_NAME,
		CL_PLATFORM_VENDOR,
		CL_PLATFORM_EXTENSIONS,
		0	
	};

	size_t bufsz = sizeof(cl_platform_id);
	size_t size_ret = 0;

	cl_platform_info *pi;
	for (pi = platform_info_list; *pi != 0; pi++)
	{
		clGetPlatformInfo(platform_id, *pi, 0, NULL, &size_ret);
		bufsz += size_ret;
	}

	char *buf, *ptr;
	ptr = buf = (char *)malloc(bufsz);
	
	for (pi = platform_info_list; *pi != 0; pi++)
	{
		clGetPlatformInfo(platform_id, *pi, bufsz, ptr, &size_ret);
		ptr += size_ret;
	}

	/* finally add the local platform id */
	memcpy(ptr, &platform_id, sizeof(cl_platform_id));
	ptr += sizeof(cl_platform_id);

	/* send platform information to the agent */	
	provider_agent_sendmsg(CLARA_MSG_OPENCL_PLATFORM_INFO, buf, bufsz);
	free((void *)buf);

	/* receive global platform ID from the agent */	
	type = CLARA_MSG_OPENCL_PLATFORM_ID;
	provider_agent_recvmsg(&type, (char *)&platform->global_id, sizeof(global_id_t));
	assert(mapping_create(&mapping, platform_id, (global_id_t)platform->global_id));
	assert(set_insert(provider->id_map, mapping->virt, mapping));

	/* send device information */
	set_start(provider->platforms);
	while ((platform = set_next(provider->platforms)) != NULL)
	{
//		debug_printf("platform %16lx --> %016lX\n", platform->id, platform->global_id);

		/* get number of devices */
		cl_uint num_devices;

		if (clGetDeviceIDs(platform->id, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices) != CL_SUCCESS)
		{
			debug_printf("provider_startup: clGetDeviceIDs failed\n");
			set_finish(provider->platforms);
			return false;
		}

		assert(set_create(&platform->devices));

		cl_device_id *device_ids = (cl_device_id *)calloc(num_devices, sizeof(cl_device_id));
		if (clGetDeviceIDs(platform->id, CL_DEVICE_TYPE_ALL, num_devices, device_ids, NULL) != CL_SUCCESS)
		{
			debug_printf("provider_startup: clGetDeviceIDs failed\n");
			set_finish(provider->platforms);
			return false;
		}

		cl_int status;
		for (i = 0; i < num_devices; i++)
		{
			if ((provider->opts.device != -1) && (i != provider->opts.device))
				continue;

			device_t device = calloc(1, sizeof(*device));
			assert(set_insert(platform->devices, (global_id_t)device, device));

			device->id = device_ids[i];
	
			bufsz = sizeof(cl_device_id);
			size_ret = 0;
	
			const cl_device_info *di;
			for (di = device_info_list; *di != 0; di++)
			{
				status = clGetDeviceInfo(device->id, *di, 0, NULL, &size_ret);
				switch (status)
				{
				case CL_SUCCESS:
					break;
				case CL_INVALID_DEVICE:
					debug_printf("provider_startup: clGetDeviceInfo returned CL_INVALID_DEVICE\n");
					break;
				case CL_INVALID_VALUE:
					debug_printf("provider_startup: clGetDeviceInfo returned CL_INVALID_VALUE\n");
					break;
				default:
					debug_printf("provider_startup: clGetDeviceInfo returned %d\n", status);
					break;
				}
				bufsz += size_ret;
			}
	
			ptr = buf = (char *)malloc(bufsz);
	
			for (di = device_info_list; *di != 0; di++)
			{
				status = clGetDeviceInfo(device->id, *di, bufsz, ptr, &size_ret);
				if (status != CL_SUCCESS)
					debug_printf("provider_startup: clGetDeviceInfo returned %d\n", status);
				ptr += size_ret;
			}
	
			/* finally add the local device id */
			memcpy(ptr, &device->id, sizeof(device->id));
			ptr += sizeof(device->id);

			/* send device information to the agent */	
			provider_agent_sendmsg(CLARA_MSG_OPENCL_DEVICE_INFO, buf, bufsz);
			free((void *)buf);

			/* receive global device ID from the agent */	
			type = CLARA_MSG_OPENCL_DEVICE_ID;	
			provider_agent_recvmsg(&type, (char *)&device->global_id, sizeof(device->global_id));
			assert(mapping_create(&mapping, device->id, (global_id_t)device->global_id));
			assert(set_insert(provider->id_map, mapping->virt, mapping));
	
//			debug_printf("  device %16lx --> %016lX\n", device->id, device->global_id);

			if (provider->opts.device != -1)
				break;
		}
	}
	set_finish(provider->platforms);

	return true;
}

bool provider_init(provider_options_t opts)
{
	if (provider != NULL)
		return false;

	provider = (provider_t )calloc(1, sizeof(*provider));

	if (provider == NULL)
		return false;

	provider->agent = (agent_t )calloc(1, sizeof(*provider->agent));

	assert(set_create(&provider->id_map));
	assert(set_create(&provider->allocations));

	/* copy options */
	memcpy(&provider->opts, opts, sizeof(provider->opts));

	return true;
}

void provider_deinit(void)
{
//	debug_printf("provider_deinit\n");

	if (provider == NULL)
		return;

	set_destroy(provider->allocations);
	set_destroy(provider->id_map);

	if (provider->agent != NULL)
		free((void *)provider->agent);

	memset(provider, 0, sizeof(*provider));
	free((void *)provider);

	provider = NULL;
}

void
provider_list(void)
{
	cl_platform_id *platforms, *platform;
	cl_device_id *devices, *device;

	int device_counter = 0;

	char *name;

	if ((platforms = clara_GetPlatformIDs()) == NULL)
		return;

	for (platform = platforms; *platform != NULL; platform++)
	{
		name = clara_GetPlatformInfo(*platform, CL_PLATFORM_NAME);
		clara_printf("platform %lx: %s\n", *platform, name);
		free((void *)name);

		if ((devices = clara_GetDeviceIDs(*platform, CL_DEVICE_TYPE_ALL)) == NULL)
		{
			free((void *)platforms);
			return;
		}

		for (device = devices; *device != NULL; device++)
		{
			name = clara_GetDeviceInfo(*device, CL_DEVICE_NAME);
			clara_printf("  device %lx: %s (%i)\n", *device, name, device_counter++);
			free((void *)name);
		}

		free((void *)devices);

		clara_printf("\n");
	}

	free((void *)platforms);
}

bool
provider_main(provider_options_t opts)
{
	int rc = false;

	message_type_t type;
	ssize_t sz;

	if (!provider_init(opts))
	{
		debug_printf("provider_main: provider_init failed\n");
		return false;
	}

	if (provider->opts.list_devices)
	{
		provider_list();
		provider_deinit();
		return true;
	}

	if (!clara_sockaddr(provider->opts.addr, provider->opts.port, &provider->agent->addr))
	{
		debug_printf("provider_main: clara_sockaddr failed, errno = %d\n", errno);
		goto clara_sockaddr_failed;
	}

	if ((provider->sd = clara_socket()) == -1)
	{
		fprintf(stderr, "provider_main: clara_socket failed, errno = %d\n", errno);
		goto clara_socket_failed;
	}

	clara_printf("Trying %s:%u...\n", inet_ntoa(provider->agent->addr.sin_addr), provider->agent->addr.sin_port);

	if (provider_agent_sendmsg(CLARA_MSG_PROVIDER, NULL, 0) == -1)
	{
		fprintf(stderr, "provider_main: provider_agent_sendmsg (%s:%u) failed, errno = %d\n", __FILE__, __LINE__, errno);
		goto failed;
	}

	type = CLARA_MSG_INVALID;
	if ((sz = provider_agent_recvmsg(&type, NULL, 0)) == -1)
	{
		clara_printf("Connection refused.\n");
		errno = EAGAIN;
		goto message_receive_failed;
	}

	if (sz != 0)
	{
		debug_printf("provider_main: unexpected trailing data (%d bytes)\n", sz);
		goto size_mismatch;
	}

	if (type == CLARA_MSG_NACK)
	{
		errno = EPERM;
		goto permission_denied;
	}
	else if (type != CLARA_MSG_ACK)
	{
		errno = EPERM;
		goto permission_denied;
	}

	if (!provider_startup())
		goto startup_error;

	/* going to idle state */
	provider_agent_sendmsg(CLARA_MSG_IDLE, NULL, 0);
	clara_printf("Connected.\n");

	int count = 0;
	ssize_t ssz;
	struct sockaddr_in from;
	struct sctp_sndrcvinfo recvinfo;
	int flags;

	size_t bufsz = PROVIDER_RECVBUFSZ;
	char *buf = (char *)calloc(bufsz, sizeof(*buf));

	struct timespec *tp;

	bool end = false;
	rc = 0;

	while (!end)
	{
		type = 0;
		memset(&from, 0, sizeof(from));
		memset(&recvinfo, 0, sizeof(recvinfo));
		memset(&flags, 0, sizeof(flags));

		if ((ssz = clara_recvmsg(provider->sd, &type, buf, bufsz, &from, &recvinfo, &flags)) == -1)
			break;

		if (flags & MSG_NOTIFICATION)
		{
			union sctp_notification *notif = (union sctp_notification *)buf;

			switch (notif->sn_header.sn_type)
			{
			case SCTP_SHUTDOWN_EVENT:
				errno = EAGAIN;
				rc = 0;
				end = true;
				break;
			default:
				break;
			}
		}
		else
		{
			struct clara_allocation *allocation;
			allocate_request_t areqparams;
			execute_request_t ereqparams;
			struct clara_mapping_params *mreqparams;
			mapatptr_request_t mapreq_p;
			fetch_request_t freqparams;
			struct memput_stream_params *mpp;

			mapping_t mapping;
			size_t extra;
#if 0
			struct sctp_sndrcvinfo sinfo;
			int msg_flags;
#endif
			switch (type)
			{
			case CLARA_MSG_PING:
				tp = (struct timespec *)buf;
				ssz = clara_sendmsg(provider->sd, CLARA_MSG_PONG, &tp->tv_sec, sizeof(time_t), &from, 0, 0, CLARA_CONTROL_STREAM, 0, 0);
				break;
			case CLARA_MSG_ALLOCATE_REQUEST:
				areqparams = (allocate_request_t)buf;
				extra = ssz - sizeof(*areqparams);
/*
				if (extra > 0)
					debug_printf("provider_main: ALLOCATE request with data (%lu bytes)\n", extra);
				else
					debug_printf("provider_main: ALLOCATE request without data\n");
*/
				allocation = clara_create_allocation(areqparams);
				assert(set_insert(provider->allocations, allocation->handle, allocation));
				assert(mapping_create(&mapping, allocation->ptr, (global_id_t)allocation->handle));
				assert(set_insert(provider->id_map, mapping->virt, mapping));

				if (extra > 0)
				{
					/* initialize allocated memory with data provided by the agent resp. consumer */
					memcpy(allocation->ptr, areqparams + 1, allocation->nmemb * allocation->size);
				}

				if (!clara_send_allocate_response(provider->sd, allocation->handle, &from))
				{
					debug_printf("provider_main: clara_send_allocate_response failed\n");
					assert(mapping_find_by_virt(provider->id_map, (global_id_t)allocation->handle, &mapping));
					assert(set_remove(provider->id_map, mapping->virt));
					assert(mapping_destroy(mapping));
					assert(set_remove(provider->allocations, allocation->handle));
					clara_destroy_allocation(allocation, NULL);
				}
				break;
			case CLARA_MSG_EXECUTE_REQUEST:
//				debug_printf("CLARA_MSG_EXECUTE_REQUEST\n");
				ereqparams = (execute_request_t)buf;
//				debug_printf("provider_main: EXECUTE request (fun=%04x)\n", ereqparams->fun);
				
				arg_t retval = 0;
				struct cl_function_handler_args args;
				args.msgbuf = buf;
				args.msgsz = ssz;

				bool successful;
				assert((successful = clara_execute(&exec_fun_table, provider_id_map, provider, ereqparams, &retval, &args)) == true);
				assert(clara_send_execute_response(provider->sd, retval, &from, successful));
				break;
			case CLARA_MSG_MAPPING:
//				debug_printf("provider_main: MAPPING\n");
				mreqparams = (struct clara_mapping_params *)buf;

				assert(mapping_create(&mapping, mreqparams->real, mreqparams->virt));
				assert(set_insert(provider->id_map, mapping->virt, mapping));
//				debug_printf("provider_main: add mapping: %016lx -> %016lx (type=%u)\n", mreqparams->virt, mreqparams->real, mreqparams->type);

				break;

			case CLARA_MSG_MAPATPTR_REQUEST:
				mapreq_p = (mapatptr_request_t)buf;

				void *real = NULL;
				/* get mapping of the pointer */
				if (mapping_find_by_virt(provider->id_map, mapreq_p->ptr, &mapping))
				{
					/* get the pointer of the object */
					void **ptr = (void **)mapping->real;
					/* get the real ID of the object */
					real = *ptr;
					/* create mapping for the object */
					assert(mapping_create(&mapping, real, mapreq_p->virt));
					assert(set_insert(provider->id_map, mapping->virt, mapping));
					/* rewrite object ID at memory location */
					assert(sizeof(*ptr) == sizeof(mapreq_p->virt));
					memcpy(ptr, &mapreq_p->virt, sizeof(*ptr));
				}
				else
				{
					debug_printff("CLARA_MSG_MAPATPTR_REQUEST: mapping_find_by_virt(..., %lx, ...) failed", mapreq_p->ptr);
				}
				/* send real object ID or zero to the agent */
				assert(clara_send_mapatptr_response(provider->sd, real, &from));

				break;

			case CLARA_MSG_MEMPUT_STREAM:
//				debug_printf("CLARA_MSG_MEMPUT_REQUEST\n");
				mpp = (struct memput_stream_params *)buf;
				assert(mapping_find_by_virt(provider->id_map, (global_id_t)mpp->dst, &mapping));
				char *dstx = (char *)mapping->real + mpp->offset;

				size_t bytes_received;

				bytes_received = stream_recv(provider->sd, dstx, mpp->length);
				if (bytes_received != mpp->length)
				{
					debug_printff("stream_recv failed, %u bytes received, %u expected", bytes_received, mpp->length);
				}

//				debug_printf("provider_main: receiving %u bytes, writing at %016lx\n", mpp->length, dstx);
				break;

			case CLARA_MSG_FETCH_REQUEST:
				freqparams = (fetch_request_t)buf;

//				debug_printf("provider_main: consumer %s:%u request %d bytes of data\n",
//					inet_ntoa(freqparams->consumer.sin_addr), freqparams->consumer.sin_port, freqparams->size);

				char *resp_buf;
				size_t resp_bufsz;

				allocation = clara_find_allocation(provider->allocations, freqparams->handle);

				if (freqparams->size > 0)
				{
					resp_buf = allocation->ptr;
					resp_bufsz = allocation->nmemb * allocation->size;

//					debug_printf("provider_main: sending %d bytes\n", resp_bufsz); 
					if (!clara_send_fetch_response(provider->sd, resp_buf, resp_bufsz, &freqparams->stream_opts, &provider->agent->addr))
					{
						debug_printf("provider_main: clara_send_fetch_response failed\n");
					}
				}

				assert(mapping_find_by_virt(provider->id_map, (global_id_t)allocation->handle, &mapping));
				assert(set_remove(provider->id_map, mapping->virt));
				assert(mapping_destroy(mapping));
				assert(set_remove(provider->allocations, allocation->handle));
				clara_destroy_allocation(allocation, NULL);
				break;
			default:
				debug_printf("provider_main: ssz=%d, type=%u, from=%s:%u, flags=%08x, count=%d\n",
					ssz, type, inet_ntoa(from.sin_addr), from.sin_port, flags, count);
				break;
			}
			count++;
		}
	}

	//shutdown(provider->sd, SHUT_RDWR);
	clara_printf("Disconnected.\n");

message_receive_failed:

startup_error:
permission_denied:

size_mismatch:

failed:

	provider_shutdown();
	usleep(300000);

clara_sockaddr_failed:
clara_socket_failed:
	provider_deinit();

	return (rc);
}


