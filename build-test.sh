#!/bin/sh
# Build on Mac OSX/Linux

BASEPATH=`pwd`

# Path to VICE c64 emulator executable (replace /Applications/vice-x86-64-gtk3-3.7.1 with the installation path for VICE)
VICEBIN=/Applications/vice-x86-64-gtk3-3.7.1/bin/x64sc
# Refer to: https://vice-emu.sourceforge.io/vice_2.html#SEC50
VICEOPTS="-silent -autostartprgmode 1"

# Some compilers can't handle a space in the COMPILER_BASE_PATH (replace $HOME/Compilers/6502 with the installation path for your 6502 compilers)
COMPILER_BASE_PATH=$HOME/Compilers/6502
# For a handy overview of 6502 compilers refer to: https://gglabs.us/node/2293

# Replace graphics-test with the C source file name for your game (without the file extension, .c .rel .ihx .prg will be appended as necessary)
GAME_SOURCE_NAME=graphics-test

KICKC_PATH=$COMPILER_BASE_PATH/kickc/bin
# Refer to: kickc.pdf (KickC Reference Manual, Version 0.8, Section 3 "KickC Command Line Reference")
KICKC_OPTIMIZE="-Oliverangecallpath"
KICKC_OPTS="-t c64 -a -Sl $KICKC_OPTIMIZE -DKICKC"
BUILD_NAME=$GAME_SOURCE_NAME-kickc

$KICKC_PATH/kickc.sh $KICKC_OPTS -emu $VICEBIN -o $BASEPATH/$BUILD_NAME.prg $BASEPATH/c64-graphics.c $BASEPATH/c64-charset.c $BASEPATH/c64-util.c $BASEPATH/$GAME_SOURCE_NAME.c

#if [ -f $BASEPATH/$BUILD_NAME.prg ]; then
#    $VICEBIN $VICEOPTS $BASEPATH/$BUILD_NAME.prg
#fi

#cd $BASEPATH
