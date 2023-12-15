/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Entrypoint (ia32)
 *
 * Copyright 2018, 2023 Phoenix Systems
 * Author: Michal Miroslaw, Hubert Badocha
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
	/* Call to ensure proper alignment. As %esp is 16 aligned at program entry. */
	call _startc
.size _start, .-_start
