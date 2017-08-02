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


void thread1(void *arg)
{
	char *m = "-\\|/";
	unsigned int i = 0;

	printf("test_threads.thread1: I'm ready\n");
	printf("test_threads.thread1:  ");

	for (;;) {
		printf("\b%c", m[i++ % 4]);
		usleep(100);
	}
}


int main(void)
{
	char stack[512];

	printf("test_threads: Starting, main is at %p\n", main);

	beginthread(thread1, 3, stack + sizeof(stack), NULL);

	for (;;) {
		printf(".");
		usleep(1000);
	}

	return 0;
}
