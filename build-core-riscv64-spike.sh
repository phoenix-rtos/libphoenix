#!/bin/bash
#
# Shell script for building Phoenix-RTOS firmware
#
# Builder for Phoenix-RTOS core components
#
# Copyright 2019 Phoenix Systems
# Author: Kaja Swat, Aleksander Kaminski, Pawel Pisarczyk, Lukasz Kosinski
#

b_log "Building phoenix-rtos-kernel"

KERNEL_MAKECMDGOALS="install-headers"
if [ "X$CLEAN" == "Xclean" ]; then
	KERNEL_MAKECMDGOALS="$CLEAN $KERNEL_MAKECMDGOALS"
fi

(cd phoenix-rtos-kernel && make $MAKEFLAGS $KERNEL_MAKECMDGOALS all)

b_log "Building libphoenix"
(cd libphoenix && make $MAKEFLAGS $CLEAN all install)

b_log "Building phoenix-rtos-filesystems"
(cd phoenix-rtos-filesystems && make $MAKEFLAGS $CLEAN all)

b_install "$PREFIX_PROG_STRIPPED/dummyfs" /sbin

b_log "Building phoenix-rtos-devices"
(cd phoenix-rtos-devices && make $MAKEFLAGS $CLEAN all)
b_install "$PREFIX_PROG_STRIPPED/spike-tty" /sbin

b_log "Building psh"
(cd phoenix-rtos-utils && make $MAKEFLAGS $CLEAN all)
b_install "$PREFIX_PROG_STRIPPED/psh" /bin

b_log "Building phoenix-rtos-tests"
(cd phoenix-rtos-tests && make $MAKEFLAGS $CLEAN all)
b_install "$PREFIX_PROG_STRIPPED/test_msg" /bin

#b_log "Building phoenix-rtos-lwip"
#(cd phoenix-rtos-lwip && make $MAKEFLAGS $CLEAN all)
#b_install "$PREFIX_PROG_STRIPPED/lwip" /sbin

#b_log "Building posixsrv"
#(cd phoenix-rtos-posixsrv && make $MAKEFLAGS $CLEAN all)
#b_install "$PREFIX_PROG_STRIPPED/posixsrv" /bin
