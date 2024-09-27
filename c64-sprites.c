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

#ifdef KICKC
 #include <c64.h>
#endif

void MakeSingleColorSprite(
    // [in] sprite index is a number between 0 - 7
    byte index,
    // [in] pointer to sprite data encoded as a high resolution sprite (24x21 pixels) in a 3x21 byte array
    byte* dataPtr,
    // [in] sprite color is a number between 0 - 15 (use the color constants in kickc/include/c64.h to improve readability)
    byte color,
    // [in] sprite position horizontal is a number between 0 - 160 or 0 - 320 depending on the screen mode
    unsigned word x,
    // [in] sprite position vertical is a number between 0 - 200
    unsigned word y,
    // [in] double sprite width (each pixel is expanded to two pixels)
    bool expandHorizontal,
    // [in] double sprite height (each pixel is expanded to two pixels)
    bool expandVertical,
    // [in] VIC-II sprite data pointers to support the 8 VIC-II sprites
    unsigned char *spriteDataPtrs)
{
    unsigned byte indexBit = (unsigned byte)%00000001 << index;
    VICII->SPRITES_ENABLE = VICII->SPRITES_ENABLE | indexBit;

    if (expandHorizontal)
    {
        VICII->SPRITES_EXPAND_X = VICII->SPRITES_EXPAND_X | indexBit;
    }
    else
    {
        VICII->SPRITES_EXPAND_X = VICII->SPRITES_EXPAND_X & (~indexBit);
    }

    if (expandVertical)
    {
        VICII->SPRITES_EXPAND_Y = VICII->SPRITES_EXPAND_Y | indexBit;
    }
    else
    {
        VICII->SPRITES_EXPAND_Y = VICII->SPRITES_EXPAND_Y & (~indexBit);
    }

    VICII->SPRITES_MC = VICII->SPRITES_MC & (~indexBit);

    SpriteFrameUpdate(index, dataPtr, spriteDataPtrs);
    ColorSprite(index, color);
    MoveSprite(index, x, y);
}

void SpriteFrameUpdate(
    // [in] sprite index is a number between 0 - 7
    byte index,
    // [in] pointer to sprite data
    byte *dataPtr,
    // [in] VIC-II sprite data pointers to support the 8 VIC-II sprites
    unsigned char *spriteDataPtrs)
{
    byte spr_id = (byte)((word)dataPtr/64);
    spriteDataPtrs[index] = spr_id;
}

void MakeMultiColorSprite(
    // [in] sprite index is a number between 0 - 7
    byte index,
    // [in] pointer to sprite data encoded as a multicolor sprite (12x21 pixels) in a 3x21 byte array
    byte* dataPtr,
    // [in] sprite color is a number between 0 - 15 (use the color constants in kickc/include/c64.h to improve readability)
    byte spriteColor,
    // [in] sprite common color #1 is a number between 0 - 15 (use the color constants in kickc/include/c64.h to improve readability)
    byte commonColor1,
    // [in] sprite common color #2 is a number between 0 - 15 (use the color constants in kickc/include/c64.h to improve readability)
    byte commonColor2,
    // [in] sprite position horizontal is a number between 0 - 160 or 0 - 320 depending on the screen mode
    unsigned word x,
    // [in] sprite position vertical is a number between 0 - 200
    unsigned word y,
    // [in] double sprite width (each pixel is expanded to two pixels)
    bool expandHorizontal,
    // [in] double sprite height (each pixel is expanded to two pixels)
    bool expandVertical,
    // [in] VIC-II sprite data pointers to support the 8 VIC-II sprites
    unsigned char *spriteDataPtrs)
{
    unsigned byte indexBit = (unsigned byte)%00000001 << index;
    VICII->SPRITES_ENABLE = VICII->SPRITES_ENABLE | indexBit;

    if (expandHorizontal)
    {
        VICII->SPRITES_EXPAND_X = VICII->SPRITES_EXPAND_X | indexBit;
    }
    else
    {
        VICII->SPRITES_EXPAND_X = VICII->SPRITES_EXPAND_X & (~indexBit);
    }

    if (expandVertical)
    {
        VICII->SPRITES_EXPAND_Y = VICII->SPRITES_EXPAND_Y | indexBit;
    }
    else
    {
        VICII->SPRITES_EXPAND_Y = VICII->SPRITES_EXPAND_Y & (~indexBit);
    }

    VICII->SPRITES_MC = VICII->SPRITES_MC | indexBit;

    SpriteFrameUpdate(index, dataPtr, spriteDataPtrs);
    ColorSprite(index, spriteColor);
    VICII->SPRITES_MCOLOR1 = commonColor1;
    VICII->SPRITES_MCOLOR2 = commonColor2;
    MoveSprite(index, x, y);
}

void RemoveSprite(
    // [in] sprite index is a number between 0 - 7
    byte index)
{
    unsigned byte indexBit = (unsigned byte)%00000001 << index;
    VICII->SPRITES_ENABLE = VICII->SPRITES_ENABLE & (~indexBit);
}

void SpritePriority(
    // [in] sprite index is a number between 0 - 7
    byte index,
    // [in] if true then put sprite behind background
    bool behindBackground)
{
    unsigned byte indexBit = (unsigned byte)%00000001 << index;
    if (behindBackground)
    {
        VICII->SPRITES_PRIORITY = VICII->SPRITES_PRIORITY | indexBit;
    }
    else //in front of background
    {
        VICII->SPRITES_PRIORITY = VICII->SPRITES_PRIORITY & (~indexBit);
    }
}

void MoveSprite(
    // [in] sprite index is a number between 0 - 7
    byte index,
    // [in] sprite position horizontal is a number between 0 - 160 or 0 - 320 depending on the screen mode
    unsigned word x,
    // [in] sprite position vertical is a number between 0 - 200
    unsigned word y)
{
    unsigned byte x_msb = VICII->SPRITES_XMSB;
    unsigned byte x_bit = (unsigned byte)%00000001 << index;

    if (x >= 255)
    {
        x_msb = x_msb | x_bit;
        x = x - 255;
    }
    else if (x_msb & x_bit)
    {
        x_msb = x_msb & (~x_bit);
    }

    unsigned byte x_byte = (unsigned byte)x;
    SPRITES_XPOS[index*2] = x_byte;
    unsigned byte y_byte = (unsigned byte)y;
    SPRITES_YPOS[index*2] = y_byte;

    VICII->SPRITES_XMSB = x_msb;

#ifdef DEBUG
    print_uchar_pos(x_byte, 1, 0);
    print_uchar_pos(x_msb, 2, 0);
    print_uchar_pos((unsigned byte)(x >> 8), 3, 0);
    print_uchar_pos((unsigned byte)x, 3, 2);
    print_uchar_pos((unsigned byte)y, 3, 5);
#endif //DEBUG
}

void ColorSprite(
    // [in] sprite index is a number between 0 - 7
    byte index,
    // [in] sprite color is a number between 0 - 15 (use the color constants in kickc/include/c64.h to improve readability)
    byte color)
{
    SPRITES_COLOR[index] = color;
}

/* Multi-color sprites consist of 21 rows of 3 bytes each. To reverse them horizontally 
 * we will need to swap the bytes in each row left <-> right and then reverse the order of two-bit pairs in each byte. */
void ReverseMulticolorSprite(
    // [in] pointer to sprite data encoded as a multicolor sprite (12x21 pixels) in a 3x21 byte array
    byte *spritePtr)
{
    for (byte i=0; i<21; i++)
    {
        byte tmpLeft = spritePtr[i*3];
        byte tmpMiddle = spritePtr[i*3+1];
        byte tmpRight = spritePtr[i*3+2];

        spritePtr[i*3] = ReverseByteByTwoBitPairs(tmpRight);
        spritePtr[i*3+1] = ReverseByteByTwoBitPairs(tmpMiddle);
        spritePtr[i*3+2] = ReverseByteByTwoBitPairs(tmpLeft);
    }
}

/* Each multicolor sprite pixel consists of a 2 bit encoding of either 00, 01, or 11. 
 * These two bit pairs must be preserved as they are moved */
byte ReverseByteByTwoBitPairs(
    byte input)
{
    byte output = 0;
    output |= ((input & 0xC0) >> 6);
    output |= ((input & 0x30) >> 2);
    output |= ((input & 0x0C) << 2);
    output |= ((input & 0x03) << 6);
    return output;
}

/* Single-color sprites consist of 21 rows of 3 bytes each. To reverse them horizontally 
 * we will need to swap each row left <-> right and then reverse each byte. */
void ReverseSinglecolorSprite(
    // [in] pointer to sprite data encoded as a high resolution sprite (24x21 pixels) in a 3x21 byte array
    byte *spritePtr)
{
    for (byte i=0; i<21; i++)
    {
        byte tmpLeft = spritePtr[i*3];
        byte tmpMiddle = spritePtr[i*3+1];
        byte tmpRight = spritePtr[i*3+2];

        spritePtr[i*3] = ReverseByteBitwise(tmpRight);
        spritePtr[i*3+1] = ReverseByteBitwise(tmpMiddle);
        spritePtr[i*3+2] = ReverseByteBitwise(tmpLeft);
    }
}

byte ReverseByteBitwise(
    byte input)
{
    byte output = 0;
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

