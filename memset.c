#include "libphoenix.h"

void *memset(void *s, int c, size_t n)
{
	size_t i;
	for(i = 0; i < n; ++i) {
		*((u8*)s+i) = c;
	}
	return s;
}
