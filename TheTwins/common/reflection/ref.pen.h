#pragma once

#include "ref.serialization.h"
#include <wingdi.h>

namespace Ref
{
    class Pen: public LOGPEN
             , public Serializable<Pen>
    {
    public:
        Pen(UINT style = 0, LONG cx = 0, LONG cy = 0, COLORREF color = 0);

        template <class A>
        void Serialize(A& ar);
    };

    inline Pen::Pen(UINT style /* = 0 */, LONG cx /* = 0 */, LONG cy /* = 0 */, COLORREF color /* = 0 */)
    {
        lopnStyle = style;
        lopnWidth.x = cx;
        lopnWidth.y = cy;
        lopnColor = color;
    }

    template <class A>
    inline void Pen::Serialize(A& ar)
    {
        ar & boost::serialization::make_nvp("Style", lopnStyle);
        ar & boost::serialization::make_nvp("Width", lopnWidth.x);
        ar & boost::serialization::make_nvp("Height", lopnWidth.y);
        ar & boost::serialization::make_nvp("Color", lopnColor);
    }

    template <class C, class T>
    inline std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& stream, Pen const& pen)
    {
        stream << std::dec << pen.lopnStyle 
               << (C)' ' << pen.lopnWidth.x << (C)'x' << pen.lopnWidth.y 
               << (C)' ' << std::hex << pen.lopnColor << std::dec;

        return stream;
    }
}
