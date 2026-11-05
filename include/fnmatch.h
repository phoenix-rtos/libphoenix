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


#define FNM_NOMATCH     0x01
#define FNM_PATHNAME    0x02
#define FNM_PERIOD      0x04
#define FNM_NOESCAPE    0x08
#define FNM_LEADING_DIR 0x10
#define FNM_CASEFOLD    0x20
#define FNM_IGNORECASE  FNM_CASEFOLD
#define FNM_FILE_NAME   FNM_PATHNAME


int fnmatch(const char *pattern, const char *string, int flags);


#ifdef __cplusplus
}
#endif


#endif
