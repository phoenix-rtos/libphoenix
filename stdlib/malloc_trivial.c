/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * stdlib/malloc
 *
 * Copyright 2017 Phoenix Systems
 * Author: Andrzej Asztemborski
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

#include "malloc_trivial.h"

/*
 * global data: list of page_sets protected by mutex if application is multi-threaded
 * data structures: page_set contains list of chunks
 *
 * malloc steps:
 * 1. find the first chunk large enough to fit aligned size;
 * 2. if no such chunk exists, mmap new page_set with appropriate size;
 * #if 0 2a. check if new mmap'ed page_set can be coalesced with any existing; #endif
 * 3. cut off new chunk from the one found or mmap'ed and return its contents to the user.
 *
 * free steps:
 * 1. find chunk based on address;
 * 2. set it free and check if it can be coalesced with any neighbour;
 * 3. check if changes performed allows parent page_set #if 0 (or part of it) #endif to be munmap'ed;
 *
 * realloc steps:
 * 0. check if realloc(NULL, size) or realloc(ptr, 0)
 * 1. find chunk based on address;
 * 2. if there is available space in the chunk found: adjust the chunk and its successor;
 * 3. otherwise if its successor is free, coalesce it with chunk found and cut required space;
 * 4. otherwise: malloc, memcpy, free.
 *
 * calloc steps:
 * 1. malloc(nmemb*size);
 * 2. memset().
 */

static int page_set_fit(listnode_t *page_set_node, listnode_t *request_node);
static int chunk_fit(listnode_t *chunk_node, listnode_t *request_node);

struct malloc_common malloc_common = { .page_sets = { .compare = page_set_fit } };

struct chunk {
	listnode_t listnode;
	size_t size; /* net, only user available space */
	int free; /* flag */
	unsigned char contents[0];
};

struct page_set {
	listnode_t listnode;
	size_t total_size; /* gross, including header */
	size_t max_free_chunk_size; /* net, only user available space */
	list_t chunks;
	struct chunk first_chunk;
};

#define offsetof(type, member)  __builtin_offsetof (type, member)
#define chunk_of_contents(ptr) ((struct chunk *)((void *)(ptr) - offsetof(struct chunk, contents)))
static struct page_set *page_set_of_chunk(struct chunk *chunk)
{
	listnode_t *current = &chunk->listnode;
	struct chunk *first;
	while (current)
		current = current->prev;
	first = lib_listof(struct chunk, listnode, current);
	return (struct page_set *)((void *)first - offsetof(struct page_set, first_chunk));
}

#define PAGE_SIZE	0x100

static inline size_t PAGE_CEIL(size_t size)
{
	size_t result = size / PAGE_SIZE;
	if (size % PAGE_SIZE)
		++result;

	return result * PAGE_SIZE;
}

static inline size_t ALIGN_CEIL(size_t size)
{
	size_t result = size / __BIGGEST_ALIGNMENT__;
	if (size % __BIGGEST_ALIGNMENT__)
		++result;

	return result * __BIGGEST_ALIGNMENT__;
}

static int page_set_fit(listnode_t *page_set_node, listnode_t *request_node)
{
	struct page_set *set = lib_listof(struct page_set, listnode, page_set_node),
		*request = lib_listof(struct page_set, listnode, request_node);

	if (set->max_free_chunk_size >= request->max_free_chunk_size)
		return 0;

	return 1;
}

static int chunk_fit(listnode_t *chunk_node, listnode_t *request_node)
{
	struct chunk *chunk = lib_listof(struct chunk, listnode, chunk_node),
		*request = lib_listof(struct chunk, listnode, request_node);

	if (chunk->size >= request->size)
		return 0;

	return 1;
}

static struct page_set *alloc_page_set(size_t size)
{
	oid_t dummy = { 0, 0 };
	size_t page_set_total_size = PAGE_CEIL(offsetof(struct page_set, first_chunk.contents) + size);
	struct page_set *result = mmap(NULL, page_set_total_size, PROT_READ | PROT_WRITE,
				       MAP_PRIVATE | MAP_ANONYMOUS, dummy, 0);
	if (!result) /* out of memory */
		return NULL;

	result->total_size = page_set_total_size;
	lib_listInit(&result->chunks, chunk_fit);
	lib_listAppend(&result->chunks, &result->first_chunk.listnode);
	result->first_chunk.size = result->max_free_chunk_size =
		page_set_total_size - offsetof(struct page_set, first_chunk.contents);
	result->first_chunk.free = 1;

	lib_listAppend(&malloc_common.page_sets, &result->listnode);
	return result;
}

/* update max free chunk size in page_set structure */
static void update_page_set(struct page_set *page_set)
{
	size_t max_free_chunk_size = 0;
	listnode_t *current_node;
	struct chunk *current_chunk;

	for (current_node = lib_listHead(&page_set->chunks); current_node;
	     current_node = current_node->next) {
		current_chunk = lib_listof(struct chunk, listnode, current_node);
		if (current_chunk->free && max_free_chunk_size < current_chunk->size)
			max_free_chunk_size = current_chunk->size;
	}

	page_set->max_free_chunk_size = max_free_chunk_size;
}

static void *cut_chunk(struct page_set *page_set, struct chunk *chunk, size_t size)
{
	struct chunk *remainder;

	assert(chunk->free);
	if (chunk->size > size + sizeof(struct chunk) + ALIGN_CEIL(1)) {
		chunk->size = size;
		remainder = (struct chunk *)(chunk->contents + size);
		remainder->size = chunk->size - size - sizeof(struct chunk);
		remainder->free = 1;
		lib_listInsertAfter(&page_set->chunks, &chunk->listnode, &remainder->listnode);
	}
	chunk->free = 0;
	update_page_set(page_set);
	return chunk->contents;
}

static void *allocate(size_t size) {
	struct page_set page_set_requested = { .max_free_chunk_size = size }, *page_set_found;
	struct chunk chunk_requested = { .size = size }, *chunk_found;

	page_set_found = lib_listof(struct page_set, listnode,
				    lib_listFind(&malloc_common.page_sets, &page_set_requested.listnode));
	if (!page_set_found) {
		page_set_found = alloc_page_set(size);
		if (!page_set_found) { /* out of memory */
			return NULL;
		}
	}

	chunk_found = lib_listof(struct chunk, listnode,
				 lib_listFind(&page_set_found->chunks, &chunk_requested.listnode));
	assert(chunk);
	return cut_chunk(page_set_found, chunk_found, size);
}

void *malloc(size_t size)
{
	void *result;

	if (!size)
		return NULL;

	size = ALIGN_CEIL(size);

	if (!malloc_common.mutex)
		mutex(&malloc_common.mutex);
	lock(malloc_common.mutex);
	result = allocate(size);
	unlock(malloc_common.mutex);
	return result;
}

/* Check and coalesce if possible first_chunk and its successor */
static void coalesce(struct page_set *page_set, struct chunk *first_chunk)
{
	struct chunk *next_chunk;

	if (!first_chunk)
		return;

	next_chunk = lib_listof(struct chunk, listnode, first_chunk->listnode.next);
	if (!next_chunk)
		return;

	if (first_chunk->free && next_chunk->free) {
		lib_listRemove(&page_set->chunks, &next_chunk->listnode);
		first_chunk->size += next_chunk->size + sizeof(struct chunk);
	}
}

static void release_chunk(struct page_set *page_set, struct chunk *chunk)
{
	chunk->free = 1;
	coalesce(page_set, chunk);
	coalesce(page_set, lib_listof(struct chunk, listnode, chunk->listnode.prev));
	if (lib_listSize(&page_set->chunks) == 1) {
		lib_listRemove(&malloc_common.page_sets, &page_set->listnode);
		munmap(page_set, page_set->total_size);
	} else {
		update_page_set(page_set);
	}
}

void free(void *ptr)
{
	struct chunk *freed_chunk;
	struct page_set *page_set;

	if (!ptr)
		return;

	if (!malloc_common.mutex)
		mutex(&malloc_common.mutex);
	lock(malloc_common.mutex);
	freed_chunk = chunk_of_contents(ptr);
	page_set = page_set_of_chunk(freed_chunk);
	release_chunk(page_set, freed_chunk);
	unlock(malloc_common.mutex);
}

void *realloc(void *ptr, size_t size)
{
	void *result;
	struct chunk *current_chunk, *next_chunk;
	struct page_set *page_set;


	if (!ptr)
		return malloc(size);

	if (!size) {
		free(ptr);
		return NULL;
	}

	size = ALIGN_CEIL(size);

	if (!malloc_common.mutex)
		mutex(&malloc_common.mutex);
	lock(malloc_common.mutex);
	current_chunk = chunk_of_contents(ptr);
	page_set = page_set_of_chunk(current_chunk);
	if (current_chunk->size >= size) {
		current_chunk->free = 1; /* to satisfy cut */
		result = cut_chunk(page_set, current_chunk, size);
		coalesce(page_set, lib_listof(struct chunk, listnode, current_chunk->listnode.next));
		update_page_set(page_set);
	} else if (next_chunk = lib_listof(struct chunk, listnode, current_chunk->listnode.next),
		   next_chunk && next_chunk->free &&
		   current_chunk->size + sizeof(struct chunk) + next_chunk->size >= size) {
		current_chunk->free = 1; /* to satisfy coalesce and cut */
		coalesce(page_set, current_chunk);
		result = cut_chunk(page_set, current_chunk, size);
		update_page_set(page_set);
	} else {
		result = allocate(size);
		if (result) {
			memcpy(result, current_chunk, current_chunk->size);
			release_chunk(page_set, current_chunk);
		}
	}
	unlock(malloc_common.mutex);
	return result;
}

void *calloc(size_t nmemb, size_t size)
{
	void *result;

	if (!nmemb || !size)
		return NULL;

	size = ALIGN_CEIL(nmemb * size);

	if (!malloc_common.mutex)
		mutex(&malloc_common.mutex);
	lock(malloc_common.mutex);
	result = allocate(size);
	if (result)
		memset(result, 0, size);
	unlock(malloc_common.mutex);
	return result;
}
