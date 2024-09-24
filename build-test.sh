#!/bin/sh
# Build on Mac OSX/Linux

# Path to VICE 3.8 c64 emulator executable
VICEBIN=/Applications/vice-x86-64-gtk3-3.7.1/bin/x64sc

# Path to 'bin' directory for each compiler (relative or absolute path, enclose in quotes if path has a space in it)
KICKC_PATH=kickc/bin

# Refer to: kickc.pdf (KickC Reference Manual, Version 0.8, Section 3 "KickC Command Line Reference")
KICKC_OPTS=-t c64 -a -Oliverangecallpath -Sl -DKICKC

$KICKC_PATH/kickc.sh $KICKC_OPTS -emu $VICEBIN ../../c64-graphics.c ../../graphics-test.c

