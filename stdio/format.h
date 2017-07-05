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

#include "stdarg.h"


typedef void (*feedfunc)(void *, char);


extern void format_parse(void *ctx, feedfunc feed, const char *format, va_list args);


#endif
