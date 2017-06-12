#include "libphoenix.h"

static int isDigit(char c)
{
	if (c < '0')
		return 0;

	if (c > '9') {
		/* to uppercase */
		c &= ~0x20;

		if (c < 'A' || c > 'Z')
			return 0;
	}

	return 1;
}


unsigned int strtoul(char *nptr, char **endptr, int base)
{
	unsigned int t, v = 0;

	if (base == 16 && nptr[0] == '0' && nptr[1] == 'x')
		nptr += 2;

	if (*endptr != NULL)
		*endptr = nptr;

	while (isDigit(*nptr)) {
		t = *nptr - '0';
		if (t > 9) {
			if (base > 10) {
				t = (*nptr | 0x20) - 'a' + 10;
				if (t >= (unsigned int)base)
					break;
			}
			else {
				break;
			}
		}
		else if (t >= (unsigned int)base) {
			break;
		}

		v = (v * base) + t;

		if (*endptr != NULL)
			*endptr = nptr;

		++nptr;
	}

	return v;
}
