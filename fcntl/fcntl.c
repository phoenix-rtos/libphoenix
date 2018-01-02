/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * fcntl (POSIX routines for file operations)
 *
 * Copyright 2017-2018 Phoenix Systems
 * Author: Aleksander Kaminski, Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include ARCH

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/file.h>


int open(const char *path, int oflag, ...)
{
	oid_t oid;
	msg_t msg;
	unsigned int h;
	
	if (path == NULL)
		return -EINVAL;

	if (lookup((char *)path, &oid) == EOK)
		return -EACCES;

	msg.type = mtOpen;
	memcpy(&msg.i.open.oid, &oid, sizeof(oid_t));

	msg.i.open.flags = oflag;
	msg.i.data = (char *)path;
	msg.i.size = strlen(path);

	msg.o.data = NULL;
	msg.o.size = 0;

	if (msgSend(oid.port, &msg) < 0)
		return -EIO;

	if (fileAdd(&h, &oid) < 0)
		return -ENFILE;

	return h;
}
