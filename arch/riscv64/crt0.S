/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Entrypoint (RISCV64)
 *
 * Copyright 2018, 2023 Phoenix Systems
 * Author: Michal Miroslaw, Pawel Pisarczyk, Hubert Badocha
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
	la   gp, __global_pointer$
.option pop
	ld   a0,   (sp)
	ld   a1,  8(sp)
	ld   a2, 16(sp)
	ld   a3, 24(sp)
	addi sp, sp, 32
	j    _startc
.size _start, .-_start
