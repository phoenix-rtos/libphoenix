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
#include <posix/utils.h>


int open(const char *path, int oflag, ...)
{
	oid_t oid;
	unsigned int h;
	msg_t msg = { 0 };
	char *canonical_name, *name, *parent;

	if (path == NULL)
		return -1;

	canonical_name = canonicalize_file_name(path);

	if (lookup(canonical_name, &oid) == EOK) {
		msg.type = mtOpen;
		memcpy(&msg.i.openclose.oid, &oid, sizeof(oid));

		if (msgSend(oid.port, &msg) != EOK) {
			free(canonical_name);
			return -1;
		}

		if (msg.o.io.err < 0) {
			free(canonical_name);
			return -1;
		}
	}
	else if (oflag & O_CREAT) {
		msg.type = mtCreate;
		msg.i.create.type = otFile;

		splitname(canonical_name, &name, &parent);

		if (lookup(parent, &oid) < EOK) {
			free(canonical_name);
			return -1;
		}

		memcpy(&msg.i.create.dir, &oid, sizeof(oid_t));
		msg.i.data = name;
		msg.i.size = strlen(name) + 1;

		if (msgSend(oid.port, &msg) != EOK) {
			free(canonical_name);
			return -1;
		}

		msg.type = mtOpen;
		memcpy(&oid, &msg.o.create.oid, sizeof(oid));
		memcpy(&msg.i.openclose.oid, &oid, sizeof(oid));

		if (msgSend(oid.port, &msg) != EOK) {
			free(canonical_name);
			return -1;
		}

		if (msg.o.io.err < 0) {
			free(canonical_name);
			return -1;
		}
	}
	else {
		free(canonical_name);
		return -1;
	}

	free(canonical_name);

	if (fileAdd(&h, &oid) < 0)
		return -1;

	if (oflag & O_TRUNC) {
		msg.type = mtTruncate;
		memcpy(&msg.i.io.oid, &oid, sizeof(oid));
		msg.i.io.len = 0;
		msg.i.data = NULL;
		msg.i.size = 0;

		if (msgSend(oid.port, &msg) < 0)
			return -1;
	}

	if (oflag & O_APPEND) {
		msg.type = mtGetAttr;
		msg.i.attr.type = atSize;
		msg.i.data = NULL;
		msg.i.size = 0;

		if (msgSend(oid.port, &msg) < 0)
			return -1;

		if (fileSet(h, 2, NULL, msg.o.attr.val) < 0)
			return -1;
	}

	return h;
}


int access(const char *path, int amode)
{
	return 0;
}


/* UNTESTED */
int dup(int fildes)
{
	unsigned int h;
	oid_t oid = {0};

	if (fileGet(fildes, 1, &oid, NULL) < 0)
		return -EBADF;

	if (fileAdd(&h, &oid) < 0)
		return -EMFILE;

	return h;
}


int dup2(int fildes, int fildes2)
{
	oid_t oid, oid2;
	msg_t msg;
	offs_t offs;

	if (fildes == fildes2)
		return fildes2;

	if (fildes2 < 0 || fileGet(fildes, 3, &oid, &offs) < 0)
		return -1; // EBADF

	if (fileGet(fildes2, 1, &oid2, NULL) < 0) {
		fileAdd((unsigned *)&fildes2, &oid2);
	}
	else {
		msg.type = mtClose;
		memcpy(&msg.i.openclose.oid, &oid2, sizeof(oid_t));

		msg.i.data = NULL;
		msg.i.size = 0;
		msg.o.data = NULL;
		msg.o.size = 0;

		/* NOTE: what happens when oid2 was not open? */
		if (msgSend(oid2.port, &msg) < 0)
			return -1; // EIO
	}

	msg.type = mtOpen;
	memcpy(&msg.i.openclose.oid, &oid, sizeof(oid_t));

	msg.i.data = NULL;
	msg.i.size = 0;
	msg.o.data = NULL;
	msg.o.size = 0;

	if (msgSend(oid.port, &msg) < 0)
		return -1; // EIO


	fileSet(fildes2, 3, &oid, offs);
	return fildes2;
}


int fcntl(int fildes, int cmd, ...)
{
	unsigned min;
	va_list args;
	offs_t offs;
	oid_t oid;

	if (fileGet(fildes, 3, &oid, &offs) < 0)
		return -1;

	va_start(args, cmd);

	switch (cmd) {
	case F_DUPFD:
		min = va_arg(args, int);
		if (fileAdd(&min, &oid) < 0)
			return -1;

		if (fileSet(min, 2, NULL, offs) < 0)
			return -1;

		return min;

	case F_GETFD:
		return 0;

	case F_SETFD:
		return 0;

	case F_GETFL:
	case F_SETFL:
	case F_GETLK:
	case F_SETLK:
	case F_SETLKW:
	case F_GETOWN:
	case F_SETOWN:
	default:
		printf("fcntl: cmd %d not implemented\n", cmd);
		break;
	}

	va_end(args);
	return -1;
}
