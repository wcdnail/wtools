#pragma once

#include <wcdafx.api.h>
#include <strint.h>

enum TraceLevel: unsigned
{
    TL_Trace = 0,
    TL_DebugOut,    // captured OutputDebugString
    TL_Info,
    TL_Notify,
    TL_Warning,
    TL_Error,
    TL_Fatal,
    TL_Module,
    TL_COUNT,

    TL_LevelMask    = 0x0000ffff,
    TL_FlagMask     = 0xffff0000,
    TL_NoDCOutput   = 0x80000000,
};

namespace DH
{
    WCDAFX_API void StrTraceLevel(unsigned nLevel, std::string& szLevel);
    WCDAFX_API void StrTraceLevel(unsigned nLevel, std::wstring& szLevel);

    WCDAFX_API WString const& ModuleName();
    WCDAFX_API WString const& ModuleDir();
}
