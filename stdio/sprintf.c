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


typedef struct _vsnprintf_ctx_t {
	char *buff;
	size_t n;
	size_t max_len;
} vsnprintf_ctx_t;


static void vsnprintf_feed(void *context, char c)
{
	vsnprintf_ctx_t* ctx = (vsnprintf_ctx_t *)context;

	if (ctx->max_len == 0 || ctx->n + 1 < ctx->max_len) {
		ctx->buff[ctx->n++] = c;

		if (ctx->max_len && ctx->n + 1 == ctx->max_len)
			ctx->buff[ctx->n] = '\0';

	} else {
		/* Count anyway to return the number of characters that would have been
		 * written if buffer had been sufficiently large */
		ctx->n++;
	}
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


int vsprintf(char *str, const char *format, va_list arg)
{
	vsnprintf_ctx_t ctx;

	ctx.buff = str;
	ctx.n = 0;
	ctx.max_len = 0;

	format_parse(&ctx, vsnprintf_feed, format, arg);
	vsnprintf_feed(&ctx, '\0');

	return ctx.n - 1;
}


int vsnprintf(char *str, size_t n, const char *format, va_list arg)
{
	vsnprintf_ctx_t ctx;

	if (n == 0) return 0;
	if (n == 1) {
		str[0] = '\0';
		return 0;
	}

	ctx.buff = str;
	ctx.n = 0;
	ctx.max_len = n;

	format_parse(&ctx, vsnprintf_feed, format, arg);
	vsnprintf_feed(&ctx, '\0');

	return ctx.n - 1;
}
