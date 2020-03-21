/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * test/test_mmap
 *
 * Copyright 2020 Phoenix Systems
 * Author: Andrzej Głowinski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <arch.h>
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"
#include "sys/mman.h"
#include "sys/threads.h"

#include <stdio.h>
#include <string.h>
#include "test_common.h"

#define SEED 1581072278
#define BUF_LEN 4000
#define BYTES_COPIED 256
#define OFFSET_VARIANCE 4
#define SIZE_VARIANCE 4
#define BUF_DUMP_WIDTH 20

char pattern[BUF_LEN];
char buffer[BUF_LEN] __attribute__ ((aligned (4)));


/* print bytes from buffer in hexadecimal form */
void print_buffer(void *buffer, int n)
{
	int i;
	for (i = 0; i < n; i++) {
		if ((i % (BUF_DUMP_WIDTH)) == 0)
			printf("\nbytes %d - %d\n", i, i + BUF_DUMP_WIDTH - 1);
		printf("%02X ", *((unsigned char *)buffer + i));
	}
	printf("\n\n");
}


void fill_random(char *buffer, int len)
{
	int i;
	srand(SEED);
	for (i = 0; i < len; i++) {
		buffer[i] = (char)rand();
	}
}


/* loop over buffer and check every byte - copied as well as non-modified ones */
int memmove_check_result(char *buffer, char *pattern, int src_off, int dst_off, int len)
{
	char current_byte;
	int i, err, incorrect_bytes = 0, in_dst;

	for (i = 0; i < BUF_LEN; i++) {
		err = 0;
		current_byte = *(buffer + i);
		/* check if i is in destination buffer */
		in_dst = (i >= dst_off && i < dst_off + len);

		if (!in_dst)
			err = (current_byte != *(pattern + i));
		else
			err = (current_byte != *(pattern + src_off + i - dst_off));

		if (err && verbose_test())
			printf("Incorrect byte nr: %d value: %02X\n", i, (unsigned char)current_byte);

		incorrect_bytes += err;
	}
	return incorrect_bytes;
}


void copy_buffer(char *buffer, char *pattern, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		*(buffer + i) = *(pattern + i);
	}
}


/*
 * Perform memmove when src and dst are pointers inside one continous buffer.
 * In loop add offset to ptrs in aligned buffer to simulate misaligned data
 * Check if data was copied properly and if bytes around weren't violeted
 */
int memmove_check_var_off(char *buffer, char *pattern, int off_src, int off_dst,  int len)
{
	int result;
	void *ptr_dst, *ptr_src;
	int off_src_shifted, off_dst_shifted, delta_off_src, delta_off_dst;

	for (delta_off_src = 0; delta_off_src < OFFSET_VARIANCE; delta_off_src++) {
		for (delta_off_dst = 0; delta_off_dst < OFFSET_VARIANCE; delta_off_dst++) {
			off_src_shifted = off_src + delta_off_src;
			off_dst_shifted = off_dst + delta_off_dst;
			ptr_dst = buffer + off_dst_shifted;
			ptr_src = buffer + off_src_shifted;

			copy_buffer(buffer, pattern, BUF_LEN);
			memmove(ptr_dst,  ptr_src, len);
			result = memmove_check_result(buffer, pattern, off_src_shifted, off_dst_shifted, len);

			if (!result)
				continue;

			/* print failed test details */
			printf("\nIncorrect bytes: %d \n", result);
			printf("len %d off_src %d delta_off_src %d off_dst %d delta_off_dst %d\n\n",
				len, off_src, delta_off_src, off_dst, delta_off_dst);

			if (verbose_test()) {
				printf("Dumping output buffer\n");
				print_buffer(buffer, BUF_LEN);
			}

			printf("Test failed - fatal. Ending test\n");
			exit(-1);
		}
	}
	return 0;
}


/*
 * Test for one buffer when dst and src segments are not overlapping.
 * Add variation to nr of bytes copied to simulate ending on aligned/misaligned segment
 */
void test_memmove_non_overlapping(void)
{
	printf("\nStarting %s\n", __func__);
	int delta_size;

	for (delta_size = 0; delta_size < SIZE_VARIANCE; delta_size++) {
		/* test memmove when source buffer is before destination, without overlapping */
		memmove_check_var_off(buffer, pattern, 0, 2000, BYTES_COPIED - delta_size);
		/* test memmove when source buffer is after destination, without overlapping */
		memmove_check_var_off(buffer, pattern, 2000, 0, BYTES_COPIED - delta_size);
	}

	printf("Test OK\n");
}


/*
 * Test for one buffer and dst and src segments are overlapping
 * Use variable nr of bytes copied to simulate ending on aligned/misaligned segment
 */
void test_memmove_overlapping(void)
{
	printf("\nStarting %s\n", __func__);
	int delta_size;

	for (delta_size = 0; delta_size < SIZE_VARIANCE; delta_size++) {
		/* test memmove when source buffer is before destination, and they are overlapping */
		memmove_check_var_off(buffer, pattern, 100, 200, BYTES_COPIED - delta_size);
		/* test memmove when source buffer is after destination, and thay are overlapping */
		memmove_check_var_off(buffer, pattern, 200, 100, BYTES_COPIED - delta_size);
	}

	printf("Test OK\n");
}


/* Other tests */
void test_memmove_special_cases(void)
{
	printf("\nStarting %s\n", __func__);
	int failed = 0, incorrect_bytes, i;

	/* Special case: copying less than a word (including 0 bytes) */
	for (i = 0; i < 4; i++) {
		copy_buffer(buffer, pattern, BUF_LEN);
		memmove(buffer, buffer + BUF_LEN / 2, i);
		incorrect_bytes = memmove_check_result(buffer, buffer, 0, BUF_LEN / 2, i);
		if (incorrect_bytes) {
			printf("Special case for %d len failed\n", i);
			failed++;
		}
	}

	/* Special case: dst is the same as src */
	copy_buffer(buffer, pattern, BUF_LEN);
	memmove(buffer, buffer, BYTES_COPIED);
	incorrect_bytes = memmove_check_result(buffer, pattern, 0, BUF_LEN / 2, 0);
	if (incorrect_bytes) {
		printf("Special case for dst same as src len failed\n");
		failed++;
	}

	printf("Assertions failed: %d\n", failed);
}


int main(void)
{
	printf("MEMMOVE TEST STARTED\n");
	save_env();

	fill_random(pattern, BUF_LEN);

	test_memmove_non_overlapping();
	test_memmove_overlapping();
	test_memmove_special_cases();
	return 0;
}
