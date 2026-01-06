/** Copyright 2024-2026 Warren Wilbur - MIT License
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

#define LOAD_OPERATION_FLAG 0
#define VERIFY_OPERATION_FLAG 1

// Load a file to memory
// Returns a status:
// - 0xff: Success
// - other: Kernal Error Code (https://commodore.ca/manuals/pdfs/commodore_error_messages.pdf)
char LoadFileToMemory(
        // [in] input device number, e.g. 8 for disk drive one, 9 for disk drive two, 1 for tape drive
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
        char directoryEntryNumber)
{
    setnam(inputFilename);
    setlfs(storageDevice);
    return load(toAddress, LOAD_OPERATION_FLAG);
}

// Save memory region to file
// Returns a status:
// - 0xff: Success
// - other: Kernal Error Code (https://commodore.ca/manuals/pdfs/commodore_error_messages.pdf)
char SaveMemoryToFile(
        // [in] memory address of the start of the region to save to file
        char* startAddress,
        // [in] output device number, e.g. 8 for disk drive one, 9 for disk drive two, 1 for tape drive
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
        char* directoryEntryNumber)
{
    if (storageDevice != 1)
    {
        setnam(outputFilename);
    }
    setlfs(storageDevice);
    return save(startAddress, length);
}

// Basic ERROR function
// ERROR. Show error.
void error(
        char err)
{
    asm {
        // Basic SHOWERR function
        // Input: X = Error Code
        ldx err
        jsr $a437
    }
}

// Kernal SETNAM function
// SETNAM. Set file name parameters.
void setnam(
        // [in] file name to load, e.g. "sprite1.dat"
        char* inputFilename)
{
    char inputFilenameLength = (char)strlen(inputFilename);
    asm {
        // Kernal SETNAM function
        // SETNAM. Set file name parameters.
        // Input: A = File name length; X/Y = Pointer to file name.
        lda inputFilenameLength
        ldx inputFilename
        ldy inputFilename+1
        jsr $ffbd
    }
}

// SETLFS. Set file parameters.
void setlfs(
        // [in] input device number, e.g. 8 for disk drive, 1 for tape drive
        char device)
{
    asm {
        // SETLFS. Set file parameters.
        // Input: A = Logical number; X = Device number; Y = Secondary address.
        ldx device
        lda #1
        ldy #0
        jsr $ffba
    }
}

// LOAD. Load or verify file. (Must call SETLFS and SETNAM beforehands.)
// - verify: 0 = Load, 1-255 = Verify
//
// Returns a status, 0xff: Success other: Kernal Error Code
char load(
        // [in] memory address to load the file data at (caller must reserve enough memory at this address to hold the file or risk buffer overflow)
        char* toAddress,
        // [in] 0 for load, non-zero for verify
        char operationFlag)
{
    char status;
    asm {
        // Input: A: 0 = Load, 1-255 = Verify; X/Y = Load address (if secondary address = 0).
        ldx toAddress
        ldy toAddress+1
        lda operationFlag
        jsr $ffd5
        // Output: Carry: 0 = No errors, 1 = Error; A = KERNAL error code (if Carry = 1); X/Y = Address of last byte loaded/verified (if Carry = 0).
        bcs error
        lda #$ff
        error:
        sta status
    }
    return status;
}

/** SAVE. Save memory region to file. (Must call SETLFS and SETNAM beforehands for floppy device. Only SETLFS is required for tape device.)
 *
 * Returns a status, 0xff: Success other: Kernal Error Code */

//UNTESTED!!!!!!
char save(
        // [in] memory address of the start of the region to save to file
        char* startAddress,
        // [in] length in bytes of the region to save (beginning from startAddress)
        unsigned short length)
{
    char *endAddress = startAddress + length;
    char status;
    asm {
        // Input: A: Start address of the memory region to save; X/Y = End address of the region to save.
        lda startAddress
        ldx endAddress
        ldy endAddress+1
        jsr $ffd8
        // Output: Carry: 0 = No errors, 1 = Error; A = KERNAL error code (if Carry = 1).
        bcs error
        lda #$ff
        error:
        sta status
    }
    return status;
}

#endif //KICKC

#ifdef OSCAR64

#include <c64/kernalio.h>

// Load a file to memory
// Returns a status:
// - 0xff: Success
// - other: Kernal Error Code (https://commodore.ca/manuals/pdfs/commodore_error_messages.pdf)
char LoadFileToMemory(
        // [in] input device number, e.g. 8 for disk drive one, 9 for disk drive two, 1 for tape drive
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
        char directoryEntryNumber)
{
    int status = 0;
    if (storageDevice != 1)
    {
        krnio_setnam(inputFilename);
    }
    // The 1541 disk drive has 16 channels: 0-1=reserved for DOS, 2-14=data communication, 15=command channel. Refer to: Inside Commodore DOS, section 2.4 'Using the Command Channel', pg#17.
    if (krnio_open(directoryEntryNumber, storageDevice, 2))
    {
        status = krnio_status();
        if (status == KRNIO_OK)
        {
            status = krnio_read(directoryEntryNumber, toAddress, length);
            krnio_close(directoryEntryNumber);
            if (status < 0)
            {
                return status;
            }
            if (status != length)
            {
                return status; //not really a properly defined error code, returning the length that was read (since it wasn't the length that was requested)
            }
        }
    }
    else
    {
        return krnio_status();
    }
    return 0xff;
}

// Save memory region to file
// Returns a status:
// - 0xff: Success
// - other: Kernal Error Code (https://commodore.ca/manuals/pdfs/commodore_error_messages.pdf)
char SaveMemoryToFile(
        // [in] memory address of the start of the region to save to file
        char* startAddress,
        // [in] output device number, e.g. 8 for disk drive one, 9 for disk drive two, 1 for tape drive
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
        char* directoryEntryNumber)
{
    char *endAddress = startAddress + length;
    char fnum = 2;
    if (storageDevice != 1)
    {
        krnio_setnam(outputFilename);
    }
    // The 1541 disk drive has 16 channels: 0-1=reserved for DOS, 2-14=data communication, 15=command channel. Refer to: Inside Commodore DOS, section 2.4 'Using the Command Channel', pg#17.
    if (krnio_open(directoryEntryNumber, storageDevice, 2))
    {
        krnio_save(storageDevice, startAddress, endAddress);
        krnio_close(directoryEntryNumber);
    }
    else
    {
        return krnio_status();
    }
    return 0xff;
}

#endif //OSCAR64

