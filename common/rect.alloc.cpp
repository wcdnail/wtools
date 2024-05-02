#include "stdafx.h"
#include "rect.alloc.h"

namespace CF
{
    template <class T>
    T RectzAllocator<T>::Max(T v, T limit)
    {
        return (v > 0 ? (v > limit ? limit : v) : limit);
    }

    template <class T>
    RectzAllocator<T>::~RectzAllocator()
    {
    }

    template <class T>
    RectzAllocator<T>::RectzAllocator()
    {
    }

    template <class T>
    RectzAllocator<T>::RectzAllocator(MyRect const& rc)
        : org(rc)
        ,   x(rc.Left())
        ,   y(rc.Top())
        ,   w(rc.Width())
        ,   h(rc.Height())
    {
    }

    template <class T>
    RectzAllocator<T>::RectzAllocator(const RectzAllocator& rhs)
        : org(rhs.org)
        ,   x(rhs.x)
        ,   y(rhs.y)
        ,   w(rhs.w)
        ,   h(rhs.h)
    {
    }

    template <class T>
    void RectzAllocator<T>::Reset(MyRect const& rc)
    {
        org = rc;
        x = rc.Left();
        y = rc.Top();
        w = rc.Width();
        h = rc.Height();
    }

    template <class T>
    void RectzAllocator<T>::Shift(T sx, T sy)
    {
        x += sx;
        y += sy;
        w -= sx;
        h -= sy;
    }

    template <class T>
    RectT<T> RectzAllocator<T>::Next(T cx, T cy, T sx, T sy)
    {
        MyRect rv(x, y, Max(cx, w), Max(cy, h));

        T nx = rv.Width() + sx;
        x += nx;
        w -= nx;
        if (cy <= 0) {
            org.x = x;
            org.cx = w;
        }
        if (w < sx) {
            T ny = rv.Height() + sy;
            y += ny;
            h -= ny;

            x = org.Left();
            w = org.Width();
                
            org.y = y;
            org.cy = h;
        }
        if (h < sy) {
            y = org.Top();
            h = org.Height();

            org.x += nx;
            org.cx -= nx;
        }

        return rv;
    }

    template <class T>
    RectT<T> RectzAllocator<T>::NextFromRight(T cx, T cy, T sx, T sy)
    {
        return Next(org.Width() - cx, cy, sx, sy);
    }

    template <class T>
    RectT<T> RectzAllocator<T>::NextFromBottom(T cx, T cy, T sx, T sy)
    {
        return Next(cx, org.Height() - cy, sx, sy);
    }

    template <class T>
    T RectzAllocator<T>::Width() const
    {
        return org.Width();
    }

    template <class T>
    T RectzAllocator<T>::Height() const
    {
        return org.Height();
    }

    template class RectzAllocator<long>;
    template class RectzAllocator<double>;
}
