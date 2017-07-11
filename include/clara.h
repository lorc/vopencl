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

#if !defined(__CLARA_H__)
#define __CLARA_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <CL/cl.h>

#if !defined(NDEBUG)
#include "debug.h"
#else
#include "ndebug.h"
#endif

#define CLARA_DEFAULT_PORT		"52428"

typedef struct clara_consumer *		consumer_t;
typedef struct clara_provider *		provider_t;

#if !defined(GLOBAL_ID_T)
#define GLOBAL_ID_T
typedef uint64_t global_id_t;
#define INVALID_ID	((global_id_t)0)
#endif

typedef uint64_t arg_t;
typedef arg_t handle_t;

#if defined(__LP64__)
#define PTR2ARG(ptr)	((arg_t)ptr)
#define ARG2PTR(arg)	((void *)arg)
#else
#define PTR2ARG(ptr)	((arg_t)((uint32_t)ptr))
static inline void *
ARG2PTR(arg_t arg)
{
	assert((arg >> 32) == 0);
	return (void *)((uint32_t)arg);
}
#endif /* __LP64__ */

/* aux.c */
#define CLARA_STDOUT			stdout
int clara_printf(const char *format, ...);
void clara_srand(void);
bool clara_arg2buf(arg_t *ptr, va_list ap, int argc);
size_t clara_malloc_and_memcpy(void **dst, const void *src, size_t n);
size_t clara_malloc_and_strcpy(char **dst, const char *src);

#define CLARA_DOMAIN	AF_INET

#define CLARA_CONTROL_STREAM		0

#include "message.h"

#define INVALID_HANDLE	((handle_t)0)

struct clara_allocation {

	handle_t handle;
	size_t nmemb;
	size_t size;
	void *ptr;

};

#if !defined(SET_T)
#define SET_T
typedef struct _set *set_t;
#endif

/* map.c */

typedef struct _mapping {

	void *real;
	global_id_t virt;
	void *data;

} *mapping_t;

bool mapping_create(mapping_t *result, void *real, global_id_t virt);
bool mapping_destroy(mapping_t mapping);
bool mapping_find_by_virt(set_t map, global_id_t virt, mapping_t *result);
bool mapping_find_by_real(set_t map, void *real, mapping_t *result);
unsigned int mapping_translate_list_to_real(set_t map, unsigned int num_items, global_id_t *virt_list, void **real_list);

/* net.c */
int clara_socket(void);
int clara_sockaddr(const char *addr, const char *port, struct sockaddr_in *sa);

int clara_send(int sd, const void* msg, size_t len, struct sockaddr_in *to,
	uint32_t ppid, uint32_t flags, uint16_t stream_no, uint32_t timetolive, uint32_t context);
int clara_sendmsg(int sd, message_type_t type, const void* msg, size_t len, struct sockaddr_in *to,
	uint32_t ppid, uint32_t flags, uint16_t stream_no, uint32_t timetolive, uint32_t context);

int clara_recv(int sd, void *msg, size_t len, struct sockaddr_in* from,
	struct sctp_sndrcvinfo *sinfo, int *msg_flags);
int clara_recvmsg(int sd, message_type_t *ptype, void *msg, size_t len, struct sockaddr_in* from,
	struct sctp_sndrcvinfo *sinfo, int *msg_flags);

/* block.c */
typedef struct block_params {
	size_t length;
} *block_params_t;

size_t block_send(int sd, const void *data, size_t len, struct sockaddr_in *to);
size_t block_recv(int sd, void *data, size_t len, block_params_t blp);
size_t block_forward(int sd_in, int sd_out, struct sockaddr_in *to, block_params_t blp);

/* stream.c */
typedef enum _stream_format_t {
	STREAM_FORMAT_RAW = 0,
	STREAM_FORMAT_LZO1X_1,
} stream_format_t;

typedef struct stream_options {
	stream_format_t fmt;
	size_t blksz;
} *stream_options_t;

size_t stream_send(int sd, const void *data, size_t len, stream_options_t opts, struct sockaddr_in *to);
size_t stream_recv(int sd, void *data, size_t len);
size_t stream_forward(int sd_in, int sd_out, struct sockaddr_in *to);

typedef struct stream_params {
	stream_format_t fmt;
	size_t blksz;
} *stream_params_t;

/* rpc.c */
typedef struct allocate_request {

	handle_t handle;		/* left blank by consumer, filled in by agent */
	size_t nmemb;
	size_t size;
	global_id_t hint;
	
} *allocate_request_t;

typedef struct allocate_response {

	handle_t handle;

} *allocate_response_t;

typedef struct execute_request {

	global_id_t hint;
	arg_t fun;

	arg_t retval;			/* need to be the last argument */

} * execute_request_t;

typedef struct execute_response {

	arg_t retval;

} *execute_response_t;

typedef struct fetch_request {

	handle_t handle;
	size_t size;
	global_id_t hint;

	struct stream_options stream_opts;

} *fetch_request_t;

typedef struct fetch_response {

	size_t length;

} *fetch_response_t;

struct clara_mapping_params {

	global_id_t virt;
	void *real;

};

typedef struct mapatptr_request {

	global_id_t virt;
	global_id_t ptr;

} *mapatptr_request_t;

typedef struct mapatptr_response {

	void *real;

} *mapatptr_response_t;

struct memput_stream_params {

	handle_t dst;
	off_t offset;
	size_t length;
	global_id_t hint;

};

#define NONE	0
#define ARG1	(1 << 0)
#define ARG2	(1 << 1)
#define ARG3	(1 << 2)
#define ARG4	(1 << 3)
#define ARG5	(1 << 4)
#define ARG6	(1 << 5)
#define ARG7	(1 << 6)
#define ARG8	(1 << 7)
#define ARG9	(1 << 8)

#define MAX_ARGS	(sizeof(unsigned int) * 8)		/* depends on 'mask' */

#define CLARA_CLFUN_ARGS			struct cl_function_handler_args *args

struct cl_function_handler_args
{
	arg_t *argv;
	union {
		void *msgbuf;
		execute_request_t params;
	};
	size_t msgsz;

	provider_t target;
	consumer_t source;
};

typedef void cl_function_handler(struct cl_function_handler_args *args);

typedef struct fun_table_entry
{
	cl_function_handler *pfn;
	unsigned int ptrargs;
} *fntbl_entry_t;

struct fun_table
{
	const size_t num_entries;
	const struct fun_table_entry *entries;
};

extern const size_t fun_argc[];

typedef arg_t id_map_function_t(global_id_t, void *);

bool clara_execute(const struct fun_table *fntbl, id_map_function_t id_map, void *data, execute_request_t params, arg_t *retval, struct cl_function_handler_args *args);
bool clara_send_allocate_request(int sd, size_t nmemb, size_t size, void *data, global_id_t hint, struct sockaddr_in *to);
bool clara_send_allocate_response(int sd, handle_t handle, struct sockaddr_in *to);
bool clara_recv_allocate_response(int sd, handle_t *handle);
bool clara_send_execute_request(int sd, arg_t d, arg_t *argv, int argc, global_id_t hint, struct sockaddr_in *to);
bool clara_send_execute_response(int sd, arg_t retval, struct sockaddr_in *to, bool successful);
bool clara_recv_execute_response(int sd, void *retbuf, size_t retbuflen);
bool clara_send_fetch_request(int sd, handle_t src, size_t n, global_id_t hint, stream_options_t opts, struct sockaddr_in *to);
bool clara_send_fetch_response(int sd, const void *buf, size_t count, stream_options_t opts, struct sockaddr_in *to);
bool clara_recv_fetch_response(int sd, fetch_response_t params, void *buf, size_t count);
bool clara_send_mapatptr_request(int sd, global_id_t virt, global_id_t ptr, struct sockaddr_in *to);
bool clara_send_mapatptr_response(int sd, void *real, struct sockaddr_in *to);
bool clara_recv_mapatptr_response(int sd, void **real);

/* alloc.c */
struct clara_allocation *clara_create_allocation(allocate_request_t params);
struct clara_allocation *clara_find_allocation(set_t allocations, handle_t handle);
bool clara_destroy_allocation(struct clara_allocation *allocation, handle_t *old_handle);

#endif /* __CLARA_H__ */

