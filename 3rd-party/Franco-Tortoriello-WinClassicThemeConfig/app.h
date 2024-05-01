#pragma once
#if !defined(APP_H)
#define APP_H

#include "config.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/* Program exit codes */
enum returnCode
{
    RETURN_NO_CHANGES,
    RETURN_CHANGES,
    RETURN_ERROR,
    RETURN_USAGE,
    RETURN_EXISTING_INSTANCE
};

#define DEFAULT_FONT_DPI 72

typedef struct tagPROPSHEET
{
    HINSTANCE hInstance;
    HWND hWnd;
    HANDLE heap;
    UINT dpi;
} PROPSHEET;

extern PROPSHEET g_propSheet;

#define Alloc(flags, size) \
    HeapAlloc(g_propSheet.heap, flags, size)

#define ReAlloc(flags, mem, size) \
    HeapReAlloc(g_propSheet.heap, flags, mem, size)

#define Free(pMem) \
    HeapFree(g_propSheet.heap, 0, pMem)

#define ScaleForDpi(n) \
    MulDiv(n, g_propSheet.dpi, USER_DEFAULT_SCREEN_DPI)

#define UnscaleForDpi(n) \
    MulDiv(n, USER_DEFAULT_SCREEN_DPI, g_propSheet.dpi)

#define FontLogToPt(logUnits) \
    -MulDiv(logUnits, DEFAULT_FONT_DPI, g_propSheet.dpi)

#define FontPtToLog(pt) \
    -MulDiv(pt, g_propSheet.dpi, DEFAULT_FONT_DPI)

#endif  /* !defined(APP_H) */
