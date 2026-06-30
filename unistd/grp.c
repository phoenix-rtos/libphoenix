/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Group related functions
 *
 * Copyright 2026 Phoenix Systems
 * Author: Julian Uziembło
 *
 * This file is part of Phoenix-RTOS.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdbool.h>
#include <errno.h>
#include <sys/minmax.h>
#include <ctype.h>
#include <grp.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define GRP_PATH "/etc/group"

#define GRP_DEFAULT_BUFSIZE 128
#define GRP_MAX_BUFSIZE     2048


static struct {
	FILE *fp;
	struct group grp;
	char *buf;
	size_t size;
	pthread_mutex_t lock;
} grp_common;


__attribute__((destructor(101))) static void grp_destr(void)
{
	pthread_mutex_destroy(&grp_common.lock);

	if (grp_common.buf != NULL) {
		free(grp_common.buf);
	}
}


static int grp_bufferGrow(void)
{
	/* cap the realloc */
	if (grp_common.size == GRP_MAX_BUFSIZE) {
		return -ENOMEM;
	}

	size_t newSize = min(GRP_MAX_BUFSIZE, max(GRP_DEFAULT_BUFSIZE, grp_common.size + grp_common.size / 2));

	char *newBuf = realloc(grp_common.buf, newSize);
	if (newBuf == NULL) {
		return -ENOMEM;
	}

	grp_common.buf = newBuf;
	grp_common.size = newSize;

	return 0;
}


static bool grenteof(FILE *fp)
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


static int readuntil(FILE *fp, char *buf, size_t maxlen, const char sep, bool *islast)
{
	int c = 0;
	size_t i = 0;

	if (maxlen == 0) {
		return -ERANGE;
	}

	for (;;) {
		c = fgetc(fp);
		if (c == sep || c == '\n' || c == EOF) {
			break;
		}

		if (i < maxlen - 1) {
			buf[i++] = c;
		}
		else {
			return -ERANGE;
		}
	}

	*islast = (c == EOF || c == '\n');
	buf[i++] = '\0';

	return i;
}


/* read field is terminated with '\0'. returns 0 on success and negative error on failure */
static int readgrfield(FILE *fp, char *grpbuf, size_t maxlen)
{
	int res = 0;
	bool islast;

	res = readuntil(fp, grpbuf, maxlen, ':', &islast);
	if (res < 0) {
		return res;
	}

	if (islast) {
		/* add back a newline so we can skip it later */
		ungetc('\n', fp);
		return -EINVAL;
	}

	return res;
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


static void skipline(FILE *fp)
{
	int c;
	while ((c = fgetc(fp)) != EOF && c != '\n')
		;
}


static int readgrusers(FILE *fp, struct group *grp, char *buf, size_t bufsize)
{
	bool islast;
	int len, i = 0, ret = 0;
	uintptr_t start, end;

	if (sizeof(char *) > bufsize) {
		skipline(fp);
		return ERANGE;
	}

	/* we keep gr_mem array at the end of the buffer, so align the start and end */
	start = ((uintptr_t)buf + sizeof(char *) - 1) & ~(sizeof(char *) - 1);
	end = ((uintptr_t)buf + bufsize) & ~(sizeof(char *) - 1);
	if (start >= end) {
		return ERANGE;
	}
	buf = (char *)start;
	bufsize = end - start;

	bufsize -= sizeof(char *);
	*(char **)(buf + bufsize) = NULL;

	do {
		len = readuntil(fp, buf, bufsize, ',', &islast);
		if (len < 0) {
			ret = -len;
			break;
		}

		if (len == 1) {
			if (islast) {
				break;
			}
			else {
				continue;
			}
		}

		if (sizeof(char *) > bufsize - len) {
			ret = ERANGE;
			break;
		}

		bufsize -= sizeof(char *);
		*(char **)(buf + bufsize) = buf;
		buf += len;
		bufsize -= len;

		i++;
	} while (!islast);

	if (ret != 0) {
		skipline(fp);
	}
	else {
		grp->gr_mem = (char **)(buf + bufsize);

		if (i > 1) {
			for (int j = 0; j < i / 2; j++) {
				char *tmp = grp->gr_mem[j];
				grp->gr_mem[j] = grp->gr_mem[i - 1 - j];
				grp->gr_mem[i - 1 - j] = tmp;
			}
		}
	}

	return ret;
}


/* reads only the header of a row - gr_name, gr_group and gr_gid. returns 0 on success or __positive__ error code on error */
static int readgrhdr(FILE *fp, struct group *grp, char **buf, size_t *bufsize)
{
	int ret = 0, len = 0, id;
	char intstr[16];

	do {
		grp->gr_name = *buf;
		len = readgrfield(fp, grp->gr_name, *bufsize);
		if (len < 0) {
			ret = -len;
			break;
		}
		*bufsize -= len;
		*buf += len;

		grp->gr_passwd = *buf;
		len = readgrfield(fp, grp->gr_passwd, *bufsize);
		if (len < 0) {
			ret = -len;
			break;
		}
		*bufsize -= len;
		*buf += len;

		len = readgrfield(fp, intstr, sizeof(intstr));
		if (len < 0) {
			/* return EINVAL instead of ERANGE to signal that the GID is improper (too large) */
			ret = EINVAL;
			break;
		}
		if (readid(intstr, &id) < 0) {
			ret = EINVAL;
			break;
		}
		grp->gr_gid = (gid_t)id;
	} while (0);

	if (ret != 0) {
		skipline(fp);
	}

	return ret;
}


static int getlinematch(FILE *fp, const char *name, gid_t *gid, struct group *grp, char *buffer, size_t bufsize, struct group **result, bool staticBuf)
{
	int ret = 0;
	char *currentBuf = buffer;
	size_t currentSize = bufsize;

	while (!grenteof(fp)) {
		long pos = ftell(fp);

		ret = readgrhdr(fp, grp, &currentBuf, &currentSize);
		if (ret == 0) {
			if ((name == NULL && gid == NULL) || (name != NULL && strcmp(grp->gr_name, name) == 0) || (gid != NULL && grp->gr_gid == *gid)) {
				ret = readgrusers(fp, grp, currentBuf, currentSize);
			}
			else {
				skipline(fp);
				continue;
			}
		}

		if (ret == ERANGE) {
			if (!staticBuf) {
				break;
			}

			/* grow the buffer and read the line again */
			if (grp_bufferGrow() < 0) {
				ret = ENOMEM;
				break;
			}

			currentBuf = grp_common.buf;
			currentSize = grp_common.size;

			if (fseek(fp, pos, SEEK_SET) < 0) {
				ret = EIO;
				break;
			}

			continue;
		}
		else if (ret != 0) {
			/* skip malformed lines */
			ret = 0;
			continue;
		}
		else {
			*result = grp;
			break;
		}
	}

	if (ferror(fp)) {
		ret = EIO;
		*result = NULL;
	}

	return ret;
}


static int getgrby_r(const char *name, gid_t *gid, struct group *grp, char *buffer, size_t bufsize, struct group **result, bool staticBuf)
{
	int ret = 0;

	*result = NULL;

	FILE *fp = fopen(GRP_PATH, "r");
	if (fp == NULL) {
		return errno;
	}

	ret = getlinematch(fp, name, gid, grp, buffer, bufsize, result, staticBuf);

	fclose(fp);

	return ret;
}


static struct group *getgrby(const char *name, gid_t *gid)
{
	struct group *result;

	if (pthread_mutex_lock(&grp_common.lock) != 0) {
		errno = ENOMEM;
		return NULL;
	}

	int ret = getgrby_r(name, gid, &grp_common.grp, grp_common.buf, grp_common.size, &result, true);

	(void)pthread_mutex_unlock(&grp_common.lock);

	if (ret != 0) {
		errno = ret;
	}
	return result;
}


struct group *getgrnam(const char *name)
{
	if (name == NULL) {
		errno = EINVAL;
		return NULL;
	}
	return getgrby(name, NULL);
}


struct group *getgrgid(gid_t gid)
{
	return getgrby(NULL, &gid);
}


int getgrnam_r(const char *name, struct group *grp, char *buffer, size_t bufsize, struct group **result)
{
	if (name == NULL) {
		*result = NULL;
		return EINVAL;
	}
	return getgrby_r(name, NULL, grp, buffer, bufsize, result, false);
}


int getgrgid_r(gid_t gid, struct group *grp, char *buffer, size_t bufsize, struct group **result)
{
	return getgrby_r(NULL, &gid, grp, buffer, bufsize, result, false);
}


struct group *getgrent(void)
{
	struct group *result = NULL;
	int ret;

	if (pthread_mutex_lock(&grp_common.lock) != 0) {
		errno = ENOMEM;
		return NULL;
	}

	do {
		if (grp_common.fp == NULL) {
			grp_common.fp = fopen(GRP_PATH, "r");
			if (grp_common.fp == NULL) {
				break;
			}
		}

		if (grenteof(grp_common.fp)) {
			if (ferror(grp_common.fp)) {
				errno = EIO;
			}
			break;
		}

		ret = getlinematch(grp_common.fp, NULL, NULL, &grp_common.grp, grp_common.buf, grp_common.size, &result, true);
		if (ret != 0) {
			errno = ret;
			break;
		}
	} while (0);

	(void)pthread_mutex_unlock(&grp_common.lock);

	return result;
}


void endgrent(void)
{
	(void)pthread_mutex_lock(&grp_common.lock);

	if (grp_common.fp != NULL) {
		(void)fclose(grp_common.fp);
		grp_common.fp = NULL;
	}

	(void)pthread_mutex_unlock(&grp_common.lock);
}


void setgrent(void)
{
	(void)pthread_mutex_lock(&grp_common.lock);

	if (grp_common.fp == NULL) {
		grp_common.fp = fopen(GRP_PATH, "r");
	}
	if (grp_common.fp != NULL) {
		rewind(grp_common.fp);
	}

	(void)pthread_mutex_unlock(&grp_common.lock);
}
