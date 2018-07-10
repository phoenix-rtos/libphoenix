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


extern int open_absolute(const char *filename, int oflag, ...);
extern int mkfifo_absolute(const char *filename, mode_t mode);


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
	err = open_absolute(canonical, oflag, mode);
	free(canonical);
	return err;
}


int mkfifo(const char *filename, mode_t mode)
{
	int err;
	char *canonical;

	canonical = canonicalize_file_name(filename);
	err = mkfifo_absolute(canonical, mode);
	free(canonical);
	return err;
}


int symlink(const char *path1, const char *path2)
{
	return -1;
}


int isatty(int fildes)
{
	return fildes == 0 || fildes == 1;
}


int access(const char *path, int amode)
{
	return 0;
}


int create_dev(oid_t *oid, const char *path)
{
	oid_t odir;
	msg_t msg = { 0 };
	char *canonical_path, *dir, *name;

	if (path == NULL)
		return -1;

	if ((canonical_path = canonicalize_file_name(path)) == NULL)
		return -1;

	if (lookup(canonical_path, &odir) == EOK) {
		free(canonical_path);
		return -1; /* EEXIST */
	}

	splitname(canonical_path, &name, &dir);

	if (lookup(dir, &odir) < 0) {
		free(canonical_path);
		return -1;
	}

	msg.type = mtCreate;
	memcpy(&msg.i.create.dir, &odir, sizeof(odir));
	memcpy(&msg.i.create.dev, oid, sizeof(*oid));

	msg.i.create.type = otDev;
	msg.i.create.mode = 0;

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
