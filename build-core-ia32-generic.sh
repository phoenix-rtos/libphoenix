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

b_log "Building phoenix-rtos-corelibs"
make -C "phoenix-rtos-corelibs" all install

b_log "Building phoenix-rtos-filesystems"
make -C "phoenix-rtos-filesystems" all install

b_log "Building phoenix-rtos-devices"
make -C "phoenix-rtos-devices" all install

b_log "Building coreutils"
make -C "phoenix-rtos-utils" all install

if [ "$CORE_NETWORKING_DISABLE" != "y" ]; then
	b_log "Building phoenix-rtos-lwip"
	make -C "phoenix-rtos-lwip" all
	b_install "$PREFIX_PROG_STRIPPED/lwip" /sbin
fi

b_log "Building posixsrv"
make -C "phoenix-rtos-posixsrv" all install
