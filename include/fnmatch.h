/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * fnmatch.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_FNMATCH_H_
#define _LIBPHOENIX_FNMATCH_H_


#include <sys/cdefs.h>


#ifdef __cplusplus
extern "C" {
#endif


enum { FNM_NOMATCH = 0x1, FNM_PATHNAME = 0x2, FNM_PERIOD = 0x4, FNM_NOESCAPE = 0x8, FNM_LEADING_DIR = 0x10,
	FNM_CASEFOLD = 0x20, FNM_IGNORECASE = FNM_CASEFOLD, FNM_FILE_NAME = FNM_PATHNAME };


int fnmatch(const char *pattern, const char *string, int flags);


#ifdef __cplusplus
}
#endif


#endif
