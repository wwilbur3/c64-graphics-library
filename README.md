# c64-graphics-library
Graphics library written in C to support the various VIC-II graphics modes of the Commodore 64.

Features:
- Screen fill
- Screen clear
- Set foreground color
- Set background color(s)
- Draw single point
- Clear single point
- Draw line
- Draw rectangle
- Draw circle
- Draw character screen region (copied from memory buffer)
- TBD: Filled polygons (triangle, rectangle, circle)

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
- TBD: SDCC
- TBD: Oscar64
