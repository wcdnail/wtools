#pragma once

#ifdef _WIN32

#include <string.hp.h>
#include <boost/system/error_code.hpp>

namespace Ntfs
{
    HANDLE OpenDirectory(PCWSTR rawPathName, bool rw);
    bool IsDirectoryJunction(PCWSTR rawPathName, HANDLE dir);
    WidecharString QueryLinkTarget(WidecharString const& dirpath, boost::system::error_code& ec);
}

#endif
