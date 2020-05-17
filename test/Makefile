#
# Makefile for libphoenix/test
#
# Copyright 2017, 2020 Phoenix Systems
# Author: Pawel Pisarczyk
#

$(PREFIX_PROG)test_condwait: $(PREFIX_O)test/test_condwait.o $(PREFIX_A)libphoenix.a
	$(LINK)

$(PREFIX_PROG)test_fcntl: $(PREFIX_O)test/test_fcntl.o $(PREFIX_A)libphoenix.a
	$(LINK)

$(PREFIX_PROG)test_env: $(PREFIX_O)test/test_env.o $(PREFIX_A)libphoenix.a
	$(LINK)

$(PREFIX_PROG)test_mmap: $(PREFIX_O)test/test_mmap.o $(PREFIX_A)libphoenix.a
	$(LINK)

$(PREFIX_PROG)test_msg: $(PREFIX_O)test/test_msg.o $(PREFIX_A)libphoenix.a
	$(LINK)

$(PREFIX_PROG)test_scanf: $(PREFIX_O)test/test_scanf.o $(PREFIX_A)libphoenix.a
	$(LINK)

$(PREFIX_PROG)test_signals: $(PREFIX_O)test/test_signals.o $(PREFIX_A)libphoenix.a
	$(LINK)

$(PREFIX_PROG)test_str2num: $(PREFIX_O)test/test_str2num.o $(PREFIX_A)libphoenix.a
	$(LINK)

$(PREFIX_PROG)test_threads: $(PREFIX_O)test/test_threads.o $(PREFIX_A)libphoenix.a
	$(LINK)

$(PREFIX_PROG)test_mktime: $(PREFIX_O)test/test_mktime.o $(PREFIX_A)libphoenix.a
	$(LINK)

$(PREFIX_PROG)test_gmtime: $(PREFIX_O)test/test_gmtime.o $(PREFIX_A)libphoenix.a
	$(LINK)

$(PREFIX_PROG)test_strftime: $(PREFIX_O)test/test_strftime.o $(PREFIX_A)libphoenix.a
	$(LINK)

$(PREFIX_PROG)test_memmove: $(PREFIX_O)test/test_memmove.o $(PREFIX_A)libphoenix.a
	$(LINK)

all: $(addprefix $(PREFIX_PROG_STRIPPED), test_condwait test_env test_mmap test_msg test_scanf test_signals test_str2num test_threads test_mktime test_gmtime test_strftime test_memmove)
