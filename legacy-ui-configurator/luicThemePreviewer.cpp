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

// ##TODO: ������ �������� ����������� ������ ��� ������������� ATL/WTL
#if 0
CThemePreviewer::CThemePreviewer()
    :          Super{}
    ,    m_Wallpaper{}
    , m_SelectedRect{-1, -1}
    ,  m_bUserSelect{false}
{
    auto& classInfo = GetWndClassInfo();
    ATOM atom = classInfo.Register(&this->m_pfnSuperWindowProc);
    if (!atom) {
        const auto code = static_cast<HRESULT>(GetLastError());
        ReportError(Str::ElipsisW::Format(L"Register WCLASS '%s' failure!", classInfo.m_wc.lpszClassName), code, true, MB_ICONERROR);
    }
}
#endif

CThemePreviewer::CThemePreviewer()
    : DoubleBuffered{true}
    ,         m_hWnd{nullptr}
    ,    m_Wallpaper{}
    , m_SelectedRect{-1, -1}
    ,  m_bUserSelect{false}
{
}

void CThemePreviewer::SubclassIt(HWND hWnd)
{
    ATLASSUME(hWnd != nullptr);
    const auto CThemePreviewer_pUserData = reinterpret_cast<void*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    ATLASSUME(CThemePreviewer_pUserData == nullptr);
    SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    m_hWnd = hWnd;
    InitWallpapers();
}

LRESULT CThemePreviewer::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG_PTR tempLP = GetWindowLongPtrW(hWnd, GWLP_USERDATA);
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
    HighLightSeletced(dc, bufferedPaint.GetRect());
}

void CThemePreviewer::CalcRects(DrawWindowArgs (&params)[WND_Count],
                                CRect const&               rcClient,
                                DRect&                       rcAux1,
                                DRect&                       rcAux2
)
{
    const double   sx = 5.;
    const double   sy = 5.;
    DRect rcFull = FromCRect<double>(rcClient);
    rcFull.Shrink(sx, sy);

    CF::RectzAllocator<double> rcAlloc{ rcFull };
    const double fCX = rcFull.Width();
    const double fCY = rcFull.Height();

    rcAux1 = rcAlloc.Next(fCX / 3., fCY / 3., sx, sy);
    params[WND_Front].drect = rcAlloc.NextFromRight(fCX, fCY / 1.2, sx, sy);

    rcAlloc.Shift(0, -(params[WND_Front].drect.Height() - rcAux1.Height()));
    rcAux2 = rcAlloc.Next(fCX / 5., fCY / 2., sx, sy);
    params[WND_Back].drect = rcAlloc.Next(fCX, fCY, sx, sy);

    rcAlloc.Shift(0, -(params[WND_Back].drect.Height() - rcAux2.Height()));
    params[WND_MsgBox].drect = rcAlloc.Next(fCX / 5., fCY, sx, sy);

    params[WND_Front].drect.cy -= fCY / 6.;
    params[WND_Back].drect.cx -= fCX / 15.;

    params[WND_MsgBox].drect.y -= fCY / 3.5;
    params[WND_MsgBox].drect.cx += fCX / 18.;
    params[WND_MsgBox].drect.cy += fCY / 5.5;
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

const WinText MsgBoxText[] = {
    {  WT_Usual, L"Message Text" },
    {    WT_URL, L"https://www.hyperlink.my" },
    {    WT_URL, L"Button" },
};
} // namespace

void CThemePreviewer::DrawDesktop(CDCHandle dc, CRect const& rcClient)
{
    const auto& theme = CLUIApp::App()->CurrentTheme();
    const auto  hMenu = CLUIApp::App()->GetTestMenu();
    DrawWindowArgs params[WND_Count] = {
        {   m_WndRect[WND_Back], {}, L"Inactive window", 0, nullptr, -1, { Il_DL_Begin, _countof(Il_DL_Begin), 0 } },
        {  m_WndRect[WND_Front], {}, L"Active window", DC_ACTIVE, hMenu, 2, { Il_DL_Begin, _countof(Il_DL_Begin), 0 } },
        { m_WndRect[WND_MsgBox], {}, L"Tool window", DC_ACTIVE | DC_SMALLCAP, nullptr, -1, { MsgBoxText, _countof(MsgBoxText), 0 } },
    };
    DRect rcAux1;
    DRect rcAux2;
    CalcRects(params, rcClient, rcAux1, rcAux2);

    dc.FillSolidRect(rcClient, theme.GetColor(COLOR_BACKGROUND));

    if (CLUIApp::App()->ShowDesktopWallpaper() && (m_Wallpaper.size() > 0)) {
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
    drawings.DrawDesktopIcon(dc, ToCRect(rcAux1), L"Icon Text", true);
    for (auto& it : params) {
        drawings.CalcRects(ToCRect<double>(it.drect), it.captFlags, it.rects);
        drawings.DrawWindow(dc, it);
    }
    drawings.DrawToolTip(dc, m_WndRect[WND_MsgBox].rect[WR_Tooltip], L"TooTip Hint");
}

void CThemePreviewer::HighLightSeletced(CDCHandle dc, CRect const& rcClient)
{
    if (m_bUserSelect) {
        m_bUserSelect = false;
        const int wi = m_SelectedRect.first;
        const int ri = m_SelectedRect.second;
        if (wi < 0 || wi > WND_Count - 1) {
            return ;
        }
        if (ri < 0 || ri > WR_Count - 1) {
            return ;
        }
        CRect const& rc = m_WndRect[wi].rect[ri];
        dc.InvertRect(rc);
    }
}

void CThemePreviewer::OnLButton(UINT nFlags, CPoint point)
{
    for (int j = 0; j < WND_Count; j++) {
        WindowRects const& wr = m_WndRect[j];
        for (int i = 0; i < WR_Count; i++) {
            CRect const& rc = wr.rect[i];
            if (!rc.IsRectEmpty() && rc.PtInRect(point)) {
                m_SelectedRect = std::make_pair(j, i);
                m_bUserSelect = true;
                ::InvalidateRect(m_hWnd, nullptr, FALSE);
                return ;
            }
        }
    }
    m_SelectedRect = std::make_pair(-1, -1);
    ::InvalidateRect(m_hWnd, nullptr, FALSE);
}

