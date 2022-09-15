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
	ld [%o5 + 0], %o7
	ld [%o5 + 4], %o0
	ld [%o5 + 8], %o1
	ld [%o5 + 12], %o2
	call _startc
	nop
.size _start, .-_start
