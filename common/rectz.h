#pragma once

#include <iosfwd>

#ifdef _WIN32
#include <winuser.h>
#include <atltypes.h>
#endif

struct PutAt
{
    static const int Left    = 0x00;
    static const int Top     = 0x01;
    static const int Right   = 0x02;
    static const int Bottom  = 0x04;
    static const int XCenter = 0x08;
    static const int YCenter = 0x10;
    static const int Fill    = 0x20;
    static const int Center  = XCenter | YCenter;

    static bool In(unsigned flag, unsigned val)
    {
        return (flag & val) != 0;
    }
};

template <typename T>
struct PointT
{
    T x;
    T y;

    template <typename Y>
    PointT(PointT<Y> const& other): x((T)other.X()), y((T)other.Y()) {}
    PointT(): x(0), y(0) {}
    PointT(T a, T b): x(a), y(b) {}

    void Set(T a, T b) { x = a, y = b; }

    template <typename Y>
    PointT& operator += (PointT<Y> const& pt)
    {
        x += pt.x;
        y += pt.y;
        return *this;
    }

    template <typename Y>
    PointT& operator -= (PointT<Y> const& pt)
    {
        x -= pt.x;
        y -= pt.y;
        return *this;
    }

    bool IsZero() const { return (0 == x) && (0 == y); }
};

template <typename T>
struct RectT
{
    T x;
    T y;
    T cx;
    T cy;

    template <class Y>
    RectT(RectT<Y> const& rhs): x(rhs.x), y(rhs.y), cx(rhs.cx), cy(rhs.cy) {}
    RectT(): x(0), y(0), cx(0), cy(0) {}
    RectT(T a, T b, T c, T d): x(a), y(b), cx(c), cy(d) {}
    RectT(PointT<T> const& a, PointT<T> const& b): x(a.x), y(a.y), cx(b.x), cy(b.y) {}
    RectT(PointT<T> const& a, T c, T d): x(a.x), y(a.y), cx(c), cy(d) {}
    RectT(T a, T b, PointT<T> const& s): x(a), y(b), cx(s.x), cy(s.y) {}
    RectT(RectT<T> const& external, int align) { PutInto(external, align); }
    RectT(T c, T d, RectT<T> const& external, unsigned align): x(0), y(0), cx(c), cy(d) { PutInto(external, align); }

    void Set(T a, T b, T c, T d) { x = a; y = b; cx = c; cy = d; }

    void PutInto(RectT<T> const& rc, unsigned how)
    {
        if (PutAt::In(how, PutAt::Fill))
        {
            *this = rc;
        }
        else
        {
            T l = rc.x;
            T t = rc.y;

            if (PutAt::In(how, PutAt::Right))
                l = rc.Right() - cx;
                
            if (PutAt::In(how, PutAt::Bottom))
                t = rc.Bottom() - cy;

            if (PutAt::In(how, PutAt::XCenter))
                l += (rc.cx - cx) / 2;

            if (PutAt::In(how, PutAt::YCenter))
                t += (rc.cy - cy) / 2;

            x = l;
            y = t;
        }
    }

    void Shrink(T a, T b)
    {
        x += a;
        y += b;
        cx -= a * 2;
        cy -= b * 2;
    }

    T Left() const { return x; }
    T Top() const { return y; }
    T Right() const { return x + cx; }
    T Bottom() const { return y + cy; }
    T Width() const { return cx; }
    T Height() const { return cy; }
    PointT<T> Start() const { return PointT<T>(x, y); }
    PointT<T> Size() const { return PointT<T>(cx, cy); }
    bool IsZero() const { return (0 == x) && (0 == y) && (0 == cx) && (0 == cy); }

    void SetRight(T v) { cx = v - x; }
    void SetBottom(T v) { cy = v - y; }
};

template <typename C, typename T, typename U>
inline std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& s, PointT<U> const& pt)
{
    s << pt.x << (C)' ' << pt.y;
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
    return (l.x == (U)r.x) && (l.y == (U)r.y);
}

template <typename C, typename T, typename U>
inline std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& s, RectT<U> const& rc)
{
    s << rc.x << (C)' ' << rc.y << (C)' ' << rc.cx << (C)' ' << rc.cy;
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


#ifdef _WIN32

const RectT<int> ScreenRect = 
    RectT<int>(  0
               , 0
               , ::GetSystemMetrics(SM_CXSCREEN)
               , ::GetSystemMetrics(SM_CYSCREEN));

const RectT<int> DesktopRect = 
    RectT<int>(  0
               , 0
               , ::GetSystemMetrics(SM_CXMAXIMIZED) - (::GetSystemMetrics(SM_CXSIZEFRAME) * 2 + ::GetSystemMetrics(SM_CXFRAME) * 2)
               , ::GetSystemMetrics(SM_CYMAXIMIZED) - (::GetSystemMetrics(SM_CYSIZEFRAME) * 2 + ::GetSystemMetrics(SM_CYFRAME) * 2)
               );

template <class T> ::CRect ToRect(RectT<T> const& rc) { return ::CRect((LONG)rc.Left(), (LONG)rc.Top(), (LONG)rc.Right(), (LONG)rc.Bottom()); }
template <class T> RectT<T> FromRect(CRect const& rc) { return RectT<T>((T)rc.left, (T)rc.top, (T)rc.Width(), (T)rc.Height()); }

#endif

namespace Cf
{
    typedef RectT<int> Rect;
}
