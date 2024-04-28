#pragma once

#include "ref.serialization.h"
#include <wingdi.h>
#include <iosfwd>

namespace Ref
{
    class Brush: public LOGBRUSH
               , public Serializable<Brush>
    {
    public:
        Brush(UINT style = 0, COLORREF color = 0, ULONG_PTR hatch = 0);

        template <class A>
        void Serialize(A& ar);
    };

    inline Brush::Brush(UINT style /* = 0 */, COLORREF color /* = 0 */, ULONG_PTR hatch /* = 0 */)
    {
        lbStyle = style;
        lbColor = color;
        lbHatch = hatch;
    }

    template <class A>
    inline void Brush::Serialize(A& ar)
    {
        ar & boost::serialization::make_nvp("Style", lbStyle);
        ar & boost::serialization::make_nvp("Color", lbColor);
        ar & boost::serialization::make_nvp("Hatch", lbHatch);
    }

    template <class C, class T>
    inline std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& stream, Brush const& brush)
    {
        stream << std::dec << brush.lbStyle << (C)' ' << std::hex << brush.lbColor << (C)' ' << std::dec << brush.lbHatch;
        return stream;
    }
}
