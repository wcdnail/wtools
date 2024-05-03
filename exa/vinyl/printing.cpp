#include "StdAfx.h"
#include <ddraw.h>
#include <io.h>
#include <fcntl.h>
#include "gfx.h"

unsigned char defFont[4096];

typedef void (drawCharFunc)(int, int, unsigned char, unsigned);
drawCharFunc *drawChar = NULL;

#define CP866_to_CP1251 	if ( ((Ch>=192) && (Ch<=239)) ) Ch-=64;\
							if ( ((Ch>=240) && (Ch<=255)) ) Ch-=16;


void drawChar8(int x, int y, unsigned char Ch, unsigned col) {
	unsigned char *scr = (unsigned char *)ddsd.lpSurface;

	CP866_to_CP1251

	int w = Ch<<4, d = x+y*maxx;
	unsigned char c, k;

	for (int i=0; i<16; i++) {
		c = defFont[w+i];
		for (int j=0; j<8; j++) {
 			k = c << j;
			if (k & 0x80) scr[d+j] = (unsigned char) col;
		}
		d += maxx;
	}
}

void drawChar16(int x, int y, unsigned char Ch, unsigned col) {
	unsigned short *scr = (unsigned short *)ddsd.lpSurface;

	CP866_to_CP1251

	int w = Ch<<4, d = x + y*(maxx/2);
	unsigned char c, k;

	for (int i=0; i<16; i++) {
		c = defFont[w+i];
		for (int j=0; j<8; j++) {
 			k = c << j;
			if (k & 0x80) scr[d+j] = (unsigned short) col;
		}
		d += maxx/2;
	}
}

void drawChar24(int x, int y, unsigned char Ch, unsigned col) {
}

void drawChar32(int x, int y, unsigned char Ch, unsigned col) {
}


void drawString(int x, int y, char *string, unsigned col, LPDIRECTDRAWSURFACE ddsurf) {
	ddsdinit
	errc = ddsurf->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);	
	if (errc!=DD_OK) return;

	for (unsigned i=0; i<strlen(string); i++) 
		drawChar(x+i*8, y, string[i], col);

	ddsurf->Unlock(NULL);
}

void drawString(int x, int y, char *string, unsigned col) {
    if (NULL != drawChar)
	    for (unsigned i=0; i<strlen(string); i++) 
		    drawChar(x+i*8, y, string[i], col);
}

inline void chooseFunx() {
	switch (pixdepth) {
		case 15: 
		case 16: drawChar = drawChar16; break;
		case 24: drawChar = drawChar24; break;
		case 32: drawChar = drawChar32; break;
		default: drawChar = drawChar8;
	}
}

bool loadFont8x16(char *fnt_nme, unsigned char *buffer) {

	int inp = _open(fnt_nme, O_RDONLY);
	if (inp==-1) return false;

	int i = _read(inp, buffer, 4096);

	_close(inp);
	chooseFunx();
	return true;
}

bool loadFont8x16(char *fnt_nme) {

	int inp = _open(fnt_nme, O_RDONLY);
	if (inp==-1) return false;

	int i = _read(inp, defFont, 4096);

	_close(inp);
	chooseFunx();
	return true;
}
