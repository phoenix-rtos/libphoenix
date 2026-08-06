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
#include <signal.h>


int pause(void)
{
	sigset_t mask;
	(void)sigprocmask(SIG_BLOCK, NULL, &mask);
	return sigsuspend(&mask);
}
