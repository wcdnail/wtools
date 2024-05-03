#pragma once

#include <wingdi.h>

typedef unsigned char Byte;
typedef Byte* Bytes;

bool PlazzmaInit(int cx, int cy);
void PlazzmaDestroy();
void PlazzmaCalc();
void PlazzmaPaletteRotation();
void PlazzmaDefaultPalette(PALETTEENTRY* palette, int count);
void PlazzmaBlender();
