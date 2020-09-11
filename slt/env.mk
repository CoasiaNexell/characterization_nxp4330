
BASEDIR="$(cd "$(dirname "$0")" && pwd)/../.."
LINUX_TOOLCHAIN=$(TOPDIR)/../crosstool/arm-cortex_a9-eabi-4.7-eglibc-2.18/bin/arm-cortex_a9-linux-gnueabi-

CROSSNAME	:= $(LINUX_TOOLCHAIN)
#CROSSNAME	:= 

#########################################################################
#	Toolchain.
#########################################################################
CROSS 	 	:= $(CROSSNAME)
CC 		:= $(CROSS)gcc
CPP		:= $(CROSS)g++
AR 		:= $(CROSS)ar
LD 		:= $(CROSS)ld
NM 		:= $(CROSS)nm
RANLIB 	 	:= $(CROSS)ranlib
OBJCOPY	 	:= $(CROSS)objcopy
STRIP	 	:= $(CROSS)strip


#########################################################################
#	Library & Header macro
#########################################################################
INCLUDE   	:= 

#########################################################################
# 	Build Options
#########################################################################
OPTS		:= -Wall -O2 -Wextra -Wcast-align -Wno-unused-parameter -Wshadow \
			   -Wwrite-strings -Wcast-qual -fno-strict-aliasing -fstrict-overflow \
			   -fsigned-char -fno-omit-frame-pointer -fno-optimize-sibling-calls
COPTS 		:= $(OPTS)
CPPOPTS 	:= $(OPTS) -Wnon-virtual-dtor

CFLAGS 	 	:= $(COPTS)
CPPFLAGS 	:= $(CPPOPTS)
AFLAGS 		:=

ARFLAGS		:= crv
LDFLAGS  	:=
LIBRARY		:=


#########################################################################
# 	Generic Rules
#########################################################################
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

%.o: %.cpp
	$(CPP) $(CFLAGS) $(INCLUDE) -c -o $@ $<

