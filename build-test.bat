@echo off

set BASEPATH=%CD%

REM Path to VICE 3.8 c64 emulator executable
set VICEBIN=GTK3VICE-3.8-win64\bin\x64sc.exe
REM Refer to: https://vice-emu.sourceforge.io/vice_2.html#SEC50
set VICEOPTS=-silent -autostartprgmode 1 

REM For a handy overview of 6502 compilers refer to: https://gglabs.us/node/2293

set /p build_kickc= "Build for kickc (y/n) [n]: "
if "%build_kickc%"=="y" (
    echo *****************************************
    echo ************ Build with kickc ***********
    echo *****************************************
    REM Path to 'bin' directory for the compiler (relative or absolute path, enclose in quotes if path has a space in it)
    set KICKC_PATH=kickc\bin
    echo Using compiler path: %KICKC_PATH%\kickc.bat

    echo *****************************************
    echo * Set command line options for compiler *
    echo *****************************************
    REM Refer to: kickc.pdf (KickC Reference Manual, Version 0.8, Section 3 "KickC Command Line Reference")
    set KICKC_OPTS=-t c64 -a -Oliverangecallpath -Sl -DKICKC
    echo Using compiler options: %KICKC_OPTS%

    set BUILD_NAME=graphics-test-kickc
    IF EXIST %BUILD_NAME%.prg del /F /Q %BUILD_NAME%.prg
    cd %KICKC_PATH%
    call kickc.bat %KICKC_OPTS% -o %BASEPATH%\%BUILD_NAME%.prg %BASEPATH%\c64-graphics.c %BASEPATH%\graphics-test.c
    cd %BASEPATH%
    IF EXIST %BUILD_NAME%.prg %VICEBIN% %VICEOPTS% %BUILD_NAME%.prg
)

set /p build_vbcc= "Build for vbcc (y/n) [n]: "
if "%build_vbcc%"=="y" (
    echo *****************************************
    echo ************ Build with vbcc ************
    echo *****************************************
    REM Path to 'bin' directory for the compiler (relative or absolute path, enclose in quotes if path has a space in it)
    set VBCC_PATH=vbcc\vbcc6502_r3p3\vbcc6502\vbcc6502_win\vbcc\bin
    echo Using compiler path: %VBCC_PATH%\vc.exe

    echo *****************************************
    echo * Set command line options for compiler *
    echo *****************************************
    REM Refer to: vbcc.pdf (vbcc compiler system, Feb 2022, section 3.1 "General Compiler Options")
    set VBCC_OPTS=+c64 -lm -O4 -DVBCC
    echo Using compiler options: %VBCC_OPTS%

    set BUILD_NAME=graphics-test-vbcc
    IF EXIST %BUILD_NAME%.prg del /F /Q %BUILD_NAME%.prg
    cd %VBCC_PATH%
    set VBCC=..
    copy ..\config\c64 .
    vc.exe %VBCC_OPTS% -o %BASEPATH%/%BUILD_NAME%.prg %BASEPATH%/c64-graphics.c %BASEPATH%/graphics-test.c
    del c64
    cd %BASEPATH%
    IF EXIST %BUILD_NAME%.prg %VICEBIN% %VICEOPTS% %BUILD_NAME%.prg
)

set /p build_cc65= "Build for cc65 (y/n) [n]: "
if "%build_cc65%"=="y" (
    echo *****************************************
    echo ************ Build with cc65 ************
    echo *****************************************
    REM Path to 'bin' directory for the compiler (relative or absolute path, enclose in quotes if path has a space in it)
    set CC65_PATH=cc65\bin
    echo Using compiler path: %CC65_PATH%\cl65.exe

    echo *****************************************
    echo * Set command line options for compiler *
    echo *****************************************
    REM Refer to: https://www.cc65.org/doc/cc65-2.html
    set CC65_OPTS=-Oisr --static-locals -t c64 -T -DCC65
    echo Using compiler options: %CC65_OPTS%

    set BUILD_NAME=graphics-test-cc65
    IF EXIST %BUILD_NAME%.prg del /F /Q %BUILD_NAME%.prg
    cd %CC65_PATH%
    cl65.exe %CC65_OPTS% -o %BASEPATH%/%BUILD_NAME%.prg %BASEPATH%/c64-graphics.c %BASEPATH%/graphics-test.c
    cd %BASEPATH%
    IF EXIST %BUILD_NAME%.prg %VICEBIN% %VICEOPTS% %BUILD_NAME%.prg
)

REM set /p build_sdcc= "Build for sdcc (y/n) [n]: "
if "%build_sdcc%"=="y" (
    echo *****************************************
    echo ************ Build with sdcc ************
    echo *****************************************
    REM Path to 'bin' directory for the compiler (relative or absolute path, enclose in quotes if path has a space in it)
    set SDCC_PATH="D:\Program Files\SDCC\bin"
    echo Using compiler path: %SDCC_PATH%\sdcc.exe

    echo *****************************************
    echo * Set command line options for compiler *
    echo *****************************************
    REM Refer to: sdccman.pdf (SDCC Compiler User Guide, SDCC 4.4.1, Section 3.3 "Command Line Options")
    set SDCC_OPTS=-mmos6502 -DSDCC
    echo Using compiler options: %SDCC_OPTS%

    set BUILD_NAME=graphics-test-sdcc
    IF EXIST %BUILD_NAME%.prg del /F /Q %BUILD_NAME%.prg
    type c64-graphics.c graphics-test.c > all.c
    cd %SDCC_PATH%
    sdcc.exe %SDCC_OPTS% %BASEPATH%/all.c
    cd %BASEPATH%
    del all.c
    IF EXIST %BUILD_NAME%.prg %VICEBIN% %VICEOPTS% %BUILD_NAME%.prg
  
REM     Example Makefile for sdcc build
REM     sdcc:
REM     	$(SDCCDIR)/bin/sdas6500 -og c64_crt.rel c64_crt.s
REM     	$(SDCC) $(SDCCTARGET) -c c64lib.c
REM     	$(SDCC) $(SDCCTARGET) -c cia_timer.c
REM     	$(SDCC) $(SDCCTARGET) -c main.c
REM     	$(SDCC) $(SDCCTARGET) $(SDCCOPTS) -c $(PRG).c
REM     
REM     #	if [ $(EXTRA) ] ; then 	$(SDCC) $(SDCCTARGET) -c $(EXTRA); fi
REM     #	$(SDCC) $(SDCCTARGET) --no-std-crt0 --code-loc 0x7ff --xram-loc 0x6000 -mmos6502 c64_crt.rel c64lib.rel \
REM     #		cia_timer.rel main.rel $(EXTRAOBJ) $(PRG).c --out-fmt-ihx
REM     	$(SDCC) $(SDCCTARGET) --no-std-crt0 --code-loc 0x7ff -mmos6502 c64_crt.rel c64lib.rel \
REM     		cia_timer.rel main.rel $(PRG).rel -o $(PRG).ihx
REM     	$(SDCCDIR)/bin/makebin -o 2047 -p $(PRG).ihx  > $(PRG)-sdcc.prg
)

REM set /p build_oscar64= "Build for oscar64 (y/n) [n]: "
if "%build_oscar64%"=="y" (
    echo *****************************************
    echo *********** Build with oscar64 **********
    echo *****************************************
    REM Path to 'bin' directory for the compiler (relative or absolute path, enclose in quotes if path has a space in it)
    set OSCAR64_PATH=""
    echo Using compiler path: %OSCAR64_PATH%\oscar64.exe

    echo *****************************************
    echo * Set command line options for compiler *
    echo *****************************************
    REM Refer to: 
    set OSCAR64_OPTS=
    echo Using compiler options: %OSCAR64_OPTS%

    set BUILD_NAME=graphics-test-oscar64
    IF EXIST %BUILD_NAME%.prg del /F /Q %BUILD_NAME%.prg
    cd %OSCAR64_PATH%
    oscar64.exe %OSCAR64_OPTS% -o %BASEPATH%/%BUILD_NAME%.prg %BASEPATH%/c64-graphics.c %BASEPATH%/graphics-test.c
    cd %BASEPATH%
    IF EXIST %BUILD_NAME%.prg %VICEBIN% %VICEOPTS% %BUILD_NAME%.prg
)

