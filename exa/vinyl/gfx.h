#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(__WCD_GFX_LIB)
#define __WCD_GFX_LIB

#include <ddraw.h>
#include "structs.h"

extern LPDIRECTDRAW				ddraw;
extern LPDIRECTDRAWSURFACE		ddfront;
extern LPDIRECTDRAWSURFACE		ddback;
extern LPDIRECTDRAWPALETTE		defPalette;
extern DDSURFACEDESC			ddsd;

extern int		 				maxx, maxy, pixdepth, scrx;

extern HRESULT					errc;

extern HRESULT gfxInit(HWND, int, int, int);
extern void gfxDone(void);
extern void retResultAnalizer(HWND);

extern void fillSurface(LPDIRECTDRAWSURFACE, DWORD);
extern void fillBuffer(unsigned char *, unsigned, DWORD);
extern void bytes2screen(unsigned char *, int, LPDIRECTDRAWSURFACE);
extern void test256colors(int, LPDIRECTDRAWSURFACE);

extern bool loadPaletteFromFile(char *, int);
extern bool setPalette(PALETTEENTRY *);

extern void setColor(unsigned, unsigned char, unsigned char, unsigned char);
extern void rotateColors();

extern HRESULT locks(LPDIRECTDRAWSURFACE);
extern void unlocks(LPDIRECTDRAWSURFACE);

extern spixfunc *spix;
extern hlinefunc *hline;
extern void bres_line(int, int, int, int, unsigned);

#define	ddsdinit		{ 	ZeroMemory(&ddsd, sizeof(&ddsd));\
							ddsd.dwSize = sizeof(ddsd); }

#define PI 3.1415926535897932384626433832785

#endif // __WCD_GFX_LIB