/** Copyright 2024-2026 Warren Wilbur - MIT License
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

// Assumptions: If using KICKC you must call keyboard_init() (from c64-keyboard.h) before using this function
bool CheckIfKeyPressed(
    //! [in] keyboard scan code to check if pressed
    char desiredKey);

// Assumptions: If using KICKC you must call keyboard_init() (from c64-keyboard.h) before using this function
// return the scancode of the next key that is pressed
char GetNextKeyPressed(void);

// Assumptions: If using KICKC you must call keyboard_init() (from c64-keyboard.h) before using this function
void WaitUntilKeyPressed(
    //! [in] PETSCII code for the key to wait until pressed (use codes in c64-keyboard.h)
    char desiredKey);

// Use this to prevent screen tearing
void WaitUntilRasterOffscreen(void);

#endif
