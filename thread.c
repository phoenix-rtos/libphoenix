#include "libphoenix.h"

void ph_beginthread(void (*start)(void *), unsigned int priority, void *arg)
{
	return ph_syscall(SYS_threadCreate, start, priority, arg);
}

void ph_exitthread(void)
{
	ph_syscall(SYS_threadExit);
}
