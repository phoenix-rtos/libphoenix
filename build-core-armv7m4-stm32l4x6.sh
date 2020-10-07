#!/bin/bash
#
# Shell script for building Phoenix-RTOS firmware
#
# Builder for Phoenix-RTOS core components
#
# Copyright 2018, 2019 Phoenix Systems
# Author: Kaja Swat, Aleksander Kaminski, Pawel Pisarczyk
#

# fail immediately if any of the commands fails
set -e

b_log "Building phoenix-rtos-kernel"
KERNEL_MAKECMDGOALS="install-headers"
(cd phoenix-rtos-kernel && make $MAKEFLAGS $CLEAN $KERNEL_MAKECMDGOALS all)

b_log "Building libphoenix"
(cd libphoenix && make $MAKEFLAGS $CLEAN all install)

b_log "Building phoenix-rtos-devices"
(cd phoenix-rtos-devices && make $MAKEFLAGS $CLEAN all)

b_log "Building phoenix-rtos-filesystems"
(cd phoenix-rtos-filesystems && make $MAKEFLAGS $CLEAN all)

b_log "Building coreutils"
(cd phoenix-rtos-utils && make $MAKEFLAGS $CLEAN all)
