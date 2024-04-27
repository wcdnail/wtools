#pragma once

#include <atlstr.h>

namespace Str
{
    template <class C>
    struct Elipsis
    {
        typedef typename CStringT< C, StrTraitATL< C, ChTraitsCRT< C > > > String;

        static String FormatV(C const* format, va_list ap)
        {
            String result;
            result.FormatV(format, ap);
            return result;
        }

        static String Format(C const* format, ...)
        {
            va_list ap;
            va_start(ap, format);

            String result;
            result.FormatV(format, ap);

            va_end(ap);
            return result;
        }
    };
}
