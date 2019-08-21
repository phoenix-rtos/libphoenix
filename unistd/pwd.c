/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Password related functions
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <pwd.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

#define PASSWD_PATH "/etc/passwd"

struct passwd pwnam;
char pw_name[NAME_MAX];
char pw_gecos[128];
char pw_dir[PATH_MAX];
char pw_shell[PATH_MAX];
char pw_passwd[128];


struct passwd *getpwnam(const char *name)
{
	FILE *fp;
	char buf[1024];

	if ((fp = fopen(PASSWD_PATH, "r")) == NULL)
		return NULL;

	pwnam.pw_name = pw_name;
	pwnam.pw_gecos = pw_name;
	pwnam.pw_dir = pw_dir;
	pwnam.pw_gecos = pw_gecos;
	pwnam.pw_shell = pw_shell;
	pwnam.pw_passwd = pw_passwd;

	while (fgets(buf, sizeof(buf), fp)) {
		sscanf (buf, "%[^:]:%[^:]:%d:%d:%[^:]:%[^:]:%s\n",
			pwnam.pw_name, pwnam.pw_passwd,
			&pwnam.pw_uid, &pwnam.pw_gid,
			pwnam.pw_gecos, pwnam.pw_dir, pwnam.pw_shell);
		if (strcmp(pwnam.pw_name, name) == 0) {
			fclose(fp);
			return &pwnam;
		}
	}
	fclose(fp);
	return NULL;
}


struct passwd *getpwuid(uid_t uid)
{
	FILE *fp;
	char buf[1024];

	if ((fp = fopen(PASSWD_PATH, "r")) == NULL)
		return NULL;

	pwnam.pw_name = pw_name;
	pwnam.pw_gecos = pw_name;
	pwnam.pw_dir = pw_dir;
	pwnam.pw_gecos = pw_gecos;
	pwnam.pw_shell = pw_shell;
	pwnam.pw_passwd = pw_passwd;

	while (fgets(buf, sizeof(buf), fp)) {
		sscanf (buf, "%[^:]:%[^:]:%d:%d:%[^:]:%[^:]:%s\n",
			pwnam.pw_name, pwnam.pw_passwd,
			&pwnam.pw_uid, &pwnam.pw_gid,
			pwnam.pw_gecos, pwnam.pw_dir, pwnam.pw_shell);
		if (pwnam.pw_uid == uid) {
			fclose(fp);
			return &pwnam;
		}
	}
	fclose(fp);
	return NULL;
}


int getpwnam_r(const char *name, struct passwd *pwd, char *buffer,
	       size_t bufsize, struct passwd **result)
{
	return -1;
}
