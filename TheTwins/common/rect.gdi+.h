#pragma once

#include <atltypes.h>
#include <gdiplus.h>

inline CRect FromRect(Gdiplus::Rect const& rc)
{
    return CRect(rc.GetLeft(), rc.GetTop(), rc.GetRight(), rc.GetBottom());
}

inline CRect FromRectF(Gdiplus::RectF const& rc)
{
    return CRect((LONG)rc.GetLeft(), (LONG)rc.GetTop(), (LONG)rc.GetRight(), (LONG)rc.GetBottom());
}

inline Gdiplus::Rect ToRect(CRect const& rc)
{
    return Gdiplus::Rect(rc.left, rc.top, rc.Width(), rc.Height());
}

inline Gdiplus::RectF ToRectF(CRect const& rc)
{
    return Gdiplus::RectF((Gdiplus::REAL)rc.left, (Gdiplus::REAL)rc.top,
        (Gdiplus::REAL)rc.Width(), (Gdiplus::REAL)rc.Height());
}
