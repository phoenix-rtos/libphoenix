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


struct {
	unsigned nothreads;
	unsigned allocslen;

	struct {
		char stack[1024] __attribute__ ((aligned(8)));
		unsigned seed, noallocs;

		struct test_malloc_alloc {
			unsigned sz;
			char *buf;
		} allocs[40];
	} threads[3];

	handle_t mutex;

} test_malloc_common;



#define test_printf(...) do {\
		mutexLock(test_malloc_common.mutex);\
		printf(__VA_ARGS__);\
		mutexUnlock(test_malloc_common.mutex);\
	} while(0)


void test_malloc(unsigned threadId)
{
	unsigned *seed = &test_malloc_common.threads[threadId].seed;
	char *ptr;
	int i, j, k, szmode;
	unsigned size = 0, imax, total = 0, ftotal = 0, nofailed = 0, counter = 0;

	struct test_malloc_alloc *allocs = test_malloc_common.threads[threadId].allocs;

	enum {
		IMODE_RANDOM = 0,
		IMODE_SEQUENTIAL,
		IMODE_NUM_MODES
	} imode;

	test_printf("test thread %d: malloc/realloc randomized tests, seed = %u\n", threadId, *seed);

	for (i = 0; i < test_malloc_common.allocslen; i++) {
		allocs[i].sz = 0;
		allocs[i].buf = NULL;
	}

	for (;;) {
		imode = rand_r(seed) % IMODE_NUM_MODES;
		szmode = rand_r(seed) % 11;
		imax = 1 + rand_r(seed) % test_malloc_common.allocslen;

		test_printf("test thread %d: size picking mode set to %d, buffer slot: %s up to %u\n", threadId, szmode,
			    (imode == IMODE_RANDOM) ? "random" : "picked sequentially", imax);


		switch (szmode) {
		case 6:
			size = 1 + rand_r(seed) % 50;
			break;
		case 7:
			size = 300 + rand_r(seed) % 100;
			break;
		case 8:
			size = 800 + rand_r(seed) % 500;
			break;
		case 9:
			size = 3000 + rand_r(seed) % 1000;
			break;
		default:
			break;
		};

		for (i = 0, nofailed = 0, ftotal = 0, k = 1; k <= test_malloc_common.threads[threadId].noallocs; ++k) {
			switch (imode) {
			case IMODE_RANDOM:
				i = rand_r(seed) % imax;
				break;
			case IMODE_SEQUENTIAL:
				i = (i + 1) % test_malloc_common.allocslen;
				break;
			case IMODE_NUM_MODES:
				break;
			}

			switch (szmode) {
			case 0:
				size = 1 + rand_r(seed) % 5;
				break;
			case 1:
				size = 1 + rand_r(seed) % 50;
				break;
			case 2:
				size = 1 + rand_r(seed) % 500;
				break;
			case 3:
				size = 1 + rand_r(seed) % 1500;
				break;
			case 4:
				size = 1 + rand_r(seed) % 5000;
				break;
			case 5:
				size = 1000 + rand_r(seed) % 200;
				break;
			case 10:
				size = 1 << (rand_r(seed) % 10);
				break;
			default:
				break;
			};


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
							for (;;) ;
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
							for (;;) ;
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
		}

		if (nofailed)
			test_printf("test thread %d: %d/%u allocations failed with avg %u bytes in use\n", threadId, nofailed, test_malloc_common.threads[threadId].noallocs, ftotal / nofailed);

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
	test_malloc((unsigned) id);
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

	if ((ptr = malloc((size_t) -1)) != NULL) {
		printf("test: malloc(-1) succeded with %p\n", ptr);
	}

	for (i = 0; i < test_malloc_common.nothreads; ++i) {
		test_malloc_common.threads[i].seed = i;
		test_malloc_common.threads[i].noallocs = 10000;
		test_printf("test: launching thread %d, stack: %p\n", i, test_malloc_common.threads[i].stack);
		beginthread(test_malloc_thread, 1, test_malloc_common.threads[i].stack + sizeof(test_malloc_common.threads[0].stack), (void*) i);
	}

	for (;;) usleep(1000000);

	return 0;
}
