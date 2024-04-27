#include "stdafx.h"
#include "string.utils.format.h"

namespace Str
{
    template <class C>
    Elipsis<C>::String Elipsis<C>::FormatV(C const* format, va_list ap)
    {
        String result;
        result.FormatV(format, ap);
        return result;
    }

    template <class C>
    Elipsis<C>::String Elipsis<C>::Format(C const* format, ...)
    {
        va_list ap;
        va_start(ap, format);
        String result;
        result.FormatV(format, ap);
        va_end(ap);
        return result;
    }
}

template Str::Elipsis<char>;
template Str::Elipsis<wchar_t>;
