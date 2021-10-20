#!/bin/bash
#
# Shell script for building boot layout for Zynq-7000
# Based on Bootgen User Guide - UG1283 (v2018.2) September 28, 2018
#
# Copyright 2021 Phoenix Systems
# Author: Hubert Buczynski


#
# Boot image layout which is created within this script describes only one image (calling bootloader).
# BootRom always copies image to the OC RAM memory at the address 0x0
# and makes jump to the specific entry address.
# Output image is dedicated to launch bootloader or bare-metal application from QSPI or SD boot mode.
#
# Usage:
#    $1 - path to image
#    $2 - path to output image
#    $3 - entry address in the image
#
#    example: ./mkimg-boot-zynq7000.sh plo-armv7a9-zynq7000.img boot_plo.img 0x20
#    output: plo-armv7a9-zynq7000.img - contains boot layout at the beginning
#

PATH_IMG=$1
PATH_OUTPUT=$2
ADDR_ENTRY=$3

if [ "$1" == "-h" ] || [ $# -lt 3 ]; then
	echo "usage: $0 <input img file> <output file> <entry address>"
	exit 1
fi


SIZE_IMG=$((($(wc -c < "$PATH_IMG") + 0x3) & ~0x3))

OFFS_DEVH=0x000008c0  # Device image header table offset
OFFS_IMGH=0x00000900  # Image header offset
OFFS_PARTH=0x00000c80 # Partition header offset
OFFS_IMG=0x00001000   # Bootloader or app image offset in the final image


#
# Headers descriptions
#

BOOT_HEADER=(
	0xaa995566    # Width detection word
	0x584c4e58    # Header signature - 'XLNX'
	0x00000000    # Key source
	0x01010000    # Header version
	"$OFFS_IMG"   # Bootloader offset in the image
	"$SIZE_IMG"   # Bootloader size
	0x00000000    # Destination load address to which to copy bootloader
	"$ADDR_ENTRY" # Entry address
	"$SIZE_IMG"   # Bootloader size after encryption
	0x00000001    # QSPI configuration word, hard-coded
)

DEV_TABLE=(
	0x01020000          # Version
	0x00000001          # Count of image headers
	$((OFFS_PARTH / 4)) # Address to the first partition header offset (words)
	$((OFFS_IMGH / 4))  # Address to the image header
	0x00000000
)

IMG_HEADER=(
	0x0                 # Next image header, 0 if last
	$((OFFS_PARTH / 4)) # First partition header offset (words)
	0x00000000          # Reserved
	0x00000001          # Partition count length
	0x00000000          # Image name
	0x00000000          # String terminator
)

PARTITION_HEADER=(
	$((SIZE_IMG / 4))  # Encrypted partition length (words)
	$((SIZE_IMG / 4))  # Unencrypted partition length (words)
	$((SIZE_IMG / 4))  # Total partition length (words)
	0x00000000         # Destination load address to which to copy image
	"$ADDR_ENTRY"      # Entry point of the partition
	$((OFFS_IMG / 4))  # Data offset in the image (words)
	0x00000013         # Attribute bits
	0x00000001         # Section count
	0x00000000         # Location of the checksum word
	$((OFFS_IMGH / 4)) # Image header offset (words)
	0x00000000         # Authentication certification word offset
	0x00000000         # Reserved
	0x00000000         # Reserved
	0x00000000         # Reserved
	0x00000000         # Reserved
)


#
# Auxiliary functions
#


reverse() {
	for data in "$@"; do
		for (( i=0; i<4; ++i )); do
			printf "%02x" $((data & 0xff))
			data=$((data >> 8))
		done
	done
}


reverse_rpt() {
	for (( j=0; j<$2; ++j )); do
		reverse "$1"
	done
}


checksum() {
	local checksum=0

	for data in "$@"; do
		checksum=$((checksum + data))
	done

	checksum=$((checksum ^ 0xffffffff))
	reverse $checksum
}


#
# Make Boot Image Layout and add it at the beginning of the image
#

make_boot_layout() {
	local filename="boot_layout"

	# Save layout to the hex file. Empty spaces between headers should be filled with 0xff.
	{
		# Boot Header
		reverse_rpt 0xeafffffe 8    # Dummy ARM Vector Table
		reverse "${BOOT_HEADER[@]}"
		checksum "${BOOT_HEADER[@]}"
		reverse_rpt 0x00000000 19   # User defined fields
		reverse "$OFFS_DEVH"
		reverse "$OFFS_PARTH"

		# Register Initialization Table
		for (( j=0; j<256; ++j)); do
			reverse 0xffffffff
			reverse 0x00000000
		done
		reverse_rpt 0xffffffff $(((OFFS_DEVH - 256 * 4 * 2 - 0xa0) / 4 ))

		# Device Image Header Table
		reverse "${DEV_TABLE[@]}"
		reverse_rpt 0xffffffff $(((OFFS_IMGH - (OFFS_DEVH + ${#DEV_TABLE[@]} * 4)) / 4))

		# Image Header
		reverse "${IMG_HEADER[@]}"
		reverse_rpt 0xffffffff $(((OFFS_PARTH - (OFFS_IMGH + ${#IMG_HEADER[@]} * 4)) / 4))

		# Partition Header
		reverse "${PARTITION_HEADER[@]}"
		checksum "${PARTITION_HEADER[@]}"
		reverse_rpt 0xffffffff $(((OFFS_IMG - (OFFS_PARTH + (${#PARTITION_HEADER[@]} + 1) * 4)) / 4))

	} > "${filename}.hex"

	xxd -r -p "${filename}.hex" > "${filename}.bin"

	# Add boot image layout to the corresponding image
	cat "$PATH_IMG" >> "${filename}.bin"
	mv "${filename}.bin" "$PATH_OUTPUT"

	rm -f "${filename}.hex"
}

make_boot_layout
