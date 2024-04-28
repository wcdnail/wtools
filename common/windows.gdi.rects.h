#pragma once

#include "wcdafx.api.h"
#include <atltypes.h>

namespace CF
{
    WCDAFX_API void FrameRect(HDC curDc, CRect const& rc);
    WCDAFX_API void GradRect(HDC curDc, CRect const& rc, COLORREF c0, COLORREF c1, DWORD mode = GRADIENT_FILL_RECT_V);
    WCDAFX_API void GradRect(HDC curDc, CRect const& rc, COLORREF const (&colors)[2], DWORD mode = GRADIENT_FILL_RECT_V);
}
