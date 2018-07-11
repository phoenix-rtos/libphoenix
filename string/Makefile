#
# Makefile for libphoenix (string)
#
# Copyright 2017 Phoenix Systems
# Author: Pawel Pisarczyk
#
ifeq (,$(TOPDIR))
$(error Only main Makefile can be used for compilation)
endif

SRCS = string.c strdup.c strerror.c fnmatch.c glob.c
OBJS = $(SRCS:.c=.o)


all: $(ARCH)

$(ARCH): $(OBJS)

strerror.o: errno.str.inc errno.tab.inc gaierr.str.inc gaierr.tab.inc


ERR_EXTRACT := sed -En -e '/^\s*\#\s*define\s+E\w+\s+[[:digit:]]+/{s/^[^d]*define\s+(\w+)\s+([[:digit:]]+).*/\2\t\1/;p;d}' \
 -e '/^\s*E\w+\s*=\s*-?[[:digit:]]+/{s/^\s*(\w+)\s*=\s*-?([[:digit:]]+).*/\2\t\1/;p;d}'


errno.list: $(HEADERS_INSTALL_DIR)/phoenix/errno.h $(SRCDIR)/errno.h
	@(printf " GEN %-24s  \n" "$@")
	$(SIL)$(ERR_EXTRACT) $^ | sort -n > $@

gaierr.list: $(SRCDIR)/netdb.h
	@(printf " GEN %-24s  \n" "$@")
	$(SIL)$(ERR_EXTRACT) $^ | sort -n > $@

%.str.inc: %.list Makefile
	@(printf " GEN %-24s  \n" "$@")
	$(SIL)sed -e 's/^.*\t\(.*\)$$/"\1\\0"/' $< > $@

%.tab.inc: %.list Makefile
	@(printf " GEN %-24s  \n" "$@")
	$(SIL)bash -c 'o=0; while read num name; do echo "{ $$num, $$o },"; o=$$((o+$${#name}+1)); done' < $< > $@

clean-errno:
	$(SIL)rm -f errno.list errno.str.inc errno.tab.inc

clean-gaierr:
	$(SIL)rm -f gaierr.list gaierr.str.inc gaierr.tab.inc

clean: clean-errno clean-gaierr

# include after all dependencies are set
include $(TOPDIR)/Makefile.rules
