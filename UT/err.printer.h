#pragma once

#include "string.utils.error.code.h"
#include <atlstr.h>
#include <atlconv.h>

static inline void PrintLastError(CStringA&& caption)
{
    HRESULT   hr = static_cast<HRESULT>(GetLastError());
    CStringW msg = Str::ErrorCode<wchar_t>::SystemMessage(hr);
    CW2AEX<512> conv(msg.GetString(), CP_UTF8);
    fprintf_s(stderr, "%s %08x '%s'\n", caption.GetString(), hr, conv.m_psz);
}
