/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * test/psh
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "../../phoenix-rtos-kernel/include/threadinfo.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "dirent.h"
#include "sys/threads.h"
#include "sys/msg.h"
#include "sys/file.h"
#include "sys/stat.h"
#include "sys/wait.h"


static int psh_isNewline(char c)
{
	if (c == '\r' || c == '\n')
		return 1;

	return 0;
}


static int psh_isAcceptable(char c)
{
	if (psh_isNewline(c))
		return 1;

	return (c >= ' ');
}


static char *psh_nextString(char *buff, unsigned int *size)
{
	char *s, *p;

	/* Skip leading spaces. */
	for (s = buff; *s == ' '; ++s);

	/* Count string size. */
	for (p = s, *size = 0; *p && *p != ' '; ++p, ++(*size));

	s[*size] = '\0';

	return s;
}


static int psh_readln(oid_t *oid, char *line, int size)
{
	int count = 0;

#if 0
	if ((count = read(0, line, size - 1)) >= 0)
		memset(&line[count], '\0', size - count);
#else
	char c;

	for (;;) {
		read(0, &c, 1);

		if (c == 0)
			break;

		if (!psh_isAcceptable(c))
			continue;

		/* Check, if line has maximum size. */
		if (count >= size - 1 && c != 0x7f && !psh_isNewline(c))
			continue;

		if (psh_isNewline(c))
			break;

		line[count++] = c;
	}

	memset(&line[count], '\0', size - count);
#endif

	return count;
}


static void psh_help(oid_t *oid)
{
	printf("Available commands:\n");
	printf("  help   - prints this help\n");
	printf("  ls     - lists files in the namespace\n");
	printf("  mkdir  - create directory\n");
	printf("  mem    - prints memory map\n");
	printf("  ps     - prints list of processes and threads\n");
	printf("  exit   - exit from shell\n");
}


static void psh_ls(oid_t *oid, char *args)
{
	char *path = args;
	unsigned int len;
	DIR *stream;
	struct dirent *dir;

	path = psh_nextString(path, &len);

	if (!len)
		path = ".";

	do {
		stream = opendir(path);

		if (stream == NULL) {
			puts(path);
			puts(": no such directory\n");
			break;
		}

		while ((dir = readdir(stream)) != NULL) {
			puts(dir->d_name);
			puts("\n");
		}

		puts("\n");

		closedir(stream);
		path += len + 1;
	}
	while ((path = psh_nextString(path, &len)) && len);
}


static void psh_mkdir(oid_t *oid, char *args)
{
	char *path = args;
	unsigned int len;

	while ((path = psh_nextString(path, &len)) && len) {
		if (mkdir(path, 0) < 0) {
			puts(path);
			puts(": failed to create directory\n");
		}

		path += len + 1;
	}
}


static void psh_mem(oid_t *oid)
{
	printf("mem\n");
}


static void psh_ps(oid_t *oid)
{
	threadinfo_t *info;
	int tcnt, i, n = 32;

	info = malloc(n * sizeof(threadinfo_t));

	while ((tcnt = threadslist(n, info)) >= n) {
		n *= 2;
		info = realloc(info, n * sizeof(threadinfo_t));
	}

	printf("%9s %5s %4s %5s %5s %32s\n", "PID", "TTY", "PRT", "STATE", "%CPU", "CMD");

	for (i = 0; i < tcnt; ++i) {
		printf("%9x %5s %4d %5s %3d.%d %32s\n", info[i].pid, "-", info[i].priority, info[i].state ? "ready" : "sleep", info[i].load / 10, info[i].load % 10, info[i].name);
	}

	free(info);
}


void psh_runfile(oid_t *oid, char *cmd)
{
	int pid;

	if (!(pid = vfork()))
		exit(execle(cmd, cmd));

	wait(0);
}


void psh_run(oid_t *oid)
{
	unsigned int n;
	char buff[128];
	char *cmd;

	for (;;) {
		while (write(1, "(psh)% ", 7) < 0);

		psh_readln(oid, buff, sizeof(buff));
		cmd = psh_nextString(buff, &n);

		if (n == 0)
			continue;

		if (!strcmp(cmd, "help"))
			psh_help(oid);

		else if (!strcmp(cmd, "ls"))
			psh_ls(oid, cmd + 3);

		else if (!strcmp(cmd, "mem"))
			psh_mem(oid);

		else if (!strcmp(cmd, "ps"))
			psh_ps(oid);

		else if (!strcmp(cmd, "mkdir"))
			psh_mkdir(oid, cmd + 6);
#if 0
		else if (cmd[0] == '/')
			psh_runfile(oid, cmd);
#endif
		else if (!strcmp(cmd, "exit"))
			exit(EXIT_SUCCESS);

		else
			printf("Unknown command!\n");

	}
}


int main(void)
{
	unsigned int h;

	oid_t oid = { 0, 0 };

	/* (MOD) */
	fileAdd(&h, &oid);

	psh_run(&oid);
	return 0;
}
