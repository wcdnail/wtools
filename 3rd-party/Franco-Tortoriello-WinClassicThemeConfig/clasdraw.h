#pragma once
#if !defined(CLASDRAW_H)
#define CLASDRAW_H

#include "config.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/* DrawMenuBarTemp:
 * - Does not exist on 95, but it does on NT4
 * - Works between XP and 7, but does not allow temporarily changing the flat
 *   menu setting
 * - Exists but does not render anything on 8 and later
 * - Does not render the selected item properly on Wine (as of v8.0)
 */
#if defined(WITH_UNDOCUMENTED_API) && \
   !defined(WINVER_IS_95) && WINVER <= WINVER_7 && !defined(WINE) && \
    (WINVER <= WINVER_2K || !defined(WITH_CLASSIC_ADVANCED_SETTINGS))
#define WITH_SYSTEM_DrawMenuBarTemp
#endif

/* DrawCaptionTemp:
 * - Draws an incorrect background on the icon on certain cases when switching
 *   scheme previews
 * - Always renders an icon on Wine (as of v8.0)
 */
#if defined(WITH_UNDOCUMENTED_API) && !defined(WINE) && \
    !defined(WITH_NEW_PREVIEW)
#define WITH_SYSTEM_DrawCaptionTemp
#endif

void SetPreviewSettings(_In_ COLORREF *colors, _In_ HBRUSH *brushes
#if WINVER >= WINVER_XP && defined(WITH_CLASSIC_ADVANCED_SETTINGS)
    , BOOL bFlatMenus
#endif
);

#if !defined(WITH_UNDOCUMENTED_API)

_Success_(return)
BOOL MyDrawFrameControl(HDC hdc, _Inout_ RECT *pRect, UINT uType, UINT uState);

_Success_(return)
BOOL MyDrawEdge(HDC hdc, _Inout_ RECT *pRect, UINT edge, UINT uFlags);

#else

#define MyDrawFrameControl DrawFrameControl
#define MyDrawEdge DrawEdge

#endif

#if !defined(WITH_SYSTEM_DrawCaptionTemp)
_Success_(return)
BOOL DrawCaptionTemp(HWND hWnd, HDC hdc, _In_ const RECT *pRect,
    HFONT hFont, HICON hIcon, _In_ const TCHAR *str, UINT uFlags);
#endif

_Success_(return)
BOOL DrawMenuBarBackground(HDC hdc, RECT *pRect);

#if !defined(WITH_SYSTEM_DrawMenuBarTemp)
_Success_(return == 0)
int DrawMenuBarTemp(
    HWND hWnd, HDC hdc, _In_ const RECT *pRect, HMENU hMenu, HFONT hFont);
#endif

_Success_(return)
BOOL DrawBottomBorder(HDC hdc, _In_ const RECT *pRect);

_Success_(return)
BOOL DrawBorderedWindow(
    HDC hdc, _In_ const RECT *pRect, int borderSize, int iBorderColor);

_Success_(return)
BOOL DrawCaptionButtons(HDC hdc, _In_ const RECT *pRect,
    BOOL withMinMax, int buttonWidth, UINT uFlags);

_Success_(return)
BOOL DrawScrollbar(
    HDC hdc, _In_ const RECT *pRect, int buttonHeight
#if defined(WITH_NEW_PREVIEW)
    , HBRUSH hbrScrollBk, BOOL enabled
#endif
);

#endif  /* !defined(CLASDRAW_H) */
