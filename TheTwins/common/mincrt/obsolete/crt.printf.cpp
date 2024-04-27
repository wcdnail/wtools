#include "stdafx.h"

extern "C" int _vsnprintf(char* buffer, size_t count, char const* format, va_list ap)
{
    return ::wvsprintfA(buffer, format, ap);
}

extern "C" int _vsnwprintf(wchar_t* buffer, size_t count, wchar_t const* format, va_list ap)
{
    return ::wvsprintfW(buffer, format, ap);
}
