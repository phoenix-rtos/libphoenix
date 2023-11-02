/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stdlib/malloc_trivial - trivial dynamic memory allocator
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk, Andrzej Asztemborski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "assert.h"
#include "stdlib.h"
#include "sys/list.h"
#include "sys/threads.h"
#include "sys/mman.h"


#define CEIL(x, s)  (((x) + (s) - 1) & ~((s) - 1))


typedef struct _chunk_t {
	size_t size;

	union {
		char userspace[0];
		struct {
			struct _chunk_t *next;
			struct _chunk_t *prev;
		};
	};
} chunk_t;


typedef struct _heap_t {
	struct _heap_t *next;
	struct _heap_t *prev;

	size_t size;
	size_t maxchunk;
	chunk_t *chunks;
} heap_t;


struct {
	handle_t mutex;
	heap_t *heaps;
} malloc_common;


static heap_t *_malloc_heapCreate(size_t size)
{
	heap_t *heap;
	chunk_t *chunk;

	heap = mmap((void *)0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (heap == NULL) {
		return NULL;
	}

	heap->size = size;
	chunk = (chunk_t *)(heap + 1);

	CHUNK_SETSIZE(chunk, (heap->size - sizeof(heap_t) - 2 * sizeof(size_t)) & ~7);
	heap->maxchunk = chunk->size;

	LIST_ADD(&heap->chunks, chunk);
	LIST_ADD(&malloc_common.heaps, heap);

	return heap;
}


static void *_malloc_heapDestroy(heap_t *heap)
{
	munmap(heap, heap->size);
	return NULL;
}


static void *_malloc_heapAlloc(size_t size)
{
	heap_t *heap;
	chunk_t *chunk, *remainder;

	if ((chunk = heap->chunks) == NULL)
		return NULL;

	LIST_REMOVE(&heap->chunks, chunk);

	if (chunk->size > size) {
		remainder = (chunk_t *)(chunk->userspace + chunk->size + sizeof(size_t));

		CHUNK_SETSIZE(remainder, chunk->size - size - 2 * sizeof(size_t));
		CHUNK_SETSIZE(chunk, size);

		LIST_ADD(&heap->chunks, remainder);
	}

	_malloc_heapUpdate(heap);

	return chunk;
}


static void _malloc_heapRelease(heap_t *heap, chunk_t *chunk)
{
	chunk_t *prev, *next;

	prev = CHUNK_PREV(chunk);
	if ((prev != NULL) && ((prev->size & 1) == 0)) {
		LIST_REMOVE(&heap->chunks, prev);
		CHUNK_SETSIZE(prev, prev->size + chunk->size + 2 * sizeof(size_t));
		chunk = prev;
	}

	next = CHUNK_NEXT(chunk);
	if ((next != NULL) && ((next->size & 1) == 0)) {
		LIST_REMOVE(&heap->chunks, next);
		CHUNK_SETSIZE(chunk, chunk->size + next->size + 2 * sizeof(size_t));
	}

	LIST_ADD(&heap->chunks, chunk);
}


void *malloc(size_t size)
{
	heap_t *heap;
	void *m;

	if (!size)
		return NULL;

	size = CEIL(size, 8);

	mutexLock(malloc_common.mutex);
	if ((heap = LIST_FIND(malloc_common.heaps, size)) == NULL)
		heap = malloc_heapCreate();

	m = _malloc_heapAlloc(size);
	mutexUnlock(malloc_common.mutex);

	if (m == NULL) {
		errno = ENOMEM;
	}

	return m;
}


void free(void *ptr)
{
	heap_t *heap;
	chunk_t *chunk;

	mutexLock(malloc_common.mutex);

	chunk = ptr - sizeof(size_t);
	heap = _malloc_heapFind(chunk);
	_malloc_heapRelease(heap, chunk);
	mutexUnlock(malloc_common.mutex);
}


void *calloc(size_t nmemb, size_t size)
{
	void *m;

	if ((m = malloc(CEIL(nmemb * size, 8))) == NULL)
		return NULL;

	memset(m, 0, CEIL(nmemb * size, 8));
	return m;
}


void *realloc(void *ptr, size_t size)
{
	void *result;
	chunk_t *chunk, *right, *newchunk;
	heap_t *heap;

	if (ptr == NULL)
		return malloc(size);

	if (!size) {
		free(ptr);
		return NULL;
	}

	size = CEIL(size, 8);

	mutexLock(malloc_common.mutex);
	chunk = ptr - sizeof(chunk_t);
	if (chunk->size == size) {
		mutexUnlock(malloc_common.mutex);
		return ptr;
	}

	heap = _malloc_heapFind(chunk);

	if (size > chunk->size) {
		right = CHUNK_NEXT(chunk);
		if ((right != NULL) && ((right->size & 1) == 0) && (chunk->size + right->size >= size)) {
			LIST_REMOVE(&heap->chunks, right);
			CHUNK_SETSIZE(chunk, chunk->size + right->size);
		}
		else {
			newchunk = _malloc_heapAllocate(heap, size);
			memcpy(newchunk->userspace, chunk->userspace, chunk->size);
			_malloc_heapRelease(heap, chunk);
		}
	}
	else if (size < chunk->size) {
		newchunk = (chunk_t *)(chunk->userspace + chunk->size + sizeof(size_t));
		CHUNK_SETSIZE(newchunk, chunk->size - size - 2 * sizeof(size_t));
		CHUNK_SETSIZE(chunk, size);
		LIST_ADD(&heap->chunks, newchunk);
	}

	_malloc_heapUpdate(heap);
	mutexUnlock(malloc_common.mutex);

	return result;
}


void _malloc_init(void)
{
	malloc_common.heaps = NULL;
	mutexCreate(&malloc_common.mutex);
}
