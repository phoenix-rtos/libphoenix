/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * vsnprintf.c
 *
 * Copyright 2017 Phoenix Systems
 * Author: Adrian Kepka, Krystian Wasik
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "stdlib.h"
#include "stdio.h"
#include "format.h"
#include "errno.h"


typedef struct _vsprintf_ctx_t {
	char *buff;
	size_t n;
} vsprintf_ctx_t;


typedef struct _vsnprintf_ctx_t {
	char *buff;
	size_t n;
	size_t max_len;
} vsnprintf_ctx_t;


static int vsprintf_feed(void *context, char c)
{
	vsprintf_ctx_t *ctx = (vsprintf_ctx_t *)context;

	ctx->buff[ctx->n++] = c;

	return 0;
}


static int vsnprintf_feed(void *context, char c)
{
	vsnprintf_ctx_t *ctx = (vsnprintf_ctx_t *)context;

	if ((ctx->n + 1) < ctx->max_len) {
		ctx->buff[ctx->n] = c;
	}
	else if ((ctx->n + 1) == ctx->max_len) {
		ctx->buff[ctx->n] = '\0';
	}

	/* Count anyway to return the number of characters that would have been
	 * written if buffer had been sufficiently large */
	ctx->n++;

	return 0;
}


int sprintf(char *str, const char *format, ...)
{
	va_list ap;
	int retVal;

	va_start(ap, format);
	retVal = vsprintf(str, format, ap);
	va_end(ap);

	return retVal;
}


int snprintf(char *str, size_t n, const char *format, ...)
{
	va_list ap;
	int retVal;

	va_start(ap, format);
	retVal = vsnprintf(str, n, format, ap);
	va_end(ap);

	return retVal;
}


/* errno is set by `format_parse` */
int vsprintf(char *str, const char *format, va_list arg)
{
	vsprintf_ctx_t ctx;
	int ret;

	ctx.buff = str;
	ctx.n = 0;

	ret = format_parse(&ctx, vsprintf_feed, format, arg);
	(void)vsprintf_feed(&ctx, '\0');

	return (ret == 0) ? ctx.n - 1 : -1;
}


int vsnprintf(char *str, size_t n, const char *format, va_list arg)
{
	vsnprintf_ctx_t ctx;
	int ret;

	ctx.buff = str;
	ctx.n = 0;
	ctx.max_len = n;

	ret = format_parse(&ctx, vsnprintf_feed, format, arg);
	(void)vsnprintf_feed(&ctx, '\0');

	return (ret == 0) ? ctx.n - 1 : -1;
}
