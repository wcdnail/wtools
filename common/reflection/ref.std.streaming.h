#pragma once

#include <iosfwd>
#include <string>
#include <vector>

namespace std
{
    template <class C, class T, class Cs, class Ts, class As>
    inline std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& stream, std::basic_string<Cs, Ts, As> const& string)
    {
        for (typename std::basic_string<Cs, Ts, As>::const_iterator it = string.begin(); it != string.end(); ++it)
            stream.put((C)*it);

        return stream;
    }

    template <class C, class T, class Tv, class Av>
    inline std::basic_ostream<C, T>& operator << (std::basic_ostream<C, T>& stream, std::vector<Tv, Av> const& vector)
    {
        for (typename std::vector<Tv, Av>::const_iterator it = vector.begin(); it != vector.end(); ++it)
            stream << *it;

        return stream;
    }
}
