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

#ifdef KICKC
    #include <c64.h>
    #include <c64-keyboard.h>
#else //SDCC, CC65, VBCC, OSCAR64
    #define RASTER         ((unsigned char*)0xD012)
    #ifdef OSCAR64
        #include <conio.h>
    #else //SDCC, CC65, VBCC
        #include <stdio.h>
//        #ifndef VBCC
//            #include <conio.h>
//        #endif
    #endif
#endif

#ifdef KICKC
    // Assumptions: you have already called keyboard_init() (from c64-keyboard.h) before using this function
    bool CheckIfKeyPressed(
            //! [in] keyboard scan code to check if pressed
            char desiredKey_scanCode)
    {
        char ch = 0xFF;

        keyboard_event_scan();
        ch = keyboard_event_get();
        return (ch != desiredKey_scanCode);
    }

    // Assumptions: you have already called keyboard_init() (from c64-keyboard.h) before using this function
    // return the scancode of the next key that is pressed
    char GetNextKeyPressed(void)
    {
        char ch = 0xFF;
        do
        {
            keyboard_event_scan();
            ch = keyboard_event_get();
        } while (ch == 0xFF);

        return ch;
    }

    // Assumptions: you have already called keyboard_init() (from c64-keyboard.h) before using this function
    void WaitUntilKeyPressed(
        //! [in] PETSCII code for the key to wait until pressed (use codes in c64-keyboard.h)
        char desiredKey_scanCode)
    {
        char ch = 0xFF;

        do
        {
            keyboard_event_scan();
            ch = keyboard_event_get();
        } while (ch != desiredKey_scanCode);
    }
#else //SDCC, CC65, VBCC, OSCAR64
    #ifdef OSCAR64
        bool CheckIfKeyPressed(
                //! [in] keyboard scan code to wait until pressed
                char desiredKey_scanCode)
        {
            char ch;

            ch = getchx();
            return (ch != desiredKey_scanCode);
        }

        // return the scancode of the next key that is pressed
        char GetNextKeyPressed(void)
        {
            char ch = 0x00;

            do
            {
                ch = getchx();
            } while (ch == 0x00);

            return ch;
        }

        void WaitUntilKeyPressed(
                //! [in] keyboard scan code to wait until pressed
                char desiredKey_scanCode)
        {
            char ch = 0xFF;

            do
            {
                ch = getchx();
            } while(ch != desiredKey_scanCode);
        }
    #else //SDCC, CC65, VBCC
        void WaitUntilKeyPressed(
                //! [in] keyboard scan code to wait until pressed
                char desiredKey_scanCode)
        {
            char ch = 0xFF;

            do
            {
                ch = getchar();
            } while (ch != desiredKey_scanCode);
        }
    #endif
#endif

// Use this to prevent screen tearing
void WaitUntilRasterOffscreen(void)
{
    volatile unsigned char const *rasterLinePtr = RASTER;
    while (*rasterLinePtr != 200) // Wait until raster line 200 on screen is drawn (vertical resolution is 0-199). Lines 200-255 are off the screen.
    {
        ; // Nothing
    }
}

