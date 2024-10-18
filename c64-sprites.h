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

#ifndef C64_SPRITES_H
#define C64_SPRITES_H

void ColorSprite(
    // [in] sprite index is a number between 0 - 7
    unsigned char index,
    // [in] sprite color is a number between 0 - 15 (use the color constants in kickc/include/c64.h to improve readability)
    unsigned char color);

void MoveSprite(
    // [in] sprite index is a number between 0 - 7
    unsigned char index,
    // [in] sprite position horizontal is a number between 0 - 160 or 0 - 320 depending on the screen mode
    unsigned short x,
    // [in] sprite position vertical is a number between 0 - 200
    unsigned short y);

void SpriteFrameUpdate(
    // [in] sprite index is a number between 0 - 7
    unsigned char index,
    // [in] pointer to sprite data
    unsigned char *dataPtr,
    // [in] VIC-II sprite data pointers to support the 8 VIC-II sprites
    unsigned char *spriteDataPtrs);

void MakeSingleColorSprite(
    // [in] sprite index is a number between 0 - 7
    unsigned char index,
    // [in] pointer to sprite data encoded as a high resolution sprite (24x21 pixels) in a 3x21 byte array
    unsigned char* dataPtr,
    // [in] sprite color is a number between 0 - 15 (use the color constants in kickc/include/c64.h to improve readability)
    unsigned char color,
    // [in] sprite position horizontal is a number between 0 - 160 or 0 - 320 depending on the screen mode
    unsigned short x,
    // [in] sprite position vertical is a number between 0 - 200
    unsigned short y,
    // [in] non-zero (true) to double sprite width (each pixel is expanded to two pixels)
    unsigned char expandHorizontal,
    // [in] non-zero (true) to double sprite height (each pixel is expanded to two pixels)
    unsigned char expandVertical,
    // [in] VIC-II sprite data pointers to support the 8 VIC-II sprites
    unsigned char *spriteDataPtrs);

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
    // [in] non-zero (true) to double sprite width (each pixel is expanded to two pixels)
    unsigned char expandHorizontal,
    // [in] non-zero (true) to double sprite height (each pixel is expanded to two pixels)
    unsigned char expandVertical,
    // [in] VIC-II sprite data pointers to support the 8 VIC-II sprites
    unsigned char *spriteDataPtrs);

void RemoveSprite(
    // [in] sprite index is a number between 0 - 7
    unsigned char index);

void SpritePriority(
    // [in] sprite index is a number between 0 - 7
    unsigned char index,
    // [in] if non-zero (true) then put sprite behind background
    unsigned char behindBackground);

void ReverseVerticalSprite(
    // [in] pointer to sprite data in a 21x3 byte array
    unsigned char *spritePtr);

void ReverseHorizontalMulticolorSprite(
    // [in] pointer to sprite data encoded as a multicolor sprite (12x21 pixels) in a 21x3 byte array
    unsigned char *spritePtr);

void ReverseHorizontalSinglecolorSprite(
    // [in] pointer to sprite data encoded as a high resolution sprite (24x21 pixels) in a 21x3 byte array
    unsigned char *spritePtr);

#endif //C64_SPRITES_H

