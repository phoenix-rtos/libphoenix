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


extern ssize_t sys_read(int fildes, void *buf, size_t nbyte, off_t offset);
extern ssize_t sys_write(int fildes, const void *buf, size_t nbyte, off_t offset);
extern int sys_open(const char *filename, int oflag, ...);
extern int sys_mkfifo(const char *filename, mode_t mode);
extern int sys_link(const char *path1, const char *path2);
extern int sys_unlink(const char *path);
extern int sys_pipe(int fildes[2]);
extern int sys_fstat(int fd, struct stat *buf);
extern int sys_lseek(int fildes, off_t *offset, int whence);

WRAP_ERRNO_DEF(int, close, (int fildes), (fildes))
WRAP_ERRNO_DEF(int, ftruncate, (int fildes, off_t length), (fildes, length))
WRAP_ERRNO_DEF(int, dup, (int fildes), (fildes))
WRAP_ERRNO_DEF(int, dup2, (int fildes, int fildes2), (fildes, fildes2))
WRAP_ERRNO_DEF(int, fsync, (int fildes), (fildes))


ssize_t read(int fildes, void *buf, size_t nbyte)
{
	return SET_ERRNO(sys_read(fildes, buf, nbyte, -1));
}


ssize_t write(int fildes, const void *buf, size_t nbyte)
{
	return SET_ERRNO(sys_write(fildes, buf, nbyte, -1));
}


ssize_t pread(int fildes, void *buf, size_t nbyte, off_t offset)
{
	if (offset < 0) {
		errno = EINVAL;
		return -1;
	}
	return SET_ERRNO(sys_read(fildes, buf, nbyte, offset));
}


ssize_t pwrite(int fildes, const void *buf, size_t nbyte, off_t offset)
{
	if (offset < 0) {
		errno = EINVAL;
		return -1;
	}
	return SET_ERRNO(sys_write(fildes, buf, nbyte, offset));
}


static ssize_t __safe_write_general(int fd, const void *buf, size_t size, off_t offset)
{
	size_t left = size;
	ssize_t wlen = 0;
	const char *ptr = buf;

	while (left > 0) {
		wlen = SET_ERRNO(sys_write(fd, ptr, left, offset));
		if (wlen > 0) {
			left -= wlen;
			ptr += wlen;
			if (offset >= 0) {
				offset += wlen;
			}
		}
		else if (wlen == 0) {
			/* Undefined behavior: write() returning 0, treat it as an I/O error */
			errno = EIO;
			return -1;
		}
		else if (errno == EINTR || errno == EAGAIN) {
			/* Retry on interrupt or non-blocking case */
			continue;
		}
		else {
			return -1;
		}
	}

	return size;
}


ssize_t __safe_write(int fd, const void *buf, size_t size)
{
	return __safe_write_general(fd, buf, size, -1);
}


ssize_t __safe_pwrite(int fd, const void *buf, size_t size, off_t offset)
{
	if (offset < 0) {
		errno = EINVAL;
		return -1;
	}
	return __safe_write_general(fd, buf, size, offset);
}


static ssize_t __safe_write_nb_general(int fd, const void *buf, size_t size, off_t offset)
{
	ssize_t wlen;

	do {
		wlen = SET_ERRNO(sys_write(fd, buf, size, offset));
	} while (wlen < 0 && errno == EINTR);

	if (wlen == 0) {
		/* Undefined behavior: write() returning 0, treat it as an I/O error */
		errno = EIO;
		return -1;
	}

	return wlen;
}


ssize_t __safe_write_nb(int fd, const void *buf, size_t size)
{
	return __safe_write_nb_general(fd, buf, size, -1);
}


ssize_t __safe_pwrite_nb(int fd, const void *buf, size_t size, off_t offset)
{
	if (offset < 0) {
		errno = EINVAL;
		return -1;
	}
	return __safe_write_nb_general(fd, buf, size, offset);
}


static ssize_t __safe_read_general(int fd, void *buf, size_t size, off_t offset)
{
	ssize_t rlen = 0;
	size_t left = size;
	char *ptr = buf;

	while (left > 0) {
		rlen = SET_ERRNO(sys_read(fd, ptr, left, offset));
		if (rlen > 0) {
			left -= rlen;
			ptr += rlen;
			if (offset >= 0) {
				offset += rlen;
			}
		}
		else if (rlen == 0) {
			/* EOF */
			break;
		}
		else if (errno == EINTR || errno == EAGAIN) {
			/* Retry on interrupt or non-blocking case */
			continue;
		}
		else {
			return -1;
		}
	}

	return size - left;
}


ssize_t __safe_read(int fd, void *buf, size_t size)
{
	return __safe_read_general(fd, buf, size, -1);
}


ssize_t __safe_pread(int fd, void *buf, size_t size, off_t offset)
{
	if (offset < 0) {
		errno = EINVAL;
		return -1;
	}
	return __safe_read_general(fd, buf, size, offset);
}


static ssize_t __safe_read_nb_general(int fd, void *buf, size_t size, off_t offset)
{
	ssize_t rlen;

	do {
		rlen = SET_ERRNO(sys_read(fd, buf, size, offset));
	} while (rlen < 0 && errno == EINTR);

	return rlen;
}


ssize_t __safe_read_nb(int fd, void *buf, size_t size)
{
	return __safe_read_nb_general(fd, buf, size, -1);
}


ssize_t __safe_pread_nb(int fd, void *buf, size_t size, off_t offset)
{
	if (offset < 0) {
		errno = EINVAL;
		return -1;
	}
	return __safe_read_nb_general(fd, buf, size, offset);
}


int __safe_open(const char *path, int oflag, mode_t mode)
{
	int err;
	do {
		err = open(path, oflag, mode);
	} while ((err < 0) && (errno == EINTR));

	return err;
}


int __safe_close(int fd)
{
	int err;
	do {
		err = close(fd);
	} while ((err < 0) && (errno == EINTR));

	return err;
}


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
	if ((target == NULL) || (linkpath == NULL)) {
		return -EINVAL;
	}

	/* allow_missing_leaf = 1 -> we will be creating a file */
	char *canonical_linkpath = resolve_path(linkpath, NULL, 1, 1);
	if (canonical_linkpath == NULL) {
		return -1; /* errno set by resolve_path */
	}

	char *dir_name, *name;
	splitname(canonical_linkpath, &name, &dir_name);

	oid_t dir;
	int ret = lookup(dir_name, NULL, &dir);
	if (ret < 0) {
		free(canonical_linkpath);
		return SET_ERRNO(ret);
	}

	size_t len1 = strlen(name);
	size_t len2 = strlen(target);

	msg_t msg = {
		.type = mtCreate,
		.oid = dir,
		.i.create.type = otSymlink,
		/* POSIX: symlink file permissions are undefined, use sane default */
		.i.create.mode = S_IFLNK | 0777,
		.i.size = len1 + len2 + 2
	};

	void *idata = calloc(msg.i.size, 1);
	if (idata == NULL) {
		free(canonical_linkpath);
		return SET_ERRNO(-ENOMEM);
	}

	memcpy(idata, name, len1);
	memcpy(idata + len1 + 1, target, len2);
	msg.i.data = idata;

	ret = msgSend(dir.port, &msg);

	free(canonical_linkpath);
	free(idata);

	return (ret != EOK) ? SET_ERRNO(-EIO) : SET_ERRNO(msg.o.err);
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
	if (length < 0) {
		return SET_ERRNO(-EINVAL);
	}

	char *canonical_name = resolve_path(path, NULL, 1, 0);
	if (canonical_name == NULL) {
		return -1; /* errno set by resolve_path */
	}

	oid_t oid;
	if (lookup(canonical_name, &oid, NULL) < 0) {
		free(canonical_name);
		return SET_ERRNO(-ENOENT);
	}
	free(canonical_name);

	msg_t msg = {
		.type = mtTruncate,
		.oid = oid,
		.i.io.len = length
	};

	if (msgSend(oid.port, &msg) < 0) {
		return SET_ERRNO(-EIO);
	}

	return SET_ERRNO(msg.o.err);
}


int create_dev(oid_t *oid, const char *path)
{
	oid_t odev, odir;
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

				canonical_path = strdup(tpath);

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

	canonical_path = strdup(path);

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

		msg_t msg = {
			.type = mtCreate,
			.oid = odev,
			.i.create.type = otDir,
			.i.create.mode = DEFFILEMODE | S_IFDIR,
			.i.data = dir,
			.i.size = strlen(dir) + 1
		};

		if (msgSend(odev.port, &msg) < 0) {
			free(canonical_path);
			return -ENOMEM;
		}

		if (msg.o.err == 0) {
			odev = msg.o.create.oid;
		}
		else if (msg.o.err == -EEXIST) {
			msg.type = mtLookup;
			msg.oid = odev;
			msg.i.size = strlen(dir) + 1;
			msg.i.data = dir;

			if (msgSend(odev.port, &msg) < 0) {
				free(canonical_path);
				return -ENOMEM;
			}

			if (msg.o.err >= 0) {
				odev = msg.o.lookup.dev;
			}
			else {
				free(canonical_path);
				return msg.o.err;
			}
		}
		else {
			free(canonical_path);
			return msg.o.err;
		}
		dir = sep;
	}

	msg_t msg = {
		.type = mtCreate,
		.oid = odev,
		.i.create.dev = *oid,
		.i.create.type = otDev,
		/* TODO: create_dev() should take mode argument */
		.i.create.mode = S_IFCHR | 0666,
		.i.data = name,
		.i.size = strlen(name) + 1
	};

	if (msgSend(odev.port, &msg) != EOK) {
		free(canonical_path);
		return -ENOMEM;
	}

	free(canonical_path);

	return msg.o.err;
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
	void *val;

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
