#pragma once

#include "rectz.h"
#include "dh.tracing.h"
#include "err.printer.h"
#include "rez/resource.h"

static inline void MakeDlgAppWnd(HWND hWnd)
{
    HINSTANCE hResInst = GetModuleHandleW(nullptr);

    HICON hIcon = LoadIconW(hResInst, MAKEINTRESOURCEW(IDI_WTL_APP));
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

struct WndMonMover
{
    enum : DWORD { FirstNotPrimary = static_cast<DWORD>(-1) };

    HWND           hWnd = nullptr;
    DWORD  dwDesiredNum = FirstNotPrimary;
    unsigned rcHowToPut = PutAt::YCenter | PutAt::Right;
    CRect         rcWnd;

    BOOL MoveAttempt(MONITORINFOEXW const& monInfo) const;
};

inline BOOL WndMonMover::MoveAttempt(MONITORINFOEXW const& monInfo) const
{
    if (!monInfo.szDevice) {
        return TRUE; 
    }
    if (FirstNotPrimary == dwDesiredNum) {
        if (0 != (monInfo.dwFlags & MONITORINFOF_PRIMARY)) {
            return TRUE;
        }
    }
    else {
        CStringW szDev(monInfo.szDevice);
        int pos = szDev.FindOneOf(L"0123456789");
        if (-1 == pos) {
            return TRUE; 
        }
        CStringW monNumStr = szDev.Mid(pos);
        if (monNumStr.IsEmpty()) {
            return TRUE; 
        }
        int monNum = _wtoi(monNumStr.GetString());
        if (monNum != dwDesiredNum) {
            return TRUE; 
        }
    }
    LRect rcMon = FromCRect<LONG>(monInfo.rcWork);
    LRect rcWin = FromCRect<LONG>(rcWnd);
    rcWin.PutInto(rcMon, rcHowToPut);
    if (0) { // TODO: method
        if (!MoveWindow(hWnd, rcWin.Left(), rcWin.Top(), rcWin.Width(), rcWin.Height(), TRUE)) {
            PrintLastError("Error in 'MoveWindow'");
            return TRUE;
        }
    }
    else {
        if (!SetWindowPos(hWnd, HWND_TOP, rcWin.Left(), rcWin.Top(), rcWin.Width(), rcWin.Height(), 0)) {
            PrintLastError("Error in 'SetWindowPos'");
            return TRUE;
        }
    }
    if (0) { // TODO: validate wnd rect after moving
        CRect rcNew;
        GetWindowRect(hWnd, rcNew);
        if (rcWin != FromCRect<LONG>(rcNew)) {
            DebugThreadPrintfc(DH::Category::Module(), L"WARNING: window rect changed!\n");
        }
    }
    DebugThreadPrintfc(DH::Category::Module(), L"WND %08x moved to '%s'\n", hWnd, monInfo.szDevice);
    return FALSE;
}

static inline BOOL CALLBACK EnumPlaceWndToMonitor(HMONITOR hMon, HDC hDC, LPRECT lpRect, WndMonMover const& mover)
{
    MONITORINFOEXW monInfo;
    ZeroMemory(&monInfo, sizeof(monInfo));
    monInfo.cbSize = sizeof(monInfo);
    GetMonitorInfoW(hMon, &monInfo);
    //DebugThreadPrintfc(DH::Category::Module(), L"Enum monitor: '%s'\n", monInfo.szDevice);
    return mover.MoveAttempt(monInfo);
}

static inline bool PlaceWndToMonitor(HWND hWnd, DWORD dwDesiredNum, unsigned rcHowToPut)
{
#if 0
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
#endif
    WndMonMover mover;
    mover.hWnd = hWnd;
    mover.dwDesiredNum = dwDesiredNum;
    mover.rcHowToPut = rcHowToPut;
    GetWindowRect(hWnd, mover.rcWnd);
    const BOOL rv = EnumDisplayMonitors(nullptr, nullptr,
                                        reinterpret_cast<MONITORENUMPROC>(EnumPlaceWndToMonitor),
                                        reinterpret_cast<LPARAM>(&mover));
    return FALSE != rv;
}