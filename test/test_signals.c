/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * test/test_signals
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "sys/threads.h"


void handler(int s)
{
	printf("\r%8x: %2d", getpid(), s);
	signalReturn(s);
}


void signaler(void)
{
	int ppid = getppid(), pid = getpid();
	unsigned seed = 100;

	printf("test_signals: signaler %x, son of %x\n", pid, ppid);

	signalHandle(handler, 0, 0);
	signalMask(0xffff, 0xffff);

	for (;;) {
		signalPost(pid, -1, 1 + rand_r(&seed) % 31);
		usleep(rand_r(&seed) % (50 * 1000));
		signalPost(ppid, -1, 1 + rand_r(&seed) % 31);
		usleep(rand_r(&seed) % (50 * 1000));
	}
}


int main(void)
{
	printf("test_signals: start (%x)\n", getpid());

	if (!fork())
		signaler();

	signalHandle(handler, 0, 0);
	signalMask(0xffff0000, 0xffff0000);

	for (;;)
		usleep(1000 * 1000);

	return 0;
}
