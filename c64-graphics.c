/** Copyright 2024 Warren Wilbur - MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "c64-graphics.h"

#include <string.h>
#ifdef DEBUG
    #include <printf.h>
#endif

#ifndef KICKC
    // For portability we will define c64 memory locations here
    #define VICII_MEMORY   ((unsigned char* const)0xd018)
    #define VICII_CONTROL1 ((unsigned char* const)0xd011)
    #define VICII_CONTROL2 ((unsigned char* const)0xd016)
    #define VICII_MCM      ((unsigned char)0x10)
    #define VICII_BMM      ((unsigned char)0x20)
    #define VICII_ECM      ((unsigned char)0x40)
    #define BG_COLOR       ((unsigned char* const)0xd021)
    #define BG_COLOR0      ((unsigned char* const)0xd021)
    #define BG_COLOR1      ((unsigned char* const)0xd022)
    #define BG_COLOR2      ((unsigned char* const)0xd023)
    #define BG_COLOR3      ((unsigned char* const)0xd024)
    #define BORDER_COLOR   ((unsigned char* const)0xd020)
    #define COLORRAM       ((unsigned char* const)0xd800)
#endif

// Print troubleshooting information for each line
//#define DEBUG

// Determines if drawing routines will truncate drawing outside the screen
// space (useful if you want to draw lines, rectangles, circles that are only
// partly on screen)
//#define SAFE_DRAW


/** VIC-II Graphics Chip Routines */

unsigned char* GetVic2VideoMemoryBank(void)
{
    // The currently selected block of 16384 chars that is used by the VIC-II chip is known as the video memory bank. The VIC-II
    // chip can only address 16384 chars of memory at a time. Reading the current VIC-II video memory bank setting:
    //      %00 = 49152
    //      %01 = 32768
    //      %10 = 16384
    //      %11 = 0
    unsigned char memoryBankIndex = (*(unsigned char*)0xDD00) & 0x03;
    // Calculate 6510 memory mapped address of VIC-II video memory bank
    unsigned char *memoryBankAddressPtr = (unsigned char*)0x0000;
    switch (memoryBankIndex)
    {
        case 0:
            memoryBankAddressPtr = (unsigned char*)0xC000;
            break;
        case 1:
            memoryBankAddressPtr = (unsigned char*)0x8000;
            break;
        case 2:
            memoryBankAddressPtr = (unsigned char*)0x4000;
            break;
        case 3:
            memoryBankAddressPtr = (unsigned char*)0x0000;
            break;
    }
    return memoryBankAddressPtr;
}

unsigned char* SetVic2VideoMemoryBank(
        // [in] VIC-II video memory bank index [3-0]. The VIC-II video memory bank will take 16384 chars and can only start at an
        // address that is a multiple of 16384 chars from the start of the 6510 memory map. Set the index from 3 to 0:
        //      3 for offset 0,
        //      2 for offset 16384,
        //      1 for offset 32768,
        //      0 for offset 49152
        // from the start of the 6510 memory map.
        unsigned char memoryBankIndex)
{
    // The currently selected block of 16384 chars that is used by the VIC-II chip is known as the video memory bank. The VIC-II
    // chip can only address 16384 chars of memory at a time. Setting the current VIC-II video memory bank as follows:
    //      %11 = 0
    //      %10 = 16384
    //      %01 = 32768
    //      %00 = 49152
    *(unsigned char*)0xDD00 = ((*(unsigned char*)0xDD00) & 0xFC) | memoryBankIndex;
    return GetVic2VideoMemoryBank();
}

/** VIC-II Graphics Chip Routines - Character Mode */

void GetVic2CharacterModeMemoryMappedAddresses(
        // [in] ptr to CharacterModeMemoryMappedAddresses struct, will set all fields from current VIC-II register values
        CharacterModeMemoryMappedAddresses_t *memoryMappedAddressesPtr)
{
    unsigned char characterSetDataIndex;
    unsigned char characterScreenDataIndex;

    memoryMappedAddressesPtr->vic2MemoryBankPtr = GetVic2VideoMemoryBank();
    characterSetDataIndex = (unsigned char)(*VICII_MEMORY) & 0x0E;
    memoryMappedAddressesPtr->characterSetDataPtr = memoryMappedAddressesPtr->vic2MemoryBankPtr + (unsigned short)characterSetDataIndex*1024;
    characterScreenDataIndex = ((unsigned char)(*VICII_MEMORY) & 0xF0) >> 4;
    memoryMappedAddressesPtr->screenDataPtr = memoryMappedAddressesPtr->vic2MemoryBankPtr + (unsigned short)characterScreenDataIndex*1024;
    memoryMappedAddressesPtr->spriteDataPtrs = memoryMappedAddressesPtr->screenDataPtr + 1016;
}

void SetVic2CharacterModeMemoryLocations(
        // [in] character screen data index [0/15]. The character screen data will take 1024 chars of the available VIC-II video bank
        // and can only start at a location that is a multiple of 1024 chars from the start of the currently selected VIC-II video
        // bank. Set the index to 0 to place character screen data at offset 0, 1 for offset 1024, 2 for offset 2048, ..., 15 for
        // offset 15360 from the start of the currently selected VIC-II video bank.
        unsigned char characterScreenDataIndex,
        // [in] character set memory data index [0,2,4,6,8,10,12,14]. The character set memory data will take 2048 chars of the
        // available VIC-II video bank and can only start at an address that is an even multiple of 1024 chars from the start of
        // the currently selected VIC-II video bank. Set the index to an even value from 0 to 14: 0 for offset 0000, 2 for offset 2048,
        // 4 for offset 4096, ..., 14 for offset 14336 from the start of the currently selected VIC-II video bank.
        unsigned char characterSetDataIndex,
        // [in] ptr to CharacterModeMemoryMappedAddresses struct, will set screenDataPtr and characterSetDataPtr values
        CharacterModeMemoryMappedAddresses_t *memoryMappedAddressesPtr)
{
    *(unsigned char*)VICII_MEMORY = (characterScreenDataIndex << 4) | characterSetDataIndex;
    memoryMappedAddressesPtr->characterSetDataPtr = memoryMappedAddressesPtr->vic2MemoryBankPtr + (unsigned short)characterSetDataIndex*1024;
    memoryMappedAddressesPtr->screenDataPtr = memoryMappedAddressesPtr->vic2MemoryBankPtr + (unsigned short)characterScreenDataIndex*1024;
}

/** VIC-II Graphics Chip Routines - Bitmap Mode */

void GetVic2BitmapModeMemoryMappedAddresses(
        // [in] ptr to BitmapModeMemoryMappedAddresses struct, will set all fields from current VIC-II register values
        BitmapModeMemoryMappedAddresses_t *memoryMappedAddressesPtr)
{
    unsigned char bitmapDataIndex;
    unsigned char colorMemoryDataIndex;

    memoryMappedAddressesPtr->vic2MemoryBankPtr = GetVic2VideoMemoryBank();
    bitmapDataIndex = ((unsigned char)(*VICII_MEMORY) & 0x08) >> 3;
    memoryMappedAddressesPtr->bitmapDataPtr = memoryMappedAddressesPtr->vic2MemoryBankPtr + (unsigned short)bitmapDataIndex*8192;
    colorMemoryDataIndex = ((unsigned char)(*VICII_MEMORY) & 0xF0) >> 4;
    memoryMappedAddressesPtr->colorDataPtr = memoryMappedAddressesPtr->vic2MemoryBankPtr + (unsigned short)colorMemoryDataIndex*1024;
}

void SetVic2BitmapModeMemoryLocations(
        // [in] bitmap data index [0/1]. The bitmap data will take 8000 chars of the available VIC-II video bank
        // and can only start at a location that is a multiple of 8192 chars from the start of the currently selected VIC-II video
        // bank. Set the index to 0 to place bitmap data at offset 0 and set the index to 1 for offset 8192 from the start
        // of the currently selected VIC-II video bank.
        unsigned char bitmapDataIndex,
        // [in] color data index [0-15]. The bitmap color data will take 1000 chars of the available VIC-II video
        // bank and can only start at an address that is a multiple of 1024 chars from the start of the currently selected VIC-II
        // video bank. Set the index from 0 to 15: 0 for offset 0000, 1 for offset 1024, 2 for offset 2048, ..., 15 for offset
        // 15360 from the start of the currently selected VIC-II video bank.
        unsigned char colorDataIndex,
        // [in] ptr to BitmapModeMemoryMappedAddresses struct, will set bitmapDataPtr and colorDataPtr values
        BitmapModeMemoryMappedAddresses_t *memoryMappedAddressesPtr)
{
    *(unsigned char*)VICII_MEMORY = (colorDataIndex << 4) | (bitmapDataIndex << 3);
    memoryMappedAddressesPtr->bitmapDataPtr = memoryMappedAddressesPtr->vic2MemoryBankPtr + (unsigned short)bitmapDataIndex*8192;
    memoryMappedAddressesPtr->colorDataPtr = memoryMappedAddressesPtr->vic2MemoryBankPtr + (unsigned short)colorDataIndex*1024;
}

/** Generic routines for all graphics modes */

void SetScreenBackgroundAndBorderColors(
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor0,
        // [in] border color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char borderColor)
{
    *BG_COLOR0 = backgroundColor0;
    *BORDER_COLOR = borderColor;
}

/** Standard Character Mode */

void SetMode_StandardCharacterMode(
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor0)
{
    *VICII_CONTROL1 = *VICII_CONTROL1 & (~VICII_BMM);
    *VICII_CONTROL1 = *VICII_CONTROL1 & (~VICII_ECM);
    *VICII_CONTROL2 = *VICII_CONTROL2 & (~VICII_MCM);    
    *BG_COLOR0 = backgroundColor0;
}

void SetScreenForegroundColor_StandardCharacterMode(
        // [in] text foreground color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char foregroundColor)
{
    memset(COLORRAM, foregroundColor, 1000);
}

void SetScreenBackgroundColor_StandardCharacterMode(
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor0)
{
    *BG_COLOR0 = backgroundColor0;
}

void FillScreen_StandardCharacterMode(
        // [in] PETSCII code of the character to draw
        char ch,
        // [in] screenDataPtr from CharacterModeMemoryMappedAddresses_t
        unsigned char* screenDataPtr)
{
    memset(screenDataPtr, ch, 1000);
}

void ClearScreen_StandardCharacterMode(
        // [in] screenDataPtr from CharacterModeMemoryMappedAddresses_t
        unsigned char* screenDataPtr)
{
    FillScreen_StandardCharacterMode(0x20, screenDataPtr); //0x20 is the PETSCII <space> character code
}

void DrawChar_StandardCharacterMode(
        // [in] PETSCII code of the character to draw
        char ch,
        // [in] column index, a number between 0-39
        unsigned char x,
        // [in] row index, a number between 0-24
        unsigned char y,
        // [in] screenDataPtr from CharacterModeMemoryMappedAddresses_t
        unsigned char* screenDataPtr)
{
#ifdef SAFE_DRAW
    if ( (x < CHARACTER_SCREEN_WIDTH) && (y < CHARACTER_SCREEN_HEIGHT) )
    {
#endif
        screenDataPtr[(unsigned short)y*CHARACTER_SCREEN_WIDTH + x] = ch;
#ifdef SAFE_DRAW
    }
#endif
}

void SetCharForegroundColor_StandardCharacterMode(
        // [in] text foreground color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char foregroundColor,
        // [in] column index, a number between 0-39
        unsigned char x,
        // [in] row index, a number between 0-24
        unsigned char y)
{
#ifdef SAFE_DRAW
    if ( (x < CHARACTER_SCREEN_WIDTH) && (y < CHARACTER_SCREEN_HEIGHT) )
    {
#endif
        COLORRAM[(unsigned short)y*CHARACTER_SCREEN_WIDTH + x] = foregroundColor;
#ifdef SAFE_DRAW
    }
#endif
}

void DrawCharAndSetForegroundColor_StandardCharacterMode(
        // [in] PETSCII code of the character to draw
        char ch,
        // [in] text foreground color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char foregroundColor,
        // [in] column index, a number between 0-39
        unsigned char x,
        // [in] row index, a number between 0-24
        unsigned char y,
        // [in] screenDataPtr from CharacterModeMemoryMappedAddresses_t
        unsigned char* screenDataPtr)
{
#ifdef SAFE_DRAW
    if ( (x < CHARACTER_SCREEN_WIDTH) && (y < CHARACTER_SCREEN_HEIGHT) )
    {
#endif
        DrawChar_StandardCharacterMode(ch, x, y, screenDataPtr);
        SetCharForegroundColor_StandardCharacterMode(foregroundColor, x, y);
#ifdef SAFE_DRAW
    }
#endif
}

void DrawRegion_StandardCharacterMode(
        // [in] character map to use
        char *characterRegionPtr,
        // [in] color map to use
        char *colorRegionPtr,
        // [in] region column width, a number between 1-40
        unsigned char xRegionWidth,
        // [in] region row height, a number between 1-25
        unsigned char yRegionHeight,
        // [in] screen column offset, a number between 0-39
        unsigned char xScreenOffset,
        // [in] screen row offset, a number between 0-24
        unsigned char yScreenOffset,
        // [in] screenDataPtr from CharacterModeMemoryMappedAddresses_t
        unsigned char* screenDataPtr)
{
    unsigned short i;
    unsigned short regionOffset;
    unsigned short screenOffset;

    for (i=0; i<yRegionHeight; i++)
    {
#ifdef KICKC
        regionOffset = (unsigned short)mul16u(i, xRegionWidth);
        screenOffset = (unsigned short)mul16u(i+yScreenOffset, CHARACTER_SCREEN_WIDTH) + xScreenOffset;
#else
        regionOffset = i*xRegionWidth;
        screenOffset = (unsigned short)(i+yScreenOffset)*CHARACTER_SCREEN_WIDTH + xScreenOffset;
#endif
        memcpy(&screenDataPtr[screenOffset], &characterRegionPtr[regionOffset], xRegionWidth);
        memcpy(&COLORRAM[screenOffset], &colorRegionPtr[regionOffset], xRegionWidth);
    }
}

// Generalized Bresenham's line algorithm for Octant0: https://www.phatcode.net/res/224/files/html/ch35/35-03.html
void DrawOctant0Line_StandardCharacterMode(
        // [in] PETSCII code of the character to draw
        char ch,
        // [in] text foreground color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char foregroundColor,
        // [in] starting column index, a number between 0-39
        unsigned char x0,
        // [in] starting row index, a number between 0-24
        unsigned char y0,
        // [in] horizontal line length (must be > 0)
        signed char deltaX,
        // [in] vertical line length (must be > 0)
        signed char deltaY,
        // [in] -1 to draw line from right to left, 1 to draw from left to right
        signed char xDirection,
        // [in] screenDataPtr from CharacterModeMemoryMappedAddresses_t
        unsigned char* screenDataPtr)
{
    signed char deltaYx2 = deltaY * 2;
    signed char deltaYx2MinusDeltaXx2 = deltaYx2 - (deltaX * 2);
    signed char error = deltaYx2 - deltaX;
    unsigned char x = x0;
    unsigned char y = y0;

#ifdef DEBUG
    printf("0:dyx2-dxx2=%d,dyx2=%d,dx=%d,dy=%d,xdir=%d ", deltaYx2MinusDeltaXx2, deltaYx2, deltaX, deltaY, xDirection);
#endif
    DrawCharAndSetForegroundColor_StandardCharacterMode(ch, foregroundColor, x, y, screenDataPtr);
    while (deltaX--)
    {
#ifdef DEBUG
        printf("0:%x,%x,%d ", x, y, error);
#endif
        if (error >= 0)
        {
            y++;
            error += deltaYx2MinusDeltaXx2;
        }
        else
        {
            error += deltaYx2;
        }
        x += xDirection;
        DrawCharAndSetForegroundColor_StandardCharacterMode(ch, foregroundColor, x, y, screenDataPtr);
    }
}

// Generalized Bresenham's line algorithm for Octant1: https://www.phatcode.net/res/224/files/html/ch35/35-03.html
void DrawOctant1Line_StandardCharacterMode(
        // [in] PETSCII code of the character to draw
        char ch,
        // [in] text foreground color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char foregroundColor,
        // [in] starting column index, a number between 0-39
        unsigned char x0,
        // [in] starting row index, a number between 0-24
        unsigned char y0,
        // [in] horizontal line length (must be > 0)
        signed char deltaX,
        // [in] vertical line length (must be > 0)
        signed char deltaY,
        // [in] -1 to draw line from right to left, 1 to draw from left to right
        signed char xDirection,
        // [in] screenDataPtr from CharacterModeMemoryMappedAddresses_t
        unsigned char* screenDataPtr)
{
    signed char deltaXx2 = deltaX * 2;
    signed char deltaXx2MinusDeltaYx2 = deltaXx2 - (deltaY * 2);
    signed char error = deltaXx2 - deltaY;
    unsigned char x = x0;
    unsigned char y = y0;

#ifdef DEBUG
    printf("1:dxx2-dyx2=%d,dxx2=%d,dx=%d,dy=%d,xdir=%d ", deltaXx2MinusDeltaYx2, deltaXx2, deltaX, deltaY, xDirection);
#endif
    DrawCharAndSetForegroundColor_StandardCharacterMode(ch, foregroundColor, x, y, screenDataPtr);
    while (deltaY--)
    {
#ifdef DEBUG
        printf("1:%x,%x,%d ", x, y, error);
#endif
        if (error >= 0)
        {
            x += xDirection;
            error += deltaXx2MinusDeltaYx2;
        }
        else
        {
            error += deltaXx2;
        }
        y++;
        DrawCharAndSetForegroundColor_StandardCharacterMode(ch, foregroundColor, x, y, screenDataPtr);
    }
}

// Generalized Bresenham's line algorithm: https://www.phatcode.net/res/224/files/html/ch35/35-03.html
void DrawLine_StandardCharacterMode(
        // [in] PETSCII code of the character to draw
        char ch,
        // [in] text foreground color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char foregroundColor,
        // [in] starting column index, a number between 0-39
        unsigned char x0,
        // [in] starting row index, a number between 0-24
        unsigned char y0,
        // [in] ending column index, a number between 0-39
        unsigned char x1,
        // [in] ending row index, a number between 0-24
        unsigned char y1,
        // [in] screenDataPtr from CharacterModeMemoryMappedAddresses_t
        unsigned char* screenDataPtr)
{
    signed char deltaX;
    signed char deltaY;

    if (y0 > y1)
    {
        unsigned char temp = y0;
        y0 = y1;
        y1 = temp;

        temp = x0;
        x0 = x1;
        x1 = temp;
    }

    deltaX = (signed char)x1 - (signed char)x0;
    deltaY = (signed char)y1 - (signed char)y0;
    if (deltaX > 0)
    {
        if (deltaX > deltaY)
        {
#ifdef DEBUG
            printf("a0:x0=%x,y0=%x,x1=%x,y1=%x,dx=%d,dy=%d,xdir=%d ", x0, y0, x1, y1, deltaX, deltaY, 1);
#endif
            DrawOctant0Line_StandardCharacterMode(ch, foregroundColor, x0, y0, deltaX, deltaY, (signed char)1, screenDataPtr);
        }
        else
        {
#ifdef DEBUG
            printf("a1:x0=%x,y0=%x,x1=%x,y1=%x,dx=%d,dy=%d,xdir=%d ", x0, y0, x1, y1, deltaX, deltaY, 1);
#endif
            DrawOctant1Line_StandardCharacterMode(ch, foregroundColor, x0, y0, deltaX, deltaY, (signed char)1, screenDataPtr);
        }
    }
    else
    {
        deltaX = -deltaX;
        if (deltaX > deltaY)
        {
#ifdef DEBUG
            printf("b0:x0=%x,y0=%x,x1=%x,y1=%x,dx=%d,dy=%d,xdir=%d ", x0, y0, x1, y1, deltaX, deltaY, -1);
#endif
            DrawOctant0Line_StandardCharacterMode(ch, foregroundColor, x0, y0, deltaX, deltaY, (signed char)-1, screenDataPtr);
        }
        else
        {
#ifdef DEBUG
            printf("b1:x0=%x,y0=%x,x1=%x,y1=%x,dx=%d,dy=%d,xdir=%d ", x0, y0, x1, y1, deltaX, deltaY, -1);
#endif
            DrawOctant1Line_StandardCharacterMode(ch, foregroundColor, x0, y0, deltaX, deltaY, (signed char)-1, screenDataPtr);
        }
    }
}

void DrawHorizontalLine_StandardCharacterMode(
        // [in] PETSCII code of the character to draw
        char ch,
        // [in] text foreground color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char foregroundColor,
        // [in] starting column index, a number between 0-39
        unsigned char x0,
        // [in] ending column index, a number between 0-39
        unsigned char x1,
        // [in] row index, a number between 0-24
        unsigned char y,
        // [in] screenDataPtr from CharacterModeMemoryMappedAddresses_t
        unsigned char* screenDataPtr)
{
    unsigned char temp;
    unsigned short index;
    unsigned char length;

    if (x1 < x0)
    {
        temp = x0;
        x0 = x1;
        x1 = temp;
    }
#ifdef SAFE_DRAW
    if (x1 > (CHARACTER_SCREEN_WIDTH-1))
    {
        x1 = (CHARACTER_SCREEN_WIDTH-1);
    }
    if ( (y < CHARACTER_SCREEN_HEIGHT) && (x0 < CHARACTER_SCREEN_WIDTH) && (x0 < x1) )
    {
#endif
        index = (unsigned short)y*CHARACTER_SCREEN_WIDTH + x0;
        length = x1-x0;
        memset(&screenDataPtr[index], ch, length);
        memset(&COLORRAM[index], foregroundColor, length);
#ifdef SAFE_DRAW
    }
#endif
}

void DrawVerticalLine_StandardCharacterMode(
        // [in] PETSCII code of the character to draw
        char ch,
        // [in] text foreground color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char foregroundColor,
        // [in] column index, a number between 0-39
        unsigned char x,
        // [in] starting row index, a number between 0-24
        unsigned char y0,
        // [in] ending row index, a number between 0-24
        unsigned char y1,
        // [in] screenDataPtr from CharacterModeMemoryMappedAddresses_t
        unsigned char* screenDataPtr)
{
    unsigned char y;
    unsigned char temp;

    if (y1 < y0)
    {
        temp = y0;
        y0 = y1;
        y1 = temp;
    }
#ifdef SAFE_DRAW
    if (y1 > (CHARACTER_SCREEN_HEIGHT-1))
    {
        y1 = (CHARACTER_SCREEN_HEIGHT-1);
    }
    if ( (x < CHARACTER_SCREEN_WIDTH) && (y0 < CHARACTER_SCREEN_HEIGHT) && (y0 < y1) )
    {
#endif
        for (y = y0; y <= y1; y++)
        {
            DrawCharAndSetForegroundColor_StandardCharacterMode(ch, foregroundColor, x, y, screenDataPtr);
        }
#ifdef SAFE_DRAW
    }
#endif
}

void DrawRectangle_StandardCharacterMode(
        // [in] PETSCII code of the character to draw
        char ch,
        // [in] text foreground color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char foregroundColor,
        // [in] column index of top left corner of rectangle, a number between 0-39
        unsigned char x,
        // [in] row index of top left corner of rectangle, a number between 0-24
        unsigned char y,
        // [in] column width of rectangle, a number between 0-39 (must be on screen)
        unsigned char width,
        // [in] row height of rectangle, a number between 0-24 (must be on screen)
        unsigned char height,
        // [in] screenDataPtr from CharacterModeMemoryMappedAddresses_t
        unsigned char* screenDataPtr)
{
    DrawHorizontalLine_StandardCharacterMode(ch, foregroundColor, x,       x+width, y,        screenDataPtr);
    DrawHorizontalLine_StandardCharacterMode(ch, foregroundColor, x,       x+width, y+height, screenDataPtr);
    DrawVerticalLine_StandardCharacterMode(  ch, foregroundColor, x,       y,       y+height, screenDataPtr);
    DrawVerticalLine_StandardCharacterMode(  ch, foregroundColor, x+width, y,       y+height, screenDataPtr);
}

void DrawTriangle_StandardCharacterMode(
        // [in] PETSCII code of the character to draw
        char ch,
        // [in] text foreground color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char foregroundColor,
        // [in] column index of corner of the triangle, a number between 0-39
        unsigned char x1,
        // [in] row index of of corner of the triangle, a number between 0-24
        unsigned char y1,
        // [in] column index of corner of the triangle, a number between 0-39
        unsigned char x2,
        // [in] row index of of corner of the triangle, a number between 0-24
        unsigned char y2,
        // [in] column index of corner of the triangle, a number between 0-39
        unsigned char x3,
        // [in] row index of of corner of the triangle, a number between 0-24
        unsigned char y3,
        // [in] screenDataPtr from CharacterModeMemoryMappedAddresses_t
        unsigned char* screenDataPtr)
{
    DrawLine_StandardCharacterMode(ch, foregroundColor, x1, y1, x2, y2, screenDataPtr);
    DrawLine_StandardCharacterMode(ch, foregroundColor, x2, y2, x3, y3, screenDataPtr);
    DrawLine_StandardCharacterMode(ch, foregroundColor, x3, y3, x1, y1, screenDataPtr);
}

// Bresenham's Circle Algorithm: http://members.chello.at/%7Eeasyfilter/Bresenham.pdf
void DrawCircle_StandardCharacterMode(
        // [in] PETSCII code of the character to draw
        char ch,
        // [in] text foreground color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char foregroundColor,
        // [in] column index of the center of the circle, a number between 0-39
        unsigned char x0,
        // [in] row index of the center of the circle, a number between 0-24
        unsigned char y0,
        // [in] radius of circle
        unsigned char radius,
        // [in] screenDataPtr from CharacterModeMemoryMappedAddresses_t
        unsigned char* screenDataPtr)
{
    signed char sRadius = (signed char)radius;
    signed char x = -sRadius;
    signed char y = 0;
    signed char error = 2-(2*sRadius);
    signed char point0_x;
    signed char point0_y;
    signed char point1_x;
    signed char point1_y;
    signed char point2_x;
    signed char point2_y;
    signed char point3_x;
    signed char point3_y;

    do
    {
        point0_x = (signed char)x0 - x;
        point0_y = (signed char)y0 + y;
        DrawCharAndSetForegroundColor_StandardCharacterMode(ch, foregroundColor, (unsigned char)point0_x, (unsigned char)point0_y, screenDataPtr);
        point1_x = (signed char)x0 - y;
        point1_y = (signed char)y0 - x;
        DrawCharAndSetForegroundColor_StandardCharacterMode(ch, foregroundColor, (unsigned char)point1_x, (unsigned char)point1_y, screenDataPtr);
        point2_x = (signed char)x0 + x;
        point2_y = (signed char)y0 - y;
        DrawCharAndSetForegroundColor_StandardCharacterMode(ch, foregroundColor, (unsigned char)point2_x, (unsigned char)point2_y, screenDataPtr);
        point3_x = (signed char)x0 + y;
        point3_y = (signed char)y0 + x;
        DrawCharAndSetForegroundColor_StandardCharacterMode(ch, foregroundColor, (unsigned char)point3_x, (unsigned char)point3_y, screenDataPtr);
        sRadius = error;
        if (sRadius <= y)
        {
            y++;
            error += y*2+1;
        }
        if ( (sRadius > x) || (error > y) )
        {
            x++;
            error += x*2+1;
        }
    } while (x < 0);
}

/** Multicolor Character Mode
 *
 * Use *_StandardCharacterMode routines but ensure that foreground color has always has high (i.e. 0x08) bit set
 * for multicolor character mode. This means that the only foreground color codes that can be used in multicolor
 * mode are:
 *
 * 0x08 = multicolor mode BLACK
 * 0x09 = multicolor mode WHITE
 * 0x0A = multicolor mode RED
 * 0x0B = multicolor mode CYAN
 * 0x0C = multicolor mode PURPLE
 * 0x0D = multicolor mode GREEN
 * 0x0E = multicolor mode BLUE
 * 0x0F = multicolor mode YELLOW
 *
 * If the high bit (0x08) isn't set in the foreground color then the character is drawn by the VIC-II chip in
 * standard character mode (thus you can mix standard and multicolor mode characters on the screen, but with a
 * limited foreground color palette):
 *
 * 0x01 = standard mode BLACK
 * 0x02 = standard mode WHITE
 * 0x03 = standard mode RED
 * 0x04 = standard mode CYAN
 * 0x05 = standard mode PURPLE
 * 0x06 = standard mode GREEN
 * 0x07 = standard mode BLUE
 * 
 * The three background colors can be chosen from the entire C64 color palette.
 */

void SetMode_MulticolorCharacterMode(
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor0,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor1,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor2)
{
    *VICII_CONTROL1 = *VICII_CONTROL1 & (~VICII_BMM);
    *VICII_CONTROL1 = *VICII_CONTROL1 & (~VICII_ECM);
    *VICII_CONTROL2 = *VICII_CONTROL2 | VICII_MCM;    
    *BG_COLOR0 = backgroundColor0;
    *BG_COLOR1 = backgroundColor1;
    *BG_COLOR2 = backgroundColor2;
}

void SetScreenBackgroundColors_MulticolorCharacterMode(
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor0,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor1,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor2)
{
    *BG_COLOR0 = backgroundColor0;
    *BG_COLOR1 = backgroundColor1;
    *BG_COLOR2 = backgroundColor2;
}

/** Extended Background Color (Character) Mode
 *
 * Use *_StandardCharacterMode routines but ensure that character code (ch) is calculated using the CalculateCharacterCode_ExtendedBackgroundColorMode routine
 *
 * Because the high two bits (0x80 and 0x40) of each character are used to indicate background color this mode can only use the first 64 characters of the
 * current C64 character set.
 */

void SetMode_ExtendedBackgroundColorMode(
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor0,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor1,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor2,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor3)
{
    *VICII_CONTROL1 = *VICII_CONTROL1 & (~VICII_BMM);
    *VICII_CONTROL2 = *VICII_CONTROL2 & (~VICII_MCM);    
    *VICII_CONTROL1 = *VICII_CONTROL1 | (VICII_ECM);
    *BG_COLOR0 = backgroundColor0;
    *BG_COLOR1 = backgroundColor1;
    *BG_COLOR2 = backgroundColor2;
    *BG_COLOR3 = backgroundColor3;
}

void SetScreenBackgroundColors_ExtendedBackgroundColorMode(
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor0,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor1,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor2,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor3)
{
    *BG_COLOR0 = backgroundColor0;
    *BG_COLOR1 = backgroundColor1;
    *BG_COLOR2 = backgroundColor2;
    *BG_COLOR3 = backgroundColor3;
}

char CalculateCharacterCode_ExtendedBackgroundColorMode(
        // [in] PETSCII code of the character to draw
        char ch,
        // [in] background color index from 0-3, specify 0 for Background Color 0, specify 1 for Background Color 1, specify 2 for Background Color 2, and specify 3 for Background Color 3.
        char backgroundColorIndex)
{
    switch (backgroundColorIndex) {
        case 0:
            ch = (ch & 0x3f) | 0x00;
            break;
        case 1:
            ch = (ch & 0x3f) | 0x40;
            break;
        case 2:
            ch = (ch & 0x3f) | 0x80;
            break;
        case 3:
            ch = (ch & 0x3f) | 0xc0;
            break;
    }
    return ch;
}

/** Standard Bitmap Mode */

void SetMode_StandardBitmapMode(void)
{
    *VICII_CONTROL1 = *VICII_CONTROL1 | VICII_BMM;
    *VICII_CONTROL1 = *VICII_CONTROL1 & (~VICII_ECM);
    *VICII_CONTROL2 = *VICII_CONTROL2 & (~VICII_MCM);
}

void SetScreenColor_StandardBitmapMode(
        // [in] foreground color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char foregroundColor,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor,
        // [in] colorDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *colorDataPtr)
{
    unsigned char color = (foregroundColor << 4) | backgroundColor;
    memset(colorDataPtr, color, 1000);
}

void FillScreen_StandardBitmapMode(
        // [in] bitmapDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *bitmapDataPtr)
{
    memset(bitmapDataPtr, 0xff, 8000);
}

void ClearScreen_StandardBitmapMode(
        // [in] bitmapDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *bitmapDataPtr)
{
    memset(bitmapDataPtr, 0x00, 8000);
}

void DrawPixel_StandardBitmapMode(
        // [in] pixel column index, a number between 0-319
        unsigned short x,
        // [in] pixel row index, a number between 0-199
        unsigned short y,
        // [in] bitmapDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *bitmapDataPtr)
{
    unsigned short yCell;
    unsigned short xCell;
    unsigned short yByte;
    unsigned short index;
    unsigned char xBit;

#ifdef SAFE_DRAW
    if ( (x < BITMAP_SCREEN_WIDTH) && (y < BITMAP_SCREEN_HEIGHT) )
    {
#endif
        // For more information on bitmap memory mapping see Commodore 64 Sound and Graphics, pg127, by Peter Falconer.
        yCell = y/8;
        xCell = x/8;
        yByte = y & 7;
        index = (yCell*BITMAP_SCREEN_WIDTH) + (xCell*8) + yByte;
        xBit = 7 - (x & 7);
        bitmapDataPtr[index] = bitmapDataPtr[index] | (0x01 << xBit);
#ifdef SAFE_DRAW
    }
#endif
}

void ClearPixel_StandardBitmapMode(
        // [in] column index, a number between 0-319
        unsigned short x,
        // [in] row index, a number between 0-199
        unsigned short y,
        // [in] bitmapDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *bitmapDataPtr)
{
    unsigned short yCell;
    unsigned short xCell;
    unsigned short yByte;
    unsigned short index;
    unsigned char xBit;

#ifdef SAFE_DRAW
    if ( (x < BITMAP_SCREEN_WIDTH) && (y < BITMAP_SCREEN_HEIGHT) )
    {
#endif
        // For more information on bitmap memory mapping see Commodore 64 Sound and Graphics, pg127, by Peter Falconer.
        yCell = y/8;
        xCell = x/8;
        yByte = y & 7;
        index = (yCell*BITMAP_SCREEN_WIDTH) + (xCell*8) + yByte;
        xBit = 7 - (x & 7);
        bitmapDataPtr[index] = bitmapDataPtr[index] & (0xff - (0x01 << xBit));
#ifdef SAFE_DRAW
    }
#endif
}

void SetCellColor_StandardBitmapMode(
        // [in] foreground color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char foregroundColor,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor,
        // [in] 8x8 color cell column index, a number between 0-39
        unsigned char x,
        // [in] 8x8 color cell row index, a number between 0-24
        unsigned char y,
        // [in] colorDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *colorDataPtr)
{
    unsigned short index;

#ifdef SAFE_DRAW
    if ( (x < (BITMAP_SCREEN_CELL_WIDTH)) && (y < (BITMAP_SCREEN_CELL_HEIGHT)) )
    {
#endif
        index = ((unsigned short)y*(BITMAP_SCREEN_CELL_WIDTH)) + x;
        colorDataPtr[index] = (foregroundColor << 4) | backgroundColor;
#ifdef SAFE_DRAW
    }
#endif
}

// Generalized Bresenham's line algorithm for Octant0: https://www.phatcode.net/res/224/files/html/ch35/35-03.html
void DrawOctant0Line_StandardBitmapMode(
        // [in] starting column index, a number between 0-319
        unsigned short x0,
        // [in] starting row index, a number between 0-199
        unsigned short y0,
        // [in] horizontal line length (must be > 0)
        signed short deltaX,
        // [in] vertical line length (must be > 0)
        signed short deltaY,
        // [in] -1 to draw line from right to left, 1 to draw from left to right
        signed short xDirection,
        // [in] bitmapDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *bitmapDataPtr)
{
    signed short deltaYx2 = deltaY * 2;
    signed short deltaYx2MinusDeltaXx2 = deltaYx2 - (deltaX * 2);
    signed short error = deltaYx2 - deltaX;

    unsigned short x = x0;
    unsigned short y = y0;
#ifdef DEBUG
    printf("0:dyx2-dxx2=%d,dyx2=%d,dx=%d,dy=%d,xdir=%d ", deltaYx2MinusDeltaXx2, deltaYx2, deltaX, deltaY, xDirection);
#endif
    DrawPixel_StandardBitmapMode(x, y, bitmapDataPtr);
    while (deltaX > 0)
    {
        deltaX--;
#ifdef DEBUG
        printf("0:%x,%x,%d ", x, y, error);
#endif
        if (error >= 0)
        {
            y++;
            error += deltaYx2MinusDeltaXx2;
        }
        else
        {
            error += deltaYx2;
        }
        x += xDirection;
        DrawPixel_StandardBitmapMode(x, y, bitmapDataPtr);
    }
}

// Generalized Bresenham's line algorithm for Octant1: https://www.phatcode.net/res/224/files/html/ch35/35-03.html
void DrawOctant1Line_StandardBitmapMode(
        // [in] starting column index, a number between 0-319
        unsigned short x0,
        // [in] starting row index, a number between 0-199
        unsigned short y0,
        // [in] horizontal line length (must be > 0)
        signed short deltaX,
        // [in] vertical line length (must be > 0)
        signed short deltaY,
        // [in] -1 to draw line from right to left, 1 to draw from left to right
        signed short xDirection,
        // [in] bitmapDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *bitmapDataPtr)
{
    signed short deltaXx2 = deltaX * 2;
    signed short deltaXx2MinusDeltaYx2 = deltaXx2 - (deltaY * 2);
    signed short error = deltaXx2 - deltaY;

    unsigned short x = x0;
    unsigned short y = y0;
#ifdef DEBUG
    printf("1:dxx2-dyx2=%d,dxx2=%d,dx=%d,dy=%d,xdir=%d ", deltaXx2MinusDeltaYx2, deltaXx2, deltaX, deltaY, xDirection);
#endif
    DrawPixel_StandardBitmapMode(x, y, bitmapDataPtr);
    while (deltaY > 0)
    {
        deltaY--;
#ifdef DEBUG
        printf("1:%x,%x,%d ", x, y, error);
#endif
        if (error >= 0)
        {
            x += xDirection;
            error += deltaXx2MinusDeltaYx2;
        }
        else
        {
            error += deltaXx2;
        }
        y++;
        DrawPixel_StandardBitmapMode(x, y, bitmapDataPtr);
    }
}

// Generalized Bresenham's line algorithm: https://www.phatcode.net/res/224/files/html/ch35/35-03.html
void DrawLine_StandardBitmapMode(
        // [in] starting column index, a number between 0-319
        unsigned short x0,
        // [in] starting row index, a number between 0-199
        unsigned short y0,
        // [in] ending column index, a number between 0-319
        unsigned short x1,
        // [in] ending row index, a number between 0-199
        unsigned short y1,
        // [in] bitmapDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *bitmapDataPtr)
{
    unsigned short temp;
    signed short deltaX;
    signed short deltaY;

    if (y0 > y1)
    {
        temp = y0;
        y0 = y1;
        y1 = temp;

        temp = x0;
        x0 = x1;
        x1 = temp;
    }

    //TODO: add SAFE_DRAW logic

    deltaX = (signed short)x1 - (signed short)x0;
    deltaY = (signed short)y1 - (signed short)y0;
    if (deltaX > 0)
    {
        if (deltaX > deltaY)
        {
#ifdef DEBUG
            printf("a0:x0=%x,y0=%x,x1=%x,y1=%x,dx=%d,dy=%d,xdir=%d ", x0, y0, x1, y1, deltaX, deltaY, 1);
#endif
            DrawOctant0Line_StandardBitmapMode(x0, y0, deltaX, deltaY, (signed short)1, bitmapDataPtr);
        }
        else
        {
#ifdef DEBUG
            printf("a1:x0=%x,y0=%x,x1=%x,y1=%x,dx=%d,dy=%d,xdir=%d ", x0, y0, x1, y1, deltaX, deltaY, 1);
#endif
            DrawOctant1Line_StandardBitmapMode(x0, y0, deltaX, deltaY, (signed short)1, bitmapDataPtr);
        }
    }
    else
    {
        deltaX = -deltaX;
        if (deltaX > deltaY)
        {
#ifdef DEBUG
            printf("b0:x0=%x,y0=%x,x1=%x,y1=%x,dx=%d,dy=%d,xdir=%d ", x0, y0, x1, y1, deltaX, deltaY, -1);
#endif
            DrawOctant0Line_StandardBitmapMode(x0, y0, deltaX, deltaY, (signed short)-1, bitmapDataPtr);
        }
        else
        {
#ifdef DEBUG
            printf("b1:x0=%x,y0=%x,x1=%x,y1=%x,dx=%d,dy=%d,xdir=%d ", x0, y0, x1, y1, deltaX, deltaY, -1);
#endif
            DrawOctant1Line_StandardBitmapMode(x0, y0, deltaX, deltaY, (signed short)-1, bitmapDataPtr);
        }
    }
}

void DrawHorizontalLine_StandardBitmapMode(
        // [in] starting column index, a number between 0-319
        unsigned short x0,
        // [in] ending column index, a number between 0-319
        unsigned short x1,
        // [in] row index, a number between 0-199
        unsigned short y,
        // [in] bitmapDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *bitmapDataPtr)
{
    signed short xDirection;
    signed short x;

    if (x1 > x0)
    {
        xDirection = 1;
    }
    else
    {
        xDirection = -1;
    }

#ifdef SAFE_DRAW
    if (x0 > (BITMAP_SCREEN_WIDTH-1))
    {
        x0 = (BITMAP_SCREEN_WIDTH-1);
    }
    if (x1 > (BITMAP_SCREEN_WIDTH-1))
    {
        x1 = (BITMAP_SCREEN_WIDTH-1);
    }
    if (y < BITMAP_SCREEN_HEIGHT)
    {
#endif
        x = (signed short)x0;
        DrawPixel_StandardBitmapMode((unsigned short)x, y, bitmapDataPtr);
        while (x != (signed short)x1)
        {
            x += xDirection;
            DrawPixel_StandardBitmapMode((unsigned short)x, y, bitmapDataPtr);
        }
#ifdef SAFE_DRAW
    }
#endif
}

void DrawVerticalLine_StandardBitmapMode(
        // [in] column index, a number between 0-319
        unsigned short x,
        // [in] starting row index, a number between 0-199
        unsigned short y0,
        // [in] ending row index, a number between 0-199
        unsigned short y1,
        // [in] bitmapDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *bitmapDataPtr)
{
    signed short yDirection;
    signed short y;

    if (y1 > y0)
    {
        yDirection = 1;
    }
    else
    {
        yDirection = -1;
    }
#ifdef SAFE_DRAW
    if (y0 > (BITMAP_SCREEN_HEIGHT-1))
    {
        y0 = (BITMAP_SCREEN_HEIGHT-1);
    }
    if (y1 > (BITMAP_SCREEN_HEIGHT-1))
    {
        y1 = (BITMAP_SCREEN_HEIGHT-1);
    }
    if (x < BITMAP_SCREEN_WIDTH)
    {
#endif
        y = (signed short)y0;
        DrawPixel_StandardBitmapMode(x, (unsigned short)y, bitmapDataPtr);
        while (y != (signed short)y1)
        {
            y += yDirection;
            DrawPixel_StandardBitmapMode(x, (unsigned short)y, bitmapDataPtr);
        }
#ifdef SAFE_DRAW
    }
#endif
}

void DrawRectangle_StandardBitmapMode(
        // [in] column index of top left corner of rectangle, a number between 0-319
        unsigned short x,
        // [in] row index of top left corner of rectangle, a number between 0-199
        unsigned short y,
        // [in] column width of rectangle, a number between 0-319 (must be on screen)
        unsigned short width,
        // [in] row height of rectangle, a number between 0-199 (must be on screen)
        unsigned short height,
        // [in] bitmapDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *bitmapDataPtr)
{
    DrawHorizontalLine_StandardBitmapMode(x,       x+width, y,        bitmapDataPtr);
    DrawHorizontalLine_StandardBitmapMode(x,       x+width, y+height, bitmapDataPtr);
    DrawVerticalLine_StandardBitmapMode(  x,       y,       y+height, bitmapDataPtr);
    DrawVerticalLine_StandardBitmapMode(  x+width, y,       y+height, bitmapDataPtr);
}

void DrawTriangle_StandardBitmapMode(
        // [in] column index of corner of the triangle, a number between 0-319
        unsigned short x1,
        // [in] row index of corner of the triangle, a number between 0-199
        unsigned short y1,
        // [in] column index of corner of the triangle, a number between 0-319
        unsigned short x2,
        // [in] row index of corner of the triangle, a number between 0-199
        unsigned short y2,
        // [in] column index of corner of the triangle, a number between 0-319
        unsigned short x3,
        // [in] row index of corner of the triangle, a number between 0-199
        unsigned short y3,
        // [in] bitmapDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *bitmapDataPtr)
{
    DrawLine_StandardBitmapMode(x1, y1, x2, y2, bitmapDataPtr);
    DrawLine_StandardBitmapMode(x2, y2, x3, y3, bitmapDataPtr);
    DrawLine_StandardBitmapMode(x3, y3, x1, y1, bitmapDataPtr);
}

// Bresenham's Circle Algorithm: http://members.chello.at/~easyfilter/bresenham.html and http://members.chello.at/%7Eeasyfilter/Bresenham.pdf
void DrawCircle_StandardBitmapMode(
        // [in] column index of the center of the circle, a number between 0-319
        unsigned short x0,
        // [in] row index of the center of the circle, a number between 0-199
        unsigned short y0,
        // [in] radius of circle
        unsigned short radius,
        // [in] bitmapDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *bitmapDataPtr)
{
    signed short sRadius = (signed short)radius;
    signed short x = -sRadius;
    signed short y = 0;
    signed short error = 2-(2*sRadius);
    signed short point0_x;
    signed short point0_y;
    signed short point1_x;
    signed short point1_y;
    signed short point2_x;
    signed short point2_y;
    signed short point3_x;
    signed short point3_y;

    do
    {
        point0_x = (signed short)x0 - x;
        point0_y = (signed short)y0 + y;
        DrawPixel_StandardBitmapMode((unsigned short)point0_x, (unsigned short)point0_y, bitmapDataPtr);
        point1_x = (signed short)x0 - y;
        point1_y = (signed short)y0 - x;
        DrawPixel_StandardBitmapMode((unsigned short)point1_x, (unsigned short)point1_y, bitmapDataPtr);
        point2_x = (signed short)x0 + x;
        point2_y = (signed short)y0 - y;
        DrawPixel_StandardBitmapMode((unsigned short)point2_x, (unsigned short)point2_y, bitmapDataPtr);
        point3_x = (signed short)x0 + y;
        point3_y = (signed short)y0 + x;
        DrawPixel_StandardBitmapMode((unsigned short)point3_x, (unsigned short)point3_y, bitmapDataPtr);
        sRadius = error;
        if (sRadius <= y)
        {
            y++;
            error += y*2+1;
        }
        if ( (sRadius > x) || (error > y) )
        {
            x++;
            error +=  x*2+1;
        }
    } while (x < 0);
}

/** Multicolor Bitmap Mode */

void SetMode_MulticolorBitmapMode(
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor)
{
    *VICII_CONTROL1 = *VICII_CONTROL1 | VICII_BMM;
    *VICII_CONTROL1 = *VICII_CONTROL1 & (~VICII_ECM);
    *VICII_CONTROL2 = *VICII_CONTROL2 | VICII_MCM;
    *BG_COLOR = backgroundColor;
}

void ClearScreen_MulticolorBitmapMode(void)
{
}
