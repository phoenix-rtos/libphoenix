/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd (POSIX routines for directory operations)
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <libgen.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <posix/utils.h>


static struct {
	char *cwd;
} dir_common;

static ssize_t _readlink_abs(const char *path, char *buf, size_t bufsiz);
static int _resolve_abspath(char *path, char *result, int resolve_last_symlink, int allow_missing_leaf);


/* stop-gap for waitpid POSIX incompatibilty (spurious SIGCHLD delivered during other syscalls) */
static int safe_lookup(const char *name, oid_t *file, oid_t *dev)
{
	int err;
	while ((err = lookup(name, file, dev)) == -EINTR)
		;

	return err;
}


int chdir(const char *path)
{
	struct stat s;
	int len;
	char *canonical;

	if ((canonical = resolve_path(path, NULL, 1, 0)) == NULL)
		return -1; /* errno set by resolve_path */

	if (stat(canonical, &s) < 0) {
		free(canonical);
		return SET_ERRNO(-ENOENT);
	}

	if (!S_ISDIR(s.st_mode)) {
		free(canonical);
		return SET_ERRNO(-ENOTDIR);
	}

	/* TODO:
	 * - chdir should symlinks resolved (this is correct)
	 * - PWD should have symlinks unresolved (TODO) and should be maintained by shell, not by libc
	 */

	len = strlen(canonical) + 1;
	if ((dir_common.cwd = realloc(dir_common.cwd, len)) == NULL) {
		free(canonical);
		return SET_ERRNO(-ENOMEM);
	}

	setenv("PWD", canonical, 1);

	memcpy(dir_common.cwd, canonical, len);
	free(canonical);

	return EOK;
}


static int getcwd_len(void)
{
	int len;
	char *wd;

	if (dir_common.cwd == NULL) {
		if ((wd = getenv("PWD")) == NULL)
			wd = "/";

		len = strlen(wd);

		if ((dir_common.cwd = malloc(len + 1)) == NULL)
			return -1;

		memcpy(dir_common.cwd, wd, len + 1);
		return len;
	}

	return strlen(dir_common.cwd);
}


char *getcwd(char *buf, size_t size)
{
	int len = getcwd_len();

	if (len == -1) {
		errno = ENOMEM;
		return NULL;
	}

	if (size == 0) {
		if (buf == NULL) {
			size = len + 1;
		}
		else {
			errno = EINVAL;
			return NULL;
		}
	}
	else if (size < (len + 1)) {
		errno = ERANGE;
		return NULL;
	}

	if (buf == NULL) {
		if ((buf = malloc(size)) == NULL) {
			errno = ENOMEM;
			return NULL;
		}
	}

	memcpy(buf, dir_common.cwd, len + 1);
	return buf;
}


char *canonicalize_file_name(const char *path)
{
	return realpath(path, NULL);
}


/* resolves absolute paths (with symlinks)
 * buffers pointed by path and result need to be at least PATH_MAX in size
 * path might be rewritten as a part of the path resolving */
static int _resolve_abspath(char *path, char *result, int resolve_last_symlink, int allow_missing_leaf)
{
	char *r = result; /* r -  first unused result byte */
	char *p = path;   /* p - first byte of remaining unresolved path */
	int symlinkcnt = 0;

	assert(path && path[0] == '/');

	/* path is shifted right to use its's start for symlink resolution */
	const size_t path_len = strlen(path) + 1; /* including '\0' */
	if (path_len < PATH_MAX) {
		p = path + PATH_MAX - path_len;
		memmove(p, path, path_len);
	}
	p += 1; /* skip initial '/' */

	while (*p != '\0') {
		char *const node = p;
		p = strchrnul(node, '/');
		const size_t node_len = p - node;

		/* skip trailing slashes to properly detect leaf node */
		while (*p == '/') {
			p += 1;
		}

		const int is_leaf = (*p == '\0') ? 1 : 0;

		/* check for '.' and '..' */
		if (node_len == 0) { /* multiple '/' */
			continue;
		}
		if ((node_len == 1) && (node[0] == '.')) { /* '.' dir */
			continue;
		}
		if ((node_len == 2) && (node[0] == '.') && (node[1] == '.')) { /* '..' dir */
			while (r > result && *--r != '/')
				;
			continue;
		}

		if ((r + 1 + node_len + 1) > (result + PATH_MAX)) {
			return SET_ERRNO(-ENAMETOOLONG);
		}

		*r++ = '/';
		memcpy(r, node, node_len);
		r += node_len;
		*r = '\0';

		if ((is_leaf != 0) && (resolve_last_symlink == 0)) {
			/* WARN: slight inconsistency: we're not checking if the path actually exists in this case (TODO?) */
			break;
		}

		/*
		 * Number of initial unused `path` bytes that can be used to resolve current node. If link content is
		 * longer we will not be able to merge it with remaining path and still fit in PATH_MAX buffer
		 */
		const size_t readlink_max_len = p - path;

		/* (hackish) save some messsaging by not calling lstat, but directly readlink() and checking error code */
		ssize_t symlink_len = _readlink_abs(result, path, readlink_max_len);

		if (symlink_len < 0) {
			if (errno == EINVAL) { /* not a symlink */
				continue;
			}
			else if ((errno == ENOENT) && (is_leaf != 0) && (allow_missing_leaf != 0)) { /* non-esixting leaf */
				break;
			}
			else {
				/* errno set by _readlink_abs */
				return -1;
			}
		}

		if (symlink_len == 0) {
			/* NOTE: this case is not defined in POSIX */
			/* see: https://lwn.net/Articles/551224/ */
			return SET_ERRNO(-ENOENT);
		}

		symlinkcnt += 1;
		if (symlinkcnt > SYMLOOP_MAX) {
			return SET_ERRNO(-ELOOP);
		}

		assert(symlink_len <= readlink_max_len);
		if (symlink_len == readlink_max_len) {
			/* `readlink(..., max_len) == max_len` means result could have been truncated */
			return SET_ERRNO(-ENAMETOOLONG);
		}

		/* prepend resolved symlink to remaining unresolved path */
		p -= 1;
		*p = '/';
		p -= symlink_len;
		memmove(p, path, symlink_len);

		if (p[0] == '/') { /* ABS symlink - reset result to '/' */
			p += 1;
			r = result;
		}
		else {
			/* relative symlink - remove resolved symlink node */
			r -= node_len + 1;
		}
	}

	if (r == result)
		*r++ = '/';

	*r = '\0';
	return r - result;
}


/* Resolving path to absolute paths with '.', '..' and symlinks support. Additional params to satisfy multiple libc use cases.
 *   resolved_path: if NULL, will be allocated by malloc()
 *   allow_missing_leaf: don't return ENOENT if last path node is not existing
 *   resolve_last_symlink: if 0 return final symlink path instead of symlink destination
 */
char *resolve_path(const char *path, char *resolved_path, int resolve_last_symlink, int allow_missing_leaf)
{
	char *alloc_resolved_path = NULL; /* internally allocated path needed to be freed on error */
	char *path_copy, *p;
	size_t pathlen;

	if (!path) {
		errno = EINVAL;
		return NULL;
	}

	if (!path[0]) {
		errno = ENOENT;
		return NULL;
	}

	if ((p = path_copy = malloc(PATH_MAX)) == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	pathlen = strlen(path);
	if (path[0] != '/') {
		if (getcwd(path_copy, PATH_MAX) == NULL) {
			/* errno set by getcwd */
			free(path_copy);
			return NULL;
		}

		p = strchr(path_copy, 0);
		*p++ = '/';
	}

	if ((p - path_copy) + pathlen + 1 > PATH_MAX) {
		free(path_copy);
		errno = ENAMETOOLONG;
		return NULL;
	}
	memcpy(p, path, pathlen + 1);

	if (!resolved_path) {
		if ((alloc_resolved_path = malloc(PATH_MAX)) == NULL) {
			free(path_copy);
			errno = ENOMEM;
			return NULL;
		}

		resolved_path = alloc_resolved_path;
	}

	if (_resolve_abspath(path_copy, resolved_path, resolve_last_symlink, allow_missing_leaf) < 0) {
		/* proper errno set by resolve_path */
		free(alloc_resolved_path);
		free(path_copy);
		return NULL;
	}

	free(path_copy);
	return resolved_path;
}


char *realpath(const char *path, char *resolved_path)
{
	/* resolve_last_symlink = 1, allow_missing_leaf = 0 */
	return resolve_path(path, resolved_path, 1, 0);
}


struct dirent *readdir(DIR *s)
{
	msg_t msg;
	memset(&msg, 0, sizeof(msg));

	if (s->dirent == NULL) {
		if ((s->dirent = calloc(1, sizeof(struct dirent) + NAME_MAX + 1)) == NULL)
			return NULL;
	}

	msg.type = mtReaddir;
	msg.i.readdir.offs = s->pos;

	memcpy(&msg.i.readdir.dir, &s->oid, sizeof(oid_t));
	msg.o.data = s->dirent;
	msg.o.size = sizeof(struct dirent) + NAME_MAX + 1;

	if (msgSend(s->oid.port, &msg) < 0) {
		free(s->dirent);
		s->dirent = NULL;
		return NULL; /* EIO */
	}

	if (msg.o.io.err < 0) {
		free(s->dirent);
		s->dirent = NULL;
		return NULL;
	}

	s->pos += s->dirent->d_reclen;

	return s->dirent;
}


DIR *opendir(const char *dirname)
{
	msg_t msg = { 0 };
	char *canonical_name = resolve_path(dirname, NULL, 1, 0);
	DIR *s = calloc(1, sizeof(DIR));

	if ((canonical_name == NULL) || (s == NULL)) {
		free(canonical_name);
		free(s);
		return NULL; /* errno set by resolve_path */
	}

	if (!dirname[0] || (safe_lookup(canonical_name, NULL, &s->oid) < 0)) {
		free(canonical_name);
		free(s);
		errno = ENOENT;
		return NULL; /* ENOENT */
	}

	free(canonical_name);
	s->dirent = NULL;

#if 1
	memset(&msg, 0, sizeof(msg));
	msg.type = mtGetAttr;
	msg.i.attr.type = atType;
	memcpy(&msg.i.attr.oid, &s->oid, sizeof(oid_t));

	if ((msgSend(s->oid.port, &msg) < 0) || (msg.o.attr.err < 0)) {
		free(s);
		errno = EIO;
		return NULL; /* EIO */
	}

	if (msg.o.attr.val != otDir) {
		free(s);
		errno = ENOTDIR;
		return NULL; /* ENOTDIR */
	}
#endif

	memset(&msg, 0, sizeof(msg));
	msg.type = mtOpen;
	memcpy(&msg.i.openclose.oid, &s->oid, sizeof(oid_t));
	msg.i.openclose.flags = 0;

	if (msgSend(s->oid.port, &msg) < 0) {
		free(s);
		errno = EIO;
		return NULL; /* EIO */
	}

	if (msg.o.io.err < 0) {
		free(s);
		errno = EIO;
		return NULL;
	}

	return s;
}


void rewinddir(DIR *dirp)
{
	dirp->pos = 0;
}


int closedir(DIR *dirp)
{
	msg_t msg = { 0 };
	int ret = 0;

	msg.type = mtClose;
	memcpy(&msg.i.openclose.oid, &dirp->oid, sizeof(oid_t));

	if ((msgSend(dirp->oid.port, &msg) < 0) || (msg.o.io.err < 0))
		ret = -1;

	free(dirp->dirent);
	free(dirp);

	return ret;
}


/* readlink without path resolution, to be used internally */
/* WARN: POSIX compliance: does not append '\0' */
static ssize_t _readlink_abs(const char *path, char *buf, size_t bufsiz)
{
	int ret;
	msg_t msg = { 0 };
	oid_t oid;

	assert(path && path[0] == '/');

	if ((ret = safe_lookup(path, &oid, NULL)) < 0)
		return SET_ERRNO(ret);

	msg.type = mtGetAttr;
	memcpy(&msg.i.attr.oid, &oid, sizeof(oid_t));
	msg.i.attr.type = atMode;

	if ((ret = msgSend(oid.port, &msg)) != EOK)
		return SET_ERRNO(ret);

	if (msg.o.attr.err != EOK)
		return SET_ERRNO(msg.o.attr.err);

	if (!S_ISLNK(msg.o.attr.val))
		return SET_ERRNO(-EINVAL);

	memset(&msg, 0, sizeof(msg_t));
	msg.type = mtRead;

	memcpy(&msg.i.io.oid, &oid, sizeof(oid_t));

	msg.o.size = bufsiz;
	msg.o.data = buf;

	if ((ret = msgSend(oid.port, &msg)) != EOK)
		return SET_ERRNO(ret);

	if (msg.o.io.err < 0)
		return SET_ERRNO(msg.o.io.err);

	/* number of bytes written without terminating NULL byte */
	return msg.o.io.err;
}


ssize_t readlink(const char *path, char *buf, size_t bufsiz)
{
	/* resolve_last_symlink = 0 -> read a symlink value */
	char *canonical = resolve_path(path, NULL, 0, 0);
	ssize_t ret;

	if (canonical == NULL)
		return -1; /* errno set by resolve_path */

	ret = _readlink_abs(canonical, buf, bufsiz);

	free(canonical);

	/* if error - errno set by _readlink_abs() */
	return ret;
}


int rmdir(const char *path)
{
	msg_t msg = { 0 };
	oid_t dir, dev;
	char *canonical_name, *dirname, *name;
	struct stat s;

	if ((canonical_name = resolve_path(path, NULL, 1, 0)) == NULL)
		return -1; /* errno set by resolve_path */

	if (stat(canonical_name, &s) < 0) {
		free(canonical_name);
		return SET_ERRNO(-ENOENT);
	}

	if (!S_ISDIR(s.st_mode)) {
		free(canonical_name);
		return SET_ERRNO(-ENOTDIR);
	}

	if (safe_lookup(canonical_name, &dir, &dev)) {
		free(canonical_name);
		return SET_ERRNO(-ENOENT);
	}

	/* Don't remove active mount point */
	if (dir.port != dev.port) {
		free(canonical_name);
		return SET_ERRNO(-EBUSY);
	}

	splitname(canonical_name, &name, &dirname);

	if (safe_lookup(dirname, NULL, &dev)) {
		free(canonical_name);
		return SET_ERRNO(-ENOENT);
	}

	msg.type = mtUnlink;
	memcpy(&msg.i.ln.dir, &dev, sizeof(dev));
	msg.i.data = name;
	msg.i.size = strlen(name) + 1;

	if (msgSend(dev.port, &msg) != EOK) {
		free(canonical_name);
		return SET_ERRNO(-EIO);
	}

	free(canonical_name);

	if (msg.o.io.err < 0)
		return SET_ERRNO(msg.o.io.err);

	return 0;
}


char *dirname(char *path)
{
	size_t len;

	if (path == NULL || (len = strlen(path)) == 0)
		return ".";

	char *last = path + len - 1;

	while ((last >= path) && (*last == '/'))
		--last;

	if (last < path)
		return "/";

	while ((last >= path) && (*last != '/'))
		--last;

	if (last < path)
		return ".";

	while ((last >= path) && (*last == '/'))
		--last;

	if (last < path)
		return "/";

	*(last + 1) = '\0';

	return path;
}


char *basename(char *path)
{
	size_t len;

	if (path == NULL || (len = strlen(path)) == 0)
		return ".";

	char *last = path + len - 1;

	while ((last >= path) && (*last == '/'))
		--last;

	if (last < path)
		return "/";

	*(last + 1) = '\0';

	while ((last >= path) && (*last != '/'))
		--last;

	return (last + 1);
}
