#include "libphoenix.h"

int strtol(char *nptr, char **endptr, int base)
{
	int sign = 1;

	if (*nptr == '-') {
		sign = -1;
		++nptr;
	}

	return sign * strtoul(nptr, endptr, base);
}
