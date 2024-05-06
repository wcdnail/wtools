#include "stdafx.h"
#include "luicThemePreviewer.h"
#include "luicMain.h"
#include "dh.tracing.h"
#include "dh.tracing.defs.h"
#include "rect.alloc.h"
#include "string.utils.format.h"
#include "windows.uses.gdi+.h"
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

    ATOM atom = RegisterClassExW(&wc);
    if (!atom) {
        code = static_cast<HRESULT>(GetLastError());
        ReportError(Str::ElipsisW::Format(L"Register WCLASS '%s' failure!", wc.lpszClassName), code, true, MB_ICONERROR);
    }
    return atom;
}

CThemePreviewer::~CThemePreviewer()
{
}

#if 0
CThemePreviewer::CThemePreviewer()
    :          Super{}
    ,    m_Wallpaper{}
    , m_prSelected{-1, -1}
    ,  m_bLBtnDown{false}
{
    HRESULT code;
    const ATOM atom = ATL::AtlModuleRegisterClassExW(nullptr, &GetWndClassInfo().m_wc);
    if (!atom) {
        code = static_cast<HRESULT>(GetLastError());
        ReportError(Str::ElipsisW::Format(L"Register WCLASS '%s' failure!", 
            GetWndClassInfo().m_wc.lpszClassName), code, true, MB_ICONERROR
        );
    }
    else { 
        if (!m_thunk.Init(nullptr, nullptr)) {
            SetLastError(ERROR_OUTOFMEMORY);
        }
        else {
            ATL::_AtlWinModule.AddCreateWndData(&m_thunk.cd, this);
        }
    }
}
#endif

CThemePreviewer::CThemePreviewer()
    :   ATL::CWindow{}
    , DoubleBuffered{true}
    ,       m_pTheme{nullptr}
    ,    m_Wallpaper{}
    , m_prSelected{-1, -1}
    ,  m_bLBtnDown{false}
{
}

void CThemePreviewer::SubclassIt(HWND hWnd)
{
    ATLASSUME(hWnd != nullptr);
    const auto CThemePreviewer_pUserData = reinterpret_cast<void*>(::GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    ATLASSUME(CThemePreviewer_pUserData == nullptr);
    ::SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    m_hWnd = hWnd;
    InitWallpapers();
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
            auto*   self = reinterpret_cast<CThemePreviewer*>(tempLP);
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
            /// CHAIN_MSG_MAP(Cf::DoubleBuffered)
            LRESULT lRes = 0;
            if (self->ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lRes, 0)) {
                return lRes;
            }
        }
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

HRESULT CThemePreviewer::InitWallpapers()
{
    using   COMStrPtr = std::shared_ptr<void>;
    HRESULT      code = S_OK;
    auto*  pWallpaper = CLUIApp::App()->GetWallpaperMan();
    uint32_t mdpCount = 0;
    code = pWallpaper->GetMonitorDevicePathCount(&mdpCount);
    if (FAILED(code)) {
        SetMFStatus(code, L"GetMonitorDevicePathCount failure");
        return code;
    }
    GdipImageVec imageVec;
    for (uint32_t i = 0; i < mdpCount; i++) {
        PWSTR pDevicePath;
        code = pWallpaper->GetMonitorDevicePathAt(i, &pDevicePath);
        if (FAILED(code)) {
            SetMFStatus(code, L"GetMonitorDevicePathAt failure");
            break;
        }
        COMStrPtr pDevicePathPtr(pDevicePath, CoTaskMemFree);
        DebugThreadPrintf(LTH_DESK_WALLPPR L" DevicePth: '%s'\n", pDevicePath);
        PWSTR pWallpaperPath;
        code = pWallpaper->GetWallpaper(pDevicePath, &pWallpaperPath);
        if (FAILED(code)) {
            SetMFStatus(code, L"GetWallpaper failure");
            break;
        }
        COMStrPtr pWallpaperPathPtr(pWallpaperPath, CoTaskMemFree);
        DESKTOP_WALLPAPER_POSITION pos = DWPOS_CENTER;
        pWallpaper->GetPosition(&pos);
        DebugThreadPrintf(LTH_DESK_WALLPPR L" Wallpaper: '%s' %d\n", pWallpaperPath, pos);
        GdipImagePtr image{Gdiplus::Image::FromFile(pWallpaperPath, FALSE)};
        if (!image) {
            code = static_cast<HRESULT>(GetLastError());
            SetMFStatus(code, L"Gdiplus::Image::FromFile '%s' failure: image == NULL", pWallpaperPath);
            break;
        }
        auto status = image->GetLastStatus();
        if (Gdiplus::Status::Ok != status) {
            code = ERROR_ACCESS_DENIED;
            SetMFStatus(code, L"Gdiplus::Image::FromFile '%s' failure: %s", pWallpaperPath, GdiPlus::StatusString(status));
            break;
        }
        Gdiplus::RectF rcImage;
        Gdiplus::Unit units;
        image->GetBounds(&rcImage, &units);
        CRect rcWallpaper = FromRectF(rcImage);
        DebugThreadPrintf(LTH_DESK_WALLPPR L" Wallpaper: %d x %d [%d]\n", rcWallpaper.Width(), rcWallpaper.Height(), units);
        imageVec.emplace_back(std::move(image));
        break; // ##TODO: only one will be enough
    }
    m_Wallpaper.swap(imageVec);
    return m_Wallpaper.size() > 0 ? S_OK : code;
}

int CThemePreviewer::OnCreate(LPCREATESTRUCT pCS)
{
    if (m_Wallpaper.empty()) {
        InitWallpapers();
    }
    return 0;
}

void CThemePreviewer::OnPaint(CDCHandle dcParam)
{
    UNREFERENCED_PARAMETER(dcParam);
    CPaintDC dcPaint(m_hWnd);
    CF::BufferedPaint bufferedPaint(dcPaint, GetSecondDc(), IsDoubleBuffered(), m_hWnd);
    CDCHandle dc = bufferedPaint.GetCurrentDc();
    DrawDesktop(dc, bufferedPaint.GetRect());
}

void CThemePreviewer::CalcRects(CRect const& rcClient, CRect& rcFront, CRect& rcBack, CRect& rcMsg, CRect& rcIcon)
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
    {  WT_Usual, L"  ������ � ����� � ������ � ��������������� ��������, ������" },
    {  WT_Usual, L"��������. � ��������� �����  �������� �������  ������������" },
    {  WT_Usual, L"������, �������������  �� ������ �� ������� ���������� �����" },
    {  WT_Usual, L"�  ��� ��  ��������, �� � �� ��������  ������������.  ������" },
    {  WT_Usual, L"�������   �������������   �������  ������  �  ��� ���  ����" },
    {  WT_Usual, L"��������������    (�����������    ������,   ���������������" },
    {  WT_Usual, L"������, �������������, ����������� � �.�.)." },
    { WT_Select, L"  �������  �������  �  �������� ������ ��� ����� �� ��������" },
    { WT_Select, L"�������  �� ������� ��������� ������� ��������� � �������� �" },
    {  WT_Usual, L"����     �������������,    �����������������,    �����������" },
    {  WT_Usual, L"���������, � ��� ����� ������������� � ������� ��������, ��" },
    {  WT_Usual, L"��������� �������� � ��� ��������, � �������� �����," },
    {  WT_Usual, L"�������� �������� �������� � �.�. �� �������� ������� ������" },
    {  WT_Usual, L"������ ���� � ������� �������� ������ ������� ������" },
    {  WT_Usual, L"�������� ����������� ����������� � ������������ �" },
    {  WT_Usual, L"���������, ���������� � ����������." },
    {  WT_Usual, L"  �����������-��������� ������� ������� � �������� ������," },
    {  WT_Usual, L"�������������� ����� ���� ���� ������� ����������� �����," },
    {  WT_Usual, L"����������� �������, ��������� � ������� � ���� �����������" },
    {  WT_Usual, L"� ������������������ ����������� ������ ����������" },
    {  WT_Usual, L"�������������� ��������� � ������� ������ ��������." },
    {  WT_Usual, L"������, ������������� �� ������ ����������-" },
    {  WT_Usual, L"������������������� ��������� � ������� �������� ��������" },
    {  WT_Usual, L"� ��������������� ������������ ������ �������� ������������" },
    {  WT_Usual, L"��������, ����� � �������, ������ ��������� ��������������" },
    {  WT_Usual, L"�������." },
    {  WT_Usual, L"  # �������������� ������" },
    {  WT_Usual, L"  ������ �������������� � ����� �� ����������� ������ �" },
    {  WT_Usual, L"������� �������� ������������� ��������, ����������� ���" },
    {  WT_Usual, L"������������ ������� ��������� �������� ���� � ������" },
    {  WT_Usual, L"�����, ��� ����������� ������ � � ��������. � ���� ���������" },
    {  WT_Usual, L"������ �������������� ��������� � ����������� � �������" },
    {  WT_Usual, L"�������� ������������." },
};

const WinText MsgBox_Text[] = {
    {  WT_Usual, L"Message Text" },
    {    WT_URL, L"https://www.hyperlink.my" },
    {    WT_URL, L"Button" },
};
} // namespace

void CThemePreviewer::DrawDesktop(CDCHandle dc, CRect const& rcClient)
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
    CalcRects(rcClient, rcFront, rcBack, rcMsg, rcIcon);
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
    if (pApp->ShowDesktopWallpaper() && (m_Wallpaper.size() > 0)) {
        GdipImagePtr const& imPtr = m_Wallpaper[0];

        Gdiplus::RectF rcfImage;
        Gdiplus::Unit     units;
        imPtr->GetBounds(&rcfImage, &units);

        Gdiplus::ImageAttributes *attrs = nullptr;

        Gdiplus::Graphics graphics(dc);
        graphics.DrawImage(imPtr.get(),
            ToRect(rcClient),
            rcClient.left,    rcClient.top,
            rcClient.Width(), rcClient.Height(),
            units, attrs);
    }
    CDrawRoutine drawings(theme);
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
    static const int gs_nItemIndex[WND_Count][WR_Count] = {
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
        /* WR_Workspace    */ EN_DisabledItem,
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

void CThemePreviewer::NotifyParent()
{
    if (!m_pcbItem) {
        return ;
    }
    int nElementId = RectIndexToElementId();
    m_pcbItem->SetCurSel(nElementId);
}
