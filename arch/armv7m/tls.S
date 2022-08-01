/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * tls.S
 *
 * Copyright 2022 Phoenix Systems
 * Author: Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#define __ASSEMBLY__

.thumb
.syntax unified

.align 2
.thumb_func
.globl __aeabi_read_tp
.type __aeabi_read_tp,%function
/* __aeabi_read_tp must not clobber any registers except r0 */
__aeabi_read_tp:
	push {r1-r3, lr}
	blx aeabi_read_tp_internal
	pop {r1-r3, lr}
	bx lr
.size __aeabi_read_tp, .-__aeabi_read_tp
