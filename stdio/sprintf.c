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

#include "stdio.h"
#include "format.h"


typedef struct _sprintf_ctx_t {
	char *buff;
	size_t n;
} sprintf_ctx_t;


static void sprintf_feed(void *b, char c)
{
	sprintf_ctx_t* ctx = (sprintf_ctx_t *)b;

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
