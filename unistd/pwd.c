/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Password related functions
 *
 * Copyright 2018, 2021, 2026 Phoenix Systems
 * Author: Jan Sikorski, Mateusz Niewiadomski, Julian Uziembło
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdbool.h>
#include <errno.h>
#include <ctype.h>
#include <pwd.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PASSWD_PATH "/etc/passwd"


static struct {
	FILE *pwdfp;
	struct passwd pwentry;
	char pwentrybuf[NAME_MAX + 128 /* pw_passwd */ + 128 /* pw_gecos */ + PATH_MAX + PATH_MAX];
} pwd_common;


static bool pwenteof(FILE *fp)
{
	int c;
	if (feof(fp)) {
		return true;
	}

	while ((c = fgetc(fp)) == '\n')
		;

	if (c == EOF) {
		return true;
	}
	ungetc(c, fp);

	return false;
}


/* read field is terminated with '\0' */
static int readpwfield(FILE *fp, char *pwentry, size_t maxlen, bool islast)
{
	int c = 0;
	size_t i = 0;

	if (maxlen == 0) {
		return -ERANGE;
	}

	for (;;) {
		c = fgetc(fp);
		if (c == ':' || c == '\n' || c == EOF) {
			break;
		}

		if (i < maxlen - 1) {
			pwentry[i++] = c;
		}
		else {
			return -ERANGE;
		}
	}

	if ((c == EOF || c == '\n') && !islast) {
		if (c != EOF) {
			ungetc(c, fp);
		}
		return -EIO;
	}

	if (c != EOF && c != '\n' && islast) {
		ungetc(c, fp);
		return -EIO;
	}

	pwentry[i++] = '\0';

	return i;
}


static int readid(char *buf, int *id)
{
	int i, err = 0;
	unsigned long conv;
	char *endp;

	if (buf[0] == '\0') {
		return -1;
	}

	for (i = 0; buf[i] != '\0'; ++i) {
		if (!isdigit((unsigned char)buf[i])) {
			return -1;
		}
	}

	errno = 0;
	conv = strtoul(buf, &endp, 10);
	if (errno != 0 || endp == buf || conv > INT_MAX) {
		return -1;
	}

	*id = conv;

	return err;
}


/* reads full entry line into pwentry. returns 0 on success or __positive__ error code on error */
static int readpwentryline(FILE *fp, struct passwd *pwd, char *buf, size_t bufsize)
{
	int c, n = 0, len, ret = 0, id;
	char intstr[16];

	do {
		pwd->pw_name = buf;
		len = readpwfield(fp, pwd->pw_name, bufsize, false);
		if (len < 0) {
			ret = -len;
			break;
		}
		n += len;

		pwd->pw_passwd = buf + n;
		len = readpwfield(fp, pwd->pw_passwd, bufsize - n, false);
		if (len < 0) {
			ret = -len;
			break;
		}
		n += len;

		len = readpwfield(fp, intstr, sizeof(intstr), false);
		if (len < 0) {
			ret = -len;
			break;
		}
		if (readid(intstr, &id) < 0) {
			ret = EINVAL;
			break;
		}
		pwd->pw_uid = (uid_t)id;

		len = readpwfield(fp, intstr, sizeof(intstr), false);
		if (len < 0) {
			ret = -len;
			break;
		}
		if (readid(intstr, &id) < 0) {
			ret = EINVAL;
			break;
		}
		pwd->pw_gid = (gid_t)id;

		pwd->pw_gecos = buf + n;
		len = readpwfield(fp, pwd->pw_gecos, bufsize - n, false);
		if (len < 0) {
			ret = -len;
			break;
		}
		n += len;

		pwd->pw_dir = buf + n;
		len = readpwfield(fp, pwd->pw_dir, bufsize - n, false);
		if (len < 0) {
			ret = -len;
			break;
		}
		n += len;

		pwd->pw_shell = buf + n;
		len = readpwfield(fp, pwd->pw_shell, bufsize - n, true);
		if (len < 0) {
			ret = -len;
			break;
		}
	} while (0);

	if (ret != 0) {
		while ((c = fgetc(fp)) != EOF && c != '\n')
			;
	}

	return ret;
}


static int getpwby_r(const char *name, uid_t *uid, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result, bool skipOnErange)
{
	int ret = 0;
	*result = NULL;

	FILE *fp = fopen(PASSWD_PATH, "r");
	if (fp == NULL) {
		return errno;
	}

	while (!pwenteof(fp)) {
		ret = readpwentryline(fp, pwd, buffer, bufsize);
		if (ret != 0) {
			if (ret == ERANGE && !skipOnErange) {
				break;
			}
			else {
				ret = 0;
				continue;
			}
		}

		if (name != NULL && strcmp(pwd->pw_name, name) == 0) {
			*result = pwd;
			break;
		}

		if (uid != NULL && pwd->pw_uid == *uid) {
			*result = pwd;
			break;
		}
	}

	if (ferror(fp)) {
		ret = EIO;
	}

	fclose(fp);
	return ret;
}


static struct passwd *getpwby(const char *name, uid_t *uid)
{
	struct passwd *result;
	int ret = getpwby_r(name, uid, &pwd_common.pwentry, pwd_common.pwentrybuf, sizeof(pwd_common.pwentrybuf), &result, true);
	if (ret != 0) {
		errno = ret;
	}
	return result;
}


struct passwd *getpwnam(const char *name)
{
	if (name == NULL) {
		errno = EINVAL;
		return NULL;
	}
	return getpwby(name, NULL);
}


struct passwd *getpwuid(uid_t uid)
{
	return getpwby(NULL, &uid);
}


int getpwnam_r(const char *name, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result)
{
	if (name == NULL) {
		*result = NULL;
		return EINVAL;
	}
	return getpwby_r(name, NULL, pwd, buffer, bufsize, result, false);
}


int getpwuid_r(uid_t uid, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result)
{
	return getpwby_r(NULL, &uid, pwd, buffer, bufsize, result, false);
}


struct passwd *getpwent(void)
{
	if (pwd_common.pwdfp == NULL) {
		pwd_common.pwdfp = fopen(PASSWD_PATH, "r");
		if (pwd_common.pwdfp == NULL) {
			return NULL;
		}
	}

	/* skip malformed lines */
	do {
		if (pwenteof(pwd_common.pwdfp)) {
			return NULL;
		}
	} while (readpwentryline(pwd_common.pwdfp, &pwd_common.pwentry, pwd_common.pwentrybuf, sizeof(pwd_common.pwentrybuf)) != 0);

	return &pwd_common.pwentry;
}


void endpwent(void)
{
	if (pwd_common.pwdfp != NULL) {
		(void)fclose(pwd_common.pwdfp);
		pwd_common.pwdfp = NULL;
	}
}


void setpwent(void)
{
	if (pwd_common.pwdfp != NULL) {
		rewind(pwd_common.pwdfp);
	}
}
