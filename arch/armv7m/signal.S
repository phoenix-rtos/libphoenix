/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Signal trampoline (armv7)
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
	/* Signal number, cpu_context * */
	pop {r0,r4}
	blx _signal_handler

	/* Old signal mask in r0 */
	mov r1, r4
	pop {r2,r3} /* pc, sp */
	/* psr on stack */
	bl sigreturn
.size _signal_trampoline, .-_signal_trampoline
