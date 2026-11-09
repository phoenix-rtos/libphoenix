#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <phoenix/shm.h>

#define SHM_VERBOSE
#ifdef SHM_VERBOSE
#define SHM_TRACE(fmt, ...) printf("shm:" fmt "\n", ##__VA_ARGS__)
#else
#define SHM_TRACE(fmt, ...)
#endif


#define SHM_PATH_MAX (sizeof(SHM_PATH) + SHM_NAME_MAX)

static int shm_path(const char *name, char *path, size_t size)
{
	int len;

	if (*name == '/') {
		name++;
	}

	if (*name == '\0' || strchr(name, '/') != NULL) {
		return -EINVAL;
	}

	len = snprintf(path, size, "%s%s", SHM_PATH, name);
	if (len < 0 || (size_t)len >= size) {
		return -ENAMETOOLONG;
	}

	return EOK;
}


static int shm_create(const char *name, mode_t mode)
{
	int ret;
	oid_t shmctl;
	msg_t msg = { 0 };

	ret = lookup(SHMCTL_PATH, NULL, &shmctl);
	if (ret != EOK) {
		return ret;
	}

	msg.type = mtCreate;
	msg.oid = shmctl;
	msg.i.create.mode = mode & ~__getumask();
	msg.i.data = name;
	msg.i.size = strlen(name) + 1;

	ret = msgSend(shmctl.port, &msg);
	if (ret == EOK) {
		return msg.o.err;
	}

	return ret;
}


int shm_open(const char *name, int oflag, ...)
{
	va_list list;
	char path[SHM_PATH_MAX];
	mode_t mode = 0;
	int err, fd;

	if (name == NULL) {
		errno = -EINVAL;
		return -1;
	}

	if ((oflag & O_CREAT) != 0) {
		va_start(list, oflag);
		mode = va_arg(list, mode_t);
		va_end(list);
	}

	err = shm_path(name, path, sizeof(path));
	if (err != EOK) {
		errno = -err;
		return -1;
	}

	if ((oflag & O_CREAT) != 0 && (oflag & O_EXCL) != 0) {
		err = shm_create(name, mode);
		if (err != EOK) {
			errno = -err;
			return -1;
		}
	}

	fd = open(path, oflag & (~O_CREAT) & (~O_EXCL));
	if (fd < 0 && (oflag & O_CREAT) != 0 && (oflag & O_EXCL) == 0) {
		err = shm_create(name, mode);
		if (err != EOK && err != -EEXIST) {
			errno = -err;
			return -1;
		}

		fd = open(path, O_RDWR | O_CLOEXEC);
	}

	return fd;
}


int shm_unlink(const char *name)
{
	char path[SHM_PATH_MAX];
	int ret;
	oid_t oid;
	msg_t msg = { 0 };

	if (NULL == name) {
		return SET_ERRNO(-EINVAL);
	}

	ret = shm_path(name, path, SHM_PATH_MAX);
	if (EOK != ret) {
		return SET_ERRNO(ret);
	}

	if (lookup(path, NULL, &oid) < 0) {
		return SET_ERRNO(-ENOENT);
	}

	msg.type = mtDestroy;
	msg.oid = oid;
	ret = msgSend(oid.port, &msg);
	if (EOK == ret) {
		ret = msg.o.err;
	}

	return SET_ERRNO(ret);
}
