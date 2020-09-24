/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Signal trampoline (armv7)
 *
 * Copyright 2019 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#define __ASSEMBLY__

.text
.globl _signal_trampoline
.type _signal_trampoline, %function
_signal_trampoline:
	// TODO!
.size _signal_trampoline, .-_signal_trampoline
