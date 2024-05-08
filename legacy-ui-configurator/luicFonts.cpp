#include "stdafx.h"
#include "luicFonts.h"
#include <dh.tracing.h>
#include <string.utils.error.code.h>

CFonts::~CFonts() = default;
CFonts::CFonts() = default;

PCWSTR CFonts::Title(int index)
{
    static constexpr PCWSTR gs_name[FONTS_Count] = {
        L"Caption Font",
        L"Small Caption Font",
        L"Menu Font",
        L"Status Font",
        L"Message Font",
        L"Desktop Font",
        L"Hyperlink Font",
    };
    if (index < 0 || index > FONTS_Count - 1) {
        DH::TPrintf(L"%s: ERROR: index [%d] out of range\n", __FUNCTIONW__, index);
        return L"INVALID FONT";
    }
    return gs_name[index];
}

template <typename ReturnType, typename SelfRef>
ReturnType& CFonts::getRefByIndex(SelfRef& thiz, int index)
{
    if (index < 0 || index > FONTS_Count - 1) {
        static ReturnType dummy{};
        DH::TPrintf(L"%s: ERROR: index [%d] out of range\n", __FUNCTIONW__, index);
        return dummy;
    }
    return thiz.m_Pair[index];
}

CFontPair& CFonts::operator[](int index)
{
    return getRefByIndex<CFontPair>(*this, index);
}

CFontPair const& CFonts::operator[](int index) const
{
    return getRefByIndex<const CFontPair>(*this, index);
}

bool CFontPair::Reset(WTL::CLogFont& logFont)
{
    const HFONT hFont = logFont.CreateFontIndirectW();
    if (!hFont) {
        const auto code = static_cast<HRESULT>(GetLastError());
        const auto codeText = Str::ErrorCode<wchar_t>::SystemMessage(code);
        DH::TPrintf(L"%s: ERROR: CreateFontIndirectW failed: %d '%s'\n", __FUNCTIONW__, code, codeText.GetString());
        return false;
    }
    m_logFont = logFont;
    m_hFont.Attach(hFont);
    return true;
}

bool CFontPair::Reset(WTL::CFont& hFont)
{
    WTL::CLogFont temp;
    if (!hFont.GetLogFont(temp)) {
        const auto code = static_cast<HRESULT>(GetLastError());
        const auto codeText = Str::ErrorCode<wchar_t>::SystemMessage(code);
        DH::TPrintf(L"%s: ERROR: GetLogFont failed: %d '%s'\n", __FUNCTIONW__, code, codeText.GetString());
        return false;
    }
    m_logFont = temp;
    m_hFont.Attach(hFont.Detach());
    return true;
}
