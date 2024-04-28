#pragma once

#include "wcdafx.api.h"
#include <windef.h>

namespace CF
{
    WCDAFX_API COLORREF ReduceColor(COLORREF color, COLORREF R, COLORREF G, COLORREF B);
    WCDAFX_API COLORREF IncreaseColor(COLORREF color, COLORREF R, COLORREF G, COLORREF B);
    WCDAFX_API COLORREF InvertColor(COLORREF color);
    WCDAFX_API COLORREF CalcContrastColor(COLORREF color, COLORREF tolerance = 0x10);
    WCDAFX_API COLORREF ComplementColor(COLORREF color);
}