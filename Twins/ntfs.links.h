#pragma once

#ifdef _WIN32

#include "strint.h"
#include <boost/system/error_code.hpp>

namespace Ntfs
{
    HANDLE OpenDirectory(PCWSTR rawPathName, bool rw);
    bool IsDirectoryJunction(PCWSTR rawPathName, HANDLE dir);
    WString QueryLinkTarget(WString const& dirpath, std::error_code& ec);
}

#endif
