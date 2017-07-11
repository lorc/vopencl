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

#include "lzo/lzoconf.h"
#include "lzo/lzo1x.h"

//#define STREAM_STATISTICS

size_t
stream_send(int sd, const void *data, size_t len, stream_options_t opts, struct sockaddr_in *to)
{
	size_t bytes_encoded = 0;
	ssize_t ssz;

	/* default options */
	struct stream_options default_opts = {
		.fmt = STREAM_FORMAT_RAW,
		.blksz = 1 << 20,			/* 1 MiB blocks */
	};

	if (opts == NULL)
		opts = &default_opts;

	struct stream_params stp = { .fmt = opts->fmt, .blksz = opts->blksz };
	ssz = clara_sendmsg(sd, CLARA_MSG_STREAM_START, &stp, sizeof(stp),
		to, 0, 0, CLARA_CONTROL_STREAM, 0, 0);
	if (ssz == -1)
	{
		debug_printff("clara_sendmsg(CLARA_MSG_STREAM_START) failed");
		return bytes_encoded;
	}

	size_t raw_blksz = stp.blksz;
	size_t remaining_bytes = len;
	const uint8_t *ptr = data;

	uint8_t *blkbuf = NULL;
	uint8_t *tmpbuf = NULL;

	lzo_uint enc_blksz;

	if (stp.fmt == STREAM_FORMAT_LZO1X_1)
	{
		lzo_init();
		enc_blksz = raw_blksz + raw_blksz / 16 + 64 + 3;
		blkbuf = malloc(enc_blksz);
		tmpbuf = malloc(LZO1X_1_MEM_COMPRESS);
	}
	size_t sent;
#if defined(STREAM_STATISTICS)
	uint32_t blkcnt = 0;
	size_t bytes_sent = 0;
#endif
	while (remaining_bytes > 0)
	{
		if (raw_blksz > remaining_bytes) raw_blksz = remaining_bytes;

		if (stp.fmt == STREAM_FORMAT_LZO1X_1)
		{
			lzo1x_1_compress(ptr, raw_blksz, blkbuf, &enc_blksz, tmpbuf);
			sent = block_send(sd, blkbuf, enc_blksz, to);
			assert(sent == enc_blksz);
#if defined(STREAM_STATISTICS)
			++blkcnt;
			bytes_sent += sent;
#endif
		}
		else
		{
			sent = block_send(sd, ptr, raw_blksz, to);
			assert(sent == raw_blksz);
		}

		bytes_encoded += raw_blksz;
		ptr += raw_blksz;
		remaining_bytes -= raw_blksz;
	}

	if (blkbuf != NULL) free(blkbuf);
	if (tmpbuf != NULL) free(tmpbuf);

	ssz = clara_sendmsg(sd, CLARA_MSG_STREAM_END, NULL, 0,
		to, 0, 0, CLARA_CONTROL_STREAM, 0, 0);
	if (ssz == -1)
	{
		debug_printff("clara_sendmsg(CLARA_MSG_STREAM_END) failed");
		return bytes_encoded;
	}

#if defined(STREAM_STATISTICS)
	if (stp.fmt == STREAM_FORMAT_LZO1X_1)
	{
		float ratio = (float)bytes_sent / bytes_encoded;
		debug_printff("%u bytes sent, %u bytes uncompressed, %u blocks, compression ratio: %f",
			bytes_sent, bytes_encoded, blkcnt, ratio);
	}
#endif
//	debug_printff("%u bytes encoded", bytes_encoded);
	return bytes_encoded;
}

size_t
stream_recv(int sd, void *data, size_t len)
{
	size_t bytes_decoded = 0;
	ssize_t ssz;

	struct stream_params stp;
	message_type_t type = CLARA_MSG_STREAM_START;
	ssz = clara_recvmsg(sd, &type, &stp, sizeof(stp), NULL, NULL, NULL);
	if (ssz == -1)
	{
		debug_printff("clara_recvmsg(CLARA_MSG_STREAM_START) failed");
		return bytes_decoded;
	}

	size_t bytes_remaining = len;
	uint8_t *ptr = data;
	lzo_uint raw_blksz = stp.blksz;
	uint8_t *blkbuf = NULL;

	size_t enc_blksz;

	if (stp.fmt == STREAM_FORMAT_LZO1X_1)
	{
		lzo_init();
		enc_blksz = stp.blksz + stp.blksz / 16 + 64 + 3;
		blkbuf = malloc(enc_blksz);
	}

	size_t received;
#if defined(STREAM_STATISTICS)
	uint32_t blkcnt = 0;
	size_t bytes_received = 0;
#endif

	while (true)
	{
		/* initiating message */
		struct block_params blp;

		type = 0;	
		ssz = clara_recvmsg(sd, &type, &blp, sizeof(blp), NULL, NULL, NULL);
		if (ssz == -1)
		{
			debug_printff("clara_recvmsg failed");
			if (blkbuf != NULL) free(blkbuf);
			return bytes_decoded;
		}
		if ((type == CLARA_MSG_STREAM_END) && (ssz == 0))
		{
			break;
		}
		else if ((type == CLARA_MSG_BLOCK) && (ssz == sizeof(blp)))
		{
			if (raw_blksz > bytes_remaining) raw_blksz = bytes_remaining;

			if (stp.fmt == STREAM_FORMAT_LZO1X_1)
			{
				received = block_recv(sd, blkbuf, enc_blksz, &blp);
				assert(received <= enc_blksz);
				lzo1x_decompress(blkbuf, received, ptr, &raw_blksz, NULL);
#if defined(STREAM_STATISTICS)
				++blkcnt;
				bytes_received += received;
#endif
			}
			else
			{
				received = block_recv(sd, ptr, raw_blksz, &blp);
				assert(received == raw_blksz);
			}

			bytes_decoded += raw_blksz;
			ptr += raw_blksz;
			bytes_remaining -= raw_blksz;
		}
		else
		{
			debug_printff("This should never happen.");
			assert(0);
		}
	}

	if (blkbuf != NULL) free(blkbuf);

	if (bytes_remaining > 0)
	{
		debug_printff("There are %u bytes left in buffer. Probably something went wrong.", bytes_remaining);
	}

#if defined(STREAM_STATISTICS)
	if (stp.fmt == STREAM_FORMAT_LZO1X_1)
	{
		float ratio = (float)bytes_received / bytes_decoded;
		debug_printff("%u bytes received, %u bytes uncompressed, %u blocks, compression ratio: %f",
			bytes_received, bytes_decoded, blkcnt, ratio);
	}
#endif
//	debug_printff("%u bytes decoded", bytes_decoded);
	return bytes_decoded;
}

size_t
stream_forward(int sd_in, int sd_out, struct sockaddr_in *to)
{
	size_t bytes_forwarded = 0;
	ssize_t ssz;

	struct stream_params stp;
	message_type_t type = CLARA_MSG_STREAM_START;
	ssz = clara_recvmsg(sd_in, &type, &stp, sizeof(stp), NULL, NULL, NULL);
	if (ssz == -1)
	{
		debug_printff("clara_recvmsg(CLARA_MSG_STREAM_START) failed");
		return bytes_forwarded;
	}

	ssz = clara_sendmsg(sd_out, type, &stp, sizeof(stp),
		to, 0, 0, CLARA_CONTROL_STREAM, 0, 0);
	if (ssz == -1)
	{
		debug_printff("clara_sendmsg(CLARA_MSG_STREAM_START) failed");
		return bytes_forwarded;
	}

	while (true)
	{
		struct block_params blp;

		type = 0;
		ssz = clara_recvmsg(sd_in, &type, &blp, sizeof(blp), NULL, NULL, NULL);
		if (ssz == -1)
		{
			debug_printff("clara_recvmsg failed");
			return bytes_forwarded;
		}

		if ((type == CLARA_MSG_STREAM_END) && (ssz == 0))
		{
			break;
		}
		else if ((type == CLARA_MSG_BLOCK) && (ssz == sizeof(blp)))
		{
			bytes_forwarded += block_forward(sd_in, sd_out, to, &blp);
		}
		else
		{
			debug_printff("This should never happen.");
			assert(0);
		}
	}

	ssz = clara_sendmsg(sd_out, type, NULL, 0,
		to, 0, 0, CLARA_CONTROL_STREAM, 0, 0);
	if (ssz == -1)
	{
		debug_printff("clara_sendmsg(CLARA_MSG_STREAM_END) failed");
		return bytes_forwarded;
	}

//	debug_printff("%u bytes forwarded", bytes_forwarded);
	return bytes_forwarded;
}


