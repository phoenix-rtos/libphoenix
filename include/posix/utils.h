/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * POSIX implementation - utilities
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_POSIX_UTILS_H_
#define _LIBPHOENIX_POSIX_UTILS_H_


#ifdef __cplusplus
extern "C" {
#endif


extern int create_dev(oid_t *oid, const char *path);


extern void splitname(char *path, char **base, char **dir);


extern char *canonicalize_file_name(const char *path);


#ifdef __cplusplus
}
#endif


#endif
