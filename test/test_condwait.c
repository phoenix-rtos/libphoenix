/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * test/test_condwait
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michał Mirosław
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/threads.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int test_condwait(void)
{
	handle_t m, c;
	time_t now, then;
	int err;

	if (mutexCreate(&m) != EOK)
		return -1;

	if (condCreate(&c) != EOK)
		return -1;

	gettime(&then, NULL);
	err = condWait(c, m, 1);
	gettime(&now, NULL);

	printf("test_condwait: ret %d (%s) elapsed %llu [us]\n", err, strerror(err), now - then);
	return now - then >= 1 && err == -ETIME ? 0 : -1;
}


int main(void)
{
	printf("test_condwait: Starting, main is at %p\n", main);
	int res = test_condwait();

	printf("condwait: %s\n", res == 0 ? "PASSED" : "FAILED");

	for (;;);

	return 0;
}

