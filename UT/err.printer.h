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

static inline void PrintException(CStringA&& caption, winrt::hresult_error const& ex)
{
    winrt::hstring msg = ex.message();
    CW2AEX<512> conv(msg.c_str(), 1251);
    fprintf_s(stderr, "%s %08x ==> '%s'\n", caption.GetString(), ex.code().value, conv.m_psz);
}

static inline void PrintException(CStringA&& caption, std::exception const& ex)
{
    fprintf_s(stderr, "%s '%s'\n", caption.GetString(), ex.what());
}
