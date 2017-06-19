#include "libphoenix.h"

void *ph_mmap(void *vaddr, size_t size, int prot, int flags, oid_t oid, offs_t offs)
{
	return ph_syscall(SYS_mmap, vaddr, size, prot, flags, oid, offs);
}

int ph_munmap(void *vaddr, size_t size)
{
	return ph_syscall(SYS_munmap, vaddr, size);
}
