#pragma once

#include "rectz.h"
#include "dh.tracing.h"
#include "dh.tracing.defs.h"
#include "err.printer.h"
#include "rez/resource.h"
#include <atluser.h>

struct NormWindow
{
    static constexpr DWORD   DefaultStyle = WS_OVERLAPPEDWINDOW | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    static constexpr DWORD DefaultExStyle = WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW;

    CIcon         hIcon;
    CIcon     hPrevIcon;
    CIcon  hPrevIconBig;
    HINSTANCE  hResInst = nullptr;
    DWORD   dwPrevStyle = 0;
    DWORD dwPrevExStyle = 0;
    DWORD       dwStyle = DefaultStyle;
    DWORD     dwExStyle = DefaultExStyle;

    void MakeItNorm(HWND hWnd);
};

inline void NormWindow::MakeItNorm(HWND hWnd)
{
    if (!hResInst) {
        hResInst = GetModuleHandleW(nullptr);
    }
    if (!hIcon) {
        hIcon = LoadIconW(hResInst, MAKEINTRESOURCEW(IDI_WTL_APP));
    }
    if (hIcon) {
           hPrevIcon = reinterpret_cast<HICON>(SendMessageW(hWnd, WM_SETICON, FALSE, reinterpret_cast<LPARAM>(hIcon.m_hIcon)));
        hPrevIconBig = reinterpret_cast<HICON>(SendMessageW(hWnd, WM_SETICON,  TRUE, reinterpret_cast<LPARAM>(hIcon.m_hIcon)));
    }
      dwPrevStyle = GetWindowLongW(hWnd, GWL_STYLE);
    dwPrevExStyle = GetWindowLongW(hWnd, GWL_EXSTYLE);
    if (0 == dwStyle) {
        dwStyle = NormWindow::DefaultStyle;
    }
    if (0 == dwExStyle) {
        dwExStyle = NormWindow::DefaultExStyle;
    }
    SetWindowLongW(hWnd, GWL_STYLE, dwStyle);
    SetWindowLongW(hWnd, GWL_EXSTYLE, dwExStyle);
    SetWindowPos(hWnd, HWND_TOP, -1, 0, -1, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

struct MoveToMonitor
{
    enum : DWORD { FirstNotPrimary = static_cast<DWORD>(-1) };

    HWND           hWnd = nullptr;
    DWORD  dwDesiredNum = FirstNotPrimary;
    unsigned rcHowToPut = PutAt::YCenter | PutAt::Right;
    CRect         rcWnd;

    bool Move(HWND hWnd, DWORD dwDesiredNum = FirstNotPrimary, unsigned rcHowToPut = PutAt::Center);

private:
    BOOL MoveAttempt(MONITORINFOEXW const& monInfo) const;
    static BOOL EnumeratorProc(HMONITOR hMon, HDC hDC, LPRECT lpRect, MoveToMonitor const* self);
};

inline BOOL MoveToMonitor::MoveAttempt(MONITORINFOEXW const& monInfo) const
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
        ATL::CStringW szDev(monInfo.szDevice);
        int pos = szDev.FindOneOf(L"0123456789");
        if (-1 == pos) {
            return TRUE; 
        }
        ATL::CStringW monNumStr = szDev.Mid(pos);
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
            DBGTPrint(LTH_DBG_ASSIST L" WARNING: window rect changed!\n");
        }
    }
    DBGTPrint(LTH_DBG_ASSIST L" w:%08x moved to '%s'\n", hWnd, monInfo.szDevice);
    return FALSE;
}

inline BOOL CALLBACK MoveToMonitor::EnumeratorProc(HMONITOR hMon, HDC hDC, LPRECT lpRect, MoveToMonitor const* self)
{
    MONITORINFOEXW monInfo;
    ZeroMemory(&monInfo, sizeof(monInfo));
    monInfo.cbSize = sizeof(monInfo);
    GetMonitorInfoW(hMon, &monInfo);
    //DBGTPrint(LTH_DBG_ASSIST L" Enum monitor: '%s'\n", monInfo.szDevice);
    return self->MoveAttempt(monInfo);
}

inline bool MoveToMonitor::Move(HWND hWnd, DWORD dwDesiredNum, unsigned rcHowToPut)
{
    this->hWnd = hWnd;
    this->dwDesiredNum = dwDesiredNum;
    this->rcHowToPut = rcHowToPut;
    GetWindowRect(hWnd, rcWnd);
    const BOOL rv = EnumDisplayMonitors(nullptr, nullptr,
                                        reinterpret_cast<MONITORENUMPROC>(EnumeratorProc),
                                        reinterpret_cast<LPARAM>(this));
    if (rv) {
        DBGTPrint(LTH_DBG_ASSIST L" Could not move w:%08x to desired monitor %d\n", hWnd, dwDesiredNum);
    }
    return FALSE == rv;
}