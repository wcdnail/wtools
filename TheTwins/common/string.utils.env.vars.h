#pragma once

#ifdef _WIN32
#  include <winbase.h>
#endif
#include <string>
#include <memory>

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 5045) // warning C5045: Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
                                 //      index 'ln' range checked by comparison on this line
                                 //      feeds call on this line
#endif

namespace Env
{
    inline std::wstring Expand(std::wstring const& vn)
    {
#ifdef _WIN32
        DWORD nuLen = ::ExpandEnvironmentStringsW(vn.c_str(), nullptr, 0);
        if (nuLen > 0) {
            std::unique_ptr<wchar_t[]> buffer = std::make_unique<wchar_t[]>(nuLen);
            ::ExpandEnvironmentStringsW(vn.c_str(), buffer.get(), nuLen);
            return std::wstring{ buffer.get(), nuLen - 1 };
        }
#endif
        return {};
    }
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif
