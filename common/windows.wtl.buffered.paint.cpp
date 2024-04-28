#include "stdafx.h"
#include "windows.wtl.buffered.paint.h"

namespace Cf  // Common framework
{
    BufferedPaint::BufferedPaint(WTL::CDC& destDc, CDC& secondDc, bool doubleBuffering, HWND ownerWindow)
        : destDc_(destDc)
        , secondDc_(secondDc)
        , doubleBuffering_(doubleBuffering)
        , rc_()
    {
        ::GetClientRect(ownerWindow, rc_);
        CreateIfNull(destDc, secondDc, rc_);
    }

    BufferedPaint::~BufferedPaint()
    {
        if (doubleBuffering_)
            destDc_.BitBlt(rc_.left, rc_.top, rc_.Width(), rc_.Height(), secondDc_, 0, 0, SRCCOPY);
    }

    WTL::CDC& BufferedPaint::GetCurrentDc() { return (doubleBuffering_ ? secondDc_ : destDc_);}
    CRect const& BufferedPaint::GetRect() const { return rc_; }

    void BufferedPaint::CreateIfNull(WTL::CDC& destDc, WTL::CDC& secondDc, CRect const& rc)
    {
        if (!secondDc.m_hDC)
        {
            WTL::CBitmap tempBm;
            if (tempBm.CreateCompatibleBitmap(destDc, rc.Width(), rc.Height()))
            {
                WTL::CDC tempDc;
                if (tempDc.CreateCompatibleDC(destDc))
                {
                    secondDc.Attach(tempDc.Detach());
                    secondDc.SelectBitmap(tempBm);
                }
            }
        }
    }

    DoubleBuffered::DoubleBuffered(COLORREF backColor, bool useSecondBuffer)
        : backColor_(backColor)
        , doubleBuffered_(useSecondBuffer)
        , secondDc_()
    {}

    DoubleBuffered::~DoubleBuffered()
    {}

    void DoubleBuffered::SetBackgroundColor(COLORREF color)
    {
        backColor_ = color;
    }

    COLORREF DoubleBuffered::GetBackgroundColor() const
    {
        return backColor_;
    }

    CDC& DoubleBuffered::GetSecondDc() const
    {
        return secondDc_;
    }

    bool DoubleBuffered::IsDoubleBuffered() const
    {
        return doubleBuffered_;
    }

    BOOL DoubleBuffered::OnEraseBkgnd(CDCHandle dc) const
    {
        return TRUE;
    }

    void DoubleBuffered::OnSize(UINT type, CSize size)
    {
        // ##TODO: Release second buffer"))
        if (secondDc_.m_hDC)
        {
            WTL::CBitmap toDel(secondDc_.GetCurrentBitmap());
            secondDc_.Attach(NULL);
        }

        SetMsgHandled(FALSE);
    }
}

