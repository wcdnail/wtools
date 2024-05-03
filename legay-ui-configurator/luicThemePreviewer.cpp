#include "stdafx.h"
#include "luicThemePreviewer.h"
#include "luicMain.h"
#include "rect.alloc.h"
#include "string.utils.format.h"

CThemePreviewer::~CThemePreviewer()
{
}

CThemePreviewer::CThemePreviewer()
{
    auto& classInfo = GetWndClassInfo();
    ATOM atom = classInfo.Register(&this->m_pfnSuperWindowProc);
    if (!atom) {
        HRESULT code = static_cast<HRESULT>(GetLastError());
        ReportError(Str::ElipsisW::Format(L"Registering '%s' failure!", classInfo.m_wc.lpszClassName), code, true, MB_ICONERROR);
    }
}

void CThemePreviewer::OnPaint(CDCHandle dcParam)
{
    UNREFERENCED_PARAMETER(dcParam);

    const auto& theme = CLegacyUIConfigurator::App()->CurrentTheme();
    const auto  hMenu = CLegacyUIConfigurator::App()->GetTestMenu();
    const double   sx = 5.;
    const double   sy = 5.;

    CPaintDC dcPaint(m_hWnd);
    CDCHandle dc(dcPaint.m_hDC);
    // ##TODO: later via CreateCompatibleDC || WTL::CDoubleBufferWindowImpl

    CRect rcClient;
    GetClientRect(rcClient);

    DRect rcFull = FromCRect<double>(rcClient);
    rcFull.Shrink(sx, sy);

    CF::RectzAllocator<double> rcAlloc{ rcFull };
    DRect rcIcon1 = rcAlloc.Next(rcFull.Width() / 3., rcFull.Height() / 3., sx, sy);
    DRect  rcWin1 = rcAlloc.NextFromRight(rcFull.Width(), rcFull.Height() / 1.2, sx, sy);
    rcAlloc.Shift(0, -(rcWin1.Height() - rcIcon1.Height()));
    DRect rcIcon2 = rcAlloc.Next(rcFull.Width() / 5., rcFull.Height() / 2., sx, sy);
    DRect  rcWin2 = rcAlloc.Next(rcFull.Width(), rcFull.Height(), sx, sy);
    rcAlloc.Shift(0, -(rcWin2.Height() - rcIcon2.Height()));
    DRect rcIcon3 = rcAlloc.Next(rcFull.Width() / 5., rcFull.Height(), sx, sy);
    rcWin1.cy -= rcFull.Height() / 6.;
    rcWin2.cx -= rcFull.Width() / 15.;
    rcIcon3.y -= rcFull.Height() / 3.5;
    rcIcon3.cx += rcFull.Width() / 15.;
    rcIcon3.cy += rcFull.Height() / 5.5;

    dc.FillSolidRect(rcClient, theme.GetColor(COLOR_BACKGROUND));

    const DrawWindowArgs params[WND_Count] = {
        {   m_WndRect[WND_Back],  rcWin1, L"Inactive window", 0, nullptr, -1, L"" },
        {  m_WndRect[WND_Front],  rcWin2, L"Active window", DC_ACTIVE, hMenu, 2, L"" },
        { m_WndRect[WND_MsgBox], rcIcon3, L"Tool window", DC_ACTIVE | DC_SMALLCAP, nullptr, -1, L"" },
    };

    CDrawRoutine drawings(theme);
    for (auto& it : params) {
        it.rects.Calc(ToCRect<double>(it.rcSrc), it.captFlags, theme);
        drawings.DrawWindow(dc, it);
    }
}
