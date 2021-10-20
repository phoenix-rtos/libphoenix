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

	if (stat(canonical, &s) < 0 || !S_ISDIR(s.st_mode)) {
		free(canonical);
		return SET_ERRNO(-ENOENT);
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
	char *p = path + 1; /* skip first '/' */
	char *r = result;
	char *slash;
	char *const res_end = result + PATH_MAX;
	int is_leaf = 0;
	int symlinkcnt = 0;

	assert(path && path[0] == '/');

	*r++ = '/';

	while (!is_leaf) {
		slash = strchrnul(p, '/');
		if (*slash == 0) {
			is_leaf = 1; /* last path node */
		}
		else {
			char *rem = slash;
			while (*rem == '/')
				rem++;
			if (*rem == 0)
				is_leaf = 1; /* trailing '/' */

			*slash = 0;
		}

		/* check for '.' and '..' */
		if (p[0] == 0) { /* multiple '/' */
			r -= 1;
		}
		else if (p[0] == '.' && p[1] == 0) { /* '.' dir */
			r -= 1;
		}
		else if (p[0] == '.' && p[1] == '.' && p[2] == 0) { /* '..' dir */
			r -= 1;
			while (r > result && *--r != '/')
				;
		}
		else { /* 'normal' path node */
			char tmp_buf[PATH_MAX];
			ssize_t symlink_len;

			/* check for buffer overflows */
			if (r + (slash - p + 1) > res_end)
				return SET_ERRNO(-ENAMETOOLONG);

			memcpy(r, p, (slash - p + 1));

			if (is_leaf && !resolve_last_symlink) {
				r += slash - p; /* commit node path */

				/* WARN: slight inconsistency: we're not checking if the path actually exists in this case (TODO?) */
			}
			/* (hackish) save some messsaging by not calling lstat, but directly readlink() and checking error code */
			else if ((symlink_len = _readlink_abs(result, tmp_buf, sizeof(tmp_buf) - 1)) < 0) { /* reserve space for \0 */

				if ((errno == EINVAL) || ((errno == ENOENT) && is_leaf && allow_missing_leaf)) { /* not a link or non-esixting leaf */
					r += slash - p;
				}
				else {
					/* errno set by _readlink_abs */
					return -1;
				}
			}
			else {
				if (++symlinkcnt > SYMLOOP_MAX)
					return SET_ERRNO(-ELOOP);

				if (!is_leaf) { /* append remaining original path suffix */
					*slash = '/';
					int rempath_len = strlen(slash);
					if (symlink_len + rempath_len + 1 >= PATH_MAX) {
						return SET_ERRNO(-ENAMETOOLONG);
					}
					memcpy(tmp_buf + symlink_len, slash, rempath_len + 1);
					symlink_len += rempath_len;
				}

				/* reuse full path buffer */
				memcpy(path, tmp_buf, symlink_len);
				path[symlink_len] = 0;
				p = path;

				if (tmp_buf[0] == '/') { /* ABS symlink - reset result to '/' */
					r = result + 1;
				}
				/* else: relative symlink - keep current result */

				/* keep parsing resolved symlink */
				is_leaf = 0;
				continue;
			}
		}

		if (!is_leaf) {
			*r++ = '/';
			p = slash + 1;
		}
	}

	/* guard against corner cases */
	if (r == result)
		*r++ = '/';

	*r = 0;
	return r - result;
}


/* Resolving path to absolute paths with '.', '..' and symlinks support. Additional params to satisfy multiple libc use cases.
 *   resolved_path: if NULL, will be allocated by malloc()
 *   allow_missing_leaf: don't return ENOENT if last path node is not existing
 *   resolve_last_symlink: if 0 return final symlink path instead of symlink destination
 * NOTE: stack usage: ~PATH_MAX
 */
char *resolve_path(const char *path, char *resolved_path, int resolve_last_symlink, int allow_missing_leaf)
{
	char *alloc_resolved_path = NULL; /* internally allocated path needed to be freed on error */
	char *path_copy, *p;
	size_t pathlen;

	if (!path || !path[0]) {
		errno = EINVAL;
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
		if ((s->dirent = calloc(1, sizeof(struct dirent) + NAME_MAX)) == NULL)
			return NULL;
	}

	msg.type = mtReaddir;
	msg.i.readdir.offs = s->pos;

	memcpy(&msg.i.readdir.dir, &s->oid, sizeof(oid_t));
	msg.o.data = s->dirent;
	msg.o.size = sizeof(struct dirent) + NAME_MAX;

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

	if (!dirname[0] || (safe_lookup(canonical_name, &s->oid, NULL) < 0)) {
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

	if (msg.o.io.err <= 0)
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
	oid_t dir;
	msg_t msg = { 0 };
	char *canonical_name, *dirname, *name;

	if ((canonical_name = resolve_path(path, NULL, 1, 0)) == NULL)
		return -1; /* errno set by resolve_path */

	splitname(canonical_name, &name, &dirname);

	if (safe_lookup(dirname, &dir, NULL)) {
		free(canonical_name);
		return SET_ERRNO(-ENOENT);
	}

	msg.type = mtUnlink;
	memcpy(&msg.i.ln.dir, &dir, sizeof(dir));
	msg.i.data = name;
	msg.i.size = strlen(name) + 1;

	if (msgSend(dir.port, &msg) != EOK) {
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
