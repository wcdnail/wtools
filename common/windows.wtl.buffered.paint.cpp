#include "stdafx.h"
#include "windows.wtl.buffered.paint.h"

namespace CF  // Common framework
{
    BufferedPaint::BufferedPaint(WTL::CDC& destDc, WTL::CDC& secondDc, bool doubleBuffering, HWND ownerWindow)
        :          destDc_(destDc)
        ,        secondDc_(secondDc)
        , doubleBuffering_(doubleBuffering)
        ,              rc_()
    {
        GetClientRect(ownerWindow, rc_);
        CreateIfNull(destDc, secondDc, rc_);
    }

    BufferedPaint::~BufferedPaint()
    {
        if (doubleBuffering_)
            destDc_.BitBlt(rc_.left, rc_.top, rc_.Width(), rc_.Height(), secondDc_, 0, 0, SRCCOPY);
    }

    void BufferedPaint::CreateIfNull(WTL::CDC& destDc, WTL::CDC& secondDc, CRect const& rc)
    {
        if (!secondDc.m_hDC) {
            WTL::CBitmap tempBm;
            if (tempBm.CreateCompatibleBitmap(destDc, rc.Width(), rc.Height())) {
                WTL::CDC tempDc;
                if (tempDc.CreateCompatibleDC(destDc)) {
                    secondDc.Attach(tempDc.Detach());
                    secondDc.SelectBitmap(tempBm);
                }
            }
        }
    }

    DoubleBuffered::DoubleBuffered(bool useSecondBuffer)
        : doubleBuffered_(useSecondBuffer)
        ,       secondDc_()
    {
    }

    DoubleBuffered::~DoubleBuffered()
    {
    }

    void DoubleBuffered::OnSize(UINT type, CSize size)
    {
        // ##TODO: Release second buffer
        if (secondDc_.m_hDC) {
            WTL::CBitmap toDel(secondDc_.GetCurrentBitmap());
            secondDc_.Attach(nullptr);
        }
        SetMsgHandled(FALSE);
    }
}

