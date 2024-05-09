#pragma once

#include "luicPageImpl.h"
#include <settings.h>
#include <gdiplusheaders.h>
#include <atlcomcli.h>
#include <vector>
#include <memory>

using IDesktopWallpaperPtr = ATL::CComPtr<IDesktopWallpaper>;
using         GdipImagePtr = std::unique_ptr<Gdiplus::Image>;
using         GdipImageVec = std::vector<GdipImagePtr>;

struct CPageBackground: CPageImpl
{
    ~CPageBackground() override;
    CPageBackground(std::wstring&& caption, Conf::Section const& parentSettings);

    IDesktopWallpaper* DesktopWallpaper() const;
    bool IsThemePreviewDrawWallpaper() const;

private:
    Conf::Section          m_Settings;
    IDesktopWallpaperPtr m_pWallpaper;
    bool              m_bShowInPrevew;
    GdipImageVec          m_Wallpaper;

    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
    HRESULT WallpaperInit();
    void WallpaperPaint(WTL::CDCHandle dc, CRect const& rc) const;
};

inline IDesktopWallpaper* CPageBackground::DesktopWallpaper() const { return m_pWallpaper; }
inline bool    CPageBackground::IsThemePreviewDrawWallpaper() const { return m_bShowInPrevew; } 
