/*
 * Phoenix-RTOS
 *
 * Native API
 *
 * Debug routines
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "libphoenix.h"


void ph_printf(const char *fmt, ...)
{
	__asm__ volatile ("movl $0, %%eax; int $0x80;"::);
	return;
}
