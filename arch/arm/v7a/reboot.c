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

#if defined(LIBPHOENIX_MULTILIB)


int reboot(int magic)
{
	return -1;
}


int reboot_reason(uint32_t *val)
{
	return -1;
}


#else


#include <sys/platform.h>
#if defined(__CPU_ZYNQ7000)
#include <phoenix/arch/armv7a/zynq7000/zynq7000.h>
#elif defined(__CPU_IMX6ULL)
#include <phoenix/arch/armv7a/imx6ull/imx6ull.h>
#else
#error "Unsupported TARGET"
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

#endif
