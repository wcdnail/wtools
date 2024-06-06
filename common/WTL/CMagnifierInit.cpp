#include "stdafx.h"
#include "CMagnifierInit.h"
#include <string.utils.error.code.h>
#include <dh.tracing.defs.h>
#include <dh.tracing.h>
#include <Magnification.h>

#pragma comment(lib, "Magnification.lib")

CMagnifierInit& CMagnifierInit::Instance()
{
    static CMagnifierInit gs_MagnifierInit;
    return gs_MagnifierInit;
}

CMagnifierInit::CMagnifierInit()
{
    if (MagInitialize()) {
        DH::TPrintf(0, L"MagInitialize OK\n");
        return ;
    }
    auto const hCode{static_cast<HRESULT>(GetLastError())};
    auto const  sMsg{Str::ErrorCode<>::SystemMessage(hCode)};
    DH::TPrintf(TL_Error, L"MagInitialize failed: 0x%08x %s\n", hCode, sMsg.GetString());
}

CMagnifierInit::~CMagnifierInit()
{
    if (MagUninitialize()) {
        DH::TPrintf(0, L"MagUninitialize OK\n");
        return ;
    }
    auto const hCode{static_cast<HRESULT>(GetLastError())};
    auto const  sMsg{Str::ErrorCode<>::SystemMessage(hCode)};
    DH::TPrintf(TL_Error, L"MagUninitialize failed: 0x%08x %s\n", hCode, sMsg.GetString());
}
