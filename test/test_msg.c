/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * test/test_msg
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"
#include "sys/mman.h"
#include "sys/threads.h"
#include "sys/msg.h"


unsigned test_randsize(unsigned *seed, unsigned bufsz)
{
	unsigned sz;

	if (rand_r(seed) % 2)
		sz = (rand_r(seed) % (bufsz / SIZE_PAGE)) * SIZE_PAGE;
	else
		sz = 1 + (rand_r(seed) % bufsz);

	return sz ? sz : bufsz;
}


unsigned test_offset(unsigned *seed, unsigned size, unsigned bufsz)
{
	unsigned offs = (bufsz - size) / SIZE_PAGE;

	if (offs && rand_r(seed) % 2)
		offs = (rand_r(seed) % offs) * SIZE_PAGE;
	else if (offs && rand_r(seed) % 10)
		offs = SIZE_PAGE - (size & (SIZE_PAGE - 1));
	else if (offs && rand_r(seed) % 10)
		offs = SIZE_PAGE - (size & (SIZE_PAGE - 1)) / 2;
	else
		offs = rand_r(seed) % (bufsz - size);

	return offs;
}


int test_ping(unsigned seed, unsigned port, unsigned count)
{
	msg_t msg;
	unsigned bufsz = 4 * SIZE_PAGE, offs[2], i, k;
	void *buf[2];

	printf("test_msg/ping: starting\n");

	buf[0] = mmap(NULL, bufsz, 0, 0, NULL, 0);
	buf[1] = mmap(NULL, bufsz, 0, 0, NULL, 0);

	if (buf[0] == NULL || buf[1] == NULL) {
		printf("test_msg/ping: could not allocate buffers\n");
		return 1;
	}

	for (k = 0; !count || k < count; ++k) {
		printf("\rtest_msg/ping: % 20d OK", k);

		memset(&msg, 0, sizeof(msg));

		msg.o.size = msg.i.size = test_randsize(&seed, bufsz);

		msg.i.data = buf[0] + (offs[0] = test_offset(&seed, msg.i.size, bufsz));
		msg.o.data = buf[1] + (offs[1] = test_offset(&seed, msg.o.size, bufsz));

		for (i = 0; i < msg.o.size; ++i)
			((unsigned char *)msg.i.data)[i] = (unsigned char)rand_r(&seed);

		if (msgSend(port, &msg) < 0) {
			printf("\ntest_msg/ping: send failed\n");
			return 1;
		}

		if (msg.o.io.err < 0) {
			printf("\ntest_msg/ping: pong returned error\n");
			return 1;
		}

		if (msg.i.size != msg.o.size) {
			printf("\ntest_msg/ping: sizes mismatch\n");
			return 1;
		}

		if (memcmp(msg.o.data, msg.i.data, msg.i.size)) {
			printf("\ntest_msg/ping: data mismatch\n");

			for (i = 0; i < msg.i.size; ++i) {
				printf("%02x", ((unsigned char *)msg.i.data)[i]);
			}

			printf("\n");

			for (i = 0; i < msg.o.size; ++i) {
				printf("%02x", ((unsigned char *)msg.o.data)[i]);
			}

			printf("\n");

			return 1;
		}
	}

	return 0;
}


int test_pong(unsigned port)
{
	msg_t msg;
	unsigned rid;

	printf("test_msg/pong: starting\n");

	for (;;) {
		if (msgRecv(port, &msg, &rid) < 0) {
			printf("test_msg/pong: receive failed\n");
			msg.o.io.err = 1;
		}

		if (msg.i.size != msg.o.size) {
			printf("test_msg/pong: i/o buffers are of different sizes: 0x%x and 0x%x\n", msg.i.size, msg.o.size);
			msg.o.io.err = 1;
		}
		else
			memcpy(msg.o.data, msg.i.data, msg.i.size);

		msgRespond(port, &msg, rid);
	}

	return 0;
}


int main(void)
{
	oid_t oid;
	char portname[] = "/test/msg";

	/* Wait for console */
	while (write(1, "", 0) < 0)
		usleep(10000);

	/* Wait for filesystem */
	while (lookup("/", &oid) < 0)
		usleep(10000);

	if (lookup(portname, &oid) < 0) {
		portCreate(&oid.port);

		if (portRegister(oid.port, portname, &oid) < 0) {
			printf("test_msg/pong: can't register port %s!\n", portname);
			return 1;
		}

		return test_pong(oid.port);
	}

	return test_ping(123, oid.port, 0);
}
