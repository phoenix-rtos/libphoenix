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

#if defined(__CPU_STM32L4X6)
#include <phoenix/arch/stm32l4.h>
#elif defined(__CPU_IMXRT117X)
#include <phoenix/arch/imxrt1170.h>
#else
#include <phoenix/arch/imxrt.h>
#endif


int reboot(int magic)
{
	platformctl_t pctl = {
		.action = pctl_set,
		.type = pctl_reboot,
		.reboot = {
			.magic = magic
		}
	};

	return platformctl(&pctl);
}


int reboot_reason(uint32_t *val)
{
	platformctl_t pctl = {
		.action = pctl_get,
		.type = pctl_reboot,
	};

	*val = 0;
	if (platformctl(&pctl) < 0)
		return -1;

	*val = pctl.reboot.reason;
	return 0;
}
