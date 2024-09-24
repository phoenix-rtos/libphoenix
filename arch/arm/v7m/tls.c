/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * tls.c
 *
 * Copyright 2022 Phoenix Systems
 * Author: Lukasz Leczkowski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#include <sys/types.h>
#include <sys/tls.h>


volatile uintptr_t arm_tls_ptr = 0;


void *read_tls_ptr(void)
{
	return (void *)arm_tls_ptr;
}


void __attribute__((naked)) __aeabi_read_tp(void)
{
	__asm__ volatile(
		"push {r1-r4,r12,lr};"
		"bl read_tls_ptr;"
		"pop {r1-r4,r12,pc}");
}


struct tls_tcb *__tls_getTcb(void)
{
	return (struct tls_tcb *)read_tls_ptr();
}


void *__tls_tcbPtrToTlsPtr(struct tls_tcb *tcb)
{
	return (void *)tcb;
}


void __tls_archInit(void)
{
	tlsSetReg((void *)&arm_tls_ptr);
}
