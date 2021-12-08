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

#include <stdio.h>
#include <stdlib.h>


int vasprintf(char **strp, const char *fmt, va_list ap)
{
	*strp = malloc(1024); /* FIXME */
	vsprintf(*strp, fmt, ap);
	return 0;
}


int asprintf(char **strp, const char *format, ...)
{
	va_list ap;
	int retVal;

	va_start(ap, format);
	retVal = vasprintf(strp, format, ap);
	va_end(ap);

	return retVal;
}
