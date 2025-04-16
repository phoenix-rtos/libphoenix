/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * reboot.c
 *
 * Copyright 2019, 2024 Phoenix Systems
 * Author: Jan Sikorski, Jacek Maksymowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/reboot.h>
#include <sys/platform.h>
#if defined(__CPU_ZYNQMP)
#include <phoenix/arch/armv7r/zynqmp/zynqmp.h>
#elif defined(__CPU_TDA4VM)
#include <phoenix/arch/armv7r/tda4vm/tda4vm.h>
#else
#error "Unsupported TARGET"
#endif


int reboot(int magic)
{
	platformctl_t pctl = {
		.action = pctl_set,
		.type = pctl_reboot,
		.reboot = {
			.magic = magic }
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
	if (platformctl(&pctl) < 0) {
		return -1;
	}

	*val = pctl.reboot.reason;
	return 0;
}
