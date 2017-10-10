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
#include "sys/threads.h"
#include "sys/mman.h"

#include ARCH

/*
 * global data: list of page_sets protected by mutex if application is multi-threaded
 * data structures: page_set contains list of chunks
 *
 * malloc steps:
 * 1. find the first chunk large enough to fit aligned size;
 * 2. if no such chunk exists, mmap new page_set with appropriate size;
 * 3. cut off new chunk from the one found or mmap'ed and return its contents to the user.
 *
 * free steps:
 * 1. find chunk based on address;
 * 2. set it free and check if it can be coalesced with any neighbour;
 * 3. check if changes performed allows parent page_set to be munmap'ed;
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

typedef struct listnode {
	struct listnode *prev, *next;
} listnode_t;

typedef struct list {
	struct listnode *head, *tail;
	int (*compare)(struct listnode *a, struct listnode *b);
	size_t size;
} list_t;

#define LIST_OF(container, field, instance)				\
	((container *)((unsigned char *)instance - (__builtin_offsetof(container, field))))

#define LIST_INIT(list, cmp_func) do {		\
		(list)->head = NULL;		\
		(list)->tail = NULL;		\
		(list)->compare = (cmp_func);	\
		(list)->size = 0;		\
	} while (0)

#define LIST_INSERT_AFTER(list, previous, toinsert) do {	\
		(toinsert)->prev = (previous);			\
		if ((previous)->next == NULL) {			\
			(toinsert)->next = NULL;		\
			(list)->tail = (toinsert);		\
		} else {					\
			(toinsert)->next = (previous)->next;	\
			(previous)->next->prev = (toinsert);	\
		}						\
		(previous)->next = (toinsert);			\
		++(list)->size;					\
	} while (0)

#define LIST_INSERT_BEFORE(list, successor, toinsert) do {	\
		(toinsert)->next = (successor);			\
		if ((successor)->prev == NULL) {		\
			(toinsert)->prev = NULL;		\
			(list)->head = (toinsert);		\
		} else {					\
			(toinsert)->prev = (successor)->prev;	\
			(successor)->prev->next = (toinsert);	\
		}						\
		(successor)->prev = (toinsert);			\
		++(list)->size;					\
	} while (0)

#define LIST_PREPEND(list, toinsert) do {				\
		if ((list)->head == NULL) {				\
			(list)->head = (toinsert);			\
			(list)->tail = (toinsert);			\
			(toinsert)->prev = NULL;			\
			(toinsert)->next = NULL;			\
			++(list)->size;					\
		} else {						\
			LIST_INSERT_BEFORE((list), (list)->head, (toinsert)); \
		}							\
	} while (0)

#define LIST_APPEND(list, toinsert) do {				\
		if ((list)->tail == NULL)				\
			LIST_PREPEND((list), (toinsert));		\
		else							\
			LIST_INSERT_AFTER((list), (list)->tail, (toinsert)); \
	} while (0)

#define LIST_REMOVE(list, todelete) do {				\
		if ((todelete)->prev == NULL) {				\
			(list)->head = (todelete)->next;		\
			if ((list)->head != NULL)			\
				(list)->head->prev = NULL;		\
		} else							\
			(todelete)->prev->next = (todelete)->next;	\
		if ((todelete)->next == NULL) {				\
			(list)->tail = (todelete)->prev;		\
			if ((list)->tail != NULL)			\
				(list)->tail->next = NULL;		\
		} else							\
			(todelete)->next->prev = (todelete)->prev;	\
		--(list)->size;						\
	} while (0)

#define LIST_FIND(list, tofind, found) do {				\
		listnode_t *it = (list)->head;				\
		for (it = (list)->head; it != NULL; it = it->next) {	\
			if ((list)->compare(it, (tofind)) == 0)		\
				break;					\
		}							\
		(found) = it;						\
	} while (0)




static int _page_set_fit(listnode_t *page_set_node, listnode_t *request_node);
static int _chunk_fit(listnode_t *chunk_node, listnode_t *request_node);

struct malloc_common {
	handle_t mutex;
	list_t page_sets;
};

struct malloc_common malloc_common;

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

static struct page_set *_page_set_of_chunk(struct chunk *chunk)
{
	listnode_t *current = &chunk->listnode;
	struct chunk *first;
	while (current->prev)
		current = current->prev;
	first = LIST_OF(struct chunk, listnode, current);
	return (struct page_set *)((void *)first - offsetof(struct page_set, first_chunk));
}

#define MAP_FAILED	((void *)-1)

#define CEIL_POWOF2(x, roundto) (((x) + (roundto) - 1) & ~((roundto) - 1))
#define PAGE_CEIL(x) CEIL_POWOF2((x), SIZE_PAGE)
#define ALIGN_CEIL(x) CEIL_POWOF2((x), __BIGGEST_ALIGNMENT__)

static int _page_set_fit(listnode_t *page_set_node, listnode_t *request_node)
{
	struct page_set *set = LIST_OF(struct page_set, listnode, page_set_node),
		*request = LIST_OF(struct page_set, listnode, request_node);

	if (set->max_free_chunk_size >= request->max_free_chunk_size)
		return 0;

	return 1;
}

static int _chunk_fit(listnode_t *chunk_node, listnode_t *request_node)
{
	struct chunk *chunk = LIST_OF(struct chunk, listnode, chunk_node),
		*request = LIST_OF(struct chunk, listnode, request_node);

	if (chunk->free && chunk->size >= request->size)
		return 0;

	return 1;
}

static struct page_set *_alloc_page_set(size_t size)
{
	oid_t dummy = { 0, 0 };
	size_t page_set_total_size = PAGE_CEIL(offsetof(struct page_set, first_chunk.contents) + size);
	struct page_set *result = mmap(NULL, page_set_total_size, PROT_READ | PROT_WRITE,
				       MAP_PRIVATE | MAP_ANONYMOUS, &dummy, 0);
	if (result == MAP_FAILED) /* out of memory */
		return NULL;

	result->total_size = page_set_total_size;
	LIST_INIT(&result->chunks, _chunk_fit);
	LIST_APPEND(&result->chunks, &result->first_chunk.listnode);
	result->first_chunk.size = result->max_free_chunk_size =
		page_set_total_size - offsetof(struct page_set, first_chunk.contents);
	result->first_chunk.free = 1;

	LIST_APPEND(&malloc_common.page_sets, &result->listnode);
	return result;
}

/* update max free chunk size in page_set structure */
static void _update_page_set(struct page_set *page_set)
{
	size_t max_free_chunk_size = 0;
	listnode_t *current_node;
	struct chunk *current_chunk;

	for (current_node = page_set->chunks.head; current_node != NULL;
	     current_node = current_node->next) {
		current_chunk = LIST_OF(struct chunk, listnode, current_node);
		if (current_chunk->free && max_free_chunk_size < current_chunk->size)
			max_free_chunk_size = current_chunk->size;
	}

	page_set->max_free_chunk_size = max_free_chunk_size;
}

static void *_cut_chunk(struct page_set *page_set, struct chunk *chunk, size_t size)
{
	struct chunk *remainder;

	assert(chunk->free);
	if (chunk->size > size + sizeof(struct chunk) + ALIGN_CEIL(1)) {
		remainder = (struct chunk *)(chunk->contents + size);
		remainder->size = chunk->size - size - sizeof(struct chunk);
		remainder->free = 1;
		LIST_INSERT_AFTER(&page_set->chunks, &chunk->listnode, &remainder->listnode);
		chunk->size = size;
	}
	chunk->free = 0;
	_update_page_set(page_set);
	return chunk->contents;
}

static void *_allocate(size_t size) {
	struct page_set page_set_requested = { .max_free_chunk_size = size }, *page_set_found;
	struct chunk chunk_requested = { .size = size }, *chunk_found;
	listnode_t *page_set_node_found, *chunk_node_found;

	LIST_FIND(&malloc_common.page_sets, &page_set_requested.listnode, page_set_node_found);

	if (page_set_node_found == NULL) {
		page_set_found = _alloc_page_set(size);
		if (page_set_found == NULL) { /* out of memory */
			return NULL;
		}
	} else {
		page_set_found = LIST_OF(struct page_set, listnode, page_set_node_found);
	}

	LIST_FIND(&page_set_found->chunks, &chunk_requested.listnode, chunk_node_found);
	assert(chunk_node_found != NULL);
	chunk_found = LIST_OF(struct chunk, listnode, chunk_node_found);
	return _cut_chunk(page_set_found, chunk_found, size);
}

void *malloc(size_t size)
{
	void *result;

	if (size == 0)
		return NULL;

	size = ALIGN_CEIL(size);

	mutexLock(malloc_common.mutex);
	result = _allocate(size);
	mutexUnlock(malloc_common.mutex);
	return result;
}

/* Check and coalesce if possible first_chunk and its successor */
static void _coalesce(struct page_set *page_set, struct chunk *first_chunk)
{
	struct chunk *next_chunk;

	if (first_chunk == NULL)
		return;

	next_chunk = LIST_OF(struct chunk, listnode, first_chunk->listnode.next);
	if (next_chunk == NULL)
		return;

	if (first_chunk->free && next_chunk->free) {
		LIST_REMOVE(&page_set->chunks, &next_chunk->listnode);
		first_chunk->size += next_chunk->size + sizeof(struct chunk);
	}
}

static void _release_chunk(struct page_set *page_set, struct chunk *chunk)
{
	chunk->free = 1;
	_coalesce(page_set, chunk);
	_coalesce(page_set, LIST_OF(struct chunk, listnode, chunk->listnode.prev));
	if (page_set->chunks.size == 1) {
		LIST_REMOVE(&malloc_common.page_sets, &page_set->listnode);
		munmap(page_set, page_set->total_size);
	} else {
		_update_page_set(page_set);
	}
}

void free(void *ptr)
{
	struct chunk *freed_chunk;
	struct page_set *page_set;

	if (ptr == NULL)
		return;

	mutexLock(malloc_common.mutex);
	freed_chunk = chunk_of_contents(ptr);
	page_set = _page_set_of_chunk(freed_chunk);
	_release_chunk(page_set, freed_chunk);
	mutexUnlock(malloc_common.mutex);
}

void *realloc(void *ptr, size_t size)
{
	void *result;
	struct chunk *current_chunk, *next_chunk;
	struct page_set *page_set;


	if (ptr == NULL)
		return malloc(size);

	if (size == 0) {
		free(ptr);
		return NULL;
	}

	size = ALIGN_CEIL(size);

	mutexLock(malloc_common.mutex);
	current_chunk = chunk_of_contents(ptr);
	page_set = _page_set_of_chunk(current_chunk);
	if (current_chunk->size >= size) {
		current_chunk->free = 1; /* to satisfy cut */
		result = _cut_chunk(page_set, current_chunk, size);
		_coalesce(page_set, LIST_OF(struct chunk, listnode, current_chunk->listnode.next));
		_update_page_set(page_set);
	} else if (next_chunk = LIST_OF(struct chunk, listnode, current_chunk->listnode.next),
		   next_chunk != NULL && next_chunk->free &&
		   current_chunk->size + sizeof(struct chunk) + next_chunk->size >= size) {
		current_chunk->free = 1; /* to satisfy coalesce and cut */
		_coalesce(page_set, current_chunk);
		result = _cut_chunk(page_set, current_chunk, size);
		_update_page_set(page_set);
	} else {
		result = _allocate(size);
		if (result != NULL) {
			memcpy(result, current_chunk, current_chunk->size);
			_release_chunk(page_set, current_chunk);
		}
	}
	mutexUnlock(malloc_common.mutex);
	return result;
}

void *calloc(size_t nmemb, size_t size)
{
	void *result;

	if (nmemb == 0 || size == 0)
		return NULL;

	size = ALIGN_CEIL(nmemb * size);

	mutexLock(malloc_common.mutex);
	result = _allocate(size);
	if (result != NULL)
		memset(result, 0, size);
	mutexUnlock(malloc_common.mutex);
	return result;
}

void _malloc_init(void)
{
	malloc_common.page_sets.compare = _page_set_fit;
	mutexCreate(&malloc_common.mutex);
}
