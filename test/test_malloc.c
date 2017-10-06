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
#include "sys/mman.h"


void test_malloc(void)
{
	char *buff[40];
	unsigned sizes[40];

	int i, j, k;
	unsigned size, seed = 23;

	unsigned total = 0;

	printf("test: Testing malloc\n");

	memset(sizes, 0, 40 * sizeof(unsigned));

	for (i = 0; i < sizeof(buff) / sizeof(buff[0]); i++)
		buff[i] = NULL;

	for (k = 0; k < 200; ++k) {
		i = rand_r(&seed) % (sizeof(buff) / sizeof(buff[0]));
		size = rand_r(&seed) % 500;

		if (buff[i] != NULL) {
			for (j = 0; j < sizes[i]; ++j) {
				if (buff[i][j] != i) {
					printf("test: memory corrupted\n");
					break;
				}
			}

			total -= sizes[i];
			free(buff[i]);
			buff[i] = NULL;
		}

		printf("\rtest: %d/200 allocating %u  ", k, size);

		buff[i] = malloc(size);

		if (buff[i]) {
			sizes[i] = size;
			total += size;
			memset(buff[i], i, size);
		}
		else {
			sizes[i] = 0;
			printf("malloc failed, total memory: %u\n", total);
		}
	}

	printf("\ntest: edge cases\n");

	printf("test: allocating 0\n");
	if (buff[0] = malloc(0)) {
		printf("test:      succeded with %p\n", buff[0]);
	}

	printf("test: freeing 0\n");
	free(NULL);

	printf("test: allocating -1\n");

	if (buff[0] = malloc((size_t) -1)) {
		printf("test:      succeded with %p\n", buff[0]);
	}

	printf("test: done\n");
}


int main(void)
{
	printf("test_malloc: Starting, main is at %p\n", main);
	test_malloc();

	for (;;);

	return 0;
}
