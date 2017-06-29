/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * sys/rb
 *
 * Copyright 2017 Phoenix Systems
 * Author: Jakub Sejdak
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "rb.h"
#include "errno.h"
#include "string.h"

rbnode_t nil;


void lib_rbInit(rbtree_t *tree, rbcomp_t compare, rbaugment_t augment)
{
	nil.color = RB_BLACK;

	tree->root = &nil;
	tree->compare = compare;
	tree->augment = augment;
}


static inline void rb_augment(rbtree_t *tree, rbnode_t *node)
{
	if (node == &nil)
		return;

	if (tree->augment != NULL)
		tree->augment(node);
}


static void rb_rotateLeft(rbtree_t *tree, rbnode_t *x)
{
	rbnode_t *y = x->right;
	x->right = y->left;

	if (y->left != &nil)
		y->left->parent = x;

	y->parent = x->parent;
	if (x->parent == &nil)
		tree->root = y;
	else if (x == x->parent->left)
		x->parent->left = y;
	else
		x->parent->right = y;

	y->left = x;
	x->parent = y;

	rb_augment(tree, y->left);
	rb_augment(tree, y->right);
}


static void rb_rotateRight(rbtree_t *tree, rbnode_t *x)
{
	rbnode_t *y = x->left;
	x->left = y->right;

	if (y->right != &nil)
		y->right->parent = x;

	y->parent = x->parent;
	if (x->parent == &nil)
		tree->root = y;
	else if (x == x->parent->right)
		x->parent->right = y;
	else
		x->parent->left = y;

	y->right = x;
	x->parent = y;

	rb_augment(tree, y->left);
	rb_augment(tree, y->right);
}


static void lib_rbInsertBalance(rbtree_t *tree, rbnode_t *node)
{
	rbnode_t *z = node;
	rbnode_t *y;
 
	while (z->parent->color == RB_RED) {
		if (z->parent == z->parent->parent->left) {
			y = z->parent->parent->right;
			if (y->color == RB_RED) {
				z->parent->color = RB_BLACK;
				y->color = RB_BLACK;
				z->parent->parent->color = RB_RED;
				z = z->parent->parent;
			}
			else if (z == z->parent->right) {
				z = z->parent;
				rb_rotateLeft(tree, z);
			}
			else {
				z->parent->color = RB_BLACK;
				z->parent->parent->color = RB_RED;
				rb_rotateRight(tree, z->parent->parent);
			}
		}
		else {
			y = z->parent->parent->left;
			if (y->color == RB_RED) {
				z->parent->color = RB_BLACK;
				y->color = RB_BLACK;
				z->parent->parent->color = RB_RED;
				z = z->parent->parent;
			}
			else if (z == z->parent->left) {
				z = z->parent;
				rb_rotateRight(tree, z);
			}
			else {
				z->parent->color = RB_BLACK;
				z->parent->parent->color = RB_RED;
				rb_rotateLeft(tree, z->parent->parent);
			}
		}
	}

	tree->root->color = RB_BLACK;
}


static void lib_rbRemoveBalance(rbtree_t *tree, rbnode_t *node)
{
	rbnode_t *x = node;
	rbnode_t *w;

	while (x != tree->root && x->color == RB_BLACK) {
		if (x == x->parent->left) {
			w = x->parent->right;
			if (w->color == RB_RED) {
				w->color = RB_BLACK;
				x->parent->color = RB_RED;
				rb_rotateLeft(tree, x->parent);
				w = x->parent->right;
			}

			if (w->left->color == RB_BLACK && w->right->color == RB_BLACK) {
				w->color = RB_RED;
				x = x->parent;
			}
			else if (w->right->color == RB_BLACK) {
				w->left->color = RB_BLACK;
				w->color = RB_RED;
				rb_rotateRight(tree, w);
				w = x->parent->right;
			}
			else {
				w->color = x->parent->color;
				x->parent->color = RB_BLACK;
				w->right->color = RB_BLACK;
				rb_rotateLeft(tree, x->parent);
				x = tree->root;
			}
		}
		else {
			w = x->parent->left;
			if (w->color == RB_RED) {
				w->color = RB_BLACK;
				x->parent->color = RB_RED;
				rb_rotateRight(tree, x->parent);
				w = x->parent->left;
			}

			if (w->right->color == RB_BLACK && w->left->color == RB_BLACK) {
				w->color = RB_RED;
				x = x->parent;
			}
			else if (w->left->color == RB_BLACK) {
				w->right->color = RB_BLACK;
				w->color = RB_RED;
				rb_rotateLeft(tree, w);
				w = x->parent->left;
			}
			else {
				w->color = x->parent->color;
				x->parent->color = RB_BLACK;
				w->left->color = RB_BLACK;
				rb_rotateRight(tree, x->parent);
				x = tree->root;
			}
		}
	}

	x->color = RB_BLACK;
}


static void rb_transplant(rbtree_t *tree, rbnode_t *u, rbnode_t *v)
{
	if (u->parent != &nil) {
		if (u == u->parent->left)
			u->parent->left = v;
		else
			u->parent->right = v;

		rb_augment(tree, u->parent);
	}
	else
		tree->root = v;

	v->parent = u->parent;
	rb_augment(tree, v);
}


int lib_rbInsert(rbtree_t *tree, rbnode_t *z)
{
	rbnode_t *y = &nil;
	rbnode_t *x = tree->root;
	int c;

	while (x != &nil) {
		y = x;

		c = tree->compare(y, z);
		if (c == 0)
			return -EEXIST;

		x = (c > 0) ? x->left : x->right;
	}

	z->parent = y;
	if (y == &nil)
		tree->root = z;
	else if (c > 0)
		y->left = z;
	else
		y->right = z;

	z->left = &nil;
	z->right = &nil;
	z->color = RB_RED;

	rb_augment(tree, z);
	lib_rbInsertBalance(tree, z);
	return EOK;
}


void lib_rbRemove(rbtree_t *tree, rbnode_t *z)
{
	rbnode_t *y = z;
	rbnode_t *x;
	rbnode_t *t;
	rbcolor_t c = y->color;

	if (z->left == &nil) {
		x = z->right;
		rb_transplant(tree, z, z->right);
	}
	else if (z->right == &nil) {
		x = z->left;
		rb_transplant(tree, z, z->left);
	}
	else {
		y = lib_rbMinimum(z->right);
		c = y->color;
		x = y->right;

		if (y->parent == z)
			x->parent = y;
		else {
			rb_transplant(tree, y, y->right);
			y->right = z->right;
			y->right->parent = y;
		}

		rb_transplant(tree, z, y);
		y->left = z->left;
		y->left->parent = y;
		y->color = z->color;

		t = lib_rbMaximum(y->left);
		rb_augment(tree, t);

		t = lib_rbMinimum(y->right);
		rb_augment(tree, t);
	}

	if (c == RB_BLACK)
		lib_rbRemoveBalance(tree, x);
}


rbnode_t *lib_rbMinimum(rbnode_t *node)
{
	rbnode_t *x = node;

	if (x == &nil)
		return x;

	while (x->left != &nil)
		x = x->left;

	return x;
}


rbnode_t *lib_rbMaximum(rbnode_t *node)
{
	rbnode_t *x = node;

	if (x == &nil)
		return x;

	while (x->right != &nil)
		x = x->right;

	return x;
}


rbnode_t *lib_rbFind(rbtree_t *tree, rbnode_t *node)
{
	return lib_rbFindEx(tree->root, node, tree->compare);
}


rbnode_t *lib_rbFindEx(rbnode_t *root, rbnode_t *node, rbcomp_t compare)
{
	rbnode_t *it = root;
	int c;

	while (it != &nil) {
		c = compare(it, node);
		if (c == 0)
			break;

		it = (c > 0) ? it->left : it->right;
	}

	return it;
}


#define RB_DUMP_DEPTH	16


void lib_rbDumpEx(rbnode_t *node, rbdump_t dump, unsigned int *depth, unsigned char d[RB_DUMP_DEPTH])
{
	unsigned int i;

	for (i = 0; i < *depth; i++)
		printf("%c   ", d[i] ? '|' : ' ');

	if (node == &nil) {
		printf("%s() *\n", *depth ? "`-" : "");
		return;
	}

	printf("%s(", depth ? "`-" : "");
	dump(node);
	printf(")%c\n", node->color == RB_BLACK ? '*' : ' ');

	(*depth)++;
	if ((node->left != &nil) || (node->right != &nil)) {

		if (*depth < RB_DUMP_DEPTH) {
			d[*depth] = 1;
			lib_rbDumpEx(node->left, dump, depth, d);		
			d[*depth] = 0;
			lib_rbDumpEx(node->right, dump, depth, d);
		}
		else {
			for (i = 0; i < *depth; i++)
				printf("%c   ", d[i] ? '|' : ' ');

			printf("%s(..)\n", *depth ? "`-" : "");
		}
	}
	(*depth)--;
}


void lib_rbDump(rbnode_t *node, rbdump_t dump)
{
	unsigned int depth = 0;
	unsigned char d[RB_DUMP_DEPTH];

	memset(d, 0, RB_DUMP_DEPTH);
	lib_rbDumpEx(node, dump, &depth, d);
}
