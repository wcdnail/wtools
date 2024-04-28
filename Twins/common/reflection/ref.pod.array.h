#pragma once

#include <iosfwd>

namespace Ref
{
    template <class C, class T, size_t Size, class Array>
    inline std::basic_istream<C, T>& operator >> (std::basic_istream<C, T>& stream, Array (&arr)[Size])
    {
        for (size_t i=0; i<Size; i++)
        {
            Array temp;
            stream >> temp;
            arr[i]  = temp;
        }

        return stream;
    }

    template <class C, class T, size_t Size, class Array>
    inline std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& stream, Array (&arr)[Size])
    {
        for (size_t i=0; i<Size; i++)
            stream << arr[i] << (C)' ';

        return stream;
    }
}
