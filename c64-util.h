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

#ifndef C64_UTIL_H
#define C64_UTIL_H

#ifdef KICKC
    bool CheckIfKeyPressed(
        //! [in] keyboard scan code to wait until pressed
        char desiredKey);

    // Assumptions: you have already called keyboard_init() (from c64-keyboard.h) before using this function
    void WaitUntilKeyPressed(
        //! [in] PETSCII code for the key to wait until pressed (use codes in c64-keyboard.h)
        char desiredKey);
#else //SDCC, CC65, VBCC, OSCAR64
    void WaitUntilKeyPressed(
        //! [in] keyboard scan code to wait until pressed
        char desiredKey);
#endif

// Use this to prevent screen tearing
void WaitUntilRasterOffscreen(void);

#endif
