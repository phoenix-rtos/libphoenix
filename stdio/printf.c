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
#include "errno.h"

typedef struct _printf_ctx_t {
	size_t n;
} printf_ctx_t;


static int printf_feed(void *context, char c)
{
	int ret;
	size_t *n = context;

	ret = putchar(c);
	if (ret == EOF) {
		return -1;
	}

	*n = *n + 1;
	return ret;
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


/* errno is set by `format_parse` */
int vprintf(const char *format, va_list arg)
{
	size_t n = 0;
	int ret = format_parse(&n, printf_feed, format, arg);
	if (ret == 0) {
		return n;
	}
	else {
		return -1;
	}
}
