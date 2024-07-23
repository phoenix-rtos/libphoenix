/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * reboot.c
 *
 * Copyright 2024 Phoenix Systems
 * Author: Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/reboot.h>
#include <sys/platform.h>
#if defined(__CPU_MPS3AN536)
#include <phoenix/arch/armv8r/mps3an536/mps3an536.h>
#else
#error "Unsupported TARGET"
#endif


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
