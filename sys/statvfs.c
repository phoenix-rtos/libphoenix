/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * statvfs
 *
 * Copyright 2022, 2025 Phoenix Systems
 * Author: Lukasz Kosinski, Hubert Badocha
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <sys/file.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/statvfs.h>


extern int sys_statvfs(const char *path, int fd, struct statvfs *buf);


int statvfs(const char *path, struct statvfs *buf)
{
	char *canonical = resolve_path(path, NULL, 1, 0);
	if (canonical == NULL) {
		/* errno set by resolve_path() */
		return -1;
	}
	int res = sys_statvfs(canonical, -1, buf);

	free(canonical);

	return set_errno(res);
}


int fstatvfs(int fildes, struct statvfs *buf)
{
	int res = sys_statvfs(NULL, fildes, buf);

	return set_errno(res);
}
