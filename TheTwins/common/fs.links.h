#pragma once

#include "wcdafx.api.h"
#include <boost/system/error_code.hpp>

namespace Cf
{
#ifdef _WIN32
    WCDAFX_API std::wstring QueryLinkTarget(std::wstring const& dirpath, boost::system::error_code& ec);
#endif
}
