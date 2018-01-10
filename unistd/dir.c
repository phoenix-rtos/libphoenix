/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd (POSIX routines for directory operations)
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>

int chdir(const char *path)
{
	return -ENOENT;
}


char *getcwd(char *buf, size_t size)
{
	return NULL;
}


struct dirent *readdir(DIR *dirp)
{
	return NULL;
}


DIR *opendir(const char *dirname)
{
	return NULL;
}


int closedir(DIR *dirp)
{
	return 0;
}
