/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * wchar.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_WCHAR_H_
#define _LIBPHOENIX_WCHAR_H_

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>


#define WEOF ((wchar_t)-1) /* Constant expression of type wint_t that is returned by several WP functions to indicate end of file */


typedef int wint_t; /* Should store any valid value of wchar_t or WEOF */


#endif
