/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * err (BSD warn/error convenience functions)
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michal Miroslaw
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "err.h"
#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


extern const char *argv_progname;


static void vmsg(int perr, const char *fmt, va_list va)
{
	const char *errfmt = ": %s (%d)\n";
	int err = errno;

	printf("%s: ", argv_progname);

	if (fmt)
		vprintf(fmt, va);
	else
		errfmt += 2;

	if (!perr)
		errfmt = "\n";

	printf(errfmt, strerror(err), err);
}


void vwarn(const char *fmt, va_list va)
{
	vmsg(1, fmt, va);
}


void vwarnx(const char *fmt, va_list va)
{
	vmsg(0, fmt, va);
}


void verr(int status, const char *fmt, va_list va)
{
	vmsg(1, fmt, va);
	exit(status);
}


void verrx(int status, const char *fmt, va_list va)
{
	vmsg(0, fmt, va);
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
