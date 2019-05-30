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
#include <libgen.h>
#include <sys/msg.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "posix/utils.h"


extern int sys_open(const char *filename, int oflag, ...);
extern int sys_mkfifo(const char *filename, mode_t mode);
extern int sys_link(const char *path1, const char *path2);
extern int sys_unlink(const char *path);
extern int sys_pipe(int fildes[2]);
extern int sys_fstat(int fd, struct stat *buf);

WRAP_ERRNO_DEF(int, read, (int fildes, void *buf, size_t nbyte), (fildes, buf, nbyte))
WRAP_ERRNO_DEF(int, write, (int fildes, const void *buf, size_t nbyte), (fildes, buf, nbyte))
WRAP_ERRNO_DEF(int, close, (int fildes), (fildes))
WRAP_ERRNO_DEF(int, ftruncate, (int fildes, off_t length), (fildes, length))
WRAP_ERRNO_DEF(int, lseek, (int fildes, off_t offset, int whence), (fildes, offset, whence))
WRAP_ERRNO_DEF(int, dup, (int fildes), (fildes))
WRAP_ERRNO_DEF(int, dup2, (int fildes, int fildes2), (fildes, fildes2))
// WRAP_ERRNO_DEF(int, pipe, (int fildes[2]), (fildes))
// WRAP_ERRNO_DEF(int, fstat, (int fd, struct stat *buf), (fd, buf))


int pipe(int fildes[2])
{
	int err;
	while ((err = sys_pipe(fildes)) == -EINTR)
		;
	return SET_ERRNO(err);
}


int fstat(int fd, struct stat *buf)
{
	int err;
	while ((err = sys_fstat(fd, buf)) == -EINTR)
		;
	return SET_ERRNO(err);
}


int link(const char *path1, const char *path2)
{
	char *canonical1, *canonical2;
	int err;
	canonical1 = canonicalize_file_name(path1);
	canonical2 = canonicalize_file_name(path2);
	err = sys_link(canonical1, canonical2);
	free(canonical1);
	free(canonical2);

	return SET_ERRNO(err);
}


int unlink(const char *path)
{
	char *canonical;
	int err;
	canonical = canonicalize_file_name(path);
	err = sys_unlink(canonical);
	free(canonical);
	return SET_ERRNO(err);
}


int open(const char *filename, int oflag, ...)
{
	va_list ap;
	mode_t mode = 0;
	int err;
	char *canonical;

	/* FIXME: handle varargs properly */
	va_start(ap, oflag);
	mode = va_arg(ap, mode_t);
	va_end(ap);

	canonical = canonicalize_file_name(filename);

	do
		err = sys_open(canonical, oflag, mode);
	while (err == -EINTR);

	free(canonical);
	return SET_ERRNO(err);
}


int mkfifo(const char *filename, mode_t mode)
{
	int err;
	char *canonical;

	canonical = canonicalize_file_name(filename);
	while ((err = sys_mkfifo(canonical, mode)) == -EINTR)
		;
	free(canonical);
	return SET_ERRNO(err);
}


int symlink(const char *path1, const char *path2)
{
	oid_t dir;
	char *canonical1, *canonical2;
	char *dir_name, *name;
	msg_t msg;
	int len1, len2;
	int ret;

	if (path1 == NULL || path2 == NULL)
		return -EINVAL;

	canonical1 = canonicalize_file_name(path2);
	canonical2 = strdup(canonical1);

	dir_name = dirname(canonical1);

	if ((ret = lookup(dir_name, NULL, &dir)) < 0) {
		free(canonical1);
		free(canonical2);
		return ret;
	}

	free(canonical1);
	name = basename(canonical2);

	memset(&msg, 0, sizeof(msg_t));
	msg.type = mtCreate;

	memcpy(&msg.i.create.dir, &dir, sizeof(oid_t));
	msg.i.create.type = otSymlink;
	msg.i.create.mode = S_IFLNK | DEFFILEMODE;

	len1  = strlen(name);
	len2  = strlen(path1);

	msg.i.size = len1 + len2 + 2;
	msg.i.data = malloc(msg.i.size);
	memset(msg.i.data, 0, msg.i.size);

	memcpy(msg.i.data, name, len1);
	memcpy(msg.i.data + len1 + 1, path1, len2);

	if ((ret = msgSend(dir.port, &msg)) != EOK) {
		free(canonical2);
		free(msg.i.data);
		return ret;
	}

	free(canonical2);
	free(msg.i.data);

	return msg.o.create.err;
}


int access(const char *path, int amode)
{
	oid_t oid, dev;

	if (amode == W_OK)
		return 0;

	char *canonical_name = canonicalize_file_name(path);

	// NOTE: for now checking only if file exists
	if (lookup(canonical_name, &oid, &dev) < 0) {
		free(canonical_name);
		errno = ENOENT;
		return -1;
	}

	free(canonical_name);

	return 0;
}


int create_dev(oid_t *oid, const char *path)
{
	oid_t odir;
	msg_t msg = { 0 };
	char *canonical_path, *dir, *name;
	int err;

	if (path == NULL)
		return -1;

	if ((canonical_path = canonicalize_file_name(path)) == NULL)
		return -1;

	splitname(canonical_path, &name, &dir);

	if (lookup(dir, NULL, &odir) < 0) {
		err = mkdir(dir, 0);
		if (err < 0 && err != -EEXIST) {
			free(canonical_path);
			return -1;
		}

		if (lookup(dir, NULL, &odir) < 0) {
			free(canonical_path);
			return -1;
		}
	}

	msg.type = mtCreate;
	memcpy(&msg.i.create.dir, &odir, sizeof(odir));
	memcpy(&msg.i.create.dev, oid, sizeof(*oid));

	msg.i.create.type = otDev;
	msg.i.create.mode = S_IFCHR | 0666;

	msg.i.data = name;
	msg.i.size = strlen(name) + 1;

	if (msgSend(odir.port, &msg) != EOK) {
		free(canonical_path);
		return -1;
	}

	free(canonical_path);

	if (msg.o.io.err < 0)
		return -1;

	return 0;
}


extern int sys_fcntl(int fd, int cmd, unsigned val);


int fcntl(int fd, int cmd, ...)
{
	va_list ap;
	unsigned val;

	/* FIXME: handle varargs properly */
	va_start(ap, cmd);
	val = va_arg(ap, unsigned);
	va_end(ap);

	return SET_ERRNO(sys_fcntl(fd, cmd, val));
}


extern int sys_ioctl(int fildes, unsigned long request, void *val);


int ioctl(int fildes, unsigned long request, ...)
{
	va_list ap;
	void * val;

	/* FIXME: handle varargs properly */
	va_start(ap, request);
	val = va_arg(ap, void *);
	va_end(ap);

	return SET_ERRNO(sys_ioctl(fildes, request, val));
}


int grantpt(int fd)
{
	return EOK;
}


int unlockpt(int fd)
{
	int lock = 0;
	return ioctl(fd, TIOCSPTLCK, &lock);
}


int ptsname_r(int fd, char *buf, size_t buflen)
{
	int id, len;

	while (ioctl(fd, TIOCGPTN, &id) < 0) {
		if (errno != EINTR)
			return -1;
	}

	len = snprintf(buf, buflen, "/dev/pts/%d", id);

	if (len >= buflen)
		return SET_ERRNO(-ERANGE);

	return 0;
}


int isatty(int fd)
{
/* FIXME remove after moving ioctls from devctls */
#ifdef NOMMU
	if (fd == 1 || fd == 0)
		return 1;

	return 0;
#endif

	struct termios t;
	return tcgetattr(fd, &t) == 0;
}
