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
static int mktemp(char *template, int suffixlen)
{
	int i, len, rand, fd;
	char *tail;
	oid_t oid;

	if ((template == NULL) || (suffixlen < 0))
		return -EINVAL;

	len = strlen(template);
	tail = template + len - suffixlen - 6;

	if ((len < suffixlen + 6) || strncmp(tail, "XXXXXX", 6))
		return -EINVAL;

	if ((fd = open("/dev/urandom", O_RDONLY)) < 0)
		return fd;

	for (i = 0; i < 6; i++) {
		read(fd, &rand, sizeof(rand));
		tail[i] = pfcs[rand % sizeof(pfcs)];
	}

	close(fd);

	if (lookup(template, &oid, NULL) == EOK)
		return -EEXIST;

	return EOK;
}


int mkostemps(char *template, int suffixlen, int flags)
{
	int fd;

	if (mktemp(template, suffixlen) < 0)
		return -1;

	if ((fd = open(template, (flags & ~0x03) | O_CREAT | O_RDWR | O_EXCL, DEFFILEMODE)) < 0)
		return -1;

	return fd;
}


int mkstemps(char *template, int suffixlen)
{
	return mkostemps(template, suffixlen, 0);
}


int mkostemp(char *template, int flags)
{
	return mkostemps(template, 0, flags);
}


int mkstemp(char *template)
{
	return mkstemps(template, 0);
}


char *mkdtemp(char *template)
{
	if (mktemp(template, 0) < 0)
		return NULL;

	if (mkdir(template, S_IRUSR | S_IWUSR | S_IXUSR) < 0)
		return NULL;
	
	return template;
}
