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


struct {
	volatile unsigned int rotations[8];
	void *stacks[8];
	volatile time_t tm;
	handle_t m;
	handle_t c;
} test_threads_common;


/*
 * Thread test without conditional waiting
 */


static void test_threads_busythr(void *arg)
{
	for (;;);
	return;
}


static void test_threads_rotthr1(void *arg)
{
	unsigned int i = (unsigned int)(long)arg;

	for (;;) {
		test_threads_common.rotations[i]++;
		usleep(50 * i * 1000);
	}

	return;
}


void test_threads_threads1(void)
{
	unsigned int i, stacksz = 512;

	for (i = 0; i < 8; i++) {
		test_threads_common.rotations[i] = 0;
		test_threads_common.stacks[i] = malloc(stacksz);
		*(int *)(test_threads_common.stacks[i] + stacksz - 4) = i;
	}

	beginthread(test_threads_rotthr1, 1, test_threads_common.stacks[1], stacksz, (void *)1);
	beginthread(test_threads_rotthr1, 2, test_threads_common.stacks[2], stacksz, (void *)2);
	beginthread(test_threads_rotthr1, 3, test_threads_common.stacks[3], stacksz, (void *)3);
	beginthread(test_threads_rotthr1, 4, test_threads_common.stacks[4], stacksz, (void *)4);
	beginthread(test_threads_rotthr1, 5, test_threads_common.stacks[5], stacksz, (void *)5);
	beginthread(test_threads_rotthr1, 6, test_threads_common.stacks[6], stacksz, (void *)6);
	beginthread(test_threads_rotthr1, 7, test_threads_common.stacks[7], stacksz, (void *)7);

	beginthread(test_threads_busythr, 6, test_threads_common.stacks[0], stacksz, NULL);
}


/*
 * Thread test with conditional waiting
 */


static void test_threads_timethr(void *arg)
{
	for (;;) {
		mutexLock(test_threads_common.m);
		test_threads_common.tm++;

		condSignal(test_threads_common.c);
		mutexUnlock(test_threads_common.m);
		usleep(1000);
	}
}


static void test_threads_rotthr2(void *arg)
{
	unsigned int i = (unsigned int)(long)arg;
	time_t otm = test_threads_common.tm;

	for (;;) {
		test_threads_common.rotations[i]++;

		mutexLock(test_threads_common.m);
		for (;;) {
			condWait(test_threads_common.c, test_threads_common.m, 0);

			if (test_threads_common.tm > otm + 20 * i) {
				otm = test_threads_common.tm;
				break;
			}
		}
		mutexUnlock(test_threads_common.m);
	}
	return;
}


void test_threads_threads2(void)
{
	unsigned int i, stacksz = 512;

	for (i = 0; i < 8; i++) {
		test_threads_common.rotations[i] = 0;
		test_threads_common.stacks[i] = malloc(stacksz);
		*(int *)(test_threads_common.stacks[i] + stacksz - 4) = i;
	}
	test_threads_common.tm = 0;
	mutexCreate(&test_threads_common.m);
	condCreate(&test_threads_common.c);

	beginthread(test_threads_timethr, 0, test_threads_common.stacks[0], stacksz, NULL);

	beginthread(test_threads_rotthr2, 1, test_threads_common.stacks[1], stacksz, (void *)1);
	beginthread(test_threads_rotthr2, 2, test_threads_common.stacks[2], stacksz, (void *)2);
	beginthread(test_threads_rotthr2, 3, test_threads_common.stacks[3], stacksz, (void *)3);
	beginthread(test_threads_rotthr2, 4, test_threads_common.stacks[4], stacksz, (void *)4);
	beginthread(test_threads_rotthr2, 5, test_threads_common.stacks[5], stacksz, (void *)5);
	beginthread(test_threads_rotthr2, 6, test_threads_common.stacks[6], stacksz, (void *)6);
	beginthread(test_threads_rotthr2, 7, test_threads_common.stacks[7], stacksz, (void *)7);

	return;
}


int main(void)
{
	char *indicator = "o|/-\\|/-\\";
	unsigned int i = 0;

	printf("test_threads: Starting, main is at %p\n", main);

	test_threads_threads2();

	printf("\033[?25l");
	for ( i = 0;; i++) {
		printf("\rtest_threads: %2d %c %c %c %c %c %c %c  %02d %02d %02d %02d %02d %02d %02d",
			i % 100,
			indicator[test_threads_common.rotations[1] % 8],
			indicator[test_threads_common.rotations[2] % 8],
			indicator[test_threads_common.rotations[3] % 8],
			indicator[test_threads_common.rotations[4] % 8],
			indicator[test_threads_common.rotations[5] % 8],
			indicator[test_threads_common.rotations[6] % 8],
			indicator[test_threads_common.rotations[7] % 8],

			test_threads_common.rotations[1] % 100,
			test_threads_common.rotations[2] % 100,
			test_threads_common.rotations[3] % 100,
			test_threads_common.rotations[4] % 100,
			test_threads_common.rotations[5] % 100,
			test_threads_common.rotations[6] % 100,
			test_threads_common.rotations[7] % 100);

		usleep(5000);
	}

	return 0;
}
