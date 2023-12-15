/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Entrypoint (armv7a)
 *
 * Copyright 2018, 2023 Phoenix Systems
 * Author: Aleksander Kaminski, Hubert Badocha
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
	pop {r0-r3}
	b   _startc
.size _start, .-_start
