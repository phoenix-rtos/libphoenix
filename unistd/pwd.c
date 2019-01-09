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

#define PASSDB_PATH "/etc/passwd"

struct passwd pwnam;
char pw_name[NAME_MAX];
char pw_dir[PATH_MAX];
char pw_shell[PATH_MAX];
char pw_passwd[128];

struct passwd *getpwnam(const char *name)
{
	char passdb_ln[1024] = {0};
	char *cur, *ret;
	int len;
	FILE *passdb = fopen(PASSDB_PATH, "r");

	if (passdb == NULL)
		return NULL;

	pwnam.pw_name = pw_name;
	pwnam.pw_gecos = pw_name;
	pwnam.pw_dir = pw_dir;
	pwnam.pw_shell = pw_shell;
	pwnam.pw_passwd = pw_passwd;

	while (fgets(passdb_ln, 1024, passdb) != NULL) {

		/* we assume passwd file line format:
		 * username:passwd:uid:gid:comment:dir:shell */

		ret = strchr(passdb_ln, ':');
		*ret = 0;
		if (!strcmp(name, passdb_ln)) {
			len = strlen(passdb_ln);
			memcpy(pwnam.pw_name, passdb_ln, len + 1);

			cur = ret + 1;
			ret = strchr(cur, ':');
			*ret = 0;
			len = strlen(cur);
			memcpy(pwnam.pw_passwd, cur, len + 1);

			cur = ret + 1;
			ret = strchr(cur, ':');
			*ret = 0;
			pwnam.pw_uid = atoi(cur);

			cur = ret + 1;
			ret = strchr(cur, ':');
			*ret = 0;
			pwnam.pw_gid = atoi(cur);

			/* skip comment */
			cur = ret + 1;
			ret = strchr(cur, ':');
			*ret = 0;

			cur = ret + 1;
			ret = strchr(cur, ':');
			*ret = 0;
			len = strlen(cur);
			memcpy(pwnam.pw_dir, cur, len + 1);

			cur = ret + 1;
			ret = strchr(cur, '\n');
			if (ret != NULL)
				*ret = 0;
			len = strlen(cur);
			memcpy(pwnam.pw_shell , cur, len + 1);

			fclose(passdb);
			return &pwnam;
		}
	}

	fclose(passdb);
	return NULL;
}


struct passwd *getpwuid(uid_t uid)
{
	return NULL;
}


int getpwnam_r(const char *name, struct passwd *pwd, char *buffer,
	       size_t bufsize, struct passwd **result)
{
	return -1;
}
