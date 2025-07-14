/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Signal trampoline (arm)
 *
 * Copyright 2019, 2023 Phoenix Systems
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
	/* Signal number, handler */
	pop {r0, r1}
	blx r1

	pop {r0,r1,r2,r3} /* oldmask, cpu_context_t *, pc, psp */
	/* psr on stack */
	bl sigreturn
.size _signal_trampoline, .-_signal_trampoline
