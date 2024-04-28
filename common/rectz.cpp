#include "stdafx.h"
#include "rectz.h"
#ifdef _WIN32
#  include <winuser.h>
#endif

template <typename X, typename Y>
static void Swap2(X& x, Y& y) noexcept
{
    X saved = x;
    x = static_cast<X>(y);
    y = static_cast<Y>(saved);
}

template struct PointT<int>;
template struct PointT<long>;
template struct PointT<float>;
template struct PointT<double>;
template struct RectT<int>;
template struct RectT<long>;
template struct RectT<float>;
template struct RectT<double>;

Rect GetDesktopRect()
{
    return Rect(0, 0,
        GetSystemMetrics(SM_CXMAXIMIZED) - (GetSystemMetrics(SM_CXSIZEFRAME) * 2 + GetSystemMetrics(SM_CXFRAME) * 2),
        GetSystemMetrics(SM_CYMAXIMIZED) - (GetSystemMetrics(SM_CYSIZEFRAME) * 2 + GetSystemMetrics(SM_CYFRAME) * 2)
    );
}

Rect GetScreenRect()
{
    return Rect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
}

DRect LRect2DRect(LRect const& src)
{
    return DRect {  static_cast<double>(src.x),  static_cast<double>(src.y),
                   static_cast<double>(src.cx), static_cast<double>(src.cy) };
}

template <typename T>
PointT<T>::~PointT()
{
}

template <typename T>
PointT<T>::PointT()
    : x(0)
    , y(0)
{
}

template <typename T>
PointT<T>::PointT(T a, T b)
    : x(a)
    , y(b)
{
}

template <typename T>
PointT<T>::PointT(PointT const& rhs)
    : x(rhs.x)
    , y(rhs.y)
{
}

template <typename T>
PointT<T>::PointT(PointT&& rhs)
    : PointT<T>(0, 0)
{
    Swap(rhs);
}

template <typename T>
PointT<T>& PointT<T>::operator=(PointT const& rhs)
{
    if (&rhs != this) {
        PointT<T> temp(rhs);
        this->Swap(temp);
    }
    return *this;
}

template <typename T>
PointT<T>& PointT<T>::operator=(PointT&& rhs)
{
    if (&rhs != this) {
        this->Swap(rhs);
    }
    return *this;
}

template <typename T>
bool PointT<T>::IsZero() const
{
    return (0 == x) && (0 == y);
}

template <typename T>
void PointT<T>::Set(T a, T b)
{
    x = a;
    y = b;
}

template <typename T>
void PointT<T>::Swap(PointT& same) noexcept
{
    std::swap(x, same.x);
    std::swap(y, same.y);
}

template <typename T>
PointT<T>& PointT<T>::operator+=(PointT const& pt)
{
    x += pt.x;
    y += pt.y;
    return *this;
}

template <typename T>
PointT<T>& PointT<T>::operator-=(PointT const& pt)
{
    x -= pt.x;
    y -= pt.y;
    return *this;
}

template <typename T>
RectT<T>::~RectT()
{
}

template <typename T>
RectT<T>::RectT()
    :  x(0)
    ,  y(0)
    , cx(0)
    , cy(0)
{
}

template <typename T>
RectT<T>::RectT(T a, T b, T c, T d)
    :  x(a)
    ,  y(b)
    , cx(c)
    , cy(d)
{
}

template <typename T>
RectT<T>::RectT(const PointT<T>& a, const PointT<T>& b)
    :  x(a.x)
    ,  y(a.y)
    , cx(b.x)
    , cy(b.y)
{
}

template <typename T>
RectT<T>::RectT(const PointT<T>& a, T c, T d)
    : x(a.x)
    , y(a.y)
    , cx(c)
    , cy(d)
{
}

template <typename T>
RectT<T>::RectT(T a, T b, const MyPoint& s)
    : x(a)
    , y(b)
    , cx(s.x)
    , cy(s.y)
{
}

template <typename T>
RectT<T>::RectT(T c, T d, const RectT& external, unsigned align)
    : x(0)
    , y(0)
    , cx(c)
    , cy(d)
{
    PutInto(external, align);
}

template <typename T>
RectT<T>::RectT(RectT const& rhs)
    :  x(rhs.x)
    ,  y(rhs.y)
    , cx(rhs.cx)
    , cy(rhs.cy)
{
}

template <typename T>
RectT<T>::RectT(RectT&& rhs)
    : RectT<T>()
{
    Swap(rhs);
}

template <typename T>
RectT<T>& RectT<T>::operator=(RectT const& rhs)
{
    if (&rhs != this) {
        RectT<T> temp(rhs);
        this->Swap(temp);
    }
    return *this;
}

template <typename T>
RectT<T>& RectT<T>::operator=(RectT&& rhs)
{
    if (&rhs != this) {
        this->Swap(rhs);
    }
    return *this;
}

template <typename T>
void RectT<T>::Set(T a, T b, T c, T d)
{
    x = a;
    y = b;
    cx = c;
    cy = d;
}

template <typename T>
void RectT<T>::PutInto(RectT const& rc, unsigned how)
{
    if (PutAt_In(how, PutAt::Fill)) {
        *this = rc;
    }
    else {
        T l = rc.x;
        T t = rc.y;
        if (PutAt_In(how, PutAt::Right)) {
            l = rc.Right() - cx;
        }
        else if (PutAt_In(how, PutAt::XCenter)) {
            l += (rc.cx - cx) / 2;
        }
        if (PutAt_In(how, PutAt::Bottom)) {
            t = rc.Bottom() - cy;
        }
        else if (PutAt_In(how, PutAt::YCenter)) {
            t += (rc.cy - cy) / 2;
        }
        x = l;
        y = t;
    }
}

template <typename T>
void RectT<T>::Shrink(T a, T b)
{
    x += a;
    y += b;
    cx -= a * 2;
    cy -= b * 2;
}

template <typename T>
void RectT<T>::Swap(RectT& same) noexcept
{
    std::swap(x, same.x);
    std::swap(y, same.y);
    std::swap(cx, same.cx);
    std::swap(cy, same.cy);
}

template <typename T>
T RectT<T>::Left() const
{
    return x;
}

template <typename T>
T RectT<T>::Top() const
{
    return y;
}

template <typename T>
T RectT<T>::Right() const
{
    return x + cx;
}

template <typename T>
T RectT<T>::Bottom() const
{
    return y + cy;
}

template <typename T>
T RectT<T>::Width() const
{
    return cx;
}

template <typename T>
T RectT<T>::Height() const
{
    return cy;
}

template <typename T>
PointT<T> RectT<T>::Start() const
{
    return PointT<T>(x, y);
}

template <typename T>
PointT<T> RectT<T>::Size() const
{
    return PointT<T>(cx, cy);
}

template <typename T>
bool RectT<T>::IsZero() const
{
    return (0 == x) && (0 == y) && (0 == cx) && (0 == cy);
}

template <typename T>
void RectT<T>::SetRight(T v)
{
    cx = v - x;
}

template <typename T>
void RectT<T>::SetBottom(T v)
{
    cy = v - y;
}
