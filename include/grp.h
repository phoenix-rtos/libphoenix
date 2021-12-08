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


struct group {
	char *gr_name;   /* Group name */
	gid_t gr_gid;    /* Group ID */
	char **gr_mem;   /* Group member names */
	char *gr_passwd; /* Group password */
};


extern int initgroups(const char *user, gid_t group);


extern struct group *getgrnam(const char *name);


extern struct group *getgrgid(gid_t gid);


extern int setgroups(size_t size, const gid_t *list);


#endif
