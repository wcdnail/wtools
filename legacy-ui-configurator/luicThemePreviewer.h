#pragma once

#include "luicDrawings.h"
#include <atlwin.h>
#include <gdiplusheaders.h>
#include <memory>
#include <vector>

using GdipImagePtr = std::unique_ptr<Gdiplus::Image>;
using GdipImageVec = std::vector<GdipImagePtr>;

struct CThemePreviewer
{
    static ATOM Register(HRESULT& code);

    ~CThemePreviewer();
    CThemePreviewer();

    void SubclassIt(HWND hWnd);

private:
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

    static LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HRESULT InitWallpapers();

    static void CalcRects(DrawWindowArgs (&params)[WND_Count],
                          CRect const&               rcClient,
                          DRect&                       rcAux1,
                          DRect&                       rcAux2
    );
    void DrawDesktop(CDCHandle dc, CRect const& rc);
    void HighLightSeletced(CDCHandle dc, CRect const& rcClient);

    int OnCreate(LPCREATESTRUCT pCS);
    void OnPaint(CDCHandle dc);
    void OnLButton(UINT nFlags, CPoint point);
};

#if 0
using CThemePreviewerTraits = ATL::CWinTraits<WS_CHILD | WS_VISIBLE | WS_BORDER, 0>;

struct CThemePreviewer: ATL::CWindowImpl<CThemePreviewer, ATL::CWindow, CThemePreviewerTraits>
{
    DECLARE_WND_CLASS_EX(_T("CThemePreviewer"), CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS, COLOR_WINDOW)

    using Super = ATL::CWindowImpl<CThemePreviewer, ATL::CWindow, CThemePreviewerTraits>;

    static ATOM Register(HRESULT& code);

    ~CThemePreviewer() override;
    CThemePreviewer();

    HRESULT InitWallpapers();

private:
    enum WND_Index : int
    {
        WND_Invalid = -1,
        WND_Front = 0,
        WND_Back,
        WND_MsgBox,
        WND_Count
    };

    friend Super;

    GdipImageVec           m_Wallpaper;
    std::pair<int, int> m_SelectedRect;
    bool                 m_bUserSelect;
    WindowRects   m_WndRect[WND_Count];

    static void CalcRects(DrawWindowArgs (&params)[WND_Count],
                          CRect const&               rcClient,
                          DRect&                       rcAux1,
                          DRect&                       rcAux2
    );
    void DrawDesktop(CDCHandle dc, CRect const& rc);
    void HighLightSeletced(CDCHandle dc, CRect const& rcClient);

    int OnCreate(LPCREATESTRUCT pCS);
    void OnPaint(CDCHandle dc);
    void OnLButton(UINT nFlags, CPoint point);

    BEGIN_MSG_MAP_EX(PanelView)
        MSG_WM_CREATE(OnCreate) // Subclassed
        MSG_WM_PAINT(OnPaint)
        MSG_WM_LBUTTONDBLCLK(OnLButton)
    END_MSG_MAP()
};
#endif
