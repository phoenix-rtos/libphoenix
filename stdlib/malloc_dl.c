/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stdlib/malloc (Doug Lea)
 *
 * Copyright 2017, 2020 Phoenix Systems
 * Author: Jakub Sejdak, Jan Sikorski, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <errno.h>
#include <sys/list.h>
#include <sys/minmax.h>
#include <sys/rb.h>
#include <sys/threads.h>
#include <sys/minmax.h>
#include <sys/mman.h>
#include <sys/debug.h>

#include <arch.h>
#include <stddef.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#define CEIL(value, size)          ((((value) + (size) - 1) / (size)) * (size))
#define FLOOR(value, size)         (((value) / (size)) * (size))

#define CHUNK_PUSED                1
#define CHUNK_CUSED                2

#define CHUNK_OVERHEAD             CEIL(__builtin_offsetof(chunk_t, next), 8)
#define CHUNK_MIN_SIZE             CEIL(__builtin_offsetof(chunk_t, node) + sizeof(size_t), 8)
#define CHUNK_SMALLBIN_MAX_SIZE    (256 - CHUNK_OVERHEAD)


typedef struct {
	size_t size;
	size_t freesz;
	uint8_t space[];
} heap_t;


typedef struct _chunk_t {
/*	size_t prevSize; This is a foot field of the previous chunk! */
	size_t size;
	heap_t *heap;

	/* Following fields are used only when the chunk is free */
	struct _chunk_t *next;
	struct _chunk_t *prev;
	rbnode_t node; /* Only used for big chunks */
} chunk_t;


struct {
	uint32_t sbinmap;
	uint32_t lbinmap;
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

	if (e1sz == e2sz)
		return 0;

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


static inline int malloc_chunkIsFirst(chunk_t *chunk)
{
	return (chunk->heap->space == (uint8_t*) chunk);
}


static long int malloc_chunkIsLast(chunk_t *chunk)
{
	return ((uintptr_t) chunk + malloc_chunkSize(chunk) + CHUNK_MIN_SIZE > (uintptr_t) chunk->heap + chunk->heap->size);
}


static inline chunk_t *malloc_chunkPrev(chunk_t *chunk)
{
	unsigned prevSize = (chunk->size & CHUNK_PUSED) ? 0 : *((size_t *) chunk - 1);
	if (prevSize == 0)
		return NULL;

	return (chunk_t *) ((uintptr_t) chunk - prevSize);
}


static inline chunk_t *malloc_chunkNext(chunk_t *chunk)
{
	if (malloc_chunkIsLast(chunk))
		return NULL;

	return (chunk_t *) ((uintptr_t) chunk + malloc_chunkSize(chunk));
}


static inline void malloc_chunkSetFooter(chunk_t *chunk)
{
	size_t size = malloc_chunkSize(chunk);
	*((size_t *)((uintptr_t) chunk + size) - 1) = size;
}


static void malloc_chunkInit(chunk_t *chunk, heap_t *heap, size_t size)
{
	chunk->size = size;
	chunk->heap = heap;
	chunk->next = NULL;
	chunk->prev = NULL;

	malloc_chunkSetFooter(chunk);
}


static void _malloc_chunkAdd(chunk_t *chunk)
{
	unsigned int idx;
	size_t chunksz = malloc_chunkSize(chunk);
	chunk_t *exist;

	if (chunksz <= CHUNK_SMALLBIN_MAX_SIZE) {
		idx = malloc_getsidx(chunksz);
		LIST_ADD(&malloc_common.sbins[idx], chunk);
		malloc_common.sbinmap |= (1 << idx);
		return;
	}

	idx = malloc_getlidx(chunksz);
	exist = lib_treeof(chunk_t, node, lib_rbInsert(&malloc_common.lbins[idx], &chunk->node));
	if (exist != NULL)
		/* Mark chunk as not actually being in the tree */
		chunk->node.parent = &chunk->node;
	LIST_ADD(&exist, chunk);

	malloc_common.lbinmap |= (1 << idx);
}


static void _malloc_chunkRemove(chunk_t *chunk)
{
	unsigned int idx;
	size_t chunksz = malloc_chunkSize(chunk);
	chunk_t *next = chunk;

	if (chunksz <= CHUNK_SMALLBIN_MAX_SIZE) {
		idx = malloc_getsidx(chunksz);
		LIST_REMOVE(&malloc_common.sbins[idx], chunk);
		if (malloc_common.sbins[idx] == NULL)
			malloc_common.sbinmap &= ~(1 << idx);

		return;
	}

	idx = malloc_getlidx(chunksz);
	LIST_REMOVE(&next, chunk);

	if (next == NULL) {
		lib_rbRemove(&malloc_common.lbins[idx], &chunk->node);

		if (malloc_common.lbins[idx].root == NULL)
			malloc_common.lbinmap &= ~(1 << idx);
	}
	else if (chunk->node.parent != &chunk->node) {
		next->node = chunk->node;
		rb_transplant(&malloc_common.lbins[idx], &chunk->node, &next->node);
		if (next->node.left != NULL)
			next->node.left->parent = &next->node;
		if (next->node.right != NULL)
			next->node.right->parent = &next->node;
	}
}


static inline int malloc_chunkCanSplit(chunk_t *chunk, size_t size)
{
	return (size >= CHUNK_OVERHEAD) && (malloc_chunkSize(chunk) >= size + CHUNK_MIN_SIZE);
}


static void _malloc_chunkSplit(chunk_t *chunk, size_t size)
{
	chunk_t *sibling;

	_malloc_chunkRemove(chunk);

	sibling = (chunk_t *) ((uintptr_t) chunk + size);
	malloc_chunkInit(sibling, chunk->heap, malloc_chunkSize(chunk) - size);

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
	heap->freesz = heap->size - sizeof(heap_t);
}


static heap_t *_malloc_heapAlloc(size_t size)
{
	chunk_t *chunk;
	size_t heapSize = CEIL(sizeof(heap_t) + size, _PAGE_SIZE);
	heap_t *heap;

	if (heapSize < size) {
		return NULL;
	}

	heap = mmap(NULL, heapSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (heap == MAP_FAILED) {
		return NULL;
	}

	chunk = (chunk_t*) heap->space;

	malloc_heapInit(heap, heapSize);
	malloc_chunkInit(chunk, heap, FLOOR(heap->size - sizeof(heap_t), 8));
	chunk->size |= CHUNK_PUSED;
	_malloc_chunkAdd(chunk);
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

	if ((chunkNext = malloc_chunkNext(chunk)) != NULL)
		chunkNext->size |= CHUNK_PUSED;

	return (void *) ((uintptr_t) chunk + CHUNK_OVERHEAD);
}


static void *_malloc_allocLarge(size_t size)
{
	/* Lookup table to speed-up operation reverse to malloc_getlidx(). */
	static const size_t lookup[32] = {
		   0x17f,    0x1ff,    0x2ff,    0x3ff,    0x5ff,    0x7ff,    0xbff,      0xfff,
		  0x17ff,   0x1fff,   0x2fff,   0x3fff,   0x5fff,   0x7fff,   0xbfff,     0xffff,
		 0x17fff,  0x1ffff,  0x2ffff,  0x3ffff,  0x5ffff,  0x7ffff,  0xbffff,    0xfffff,
		0x17ffff, 0x1fffff, 0x2fffff, 0x3fffff, 0x5fffff, 0x7fffff, 0xbfffff,        0x0
	};

	unsigned int idx = malloc_getlidx(size);
	unsigned int binmap = malloc_common.lbinmap & ~((1 << idx) - 1);
	heap_t *heap;
	chunk_t *chunk = NULL;
	chunk_t t;
	t.size = size;

	while (idx < 32 && binmap) {
		chunk = lib_treeof(chunk_t, node, lib_rbFindEx(malloc_common.lbins[idx].root, &t.node, malloc_find));
		if (chunk != NULL)
			break;

		binmap = binmap & ~(1 << idx++);
	}

	if (chunk == NULL) {
		idx = malloc_getlidx(size);
		if ((heap = _malloc_heapAlloc(max(lookup[idx], size))) == NULL)
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
			malloc_chunkSetFooter(chunk);
			_malloc_chunkAdd(chunk);
		}
	}

	return _malloc_allocFrom(chunk, targetSize);
}


size_t malloc_usable_size(void *ptr)
{
	chunk_t *chunk;
	size_t size = 0;

	if (ptr != NULL) {
		mutexLock(malloc_common.mutex);
		chunk = (chunk_t *)((uintptr_t)ptr - CHUNK_OVERHEAD);
		size = malloc_chunkSize(chunk) - CHUNK_OVERHEAD;
		mutexUnlock(malloc_common.mutex);
	}

	return size;
}


void *malloc(size_t size)
{
	void *ptr = NULL;

	if (size == 0) {
		return NULL;
	}

	if ((size + CHUNK_OVERHEAD) < size) {
		errno = ENOMEM;
		return NULL;
	}

	size = CEIL(max(size + CHUNK_OVERHEAD, CHUNK_MIN_SIZE), 8);

	mutexLock(malloc_common.mutex);
	if (size <= CHUNK_SMALLBIN_MAX_SIZE) {
		ptr = _malloc_allocSmall(size);
	}
	else {
		ptr = _malloc_allocLarge(size);
	}
	mutexUnlock(malloc_common.mutex);

	if (ptr == NULL) {
		errno = ENOMEM;
	}

	return ptr;
}


void *calloc(size_t nitems, size_t size)
{
	void *ptr;
	uint64_t allocSize = (uint64_t)nitems * size;

	if (allocSize > (uint64_t)UINT_MAX) {
		errno = ENOMEM;
		return NULL;
	}

	if ((ptr = malloc((size_t) allocSize)) == NULL)
		return NULL;

	memset(ptr, 0, (size_t)allocSize);
	return ptr;
}


void free(void *ptr)
{
	chunk_t *chunk, *chunkNext;
	heap_t *heap;

	if (ptr == NULL)
		return;

	mutexLock(malloc_common.mutex);

	chunk = (chunk_t *) ((uintptr_t) ptr - CHUNK_OVERHEAD);
	heap = chunk->heap;

	if (!(chunk->size & CHUNK_CUSED)) {
		debug("Double free detected\n");
		_exit(EX_SOFTWARE);
	}

	chunk->size &= ~CHUNK_CUSED;
	malloc_chunkSetFooter(chunk);

	if ((chunkNext = malloc_chunkNext(chunk)) != NULL)
		chunkNext->size &= ~CHUNK_PUSED;

	heap->freesz += malloc_chunkSize(chunk);
	_malloc_chunkAdd(chunk);
	_malloc_chunkJoin(chunk);

	if (heap->freesz == heap->size - sizeof(heap_t)) {
		chunk = (chunk_t *) heap->space;
		_malloc_chunkRemove(chunk);
		munmap(heap, heap->size);
	}

	mutexUnlock(malloc_common.mutex);
}


void *realloc(void *ptr, size_t size)
{
	chunk_t *chunk, *sibling, *next;
	heap_t *heap;
	size_t chunksz;

	void *p;

	if (ptr == NULL)
		return malloc(size);

	if (size == 0) {
		free(ptr);
		return NULL;
	}

	if ((size + CHUNK_OVERHEAD) < size) {
		errno = ENOMEM;
		return NULL;
	}

	size = CEIL(max(size + CHUNK_OVERHEAD, CHUNK_MIN_SIZE), 8);

	mutexLock(malloc_common.mutex);

	chunk = (chunk_t *) ((uintptr_t) ptr - CHUNK_OVERHEAD);
	heap = chunk->heap;
	chunksz = malloc_chunkSize(chunk);


	if (size < chunksz && malloc_chunkCanSplit(chunk, size)) {
		sibling = (chunk_t *) ((uintptr_t) chunk + size);
		malloc_chunkInit(sibling, heap, chunksz - size);
		sibling->size |= CHUNK_PUSED;
		_malloc_chunkAdd(sibling);
		heap->freesz += chunksz - size;

		chunk->size = size | CHUNK_CUSED | (chunk->size & CHUNK_PUSED);

		_malloc_chunkJoin(sibling);

		if ((next = malloc_chunkNext(sibling)) != NULL)
			next->size &= ~CHUNK_PUSED;
	}
	else if (size > chunksz) {
		if ((next = malloc_chunkNext(chunk)) != NULL && !(next->size & CHUNK_CUSED) &&
				(malloc_chunkSize(next) >= (size - chunksz))) {
			_malloc_allocFrom(next, size - chunksz);
			chunk->size += malloc_chunkSize(next);
		}
		else {
			mutexUnlock(malloc_common.mutex);

			p = malloc(size);
			if (p != NULL) {
				memcpy(p, ptr, chunksz - CHUNK_OVERHEAD);
				free(ptr);
			}

			return p;
		}
	}

	mutexUnlock(malloc_common.mutex);

	return ptr;
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

	mutexCreate(&malloc_common.mutex);
}


#define ASSERT(cond, ...) do {					\
		if (!(cond)) {					\
			printf(__VA_ARGS__);			\
			for (;;) ;				\
		}						\
	} while (0)


static void malloc_test_heap(chunk_t *chunk)
{
	chunk_t *next = malloc_chunkNext(chunk);
	const char *unmerged = "malloc_dl: unmerged free chunks\n";
	const char *pused = "malloc_dl: invalid PUSED flag\n";

	ASSERT(chunk->size & CHUNK_PUSED, pused);
	ASSERT(next == NULL || (next->size & CHUNK_CUSED), unmerged);
	ASSERT(next == NULL || !(next->size & CHUNK_PUSED), pused);

	next = malloc_chunkPrev(chunk);
	ASSERT(next == NULL || (next->size & CHUNK_CUSED), unmerged);
	ASSERT(chunk->size & CHUNK_PUSED, pused);
}


static void malloc_test_lbin(int lidx, chunk_t *chunk)
{
	size_t sz;
	chunk_t *c;

	if (chunk == NULL)
		return;

	malloc_test_heap(chunk);

	sz = malloc_chunkSize(chunk);
	ASSERT(!(chunk->size & CHUNK_CUSED), "malloc_dl: free chunk marked as used\n");
	ASSERT(malloc_getlidx(sz) == lidx, "malloc_dl: wrong chunk size (%zu) at lbin %d", sz, lidx);

	c = chunk;
	do  {
		ASSERT(!(c->size & CHUNK_CUSED), "malloc_dl: free chunk marked as used\n");
		ASSERT(malloc_chunkSize(c) == sz, "malloc_dl: wrong chunk size at lidx %d\n", lidx);
	} while (c->next != chunk && (c = c->next));

	malloc_test_lbin(lidx, lib_treeof(chunk_t, node, chunk->node.left));
	malloc_test_lbin(lidx, lib_treeof(chunk_t, node, chunk->node.right));
}


void malloc_test(void)
{
	int i;
	chunk_t *chunk;
	mutexLock(malloc_common.mutex);

	for (i = 0; i < 32; ++i) {
		if (malloc_common.sbinmap & (1 << i)) {
			ASSERT(malloc_common.sbins[i] != NULL, "malloc_dl: sbinmap bit %d set but bin is empty\n", i);
			chunk = malloc_common.sbins[i];

			do  {
				malloc_test_heap(chunk);
				ASSERT(!(chunk->size & CHUNK_CUSED), "malloc_dl: free chunk marked as used\n");
				ASSERT(malloc_chunkSize(chunk) == (i << 3), "malloc_dl: wrong chunk size at sidx %d\n", i);
			} while (chunk->next != malloc_common.sbins[i] && (chunk = chunk->next));
		}
		else
			ASSERT(malloc_common.sbins[i] == NULL, "malloc_dl: empty lbin %d should be NULL\n", i);
	}

	for (i = 0; i < 32; ++i) {
		if (malloc_common.lbinmap & (1 << i)) {
			ASSERT(malloc_common.lbins[i].root != NULL, "malloc_dl: lbinmap bit %d set but bin is empty\n", i);
			chunk = lib_treeof(chunk_t, node, malloc_common.lbins[i].root);

			malloc_test_lbin(i, chunk);
		}
		else
			ASSERT(malloc_common.lbins[i].root == NULL, "malloc_dl: empty lbin %d should be NULL\n", i);
	}

	mutexUnlock(malloc_common.mutex);
}
