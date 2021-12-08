/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * pwd.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_PWD_H_
#define _LIBPHOENIX_PWD_H_

#include <sys/types.h>


struct passwd {
	char *pw_name;   /* User login name */
	uid_t pw_uid;    /* User ID */
	gid_t pw_gid;    /* Group ID */
	char *pw_dir;    /* Initial working directory */
	char *pw_shell;  /* Program to use as shell */
	char *pw_passwd; /* User password */
	char *pw_gecos;  /* User information */
};


extern struct passwd *getpwnam(const char *name);


extern struct passwd *getpwuid(uid_t uid);


extern int getpwnam_r(const char *name, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result);


#endif
