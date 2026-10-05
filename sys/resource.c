/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/resource.c
 *
 * Copyright 2019 Phoenix Systems
 * Author: Andrzej Glowinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <errno.h>
#include <sched.h>
#include <sys/proc.h>
#include <sys/resource.h>


/*
 * Since the kernel doesn't support policies other than SCHED_RR and POSIX says
 * that SCHED_RR threads are unaffected by setpriority/getpriority, just do nothing.
 */
static int sched_rr_noop(int which, id_t who)
{
	if (which != PRIO_USER && which != PRIO_PGRP && which != PRIO_PROCESS) {
		return SET_ERRNO(-EINVAL);
	}

	if (who == 0) {
		return 0;
	}

	if (which == PRIO_USER) {
		/* OS-LIMITATION: users are not supported */
		return SET_ERRNO(-EINVAL);
	}

	if (which == PRIO_PROCESS && !pidExists((pid_t)who)) {
		return SET_ERRNO(-ESRCH);
	}

	if (which == PRIO_PGRP && !pgidExists((pid_t)who)) {
		return SET_ERRNO(-ESRCH);
	}

	return 0;
}


int setpriority(int which, id_t who, int prio)
{
	(void)prio;
	return sched_rr_noop(which, who);
}


int getpriority(int which, id_t who)
{
	return sched_rr_noop(which, who);
}
