#pragma once

#include "wcdafx.api.h"
#include <iosfwd>
#ifdef _WIN32
#  include <atltypes.h>
#endif

struct PutAt
{
    enum : unsigned
    {
        Left    = 0x00,
        Top     = 0x01,
        Right   = 0x02,
        Bottom  = 0x04,
        XCenter = 0x08,
        YCenter = 0x10,
        Fill    = 0x20,
        Center  = XCenter | YCenter,
    };
};

#define PutAt_In(flag, val) (0 != ((flag) & (val)))

template <typename T>
struct PointT
{
    using MyPoint = PointT<T>;

    T x;
    T y;

    WCDAFX_API ~PointT();
    WCDAFX_API PointT();
    WCDAFX_API PointT(T a, T b);

    WCDAFX_API PointT(PointT const& rhs);
    WCDAFX_API PointT(PointT&& rhs);
    PointT& operator = (PointT const& rhs);
    PointT& operator = (PointT&& rhs);

    WCDAFX_API bool IsZero() const;
    WCDAFX_API void Set(T a, T b);
    WCDAFX_API void Swap(PointT& same) noexcept;

    WCDAFX_API PointT& operator += (PointT const& pt);
    WCDAFX_API PointT& operator -= (PointT const& pt);
    
};

template <typename T>
struct RectT
{
    using MyPoint = PointT<T>;
    using  MyRect = RectT<T>;

    T  x;
    T  y;
    T cx;
    T cy;

    WCDAFX_API ~RectT();
    WCDAFX_API RectT();
    WCDAFX_API RectT(T a, T b, T c, T d);
    WCDAFX_API RectT(MyPoint const& a, MyPoint const& b);
    WCDAFX_API RectT(MyPoint const& a, T c, T d);
    WCDAFX_API RectT(T a, T b, MyPoint const& s);
    WCDAFX_API RectT(MyRect const& external, unsigned align) { PutInto(external, align); }
    WCDAFX_API RectT(T c, T d, MyRect const& external, unsigned align);

    WCDAFX_API RectT(RectT const& rhs);
    WCDAFX_API RectT(RectT&& rhs);
    RectT& operator = (RectT const& rhs);
    RectT& operator = (RectT&& rhs);

    WCDAFX_API void Set(T a, T b, T c, T d);
    WCDAFX_API void PutInto(RectT const& rc, unsigned how);
    WCDAFX_API void Shrink(T a, T b);
    WCDAFX_API void Swap(RectT& same) noexcept;

    WCDAFX_API T Left() const;
    WCDAFX_API T Top() const;
    WCDAFX_API T Right() const;
    WCDAFX_API T Bottom() const;
    WCDAFX_API T Width() const;
    WCDAFX_API T Height() const;
    WCDAFX_API MyPoint Start() const;
    WCDAFX_API MyPoint Size() const;
    WCDAFX_API bool IsZero() const;

    WCDAFX_API void SetRight(T v);
    WCDAFX_API void SetBottom(T v);
};

using  Poinr = PointT<int>;
using LPoinr = PointT<long>;
using FPoinr = PointT<float>;
using DPoinr = PointT<double>;
using   Rect = RectT<int>;
using  LRect = RectT<long>;
using  FRect = RectT<float>;
using  DRect = RectT<double>;

WCDAFX_API Rect GetDesktopRect();
WCDAFX_API Rect GetScreenRect();

template <class T>
inline CRect ToCRect(RectT<T> const& rc)
{
    return CRect(static_cast<LONG>(rc.Left()),
                 static_cast<LONG>(rc.Top()),
                 static_cast<LONG>(rc.Right()),
                 static_cast<LONG>(rc.Bottom()));
}

template <class T>
inline RectT<T> FromCRect(CRect const& rc)
{
    return RectT<T>(static_cast<T>(rc.left),
                    static_cast<T>(rc.top),
                    static_cast<T>(rc.Width()),
                    static_cast<T>(rc.Height()));
}

template <typename C, typename T, typename U>
inline std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& s, PointT<U> const& pt)
{
    s << pt.x << static_cast<C>(' ') << pt.y;
    return s;
}

template <typename C, typename T, typename U>
inline std::basic_istream<C, T>& operator >> (std::basic_istream<C, T>& s, PointT<U>& pt)
{
    s >> pt.x >> pt.y;
    return s;
}

template <typename U, typename V>
inline bool operator == (PointT<U> const& l, PointT<V> const& r)
{
    return (l.x == static_cast<U>(r.x)) && (l.y == static_cast<U>(r.y));
}

template <typename C, typename T, typename U>
inline std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& s, RectT<U> const& rc)
{
    s << rc.x << static_cast<C>(' ') << rc.y << static_cast<C>(' ') << rc.cx << static_cast<C>(' ') << rc.cy;
    return s;
}

template <typename C, typename T, typename U>
inline std::basic_istream<C, T>& operator >> (std::basic_istream<C, T>& s, RectT<U>& rc)
{
    s >> rc.x >> rc.y >> rc.cx >> rc.cy;
    return s;
}

template <typename U, typename V>
inline bool operator == (RectT<U> const& l, RectT<V> const& r)
{
    return (l.Start() == r.Start()) && (l.Size() == r.Size());
}

WCDAFX_API DRect LRect2DRect(LRect const& src);
