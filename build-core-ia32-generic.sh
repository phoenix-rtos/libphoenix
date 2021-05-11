#!/bin/bash
#
# Shell script for building Phoenix-RTOS firmware
#
# Builder for Phoenix-RTOS core components
#
# Copyright 2019 Phoenix Systems
# Author: Kaja Swat, Aleksander Kaminski, Pawel Pisarczyk, Lukasz Kosinski
#

# fail immediately if any of the commands fails
set -e

b_log "Building phoenix-rtos-kernel"
KERNEL_MAKECMDGOALS="install-headers"
make -C "phoenix-rtos-kernel" $KERNEL_MAKECMDGOALS all

b_log "Building libphoenix"
make -C "libphoenix" all install

b_log "Building phoenix-rtos-filesystems"
make -C "phoenix-rtos-filesystems" all
b_install "$PREFIX_PROG_STRIPPED/dummyfs" /sbin
#b_install "$PREFIX_PROG_STRIPPED/ext2" /sbin

b_log "Building phoenix-rtos-devices"
make -C "phoenix-rtos-devices" all
b_install "$PREFIX_PROG_STRIPPED/pc-tty" /sbin
#b_install "$PREFIX_PROG_STRIPPED/pc-uart" /sbin
b_install "$PREFIX_PROG_STRIPPED/uart16550" /sbin
b_install "$PREFIX_PROG_STRIPPED/pc-ata" /sbin

b_log "Building coreutils"
make -C "phoenix-rtos-utils" all
b_install "$PREFIX_PROG_STRIPPED/psh" /bin

#FIXME: tests should not always be built as a part of CORE
b_log "Building phoenix-rtos-tests"
make -C "phoenix-rtos-tests" all
b_install "$PREFIX_PROG_STRIPPED/test_msg" /bin

#b_log "Building phoenix-rtos-lwip"
#make -C "phoenix-rtos-lwip" all
#b_install "$PREFIX_PROG_STRIPPED/lwip" /sbin

b_log "Building posixsrv"
make -C "phoenix-rtos-posixsrv" all
b_install "$PREFIX_PROG_STRIPPED/posixsrv" /bin
