/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * wchar functions
 *
 * Copyright 2022 Phoenix Systems
 * Author: Damian Loewnau
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <wchar.h>


int wcscmp(const wchar_t *ws1, const wchar_t *ws2)
{
	wchar_t c1, c2;
	do
	{
	c1 = *ws1++;
	c2 = *ws2++;
		if (c2 == L'\0')
			return c1 - c2;
	}
	while (c1 == c2);
	return c1 < c2 ? -1 : 1;
}

// void uuid_generate(uuid_t out)
// {
// 	int pos = uuid_seed / 256;
// 	for (int i = 0; i < sizeof(uuid_t); i++) {
// 		if (i < pos) {
// 			out[i] = 255;
// 		}
// 		else if (i == pos) {
// 			out[i] = uuid_seed % 256;
// 		}
// 		else {
// 			out[i] = 0;
// 		}
// 	}

// 	uuid_seed++;
// }


// void uuid_unparse(const uuid_t uu, char *out)
// {
// 	unsigned char temp;
// 	for (int i = 0; i < sizeof(uuid_t); i++) {
// 		temp = uu[sizeof(uuid_t) - 1 - i];
// 		if (i == 0) {
// 			sprintf(out, "%x", temp);
// 		}
// 		else if (i == 3 || i == 5 || i == 7 || i == 9) {
// 			sprintf(out + strlen(out), "%x-", temp);
// 		}
// 		else {
// 			sprintf(out + strlen(out), "%x", temp);
// 		}
// 	}
// }
