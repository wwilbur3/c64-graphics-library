/** Copyright 2024 Warren Wilbur - MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef C64_GRAPHICS_H
#define C64_GRAPHICS_H

#ifdef KICKC
    #include <c64.h>
#else
    #define BLACK          ((unsigned char)0x00)
    #define WHITE          ((unsigned char)0x01)
    #define RED            ((unsigned char)0x02)
    #define CYAN           ((unsigned char)0x03)
    #define PURPLE         ((unsigned char)0x04)
    #define GREEN          ((unsigned char)0x05)
    #define BLUE           ((unsigned char)0x06)
    #define YELLOW         ((unsigned char)0x07)
    #define ORANGE         ((unsigned char)0x08)
    #define BROWN          ((unsigned char)0x09)
    #define PINK           ((unsigned char)0x0a)
    #define DARK_GREY      ((unsigned char)0x0b)
    #define GREY           ((unsigned char)0x0c)
    #define LIGHT_GREEN    ((unsigned char)0x0d)
    #define LIGHT_BLUE     ((unsigned char)0x0e)
    #define LIGHT_GREY     ((unsigned char)0x0f)
#endif

// constants for C64 character modes
#define CHARACTER_SCREEN_WIDTH 40
#define CHARACTER_SCREEN_HEIGHT 25

// constants for C64 bitmap modes
#define BITMAP_SCREEN_WIDTH 320
#define BITMAP_SCREEN_HEIGHT 200
#define BITMAP_SCREEN_CELL_WIDTH 40
#define BITMAP_SCREEN_CELL_HEIGHT 25


/** Determines if drawing routines will truncate drawing outside the screen space (useful if you want to draw lines, rectangles,
 * circles that are only partly on screen) */
//#define SAFE_DRAW

/** VIC-II Graphics Chip Routines */

unsigned char* GetVic2VideoMemoryBank(void);

unsigned char* SetVic2VideoMemoryBank(
        // [in] VIC-II video memory bank index [3-0]. The VIC-II video memory bank will take 16384 chars and can only start at an
        // address that is a multiple of 16384 chars from the start of the 6510 memory map. Set the index from 3 to 0:
        //      3 for offset 0,
        //      2 for offset 16384,
        //      1 for offset 32768,
        //      0 for offset 49152
        // from the start of the 6510 memory map.
        unsigned char memoryBankIndex);

/** VIC-II Graphics Chip Routines - Character Mode */

struct CharacterModeMemoryMappedAddresses
{
    unsigned char *vic2MemoryBankPtr; //6502 address of memory mapped vic2 memory bank
    unsigned char *screenDataPtr; //6502 address of memory mapped character (PETSCII) code screen data
    unsigned char *characterSetDataPtr; //6502 address of memory mapped character set (pixel) definition data
    //color ram is fixed at 0xd800 in character graphics mode
};
typedef struct CharacterModeMemoryMappedAddresses CharacterModeMemoryMappedAddresses_t;

void GetVic2CharacterModeMemoryMappedAddresses(
        // [in] ptr to CharacterModeMemoryMappedAddresses struct, will set all fields from current VIC-II register values
        CharacterModeMemoryMappedAddresses_t *memoryMappedAddressesPtr);

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
        CharacterModeMemoryMappedAddresses_t *memoryMappedAddressesPtr);

/** VIC-II Graphics Chip Routines - Bitmap Mode */

struct BitmapModeMemoryMappedAddresses
{
    unsigned char *vic2MemoryBankPtr; //6502 address of memory mapped vic2 memory bank
    unsigned char *bitmapDataPtr; //6502 address of memory mapped bitmap (pixel) screen data
    unsigned char *colorDataPtr;  //6502 address of memory mapped bitmap (cell) color data
};
typedef struct BitmapModeMemoryMappedAddresses BitmapModeMemoryMappedAddresses_t;

void GetVic2BitmapModeMemoryMappedAddresses(
        // [in] ptr to BitmapModeMemoryMappedAddresses struct, will set all fields from current VIC-II register values
        BitmapModeMemoryMappedAddresses_t *memoryMappedAddressesPtr);

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
        BitmapModeMemoryMappedAddresses_t *memoryMappedAddressesPtr);

/** Generic routines for all graphics modes */

void SetScreenBackgroundAndBorderColors(
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor0,
        // [in] border color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char borderColor);

// Use this to prevent screen tearing
void WaitUntilRasterOffscreen(void);

/** Standard Character Mode */

void SetMode_StandardCharacterMode(
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor0);

void SetScreenForegroundColor_StandardCharacterMode(
        // [in] text foreground color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char foregroundColor);

void SetScreenBackgroundColor_StandardCharacterMode(
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor0);

void FillScreen_StandardCharacterMode(
        // [in] PETSCII code of the character to draw
        char ch,
        // [in] screenDataPtr from CharacterModeMemoryMappedAddresses_t
        unsigned char* screenDataPtr);

void ClearScreen_StandardCharacterMode(
        // [in] screenDataPtr from CharacterModeMemoryMappedAddresses_t
        unsigned char* screenDataPtr);

void DrawChar_StandardCharacterMode(
        // [in] PETSCII code of the character to draw
        char ch,
        // [in] column index, a number between 0-39
        unsigned char x,
        // [in] row index, a number between 0-24
        unsigned char y,
        // [in] screenDataPtr from CharacterModeMemoryMappedAddresses_t
        unsigned char* screenDataPtr);

void SetCharForegroundColor_StandardCharacterMode(
        // [in] text foreground color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char foregroundColor,
        // [in] column index, a number between 0-39
        unsigned char x,
        // [in] row index, a number between 0-24
        unsigned char y);

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
        unsigned char* screenDataPtr);

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
        unsigned char* screenDataPtr);

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
        unsigned char* screenDataPtr);

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
        unsigned char* screenDataPtr);

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
        unsigned char* screenDataPtr);

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
        unsigned char* screenDataPtr);

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
        unsigned char* screenDataPtr);

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
 * Because the high bit in the foreground color is used to indicate Multicolor Character Mode (for each character),
 * this prevents the use of Orange, Brown, Light Red, Dark Grey, Grey, Light Green, Light Blue, or Light Grey as
 * foreground colors (in Multicolor Character Mode).
 * 
 * If the high bit (0x08) isn't set in the foreground color then the character is drawn by the VIC-II chip in
 * standard character mode. Thus you can mix standard and multicolor mode characters on the screen, but with a
 * limited foreground color palette:
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
        unsigned char backgroundColor2);

void SetScreenBackgroundColors_MulticolorCharacterMode(
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor0,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor1,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor2);

/** Extended Background Color (Character) Mode
 *
 * Use *_StandardCharacterMode routines but ensure that character code (ch) is calculated using the
 * CalculateCharacterCode_ExtendedBackgroundColorMode routine.
 *
 * Because the high two bits (0x80 and 0x40) of each character are used to indicate background color this mode can
 * only use the first 64 characters of the current C64 character set.
 *
 * The four background colors can be chosen from the entire C64 color palette.
 */

void SetMode_ExtendedBackgroundColorMode(
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor0,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor1,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor2,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor3);

void SetScreenBackgroundColors_ExtendedBackgroundColorMode(
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor0,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor1,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor2,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor3);

char CalculateCharacterCode_ExtendedBackgroundColorMode(
        // [in] PETSCII code of the character to draw
        char ch,
        // [in] background color index from 0-3, specify 0 for Background Color 0, specify 1 for Background Color 1,
        // specify 2 for Background Color 2, and specify 3 for Background Color 3.
        char backgroundColorIndex);

/** Standard Bitmap Mode */

void SetMode_StandardBitmapMode(void);

void SetScreenColor_StandardBitmapMode(
        // [in] foreground color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char foregroundColor,
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor,
        // [in] colorDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *colorDataPtr);

void FillScreen_StandardBitmapMode(
        // [in] bitmapDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *bitmapDataPtr);

void ClearScreen_StandardBitmapMode(
        // [in] bitmapDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *bitmapDataPtr);

void DrawPixel_StandardBitmapMode(
        // [in] pixel column index, a number between 0-319
        unsigned short x,
        // [in] pixel row index, a number between 0-199
        unsigned short y,
        // [in] bitmapDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *bitmapDataPtr);

void ClearPixel_StandardBitmapMode(
        // [in] column index, a number between 0-319
        unsigned short x,
        // [in] row index, a number between 0-199
        unsigned short y,
        // [in] bitmapDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *bitmapDataPtr);

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
        unsigned char *colorDataPtr);

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
        unsigned char *bitmapDataPtr);

void DrawHorizontalLine_StandardBitmapMode(
        // [in] starting column index, a number between 0-319
        unsigned short x0,
        // [in] ending column index, a number between 0-319
        unsigned short x1,
        // [in] row index, a number between 0-199
        unsigned short y,
        // [in] bitmapDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *bitmapDataPtr);

void DrawVerticalLine_StandardBitmapMode(
        // [in] column index, a number between 0-319
        unsigned short x,
        // [in] starting row index, a number between 0-199
        unsigned short y0,
        // [in] ending row index, a number between 0-199
        unsigned short y1,
        // [in] bitmapDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *bitmapDataPtr);

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
        unsigned char *bitmapDataPtr);

void DrawCircle_StandardBitmapMode(
        // [in] column index of the center of the circle, a number between 0-319
        unsigned short x0,
        // [in] row index of the center of the circle, a number between 0-199
        unsigned short y0,
        // [in] radius of circle
        unsigned short radius,
        // [in] bitmapDataPtr from BitmapModeMemoryMappedAddresses_t
        unsigned char *bitmapDataPtr);

/** Multicolor Bitmap Mode */

void SetMode_MulticolorBitmapMode(
        // [in] background color is a number between 0-15 (use the color constants in c64.h to improve readability)
        unsigned char backgroundColor);

void ClearScreen_MulticolorBitmapMode(void);

#endif //C64_GRAPHICS_H

