# c64-graphics-library
Graphics library written in C to support the various VIC-II graphics modes of the Commodore 64.

Features:
- Screen fill
- Screen clear
- Set foreground color
- Set background color(s)
- Draw single point/char
- Clear single point/char
- Draw line of points/chars
- Draw rectangle of points/chars
- Draw triangle of points/chars
- Draw circle of points/chars
- Draw character screen region (copied from memory buffer)
- TBD: Filled polygons (triangle, rectangle, circle)
- Create, color, flip, move, and animate single color (high resolution) sprites
- Create, color, flip, move, and animate multicolor sprites

Supports the following VIC-II graphic modes:
- Standard (High Resolution) Character Mode
- Multicolor Character Mode
- Extended (Background) Color Mode
- (Standard) Bitmap (High Resolution) Mode
- TBD: Multicolor Bitmap Mode

Build with:
- KICKC
- VBCC
- CC65
- Oscar64
- TBD: SDCC

No optimization has been done yet.
