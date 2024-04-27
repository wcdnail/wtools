#include "stdafx.h"
#include "color.stuff.h"
#include <wingdi.h>

namespace Cf
{
    WCDAFX_API COLORREF ReduceColor(COLORREF color, COLORREF R, COLORREF G, COLORREF B)
    {
        return RGB(GetRValue(color) < R ? 0 : GetRValue(color) - R,
                   GetGValue(color) < G ? 0 : GetGValue(color) - G,
                   GetBValue(color) < B ? 0 : GetBValue(color) - B);
    }

    WCDAFX_API COLORREF IncreaseColor(COLORREF color, COLORREF R, COLORREF G, COLORREF B)
    {
        return RGB(min(GetRValue(color) + R, 255),
                   min(GetGValue(color) + G, 255),
                   min(GetBValue(color) + B, 255));
    }

    WCDAFX_API COLORREF InvertColor(COLORREF color)
    {
        return RGB(255 - GetRValue(color),
                   255 - GetGValue(color),
                   255 - GetBValue(color));
    }

    WCDAFX_API COLORREF CalcContrastColor(COLORREF color, COLORREF tolerance /*= 0x10*/)
    {
        if (   (abs((int)((color ) & 0xff) - 0x80) <= (int)tolerance)
            && (abs((int)((color >> 8) & 0xff) - 0x80) <= (int)tolerance)
            && (abs((int)((color >> 16) & 0xff) - 0x80) <= (int)tolerance)
           ) 
            return (0x7f7f7f + color) & 0xffffff;
        
        return color ^ 0xffffff;
    }

    WCDAFX_API COLORREF ComplementColor(COLORREF color)
    { 
        return (0xff - (color & 0xff))
            | ((0xff - ((color >> 8) & 0xff)) << 8)
            | ((0xff - ((color >> 16) & 0xff)) << 16)
            ;
    }
}
