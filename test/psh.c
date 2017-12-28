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
#include "unistd.h"
#include "sys/threads.h"
#include "sys/msg.h"


int psh_write(oid_t *oid, char *buff, size_t len)
{
	msg_t msg;

	msg.type = mtWrite;
	memcpy(&msg.i.io.oid, &oid, sizeof(oid_t));

	msg.i.io.offs = 0;
	msg.i.data = buff;
	msg.i.size = len;

	msg.o.data = NULL;
	msg.o.size = 0;

	if (msgSend(oid->port, &msg) < 0)
		return -1;
	
	return 0;
}


int psh_read(oid_t *oid, char *buff, size_t len)
{
	msg_t msg;

	msg.type = mtRead;
	memcpy(&msg.i.io.oid, &oid, sizeof(oid_t));

	msg.i.io.offs = 0;
	msg.i.data = NULL;
	msg.i.size = 0;

	msg.o.data = buff;
	msg.o.size = len;

	if (msgSend(oid->port, &msg) < 0)
		return -1;

	return 0;
}


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
		psh_read(oid, &c, 1);

		if (c == 0)
			break;

		if (!psh_isAcceptable(c))
			continue;

		/* Check, if line has maximum size. */
		if (count >= size - 1 && c != 0x7f && !psh_isNewline(c))
			continue;

		if (psh_isNewline(c))
			break;

		/* Print character. */
		if (c != 0x7f || count > 0)
			psh_write(oid, &c, 1);

		/* Handle delete. */
		if (c == 0x7f) {
			if (count > 0) {
				--count;
				psh_write(oid, "\b \b", 3);
			}
		}
		else
			line[count++] = c;
	}

	memset(&line[count], '\0', size - count);
	psh_write(oid, "\n", 1);

	return count;
}


static void psh_threads(oid_t *oid)
{
	psh_write(oid, "threads\n", 8);

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


static void psh_mem(oid_t *oid)
{
	psh_write(oid, "mem\n", 4);
}


static void psh_help(oid_t *oid)
{
	psh_write(oid, "help\n", 5);

/*	lib_printf("Available commands:\n");
	lib_printf("\thelp          - prints this help\n");
	lib_printf("\tpidin         - threads statistics\n");
	lib_printf("\tmmap          - memory map\n");
	lib_printf("\tlogo          - displays Phoenix-RTOS logo\n");
	lib_printf("\ticon [ 1..4 ] - change current LCD screen\n");*/
}


void psh_run(oid_t *oid)
{
	unsigned int n;
	char buff[128];
	char *cmd;

	for (;;) {
		while (psh_write(oid, "(psh)% ", 7) < 0);

		psh_readln(oid, buff, sizeof(buff));
		cmd = psh_nextString(buff, &n);

		if (n == 0)
			continue;

		if (!strcmp(cmd, "help"))
			psh_help(oid);

		else if (!strcmp(cmd, "threads"))
			psh_threads(oid);

		else if (!strcmp(cmd, "mem"))
			psh_mem(oid);

		else
			psh_write(oid, "Unknown command!\n", 17);

	}
}


int main(void)
{
	oid_t oid = { 0, 0 };

	psh_run(&oid);
	return 0;
}
