/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * id allocating tree
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _LIBPHOENIX_IDTREE_H_
#define _LIBPHOENIX_IDTREE_H_

#include <sys/rb.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef rbtree_t idtree_t;


typedef struct {
	rbnode_t linkage;
	unsigned int lmaxgap, rmaxgap;
	unsigned int id;
} idnode_t;


void idtree_init(idtree_t *tree);


idnode_t *idtree_find(idtree_t *tree, int id);


void idtree_remove(idtree_t *tree, idnode_t *node);


int idtree_id(idnode_t *node);


int idtree_alloc(idtree_t *tree, idnode_t *n);


#ifdef __cplusplus
}
#endif


#endif
