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
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>


char exec_buffer[64];


static int shebang(const char *path)
{
	int fd;
	char sb[2] = {0, 0};

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
	return execve(path, argv, NULL);
}


int execve(const char *path, char *const argv[], char *const envp[])
{
	int fd, noargs = 0, err;
	char *interp = exec_buffer, *end, **sb_args = NULL;

	if ((fd = shebang(path)) >= 0) {
		if (read(fd, exec_buffer, sizeof(exec_buffer)) < 0) {
			close(fd);
			return -EIO;
		}

		exec_buffer[sizeof(exec_buffer) - 1] = 0;

		while (*interp == ' ') ++interp;
		end = interp;
		while (*end && !isspace(*end)) ++end;
		*end = 0;

		while (argv[noargs++] != NULL) ;

		if ((sb_args = calloc(noargs + 1, sizeof(char *))) == NULL) {
			close(fd);
			return -ENOMEM;
		}

		while (noargs-- > 0)
			sb_args[noargs + 1] = argv[noargs];

		sb_args[0] = interp;

		close(fd);
		path = interp;
		argv = sb_args;
	}

	err = exec(path, argv, envp);
	free(sb_args);
	return err;
}


int execvp(const char *file, char *const argv[])
{
	return execve(file, argv, NULL);
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

	err = execve(path, argv, NULL);
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


/* TODO :move */
unsigned int major(int dev)
{
	return 0;
}


unsigned int minor(int dev)
{
	return 0;
}


unsigned sleep(unsigned seconds)
{
	return usleep(seconds * 1000 * 1000);
}
