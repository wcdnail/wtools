#pragma once

#include "wcdafx.api.h"
#include "rectz.h"

namespace CF
{
    template <class T>
    class RectzAllocator
    {
    public:
        using MyRect = RectT<T>;

        WCDAFX_API ~RectzAllocator();
        WCDAFX_API RectzAllocator();
        WCDAFX_API RectzAllocator(MyRect const& rc);
        WCDAFX_API RectzAllocator(RectzAllocator<T> const& rhs);

        WCDAFX_API void Reset(MyRect const& rc);

        WCDAFX_API MyRect Next(T cx, T cy = 0, T sx = 2, T sy = 2);
        WCDAFX_API MyRect NextFromRight(T cx, T cy = 0, T sx = 2, T sy = 2);
        WCDAFX_API MyRect NextFromBottom(T cx, T cy = 0, T sx = 2, T sy = 2);

        WCDAFX_API T Width() const;
        WCDAFX_API T Height() const;

    private:
        static T Max(T v, T limit);

        MyRect   org;
        T x, y, w, h;
    };
}
