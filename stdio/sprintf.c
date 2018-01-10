/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * vsprintf.c
 *
 * Copyright 2017 Phoenix Systems
 * Author: Adrian Kepka
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "stdlib.h"
#include "stdio.h"
#include "format.h"


typedef struct _sprintf_ctx_t {
	char *buff;
	size_t n;
} sprintf_ctx_t;


static void sprintf_feed(void *context, char c)
{
	sprintf_ctx_t* ctx = (sprintf_ctx_t *)context;

	ctx->buff[ctx->n++] = c;
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


int vsprintf(char *str, const char *format, va_list arg)
{
	sprintf_ctx_t ctx;

	ctx.buff = str;
	ctx.n = 0;

	format_parse(&ctx, sprintf_feed, format, arg);

	return ctx.n;
}


int vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
	return 0;
}


int vasprintf(char **strp, const char *fmt, va_list ap)
{
	/* Temporary */
	*strp = malloc(1024);
	vsprintf(*strp, fmt, ap);
	return 0;
}
