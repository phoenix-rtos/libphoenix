/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/interrupt
 *
 * Copyright 2026 Phoenix Systems
 * Author: Lukasz Leczkowski
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <errno.h>
#include <sys/interrupt.h>
#include <sys/threads.h>


int interrupt_queueCreate(handle_t *queue, int clock)
{
	const struct condAttr attr = { .clock = clock, .type = PH_COND_UNLOCKED };

	return phCondCreate(queue, &attr);
}


int interrupt_queueCreateLocked(handle_t *queue, int clock)
{
	const struct condAttr attr = { .clock = clock, .type = PH_COND_NORMAL };

	return phCondCreate(queue, &attr);
}


int interrupt_queueWait(handle_t queue, time_t timeout)
{
	int err;

	do {
		/* FIXME: for PH_CLOCK_RELATIVE the timeout should be recalculated on EINTR */
		err = phCondWait(queue, (handle_t)-1, timeout);
	} while (err == -EINTR);

	return err;
}


int interrupt_queueWaitLocked(handle_t queue, handle_t lock, time_t timeout)
{
	return condWait(queue, lock, timeout);
}


int interrupt_queueSignal(handle_t queue)
{
	return condSignal(queue);
}


int interrupt_queueBroadcast(handle_t queue)
{
	return condBroadcast(queue);
}
