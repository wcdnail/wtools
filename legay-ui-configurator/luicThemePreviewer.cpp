#include "stdafx.h"
#include "luicThemePreviewer.h"
#include "luicMain.h"
#include "dh.tracing.h"
#include "dh.tracing.defs.h"
#include "rect.alloc.h"
#include "string.utils.format.h"
#include "windows.uses.gdi+.h"

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
        {  WT_Usual, L"�������  �� ������� ��������� ������� ��������� � �������� �" },
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
        DebugThreadPrintf(LTH_DESK_WALLPPR L" Wallpaper: %fx%f %d\n", rcImage.Width, rcImage.Height, units);
        imageVec.emplace_back(std::move(image));
    }
    m_Wallpaper.swap(imageVec);
    return m_Wallpaper.size() > 0 ? S_OK : code;
}

int CThemePreviewer::OnCreate(LPCREATESTRUCT pCS)
{
    /* InitWallpapers(); */ 
    return 0;
}

void CThemePreviewer::OnPaint(CDCHandle dcParam)
{
    UNREFERENCED_PARAMETER(dcParam);

    const auto& theme = CLUIApp::App()->CurrentTheme();
    const auto  hMenu = CLUIApp::App()->GetTestMenu();
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
    const double fCX = rcFull.Width();
    const double fCY = rcFull.Height();
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
    rcIcon3.cx += fCX / 18.;
    rcIcon3.cy += fCY / 5.5;

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
