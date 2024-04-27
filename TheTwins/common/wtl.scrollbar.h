#pragma once

#if 0
#include "wcdafx.api.h"
#include <atltypes.h>
#include <atlwin.h>
#include <atlctrls.h>
#include <atlcrack.h>
#include <atlgdi.h>

class CAdvScrollBar: public CWindowImpl<CAdvScrollBar, WTL::CScrollBar, ATL::CControlWinTraits>
{
private:
    typedef CWindowImpl<CAdvScrollBar, WTL::CScrollBar, ATL::CControlWinTraits> Super;

public:
	WCDAFX_API CAdvScrollBar();
	WCDAFX_API ~CAdvScrollBar();

    WCDAFX_API bool IsVertical() const;
	
private:
    friend class Super;

    COLORREF MyBackColor;
    COLORREF MyScrollerColor[2];

    CRect GetArrowRect(CRect const& rc, bool up, bool isVert) const;
    CRect GetScrollAreaRect(CRect const& rc, bool isVert) const;
    CRect GetScrollBarRect(CRect const& rc, bool isVert) const;

    BOOL OnEraseBkgnd(CDCHandle);
    void OnPaint(CDCHandle);
    LRESULT OnSetScrollInfo(UINT, WPARAM wParam, LPARAM lParam);
    LRESULT OnNcHitTest(UINT, WPARAM wParam, LPARAM lParam);

    DECLARE_WND_SUPERCLASS(_T("CAdvScrollBar"), Super::GetWndClassName())

    BEGIN_MSG_MAP_EX(CAdvScrollBar)
        MSG_WM_ERASEBKGND(OnEraseBkgnd)
        MSG_WM_PAINT(OnPaint)
        MESSAGE_HANDLER_EX(SBM_SETSCROLLINFO, OnSetScrollInfo)
        MESSAGE_HANDLER_EX(WM_NCHITTEST, OnNcHitTest)
    END_MSG_MAP()
};
#endif
