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

/* TODO: add implementations for remaining wchar functions */
#include <wchar.h>


int wcscmp(const wchar_t *ws1, const wchar_t *ws2)
{
	int i, ret = 1;

	for (i = 0; ws1[i] == ws2[i]; i++) {
		if (ws1[i] == L'\0') {
			ret = 0;
			break;
		}
	}

	if (ret != 0) {
		if (ws1[i] > ws2[i]) {
			ret = 1;
		}
		else {
			ret = -1;
		}
	}

	return ret;
}
