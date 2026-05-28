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


#ifdef __cplusplus
extern "C" {
#endif


struct passwd {
	char  *pw_name;
	uid_t  pw_uid;
	gid_t  pw_gid;
	char  *pw_dir;
	char  *pw_shell;
	char  *pw_passwd;
	char  *pw_gecos;
};


struct passwd *getpwnam(const char *name);


struct passwd *getpwuid(uid_t uid);


int getpwnam_r(const char *name, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result);


int getpwuid_r(uid_t uid, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result);


void endpwent(void);


struct passwd *getpwent(void);


void setpwent(void);


#ifdef __cplusplus
}
#endif


#endif
