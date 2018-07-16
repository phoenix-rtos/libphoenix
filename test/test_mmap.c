/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * test/test_mmap
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
#include "string.h"
#include "sys/mman.h"
#include "sys/threads.h"


struct {
	handle_t mutex;
} test_mmap_common;


#define test_printf(...) do {\
		mutexLock(test_mmap_common.mutex);\
		printf(__VA_ARGS__);\
		mutexUnlock(test_mmap_common.mutex);\
	} while(0)


void test_mmap(void *threadId)
{
	const unsigned bufsz = 16;
	char *buf[bufsz];
	unsigned sizes[bufsz];

	int i = 0, j;

	unsigned seed = (unsigned)(long)threadId, sz, cnt, nofailed = 0;

	test_printf("thread %d launching\n", (int)((long) threadId));

	for (i = 0; i < bufsz; ++i) {
		buf[i] = (char*) -1;
	}

	memset(sizes, 0, sizeof(sizes));

	for (cnt = 0; ; ++cnt) {

		if (cnt && (cnt % 1000 == 0)) {
			test_printf("test (thread %d): integrity ok, %u/%u (%.2f%%) allocs failed\n", (int)(long)threadId, nofailed, cnt, 100 * (float) nofailed / cnt);
		}

		i = rand_r(&seed) % bufsz;

		if (buf[i] != (char*) -1) {
			for (j = 0; j < sizes[i]; ++j) {
				if (buf[i][j] != (char) i) {
					test_printf("test (thread %d): user memory corrupt, buffer %d at %d is %d\n", (int)(long)threadId, i, j, buf[i][j]);
					for (;;)
						usleep(1000000);
				}
			}

			munmap(buf[i], sizes[i]);
			buf[i] = (char*) -1;
			sizes[i] = 0;
		}

		sz = SIZE_PAGE * (1 + rand_r(&seed) % 32);

		buf[i] = mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_PRIVATE, NULL, 0);

		if (buf[i] != (char*) -1) {
			sizes[i] = sz;
			memset(buf[i], (char) i, sz);
		}
		else
			nofailed++;
	}
}


int main(void)
{
	int i;

	printf("test_mmap: Starting, main is at %p\n", main);
	mutexCreate(&test_mmap_common.mutex);

	for(i = 0; i < 3; ++i)
		beginthread(test_mmap, 1, malloc(1024), 1024, (void *)(uintptr_t)i);

	for (;;)
		usleep(1000000);

	return 0;
}
