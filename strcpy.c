#include "libphoenix.h"

char *strcpy(char *dest, const char *src)
{
	int i = 0;

	do {
		dest[i] = src[i];
	} while(src[i++] != '\0');

	return dest;
}
