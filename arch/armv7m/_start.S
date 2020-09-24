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
.syntax unified

.globl _start
.type _start, %function
_start:
	pop {lr}
	pop {r0-r2}

	ldr r3, =~7
	mov r4, sp
	and r3, r3, r4
	mov sp, r3
	bl _startc
.size _start, .-_start
