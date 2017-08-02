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
#include "unistd.h"
#include "sys/mman.h"


void test_malloc(void)
{
	char *buff[150];
	int i, k;
	unsigned size, seed = 23;

	printf("test: Testing malloc\n");

	for (i = 0; i < sizeof(buff) / sizeof(buff[0]); i++)
		buff[i] = NULL;

	for (k = 0; k < 1000; k++) {
		size = (unsigned long long)(1 << 16) * rand_r(&seed) / RAND_MAX;
		i = (unsigned long long)(sizeof(buff) / sizeof(buff[0]) - 1) * rand_r(&seed) / RAND_MAX;

		if (buff[i] != NULL) {
			free(buff[i]);
			buff[i] = NULL;
		}

		printf("\rtest: [%6d] allocating %09d, %4d", k, size, i);
 		buff[i] = malloc(size);
	}
	printf("\n");

	for (i = 0; i < sizeof(buff) / sizeof(buff[0]); i++) {
		if (buff[i] != NULL)
			free(buff[i]);
	}
}


int main(void)
{
	printf("test_malloc: Starting, main is at %p\n", main);
	test_malloc();

	for (;;);

	return 0;
}
