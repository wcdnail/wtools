#pragma once
#if !defined(COLORBTN_H)
#define COLORBTN_H

#include "config.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

HBITMAP CreateColorButtonBitmap(HWND hButton, _In_ RECT *lpRcBmp);

void FillColorButton(
    HWND hButton, _In_ RECT *lpRcBmp, HBITMAP hbmpColor, HBRUSH hBrush);

void DrawColorButtonBorder(
    HWND hButton, _In_ RECT *lpRcBmp, HBITMAP hbmpColor, HBRUSH hBrush);

void DisableColorButton(HWND hButton);

BOOL ChooseCustomColor(_Inout_ COLORREF *crColor, HWND hwndOwner);

#endif  /* !defined(COLORBTN_H) */
