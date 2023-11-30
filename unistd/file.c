/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd (POSIX routines for file operations)
 *
 * Copyright 2017-2023 Phoenix Systems
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
extern int sys_lseek(int fildes, off_t *offset, int whence);

WRAP_ERRNO_DEF(ssize_t, read, (int fildes, void *buf, size_t nbyte), (fildes, buf, nbyte))
WRAP_ERRNO_DEF(ssize_t, write, (int fildes, const void *buf, size_t nbyte), (fildes, buf, nbyte))
WRAP_ERRNO_DEF(int, close, (int fildes), (fildes))
WRAP_ERRNO_DEF(int, ftruncate, (int fildes, off_t length), (fildes, length))
WRAP_ERRNO_DEF(int, dup, (int fildes), (fildes))
WRAP_ERRNO_DEF(int, dup2, (int fildes, int fildes2), (fildes, fildes2))
WRAP_ERRNO_DEF(int, fsync, (int fildes), (fildes))


off_t lseek(int fildes, off_t offset, int whence)
{
	int retval = sys_lseek(fildes, &offset, whence);
	if (retval < 0) {
		return (off_t)SET_ERRNO(retval);
	}

	return offset;
}


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


int link(const char *oldpath, const char *newpath)
{
	char *oldcanonical, *newcanonical;
	int err;

	/* resolve_last_symlink = 0 -> we should link symlink instead of target */
	if ((oldcanonical = resolve_path(oldpath, NULL, 0, 0)) == NULL)
		return -1; /* errno set by resolve_path */

	/* allow_missing_leaf = 1 -> we should be creating new link */
	if ((newcanonical = resolve_path(newpath, NULL, 1, 1)) == NULL) {
		free(oldcanonical);
		return -1; /* errno set by resolve_path */
	}

	err = sys_link(oldcanonical, newcanonical);
	free(oldcanonical);
	free(newcanonical);

	return SET_ERRNO(err);
}


int unlink(const char *path)
{
	char *canonical;
	int err;

	/* resolve_last_symlink = 0 -> we should delete symlink instead of target file */
	canonical = resolve_path(path, NULL, 0, 0);

	if (canonical == NULL)
		return -1; /* errno set by resolve_path */

	err = sys_unlink(canonical);
	free(canonical);
	return SET_ERRNO(err);
}


int open(const char *filename, int oflag, ...)
{
	va_list ap;
	struct stat st;
	mode_t mode = 0;
	int err;
	char *canonical;

	/* FIXME: handle varargs properly */
	va_start(ap, oflag);
	mode = va_arg(ap, mode_t);
	va_end(ap);

	if (oflag & (O_WRONLY | O_RDWR)) {
		if ((err = stat(filename, &st)) < 0) {
			if (errno != ENOENT)
				return err;
		}
		else if (S_ISDIR(st.st_mode)) {
			return SET_ERRNO(-EISDIR);
		}
	}

	/* allow_missing_leaf = 1 -> open() may be creating a file */
	canonical = resolve_path(filename, NULL, 1, 1);
	if (canonical == NULL)
		return -1; /* errno set by resolve_path */

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

	/* allow_missing_leaf = 1 -> we will be creating a file */
	canonical = resolve_path(filename, NULL, 1, 1);
	if (canonical == NULL)
		return -1; /* errno set by resolve_path */

	while ((err = sys_mkfifo(canonical, mode)) == -EINTR)
		;
	free(canonical);
	return SET_ERRNO(err);
}


int symlink(const char *target, const char *linkpath)
{
	oid_t dir;
	char *canonical_linkpath;
	char *dir_name, *name;
	msg_t msg;
	int len1, len2;
	int ret;

	if (target == NULL || linkpath == NULL)
		return -EINVAL;

	/* allow_missing_leaf = 1 -> we will be creating a file */
	canonical_linkpath = resolve_path(linkpath, NULL, 1, 1);
	if (canonical_linkpath == NULL)
		return -1; /* errno set by resolve_path */

	splitname(canonical_linkpath, &name, &dir_name);

	if ((ret = lookup(dir_name, NULL, &dir)) < 0) {
		free(canonical_linkpath);
		return SET_ERRNO(ret);
	}

	memset(&msg, 0, sizeof(msg_t));
	msg.type = mtCreate;

	memcpy(&msg.i.create.dir, &dir, sizeof(oid_t));
	msg.i.create.type = otSymlink;
	/* POSIX: symlink file permissions are undefined, use sane default */
	msg.i.create.mode = S_IFLNK | 0777;

	len1 = strlen(name);
	len2 = strlen(target);

	msg.i.size = len1 + len2 + 2;
	msg.i.data = malloc(msg.i.size);
	memset(msg.i.data, 0, msg.i.size);

	memcpy(msg.i.data, name, len1);
	memcpy(msg.i.data + len1 + 1, target, len2);

	ret = msgSend(dir.port, &msg);

	free(canonical_linkpath);
	free(msg.i.data);

	return ret != EOK ? SET_ERRNO(-EIO) : SET_ERRNO(msg.o.create.err);
}


int access(const char *path, int amode)
{
	oid_t oid, dev;

	if (amode == W_OK)
		return 0;

	char *canonical_name = resolve_path(path, NULL, 1, 0);
	if (canonical_name == NULL) {
		return -1; /* errno set by resolve_path */
	}

	// NOTE: for now checking only if file exists
	if (lookup(canonical_name, &oid, &dev) < 0) {
		free(canonical_name);
		errno = ENOENT;
		return -1;
	}

	free(canonical_name);

	return 0;
}


int truncate(const char *path, off_t length)
{
	char *canonical_name;
	msg_t msg = { 0 };
	oid_t oid;

	if (length < 0) {
		return SET_ERRNO(-EINVAL);
	}

	canonical_name = resolve_path(path, NULL, 1, 0);
	if (canonical_name == NULL) {
		return -1; /* errno set by resolve_path */
	}

	if (lookup(canonical_name, &oid, NULL) < 0) {
		free(canonical_name);
		return SET_ERRNO(-ENOENT);
	}
	free(canonical_name);

	msg.type = mtTruncate;
	msg.i.io.oid = oid;
	msg.i.io.len = length;
	if (msgSend(oid.port, &msg) < 0) {
		return SET_ERRNO(-EIO);
	}

	return SET_ERRNO(msg.o.io.err);
}


int create_dev(oid_t *oid, const char *path)
{
	oid_t odev, odir;
	msg_t msg = { 0 };
	int retry = 0, err;
	char *canonical_path, *dir, *sep, *name, *tpathalloc = NULL;
	const char *tpath = path;
	static int nodevfs = 0;

	if (path == NULL || oid == NULL)
		return -1;

	while (lookup("devfs", NULL, &odev) < 0) {
		/* if create_dev() is called by anyone started from syspage devfs
		 * may be not registered yet so we try 3 times until we give up */
		if (++retry > 3 || nodevfs) {
			nodevfs = 1;
			/* fallback */
			mkdir("/dev", 0777);

			if (lookup("/dev", NULL, &odir) < 0) {
				/* Looks like we don't have a filesystem.
				 * Fall back to portRegister. */
				if (*path != '/') {
					/* Move point to /dev */
					if ((tpathalloc = malloc(strlen(path) + 6)) == NULL)
						return -ENOMEM;
					strcpy(tpathalloc, "/dev/");
					strcpy(tpathalloc + 5, path);
					tpath = tpathalloc;
				}

				/* TODO: not needed, we may assume it will always be canonical? */
#if 0
				if ((canonical_path = canonicalize_file_name(tpath)) == NULL) {
					free(tpathalloc);
					return -1;
				}
#else
				canonical_path = strdup(tpath);
#endif

				err = portRegister(oid->port, canonical_path, oid);
				free(canonical_path);
				free(tpathalloc);
				return err;
			}
			odev = odir;
			break;
		}
		else {
			usleep(100000);
		}
	}

	/* if someone uses full path to create device cut /dev prefix */
	if (!strncmp("/dev", path, 4))
		path += 4;

#if 0
	/* TODO: invalid usage of canonicalize_file_name() - non-existing file
	 * do we need normalize_path() here or can we assume developers are sane? */

	if ((canonical_path = canonicalize_file_name(path)) == NULL)
		return -1;
#else
	canonical_path = strdup(path);
#endif

	splitname(canonical_path, &name, &dir);

	if (name == canonical_path)
		dir = NULL;

	while (dir != NULL && *dir != 0) {
		while (*dir == '/')
			dir++;

		if (*dir == 0)
			break;

		sep = strchr(dir, '/');
		if (sep != NULL)
			*sep++ = 0;

		msg.type = mtCreate;
		msg.i.create.type = otDir;
		msg.i.create.mode = DEFFILEMODE | S_IFDIR;
		msg.i.create.dir = odev;
		msg.i.data = dir;
		msg.i.size = strlen(dir) + 1;

		if (msgSend(odev.port, &msg) < 0) {
			free(canonical_path);
			return -ENOMEM;
		}

		if (!msg.o.create.err) {
			odev = msg.o.create.oid;
		}
		else if (msg.o.create.err == -EEXIST) {
			msg.type = mtLookup;
			msg.i.size = strlen(dir) + 1;
			msg.i.data = dir;
			msg.i.lookup.dir = odev;

			if (msgSend(odev.port, &msg) < 0) {
				free(canonical_path);
				return -ENOMEM;
			}

			if (msg.o.lookup.err >= 0) {
				odev = msg.o.lookup.dev;
			}
			else {
				free(canonical_path);
				return msg.o.lookup.err;
			}
		} else {
			free(canonical_path);
			return msg.o.create.err;
		}
		dir = sep;
	}

	msg.type = mtCreate;
	memcpy(&msg.i.create.dir, &odev, sizeof(oid_t));
	memcpy(&msg.i.create.dev, oid, sizeof(*oid));

	msg.i.create.type = otDev;
	/* TODO: create_dev() should take mode argument */
	msg.i.create.mode = S_IFCHR | 0666;

	msg.i.data = name;
	msg.i.size = strlen(name) + 1;

	if (msgSend(odev.port, &msg) != EOK) {
		free(canonical_path);
		return -ENOMEM;
	}

	free(canonical_path);

	return msg.o.create.err;
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
	struct termios t;
	return tcgetattr(fd, &t) == 0;
}
