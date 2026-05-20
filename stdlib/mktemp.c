/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * mktemp.c - create uniquely named files and directories
 *
 * Copyright 2018, 2020, 2023 Phoenix Systems
 * Author: Kamil Amanowicz, Lukasz Kosinski, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/minmax.h>
#include <unistd.h>

#include <sys/msg.h>
#include <sys/stat.h>


/* Portable filename character set */
static char pfcs[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789._-";


/* Generates a unique filename */
static int __mktemp(char *templt, int suffixlen)
{
	if ((templt == NULL) || (suffixlen < 0)) {
		return SET_ERRNO(-EINVAL);
	}

	int len = strlen(templt);
	char *tail = templt + len - suffixlen - 6;

	if ((len < (suffixlen + 6)) || (strncmp(tail, "XXXXXX", 6) != 0)) {
		return SET_ERRNO(-EINVAL);
	}

	int fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0) {
		/* errno set by open */
		return fd;
	}

	unsigned int rand[6];
	ssize_t left = sizeof(rand);
	do {
		ssize_t err;
		do {
			err = read(fd, (unsigned char *)rand + sizeof(rand) - left, left);
		} while ((err < 0) && (errno == -EINTR));

		if (err <= 0) {
			close(fd);
			/* errno set by read */
			return -1;
		}
		left -= err;
	} while (left != 0);
	close(fd);

	for (int i = 0; i < 6; ++i) {
		tail[i] = pfcs[rand[i] % (sizeof(pfcs) - 1)];
	}

	oid_t oid;
	if (lookup(templt, &oid, NULL) == 0) {
		return SET_ERRNO(-EEXIST);
	}

	return 0;
}


int mkostemps(char *templt, int suffixlen, int flags)
{
	int fd;

	if (__mktemp(templt, suffixlen) < 0)
		return -1;

	if ((fd = open(templt, (flags & ~0x03) | O_CREAT | O_RDWR | O_EXCL, DEFFILEMODE)) < 0)
		return -1;

	return fd;
}


int mkstemps(char *templt, int suffixlen)
{
	return mkostemps(templt, suffixlen, 0);
}


int mkostemp(char *templt, int flags)
{
	return mkostemps(templt, 0, flags);
}


int mkstemp(char *templt)
{
	return mkstemps(templt, 0);
}


char *mkdtemp(char *templt)
{
	if (__mktemp(templt, 0) < 0)
		return NULL;

	if (mkdir(templt, S_IRUSR | S_IWUSR | S_IXUSR) < 0)
		return NULL;

	return templt;
}


char *tmpnam(char *str)
{
	static char template[L_tmpnam];
	int err, tries = 0;

	if (str == NULL) {
		str = template;
	}

	do {
		(void)strcpy(str, "/tmp/fileXXXXXX");
		err = __mktemp(str, 0);
	} while ((err < 0) && errno == EEXIST && ++tries < TMP_MAX);

	return (err < 0) ? NULL : str;
}


static bool checkdir(const char *dir)
{
	struct stat statbuf;
	return stat(dir, &statbuf) >= 0 && S_ISDIR(statbuf.st_mode);
}


char *tempnam(const char *dir, const char *pfx)
{
	int err, sep = 0, tries = 0;
	size_t dirlen, pfxlen;
	char *name, *ptr;

	if (dir == NULL || !checkdir(dir)) {
		if (!checkdir(P_tmpdir)) {
			dir = "/";
		}
		else {
			dir = P_tmpdir;
		}
	}

	if (pfx == NULL) {
		pfx = "file";
		pfxlen = sizeof("file") - 1;
	}
	else {
		pfxlen = min(strlen(pfx), 5 /* POSIX */);
	}

	dirlen = strlen(dir);
	if (dirlen > 0 && dir[dirlen - 1] != '/') {
		sep = 1;
	}

	name = malloc(dirlen + sep + pfxlen + (sizeof("XXXXXX") - 1) + 1);
	if (name == NULL) {
		return NULL;
	}

	do {
		ptr = name;

		memcpy(ptr, dir, dirlen);
		ptr += dirlen;

		if (sep != 0) {
			*ptr = '/';
			ptr++;
		}

		memcpy(ptr, pfx, pfxlen);
		ptr += pfxlen;

		memcpy(ptr, "XXXXXX", sizeof("XXXXXX") - 1);
		ptr += sizeof("XXXXXX") - 1;

		*ptr = '\0';

		err = __mktemp(name, 0);
	} while (err < 0 && errno == EEXIST && ++tries < TMP_MAX);

	if (err < 0) {
		free(name);
		name = NULL;
	}

	return name;
}
