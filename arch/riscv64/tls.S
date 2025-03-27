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

/* Declared as weak to be overwritten by dynamic linker */
.weak __tls_get_addr
.type __tls_get_addr, %function
__tls_get_addr:
	/* Access ti_tlsoffset. */
	ld   a0, 8(a0)
	/* On RISC-V Dynamic Thread Vector pointer points at TLS block start + 0x800  */
	li   t0, 0x800
	add  a0, a0, t0
	add  a0, a0, tp
	ret
.size __tls_get_addr, .-__tls_get_addr
