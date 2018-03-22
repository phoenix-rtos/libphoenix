/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * POSIX implementation - utilities
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <string.h>

void splitname(char *path, char **base, char **dir)
{
	char *slash;

	slash = strrchr(path, '/');

	if (slash == NULL) {
		*dir = ".";
		*base = path;
	}
	else if (slash == path) {
		*base = path + 1;
		*dir = "/";
	}
	else {
		*dir = path;
		*base = slash + 1;
		*slash = 0;
	}
}
