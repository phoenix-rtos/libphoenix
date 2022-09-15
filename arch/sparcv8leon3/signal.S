/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Signal trampoline (sparcv8leon3)
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

.globl _signal_trampoline
.type _signal_trampoline, #function
_signal_trampoline:
	/* TODO */
	retl
	nop
.size _signal_trampoline, .-_signal_trampoline
