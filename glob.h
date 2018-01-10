/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * glob.h - pathname pattern-matching types
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_GLOB_H_
#define _LIBPHOENIX_GLOB_H_


#include <stdint.h>


enum {
	GLOB_ERR = 1 << 0,
	GLOB_MARK = 1 << 1,
	GLOB_NOSORT = 1 << 2,
	GLOB_DOOFFS = 1 << 3,
	GLOB_NOCHECK = 1 << 4,
	GLOB_APPEND = 1 << 5,
	GLOB_NOESCAPE = 1 << 6,
	GLOB_PERIOD = 1 << 7,
	GLOB_ALTDIRFUNC = 1 << 8,
	GLOB_BRACE = 1 << 9,
	GLOB_NOMAGIC = 1 << 10,
	GLOB_TILDE = 1 << 11,
	GLOB_TILDE_CHECK = 1 << 12,
	GLOB_ONLYDIR = 1 << 13,
};


enum { GLOB_NOSPACE = 1, GLOB_ABORTED, GLOB_NOMATCH };


typedef struct {
	size_t   gl_pathc;
	char   **gl_pathv;
	size_t   gl_offs;
} glob_t;


extern int glob(const char *restrict pattern, int flags, int (*errfunc)(const char *epath, int eerrno), glob_t *restrict pglob);


extern void globfree(glob_t *pglob);


#endif
