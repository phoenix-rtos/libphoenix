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
#include <stdlib.h>
#include <string.h>
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

	int rand[6];
	ssize_t left = sizeof(rand);
	do {
		ssize_t err;
		do {
			err = read(fd, rand + sizeof(rand) - left, left);
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
