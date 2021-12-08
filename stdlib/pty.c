/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * pty.c
 *
 * Copyright 2018, 2020 Phoenix Systems
 * Author: Kamil Amanowicz, Lukasz Kosinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/file.h>


char *ptsname(int fd)
{
	static char namebuf[1024];

	if (ptsname_r(fd, namebuf, sizeof(namebuf)))
		return NULL;

	return namebuf;
}
