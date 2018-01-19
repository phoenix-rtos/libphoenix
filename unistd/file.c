/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd (POSIX routines for file operations)
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
#include <poll.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/file.h>
#include <sys/types.h>


int close(int fildes)
{
	oid_t oid;
	msg_t msg;

	if (fileGet(fildes, 1, &oid, NULL) < 0)
		return -EBADF;

	msg.type = mtClose;
	memcpy(&msg.i.openclose.oid, &oid, sizeof(oid_t));

	msg.i.data = NULL;
	msg.i.size = 0;

	msg.o.data = NULL;
	msg.o.size = 0;

	if (msgSend(oid.port, &msg) < 0)
		return -EIO;

	if (fileRemove(fildes) < 0)
		return -ENFILE;

	return EOK;
}


ssize_t read(int fildes, void *buf, size_t nbyte)
{
	msg_t msg;
	oid_t oid;
	offs_t offs;

	if (fileGet(fildes, 3, &oid, &offs) < 0)
		return -EBADF;

	msg.type = mtRead;
	memcpy(&msg.i.io.oid, &oid, sizeof(oid_t));

	msg.i.io.offs = 0;
	msg.i.data = NULL;
	msg.i.size = 0;

	msg.o.data = buf;
	msg.o.size = nbyte;

	if (msgSend(oid.port, &msg) < 0)
		return -1;

	return msg.o.io.err;
}


ssize_t write(int fildes, void *buf, size_t nbyte)
{
	msg_t msg;
	oid_t oid;
	offs_t offs;

	if (fileGet(fildes, 3, &oid, &offs) < 0)
		return -EBADF;

	msg.type = mtWrite;
	memcpy(&msg.i.io.oid, &oid, sizeof(oid_t));

	msg.i.io.offs = 0;
	msg.i.data = buf;
	msg.i.size = nbyte;

	msg.o.data = NULL;
	msg.o.size = 0;

	if (msgSend(oid.port, &msg) < 0)
		return -1;

	return nbyte;
}


int isatty(int fildes)
{
	return fildes == 0 || fildes == 1;
}


int pipe(int fildes[2])
{
	return -ENOSYS;
}


int poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
	return -ENOSYS;
}


int ftruncate(int fildes, off_t length)
{
	return EOK;
}


int link(const char *path1, const char *path2)
{
	return 0;
}


int unlink(const char *path)
{
	return 0;
}


int symlink(const char *path1, const char *path2)
{
	return 0;
}
