#pragma once

#include "rectz.h"

namespace Cf
{
    template <class T>
    class RectzAllocator
    {
    public:
        RectzAllocator() 
        {}

        template <class Y>
        RectzAllocator(RectT<Y> const& rc)
            : org(rc)
            , x(rc.Left())
            , y(rc.Top())
            , w(rc.Width())
            , h(rc.Height())
        {}

        template <class Y>
        RectzAllocator(RectzAllocator<Y> const& rhs)
            : org(rhs.org)
            , x(rhs.x)
            , y(rhs.y)
            , w(rhs.w)
            , h(rhs.h)
        {}

        template <class Y>
        void Reset(RectT<Y> const& rc)
        {
            org = rc;
            x = rc.Left();
            y = rc.Top();
            w = rc.Width();
            h = rc.Height();
        }

        RectT<T> Next(T cx, T cy = 0, T sx = 2, T sy = 2)
        {
            RectT<T> rv(x, y, Max(cx, w), Max(cy, h));

            T nx = rv.Width() + sx;
            x += nx;
            w -= nx;

            if (cy <= 0)
            {
                org.x = x;
                org.cx = w;
            }

            if (w < sx)
            {
                T ny = rv.Height() + sy;
                y += ny;
                h -= ny;

                x = org.Left();
                w = org.Width();
                
                org.y = y;
                org.cy = h;
            }


            if (h < sy)
            {
                y = org.Top();
                h = org.Height();

                org.x += nx;
                org.cx -= nx;
            }

            return rv;
        }

        RectT<T> NextFromRight(T cx, T cy = 0, T sx = 2, T sy = 2)
        {
            return Next(org.Width() - cx, cy, sx, sy);
        }

        RectT<T> NextFromBottom(T cx, T cy = 0, T sx = 2, T sy = 2)
        {
            return Next(cx, org.Height() - cy, sx, sy);
        }

        T Width() const { return org.Width(); }
        T Height() const { return org.Height(); }

    private:
        static T Max(T v, T limit)
        {
            return (v > 0 ? (v > limit ? limit : v) : limit);
        }

        RectT<T> org;
        T x, y, w, h;
    };
}
