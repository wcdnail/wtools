#pragma once

#include <atlwin.h>

#include "luicTheme.h"

struct CWndFrameRects
{
    int m_BorderSize = { 0 };
    CRect m_rcBorder = {};
    CRect  m_rcFrame = {};
    CRect   m_rcCapt = {};

    void Calc(CRect const& rc, const CTheme& theme);
};

using CThemePreviewerTraits = ATL::CWinTraits<WS_CHILD | WS_VISIBLE | WS_BORDER, 0>;

struct CThemePreviewer: ATL::CWindowImpl<CThemePreviewer, ATL::CWindow, CThemePreviewerTraits>
{
    DECLARE_WND_CLASS_EX(_T("CThemePreviewer"), CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS, COLOR_WINDOW)

    using Super = ATL::CWindowImpl<CThemePreviewer, ATL::CWindow, CThemePreviewerTraits>;

    ~CThemePreviewer() override;
    CThemePreviewer();

private:
    friend Super;

    CWndFrameRects m_WndRect[3];

    void OnPaint(CDCHandle dc);

    BEGIN_MSG_MAP_EX(PanelView)
        MSG_WM_PAINT(OnPaint)
    END_MSG_MAP()
};
