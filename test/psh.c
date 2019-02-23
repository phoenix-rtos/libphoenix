/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * test/psh
 *
 * Copyright 2017, 2018 Phoenix Systems
 * Author: Pawel Pisarczyk, Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/threads.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/mount.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define PSH_SCRIPT_MAGIC ":{}:"

static int psh_isNewline(char c)
{
	if (c == '\r' || c == '\n')
		return 1;

	return 0;
}


static int psh_isAcceptable(char c)
{
	if (psh_isNewline(c))
		return 1;

	return (c >= ' ');
}


static char *psh_nextString(char *buff, unsigned int *size)
{
	char *s, *p;

	/* Skip leading spaces. */
	for (s = buff; *s == ' '; ++s);

	/* Count string size. */
	for (p = s, *size = 0; *p && *p != ' '; ++p, ++(*size));

	s[*size] = '\0';

	return s;
}


static int psh_readln(char *line, int size)
{
	int count = 0;
	char c;

	for (;;) {
		read(0, &c, 1);

		if (c == 0) /* EOF - exit */
			exit(EXIT_SUCCESS);

		if (!psh_isAcceptable(c))
			continue;

		/* Check, if line has maximum size. */
		if (count >= size - 1 && c != 0x7f && !psh_isNewline(c))
			continue;

		if (psh_isNewline(c))
			break;

		line[count++] = c;
	}

	memset(&line[count], '\0', size - count);

	return count;
}


static void psh_help(void)
{
	printf("Available commands:\n");
	printf("  help   - prints this help\n");
	printf("  ls     - lists files in the namespace\n");
	printf("  mkdir  - creates directory\n");
	printf("  touch  - changes file timestamp\n");
	printf("  cat    - concatenates files\n");
	printf("  exec   - executes a file\n");
	printf("  mem    - prints memory map\n");
	printf("  ps     - prints list of processes and threads\n");
	printf("  exit   - exits the shell\n");
}


static void psh_ls(char *args)
{
	char *path = args;
	unsigned int len, n = 0;
	DIR *stream;
	struct dirent *dir;
	char cwd[] = ".";

	path = psh_nextString(path, &len);

	if (!len)
		path = cwd;

	do {
		stream = opendir(path);

		if (stream == NULL) {
			printf("%s: no such directory\n", path);
			break;
		}

		while ((dir = readdir(stream)) != NULL) {
			if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
				if (!dir->d_type)
					printf("\033[34m%-20s\033[0m",dir->d_name);
				else
					printf("%-20s",dir->d_name);

				if (!(++n % 7))
					printf("\n");
			}
		}

		putchar('\n');

		closedir(stream);
		path += len + 1;
	}
	while ((path = psh_nextString(path, &len)) && len);
}


static void psh_mkdir(char *args)
{
	char *path = args;
	unsigned int len;

	while ((path = psh_nextString(path, &len)) && len) {
		if (mkdir(path, 0) < 0) {
			printf("%s: failed to create directory\n", path);
		}

		path += len + 1;
	}
}


static void psh_touch(char *args)
{
	char *path = args;
	unsigned int len;
	FILE *f;

	while ((path = psh_nextString(path, &len)) && len) {
		if ((f = fopen(path, "w")) == NULL) {
			printf(path, ": fopen failed\n");
		}
		else
			fclose(f);

		path += len + 1;
	}
}

static int psh_mem(char *args)
{
	char *arg, *end;
	unsigned int len, i, n;
	meminfo_t info;
	int mapsz = 0;
	entryinfo_t *e = NULL;
	pageinfo_t *p = NULL;
	char flags[8], prot[5], *f, *r;

	memset(&info, 0, sizeof(info));
	arg = psh_nextString(args, &len);
	args += len + 1;

	if (!len) {
		/* show summary */
		info.page.mapsz = -1;
		info.entry.mapsz = -1;
		info.entry.kmapsz = -1;

		meminfo(&info);

		printf("(%d+%d)/%dKB ", (info.page.alloc - info.page.boot) / 1024, info.page.boot / 1024,
			(info.page.alloc + info.page.free) / 1024);

		printf("%d/%d entries\n", info.entry.total - info.entry.free, info.entry.total);

		return EOK;
	}

	if (!strcmp("-m", arg)) {
		info.page.mapsz = -1;
		arg = psh_nextString(args, &len);

		if (!strcmp("kernel", arg))  {
			/* show memory map of the kernel */
			info.entry.mapsz = -1;
			info.entry.kmapsz = 16;

			do {
				mapsz = info.entry.kmapsz;
				if ((info.entry.kmap = realloc(info.entry.kmap, mapsz * sizeof(entryinfo_t))) == NULL) {
					printf("psh: out of memory\n");
					return -ENOMEM;
				}
				meminfo(&info);
			}
			while (info.entry.kmapsz > mapsz);

			mapsz = info.entry.kmapsz;
			e = info.entry.kmap;
		}
		else {
			/* show memory map of a process */
			if (len) {
				info.entry.pid = strtoul(arg, &end, 10);

				if (end != args + len || (!info.entry.pid && *arg != '0')) {
					printf("mem: could not parse process id: '%s'\n", arg);
					return EOK;
				}
			}
			else {
				info.entry.pid = getpid();
			}

			info.entry.kmapsz = -1;
			info.entry.mapsz = 16;

			do {
				mapsz = info.entry.mapsz;
				if ((info.entry.map = realloc(info.entry.map, mapsz * sizeof(entryinfo_t))) == NULL) {
					printf("psh: out of memory\n");
					return -ENOMEM;
				}
				meminfo(&info);
			}
			while (info.entry.mapsz > mapsz);

			if (info.entry.mapsz < 0) {
				printf("mem: process with pid %x not found\n", info.entry.pid);
				free(info.entry.map);
				return EOK;
			}

			mapsz = info.entry.mapsz;
			e = info.entry.map;
		}

		printf("%-17s  PROT  FLAGS  %16s  OBJECT\n", "SEGMENT", "OFFSET");

		e += mapsz - 1;
		for (i = 0; i < mapsz; ++i, --e) {
			memset(f = flags, 0, sizeof(flags));

			if (e->flags & MAP_NEEDSCOPY)
				*(f++) = 'C';

			if (e->flags & MAP_PRIVATE)
				*(f++) = 'P';

			if (e->flags & MAP_FIXED)
				*(f++) = 'F';

			if (e->flags & MAP_ANONYMOUS)
				*(f++) = 'A';

			memset(r = prot, 0, sizeof(prot));

			if (e->prot & PROT_READ)
				*(r++) = 'r';
			else
				*(r++) = '-';

			if (e->prot & PROT_WRITE)
				*(r++) = 'w';
			else
				*(r++) = '-';

			if (e->prot & PROT_EXEC)
				*(r++) = 'x';
			else
				*(r++) = '-';

			*(r++) = '-';

			printf("%p:%p  %4s  %5s", e->vaddr, e->vaddr + e->size - 1, prot, flags);

			if (e->offs != -1)
				printf("  %16llx", e->offs);

			else
				printf("  %16s", "");

			if (e->object == OBJECT_ANONYMOUS)
				printf("  %s", "(anonymous)");

			else if (e->object == OBJECT_MEMORY)
				printf("  %s", "mem");

			else
				printf("  %d.%llu", e->oid.port, e->oid.id);

			if (e->object != OBJECT_ANONYMOUS && e->anonsz != ~0)
				printf("/(%zuKB)\n", e->anonsz / 1024);

			else
				printf("\n");
		}

		free(info.entry.map);
		free(info.entry.kmap);

		return EOK;
	}

	if (!strcmp("-p", arg)) {
		/* show page map */
		info.entry.mapsz = ~0;
		info.entry.kmapsz = ~0;
		info.page.mapsz = 16;

		do {
			mapsz = info.page.mapsz;
			if ((info.page.map = realloc(info.page.map, mapsz * sizeof(pageinfo_t))) == NULL) {
				printf("psh: out of memory\n");
				return -ENOMEM;
			}
			meminfo(&info);
		}
		while (info.page.mapsz > mapsz);

		for (i = 0, p = info.page.map; i < info.page.mapsz; ++i, ++p) {
			if (p != info.page.map && (n = (p->addr - (p - 1)->addr) / SIZE_PAGE - (p - 1)->count)) {
				if (n > 3) {
					printf("[%ux]", n);
				}
				else {
					while (n-- > 0)
						printf("x");
				}
			}

			if ((n = p->count) > 3) {
				printf("[%u%c]", p->count, p->marker);
				continue;
			}

			while (n-- > 0)
				printf("%c", p->marker);
		}
		printf("\n");

		free(info.page.map);
		return EOK;
	}

	printf("mem: unrecognized option '%s'\n", arg);
	return EOK;
}


static int psh_perf(char *args)
{
	char *timeout_s;
	unsigned len;
	time_t timeout, elapsed = 0, sleeptime = 200 * 1000;
	perf_event_t *buffer;
	const size_t bufsz = 4 << 20;
	int bcount, tcnt, n = 32;
	threadinfo_t *info;

	if ((info = malloc(n * sizeof(threadinfo_t))) == NULL) {
		fprintf(stderr, "perf: out of memory\n");
		return -ENOMEM;
	}

	while ((tcnt = threadsinfo(n, info)) >= n) {
		n *= 2;
		if ((info = realloc(info, n * sizeof(threadinfo_t))) == NULL) {
			fprintf(stderr, "perf: out of memory\n");
			return -ENOMEM;
		}
	}

	if (fwrite(&tcnt, sizeof(tcnt), 1, stdout) != 1) {
		fprintf(stderr, "perf: failed or partial write\n");
		free(info);
		return -1;
	}

	if (fwrite(info, sizeof(threadinfo_t), tcnt, stdout) != tcnt) {
		fprintf(stderr, "perf: failed or partial write\n");
		free(info);
		return -1;
	}

	free(info);

	timeout_s = psh_nextString(args, &len);
	args += len + 1;

	timeout = 1000 * 1000 * strtoull(timeout_s, NULL, 10);

	buffer = malloc(bufsz);

	if (buffer == NULL) {
		fprintf(stderr, "perf: out of memory\n");
		return -ENOMEM;
	}

	if (perf_start(-1) < 0) {
		fprintf(stderr, "perf: could not start\n");
		free(buffer);
		return -1;
	}

	while (elapsed < timeout) {
		bcount = perf_read(buffer, bufsz);

		if (fwrite(buffer, 1, bcount, stdout) < bcount) {
			fprintf(stderr, "perf: failed or partial write\n");
			break;
		}

		fprintf(stderr, "perf: wrote %d/%d bytes\n", bcount, bufsz);

		usleep(sleeptime);
		elapsed += sleeptime;
	}

	perf_finish();
	free(buffer);
	return EOK;
}


static int psh_ps_cmp_name(const void *t1, const void *t2)
{
	return strcmp(((threadinfo_t *)t1)->name, ((threadinfo_t *)t2)->name);
}


static int psh_ps_cmp_pid(const void *t1, const void *t2)
{
	return (int)((threadinfo_t *)t2)->pid - (int)((threadinfo_t *)t1)->pid;
}


static int psh_ps_cmp_cpu(const void *t1, const void *t2)
{
	return ((threadinfo_t *)t2)->load - ((threadinfo_t *)t1)->load;
}


static int psh_ps(char *arg)
{
	threadinfo_t *info;
	int tcnt, i, n = 32;
	unsigned len, ipart, fpart;
	char unit;

	if ((info = malloc(n * sizeof(threadinfo_t))) == NULL) {
		printf("ps: out of memory\n");
		return -ENOMEM;
	}

	while ((tcnt = threadsinfo(n, info)) >= n) {
		n *= 2;
		if ((info = realloc(info, n * sizeof(threadinfo_t))) == NULL) {
			printf("ps: out of memory\n");
			return -ENOMEM;
		}
	}

	arg = psh_nextString(arg, &len);

	if (len) {
		if (!strcmp(arg, "-p"))
			qsort(info, tcnt, sizeof(threadinfo_t), psh_ps_cmp_pid);

		else if (!strcmp(arg, "-n"))
			qsort(info, tcnt, sizeof(threadinfo_t), psh_ps_cmp_name);

		else if (!strcmp(arg, "-c"))
			qsort(info, tcnt, sizeof(threadinfo_t), psh_ps_cmp_cpu);

		else
			printf("ps: unknown option '%s'\n", arg);
	}

	printf("%9s %5s %4s  %5s %5s %12s %6s %7s  %s\n", "PID", "TTY", "PRI", "STATE", "%CPU", "WAIT", "TIME", "VMEM", "CMD");

	for (i = 0; i < tcnt; ++i) {
		printf("%9d %5s %4d  %5s %3d.%d %12llu %3u:%02u", info[i].pid, "-", info[i].priority, info[i].state ? "sleep" : "ready",
			info[i].load / 10, info[i].load % 10, info[i].wait,
			info[i].cpu_time / 60, info[i].cpu_time % 60);

		info[i].vmem /= 1024;

		if (info[i].vmem < 1000) {
			ipart = info[i].vmem;
			fpart = 0;
			unit = 'K';
		}
		else {
			ipart = info[i].vmem / 1024;
			fpart = ((info[i].vmem % 1024) * 100) / 1024;
			fpart = ((fpart % 10 >= 5) ? fpart + 10 : fpart) / 10;
			unit = 'M';

			if (fpart >= 10) {
				++ipart;
				fpart = 0;
			}

			if (ipart >= 1000) {
				fpart = ((ipart % 1024) * 100) / 1024;
				fpart = ((fpart % 10 >= 5) ? fpart + 10 : fpart) / 10;
				ipart /= 1024;
				unit = 'G';
			}

			if (fpart >= 10) {
				++ipart;
				fpart = 0;
			}
		}

		if (fpart)
			printf(" %3u.%1u %c", ipart, fpart, unit);
		else
			printf("  %4u %c", ipart, unit);

		printf("  %-32s\n", info[i].name);
	}

	free(info);
	return EOK;
}


int psh_exec(char *cmd)
{
	int exerr = 0;
	int argc = 0;
	char **argv = malloc(sizeof(char *));

	char *arg = cmd;
	unsigned int len;

	while ((arg = psh_nextString(arg, &len)) && len) {
		if ((argv = realloc(argv, (2 + argc) * sizeof(char *))) == NULL) {
			printf("psh: out of memory\n");
			return -ENOMEM;
		}
		argv[argc++] = arg;
		arg += len + 1;
	}

	argv[argc] = NULL;

	exerr = execve(cmd, argv, NULL);

	if (exerr == -ENOMEM)
		printf("psh: not enough memory to exec\n");

	else if (exerr == -EINVAL)
		printf("psh: invalid executable\n");

	else if (exerr < 0)
		printf("psh: exec failed with code %d\n", exerr);

	return exerr;
}


int psh_runfile(char *cmd)
{
	volatile int exerr = EOK;
	int pid;
	int argc = 0;
	char **argv = malloc(sizeof(char *));

	char *arg = cmd;
	unsigned int len;

	while ((arg = psh_nextString(arg, &len)) && len) {
		if ((argv = realloc(argv, (2 + argc) * sizeof(char *))) == NULL) {
			printf("psh: out of memory\n");
			return -ENOMEM;
		}
		argv[argc++] = arg;
		arg += len + 1;
	}

	argv[argc] = NULL;

	if ((pid = vfork()) < 0) {
		printf("psh: vfork failed\n");
		return pid;
	}
	else if (!pid) {
		exit(exerr = execve(cmd, argv, NULL));
	}

	if (exerr == EOK)
		return wait(0);

	if (exerr == -ENOMEM)
		printf("psh: not enough memory to exec\n");

	else if (exerr == -EINVAL)
		printf("psh: invalid executable\n");

	else
		printf("psh: exec failed with code %d\n", exerr);

	return exerr;
}


int psh_cat(char *args)
{
	char *arg = args, *buf;
	int rsz;
	unsigned int len;
	FILE *file;

	if ((buf = malloc(1024)) == NULL) {
		printf("cat: out of memory\n");
		return -ENOMEM;
	}

	while ((arg = psh_nextString(arg, &len)) && len) {
		file = fopen(arg, "r");

		if (file == NULL) {
			printf("cat: %s no such file\n", arg);
		}
		else {
			while ((rsz = fread(buf, 1, 1023, file))) {
				buf[rsz] = 0;
				printf(buf);
			}
		}

		fclose(file);
		arg += len + 1;
	}

	free(buf);
	return EOK;
}


static int psh_kill(char *arg)
{
	unsigned len, pid;
	char *end;

	arg = psh_nextString(arg, &len);

	if (!len) {
		printf("kill: missing argument (pid)\n");
		return -EINVAL;
	}

	pid = strtoul(arg, &end, 10);

	if ((end != arg + len) || (pid == 0 && *arg != '0')) {
		printf("kill: could not parse process id: '%s'\n", arg);
		return -EINVAL;
	}

	return signalPost(pid, -1, signal_kill);
}


static int psh_mount(int argc, char **argv)
{
	int err;

	if (argc != 5 && argc != 4) {
		printf("usage: mount source target fstype mode %d\n", argc);
		return -1;
	}

	err = mount(argv[0], argv[1], argv[2], atoi(argv[3]), argv[4]);

	if (err < 0)
		printf("mount: %s\n", strerror(err));

	return err;
}


static int psh_sync(int argc, char **argv)
{
	oid_t oid;
	msg_t msg = {0};
	msg.type = mtSync;

	if (!argc) {
		printf("usage: sync path-to-device\n");
		return -1;
	}

	if (lookup(argv[0], NULL, &oid) < 0)
		return -1;

	return msgSend(oid.port, &msg);
}


void psh_run(void)
{
	unsigned int n;
	char buff[128];
	char *cmd;

	for (;;) {
		write(1, "(psh)% ", 7);

		psh_readln(buff, sizeof(buff));
		cmd = psh_nextString(buff, &n);

		if (n == 0)
			continue;

		if (!strcmp(cmd, "help"))
			psh_help();

		else if (!strcmp(cmd, "ls"))
			psh_ls(cmd + 3);

		else if (!strcmp(cmd, "mem"))
			psh_mem(cmd + 4);

		else if (!strcmp(cmd, "ps"))
			psh_ps(cmd + 3);

		else if (!strcmp(cmd, "cat"))
			psh_cat(cmd + 4);

		else if (!strcmp(cmd, "touch"))
			psh_touch(cmd + 6);

		else if (!strcmp(cmd, "mkdir"))
			psh_mkdir(cmd + 6);

		else if (!strcmp(cmd, "exec"))
			psh_exec(cmd + 5);

		else if (!strcmp(cmd, "kill"))
			psh_kill(cmd + 5);

		else if (!strcmp(cmd, "perf"))
			psh_perf(cmd + 5);

		else if (cmd[0] == '/')
			psh_runfile(cmd);

		else if (!strcmp(cmd, "exit"))
			exit(EXIT_SUCCESS);

		else if (!strcmp(cmd, "reboot")) {
			priority(0);
			for (;;);
		}

		else
			printf("Unknown command!\n");
		fflush(NULL);
	}
}


int psh_runscript(char *path)
{
	FILE *stream;
	char *line = NULL;
	size_t n = 0;
	char *arg;
	int argc = 0;
	char **argv = NULL;;
	char *bin;
	int err;
	pid_t pid;

	stream = fopen(path, "r");

	if (stream == NULL)
		return -EINVAL;
	if (getline(&line, &n, stream) == 5) {
		if (strncmp(PSH_SCRIPT_MAGIC, line, strlen(PSH_SCRIPT_MAGIC))) {
			free(line);
			fclose(stream);
			printf("psh: %s is not a psh script\n", path);
			return -1;
		}
	}

	free(line);
	line = NULL;
	n = 0;

	while (getline(&line, &n, stream) > 0) {

		if (line[0] == 'X' || line[0] == 'W') {
			strtok(line, " ");

			bin = strtok(NULL, " ");
			if (bin == NULL) {
				free(line);
				fclose(stream);
				line = NULL;
				n = 0;
				return -1;
			}

			if (bin[strlen(bin) - 1] == '\n')
				bin[strlen(bin) - 1] = 0;

			argv = malloc(2 * sizeof(char *));
			argv[argc++] = bin;

			while ((arg = strtok(NULL, " ")) != NULL) {
				if (arg[strlen(arg) - 1] == '\n')
					arg[strlen(arg) - 1] = 0;

				argv = realloc(argv, (argc + 2) * sizeof(char *));

				if (argv == NULL) {
					printf("psh: Out of memory\n");
					free(line);
					fclose(stream);
					return -1;
				}

				argv[argc] = arg;
				argc++;
			}

			argv[argc] = NULL;

			if (!(pid = vfork())) {
				err = execve(bin, argv, NULL);
				printf("psh: execve failed %d\n", err);
				exit(err);
			}

			if (line[0] == 'W')
				waitpid(pid, NULL, 0);
		}

		free(argv);
		free(line);
		argv = NULL;
		argc = 0;
		line = NULL;
		n = 0;
	}
	fclose(stream);
	return EOK;
}


extern void splitname(char *, char **, char**);


int main(int argc, char **argv)
{
	int c;
	oid_t oid;
	char *args;
	char *base, *dir, *cmd;
	FILE *file;

	splitname(argv[0], &base, &dir);

	if (!strcmp(base, "psh")) {
		/* Wait for filesystem */
		while (lookup("/", NULL, &oid) < 0)
			usleep(10000);

		/* Wait for console */
		while (write(1, "", 0) < 0)
			usleep(50000);

		if (argc > 0 && (c = getopt(argc, argv, "i:")) != -1) {
			if (psh_runscript(optarg) != EOK)
				printf("psh: error during preinit\n");

			file = fopen("/var/preinit", "w+");

			if (file != NULL) {
				while ((cmd = argv[optind++]) != NULL) {
					fwrite(cmd, 1, strlen(cmd), file);
					fwrite(" ", 1, 1, file);
				}

				fwrite("\n", 1, 1, file);
				fclose(file);
			}
		}
		else {
			psh_run();
		}
	}
	else {
		if ((args = calloc(3000, 1)) == NULL) {
			printf("psh: out of memory\n");
			return EXIT_FAILURE;
		}

		for (c = 1; c < argc; ++c) {
			strcat(args, argv[c]);
			strcat(args, " ");
		}

		if (!strcmp(base, "mem"))
			psh_mem(args);
		else if (!strcmp(base, "ps"))
			psh_ps(args);
		else if (!strcmp(base, "perf"))
			psh_perf(args);
		else if (!strcmp(base, "mount"))
			psh_mount(argc - 1, argv + 1);
		else if (!strcmp(base, "sync"))
			psh_sync(argc - 1, argv + 1);
		else if (!strcmp(base, "reboot")) {
			priority(0);
			for (;;) ;
		}
		else
			printf("psh: %s: unknown command\n", argv[0]);
	}

	return 0;
}
