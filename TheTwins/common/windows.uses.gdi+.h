#pragma once 

#include <boost/noncopyable.hpp>

namespace Initialize
{
    struct GdiPlus: boost::noncopyable
    {
        GdiPlus(int requiredver = 1);
        ~GdiPlus();

        static char const* StatusString(int status);

    private:
        ULONG_PTR Id;
    };
}
