#pragma once

#include <atltypes.h>

namespace Rc
{
    CRect Squared(CRect const& rc, int r)
    {
        double ratio = (double)rc.Height() / (double)rc.Width();
        int radius = (int)(r * ratio);
        return CRect(0, 0, radius, radius);
    }
}
