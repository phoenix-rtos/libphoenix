/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Entrypoint (sparcv8leon3)
 *
 * Copyright 2022, 2023 Phoenix Systems
 * Author: Lukasz Leczkowski, Hubert Badocha
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#define __ASSEMBLY__

.section ".text"
.align 4

.globl _start
.type _start, #function
_start:
	save %sp, -0x60, %sp
	ld [%sp + 0xc0], %o0
	ld [%sp + 0xc4], %o1
	ld [%sp + 0xc8], %o2
	ld [%sp + 0xcc], %o3
	call _startc
	nop
.size _start, .-_start
