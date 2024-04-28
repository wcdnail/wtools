#include "stdafx.h"
#include "windows.gdi.rects.h"

namespace CF
{
    void FrameRect(HDC curDc, CRect const& rc)
    {
        MoveToEx(curDc, rc.left, rc.top, nullptr);
        LineTo(curDc, rc.right, rc.top);
        LineTo(curDc, rc.right, rc.bottom);
        LineTo(curDc, rc.left, rc.bottom);
        LineTo(curDc, rc.left, rc.top);
    }

    void GradRect(HDC curDc, CRect const& rc, COLORREF c0, COLORREF c1, DWORD mode)
    {
        TRIVERTEX vertex[2] = 
        {
            {
                rc.left,
                rc.top,
                static_cast<COLOR16>(static_cast<USHORT>(GetRValue(c0)) << 8),
                static_cast<COLOR16>(static_cast<USHORT>(GetGValue(c0)) << 8),
                static_cast<COLOR16>(static_cast<USHORT>(GetBValue(c0)) << 8),
                0x0000
            },
            {
                rc.right,
                rc.bottom,
                static_cast<COLOR16>(static_cast<USHORT>(GetRValue(c1)) << 8),
                static_cast<COLOR16>(static_cast<USHORT>(GetGValue(c1)) << 8),
                static_cast<COLOR16>(static_cast<USHORT>(GetBValue(c1)) << 8),
                0x0000
            },
        };

        GRADIENT_RECT rect[1] = {{0, 1}};

        GradientFill(curDc, vertex, _countof(vertex), rect, _countof(rect), mode);
    }

    void GradRect(HDC curDc, CRect const& rc, COLORREF const(& colors)[2], DWORD mode)
    {
        GradRect(curDc, rc, colors[0], colors[1], mode);
    }
}
