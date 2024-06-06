#include "pch.hxx"
#include "debug.assistance.h"
#include "rez/resource.h"
#include "err.printer.h"
#include <dh.tracing.h>
#include <dh.tracing.defs.h>

void NormWindow::MakeItNorm(HWND hWnd)
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

BOOL MoveToMonitor::MoveAttempt(MONITORINFOEXW const& monInfo) const
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
        const ATL::CStringW szDev(monInfo.szDevice);
        const int pos = szDev.FindOneOf(L"0123456789");
        if (-1 == pos) {
            return TRUE; 
        }
        const ATL::CStringW monNumStr = szDev.Mid(pos);
        if (monNumStr.IsEmpty()) {
            return TRUE; 
        }
        const int monNum = _wtoi(monNumStr.GetString());
        if (monNum != static_cast<int>(dwDesiredNum)) {
            return TRUE; 
        }
    }
    const LRect rcMon = FromCRect<LONG>(monInfo.rcWork);
    LRect rcWin = FromCRect<LONG>(rcWnd);
    rcWin.PutInto(rcMon, rcHowToPut);
    if constexpr (false) { // TODO: method
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
    if constexpr (false) { // TODO: validate wnd rect after moving
        CRect rcNew;
        GetWindowRect(hWnd, rcNew);
        if (rcWin != FromCRect<LONG>(rcNew)) {
            DBGTPrint(LTH_DBG_ASSIST, L" WARNING: window rect changed!\n");
        }
    }
    DBGTPrint(LTH_DBG_ASSIST, L" w:%08x moved to '%s'\n", hWnd, monInfo.szDevice);
    return FALSE;
}

BOOL CALLBACK MoveToMonitor::EnumeratorProc(HMONITOR hMon, HDC hDC, LPRECT lpRect, MoveToMonitor const* self)
{
    UNREFERENCED_PARAMETER(hDC);
    UNREFERENCED_PARAMETER(lpRect);
    MONITORINFOEXW monInfo{};
    ZeroMemory(&monInfo, sizeof(monInfo));
    monInfo.cbSize = sizeof(monInfo);
    GetMonitorInfoW(hMon, &monInfo);
    //DBGTPrint(LTH_DBG_ASSIST L" Enum monitor: '%s'\n", monInfo.szDevice);
    return self->MoveAttempt(monInfo);
}

bool MoveToMonitor::Move(HWND hWndClient, DWORD dwNum, unsigned nPut)
{
    hWnd = hWndClient;
    dwDesiredNum = dwNum;
    rcHowToPut = nPut;
    GetWindowRect(hWnd, rcWnd);
    const BOOL rv = EnumDisplayMonitors(nullptr, nullptr,
                                        reinterpret_cast<MONITORENUMPROC>(EnumeratorProc),
                                        reinterpret_cast<LPARAM>(this));
    if (rv) {
        DBGTPrint(LTH_DBG_ASSIST, L" Could not move w:%08x to desired monitor %d\n", hWnd, dwDesiredNum);
    }
    return FALSE == rv;
}
