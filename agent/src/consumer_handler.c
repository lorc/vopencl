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
#include "agent_opencl.h"

const struct fun_table_entry pre_exec_fun_table_entries[] =
{
	[CL_GETDEVICEINFO]				= { &CLARA_PRE_CLFUN_NAME(clGetDeviceInfo) },

	[CL_RETAINCONTEXT]				= { &CLARA_PRE_CLFUN_NAME(clRetainContext) },
	[CL_RELEASECONTEXT]				= { &CLARA_PRE_CLFUN_NAME(clReleaseContext) },

	[CL_RETAINCOMMANDQUEUE]			= { &CLARA_PRE_CLFUN_NAME(clRetainCommandQueue) },
	[CL_RELEASECOMMANDQUEUE]		= { &CLARA_PRE_CLFUN_NAME(clReleaseCommandQueue) },

	[CL_RETAINMEMOBJECT]			= { &CLARA_PRE_CLFUN_NAME(clRetainMemObject) },
	[CL_RELEASEMEMOBJECT]			= { &CLARA_PRE_CLFUN_NAME(clReleaseMemObject) },

	[CL_RETAINPROGRAM]				= { &CLARA_PRE_CLFUN_NAME(clRetainProgram) },
	[CL_RELEASEPROGRAM]				= { &CLARA_PRE_CLFUN_NAME(clReleaseProgram) },

	[CL_RETAINKERNEL]				= { &CLARA_PRE_CLFUN_NAME(clRetainKernel) },
	[CL_RELEASEKERNEL]				= { &CLARA_PRE_CLFUN_NAME(clReleaseKernel) },

	[CL_RETAINEVENT]				= { &CLARA_PRE_CLFUN_NAME(clRetainEvent) },
	[CL_RELEASEEVENT]				= { &CLARA_PRE_CLFUN_NAME(clReleaseEvent) },

	[CL_ENQUEUEBARRIER]				= { &CLARA_PRE_CLFUN_NAME(clEnqueueBarrier) },

	[CL_FLUSH]						= { &CLARA_PRE_CLFUN_NAME(clFlush) },
	[CL_FINISH]						= { &CLARA_PRE_CLFUN_NAME(clFinish) },
};

const struct fun_table pre_exec_fun_table = {
	.num_entries = sizeof(pre_exec_fun_table_entries) / sizeof(*pre_exec_fun_table_entries),
	.entries = pre_exec_fun_table_entries
};

const struct fun_table_entry post_exec_fun_table_entries[] =
{
	/* platform and device */
	[CL_GETPLATFORMIDS]				= { &CLARA_POST_CLFUN_NAME(clGetPlatformIDs), ARG2 | ARG3 },
	[CL_GETPLATFORMINFO]			= { &CLARA_POST_CLFUN_NAME(clGetPlatformInfo), ARG4 | ARG5 },
	[CL_GETDEVICEIDS]				= { &CLARA_POST_CLFUN_NAME(clGetDeviceIDs), ARG4 | ARG5 },
	[CL_GETDEVICEINFO]				= { &CLARA_POST_CLFUN_NAME(clGetDeviceInfo), ARG4 | ARG5 },

	/* context */
	[CL_CREATECONTEXT]				= { &CLARA_POST_CLFUN_NAME(clCreateContext) },
	[CL_CREATECONTEXTFROMTYPE]		= { &CLARA_POST_CLFUN_NAME(clCreateContextFromType) },

	/* command queue */
	[CL_CREATECOMMANDQUEUE]			= { &CLARA_POST_CLFUN_NAME(clCreateCommandQueue) },

	/* buffer */
	[CL_CREATEBUFFER]				= { &CLARA_POST_CLFUN_NAME(clCreateBuffer) },
	[CL_ENQUEUEREADBUFFER]			= { &CLARA_POST_CLFUN_NAME(clEnqueueReadBuffer) },
	[CL_ENQUEUEWRITEBUFFER]			= { &CLARA_POST_CLFUN_NAME(clEnqueueWriteBuffer) },
	[CL_ENQUEUECOPYBUFFER]			= { &CLARA_POST_CLFUN_NAME(clEnqueueCopyBuffer) },

	/* program */
	[CL_CREATEPROGRAMWITHSOURCE]	= { &CLARA_POST_CLFUN_NAME(clCreateProgramWithSource) },
	[CL_CREATEPROGRAMWITHBINARY]	= { &CLARA_POST_CLFUN_NAME(clCreateProgramWithBinary) },
	[CL_UNLOADCOMPILER]				= { &CLARA_POST_CLFUN_NAME(clUnloadCompiler) },

	/* kernel */
	[CL_CREATEKERNEL]				= { &CLARA_POST_CLFUN_NAME(clCreateKernel) },

	[CL_ENQUEUENDRANGEKERNEL]		= { &CLARA_POST_CLFUN_NAME(clEnqueueNDRangeKernel) },
	[CL_ENQUEUETASK]				= { &CLARA_POST_CLFUN_NAME(clEnqueueTask) },
};

struct fun_table post_exec_fun_table = {
	.num_entries = sizeof(post_exec_fun_table_entries) / sizeof(*post_exec_fun_table_entries),
	.entries = post_exec_fun_table_entries
};

arg_t
agent_id_map(global_id_t id, void *data)
{
	consumer_t consumer = (consumer_t)data;
	struct clara_allocation *allocation;
	allocation = clara_find_allocation(consumer->allocations, id);
	assert(allocation != NULL);
	return PTR2ARG(allocation->ptr);
}

bool
agent_execute(void *msgbuf, ssize_t msgsz, arg_t *pretval, consumer_t source)
{
	execute_request_t params = (execute_request_t)msgbuf;
	provider_t owner = NULL;
	struct cl_function_handler_args args;

	if (params->hint != INVALID_HANDLE)
	{
		object_t object;
		assert(set_find(source->agent->objects, params->hint, (void **)&object));
		owner = object->provider;
	}

	args.msgbuf = msgbuf;
	args.msgsz = msgsz;
	args.target = owner;
	args.source = source;
	
	assert(clara_execute(&pre_exec_fun_table, agent_id_map, source, params, pretval, &args));

	if (owner != NULL)
	{
		assert(agent_remote_execute(owner, msgbuf, msgsz, pretval));
	}

	assert(clara_execute(&post_exec_fun_table, agent_id_map, source, params, pretval, &args));

	return true;
}

void *
agent_consumer_handler(void *arg)
{
	int retval = 0;
	struct clara_consumer *consumer = (struct clara_consumer *)arg;
	int sd = consumer->sd;
	
	ssize_t ssz;
	message_type_t type;
	void *buf;
	size_t bufsz = AGENT_RECVBUFSZ;
	struct sockaddr_in from;
	struct sctp_sndrcvinfo rcvinfo;
	int flags;

	buf = (void *)malloc(bufsz);

	int exit = 0;

	while (!exit)
	{
		if ((ssz = clara_recvmsg(sd, &type, buf, bufsz, &from, &rcvinfo, &flags)) == -1)
		{
			debug_printf("agent_consumer_handler(%d): clara_recvmsg failed, errno = %d\n", sd, errno);
			break;
		}

		if (flags & MSG_NOTIFICATION)
		{
			union sctp_notification *notif;
			notif = (union sctp_notification *)buf;

			switch (notif->sn_header.sn_type)
			{
			case SCTP_SHUTDOWN_EVENT:
				exit = 1;
				break;
			default:
				debug_printf("agent_consumer_handler(%d): received %d bytes from %s:%d (flags=%08x)\n", sd, ssz, inet_ntoa(from.sin_addr), from.sin_port, flags);
				debug_printf("agent_consumer_handler(%d): unknown notification type: %d\n", sd, notif->sn_header.sn_type);
				break;
			}
		}
		else
		{
			size_t extra;
			struct clara_allocation *allocation = NULL;
			handle_t handle = INVALID_HANDLE;
			allocate_request_t areqparams;
			fetch_request_t freqparams;
			struct memput_stream_params *mpp;
			arg_t resp_val;

			object_t object;

			switch (type)
			{
			case CLARA_MSG_ALLOCATE_REQUEST:
				areqparams = (allocate_request_t)buf;
				extra = ssz - sizeof(*areqparams);
//				debug_printf("allocate %d bytes, extra=%d\n", areqparams->nmemb * areqparams->size, extra);
//				debug_printf("agent_consumer_handler(%d): ALLOCATE request: nmemb=%lu size=%lu hint=%016lx extra=%lu\n", sd,
//					areqparams->nmemb, areqparams->size, areqparams->hint, extra);

				if (areqparams->handle != INVALID_HANDLE)
				{
					if (!clara_send_allocate_response(consumer->sd, INVALID_HANDLE, &from))
						debug_printff("clara_send_allocate_response failed");
					break;
				}

				if (areqparams->hint == INVALID_HANDLE)
				{
					/* allocate memory on the local machine, i.e. agent */
					assert(id_create(consumer->agent->id_pool, &areqparams->handle));
					allocation = clara_create_allocation(areqparams);
					assert(allocation != NULL);
					assert(set_insert(consumer->allocations, allocation->handle, allocation));
					handle = allocation->handle;

					if (extra > 0)
					{
						/* initialize allocated memory with data provided by the consumer */
						assert(extra == allocation->nmemb * allocation->size);
						debug_printf("agent_consumer_handler(%d): initialize with %u bytes of data\n", sd, extra);
						memcpy(allocation->ptr, areqparams + 1, extra);
					}
				}
				else
				{
					/* allocate memory on a remote machine, i.e. provider */
					assert(set_find(consumer->agent->objects, areqparams->hint, (void **)&object));
					/* create a unique handle */
					assert(id_create(consumer->agent->id_pool, &areqparams->handle));

					if (!agent_remote_allocate(object->provider, areqparams, extra, &handle))
					{
						debug_printf("agent_consumer_handler(%d): agent_remote_allocate failed\n", sd);
					}
				}

//				debug_printf("agent_consumer_handler(%d): ALLOCATE response: %016lx\n", sd, handle);

				/* send the handle to the consumer */
				if (!clara_send_allocate_response(consumer->sd, handle, &from))
				{
					debug_printf("agent_consumer_handler(%d): clara_send_allocate_response failed\n", sd);
					assert(set_remove(consumer->allocations, allocation->handle));

					handle_t old_handle;
					assert(clara_destroy_allocation(allocation, &old_handle));
					assert(id_release(consumer->agent->id_pool, old_handle));
				}
				break;

			case CLARA_MSG_MEMPUT_STREAM:
				mpp = (struct memput_stream_params *)buf;
				assert(set_find(consumer->agent->objects, mpp->hint, (void **)&object));
				agent_forward_memput(consumer, object->provider, mpp);
				break;

			case CLARA_MSG_EXECUTE_REQUEST:
				resp_val = 0;

				/* handle request */
				bool successful = agent_execute(buf, ssz, &resp_val, consumer);

				/* send response */
				if (!clara_send_execute_response(consumer->sd, resp_val, &from, successful))
				{
					debug_printf("agent_execute: clara_send_execute_response failed\n");
				}
				break;
			case CLARA_MSG_FETCH_REQUEST:
				freqparams = (fetch_request_t)buf;
//				debug_printf("agent_consumer_handler(%d): FETCH request: handle=%016lx size=%lu hint=%016lx\n", sd,
//					freqparams->handle, freqparams->size, freqparams->hint);

				//debug_printf("agent_consumer_handler(%d): consumer requested %d bytes of data\n", sd, freqparams->size);

				if (freqparams->hint == INVALID_HANDLE)
				{
					char *respbuf;
					size_t respbufsz;

					allocation = clara_find_allocation(consumer->allocations, freqparams->handle);

					respbufsz = allocation->nmemb * allocation->size;
					if (freqparams->size < respbufsz)
						respbufsz = freqparams->size;

					respbuf = (respbufsz == 0) ? NULL : allocation->ptr;

					if (freqparams->size > 0)
					{
						if (!clara_send_fetch_response(consumer->sd, respbuf, respbufsz, &freqparams->stream_opts, &from))
						{
							debug_printf("agent_consumer_handler(%d): clara_send_fetch_response failed\n", sd);
						}
					}

					assert(set_remove(consumer->allocations, allocation->handle));

					handle_t old_handle;
					assert(clara_destroy_allocation(allocation, &old_handle));
					assert(id_release(consumer->agent->id_pool, old_handle));
				}
				else
				{
					assert(set_find(consumer->agent->objects, freqparams->hint, (void **)&object));
					agent_forward_fetch(consumer, object->provider, freqparams);
				}
				break;

			default:
				debug_printf("agent_consumer_handler(%d): received %d bytes from %s:%d (flags=%08x)\n", sd, ssz, inet_ntoa(from.sin_addr), from.sin_port, flags);
				break;
			}
		}
	}

	free((void *)buf);

	shutdown(sd, SHUT_RDWR);

	assert(consumer_detach(consumer));
	assert(consumer_destroy(consumer));

	//debug_printf("agent_consumer_handler(%d): exit with code %d\n", sd, retval);
	pthread_exit(&retval);
}

