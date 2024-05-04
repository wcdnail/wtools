#include "stdafx.h"
#include "luicThemePreviewer.h"
#include "luicMain.h"
#include "dh.tracing.h"
#include "dh.tracing.defs.h"
#include "rect.alloc.h"
#include "string.utils.format.h"
#include "windows.uses.gdi+.h"
#include "rect.gdi+.h"
#include "rect.putinto.h"

#include <random>
    //std::random_device rd;
    //std::uniform_int_distribution<int> dist(0, static_cast<int>(m_Wallpaper.size()) - 1);
    //std::default_random_engine gen(rd());
    //int wIndex = gen();

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
}

CThemePreviewer::~CThemePreviewer()
{
}

CThemePreviewer::CThemePreviewer()
    :       Super{}
    , m_Wallpaper{}
{
    auto& classInfo = GetWndClassInfo();
    ATOM atom = classInfo.Register(&this->m_pfnSuperWindowProc);
    if (!atom) {
        HRESULT code = static_cast<HRESULT>(GetLastError());
        ReportError(Str::ElipsisW::Format(L"Register WCLASS '%s' failure!", classInfo.m_wc.lpszClassName), code, true, MB_ICONERROR);
    }
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
    CDCHandle dc(dcPaint.m_hDC);
    // ##TODO: later via CreateCompatibleDC || WTL::CDoubleBufferWindowImpl

    CRect rcClient;
    GetClientRect(rcClient);
    DrawDesktop(dc, rcClient);
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
    drawings.DrawToolTip(dc, m_WndRect[WND_MsgBox].m_rcTooltip, L"TooTip Hint");
}
