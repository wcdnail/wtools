#include "stdafx.h"
#include "ut.wtl.control.h"

namespace Ut
{
    HLControl::HLControl()
        : Cf::DoubleBuffered(0xffffff, true)
        , onPaint_()
    {}

    HLControl::~HLControl()
    {}

    int HLControl::OnCreate(LPCREATESTRUCT)
    {
        return 0;
    }

    void HLControl::OnPaint(CDCHandle senderDc)
    {
        WTL::CPaintDC paintDc(m_hWnd);
        Cf::BufferedPaint bufferedPaint(paintDc, GetSecondDc(), IsDoubleBuffered(), m_hWnd);
        CDC& curDc = bufferedPaint.GetCurrentDc();
        if (!curDc.m_hDC)
        {
            ::InvalidateRect(m_hWnd, NULL, TRUE);
            return ;
        }

        CRect rc;
        GetClientRect(rc);
        curDc.FillSolidRect(rc, GetBackgroundColor());

        if (onPaint_)
            onPaint_(m_hWnd, curDc, rc);
    }
}
