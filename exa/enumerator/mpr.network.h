#pragma once

#include "auto.close.object.h"
#include <string>
#include <winnetwk.h>

namespace Mpr
{
    typedef Cf::AutoClosable<HANDLE, NULL> EnumHandle;

    PCWSTR DispTypeString(DWORD type);
    std::wstring UsageString(DWORD usage);
    bool Enumerate(LPNETRESOURCEW lpnr, DWORD scope, DWORD type, DWORD usage, int deep, bool shallAll);
}
