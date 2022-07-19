/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * mktemp.c - create uniquely named files and directories
 *
 * Copyright 2018, 2020 Phoenix Systems
 * Author: Kamil Amanowicz, Lukasz Kosinski
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
static int mktemp(char *templt, int suffixlen)
{
	int i, len, rand, fd;
	char *tail;
	oid_t oid;

	if ((templt == NULL) || (suffixlen < 0))
		return -EINVAL;

	len = strlen(templt);
	tail = templt + len - suffixlen - 6;

	if ((len < suffixlen + 6) || strncmp(tail, "XXXXXX", 6))
		return -EINVAL;

	if ((fd = open("/dev/urandom", O_RDONLY)) < 0)
		return fd;

	for (i = 0; i < 6; i++) {
		read(fd, &rand, sizeof(rand));
		tail[i] = pfcs[rand % sizeof(pfcs)];
	}

	close(fd);

	if (lookup(templt, &oid, NULL) == EOK)
		return -EEXIST;

	return EOK;
}


int mkostemps(char *templt, int suffixlen, int flags)
{
	int fd;

	if (mktemp(templt, suffixlen) < 0)
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
	if (mktemp(templt, 0) < 0)
		return NULL;

	if (mkdir(templt, S_IRUSR | S_IWUSR | S_IXUSR) < 0)
		return NULL;

	return templt;
}
