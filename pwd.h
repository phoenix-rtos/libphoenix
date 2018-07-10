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
	char  *pw_name;
	uid_t  pw_uid;
	gid_t  pw_gid;
	char  *pw_dir;
	char  *pw_shell;
};


extern struct passwd *getpwnam(const char *name);


extern struct passwd *getpwuid(uid_t uid);


extern int getpwnam_r(const char *name, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result);


#endif
