/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * test/test_scanf
 *
 * Copyright 2017 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "stdlib.h"
#include "stdio.h"
#include "string.h"


static inline void do_test(int success, int ix, int *failed) {
	if (!success) {
		(*failed)++;
		printf("test_scanf: test %d failed\n", ix);
	}
}


int test_scanf(void)
{
	int failed = 0, ix = 0, n;

	char c;
	int d;
	long ld;
	long long lld;

	unsigned u, o, x, x1, x2;
	unsigned long lu, lx;

	short hd;
	void* p;
	char s[8];
	const char baddata[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0};

	do_test(sscanf("0x1234babe", "%p", &p) == 1 && p == (void*)0x1234babe, ix++, &failed);
	do_test(sscanf("1234FACE", "%p", &p) == 1 && p == (void*)0x1234face, ix++, &failed);
	do_test(sscanf("abc", "%c", &c) == 1 && c == 'a', ix++, &failed);
	do_test(sscanf("12 f 42 thrash", "%d %c %ld", &d, &c, &ld) == 3 && d == 12 && c == 'f' && ld == 42, ix++, &failed);
	do_test(sscanf("a", "%c", &c) == 1 && c == 'a', ix++, &failed);
	do_test(sscanf("1234565432100", "%lld", &lld) == 1 && lld == (long long)1234565432100, ix++, &failed);
	do_test(sscanf("123456789000 -123456789000", "%lu %ld", &lu, &ld) == 2 && lu == (unsigned long)123456789000 && ld == (long)-123456789000, ix++, &failed);
	do_test(sscanf("0xbeefbabe dum bam 01234567 bom", "%x %s %*s %o", &x, s, &o) == 3 && x == 0xbeefbabe && strcmp(s, "dum") == 0 && o == 01234567, ix++, &failed);
	do_test(sscanf("0x1234face 51242 bla!bla0Xab1500 bla 12345", "%lx %n %d bla%cbla%p bla %s", &lx, &n, &d, &c, &p, s) == 5 && lx == 0x1234face && n == 11 && d == 51242 && c == '!' && p == (void*)0xab1500 && strcmp(s, "12345") == 0, ix++, &failed);
	do_test(sscanf("5534-1234--12345654321", "%hd-%u-%lld", &hd, &u, &lld) == 3 && hd == 5534 && u == 1234 && lld == (long long)-12345654321, ix++, &failed);
	do_test(sscanf("+123 126123124123412431826481263812641263812674182736182746 555", "%d %*d %u", &d, &u) == 2 && d == 123 && u == 555, ix++, &failed);
	do_test(sscanf("babababababababababa!", "%*[ba]%c", &c) == 1 && c == '!', ix++, &failed);
	do_test(sscanf("babababababababababa", "%*10[ba]%c", &c) == 1 && c == 'b', ix++, &failed);
	do_test(sscanf("babababababababababa", "%*9[ba]%c", &c) == 1 && c == 'a', ix++, &failed);
	do_test(sscanf("ASDBGIiufhsiu!asd", "%*[^!]%c", &c) == 1 && c == '!', ix++, &failed);
	do_test(sscanf("abcbdbabcebfbg?das", "%*[a-w]%c", &c) == 1 && c == '?', ix++, &failed);
	do_test(sscanf("deadbeef12345678deadc0de", "%8x%8x%8x", &x, &x1, &x2) == 3 && x == 0xdeadbeef && x1 == 0x12345678 && x2 == 0xdeadc0de, ix++, &failed);

	do_test(sscanf("deadbeef 12345678 Zeadc0de", "%8x %8x %8x", &x, &x1, &x2) == 2 && x == 0xdeadbeef && x1 == 0x12345678, ix++, &failed);
	do_test(sscanf("12345678a1234567", "%8d%8u", &d, &u) == 1 && d == 12345678, ix++, &failed);

	do_test(sscanf("!@%SFDS@#$", "%8d", &d) == 0, ix++, &failed);
	do_test(sscanf("$#&$%&%$@#^#$^&%$^@#$^&%$^#$%@#%#$^&%*^&*(^&&%$^#%$^&$%S@#$", "%d", &d) == 0, ix++, &failed);
	do_test(sscanf("ąęść≠€ß¢½", "%d", &d) == 0, ix++, &failed);
	do_test(sscanf("·½§²€³€¢³", "%d", &d) == 0, ix++, &failed);
	do_test(sscanf(baddata, "%d", &d) == 0, ix++, &failed);

	return failed;
}


int main(void)
{
	int failed;

	failed = test_scanf();
	printf("test_scanf: ");

	if (failed)
		printf("%d tests failed\n", failed);
	else
		printf("all tests passed\n");

	for (;;);
}
