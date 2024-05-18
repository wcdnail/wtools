#pragma once

#include <rectz.h>
#include <atluser.h>

struct NormWindow
{
    static constexpr DWORD   DefaultStyle = WS_OVERLAPPEDWINDOW | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    static constexpr DWORD DefaultExStyle = WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW;

    WTL::CIcon         hIcon;
    WTL::CIcon     hPrevIcon;
    WTL::CIcon  hPrevIconBig;
    HINSTANCE       hResInst = nullptr;
    DWORD        dwPrevStyle = 0;
    DWORD      dwPrevExStyle = 0;
    DWORD            dwStyle = DefaultStyle;
    DWORD          dwExStyle = DefaultExStyle;

    void MakeItNorm(HWND hWnd);
};

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

