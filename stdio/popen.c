/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * popen.c
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>


typedef struct {
	FILE file; /* Must be the first member */
	pid_t pid;
} popen_FILE;


FILE *popen(const char *command, const char *mode)
{
	int fd[2], pid;
	popen_FILE *pf;

	if (pipe(fd) < 0)
		return NULL;

	if ((pid = vfork()) < 0) {
		close(fd[0]);
		close(fd[1]);
		return NULL;
	}
	else if (!pid) {
		if (mode[0] == 'r')
			dup2(fd[1], 1);
		else
			dup2(fd[0], 0);

		close(fd[1]);
		close(fd[0]);

		execl("/bin/sh", "sh", "-c", command, NULL);
		exit(EXIT_FAILURE);
	}

	pf = malloc(sizeof(popen_FILE));

	pf->file.buff = NULL;
	pf->file.buffsz = 0;

	if (mode[0] == 'r') {
		pf->file.fd = fd[0];
		close(fd[1]);
	}
	else {
		pf->file.fd = fd[1];
		close(fd[0]);
	}

	return &pf->file;
}


int pclose(FILE *file)
{
	popen_FILE *pf = (popen_FILE *) file;
	int stat;
	pid_t pid = pf->pid;

	fclose(file);

	while (waitpid(pid, &stat, 0) == -1) {
		if (errno != EINTR) {
			stat = -1;
			break;
		}
	}

	return stat;
}
