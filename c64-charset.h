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

#ifndef C64_CHARSET_H
#define C64_CHARSET_H


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
unsigned char GetCharacterSetAddressIndex();

//UPPERCASE is the default
#define C64_UPPERCASE_CHARACTER_SET_ADDRESS_INDEX      4
#define C64_UPPERLOWERCASE_CHARACTER_SET_ADDRESS_INDEX 6

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
    unsigned char characterSet_AddressIndex);

#endif //C64_CHARSET_H
