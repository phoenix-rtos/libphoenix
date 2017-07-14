#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "/usr/include/errno.h"

#if 1
#define printf(x, ...)
#define putchar(x)
#endif

/* #include "libphoenix.h" */


int main(int argc, char *argv[]) {
	int i, ops, count, size, max_size;
	unsigned char **bufs;
	
	if (argc > 1)
		count = strtol(argv[1], NULL, 0);
	else
		count = 33;

	if (argc > 2)
		max_size = strtol(argv[2], NULL, 0);
	else
		max_size = 65;

	_malloc_init();
	bufs = malloc(count * sizeof(char *));
	if (!bufs) {
		printf("Out of memory(%s).\n", strerror(errno));
		exit(1);
	}
	for (i = 0, ops = 0; i < count; ++i, ++ops) {
		size = 0x20 * i + 0x50;
		if (size > max_size)
			size = max_size;
		     
		bufs[i] = malloc(size);
		memset(bufs[i], 0xff, size);
		putchar('.');
	}

	printf("%d buffers allocated\n", ops);
	
	for (i = 16, ops = 0; i < count; ++i, ++ops) {
		free(bufs[i]);
		putchar(',');
	}

	printf("%d buffers freed\n", ops);

	for (i = 16, ops = 0; i < count; ++i, ++ops) {
		size = 0x20 * i + 0x50;
		if (size > max_size)
			size = max_size;
		     
		bufs[i] = calloc(size / 4, 2);
		memset(bufs[i], 0xff, (size / 4) * 2);
		putchar('.');
	}

	printf("%d buffers allocated\n", ops);
	
	for (i = 0, ops = 0; i < count; i += 2, ++ops) {
		size = 0x20 * i + 0x90;
		if (size > max_size)
			size = max_size;
		     
		bufs[i] = realloc(bufs[i], size);
		memset(bufs[i], 0xff, size);
		putchar('.');
	}

	printf("%d buffers reallocated\n", ops);

	for (i = 0, ops = 0; i < count / 2; ++i, ++ops) {
		free(bufs[i]);
		putchar(',');
	}

	printf("%d buffers freed\n", ops);

	for (i = 0, ops = 0; i < count / 2; ++i, ++ops) {
		size = 0x11 * i + 0x20;
		if (size > max_size)
			size = max_size;
		     
		bufs[i] = malloc(size);
		memset(bufs[i], 0xff, size);
		putchar('.');
	}

	printf("%d buffers allocated\n", ops);
	
	for (i = 0, ops = 0; i < count; ++i, ++ops) {
		free(bufs[i]);
#ifdef USE_AFTER_FREE
		memset(bufs[i], 0xff, size);
#endif
		putchar(',');
	}

	printf("%d buffers freed\n", ops);

	free(bufs);
	
	return 0;
}
