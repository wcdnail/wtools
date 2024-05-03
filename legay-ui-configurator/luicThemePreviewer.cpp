#include "stdafx.h"
#include "luicThemePreviewer.h"
#include "luicMain.h"
#include "rect.alloc.h"
#include "string.utils.format.h"

namespace 
{
    const WinText Il_DL_Begin[] = {
        { false, L"  Логика — наука о формах и закономерностях мышления, теория" },
        {  true, L"мышления. В настоящее время  термином «логика»  обозначаются" },
        { false, L"теории, различающиеся  не только по способу разработки одних" },
        { false, L"и  тех же  вопросов, но и по предмету  исследования.  Термин" },
        {  true, L"«логика»   употребляется   поэтому  обычно  с  тем или  иным" },
        {  true, L"прилагательным    («формальная    логика»,   «математическая" },
        { false, L"логика», «индуктивная», «модальная» и т.д.)." },
        { false, L"  Решение  вопроса  о  предмете логики как науки по существу" },
        { false, L"зависит  от решения основного вопроса философии и отражает в" },
        { false, L"себе     теоретические,    мировоззренческие,    философские" },
        { false, L"установки, в том числе представление о природе мышления, об" },
        { false, L"отношении мышления к его предмету, о движущих силах," },
        { false, L"стимулах развития мышления и т.д. Не случайно поэтому логика" },
        { false, L"всегда была и остаётся объектом острой идейной борьбы" },
        { false, L"основных философских направлений — материализма и" },
        { false, L"идеализма, диалектики и метафизики." },
        { false, L"  Марксистско-ленинское решение вопроса о предмете логики," },
        { false, L"представляющее собой итог всей истории философской мысли," },
        { false, L"разработано Марксом, Энгельсом и Лениным в ходе критической" },
        { false, L"и материалистической переработки высших достижений" },
        { false, L"предшествующей философии в области теории мышления." },
        { false, L"Логика, разработанная на основе диалектико-" },
        { false, L"материалистического понимания и решения проблемы мышления" },
        { false, L"и соответствующая современному уровню развития человеческой" },
        { false, L"культуры, науки и техники, обычно именуется диалектической" },
        { false, L"логикой." },
        {  true, L"  # Диалектическая логика" },
        { false, L"  Логика диалектическая — наука об объективных формах и" },
        { false, L"законах развития человеческого мышления, понимаемого как" },
        { false, L"исторический процесс отражения внешнего мира в знании" },
        { false, L"людей, как объективная истина в её развитии. В этом понимании" },
        { false, L"логика диалектическая совпадает с диалектикой и теорией" },
        { false, L"познания материализма." },
    };
}

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
        {   m_WndRect[WND_Back],  rcWin1, L"Inactive window", 0, nullptr, -1, { Il_DL_Begin, _countof(Il_DL_Begin), 0 } },
        {  m_WndRect[WND_Front],  rcWin2, L"Active window", DC_ACTIVE, hMenu, 2, { Il_DL_Begin, _countof(Il_DL_Begin), 0 } },
        { m_WndRect[WND_MsgBox], rcIcon3, L"Tool window", DC_ACTIVE | DC_SMALLCAP, nullptr, -1, { nullptr, 0, 0 } },
    };

    CDrawRoutine drawings(theme);
    for (auto& it : params) {
        drawings.CalcRects(ToCRect<double>(it.rcSrc), it.captFlags, it.rects);
        drawings.DrawWindow(dc, it);
    }
}
