#!/bin/bash
#
# Shell script for building Phoenix-RTOS based firmware
#
# Main builder
#
# Copyright 2018, 2019 Phoenix Systems
# Author: Kaja Swat, Aleksander Kaminski, Pawel Pisarczyk
#

set -e

. ./phoenix-rtos-build/build.subr
. ./build.project

TOPDIR="$(pwd)"

PREFIX_BUILD="$(pwd)/_build/$TARGET"
PREFIX_BUILD_HOST="$(pwd)/_build/host"
PREFIX_FS="$(pwd)/_fs/$TARGET"
PREFIX_BOOT="$(pwd)/_boot"

PREFIX_PROG="$PREFIX_BUILD/prog/"
PREFIX_PROG_STRIPPED="$PREFIX_BUILD/prog.stripped/"

PREFIX_A="$PREFIX_BUILD/lib/"
PREFIX_H="$PREFIX_BUILD/include/"

PREFIX_ROOTFS="$PREFIX_FS/root/"
PREFIX_ROOTSKEL="$(pwd)/_fs//root-skel/"

CFLAGS="${CFLAGS} -Dphoenix"
LDFLAGS="$LDFLAGS -L$PREFIX_A"
CC=${CROSS}gcc
AS=${CROSS}as
LD=${CROSS}ld
AR=${CROSS}ar

MAKEFLAGS="--no-print-directory -j 9"

export TARGET TARGET_FAMILY TOPDIR PREFIX_BUILD PREFIX_BUILD_HOST\
	PREFIX_FS PREFIX_BOOT PREFIX_PROG PREFIX_PROG_STRIPPED PREFIX_A\
	PREFIX_H PREFIX_ROOTFS PREFIX_ROOTSKEL CROSS CFLAGS LDFLAGS CC LD\
	AR AS CLEAN MAKEFLAGS DEVICE_FLAGS

#
# Parse command line
#
if [ $# -lt 1 ]; then
	echo "Build options should be specified!"
	echo "Usage: build.sh [clean] [all] [fs] [core] [ports] [project] [image]";
	exit 1;
fi

B_FS="n"
B_CORE="n"
B_PORTS="n"
B_PROJECT="n"
B_IMAGE="n"

for i in "$@"; do
	case "$i"
	in
		clean)
			CLEAN="clean";;
		fs)
			B_FS="y";;
		core)
			B_CORE="y";;
		ports)
			B_PORTS="y";;
		project)
			B_PROJECT="y";;
		image)
			B_IMAGE="y"
			shift;;
		all)
			B_FS="y"; B_CORE="y"; B_PORTS="y"; B_PROJECT="y"; B_IMAGE="y";
			shift;;
	esac;
done

#
# Prepare
#
mkdir -p "$PREFIX_BUILD"
mkdir -p "$PREFIX_BUILD_HOST"
if declare -f "b_prepare" > /dev/null; then
	b_prepare
fi
echo " $(git rev-parse HEAD) $(basename "$(git rev-parse --show-toplevel)") ($(git describe --always --dirty))" > "${PREFIX_BUILD}/git-version"
git submodule status --recursive >> "${PREFIX_BUILD}/git-version"

#
# Preparing filesystem
#
if [ "${B_FS}" = "y" ] && [ -d  "${PREFIX_ROOTSKEL}" ]; then
	b_log "Preparing filesystem"
	if [ "$CLEAN" = "clean" ]; then
		rm -fr "$PREFIX_FS/root/"*
	fi

	mkdir -p "${PREFIX_ROOTFS}"
	cp -a "${PREFIX_ROOTSKEL}/." "${PREFIX_ROOTFS}"

	b_log "Saving git-version"
	install -m 664 "${PREFIX_BUILD}/git-version" "$PREFIX_FS/root/etc"
fi

#
# Build core part
#
if [ "${B_CORE}" = "y" ]; then
	"./phoenix-rtos-build/build-core-${TARGET_SUBFAMILY}.sh"
fi

#
# Build ports
#
if [ "${B_PORTS}" = "y" ] && [ -d phoenix-rtos-ports ]; then
	./phoenix-rtos-ports/build.sh
fi

#
# Build project part
#
if [ "${B_PROJECT}" = "y" ]; then
	b_build
fi

#
# Build final filesystems
#
if [ "${B_IMAGE}" = "y" ]; then
	mkdir -p "${PREFIX_BOOT}"
	b_image
fi
