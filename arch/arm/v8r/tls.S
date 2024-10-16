/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * tls access function
 *
 * Copyright 2023 Phoenix Systems
 * Author: Hubert Badocha
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


 /* NOTE:
 * When libphoenix is compiled as a PIC, even when linked into a static NOPIC binary,
 * compiler generates accesses to TLS as calls to this function.
 * This functions is simple version just for static binaries purposes,
 * in dynamic binaries dynamic linker handles TLS accesses. */


/* typedef struct {
 *   unsigned long ti_moduleid;
 *   unsigned long ti_tlsoffset;
 * } TLS_index;
 *
 * void *__tls_get_addr(TLS_index *ti) */
.thumb
.syntax unified

.text

.thumb_func
/* Declared as weak to be overriden by dynamic linker */
.weak __tls_get_addr;
.type __tls_get_addr, %function;
.align 2
__tls_get_addr:
	/* Access ti_tlsoffset. */
	ldr r0, [r0, #4]
	/* Get thread pointer. */
	mrc p15, 0, r1, cr13, cr0, 3
	/* GCC expects that TLS block has 8 byte TCB pointer at the begging. */
	add r1, r1, #8
	add r0, r0, r1
	bx  lr
.size __tls_get_addr, .-__tls_get_addr
