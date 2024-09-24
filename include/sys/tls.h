/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/tls
 *
 * Copyright 2024 Phoenix Systems
 * Author: Hubert Badocha
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_TLS_H_
#define _LIBPHOENIX_SYS_TLS_H_

#include <sys/types.h>
#include <arch.h>


#if defined(__LIBPHOENIX_ARCH_TLS_VARIANT_I) && defined(__LIBPHOENIX_ARCH_TLS_VARIANT_II)
#error TLS configuration error
#endif

struct tls_tcb {
#ifdef __LIBPHOENIX_ARCH_TLS_VARIANT_I
	void **tcb_dtv;
	void *impldef; /* Implementation defined field. Currently not used in phoenix. */
#elif defined(__LIBPHOENIX_ARCH_TLS_VARIANT_II)
	struct tls_tcb *tcb_self;
	void **tcb_dtv;
#else
	char undef; /* Prevent empty struct */
#endif
};


#ifdef __cplusplus
extern "C" {
#endif


extern void tlsSetPtr(void *tlsPtr);


extern void tlsSetReg(void *tlsReg);


extern void *__tls_tcbPtrToTlsPtr(struct tls_tcb *);


extern void __tls_archInit(void);


extern struct tls_tcb *__tls_getTcb(void);


extern int __tls_alloc(struct tls_tcb **);


extern void __tls_release(void);


extern void _tls_init(void);


#ifdef __cplusplus
}
#endif


#endif
