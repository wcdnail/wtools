#pragma once

#ifdef _WIN32

#include "strint.h"
#include <system_error>

namespace Ntfs
{
    HANDLE OpenDirectory(PCWSTR rawPathName, bool rw);
    bool IsDirectoryJunction(PCWSTR rawPathName, HANDLE dir);
    WString QueryLinkTarget(WString const& dirpath, std::error_code& ec);
}

#endif
