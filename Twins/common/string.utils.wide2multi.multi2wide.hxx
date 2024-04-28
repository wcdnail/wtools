#pragma once

#include "wcdafx.api.h"
#include <string>

namespace Str
{
    WCDAFX_API void Convert(std::basic_string<char>& result, std::basic_string<char>&& source, unsigned reserved = 0);
    WCDAFX_API void Convert(std::basic_string<wchar_t>& result, std::basic_string<wchar_t>&& source, unsigned reserved = 0);
    WCDAFX_API void Convert(std::basic_string<char>& result, std::basic_string<wchar_t> const& source, unsigned codepage = CP_ACP);
    WCDAFX_API void Convert(std::basic_string<wchar_t>& result, std::basic_string<char> const& source, unsigned codepage = CP_ACP);
}
