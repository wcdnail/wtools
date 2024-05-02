#pragma once

#include <atlwin.h>

using CThemePreviewerTraits = ATL::CWinTraits<WS_CHILD | WS_VISIBLE | WS_BORDER, 0>;

struct CThemePreviewer: ATL::CWindowImpl<CThemePreviewer, ATL::CWindow, CThemePreviewerTraits>
{
    DECLARE_WND_CLASS_EX(_T("CThemePreviewer"), CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS, COLOR_WINDOW)

    using Super = ATL::CWindowImpl<CThemePreviewer, ATL::CWindow, CThemePreviewerTraits>;

    ~CThemePreviewer() override;
    CThemePreviewer();

private:
    friend Super;

    BEGIN_MSG_MAP_EX(PanelView)
        //CHAIN_MSG_MAP_MEMBER(Search)
        //CHAIN_MSG_MAP_MEMBER(DragnDrop)
        //MSG_WM_CREATE(OnCreate)
        //MSG_WM_DESTROY(OnDestroy)
        //MSG_WM_ERASEBKGND(OnEraseBkgnd)
        //MSG_WM_PAINT(OnPaint)
        //MSG_WM_KEYDOWN(OnKeyDown)
        //MSG_WM_LBUTTONDOWN(OnMouseDown)
        //MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
        //MSG_WM_RBUTTONUP(OnRButtonUp)
        //MSG_WM_MOUSEWHEEL(OnMouseWheel)
        //MSG_WM_SETFOCUS(OnSetFocus)
        //MSG_WM_VSCROLL(OnVScroll)
        //MSG_WM_DROPFILES(OnDropFiles)
        //MSG_WM_CONTEXTMENU(OnContextMenu)
        //CHAIN_MSG_MAP(SuperResizer)
    END_MSG_MAP()
};
