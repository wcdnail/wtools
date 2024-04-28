#pragma once

#ifdef __cplusplus
#include "strint.h"

namespace Str
{
    template <class ST> inline ST InBytes(ST value) { return value; }
    template <class ST> inline ST  InKils(ST value) { return value / 1024LU; }
    template <class ST> inline ST  InMegs(ST value) { return value / 1048576LU; }
    template <class ST> inline ST  InGigs(ST value) { return value / 1073741824LU; }

    inline WString HumanSize(unsigned __int64 size)
    {
        WString rv;

        while (size)
        {
            unsigned __int64 part = size % 1000;
            size /= 1000;

            WString temp = rv;

            wchar_t buffer[6] = {0};
            ::_snwprintf_s(buffer, _countof(buffer)-1, (!size ? L"%d" : L" %03d"), part);

            rv = buffer + rv;
        }

        return rv.empty() ? WString(L"0") : rv;
    }
}
#endif
