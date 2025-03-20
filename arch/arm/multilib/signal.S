/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Signal trampoline (arm)
 *
 * Copyright 2025 Phoenix Systems
 * Author: Hubert Badocha
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
	bl abort
.size _signal_trampoline, .-_signal_trampoline
