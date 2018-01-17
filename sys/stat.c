/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stat
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/stat.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdlib.h>


int fstat(int fildes, struct stat *buf)
{
	return -ENOSYS;
}


int stat(const char *path, struct stat *buf)
{
	return -ENOSYS;
}


mode_t umask(mode_t cmask)
{
	return -ENOSYS;
}


int lstat(const char *path, struct stat *buf)
{
	return -ENOSYS;
}


int creat(const char *pathname, int mode)
{
	return -ENOSYS;
}


int mkdir(const char *path, mode_t mode)
{
	msg_t msg = { 0 };
	oid_t oid, dir;
	char *canonical_name, *name, *parent;

	if (path == NULL)
		return -1;

	canonical_name = canonicalize_file_name(path);
	name = strrchr(canonical_name, '/');

	if (name == canonical_name) {
		name++;
		parent = "/";
	}
	else {
		*(name++) = 0;
		parent = canonical_name;
	}

	if (lookup(parent, &dir) < EOK) {
		free(canonical_name);
		return -1;
	}

	msg.type = mtCreate;
	msg.i.create.type = 0; /* otDir */
	msg.i.create.mode = 0;
	msg.i.create.port = dir.port;

	if (msgSend(dir.port, &msg) < 0) {
		free(canonical_name);
		return -1;
	}

	oid = msg.o.create.oid;

	msg.type = mtLink;
	msg.i.ln.dir = dir;
	msg.i.ln.oid = oid;

	msg.i.size = strlen(name) + 1;
	msg.i.data = name;

	if (msgSend(oid.port, &msg) < 0 || msg.o.io.err < 0) {
		free(canonical_name);
		return -1;
	}

	free(canonical_name);
	return 0;
}
