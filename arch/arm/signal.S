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
	/* Signal number, oldmask - keep stack aligned to 8 */
	pop {r0,r4}
	blx _signal_handler

	mov r0, r4        /* move oldmask to r0 */
	pop {r1,r2,r3,r4} /* cpu_context_t *, pc, psp, psr */

	/* put psr back on stack, keep aligned to 8 (r5 as padding value) */
	push {r4,r5}
	bl sigreturn
.size _signal_trampoline, .-_signal_trampoline
