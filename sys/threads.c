/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/threads
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/threads.h>
#include <errno.h>


int mutexLock2(handle_t m1, handle_t m2)
{
	int err;
	int tmp;

	if ((err = mutexLock(m1)) < 0)
		return err;

	while (mutexTry(m2) < 0) {
		mutexUnlock(m1);
		if ((err = mutexLock(m2)) < 0)
			return err;

		tmp = m1;
		m1 = m2;
		m2 = tmp;
	}

	return EOK;
}
