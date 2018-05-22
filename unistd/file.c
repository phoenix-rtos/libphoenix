/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd (POSIX routines for file operations)
 *
 * Copyright 2017-2018 Phoenix Systems
 * Author: Aleksander Kaminski, Pawel Pisarczyk, Kamil Amanowicz
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
#include <unistd.h>
#include <sys/msg.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>

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

	msg.i.io.offs = offs;
	msg.i.data = NULL;
	msg.i.size = 0;

	msg.o.data = buf;
	msg.o.size = nbyte;

	if (msgSend(oid.port, &msg) < 0)
		return -1;

	if (msg.o.io.err > 0) {
		offs += msg.o.io.err;
		fileSet(fildes, 2, NULL, offs);
	}

	return msg.o.io.err;
}


ssize_t write(int fildes, const void *buf, size_t nbyte)
{
	msg_t msg;
	oid_t oid;
	offs_t offs;

	if (fileGet(fildes, 3, &oid, &offs) < 0)
		return -EBADF;

	msg.type = mtWrite;
	memcpy(&msg.i.io.oid, &oid, sizeof(oid_t));

	msg.i.io.offs = offs;
	msg.i.data = buf;
	msg.i.size = nbyte;

	msg.o.data = NULL;
	msg.o.size = 0;

	if (msgSend(oid.port, &msg) < 0)
		return -1;

	if (msg.o.io.err > 0) {
		offs += msg.o.io.err;
		fileSet(fildes, 2, NULL, offs);
	}

	return msg.o.io.err;
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
	oid_t oid;
	msg_t msg = { 0 };

	if (length < 0)
		return -EINVAL;

	if (fileGet(fildes, 1, &oid, NULL) != EOK)
		return -EBADF;

	msg.type = mtTruncate;
	memcpy(&msg.i.io.oid, &oid, sizeof(oid_t));

	msg.i.io.len = length;

	if (msgSend(oid.port, &msg) < 0)
		return -1;

	if (msg.o.io.err != EOK)
		return -EIO;

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

off_t lseek(int fildes, off_t offset, int whence)
{
	oid_t oid;
	offs_t offs;
	struct stat buf;

	if (fileGet(fildes, 3, &oid, &offs) != EOK)
		return -EBADF;

	switch (whence) {

	case SEEK_SET:
		fileSet(fildes, 2, NULL, offset);
		break;

	case SEEK_CUR:
		if (offset < 0 && ~offset + 1 > offs)
			offs = 0;
		else
			offs += offset;

		if (offset > 0 && offset > offs)
			return -75; //EOVERFLOW;

		fileSet(fildes, 2, NULL, offs);
		break;

	case SEEK_END:
		if (fstat(fildes, &buf) != EOK)
			return -EBADF;

		if (offset < 0 && ~offset + 1 > buf.st_size)
			offs = 0;
		else
			offs = buf.st_size + offset;

		if (offset > 0 && offs < buf.st_size)
			return -75; //EOVERFLOW;

		fileSet(fildes, 2, NULL, offs);
		break;

	default:
		return -EINVAL;
	}

	return EOK;
}
