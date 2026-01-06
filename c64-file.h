/** Copyright 2025-2026 Warren Wilbur - MIT License
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

#ifndef C64_FILE_H
#define C64_FILE_H

#ifdef KICKC
    #define LOAD_OPERATION_FLAG 0
    #define VERIFY_OPERATION_FLAG 1
#endif //KICKC

#ifdef OSCAR64
    #include <c64/kernalio.h>
#endif //OSCAR64

// Load a file to memory
// Returns a status:
// - 0xff: Success
// - other: Kernal Error Code (https://commodore.ca/manuals/pdfs/commodore_error_messages.pdf)
char LoadFileToMemory(
        // [in] input device number, e.g. 8 for disk drive 1, 9 for disk drive 2, 1 for tape drive
        char storageDevice,
        // [in] file name to load, e.g. "sprite1.dat" (filename is ignored for tape device). Reminder: you may need to use uppercase for filenames. Filename may not exceed 16 characters.
        char* inputFilename,
        // [in] memory address to load the file data at (caller must reserve enough memory at this address to hold the file or risk buffer overflow)
        char* toAddress,
        // [in] length in bytes of the region to load (beginning from toAddress)
        unsigned short length,
        // [in] file number is the directory entry number to load, e.g. 0-119 for 1541 disk (The 1541 disk uses track 18 as the directory track. This track has 16 sectors - 1 sector for block allocation map = 15 sectors * 8 file entries / sector = 120 file entries).
        // Refer to: Inside Commodore DOS, section 4.1 'Information Management', pg#35.
        //
        // 0-295 (in theory, signed char argument can't hold that many) for 1581 disk (1581 directory track has 40 sectors - 3 sectors for block allocation map = 37 sectors * 8 file entries / sector = 296 file entries).
        // [in] input device number, e.g. 8 for disk drive, 1 for tape drive
        char directoryEntryNumber);

// Save memory region to file
// Returns a status:
// - 0xff: Success
// - other: Kernal Error Code (https://commodore.ca/manuals/pdfs/commodore_error_messages.pdf)
char SaveMemoryToFile(
        // [in] memory address of the start of the region to save to file
        char* startAddress,
        // [in] output device number, e.g. 8 for disk drive 1, 9 for disk drive 2, 1 for tape drive
        char storageDevice,
        // [in] file name to save, e.g. "sprite1.dat" (filename is ignored for tape device). Reminder: you may need to use uppercase for filenames. Filename may not exceed 16 characters.
        char* outputFilename,
        // [in] length in bytes of the region to save (beginning from startAddress)
        unsigned short length,
        // [in] file number is the directory entry number to load, e.g. 0-119 for 1541 disk (The 1541 disk uses track 18 as the directory track. This track has 16 sectors - 1 sector for block allocation map = 15 sectors * 8 file entries / sector = 120 file entries).
        // Refer to: Inside Commodore DOS, section 4.1 'Information Management', pg#35.
        //
        // 0-295 (in theory, signed char argument can't hold that many) for 1581 disk (1581 directory track has 40 sectors - 3 sectors for block allocation map = 37 sectors * 8 file entries / sector = 296 file entries).
        // [in] input device number, e.g. 8 for disk drive, 1 for tape drive
        char* directoryEntryNumber);

#endif //C64_FILE_H

