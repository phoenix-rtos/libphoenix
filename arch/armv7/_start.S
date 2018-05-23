/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Entrypoint (armv7)
 *
 * Copyright 2018 Phoenix Systems
 * Author; Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#define __ASSEMBLY__

.text
.thumb

.globl _start
.type _start, %function
_start:
	mov r0, #0
	mov r1, #0
	mov r2, #0

	ldr r3, =~7
	mov r4, sp
	and r3, r3, r4
	mov sp, r3
	bl _startc
.size _start, .-_start
