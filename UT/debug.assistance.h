#pragma once

#include "err.printer.h"
#include "rez/resource.h"

static inline void MakeDlgAppWnd(HWND hWnd)
{
    HICON hIcon = LoadIconW(nullptr, MAKEINTRESOURCEW(IDI_WTL_APP));
    if (hIcon) {
        HICON    hPrevIcon = reinterpret_cast<HICON>(SendMessageW(hWnd, WM_SETICON, FALSE, reinterpret_cast<LPARAM>(hIcon)));
        HICON hPrevIconBig = reinterpret_cast<HICON>(SendMessageW(hWnd, WM_SETICON,  TRUE, reinterpret_cast<LPARAM>(hIcon)));
    }

    DWORD  style = 0; // GetWindowLongW(hWnd, GWL_STYLE);
    DWORD estyle = 0; // GetWindowLongW(hWnd, GWL_EXSTYLE);

    estyle |= 0
           | WS_EX_APPWINDOW
           | WS_EX_OVERLAPPEDWINDOW;

    style |= 0
          | WS_OVERLAPPEDWINDOW
          | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

    SetWindowLongW(hWnd, GWL_STYLE, style);
    SetWindowLongW(hWnd, GWL_EXSTYLE, estyle);

    SetWindowPos(hWnd, HWND_TOP, -1, 0, -1, 0, 
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

static inline bool PlaceWndToMonitor(HWND hWnd)
{
    WINDOWPLACEMENT wplc;
    ZeroMemory(&wplc, sizeof(wplc));
    wplc.length = sizeof(wplc);
    if (!GetWindowPlacement(hWnd, &wplc)) {
        PrintLastError("Error in 'GetWindowPlacement'");
        return false;
    }
    ;
    if (!SetWindowPlacement(hWnd, &wplc)) {
        PrintLastError("Error in 'SetWindowPlacement'");
        return false;
    }
    return true;
}