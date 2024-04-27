#pragma once

#include <atlstr.h>

namespace Str
{
    template <class C>
    struct Elipsis
    {
        using String = CStringT<C, StrTraitATL<C, ChTraitsCRT<C>>>;

        static String FormatV(C const* format, va_list ap);
        static String Format(C const* format, ...);
    };
}
