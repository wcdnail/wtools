#pragma once
#if !defined(UXTUNDOC_H)
#define UXTUNDOC_H

#include "config.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#if WINVER >= WINVER_XP

#define BGR2RGB(c) RGB(GetBValue(c), GetGValue(c), GetRValue(c))

_Success_(return)
BOOL LoadUxThemeFunctions(void);

void UnloadUxThemeFunctions(void);

void SetVisualStyle(
    _In_ const WCHAR *pszVisualStyleFile,
    _In_ const WCHAR *pszColor,
    _In_ const WCHAR *pszSize);

#if WINVER >= WINVER_8
COLORREF GetAccentColor(void);

void SetAccentColor(COLORREF color);
#endif

#endif  /* WINVER >= WINVER_XP */

#endif  /* !defined(UXTUNDOC_H) */
