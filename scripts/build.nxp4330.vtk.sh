#!/bin/bash

BASEDIR="$(cd "$(dirname "$0")" && pwd)/../.."
RESULT="$BASEDIR/result"

# Toolchains for Bootloader and Linux
LINUX_TOOLCHAIN="$BASEDIR//characterization/crosstool/arm-cortex_a9-eabi-4.7-eglibc-2.18/bin/arm-cortex_a9-linux-gnueabi-"

# Build PATH
BR2_DIR=$BASEDIR/buildroot
UBOOT_DIR=$BASEDIR/u-boot-2016.01
KERNEL_DIR=$BASEDIR/kernel-4.4
BIN_DIR="$BASEDIR/characterization/bin"
FILES_DIR="$BASEDIR/characterization/files"
BINARY_DIR="$BASEDIR/characterization/firmwares"
MAKE_ENV="$BASEDIR/tools/mkenvimage"
SLT_DIR="$BASEDIR/characterization/slt/"
ASB_TARGET="$RESULT/rootfs/usr/bin/"

# FILES
UBOOT_NSIH="$FILES_DIR/nsih_uboot.txt"
BOOT_KEY="$FILES_DIR/bootkey"
USER_KEY="$FILES_DIR/userkey"
BINGEN_EXE="$BIN_DIR/bingen"

# BINGEN
UBOOT_BINGEN="$BIN_DIR/SECURE_BINGEN -c nxp4330 -t 3rdboot -i $RESULT/u-boot.bin
		-l 0x43c00000 -e 0x43c00000 -o $RESULT/bootloader.img"

# Images BUILD
MAKE_EXT4FS_EXE="$BASEDIR/characterization/bin/make_ext4fs"
MAKE_BOOTIMG="mkdir -p $RESULT/boot; \
		cp -a $RESULT/zImage $RESULT/boot; \
		cp -a $RESULT/*dtb $RESULT/boot; \
		$MAKE_EXT4FS_EXE -b 4096 -L boot -l 33554432 $RESULT/boot.img $RESULT/boot/"

MAKE_ROOTIMG="mkdir -p $RESULT/root; \
		$MAKE_EXT4FS_EXE -b 4096 -L root -l 1073741824 $RESULT/root.img $RESULT/root/"


# Build Targets
BUILD_IMAGES=(
	"MACHINE= nxp4330",
	"ARCH  	= arm",
	"TOOL	= $LINUX_TOOLCHAIN",
	"RESULT = $RESULT",
	"bl1   	=
		OUTPUT	: $BINARY_DIR/*",
	"uboot 	=
		PATH  	: $UBOOT_DIR,
		CONFIG	: nxp4330_vtk_defconfig,
		OUTPUT	: u-boot.bin,

		POSTCMD : $UBOOT_BINGEN",
	"br2   	=
		PATH  	: $BR2_DIR,
		CONFIG	: nxp4330_slt_defconfig,
		OUTPUT	: output/target,
		COPY  	: rootfs",
	"slt	=
		PATH 	: $SLT_DIR/src,
		OUTPUT 	: ../bin/,
		COPY	: rootfs/usr/local/",
	"kernel	=
		PATH  	: $KERNEL_DIR,
		CONFIG	: nxp4330_vtk_defconfig,
		IMAGE 	: zImage,
		OUTPUT	: arch/arm/boot/zImage",
	"dtb   	=
		PATH  	: $KERNEL_DIR,
		IMAGE 	: nxp4330-vtk.dtb,
		OUTPUT	: arch/arm/boot/dts/nxp4330-vtk.dtb",
	"bootimg =
		POSTCMD	: $MAKE_BOOTIMG",
	"params =
		POSTCMD : $MAKE_PARAMS",
	"root	=
		POSTCMD :$MAKE_ROOTIMG",
)
