/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * reboot.c
 *
 * Copyright 2022, 2024 Phoenix Systems
 * Author: Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#include <sys/reboot.h>
#include <sys/platform.h>

#if defined(__CPU_GR716)
#include <phoenix/arch/sparcv8leon3/gr716/gr716.h>
#elif defined(__CPU_GR712RC)
#include <phoenix/arch/sparcv8leon3/gr712rc/gr712rc.h>
#elif defined(__CPU_GENERIC)
#include <phoenix/arch/sparcv8leon3/generic/generic.h>
#else
#error "Unsupported TARGET"
#endif


int reboot(int magic)
{
	platformctl_t pctl = {
		.action = pctl_set,
		.type = pctl_reboot,
		/* clang-format off */
		.task.reboot = {
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
