#pragma once

#include "luicPageImpl.h"
#include <gdiplusheaders.h>
#include <vector>
#include <memory>

using GdipImagePtr = std::unique_ptr<Gdiplus::Image>;
using GdipImageVec = std::vector<GdipImagePtr>;

struct CPageBackground: CPageImpl
{
    ~CPageBackground() override;
    CPageBackground(std::wstring&& caption);

private:
    GdipImageVec m_Wallpaper;

    BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam) override;
    HRESULT InitWallpapers();
    void WallpaperPaint(CDCHandle dc, CRect const& rc);
};
