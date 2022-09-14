/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stdlib/div.c
 *
 * Copyright 2022 Phoenix Systems
 * Author: Dawid Szpejna
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdlib.h>


div_t div(int num, int den)
{
	div_t result = { .quot = num / den, .rem = num % den };

	/* Correcting if the result is wrong */
	if (num >= 0 && result.rem < 0) {
		result.quot++;
		result.rem -= den;
	}
	else if (num < 0 && result.rem > 0) {
		result.quot--;
		result.rem += den;
	}

	return result;
}


ldiv_t ldiv(long int num, long int den)
{
	ldiv_t result = { .quot = num / den, .rem = num % den };

	/* Correcting if the result is wrong */
	if (num >= 0 && result.rem < 0) {
		result.quot++;
		result.rem -= den;
	}
	else if (num < 0 && result.rem > 0) {
		result.quot--;
		result.rem += den;
	}

	return result;
}


lldiv_t lldiv(long long int num, long long int den)
{
	lldiv_t result = { .quot = num / den, .rem = num % den };

	/* Correcting if the result is wrong */
	if (num >= 0 && result.rem < 0) {
		result.quot++;
		result.rem -= den;
	}

	return result;
}
