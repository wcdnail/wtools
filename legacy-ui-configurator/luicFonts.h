#pragma once

#include <wcdafx.api.h>
#include <atlstr.h>
#include <atlgdi.h>

struct CNCMetrics;

enum EFontSizes : long
{
    MIN_FONT_SIZE = 6,
    MAX_FONT_SIZE = 24,
};

enum EFontIndex : int
{
    FONT_Caption,
    FONT_SMCaption,
    FONT_Menu,
    FONT_Tooltip,
    FONT_Message,
    FONT_Desktop,
    FONT_Hyperlink,
    FONT_Count
};

struct CFontPair
{
    WTL::CLogFont  m_logFont;
    WTL::CFontHandle m_hFont;

    bool Reset(WTL::CLogFont& logFont);
    bool Reset(WTL::CFont& hFont);
    void Swap(CFontPair& rhs) noexcept;
};

struct CFonts
{
    DELETE_COPY_MOVE_OF(CFonts);

    ~CFonts();
    CFonts();

    static PCWSTR Title(int index);

    bool LoadDefaults();
    bool LoadDefaults(CNCMetrics& ncMetrics);
    void Swap(CFonts& rhs) noexcept;

    CFontPair& operator[](int index);
    CFontPair const& operator[](int index) const;

private:
    CFontPair m_Pair[FONT_Count];

    template <typename ReturnType, typename SelfRef>
    static ReturnType& getRefByIndex(SelfRef& thiz, int index);
};
