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
#include "stdlib.h"
#include "sys/list.h"
#include "sys/threads.h"
#include "sys/mman.h"


typedef struct _malloc_arena_t {
	struct _malloc_arena_t *next;
	struct _malloc_arena_t *prev;
	size_t freesz;
	size_t *size;
} malloc_arena_t;


typedef struct _malloc_chunk_t {
	size_t size;
	struct _malloc_chunk_t *next;
	struct _malloc_chunk_t *prev;
	u8 data[];
} malloc_chunk_t;


struct {
	malloc_arena_t *freearenas;
	malloc_arena_t *usedarenas;
	malloc_chunk_t *bins[64];

	size_t maxsz;
	size_t allocsz;
	size_t freesz;

	handle_t lock;
} malloc_common;

#if 0
void *_malloc_allocchunk(size_t size)
{
	u8 idx;

	idx = _malloc_getidx(size);

	/* Find first chunk suitable for size */
	for (; (malloc_common.bins[idx] == NULL) && (idx < sizeof(malloc_common.bins) / sizeof(malloc_chunk_t *)); idx++);

printf("idx=%d\n", idx);

	if (idx >= sizeof(malloc_common.bins) / sizeof(malloc_chunk_t *))
		idx = _malloc_getidx(size);

	if ((ch = malloc_common.bins[idx]) == NULL) {
		LIST_REMOVE(&malloc_common.bins[idx], ch);
		ch->arena->freesz -= (1 << idx) + 2 * sizeof(size_t);
	}
	else {
		a = _malloc_arenaCreate(1 << _malloc_getidx(size));
		LIST_ADD(&malloc_common.freearenas, a);
		LIST_ADD(&malloc_common.bin[idx], a->chunks);
		ch = malloc_common.bin[idx];
		LIST_REMOVE(&malloc_common.bins[idx], ch);
	}

	_malloc_split(ch);
	
	if (ch->arena->freesz == 0) {
		LIST_REMOVE(&malloc_common.freearenas, ch->arena);
		LIST_ADD(&malloc_common.usedarenas, ch->arena);
	}

	return b;
}
#endif


#define NTREEBINS  64
#define TREEBIN_SHIFT 8

#define compute_tree_index(S, I)\
{\
  unsigned int X = S >> TREEBIN_SHIFT;\
  if (X == 0)\
    I = 0;\
  else if (X > 0xFFFF)\
    I = NTREEBINS-1;\
  else {\
    unsigned int K = (unsigned) sizeof(X)*__CHAR_BIT__ - 1 - (unsigned) __builtin_clz(X); \
    I =  (unsigned int)((K << 1) + ((S >> (K + (TREEBIN_SHIFT-1)) & 1)));\
  }\
}


void _malloc_init(void)
{
	unsigned int i;

	malloc_common.allocsz = 0;

	compute_tree_index(1024, i);

	printf("Compute tree index: %d\n", i);


}
