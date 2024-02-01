/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * reboot.c
 *
 * Copyright 2019 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/reboot.h>
#include <sys/platform.h>
#include <phoenix/arch/riscv64.h>


int reboot(int magic)
{
	platformctl_t pctl = {
		.action = pctl_set,
		.type = pctl_reboot,
		.task.reboot.magic = magic
	};

	return platformctl(&pctl);
}


int reboot_reason(uint32_t *val)
{
	*val = 0u;
	return 0;
}
