/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * test/test_malloc
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
#include "sys/mman.h"


enum size_mode {
	SZMODE_SMALL = 0,
	SZMODE_MEDIUM,
	SZMODE_BIG,
	SZMODE_HUGE,
	SZMODE_MIXED,
	SZMODE_NUM_MODES,
};

static struct {
	unsigned nothreads;
	unsigned allocslen;

	struct {
		char stack[1024] __attribute__ ((aligned(8)));
		unsigned seed, noallocs;

		struct test_malloc_alloc {
			unsigned sz;
			char *buf;
		} allocs[100];
	} threads[1];

	handle_t mutex;
} test_malloc_common;


extern void malloc_test(void);


#define test_printf(...) do {\
		mutexLock(test_malloc_common.mutex);\
		printf(__VA_ARGS__);\
		mutexUnlock(test_malloc_common.mutex);\
	} while(0)


unsigned random_size(enum size_mode szmode, unsigned *seed)
{
	switch (szmode) {
	case SZMODE_SMALL:
		return 1 + rand_r(seed) % 100;
	case SZMODE_MEDIUM:
		return 100 + rand_r(seed) % 400;
	case SZMODE_BIG:
		return 500 + rand_r(seed) % (10 * SIZE_PAGE);
	case SZMODE_HUGE:
		return 500 + rand_r(seed) % (32 << 20);
	case SZMODE_MIXED:
		return random_size(rand_r(seed) % SZMODE_MIXED, seed);
	default:
		return 0;
	}
}


void test_malloc(unsigned threadId)
{
	unsigned *seed = &test_malloc_common.threads[threadId].seed;
	char *ptr;
	int i, j, k;
	enum size_mode szmode;
	unsigned size = 0, imax, total = 0, ftotal = 0, nofailed = 0, counter = 0, maxtotal = 0;
	const char szmodes[SZMODE_NUM_MODES][7] = {"small", "medium", "big", "huge", "mixed"};
	struct test_malloc_alloc *allocs = test_malloc_common.threads[threadId].allocs;

	test_printf("test thread %d: malloc/realloc randomized tests, seed = %u\n", threadId, *seed);

	for (i = 0; i < test_malloc_common.allocslen; i++) {
		allocs[i].sz = 0;
		allocs[i].buf = NULL;
	}

	for (;;) {
		szmode = rand_r(seed) % SZMODE_NUM_MODES;
		imax = 1 + rand_r(seed) % test_malloc_common.allocslen;
		maxtotal = 0;

		test_printf("test thread %d: allocation size: %s, up to %u buffers\n", threadId, szmodes[szmode], imax);

		for (i = 0, nofailed = 0, ftotal = 0, k = 1; k <= test_malloc_common.threads[threadId].noallocs; ++k) {
			i = rand_r(seed) % imax;
			size = random_size(szmode, seed);

			if (rand_r(seed) % 2) {
				ptr = realloc(allocs[i].buf, size);

				if (ptr == NULL) {
					nofailed++;
					ftotal += total;
					free(allocs[i].buf);
					total -= allocs[i].sz;
					allocs[i].sz = 0;
					allocs[i].buf = NULL;
				}
				else {
					for (j = 0; j < min(size, allocs[i].sz); ++j) {
						if (ptr[j] != i) {
							test_printf("test thread %d: user memory corrupted (buffer %d at %d is %d)\n", threadId, i, j, allocs[i].buf[j]);
							for (;;)
								usleep(100000);
						}
					}

					if (size > allocs[i].sz)
						memset(ptr, i, size);

					allocs[i].buf = ptr;
					total += size - allocs[i].sz;
					allocs[i].sz = size;
				}
			}
			else {
				if (allocs[i].buf != NULL) {
					for (j = 0; j < allocs[i].sz; ++j) {
						if (allocs[i].buf[j] != i) {
							test_printf("test thread %d: user memory corrupted (buffer %d at %d is %d)\n", threadId, i, j, allocs[i].buf[j]);
							for (;;)
								usleep(100000);
						}
					}

					total -= allocs[i].sz;
					free(allocs[i].buf);
				}

				allocs[i].buf = malloc(size);

				if (allocs[i].buf != NULL) {
					allocs[i].sz = size;
					total += size;
					memset(allocs[i].buf, i, size);
				}
				else {
					nofailed++;
					ftotal += total;
					allocs[i].sz = 0;
				}
			}

			maxtotal = max(total, maxtotal);
			malloc_test();
		}

		if (nofailed)
			test_printf("test thread %d: %d/%u allocations failed with avg %u bytes in use, maxtotal: %u\n", threadId, nofailed, test_malloc_common.threads[threadId].noallocs, ftotal / nofailed, maxtotal);

		if (rand_r(seed) % 2) {
			test_printf("test thread %d: freeing all memory, global counter: %u, seed: %u\n", threadId, counter++, *seed);
			for (i = 0; i < test_malloc_common.allocslen; i++) {
				free(allocs[i].buf);
				total -= allocs[i].sz;
				allocs[i].sz = 0;
				allocs[i].buf = NULL;
			}
		}
	}
}


static void test_malloc_thread(void *id)
{
	test_malloc((unsigned)(long)id);
}


int main(void)
{
	unsigned i;
	char *ptr;

	mutexCreate(&test_malloc_common.mutex);
	test_malloc_common.allocslen = sizeof(test_malloc_common.threads[0].allocs) / sizeof(test_malloc_common.threads[0].allocs[0]);
	test_malloc_common.nothreads = sizeof(test_malloc_common.threads) / sizeof(test_malloc_common.threads[0]);

	printf("test_malloc: Starting, main is at %p\n", main);

	printf("test: malloc edge cases\n");

	if ((ptr = malloc(0)) != NULL) {
		printf("test: malloc(0) succeded with %p\n", ptr);
	}

	printf("test: freeing NULL\n");
	free(NULL);

	if ((ptr = malloc((size_t) 0x7fffffff)) != NULL) {
		printf("test: malloc(-1) succeded with %p\n", ptr);
	}

	for (i = 0; i < test_malloc_common.nothreads; ++i) {
		test_malloc_common.threads[i].seed = i;
		test_malloc_common.threads[i].noallocs = 10000;
		test_printf("test: launching thread %d, stack: %p\n", i, test_malloc_common.threads[i].stack);
		beginthread(test_malloc_thread, 6, test_malloc_common.threads[i].stack, sizeof(test_malloc_common.threads[0].stack), (void *)(long)i);
	}

	for (;;) usleep(1000000);

	return 0;
}
