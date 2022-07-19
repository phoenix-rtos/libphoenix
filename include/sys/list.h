/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * Doubly-linked list
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Pawel Pisarczyk, Jan Sikorski, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_SYS_LIST_H_
#define _LIBPHOENIX_SYS_LIST_H_

#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif


extern void lib_listAdd(void **list, void *t, size_t next_off, size_t prev_off);


extern void lib_listRemove(void **list, void *t, size_t next_off, size_t prev_off);


#ifdef __cplusplus
}
#endif


#define LIST_ADD_EX(list, t, next, prev) \
	lib_listAdd((void **)(list), (void *)(t), (uintptr_t)&(((typeof(t))0)->next), (uintptr_t)&(((typeof(t))0)->prev))


#define LIST_ADD(list, t) LIST_ADD_EX(list, t, next, prev)


#define LIST_REMOVE_EX(list, t, next, prev) \
	lib_listRemove((void **)(list), (void *)(t), (uintptr_t)&(((typeof(t))0)->next), (uintptr_t)&(((typeof(t))0)->prev))


#define LIST_REMOVE(list, t) LIST_REMOVE_EX(list, t, next, prev)


#endif
