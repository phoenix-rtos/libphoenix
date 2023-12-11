/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Entrypoint (sparcv8leon3)
 *
 * Copyright 2022 Phoenix Systems
 * Author: Lukasz Leczkowski
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
	ld [%i0 + 0], %o7
	ld [%i0 + 4], %o0
	ld [%i0 + 8], %o1
	ld [%i0 + 12], %o2
	call _startc
	nop
.size _start, .-_start
