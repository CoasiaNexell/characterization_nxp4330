#!/bin/bash

BASEDIR="$(cd "$(dirname "$0")" && pwd)/../.."
RESULT="$BASEDIR/result"

# Toolchains for Bootloader and Linux
LINUX_TOOLCHAIN="$BASEDIR/characterization/crosstool/arm-cortex_a9-eabi-4.7-eglibc-2.18/bin/arm-cortex_a9-linux-gnueabi-"
BL_TOOLCHAIN="$BASEDIR/characterization/crosstool/arm-eabi-4.8/bin/arm-eabi-"
# Build PATH
BR2_DIR=$BASEDIR/buildroot
UBOOT_DIR=$BASEDIR/u-boot-2014.07
KERNEL_DIR=$BASEDIR/kernel-3.4
BL_DIR="$BASEDIR/2ndboot"

BIN_DIR="$BASEDIR/characterization/bin"
FILES_DIR="$BASEDIR/characterization/files"
BINARY_DIR="$BASEDIR/characterization/firmwares"
MAKE_ENV="$BASEDIR/tools/mkenvimage"
SLT_DIR="$BASEDIR/characterization/slt/"
ASB_TARGET="$RESULT/rootfs/usr/bin/"
NSIH="$FILES_DIR/nsih_general_sdmmc.txt"

# build option
BL1_OPT="CHIPNAME=nxp4330 BOARD=slt DEVICE_PORT=0 KERNEL_VER=y ARM_SECURE=n SYSLOG=y"
DISP_OPT="CHIPNAME=nxp4330 BOARD=slt DEVICE_PORT=0 KERNEL_VER=y ARM_SECURE=n SYSLOG=y"
# FILES
UBOOT_NSIH="$FILES_DIR/nsih_uboot.txt"
BOOT_KEY="$FILES_DIR/bootkey"
USER_KEY="$FILES_DIR/userkey"
BINGEN_EXE="$BIN_DIR/bingen"
MAKE_DISK_IMG="$BASEDIR/characterization/scripts/partmap_diskimg.sh"
PARTMAP="$BASEDIR/characterization/files/partmap_sd.txt"

# BINGEN
UBOOT_BINGEN="$BIN_DIR/BOOT_BINGEN -c nxp4330 -t 3rdboot -i $UBOOT_DIR/u-boot.bin
		-n $NSIH -l 0x40100000 -e 0x40100000 -o $RESULT/bootloader.img"

BL_BINGEN="$BIN_DIR/BOOT_BINGEN -t 2ndboot -i $BL_DIR/out/bl1-slt.bin
	-n $NSIH -o $RESULT/2ndboot.bin -c s5p4418 -l 0x40c00000 -e 0x40c00000"

# Images BUILD
MAKE_EXT4FS_EXE="$BASEDIR/characterization/bin/make_ext4fs"

MAKE_BOOTIMG="dd if=/dev/zero of=$RESULT/boot.img bs=1M count=1000;\
		mkfs.vfat $RESULT/boot.img;\
		mkdir $RESULT/mnt;\
		sudo mount $RESULT/boot.img $RESULT/mnt;
		sudo cp $RESULT/uImage $RESULT/mnt; \
		sudo cp $RESULT/*dtb $RESULT/mnt; \
		sudo umount $RESULT/mnt;"

MAKE_ROOTIMG="mkdir -p $RESULT/root; \
		$MAKE_EXT4FS_EXE -b 4096 -L root -l 1073741824 $RESULT/root.img $RESULT/root/"

MAKE_DISK="$MAKE_DISK_IMG -f $PARTMAP -s 1 -r 0"

SLT_COPY="cp ./characterization/slt/script/S60test result/rootfs/etc/init.d/;\
	cp -av ./characterization/slt/lib/* result/rootfs/usr/lib/"

# Build Targets
BUILD_IMAGES=(
	"MACHINE= nxp4330",
	"ARCH	= arm",
	"TOOL	= $LINUX_TOOLCHAIN",
	"RESULT	= $RESULT",
	"2nd	=
		JOBS	: 1,
		PRECMD	: ln -s -f $BASEDIR/prototype $BL_DIR/prototype,
		PATH	: $BL_DIR,
		TOOL	: $BL_TOOLCHAIN,
		OPTION	: CROSS_TOOL=$BL_TOOLCHAIN,
		POSTCMD	: $BL_BINGEN",
	"uboot	=
		PATH	: $UBOOT_DIR,
		CONFIG	: nxp4330_slt_config,
		OUTPUT	: u-boot.bin,
		COPY	: u-boot.bin,
		POSTCMD	: $UBOOT_BINGEN",
	"br2	=
		PATH	: $BR2_DIR,
		CONFIG	: nxp4330_slt_defconfig,
		OUTPUT	: output/target,
		COPY	: rootfs",
	"slt	=
		PATH	: $SLT_DIR/src,
		OUTPUT	: ../bin/,
		COPY	: rootfs/usr/local/,
		POSTCMD	: $SLT_COPY",
	"kernel	=
		PATH	: $KERNEL_DIR,
		CONFIG	: nxp4330_slt_defconfig,
		IMAGE	: uImage,
		COPY	: uImage,
		OUTPUT	: arch/arm/boot/uImage",
	"boot	=
		POSTCMD	: $MAKE_BOOTIMG",
	"disk	=
		POSTCMD	: $MAKE_DISK",

)
