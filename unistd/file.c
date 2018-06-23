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

#include "posix/utils.h"
#include "posixsrv/posixsrv.h"


int close(int fildes)
{
	oid_t oid;
	msg_t msg;
	unsigned mode;

	if (fileGet(fildes, 1 | 4, &oid, NULL, &mode) < 0)
		return -EBADF;

	msg.type = mtClose;
	memcpy(&msg.i.openclose.oid, &oid, sizeof(oid_t));
	msg.i.openclose.flags = mode;
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
	unsigned mode;

	if (fileGet(fildes, 7, &oid, &offs, &mode) < 0)
		return -EBADF;

	msg.type = mtRead;
	memcpy(&msg.i.io.oid, &oid, sizeof(oid_t));

	msg.i.io.offs = offs;
	msg.i.io.mode = mode;
	msg.i.data = NULL;
	msg.i.size = 0;

	msg.o.data = buf;
	msg.o.size = nbyte;

	if (msgSend(oid.port, &msg) < 0)
		return -1;

	if (msg.o.io.err > 0) {
		offs += msg.o.io.err;
		fileSet(fildes, 2, NULL, offs, 0);
	}

	return msg.o.io.err;
}


ssize_t write(int fildes, const void *buf, size_t nbyte)
{
	msg_t msg;
	oid_t oid;
	offs_t offs;
	unsigned mode;

	if (fileGet(fildes, 7, &oid, &offs, &mode) < 0)
		return -EBADF;

	msg.type = mtWrite;
	memcpy(&msg.i.io.oid, &oid, sizeof(oid_t));

	msg.i.io.offs = offs;
	msg.i.io.mode = mode;
	msg.i.data = (void *)buf;
	msg.i.size = nbyte;

	msg.o.data = NULL;
	msg.o.size = 0;

	if (msgSend(oid.port, &msg) < 0)
		return -1;

	if (msg.o.io.err > 0) {
		offs += msg.o.io.err;
		fileSet(fildes, 2, NULL, offs, 0);
	}

	return msg.o.io.err;
}


int isatty(int fildes)
{
	return fildes == 0 || fildes == 1;
}


int pipe(int fildes[2])
{
	oid_t posix;
	msg_t msg;

	if (lookup("/posix", &posix) < 0)
		return -1;

	msg.type = mtCreate;
	msg.i.create.type = pxBufferedPipe;
	msg.i.data = NULL;
	msg.i.size = 0;
	msg.o.data = NULL;
	msg.o.size = 0;

	if (msgSend(posix.port, &msg) < 0)
		return -1;

	/* open for reading */
	msg.type = mtOpen;
	memcpy(&msg.i.openclose.oid, &msg.o.create.oid, sizeof(oid_t));

	fileAdd((unsigned *)fildes, &msg.i.openclose.oid, O_RDONLY);
	msg.i.openclose.flags = O_RDONLY;

	if (msgSend(posix.port, &msg) < 0) {
		fileRemove(fildes[0]);
		return -1;
	}

	/* open for writing */
	fileAdd((unsigned *)fildes + 1, &msg.i.openclose.oid, O_WRONLY);
	msg.i.openclose.flags = O_WRONLY;

	if (msgSend(posix.port, &msg) < 0) {
		fileRemove(fildes[1]);
		return -1;
	}

	return 0;
}


int ftruncate(int fildes, off_t length)
{
	oid_t oid;
	msg_t msg = { 0 };

	if (length < 0)
		return -EINVAL;

	if (fileGet(fildes, 1, &oid, NULL, NULL) != EOK)
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
	oid_t dir, oid;
	msg_t msg = { 0 };
	char *canonical_path1, *canonical_path2, *dir1, *name1, *dir2, *name2;

	if (path1 == NULL || path2 == NULL)
		return -1;

	if ((canonical_path1 = canonicalize_file_name(path1)) == NULL)
		return -1;

	if (lookup(canonical_path1, &oid)) {
		free(canonical_path1);
		return -1;
	}

	splitname(canonical_path1, &name1, &dir1);

	if ((canonical_path2 = canonicalize_file_name(path2)) == NULL) {
		free(canonical_path1);
		return -1;
	}

	splitname(canonical_path2, &name2, &dir2);

	if (lookup(dir2, &dir)) {
		free(canonical_path1);
		free(canonical_path2);
		return -1;
	}

	msg.type = mtLink;
	memcpy(&msg.i.ln.dir, &dir, sizeof(dir));
	memcpy(&msg.i.ln.oid, &oid, sizeof(oid));

	msg.i.data = name2;
	msg.i.size = strlen(name2) + 1;

	if (msgSend(dir.port, &msg) != EOK) {
		free(canonical_path1);
		free(canonical_path2);
		return -1;
	}

	free(canonical_path1);
	free(canonical_path2);

	if (msg.o.io.err < 0)
		return -1;

	return 0;
}


int unlink(const char *path)
{
	oid_t dir;
	msg_t msg = { 0 };
	char *canonical_name, *dirname, *name;

	if (path == NULL)
		return -1;

	if ((canonical_name = canonicalize_file_name(path)) == NULL)
		return -1;

	splitname(canonical_name, &name, &dirname);

	if (lookup(dirname, &dir)) {
		free(canonical_name);
		return -1;
	}

	msg.type = mtUnlink;
	memcpy(&msg.i.ln.dir, &dir, sizeof(dir));
	msg.i.data = name;
	msg.i.size = strlen(name) + 1;

	if (msgSend(dir.port, &msg) != EOK) {
		free(canonical_name);
		return -1;
	}

	free(canonical_name);

	if (msg.o.io.err < 0)
		return -1;

	return 0;
}


int symlink(const char *path1, const char *path2)
{
	return -1;
}


off_t lseek(int fildes, off_t offset, int whence)
{
	oid_t oid;
	offs_t offs;
	struct stat buf;

	if (fileGet(fildes, 3, &oid, &offs, NULL) != EOK)
		return -EBADF;

	switch (whence) {

	case SEEK_SET:
		fileSet(fildes, 2, NULL, offset, 0);
		break;

	case SEEK_CUR:
		if (offset < 0 && ~offset + 1 > offs)
			offs = 0;
		else
			offs += offset;

		if (offset > 0 && offset > offs)
			return -75; //EOVERFLOW;

		fileSet(fildes, 2, NULL, offs, 0);
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

		fileSet(fildes, 2, NULL, offs, 0);
		break;

	default:
		return -EINVAL;
	}

	return EOK;
}
