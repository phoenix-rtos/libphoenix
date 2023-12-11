/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * arch/armv7m
 *
 * Copyright 2017 Phoenix Systems
 * Author: Aleksander Kaminski, Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#include <stddef.h>


void *memcpy(void *dst, const void *src, size_t l)
{
	__asm__ volatile
	(" \
		mov r1, %2; \
		mov r3, %1; \
		mov r4, %0; \
		orr r2, r3, r4; \
		ands r2, #3; \
		bne 2f; \
	1: \
		cmp r1, #4; \
		ittt hs; \
		ldrhs r2, [r3], #4; \
		strhs r2, [r4], #4; \
		subshs r1, #4; \
		bhs 1b; \
	2: \
		cmp r1, #0; \
		ittt ne; \
		ldrbne r2, [r3], #1; \
		strbne r2, [r4], #1; \
		subsne r1, #1; \
		bne 2b"
	:
	: "r" (dst), "r" (src), "r" (l)
	: "r1", "r2", "r3", "r4", "memory", "cc");

	return dst;
}


void *memmove(void *dst, const void *src, size_t l)
{
	__asm__ volatile
	(" \
		mov r1, %2; \
		mov r3, %1; \
		mov r4, %0; \
		orr r2, r3, r4; \
		cmp %0, %1; \
		bhs 3f; \
		ands r2, #3; \
		bne 2f; \
	1: \
		cmp r1, #4; \
		ittt hs; \
		ldrhs r2, [r3], #4; \
		strhs r2, [r4], #4; \
		subshs r1, #4; \
		bhs 1b; \
	2: \
		cmp r1, #0; \
		ittt ne; \
		ldrbne r2, [r3], #1; \
		strbne r2, [r4], #1; \
		subsne r1, #1; \
		bne 2b; \
		b 8f; \
	3: \
		ands r2, #3; \
		bne 6f; \
		cmp r1, #4; \
		blo 6f; \
	4: \
		subs r1, #4; \
		blo 5f; \
		ldr r2, [r3, r1]; \
		str r2, [r4, r1]; \
		b 4b; \
	5: \
		add r1, #4; \
	6: \
		cmp r1, #0; \
		beq 8f; \
	7: \
		subs r1, #1; \
		blo 8f; \
		ldrb r2, [r3, r1]; \
		strb r2, [r4, r1]; \
		b 7b; \
	8:"
	:
	: "r" (dst), "r" (src), "r" (l)
	: "r1", "r2", "r3", "r4", "memory", "cc");

	return dst;
}


int memcmp(const void *ptr1, const void *ptr2, size_t num)
{
	int res = 0;

	__asm__ volatile
	(" \
		mov r1, %1; \
		mov r2, %2; \
		mov r3, %3; \
	1: \
		cmp r3, #0; \
		beq 3f; \
		sub r3, #1; \
		ldrb r4, [r1], #1; \
		ldrb r5, [r2], #1; \
		cmp r4, r5; \
		beq 1b; \
		blo 2f; \
		mov %0, #1; \
		b 3f; \
	2: \
		mov %0, #-1; \
	3: "
	: "+r" (res)
	: "r" (ptr1), "r" (ptr2), "r" (num)
	: "r1", "r2", "r3", "r4", "r5", "cc");

	return res;
}


void *memset(void *dst, int v, size_t l)
{
	__asm__ volatile
	(" \
		mov r1, %2; \
		mov r3, %1; \
		orr r3, r3, r3, lsl #8; \
		orr r3, r3, r3, lsl #16; \
		mov r4, %0; \
		ands r2, r4, #3; \
		bne 2f; \
	1: \
		cmp r1, #4; \
		itt hs; \
		strhs r3, [r4], #4; \
		subshs r1, #4; \
		bhs 1b; \
	2: \
		cmp r1, #0; \
		itt ne; \
		strbne r3, [r4], #1; \
		subsne r1, #1; \
		bne 2b"
	:
	: "r" (dst), "r" (v & 0xff), "r" (l)
	: "r1", "r2", "r3", "r4", "memory", "cc");

	return dst;
}


size_t strlen(const char *s)
{
	unsigned int k = 0;

	__asm__ volatile
	(" \
	1: \
		ldrb r1, [%1, %0]; \
		add %0, #1; \
		cmp r1, #0; \
		bne 1b; \
		sub %0, #1;"
	: "+r" (k)
	: "r" (s)
	: "r1", "cc");

	return k;
}


size_t strnlen(const char *s, size_t maxlen)
{
	unsigned int k = 0;

	__asm__ volatile
	(" \
	1: \
		cmp %0, %2; \
		bhs 2f; \
		ldrb r1, [%1, %0]; \
		add %0, #1; \
		cmp r1, #0; \
		bne 1b; \
		sub %0, #1; \
	2:"
	: "+r" (k)
	: "r" (s), "r" (maxlen)
	: "r1", "cc");

	return k;
}


int strcmp(const char *s1, const char *s2)
{
	int res = 0;

	__asm__ volatile
	(" \
		mov r1, %1; \
		mov r2, %2; \
	1: \
		ldrb r3, [r1], #1; \
		ldrb r4, [r2], #1; \
		cmp r3, #0; \
		beq 2f; \
		cmp r3, r4; \
		beq 1b; \
		blo 3f; \
		mov %0, #1; \
		b 4f; \
	2: \
		cmp r4, #0; \
		beq 4f; \
	3: \
		mov %0, #-1; \
	4: "
	: "+r" (res)
	: "r" (s1), "r" (s2)
	: "r1", "r2", "r3", "r4", "cc");

	return res;
}


int strncmp(const char *s1, const char *s2, size_t count)
{
	int res = 0;

	__asm__ volatile
	(" \
		mov r1, %1; \
		mov r2, %2; \
		mov r5, %3; \
	1: \
		cmp r5, #0; \
		beq 4f; \
		sub r5, #1; \
		ldrb r3, [r1], #1; \
		ldrb r4, [r2], #1; \
		cmp r3, #0; \
		beq 2f; \
		cmp r3, r4; \
		beq 1b; \
		blo 3f; \
		mov %0, #1; \
		b 4f; \
	2: \
		cmp r4, #0; \
		beq 4f; \
	3: \
		mov %0, #-1; \
	4: "
	: "+r" (res)
	: "r" (s1), "r" (s2), "r" (count)
	: "r1", "r2", "r3", "r4", "r5", "cc");

	return res;
}


char *strcpy(char *dest, const char *src)
{
	__asm__ volatile
	(" \
		mov r2, %0; \
		mov r3, %1; \
		ldrb r1, [r3], #1; \
	1: \
		strb r1, [r2], #1; \
		cmp r1, #0; \
		itt ne; \
		ldrbne r1, [r3], #1; \
		bne 1b"
	:
	: "r" (dest), "r" (src)
	: "r1", "r2", "r3", "memory", "cc");

	return dest;
}


char *strncpy(char *dest, const char *src, size_t n)
{
	__asm__ volatile
	(" \
		mov r2, %2; \
		mov r3, %0; \
		mov r4, %1; \
		ldrb r1, [r4], #1; \
	1: \
		cmp r2, #0; \
		beq 3f; \
		sub r2, #1; \
		strb r1, [r3], #1; \
		cmp r1, #0; \
		itt ne; \
		ldrbne r1, [r4], #1; \
		bne 1b; \
		mov r1, #0; \
	2: \
		cmp r2, #0; \
		beq 3f; \
		sub r2, #1; \
		strb r1, [r3], #1; \
		b 2b; \
	3:"
	:
	: "r" (dest), "r" (src), "r" (n)
	: "r1", "r2", "r3", "r4", "memory", "cc");

	return dest;
}
