/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * err (BSD warn/error convenience functions)
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "err.h"
#include "stdlib.h"


void vwarn(const char *fmt, va_list va)
{
	// FIXME
}


void vwarnx(const char *fmt, va_list va)
{
	// FIXME
}


void verr(int status, const char *fmt, va_list va)
{
	vwarn(fmt, va);
	exit(status);
}


void verrx(int status, const char *fmt, va_list va)
{
	vwarnx(fmt, va);
	exit(status);
}


#define VA_WRAP1(f) \
void f(const char *fmt, ...) \
{ \
	va_list ap; \
\
	va_start(ap, fmt); \
	v##f(fmt, ap); \
	va_end(ap); \
}


#define VA_WRAP2(f) \
void f(int status, const char *fmt, ...) \
{ \
	va_list ap; \
\
	va_start(ap, fmt); \
	v##f(status, fmt, ap); \
	va_end(ap); \
}


VA_WRAP1(warn)
VA_WRAP1(warnx)
VA_WRAP2(err)
VA_WRAP2(errx)
