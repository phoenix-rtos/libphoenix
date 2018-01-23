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
#include <sys/msg.h>



int chdir(const char *path)
{
	return EOK;
}


static int getcwd_len(void)
{
	return 1;
}


char *getcwd(char *buf, size_t size)
{
	if (buf == NULL)
		buf = malloc(getcwd_len() + 1);

	buf[0] = '/';
	buf[1] = 0;

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
		if ((buf = malloc(cwdlen + pathlen + 2)) == NULL)
			return NULL; /* ENOMEM */

		buf = strcat(getcwd(buf, cwdlen), path);

		if (buf[cwdlen + pathlen - 1] == '/') {
			buf[cwdlen + pathlen] = '.';
			buf[cwdlen + pathlen + 1] = 0;
		}
	}
	else {
		if ((buf = malloc(pathlen + 2)) == NULL)
			return NULL; /* ENOMEM */

		strcpy(buf, path);

		if (buf[pathlen - 1] == '/') {
			buf[pathlen] = '.';
			buf[pathlen + 1] = 0;
		}
	}

	return buf;
}


struct dirent *readdir(DIR *s)
{
	msg_t msg;

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

	s->pos += sizeof(struct dirent) + s->dirent->d_namlen;

	return s->dirent;
}


DIR *opendir(const char *dirname)
{
	msg_t msg = { 0 };
	char *canonical_name = canonicalize_file_name(dirname);
	DIR *s = calloc(1, sizeof(DIR));

	if (!dirname[0] || (lookup((char *)canonical_name, &s->oid) < 0)) {
		free(s);
		return NULL; /* ENOENT */
	}

	free(canonical_name);
	s->dirent = NULL;

#if 1
	memset(&msg, 0, sizeof(msg));
	msg.type = mtGetAttr;
	msg.i.attr.type = /*atType*/ 0;
	memcpy(&msg.i.attr.oid, &s->oid, sizeof(oid_t));

	if (msgSend(s->oid.port, &msg) < 0) {
		free(s);
		return NULL; /* EIO */
	}

	if (msg.o.attr.val != /* Dir */ 0)
		return NULL; /* ENOTDIR */
#endif

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
	return 0;
}


char *dirname(char *path)
{
	return NULL;
}
