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

PREFIX_PROJECT="$(pwd)"
# TODO: Remove
# To preserve compabilitiy with project scripts, TOPDIR is set
TOPDIR="$PREFIX_PROJECT"

PREFIX_BUILD="$PREFIX_PROJECT/_build/$TARGET"
PREFIX_BUILD_HOST="$PREFIX_PROJECT/_build/host-generic-pc"
PREFIX_FS="$PREFIX_PROJECT/_fs/$TARGET"
PREFIX_BOOT="$PREFIX_PROJECT/_boot/$TARGET"

PREFIX_PROG="$PREFIX_BUILD/prog/"
PREFIX_PROG_STRIPPED="$PREFIX_BUILD/prog.stripped/"

PREFIX_A="$PREFIX_BUILD/lib/"
PREFIX_H="$PREFIX_BUILD/include/"

PREFIX_ROOTFS="$PREFIX_FS/root/"
: "${PREFIX_ROOTSKEL:="$PREFIX_PROJECT/_fs/root-skel/"}"

CC=${CROSS}gcc
AS=${CROSS}as
LD=${CROSS}ld
AR=${CROSS}ar
OBJCPY=${CROSS}objcopy

MAKEFLAGS="--no-print-directory -j 9"

export TARGET TARGET_FAMILY TARGET_SUBFAMILY TARGET_PROJECT PROJECT_PATH TOPDIR PREFIX_PROJECT PREFIX_BUILD\
	PREFIX_BUILD_HOST PREFIX_FS PREFIX_BOOT PREFIX_PROG PREFIX_PROG_STRIPPED PREFIX_A\
	PREFIX_H PREFIX_ROOTFS PREFIX_ROOTSKEL CROSS CFLAGS LDFLAGS CC LD\
	AR AS CLEAN MAKEFLAGS DEVICE_FLAGS

# export flags for ports - call make only after all necessary env variables are already set
EXPORT_CFLAGS="$(make -f phoenix-rtos-build/Makefile.common export-cflags)"
# export only generic flags: "-z xxx", "-Lxxx", "-q"
EXPORT_LDFLAGS="$(make -f phoenix-rtos-build/Makefile.common export-ldflags | grep -E -o "(\-z [^ ]+)|(\-L[^ ]+)|(\-q)" | xargs)"

export EXPORT_CFLAGS EXPORT_LDFLAGS


#
# Parse command line
#
if [ $# -lt 1 ]; then
	echo "Build options should be specified!"
	echo "Usage: build.sh [clean] [all] [fs] [core] [test] [ports] [project] [image]";
	exit 1;
fi

B_FS="n"
B_CORE="n"
B_PORTS="n"
B_PROJECT="n"
B_IMAGE="n"
B_TEST="n"

# GA CI passes all params as quoted first param - split on ' ' if necessary
ARGS=("$@")
[ "$#" -eq 1 ] && read -ra ARGS <<< "$1"

for i in "${ARGS[@]}"; do
	case "$i"
	in
		clean)
			CLEAN="clean";;
		fs)
			B_FS="y";;
		core)
			B_CORE="y";;
		test|tests)
			B_TEST="y";;
		ports)
			B_PORTS="y";;
		project)
			B_PROJECT="y";;
		image)
			B_IMAGE="y";;
		all)
			B_FS="y"; B_CORE="y"; B_PORTS="y"; B_PROJECT="y"; B_IMAGE="y";;
	esac;
done

#
# Clean if requested
#
if [ -n "$CLEAN" ]; then
	b_log "Cleaning build dirs"
	rm -rf "$PREFIX_BUILD" "$PREFIX_BUILD_HOST"
	rm -rf "$PREFIX_FS"
	rm -rf "$PREFIX_BOOT"
fi

#
# Prepare
#
mkdir -p "$PREFIX_BUILD"
mkdir -p "$PREFIX_BUILD_HOST"
mkdir -p "$PREFIX_BOOT"
mkdir -p "$PREFIX_PROG" "$PREFIX_PROG_STRIPPED"
if declare -f "b_prepare" > /dev/null; then
	b_prepare
fi

if command -v git > /dev/null && [ -a ".git" ]; then
	echo " $(git rev-parse HEAD) $(basename "$(git rev-parse --show-toplevel)") ($(git describe --always --dirty))" > "${PREFIX_BUILD}/git-version"
	git submodule status --recursive >> "${PREFIX_BUILD}/git-version"
else
	echo "not available" > "${PREFIX_BUILD}/git-version"
fi

#
# Preparing filesystem
#
if [ "${B_FS}" = "y" ] && [ -d  "${PREFIX_ROOTSKEL}" ]; then
	b_log "Preparing filesystem"

	mkdir -p "${PREFIX_ROOTFS}"
	cp -a "${PREFIX_ROOTSKEL}/." "${PREFIX_ROOTFS}"
	mkdir -p "$PREFIX_ROOTFS/"{dev,local,data,mnt,tmp,var}

	b_log "Saving git-version"
	install -m 664 "${PREFIX_BUILD}/git-version" "$PREFIX_FS/root/etc"
fi

#
# Build core part
#
if [ "${B_CORE}" = "y" ]; then
	"./phoenix-rtos-build/build-core-${TARGET_FAMILY}-${TARGET_SUBFAMILY}.sh"
fi

#
# Build test part
#
if [ "${B_TEST}" = "y" ]; then
	b_build_test
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
	b_image
fi
