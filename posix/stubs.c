/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * POSIX implementation - stubs
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <grp.h>
#include <stdio.h>


static struct {
	char buff[6];
} common_setlocale = { .buff = "POSIX" };


char* setlocale(int category, const char* locale)
{
	if (category != LC_ALL &&
		category != LC_COLLATE &&
		category != LC_CTYPE &&
		category != LC_MONETARY &&
		category != LC_NUMERIC &&
		category != LC_TIME) {
		return NULL;
	}

	if (locale == NULL) {
		return common_setlocale.buff;
	}

	if (strcmp(locale, "POSIX") == 0 || strcmp(locale, "") == 0) {
		strcpy(common_setlocale.buff, "POSIX");
		return common_setlocale.buff;
	}
	else if (strcmp(locale, "C") == 0) {
		strcpy(common_setlocale.buff, "C");
		return common_setlocale.buff;
	}

	return NULL;
}


int fchmod(int fd, mode_t mode)
{
	return 0;
}


int fchown(int fd, uid_t owner, gid_t group)
{
	return 0;
}


int setgid(gid_t gid)
{
	return 0;
}


int setuid(uid_t uid)
{
	return 0;
}


int setegid(gid_t gid)
{
	return 0;
}


int seteuid(uid_t uid)
{
	return 0;
}


int fchdir(int fildes)
{
	return 0;
}


char *ttyname(int fildes)
{
	return NULL;
}


int ttyname_r(int fildes, char *name, size_t namesize)
{
	if (namesize > 0) {
		name[0] = '\0';
	}

	return 0;
}


int chroot(const char *path)
{
	return 0;
}


int getrlimit(int resource, struct rlimit *rlp)
{
	return 0;
}


int setrlimit(int resource, const struct rlimit *rlp)
{
	return 0;
}


struct group *getgrgid(gid_t gid)
{
	return NULL;
}


dev_t makedev(unsigned int maj, unsigned int min)
{
	return 0;
}


int getrusage(int who, struct rusage *usage)
{
	return 0;
}


void sync(void)
{
}


unsigned int major(int dev)
{
	return 0;
}


unsigned int minor(int dev)
{
	return 0;
}


struct group *getgrnam(const char *name)
{
	return NULL;
}


int initgroups(const char *user, gid_t group)
{
	return 0;
}


int flock(int fd, int operation)
{
	return 0;
}


long ulimit(int __cmd, ...)
{
	return 0;
}

char *gets(char *str)
{
	return NULL;
}

char *tmpnam(char *str)
{
	return NULL;
}

int wctomb(char *str, wchar_t wchar)
{
	return 0;
}
