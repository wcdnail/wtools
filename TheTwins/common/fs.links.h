#pragma once

#include "wcdafx.api.h"
#include <system_error>

namespace Cf
{
#ifdef _WIN32
    WCDAFX_API std::wstring QueryLinkTarget(std::wstring const& dirpath, std::error_code& ec);
#endif
}
