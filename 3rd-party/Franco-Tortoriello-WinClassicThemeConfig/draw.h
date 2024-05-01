#pragma once
#if !defined(DRAW_H)
#define DRAW_H

#include "config.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

_Success_(return)
BOOL DrawBorder(
    HDC hdc, _In_ const RECT *pRect, int borderWidth, HBRUSH hBrush);

#endif  /* !defined(DRAW_H) */
