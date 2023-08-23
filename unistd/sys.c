/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/types.h>
#include <sys/threads.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <pthread.h>


WRAP_ERRNO_DEF(int, setpgid, (pid_t pid, pid_t pgid), (pid, pgid))
WRAP_ERRNO_DEF(int, setpgrp, (void), ())

WRAP_ERRNO_DEF(pid_t, getpgid, (pid_t pid), (pid))
WRAP_ERRNO_DEF(pid_t, getpgrp, (void), ())

WRAP_ERRNO_DEF(int, setsid, (void), ())


#define PATH_DELIM ':'


static struct {
	/* execve variables */
	char *execBuff;
	char *filePathBuff;
	char *canonicalPath;
	char **sbArgs;
} sys_common;


static void sys_clear(void)
{
	free(sys_common.execBuff);
	sys_common.execBuff = NULL;

	free(sys_common.filePathBuff);
	sys_common.filePathBuff = NULL;

	free(sys_common.canonicalPath);
	sys_common.canonicalPath = NULL;

	free(sys_common.sbArgs);
	sys_common.sbArgs = NULL;
}


pid_t getsid(pid_t pid)
{
	return getpgid(pid);
}


static int shebang(const char *path)
{
	int fd;
	char sb[2] = { 0, 0 };

	if ((fd = open(path, O_RDONLY)) < 0)
		return -1;

	if ((read(fd, sb, 2) < 0) || sb[0] != '#' || sb[1] != '!') {
		close(fd);
		return -1;
	}

	return fd;
}


int execv(const char *path, char *const argv[])
{
	return execve(path, argv, environ);
}


int execve(const char *file, char *const argv[], char *const envp[])
{
	int fd, noargs = 0, err;
	char *interp, *end;
	char *path = getenv("PATH");
	int fileNameLen = strlen(file);
	struct stat buf;

	fflush(NULL);
	sys_clear();

	sys_common.execBuff = calloc(PATH_MAX, sizeof(char));
	sys_common.filePathBuff = calloc(PATH_MAX, sizeof(char));
	if (sys_common.execBuff == NULL || sys_common.filePathBuff == NULL) {
		sys_clear();
		return SET_ERRNO(-ENOMEM);
	}

	interp = sys_common.execBuff;

	if (!strchr(file, '/') && path) {
		while (*path) {
			char* delim = strchrnul(path, ':');

			int path_len = delim - path;
			if (path_len + 1 + fileNameLen + 1 <= PATH_MAX) {
				memcpy(sys_common.filePathBuff, path, path_len);
				sys_common.filePathBuff[path_len] = '/';
				memcpy(sys_common.filePathBuff + path_len + 1, file, fileNameLen);
				sys_common.filePathBuff[path_len + 1 + fileNameLen] = '\0';

				/* check if file exists with symlink resolving */
				sys_common.canonicalPath = resolve_path(sys_common.filePathBuff, NULL, 1, 0);
				if (sys_common.canonicalPath != NULL) {
					free(sys_common.canonicalPath);
					sys_common.canonicalPath = NULL;
					file = sys_common.filePathBuff;
					break;
				}
			}

			path = delim;
			if (*path == PATH_DELIM) {
				path++;
			}
		}
	}

	if ((fd = shebang(file)) >= 0) {
		if (read(fd, sys_common.execBuff, PATH_MAX) < 0) {
			close(fd);
			sys_clear();
			return SET_ERRNO(-EIO);
		}

		sys_common.execBuff[PATH_MAX - 1] = 0;

		while (*interp == ' ') {
			++interp;
		}

		end = interp;
		while (*end && !isspace(*end)) {
			++end;
		}
		*end = 0;
		/* TODO: support shebang params */

		while (argv[noargs++] != NULL)
			;

		sys_common.sbArgs = calloc(noargs + 1, sizeof(char *));
		if (sys_common.sbArgs == NULL) {
			close(fd);
			sys_clear();
			return SET_ERRNO(-ENOMEM);
		}

		while (noargs-- > 0) {
			sys_common.sbArgs[noargs + 1] = argv[noargs];
		}

		sys_common.sbArgs[0] = interp;
		sys_common.sbArgs[1] = (char *)file; /* replace first argument in case it was found in path */

		close(fd);
		file = interp;
		argv = sys_common.sbArgs;
	}

	sys_common.canonicalPath = resolve_path(file, NULL, 1, 0);
	if (sys_common.canonicalPath == NULL) {
		sys_clear();
		return -1; /* errno set by resolve_path */
	}

	/* execute only if it is regular file */
	err = stat(sys_common.canonicalPath, &buf);
	if (!err) {
		/* TODO: check execution bit presence when native chmod is available */
		err = (S_ISREG(buf.st_mode)) ? exec(sys_common.canonicalPath, argv, envp) : -EACCES;
	}

	sys_clear();

	return SET_ERRNO(err);
}


int execvp(const char *file, char *const argv[])
{
	return execvpe(file, argv, environ);
}


int execvpe(const char *file, char *const argv[], char *const envp[])
{
	return execve(file, argv, envp);
}


static char **argv_gather(const char *arg, va_list args)
{
	int argc = 0, max = 8;
	char **argv, **res;

	if ((argv = malloc(max * sizeof(char *))) == NULL)
		return NULL;

	argv[argc++] = (char *)arg;

	while ((argv[argc++] = va_arg(args, char *)) != NULL) {
		if (argc == max) {
			if ((res = realloc(argv, max *= 2)) == NULL) {
				free(argv);
				return NULL;
			}
			argv = res;
		}
	}

	return argv;
}


int execl(const char *path, const char *arg, ...)
{
	va_list args;
	char **argv;
	int err;

	va_start(args, arg);
	argv = argv_gather(arg, args);
	va_end(args);

	if (argv == NULL)
		return -ENOMEM;

	err = execve(path, argv, environ);
	free(argv);
	return err;
}


int execlp(const char *file, const char *arg, ...)
{
	va_list args;
	char **argv;
	int err;

	va_start(args, arg);
	argv = argv_gather(arg, args);
	va_end(args);

	if (argv == NULL)
		return -ENOMEM;

	err = execve(file, argv, environ);
	free(argv);
	return err;
}


int execle(const char *path, const char *arg, ...)
{
	va_list args;
	char **argv, **envp;
	int err;

	va_start(args, arg);
	argv = argv_gather(arg, args);
	envp = va_arg(args, char **);
	va_end(args);

	if (argv == NULL)
		return -ENOMEM;

	err = execve(path, argv, envp);
	free(argv);
	return err;
}


extern int nsleep(time_t *sec, long *nsec);


int usleep(useconds_t usecs)
{
	int err;
	time_t sec = usecs / (1000 * 1000);
	long nsec = (usecs % (1000 * 1000)) * 1000;

	err = nsleep(&sec, &nsec);

	SET_ERRNO(err);

	return err; /* FIXME - should return 0 or -1. Fix would be a potentially breaking change */
}


unsigned sleep(unsigned seconds)
{
	int err;
	time_t sec = seconds;
	long nsec = 0;
	unsigned unslept;

	err = nsleep(&sec, &nsec);
	unslept = (err == -EINTR) ? (unsigned)sec : 0;

	return unslept;
}


extern pid_t sys_fork(void);
extern void release(void);


pid_t fork(void)
{
	pid_t pid;
	_pthread_atfork_prepare();
	if (!(pid = sys_fork())) {
		release();
		_pthread_atfork_child();
	}
	else if (pid < 0) {
		return SET_ERRNO(pid);
	}
	else {
		_pthread_atfork_parent();
	}
	return pid;
}
