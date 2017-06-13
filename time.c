#include "libphoenix.h"

time_t ph_gettime(void)
{
	time_t result;
	ph_syscall(SYS_gettime, &result);
	return result;
}

void ph_sleep(time_t timeout)
{
	ph_syscall(SYS_sleep, &timeout);
}
