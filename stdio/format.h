/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * format.h
 *
 * Copyright 2017 Phoenix Systems
 * Author: Adrian Kepka
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_STDIO_FORMAT_H
#define _LIBPHOENIX_STDIO_FORMAT_H

#include <stdarg.h>


#define FORMAT_NIL_STR     "(nil)"
#define FORMAT_NIL_STR_LEN (sizeof(FORMAT_NIL_STR) - 1)


typedef void (*feedfunc)(void *, char);


extern int format_parse(void *ctx, feedfunc feed, const char *format, va_list args);


#endif
