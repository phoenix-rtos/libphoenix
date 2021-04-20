#!/bin/bash
#
# Shell script for building Phoenix-RTOS firmware
#
# Builder for Phoenix-RTOS core components
#
# Copyright 2021 Phoenix Systems
# Author: Tomasz Korniluk
#

# fail immediately if any of the commands fails
set -e

b_log "Building libphoenix"
(cd libphoenix && make -f Makefile.host $MAKEFLAGS $CLEAN all)

b_log "Building phoenix-rtos-filesystems"
(cd phoenix-rtos-filesystems && make $MAKEFLAGS $CLEAN all)

b_log "Building phoenix-rtos-devices"
(cd phoenix-rtos-devices && make $MAKEFLAGS $CLEAN all)

b_log "Building phoenix-rtos-tests"
(cd phoenix-rtos-tests && make $MAKEFLAGS $CLEAN all)
