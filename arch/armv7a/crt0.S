/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Entrypoint (armv7a)
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

.globl _start
.type _start, %function
_start:
	pop {lr}
	pop {r0-r2}
	bic sp, #7
	b _startc
.size _start, .-_start
