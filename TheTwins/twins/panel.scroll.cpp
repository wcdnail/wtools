#include "stdafx.h"
#if 0
#include "panel.scroll.h"
#include "theme.h"
#include <atlstr.h>
#include <windows.gdi.rects.h>

namespace Twins
{
    PanelScroller::PanelScroller()
        : CF::DoubleBuffered(Theme().panelTab.backColor[0], true) // CONF: panel tabs double buffering
        , pos_(0)
        , limit_(-1)
    {
    }

    PanelScroller::~PanelScroller()
    {
    }

    void PanelScroller::OnPaint(CDCHandle senderDc) const
    {
        WTL::CPaintDC paintDc(m_hWnd);
        CF::BufferedPaint bufferedPaint(paintDc, GetSecondDc(), IsDoubleBuffered(), m_hWnd);
        CDC& curDc = bufferedPaint.GetCurrentDc();

        CRect rc;
        GetClientRect(rc);
        curDc.FillSolidRect(rc, Theme().panelTab.backColor[0]);

        double sh = rc.Height() / (limit_ > 0 ? limit_ : 1);
        int y = -1 + (int)(pos_ * sh);
        int h = sh < 4. ? 4 : (int)sh + 1;
        CRect rcScroll(rc.left, y, rc.right, y + h);
        CF::GradRect(curDc, rcScroll, Theme().panelTab.backColor, GRADIENT_FILL_RECT_H);
        CF::FrameRect(curDc, rcScroll);
        
        CRect rcEdge = rc;
        --rcEdge.right;
        --rcEdge.bottom;
        CF::FrameRect(curDc, rcEdge);
    }

    void PanelScroller::SetPos(int pos, int limit)
    {
        pos_ = pos;
        limit_ = limit;
        ::InvalidateRect(m_hWnd, NULL, FALSE);
    }
}
#endif
