/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Password related functions
 *
 * Copyright 2018, 2021 Phoenix Systems
 * Author: Jan Sikorski, Mateusz Niewiadomski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <ctype.h>
#include <pwd.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

#define PASSWD_PATH "/etc/passwd"

struct passwd pwnam;
char pw_name[NAME_MAX];
char intstr[16];
char pw_gecos[128];
char pw_dir[PATH_MAX];
char pw_shell[PATH_MAX];
char pw_passwd[128];


static int readpwentry(FILE *fp, char *pwentry, unsigned int maxlen, unsigned int islast)
{
	int c = 0;
	unsigned int i = 0, isbad = 0;

	do {
		c = fgetc(fp);
		if (c == ':' || c == '\n' || c == EOF)
			break;

		if (i < maxlen - 1)
			pwentry[i++] = c;
		else
			isbad = 1;
	} while (!isbad);
	pwentry[i] = '\0';

	if ((c == EOF || c == '\n') && !islast)
		isbad = 1;

	return isbad;
}


static inline int readuid(int readpwentry, uid_t *uid)
{
	int i, err = readpwentry;
	*uid = strtoul(intstr, NULL, 10);

	if (readpwentry == 0) {
		for (i = 0; intstr[i] != '\0'; ++i) {
			if (!isdigit(intstr[i])) {
				err = 1;
				break;
			}
		}
	}

	return err;
}


static struct passwd *getpwby(const char *name, uid_t *uid)
{
	struct passwd *retpwnam = NULL;
	FILE *fp = fopen(PASSWD_PATH, "r");

	if (fp != NULL) {
		pwnam.pw_name = pw_name;
		pwnam.pw_dir = pw_dir;
		pwnam.pw_gecos = pw_gecos;
		pwnam.pw_shell = pw_shell;
		pwnam.pw_passwd = pw_passwd;

		do {
			if (readpwentry(fp, pwnam.pw_name, NAME_MAX, 0))
				break;
			if (readpwentry(fp, pwnam.pw_passwd, sizeof(pw_passwd), 0))
				break;
			if (readuid(readpwentry(fp, intstr, sizeof(intstr), 0), &pwnam.pw_uid))
				break;
			if (readuid(readpwentry(fp, intstr, sizeof(intstr), 0), &pwnam.pw_gid))
				break;
			if (readpwentry(fp, pwnam.pw_gecos, sizeof(pw_gecos), 0))
				break;
			if (readpwentry(fp, pwnam.pw_dir, PATH_MAX, 0))
				break;
			if (readpwentry(fp, pwnam.pw_shell, PATH_MAX, 1))
				break;

			if (name != NULL && strcmp(pwnam.pw_name, name) == 0) {
				retpwnam = &pwnam;
				break;
			}
			else if (uid != NULL && pwnam.pw_uid == *uid) {
				retpwnam = &pwnam;
				break;
			}
		} while (!feof(fp));

		fclose(fp);
	}
	return retpwnam;
}


struct passwd *getpwnam(const char *name)
{
	return (name == NULL) ? NULL : getpwby(name, NULL);
}


struct passwd *getpwuid(uid_t uid)
{
	return getpwby(NULL, &uid);
}


int getpwnam_r(const char *name, struct passwd *pwd, char *buffer,
	       size_t bufsize, struct passwd **result)
{
	return -1;
}
