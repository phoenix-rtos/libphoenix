/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * test/test_mmap
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/mman.h"


void test_mmap(void)
{
#if 0
	struct {
		void *vaddr;
		size_t size;
	} buff[150];

	for (i = 0; i < 1024; i++) {

size = (unsigned long long)(1 << 16) * rand_r(&seed) / RAND_MAX;
		v = mmap(0, 4096 * 16, 0, 0, NULL, 0);

		if (v == NULL) {
			printf("NULL at %p\n", vaddr);
			for (;;);
		}
		vaddr = v;
	
		memset(vaddr, ((int)vaddr >> 16) & 0xff, 4096);

		printf("\rvaddr=%p, %x", vaddr, ((int)vaddr >> 16) & 0xff);
	}
	printf("\ncheck\n");

	for (i = 0; i < 1024 * 8; i++) {
		for (k = 0; k < 4096 * 16; k++) {
			if (*(u8 *)(vaddr + k) != (((int)vaddr >> 16) & 0xff)) {
				printf("INTERFERENCE!!\n");
				for (;;);
			}			
		}
//		munmap(vaddr, 4096);

		printf("\rvaddr=%p, %x", vaddr, ((int)vaddr >> 16) & 0xff);
		vaddr -= 4096 * 16;
	}
	printf("\n");
#endif
}


int main(void)
{
	printf("test_mmap: Starting, main is at %p\n", main);
	test_mmap();

	for (;;);

	return 0;
}
