/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * grp.h
 *
 * Copyright 2018, 2026 Phoenix Systems
 * Author: Jan Sikorski, Julian Uziemblo
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
	char *gr_name;
	char *gr_passwd;
	gid_t gr_gid;
	char **gr_mem;
};


struct group *getgrnam(const char *name);


struct group *getgrgid(gid_t gid);


int getgrnam_r(const char *name, struct group *grp, char *buffer, size_t bufsize, struct group **result);


int getgrgid_r(gid_t gid, struct group *grp, char *buffer, size_t bufsize, struct group **result);


struct group *getgrent(void);


void endgrent(void);


void setgrent(void);


int initgroups(const char *user, gid_t group);


int setgroups(size_t size, const gid_t *list);


#ifdef __cplusplus
}
#endif


#endif
