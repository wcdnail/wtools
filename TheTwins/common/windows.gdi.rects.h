#ifndef _CF_windows_gdi_rects_h__
#define _CF_windows_gdi_rects_h__

#include <atltypes.h>

namespace Cf  // Common framework
{
    inline void FrameRect(HDC curDc, CRect const& rc)
    {
        ::MoveToEx(curDc, rc.left, rc.top, NULL);
        ::LineTo(curDc, rc.right, rc.top);
        ::LineTo(curDc, rc.right, rc.bottom);
        ::LineTo(curDc, rc.left, rc.bottom);
        ::LineTo(curDc, rc.left, rc.top);
    }

    inline void GradRect(HDC curDc, CRect const& rc, COLORREF c0, COLORREF c1, DWORD mode = GRADIENT_FILL_RECT_V) 
    {
        TRIVERTEX vertex[2] = 
        {
            { rc.left, rc.top,     GetRValue(c0) << 8, GetGValue(c0) << 8, GetBValue(c0) << 8, 0x0000 },
            { rc.right, rc.bottom, GetRValue(c1) << 8, GetGValue(c1) << 8, GetBValue(c1) << 8, 0x0000 },
        };

        GRADIENT_RECT rect[1] = {{0, 1}};

        ::GradientFill(curDc, vertex, _countof(vertex), rect, _countof(rect), mode);
    }

    inline void GradRect(HDC curDc, CRect const& rc, COLORREF const (&colors)[2], DWORD mode = GRADIENT_FILL_RECT_V)
    {
        GradRect(curDc, rc, colors[0], colors[1], mode);
    }
}

#endif // _CF_windows_gdi_rects_h__
