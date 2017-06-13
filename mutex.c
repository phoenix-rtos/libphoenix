#include "libphoenix.h"

handle_t ph_mutex(void)
{
	return ph_syscall(SYS_mutex);
}

void ph_lock(handle_t mutex)
{
	ph_syscall(SYS_lock, mutex);
}

void ph_unlock(handle_t mutex)
{
	ph_syscall(SYS_unlock, mutex);
}
