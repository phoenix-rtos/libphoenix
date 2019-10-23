/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/mman
 *
 * Copyright 2019 Phoenix Systems
 * Author: Andrzej Glowinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/mman.h>
#include <errno.h>

extern int memMap(void **vaddr, size_t size, int prot, int flags, int fd, off_t offs);
extern int memUnmap(void *vaddr, size_t size);


void *mmap(void *vaddr, size_t size, int prot, int flags, int fd, off_t offs)
{
	int err = memMap(&vaddr, size, prot, flags, fd, offs);

	if (err != EOK) {
		errno = err;
		return (void *) -1;
	}

	return vaddr;
}

int munmap(void *vaddr, size_t size)
{
	return SET_ERRNO(memUnmap(vaddr, size));
}
