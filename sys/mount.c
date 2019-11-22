/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/mount
 *
 * Copyright 2018 Phoenix Systems
 * Author: Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/msg.h>


int mount(const char *source, const char *target, const char *fstype, long mode, char *data)
{
	int fd, err;

	fd = fsMount(fstype, AT_FDCWD, source, 0);

	if (fd < 0) {
		return SET_ERRNO(fd);
	}

	err = fsBind(AT_FDCWD, target, fd, NULL);
	close(fd);
	return SET_ERRNO(err);
}
