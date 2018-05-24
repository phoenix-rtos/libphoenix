#
# Makefile for libphoenix (string)
#
# Copyright 2017 Phoenix Systems
# Author: Pawel Pisarczyk
#
ifeq (,$(SRCDIR))
$(error Only main Makefile can be used for compilation)
endif

SRCS = string.c strdup.c strerror.c fnmatch.c glob.c
OBJS = $(SRCS:.c=.o)


all: check $(ARCH)

$(ARCH): $(OBJS)

strerror.o: errstr.inc errtab.inc

errno.list: $(HEADERS_INSTALL_DIR)/phoenix/errno.h $(SRCDIR)/errno.h
	@(printf " GEN %-24s  \n" "$@")
	$(SIL)sed -En -e '/^\s*#\s*define\s+E\w+\s+[[:digit:]]+/{s/^[^d]*define\s+(\w+)\s+([[:digit:]]+).*/\2\t\1/;p;d}' \
		-e '/^\s*E\w+\s*=\s*[[:digit:]]+/{s/^\s*(\w+)\s*=\s*([[:digit:]]+).*/\2\t\1/;p;d}' \
		$^ | sort -n > $@

errstr.inc: errno.list Makefile
	@(printf " GEN %-24s  \n" "$@")
	$(SIL)sed -e 's/^.*\t\(.*\)$$/"\1\\0"/' $< > $@

errtab.inc: errno.list Makefile
	@(printf " GEN %-24s  \n" "$@")
	$(SIL)bash -c 'o=0; while read num name; do echo "{ $$num, $$o },"; o=$$((o+$${#name}+1)); done' < $< > $@

clean-errstr:
	$(SIL)rm -f errno.list errstr.inc errtab.inc

clean: clean-errstr

# include after all dependencies are set
include $(SRCDIR)/Makefile.rules
