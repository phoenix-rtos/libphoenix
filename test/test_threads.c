/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * test/test_threads
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


handle_t h;

void thread1(void *arg)
{
	char *m = "-\\|/";
	unsigned int i = 0;

	printf("test_threads.thread1: I'm ready JPW\n");
	printf("test_threads.thread1:  ");

	for (;;) {
		lock(h);
		printf("\b%c", m[i++ % 4]);
		unlock(h);
		usleep(100);
	}
}


int main(void)
{
	char stack[512];

	printf("test_threads: Starting, main is at %p\n", main);

	mutex(&h);

	beginthread(thread1, 3, stack + sizeof(stack), NULL);

	for (;;) {
		lock(h);
		printf(".");
		unlock(h);
		usleep(1000);
	}

	return 0;
}
