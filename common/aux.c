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

#if !defined(NDEBUG)
bool clara_debug_check = true;
bool clara_debug_enabled = true;
struct timespec clara_debug_time;
#endif /* !NDEBUG */

bool
clara_debug_init()
{
#if !defined(NDEBUG)
	char *envvar = getenv("DEBUG");
	if (envvar != NULL)
		clara_debug_enabled = (strcmp(envvar, "no") != 0);
	clock_gettime(CLOCK_REALTIME, &clara_debug_time);
	clara_debug_check = false;
	return clara_debug_enabled;
#else
	return false;
#endif /* !NDEBUG */
}

int
clara_printf(const char *format, ...)
{
	int retval = 0;
	va_list ap;

	va_start(ap, format);
	retval += vfprintf(CLARA_STDOUT, format, ap);
	va_end(ap);

	return (retval);
}

void
clara_srand(void)
{
	struct timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	srand(t.tv_sec);
}

bool
clara_arg2buf(arg_t *ptr, va_list ap, int argc)
{
	int i;
	va_list aq;
	va_copy(aq, ap);
	for (i = 0; i < argc; i++)
		ptr[i] = va_arg(aq, arg_t);
	va_end(aq);
	return true;
}

size_t
clara_malloc_and_memcpy(void **dst, const void *src, size_t n)
{
	void *dest;

	if (n == 0)
		return 0;

	if ((dest = malloc(n)) == NULL)
		return 0;

	*dst = memcpy(dest, src, n);

	return n;
}

size_t
clara_malloc_and_strcpy(char **dst, const char *src)
{
	return clara_malloc_and_memcpy((void **)dst, src, strlen(src) + 1);
}

