#pragma once

#include "wcdafx.api.h"
#include <atlstr.h>

namespace Str
{
    template <class C>
    struct Elipsis
    {
        using String = CStringT<C, StrTraitATL<C, ChTraitsCRT<C>>>;

        WCDAFX_API static String FormatV(C const* format, va_list ap);
        WCDAFX_API static String Format(C const* format, ...);
    };
}
