#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include "test_common.h"

#define FILE_PATH "./fcntl_testfile"
#define FILE_SIZE 200
#define MIN_FD 100
#define T_LEN 10

static const struct flock input_t1[T_LEN] = {
	{ F_RDLCK, SEEK_SET, 0, 10, 0 },
	{ F_WRLCK, SEEK_SET, 0, 10, 0 },
	{ F_RDLCK, SEEK_SET, 10, 40, 0 },
	{ F_WRLCK, SEEK_SET, 10, 40, 0 },
	{ F_RDLCK, SEEK_SET, 30, 40, 0 },
	{ F_WRLCK, SEEK_SET, 30, 40, 0 },
	{ F_RDLCK, SEEK_SET, 50, 20, 0 },
	{ F_WRLCK, SEEK_SET, 50, 20, 0 },
	{ F_RDLCK, SEEK_SET, 70, 10, 0 },
	{ F_WRLCK, SEEK_SET, 70, 10, 0 }
};

/* 1 - should pass, 0 - should fail */
static const int output_t1[T_LEN] = { 1, 1, 1, 0, 1, 0, 1, 0, 1, 1 };
static const int output_t2[T_LEN] = { 1, 1, 0, 0, 0, 0, 0, 0, 1, 1 };
static const int output_t3[T_LEN] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
static const int output_t4[T_LEN] = { 1, 1, 0, 0, 0, 0, 1, 1, 0, 0 };

static const char *l_type_str[] = { "F_RDLCK", "F_UNLCK", "F_WRLCK" };

void test_fcntl_create_file(void)
{
	int fd;
	int i;
	fd = open(FILE_PATH, O_RDWR | O_CREAT | O_TRUNC);
	for (i = 0; i < FILE_SIZE; i++) {
		write(fd, "x", 1);
	}
}


/* perform a SETLK operation on input vector in a new process and check if output is as expected */
int setlk_assert_vec(int fd, int test_len, const struct flock input_vector[], const int output_vector[])
{
	int wstatus, i, result, failed;

	switch (fork ()) {
	case -1:
		printf("Fatal error  - fork\n");
		exit(-1);

	case 0:
		/* iteration over test vector - input_vector */
		for (i = 0, failed = 0; i < test_len; i++) {
			result = fcntl(fd, F_SETLK, &input_vector[i]);
			/* check if operation was sucessful and if it should be */
			if ((result == -1 && output_vector[i]) || (result != -1 && !output_vector[i])) {
				failed++;
				/* printing details of test */
				if (!verbose_test())
					continue;
				if (result == -1)
					printf("[%d] F_SETLK failed, but expected to pass - l_start %d l_len %d l_type %s\n",
						i, input_vector[i].l_start, input_vector[i].l_len, l_type_str[input_vector[i].l_type]);
				else
					printf("[%d] F_SETLK passed, but expected to fail - l_start %d l_len %d l_type %s\n",
						i, input_vector[i].l_start, input_vector[i].l_len, l_type_str[input_vector[i].l_type]);
			}
		}
		exit(failed);
		break;

	default:
		/* wait for child process and return test result */
		wait(&wstatus);
		if (WIFEXITED(wstatus)) {
			result = WEXITSTATUS(wstatus);
			printf("Performed %d testcases, failed: %d\n", T_LEN, result);
		}
		else {
			printf("WIFEXITED error\n");
			exit(-1);
		}
		return result;
	}
}


/* perform GETLK operation in a new process and check if fields of struct fl have been changed properly */
int getlk_assert(int fd, struct flock *input_fl, struct flock *output_fl)
{
	int result = 1, wstatus;
	switch (fork ()) {
	case -1:
		printf("Fatal error - fork\n");
		return 1;
		break;

	case 0:
		fcntl(fd, F_GETLK, input_fl);

		/* compare expected struct and output of the function */
		result = (input_fl->l_type != output_fl->l_type) || (input_fl->l_whence != output_fl->l_whence) ||
			(input_fl->l_start != output_fl->l_start) || (input_fl->l_len != output_fl->l_len) ||
			(input_fl->l_pid != output_fl->l_pid);

		/* print details */
		if (result && verbose_test()){
			printf("\nOutput: l_type %s , l_whence %d, l_start %d, l_len %d, l_pid %d\n",
				l_type_str[input_fl->l_type], input_fl->l_whence, input_fl->l_start, input_fl->l_len, input_fl->l_pid);
			printf("Expected: l_type %s , l_whence %d, l_start %d, l_len %d, l_pid %d\n",
				l_type_str[output_fl->l_type], output_fl->l_whence, output_fl->l_start, output_fl->l_len, output_fl->l_pid);
		}

		exit(result);
		break;

	default:
		/* wait for child proces and return test result */
		wait(&wstatus);
		if (WIFEXITED(wstatus)) {
			result = WEXITSTATUS(wstatus);
			if (result && verbose_test())
				printf("GETLK assert failed\n\n");
		}
		else {
			printf("WIFEXITED error\n");
			exit(-1);
		}
		return result;
	}
}


/* perform simple setfd/getfd using to check if flag is set as expected */
int setfd_assert(int fd, int flags)
{
	int result = 0, flags_output;

	if (fcntl(fd, F_SETFD, flags) == -1) {
		printf("F_SETFD error\n");
		return 1;
	}

	flags_output = fcntl(fd, F_GETFD, 0);
	if (flags != flags_output)
		result = 1;

	if (verbose_test() && result)
		printf("SETFD/GETFD failed. Expected: %d got: %d\n", flags, flags_output);

	return result;
}


/* perform simple setfl/getfl test to check if flag is set as expected */
int setfl_assert(int fd, int flags, int flags_expected)
{
	int result = 0, flags_output;

	if (fcntl(fd, F_SETFL, flags) == -1) {
		printf("F_SETFL error\n");
		return 1;
	}

	flags_output = fcntl(fd, F_GETFL, 0);
	if (flags_expected != flags_output)
		result = 1;

	if (verbose_test() && result)
		printf("SETFL/GETFL failed. Expected: %d got: %d\n", flags_expected, flags_output);

	return result;

}


void set_lock(int fd, struct flock *fl)
{
	if (fcntl(fd, F_SETLK, fl) == -1) {
		printf("test fatal error - unexpected F_SETLK error\n");
		exit(-1);
	}
}


int do_dupfd(int fd)
{
	int new_fd;
	new_fd = fcntl(fd, F_DUPFD, MIN_FD);
	if (new_fd == -1)
		exit(-1);
	return new_fd;
}


int open_testfile(int flags)
{
	int fd = open(FILE_PATH, flags);
	if (fd == -1) {
		printf("Fatal error - open\n");
		exit(1);
	}
	return fd;
}


/*
 * 1. setup RDLCK
 * 2. perform SETLK on different ranges of file
 */
void test_fcntl_setlk_rdlck(void)
{
	printf("\nStarting %s\n", __func__);

	int fd;
	struct flock fl;
	fd = open_testfile(O_RDWR);

	fl = (struct flock) { F_RDLCK, SEEK_SET, 20, 40 };
	set_lock(fd, &fl);

	setlk_assert_vec(fd, T_LEN, input_t1, output_t1);
	close(fd);
}


/*
 * 1. setup WRLCK on range
 * 2. perform SETLK on various ranges of file
 */
void test_fcntl_setlk_wrlck(void)
{
	printf("\nStarting %s\n", __func__);

	int fd;
	struct flock fl;
	fd = open_testfile(O_RDWR);

	fl = (struct flock) { F_WRLCK, SEEK_SET, 20, 40 };
	set_lock(fd, &fl);

	setlk_assert_vec(fd, T_LEN, input_t1, output_t2);

	close(fd);
}


/*
 * 1. setup RDLCK
 * 2. delete part of lock
 * 3. extend lock area
 * 4. perform fcntl_assert_vec
 */
void test_fcntl_setlk_change_size(void)
{
	printf("\nStarting %s\n", __func__);

	int fd;
	struct flock fl;
	fd = open_testfile(O_RDWR);

	fl = (struct flock) { F_WRLCK, SEEK_SET, 0, 40, 0 };
	set_lock(fd, &fl);
	fl = (struct flock) { F_UNLCK, SEEK_SET, 0, 20, 0 };
	set_lock(fd, &fl);
	fl = (struct flock) { F_WRLCK, SEEK_SET, 40, 40, 0 };
	set_lock(fd, &fl);

	setlk_assert_vec(fd, T_LEN, input_t1, output_t3);

	close(fd);
}


/*
 * check if multiple locks are working
 * 1. setup 2 locks
 * 2. setup lock in different process
 * 3. perform fcntl_assert_vec
 */
void test_fcntl_setlk_multiple(void)
{
	printf("\nStarting %s\n", __func__);

	int fd;
	struct flock fl;
	fd = open_testfile(O_RDWR);

	fl = (struct flock) { F_WRLCK, SEEK_SET, 20, 10 };
	set_lock(fd, &fl);
	fl = (struct flock) { F_WRLCK, SEEK_SET, 70, 10 };
	set_lock(fd, &fl);


	switch (fork()) {
	case -1:
		printf("Fatal error - fork");
		exit(-1);
		break;
	case 0:
		fl = (struct flock) { F_WRLCK, SEEK_SET, 30, 10 };
		set_lock(fd, &fl);

		setlk_assert_vec(fd, T_LEN, input_t1, output_t4);
		exit(0);
		break;
	default:
		wait(NULL);
		break;
	}

	close(fd);
}


/*
 * 1. set lock on file on range 20, 80
 * 2. getlk on range 30, 40. First lock should be returned, with correct PID
 * 3. getlk on range 90, 100. Structure should be left unchanged escept
 *    for field l_type
 */
void test_fcntl_getlk(void)
{
	printf("\nStarting %s\n", __func__);

	int fd, failed = 0;
	struct flock fl_input, fl_output;
	fd = open_testfile(O_RDWR);

	fl_input = (struct flock) { F_WRLCK, SEEK_SET, 20, 60, 0 };
	set_lock(fd, &fl_input);


	/* getlk on area with lock */
	fl_input = (struct flock) { F_WRLCK, SEEK_SET, 30, 10, 0 };
	pid_t pid = getpid();
	fl_output = (struct flock) { F_WRLCK, SEEK_SET, 20, 60, pid };

	failed += getlk_assert(fd, &fl_input, &fl_output);

	/* getlk on area without lock */
	fl_input = (struct flock) { F_WRLCK, SEEK_SET, 90, 10, 0 };
	fl_output = (struct flock) { F_UNLCK, SEEK_SET, 90, 10, 0 };

	failed += getlk_assert(fd, &fl_input, &fl_output);

	printf("Failed assertions: %d\n", failed);

	close(fd);
}


static void handle_alarm(int signo) {
	exit(1);
}


/*
 * 1. set lock on file
 * 2. in new process sart SETLKW operation - process should wait
 * 3. remove lock - waiting process should continue
 * 4. timeout if test fails
 */
void test_fcntl_setlkw(void)
{
	printf("\nStarting %s\n", __func__);

	int fd, wstatus;
	struct flock fl;
	fd = open_testfile(O_RDWR);

	fl = (struct flock) { F_WRLCK, SEEK_SET, 20, 80 };
	set_lock(fd, &fl);

	switch (fork()) {
	case -1:
		printf("Fatal error - fork");
		exit(-1);
		break;

	case 0:
		/* set timeout for test if F_SETLKW won't end */
		signal(SIGALRM, handle_alarm);
		alarm(15);

		fl = (struct flock) { F_WRLCK, SEEK_SET, 40, 10 };
		if (fcntl(fd, F_SETLKW, &fl) == -1)
			exit(1);

		exit(0);
		break;

	default:
		sleep(2);
		/* unlock file */
		fl = (struct flock) { F_UNLCK, SEEK_SET, 20, 80 };
		set_lock(fd, &fl);

		wait(&wstatus);
		if (WIFEXITED(wstatus)) {
			printf("TEST SETLKW: %s\n", (WEXITSTATUS(wstatus) ? "ERR" : "OK"));
		}
		else {
			printf("WIFEXITED error\n");
			exit(-1);
		}
		break;
	}
	close(fd);
}


/*
 * 1. open file
 * 2. perform DUPFD operation
 * 3. check if new_fd >= MIN_FD
 * 4. read few bytes from new_fd to check if file can be accessed using new_fd
 */
void test_fcntl_dupfd(void)
{
	printf("\nStarting %s\n", __func__);

	int fd, new_fd, failed = 0;
	char buff[5];

	fd = open_testfile(O_RDWR);

	new_fd = do_dupfd(fd); 

	close(fd);

	if ((new_fd < MIN_FD)) {
		if (verbose_test())
			printf("ERROR DUPFD new fd incorrect\n");
		failed++;
	}

	read(new_fd, buff, 5);

	if ((strncmp(buff, "xxxxx", 5))) {
		if (verbose_test())
			printf("ERROR DUPFD failed read from new fd\n");
		failed++;
	}

	printf("Failed assertions: %d\n", failed);

	close(new_fd);
}


/*
 * 1. open file
 * 2. perform SETLK operation
 * 2. perform DUPFD operation
 * 3. check if new fd shares locks
 */
void test_fcntl_dupfd_locks(void)
{
	printf("\nStarting %s\n", __func__);

	int fd, new_fd, failed=0;
	struct flock fl_input, fl_output;

	fd = open_testfile(O_RDWR);

	fl_input = (struct flock) { F_WRLCK, SEEK_SET, 20, 60 };
	set_lock(fd, &fl_input);

	new_fd = do_dupfd(fd);

	fl_input = (struct flock) {F_WRLCK, SEEK_SET, 20, 10, 0};
	fl_output = (struct flock) {F_WRLCK, SEEK_SET, 20, 60, getpid()};
	failed += getlk_assert(new_fd, &fl_input, &fl_output);

	printf("Failed assertions: %d\n", failed);

	close(fd);
	close(new_fd);
}


/*
 * 1. clear flags with SETFD
 * 2. get flags with GETFD
 * 3. set flags to FD_CLOEXEC
 * 4. check if flags were set correctly
 * 5. perform DUPFD and GETFD to check if flags are not shared between file descriptors
 */
void test_fcntl_setfd(void)
{
	printf("\nStarting %s\n", __func__);

	int flags, fd, new_fd, failed = 0;
	fd = open_testfile(O_RDWR);

	failed += setfd_assert(fd, FD_CLOEXEC);
	failed += setfd_assert(fd, 0);
	failed += setfd_assert(fd, FD_CLOEXEC);

	/* ensure if fds don't share flags. 0 should be default value */
	new_fd = do_dupfd(fd);

	flags = fcntl(new_fd, F_GETFD, 0);
	if (flags != 0){
		printf("\nGETFD fail for duplicate fd, got %d, expected %d\n", flags, FD_CLOEXEC);
		failed++;
	}

	printf("Failed assertions: %d\n", failed);

	close(fd);
	close(new_fd);
}


/* basic set/get test fot F_SETFL */
void test_fcntl_setfl(void)
{
	printf("\nStarting %s\n", __func__);

	int flags_initial, flags, fd, new_fd, failed = 0;
	fd = open_testfile(O_RDWR);

	flags_initial = fcntl(fd, F_GETFL, 0);

	/* acces mode flags and file creation flags should be ignored - no change */
	failed += setfl_assert(fd, O_RDONLY, flags_initial);
	/* set O_APPEND flag and ignore O_TRUNC - creation flag */
	failed += setfl_assert(fd, O_TRUNC | O_APPEND, flags_initial | O_APPEND);
	/* set one flag */
	failed += setfl_assert(fd, O_NONBLOCK, flags_initial | O_NONBLOCK);

	/* duplicate fds should share file status flags */
	new_fd = do_dupfd(fd);
	flags_initial = fcntl(fd, F_GETFL, 0);
	flags = fcntl(new_fd, F_GETFL, 0);

	if (flags_initial != flags){
		printf("\nGETFL	fail for duplicate fd, got %d, expected %d\n", flags, flags_initial);
		failed++;
	}

	printf("Failed assertions: %d\n", failed);

	close(fd);
	close(new_fd);
}


int main(void)
{
	printf("FCNTL TEST STARTED\n");
	save_env();
	test_fcntl_create_file();

	test_fcntl_setlk_rdlck();
	test_fcntl_setlk_wrlck();
	test_fcntl_setlk_change_size();
	test_fcntl_setlk_multiple();
	test_fcntl_getlk();
	test_fcntl_setlkw();
	test_fcntl_dupfd();
	test_fcntl_dupfd_locks();
	test_fcntl_setfd();
	test_fcntl_setfl();

	return 0;
}
