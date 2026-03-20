/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/msg
 *
 * Copyright 2026 Phoenix Systems
 * Author: Julian Uziembło
 *
 * %LICENSE%
 */

#include "../common/util.h"


extern int sys_portRegister(uint32_t port, const char *name, size_t len, oid_t *oid);

extern int sys_portUnregister(const char *name, size_t len);


int portRegister(uint32_t port, const char *name, oid_t *oid)
{
	return sys_portRegister(port, name, __strlenNull(name), oid);
}


int portUnregister(const char *name)
{
	return sys_portUnregister(name, __strlenNull(name));
}
