#pragma once

#include <atltypes.h>
#include <gdiplus.h>

inline CRect FromRect(Gdiplus::Rect const& rc)
{
    return {rc.GetLeft(), rc.GetTop(), rc.GetRight(), rc.GetBottom()};
}

inline CRect FromRectF(Gdiplus::RectF const& rc)
{
    return {static_cast<LONG>(rc.GetLeft()), static_cast<LONG>(rc.GetTop()), static_cast<LONG>(rc.GetRight()), static_cast<LONG>(rc.GetBottom())};
}

inline Gdiplus::Rect ToRect(CRect const& rc)
{
    return {rc.left, rc.top, rc.Width(), rc.Height()};
}

inline Gdiplus::RectF ToRectF(CRect const& rc)
{
    return {
        static_cast<Gdiplus::REAL>(rc.left), static_cast<Gdiplus::REAL>(rc.top),
        static_cast<Gdiplus::REAL>(rc.Width()), static_cast<Gdiplus::REAL>(rc.Height())
    };
}
