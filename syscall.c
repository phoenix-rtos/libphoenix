#include "libphoenix.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic ignored "-Wunused-parameter"
void _ph_syscall_SYS_debug(const char *message)
{
	__asm__ volatile ("int $0x80;"
			  :
			  :"ra" (SYS_debug));
}
#pragma GCC diagnostic pop
