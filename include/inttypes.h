/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * inttypes.h
 *
 * Copyright 2019 Phoenix Systems
 * Author: Andrzej Glowinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_INTTYPES_H_
#define _LIBPHOENIX_INTTYPES_H_

#include <arch.h>
#include <stdint.h>

#ifdef __ARCH_INTTYPES
#include __ARCH_INTTYPES
#endif

#ifdef _USE_STANDARD_TYPES_STDINT

#define PRId8 _PRI_8 "d"
#define PRIu8 _PRI_8 "u"
#define PRIx8 _PRI_8 "x"
#define PRIX8 _PRI_8 "X"
#define PRIi8 _PRI_8 "i"
#define PRIo8 _PRI_8 "o"

#define PRId16 _PRI_16 "d"
#define PRIu16 _PRI_16 "u"
#define PRIx16 _PRI_16 "x"
#define PRIX16 _PRI_16 "X"
#define PRIi16 _PRI_16 "i"
#define PRIo16 _PRI_16 "o"

#define PRId32 _PRI_32 "d"
#define PRIu32 _PRI_32 "u"
#define PRIx32 _PRI_32 "x"
#define PRIX32 _PRI_32 "X"
#define PRIi32 _PRI_32 "i"
#define PRIo32 _PRI_32 "o"

#define PRId64 _PRI_64 "d"
#define PRIu64 _PRI_64 "u"
#define PRIx64 _PRI_64 "x"
#define PRIX64 _PRI_64 "X"
#define PRIi64 _PRI_64 "i"
#define PRIo64 _PRI_64 "o"

#endif

extern intmax_t strtoimax(const char *nptr, char **endptr, int base);


extern uintmax_t strtoumax(const char *nptr, char **endptr, int base);

#endif
