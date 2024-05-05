#pragma once

#include "luicDrawings.h"
#include "windows.wtl.buffered.paint.h"
#include <gdiplusheaders.h>
#include <memory>
#include <vector>

#if 0 // ##TODO: решить проблему регистрации класса при использовании ATL/WTL
#include <atlwin.h>
using CThemePreviewerTraits = ATL::CWinTraits<WS_CHILD | WS_VISIBLE | WS_BORDER, 0>;
struct CThemePreviewer: ATL::CWindowImpl<CThemePreviewer, ATL::CWindow, CThemePreviewerTraits>,
                        CF::DoubleBuffered
{
    DECLARE_WND_CLASS_EX(_T("CThemePreviewer"), CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS, COLOR_WINDOW)
    using Super = ATL::CWindowImpl<CThemePreviewer, ATL::CWindow, CThemePreviewerTraits>;
    static ATOM Register(HRESULT& code);
    ~CThemePreviewer() override;
    CThemePreviewer();
    BEGIN_MSG_MAP_EX(PanelView)
        MSG_WM_CREATE(OnCreate) // Subclassed
        MSG_WM_PAINT(OnPaint)
        MSG_WM_LBUTTONDBLCLK(OnLButton)
        CHAIN_MSG_MAP(Cf::DoubleBuffered)
    END_MSG_MAP()
};
#endif

using GdipImagePtr = std::unique_ptr<Gdiplus::Image>;
using GdipImageVec = std::vector<GdipImagePtr>;

struct CThemePreviewer: CF::DoubleBuffered
{
    static ATOM Register(HRESULT& code);

    ~CThemePreviewer() override;
    CThemePreviewer();

    void SubclassIt(HWND hWnd);

private:
    static LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    enum WND_Index : int
    {
        WND_Invalid = -1,
        WND_Front = 0,
        WND_Back,
        WND_MsgBox,
        WND_Count
    };

    HWND                        m_hWnd;
    GdipImageVec           m_Wallpaper;
    std::pair<int, int> m_SelectedRect;
    bool                 m_bUserSelect;
    WindowRects   m_WndRect[WND_Count];

    HRESULT InitWallpapers();
    static void CalcRects(CRect const& rcClient, DRect& rcFront, DRect& rcBack, DRect& rcMsg, DRect& rcIcon);
    void DrawDesktop(CDCHandle dc, CRect const& rc);
    CRect GetSeletcedRect();
    int OnCreate(LPCREATESTRUCT pCS);
    void OnPaint(CDCHandle dc);
    void OnLButton(UINT nFlags, CPoint point);
};
