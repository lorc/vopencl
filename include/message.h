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

#if !defined(__MESSAGE_H__)
#define __MESSAGE_H__

typedef unsigned short message_type_t;

#define CLARA_MSG_INVALID					0x0000

#define CLARA_MSG_ACK						0x0001
#define CLARA_MSG_NACK						0x0002
#define CLARA_MSG_IDLE						0x0003
#define CLARA_MSG_PING						0x0004
#define CLARA_MSG_PONG						0x0005

#define CLARA_MSG_PROVIDER					0x0010
#define CLARA_MSG_CONSUMER					0x0011

#define CLARA_MSG_ALLOCATE_REQUEST			0x0020
#define CLARA_MSG_ALLOCATE_RESPONSE			0x0021
#define CLARA_MSG_EXECUTE_REQUEST			0x0022
#define CLARA_MSG_EXECUTE_RESPONSE			0x0023
#define CLARA_MSG_FETCH_REQUEST				0x0024
#define CLARA_MSG_FETCH_RESPONSE			0x0025
#define CLARA_MSG_MAPATPTR_REQUEST			0x0028		/* map at ptr - required for event objects */
#define CLARA_MSG_MAPATPTR_RESPONSE			0x0029		/* map at ptr - required for event objects */
#define CLARA_MSG_MEMPUT_STREAM				0x0030

#define CLARA_MSG_BLOCK						0x0040		/* a block of fixed-size data */
#define CLARA_MSG_STREAM_START				0x0041
#define CLARA_MSG_STREAM_END				0x0042

#define CLARA_MSG_OPENCL_PLATFORM_INFO		0x0100
#define CLARA_MSG_OPENCL_PLATFORM_ID		0x0110
#define CLARA_MSG_OPENCL_DEVICE_INFO		0x0101
#define CLARA_MSG_OPENCL_DEVICE_ID			0x0111

#define CLARA_MSG_MAPPING					0x0200

/* ------------- */

#define CLARA_MSG_DEBUG_TOKEN			0xaa55

#define CLARA_MSG_STRING				0xffff
#define CLARA_MSG_TEXT					0xffff

#endif /* __MESSAGE_H__ */
