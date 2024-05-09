#include "stdafx.h"
#include "luicBackground.h"
#include "resz/resource.h"
#include <rect.gdi+.h>
#include <dh.tracing.h>
#include <dh.tracing.defs.h>
#include <windows.uses.gdi+.h>

void SetMFStatus(int status, PCWSTR format, ...);

CPageBackground::~CPageBackground()
{
}

CPageBackground::CPageBackground(std::wstring&& caption, Conf::Section const& parentSettings)
    :       CPageImpl{IDD_PAGE_BACKGROUND, std::move(caption)}
    ,      m_Settings{parentSettings, m_Caption}
    ,    m_pWallpaper{}
    , m_bShowInPrevew{false}
    ,     m_Wallpaper{}
{
}

BOOL CPageBackground::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
    HRESULT code{S_OK};
    code = CoCreateInstance(CLSID_DesktopWallpaper,
                            nullptr,
                            CLSCTX_ALL,
                            IID_IDesktopWallpaper,
                            reinterpret_cast<void**>(&m_pWallpaper));
    if (FAILED(code)) {
        SetMFStatus(code, L"CoCreateInstance failure");
        return FALSE;
    }
    FromSettings(m_Settings, m_bShowInPrevew);

    WallpaperInit();
    DlgResizeAdd(IDC_TEST_STA2, DLSZ_SIZE_X | DLSZ_SIZE_Y);
    return CPageImpl::OnInitDialog(wndFocus, lInitParam);
}

HRESULT CPageBackground::WallpaperInit()
{
    using   COMStrPtr = std::shared_ptr<void>;
    HRESULT      code = S_OK;
    uint32_t mdpCount = 0;
    code = m_pWallpaper->GetMonitorDevicePathCount(&mdpCount);
    if (FAILED(code)) {
        SetMFStatus(code, L"GetMonitorDevicePathCount failure");
        return code;
    }
    GdipImageVec imageVec;
    for (uint32_t i = 0; i < mdpCount; i++) {
        PWSTR pDevicePath;
        code = m_pWallpaper->GetMonitorDevicePathAt(i, &pDevicePath);
        if (FAILED(code)) {
            SetMFStatus(code, L"GetMonitorDevicePathAt failure");
            break;
        }
        COMStrPtr pDevicePathPtr(pDevicePath, CoTaskMemFree);
        DBGTPrint(LTH_DESK_WALLPPR L" DevicePth: '%s'\n", pDevicePath);
        PWSTR pWallpaperPath;
        code = m_pWallpaper->GetWallpaper(pDevicePath, &pWallpaperPath);
        if (FAILED(code)) {
            SetMFStatus(code, L"GetWallpaper failure");
            break;
        }
        COMStrPtr pWallpaperPathPtr(pWallpaperPath, CoTaskMemFree);
        DESKTOP_WALLPAPER_POSITION pos = DWPOS_CENTER;
        m_pWallpaper->GetPosition(&pos);
        DBGTPrint(LTH_DESK_WALLPPR L" Wallpaper: '%s' %d\n", pWallpaperPath, pos);
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
        DBGTPrint(LTH_DESK_WALLPPR L" Wallpaper: %d x %d [%d]\n", rcWallpaper.Width(), rcWallpaper.Height(), units);
        imageVec.emplace_back(std::move(image));
        break; // ##TODO: only one will be enough
    }
    m_Wallpaper.swap(imageVec);
    return m_Wallpaper.size() > 0 ? S_OK : code;
}

void CPageBackground::WallpaperPaint(WTL::CDCHandle dc, CRect const& rc) const
{
    if (m_Wallpaper.size() < 1) {
        return ;
    }
    GdipImagePtr const& imPtr = m_Wallpaper[0];

    Gdiplus::RectF rcfImage;
    Gdiplus::Unit     units;
    imPtr->GetBounds(&rcfImage, &units);

    Gdiplus::ImageAttributes *attrs = nullptr;

    Gdiplus::Graphics graphics(dc);
    graphics.DrawImage(imPtr.get(),
        ToRect(rc),
        rc.left,    rc.top,
        rc.Width(), rc.Height(),
        units, attrs);
}
