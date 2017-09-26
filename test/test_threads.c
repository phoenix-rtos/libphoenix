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
 * Common threads
 */


static void test_threads_busythr(void *arg)
{
	for (;;);
	return;
}


static void test_threads_timethr(void *arg)
{
	for (;;) {
		lock(test_threads_common.m);
		test_threads_common.tm++;
		//condSignal(&test_threads_common.c);
		unlock(test_threads_common.m);
		usleep(100000);
	}
}


/*
 * Thread test without conditional waiting
 */


static void test_threads_rotthr1(void *arg)
{
	unsigned int i = (unsigned int)arg;

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

	beginthread(test_threads_rotthr1, 1, test_threads_common.stacks[1] + stacksz, (void *)1);
	beginthread(test_threads_rotthr1, 2, test_threads_common.stacks[2] + stacksz, (void *)2);
	beginthread(test_threads_rotthr1, 3, test_threads_common.stacks[3] + stacksz, (void *)3);
	beginthread(test_threads_rotthr1, 4, test_threads_common.stacks[4] + stacksz, (void *)4);
	beginthread(test_threads_rotthr1, 5, test_threads_common.stacks[5] + stacksz, (void *)5);
	beginthread(test_threads_rotthr1, 6, test_threads_common.stacks[6] + stacksz, (void *)6);
	beginthread(test_threads_rotthr1, 7, test_threads_common.stacks[7] + stacksz, (void *)7);

	beginthread(test_threads_busythr, 4, test_threads_common.stacks[0] + stacksz, NULL);
}


/*
 * Thread test with conditional waiting
 */


static void test_threads_rotthr2(void *arg)
{
	unsigned int i = (unsigned int)arg;
	time_t otm = test_threads_common.tm;

	for (;;) {
		test_threads_common.rotations[i]++;

		lock(test_threads_common.m);
		for (;;) {
			condWait(&test_threads_common.c, &test_threads_common.m, 0);
			if (test_threads_common.tm > otm) {
				otm = test_threads_common.tm;
				break;
			}
		}
		unlock(test_threads_common.m);
	}
	return;
}

#if 0
void test_proc_threads2(void)
{
	unsigned int i;

	for (i = 0; i < 8; i++)
		test_proc_common.rotations[i] = 0;

	test_proc_common.tm = 0;
	test_proc_common.queue = NULL;
	hal_spinlockCreate(&test_proc_common.spinlock, "test_proc_common.spinlock");
	
	proc_threadCreate(NULL, test_proc_indthr, 0, NULL, NULL);
	proc_threadCreate(NULL, test_proc_timethr, 0, NULL, NULL);

	proc_threadCreate(NULL, test_proc_rotthr2, 1, NULL, (void *)1);
	proc_threadCreate(NULL, test_proc_rotthr2, 2, NULL, (void *)2);
	proc_threadCreate(NULL, test_proc_rotthr2, 3, NULL, (void *)3);
	proc_threadCreate(NULL, test_proc_rotthr2, 4, NULL, (void *)4);
}
#endif


int main(void)
{
	char *indicator = "o|/-\\|/-\\";

	printf("test_threads: Starting, main is at %p\n", main);

	mutex(&test_threads_common.m);

	test_threads_threads1();

	printf("\033[?25l");
	for (;;) {
		printf("\rtest_threads: %c %c %c %c %c %c %c  %02d %02d %02d %02d %02d %02d %02d",
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
