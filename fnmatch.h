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


enum { FNM_NOMATCH = 1, FNM_PATHNAME = 2, FNM_PERION = 4, FNM_NOESCAPE = 8 };


int fnmatch(const char *pattern, const char *string, int flags);


#endif
