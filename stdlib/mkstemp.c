/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * mkstemp - make a unique filename
 *
 * Copyright 2018 Phoenix Systems
 * Author: Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <unistd.h>

/* portable filename character set */
static char pfcs[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789._-";

#define MKSTEMP_TSZ 6

int mkstemp(char *template)
{
	int i, fd;
	char *tail;
	int templen;
	unsigned rand;

	if (template == NULL)
		return -1;

	templen = strlen(template);

	if (templen < MKSTEMP_TSZ)
		return -1;

	tail = template + templen - MKSTEMP_TSZ;

	if (strcmp(tail, "XXXXXX"))
		return -1;

	fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0)
		return -1;

	for (i = 0; i < MKSTEMP_TSZ; i++) {
		read(fd, &rand, sizeof(rand));
		tail[i] = pfcs[rand % sizeof(pfcs)];
	}
	close(fd);

	if (access(template, 0) == EOK)
		return -1;

	fd = open(template, O_CREAT | O_RDWR, DEFFILEMODE);
	return fd < 0 ? -1 : fd;
}
