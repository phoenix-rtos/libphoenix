/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Entrypoint (RISCV64)
 *
 * Copyright 2018 Phoenix Systems
 * Author: Michal Miroslaw, Pawel Pisarczyk
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
	ld ra, (sp)
	ld a0, 8(sp)
	ld a1, 16(sp)
	ld a2, 24(sp)
	addi sp, sp, 32
	j _startc
.size _start, .-_start
