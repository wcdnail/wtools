#pragma once

#include <string>
#ifdef _WIN32
#include <winbase.h>
#endif

namespace Env
{
    inline std::wstring Expand(std::wstring const& vn)
    {
#ifdef _WIN32
        DWORD ln = ::ExpandEnvironmentStringsW(vn.c_str(), NULL, 0);
        if (ln > 0)
        {
            std::wstring rv(ln, L' ');
            ::ExpandEnvironmentStringsW(vn.c_str(), &rv[0], ln);

            return rv;
        }
#endif

        return vn;
    }
}

