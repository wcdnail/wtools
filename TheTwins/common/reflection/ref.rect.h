#pragma once

#include "ref.serialization.h"
#include <atltypes.h>

namespace Ref
{
    class Rect: public ::CRect
              , public Serializable<Rect>
    {
    public:
        Rect();
        Rect(LONG l, LONG t, LONG r, LONG b);
        Rect(CRect const& rc);

        template <class A>
        void Serialize(A& ar);
    };

    inline Rect::Rect()
        : ::CRect()
    {}

    inline Rect::Rect(LONG l, LONG t, LONG r, LONG b)
        : ::CRect(l, t, r, b)
    {}

    inline Rect::Rect(CRect const& rc)
        : ::CRect(rc)
    {}

    template <class A>
    inline void Rect::Serialize(A& ar)
    {
        ar & BOOST_SERIALIZATION_NVP(left);
        ar & BOOST_SERIALIZATION_NVP(top);
        ar & BOOST_SERIALIZATION_NVP(right);
        ar & BOOST_SERIALIZATION_NVP(bottom);
    }

    template <class C, class T>
    inline std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& stream, Rect const& rect)
    {
        stream << rect.left << (C)' ' 
               << rect.top << (C)' ' 
               << rect.right << (C)' ' 
               << rect.bottom << (C)' '
               << rect.Width() << (C)'x'
               << rect.Height()
               ;

        return stream;
    }
}
