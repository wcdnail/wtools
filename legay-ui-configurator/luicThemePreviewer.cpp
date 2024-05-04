#include "stdafx.h"
#include "luicThemePreviewer.h"
#include "luicMain.h"
#include "rect.alloc.h"
#include "string.utils.format.h"

namespace 
{
    const WinText Il_DL_Begin[] = {
        {  WT_Usual, L"  Логика — наука о формах и закономерностях мышления, теория" },
        {  WT_Usual, L"мышления. В настоящее время  термином «логика»  обозначаются" },
        {  WT_Usual, L"теории, различающиеся  не только по способу разработки одних" },
        {  WT_Usual, L"и  тех же  вопросов, но и по предмету  исследования.  Термин" },
        {  WT_Usual, L"«логика»   употребляется   поэтому  обычно  с  тем или  иным" },
        {  WT_Usual, L"прилагательным    («формальная    логика»,   «математическая" },
        {  WT_Usual, L"логика», «индуктивная», «модальная» и т.д.)." },
        { WT_Select, L"  Решение  вопроса  о  предмете логики как науки по существу" },
        {  WT_Usual, L"зависит  от решения основного вопроса философии и отражает в" },
        {  WT_Usual, L"себе     теоретические,    мировоззренческие,    философские" },
        {  WT_Usual, L"установки, в том числе представление о природе мышления, об" },
        {  WT_Usual, L"отношении мышления к его предмету, о движущих силах," },
        {  WT_Usual, L"стимулах развития мышления и т.д. Не случайно поэтому логика" },
        {  WT_Usual, L"всегда была и остаётся объектом острой идейной борьбы" },
        {  WT_Usual, L"основных философских направлений — материализма и" },
        {  WT_Usual, L"идеализма, диалектики и метафизики." },
        {  WT_Usual, L"  Марксистско-ленинское решение вопроса о предмете логики," },
        {  WT_Usual, L"представляющее собой итог всей истории философской мысли," },
        {  WT_Usual, L"разработано Марксом, Энгельсом и Лениным в ходе критической" },
        {  WT_Usual, L"и материалистической переработки высших достижений" },
        {  WT_Usual, L"предшествующей философии в области теории мышления." },
        {  WT_Usual, L"Логика, разработанная на основе диалектико-" },
        {  WT_Usual, L"материалистического понимания и решения проблемы мышления" },
        {  WT_Usual, L"и соответствующая современному уровню развития человеческой" },
        {  WT_Usual, L"культуры, науки и техники, обычно именуется диалектической" },
        {  WT_Usual, L"логикой." },
        {  WT_Usual, L"  # Диалектическая логика" },
        {  WT_Usual, L"  Логика диалектическая — наука об объективных формах и" },
        {  WT_Usual, L"законах развития человеческого мышления, понимаемого как" },
        {  WT_Usual, L"исторический процесс отражения внешнего мира в знании" },
        {  WT_Usual, L"людей, как объективная истина в её развитии. В этом понимании" },
        {  WT_Usual, L"логика диалектическая совпадает с диалектикой и теорией" },
        {  WT_Usual, L"познания материализма." },
    };

    const WinText MsgBoxText[] = {
        {  WT_Usual, L"Message Text" },
        {    WT_URL, L"https://www.hyperlink.my" },
        {    WT_URL, L"Button" },
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
    rcIcon3.cx += rcFull.Width() / 18.;
    rcIcon3.cy += rcFull.Height() / 5.5;

    dc.FillSolidRect(rcClient, theme.GetColor(COLOR_BACKGROUND));

    const DrawWindowArgs params[WND_Count] = {
        {   m_WndRect[WND_Back],  rcWin1, L"Inactive window", 0, nullptr, -1, { Il_DL_Begin, _countof(Il_DL_Begin), 0 } },
        {  m_WndRect[WND_Front],  rcWin2, L"Active window", DC_ACTIVE, hMenu, 2, { Il_DL_Begin, _countof(Il_DL_Begin), 0 } },
        { m_WndRect[WND_MsgBox], rcIcon3, L"Tool window", DC_ACTIVE | DC_SMALLCAP, nullptr, -1, { MsgBoxText, _countof(MsgBoxText), 0 } },
    };

    CDrawRoutine drawings(theme);
    for (auto& it : params) {
        drawings.CalcRects(ToCRect<double>(it.rcSrc), it.captFlags, it.rects);
        drawings.DrawWindow(dc, it);
    }
}
