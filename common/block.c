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

#define BLOCKSIZE 1024

size_t
block_send(int sd, const void *data, size_t len, struct sockaddr_in *to)
{
	size_t bytes_sent = 0;
	ssize_t ssz;

	/* initiating message */
	struct block_params blp = { .length = len };

	ssz = clara_sendmsg(sd, CLARA_MSG_BLOCK, &blp, sizeof(blp), to, 0, 0, CLARA_CONTROL_STREAM, 0, 0);
	if (ssz == -1)
	{
		debug_printff("clara_sendmsg failed");
		return bytes_sent;
	}

	/* send data */
	size_t remaining_bytes = len;
	size_t blocksize = BLOCKSIZE;
	const uint8_t *ptr = data;

	while (remaining_bytes > 0)
	{
		if (blocksize > remaining_bytes) blocksize = remaining_bytes;
		ssz = clara_send(sd, ptr, blocksize, to, 0, 0, CLARA_CONTROL_STREAM, 0, 0);
		if (ssz == -1)
		{
			debug_printff("clara_send failed");
			break;
		}

		bytes_sent += ssz;
		ptr += ssz;
		remaining_bytes -= ssz;
	}

	return bytes_sent;
}

size_t
block_recv(int sd, void *data, size_t len, block_params_t blp)
{
	size_t bytes_received = 0;
	ssize_t ssz;

	if (blp->length > len)
	{
		debug_printff("buffer too small: %u bytes, %d bytes needed", len, blp->length);
		assert(0);
	}

	/* receive data */
	size_t remaining_bytes = blp->length;
	uint8_t *ptr = data;

	while (remaining_bytes > 0)
	{
		ssz = clara_recv(sd, ptr, remaining_bytes, NULL, NULL, NULL);
		if (ssz == -1)
		{
			debug_printff("clara_recv failed");
			break;
		}

		bytes_received += ssz;
		ptr += ssz;
		remaining_bytes -= ssz;
	}

	return bytes_received;
}

size_t
block_forward(int sd_in, int sd_out, struct sockaddr_in *to, block_params_t blp)
{
	size_t bytes_forwarded = 0;
	ssize_t bytes_received;
	ssize_t ssz;

	ssz = clara_sendmsg(sd_out, CLARA_MSG_BLOCK, blp, sizeof(*blp), to, 0, 0, CLARA_CONTROL_STREAM, 0, 0);
	if (ssz == -1)
	{
		debug_printff("clara_sendmsg failed");
		return bytes_forwarded;
	}

	/* forward data */
	size_t remaining_bytes = blp->length;
	size_t blocksize = BLOCKSIZE;
	uint8_t *buf = malloc(blocksize);
	uint8_t *ptr;

	while (remaining_bytes > 0)
	{
		if (blocksize > remaining_bytes) blocksize = remaining_bytes;
		bytes_received = clara_recv(sd_in, buf, blocksize, NULL, NULL, NULL);
		if (bytes_received == -1)
		{
			debug_printff("clara_recv failed, %u bytes forwarded", bytes_forwarded);
			goto block_forward_exit;
		}

		ptr = buf;
		while (bytes_received > 0)
		{
			ssz = clara_send(sd_out, ptr, bytes_received, to, 0, 0, CLARA_CONTROL_STREAM, 0, 0);
			if (ssz == -1)
			{
				debug_printff("clara_send failed");
				goto block_forward_exit;
			}
			bytes_forwarded += ssz;
			ptr += ssz;
			bytes_received -= ssz;
			remaining_bytes -= ssz;
		}
	}

block_forward_exit:
	free(buf);
	return bytes_forwarded;
}

