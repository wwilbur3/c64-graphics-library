/** Copyright 2024 Warren Wilbur - MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the �Software�), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED �AS IS�, WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifdef KICKC
    #include <c64.h>
    #include <printf.h>
    #include <c64-keyboard.h>
    #include <multiply.h>
    #include <division.h>
    #include <stdlib.h>
    #include <c64-time.h>
    #include <c64-print.h>
    #include <division.h>
#else //SDCC, CC65, VBCC, OSCAR64
    #ifdef OSCAR64
        #define KEY_RETURN 0x01
        #include <conio.h>
    #else //SDCC, CC65, VBCC
        #define KEY_RETURN 0x0D
    #endif
    #include <stdio.h>
#endif

#include <string.h>

#include "c64-graphics.h"
#include "c64-util.h"

// Wait until the user presses <RETURN> after each drawing test
//#define PAUSE

// Print debug output during testing
//#define DEBUG

// Determines if drawing routines will attempt to draw outside the screen (for testing)
//#define TEST_SAFE_DRAW


int main()
{
    CharacterModeMemoryMappedAddresses_t characterModeAddresses;
    BitmapModeMemoryMappedAddresses_t bitmapModeAddresses;
    unsigned char color = 0;
    unsigned char color2;
    unsigned char color3;
    char ch = 0;
    unsigned char i = 0;
    signed char si;
    unsigned char width_c;
    unsigned char height_c;
    unsigned short j;
    signed short sj;
    unsigned short width_s;
    unsigned short height_s;

#ifdef KICKC
    keyboard_init();
#endif

#ifdef OSCAR64
    ch = 65; //Use PETSCII code 65 ('A')
#else
    ch = 'a'; //Use ASCII code 'a' (which equals PETSCII code 'A')
#endif
#ifdef OSCAR64
    //switch to the lowercase PETSCII font while printing (via printf)
    iocharmap(IOCHM_PETSCII_2);
#endif
    printf("\nwelcome to the c64 graphics library test");
    for (i=0; i<255; i++)
    {
        SetScreenForegroundColor_StandardCharacterMode(1);
        SetScreenBackgroundAndBorderColors(i&0x0f, i&0x0f);
    }
#ifdef PAUSE
    printf("      press return after each test");
    WaitUntilKeyPressed(KEY_RETURN);
#endif

    // standard character mode testing
    GetVic2CharacterModeMemoryMappedAddresses(&characterModeAddresses);
    //characterModeAddresses.vic2MemoryBankPtr = SetVic2VideoMemoryBank(1);
    //SetVic2CharacterModeMemoryLocations(1, 1, &characterModeAddresses);
    SetMode_StandardCharacterMode(BLACK);

    ClearScreen_StandardCharacterMode(characterModeAddresses.screenDataPtr);
    SetScreenForegroundColor_StandardCharacterMode(WHITE);
    SetScreenBackgroundAndBorderColors(BLACK, BLACK);
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif

    // screen fill test - standard character mode
    for (i=0; i<26; i++)
    {
        WaitUntilRasterOffscreen();
#ifdef OSCAR64
        ch = 65 + i; //Use PETSCII screen code 65 ('A')
#else
        ch = 'a' + i; //Use ASCII code 'a' (which equals PETSCII code 'A')
#endif
#ifdef DEBUG
        printf("s1:i=%x, char=%c ", i, ch);
#endif
        FillScreen_StandardCharacterMode(ch, characterModeAddresses.screenDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif

    // screen color test - standard character mode
    for (i=0; i<16; i++)
    {
        WaitUntilRasterOffscreen();
#ifdef DEBUG
        printf("s2:i=%x ", i);
#endif
        SetScreenForegroundColor_StandardCharacterMode(i);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif

    // line drawing tests - standard character mode
#ifdef OSCAR64
    ch = 65; //Use PETSCII screen code 65 ('A')
#else
    ch = 'a'; //Use ASCII code 'a' (which equals PETSCII code 'A')
#endif
    FillScreen_StandardCharacterMode(ch, characterModeAddresses.screenDataPtr);
    SetScreenForegroundColor_StandardCharacterMode(1);
    // line drawing tests 1 - standard character mode - originate from top-left
    for (i=0; i<CHARACTER_SCREEN_WIDTH; i++)
    {
        color = ++color % 16;
#ifdef DEBUG
        printf("l1a:x1=%x ", i);
#endif
        DrawLine_StandardCharacterMode(ch, color, 0, 0, i, (CHARACTER_SCREEN_HEIGHT-1), characterModeAddresses.screenDataPtr);
    }
    for (si=(CHARACTER_SCREEN_HEIGHT-1); si>=0; si--)
    {
        i = (unsigned char)si;
        color = ++color % 16;
#ifdef DEBUG
        printf("l1b:si=%d, y1=%x ", si, i);
#endif
        DrawLine_StandardCharacterMode(ch, color, 0, 0, (CHARACTER_SCREEN_WIDTH-1), i, characterModeAddresses.screenDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardCharacterMode(characterModeAddresses.screenDataPtr);

    FillScreen_StandardCharacterMode(ch, characterModeAddresses.screenDataPtr);
    SetScreenForegroundColor_StandardCharacterMode(1);
    // line drawing tests 2 - standard character mode - originate from bottom-left
    for (i=0; i<CHARACTER_SCREEN_WIDTH; i++)
    {
        color = ++color % 16;
#ifdef DEBUG
        printf("l2a:x1=%x ", i);
#endif
        DrawLine_StandardCharacterMode(ch, color, 0, (CHARACTER_SCREEN_HEIGHT-1), i, 0, characterModeAddresses.screenDataPtr);
    }
    for (i=0; i<CHARACTER_SCREEN_HEIGHT; i++)
    {
        color = ++color % 16;
#ifdef DEBUG
        printf("l2b:y1=%x ", i);
#endif
        DrawLine_StandardCharacterMode(ch, color, 0, (CHARACTER_SCREEN_HEIGHT-1), (CHARACTER_SCREEN_WIDTH-1), i, characterModeAddresses.screenDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardCharacterMode(characterModeAddresses.screenDataPtr);

    FillScreen_StandardCharacterMode(ch, characterModeAddresses.screenDataPtr);
    SetScreenForegroundColor_StandardCharacterMode(1);
    // line drawing tests 3 - standard character mode - originate from bottom-right
    for (si=(CHARACTER_SCREEN_WIDTH-1); si>=0; si--)
    {
        i = (unsigned char)si;
        color = ++color % 16;
#ifdef DEBUG
        printf("l3a:si=%d, x1=%x ", si, i);
#endif
        DrawLine_StandardCharacterMode(ch, color, (CHARACTER_SCREEN_WIDTH-1), (CHARACTER_SCREEN_HEIGHT-1), i, 0, characterModeAddresses.screenDataPtr);
    }
    for (i=0; i<CHARACTER_SCREEN_HEIGHT; i++)
    {
        color = ++color % 16;
#ifdef DEBUG
        printf("l3b:y1=%x ", i);
#endif
        DrawLine_StandardCharacterMode(ch, color, (CHARACTER_SCREEN_WIDTH-1), (CHARACTER_SCREEN_HEIGHT-1), 0, i, characterModeAddresses.screenDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardCharacterMode(characterModeAddresses.screenDataPtr);

    FillScreen_StandardCharacterMode(ch, characterModeAddresses.screenDataPtr);
    SetScreenForegroundColor_StandardCharacterMode(1);
    // line drawing tests 4 - standard character mode - originate from top-right
    for (si=(CHARACTER_SCREEN_WIDTH-1); si>=0; si--)
    {
        i = (unsigned char)si;
        color = ++color % 16;
#ifdef DEBUG
        printf("l4a:si=%d, x1=%x ", si, i);
#endif
        DrawLine_StandardCharacterMode(ch, color, (CHARACTER_SCREEN_WIDTH-1), 0, i, (CHARACTER_SCREEN_HEIGHT-1), characterModeAddresses.screenDataPtr);
    }
    for (si=(CHARACTER_SCREEN_HEIGHT-1); si>=0; si--)
    {
        i = (unsigned char)si;
        color = ++color % 16;
#ifdef DEBUG
        printf("l4b:si=%d, y1=%x ", si, i);
#endif
        DrawLine_StandardCharacterMode(ch, color, (CHARACTER_SCREEN_WIDTH-1), 0, 0, i, characterModeAddresses.screenDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardCharacterMode(characterModeAddresses.screenDataPtr);

    // rectangle drawing test - standard character mode
    for (i=0; i<(CHARACTER_SCREEN_HEIGHT/2)+1; i++)
    {
        width_c = (CHARACTER_SCREEN_WIDTH-1) - (i*2);
        height_c = (CHARACTER_SCREEN_HEIGHT-1) - (i*2);
        color = ++color % 16;
        DrawRectangle_StandardCharacterMode(ch, color, i, i, width_c, height_c, characterModeAddresses.screenDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardCharacterMode(characterModeAddresses.screenDataPtr);

    // circle drawing test - standard character mode
#ifdef TEST_SAFE_DRAW
    for (i=1; i<20; i++)
#else
    for (i=1; i<(CHARACTER_SCREEN_HEIGHT/2)+1; i++)
#endif
    {
        color = ++color % 16;
        DrawCircle_StandardCharacterMode(ch, color, CHARACTER_SCREEN_WIDTH/2, CHARACTER_SCREEN_HEIGHT/2, i, characterModeAddresses.screenDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardCharacterMode(characterModeAddresses.screenDataPtr);

    SetMode_MulticolorCharacterMode(0, 1, 2);
    // multicolor character mode testing
    for (i=0; i<26; i++)
    {
        color = ++color % 16;
        color2 = (color+1) % 16;
        color3 = (color+2) % 16;
#ifdef OSCAR64
        ch = 65 + i; //Use PETSCII screen code 65 ('A')
#else
        ch = 'a' + i; //Use ASCII code 'a' (which equals PETSCII code 'A')
#endif
        SetScreenBackgroundColors_MulticolorCharacterMode(color, color2, color3);
        FillScreen_StandardCharacterMode(ch, characterModeAddresses.screenDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardCharacterMode(characterModeAddresses.screenDataPtr);

    // standard bitmap mode testing
#ifdef DEBUG
    printf("SetMode_StandardBitmapMode()");
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    SetMode_StandardBitmapMode();
#ifdef DEBUG
    printf("GetVic2BitmapModeMemoryMappedAddresses()");
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    GetVic2BitmapModeMemoryMappedAddresses(&bitmapModeAddresses);
#ifdef DEBUG
    printf("SetVic2VideoMemoryBank()");
    WaitUntilKeyPressed(KEY_RETURN);
#endif
#ifdef VBCC
    bitmapModeAddresses.vic2MemoryBankPtr = SetVic2VideoMemoryBank(1);
#else
    bitmapModeAddresses.vic2MemoryBankPtr = SetVic2VideoMemoryBank(2);
#endif
#ifdef DEBUG
    printf("SetVic2BitmapModeMemoryLocations()");
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    SetVic2BitmapModeMemoryLocations(1, 1, &bitmapModeAddresses);

    // screen fill test - standard bitmap mode
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);
    FillScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    // screen color test - standard bitmap mode
    for (i=0; i<16; i++)
    {
        color = ++color % 16;
        WaitUntilRasterOffscreen();
        SetScreenColor_StandardBitmapMode(color, 0, bitmapModeAddresses.colorDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif

    // line drawing tests - standard bitmap mode
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);
    SetScreenColor_StandardBitmapMode(1, 0, bitmapModeAddresses.colorDataPtr);

    // line drawing tests - standard bitmap mode - originate from top-left
    for (j=0; j<BITMAP_SCREEN_WIDTH; j++)
    {
        DrawLine_StandardBitmapMode(0, 0, j, BITMAP_SCREEN_HEIGHT-1, bitmapModeAddresses.bitmapDataPtr);
    }
    for (sj=(BITMAP_SCREEN_HEIGHT-1); sj>=0; sj--)
    {
        DrawLine_StandardBitmapMode(0, 0, BITMAP_SCREEN_WIDTH-1, (unsigned short)sj, bitmapModeAddresses.bitmapDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    // line drawing tests - standard bitmap mode - originate from bottom-left
    for (j=0; j<BITMAP_SCREEN_WIDTH; j++)
    {
        DrawLine_StandardBitmapMode(0, (BITMAP_SCREEN_HEIGHT-1), j, 0, bitmapModeAddresses.bitmapDataPtr);
    }
    for (j=0; j<BITMAP_SCREEN_HEIGHT; j++)
    {
        DrawLine_StandardBitmapMode(0, (BITMAP_SCREEN_HEIGHT-1), (BITMAP_SCREEN_WIDTH-1), j, bitmapModeAddresses.bitmapDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    // line drawing tests - standard bitmap mode - originate from bottom-right
    for (sj=(BITMAP_SCREEN_WIDTH-1); sj>=0; sj--)
    {
        DrawLine_StandardBitmapMode((BITMAP_SCREEN_WIDTH-1), (BITMAP_SCREEN_HEIGHT-1), (unsigned short)sj, 0, bitmapModeAddresses.bitmapDataPtr);
    }
    for (j=0; j<BITMAP_SCREEN_HEIGHT; j++)
    {
        DrawLine_StandardBitmapMode((BITMAP_SCREEN_WIDTH-1), (BITMAP_SCREEN_HEIGHT-1), 0, j, bitmapModeAddresses.bitmapDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    // line drawing tests - standard bitmap mode - originate from top-right
    for (sj=(BITMAP_SCREEN_WIDTH-1); sj>=0; sj--)
    {
        DrawLine_StandardBitmapMode((BITMAP_SCREEN_WIDTH-1), 0, (unsigned short)sj, (BITMAP_SCREEN_HEIGHT-1), bitmapModeAddresses.bitmapDataPtr);
    }
    for (sj=(BITMAP_SCREEN_HEIGHT-1); sj>=0; sj--)
    {
        DrawLine_StandardBitmapMode((BITMAP_SCREEN_WIDTH-1), 0, 0, (unsigned short)sj, bitmapModeAddresses.bitmapDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    // rectangle drawing test - standard bitmap mode
    for (j=0; j<(BITMAP_SCREEN_HEIGHT/2); j++)
    {
        width_s = (BITMAP_SCREEN_WIDTH-1) - (j*2);
        height_s = (BITMAP_SCREEN_HEIGHT-1) - (j*2);
        DrawRectangle_StandardBitmapMode(j, j, width_s, height_s, bitmapModeAddresses.bitmapDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    // circle drawing test - standard bitmap mode
    for (j=1; j<(BITMAP_SCREEN_HEIGHT/2); j++)
    {
        DrawCircle_StandardBitmapMode(BITMAP_SCREEN_WIDTH/2, BITMAP_SCREEN_HEIGHT/2, j, bitmapModeAddresses.bitmapDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    SetMode_StandardCharacterMode(BLACK);
    ClearScreen_StandardCharacterMode(characterModeAddresses.screenDataPtr);
    SetScreenForegroundColor_StandardCharacterMode(WHITE);
    SetScreenBackgroundAndBorderColors(BLACK, BLACK);
    return 0;
}

#ifdef KICKC
 // Reserve zeropage addresses used by the BASIC FP operations
 #pragma reserve(0x07, 0x0d, 0x0e, 0x12)
#endif
