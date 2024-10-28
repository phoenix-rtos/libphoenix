/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Entrypoint (aarch64)
 *
 * Copyright 2024 Phoenix Systems
 * Author: Jacek Maksymowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#define __ASSEMBLY__

.text

.globl _start
.type _start, %function
_start:
	ldp x0, x1, [sp], #0x10
	ldp x2, x3, [sp], #0x10
	b   _startc
.size _start, .-_start
