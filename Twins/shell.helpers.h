#pragma once

#include <atluser.h>

namespace Twins
{
    CIconHandle LoadShellIcon(std::wstring const& entry, unsigned attrs = INVALID_FILE_ATTRIBUTES, UINT flags = SHGFI_SMALLICON | SHGFI_ADDOVERLAYS);
}

