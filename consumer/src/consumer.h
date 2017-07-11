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

#if !defined(__CONSUMER_H__)
#define __CONSUMER_H__

#include "clara.h"
#include "api.h"

struct clara_consumer {

	int sd;
	struct sockaddr_in agent_addr;

} *clara_consumer_t;

#define STREAM_FORMAT(size)		(((size) > 16384) ? STREAM_FORMAT_LZO1X_1 : STREAM_FORMAT_RAW)
#define STREAM_BLOCKSIZE		(1 << 22)		/* 4 MiB blocks */

bool consumer_begin_allocate(size_t nmemb, size_t size, void *data, global_id_t hint);
handle_t consumer_end_allocate(void);
bool consumer_execute(arg_t d, void *retbuf, size_t retbuflen, global_id_t hint, ...);
bool consumer_begin_fetch(handle_t src, size_t n, global_id_t hint);
bool consumer_end_fetch(void *dst, size_t n);

handle_t consumer_allocate(size_t nmemb, size_t size, void *data, global_id_t hint);
bool consumer_fetch(handle_t src, void *dst, size_t n, global_id_t hint);
bool consumer_free(handle_t src, global_id_t hint);
bool consumer_memcpy(handle_t dst, void *src, size_t size, global_id_t hint);

#define HINT					___hint
#define DECLARE_HINT(var)		global_id_t HINT = PTR2ARG(var); \
								if (HINT == (global_id_t)INVALID_HANDLE) RETURN;
#define DECLARE_NO_HINT			global_id_t HINT = (global_id_t)INVALID_HANDLE

#define ERRCODE_SET(value)		{ if (errcode_ret != NULL) *errcode_ret = value; }
#define ERRCODE_INIT()			consumer_allocate(1, sizeof(cl_int), NULL, HINT)
#define ERRCODE_FETCH()			consumer_fetch(h_errcode_ret, errcode_ret, sizeof(*errcode_ret), HINT)

#define DECLARE_HANDLE(var)		handle_t h_ ## var = INVALID_HANDLE

#define MALLOC(var, size)			{ if (var != NULL) h_ ## var = consumer_allocate(1, (size), (void *)(var), HINT); }
#define CALLOC(var, nmemb, size)	{ if (var != NULL) h_ ## var = consumer_allocate((nmemb), (size), NULL, HINT); }

#define FETCH(var, size)		{ if (h_ ## var != INVALID_HANDLE) consumer_fetch(h_ ## var, var, (size), HINT); }
#define FREE(var)				{ if (h_ ## var != INVALID_HANDLE) consumer_free(h_ ## var, HINT); }

#define MEMPUT(var, size)		consumer_memcpy(HANDLE(var), (void *)(var), (size), HINT)

#define EXECUTE(fn, ...)		assert(consumer_execute(fn, &__rv, sizeof(__rv), HINT, ##__VA_ARGS__))
#define HANDLE(var)				((arg_t)h_ ## var)

#define INIT_STATUS(value)		cl_int __rv = (cl_int)(value)
#define INIT_OBJECT(type)		type __rv = (type)NULL;
#define RETURN					return __rv

#define PRINT_RETVAL			debug_printf("%30s -> %d\n", __FUNCTION__, __rv);
#define PRINT_RETOBJ			debug_printf("%30s -> %08lx\n", __FUNCTION__, __rv);
#define PRINT_NOTIMPL			debug_printf("%s: not yet implemented\n", __FUNCTION__);
#define PRINT_NOTSUPP			debug_printf("%s: not supported\n", __FUNCTION__);

#define DECLARE_GETOBJECTINFO(name, typ, fn) \
cl_int clGet ## name ## Info ( \
	cl_ ## typ object, \
	cl_ ## typ ## _info param_name, \
	size_t param_value_size, \
	void *param_value, \
	size_t *param_value_size_ret) \
{ \
	INIT_STATUS(CL_SUCCESS); \
	DECLARE_HANDLE(param_value); \
	DECLARE_HANDLE(param_value_size_ret); \
	DECLARE_HINT(object); \
	CALLOC(param_value, 1, param_value_size); \
	CALLOC(param_value_size_ret, 1, sizeof(size_t)); \
	EXECUTE(fn, PTR2ARG(object), (arg_t)param_name, (arg_t)param_value_size, \
		HANDLE(param_value), HANDLE(param_value_size_ret)); \
	FETCH(param_value, param_value_size); \
	FETCH(param_value_size_ret, sizeof(size_t)); \
	RETURN; \
}

#endif /* __CONSUMER_H__ */
