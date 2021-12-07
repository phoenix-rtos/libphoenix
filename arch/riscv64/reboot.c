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
#include <errno.h>


int reboot(int magic)
{
	return -EINVAL;
}


int reboot_reason(uint32_t *val)
{
	return -EINVAL;
}
