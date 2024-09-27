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
    #include <c64-keyboard.h>
#else //SDCC, CC65, VBCC, OSCAR64
    #define RASTER         ((unsigned char*)0xD012)
    #include <stdio.h>
//    #ifndef VBCC
//        #include <conio.h>
//    #endif
#endif

#ifdef KICKC
    bool CheckIfKeyPressed(
            //! [in] keyboard scan code to wait until pressed
            char desiredKey)
    {
        char ch = 0xFF;

        keyboard_event_scan();
        ch = keyboard_event_get();
        return (ch != desiredKey);
    }

    // Assumptions: you have already called keyboard_init() (from c64-keyboard.h) before using this function
    void WaitUntilKeyPressed(
        //! [in] PETSCII code for the key to wait until pressed (use codes in c64-keyboard.h)
        char desiredKey)
    {
        char ch = 0xFF;

        do
        {
            keyboard_event_scan();
            ch = keyboard_event_get();
        } while(ch != desiredKey);
    }
#else //SDCC, CC65, VBCC, OSCAR64
    void WaitUntilKeyPressed(
            //! [in] keyboard scan code to wait until pressed
            char desiredKey)
    {
        char ch = 0xFF;

        do
        {
            ch = getchar();
        } while(ch != desiredKey);
    }
#endif

// Use this to prevent screen tearing
void WaitUntilRasterOffscreen(void)
{
#ifndef OSCAR64
    while (*RASTER != 200) // Wait until raster line 200 on screen is drawn (vertical resolution is 0-199). Lines 200-255 are off the screen.
    {
        ; // Nothing
    }
#endif
}

