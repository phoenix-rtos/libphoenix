#include "libphoenix.h"

int ph_send(u32 port, msgop_t op, void *data, size_t size, msgtype_t type, void *rdata, size_t rsize)
{
	return ph_syscall(SYS_send, port, op, data, size, type, rdata, rsize);
}

int ph_recv(u32 port, void *data, size_t size, msgtype_t *type, msgop_t *op, size_t *rsize, unsigned int *sender)
{
	return ph_syscall(SYS_recv, port, data, size, type, op, rsize, sender);
}

int ph_respond(u32 port, int err, void *data, size_t size)
{
	return ph_syscall(SYS_respond, port, err, data, size);
}
