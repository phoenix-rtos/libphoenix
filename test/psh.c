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

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "dirent.h"
#include "sys/threads.h"
#include "sys/mman.h"
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


static int psh_readln(char *line, int size)
{
	int count = 0;
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

	return count;
}


static void psh_help(void)
{
	printf("Available commands:\n");
	printf("  help   - prints this help\n");
	printf("  ls     - lists files in the namespace\n");
	printf("  mkdir  - creates directory\n");
	printf("  touch  - changes file timestamp\n");
	printf("  mem    - prints memory map\n");
	printf("  ps     - prints list of processes and threads\n");
	printf("  exit   - exits the shell\n");
}


static void psh_ls(char *args)
{
	char *path = args;
	unsigned int len, n = 0;
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
			if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
				if (!dir->d_type)
					printf("\033[34m%-20s\033[0m",dir->d_name);
				else
					printf("%-20s",dir->d_name);

				if (!(++n % 7))
					printf("\n");
			}
		}

		puts("\n");

		closedir(stream);
		path += len + 1;
	}
	while ((path = psh_nextString(path, &len)) && len);
}


static void psh_mkdir(char *args)
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


static void psh_touch(char *args)
{
	char *path = args;
	unsigned int len;
	FILE *f;

	while ((path = psh_nextString(path, &len)) && len) {
		if ((f = fopen(path, "w")) == NULL) {
			puts(path);
			puts(": fopen failed\n");
		}
		else
			fclose(f);

		path += len + 1;
	}
}

static void psh_mem(char *args)
{
	char *arg, *end;
	unsigned int len, i, n;
	meminfo_t info;
	int mapsz = 0;
	entryinfo_t *e = NULL;
	pageinfo_t *p = NULL;
	char flags[8], *f;

	memset(&info, 0, sizeof(info));
	arg = psh_nextString(args, &len);
	args += len + 1;

	if (!len) {
		/* show summary */
		info.page.mapsz = -1;
		info.entry.mapsz = -1;
		info.entry.kmapsz = -1;

		meminfo(&info);

		printf("(%d+%d)/%dKB\n", (info.page.alloc - info.page.boot) / 1024, info.page.boot / 1024,
			(info.page.alloc + info.page.free) / 1024);

		/*printf(
			"total map entries: %12u\n"
			"free map entries:  %12u\n"
			"map entry size:    %12u\n",
			info.entry.total, info.entry.free, info.entry.sz);*/

		return;
	}

	if (!strcmp("-m", arg)) {
		info.page.mapsz = -1;
		arg = psh_nextString(args, &len);

		if (!strcmp("kernel", arg))  {
			/* show memory map of the kernel */
			info.entry.mapsz = -1;
			info.entry.kmapsz = 16;

			do {
				mapsz = info.entry.kmapsz;
				info.entry.kmap = realloc(info.entry.kmap, mapsz * sizeof(entryinfo_t));
				meminfo(&info);
			}
			while (info.entry.kmapsz > mapsz);

			mapsz = info.entry.kmapsz;
			e = info.entry.kmap;
		}
		else {
			/* show memory map of a process */
			if (len) {
				info.entry.pid = strtoul(arg, &end, 16);

				if (end + 1 != args + len || (!info.entry.pid && *arg != '0')) {
					printf("mem: could not parse process id: '%s'\n", arg);
					return;
				}
			}
			else {
				info.entry.pid = getpid();
			}

			info.entry.kmapsz = -1;
			info.entry.mapsz = 16;

			do {
				mapsz = info.entry.mapsz;
				info.entry.map = realloc(info.entry.map, mapsz * sizeof(entryinfo_t));
				meminfo(&info);
			}
			while (info.entry.mapsz > mapsz);

			if (info.entry.mapsz < 0) {
				printf("mem: process with pid %x not found\n", info.entry.pid);
				free(info.entry.map);
				return;
			}

			mapsz = info.entry.mapsz;
			e = info.entry.map;
		}

		printf("%-17s  PROT  FLAGS  %16s  OBJECT\n", "SEGMENT", "OFFSET");

		e += mapsz - 1;
		for (i = 0; i < mapsz; ++i, --e) {
			memset(f = flags, 0, sizeof(flags));

			if (e->flags & MAP_PRIVATE)
				*(f++) = 'P';

			if (e->flags & MAP_FIXED)
				*(f++) = 'F';

			if (e->flags & MAP_ANONYMOUS)
				*(f++) = 'A';

			char *s;

			if (e->object == NULL)
				s = "(anonymous)";
			else if (e->object == (void *)-1)
				s = "mem";
			else
				s = "1.1";

			printf("%p:%p  rwx-  %5s  %16llx  %s\n", e->vaddr, e->vaddr + e->size - 1, flags, e->offs, s);
		}

		free(info.entry.map);
		free(info.entry.kmap);

		return;
	}

	if (!strcmp("-p", arg)) {
		/* show page map */
		info.entry.mapsz = ~0;
		info.entry.kmapsz = ~0;
		info.page.mapsz = 16;

		do {
			mapsz = info.page.mapsz;
			info.page.map = realloc(info.page.map, mapsz * sizeof(pageinfo_t));
			meminfo(&info);
		}
		while (info.page.mapsz > mapsz);

		for (i = 0, p = info.page.map; i < info.page.mapsz; ++i, ++p) {
			if (p != info.page.map && (n = (p->addr - (p - 1)->addr) / SIZE_PAGE - (p - 1)->count)) {
				if (n > 3) {
					printf("[%ux]", n);
				}
				else {
					while (n-- > 0)
						printf("x");
				}
			}

			if ((n = p->count) > 3) {
				printf("[%u%c]", p->count, p->marker);
				continue;
			}

			while (n-- > 0)
				printf("%c", p->marker);
		}
		printf("\n");

		free(info.page.map);
		return;
	}

	printf("mem: unrecognized option '%s'\n", arg);
}


static void psh_ps(void)
{
	threadinfo_t *info;
	int tcnt, i, n = 32;

	info = malloc(n * sizeof(threadinfo_t));

	while ((tcnt = threadsinfo(n, info)) >= n) {
		n *= 2;
		info = realloc(info, n * sizeof(threadinfo_t));
	}

	printf("%9s %5s %4s  %5s %5s %s\n", "PID", "TTY", "PRI", "STATE", "%CPU", "CMD");

	for (i = 0; i < tcnt; ++i) {
		printf("%9x %5s %4d  %5s %3d.%d  %-32s\n", info[i].pid, "-", info[i].priority, info[i].state ? "sleep" : "ready", info[i].load / 10, info[i].load % 10, info[i].name);
	}

	free(info);
}


void psh_runfile(char *cmd)
{
	int pid;
	int argc = 0;
	char **argv = NULL;

	char *arg = cmd;
	unsigned int len;

	while ((arg = psh_nextString(arg, &len)) && len) {
		argv = realloc(argv, (1 + argc) * sizeof(char *));
		argv[argc] = arg;

		argc++;
		arg += len + 1;
	}

	argv[argc] = NULL;

	if (!(pid = vfork()))
		exit(execve(cmd, argv, NULL));

	wait(0);
}


void psh_run(void)
{
	unsigned int n;
	char buff[128];
	char *cmd;

	for (;;) {
		write(1, "(psh)% ", 7);

		psh_readln(buff, sizeof(buff));
		cmd = psh_nextString(buff, &n);

		if (n == 0)
			continue;

		if (!strcmp(cmd, "help"))
			psh_help();

		else if (!strcmp(cmd, "ls"))
			psh_ls(cmd + 3);

		else if (!strcmp(cmd, "mem"))
			psh_mem(cmd + 4);

		else if (!strcmp(cmd, "ps"))
			psh_ps();

		else if (!strcmp(cmd, "touch"))
			psh_touch(cmd + 6);

		else if (!strcmp(cmd, "mkdir"))
			psh_mkdir(cmd + 6);

		else if (cmd[0] == '/')
			psh_runfile(cmd);

		else if (!strcmp(cmd, "exit"))
			exit(EXIT_SUCCESS);

		else
			printf("Unknown command!\n");
	}
}


int main(void)
{
	oid_t oid;

	/* Wait for filesystem */
	while (lookup("/", &oid) < 0)
		usleep(10000);

	/* Wait for console */
	while (write(1, "", 0) < 0)
		usleep(10000);

	psh_run();
	return 0;
}
