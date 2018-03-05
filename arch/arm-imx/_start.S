/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Entrypoint (arm-imx)
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
	pop {r0}
	pop {r1}
	pop {r2}
	sub sp, #7
	and sp, #~7
	b _startc
.size _start, .-_start
