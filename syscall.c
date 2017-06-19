#include "libphoenix.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#define SYSCALL_WRAPPER(return_value, name, ...) \
	return_value _ph_syscall_SYS_ ## name (__VA_ARGS__) \
	{						    \
	__asm__ volatile ("int $0x80;"			    \
			  :				    \
			  :"ra" (SYS_ ## name));	    \
	}

SYSCALL_WRAPPER(void, debug, const char *message);
SYSCALL_WRAPPER(void *, mmap, void *vaddr, size_t size, int prot, int flags, oid_t oid, offs_t offs);
SYSCALL_WRAPPER(int, munmap, void *vaddr, size_t size);
SYSCALL_WRAPPER(void, threadCreate, void (*start)(void *), unsigned int priority, void *arg);
SYSCALL_WRAPPER(void, threadExit, void);
SYSCALL_WRAPPER(int, send, u32 port, msgop_t op, void *data, size_t size, msgtype_t type, void *rdata, size_t rsize);
SYSCALL_WRAPPER(int, recv, u32 port, void *data, size_t size, msgtype_t *type, msgop_t *op, size_t *rsize, unsigned int *sender);
SYSCALL_WRAPPER(int, respond, u32 port, int err, void *data, size_t size);
SYSCALL_WRAPPER(handle_t, mutex, void);
SYSCALL_WRAPPER(void, lock, handle_t mutex);
SYSCALL_WRAPPER(void, unlock, handle_t mutex);
SYSCALL_WRAPPER(void, gettime, time_t *time);
SYSCALL_WRAPPER(void, sleep, time_t *timeout);

#pragma GCC diagnostic pop
