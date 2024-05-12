#pragma once

#include <wcdafx.api.h>
#include <atlstr.h>
#include <atlgdi.h>

struct CRegistry;
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
    DELETE_COPY_MOVE_OF(CFontPair);

    WTL::CLogFont  m_logFont;
    WTL::CFont       m_CFont;

    ~CFontPair();
    CFontPair();

    bool Reset(WTL::CLogFont& logFont);
    bool Reset(WTL::CFont& hFont);
    void Swap(CFontPair& rhs) noexcept;
    void CopyTo(CFontPair& target) const;
};

struct CFonts
{
    DELETE_COPY_MOVE_OF(CFonts);

    ~CFonts();
    CFonts();

    static PCWSTR Title(int index);

    bool LoadDefaults();
    bool LoadValues(CNCMetrics& ncMetrics);
    bool LoadValues(CRegistry const& regScheme);
    void Swap(CFonts& rhs) noexcept;
    void CopyTo(CFonts& target) const;

    CFontPair& operator[](int index);
    CFontPair const& operator[](int index) const;

    WTL::CLogFont const& GetLogFont(int index) const { return (*this)[index].m_logFont; }

    bool IsNotEqual(CFonts const& rhs) const;

private:
    CFontPair m_Pair[FONT_Count];

    template <typename ReturnType, typename SelfRef>
    static ReturnType& getRefByIndex(SelfRef& thiz, int index);
};
