/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stat
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski, Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/file.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


int fstat(int fildes, struct stat *buf)
{
	oid_t oid;
	msg_t msg = { 0 };

	if (fileGet(fildes, 1, &oid, NULL) != EOK)
	   return -EBADF;

	buf->st_ino = oid.id;

	msg.type = mtGetAttr;
	msg.i.attr.oid = oid;
	msg.i.attr.val = 0;

	msg.i.attr.type = 1; /* Uid */
	msgSend(oid.port, &msg);
	buf->st_uid = msg.o.attr.val;

	msg.i.attr.type = 2; /* Gid */
	msgSend(oid.port, &msg);
	buf->st_gid = msg.o.attr.val;

	msg.i.attr.type = 3; /* Size */
	msgSend(oid.port, &msg);
	buf->st_size = msg.o.attr.val;

	msg.i.attr.type = 4; /* Type */
	msgSend(oid.port, &msg);

	switch (msg.o.attr.val) {
	case 0:
		buf->st_mode = S_IFDIR;
		break;

	case 1:
		buf->st_mode = S_IFREG;
		break;

	case 2:
		buf->st_mode = S_IFCHR;
		break;
	}

	msg.i.attr.type = 5; /* Port */
	msgSend(oid.port, &msg);
	buf->st_dev = msg.o.attr.val;

	return EOK;
}


int stat(const char *path, struct stat *buf)
{
	oid_t oid;
	msg_t msg = {0};
	char *canonical_name = canonicalize_file_name(path);

	if (lookup(canonical_name, &oid) < 0) {
		free(canonical_name);
		return -ENOENT;
	}

	free(canonical_name);

	buf->st_ino = oid.id;

	msg.type = mtGetAttr;
	msg.i.attr.oid = oid;
	msg.i.attr.val = 0;

#if 0
	msg.i.attr.type = 0; /* Mode */
	msgSend(oid.port, &msg);
	buf->st_mode = msg.o.attr.val;
#endif

	msg.i.attr.type = 1; /* Uid */
	msgSend(oid.port, &msg);
	buf->st_uid = msg.o.attr.val;

	msg.i.attr.type = 2; /* Gid */
	msgSend(oid.port, &msg);
	buf->st_gid = msg.o.attr.val;

	msg.i.attr.type = 3; /* Size */
	msgSend(oid.port, &msg);
	buf->st_size = msg.o.attr.val;

	msg.i.attr.type = 4; /* Type */
	msgSend(oid.port, &msg);

	switch (msg.o.attr.val) {
	case 0:
		buf->st_mode = S_IFDIR;
		break;

	case 1:
		buf->st_mode = S_IFREG;
		break;

	case 2:
		buf->st_mode = S_IFCHR;
		break;
	}

	msg.i.attr.type = 5; /* Port */
	msgSend(oid.port, &msg);
	buf->st_dev = msg.o.attr.val;

	return EOK;
}


mode_t umask(mode_t cmask)
{
	return -ENOSYS;
}


int lstat(const char *path, struct stat *buf)
{
	return stat(path, buf);
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


int chmod(const char *path, mode_t mode)
{
	return 0;
}


int lchown(const char *path, uid_t owner, gid_t group)
{
	return 0;
}


int mknod(const char *path, mode_t mode, dev_t dev)
{
	return 0;
}


int rename(const char *old, const char *new)
{
	return 0;
}


int chown(const char *path, uid_t owner, gid_t group)
{
	return 0;
}
