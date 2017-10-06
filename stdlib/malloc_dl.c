/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stdlib/malloc (Doug Lea)
 *
 * Copyright 2017 Phoenix Systems
 * Author: Jakub Sejdak
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include ARCH
#include "stdlib.h"
#include "limits.h"
#include "string.h"
#include "sys/list.h"
#include "sys/rb.h"
#include "sys/threads.h"
#include "sys/mman.h"

#define CHUNK_PUSED                1
#define CHUNK_CUSED                2

#define CHUNK_OVERHEAD             (sizeof(size_t) + sizeof(heap_t*))
#define CHUNK_MIN_SIZE             (__builtin_offsetof(chunk_t, node) + sizeof(size_t))
#define CHUNK_SMALLBIN_MAX_SIZE    (256 - CHUNK_OVERHEAD)

#define CEIL(value, size)			((((value) + (size) - 1) / (size)) * (size))
#define FLOOR(value, size)			(((value) / (size)) * (size))


typedef struct _heap_t {
	size_t size;
	size_t spaceSize;
	size_t freesz;
	u8 space[];
} __attribute__ ((packed)) heap_t;


typedef struct _chunk_t {
/*	size_t prevSize; This is a foot field of the previous chunk! */
	size_t size;
	heap_t *heap;

	/* Following fields are used only when the chunk is free */
	struct _chunk_t *next;
	struct _chunk_t *prev;
	rbnode_t node; /* Only used for big chunks */
} __attribute__ ((packed)) chunk_t;


struct {
	u32 sbinmap;
	u32 lbinmap;
	chunk_t *sbins[32];
	rbtree_t lbins[32];

	size_t allocsz;
	size_t freesz;

	handle_t mutex;
} malloc_common;


static inline size_t malloc_chunkSize(chunk_t *chunk)
{
	return chunk->size & ~(CHUNK_CUSED | CHUNK_PUSED);
}


static int malloc_cmp(rbnode_t *n1, rbnode_t *n2)
{
	chunk_t *e1 = lib_treeof(chunk_t, node, n1);
	chunk_t *e2 = lib_treeof(chunk_t, node, n2);

	size_t e1sz = malloc_chunkSize(e1);
	size_t e2sz = malloc_chunkSize(e2);

	if (e1sz == e2sz) {
		if (e1 == e2)
			return 0;

		return (e1 > e2) ? 1 : -1;
	}

	return (e1sz > e2sz) ? 1 : -1;
}


static int malloc_find(rbnode_t *n1, rbnode_t *n2)
{
	chunk_t *e1 = lib_treeof(chunk_t, node, n1);
	chunk_t *e1_left = lib_treeof(chunk_t, node, n1->left);
	chunk_t *e2 = lib_treeof(chunk_t, node, n2);

	size_t e1sz = malloc_chunkSize(e1);
	size_t e2sz = malloc_chunkSize(e2);

	if (e1sz == e2sz)
		return 0;

	if (e1sz > e2sz) {
		if (e1_left != NULL && malloc_chunkSize(e1_left) >= e2sz)
			return 1;

		return 0;
	}

	return -1;
}


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


static inline size_t malloc_chunkPrevSize(chunk_t *chunk)
{
	return (chunk->size & CHUNK_PUSED) ? 0 : *((size_t*) chunk - 1);
}


static inline int malloc_chunkIsFirst(chunk_t *chunk)
{
	return (chunk->heap->space == (u8*) &chunk);
}


static inline int malloc_chunkIsLast(chunk_t *chunk)
{
	return ((u32) chunk + malloc_chunkSize(chunk) + sizeof(chunk_t) > (u32) chunk->heap + chunk->heap->size);
}


static inline chunk_t *malloc_chunkPrev(chunk_t *chunk)
{
	unsigned prevSize = malloc_chunkPrevSize(chunk);
	if (prevSize == 0)
		return NULL;

	return (chunk_t *) ((u32) chunk - prevSize);
}


static inline chunk_t *malloc_chunkNext(chunk_t *chunk)
{
	if (malloc_chunkIsLast(chunk))
		return NULL;

	return (chunk_t *) ((u32) chunk + malloc_chunkSize(chunk));
}


static inline void malloc_chunkSetFooter(chunk_t *chunk)
{
	size_t size = malloc_chunkSize(chunk);
	*((size_t*)((u32) chunk + size) - 1) = size;
}


static void malloc_chunkInit(chunk_t *chunk, heap_t *heap, size_t prevSize, size_t size)
{
	chunk->size = size;

	if (prevSize)
		*((size_t*) chunk - 1) = prevSize;
	else
		chunk->size |= CHUNK_PUSED;

	chunk->heap = heap;
	chunk->next = NULL;
	chunk->prev = NULL;

	malloc_chunkSetFooter(chunk);
}


static void _malloc_chunkAdd(chunk_t *chunk)
{
	unsigned int idx;
	size_t chunksz = malloc_chunkSize(chunk);

	if (chunksz <= CHUNK_SMALLBIN_MAX_SIZE) {
		idx = malloc_getsidx(chunksz);
		LIST_ADD(&malloc_common.sbins[idx], chunk);
		malloc_common.sbinmap |= (1 << idx);
		return;
	}

	idx = malloc_getlidx(chunksz);
	lib_rbInsert(&malloc_common.lbins[idx], &chunk->node);
	malloc_common.lbinmap |= (1 << idx);
}


static void _malloc_chunkRemove(chunk_t *chunk)
{
	unsigned int idx;
	size_t chunksz = malloc_chunkSize(chunk);

	if (chunksz <= CHUNK_SMALLBIN_MAX_SIZE) {
		idx = malloc_getsidx(chunksz);
		LIST_REMOVE(&malloc_common.sbins[idx], chunk);
		if (malloc_common.sbins[idx] == NULL)
			malloc_common.sbinmap &= ~(1 << idx);

		return;
	}

	idx = malloc_getlidx(chunksz);
	lib_rbRemove(&malloc_common.lbins[idx], &chunk->node);
	malloc_common.lbinmap &= ~(1 << idx);
}


static inline int malloc_chunkCanSplit(chunk_t *chunk, size_t size)
{
	return !((malloc_chunkSize(chunk) <= size + CHUNK_MIN_SIZE) ||
		 (((u32) chunk + size + sizeof(chunk_t)) > ((u32) chunk->heap + chunk->heap->size)));
}


static void _malloc_chunkSplit(chunk_t *chunk, size_t size)
{
	chunk_t *sibling;

	sibling = (chunk_t *) ((u32) chunk + size);
	malloc_chunkInit(sibling, chunk->heap, size, malloc_chunkSize(chunk) - size);

	_malloc_chunkRemove(chunk);
	chunk->size = size | CHUNK_PUSED;
	_malloc_chunkAdd(sibling);
}


static void _malloc_chunkJoin(chunk_t *chunk)
{
	chunk_t *it = chunk;
	chunk_t *sibling;

	/* Join with the previous chunks. */
	while (!malloc_chunkIsFirst(it) && (it->size & CHUNK_PUSED) == 0) {
		sibling = malloc_chunkPrev(it);
		_malloc_chunkRemove(sibling);
		_malloc_chunkRemove(it);

		sibling->size += malloc_chunkSize(it);
		_malloc_chunkAdd(sibling);
		it = sibling;
	}

	/* Join with the following chunks. */
	while (!malloc_chunkIsLast(it) && (malloc_chunkNext(it)->size & CHUNK_CUSED) == 0) {
		sibling = malloc_chunkNext(it);
		_malloc_chunkRemove(it);
		_malloc_chunkRemove(sibling);

		it->size += malloc_chunkSize(sibling);
		_malloc_chunkAdd(it);
	}

	malloc_chunkSetFooter(it);
}


static void malloc_heapInit(heap_t *heap, size_t size)
{
	heap->size = size;
	heap->spaceSize = heap->size - sizeof(heap_t);
	heap->freesz = heap->spaceSize;
}


static heap_t *_malloc_heapAlloc(size_t size)
{
	size_t heapSize = CEIL(sizeof(heap_t) + size, SIZE_PAGE);
	heap_t *heap = mmap(NULL, heapSize, PROT_WRITE, MAP_ANONYMOUS, NULL, 0);
	if (heap == (heap_t*) -1)
		return NULL;

	malloc_heapInit(heap, heapSize);
	malloc_chunkInit((chunk_t *) heap->space, heap, 0, FLOOR(heap->spaceSize, 8));
	_malloc_chunkAdd((chunk_t *) heap->space);
	return heap;
}


static inline void *_malloc_allocFrom(chunk_t *chunk, size_t size)
{
	chunk_t *chunkNext;

	if (malloc_chunkCanSplit(chunk, size))
		_malloc_chunkSplit(chunk, size);
	else
		_malloc_chunkRemove(chunk);

	chunk->heap->freesz -= malloc_chunkSize(chunk);

	chunk->size |= CHUNK_CUSED;

	if ((chunkNext = malloc_chunkNext(chunk))) {
		chunkNext->size |= CHUNK_PUSED;
	}

	return (void *) ((u32) chunk + CHUNK_OVERHEAD);
}


static void *_malloc_allocLarge(size_t size)
{
	/* Lookup table to speed-up operation reverse to malloc_getlidx(). */
	static const size_t lookup[32] = {
		   0x17f,    0x1ff,    0x2ff,    0x3ff,    0x5ff,    0x7ff,    0xbff,      0xfff,
		  0x17ff,   0x1fff,   0x2fff,   0x3fff,   0x5fff,   0x7fff,   0xbfff,     0xffff,
		 0x17fff,  0x1ffff,  0x2ffff,  0x3ffff,  0x5ffff,  0x7ffff,  0xbffff,    0xfffff,
		0x17ffff, 0x1fffff, 0x2fffff, 0x3fffff, 0x5fffff, 0x7fffff, 0xbfffff, 0xffffffff
	};

	unsigned int binmap = malloc_common.lbinmap;
	unsigned int idx = malloc_getlidx(size);
	heap_t *heap;
	chunk_t *chunk = NULL;
	chunk_t t;
	t.size = size;

	while (idx < 32 && binmap != 0) {
		chunk = lib_treeof(chunk_t, node, lib_rbFindEx(malloc_common.lbins[idx].root, &t.node, malloc_find));
		if (chunk != NULL)
			break;

		binmap = malloc_common.lbinmap & ~((1 << ++idx) - 1);
	}

	if (chunk == NULL) {
		idx = malloc_getlidx(size);
		if ((heap = _malloc_heapAlloc(lookup[idx])) == NULL)
			return NULL;

		chunk = (chunk_t *) heap->space;
	}

	return _malloc_allocFrom(chunk, size);
}


static void *_malloc_allocSmall(size_t size)
{
	unsigned int idx = malloc_getsidx(size);
	unsigned int binmap = malloc_common.sbinmap & ~((1 << idx) - 1);
	size_t targetSize = idx << 3;
	size_t idxSize;
	chunk_t *chunk;
	heap_t *heap;

	if (binmap)
		idx = __builtin_ctz(binmap);
	else if (malloc_common.lbinmap)
		return _malloc_allocLarge(size);

	idxSize = idx << 3;
	if ((chunk = malloc_common.sbins[idx]) == NULL) {
		if ((heap = _malloc_heapAlloc(idxSize)) == NULL)
			return NULL;

		chunk = (chunk_t *) heap->space;

		if (malloc_chunkCanSplit(chunk, idxSize)) {
			_malloc_chunkSplit(chunk, idxSize);
			_malloc_chunkAdd(chunk);
		}
	}

	return _malloc_allocFrom(chunk, targetSize);
}


void *malloc(size_t size)
{
	void *ptr = NULL;

	if (size == 0 || (size + CHUNK_OVERHEAD) < size)
		return NULL;

	size = CEIL(max(size + CHUNK_OVERHEAD, CHUNK_MIN_SIZE), 8);

	lock(malloc_common.mutex);
	if (size <= CHUNK_SMALLBIN_MAX_SIZE)
		ptr = _malloc_allocSmall(size);
	else
		ptr = _malloc_allocLarge(size);
	unlock(malloc_common.mutex);

	return ptr;
}


void *calloc(size_t nitems, size_t size)
{
	void *ptr;
	u64 allocSize = nitems * size;

	if (allocSize > UINT_MAX)
		return NULL;

	if ((ptr = malloc((size_t) allocSize)) == NULL)
		return NULL;

	memset(ptr, 0, (size_t) allocSize);
	return ptr;
}


void free(void *ptr)
{
	chunk_t *chunk, *chunkNext;
	heap_t *heap;

	if (ptr == NULL)
		return;

	lock(malloc_common.mutex);

	chunk = (chunk_t *) ((u32) ptr - CHUNK_OVERHEAD);
	heap = chunk->heap;
	chunk->size &= ~CHUNK_CUSED;
	malloc_chunkSetFooter(chunk);

	if ((chunkNext = malloc_chunkNext(chunk))) {
		chunkNext->size &= ~CHUNK_PUSED;
	}

	heap->freesz += malloc_chunkSize(chunk);
	_malloc_chunkAdd(chunk);
	_malloc_chunkJoin(chunk);

	if (heap->freesz == heap->spaceSize) {
		chunk = (chunk_t *) heap->space;
		_malloc_chunkRemove(chunk);
		munmap(heap, heap->size);
	}

	unlock(malloc_common.mutex);
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
		lib_rbInit(&malloc_common.lbins[i], malloc_cmp, NULL);
	}

	mutex(&malloc_common.mutex);
}
