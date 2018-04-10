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
#include <string.h>
#include <sys/msg.h>
#include <sys/file.h>


int open(const char *path, int oflag, ...)
{
	oid_t oid;
	msg_t msg;
	unsigned int h;

	if (path == NULL)
		return -EINVAL;

	if (lookup((char *)path, &oid) != EOK)
		return -EACCES;

	msg.type = mtOpen;
	memcpy(&msg.i.openclose.oid, &oid, sizeof(oid_t));

	msg.i.openclose.flags = oflag;
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


int access(const char *path, int amode)
{
	return -ENOENT;
}


/* UNTESTED */
int dup(int fildes)
{
	unsigned int h;
	oid_t oid;

	if (fileGet(fildes, 1, &oid, NULL) < 0)
		return -EBADF;

	if (fileAdd(&h, &oid) < 0)
		return -EMFILE;

	return h;
}


/* UNTESTED */
int dup2(int fildes, int fildes2)
{
	oid_t oid, oid2;
	msg_t msg;

	if (fildes == fildes2)
		return fildes2;

	if (fildes2 < 0 || fileGet(fildes, 1, &oid, NULL) < 0 || fileGet(fildes2, 1, &oid2, NULL) < 0)
		return -EBADF;

	msg.type = mtClose;
	memcpy(&msg.i.openclose.oid, &oid2, sizeof(oid_t));

	/* NOTE: what happens when oid2 was not open? */
	if (msgSend(oid.port, &msg) < 0)
		return -EIO;

	fileSet(fildes2, 1, &oid, 0);
	return fildes2;
}


int fcntl(int fildes, int cmd, ...)
{
	return -ENOSYS;
}
