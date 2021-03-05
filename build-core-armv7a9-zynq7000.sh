#!/bin/bash
#
# Shell script for building Phoenix-RTOS firmware
#
# Builder for Phoenix-RTOS core components
#
# Copyright 2021 Phoenix Systems
# Author: Hubert Buczynski
#

# fail immediately if any of the commands fails
set -e

b_log "Building phoenix-rtos-kernel"
KERNEL_MAKECMDGOALS="install-headers"
(cd phoenix-rtos-kernel && make $MAKEFLAGS $CLEAN $KERNEL_MAKECMDGOALS all)


b_log "Building libphoenix"
#(cd libphoenix && make $MAKEFLAGS $CLEAN all install)

b_log "Building plo"
(cd plo && make $MAKEFLAGS $CLEAN all)
