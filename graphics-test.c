/** Copyright 2024 Warren Wilbur - MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
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
#include "c64-charset.h"
#include "c64-sprites.h"
#include "c64-util.h"

// Wait until the user presses <RETURN> after each drawing test
//#define PAUSE

// Print debug output during testing
//#define DEBUG

// Determines if drawing routines will attempt to draw outside the screen (for testing)
//#define TEST_SAFE_DRAW

// Faster bitmap testing, increase loop increment value to not draw every line (e.g. two to draw every other line)
#define BITMAP_TEST_STEP 1

unsigned char const sprite_T[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x00,0x18,0x00,0x00,
                                  0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,
                                  0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,
                                  0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};

unsigned char const sprite_E[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x30,0x00,0x00,0x30,
                                  0x00,0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x3F,0xFF,0x00,0x3F,0xFF,
                                  0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x30,0x00,0x00,
                                  0x30,0x00,0x00,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x01};

unsigned char const sprite_S[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x30,0x00,0x00,0x30,
                                  0x00,0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x3F,0xFF,
                                  0xFC,0x3F,0xFF,0xFC,0x00,0x00,0x0C,0x00,0x00,0x0C,0x00,0x00,0x0C,0x00,0x00,0x0C,
                                  0x00,0x00,0x0C,0x3F,0xFF,0xFC,0x3F,0xFF,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x01};

unsigned char* sprite_T_Ptr  = (unsigned char*)0x3200;
unsigned char* sprite_E_Ptr  = (unsigned char*)0x3240;
unsigned char* sprite_S_Ptr  = (unsigned char*)0x3280;
unsigned char* sprite_T2_Ptr = (unsigned char*)0x32C0;

int main()
{
    CharacterModeMemoryMappedAddresses_t characterModeAddresses;
    BitmapModeMemoryMappedAddresses_t bitmapModeAddresses;
    unsigned char color = 0;
    unsigned char color2;
    unsigned char color3;
    char ch = 65; //Use PETSCII screen code 65 ('A')
    unsigned char i = 0;
    signed char si;
    unsigned char width_c;
    unsigned char height_c;
    unsigned short j;
    signed short sj;
    unsigned short width_s;
    unsigned short height_s;

    SetCharacterSetAddressIndex(C64_UPPERLOWERCASE_CHARACTER_SET_ADDRESS_INDEX);

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
#ifdef KICKC
    keyboard_init();
#endif
    WaitUntilKeyPressed(KEY_RETURN);
#endif

    // standard character mode testing
    GetVic2CharacterModeMemoryMappedAddresses(&characterModeAddresses);
    //characterModeAddresses.vic2MemoryBankPtr = SetVic2VideoMemoryBank(1);
    //SetVic2CharacterModeMemoryLocations(1, 1, &characterModeAddresses);
    SetMode_StandardCharacterMode(BLACK);

    memcpy(sprite_T_Ptr,  sprite_T, sizeof(sprite_T));
    memcpy(sprite_E_Ptr,  sprite_E, sizeof(sprite_E));
    memcpy(sprite_S_Ptr,  sprite_S, sizeof(sprite_S));
    memcpy(sprite_T2_Ptr, sprite_T, sizeof(sprite_T));

    MakeSingleColorSprite(0, sprite_T_Ptr,  1,  20, 20, 1, 1, characterModeAddresses.spriteDataPtrs);
    MakeSingleColorSprite(1, sprite_E_Ptr,  1,  65, 20, 1, 1, characterModeAddresses.spriteDataPtrs);
    MakeSingleColorSprite(2, sprite_S_Ptr,  1, 110, 20, 1, 1, characterModeAddresses.spriteDataPtrs);
    MakeSingleColorSprite(3, sprite_T2_Ptr, 1, 155, 20, 1, 1, characterModeAddresses.spriteDataPtrs);

    for (i=0; i<140; i++)
    {
        WaitUntilRasterOffscreen();
        WaitUntilRasterOffscreen();
        MoveSprite(0,  20+i, 20+i);
        MoveSprite(1,  65+i, 20+i);
        MoveSprite(2, 110+i, 20+i);
        MoveSprite(3, 155+i, 20+i);

        if ((i & 0x0F) == 0x08)
        {
            ReverseVerticalSprite(sprite_T_Ptr);
            ReverseVerticalSprite(sprite_E_Ptr);
            ReverseVerticalSprite(sprite_S_Ptr);
            ReverseVerticalSprite(sprite_T2_Ptr);
        }
        if ((i & 0x0F) == 0x04)
        {
            ReverseHorizontalSinglecolorSprite(sprite_T_Ptr);
            ReverseHorizontalSinglecolorSprite(sprite_E_Ptr);
            ReverseHorizontalSinglecolorSprite(sprite_S_Ptr);
            ReverseHorizontalSinglecolorSprite(sprite_T2_Ptr);
        }
    }

    RemoveSprite(0);
    RemoveSprite(1);
    RemoveSprite(2);
    RemoveSprite(3);

    ClearScreen_StandardCharacterMode(characterModeAddresses.screenDataPtr);
    SetScreenForegroundColor_StandardCharacterMode(WHITE);
    SetScreenBackgroundAndBorderColors(BLACK, BLACK);
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif

    // screen fill test - standard character mode
    for (i=0; i<26; i++)
    {
        ch = 65 + i; //Use PETSCII screen code 65 ('A')
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
#ifdef DEBUG
        printf("s2:i=%x ", i);
#endif
        SetScreenForegroundColor_StandardCharacterMode(i);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif

    // line drawing tests - standard character mode
    ch = 65; //Use PETSCII screen code 65 ('A')
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

    width_c = CHARACTER_SCREEN_WIDTH/2;
    // triangle drawing test - standard character mode
    for (i=0; i<(CHARACTER_SCREEN_HEIGHT/2); i++)
    {
        color = ++color % 16;
        DrawTriangle_StandardCharacterMode(ch, color,                            i,                           i/2, width_c-1-(i/2), i/2,                              i, (CHARACTER_SCREEN_HEIGHT-1)-i, characterModeAddresses.screenDataPtr);
        color = ++color % 16;
        DrawTriangle_StandardCharacterMode(ch, color, (CHARACTER_SCREEN_WIDTH-1)-i,                           i/2, width_c+1+(i/2), i/2,   (CHARACTER_SCREEN_WIDTH-1)-i, (CHARACTER_SCREEN_HEIGHT-1)-i, characterModeAddresses.screenDataPtr);
        color = ++color % 16;
        DrawTriangle_StandardCharacterMode(ch, color,                          i+1, (CHARACTER_SCREEN_HEIGHT-1)-i, width_c        ,   i, (CHARACTER_SCREEN_WIDTH-1)-1-i, (CHARACTER_SCREEN_HEIGHT-1)-i, characterModeAddresses.screenDataPtr);
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
        ch = 65 + i; //Use PETSCII screen code 65 ('A')
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
    #ifdef PAUSE
        WaitUntilKeyPressed(KEY_RETURN);
    #endif
#endif
    SetMode_StandardBitmapMode();
#ifdef DEBUG
    #ifdef PAUSE
        WaitUntilKeyPressed(KEY_RETURN);
    #endif
#endif
    GetVic2BitmapModeMemoryMappedAddresses(&bitmapModeAddresses);
#ifdef DEBUG
    #ifdef PAUSE
        WaitUntilKeyPressed(KEY_RETURN);
    #endif
#endif
#ifdef VBCC
    bitmapModeAddresses.vic2MemoryBankPtr = SetVic2VideoMemoryBank(1);
#else
    bitmapModeAddresses.vic2MemoryBankPtr = SetVic2VideoMemoryBank(2);
#endif
#ifdef DEBUG
    #ifdef PAUSE
        WaitUntilKeyPressed(KEY_RETURN);
    #endif
#endif
    SetVic2BitmapModeMemoryLocations(1, 1, &bitmapModeAddresses);

    // screen fill test - standard bitmap mode - foregroundColor
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);
    FillScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    // screen color test - standard bitmap mode - foregroundColor
    for (i=0; i<16; i++)
    {
        color = ++color % 16;
        SetScreenColor_StandardBitmapMode(color, 0, bitmapModeAddresses.colorDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif

    // screen fill test - standard bitmap mode - backgroundColor
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    // screen color test - standard bitmap mode - backgroundColor
    for (i=0; i<16; i++)
    {
        color = ++color % 16;
        SetScreenColor_StandardBitmapMode(0, color, bitmapModeAddresses.colorDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif

    // line drawing tests - standard bitmap mode
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);
    SetScreenColor_StandardBitmapMode(1, 0, bitmapModeAddresses.colorDataPtr);

    // line drawing tests - standard bitmap mode - originate from top-left
    for (j=0; j<STANDARD_BITMAP_SCREEN_WIDTH; j+=BITMAP_TEST_STEP)
    {
        DrawLine_StandardBitmapMode(0, 0, j, STANDARD_BITMAP_SCREEN_HEIGHT-1, bitmapModeAddresses.bitmapDataPtr);
    }
    for (sj=(STANDARD_BITMAP_SCREEN_HEIGHT-1); sj>=0; sj-=BITMAP_TEST_STEP)
    {
        DrawLine_StandardBitmapMode(0, 0, STANDARD_BITMAP_SCREEN_WIDTH-1, (unsigned short)sj, bitmapModeAddresses.bitmapDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    // line drawing tests - standard bitmap mode - originate from bottom-left
    for (j=0; j<STANDARD_BITMAP_SCREEN_WIDTH; j+=BITMAP_TEST_STEP)
    {
        DrawLine_StandardBitmapMode(0, (STANDARD_BITMAP_SCREEN_HEIGHT-1), j, 0, bitmapModeAddresses.bitmapDataPtr);
    }
    for (j=0; j<STANDARD_BITMAP_SCREEN_HEIGHT; j+=BITMAP_TEST_STEP)
    {
        DrawLine_StandardBitmapMode(0, (STANDARD_BITMAP_SCREEN_HEIGHT-1), (STANDARD_BITMAP_SCREEN_WIDTH-1), j, bitmapModeAddresses.bitmapDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    // line drawing tests - standard bitmap mode - originate from bottom-right
    for (sj=(STANDARD_BITMAP_SCREEN_WIDTH-1); sj>=0; sj-=BITMAP_TEST_STEP)
    {
        DrawLine_StandardBitmapMode((STANDARD_BITMAP_SCREEN_WIDTH-1), (STANDARD_BITMAP_SCREEN_HEIGHT-1), (unsigned short)sj, 0, bitmapModeAddresses.bitmapDataPtr);
    }
    for (j=0; j<STANDARD_BITMAP_SCREEN_HEIGHT; j+=BITMAP_TEST_STEP)
    {
        DrawLine_StandardBitmapMode((STANDARD_BITMAP_SCREEN_WIDTH-1), (STANDARD_BITMAP_SCREEN_HEIGHT-1), 0, j, bitmapModeAddresses.bitmapDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    // line drawing tests - standard bitmap mode - originate from top-right
    for (sj=(STANDARD_BITMAP_SCREEN_WIDTH-1); sj>=0; sj-=BITMAP_TEST_STEP)
    {
        DrawLine_StandardBitmapMode((STANDARD_BITMAP_SCREEN_WIDTH-1), 0, (unsigned short)sj, (STANDARD_BITMAP_SCREEN_HEIGHT-1), bitmapModeAddresses.bitmapDataPtr);
    }
    for (sj=(STANDARD_BITMAP_SCREEN_HEIGHT-1); sj>=0; sj-=BITMAP_TEST_STEP)
    {
        DrawLine_StandardBitmapMode((STANDARD_BITMAP_SCREEN_WIDTH-1), 0, 0, (unsigned short)sj, bitmapModeAddresses.bitmapDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    // rectangle drawing test - standard bitmap mode
    for (j=0; j<(STANDARD_BITMAP_SCREEN_HEIGHT/2); j+=BITMAP_TEST_STEP)
    {
        width_s = (STANDARD_BITMAP_SCREEN_WIDTH-1) - (j*2);
        height_s = (STANDARD_BITMAP_SCREEN_HEIGHT-1) - (j*2);
        DrawRectangle_StandardBitmapMode(j, j, width_s, height_s, bitmapModeAddresses.bitmapDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    // circle drawing test - standard bitmap mode
    for (j=1; j<(STANDARD_BITMAP_SCREEN_HEIGHT/2); j+=BITMAP_TEST_STEP)
    {
        DrawCircle_StandardBitmapMode(STANDARD_BITMAP_SCREEN_WIDTH/2, STANDARD_BITMAP_SCREEN_HEIGHT/2, j, bitmapModeAddresses.bitmapDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    width_s = STANDARD_BITMAP_SCREEN_WIDTH/2;
    // triangle drawing test - standard bitmap mode
    for (i=0; i<(STANDARD_BITMAP_SCREEN_HEIGHT/2); i+=BITMAP_TEST_STEP)
    {
        DrawTriangle_StandardBitmapMode(                                 i,                                 i/2, width_s-1-(i/2), i/2,                                    i, (STANDARD_BITMAP_SCREEN_HEIGHT-1)-i, bitmapModeAddresses.bitmapDataPtr);
        DrawTriangle_StandardBitmapMode((STANDARD_BITMAP_SCREEN_WIDTH-1)-i,                                 i/2, width_s+1+(i/2), i/2,   (STANDARD_BITMAP_SCREEN_WIDTH-1)-i, (STANDARD_BITMAP_SCREEN_HEIGHT-1)-i, bitmapModeAddresses.bitmapDataPtr);
        DrawTriangle_StandardBitmapMode(                               i+1, (STANDARD_BITMAP_SCREEN_HEIGHT-1)-i, width_s        ,   i, (STANDARD_BITMAP_SCREEN_WIDTH-1)-1-i, (STANDARD_BITMAP_SCREEN_HEIGHT-1)-i, bitmapModeAddresses.bitmapDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    // multicolor bitmap mode testing
#ifdef DEBUG
    printf("SetMode_MulticolorBitmapMode()");
    #ifdef PAUSE
        WaitUntilKeyPressed(KEY_RETURN);
    #endif
#endif
    SetMode_MulticolorBitmapMode(0);
#ifdef DEBUG
    #ifdef PAUSE
        WaitUntilKeyPressed(KEY_RETURN);
    #endif
#endif
    GetVic2BitmapModeMemoryMappedAddresses(&bitmapModeAddresses);
#ifdef DEBUG
    #ifdef PAUSE
        WaitUntilKeyPressed(KEY_RETURN);
    #endif
#endif
#ifdef VBCC
    bitmapModeAddresses.vic2MemoryBankPtr = SetVic2VideoMemoryBank(1);
#else
    bitmapModeAddresses.vic2MemoryBankPtr = SetVic2VideoMemoryBank(2);
#endif
#ifdef DEBUG
    #ifdef PAUSE
        WaitUntilKeyPressed(KEY_RETURN);
    #endif
#endif
    SetVic2BitmapModeMemoryLocations(1, 1, &bitmapModeAddresses);

    // screen fill test - multicolor bitmap mode - foregroundColor1
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);
    FillScreen_MulticolorBitmapMode(0x55, bitmapModeAddresses.bitmapDataPtr);

    // screen color test - multicolor bitmap mode - foregroundColor1
    for (i=0; i<16; i++)
    {
        color = ++color % 16;
        SetScreenColor_StandardBitmapMode(color, 0, bitmapModeAddresses.colorDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif

    // screen fill test - multicolor bitmap mode - foregroundColor2
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);
    FillScreen_MulticolorBitmapMode(0xAA, bitmapModeAddresses.bitmapDataPtr);

    // screen color test - multicolor bitmap mode - foregroundColor2
    for (i=0; i<16; i++)
    {
        color = ++color % 16;
        SetScreenColor_StandardBitmapMode(0, color, bitmapModeAddresses.colorDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif

    // screen fill test - multicolor bitmap mode - foregroundColor3
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);
    FillScreen_MulticolorBitmapMode(0xFF, bitmapModeAddresses.bitmapDataPtr);

    // screen color test - multicolor bitmap mode - foregroundColor3
    for (i=0; i<16; i++)
    {
        color = ++color % 16;
        SetScreenForegroundColor_StandardCharacterMode(color);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif

    // screen fill test - multicolor bitmap mode - backgroundColor
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    // screen color test - multicolor bitmap mode - backgroundColor
    for (i=0; i<16; i++)
    {
        color = ++color % 16;
        SetScreenBackgroundColor_StandardCharacterMode(color);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif

    // line drawing tests - multicolor bitmap mode
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);
    SetScreenColor_StandardBitmapMode(2, 4, bitmapModeAddresses.colorDataPtr);
    SetScreenForegroundColor_StandardCharacterMode(5);
    SetScreenBackgroundColor_StandardCharacterMode(6);

    // line drawing tests - multicolor bitmap mode - originate from top-left
    for (j=0; j<MULTICOLOR_BITMAP_SCREEN_WIDTH; j+=BITMAP_TEST_STEP)
    {
        i = (unsigned char)(j & 0x0003);
        DrawLine_MulticolorBitmapMode(i, 0, 0, j, MULTICOLOR_BITMAP_SCREEN_HEIGHT-1, bitmapModeAddresses.bitmapDataPtr);
    }
    for (sj=(MULTICOLOR_BITMAP_SCREEN_HEIGHT-1); sj>=0; sj-=BITMAP_TEST_STEP)
    {
        i = (unsigned char)(sj & 0x0003);
        DrawLine_MulticolorBitmapMode(i, 0, 0, MULTICOLOR_BITMAP_SCREEN_WIDTH-1, (unsigned short)sj, bitmapModeAddresses.bitmapDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    // line drawing tests - multicolor bitmap mode - originate from bottom-left
    for (j=0; j<MULTICOLOR_BITMAP_SCREEN_WIDTH; j+=BITMAP_TEST_STEP)
    {
        i = (unsigned char)(j & 0x0003);
        DrawLine_MulticolorBitmapMode(i, 0, (MULTICOLOR_BITMAP_SCREEN_HEIGHT-1), j, 0, bitmapModeAddresses.bitmapDataPtr);
    }
    for (j=0; j<MULTICOLOR_BITMAP_SCREEN_HEIGHT; j+=BITMAP_TEST_STEP)
    {
        i = (unsigned char)(j & 0x0003);
        DrawLine_MulticolorBitmapMode(i, 0, (MULTICOLOR_BITMAP_SCREEN_HEIGHT-1), (MULTICOLOR_BITMAP_SCREEN_WIDTH-1), j, bitmapModeAddresses.bitmapDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    // line drawing tests - multicolor bitmap mode - originate from bottom-right
    for (sj=(MULTICOLOR_BITMAP_SCREEN_WIDTH-1); sj>=0; sj-=BITMAP_TEST_STEP)
    {
        i = (unsigned char)(sj & 0x0003);
        DrawLine_MulticolorBitmapMode(i, (MULTICOLOR_BITMAP_SCREEN_WIDTH-1), (MULTICOLOR_BITMAP_SCREEN_HEIGHT-1), (unsigned short)sj, 0, bitmapModeAddresses.bitmapDataPtr);
    }
    for (j=0; j<MULTICOLOR_BITMAP_SCREEN_HEIGHT; j+=BITMAP_TEST_STEP)
    {
        i = (unsigned char)(j & 0x0003);
        DrawLine_MulticolorBitmapMode(i, (MULTICOLOR_BITMAP_SCREEN_WIDTH-1), (MULTICOLOR_BITMAP_SCREEN_HEIGHT-1), 0, j, bitmapModeAddresses.bitmapDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    // line drawing tests - multicolor bitmap mode - originate from top-right
    for (sj=(MULTICOLOR_BITMAP_SCREEN_WIDTH-1); sj>=0; sj-=BITMAP_TEST_STEP)
    {
        i = (unsigned char)(sj & 0x0003);
        DrawLine_MulticolorBitmapMode(i, (MULTICOLOR_BITMAP_SCREEN_WIDTH-1), 0, (unsigned short)sj, (MULTICOLOR_BITMAP_SCREEN_HEIGHT-1), bitmapModeAddresses.bitmapDataPtr);
    }
    for (sj=(MULTICOLOR_BITMAP_SCREEN_HEIGHT-1); sj>=0; sj-=BITMAP_TEST_STEP)
    {
        i = (unsigned char)(sj & 0x0003);
        DrawLine_MulticolorBitmapMode(i, (MULTICOLOR_BITMAP_SCREEN_WIDTH-1), 0, 0, (unsigned short)sj, bitmapModeAddresses.bitmapDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    // rectangle drawing test - multicolor bitmap mode
    for (j=0; j<(MULTICOLOR_BITMAP_SCREEN_WIDTH/2); j+=BITMAP_TEST_STEP)
    {
        i = (unsigned char)(j & 0x0003);
        width_s = (MULTICOLOR_BITMAP_SCREEN_WIDTH-1) - (j*2);
        height_s = (MULTICOLOR_BITMAP_SCREEN_HEIGHT-1) - (j*2);
        DrawRectangle_MulticolorBitmapMode(i, j, j, width_s, height_s, bitmapModeAddresses.bitmapDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    // circle drawing test - multicolor bitmap mode
    for (j=1; j<(MULTICOLOR_BITMAP_SCREEN_WIDTH/2); j+=BITMAP_TEST_STEP)
    {
        i = (unsigned char)(j & 0x0003);
        DrawCircle_MulticolorBitmapMode(i, MULTICOLOR_BITMAP_SCREEN_WIDTH/2, MULTICOLOR_BITMAP_SCREEN_HEIGHT/2, j, bitmapModeAddresses.bitmapDataPtr);
    }
#ifdef PAUSE
    WaitUntilKeyPressed(KEY_RETURN);
#endif
    ClearScreen_StandardBitmapMode(bitmapModeAddresses.bitmapDataPtr);

    width_s = MULTICOLOR_BITMAP_SCREEN_WIDTH/2;
    // triangle drawing test - multicolor bitmap mode
    for (i=0; i<(MULTICOLOR_BITMAP_SCREEN_WIDTH/2); i+=BITMAP_TEST_STEP)
    {
        DrawTriangle_MulticolorBitmapMode(i & 0x03,                                    i,                                   i/2, width_s-1-(i/2), i/2,                                      i, (MULTICOLOR_BITMAP_SCREEN_HEIGHT-1)-i, bitmapModeAddresses.bitmapDataPtr);
        DrawTriangle_MulticolorBitmapMode(i & 0x03, (MULTICOLOR_BITMAP_SCREEN_WIDTH-1)-i,                                   i/2, width_s+1+(i/2), i/2,   (MULTICOLOR_BITMAP_SCREEN_WIDTH-1)-i, (MULTICOLOR_BITMAP_SCREEN_HEIGHT-1)-i, bitmapModeAddresses.bitmapDataPtr);
        DrawTriangle_MulticolorBitmapMode(i & 0x03,                                  i+1, (MULTICOLOR_BITMAP_SCREEN_HEIGHT-1)-i, width_s        ,   i, (MULTICOLOR_BITMAP_SCREEN_WIDTH-1)-1-i, (MULTICOLOR_BITMAP_SCREEN_HEIGHT-1)-i, bitmapModeAddresses.bitmapDataPtr);
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
