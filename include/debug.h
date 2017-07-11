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

#if !defined(__DEBUG_H__)
#define __DEBUG_H__

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define DEBUG_LEVEL		(~(uint64_t)0)

#define DEBUG_INFO		0x0001
#define DEBUG_WARN		0x0002
#define DEBUG_CRIT		0x0004

#define CLARA_STDDBG	stderr

bool clara_debug_init(void);
extern bool clara_debug_check;
extern bool clara_debug_enabled;
extern struct timespec clara_debug_time;

static inline
bool
debug_enabled(void)
{
	if (clara_debug_check)
	{
		clara_debug_check = false;
		clara_debug_init();
	}
	return clara_debug_enabled;
}

static inline
int
debug_printtime()
{
	struct timespec t;
	clock_gettime(CLOCK_REALTIME, &t);

	if (t.tv_nsec > clara_debug_time.tv_nsec)
	{
		t.tv_sec -= clara_debug_time.tv_sec;
		t.tv_nsec -= clara_debug_time.tv_nsec;
	}
	else
	{
		t.tv_sec -= clara_debug_time.tv_sec + 1;
		t.tv_nsec = 1000000000 - (clara_debug_time.tv_nsec - t.tv_nsec);
	}

	double timestamp = (double)t.tv_nsec / 1000000000 + t.tv_sec;
	return (fprintf(CLARA_STDDBG, "[%12.6f]: ", timestamp));
}

static inline
int
debug_printf(const char *format, ...)
{
	int retval = 0;
	va_list ap;

	if (!debug_enabled())
		return 0;

	retval += debug_printtime();

	va_start(ap, format);
	retval += vfprintf(CLARA_STDDBG, format, ap);
	va_end(ap);

	return (retval);
}

#define debug_printff(format, ...)		__debug_printff(__FUNCTION__, format, ##__VA_ARGS__)	

static inline
int
__debug_printff(const char *fname, const char *format, ...)
{
	int retval = 0;
	va_list ap;

	if (!debug_enabled())
		return 0;

	retval += debug_printtime();

	const char *delim = *format == '(' ? "" : ": ";
	retval += fprintf(CLARA_STDDBG, "%s%s", fname, delim);

	va_start(ap, format);
	retval += vfprintf(CLARA_STDDBG, format, ap);
	va_end(ap);

	if (format[strlen(format) - 1] != '\n')
	{
		fputc('\n', CLARA_STDDBG);
		retval++;
	}

	return (retval);
}

static inline
int
debug_printf_raw(const char *format, ...)
{
	int retval = 0;
	va_list ap;

	if (!debug_enabled())
		return 0;

	va_start(ap, format);
	retval += vfprintf(CLARA_STDDBG, format, ap);
	va_end(ap);

	return (retval);
}

#if 1
static inline
int
debug_printf_ex(uint64_t level, const char *format, ...)
{
	int retval = 0;
	va_list ap;

	if (!debug_enabled())
		return 0;

	if ((DEBUG_LEVEL & level))
	{
		retval += debug_printtime();

		va_start(ap, format);
		retval += vfprintf(CLARA_STDDBG, format, ap);
		va_end(ap);
	}

	return (retval);
}
#endif

#endif /* __DEBUG_H__ */
