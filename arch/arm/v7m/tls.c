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


volatile uintptr_t arm_tls_ptr __attribute__((section("armtls"))) = 0;


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
