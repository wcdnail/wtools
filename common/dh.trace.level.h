#pragma once

#include <wcdafx.api.h>
#include <strint.h>

enum TraceLevel
{
    TL_Trace = 0,
    TL_Info,
    TL_Notify,
    TL_Warning,
    TL_Error,
    TL_Fatal,
    TL_Module,
};

namespace DH
{
    WCDAFX_API void StrTraceLevel(int nLevel, std::string& szLevel);
    WCDAFX_API void StrTraceLevel(int nLevel, std::wstring& szLevel);

    WCDAFX_API WString const& ModuleName();
    WCDAFX_API WString const& ModuleDir();
}
