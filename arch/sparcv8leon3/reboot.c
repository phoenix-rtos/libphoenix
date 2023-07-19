/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * reboot.c
 *
 * Copyright 2022 Phoenix Systems
 * Author: Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#include <sys/reboot.h>
#include <sys/platform.h>
#include <phoenix/arch/gr716.h>


int reboot(int magic)
{
	platformctl_t pctl = {
		.action = pctl_set,
		.type = pctl_reboot,
		/* clang-format off */
		.reboot = {
			.magic = magic
		}
		/* clang-format on */
	};

	return platformctl(&pctl);
}


int reboot_reason(uint32_t *val)
{
	*val = 0u;
	return 0;
}
