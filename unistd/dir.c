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
#include <string.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <posix/utils.h>


static struct {
	char *cwd;
} dir_common;


int chdir(const char *path)
{
	struct stat s;
	int len;

	if (stat(path, &s) < 0 || !S_ISDIR(s.st_mode))
		return -1;

	len = strlen(path) + 1;
	if (realloc(dir_common.cwd, len) == NULL)
		return -1; /* ENOMEM */

	memcpy(dir_common.cwd, path, len);

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

	if (buf == NULL)
		buf = malloc(len);
	else if (size < len)
		return NULL; /* ERANGE */

	memcpy(buf, dir_common.cwd, len + 1);
	return buf;
}


/* TODO: resolve links, handle '//', '.' and '..' */
char *canonicalize_file_name(const char *path)
{
	char *buf;
	int cwdlen = getcwd_len();
	int pathlen;

	if (path == NULL)
		return NULL;

	pathlen = strlen(path);

	if (*path != '/') {
		if ((buf = malloc(cwdlen + pathlen + 3)) == NULL)
			return NULL; /* ENOMEM */

		buf = getcwd(buf, cwdlen);
		if (buf[cwdlen - 1] != '/') {
			buf[cwdlen] = '/';
			buf[cwdlen + 1] = 0;
		}
		buf = strcat(buf, path);

		pathlen += cwdlen + 1;
	}
	else {
		if ((buf = malloc(pathlen + 2)) == NULL)
			return NULL; /* ENOMEM */

		strcpy(buf, path);
	}

	if (buf[pathlen - 1] == '/') {
		buf[pathlen] = '.';
		buf[pathlen + 1] = 0;
	}

	return buf;
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
	char *canonical_name = canonicalize_file_name(dirname);
	DIR *s = calloc(1, sizeof(DIR));

	if (!dirname[0] || (lookup((char *)canonical_name, NULL, &s->oid) < 0)) {
		free(s);
		return NULL; /* ENOENT */
	}

	free(canonical_name);
	s->dirent = NULL;

#if 1
	memset(&msg, 0, sizeof(msg));
	msg.type = mtGetAttr;
	msg.i.attr.type = /*atType*/ 4;
	memcpy(&msg.i.attr.oid, &s->oid, sizeof(oid_t));

	if (msgSend(s->oid.port, &msg) < 0) {
		free(s);
		return NULL; /* EIO */
	}

	if (msg.o.attr.val != /* Dir */ 0)
		return NULL; /* ENOTDIR */
#endif

	memset(&msg, 0, sizeof(msg));
	msg.type = mtOpen;
	memcpy(&msg.i.openclose.oid, &s->oid, sizeof(oid_t));
	msg.i.openclose.flags = 0;

	if (msgSend(s->oid.port, &msg) < 0) {
		free(s);
		return NULL; /* EIO */
	}

	if (msg.o.io.err < 0) {
		free(s);
		return NULL;
	}

	return s;
}


int closedir(DIR *dirp)
{
	msg_t msg = { 0 };

	msg.type = mtClose;
	memcpy(&msg.i.openclose.oid, &dirp->oid, sizeof(oid_t));

	if (msgSend(dirp->oid.port, &msg) < 0)
		return -1; /* EIO */

	if (msg.o.io.err < 0)
		return -1;

	free(dirp->dirent);
	free(dirp);

	return 0;
}


ssize_t readlink(const char *pathname, char *buf, size_t bufsiz)
{
	return 0;
}


int rmdir(const char *path)
{
	oid_t dir;
	msg_t msg = { 0 };
	char *canonical_name, *dirname, *name;

	if (path == NULL)
		return -1;

	if ((canonical_name = canonicalize_file_name(path)) == NULL)
		return -1;

	splitname(canonical_name, &name, &dirname);

	if (lookup(dirname, NULL, &dir)) {
		free(canonical_name);
		return -1;
	}

	msg.type = mtUnlink;
	memcpy(&msg.i.ln.dir, &dir, sizeof(dir));
	msg.i.data = name;
	msg.i.size = strlen(name) + 1;

	if (msgSend(dir.port, &msg) != EOK) {
		free(canonical_name);
		return -1;
	}

	free(canonical_name);

	if (msg.o.io.err < 0)
		return -1;

	return 0;
}


char *dirname(char *path)
{
	char *slash;

	if ((slash = strrchr(path, '/')) == NULL)
		return ".";

	if (slash != path) {
		*slash = 0;
		return path;
	}

	return "/";
}
