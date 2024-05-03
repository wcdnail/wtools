///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//    Итак поехали. Когда то давным давно в далекой галактике бушевали... ну да ладно
// кодато у меня был 80386DX и я сидел под 320х200х8 с появлением 80486DX перешел на
// 640х400х8, ну а когда появился пенёк начал писать под 640х480х16 и т.д., но под 
// дос'ом вывод в этих модах медленый. Поэнтому пришлось учить Директ Ха especialy 
// DirectDraw (и подобный шлак).
//    Под МастДаем конечно выводит махом, и полно всяких фентифлюшек, написанных за 
// тебя, чувак, добрыми дядьками из МелкоМягкого, но есть свои прибабахи. Вот чтобы
// плюхнуть одну точку хотябы в центр экрана, не достаточно просто подсчитать ее адресс
// и вывести в видео память (режим то защищенный), надо заблокировать поверхность, 
// получить ее адресс, плюхнуть точку, разблокировать поверхность и т.п. (короче полный 
// геморой). Но зато это того стоит - видео морду можно выбрать любую, это зависит от 
// твоей видюхи, чувак, если она у тебя слабая (у меня когдато была однометровая 
// S3 Trio), то легкого поноса не избежать. Плюс все прелести MastDainogo C++.
// Так что, как говорили предки, учи DX, WIN32_LEAN_AND_MEAN (MFC/VCL тоже МастДай), 
// и не желай R2D2 ближнего своего. 
//
//					Получай чувак gfx lib от МеНя!!! wcdNail, 2004
//
//						Пока ПсЁ. Пишите письма wcdnail@mail.ru


#include "stdafx.h"
#include "structs.h"
#include <ddraw.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>

LPDIRECTDRAW				ddraw			= NULL;
LPDIRECTDRAWSURFACE			ddfront			= NULL;
LPDIRECTDRAWSURFACE			ddback			= NULL;
LPDIRECTDRAWPALETTE			defPalette		= NULL;

DDSURFACEDESC				ddsd;
#define	ddsdinit		{ 	ZeroMemory(&ddsd, sizeof(&ddsd));\
    ddsd.dwSize = sizeof(ddsd); }
DDSCAPS						ddscaps;


HRESULT					errc = 0;
int						maxx, maxy, pixdepth, scrx;

#define checkb			if ((x<0)||(x>maxx)) return;\
    if ((y<0)||(y>maxy)) return;



void spix8(int x, int y, unsigned c) {
    checkb
        unsigned char *dst = (unsigned char *)ddsd.lpSurface;
    dst[x+y*maxx] = (unsigned char) c;
}

void spix16(int x, int y, unsigned c) {
    checkb
        unsigned short *dst = (unsigned short *)ddsd.lpSurface;
    dst[x+y*maxx/2] = (unsigned short) c;
}

void spix24(int x, int y, unsigned c) {
}

void spix32(int x, int y, unsigned c) {
}

spixfunc *spix;



void hline16(int y, int x0, int x1, unsigned c) {
    if ((x0<0)||(x0>maxx)) return;
    if ((x1<0)||(x1>maxx)) return;
    if ((y<0)||(y>maxy)) return;

    unsigned short *dst = (unsigned short *)ddsd.lpSurface;
    if (x1<x0) {
        int t = x0;
        x0 = x1;
        x1 = t;
    }
    int d = (maxx/2)*y;
    for (int i=x0; i<=x1; i++) dst[i+d] = (unsigned short) c;
}

hlinefunc *hline;

inline void chooseSpixx() {
    switch (pixdepth) {
    case 15: 
    case 16: 
        spix = spix16; 
        hline = hline16;
        break;
    case 24: 
        spix = spix24; 
        break;
    case 32: 
        spix = spix32; 
        break;
    default: 
        spix = spix8;
    }
}

typedef HRESULT (WINAPI *PDirectDrawCreate)(GUID FAR*, LPDIRECTDRAW FAR*, IUnknown FAR*);
static HMODULE ddrawdll = NULL;

HRESULT gfxInit(HWND hWnd, int X, int Y, int Depth) {

    PDirectDrawCreate pDirectDrawCreate = NULL;

    if (NULL == ddrawdll)
    {
        ddrawdll = ::LoadLibraryW(L"ddraw");
        pDirectDrawCreate = (PDirectDrawCreate)::GetProcAddress(ddrawdll, "DirectDrawCreate");
    }

    HRESULT errc = E_NOTIMPL;

    if (pDirectDrawCreate)
        errc = pDirectDrawCreate(NULL, &ddraw, NULL);

    if (errc!=DD_OK) return errc;

    errc = ddraw->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
    if (errc!=DD_OK) return errc;

    errc = ddraw->SetDisplayMode(X, Y, Depth);
    if (errc!=DD_OK) return errc;

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
    ddsd.dwBackBufferCount = 1;

    errc = ddraw->CreateSurface(&ddsd, &ddfront, NULL);
    if (errc!=DD_OK) return errc;

    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
    errc = ddfront->GetAttachedSurface(&ddscaps, &ddback);
    if (errc!=DD_OK) return errc;

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_ALL;
    ddfront->GetSurfaceDesc(&ddsd);

    maxx = ddsd.lPitch;
    scrx = ddsd.dwWidth;
    maxy = ddsd.dwHeight;
    pixdepth = ddsd.ddpfPixelFormat.dwRGBBitCount;

    chooseSpixx();

    return DD_OK;
}

void gfxDone(void) {
    if (defPalette != NULL) defPalette->Release();

    if(ddraw != NULL ) {

        if( ddfront != NULL ) {

            ddfront->Release(); 
            ddfront = NULL;

        }

        ddraw->Release(); 
        ddraw = NULL;

    }

    if (NULL != ddrawdll)
    {
        FreeLibrary(ddrawdll);
        ddrawdll = NULL;
    }
}

void retResultAnalizer(HWND hWnd)
{
    char *err_msg;

    switch (errc) {

    case DD_OK: return;

    case DDERR_GENERIC:
        err_msg = "Undefined error condition";
        break;

    case DDERR_OUTOFMEMORY:
        err_msg = "Аут оф мемори ;-)";
        break;

    case DDERR_DIRECTDRAWALREADYCREATED:
        err_msg = "Объект DirectDraw уже создан";
        break;

    case DDERR_INVALIDDIRECTDRAWGUID:
        err_msg = "Неправильный GUID";
        break;

    case DDERR_INVALIDPARAMS:
        err_msg = "Неверные аргументы вызова методов DirectDraw";
        break;

    case DDERR_NODIRECTDRAWHW:
        err_msg = "Невозможно создать объект DirectDraw";
        break;

    case DDERR_UNSUPPORTED:
        err_msg = "Unsupported метод";
        break;

    case DDERR_INVALIDOBJECT:
        err_msg = "Инвалид DirectDraw объект";
        break;

    case DDERR_EXCLUSIVEMODEALREADYSET:
        err_msg = "Эксклюзивный режим доступа уже установлен";
        break;

    case DDERR_HWNDALREADYSET:
        err_msg = "Cooperative-уровень уже установлен";
        break;

    case DDERR_HWNDSUBCLASSED:
        err_msg = "The window used by DirectDraw has been subclassed";
        break;

    case DDERR_INVALIDMODE:
        err_msg = "Инвалид видео-режим";
        break;

    case DDERR_LOCKEDSURFACES:
        err_msg = "Поверхность заблокированна";
        break;

    case DDERR_WASSTILLDRAWING:
        err_msg = "Все еще рисую ;-)";
        break;

    case DDERR_SURFACEBUSY:
        err_msg = "Поверхность занята";
        break;

    case DDERR_NOEXCLUSIVEMODE:
        err_msg = "Эксклюзивный режим required";
        break;

    case DDERR_OUTOFVIDEOMEMORY:
        err_msg = "Нехватает видео-памяти";
        break;

    case DDERR_NOCOOPERATIVELEVELSET:
        err_msg = "Не установлен cooperative level";
        break;

    case DDERR_INVALIDCAPS:
        err_msg = "Инвалид Caps биты";
        break;

    case DDERR_INVALIDPIXELFORMAT:
        err_msg = "Неправильный формат точки";
        break;

    case DDERR_NOALPHAHW:
        err_msg = "No alpha accleration hardware present or available";
        break;

    case DDERR_NOFLIPHW:
        err_msg = "Fliping не поддерживается";
        break;

    case DDERR_NOZBUFFERHW:
        err_msg = "No hardware support for zbuffer blitting";
        break;

    case DDERR_PRIMARYSURFACEALREADYEXISTS:
        err_msg = "Первичная поверхность уже есть";
        break;

    case DDERR_NOEMULATION:
        err_msg = "HEL не доступен";
        break;

    case DDERR_INCOMPATIBLEPRIMARY:
        err_msg = "Unable to match primary surface creation request with existing primary surface.";
        break;

    case DDERR_NOTFOUND:
        err_msg = "Requested item was not found";
        break;

    }

    MessageBox(hWnd, err_msg, "[wcd]MS WinDoWs MustDie!", MB_OK | MB_ICONEXCLAMATION);
}

///////////////////////////////////////////////////////////////////////////////////////
// функции копирования

void fillSurface(LPDIRECTDRAWSURFACE ddsurf, DWORD color) 
{
    HDC dc;

    ddsdinit

    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;

    ddsurf->GetSurfaceDesc(&ddsd);

    if ((ddsurf->GetDC(&dc))==DD_OK) {
        PatBlt(dc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, color);
        ddsurf->ReleaseDC(dc);
    }
}

void fillBuffer(unsigned char *dst, unsigned sz, DWORD color)
{
    if (pixdepth==32) 
        __asm {
            mov		edi, dst
            mov		eax, color
            mov		ecx, sz
            shr		ecx, 2
            rep		stosd
        }

    if (pixdepth==24) 
        __asm {
            mov		edi, dst
            mov		eax, sz
            mov		ecx, 3
            div		ecx
            mov		ecx, eax
            mov		eax, color
fillin24:
            stosd
            dec		edi
            loop	fillin24
    }
}

void bytes2screen(unsigned char *from, int count, LPDIRECTDRAWSURFACE ddsurf) {
    ddsdinit

    errc = ddsurf->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
    unsigned char *scr = (unsigned char *)ddsd.lpSurface;

    if (errc!=DD_OK) return;

    for (int i=0; i<count; i++)	scr[i]=from[i];

    ddsurf->Unlock(NULL);
}

///////////////////////////////////////////////////////////////////////////////////////
// функции misc назначения

void test256colors(int h, LPDIRECTDRAWSURFACE ddsurf) {
    ddsdinit

    errc = ddsurf->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
    if (errc!=DD_OK) return;

    unsigned char *scr = (unsigned char *)ddsd.lpSurface;

    int st_x = maxx/2 - 128, st_y = maxy/2 - h/2;

    for (int i=0; i<256; i++) 
        for (int j=0; j<h; j++)
            scr[(st_y+j)*maxx+(st_x+i)] = i;

    ddsurf->Unlock(NULL);
}

///////////////////////////////////////////////////////////////////////////////////////
// функции работы с палитрой ! bpp = 8 only !

bool loadPaletteFromFile(char *pal_nme, int flags) {
    int inp = _open(pal_nme, O_RDONLY);
    if (inp==-1) return false;

    unsigned char pal[768];
    PALETTEENTRY rgb[256];

    ZeroMemory(&rgb, sizeof(rgb));

    int i = _read(inp, &pal, 768);
    _close(inp);

    for (i=0; i<256; i++) {
        rgb[i].peRed = (flags == 1 ? pal[i*3]<<2 : pal[i*3]);
        rgb[i].peGreen = (flags == 1 ? pal[i*3+1]<<2 : pal[i*3+1]);
        rgb[i].peBlue = (flags == 1 ? pal[i*3+2]<<2 : pal[i*3+2]);
    }

    if (defPalette!=NULL) defPalette->Release();

    errc = ddraw->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256, rgb, &defPalette, NULL);
    if (errc!=DD_OK) return false;

    ddfront->SetPalette(defPalette);

    return true;
}

bool setPalette(PALETTEENTRY *Pal) {
    if (defPalette!=NULL) defPalette->Release();

    errc = ddraw->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256, Pal, &defPalette, NULL);
    if (errc!=DD_OK) return false;

    ddfront->SetPalette(defPalette);

    return true;
}

void setColor(unsigned X, unsigned char R, unsigned char G, unsigned char B) {
    if (!defPalette) return;
    PALETTEENTRY ent = { R, G, B, 0 };

    defPalette->SetEntries(0, X, 1, &ent);
}

void rotateColors() {
    static double r=1.0/6.0*PI, g=3.0/6.0*PI, b=5.0/6.0*PI;
    int i=0;
    double u;

#define mycol(u,a) (cos((u)+(a))+1)*127

    while (i<256) 	{
        u=2*PI/256*i;
        setColor(i, (unsigned char)mycol(u,r), (unsigned char)mycol(u,g), (unsigned char)mycol(u,b));
        i++;
    }

#undef mycol

    r+=0.05;
    g-=0.05;
    b+=0.1;
}


HRESULT locks(LPDIRECTDRAWSURFACE ddsurf) {
    ddsdinit
        return ddsurf->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
}

void unlocks(LPDIRECTDRAWSURFACE ddsurf) {
    ddsurf->Unlock(NULL);
}

//////////////////////////////////////////////////////////////////////////////////////////
//						 Брезенхэм														//
//////////////////////////////////////////////////////////////////////////////////////////

void bres_line(int x0, int y0, int x1, int y1, unsigned color) {
    int dx, two_dx, dy, two_dy;
    int err, eps, t;

    dx = abs(x1 - x0);
    dy = abs(y1 - y0);
    two_dx = 2*dx;
    two_dy = 2*dy;
    if (dx <= dy) {
        if (x1 < x0) {
            t = x0;
            x0 = x1;
            x1 = t;
            t = y0;
            y0 = y1;
            y1 = t;
        }
        if (y0 < y1) eps = 1; else eps = -1;
        err = -dy;
        while (abs(y1-y0) >= 2) {
            spix(x0, y0, color);
            spix(x1, y1, color);
            err += two_dx;
            if (err >= 0) {
                err -= two_dy;
                x0++;
                x1--;
            }
            y0 += eps;
            y1 -= eps;
        }
        if (y0 == y1) spix(x0, y0, color); else {
            spix(x0, y0, color);
            spix(x1, y1, color);
        }
    } else {
        if (y1 < y0) {
            t = x0;
            x0 = x1;
            x1 = t;
            t = y0;
            y0 = y1;
            y1 = t;
        }
        if (x0<x1) eps = 1; else eps = -1;
        err = -dx;
        while (abs(x1-x0) >= 2) {
            spix(x0, y0, color);
            spix(x1, y1, color);
            err += two_dy;
            if (err >= 0) {
                err -= two_dx;
                y0++;
                y1--;
            }
            x0 += eps;
            x1 -= eps;
        }
        if (x0 == x1) spix(x0, y0, color); else {
            spix(x0, y0, color);
            spix(x1, y1, color);
        }
    }
}
