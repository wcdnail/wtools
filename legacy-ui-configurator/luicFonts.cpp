#include "stdafx.h"
#include "luicFonts.h"
#include <dh.tracing.h>
#include <string.utils.error.code.h>

#include "luicNCMetric.h"

CFonts::~CFonts() = default;
CFonts::CFonts() = default;

PCWSTR CFonts::Title(int index)
{
    static constexpr PCWSTR gs_name[FONT_Count] = {
        L"Caption Font",
        L"Small Caption Font",
        L"Menu Font",
        L"Status Font",
        L"Message Font",
        L"Desktop Font",
        L"Hyperlink Font",
    };
    if (index < 0 || index > FONT_Count - 1) {
        DH::TPrintf(L"%s: ERROR: index [%d] out of range\n", __FUNCTIONW__, index);
        return L"INVALID FONT";
    }
    return gs_name[index];
}

bool CFonts::LoadDefaults()
{
    WTL::CLogFont lfTemp;
    if (!SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(lfTemp), &lfTemp, 0)) {
        return false;
    }
    m_Pair[FONT_Desktop].Reset(lfTemp);
    lfTemp.lfUnderline = TRUE;
    m_Pair[FONT_Hyperlink].Reset(lfTemp);
    return true;
}

bool CFonts::LoadDefaults(CNCMetrics& ncMetrics)
{
    auto&   lfCaption = static_cast<WTL::CLogFont&>(ncMetrics.lfCaptionFont);
    auto& lfSmCaption = static_cast<WTL::CLogFont&>(ncMetrics.lfSmCaptionFont);
    auto&      lfMenu = static_cast<WTL::CLogFont&>(ncMetrics.lfMenuFont);
    auto&   lfTooltip = static_cast<WTL::CLogFont&>(ncMetrics.lfStatusFont);
    auto&    lfMsgBox = static_cast<WTL::CLogFont&>(ncMetrics.lfMessageFont);
    m_Pair[FONT_Caption].Reset(lfCaption);
    m_Pair[FONT_SMCaption].Reset(lfSmCaption);
    m_Pair[FONT_Menu].Reset(lfMenu);
    m_Pair[FONT_Tooltip].Reset(lfTooltip);
    m_Pair[FONT_Message].Reset(lfMsgBox);
    return true;
}

void CFontPair::Swap(CFontPair& rhs) noexcept
{
    std::swap(m_logFont, rhs.m_logFont);
    m_hFont.Attach(rhs.m_hFont.Detach());
}

void CFonts::Swap(CFonts& rhs) noexcept
{
    for (int i=0; i < FONT_Count; i++) {
        rhs[i].Swap(m_Pair[i]);
    }
}

template <typename ReturnType, typename SelfRef>
ReturnType& CFonts::getRefByIndex(SelfRef& thiz, int index)
{
    if (index < 0 || index > FONT_Count - 1) {
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
    WTL::CFont temp{logFont.CreateFontIndirectW()};
    return Reset(temp);
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
