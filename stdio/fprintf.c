/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * printf.c
 *
 * Copyright 2017, 2022 Phoenix Systems
 * Author: Adrian Kepka, Gerard Swiderski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "format.h"
#include "sys/debug.h"


/* clang-format off */
struct feed_ctx_s {
	/* NOTE: Structure layout optimized for 32bit, and 64bit arch */
	union {
		FILE *stream;
		int fd;
	} h;
	size_t n;
	size_t total;
	int error;
	enum { feed_hStream = 0, feed_hDescriptor } hType;
	char buff[16];
};
/* clang-format on */


static size_t safe_write(int fd, const char *buff, size_t size)
{
	size_t todo = size;
	ssize_t wlen;

	while (todo != 0) {
		wlen = write(fd, buff, todo);
		if (wlen > 0) {
			todo -= wlen;
			buff += wlen;
			continue;
		}
		else if (wlen < 0) {
			if ((errno == EINTR) || (errno == EAGAIN)) {
				continue;
			}
			break;
		}
		else {
			/* Undefined behaviour (wlen==0) */
			errno = EIO;
			break;
		}
	}

	return size - todo;
}


static void format_feed(void *context, char c)
{
	size_t res;
	struct feed_ctx_s *ctx = (struct feed_ctx_s *)context;

	if (ctx->error < 0) {
		return;
	}

	ctx->buff[ctx->n++] = c;
	ctx->buff[ctx->n] = '\0';

	if (ctx->n == sizeof(ctx->buff) - 1) {
		res = (ctx->hType == feed_hStream) ?
			fwrite(ctx->buff, 1, ctx->n, ctx->h.stream) :
			safe_write(ctx->h.fd, ctx->buff, ctx->n);

		ctx->total += res;
		if (ctx->n != res) {
			ctx->error = (ctx->hType == feed_hStream ? -1 : -errno);
		}

		ctx->n = 0;
	}
}


int fprintf(FILE *stream, const char *format, ...)
{
	int err;
	va_list arg;

	va_start(arg, format);
	err = vfprintf(stream, format, arg);
	va_end(arg);

	return err;
}


int vfprintf(FILE *stream, const char *format, va_list arg)
{
	struct feed_ctx_s ctx;
	size_t res;
	int ret;

	ctx.hType = feed_hStream;
	ctx.h.stream = stream;
	ctx.n = 0;
	ctx.total = 0;
	ctx.error = 0;

	ret = format_parse(&ctx, format_feed, format, arg);

	if ((ret == 0) && (ctx.n != 0)) {
		res = fwrite(ctx.buff, 1, ctx.n, ctx.h.stream);
		ctx.total += res;

		if (ctx.n != res) {
			ctx.error = -1;
		}
	}
	if (ret != 0) {
		return SET_ERRNO(ret);
	}
	else {
		return ctx.error < 0 ? -1 : /* check error with feof() or ferror() */ ctx.total;
	}
}


int vdprintf(int fd, const char *format, va_list arg)
{
	struct feed_ctx_s ctx;
	size_t res;
	int ret;

	ctx.hType = feed_hDescriptor;
	ctx.h.fd = fd;
	ctx.n = 0;
	ctx.total = 0;
	ctx.error = 0;

	ret = format_parse(&ctx, format_feed, format, arg);

	if ((ret == 0) && (ctx.n != 0)) {
		res = safe_write(ctx.h.fd, ctx.buff, ctx.n);
		ctx.total += res;

		if (ctx.n != res) {
			ctx.error = -errno;
		}
	}

	if (ret != 0) {
		return SET_ERRNO(ret);
	}
	else {
		return ctx.error < 0 ? SET_ERRNO(ctx.error) : ctx.total;
	}
}


int dprintf(int fd, const char *format, ...)
{
	int retVal;
	va_list ap;

	va_start(ap, format);
	retVal = vdprintf(fd, format, ap);
	va_end(ap);

	return retVal;
}
