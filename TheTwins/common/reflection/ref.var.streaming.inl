#pragma once

#include "ref.std.streaming.h"
#include <iosfwd>
#include <string>

namespace Ref
{
    template <class T> 
    inline void Var::Impl<T>::Serialize(std::ostream& ar)
    {
        ar << ref_;
    }

    template <class T> 
    inline void Var::Impl<T>::Serialize(std::wostream& ar)
    {
        ar << ref_;
    }

    template <class C, class T>
    inline std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& stream, Var& var)
    {
        var.Serialize(stream);
        return stream;
    }
}
