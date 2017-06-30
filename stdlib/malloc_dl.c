/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stdlib/malloc (Doug Lea)
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/list.h"
#include "sys/threads.h"
#include "sys/mman.h"


typedef struct _malloc_heap_t {
	struct _malloc_heap_t *next;
	struct _malloc_heap_t *prev;
	struct _malloc_chunk_t *chunks;
	size_t freesz;
	size_t size;
} malloc_heap_t;


typedef struct _malloc_chunk_t {
	size_t size;
	struct _malloc_chunk_t *next;
	struct _malloc_chunk_t *prev;
	malloc_heap_t *h;
	u8 data[];
} malloc_chunk_t;


struct {
	malloc_heap_t *freeheaps;
	malloc_heap_t *usedheaps;
	u32 sbinmap;
	u32 lbinmap;
	malloc_chunk_t *sbins[32];
	malloc_chunk_t *lbins[32];

	size_t allocsz;
	size_t freesz;

	handle_t mutex;
} malloc_common;


static inline unsigned int malloc_getlidx(unsigned int s)
{
	unsigned int x = (s >> 8), i, k;

	if (x == 0)
		i = 0;
	else if (x > 0xffff)
		i = 32 - 1;
	else {
		k = (unsigned) sizeof(x) * __CHAR_BIT__ - 1 - (unsigned)__builtin_clz(x);
		i = (unsigned int)((k << 1) + ((s >> (k + (8 - 1)) & 1)));
	}

	return i;
}


static inline unsigned int malloc_getsidx(unsigned int s)
{
	return (s >> 3);
}


malloc_heap_t *_malloc_newheap(size_t size)
{
	malloc_heap_t *h;

printf("befoe mmap\n");
	h = mmap(NULL, max(size, SIZE_PAGE * 2), PROT_WRITE, MAP_ANONYMOUS, NULL, 0);

	return h;
}


void _malloc_split(malloc_chunk_t *chunk)
{
}


void *malloc_allocsmall(unsigned int s)
{
	unsigned int idx = malloc_getsidx(s);
	unsigned int binmap = malloc_common.sbinmap & ~((1 << idx) - 1);
	malloc_chunk_t *ch;
	malloc_heap_t *h;

	/* Find first chunk suitable for size */
	if (binmap)
		idx = __builtin_ctz(binmap);

	lock(malloc_common.mutex);
	if ((ch = malloc_common.sbins[idx]) == NULL) {

printf("cdsdfds\n");

		if ((h = _malloc_newheap(idx << 3)) == NULL) {
			unlock(malloc_common.mutex);
			return NULL;
		}

		LIST_ADD(&malloc_common.freeheaps, h);
		LIST_ADD(&malloc_common.sbins[idx], h->chunks);
		ch = malloc_common.sbins[idx];
		malloc_common.sbinmap |= (1 << idx);
	}

	LIST_REMOVE(&malloc_common.sbins[idx], ch);
	ch->h->freesz -= ((idx << 3) + 2 * sizeof(size_t));
	_malloc_split(ch);
	
	if (ch->h->freesz == 0) {
		LIST_REMOVE(&malloc_common.freeheaps, ch->h);
		LIST_ADD(&malloc_common.usedheaps, ch->h);
		malloc_common.sbinmap &= ~(1 << idx);
	}

	/* Mark chunk allocated */
	ch->size = (idx << 3) | 1;
	*(size_t *)(ch + sizeof(size_t) + (idx << 3)) = (idx << 3);

	unlock(malloc_common.mutex);

	return (ch + sizeof(size_t));
}


void *malloc(size_t size)
{
	/* Allocate small chunk */
	if (malloc_getsidx(size) < 32)
		return malloc_allocsmall(size);

	/* Allocate large chunk */
	return NULL;

}


void _malloc_init(void)
{
	unsigned int i;

	malloc_common.allocsz = 0;
	malloc_common.sbinmap = 0;
	malloc_common.lbinmap = 0;

	for (i = 0; i < 32; i++) {
		malloc_common.sbins[i] = NULL;
		malloc_common.lbins[i] = NULL;
	}

	return;
}
