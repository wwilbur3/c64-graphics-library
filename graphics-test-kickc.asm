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
  // Commodore 64 PRG executable file
.file [name="graphics-test-kickc.prg", type="prg", segments="Program"]
.segmentdef Program [segments="Basic, Code, Data"]
.segmentdef Basic [start=$0801]
.segmentdef Code [start=$80d]
.segmentdef Data [startAfter="Code"]
.segment Basic
:BasicUpstart(__start)
  /// $D011 Control Register #1 Bit#6: ECM Turn Extended Color Mode on/off
  .const VICII_ECM = $40
  /// $D011 Control Register #1  Bit#5: BMM Turn Bitmap Mode on/off
  .const VICII_BMM = $20
  /// $D016 Control register #2 Bit#4: MCM Turn Multicolor Mode on/off
  .const VICII_MCM = $10
  /// The colors of the C64
  .const BLACK = 0
  .const WHITE = 1
  .const LIGHT_BLUE = $e
  .const SIZEOF_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES = 8
  .const SIZEOF_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES = 6
  .const OFFSET_STRUCT_MOS6526_CIA_PORT_A_DDR = 2
  .const OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_CHARACTERSETDATAPTR = 4
  .const OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR = 2
  .const OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SPRITEDATAPTRS = 6
  .const OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR = 2
  .const OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_COLORDATAPTR = 4
  .const OFFSET_STRUCT_MOS6526_CIA_PORT_B_DDR = 3
  .const STACK_BASE = $103
  /// $D020 Border Color
  .label BORDER_COLOR = $d020
  /// $D021 Background Color 0
  .label BG_COLOR0 = $d021
  /// $D022 Background Color 1
  .label BG_COLOR1 = $d022
  /// $D023 Background Color 2
  .label BG_COLOR2 = $d023
  /// $D011 Control Register #1
  /// - Bit#0-#2: YSCROLL Screen Soft Scroll Vertical
  /// - Bit#3: RSEL Switch betweem 25 or 24 visible rows
  ///          RSEL|  Display window height   | First line  | Last line
  ///          ----+--------------------------+-------------+----------
  ///            0 | 24 text lines/192 pixels |   55 ($37)  | 246 ($f6)
  ///            1 | 25 text lines/200 pixels |   51 ($33)  | 250 ($fa)
  /// - Bit#4: DEN Switch VIC-II output on/off
  /// - Bit#5: BMM Turn Bitmap Mode on/off
  /// - Bit#6: ECM Turn Extended Color Mode on/off
  /// - Bit#7: RST8 9th Bit for $D012 Rasterline counter
  /// Initial Value: %10011011
  .label VICII_CONTROL1 = $d011
  /// $D016 Control register 2
  /// -  Bit#0-#2: XSCROLL Screen Soft Scroll Horizontal
  /// -  Bit#3: CSEL Switch betweem 40 or 38 visible columns
  ///           CSEL|   Display window width   | First X coo. | Last X coo.
  ///           ----+--------------------------+--------------+------------
  ///             0 | 38 characters/304 pixels |   31 ($1f)   |  334 ($14e)
  ///             1 | 40 characters/320 pixels |   24 ($18)   |  343 ($157)
  /// -  Bit#4: MCM Turn Multicolor Mode on/off
  /// -  Bit#5-#7: not used
  /// Initial Value: %00001000
  .label VICII_CONTROL2 = $d016
  /// $D018 VIC-II base addresses
  /// - Bit#0: not used
  /// - Bit#1-#3: CB Address Bits 11-13 of the Character Set (*2048)
  /// - Bit#4-#7: VM Address Bits 10-13 of the Screen RAM (*1024)
  /// Initial Value: %00010100
  .label VICII_MEMORY = $d018
  /// Color Ram
  .label COLORRAM = $d800
  /// Default address of screen character matrix
  .label DEFAULT_SCREEN = $400
  /// The CIA#1: keyboard matrix, joystick #1/#2
  .label CIA1 = $dc00
  // The number of bytes on the screen
  // The current cursor x-position
  .label conio_cursor_x = $44
  // The current cursor y-position
  .label conio_cursor_y = $3a
  // The current text cursor line start
  .label conio_line_text = $3d
  // The current color cursor line start
  .label conio_line_color = $3b
.segment Code
__start: {
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:15
    lda #0
    sta.z conio_cursor_x
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:17
    sta.z conio_cursor_y
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:19
    lda #<DEFAULT_SCREEN
    sta.z conio_line_text
    lda #>DEFAULT_SCREEN
    sta.z conio_line_text+1
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:21
    lda #<COLORRAM
    sta.z conio_line_color
    lda #>COLORRAM
    sta.z conio_line_color+1
    // /Users/warren/Compilers/6502/kickc/lib/conio-c64.c:26
    jsr conio_c64_init
    jsr main
    rts
}
// Set initial cursor position
conio_c64_init: {
    // Position cursor at current line
    .label BASIC_CURSOR_LINE = $d6
    // /Users/warren/Compilers/6502/kickc/lib/conio-c64.c:32
    ldx.z BASIC_CURSOR_LINE
    // /Users/warren/Compilers/6502/kickc/lib/conio-c64.c:33
    cpx #$19
    bcc __b1
    ldx #$19-1
  __b1:
    // /Users/warren/Compilers/6502/kickc/lib/conio-c64.c:34
    jsr gotoxy
    // /Users/warren/Compilers/6502/kickc/lib/conio-c64.c:35
    rts
}
// Output one character at the current cursor position
// Moves the cursor forward. Scrolls the entire screen if needed
// void cputc(__register(A) char c)
cputc: {
    .const OFFSET_STACK_C = 0
    tsx
    lda STACK_BASE+OFFSET_STACK_C,x
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:88
    cmp #'\n'
    beq __b1
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:91
    ldy.z conio_cursor_x
    sta (conio_line_text),y
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:92
    lda #LIGHT_BLUE
    sta (conio_line_color),y
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:93
    inc.z conio_cursor_x
    lda #$28
    cmp.z conio_cursor_x
    bne __breturn
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:94
    jsr cputln
  __breturn:
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:96
    rts
  __b1:
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:89
    jsr cputln
    rts
}
// Faster bitmap testing, increase loop increment value to not draw every line (e.g. two to draw every other line)
main: {
    .label characterModeAddresses = $6f
    .label bitmapModeAddresses = $77
    .label __26 = $40
    .label __80 = $35
    .label __143 = $31
    .label __151 = $19
    .label __159 = $2d
    .label i = $5d
    .label i_1 = $54
    .label i_2 = $4b
    .label color = $1f
    .label i_3 = $5f
    .label si = $66
    .label i_4 = $60
    .label i_5 = $61
    .label si_1 = $67
    .label i_6 = $62
    .label si_2 = $68
    .label si_3 = $69
    .label height_c = $35
    .label i_7 = $48
    .label i_8 = $63
    .label i_9 = $65
    .label color2 = $21
    .label color3 = $20
    .label i_10 = $6a
    .label i_11 = $64
    .label j = $4c
    .label sj = $55
    .label j_1 = $4e
    .label j_2 = $50
    .label sj_1 = $57
    .label j_3 = $52
    .label sj_2 = $59
    .label sj_3 = $5b
    .label width_s = $1d
    .label height_s = $31
    .label j_4 = $42
    .label j_5 = $49
    .label i_12 = $5e
    // line drawing tests - standard character mode
    .label ch = $1a
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:61
    ldy #SIZEOF_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES
    lda #0
  !:
    dey
    sta characterModeAddresses,y
    bne !-
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:62
    ldy #SIZEOF_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES
    lda #0
  !:
    dey
    sta bitmapModeAddresses,y
    bne !-
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:77
    jsr keyboard_init
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:89
    jsr printf_str
    lda #0
    sta.z i
  __b1:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:90
    lda.z i
    cmp #$ff
    bcs !__b2+
    jmp __b2
  !__b2:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:101
  // standard character mode testing
    jsr GetVic2CharacterModeMemoryMappedAddresses
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:104
    //characterModeAddresses.vic2MemoryBankPtr = SetVic2VideoMemoryBank(1);
    //SetVic2CharacterModeMemoryLocations(1, 1, &characterModeAddresses);
    jsr SetMode_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:106
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr+1
    jsr ClearScreen_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:107
    ldx #WHITE
    jsr SetScreenForegroundColor_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:108
    lda #BLACK
    tax
    jsr SetScreenBackgroundAndBorderColors
    lda #0
    sta.z i_1
  // screen fill test - standard character mode
  __b4:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:114
    lda.z i_1
    cmp #$1a
    bcs !__b5+
    jmp __b5
  !__b5:
    lda #0
    sta.z i_2
  // screen color test - standard character mode
  __b6:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:131
    lda.z i_2
    cmp #$10
    bcs !__b7+
    jmp __b7
  !__b7:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:148
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z FillScreen_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z FillScreen_StandardCharacterMode.screenDataPtr+1
  //Use ASCII code 'a' (which equals PETSCII code 'A')
    ldx #'a'
    jsr FillScreen_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:149
    ldx #1
    jsr SetScreenForegroundColor_StandardCharacterMode
    lda #0
    sta.z color
    sta.z i_3
  // line drawing tests 1 - standard character mode - originate from top-left
  __b9:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:151
    lda.z i_3
    cmp #$28
    bcs !__b10+
    jmp __b10
  !__b10:
    lda #$19-1
    sta.z si
  __b11:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:159
    lda.z si
    cmp #0
    bmi !__b12+
    jmp __b12
  !__b12:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:171
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr+1
    jsr ClearScreen_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:173
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z FillScreen_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z FillScreen_StandardCharacterMode.screenDataPtr+1
    ldx #'a'
    jsr FillScreen_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:174
    ldx #1
    jsr SetScreenForegroundColor_StandardCharacterMode
    lda #0
    sta.z i_4
  // line drawing tests 2 - standard character mode - originate from bottom-left
  __b14:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:176
    lda.z i_4
    cmp #$28
    bcs !__b15+
    jmp __b15
  !__b15:
    lda #0
    sta.z i_5
  __b16:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:184
    lda.z i_5
    cmp #$19
    bcs !__b17+
    jmp __b17
  !__b17:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:195
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr+1
    jsr ClearScreen_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:197
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z FillScreen_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z FillScreen_StandardCharacterMode.screenDataPtr+1
    ldx #'a'
    jsr FillScreen_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:198
    ldx #1
    jsr SetScreenForegroundColor_StandardCharacterMode
    lda #$28-1
    sta.z si_1
  // line drawing tests 3 - standard character mode - originate from bottom-right
  __b19:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:200
    lda.z si_1
    cmp #0
    bmi !__b20+
    jmp __b20
  !__b20:
    lda #0
    sta.z i_6
  __b21:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:209
    lda.z i_6
    cmp #$19
    bcs !__b22+
    jmp __b22
  !__b22:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:220
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr+1
    jsr ClearScreen_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:222
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z FillScreen_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z FillScreen_StandardCharacterMode.screenDataPtr+1
    ldx #'a'
    jsr FillScreen_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:223
    ldx #1
    jsr SetScreenForegroundColor_StandardCharacterMode
    lda #$28-1
    sta.z si_2
  // line drawing tests 4 - standard character mode - originate from top-right
  __b24:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:225
    lda.z si_2
    cmp #0
    bmi !__b25+
    jmp __b25
  !__b25:
    lda #$19-1
    sta.z si_3
  __b26:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:234
    lda.z si_3
    cmp #0
    bmi !__b27+
    jmp __b27
  !__b27:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:246
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr+1
    jsr ClearScreen_StandardCharacterMode
    lda #0
    sta.z i_7
  // rectangle drawing test - standard character mode
  __b29:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:249
    lda.z i_7
    cmp #$19/2+1
    bcs !__b30+
    jmp __b30
  !__b30:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:259
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr+1
    jsr ClearScreen_StandardCharacterMode
    lda #1
    sta.z i_8
  // circle drawing test - standard character mode
  __b32:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:265
    lda.z i_8
    cmp #$19/2+1
    bcs !__b33+
    jmp __b33
  !__b33:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:274
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr+1
    jsr ClearScreen_StandardCharacterMode
    lda #0
    sta.z i_9
  // triangle drawing test - standard character mode
  __b35:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:278
    lda.z i_9
    cmp #$19/2
    bcs !__b36+
    jmp __b36
  !__b36:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:290
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr+1
    jsr ClearScreen_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:292
    jsr SetMode_MulticolorCharacterMode
    lda #0
    sta.z i_10
  // multicolor character mode testing
  __b38:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:294
    lda.z i_10
    cmp #$1a
    bcs !__b39+
    jmp __b39
  !__b39:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:310
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr+1
    jsr ClearScreen_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:319
    // standard bitmap mode testing
    jsr SetMode_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:325
    jsr GetVic2BitmapModeMemoryMappedAddresses
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:334
    jsr SetVic2VideoMemoryBank
    lda.z __26
    sta.z bitmapModeAddresses
    lda.z __26+1
    sta.z bitmapModeAddresses+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:341
    jsr SetVic2BitmapModeMemoryLocations
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:344
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z ClearScreen_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z ClearScreen_StandardBitmapMode.bitmapDataPtr+1
  // screen fill test - standard bitmap mode
    jsr ClearScreen_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:345
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z FillScreen_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z FillScreen_StandardBitmapMode.bitmapDataPtr+1
    jsr FillScreen_StandardBitmapMode
    lda #0
    sta.z i_11
  // screen color test - standard bitmap mode
  __b41:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:348
    lda.z i_11
    cmp #$10
    bcs !__b42+
    jmp __b42
  !__b42:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:358
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z ClearScreen_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z ClearScreen_StandardBitmapMode.bitmapDataPtr+1
  // line drawing tests - standard bitmap mode
    jsr ClearScreen_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:359
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_COLORDATAPTR
    sta.z SetScreenColor_StandardBitmapMode.colorDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_COLORDATAPTR+1
    sta.z SetScreenColor_StandardBitmapMode.colorDataPtr+1
    ldx #1
    jsr SetScreenColor_StandardBitmapMode
    lda #<0
    sta.z j
    sta.z j+1
  // line drawing tests - standard bitmap mode - originate from top-left
  __b44:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:362
    lda.z j+1
    cmp #>$140
    bcs !__b45+
    jmp __b45
  !__b45:
    bne !+
    lda.z j
    cmp #<$140
    bcs !__b45+
    jmp __b45
  !__b45:
  !:
    lda #<$c8-1
    sta.z sj
    lda #>$c8-1
    sta.z sj+1
  __b46:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:366
    lda.z sj+1
    bmi !__b47+
    jmp __b47
  !__b47:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:373
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z ClearScreen_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z ClearScreen_StandardBitmapMode.bitmapDataPtr+1
    jsr ClearScreen_StandardBitmapMode
    lda #<0
    sta.z j_1
    sta.z j_1+1
  // line drawing tests - standard bitmap mode - originate from bottom-left
  __b49:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:376
    lda.z j_1+1
    cmp #>$140
    bcs !__b50+
    jmp __b50
  !__b50:
    bne !+
    lda.z j_1
    cmp #<$140
    bcs !__b50+
    jmp __b50
  !__b50:
  !:
    lda #<0
    sta.z j_2
    sta.z j_2+1
  __b51:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:380
    lda.z j_2+1
    bne !+
    lda.z j_2
    cmp #$c8
    bcs !__b52+
    jmp __b52
  !__b52:
  !:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:387
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z ClearScreen_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z ClearScreen_StandardBitmapMode.bitmapDataPtr+1
    jsr ClearScreen_StandardBitmapMode
    lda #<$140-1
    sta.z sj_1
    lda #>$140-1
    sta.z sj_1+1
  // line drawing tests - standard bitmap mode - originate from bottom-right
  __b54:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:390
    lda.z sj_1+1
    bmi !__b55+
    jmp __b55
  !__b55:
    lda #<0
    sta.z j_3
    sta.z j_3+1
  __b56:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:394
    lda.z j_3+1
    bne !+
    lda.z j_3
    cmp #$c8
    bcs !__b57+
    jmp __b57
  !__b57:
  !:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:401
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z ClearScreen_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z ClearScreen_StandardBitmapMode.bitmapDataPtr+1
    jsr ClearScreen_StandardBitmapMode
    lda #<$140-1
    sta.z sj_2
    lda #>$140-1
    sta.z sj_2+1
  // line drawing tests - standard bitmap mode - originate from top-right
  __b59:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:404
    lda.z sj_2+1
    bmi !__b60+
    jmp __b60
  !__b60:
    lda #<$c8-1
    sta.z sj_3
    lda #>$c8-1
    sta.z sj_3+1
  __b61:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:408
    lda.z sj_3+1
    bmi !__b62+
    jmp __b62
  !__b62:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:415
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z ClearScreen_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z ClearScreen_StandardBitmapMode.bitmapDataPtr+1
    jsr ClearScreen_StandardBitmapMode
    lda #<0
    sta.z j_4
    sta.z j_4+1
  // rectangle drawing test - standard bitmap mode
  __b64:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:418
    lda.z j_4+1
    bne !+
    lda.z j_4
    cmp #$c8/2
    bcs !__b65+
    jmp __b65
  !__b65:
  !:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:427
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z ClearScreen_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z ClearScreen_StandardBitmapMode.bitmapDataPtr+1
    jsr ClearScreen_StandardBitmapMode
    lda #<1
    sta.z j_5
    lda #>1
    sta.z j_5+1
  // circle drawing test - standard bitmap mode
  __b67:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:430
    lda.z j_5+1
    bne !+
    lda.z j_5
    cmp #$c8/2
    bcs !__b68+
    jmp __b68
  !__b68:
  !:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:437
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z ClearScreen_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z ClearScreen_StandardBitmapMode.bitmapDataPtr+1
    jsr ClearScreen_StandardBitmapMode
    lda #0
    sta.z i_12
  // triangle drawing test - standard bitmap mode
  __b70:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:441
    lda.z i_12
    cmp #$c8/2
    bcc __b71
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:450
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr+1
    jsr ClearScreen_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:452
    jsr SetMode_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:453
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z ClearScreen_StandardCharacterMode.screenDataPtr+1
    jsr ClearScreen_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:454
    ldx #WHITE
    jsr SetScreenForegroundColor_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:455
    lda #BLACK
    tax
    jsr SetScreenBackgroundAndBorderColors
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:457
    rts
  __b71:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:443
    lda.z i_12
    lsr
    sta.z DrawTriangle_StandardBitmapMode.y2
    lda #0
    sta.z DrawTriangle_StandardBitmapMode.y2+1
    lda.z DrawTriangle_StandardBitmapMode.y2
    sta.z __151
    sec
    lda #$140/2-1
    sbc.z __151
    sta.z DrawTriangle_StandardBitmapMode.x2
    lda #0
    sbc #0
    sta.z DrawTriangle_StandardBitmapMode.x2+1
    sec
    lda #$c8-1
    sbc.z i_12
    sta.z DrawTriangle_StandardBitmapMode.y3
    lda #0
    sbc #0
    sta.z DrawTriangle_StandardBitmapMode.y3+1
    lda.z i_12
    sta.z DrawTriangle_StandardBitmapMode.x1
    lda #0
    sta.z DrawTriangle_StandardBitmapMode.x1+1
    lda.z i_12
    sta.z DrawTriangle_StandardBitmapMode.x3
    lda #0
    sta.z DrawTriangle_StandardBitmapMode.x3+1
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z DrawTriangle_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z DrawTriangle_StandardBitmapMode.bitmapDataPtr+1
    lda.z DrawTriangle_StandardBitmapMode.y2
    sta.z DrawTriangle_StandardBitmapMode.y1
    lda.z DrawTriangle_StandardBitmapMode.y2+1
    sta.z DrawTriangle_StandardBitmapMode.y1+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:443
    jsr DrawTriangle_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:444
    sec
    lda #<$140-1
    sbc.z i_12
    sta.z DrawTriangle_StandardBitmapMode.x3
    lda #>$140-1
    sbc #0
    sta.z DrawTriangle_StandardBitmapMode.x3+1
    lda.z i_12
    lsr
    sta.z DrawTriangle_StandardBitmapMode.y2
    lda #0
    sta.z DrawTriangle_StandardBitmapMode.y2+1
    lda.z DrawTriangle_StandardBitmapMode.y2
    sta.z __159
    lda #$140/2+1
    clc
    adc.z __159
    sta.z DrawTriangle_StandardBitmapMode.x2
    lda #0
    adc #0
    sta.z DrawTriangle_StandardBitmapMode.x2+1
    sec
    lda #$c8-1
    sbc.z i_12
    sta.z DrawTriangle_StandardBitmapMode.y3
    lda #0
    sbc #0
    sta.z DrawTriangle_StandardBitmapMode.y3+1
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z DrawTriangle_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z DrawTriangle_StandardBitmapMode.bitmapDataPtr+1
    lda.z DrawTriangle_StandardBitmapMode.x3
    sta.z DrawTriangle_StandardBitmapMode.x1
    lda.z DrawTriangle_StandardBitmapMode.x3+1
    sta.z DrawTriangle_StandardBitmapMode.x1+1
    lda.z DrawTriangle_StandardBitmapMode.y2
    sta.z DrawTriangle_StandardBitmapMode.y1
    lda.z DrawTriangle_StandardBitmapMode.y2+1
    sta.z DrawTriangle_StandardBitmapMode.y1+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:444
    jsr DrawTriangle_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:445
    lda.z i_12
    clc
    adc #1
    sta.z DrawTriangle_StandardBitmapMode.x1
    lda #0
    adc #0
    sta.z DrawTriangle_StandardBitmapMode.x1+1
    sec
    lda #$c8-1
    sbc.z i_12
    sta.z DrawTriangle_StandardBitmapMode.y3
    lda #0
    sbc #0
    sta.z DrawTriangle_StandardBitmapMode.y3+1
    sec
    lda #<$140-1-1
    sbc.z i_12
    sta.z DrawTriangle_StandardBitmapMode.x3
    lda #>$140-1-1
    sbc #0
    sta.z DrawTriangle_StandardBitmapMode.x3+1
    lda.z i_12
    sta.z DrawTriangle_StandardBitmapMode.y2
    lda #0
    sta.z DrawTriangle_StandardBitmapMode.y2+1
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z DrawTriangle_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z DrawTriangle_StandardBitmapMode.bitmapDataPtr+1
    lda.z DrawTriangle_StandardBitmapMode.y3
    sta.z DrawTriangle_StandardBitmapMode.y1
    lda.z DrawTriangle_StandardBitmapMode.y3+1
    sta.z DrawTriangle_StandardBitmapMode.y1+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:445
    lda #<$140/2
    sta.z DrawTriangle_StandardBitmapMode.x2
    lda #>$140/2
    sta.z DrawTriangle_StandardBitmapMode.x2+1
    jsr DrawTriangle_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:441
    lax.z i_12
    axs #-[5]
    stx.z i_12
    jmp __b70
  __b68:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:432
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z DrawCircle_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z DrawCircle_StandardBitmapMode.bitmapDataPtr+1
    jsr DrawCircle_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:430
    lda #5
    clc
    adc.z j_5
    sta.z j_5
    bcc !+
    inc.z j_5+1
  !:
    jmp __b67
  __b65:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:420
    lda.z j_4
    asl
    sta.z __143
    lda.z j_4+1
    rol
    sta.z __143+1
    sec
    lda #<$140-1
    sbc.z __143
    sta.z width_s
    lda #>$140-1
    sbc.z __143+1
    sta.z width_s+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:421
    lda #<$c8-1
    sec
    sbc.z height_s
    sta.z height_s
    lda #>$c8-1
    sbc.z height_s+1
    sta.z height_s+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:422
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z DrawRectangle_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z DrawRectangle_StandardBitmapMode.bitmapDataPtr+1
    jsr DrawRectangle_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:418
    lda #5
    clc
    adc.z j_4
    sta.z j_4
    bcc !+
    inc.z j_4+1
  !:
    jmp __b64
  __b62:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:410
    lda.z sj_3
    sta.z DrawLine_StandardBitmapMode.y1
    lda.z sj_3+1
    sta.z DrawLine_StandardBitmapMode.y1+1
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z DrawLine_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z DrawLine_StandardBitmapMode.bitmapDataPtr+1
    lda #<$140-1
    sta.z DrawLine_StandardBitmapMode.x0
    lda #>$140-1
    sta.z DrawLine_StandardBitmapMode.x0+1
    lda #<0
    sta.z DrawLine_StandardBitmapMode.x1
    sta.z DrawLine_StandardBitmapMode.x1+1
    sta.z DrawLine_StandardBitmapMode.y0
    sta.z DrawLine_StandardBitmapMode.y0+1
    jsr DrawLine_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:408
    lda.z sj_3
    sec
    sbc #5
    sta.z sj_3
    lda.z sj_3+1
    sbc #>5
    sta.z sj_3+1
    jmp __b61
  __b60:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:406
    lda.z sj_2
    sta.z DrawLine_StandardBitmapMode.x1
    lda.z sj_2+1
    sta.z DrawLine_StandardBitmapMode.x1+1
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z DrawLine_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z DrawLine_StandardBitmapMode.bitmapDataPtr+1
    lda #<$140-1
    sta.z DrawLine_StandardBitmapMode.x0
    lda #>$140-1
    sta.z DrawLine_StandardBitmapMode.x0+1
    lda #<$c8-1
    sta.z DrawLine_StandardBitmapMode.y1
    lda #>$c8-1
    sta.z DrawLine_StandardBitmapMode.y1+1
    lda #<0
    sta.z DrawLine_StandardBitmapMode.y0
    sta.z DrawLine_StandardBitmapMode.y0+1
    jsr DrawLine_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:404
    lda.z sj_2
    sec
    sbc #5
    sta.z sj_2
    lda.z sj_2+1
    sbc #>5
    sta.z sj_2+1
    jmp __b59
  __b57:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:396
    lda.z j_3
    sta.z DrawLine_StandardBitmapMode.y1
    lda.z j_3+1
    sta.z DrawLine_StandardBitmapMode.y1+1
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z DrawLine_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z DrawLine_StandardBitmapMode.bitmapDataPtr+1
    lda #<$140-1
    sta.z DrawLine_StandardBitmapMode.x0
    lda #>$140-1
    sta.z DrawLine_StandardBitmapMode.x0+1
    lda #<0
    sta.z DrawLine_StandardBitmapMode.x1
    sta.z DrawLine_StandardBitmapMode.x1+1
    lda #<$c8-1
    sta.z DrawLine_StandardBitmapMode.y0
    lda #>$c8-1
    sta.z DrawLine_StandardBitmapMode.y0+1
    jsr DrawLine_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:394
    lda #5
    clc
    adc.z j_3
    sta.z j_3
    bcc !+
    inc.z j_3+1
  !:
    jmp __b56
  __b55:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:392
    lda.z sj_1
    sta.z DrawLine_StandardBitmapMode.x1
    lda.z sj_1+1
    sta.z DrawLine_StandardBitmapMode.x1+1
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z DrawLine_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z DrawLine_StandardBitmapMode.bitmapDataPtr+1
    lda #<$140-1
    sta.z DrawLine_StandardBitmapMode.x0
    lda #>$140-1
    sta.z DrawLine_StandardBitmapMode.x0+1
    lda #<0
    sta.z DrawLine_StandardBitmapMode.y1
    sta.z DrawLine_StandardBitmapMode.y1+1
    lda #<$c8-1
    sta.z DrawLine_StandardBitmapMode.y0
    lda #>$c8-1
    sta.z DrawLine_StandardBitmapMode.y0+1
    jsr DrawLine_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:390
    lda.z sj_1
    sec
    sbc #5
    sta.z sj_1
    lda.z sj_1+1
    sbc #>5
    sta.z sj_1+1
    jmp __b54
  __b52:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:382
    lda.z j_2
    sta.z DrawLine_StandardBitmapMode.y1
    lda.z j_2+1
    sta.z DrawLine_StandardBitmapMode.y1+1
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z DrawLine_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z DrawLine_StandardBitmapMode.bitmapDataPtr+1
    lda #<0
    sta.z DrawLine_StandardBitmapMode.x0
    sta.z DrawLine_StandardBitmapMode.x0+1
    lda #<$140-1
    sta.z DrawLine_StandardBitmapMode.x1
    lda #>$140-1
    sta.z DrawLine_StandardBitmapMode.x1+1
    lda #<$c8-1
    sta.z DrawLine_StandardBitmapMode.y0
    lda #>$c8-1
    sta.z DrawLine_StandardBitmapMode.y0+1
    jsr DrawLine_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:380
    lda #5
    clc
    adc.z j_2
    sta.z j_2
    bcc !+
    inc.z j_2+1
  !:
    jmp __b51
  __b50:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:378
    lda.z j_1
    sta.z DrawLine_StandardBitmapMode.x1
    lda.z j_1+1
    sta.z DrawLine_StandardBitmapMode.x1+1
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z DrawLine_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z DrawLine_StandardBitmapMode.bitmapDataPtr+1
    lda #<0
    sta.z DrawLine_StandardBitmapMode.x0
    sta.z DrawLine_StandardBitmapMode.x0+1
    sta.z DrawLine_StandardBitmapMode.y1
    sta.z DrawLine_StandardBitmapMode.y1+1
    lda #<$c8-1
    sta.z DrawLine_StandardBitmapMode.y0
    lda #>$c8-1
    sta.z DrawLine_StandardBitmapMode.y0+1
    jsr DrawLine_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:376
    lda #5
    clc
    adc.z j_1
    sta.z j_1
    bcc !+
    inc.z j_1+1
  !:
    jmp __b49
  __b47:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:368
    lda.z sj
    sta.z DrawLine_StandardBitmapMode.y1
    lda.z sj+1
    sta.z DrawLine_StandardBitmapMode.y1+1
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z DrawLine_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z DrawLine_StandardBitmapMode.bitmapDataPtr+1
    lda #<0
    sta.z DrawLine_StandardBitmapMode.x0
    sta.z DrawLine_StandardBitmapMode.x0+1
    lda #<$140-1
    sta.z DrawLine_StandardBitmapMode.x1
    lda #>$140-1
    sta.z DrawLine_StandardBitmapMode.x1+1
    lda #<0
    sta.z DrawLine_StandardBitmapMode.y0
    sta.z DrawLine_StandardBitmapMode.y0+1
    jsr DrawLine_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:366
    lda.z sj
    sec
    sbc #5
    sta.z sj
    lda.z sj+1
    sbc #>5
    sta.z sj+1
    jmp __b46
  __b45:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:364
    lda.z j
    sta.z DrawLine_StandardBitmapMode.x1
    lda.z j+1
    sta.z DrawLine_StandardBitmapMode.x1+1
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    sta.z DrawLine_StandardBitmapMode.bitmapDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    sta.z DrawLine_StandardBitmapMode.bitmapDataPtr+1
    lda #<0
    sta.z DrawLine_StandardBitmapMode.x0
    sta.z DrawLine_StandardBitmapMode.x0+1
    lda #<$c8-1
    sta.z DrawLine_StandardBitmapMode.y1
    lda #>$c8-1
    sta.z DrawLine_StandardBitmapMode.y1+1
    lda #<0
    sta.z DrawLine_StandardBitmapMode.y0
    sta.z DrawLine_StandardBitmapMode.y0+1
    jsr DrawLine_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:362
    lda #5
    clc
    adc.z j
    sta.z j
    bcc !+
    inc.z j+1
  !:
    jmp __b44
  __b42:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:350
    ldx.z color
    inx
    lda #$10-1
    sax.z color
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:351
    ldx.z color
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_COLORDATAPTR
    sta.z SetScreenColor_StandardBitmapMode.colorDataPtr
    lda.z bitmapModeAddresses+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_COLORDATAPTR+1
    sta.z SetScreenColor_StandardBitmapMode.colorDataPtr+1
    jsr SetScreenColor_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:348
    inc.z i_11
    jmp __b41
  __b39:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:296
    ldx.z color
    inx
    lda #$10-1
    sax.z color
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:297
    ldx.z color
    inx
    sax.z color2
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:298
    lda.z color
    clc
    adc #2
    and #$10-1
    sta.z color3
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:302
    lax.z i_10
    axs #-['a']
    stx.z ch
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:304
    ldy.z color
    ldx.z color2
    lda.z color3
    //Use ASCII code 'a' (which equals PETSCII code 'A')
    jsr SetScreenBackgroundColors_MulticolorCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:305
    ldx.z ch
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z FillScreen_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z FillScreen_StandardCharacterMode.screenDataPtr+1
    jsr FillScreen_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:294
    inc.z i_10
    jmp __b38
  __b36:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:280
    ldx.z color
    inx
    lda #$10-1
    sax.z color
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:281
    lda.z i_9
    lsr
    sta.z DrawTriangle_StandardCharacterMode.y2
    eor #$ff
    sec
    adc #$28/2-1
    sta.z DrawTriangle_StandardCharacterMode.x2
    lda #$19-1
    sec
    sbc.z i_9
    sta.z DrawTriangle_StandardCharacterMode.y3
    lda.z i_9
    sta.z DrawTriangle_StandardCharacterMode.x1
    lda.z i_9
    sta.z DrawTriangle_StandardCharacterMode.x3
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z DrawTriangle_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z DrawTriangle_StandardCharacterMode.screenDataPtr+1
    lda.z DrawTriangle_StandardCharacterMode.y2
    sta.z DrawTriangle_StandardCharacterMode.y1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:281
    jsr DrawTriangle_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:282
    ldx.z color
    inx
    lda #$10-1
    sax.z color
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:283
    lda #$28-1
    sec
    sbc.z i_9
    sta.z DrawTriangle_StandardCharacterMode.x3
    lda.z i_9
    lsr
    sta.z DrawTriangle_StandardCharacterMode.y2
    clc
    adc #$28/2+1
    sta.z DrawTriangle_StandardCharacterMode.x2
    lda #$19-1
    sec
    sbc.z i_9
    sta.z DrawTriangle_StandardCharacterMode.y3
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z DrawTriangle_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z DrawTriangle_StandardCharacterMode.screenDataPtr+1
    lda.z DrawTriangle_StandardCharacterMode.x3
    sta.z DrawTriangle_StandardCharacterMode.x1
    lda.z DrawTriangle_StandardCharacterMode.y2
    sta.z DrawTriangle_StandardCharacterMode.y1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:283
    jsr DrawTriangle_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:284
    ldx.z color
    inx
    lda #$10-1
    sax.z color
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:285
    ldy.z i_9
    iny
    sty.z DrawTriangle_StandardCharacterMode.x1
    lda #$19-1
    sec
    sbc.z i_9
    sta.z DrawTriangle_StandardCharacterMode.y3
    lda #$28-1-1
    sec
    sbc.z i_9
    sta.z DrawTriangle_StandardCharacterMode.x3
    lda.z i_9
    sta.z DrawTriangle_StandardCharacterMode.y2
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z DrawTriangle_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z DrawTriangle_StandardCharacterMode.screenDataPtr+1
    lda.z DrawTriangle_StandardCharacterMode.y3
    sta.z DrawTriangle_StandardCharacterMode.y1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:285
    lda #$28/2
    sta.z DrawTriangle_StandardCharacterMode.x2
    jsr DrawTriangle_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:278
    inc.z i_9
    jmp __b35
  __b33:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:268
    ldx.z color
    inx
    lda #$10-1
    sax.z color
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:269
    ldx.z i_8
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z DrawCircle_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z DrawCircle_StandardCharacterMode.screenDataPtr+1
    jsr DrawCircle_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:265
    inc.z i_8
    jmp __b32
  __b30:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:251
    lda.z i_7
    asl
    sta.z __80
    lda #$28-1
    sec
    sbc.z __80
    tay
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:252
    lda #$19-1
    sec
    sbc.z height_c
    sta.z height_c
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:253
    ldx.z color
    inx
    lda #$10-1
    sax.z color
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:254
    lda.z i_7
    sta.z DrawRectangle_StandardCharacterMode.x
    sty.z DrawRectangle_StandardCharacterMode.width
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z DrawRectangle_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z DrawRectangle_StandardCharacterMode.screenDataPtr+1
    jsr DrawRectangle_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:249
    inc.z i_7
    jmp __b29
  __b27:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:237
    ldx.z color
    inx
    lda #$10-1
    sax.z color
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:241
    ldy.z si_3
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z DrawLine_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z DrawLine_StandardCharacterMode.screenDataPtr+1
    lda #$28-1
    sta.z DrawLine_StandardCharacterMode.x0
    ldx #0
    txa
    sta.z DrawLine_StandardCharacterMode.y0
    jsr DrawLine_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:234
    dec.z si_3
    jmp __b26
  __b25:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:228
    ldx.z color
    inx
    lda #$10-1
    sax.z color
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:232
    ldx.z si_2
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z DrawLine_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z DrawLine_StandardCharacterMode.screenDataPtr+1
    lda #$28-1
    sta.z DrawLine_StandardCharacterMode.x0
    ldy #$19-1
    lda #0
    sta.z DrawLine_StandardCharacterMode.y0
    jsr DrawLine_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:225
    dec.z si_2
    jmp __b24
  __b22:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:211
    ldx.z color
    inx
    lda #$10-1
    sax.z color
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:215
    ldy.z i_6
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z DrawLine_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z DrawLine_StandardCharacterMode.screenDataPtr+1
    lda #$28-1
    sta.z DrawLine_StandardCharacterMode.x0
    ldx #0
    lda #$19-1
    sta.z DrawLine_StandardCharacterMode.y0
    jsr DrawLine_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:209
    inc.z i_6
    jmp __b21
  __b20:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:203
    ldx.z color
    inx
    lda #$10-1
    sax.z color
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:207
    ldx.z si_1
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z DrawLine_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z DrawLine_StandardCharacterMode.screenDataPtr+1
    lda #$28-1
    sta.z DrawLine_StandardCharacterMode.x0
    ldy #0
    lda #$19-1
    sta.z DrawLine_StandardCharacterMode.y0
    jsr DrawLine_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:200
    dec.z si_1
    jmp __b19
  __b17:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:186
    ldx.z color
    inx
    lda #$10-1
    sax.z color
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:190
    ldy.z i_5
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z DrawLine_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z DrawLine_StandardCharacterMode.screenDataPtr+1
    lda #0
    sta.z DrawLine_StandardCharacterMode.x0
    ldx #$28-1
    lda #$19-1
    sta.z DrawLine_StandardCharacterMode.y0
    jsr DrawLine_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:184
    inc.z i_5
    jmp __b16
  __b15:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:178
    ldx.z color
    inx
    lda #$10-1
    sax.z color
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:182
    ldx.z i_4
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z DrawLine_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z DrawLine_StandardCharacterMode.screenDataPtr+1
    lda #0
    sta.z DrawLine_StandardCharacterMode.x0
    tay
    lda #$19-1
    sta.z DrawLine_StandardCharacterMode.y0
    jsr DrawLine_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:176
    inc.z i_4
    jmp __b14
  __b12:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:162
    ldx.z color
    inx
    lda #$10-1
    sax.z color
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:166
    ldy.z si
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z DrawLine_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z DrawLine_StandardCharacterMode.screenDataPtr+1
    lda #0
    sta.z DrawLine_StandardCharacterMode.x0
    ldx #$28-1
    sta.z DrawLine_StandardCharacterMode.y0
    jsr DrawLine_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:159
    dec.z si
    jmp __b11
  __b10:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:153
    ldx.z color
    inx
    lda #$10-1
    sax.z color
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:157
    ldx.z i_3
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z DrawLine_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z DrawLine_StandardCharacterMode.screenDataPtr+1
    lda #0
    sta.z DrawLine_StandardCharacterMode.x0
    ldy #$19-1
    sta.z DrawLine_StandardCharacterMode.y0
    jsr DrawLine_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:151
    inc.z i_3
    jmp __b9
  __b7:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:136
    ldx.z i_2
    jsr SetScreenForegroundColor_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:131
    inc.z i_2
    jmp __b6
  __b5:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:119
    lax.z i_1
    axs #-['a']
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:124
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    sta.z FillScreen_StandardCharacterMode.screenDataPtr
    lda.z characterModeAddresses+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    sta.z FillScreen_StandardCharacterMode.screenDataPtr+1
  //Use ASCII code 'a' (which equals PETSCII code 'A')
    jsr FillScreen_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:114
    inc.z i_1
    jmp __b4
  __b2:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:92
    ldx #1
    jsr SetScreenForegroundColor_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:93
    lda #$f
    and.z i
    tax
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:93
    jsr SetScreenBackgroundAndBorderColors
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/graphics-test.c:90
    inc.z i
    jmp __b1
  .segment Data
    s: .text @"\nwelcome to the c64 graphics library test"
    .byte 0
}
.segment Code
// Set the cursor to the specified position
// void gotoxy(char x, __register(X) char y)
gotoxy: {
    .const x = 0
    .label __5 = $6d
    .label __6 = $46
    .label __7 = $46
    .label line_offset = $46
    .label __8 = $6b
    .label __9 = $46
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:50
    cpx #$19+1
    bcc __b2
    ldx #0
  __b2:
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:52
    lda #x
    sta.z conio_cursor_x
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:53
    stx.z conio_cursor_y
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:54
    txa
    sta.z __7
    lda #0
    sta.z __7+1
    lda.z __7
    asl
    sta.z __8
    lda.z __7+1
    rol
    sta.z __8+1
    asl.z __8
    rol.z __8+1
    clc
    lda.z __9
    adc.z __8
    sta.z __9
    lda.z __9+1
    adc.z __8+1
    sta.z __9+1
    asl.z line_offset
    rol.z line_offset+1
    asl.z line_offset
    rol.z line_offset+1
    asl.z line_offset
    rol.z line_offset+1
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:55
    lda.z line_offset
    clc
    adc #<DEFAULT_SCREEN
    sta.z __5
    lda.z line_offset+1
    adc #>DEFAULT_SCREEN
    sta.z __5+1
    lda.z __5
    sta.z conio_line_text
    lda.z __5+1
    sta.z conio_line_text+1
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:56
    lda.z __6
    clc
    adc #<COLORRAM
    sta.z __6
    lda.z __6+1
    adc #>COLORRAM
    sta.z __6+1
    lda.z __6
    sta.z conio_line_color
    lda.z __6+1
    sta.z conio_line_color+1
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:57
    rts
}
// Print a newline
cputln: {
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:100
    lda #$28
    clc
    adc.z conio_line_text
    sta.z conio_line_text
    bcc !+
    inc.z conio_line_text+1
  !:
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:101
    lda #$28
    clc
    adc.z conio_line_color
    sta.z conio_line_color
    bcc !+
    inc.z conio_line_color+1
  !:
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:102
    lda #0
    sta.z conio_cursor_x
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:103
    inc.z conio_cursor_y
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:104
    jsr cscroll
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:105
    rts
}
// Initialize keyboard reading by setting CIA#1 Data Direction Registers
keyboard_init: {
    // /Users/warren/Compilers/6502/kickc/lib/c64-keyboard.c:44
    // Keyboard Matrix Columns Write Mode
    lda #$ff
    sta CIA1+OFFSET_STRUCT_MOS6526_CIA_PORT_A_DDR
    // /Users/warren/Compilers/6502/kickc/lib/c64-keyboard.c:46
    // Keyboard Matrix Columns Read Mode
    lda #0
    sta CIA1+OFFSET_STRUCT_MOS6526_CIA_PORT_B_DDR
    // /Users/warren/Compilers/6502/kickc/lib/c64-keyboard.c:47
    rts
}
/// Print a NUL-terminated string
// void printf_str(void (*putc)(char), __zp($28) const char *s)
printf_str: {
    .label s = $28
    lda #<main.s
    sta.z s
    lda #>main.s
    sta.z s+1
  __b1:
    // /Users/warren/Compilers/6502/kickc/lib/printf.c:17
    ldy #0
    lda (s),y
    inc.z s
    bne !+
    inc.z s+1
  !:
    cmp #0
    bne __b2
    // /Users/warren/Compilers/6502/kickc/lib/printf.c:19
    rts
  __b2:
    // /Users/warren/Compilers/6502/kickc/lib/printf.c:18
    pha
    jsr cputc
    pla
    jmp __b1
}
/** VIC-II Graphics Chip Routines - Character Mode */
// void GetVic2CharacterModeMemoryMappedAddresses(struct CharacterModeMemoryMappedAddresses *memoryMappedAddressesPtr)
GetVic2CharacterModeMemoryMappedAddresses: {
    .label memoryMappedAddressesPtr = main.characterModeAddresses
    .label __0 = $40
    .label __2 = 6
    .label __3 = 6
    .label __6 = $33
    .label __7 = $33
    .label __8 = $37
    .label __18 = 6
    .label __22 = $33
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:115
    jsr GetVic2VideoMemoryBank
    lda.z __0
    sta.z memoryMappedAddressesPtr
    lda.z __0+1
    sta.z memoryMappedAddressesPtr+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:116
    lda VICII_MEMORY
    and #$e
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:117
    sta.z __18
    lda #0
    sta.z __18+1
    ldy #$a
    cpy #0
    beq !e+
  !:
    asl.z __2
    rol.z __2+1
    dey
    bne !-
  !e:
    clc
    lda.z __3
    adc.z memoryMappedAddressesPtr
    sta.z __3
    lda.z __3+1
    adc.z memoryMappedAddressesPtr+1
    sta.z __3+1
    lda.z __3
    sta.z memoryMappedAddressesPtr+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_CHARACTERSETDATAPTR
    lda.z __3+1
    sta.z memoryMappedAddressesPtr+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_CHARACTERSETDATAPTR+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:118
    lda VICII_MEMORY
    and #$f0
    lsr
    lsr
    lsr
    lsr
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:119
    sta.z __22
    lda #0
    sta.z __22+1
    ldy #$a
    cpy #0
    beq !e+
  !:
    asl.z __6
    rol.z __6+1
    dey
    bne !-
  !e:
    clc
    lda.z __7
    adc.z memoryMappedAddressesPtr
    sta.z __7
    lda.z __7+1
    adc.z memoryMappedAddressesPtr+1
    sta.z __7+1
    lda.z __7
    sta.z memoryMappedAddressesPtr+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    lda.z __7+1
    sta.z memoryMappedAddressesPtr+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:120
    clc
    lda.z memoryMappedAddressesPtr+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR
    adc #<$3f8
    sta.z __8
    lda.z memoryMappedAddressesPtr+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SCREENDATAPTR+1
    adc #>$3f8
    sta.z __8+1
    lda.z __8
    sta.z memoryMappedAddressesPtr+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SPRITEDATAPTRS
    lda.z __8+1
    sta.z memoryMappedAddressesPtr+OFFSET_STRUCT_CHARACTERMODEMEMORYMAPPEDADDRESSES_SPRITEDATAPTRS+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:121
    rts
}
/** Standard Character Mode */
// void SetMode_StandardCharacterMode(char backgroundColor0)
SetMode_StandardCharacterMode: {
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:195
    lda #VICII_BMM^$ff
    and VICII_CONTROL1
    sta VICII_CONTROL1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:196
    lda #VICII_ECM^$ff
    and VICII_CONTROL1
    sta VICII_CONTROL1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:197
    lda #VICII_MCM^$ff
    and VICII_CONTROL2
    sta VICII_CONTROL2
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:198
    lda #BLACK
    sta BG_COLOR0
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:199
    rts
}
// void ClearScreen_StandardCharacterMode(__zp(6) char *screenDataPtr)
ClearScreen_StandardCharacterMode: {
    .label screenDataPtr = 6
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:228
    ldx #$20
    jsr FillScreen_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:229
    rts
}
// void SetScreenForegroundColor_StandardCharacterMode(__register(X) char foregroundColor)
SetScreenForegroundColor_StandardCharacterMode: {
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:205
    lda #<COLORRAM
    sta.z memset.str
    lda #>COLORRAM
    sta.z memset.str+1
    lda #<$3e8
    sta.z memset.num
    lda #>$3e8
    sta.z memset.num+1
    jsr memset
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:206
    rts
}
/** Generic routines for all graphics modes */
// void SetScreenBackgroundAndBorderColors(__register(X) char backgroundColor0, __register(A) char borderColor)
SetScreenBackgroundAndBorderColors: {
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:185
    stx BG_COLOR0
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:186
    sta BORDER_COLOR
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:187
    rts
}
// void FillScreen_StandardCharacterMode(__register(X) char ch, __zp(6) char *screenDataPtr)
FillScreen_StandardCharacterMode: {
    .label screenDataPtr = 6
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:221
    lda #<$3e8
    sta.z memset.num
    lda #>$3e8
    sta.z memset.num+1
    jsr memset
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:222
    rts
}
/** Multicolor Character Mode
 *
 * Use *_StandardCharacterMode routines but ensure that foreground color has always has high (i.e. 0x08) bit set
 * for multicolor character mode. This means that the only foreground color codes that can be used in multicolor
 * mode are:
 *
 * 0x08 = multicolor mode BLACK
 * 0x09 = multicolor mode WHITE
 * 0x0A = multicolor mode RED
 * 0x0B = multicolor mode CYAN
 * 0x0C = multicolor mode PURPLE
 * 0x0D = multicolor mode GREEN
 * 0x0E = multicolor mode BLUE
 * 0x0F = multicolor mode YELLOW
 *
 * If the high bit (0x08) isn't set in the foreground color then the character is drawn by the VIC-II chip in
 * standard character mode (thus you can mix standard and multicolor mode characters on the screen, but with a
 * limited foreground color palette):
 *
 * 0x01 = standard mode BLACK
 * 0x02 = standard mode WHITE
 * 0x03 = standard mode RED
 * 0x04 = standard mode CYAN
 * 0x05 = standard mode PURPLE
 * 0x06 = standard mode GREEN
 * 0x07 = standard mode BLUE
 * 
 * The three background colors can be chosen from the entire C64 color palette.
 */
// void SetMode_MulticolorCharacterMode(char backgroundColor0, char backgroundColor1, char backgroundColor2)
SetMode_MulticolorCharacterMode: {
    .const backgroundColor0 = 0
    .const backgroundColor1 = 1
    .const backgroundColor2 = 2
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:714
    lda #VICII_BMM^$ff
    and VICII_CONTROL1
    sta VICII_CONTROL1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:715
    lda #VICII_ECM^$ff
    and VICII_CONTROL1
    sta VICII_CONTROL1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:716
    lda #VICII_MCM
    ora VICII_CONTROL2
    sta VICII_CONTROL2
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:717
    lda #backgroundColor0
    sta BG_COLOR0
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:718
    lda #backgroundColor1
    sta BG_COLOR1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:719
    lda #backgroundColor2
    sta BG_COLOR2
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:720
    rts
}
/** Standard Bitmap Mode */
SetMode_StandardBitmapMode: {
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:805
    lda #VICII_BMM
    ora VICII_CONTROL1
    sta VICII_CONTROL1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:806
    lda #VICII_ECM^$ff
    and VICII_CONTROL1
    sta VICII_CONTROL1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:807
    lda #VICII_MCM^$ff
    and VICII_CONTROL2
    sta VICII_CONTROL2
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:808
    rts
}
/** VIC-II Graphics Chip Routines - Bitmap Mode */
// void GetVic2BitmapModeMemoryMappedAddresses(struct BitmapModeMemoryMappedAddresses *memoryMappedAddressesPtr)
GetVic2BitmapModeMemoryMappedAddresses: {
    .label memoryMappedAddressesPtr = main.bitmapModeAddresses
    .label __0 = $40
    .label __3 = 6
    .label __4 = 6
    .label __7 = $33
    .label __8 = $33
    .label __16 = 6
    .label __20 = $33
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:151
    jsr GetVic2VideoMemoryBank
    lda.z __0
    sta.z memoryMappedAddressesPtr
    lda.z __0+1
    sta.z memoryMappedAddressesPtr+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:152
    lda VICII_MEMORY
    and #8
    lsr
    lsr
    lsr
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:153
    sta.z __16
    lda #0
    sta.z __16+1
    ldy #$d
    cpy #0
    beq !e+
  !:
    asl.z __3
    rol.z __3+1
    dey
    bne !-
  !e:
    clc
    lda.z __4
    adc.z memoryMappedAddressesPtr
    sta.z __4
    lda.z __4+1
    adc.z memoryMappedAddressesPtr+1
    sta.z __4+1
    lda.z __4
    sta.z memoryMappedAddressesPtr+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    lda.z __4+1
    sta.z memoryMappedAddressesPtr+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:154
    lda VICII_MEMORY
    and #$f0
    lsr
    lsr
    lsr
    lsr
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:155
    sta.z __20
    lda #0
    sta.z __20+1
    ldy #$a
    cpy #0
    beq !e+
  !:
    asl.z __7
    rol.z __7+1
    dey
    bne !-
  !e:
    clc
    lda.z __8
    adc.z memoryMappedAddressesPtr
    sta.z __8
    lda.z __8+1
    adc.z memoryMappedAddressesPtr+1
    sta.z __8+1
    lda.z __8
    sta.z memoryMappedAddressesPtr+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_COLORDATAPTR
    lda.z __8+1
    sta.z memoryMappedAddressesPtr+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_COLORDATAPTR+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:156
    rts
}
// __zp($40) char * SetVic2VideoMemoryBank(char memoryBankIndex)
SetVic2VideoMemoryBank: {
    .const memoryBankIndex = 2
    .label return = $40
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:102
    lda #$fc
    and $dd00
    ora #memoryBankIndex
    // The currently selected block of 16384 chars that is used by the VIC-II chip is known as the video memory bank. The VIC-II
    // chip can only address 16384 chars of memory at a time. Setting the current VIC-II video memory bank as follows:
    //      %11 = 0
    //      %10 = 16384
    //      %01 = 32768
    //      %00 = 49152
    sta $dd00
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:103
    jsr GetVic2VideoMemoryBank
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:104
    rts
}
// void SetVic2BitmapModeMemoryLocations(char bitmapDataIndex, char colorDataIndex, struct BitmapModeMemoryMappedAddresses *memoryMappedAddressesPtr)
SetVic2BitmapModeMemoryLocations: {
    .const bitmapDataIndex = 1
    .const colorDataIndex = 1
    .label memoryMappedAddressesPtr = main.bitmapModeAddresses
    .label __4 = $37
    .label __6 = $c
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:172
    lda #colorDataIndex<<4|bitmapDataIndex<<3
    sta VICII_MEMORY
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:173
    clc
    lda.z memoryMappedAddressesPtr
    adc #<bitmapDataIndex*$2000
    sta.z __4
    lda.z memoryMappedAddressesPtr+1
    adc #>bitmapDataIndex*$2000
    sta.z __4+1
    lda.z __4
    sta.z memoryMappedAddressesPtr+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR
    lda.z __4+1
    sta.z memoryMappedAddressesPtr+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_BITMAPDATAPTR+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:174
    clc
    lda.z memoryMappedAddressesPtr
    adc #<colorDataIndex*$400
    sta.z __6
    lda.z memoryMappedAddressesPtr+1
    adc #>colorDataIndex*$400
    sta.z __6+1
    lda.z __6
    sta.z memoryMappedAddressesPtr+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_COLORDATAPTR
    lda.z __6+1
    sta.z memoryMappedAddressesPtr+OFFSET_STRUCT_BITMAPMODEMEMORYMAPPEDADDRESSES_COLORDATAPTR+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:175
    rts
}
// void ClearScreen_StandardBitmapMode(__zp(6) char *bitmapDataPtr)
ClearScreen_StandardBitmapMode: {
    .label bitmapDataPtr = 6
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:833
    ldx #0
    lda #<$1f40
    sta.z memset.num
    lda #>$1f40
    sta.z memset.num+1
    jsr memset
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:834
    rts
}
// void FillScreen_StandardBitmapMode(__zp(6) char *bitmapDataPtr)
FillScreen_StandardBitmapMode: {
    .label bitmapDataPtr = 6
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:826
    ldx #$ff
    lda #<$1f40
    sta.z memset.num
    lda #>$1f40
    sta.z memset.num+1
    jsr memset
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:827
    rts
}
// void SetScreenColor_StandardBitmapMode(__register(X) char foregroundColor, char backgroundColor, __zp(6) char *colorDataPtr)
SetScreenColor_StandardBitmapMode: {
    .label colorDataPtr = 6
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:818
    txa
    asl
    asl
    asl
    asl
    tax
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:819
    lda #<$3e8
    sta.z memset.num
    lda #>$3e8
    sta.z memset.num+1
    jsr memset
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:820
    rts
}
// void DrawTriangle_StandardBitmapMode(__zp($28) unsigned int x1, __zp(6) unsigned int y1, __zp($33) unsigned int x2, __zp($37) unsigned int y2, __zp(2) unsigned int x3, __zp(4) unsigned int y3, __zp($10) char *bitmapDataPtr)
DrawTriangle_StandardBitmapMode: {
    .label x1 = $28
    .label x2 = $33
    .label y2 = $37
    .label x3 = 2
    .label y3 = 4
    .label bitmapDataPtr = $10
    .label y1 = 6
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1205
    lda.z x1
    sta.z DrawLine_StandardBitmapMode.x0
    lda.z x1+1
    sta.z DrawLine_StandardBitmapMode.x0+1
    lda.z y1
    sta.z DrawLine_StandardBitmapMode.y0
    lda.z y1+1
    sta.z DrawLine_StandardBitmapMode.y0+1
    lda.z x2
    sta.z DrawLine_StandardBitmapMode.x1
    lda.z x2+1
    sta.z DrawLine_StandardBitmapMode.x1+1
    lda.z y2
    sta.z DrawLine_StandardBitmapMode.y1
    lda.z y2+1
    sta.z DrawLine_StandardBitmapMode.y1+1
    jsr DrawLine_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1206
    lda.z x2
    sta.z DrawLine_StandardBitmapMode.x0
    lda.z x2+1
    sta.z DrawLine_StandardBitmapMode.x0+1
    lda.z y2
    sta.z DrawLine_StandardBitmapMode.y0
    lda.z y2+1
    sta.z DrawLine_StandardBitmapMode.y0+1
    lda.z x3
    sta.z DrawLine_StandardBitmapMode.x1
    lda.z x3+1
    sta.z DrawLine_StandardBitmapMode.x1+1
    lda.z y3
    sta.z DrawLine_StandardBitmapMode.y1
    lda.z y3+1
    sta.z DrawLine_StandardBitmapMode.y1+1
    jsr DrawLine_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1207
    lda.z x3
    sta.z DrawLine_StandardBitmapMode.x0
    lda.z x3+1
    sta.z DrawLine_StandardBitmapMode.x0+1
    lda.z y3
    sta.z DrawLine_StandardBitmapMode.y0
    lda.z y3+1
    sta.z DrawLine_StandardBitmapMode.y0+1
    lda.z x1
    sta.z DrawLine_StandardBitmapMode.x1
    lda.z x1+1
    sta.z DrawLine_StandardBitmapMode.x1+1
    lda.z y1
    sta.z DrawLine_StandardBitmapMode.y1
    lda.z y1+1
    sta.z DrawLine_StandardBitmapMode.y1+1
    jsr DrawLine_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1208
    rts
}
// Bresenham's Circle Algorithm: http://members.chello.at/~easyfilter/bresenham.html and http://members.chello.at/%7Eeasyfilter/Bresenham.pdf
// void DrawCircle_StandardBitmapMode(unsigned int x0, unsigned int y0, __zp($49) unsigned int radius, __zp($10) char *bitmapDataPtr)
DrawCircle_StandardBitmapMode: {
    .const x0 = $140/2
    .const y0 = $c8/2
    .label __17 = 4
    .label __18 = 4
    .label __23 = 2
    .label __24 = 2
    .label x = $33
    .label point0_x = 8
    .label point0_y = $a
    .label point1_x = 8
    .label point1_y = $a
    .label point2_x = 8
    .label point2_y = $a
    .label point3_x = 8
    .label point3_y = $a
    .label y = $37
    .label error = 4
    .label error_1 = 2
    .label radius = $49
    .label bitmapDataPtr = $10
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1222
    sec
    lda #0
    sbc.z radius
    sta.z x
    lda #0
    sbc.z radius+1
    sta.z x+1
    lda #<2-(2<<1)
    sta.z error_1
    lda #>2-(2<<1)
    sta.z error_1+1
    lda #<0
    sta.z y
    sta.z y+1
  __b1:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1236
    lda #<x0
    sec
    sbc.z x
    sta.z point0_x
    lda #>x0
    sbc.z x+1
    sta.z point0_x+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1237
    clc
    lda.z y
    adc #<y0
    sta.z point0_y
    lda.z y+1
    adc #>y0
    sta.z point0_y+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1238
    jsr DrawPixel_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1239
    lda #<x0
    sec
    sbc.z y
    sta.z point1_x
    lda #>x0
    sbc.z y+1
    sta.z point1_x+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1240
    lda #<y0
    sec
    sbc.z x
    sta.z point1_y
    lda #>y0
    sbc.z x+1
    sta.z point1_y+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1241
    jsr DrawPixel_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1242
    clc
    lda.z x
    adc #<x0
    sta.z point2_x
    lda.z x+1
    adc #>x0
    sta.z point2_x+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1243
    lda #<y0
    sec
    sbc.z y
    sta.z point2_y
    lda #>y0
    sbc.z y+1
    sta.z point2_y+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1244
    jsr DrawPixel_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1245
    clc
    lda.z y
    adc #<x0
    sta.z point3_x
    lda.z y+1
    adc #>x0
    sta.z point3_x+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1246
    clc
    lda.z x
    adc #<y0
    sta.z point3_y
    lda.z x+1
    adc #>y0
    sta.z point3_y+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1247
    jsr DrawPixel_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1249
    lda.z y
    cmp.z error_1
    lda.z y+1
    sbc.z error_1+1
    bvc !+
    eor #$80
  !:
    bmi __b11
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1251
    inc.z y
    bne !+
    inc.z y+1
  !:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1252
    lda.z y
    asl
    sta.z __17
    lda.z y+1
    rol
    sta.z __17+1
    clc
    lda.z __18
    adc #1
    sta.z __18
    bcc !+
    inc.z __18+1
  !:
    clc
    lda.z error
    adc.z error_1
    sta.z error
    lda.z error+1
    adc.z error_1+1
    sta.z error+1
  __b2:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1254
    lda.z x
    cmp.z error_1
    lda.z x+1
    sbc.z error_1+1
    bvc !+
    eor #$80
  !:
    bmi __b5
    lda.z y
    cmp.z error
    lda.z y+1
    sbc.z error+1
    bvc !+
    eor #$80
  !:
    bpl __b12
  __b5:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1256
    inc.z x
    bne !+
    inc.z x+1
  !:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1257
    lda.z x
    asl
    sta.z __23
    lda.z x+1
    rol
    sta.z __23+1
    clc
    lda.z __24
    adc #1
    sta.z __24
    bcc !+
    inc.z __24+1
  !:
    clc
    lda.z error_1
    adc.z error
    sta.z error_1
    lda.z error_1+1
    adc.z error+1
    sta.z error_1+1
  __b3:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1259
    lda.z x+1
    bpl !__b1+
    jmp __b1
  !__b1:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1260
    rts
  __b12:
    lda.z error
    sta.z error_1
    lda.z error+1
    sta.z error_1+1
    jmp __b3
  __b11:
    lda.z error_1
    sta.z error
    lda.z error_1+1
    sta.z error+1
    jmp __b2
}
// void DrawRectangle_StandardBitmapMode(__zp($42) unsigned int x, __zp($42) unsigned int y, __zp($1d) unsigned int width, __zp($31) unsigned int height, __zp($10) char *bitmapDataPtr)
DrawRectangle_StandardBitmapMode: {
    .label x = $42
    .label y = $42
    .label width = $1d
    .label height = $31
    .label bitmapDataPtr = $10
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1183
    lda.z x
    clc
    adc.z width
    sta.z DrawHorizontalLine_StandardBitmapMode.x1
    lda.z x+1
    adc.z width+1
    sta.z DrawHorizontalLine_StandardBitmapMode.x1+1
    lda.z y
    sta.z DrawHorizontalLine_StandardBitmapMode.y
    lda.z y+1
    sta.z DrawHorizontalLine_StandardBitmapMode.y+1
    jsr DrawHorizontalLine_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1184
    lda.z x
    clc
    adc.z width
    sta.z DrawHorizontalLine_StandardBitmapMode.x1
    lda.z x+1
    adc.z width+1
    sta.z DrawHorizontalLine_StandardBitmapMode.x1+1
    lda.z y
    clc
    adc.z height
    sta.z DrawHorizontalLine_StandardBitmapMode.y
    lda.z y+1
    adc.z height+1
    sta.z DrawHorizontalLine_StandardBitmapMode.y+1
    jsr DrawHorizontalLine_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1185
    lda.z y
    clc
    adc.z height
    sta.z DrawVerticalLine_StandardBitmapMode.y1
    lda.z y+1
    adc.z height+1
    sta.z DrawVerticalLine_StandardBitmapMode.y1+1
    lda.z x
    sta.z DrawVerticalLine_StandardBitmapMode.x
    lda.z x+1
    sta.z DrawVerticalLine_StandardBitmapMode.x+1
    jsr DrawVerticalLine_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1186
    lda.z x
    clc
    adc.z width
    sta.z DrawVerticalLine_StandardBitmapMode.x
    lda.z x+1
    adc.z width+1
    sta.z DrawVerticalLine_StandardBitmapMode.x+1
    lda.z y
    clc
    adc.z height
    sta.z DrawVerticalLine_StandardBitmapMode.y1
    lda.z y+1
    adc.z height+1
    sta.z DrawVerticalLine_StandardBitmapMode.y1+1
    jsr DrawVerticalLine_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1187
    rts
}
// Generalized Bresenham's line algorithm: https://www.phatcode.net/res/224/files/html/ch35/35-03.html
// void DrawLine_StandardBitmapMode(__zp($16) unsigned int x0, __zp($40) unsigned int y0, __zp(8) unsigned int x1, __zp($a) unsigned int y1, __zp($10) char *bitmapDataPtr)
DrawLine_StandardBitmapMode: {
    .label deltaX = $24
    .label deltaY = $22
    .label x0 = $16
    .label y0 = $40
    .label x1 = 8
    .label y1 = $a
    .label bitmapDataPtr = $10
    .label x1_1 = $24
    .label x0_1 = 8
    .label y1_1 = $22
    .label y0_1 = $a
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1028
    lda.z y0+1
    cmp.z y1+1
    bne !+
    lda.z y0
    cmp.z y1
    bne !__b9+
    jmp __b9
  !__b9:
  !:
    bcs !__b9+
    jmp __b9
  !__b9:
    lda.z x0
    sta.z x1_1
    lda.z x0+1
    sta.z x1_1+1
    lda.z y0
    sta.z y1_1
    lda.z y0+1
    sta.z y1_1+1
  __b1:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1041
    lda.z deltaX
    sec
    sbc.z x0_1
    sta.z deltaX
    lda.z deltaX+1
    sbc.z x0_1+1
    sta.z deltaX+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1042
    lda.z deltaY
    sec
    sbc.z y0_1
    sta.z deltaY
    lda.z deltaY+1
    sbc.z y0_1+1
    sta.z deltaY+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1043
    lda.z deltaX+1
    bne !+
    lda.z deltaX
    beq !e+
    lsr
  !:
    bpl __b2
  !e:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1062
    lda #0
    sec
    sbc.z deltaX
    sta.z deltaX
    lda #0
    sbc.z deltaX+1
    sta.z deltaX+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1063
    lda.z deltaY
    cmp.z deltaX
    lda.z deltaY+1
    sbc.z deltaX+1
    bvc !+
    eor #$80
  !:
    bmi __b7
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1075
    lda #<-1
    sta.z DrawOctant1Line_StandardBitmapMode.xDirection
    sta.z DrawOctant1Line_StandardBitmapMode.xDirection+1
    jsr DrawOctant1Line_StandardBitmapMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1078
    rts
  __b7:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1068
    lda #<-1
    sta.z DrawOctant0Line_StandardBitmapMode.xDirection
    sta.z DrawOctant0Line_StandardBitmapMode.xDirection+1
    jsr DrawOctant0Line_StandardBitmapMode
    rts
  __b2:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1045
    lda.z deltaY
    cmp.z deltaX
    lda.z deltaY+1
    sbc.z deltaX+1
    bvc !+
    eor #$80
  !:
    bmi __b8
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1057
    lda #<1
    sta.z DrawOctant1Line_StandardBitmapMode.xDirection
    lda #>1
    sta.z DrawOctant1Line_StandardBitmapMode.xDirection+1
    jsr DrawOctant1Line_StandardBitmapMode
    rts
  __b8:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1050
    lda #<1
    sta.z DrawOctant0Line_StandardBitmapMode.xDirection
    lda #>1
    sta.z DrawOctant0Line_StandardBitmapMode.xDirection+1
    jsr DrawOctant0Line_StandardBitmapMode
    rts
  __b9:
    lda.z x1
    sta.z x1_1
    lda.z x1+1
    sta.z x1_1+1
    lda.z x0
    sta.z x0_1
    lda.z x0+1
    sta.z x0_1+1
    lda.z y1
    sta.z y1_1
    lda.z y1+1
    sta.z y1_1+1
    lda.z y0
    sta.z y0_1
    lda.z y0+1
    sta.z y0_1+1
    jmp __b1
}
// void SetScreenBackgroundColors_MulticolorCharacterMode(__register(Y) char backgroundColor0, __register(X) char backgroundColor1, __register(A) char backgroundColor2)
SetScreenBackgroundColors_MulticolorCharacterMode: {
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:730
    sty BG_COLOR0
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:731
    stx BG_COLOR1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:732
    sta BG_COLOR2
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:733
    rts
}
// void DrawTriangle_StandardCharacterMode(char ch, __zp($1f) char foregroundColor, __zp($35) char x1, __zp($39) char y1, __zp($2e) char x2, __zp($45) char y2, __zp($3f) char x3, __zp($36) char y3, __zp($10) char *screenDataPtr)
DrawTriangle_StandardCharacterMode: {
    .label foregroundColor = $1f
    .label x1 = $35
    .label x2 = $2e
    .label y2 = $45
    .label x3 = $3f
    .label y3 = $36
    .label screenDataPtr = $10
    .label y1 = $39
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:615
    lda.z x1
    sta.z DrawLine_StandardCharacterMode.x0
    lda.z y1
    sta.z DrawLine_StandardCharacterMode.y0
    ldx.z x2
    ldy.z y2
    jsr DrawLine_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:616
    lda.z x2
    sta.z DrawLine_StandardCharacterMode.x0
    lda.z y2
    sta.z DrawLine_StandardCharacterMode.y0
    ldx.z x3
    ldy.z y3
    jsr DrawLine_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:617
    lda.z x3
    sta.z DrawLine_StandardCharacterMode.x0
    lda.z y3
    sta.z DrawLine_StandardCharacterMode.y0
    ldx.z x1
    ldy.z y1
    jsr DrawLine_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:618
    rts
}
// Bresenham's Circle Algorithm: http://members.chello.at/%7Eeasyfilter/Bresenham.pdf
// void DrawCircle_StandardCharacterMode(char ch, __zp($1f) char foregroundColor, char x0, char y0, __register(X) char radius, __zp($10) char *screenDataPtr)
DrawCircle_StandardCharacterMode: {
    .const x0 = $28/2
    .const y0 = $19/2
    .label x = $35
    .label y = $39
    .label error = $2e
    .label foregroundColor = $1f
    .label screenDataPtr = $10
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:636
    txa
    eor #$ff
    clc
    adc #1
    sta.z x
    lda #2-(2<<1)
    sta.z error
    lda #0
    sta.z y
  __b1:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:650
    lda #x0
    sec
    sbc.z x
    tay
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:651
    lax.z y
    axs #-[y0]
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:652
    sty.z DrawCharAndSetForegroundColor_StandardCharacterMode.x
    stx.z DrawCharAndSetForegroundColor_StandardCharacterMode.y
    jsr DrawCharAndSetForegroundColor_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:653
    lda #x0
    sec
    sbc.z y
    tax
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:654
    lda #y0
    sec
    sbc.z x
    tay
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:655
    stx.z DrawCharAndSetForegroundColor_StandardCharacterMode.x
    sty.z DrawCharAndSetForegroundColor_StandardCharacterMode.y
    jsr DrawCharAndSetForegroundColor_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:656
    lax.z x
    axs #-[x0]
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:657
    lda #y0
    sec
    sbc.z y
    tay
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:658
    stx.z DrawCharAndSetForegroundColor_StandardCharacterMode.x
    sty.z DrawCharAndSetForegroundColor_StandardCharacterMode.y
    jsr DrawCharAndSetForegroundColor_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:659
    lda #x0
    clc
    adc.z y
    tay
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:660
    lax.z x
    axs #-[y0]
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:661
    sty.z DrawCharAndSetForegroundColor_StandardCharacterMode.x
    stx.z DrawCharAndSetForegroundColor_StandardCharacterMode.y
    jsr DrawCharAndSetForegroundColor_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:663
    lda.z error
    sec
    sbc.z y
    beq !e+
    bvc !+
    eor #$80
  !:
    bpl __b11
  !e:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:665
    inc.z y
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:666
    lda.z y
    asl
    clc
    adc #1
    clc
    adc.z error
    tax
  __b2:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:668
    lda.z error
    sec
    sbc.z x
    beq !e+
    bvc !+
    eor #$80
  !:
    bpl __b5
  !e:
    txa
    sec
    sbc.z y
    beq __b12
    bvc !+
    eor #$80
  !:
    bmi __b12
  __b5:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:670
    inc.z x
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:671
    lda.z x
    asl
    clc
    adc #1
    stx.z $ff
    clc
    adc.z $ff
    sta.z error
  __b3:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:673
    lda.z x
    bpl !__b1+
    jmp __b1
  !__b1:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:674
    rts
  __b12:
    stx.z error
    jmp __b3
  __b11:
    ldx.z error
    jmp __b2
}
// void DrawRectangle_StandardCharacterMode(char ch, __zp($1f) char foregroundColor, __zp($1a) char x, __zp($48) char y, __zp($27) char width, __zp($35) char height, __zp($10) char *screenDataPtr)
DrawRectangle_StandardCharacterMode: {
    .label foregroundColor = $1f
    .label x = $1a
    .label y = $48
    .label width = $27
    .label height = $35
    .label screenDataPtr = $10
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:589
    lda.z x
    clc
    adc.z width
    tax
    ldy.z x
    lda.z y
    sta.z DrawHorizontalLine_StandardCharacterMode.y
    jsr DrawHorizontalLine_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:590
    lda.z x
    clc
    adc.z width
    tax
    lda.z y
    clc
    adc.z height
    sta.z DrawHorizontalLine_StandardCharacterMode.y
    ldy.z x
    jsr DrawHorizontalLine_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:591
    lda.z y
    clc
    adc.z height
    tax
    lda.z y
    sta.z DrawVerticalLine_StandardCharacterMode.y0
    jsr DrawVerticalLine_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:592
    lda.z DrawVerticalLine_StandardCharacterMode.x
    clc
    adc.z width
    sta.z DrawVerticalLine_StandardCharacterMode.x
    lda.z y
    clc
    adc.z height
    tax
    lda.z y
    sta.z DrawVerticalLine_StandardCharacterMode.y0
    jsr DrawVerticalLine_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:593
    rts
}
// Generalized Bresenham's line algorithm: https://www.phatcode.net/res/224/files/html/ch35/35-03.html
// void DrawLine_StandardCharacterMode(char ch, __zp($1f) char foregroundColor, __zp($2d) char x0, __zp($19) char y0, __register(X) char x1, __register(Y) char y1, __zp($10) char *screenDataPtr)
DrawLine_StandardCharacterMode: {
    .label deltaX = $21
    .label deltaY = $20
    .label foregroundColor = $1f
    .label x0 = $2d
    .label y0 = $19
    .label screenDataPtr = $10
    .label x1 = $21
    .label y1 = $20
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:442
    cpy.z y0
    bcs __b9
    lda.z x0
    sta.z x1
    lda.z y0
    sta.z y1
  __b1:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:453
    txa
    eor #$ff
    sec
    adc.z deltaX
    sta.z deltaX
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:454
    tya
    eor #$ff
    sec
    adc.z deltaY
    sta.z deltaY
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:455
    lda.z deltaX
    cmp #0
    beq !+
    bpl __b2
  !:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:474
    lda.z deltaX
    eor #$ff
    clc
    adc #1
    sta.z deltaX
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:475
    sec
    sbc.z deltaY
    beq !e+
    bvc !+
    eor #$80
  !:
    bpl __b7
  !e:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:487
    stx.z DrawOctant1Line_StandardCharacterMode.x0
    sty.z DrawOctant1Line_StandardCharacterMode.y0
    ldx.z deltaX
    lda #-1
    sta.z DrawOctant1Line_StandardCharacterMode.xDirection
    jsr DrawOctant1Line_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:490
    rts
  __b7:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:480
    stx.z DrawOctant0Line_StandardCharacterMode.x0
    sty.z DrawOctant0Line_StandardCharacterMode.y0
    ldx.z deltaY
    lda #-1
    sta.z DrawOctant0Line_StandardCharacterMode.xDirection
    jsr DrawOctant0Line_StandardCharacterMode
    rts
  __b2:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:457
    lda.z deltaX
    sec
    sbc.z deltaY
    beq !e+
    bvc !+
    eor #$80
  !:
    bpl __b8
  !e:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:469
    stx.z DrawOctant1Line_StandardCharacterMode.x0
    sty.z DrawOctant1Line_StandardCharacterMode.y0
    ldx.z deltaX
    lda #1
    sta.z DrawOctant1Line_StandardCharacterMode.xDirection
    jsr DrawOctant1Line_StandardCharacterMode
    rts
  __b8:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:462
    stx.z DrawOctant0Line_StandardCharacterMode.x0
    sty.z DrawOctant0Line_StandardCharacterMode.y0
    ldx.z deltaY
    lda #1
    sta.z DrawOctant0Line_StandardCharacterMode.xDirection
    jsr DrawOctant0Line_StandardCharacterMode
    rts
  __b9:
    stx.z x1
    ldx.z x0
    sty.z y1
    ldy.z y0
    jmp __b1
}
// Scroll the entire screen if the cursor is beyond the last line
cscroll: {
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:109
    lda #$19
    cmp.z conio_cursor_y
    bne __breturn
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:111
    lda #<DEFAULT_SCREEN
    sta.z memcpy.destination
    lda #>DEFAULT_SCREEN
    sta.z memcpy.destination+1
    lda #<DEFAULT_SCREEN+$28
    sta.z memcpy.source
    lda #>DEFAULT_SCREEN+$28
    sta.z memcpy.source+1
    jsr memcpy
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:112
    lda #<COLORRAM
    sta.z memcpy.destination
    lda #>COLORRAM
    sta.z memcpy.destination+1
    lda #<COLORRAM+$28
    sta.z memcpy.source
    lda #>COLORRAM+$28
    sta.z memcpy.source+1
    jsr memcpy
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:113
    ldx #' '
    lda #<DEFAULT_SCREEN+$19*$28-$28
    sta.z memset.str
    lda #>DEFAULT_SCREEN+$19*$28-$28
    sta.z memset.str+1
    lda #<$28
    sta.z memset.num
    lda #>$28
    sta.z memset.num+1
    jsr memset
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:114
    ldx #LIGHT_BLUE
    lda #<COLORRAM+$19*$28-$28
    sta.z memset.str
    lda #>COLORRAM+$19*$28-$28
    sta.z memset.str+1
    lda #<$28
    sta.z memset.num
    lda #>$28
    sta.z memset.num+1
    jsr memset
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:115
    sec
    lda.z conio_line_text
    sbc #$28
    sta.z conio_line_text
    lda.z conio_line_text+1
    sbc #0
    sta.z conio_line_text+1
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:116
    sec
    lda.z conio_line_color
    sbc #$28
    sta.z conio_line_color
    lda.z conio_line_color+1
    sbc #0
    sta.z conio_line_color+1
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:117
    dec.z conio_cursor_y
  __breturn:
    // /Users/warren/Compilers/6502/kickc/lib/conio-cbm-shared.c:122
    rts
}
/** VIC-II Graphics Chip Routines */
GetVic2VideoMemoryBank: {
    .label return = $40
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:65
    // The currently selected block of 16384 chars that is used by the VIC-II chip is known as the video memory bank. The VIC-II
    // chip can only address 16384 chars of memory at a time. Reading the current VIC-II video memory bank setting:
    //      %00 = 49152
    //      %01 = 32768
    //      %10 = 16384
    //      %11 = 0
    lda #3
    and $dd00
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:70
    cmp #0
    beq __b1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:73
    cmp #1
    beq __b2
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:76
    cmp #2
    beq __b3
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:79
    cmp #3
    lda #<0
    sta.z return
    sta.z return+1
    rts
  __b1:
    lda #<$c000
    sta.z return
    lda #>$c000
    sta.z return+1
    rts
  __b2:
    lda #<$8000
    sta.z return
    lda #>$8000
    sta.z return+1
    rts
  __b3:
    lda #<$4000
    sta.z return
    lda #>$4000
    sta.z return+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:84
    rts
}
// Copies the character c (an unsigned char) to the first num characters of the object pointed to by the argument str.
// void * memset(__zp(6) void *str, __register(X) char c, __zp($16) unsigned int num)
memset: {
    .label str = 6
    .label num = $16
    .label end = $16
    .label dst = 6
    // /Users/warren/Compilers/6502/kickc/lib/string.c:32
    lda.z num
    bne !+
    lda.z num+1
    beq __breturn
  !:
    // /Users/warren/Compilers/6502/kickc/lib/string.c:33
    clc
    lda.z end
    adc.z str
    sta.z end
    lda.z end+1
    adc.z str+1
    sta.z end+1
  __b2:
    // /Users/warren/Compilers/6502/kickc/lib/string.c:34
    lda.z dst+1
    cmp.z end+1
    bne __b3
    lda.z dst
    cmp.z end
    bne __b3
  __breturn:
    // /Users/warren/Compilers/6502/kickc/lib/string.c:38
    rts
  __b3:
    // /Users/warren/Compilers/6502/kickc/lib/string.c:35
    txa
    ldy #0
    sta (dst),y
    // /Users/warren/Compilers/6502/kickc/lib/string.c:34
    inc.z dst
    bne !+
    inc.z dst+1
  !:
    jmp __b2
}
// void DrawPixel_StandardBitmapMode(__zp(8) unsigned int x, __zp($a) unsigned int y, __zp($10) char *bitmapDataPtr)
DrawPixel_StandardBitmapMode: {
    .label __3 = $c
    .label __4 = $12
    .label __5 = $c
    .label yCell = $c
    .label xCell = $12
    .label yByte = $14
    .label index = $14
    .label x = 8
    .label y = $a
    .label bitmapDataPtr = $10
    .label __12 = $14
    .label __13 = $e
    .label __14 = $c
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:855
    lda.z y+1
    lsr
    sta.z yCell+1
    lda.z y
    ror
    sta.z yCell
    lsr.z yCell+1
    ror.z yCell
    lsr.z yCell+1
    ror.z yCell
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:856
    lda.z x+1
    lsr
    sta.z xCell+1
    lda.z x
    ror
    sta.z xCell
    lsr.z xCell+1
    ror.z xCell
    lsr.z xCell+1
    ror.z xCell
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:857
    lda #7
    and.z y
    sta.z yByte
    lda #0
    sta.z yByte+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:858
    lda.z yCell
    asl
    sta.z __13
    lda.z yCell+1
    rol
    sta.z __13+1
    asl.z __13
    rol.z __13+1
    clc
    lda.z __14
    adc.z __13
    sta.z __14
    lda.z __14+1
    adc.z __13+1
    sta.z __14+1
    lda.z __3+1
    lsr
    sta.z $ff
    lda.z __3
    ror
    sta.z __3+1
    lda #0
    ror
    sta.z __3
    lsr.z $ff
    ror.z __3+1
    ror.z __3
    asl.z __4
    rol.z __4+1
    asl.z __4
    rol.z __4+1
    asl.z __4
    rol.z __4+1
    clc
    lda.z __5
    adc.z __4
    sta.z __5
    lda.z __5+1
    adc.z __4+1
    sta.z __5+1
    clc
    lda.z index
    adc.z __5
    sta.z index
    lda.z index+1
    adc.z __5+1
    sta.z index+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:859
    lda #7
    and.z x
    eor #$ff
    tax
    axs #-7-1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:860
    lda #1
    cpx #0
    beq !e+
  !:
    asl
    dex
    bne !-
  !e:
    tax
    clc
    lda.z __12
    adc.z bitmapDataPtr
    sta.z __12
    lda.z __12+1
    adc.z bitmapDataPtr+1
    sta.z __12+1
    txa
    ldy #0
    ora (__12),y
    sta (__12),y
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:864
    rts
}
// void DrawHorizontalLine_StandardBitmapMode(__zp($42) unsigned int x0, __zp($24) unsigned int x1, __zp($a) unsigned int y, __zp($10) char *bitmapDataPtr)
DrawHorizontalLine_StandardBitmapMode: {
    .label x = $28
    .label x0 = $42
    .label x1 = $24
    .label y = $a
    .label bitmapDataPtr = $10
    .label xDirection = $22
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1093
    lda.z x0+1
    cmp.z x1+1
    bcc __b1
    bne !+
    lda.z x0
    cmp.z x1
    bcc __b1
  !:
    lda #<-1
    sta.z xDirection
    sta.z xDirection+1
    jmp __b2
  __b1:
    lda #<1
    sta.z xDirection
    lda #>1
    sta.z xDirection+1
  __b2:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1115
    lda.z x0
    sta.z DrawPixel_StandardBitmapMode.x
    lda.z x0+1
    sta.z DrawPixel_StandardBitmapMode.x+1
    jsr DrawPixel_StandardBitmapMode
    lda.z x0
    sta.z x
    lda.z x0+1
    sta.z x+1
  __b3:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1116
    lda.z x+1
    cmp.z x1+1
    bne __b4
    lda.z x
    cmp.z x1
    bne __b4
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1124
    rts
  __b4:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1118
    clc
    lda.z x
    adc.z xDirection
    sta.z x
    lda.z x+1
    adc.z xDirection+1
    sta.z x+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1119
    lda.z x
    sta.z DrawPixel_StandardBitmapMode.x
    lda.z x+1
    sta.z DrawPixel_StandardBitmapMode.x+1
    jsr DrawPixel_StandardBitmapMode
    jmp __b3
}
// void DrawVerticalLine_StandardBitmapMode(__zp(8) unsigned int x, __zp($42) unsigned int y0, __zp($28) unsigned int y1, __zp($10) char *bitmapDataPtr)
DrawVerticalLine_StandardBitmapMode: {
    .label y = $1b
    .label x = 8
    .label y0 = $42
    .label y1 = $28
    .label bitmapDataPtr = $10
    .label yDirection = $2f
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1139
    lda.z y0+1
    cmp.z y1+1
    bcc __b1
    bne !+
    lda.z y0
    cmp.z y1
    bcc __b1
  !:
    lda #<-1
    sta.z yDirection
    sta.z yDirection+1
    jmp __b2
  __b1:
    lda #<1
    sta.z yDirection
    lda #>1
    sta.z yDirection+1
  __b2:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1160
    lda.z y0
    sta.z DrawPixel_StandardBitmapMode.y
    lda.z y0+1
    sta.z DrawPixel_StandardBitmapMode.y+1
    jsr DrawPixel_StandardBitmapMode
    lda.z y0
    sta.z y
    lda.z y0+1
    sta.z y+1
  __b3:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1161
    lda.z y+1
    cmp.z y1+1
    bne __b4
    lda.z y
    cmp.z y1
    bne __b4
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1169
    rts
  __b4:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1163
    clc
    lda.z y
    adc.z yDirection
    sta.z y
    lda.z y+1
    adc.z yDirection+1
    sta.z y+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1164
    lda.z y
    sta.z DrawPixel_StandardBitmapMode.y
    lda.z y+1
    sta.z DrawPixel_StandardBitmapMode.y+1
    jsr DrawPixel_StandardBitmapMode
    jmp __b3
}
// Generalized Bresenham's line algorithm for Octant1: https://www.phatcode.net/res/224/files/html/ch35/35-03.html
// void DrawOctant1Line_StandardBitmapMode(__zp(8) unsigned int x0, __zp($a) unsigned int y0, __zp($24) int deltaX, __zp($22) int deltaY, __zp($2f) int xDirection, __zp($10) char *bitmapDataPtr)
DrawOctant1Line_StandardBitmapMode: {
    .label __1 = $2a
    .label x0 = 8
    .label y0 = $a
    .label deltaX = $24
    .label deltaY = $22
    .label bitmapDataPtr = $10
    .label deltaXx2 = $24
    .label deltaXx2MinusDeltaYx2 = $2a
    .label error = $1b
    .label x = 8
    .label y = $a
    .label xDirection = $2f
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:981
    asl.z deltaXx2
    rol.z deltaXx2+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:982
    lda.z deltaY
    asl
    sta.z __1
    lda.z deltaY+1
    rol
    sta.z __1+1
    lda.z deltaXx2
    sec
    sbc.z deltaXx2MinusDeltaYx2
    sta.z deltaXx2MinusDeltaYx2
    lda.z deltaXx2+1
    sbc.z deltaXx2MinusDeltaYx2+1
    sta.z deltaXx2MinusDeltaYx2+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:983
    lda.z deltaXx2
    sec
    sbc.z deltaY
    sta.z error
    lda.z deltaXx2+1
    sbc.z deltaY+1
    sta.z error+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:990
    jsr DrawPixel_StandardBitmapMode
  __b1:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:991
    lda.z deltaY+1
    bne !+
    lda.z deltaY
    beq !e+
    lsr
  !:
    bpl __b2
  !e:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1009
    rts
  __b2:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:993
    lda.z deltaY
    bne !+
    dec.z deltaY+1
  !:
    dec.z deltaY
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:997
    lda.z error+1
    bpl __b3
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1004
    clc
    lda.z error
    adc.z deltaXx2
    sta.z error
    lda.z error+1
    adc.z deltaXx2+1
    sta.z error+1
  __b4:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1006
    inc.z y
    bne !+
    inc.z y+1
  !:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1007
    jsr DrawPixel_StandardBitmapMode
    jmp __b1
  __b3:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:999
    clc
    lda.z x
    adc.z xDirection
    sta.z x
    lda.z x+1
    adc.z xDirection+1
    sta.z x+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:1000
    clc
    lda.z error
    adc.z deltaXx2MinusDeltaYx2
    sta.z error
    lda.z error+1
    adc.z deltaXx2MinusDeltaYx2+1
    sta.z error+1
    jmp __b4
}
// Generalized Bresenham's line algorithm for Octant0: https://www.phatcode.net/res/224/files/html/ch35/35-03.html
// void DrawOctant0Line_StandardBitmapMode(__zp(8) unsigned int x0, __zp($a) unsigned int y0, __zp($24) int deltaX, __zp($22) int deltaY, __zp($31) int xDirection, __zp($10) char *bitmapDataPtr)
DrawOctant0Line_StandardBitmapMode: {
    .label __1 = $2a
    .label x0 = 8
    .label y0 = $a
    .label deltaX = $24
    .label deltaY = $22
    .label bitmapDataPtr = $10
    .label deltaYx2 = $22
    .label deltaYx2MinusDeltaXx2 = $2a
    .label error = $1d
    .label x = 8
    .label y = $a
    .label xDirection = $31
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:936
    asl.z deltaYx2
    rol.z deltaYx2+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:937
    lda.z deltaX
    asl
    sta.z __1
    lda.z deltaX+1
    rol
    sta.z __1+1
    lda.z deltaYx2
    sec
    sbc.z deltaYx2MinusDeltaXx2
    sta.z deltaYx2MinusDeltaXx2
    lda.z deltaYx2+1
    sbc.z deltaYx2MinusDeltaXx2+1
    sta.z deltaYx2MinusDeltaXx2+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:938
    lda.z deltaYx2
    sec
    sbc.z deltaX
    sta.z error
    lda.z deltaYx2+1
    sbc.z deltaX+1
    sta.z error+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:945
    jsr DrawPixel_StandardBitmapMode
  __b1:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:946
    lda.z deltaX+1
    bne !+
    lda.z deltaX
    beq !e+
    lsr
  !:
    bpl __b2
  !e:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:964
    rts
  __b2:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:948
    lda.z deltaX
    bne !+
    dec.z deltaX+1
  !:
    dec.z deltaX
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:952
    lda.z error+1
    bpl __b3
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:959
    clc
    lda.z error
    adc.z deltaYx2
    sta.z error
    lda.z error+1
    adc.z deltaYx2+1
    sta.z error+1
  __b4:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:961
    clc
    lda.z x
    adc.z xDirection
    sta.z x
    lda.z x+1
    adc.z xDirection+1
    sta.z x+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:962
    jsr DrawPixel_StandardBitmapMode
    jmp __b1
  __b3:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:954
    inc.z y
    bne !+
    inc.z y+1
  !:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:955
    clc
    lda.z error
    adc.z deltaYx2MinusDeltaXx2
    sta.z error
    lda.z error+1
    adc.z deltaYx2MinusDeltaXx2+1
    sta.z error+1
    jmp __b4
}
// void DrawCharAndSetForegroundColor_StandardCharacterMode(char ch, __zp($1f) char foregroundColor, __zp($1a) char x, __zp($19) char y, __zp($10) char *screenDataPtr)
DrawCharAndSetForegroundColor_StandardCharacterMode: {
    .label foregroundColor = $1f
    .label x = $1a
    .label y = $19
    .label screenDataPtr = $10
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:285
    ldy.z x
    ldx.z y
    jsr DrawChar_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:286
    ldx.z foregroundColor
    ldy.z x
    lda.z y
    jsr SetCharForegroundColor_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:290
    rts
}
// void DrawHorizontalLine_StandardCharacterMode(char ch, __zp($1f) char foregroundColor, __register(Y) char x0, __register(X) char x1, __zp($45) char y, __zp($10) char *screenDataPtr)
DrawHorizontalLine_StandardCharacterMode: {
    .label __2 = $12
    .label __9 = $12
    .label index = $12
    .label length = $3f
    .label foregroundColor = $1f
    .label y = $45
    .label screenDataPtr = $10
    .label x0 = $3f
    .label __10 = $14
    .label __11 = $12
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:510
    sty.z $ff
    cpx.z $ff
    bcs __b4
    stx.z x0
  __b1:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:524
    lda.z y
    sta.z __9
    lda #0
    sta.z __9+1
    lda.z __9
    asl
    sta.z __10
    lda.z __9+1
    rol
    sta.z __10+1
    asl.z __10
    rol.z __10+1
    clc
    lda.z __11
    adc.z __10
    sta.z __11
    lda.z __11+1
    adc.z __10+1
    sta.z __11+1
    asl.z __2
    rol.z __2+1
    asl.z __2
    rol.z __2+1
    asl.z __2
    rol.z __2+1
    lda.z x0
    clc
    adc.z index
    sta.z index
    bcc !+
    inc.z index+1
  !:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:525
    tya
    sec
    sbc.z length
    sta.z length
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:526
    lda.z screenDataPtr
    clc
    adc.z index
    sta.z memset.str
    lda.z screenDataPtr+1
    adc.z index+1
    sta.z memset.str+1
    lda.z length
    sta.z memset.num
    lda #0
    sta.z memset.num+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:526
    ldx #'a'
    jsr memset
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:527
    lda.z index
    clc
    adc #<COLORRAM
    sta.z memset.str
    lda.z index+1
    adc #>COLORRAM
    sta.z memset.str+1
    ldx.z foregroundColor
    lda.z length
    sta.z memset.num
    lda #0
    sta.z memset.num+1
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:527
    jsr memset
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:531
    rts
  __b4:
    sty.z x0
    txa
    tay
    jmp __b1
}
// void DrawVerticalLine_StandardCharacterMode(char ch, __zp($1f) char foregroundColor, __zp($1a) char x, __zp($36) char y0, __register(X) char y1, __zp($10) char *screenDataPtr)
DrawVerticalLine_StandardCharacterMode: {
    .label y = $19
    .label foregroundColor = $1f
    .label x = $1a
    .label y0 = $36
    .label screenDataPtr = $10
    .label y1 = $36
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:550
    cpx.z y0
    bcs __b6
    stx.z y
  __b3:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:564
    lda.z y1
    cmp.z y
    bcs __b4
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:571
    rts
  __b4:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:566
    jsr DrawCharAndSetForegroundColor_StandardCharacterMode
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:564
    inc.z y
    jmp __b3
  __b6:
    lda.z y0
    sta.z y
    stx.z y1
    jmp __b3
}
// Generalized Bresenham's line algorithm for Octant1: https://www.phatcode.net/res/224/files/html/ch35/35-03.html
// void DrawOctant1Line_StandardCharacterMode(char ch, __zp($1f) char foregroundColor, __zp($1a) char x0, __zp($19) char y0, __register(X) signed char deltaX, __zp($20) signed char deltaY, __zp($2d) signed char xDirection, __zp($10) char *screenDataPtr)
DrawOctant1Line_StandardCharacterMode: {
    .label foregroundColor = $1f
    .label x0 = $1a
    .label y0 = $19
    .label deltaY = $20
    .label screenDataPtr = $10
    .label deltaXx2 = $27
    .label deltaXx2MinusDeltaYx2 = $2c
    .label error = $18
    .label x = $1a
    .label y = $19
    .label xDirection = $2d
    .label deltaY_1 = $26
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:393
    txa
    asl
    sta.z deltaXx2
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:394
    lda.z deltaY
    asl
    eor #$ff
    sec
    adc.z deltaXx2
    sta.z deltaXx2MinusDeltaYx2
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:395
    lda.z deltaXx2
    sec
    sbc.z deltaY
    sta.z error
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:402
    jsr DrawCharAndSetForegroundColor_StandardCharacterMode
  __b1:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:403
    ldy.z deltaY
    dey
    sty.z deltaY_1
    lda.z deltaY
    cmp #0
    bne __b2
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:420
    rts
  __b2:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:408
    lda.z error
    cmp #0
    bpl __b3
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:415
    clc
    adc.z deltaXx2
    sta.z error
  __b4:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:417
    inc.z y
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:418
    jsr DrawCharAndSetForegroundColor_StandardCharacterMode
    lda.z deltaY_1
    sta.z deltaY
    jmp __b1
  __b3:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:410
    lda.z x
    clc
    adc.z xDirection
    sta.z x
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:411
    lda.z error
    clc
    adc.z deltaXx2MinusDeltaYx2
    sta.z error
    jmp __b4
}
// Generalized Bresenham's line algorithm for Octant0: https://www.phatcode.net/res/224/files/html/ch35/35-03.html
// void DrawOctant0Line_StandardCharacterMode(char ch, __zp($1f) char foregroundColor, __zp($1a) char x0, __zp($19) char y0, __zp($21) signed char deltaX, __register(X) signed char deltaY, __zp($2d) signed char xDirection, __zp($10) char *screenDataPtr)
DrawOctant0Line_StandardCharacterMode: {
    .label foregroundColor = $1f
    .label x0 = $1a
    .label y0 = $19
    .label deltaX = $21
    .label screenDataPtr = $10
    .label deltaYx2 = $2c
    .label deltaYx2MinusDeltaXx2 = $26
    .label error = $18
    .label x = $1a
    .label y = $19
    .label xDirection = $2d
    .label deltaX_1 = $27
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:345
    txa
    asl
    sta.z deltaYx2
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:346
    lda.z deltaX
    asl
    eor #$ff
    sec
    adc.z deltaYx2
    sta.z deltaYx2MinusDeltaXx2
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:347
    lda.z deltaYx2
    sec
    sbc.z deltaX
    sta.z error
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:354
    jsr DrawCharAndSetForegroundColor_StandardCharacterMode
  __b1:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:355
    ldy.z deltaX
    dey
    sty.z deltaX_1
    lda.z deltaX
    cmp #0
    bne __b2
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:372
    rts
  __b2:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:360
    lda.z error
    cmp #0
    bpl __b3
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:367
    clc
    adc.z deltaYx2
    sta.z error
  __b4:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:369
    lda.z x
    clc
    adc.z xDirection
    sta.z x
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:370
    jsr DrawCharAndSetForegroundColor_StandardCharacterMode
    lda.z deltaX_1
    sta.z deltaX
    jmp __b1
  __b3:
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:362
    inc.z y
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:363
    lda.z error
    clc
    adc.z deltaYx2MinusDeltaXx2
    sta.z error
    jmp __b4
}
// Copy block of memory (forwards)
// Copies the values of num bytes from the location pointed to by source directly to the memory block pointed to by destination.
// void * memcpy(__zp($a) void *destination, __zp(8) void *source, unsigned int num)
memcpy: {
    .label src_end = $e
    .label dst = $a
    .label src = 8
    .label source = 8
    .label destination = $a
    // /Users/warren/Compilers/6502/kickc/lib/string.c:11
    lda.z source
    clc
    adc #<$19*$28-$28
    sta.z src_end
    lda.z source+1
    adc #>$19*$28-$28
    sta.z src_end+1
  __b1:
    // /Users/warren/Compilers/6502/kickc/lib/string.c:12
    lda.z src+1
    cmp.z src_end+1
    bne __b2
    lda.z src
    cmp.z src_end
    bne __b2
    // /Users/warren/Compilers/6502/kickc/lib/string.c:14
    rts
  __b2:
    // /Users/warren/Compilers/6502/kickc/lib/string.c:12
    ldy #0
    lda (src),y
    sta (dst),y
    inc.z dst
    bne !+
    inc.z dst+1
  !:
    inc.z src
    bne !+
    inc.z src+1
  !:
    jmp __b1
}
// void DrawChar_StandardCharacterMode(char ch, __register(Y) char x, __register(X) char y, __zp($10) char *screenDataPtr)
DrawChar_StandardCharacterMode: {
    .const ch = 'a'
    .label __0 = 2
    .label __1 = 2
    .label __2 = 2
    .label screenDataPtr = $10
    .label __3 = 2
    .label __4 = 4
    .label __5 = 2
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:245
    txa
    sta.z __2
    lda #0
    sta.z __2+1
    lda.z __2
    asl
    sta.z __4
    lda.z __2+1
    rol
    sta.z __4+1
    asl.z __4
    rol.z __4+1
    clc
    lda.z __5
    adc.z __4
    sta.z __5
    lda.z __5+1
    adc.z __4+1
    sta.z __5+1
    asl.z __0
    rol.z __0+1
    asl.z __0
    rol.z __0+1
    asl.z __0
    rol.z __0+1
    tya
    clc
    adc.z __1
    sta.z __1
    bcc !+
    inc.z __1+1
  !:
    clc
    lda.z __3
    adc.z screenDataPtr
    sta.z __3
    lda.z __3+1
    adc.z screenDataPtr+1
    sta.z __3+1
    lda #ch
    ldy #0
    sta (__3),y
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:249
    rts
}
// void SetCharForegroundColor_StandardCharacterMode(__register(X) char foregroundColor, __register(Y) char x, __register(A) char y)
SetCharForegroundColor_StandardCharacterMode: {
    .label __0 = 2
    .label __1 = 2
    .label __2 = 2
    .label __3 = 2
    .label __4 = 4
    .label __5 = 2
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:263
    sta.z __2
    lda #0
    sta.z __2+1
    lda.z __2
    asl
    sta.z __4
    lda.z __2+1
    rol
    sta.z __4+1
    asl.z __4
    rol.z __4+1
    clc
    lda.z __5
    adc.z __4
    sta.z __5
    lda.z __5+1
    adc.z __4+1
    sta.z __5+1
    asl.z __0
    rol.z __0+1
    asl.z __0
    rol.z __0+1
    asl.z __0
    rol.z __0+1
    tya
    clc
    adc.z __1
    sta.z __1
    bcc !+
    inc.z __1+1
  !:
    lda.z __3
    clc
    adc #<COLORRAM
    sta.z __3
    lda.z __3+1
    adc #>COLORRAM
    sta.z __3+1
    txa
    ldy #0
    sta (__3),y
    // /Users/warren/Checkouts/GitHub/c64-graphics-library/c64-graphics.c:267
    rts
}
