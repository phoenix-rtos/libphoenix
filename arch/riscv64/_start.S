/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Entrypoint (RISCV64)
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michał Mirosław, Pawel Pisarczyk
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

.option push
.option norelax
	la gp, __global_pointer$
.option pop

	j _startc

.size _start, .-_start
