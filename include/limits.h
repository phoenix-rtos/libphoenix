/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * limits.h
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_LIMITS_H_
#define _LIBPHOENIX_LIMITS_H_

#include <arch.h>


#ifndef CHAR_BIT
#define CHAR_BIT    8
#endif


#ifndef SCHAR_MIN
#define SCHAR_MIN    -128
#endif


#ifndef SCHAR_MAX
#define SCHAR_MAX    127
#endif


#ifndef UCHAR_MAX
#define UCHAR_MAX    255
#endif


#ifndef CHAR_MIN
#define CHAR_MIN SCHAR_MIN
#endif


#ifndef CHAR_MAX
#define CHAR_MAX SCHAR_MAX
#endif


#ifndef MB_LEN_MAX
#define MB_LEN_MAX 4
#endif


#ifndef SHRT_MIN
#define SHRT_MIN -32768
#endif


#ifndef SHRT_MAX
#define SHRT_MAX 32767
#endif


#ifndef USHRT_MAX
#define USHRT_MAX 65535
#endif


#ifndef INT_MIN
#define INT_MIN -2147483648
#endif


#ifndef INT_MAX
#define INT_MAX 0x7fffffff
#endif


#ifndef UINT_MAX
#define UINT_MAX 0xffffffff
#endif


#ifndef LONG_MIN
#define LONG_MIN INT_MIN
#endif


#ifndef LONG_MAX
#define LONG_MAX INT_MAX
#endif


#ifndef LONG_LONG_MAX
#define LONG_LONG_MAX 0x7fffffffffffffffll
#endif


#ifndef LONG_LONG_MIN
#define LONG_LONG_MIN (-LONG_LONG_MAX - 1)
#endif


#ifndef LLONG_MAX
#define LLONG_MAX LONG_LONG_MAX
#endif


#ifndef LLONG_MIN
#define LLONG_MIN LONG_LONG_MIN
#endif


#ifndef ULONG_MAX
#define ULONG_MAX UINT_MAX
#endif


#ifndef ULONG_LONG_MAX
#define ULONG_LONG_MAX (LONG_LONG_MAX * 2ULL + 1)
#endif


#ifndef ULLONG_MAX
#define ULLONG_MAX ULONG_LONG_MAX
#endif


#ifndef SSIZE_MAX
#define SSIZE_MAX ((ssize_t)((1ULL << (sizeof(ssize_t) * 8 - 1)) - 1))
#endif


#define PTHREAD_STACK_MIN   256
#define PATH_MAX 1024
#define ARG_MAX 128
#define _POSIX2_RE_DUP_MAX              255

#endif
