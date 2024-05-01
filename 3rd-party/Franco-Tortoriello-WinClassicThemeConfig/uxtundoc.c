/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   UxTheme undocumented functions
 *
 * PROGRAMMER: Franco Tortoriello (torto09@gmail.com)
 */

#include "pch.h"
#include "uxtundoc.h"

#if WINVER >= WINVER_XP

#if WINVER >= WINVER_8
typedef struct tagACCENTCOLORS {
    COLORREF color1;
    COLORREF color2;
} ACCENTCOLORS;
#endif

static HMODULE g_hUxThemeDll = NULL;

static HRESULT(WINAPI *SetSystemVisualStyle)
    (const WCHAR *pszVisualStyleFile,
    const WCHAR *pszColor, const WCHAR *pszSize,
    unsigned int dwFlags) = NULL;

#if WINVER >= WINVER_8
static ACCENTCOLORS g_colors;

static HRESULT(WINAPI *GetUserColorPreference)
    (ACCENTCOLORS *pPreference, BOOL fForceReload) = NULL;
static HRESULT(WINAPI *SetUserColorPreference)
    (const ACCENTCOLORS *pPreference, BOOL fForceCommit) = NULL;
#endif

_Success_(return)
BOOL LoadUxThemeFunctions(void)
{
    if (g_hUxThemeDll)
        return TRUE;  /* Already loaded */

    g_hUxThemeDll = LoadLibraryW(L"uxtheme.dll");
    if (!g_hUxThemeDll)
    {
        return FALSE;
    }

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

    *(FARPROC *)&SetSystemVisualStyle =
        GetProcAddress(g_hUxThemeDll, MAKEINTRESOURCEA(65));

#if WINVER >= WINVER_8
    *(FARPROC *)&GetUserColorPreference =
        GetProcAddress(g_hUxThemeDll, "GetUserColorPreference");

    *(FARPROC *)&SetUserColorPreference =
        GetProcAddress(g_hUxThemeDll, MAKEINTRESOURCEA(122));
#endif

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

    return TRUE;
}

void UnloadUxThemeFunctions(void)
{
    SetSystemVisualStyle = NULL;

#if WINVER >= WINVER_8
    GetUserColorPreference = NULL;
    SetUserColorPreference = NULL;
#endif

    if (g_hUxThemeDll)
    {
        FreeLibrary(g_hUxThemeDll);
        g_hUxThemeDll = NULL;
    }
}

void SetVisualStyle(
    _In_ const WCHAR *pszVisualStyleFile,
    _In_ const WCHAR *pszColor,
    _In_ const WCHAR *pszSize)
{
    if (!SetSystemVisualStyle)
        return;

    SetSystemVisualStyle(pszVisualStyleFile, pszColor, pszSize, 0);
}

#if WINVER >= WINVER_8
COLORREF GetAccentColor(void)
{
    if (!GetUserColorPreference)
        return CLR_INVALID;

    if (!SUCCEEDED(GetUserColorPreference(&g_colors, FALSE)))
        return CLR_INVALID;

    return g_colors.color2 & 0x00FFFFFF;
}

void SetAccentColor(COLORREF color)
{
    if (!SetUserColorPreference)
        return;

    g_colors.color2 = color;
    SetUserColorPreference(&g_colors, TRUE);
}
#endif

#endif  /* WINVER >= WINVER_XP */
