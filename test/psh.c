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
#include "sys/threads.h"
#include "sys/msg.h"
#include "sys/file.h"


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
	char c;

	for (;;) {
		read(1, &c, 1);

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


static void psh_help(oid_t *oid)
{
	printf("Available commands:\n");
	printf("  help   - prints this help\n");
	printf("  ls     - lists files in the namespace\n");
	printf("  mem    - prints memory map\n");
	printf("  ps     - prints list of processes and threads\n");
}


static void psh_ls(oid_t *oid)
{
	printf("ls\n");
}


static void psh_mem(oid_t *oid)
{
	printf("mem\n");
}


static void psh_ps(oid_t *oid)
{
	printf("%9s %8s %16s %100s\n", "PID", "TTY", "TIME", "CMD");

/*	thread_t *t;
	int load;

	t = threads_getFirstThread();

	while (t != NULL) {
		load = threads_getCpuTime(t);
		lib_printf("load: %02d.%01d%%  proc: -  id: 0x%p  priority: %01u  state: %s  stack: 0x%p\n",  load / 10, load % 10,
			t->id, t->priority, (t->state == READY) ? "ready" : "sleep", t->context);
		t = threads_getNextThread(t);
	}*/
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
			psh_ls(oid);

		else if (!strcmp(cmd, "mem"))
			psh_mem(oid);

		else if (!strcmp(cmd, "ps"))
			psh_ps(oid);

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
