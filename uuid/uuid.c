/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * uuid functions
 *
 * Copyright 2022 Phoenix Systems
 * Author: Damian Loewnau
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <uuid/uuid.h>


unsigned long long uuid_seed = 0;


void uuid_generate(uuid_t out)
{
	int pos = uuid_seed / 256;
	for (int i = 0; i < sizeof(uuid_t); i++) {
		if (i < pos) {
			out[i] = 255;
		}
		else if (i == pos) {
			out[i] = uuid_seed % 256;
		}
		else {
			out[i] = 0;
		}
	}

	uuid_seed++;
}


void uuid_unparse(const uuid_t uu, char *out)
{
	unsigned char temp;
	for (int i = 0; i < sizeof(uuid_t); i++) {
		temp = uu[sizeof(uuid_t) - 1 - i];
		if (i == 0) {
			sprintf(out, "%x", temp);
		}
		else if (i == 3 || i == 5 || i == 7 || i == 9) {
			sprintf(out + strlen(out), "%x-", temp);
		}
		else {
			sprintf(out + strlen(out), "%x", temp);
		}
	}
}
