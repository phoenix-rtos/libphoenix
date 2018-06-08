/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Password related functions
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <pwd.h>


struct passwd *getpwnam(const char *name)
{
	return NULL;
}


struct passwd *getpwuid(uid_t uid)
{
	return NULL;
}


int getpwnam_r(const char *name, struct passwd *pwd, char *buffer,
	       size_t bufsize, struct passwd **result)
{
	return -1;
}
