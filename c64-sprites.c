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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <string.h>

#ifdef KICKC
 #include <c64.h>
#else
 #define SPRITES_XPOS     ((unsigned char*)0xd000)
 #define SPRITES_YPOS     ((unsigned char*)0xd001)
 #define SPRITES_XMSB     ((unsigned char*)0xd010)
 #define SPRITES_ENABLE   ((unsigned char*)0xd015)
 #define SPRITES_EXPAND_Y ((unsigned char*)0xd017)
 #define SPRITES_PRIORITY ((unsigned char*)0xd01B)
 #define SPRITES_MC       ((unsigned char*)0xd01C)
 #define SPRITES_EXPAND_X ((unsigned char*)0xd01D)
 #define SPRITES_COLOR    ((unsigned char*)0xd027)
#endif
#define SPRITES_MCOLOR1  ((unsigned char*)0xd025)
#define SPRITES_MCOLOR2  ((unsigned char*)0xd026)

#define SPRITE_BYTE_HEIGHT 21
#define SPRITE_BYTE_WIDTH  3

// Print troubleshooting information for each line
//#define DEBUG
#ifdef DEBUG
    #include <printf.h>
#endif

void ColorSprite(
    // [in] sprite index is a number between 0 - 7
    unsigned char index,
    // [in] sprite color is a number between 0 - 15 (use the color constants in kickc/include/c64.h to improve readability)
    unsigned char color)
{
    SPRITES_COLOR[index] = color;
}

void MoveSprite(
    // [in] sprite index is a number between 0 - 7
    unsigned char index,
    // [in] sprite position horizontal is a number between 0 - 160 or 0 - 320 depending on the screen mode
    unsigned short x,
    // [in] sprite position vertical is a number between 0 - 200
    unsigned short y)
{
    unsigned char x_msb;
    unsigned char x_bit;
    unsigned char x_byte;
    unsigned char y_byte;

    x_msb = *SPRITES_XMSB;
    x_bit = (unsigned char)0x01 << index;

    if (x >= 255)
    {
        x_msb = x_msb | x_bit;
        x = x - 255;
    }
    else if (x_msb & x_bit)
    {
        x_msb = x_msb & (~x_bit);
    }

    x_byte = (unsigned char)x;
    SPRITES_XPOS[index*2] = x_byte;
    y_byte = (unsigned char)y;
    SPRITES_YPOS[index*2] = y_byte;

    *SPRITES_XMSB = x_msb;

#ifdef DEBUG
    print_uchar_pos(x_byte, 1, 0);
    print_uchar_pos(x_msb, 2, 0);
    print_uchar_pos((unsigned char)(x >> 8), 3, 0);
    print_uchar_pos((unsigned char)x, 3, 2);
    print_uchar_pos((unsigned char)y, 3, 5);
#endif //DEBUG
}

void SpriteFrameUpdate(
    // [in] sprite index is a number between 0 - 7
    unsigned char index,
    // [in] pointer to sprite data
    unsigned char *dataPtr,
    // [in] VIC-II sprite data pointers to support the 8 VIC-II sprites
    unsigned char *spriteDataPtrs)
{
    unsigned char spr_id;

    spr_id = (unsigned char)((short)dataPtr/64);
    spriteDataPtrs[index] = spr_id;
}

void MakeSingleColorSprite(
    // [in] sprite index is a number between 0 - 7
    unsigned char index,
    // [in] pointer to sprite data encoded as a high resolution sprite (24x21 pixels) in a 3x21 byte array
    unsigned char *dataPtr,
    // [in] sprite color is a number between 0 - 15 (use the color constants in kickc/include/c64.h to improve readability)
    unsigned char color,
    // [in] sprite position horizontal is a number between 0 - 160 or 0 - 320 depending on the screen mode
    unsigned short x,
    // [in] sprite position vertical is a number between 0 - 200
    unsigned short y,
    // [in] if non-zero (true) then double sprite width (each pixel is expanded to two pixels)
    unsigned char expandHorizontal,
    // [in] if non-zero (true) then double sprite height (each pixel is expanded to two pixels)
    unsigned char expandVertical,
    // [in] VIC-II sprite data pointers to support the 8 VIC-II sprites
    unsigned char *spriteDataPtrs)
{
    unsigned char indexBit;

    indexBit = (unsigned char)0x01 << index;
    *SPRITES_ENABLE = *SPRITES_ENABLE | indexBit;

    if (expandHorizontal)
    {
        *SPRITES_EXPAND_X = *SPRITES_EXPAND_X | indexBit;
    }
    else
    {
        *SPRITES_EXPAND_X = *SPRITES_EXPAND_X & (~indexBit);
    }

    if (expandVertical)
    {
        *SPRITES_EXPAND_Y = *SPRITES_EXPAND_Y | indexBit;
    }
    else
    {
        *SPRITES_EXPAND_Y = *SPRITES_EXPAND_Y & (~indexBit);
    }

    *SPRITES_MC = *SPRITES_MC & (~indexBit);

    SpriteFrameUpdate(index, dataPtr, spriteDataPtrs);
    ColorSprite(index, color);
    MoveSprite(index, x, y);
}

void MakeMultiColorSprite(
    // [in] sprite index is a number between 0 - 7
    unsigned char index,
    // [in] pointer to sprite data encoded as a multicolor sprite (12x21 pixels) in a 3x21 byte array
    unsigned char *dataPtr,
    // [in] sprite color is a number between 0 - 15 (use the color constants in kickc/include/c64.h to improve readability)
    unsigned char spriteColor,
    // [in] sprite common color #1 is a number between 0 - 15 (use the color constants in kickc/include/c64.h to improve readability)
    unsigned char commonColor1,
    // [in] sprite common color #2 is a number between 0 - 15 (use the color constants in kickc/include/c64.h to improve readability)
    unsigned char commonColor2,
    // [in] sprite position horizontal is a number between 0 - 160 or 0 - 320 depending on the screen mode
    unsigned short x,
    // [in] sprite position vertical is a number between 0 - 200
    unsigned short y,
    // [in] if non-zero (true) then double sprite width (each pixel is expanded to two pixels)
    unsigned char expandHorizontal,
    // [in] if non-zero (true) then double sprite height (each pixel is expanded to two pixels)
    unsigned char expandVertical,
    // [in] VIC-II sprite data pointers to support the 8 VIC-II sprites
    unsigned char *spriteDataPtrs)
{
    unsigned char indexBit;

    indexBit = (unsigned char)0x01 << index;
    *SPRITES_ENABLE = *SPRITES_ENABLE | indexBit;

    if (expandHorizontal)
    {
        *SPRITES_EXPAND_X = *SPRITES_EXPAND_X | indexBit;
    }
    else
    {
        *SPRITES_EXPAND_X = *SPRITES_EXPAND_X & (~indexBit);
    }

    if (expandVertical)
    {
        *SPRITES_EXPAND_Y = *SPRITES_EXPAND_Y | indexBit;
    }
    else
    {
        *SPRITES_EXPAND_Y = *SPRITES_EXPAND_Y & (~indexBit);
    }

    *SPRITES_MC = *SPRITES_MC | indexBit;

    SpriteFrameUpdate(index, dataPtr, spriteDataPtrs);
    ColorSprite(index, spriteColor);
    *SPRITES_MCOLOR1 = commonColor1;
    *SPRITES_MCOLOR2 = commonColor2;
    MoveSprite(index, x, y);
}

void RemoveSprite(
    // [in] sprite index is a number between 0 - 7
    unsigned char index)
{
    unsigned char indexBit;

    indexBit = (unsigned char)0x01 << index;
    *SPRITES_ENABLE = *SPRITES_ENABLE & (~indexBit);
}

void SpritePriority(
    // [in] sprite index is a number between 0 - 7
    unsigned char index,
    // [in] if non-zero (true) then put sprite behind background
    unsigned char behindBackground)
{
    unsigned char indexBit;

    indexBit = (unsigned char)0x01 << index;
    if (behindBackground)
    {
        *SPRITES_PRIORITY = *SPRITES_PRIORITY | indexBit;
    }
    else //in front of background
    {
        *SPRITES_PRIORITY = *SPRITES_PRIORITY & (~indexBit);
    }
}

/* Sprites consist of 21 rows of 3 bytes each. To reverse the sprite vertically we will need to swap the top <-> bottom lines in turn. */
void ReverseVerticalSprite(
    // [in] pointer to sprite data in a 21x3 byte array
    unsigned char *spritePtr)
{
    unsigned char i;
    unsigned char tmpTopLine[SPRITE_BYTE_WIDTH];
    unsigned char topLineIndex;
    unsigned char bottomLineIndex;

    for (i=0; i<SPRITE_BYTE_HEIGHT/2; i++)
    {
        topLineIndex = i*SPRITE_BYTE_WIDTH;
        bottomLineIndex = (SPRITE_BYTE_HEIGHT*SPRITE_BYTE_WIDTH)-((i+1)*SPRITE_BYTE_WIDTH);
        memcpy(tmpTopLine, &spritePtr[topLineIndex], SPRITE_BYTE_WIDTH);
        memcpy(&spritePtr[topLineIndex], &spritePtr[bottomLineIndex], SPRITE_BYTE_WIDTH);
        memcpy(&spritePtr[bottomLineIndex], tmpTopLine, SPRITE_BYTE_WIDTH);
    }
}

/* Each multicolor sprite pixel consists of a 2 bit encoding of either 00, 01, or 11. 
 * These two bit pairs must be preserved as they are moved */
unsigned char ReverseByteByTwoBitPairs(
    unsigned char input)
{
    unsigned char output;

    output = 0;
    output |= ((input & 0xC0) >> 6);
    output |= ((input & 0x30) >> 2);
    output |= ((input & 0x0C) << 2);
    output |= ((input & 0x03) << 6);
    return output;
}

/* Multi-color sprites consist of 21 rows of 3 bytes each. To reverse them horizontally 
 * we will need to swap the bytes in each row left <-> right and then reverse the order of two-bit pairs in each byte. */
void ReverseHorizontalMulticolorSprite(
    // [in] pointer to sprite data encoded as a multicolor sprite (12x21 pixels) in a 21x3 byte array
    unsigned char *spritePtr)
{
    unsigned char i;

    for (i=0; i<SPRITE_BYTE_HEIGHT; i++)
    {
        unsigned char tmpLeft   = spritePtr[i*SPRITE_BYTE_WIDTH];
        unsigned char tmpMiddle = spritePtr[i*SPRITE_BYTE_WIDTH+1];
        unsigned char tmpRight  = spritePtr[i*SPRITE_BYTE_WIDTH+2];

        spritePtr[i*SPRITE_BYTE_WIDTH]   = ReverseByteByTwoBitPairs(tmpRight);
        spritePtr[i*SPRITE_BYTE_WIDTH+1] = ReverseByteByTwoBitPairs(tmpMiddle);
        spritePtr[i*SPRITE_BYTE_WIDTH+2] = ReverseByteByTwoBitPairs(tmpLeft);
    }
}

unsigned char ReverseByteBitwise(
    unsigned char input)
{
    unsigned char output;

    output = 0;
    output |= ((input & 0x80) >> 7);
    output |= ((input & 0x40) >> 5);
    output |= ((input & 0x20) >> 3);
    output |= ((input & 0x10) >> 1);
    output |= ((input & 0x08) << 1);
    output |= ((input & 0x04) << 3);
    output |= ((input & 0x02) << 5);
    output |= ((input & 0x01) << 7);
    return output;
}

/* Single-color sprites consist of 21 rows of 3 bytes each. To reverse them horizontally 
 * we will need to swap each row left <-> right and then reverse each byte. */
void ReverseHorizontalSinglecolorSprite(
    // [in] pointer to sprite data encoded as a high resolution sprite (24x21 pixels) in a 21x3 byte array
    unsigned char *spritePtr)
{
    unsigned char i;

    for (i=0; i<SPRITE_BYTE_HEIGHT; i++)
    {
        unsigned char tmpLeft   = spritePtr[i*SPRITE_BYTE_WIDTH];
        unsigned char tmpMiddle = spritePtr[i*SPRITE_BYTE_WIDTH+1];
        unsigned char tmpRight  = spritePtr[i*SPRITE_BYTE_WIDTH+2];

        spritePtr[i*SPRITE_BYTE_WIDTH]   = ReverseByteBitwise(tmpRight);
        spritePtr[i*SPRITE_BYTE_WIDTH+1] = ReverseByteBitwise(tmpMiddle);
        spritePtr[i*SPRITE_BYTE_WIDTH+2] = ReverseByteBitwise(tmpLeft);
    }
}

