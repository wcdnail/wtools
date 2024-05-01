/*
 * COPYRIGHT: See COPYING in the top level directory
 * PURPOSE:   Handle color chooser buttons
 *
 * PROGRAMMER: Franco Tortoriello (torto09@gmail.com)
 */

#include "pch.h"
#include "app.h"
#include "draw.h"

#include <commdlg.h>

static const TCHAR szCustomColorsKey[] = TEXT("Control Panel\\Appearance");
static const TCHAR szCustomColorsValue[] = TEXT("CustomColors");

void DrawColorButtonBorder(
    HWND hButton, _In_ RECT *lpRcBmp, HBITMAP hbmpColor, HBRUSH hBrush)
{
    HDC hdcColorBtn = GetDC(hButton);
    if (!hdcColorBtn)
        return;

    HDC hdcCompat = CreateCompatibleDC(hdcColorBtn);
    if (!hdcCompat)
    {
        ReleaseDC(hButton, hdcColorBtn);
        return;
    }

    HGDIOBJ hPreviousBitmap = SelectObject(hdcCompat, hbmpColor);
    if (hPreviousBitmap)
    {
        DrawBorder(hdcCompat, lpRcBmp, 1, hBrush);
        SelectObject(hdcCompat, hPreviousBitmap);
    }
    DeleteDC(hdcCompat);
    ReleaseDC(hButton, hdcColorBtn);

    SendMessage(
        hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbmpColor);
}

/* Create the basic bitmaps for the color chooser buttons, and draw a border */
_Success_(return != NULL)
HBITMAP CreateColorButtonBitmap(HWND hButton, _In_ RECT *lpRcBmp)
{
    HDC hdcColorBtn = GetDC(hButton);
    if (!hdcColorBtn)
        return NULL;

    /* Create the button image */
    HBITMAP hbmpColor = CreateCompatibleBitmap(
        hdcColorBtn, lpRcBmp->right, lpRcBmp->bottom);

    if (hbmpColor)
    {
        DrawColorButtonBorder(
            hButton, lpRcBmp, hbmpColor, GetSysColorBrush(COLOR_BTNTEXT));
    }

    ReleaseDC(hButton, hdcColorBtn);
    return hbmpColor;
}

/* Draw a color on a color button.
 */
void FillColorButton(
    HWND hButton, _In_ RECT *lpRcBmp, HBITMAP hbmpColor, HBRUSH hBrush)
{
    HDC hdcColorBtn = GetDC(hButton);
    if (!hdcColorBtn)
        return;

    HDC hdcCompat = CreateCompatibleDC(hdcColorBtn);
    if (!hdcCompat)
    {
        ReleaseDC(hButton, hdcColorBtn);
        return;
    }

    HGDIOBJ hPreviousBitmap = SelectObject(hdcCompat, hbmpColor);
    if (hPreviousBitmap)
    {
        FillRect(hdcCompat, lpRcBmp, hBrush);
        SelectObject(hdcCompat, hPreviousBitmap);
    }

    DeleteDC(hdcCompat);
    ReleaseDC(hButton, hdcColorBtn);

    SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbmpColor);
}

void DisableColorButton(HWND hButton)
{
    SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, 0L);
    EnableWindow(hButton, FALSE);
}

/* Select a color using a color chooser.
 * Returns TRUE if a color was chosen, or FALSE if the selection was canceled.
 */
BOOL ChooseCustomColor(_Inout_ COLORREF *crColor, HWND hwndOwner)
{
    COLORREF crCustom[16];

    /* Default to white */
    for (int iColor = 0; iColor < 16; iColor++)
        crCustom[iColor] = RGB(0xFF, 0xFF, 0xFF);

    /* Load custom colors from Registry */
    HKEY hKey;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, szCustomColorsKey, 0, NULL, 0,
        KEY_QUERY_VALUE | KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS)
    {
        DWORD cbData = sizeof(crCustom);
        RegQueryValueEx(hKey, szCustomColorsValue, NULL,
            NULL, (BYTE *)crCustom, &cbData);
    }
    else
        hKey = NULL;

    /* Prepare structure */
    CHOOSECOLOR cc;

    cc.lStructSize = sizeof(CHOOSECOLOR);
    cc.hwndOwner = hwndOwner;
    cc.hInstance = NULL;
    cc.rgbResult = *crColor;
    cc.lpCustColors = crCustom;
    cc.Flags = CC_SOLIDCOLOR | CC_FULLOPEN | CC_RGBINIT;
    cc.lCustData = 0;
    cc.lpfnHook = NULL;
    cc.lpTemplateName = NULL;

    BOOL chosen;

    /* Create the color chooser */
    if (ChooseColor(&cc))
    {
        chosen = TRUE;
        *crColor = cc.rgbResult;
    }
    else
        chosen = FALSE;

    /* Save custom colors to reg */
    if (hKey)
    {
        RegSetValueEx(hKey, szCustomColorsValue, 0, REG_BINARY,
            (BYTE *)crCustom, sizeof(crCustom));
        RegCloseKey(hKey);
    }

    return chosen;
}
