#pragma once

#if 0
#include <string>
#include <boost/lexical_cast.hpp>

namespace CF  // Common framework
{
    template <class ST> inline ST InBytes(ST value) { return value; }
    template <class ST> inline ST  InKils(ST value) { return value / 1024LU; }
    template <class ST> inline ST  InMegs(ST value) { return value / 1048576LU; }
    template <class ST> inline ST  InGigs(ST value) { return value / 1073741824LU; }

    template <class CT, class ST>
    inline std::basic_string<CT> HumanSize(ST size)
    {
        typedef typename std::basic_string<CT> StringType;
        StringType rv;

        while (size)
        {
            ST part = size % 1000;
            size /= 1000;

            StringType temp = rv;
            rv = boost::lexical_cast<StringType, ST>(part);
            rv += temp;
        }

        static const StringType nullRv((CT)"0");
        return rv.empty() ? nullRv : rv;
    }
}
#endif // 0
