@echo off

set BASEPATH=%CD%

REM Path to VICE c64 emulator executable (replace D:\Emulators\Commodore with the installation path for VICE)
set VICEBIN=D:\Emulators\Commodore\GTK3VICE-3.8-win64\bin
REM Refer to: https://vice-emu.sourceforge.io/vice_2.html#SEC50
set VICEOPTS=-silent -autostartprgmode 1 

REM Some compilers can't handle a space in the COMPILER_BASE_PATH (replace D:\Compilers\6502 with the installation path for your 6502 compilers)
set COMPILER_BASE_PATH=D:\Compilers\6502
REM For a handy overview of 6502 compilers refer to: https://gglabs.us/node/2293

REM Replace graphics-test with the C source file name for your game (without the file extension, .c .rel .ihx .prg will be appended as necessary)
set GAME_SOURCE_NAME=graphics-test

set KICKC_PATH=%COMPILER_BASE_PATH%\kickc\bin
set KICKC_OPTIMIZE=-Oliverangecallpath
set KICKC_OPTS=-t c64 -a -Sl %KICKC_OPTIMIZE% -DKICKC
set BUILD_NAME=%GAME_SOURCE_NAME%-kickc
set build_kickc=n
set /p build_kickc= "Build with kickc (y/n) [n]: "
if "%build_kickc%"=="y" (
    echo ************ Build with kickc ***********
    echo Using compiler path: %KICKC_PATH%

    REM Refer to: kickc.pdf (KickC Reference Manual, Version 0.8, Section 3 "KickC Command Line Reference")
    echo Using compiler options: %KICKC_OPTS%

    IF EXIST %BUILD_NAME%.prg del /F /Q %BUILD_NAME%.prg
    cd /D %KICKC_PATH%
    echo kickc.bat %KICKC_OPTS% -o %BASEPATH%\%BUILD_NAME%.prg %BASEPATH%\c64-graphics.c %BASEPATH%\c64-util.c %BASEPATH%\%GAME_SOURCE_NAME%.c
    call kickc.bat %KICKC_OPTS% -o %BASEPATH%\%BUILD_NAME%.prg %BASEPATH%\c64-graphics.c %BASEPATH%\c64-util.c %BASEPATH%\%GAME_SOURCE_NAME%.c
    cd /D %BASEPATH%
    IF EXIST %BUILD_NAME%.prg %VICEBIN%\x64sc.exe %VICEOPTS% %BUILD_NAME%.prg
)

set VBCC_PATH=%COMPILER_BASE_PATH%\vbcc\vbcc6502_r3p3\vbcc6502\vbcc6502_win\vbcc\bin
set VBCC_OPTIMIZE=-O4
set VBCC_OPTS=+c64 -lm %VBCC_OPTIMIZE% -DVBCC
REM set VBCC so compiler can find config/c64 (needed to locate standard library includes for the selected target)
set VBCC=..
set BUILD_NAME=%GAME_SOURCE_NAME%-vbcc
set build_vbcc=n
set /p build_vbcc= "Build with vbcc (y/n) [n]: "
if "%build_vbcc%"=="y" (
    echo ************ Build with vbcc ************
    echo Using compiler path: %VBCC_PATH%

    REM Refer to: vbcc.pdf (vbcc compiler system, Feb 2022, section 3.1 "General Compiler Options")
    echo Using compiler options: %VBCC_OPTS%

    IF EXIST %BUILD_NAME%.prg del /F /Q %BUILD_NAME%.prg
    cd /D %VBCC_PATH%
    copy ..\config\c64 .
    echo vc.exe %VBCC_OPTS% -o %BASEPATH%\%BUILD_NAME%.prg %BASEPATH%\c64-graphics.c %BASEPATH%\c64-util.c %BASEPATH%\%GAME_SOURCE_NAME%.c
         vc.exe %VBCC_OPTS% -o %BASEPATH%\%BUILD_NAME%.prg %BASEPATH%\c64-graphics.c %BASEPATH%\c64-util.c %BASEPATH%\%GAME_SOURCE_NAME%.c
    del c64
    cd /D %BASEPATH%
    IF EXIST %BUILD_NAME%.prg %VICEBIN%\x64sc.exe %VICEOPTS% %BUILD_NAME%.prg
)

set CC65_PATH=%COMPILER_BASE_PATH%\cc65\bin
set CC65_OPTIMIZE=-Oisr --static-locals
set CC65_OPTIONS=%CC65_OPTIMIZE% -t c64 -T -DCC65
set BUILD_NAME=%GAME_SOURCE_NAME%-cc65
set build_cc65=n
set /p build_cc65= "Build with cc65 (y/n) [n]: "
if "%build_cc65%"=="y" (
    echo ************ Build with cc65 ************
    echo Using compiler path: %CC65_PATH%

    REM Refer to: https://www.cc65.org/doc/cc65-2.html
    echo Using compiler options: %CC65_OPTS%

    IF EXIST %BUILD_NAME%.prg del /F /Q %BUILD_NAME%.prg
    cd /D %CC65_PATH%
    echo cl65.exe %CC65_OPTS% -o %BASEPATH%\%BUILD_NAME%.prg %BASEPATH%\c64-graphics.c %BASEPATH%\c64-util.c %BASEPATH%\%GAME_SOURCE_NAME%.c
         cl65.exe %CC65_OPTS% -o %BASEPATH%\%BUILD_NAME%.prg %BASEPATH%\c64-graphics.c %BASEPATH%\c64-util.c %BASEPATH%\%GAME_SOURCE_NAME%.c
    cd /D %BASEPATH%
    IF EXIST %BUILD_NAME%.prg %VICEBIN%\x64sc.exe %VICEOPTS% %BUILD_NAME%.prg
)

set SDCC_PATH=%COMPILER_BASE_PATH%\SDCC\bin
set SDCC_OPTIONS=-mmos6502 --fverbose-asm --i-code-in-asm -D__C64__ -DSDCC
set SDCC_OPTIMIZE=--max-allocs-per-node 250000 --opt-code-speed
set BUILD_NAME=%GAME_SOURCE_NAME%-sdcc
set build_sdcc=n
set /p build_sdcc= "Build with SDCC (y/n) [n]: "
if "%build_sdcc%"=="y" (
    echo ************ Build with SDCC ************
    echo Using compiler path: %SDCC_PATH%

    REM Refer to: sdccman.pdf (SDCC Compiler User Guide, SDCC 4.4.1, Section 3.3 "Command Line Options")
    echo Using compiler options: %SDCC_OPTIONS%
    echo Using compiler optimizations: %SDCC_OPTIMIZE%

    IF EXIST %BUILD_NAME%.prg del /F /Q %BUILD_NAME%.prg
    cd /D %SDCC_PATH%

    REM FIX: cc1 must be at path SDCC\bin\..\libexec\sdcc\cc1 to compile
    IF NOT EXIST %SDCC_PATH%\..\libexec\sdcc (
        echo mkdir %SDCC_PATH%\..\libexec\sdcc
             mkdir %SDCC_PATH%\..\libexec\sdcc
    )
    IF NOT EXIST %SDCC_PATH%\..\libexec\sdcc\cc1 (
        echo copy %SDCC_PATH%\cc1 %SDCC_PATH%\..\libexec\sdcc\cc1
             copy %SDCC_PATH%\cc1 %SDCC_PATH%\..\libexec\sdcc\cc1
    )

    echo sdcc.exe %SDCC_OPTIONS% %SDCC_OPTIMIZE% %BASEPATH%\c64-graphics.c
         sdcc.exe %SDCC_OPTIONS% %SDCC_OPTIMIZE% %BASEPATH%\c64-graphics.c
    echo sdcc.exe %SDCC_OPTIONS% %SDCC_OPTIMIZE% %BASEPATH%\c64-util.c
         sdcc.exe %SDCC_OPTIONS% %SDCC_OPTIMIZE% %BASEPATH%\c64-util.c
    echo sdcc.exe %SDCC_OPTIONS% %SDCC_OPTIMIZE% %BASEPATH%\%GAME_SOURCE_NAME%.c
         sdcc.exe %SDCC_OPTIONS% %SDCC_OPTIMIZE% %BASEPATH%\%GAME_SOURCE_NAME%.c
    echo sdcc.exe -mmos6502 c64-graphics.rel c64-util.rel %GAME_SOURCE_NAME%.rel -o %BUILD_NAME%.ihx
         sdcc.exe -mmos6502 c64-graphics.rel c64-util.rel %GAME_SOURCE_NAME%.rel -o %BUILD_NAME%.ihx
    echo "makebin.exe -o 2047 -p %BUILD_NAME%.ihx > %BASEPATH%\%BUILD_NAME%.prg"
          makebin.exe -o 2047 -p %BUILD_NAME%.ihx > %BASEPATH%\%BUILD_NAME%.prg
    cd /D %BASEPATH%
    IF EXIST %BUILD_NAME%.prg %VICEBIN%\x64sc.exe %VICEOPTS% %BUILD_NAME%.prg
)

set OSCAR64_PATH=%COMPILER_BASE_PATH%\oscar64\bin
set OSCAR64_OPTIMIZE=
set OSCAR64_OPTIONS=-n -tf=prg -tm=c64 %OSCAR64_OPTIMIZE% -DOSCAR64
set BUILD_NAME=%GAME_SOURCE_NAME%-oscar64
set build_oscar64=n
set /p build_oscar64= "Build with oscar64 (y/n) [n]: "
if "%build_oscar64%"=="y" (
    echo *********** Build with oscar64 **********
    echo Using compiler path: %OSCAR64_PATH%

    REM Refer to: https://github.com/drmortalwombat/oscar64/tree/main
    echo Using compiler options: %OSCAR64_OPTIONS%

    IF EXIST %BUILD_NAME%.prg del /F /Q %BUILD_NAME%.prg
    cd /D %OSCAR64_PATH%
    echo oscar64.exe %OSCAR64_OPTIONS% -o=%BASEPATH%\%BUILD_NAME%.prg %BASEPATH%\c64-graphics.c %BASEPATH%\c64-util.c %BASEPATH%\%GAME_SOURCE_NAME%.c
         oscar64.exe %OSCAR64_OPTIONS% -o=%BASEPATH%\%BUILD_NAME%.prg %BASEPATH%\c64-graphics.c %BASEPATH%\c64-util.c %BASEPATH%\%GAME_SOURCE_NAME%.c
    cd /D %BASEPATH%
    IF EXIST %BUILD_NAME%.prg %VICEBIN%\x64sc.exe %VICEOPTS% %BUILD_NAME%.prg
)

