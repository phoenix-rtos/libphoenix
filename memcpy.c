#include "libphoenix.h"

void *memcpy(void *dest, const void *src, size_t n)
{
	size_t i;
	for(i = 0; i < n; ++i) {
		*((u8*)dest+i) = *((u8*)src+i);
	}
	return dest;
}
