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

#include "c64-charset.h"


/** C64 Character Set Routines */

// Returns byte containing the text character set data base address index within VIC-II address space. Can be any even number between 0 and 14.
// For each index (0-14) the base address will be 1024*index, such that the base address for each index is:
//                0 = 0
//                2 = 2048
//                4 = 4096 (default, upper case character set)
//                6 = 6144 (upperlower case character set)
//                8 = 8192
//               10 = 10240
//               12 = 12288
//               14 = 14336
unsigned char GetCharacterSetAddressIndex()
{
    unsigned char characterSetAddressIndex = (*(unsigned char*)0xD018) & 0x0E;
    return characterSetAddressIndex;
}

void SetCharacterSetAddressIndex(
    // [in] byte containing the text character set data base address index within VIC-II address space. Can be set to any even number between 0 and 14.
    // For each index (0-14) the base address will be 1024*index, such that the base address for each index is:
    //                0 = 0
    //                2 = 2048
    //                4 = 4096 (default, upper case character set)
    //                6 = 6144 (upperlower case character set)
    //                8 = 8192
    //               10 = 10240
    //               12 = 12288
    //               14 = 14336
    unsigned char characterSetAddressIndex)
{
    // VIC-II Chip Memory Control Register. The VIC-II Memory Control Register divides a single byte as follows:
    //      bits 0-3: Text character set data base address index within VIC-II address space. Can be set to any even number between 0 and 14.
    //                For each index (0-14) the base address will be 1024*index, such that the base address for each index is:
    //                0 = 0
    //                2 = 2048
    //                4 = 4096 (default, upper case character set)
    //                6 = 6144 (upperlower case character set)
    //                8 = 8192
    //               10 = 10240
    //               12 = 12288
    //               14 = 14336
    //      bits 4-7: Video matrix base address within VIC-II address space
    unsigned char memoryControlRegister = (*(unsigned char*)0xD018);
    *(unsigned char*)0xD018 = (memoryControlRegister & 0xF0) | characterSetAddressIndex;
}

