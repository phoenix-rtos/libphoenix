#
# Makefile for libphoenix (string)
#
# Copyright 2017 Phoenix Systems
# Author: Pawel Pisarczyk
#
ifeq (,$(TOPDIR))
$(error Only main Makefile can be used for compilation)
endif

ARCH_SRCS = string.c strdup.c strerror.c fnmatch.c glob.c

# first non-pattern target is the default
all: $(ARCH)

INCS := errno.str.inc errno.tab.inc gaierr.str.inc gaierr.tab.inc


ERR_EXTRACT := sed -En -e '/^\s*\#\s*define\s+E\w+\s+[[:digit:]]+/{s/^[^d]*define\s+(\w+)\s+([[:digit:]]+).*/\2\t\1/;p;d}' \
 -e '/^\s*E\w+\s*=\s*-?[[:digit:]]+/{s/^\s*(\w+)\s*=\s*-?([[:digit:]]+).*/\2\t\1/;p;d}'


%/errno.list: $(HEADERS_INSTALL_DIR)/phoenix/errno.h ${HEADER_DIR}/errno.h
	@(printf "GEN %-24s  \n" "$(@F)")
	$(SIL)$(ERR_EXTRACT) $^ | sort -n > $@

%/gaierr.list: ${HEADER_DIR}/netdb.h
	@(printf "GEN %-24s  \n" "$(@F)")
	$(SIL)$(ERR_EXTRACT) $^ | sort -n > $@

%.str.inc: %.list Makefile
	@(printf "GEN %-24s  \n" "$(@F)")
	$(SIL)sed -e 's/^.*\t\(.*\)$$/"\1\\0"/' $< > $@

%.tab.inc: %.list Makefile
	@(printf "GEN %-24s  \n" "$(@F)")
	$(SIL)bash -c 'o=0; while read num name; do echo "{ $$num, $$o },"; o=$$((o+$${#name}+1)); done' < $< > $@

# include after all dependencies are set
include $(TOPDIR)/Makefile.rules

$(PREFIX_O)strerror.o: $(patsubst %,$(PREFIX_O)%,$(INCS))
