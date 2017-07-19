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
#include "stdlib.h"
#include "limits.h"
#include "sys/list.h"
#include "sys/rb.h"
#include "sys/threads.h"
#include "sys/mman.h"


#define CHUNK_USED					1
#define CHUNK_MIN_SIZE				16
#define CHUNK_SMALLBIN_MAX_SIZE		248

#define CEIL(value, size)			((((value) + (size) - 1) / (size)) * (size))
#define FLOOR(value, size)			(((value) / (size)) * (size))


typedef struct _heap_t {
	size_t size;
	size_t spaceSize;
	size_t freesz;
	u8 space[];
} __attribute__ ((packed)) heap_t;


typedef struct _chunk_t {
	size_t prevSize;
	size_t size;
	heap_t *heap;
	struct _chunk_t *next;
	struct _chunk_t *prev;
	rbnode_t node;
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


static int malloc_cmp(rbnode_t *n1, rbnode_t *n2)
{
	chunk_t *e1 = lib_treeof(chunk_t, node, n1);
	chunk_t *e2 = lib_treeof(chunk_t, node, n2);

	if (e1->size == e2->size) {
		if (e1 == e2)
			return 0;

		return (e1 > e2) ? 1 : -1;
	}

	return (e1->size > e2->size) ? 1 : -1;
}


static int malloc_find(rbnode_t *n1, rbnode_t *n2)
{
	chunk_t *e1 = lib_treeof(chunk_t, node, n1);
	chunk_t *e1_left = lib_treeof(chunk_t, node, n1->left);
	chunk_t *e1_right = lib_treeof(chunk_t, node, n1->right);
	chunk_t *e2 = lib_treeof(chunk_t, node, n2);

	if (e1->size == e2->size)
		return 0;

	if (e1->size > e2->size) {
		if (e1_left != NULL && e1_left->size >= e2->size)
			return 1;

		return 0;
	}

	if (e1_right != NULL && e1_right->size >= e2->size)
		return -1;

	return 0;
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


static inline int _malloc_chunkIsFirst(chunk_t *chunk)
{
	return (chunk->prevSize == 0);
}


static inline int _malloc_chunkIsLast(chunk_t *chunk)
{
	return ((u32) chunk + chunk->size + sizeof(chunk_t) > (u32) chunk->heap + chunk->heap->size);
}


static inline chunk_t *_malloc_chunkPrev(chunk_t *chunk)
{
	if (_malloc_chunkIsFirst(chunk))
		return NULL;

	return (chunk_t *) ((u32) chunk - (chunk->prevSize & ~CHUNK_USED));
}


static inline chunk_t *_malloc_chunkNext(chunk_t *chunk)
{
	if (_malloc_chunkIsLast(chunk))
		return NULL;

	return (chunk_t *) ((u32) chunk + chunk->size);
}


static void _malloc_chunkInit(chunk_t *chunk, heap_t *heap, size_t prevSize, size_t size)
{
	chunk->prevSize = prevSize;
	chunk->size = size;
	chunk->heap = heap;
	chunk->next = NULL;
	chunk->prev = NULL;

	if ((u32) chunk + size + sizeof(size_t) <= (u32) heap + heap->size)
		((chunk_t *) ((u32) chunk + size))->prevSize = size;
}


static void _malloc_chunkAdd(chunk_t *chunk)
{
	unsigned int idx;

	if (chunk->size <= CHUNK_SMALLBIN_MAX_SIZE) {
		idx = malloc_getsidx(chunk->size);
		LIST_ADD(&malloc_common.sbins[idx], chunk);
		malloc_common.sbinmap |= (1 << idx);
		return;
	}

	idx = malloc_getlidx(chunk->size);
	lib_rbInsert(&malloc_common.lbins[idx], &chunk->node);
	malloc_common.lbinmap |= (1 << idx);
}


static void _malloc_chunkRemove(chunk_t *chunk)
{
	unsigned int idx;

	if (chunk->size <= CHUNK_SMALLBIN_MAX_SIZE) {
		idx = malloc_getsidx(chunk->size);
		LIST_REMOVE(&malloc_common.sbins[idx], chunk);
		if (malloc_common.sbins[idx] == NULL)
			malloc_common.sbinmap &= ~(1 << idx);

		return;
	}

	idx = malloc_getlidx(chunk->size);
	lib_rbRemove(&malloc_common.lbins[idx], &chunk->node);
	malloc_common.lbinmap &= ~(1 << idx);
}


static void _malloc_chunkSplit(chunk_t *chunk, size_t size)
{
	chunk_t *sibling;

	if (chunk->size == size)
		return;

	sibling = (chunk_t *) ((u32) chunk + size);
	_malloc_chunkInit(sibling, chunk->heap, size, FLOOR(chunk->size - size, 8));

	_malloc_chunkRemove(chunk);
	chunk->size = size;
	_malloc_chunkAdd(chunk);

	if (sibling->size < CHUNK_MIN_SIZE)
		return;

	_malloc_chunkAdd(sibling);
}


static void _malloc_chunkJoin(chunk_t *chunk)
{
	chunk_t *it = chunk;
	chunk_t *sibling;

	/* Join with the previous chunks. */
	while (!_malloc_chunkIsFirst(it) && (it->prevSize & CHUNK_USED) == 0) {
		sibling = _malloc_chunkPrev(it);
		_malloc_chunkRemove(sibling);
		_malloc_chunkRemove(it);

		sibling->size += it->size;
		_malloc_chunkAdd(sibling);
		it = sibling;
	}

	if (!_malloc_chunkIsLast(it))
		_malloc_chunkNext(it)->prevSize = it->size;

	/* Join with the following chunks. */
	while (!_malloc_chunkIsLast(it) && (_malloc_chunkNext(it)->size & CHUNK_USED) == 0) {
		sibling = _malloc_chunkNext(it);
		_malloc_chunkRemove(it);
		_malloc_chunkRemove(sibling);

		it->size += sibling->size;
		_malloc_chunkAdd(it);
	}

	if (!_malloc_chunkIsFirst(it))
		_malloc_chunkNext(it)->prevSize = it->size;
}


static void _malloc_heapInit(heap_t *heap, size_t size)
{
	heap->size = size;
	heap->spaceSize = heap->size - sizeof(heap_t);
	heap->freesz = heap->spaceSize;
}


static heap_t *_malloc_heapAlloc(size_t size)
{
	size_t heapSize = CEIL(sizeof(heap_t) + size, SIZE_PAGE);
	heap_t *heap = mmap(NULL, heapSize, PROT_WRITE, MAP_ANONYMOUS, NULL, 0);
	if (heap == NULL)
		return NULL;

	_malloc_heapInit(heap, heapSize);
	_malloc_chunkInit((chunk_t *) heap->space, heap, 0, FLOOR(heap->spaceSize, 8));
	_malloc_chunkAdd((chunk_t *) heap->space);
	return heap;
}


static void *malloc_allocLarge(size_t size)
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

	_malloc_chunkSplit(chunk, size);
	_malloc_chunkRemove(chunk);
	chunk->heap->freesz -= size;

	chunk->size |= CHUNK_USED;
	if (!_malloc_chunkIsLast(chunk))
		_malloc_chunkNext(chunk)->prevSize = chunk->size;

	return (void *) ((u32) chunk + 2 * sizeof(size_t) + sizeof(heap_t *));
}


static void *malloc_allocSmall(size_t size)
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
		return malloc_allocLarge(size);

	idxSize = idx << 3;
	if ((chunk = malloc_common.sbins[idx]) == NULL) {
		if ((heap = _malloc_heapAlloc(idxSize)) == NULL)
			return NULL;

		chunk = (chunk_t *) heap->space;
		_malloc_chunkSplit(chunk, idxSize);
	}

	_malloc_chunkSplit(chunk, targetSize);
	_malloc_chunkRemove(chunk);
	chunk->heap->freesz -= chunk->size;

	chunk->size |= CHUNK_USED;
	if (!_malloc_chunkIsLast(chunk))
		_malloc_chunkNext(chunk)->prevSize = chunk->size;

	return (void *) ((u32) chunk + 2 * sizeof(size_t) + sizeof(heap_t *));
}


void *malloc(size_t size)
{
	void *ptr = NULL;

	if (size == 0)
		return NULL;

	size = CEIL(max(size + sizeof(chunk_t), CHUNK_MIN_SIZE), 8);

	lock(malloc_common.mutex);
	if (size <= CHUNK_SMALLBIN_MAX_SIZE)
		ptr = malloc_allocSmall(size);
	else
		ptr = malloc_allocLarge(size);
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
	chunk_t *chunk;
	heap_t *heap;

	if (ptr == NULL)
		return;

	lock(malloc_common.mutex);

	chunk = (chunk_t *) ((u32) ptr - 2 * sizeof(size_t) - sizeof(heap_t *));
	heap = chunk->heap;
	chunk->size &= ~CHUNK_USED;
	if (!_malloc_chunkIsLast(chunk))
		_malloc_chunkNext(chunk)->prevSize = chunk->size;

	heap->freesz += chunk->size;
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
