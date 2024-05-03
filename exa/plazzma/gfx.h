#pragma once

#include <ddraw.h>

extern LPDIRECTDRAW ddIface;
extern LPDIRECTDRAWSURFACE ddSurface;
extern int ddMaxX, ddMaxY, ddBpp, ddStride;

typedef unsigned char Byte;
typedef Byte* Bytes;

bool gfxInit(HWND, int, int, int);
void gfxDone(void);
void gfxShowPalette(int);
bool gfxCreatePalette(PALETTEENTRY *palette, int count);
bool gfxSetPalette(PALETTEENTRY *, int count);
bool gfxNewPalette(PALETTEENTRY *palette, int count);
bool gfxLoadDosPalette(char const*, PALETTEENTRY* palette);
void gfxBlendColors(PALETTEENTRY* palette, int count);
void gfxDrop(Byte const* source, int x, int y, int cx, int cy);
void gfxFitDrop(Byte const* source, int sx, int sy, int cx, int cy);
