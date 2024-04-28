#include "stdafx.h"

#if 0
#include "wtl.scrollbar.h"
#include "windows.gdi.rects.h"

CAdvScrollBar::CAdvScrollBar() 
    : Super()
    , MyBackColor(0x070707)
{
    MyScrollerColor[0] = 0x4f4f4f;
    MyScrollerColor[1] = 0x1f1f1f;
}

CAdvScrollBar::~CAdvScrollBar() 
{}

bool CAdvScrollBar::IsVertical() const
{
    return 0 != (GetWindowLong(GWL_STYLE) & SBS_VERT);
}

CRect CAdvScrollBar::GetArrowRect(CRect const& rc, bool up, bool isVert) const
{
    int xy = isVert ? rc.Width() : rc.Height();
    
    CRect result(0, 0, xy, xy);

    if (isVert)
        result.MoveToY(rc.top  + (up ? 0 : rc.Height() - xy));
    else
        result.MoveToX(rc.left + (up ? 0 : rc.Width() - xy));

    return result; 
}

CRect CAdvScrollBar::GetScrollAreaRect(CRect const& rc, bool isVert) const
{
    int xy = isVert ? rc.Width() : rc.Height();

    CRect result(rc);

    if (isVert)
    {
        result.top += xy;
        result.bottom -= xy;
    }
    else
    {
        result.left += xy;
        result.right -= xy;
    }

    return result; 
}

CRect CAdvScrollBar::GetScrollBarRect(CRect const& rc, bool isVert) const
{
    SCROLLINFO si = {0};
    si.cbSize = sizeof( si );
    si.fMask = SIF_ALL;
    GetScrollInfo(&si);

    const int diff = si.nPos - si.nMin;
    int maximum = si.nMax - si.nMin;

    if (!maximum) 
        maximum = 1;

    const int xy = isVert ? rc.Height() : rc.Width();
    const int pos = diff * xy / maximum;

// ##TODO: Calc size"))
    const int size = 16;

    CRect result(0, 0, size, size);
    
    if (isVert)
        result.MoveToY(rc.top + pos);
    else
        result.MoveToX(rc.left + pos);

    return result;
}

BOOL CAdvScrollBar::OnEraseBkgnd(CDCHandle dc)
{
    return 1;
}

void CAdvScrollBar::OnPaint(CDCHandle)
{
    CPaintDC paintDc(*this);

    CRect rc;
    GetClientRect(rc);

    CMemoryDC dc(paintDc, rc);
    dc.FillSolidRect(rc, MyBackColor);

    bool isVert = IsVertical();

    //CRect rcUarrow(GetArrowRect(rc, true, isVert));
    //CRect rcDarrow(GetArrowRect(rc, false, isVert));
    //CRect rcSarea(GetScrollAreaRect(rc, isVert));
    CRect rcScroll(GetScrollBarRect(rc, isVert));

    //HPEN lp = dc.SelectPen(MyArowPen);
    //dc.SelectBrush(MyArrowBrush);
    //dc.Rectangle(rcUarrow);
    //dc.Rectangle(rcDarrow);

    //HPEN lp = dc.SelectPen(MyScrollPen);

    //dc.SelectBrush(MyScrollBrush);
    //dc.Rectangle(rcScroll);
    
    Cf::GradRect(dc, rcScroll, MyScrollerColor);

    //dc.SelectPen(lp);
}

LRESULT CAdvScrollBar::OnSetScrollInfo(UINT message, WPARAM wParam, LPARAM lParam) 
{
    const BOOL fRedraw = (BOOL)wParam;

    LRESULT rv = DefWindowProc(message, (WPARAM)FALSE, lParam);

    if ( fRedraw )
        Invalidate();

    return rv;
}

LRESULT CAdvScrollBar::OnNcHitTest(UINT, WPARAM wParam, LPARAM lParam)
{
    return 0;
}

//void CAdvScrollBar::OnLButtonDown(UINT, CPoint pt) {}
//void CAdvScrollBar::OnMouseMove(UINT, CPoint pt) {}
//void CAdvScrollBar::OnLButtonUp(UINT, CPoint pt) {}
//void CAdvScrollBar::OnMouseLeave() {}
//LRESULT CAdvScrollBar::OnGetScrollInfo(UINT, WPARAM wParam, LPARAM lParam) { return 1; }

#endif
