#include "stdafx.h"
#include "luicThemePreviewer.h"
#include "luicMain.h"
#include "dh.tracing.h"
#include "dh.tracing.defs.h"
#include "rect.alloc.h"
#include "string.utils.format.h"
#include "rect.gdi+.h"

ATOM CThemePreviewer::Register(HRESULT& code)
{
    WNDCLASSEXW wc;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = CThemePreviewer::WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hIcon = nullptr;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(nullptr);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = L"CThemePreviewer";
    wc.hIconSm = nullptr;

    const ATOM atom = RegisterClassExW(&wc);
    if (!atom) {
        code = static_cast<HRESULT>(GetLastError());
        ReportError(Str::ElipsisW::Format(L"Register WCLASS '%s' failure!", wc.lpszClassName), code, true, MB_ICONERROR);
    }
    return atom;
}

CThemePreviewer::~CThemePreviewer()
{
}

CThemePreviewer::CThemePreviewer()
    :   ATL::CWindow{}
    ,       m_pTheme{nullptr}
    ,      m_pcbItem{nullptr}
    ,   m_prSelected{-1, -1}
    ,    m_bLBtnDown{false}
{
}

void CThemePreviewer::SubclassIt(HWND hWnd)
{
    ATLASSUME(hWnd != nullptr);
    const auto CThemePreviewer_pUserData = reinterpret_cast<void*>(::GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    ATLASSUME(CThemePreviewer_pUserData == nullptr);
    ::SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    m_hWnd = hWnd;
    ModifyStyleEx(0, WS_EX_CLIENTEDGE);
}

void CThemePreviewer::OnSelectTheme(CTheme* pTheme, WTL::CComboBox* pcbItem)
{
    m_pTheme = pTheme;
    m_pcbItem = pcbItem;
    InvalidateRect(nullptr, FALSE);
}

void CThemePreviewer::OnSelectItem(int nItem)
{
    if (EN_Desktop == nItem) {
        m_prSelected = std::make_pair(-1, -1);
    }
}

LRESULT CThemePreviewer::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (WM_NCDESTROY == uMsg) {
        LONG_PTR tempLP = ::GetWindowLongPtrW(hWnd, GWLP_USERDATA);
        if (tempLP) {
            reinterpret_cast<CThemePreviewer*>(tempLP)->m_hWnd = nullptr;
            ::SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
        }
    }
    else {
        LONG_PTR tempLP = ::GetWindowLongPtrW(hWnd, GWLP_USERDATA);
        if (tempLP) {
            auto* self = reinterpret_cast<CThemePreviewer*>(tempLP);
            switch (uMsg) {
            case WM_PAINT:{
                self->OnPaint({reinterpret_cast<HDC>(wParam)});
                return 0;
            }
            case WM_LBUTTONDOWN:
            case WM_LBUTTONDBLCLK:{
                self->OnLButton(static_cast<UINT>(wParam), ::CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
                return 0;
            }
            }
            ///// CHAIN_MSG_MAP(Cf::DoubleBuffered)
            //LRESULT lRes = 0;
            //if (self->ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lRes, 0)) {
            //    return lRes;
            //}
        }
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

int CThemePreviewer::OnCreate(LPCREATESTRUCT pCS)
{
    return 0;
}

void CThemePreviewer::OnPaint(WTL::CDCHandle dcParam)
{
    UNREFERENCED_PARAMETER(dcParam);
    const WTL::CPaintDC dcPaint{m_hWnd};
    const WTL::CDCHandle dc{dcPaint};
    CRect rc;
    GetClientRect(rc);
    DrawDesktop(dc, rc);
}

void CThemePreviewer::RectsBuild(CRect const& rcClient, CRect& rcFront, CRect& rcBack, CRect& rcMsg, CRect& rcIcon)
{
    const double   sx = 5.;
    const double   sy = 5.;
    DRect rcFull = FromCRect<double>(rcClient);
    rcFull.Shrink(sx, sy);
    const double fCX = rcFull.Width();
    const double fCY = rcFull.Height();

    CF::RectzAllocator<double> rcAlloc{ rcFull };
    DRect rcIcon1 = rcAlloc.Next(fCX / 3., fCY / 3., sx, sy);
    DRect  rcWin1 = rcAlloc.NextFromRight(fCX, fCY / 1.2, sx, sy);

    rcAlloc.Shift(0, -(rcWin1.Height() - rcIcon1.Height()));
    DRect rcIcon2 = rcAlloc.Next(fCX / 5., fCY / 2., sx, sy);
    DRect  rcWin2 = rcAlloc.Next(fCX, fCY, sx, sy);

    rcAlloc.Shift(0, -(rcWin2.Height() - rcIcon2.Height()));
    DRect rcIcon3 = rcAlloc.Next(fCX / 5., fCY, sx, sy);

    rcWin1.cy -= fCY / 6.;
    rcWin2.cx -= fCX / 15.;
    rcIcon3.y -= fCY / 3.5;
    rcIcon3.cx += fCX / 15.;
    rcIcon3.cy += fCY / 5.5;

    rcFront = ToCRect<double>(rcWin2);
    rcBack  = ToCRect<double>(rcWin1);
    rcMsg   = ToCRect<double>(rcIcon3);
    rcIcon  = ToCRect<double>(rcIcon1);
}

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
    { WT_Select, L"зависит  от решения основного вопроса философии и отражает в" },
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

const WinText MsgBox_Text[] = {
    {  WT_Usual, L"Message Text" },
    {    WT_URL, L"https://www.hyperlink.my" },
    {    WT_URL, L"Button" },
};
} // namespace

void CThemePreviewer::DrawDesktop(WTL::CDCHandle dc, CRect const& rcClient)
{
    struct DrawWindowRects
    {
        WindowRects& rcTarget;
        CRect const& rcSource;
    };
    if (!m_pTheme) {
        return;
    }
    auto const*  pApp = CLUIApp::App();
    const auto& theme = *m_pTheme;
    const auto  hMenu = pApp->GetTestMenu();
    CRect     rcFront;
    CRect      rcBack;
    CRect       rcMsg;
    CRect      rcIcon;
    RectsBuild(rcClient, rcFront, rcBack, rcMsg, rcIcon);
    const DrawWindowArgs wparam[WND_Count] = {
        { L"Inactive window",                       0,   hMenu, -1, { Il_DL_Begin, _countof(Il_DL_Begin), 0 } },
        {   L"Active window",               DC_ACTIVE,   hMenu,  2, { Il_DL_Begin, _countof(Il_DL_Begin), 0 } },
        {     L"Tool window", DC_ACTIVE | DC_SMALLCAP, nullptr, -1, { MsgBox_Text, _countof(MsgBox_Text), 0 } },
    };
    DrawWindowRects wrect[WND_Count] = {
        {   m_WndRect[WND_Back],  rcBack },
        {  m_WndRect[WND_Front], rcFront },
        { m_WndRect[WND_MsgBox],   rcMsg },
    };
    dc.FillSolidRect(rcClient, theme.GetColor(COLOR_BACKGROUND));

    if (pApp->IsThemePreviewDrawWallpaper()) {
        // TODO: borrow wallpaper drawer from CPageBackground
    }

    CDrawings drawings(theme);
    drawings.DrawDesktopIcon(dc, rcIcon, L"Icon Text", true);
    for (int i = 0; i < WND_Count; i++) {
        drawings.CalcRects(wrect[i].rcSource, wparam[i].captFlags, wrect[i].rcTarget);
        drawings.DrawWindow(dc, wparam[i], wrect[i].rcTarget);
    }
    drawings.DrawToolTip(dc, m_WndRect[WND_MsgBox][WR_Tooltip], L"TooTip Hint");
    auto rcSel = GetSeletcedRect();
    if (!rcSel.IsRectEmpty()) {
        if (1) { // ##TODO: configure selection type
            rcSel.DeflateRect(1, 1);
            dc.DrawFocusRect(rcSel);
        }
        else {
            dc.InvertRect(rcSel);
        }
    }
}

CRect CThemePreviewer::GetSeletcedRect()
{
    if (!m_bLBtnDown) {
        return {};
    }
    m_bLBtnDown = false;
    const int wi = m_prSelected.first;
    const int ri = m_prSelected.second;
    if (wi < 0 || wi > WND_Count - 1) {
        return {};
    }
    if (ri < 0 || ri > WR_Count - 1) {
        return {};
    }
    return m_WndRect[wi][ri];
}

void CThemePreviewer::SetSelectedRect(int wr, int ri)
{
    m_prSelected = std::make_pair(wr, ri);
    ::InvalidateRect(m_hWnd, nullptr, FALSE);
    NotifyParent();
}

void CThemePreviewer::OnLButton(UINT nFlags, CPoint point)
{
    m_bLBtnDown = true;
    for (int j = 0; j < WND_Count; j++) {
        for (int i = 0; i < WR_Count; i++) {
            CRect const& rcExamine = m_WndRect[j][i];
            if (!rcExamine.IsRectEmpty() && rcExamine.PtInRect(point)) {
                SetSelectedRect(j, i);
                return ;
            }
        }
    }
    SetSelectedRect(TI_Invalid, TI_Invalid);
}

int CThemePreviewer::RectIndexToElementId() const
{
    static constexpr int gs_nItemIndex[WND_Count][WR_Count] = {
        {                  // WND_MsgBox ------------------
        /* WR_Tooltip      */ EN_Tooltip,
        /* WR_Button       */ EN_3DObject,
        /* WR_Hyperlink    */ EN_Hyperlink,
        /* WR_Message      */ EN_MsgBox,
        /* WR_Scroll       */ EN_MsgBox,
        /* WR_WinText      */ EN_MsgBox,
        /* WR_Workspace    */ EN_MsgBox,
        /* WR_MenuSelected */ EN_SelectedItem,
        /* WR_MenuDisabled */ EN_DisabledItem,
        /* WR_MenuItem     */ EN_FlatmenuBar,
        /* WR_Menu         */ EN_Menu,
        /* WR_Caption      */ EN_SMCaption,
        /* WR_Frame        */ EN_PaddedBorder,
        /* WR_Border       */ EN_PaddedBorder,
        }, {               // WND_Front -------------------
        /* WR_Tooltip      */ EN_Tooltip,
        /* WR_Button       */ EN_3DObject,
        /* WR_Hyperlink    */ EN_Hyperlink,
        /* WR_Message      */ EN_MsgBox,
        /* WR_Scroll       */ EN_Scrollbar,
        /* WR_WinText      */ EN_Window,
        /* WR_Workspace    */ EN_AppBackground,
        /* WR_MenuSelected */ EN_SelectedItem,
        /* WR_MenuDisabled */ EN_DisabledItem,
        /* WR_MenuItem     */ EN_FlatmenuBar,
        /* WR_Menu         */ EN_Menu,
        /* WR_Caption      */ EN_ActiveCaption,
        /* WR_Frame        */ EN_ActiveBorder,
        /* WR_Border       */ EN_PaddedBorder,
        }, {               // WND_Back --------------------
        /* WR_Tooltip      */ EN_Tooltip,
        /* WR_Button       */ EN_3DObject,
        /* WR_Hyperlink    */ EN_Hyperlink,
        /* WR_Message      */ EN_MsgBox,
        /* WR_Scroll       */ EN_Scrollbar,
        /* WR_WinText      */ EN_DisabledItem,
        /* WR_Workspace    */ EN_AppBackground,
        /* WR_MenuSelected */ EN_InactiveCaption,
        /* WR_MenuDisabled */ EN_DisabledItem,
        /* WR_MenuItem     */ EN_FlatmenuBar,
        /* WR_Menu         */ EN_Menu,
        /* WR_Caption      */ EN_InactiveCaption,
        /* WR_Frame        */ EN_InactiveBorder,
        /* WR_Border       */ EN_PaddedBorder,
        }
    };
    const int wi = m_prSelected.first;
    const int ri = m_prSelected.second;
    if (wi < 0 || wi > WND_Count - 1) {
        return EN_Desktop;
    }
    if (ri < 0 || ri > WR_Count - 1) {
        return EN_Desktop;
    }
    return gs_nItemIndex[wi][ri];
}

void CThemePreviewer::NotifyParent() const
{
    if (!m_pcbItem) {
        return ;
    }
    const int nElementId = RectIndexToElementId();
    if (CB_ERR != m_pcbItem->SetCurSel(nElementId)) {
        const WORD id = m_pcbItem->GetDlgCtrlID();
        ::SendMessageW(GetParent(), WM_COMMAND, MAKEWPARAM(id, CBN_SELENDOK), reinterpret_cast<LPARAM>(m_pcbItem->m_hWnd));
    }
}
