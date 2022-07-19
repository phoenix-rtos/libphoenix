/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * grp.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_GRP_H_
#define _LIBPHOENIX_GRP_H_


#include <sys/types.h>


#ifdef __cplusplus
extern "C" {
#endif


struct group {
	char   *gr_name;
	gid_t   gr_gid;
	char  **gr_mem;
	char   *gr_passwd;
};


int initgroups(const char *user, gid_t group);


struct group *getgrnam(const char *name);


struct group *getgrgid(gid_t gid);


int setgroups(size_t size, const gid_t *list);


#ifdef __cplusplus
}
#endif


#endif
