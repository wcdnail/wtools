#pragma once

#include "luicDrawings.h"
#include <atlwin.h>
#include <gdiplusheaders.h>
#include <memory>
#include <vector>

using GdipImagePtr = std::unique_ptr<Gdiplus::Image>;
using GdipImageVec = std::vector<GdipImagePtr>;

using CThemePreviewerTraits = ATL::CWinTraits<WS_CHILD | WS_VISIBLE | WS_BORDER, 0>;

struct CThemePreviewer: ATL::CWindowImpl<CThemePreviewer, ATL::CWindow, CThemePreviewerTraits>
{
    DECLARE_WND_CLASS_EX(_T("CThemePreviewer"), CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS, COLOR_WINDOW)

    using Super = ATL::CWindowImpl<CThemePreviewer, ATL::CWindow, CThemePreviewerTraits>;

    ~CThemePreviewer() override;
    CThemePreviewer();

    HRESULT InitWallpapers();

private:
    enum WND_Index : int
    {
        WND_Front = 0,
        WND_Back,
        WND_MsgBox,
        WND_Count
    };

    friend Super;

    GdipImageVec         m_Wallpaper;
    WindowRects m_WndRect[WND_Count];

    int OnCreate(LPCREATESTRUCT pCS);
    void OnPaint(CDCHandle dc);

    BEGIN_MSG_MAP_EX(PanelView)
      //MSG_WM_CREATE(OnCreate) // Subclassed
        MSG_WM_PAINT(OnPaint)
    END_MSG_MAP()
};
