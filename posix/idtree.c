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

#include <sys/minmax.h>
#include <sys/rb.h>

#include "posix/idtree.h"


static int idtree_cmp(rbnode_t *n1, rbnode_t *n2)
{
	idnode_t *i1 = lib_treeof(idnode_t, linkage, n1);
	idnode_t *i2 = lib_treeof(idnode_t, linkage, n2);

	return (i1->id - i2->id);
}


static int idtree_gapcmp(rbnode_t *n1, rbnode_t *n2)
{
	idnode_t *r1 = lib_treeof(idnode_t, linkage, n1);
	idnode_t *r2 = lib_treeof(idnode_t, linkage, n2);
	rbnode_t *child = NULL;
	int ret = 1;

	if (r1->lmaxgap > 0 && r1->rmaxgap > 0) {
		if (r2->id > r1->id) {
			child = n1->right;
			ret = -1;
		}
		else {
			child = n1->left;
			ret = 1;
		}
	}
	else if (r1->lmaxgap > 0) {
		child = n1->left;
		ret = 1;
	}
	else if (r1->rmaxgap > 0) {
		child = n1->right;
		ret = -1;
	}

	if (child == NULL)
		return 0;

	return ret;
}


static void idtree_augment(rbnode_t *node)
{
	rbnode_t *it;
	idnode_t *n = lib_treeof(idnode_t, linkage, node);
	idnode_t *p = n, *r, *l;

	if (node->left == NULL) {
		for (it = node; it->parent != NULL; it = it->parent) {
			p = lib_treeof(idnode_t, linkage, it->parent);
			if (it->parent->right == it)
				break;
		}

		n->lmaxgap = (n->id <= p->id) ? n->id : n->id - p->id - 1;
	}
	else {
		l = lib_treeof(idnode_t, linkage, node->left);
		n->lmaxgap = max(l->lmaxgap, l->rmaxgap);
	}

	if (node->right == NULL) {
		for (it = node; it->parent != NULL; it = it->parent) {
			p = lib_treeof(idnode_t, linkage, it->parent);
			if (it->parent->left == it)
				break;
		}

		n->rmaxgap = (n->id >= p->id) ? (unsigned)-1 - n->id - 1 : p->id - n->id - 1;
	}
	else {
		r = lib_treeof(idnode_t, linkage, node->right);
		n->rmaxgap = max(r->lmaxgap, r->rmaxgap);
	}

	for (it = node; it->parent != NULL; it = it->parent) {
		n = lib_treeof(idnode_t, linkage, it);
		p = lib_treeof(idnode_t, linkage, it->parent);

		if (it->parent->left == it)
			p->lmaxgap = max(n->lmaxgap, n->rmaxgap);
		else
			p->rmaxgap = max(n->lmaxgap, n->rmaxgap);
	}
}


void idtree_init(idtree_t *tree)
{
	lib_rbInit(tree, idtree_cmp, idtree_augment);
}


idnode_t *idtree_find(idtree_t *tree, int id)
{
	idnode_t n;
	n.id = id;
	return lib_treeof(idnode_t, linkage, lib_rbFind(tree, &n.linkage));
}


void idtree_remove(idtree_t *tree, idnode_t *node)
{
	lib_rbRemove(tree, &node->linkage);
}


int idtree_id(idnode_t *node)
{
	return node->id;
}


int idtree_alloc(idtree_t *tree, idnode_t *n)
{
	idnode_t *f;

	n->id = 0;
	if (tree->root != NULL) {
		f = lib_treeof(idnode_t, linkage, lib_rbFindEx(tree->root, &n->linkage, idtree_gapcmp));

		if (f != NULL) {
			if (f->lmaxgap > 0)
				n->id = f->id - 1;
			else
				n->id = f->id + 1;
		}
		else {
			return -1;
		}
	}

	lib_rbInsert(tree, &n->linkage);
	return n->id;
}
