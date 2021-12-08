/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/reboot.c
 *
 * Copyright 2019, 2021 Phoenix Systems
 * Author: Jan Sikorski, Lukasz Kosinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/reboot.h>
#include <sys/platform.h>


int reboot(int magic)
{
	platformctl_t pctl = {
		.action = pctl_set,
		.type = pctl_reboot,
		.reboot = {
			.magic = magic,
		}
	};

	return platformctl(&pctl);
}
