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

#define BUFLEN 40

static char *buff[BUFLEN];
static unsigned sizes[BUFLEN];

void test_malloc(unsigned seed)
{
	char *p;
	int i, j, k, szmode, nofailed;
	unsigned size = 0, imodeTrunc = 0;

	unsigned long total = 0, failedTotal;

	enum {
		IMODE_RANDOM = 0,
		IMODE_TRUNCATED_RANDOM,
		IMODE_SEQUENTIAL,
		IMODE_NUM_MODES
	} imode;

	printf("test: malloc/realloc randomized tests, seed = %u\n", seed);

	memset(sizes, 0, BUFLEN * sizeof(unsigned));

	for (i = 0; i < BUFLEN; i++)
		buff[i] = NULL;

	for (;;) {
		imode = rand_r(&seed) % IMODE_NUM_MODES;
		szmode = rand_r(&seed) % 11;

		printf("test: size picking mode set to %d, buffer slot: %s\n", szmode,
		       (imode == 0) ? "random" :
		       (imode == 1) ? "truncated random" : "picked sequentially");
		i = 0;

		if (imode == IMODE_TRUNCATED_RANDOM) {
			imodeTrunc = 1 + rand_r(&seed) % BUFLEN;
			printf("test:   truncated to %u\n", imodeTrunc);
		}

		switch (szmode) {
		case 6:
			size = 1 + rand_r(&seed) % 50;
			break;
		case 7:
			size = 300 + rand_r(&seed) % 100;
			break;
		case 8:
			size = 800 + rand_r(&seed) % 500;
			break;
		case 9:
			size = 3000 + rand_r(&seed) % 1000;
			break;
		default:
			break;
		};

		nofailed = 0;
		failedTotal = 0;
		for (k = 1; k <= 100; ++k) {
			switch (imode) {
			case IMODE_RANDOM:
				i = rand_r(&seed) % BUFLEN;
				break;
			case IMODE_TRUNCATED_RANDOM:
				i = rand_r(&seed) % imodeTrunc;
				break;
			case IMODE_SEQUENTIAL:
				i = (i + 1) % BUFLEN;
				break;
			case IMODE_NUM_MODES:
				break;
			}

			switch (szmode) {
			case 0:
				size = 1 + rand_r(&seed) % 5;
				break;
			case 1:
				size = 1 + rand_r(&seed) % 50;
				break;
			case 2:
				size = 1 + rand_r(&seed) % 500;
				break;
			case 3:
				size = 1 + rand_r(&seed) % 1500;
				break;
			case 4:
				size = 1 + rand_r(&seed) % 5000;
				break;
			case 5:
				size = 1000 + rand_r(&seed) % 200;
				break;
			case 10:
				size = 1 << (rand_r(&seed) % 10);
				break;
			default:
				break;
			};


			if (rand_r(&seed) % 2) {
				if (k % 10 == 0)
					printf("\33[2K\rtest: %d/100 reallocing %u to %u", k, sizes[i], size);

				p = realloc(buff[i], size);

				if (p == NULL) {
					nofailed++;
					failedTotal += total;
					free(buff[i]);
					total -= sizes[i];
					sizes[i] = 0;
					buff[i] = NULL;
				}
				else {
					for (j = 0; j < min(size, sizes[i]); ++j) {
						if (p[j] != i) {
							printf("\ntest: user memory corrupted (buffer %d at %d)\n", i, j);
							for (;;) ;
						}
					}

					if (size > sizes[i])
						memset(p, i, size);

					buff[i] = p;
					total += size - sizes[i];
					sizes[i] = size;
				}
			}
			else {
				if (buff[i] != NULL) {
					for (j = 0; j < sizes[i]; ++j) {
						if (buff[i][j] != i) {
							printf("\ntest: user memory corrupted (buffer %d at %d)\n", i, j);
							for (;;) ;
						}
					}

					total -= sizes[i];
					free(buff[i]);
					buff[i] = NULL;
				}

				if (k % 10 == 0)
					printf("\33[2K\rtest: %d/100 allocating %u", k, size);

				buff[i] = malloc(size);

				if (buff[i] != NULL) {
					sizes[i] = size;
					total += size;
					memset(buff[i], i, size);
				}
				else {
					nofailed++;
					failedTotal += total;
					sizes[i] = 0;
				}
			}
		}

		if (nofailed)
			printf("\ntest: %d/500 allocations failed with avg %u bytes in use\n", nofailed, failedTotal / nofailed);
		else
			printf("\n");

		if (rand_r(&seed) % 2 == 0) {
			printf("test: freeing all memory\n");
			for (i = 0; i < BUFLEN; i++) {
				free(buff[i]);
				total -= sizes[i];
				sizes[i] = 0;
				buff[i] = NULL;
			}
		}
	}
}


int main(void)
{
	unsigned seed = 137;
	char *ptr;

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

	test_malloc(seed++);

	return 0;
}
