/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd pause
 *
 * Copyright 2023 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <unistd.h>
#include <errno.h>
#include <sys/threads.h>


int pause(void)
{
	handle_t cond, lock;
	int err;

	err = condCreate(&cond);
	if (err < 0) {
		return SET_ERRNO(err);
	}

	err = mutexCreate(&lock);
	if (err < 0) {
		resourceDestroy(cond);
		return SET_ERRNO(err);
	}

	err = phMutexLock(lock);
	if (err >= 0) {
		/* Sleep on a private cond - only signal can wake us up */
		while (err >= 0) {
			err = phCondWait(cond, lock, 0);
		}

		if (err != -EINTR) {
			/* On EINTR phCondWait does not relock the lock.
			 * Unlock it manually on other cases */
			(void)mutexUnlock(lock);
		}
	}

	resourceDestroy(cond);
	resourceDestroy(lock);

	return SET_ERRNO(err);
}
