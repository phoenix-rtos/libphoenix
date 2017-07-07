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

#include ARCH
#include "stdio.h"
#include "stdlib.h"
#include "sys/list.h"
#include "sys/threads.h"
#include "sys/mman.h"


#define CHUNK_USED      1


typedef struct _heap_t {
	size_t freesz;
	size_t size;
	u8 space[];
} heap_t;


typedef struct _chunk_t {
	size_t prevSize;
	size_t size;
	heap_t *heap;
	struct _chunk_t *next;
	struct _chunk_t *prev;
} chunk_t;


struct {
	u32 sbinmap;
	u32 lbinmap;
	chunk_t *sbins[32];
	chunk_t *lbins[32];

	size_t allocsz;
	size_t freesz;

	handle_t mutex;
} malloc_common;


static inline unsigned int malloc_getsidx(size_t size)
{
	return (size + 7) >> 3;
}


static inline unsigned int malloc_getlidx(size_t size)
{
	unsigned int x = (size >> 8), k;

	if (x == 0)
		return 0;

	if (x > 0xffff)
		return 31;

	k = sizeof(x) * __CHAR_BIT__ - 1 - __builtin_clz(x);
	return (k << 1) + (size >> (k + (8 - 1)) & 1);
}


static void _malloc_heapInit(heap_t *heap, size_t size)
{
	heap->freesz = size;
	heap->size = size;
}


static heap_t *_malloc_heapAlloc(size_t chunkSize)
{
	size_t heapSize = ((chunkSize + SIZE_PAGE - 1) / SIZE_PAGE) * SIZE_PAGE;
	heap_t *heap = mmap(NULL, heapSize, PROT_WRITE, MAP_ANONYMOUS, NULL, 0);
	if (heap == NULL)
		return NULL;

	_malloc_heapInit(heap, heapSize - sizeof(heap_t));
	return heap;
}


static void _malloc_heapSplit(heap_t *heap)
{
}


static void _malloc_chunkInit(chunk_t *chunk, heap_t *heap, size_t prevSize, size_t size)
{
	chunk->prevSize = prevSize;
	chunk->size = size;
	chunk->heap = heap;
	chunk->next = NULL;
	chunk->prev = NULL;
}


static void _malloc_chunkSplit(chunk_t *chunk, size_t size)
{
	if (chunk->size == size)
		return;
}


static void *malloc_allocSmall(size_t size)
{
	unsigned int idx = malloc_getsidx(size);
	unsigned int binmap = malloc_common.sbinmap & ~((1 << idx) - 1);
	size_t chunkSize = idx << 3;
	chunk_t *chunk;
	heap_t *heap;

	/* Find first chunk suitable for size. */
	if (binmap)
		idx = __builtin_ctz(binmap);

	lock(malloc_common.mutex);
	if ((chunk = malloc_common.sbins[idx]) == NULL) {
		if ((heap = _malloc_heapAlloc(chunkSize)) == NULL) {
			unlock(malloc_common.mutex);
			return NULL;
		}

		chunk = (chunk_t *) heap->space;
		_malloc_chunkInit(chunk, heap, 0, chunkSize);
		_malloc_heapSplit(heap);

		LIST_ADD(&malloc_common.sbins[idx], chunk);
		malloc_common.sbinmap |= (1 << idx);
	}

	LIST_REMOVE(&malloc_common.sbins[idx], chunk);
	if (malloc_common.sbins[idx] == NULL)
		malloc_common.sbinmap &= ~(1 << idx);

	_malloc_chunkSplit(chunk, chunkSize);
	chunk->heap->freesz -= chunkSize;

	/* Mark chunk as used. */
	chunk->size |= CHUNK_USED;
	if ((u32) chunk->heap->space + chunk->heap->size > (u32) chunk + chunkSize)
		*((size_t *) ((u32) chunk + chunkSize)) |= CHUNK_USED;

	unlock(malloc_common.mutex);
	return (void *) ((u32) chunk + 2 * sizeof(size_t) + sizeof(heap_t *));
}


static void *malloc_allocLarge(size_t size)
{
	return NULL;
}


void *malloc(size_t size)
{
	if (size == 0)
		return NULL;

	/* Smallest allocable size is 16. */
	size = max(size + sizeof(chunk_t), 16);

	if (size <= 248)
		return malloc_allocSmall(size);

	return malloc_allocLarge(size);
}


void _malloc_init(void)
{
	int i;

	malloc_common.allocsz = 0;
	malloc_common.freesz = 0;
	malloc_common.sbinmap = 0;
	malloc_common.lbinmap = 0;

	for (i = 0; i < 32; ++i) {
		malloc_common.sbins[i] = NULL;
		malloc_common.lbins[i] = NULL;
	}

	// TODO: Initialize mutex.
	//mutex_init(&malloc_common.mutex);
}
