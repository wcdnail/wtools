#include "stdafx.h"
#include "gfx.h"
#include "ereport.h"
#include <ddraw.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>

static const double PI = 3.1415926535897932384626433832785;

LPDIRECTDRAW ddIface = NULL;
LPDIRECTDRAWSURFACE ddSurface = NULL;
int ddMaxX = 0, ddMaxY = 0, ddBpp = 0, ddStride = 0;
typedef HRESULT (WINAPI *PDirectDrawCreate)(GUID FAR*, LPDIRECTDRAW FAR*, IUnknown FAR*);
HMODULE gDDrawLib = NULL;
// EO2012 

bool gfxInit(HWND hWnd, int X, int Y, int Depth)
{
    PDirectDrawCreate pDirectDrawCreate = NULL;

    // 2012
    if (NULL == gDDrawLib)
    {
        gDDrawLib = ::LoadLibraryW(L"ddraw");
        elogMessage("Гружу ddraw", gDDrawLib ? 0 : ::GetLastError());
        pDirectDrawCreate = (PDirectDrawCreate)::GetProcAddress(gDDrawLib, "DirectDrawCreate");
        elogMessage("Беру адрес ddraw", pDirectDrawCreate ? 0 : ::GetLastError());
    }

    HRESULT errc = E_NOTIMPL;

    if (pDirectDrawCreate)
        errc = pDirectDrawCreate(NULL, &ddIface, NULL);
    // EO2012

    elogMessage("Создаю ddraw", errc);
    if (errc != DD_OK)
        return false;

    errc = ddIface->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX);
    elogMessage("Задаю cooperative level", errc);
    if (errc!=DD_OK)
        return false;

    errc = ddIface->SetDisplayMode(X, Y, Depth);
    elogMessage(errc, "Видео режим %dx%d @%d", X, Y, Depth);
    if (errc != DD_OK)
        return false;

    DDSURFACEDESC ddsd = {0};
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS;	
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    errc = ddIface->CreateSurface(&ddsd, &ddSurface, NULL);
    elogMessage("Создаю primary поверхность", errc);
    if (errc != DD_OK) 
        return false;

    DDSURFACEDESC ddsd2 = {0};
    ddsd2.dwSize = sizeof(ddsd2);
    ddsd2.dwFlags = DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;	

    errc = ddSurface->GetSurfaceDesc(&ddsd2);
    elogMessage(errc, "Параметры поверхности - ^%d %dx%d @%d"
        , ddsd2.lPitch
        , ddsd2.dwWidth, ddsd2.dwHeight
        , ddsd2.ddpfPixelFormat.dwRGBBitCount);

    ddMaxX = ddsd2.dwWidth;
    ddMaxY = ddsd2.dwHeight;
    ddBpp = ddsd2.ddpfPixelFormat.dwRGBBitCount;
    ddStride = ddsd2.lPitch;

    return true;
}

void gfxDone(void)
{ 
    ::SetLastError(0);

    if (ddIface != NULL) 
    {
        if (ddSurface != NULL) 
        {
            ddSurface->Release(); 
            ddSurface = NULL;
            elogMessage("Убиваю primary поверхность", ::GetLastError());
        }
        ddIface->Release(); 
        ddIface = NULL;
        elogMessage("Убиваю Директ Ха", ::GetLastError());
    }

    // 2012
    if (NULL != gDDrawLib)
    {
        ::FreeLibrary(gDDrawLib);
        gDDrawLib = NULL;
    }
    // EO2012
}

void gfxShowPalette(int h)
{
    DDSURFACEDESC ddsd = {0};
    ddsd.dwSize = sizeof(ddsd);
    if (DD_OK != ddSurface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL))
        return;

    unsigned char* screen = (unsigned char*)ddsd.lpSurface;
    int sx = ddMaxX / 2 - 128;
    int sy = (ddMaxY - h) / 2;
    for (int i=0; i<256; i++) 
        for (int j=0; j<h; j++)
            screen[((sy + j) * ddStride) + (sx + i)] = i;

    ddSurface->Unlock(NULL);
}

bool gfxLoadDosPalette(char const* filename, PALETTEENTRY* palette)
{
    int inp = _open(filename, O_RDONLY);
    elogMessage(errno, "гружу палитру [%s]", filename);
    if (inp==-1)
        return false;

    unsigned char pal[768] = {0};
    int i = _read(inp, &pal, 768);
    _close(inp);

    for (i=0; i<256; i++) 
    {
        palette[i].peRed   = pal[i*3]<<2;
        palette[i].peGreen = pal[i*3+1]<<2;
        palette[i].peBlue  = pal[i*3+2]<<2;
    }

    return true;
}

bool gfxSetPalette(PALETTEENTRY *palette, int count)
{
    LPDIRECTDRAWPALETTE pal = NULL;
    HRESULT hr = ddSurface->GetPalette(&pal);

    if (pal)
        hr = pal->SetEntries(0, 0, count, palette);

    return DD_OK == hr;
}

bool gfxCreatePalette(PALETTEENTRY *palette, int count)
{
    LPDIRECTDRAWPALETTE pal;
    HRESULT errc = ddIface->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256, palette, &pal, NULL);
    if (errc != DD_OK) 
    {
        elogMessage("IDirectDraw::CreatePalette", errc);
        return false;
    }

    errc = ddSurface->SetPalette(pal);
    if (errc != DD_OK) 
        elogMessage("IDirectDrawSurface::SetPalette", errc);

    return DD_OK == errc;
}

bool gfxNewPalette(PALETTEENTRY *palette, int count)
{
    LPDIRECTDRAWPALETTE pal = NULL;
    if (DD_OK == ddSurface->GetPalette(&pal))
    {
        pal->Release();
        pal = NULL;
    }

    return gfxCreatePalette(palette, count);
}

void gfxBlendColors(PALETTEENTRY* palette, int count) 
{
    static double br = 1.0 / 6.0 * PI;
    static double bg = 3.0 / 6.0 * PI;
    static double bb = 5.0 / 6.0 * PI;

    for (int i=0; i<count; i++)
    {
        double u = 2 * PI / count * i;

        #define mycol(X, Y) ((::cos((X)+(Y)) + 1.) * 127.)

        palette[i].peRed = (Byte)mycol(u, br);
        palette[i].peGreen = (Byte)mycol(u, bg);
        palette[i].peBlue = (Byte)mycol(u, bb);

        #undef mycol
    }

    br += .05;
    bg -= .05;
    bb += .1;
}

void gfxDrop(Byte const* source, int sx, int sy, int cx, int cy)
{
    DDSURFACEDESC ddsd = {0};
    ddsd.dwSize = sizeof(ddsd);
    if (DD_OK != ddSurface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL)) 
        return ;
    
    Bytes screen = (Bytes)ddsd.lpSurface;
    int right = sx + cx;
    int bottom = sy + cy;

    int sourcey = 0;
    for (int y = sy; y < bottom; y++)
    {
        int sourcex = 0;
        for (int x = sx; x < right; x++)
        {
            screen[y * ddStride + x] = source[sourcey * cx + sourcex];
            ++sourcex;
        }
        ++sourcey;
    }

    ddSurface->Unlock(NULL);
}

void gfxFitDrop(Byte const* source, int sx, int sy, int cx, int cy)
{
    DDSURFACEDESC ddsd = {0};
    ddsd.dwSize = sizeof(ddsd);
    if (DD_OK != ddSurface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL)) 
        return ;

    Bytes screen = (Bytes)ddsd.lpSurface;
    int right = sx + cx;
    int bottom = sy + cy;

    int sdy = ddMaxY / (cy ? cy : 1);
    int sdx = ddMaxX / (cx ? cx : 1);

    int dy = 0;
    for (int y = sy; y < bottom; y++)
    {
        int dx = 0;
        for (int x = sx; x < right; x++)
        {
            for (int ly = 0; ly < sdy; ly++)
                for (int lx = 0; lx < sdx; lx++)
                    screen[(dy + ly) * ddStride + dx + lx] = source[y * cx + x];

            dx += sdx;
        }
        dy += sdy;
    }

    ddSurface->Unlock(NULL);
}
