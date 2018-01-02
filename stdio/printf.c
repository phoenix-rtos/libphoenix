/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * printf.c
 *
 * Copyright 2017 Phoenix Systems
 * Author: Adrian Kepka
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "stdio.h"
#include "format.h"
#include "sys/debug.h"
#include "unistd.h"

typedef struct _printf_ctx_t {
	char buff[16];
	int n;
	size_t total;
} printf_ctx_t;


static void printf_feed(void *context, char c)
{
	printf_ctx_t* ctx = (printf_ctx_t *)context;

	ctx->buff[ctx->n++] = c;
	ctx->buff[ctx->n] = '\0';

	if(ctx->n == sizeof(ctx->buff) - 1) {

		if (write(1, ctx->buff, ctx->n) < 0)
			debug(&ctx->buff[0]);

		ctx->n = 0;
	}

	ctx->total++;
}


int printf(const char *format, ...)
{
	int retVal;
	va_list arg;

	va_start(arg, format);
	retVal = vprintf(format, arg);
	va_end(arg);

	return retVal;
}


int vprintf(const char *format, va_list arg)
{
	printf_ctx_t ctx;

	ctx.n = 0;
	ctx.total = 0;

	format_parse(&ctx, printf_feed, format, arg);

	if(ctx.n % sizeof(ctx.buff) != 0) {
		if (write(1, ctx.buff, ctx.n) < 0)
			debug(ctx.buff);
	}

	return ctx.total;
}
