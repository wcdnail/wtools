#pragma once

#include "string.utils.error.code.h"
#include <atlstr.h>
#include <atlconv.h>
#include <winrt/base.h>

static inline void PrintLastError(ATL::CStringA&& caption)
{
    HRESULT   hr = static_cast<HRESULT>(GetLastError());
    ATL::CStringW msg = Str::ErrorCode<wchar_t>::SystemMessage(hr);
    ATL::CW2AEX<512> conv(msg.GetString(), CP_UTF8);
    fprintf_s(stderr, "%s %08x '%s'\n", caption.GetString(), hr, conv.m_psz);
}

static inline void PrintException(ATL::CStringA&& caption, winrt::hresult_error const& ex)
{
    winrt::hstring msg = ex.message();
    ATL::CW2AEX<512> conv(msg.c_str(), 1251);
    fprintf_s(stderr, "%s %08x ==> '%s'\n", caption.GetString(), ex.code().value, conv.m_psz);
}

static inline void PrintException(ATL::CStringA&& caption, std::exception const& ex)
{
    fprintf_s(stderr, "%s '%s'\n", caption.GetString(), ex.what());
}
