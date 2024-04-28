#pragma once

#include "rect.screen.h"
#include <atltypes.h>

namespace Rc
{
    enum AlignConsts
    {
        Left        = 0x00,
        Top         = 0x00,
        XCenter     = 0x01,
        Right       = 0x02,
        YCenter     = 0x04,
        Bottom      = 0x08,
        Fill        = 0x10,
        Center      = XCenter | YCenter,
        AlignMask   = 0xff
    };

    struct Align
    {
        template <class T>
        Align(T x) : mask((AlignConsts)x) {}

        bool BitSetTo(AlignConsts val) { return (mask & val) != 0; }

    private:
        AlignConsts mask;
    };

    inline void PutInto(CRect const& external, CRect& rc, Align align)
    {
        if (align.BitSetTo(Fill))
        {
            rc = external;
        }
        else
        {
            LONG x = external.left;
            LONG y = external.top;
            LONG rw = rc.Width();
            LONG rh = rc.Height();

            if (align.BitSetTo(Right))
                x = external.right - rw;

            if (align.BitSetTo(Bottom))
                y = external.bottom - rh;

            if (align.BitSetTo(XCenter))
                x += (external.Width() - rw)/2;

            if (align.BitSetTo(YCenter))
                y += (external.Height() - rh)/2;

            rc.left = x;
            rc.top = y;
            rc.right = x + rw;
            rc.bottom = y + rh;
        }
    }
}
