/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Signal trampoline (arm-imx)
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
	// stack: return address, signal number
	push {r0-r3,r9,r12,lr}
	mrs r0, apsr
	push {r0}

	ldr r0, [sp, #36] // signal number
	ldr r1, [sp, #32] // signal handler
	blx r1

	pop {r0}
	msr apsr_nzcvqg, r0
	pop {r0-r3,r9,r12,lr}
	add sp, #8
	pop {pc}
.size _signal_trampoline, .-_signal_trampoline
